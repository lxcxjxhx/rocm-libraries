# Copyright Advanced Micro Devices, Inc., or its affiliates.
# SPDX-License-Identifier: MIT

"""Supervision of one tensilelite-client config that records its progress in a SQLite database.

The client commits every pair as UNKNOWN right before launching it and never relaunches a
non-pending pair on a re-run, so a client that crashed or hung can simply be started again to
resume. The supervisor decides what to do from the database, never from the exit code alone:
the exit code of a finished client is its validation failure count, not a crash indicator.

Known limitations:
  * The watchdog only detects a hang inside a pair (a pair left UNKNOWN for too long). Hangs
    outside a pair, such as the CPU reference computation or the data initialization, are not
    detected.
  * The watchdog skips a poll when the database cannot be read, which only delays the detection.
  * The watchdog kills the whole process group, including the run.sh wrapper, so the PinClocks
    reset at the end of run.sh does not run; it relies on the atexit restoreClocks handler.

A missing database is the legitimate empty state: the client has not created it yet. One that
exists but cannot be read (locked, malformed, bad values) is retried a few times and then the
supervision stops; it is never judged as empty, because that could relaunch or stop wrongly.

After a client exits abnormally, a GPU probe runs before the next client launch (warm-up).

Shutdown: SIGTERM and SIGHUP (like Ctrl-C) make the supervisor kill the client process group and
exit, with 128 + the signal number for SIGTERM and SIGHUP, so a cancelled job does not leave the
client on the GPU. A second SIGTERM or SIGHUP during that cleanup is ignored. Where no code can
run (SIGKILL of the supervisor), Linux delivers a death signal: the client wrapper and the GPU
probe are started with PR_SET_PDEATHSIG, and the client sets it on itself because the wrapper
cannot exec it. After SIGKILL the supervisor waits at most KILL_REAP_SECONDS for the wrapper to be
reaped, then stops without relaunching onto a GPU the client may still hold. That check only
follows the group leader; a client stuck below a wrapper that was already reaped is not detected.
"""

from __future__ import annotations

import contextlib
import ctypes
import enum
import os
import signal
import sqlite3
import subprocess
import sys
import threading
import time
from dataclasses import dataclass
from pathlib import Path
from typing import Callable, Iterator, List, Optional, Set, Tuple

from .BenchmarkDatabase import (
    BenchmarkDatabaseError,
    Pair,
    PathLike,
    RunStatus,
    countPairsByStatus,
    formatSummary,
    getLatestPairSessionId,
    getSessions,
    getStuckPairs,
    openDatabase,
    summarize,
    writeResultsCsv,
)
from .Common import printWarning
from .Common.Constants import DEFAULT_CLIENT_PATH

POLL_SECONDS = 2.0
KILL_GRACE_SECONDS = 5.0
# How long to wait for the process to be reaped after SIGKILL.
KILL_REAP_SECONDS = 30.0
WARMUP_ATTEMPTS = 3
PROBE_TIMEOUT_SECONDS = 120.0
PROBE_NAME = "tensilelite-gpu-probe"
STATE_READ_ATTEMPTS = 3
STATE_READ_RETRY_SECONDS = 1.0
# Each attempt already waits this long for a lock inside SQLite; the retries above cover the rest.
STATE_BUSY_TIMEOUT_SECONDS = 2.0
_PR_SET_PDEATHSIG = 1
_TERMINATION_SIGNALS = (signal.SIGTERM, signal.SIGHUP)


@dataclass(frozen=True)
class _DbState:
    lastSessionId: int = 0
    lastSessionDied: bool = False
    total: int = 0
    notBenchmarked: int = 0
    # Newest session that began or marked a pair; above the floor read before a launch only if the
    # launch touched a pair.
    latestPairSessionId: int = 0


class _StateUnreadable(Exception):
    pass


def _queryState(dbPath: PathLike) -> _DbState:
    with contextlib.closing(openDatabase(dbPath, STATE_BUSY_TIMEOUT_SECONDS)) as connection:
        sessions = getSessions(connection)
        counts = countPairsByStatus(connection)
        latestPairSessionId = getLatestPairSessionId(connection)
    last = sessions[-1] if sessions else None
    return _DbState(
        lastSessionId=last.id if last else 0,
        lastSessionDied=last is not None and last.died,
        total=sum(counts.values()),
        notBenchmarked=counts[RunStatus.NOT_BENCHMARKED],
        latestPairSessionId=latestPairSessionId,
    )


def _readState(dbPath: PathLike) -> _DbState:
    """Read what the supervisor decides on; raise _StateUnreadable rather than guess."""
    if not Path(dbPath).is_file():
        return _DbState()
    for attempt in range(1, STATE_READ_ATTEMPTS + 1):
        try:
            return _queryState(dbPath)
        except (BenchmarkDatabaseError, sqlite3.Error) as error:
            lastError = error
        if attempt < STATE_READ_ATTEMPTS:
            time.sleep(STATE_READ_RETRY_SECONDS)
    raise _StateUnreadable(
        f"could not read the benchmark database {dbPath} after {STATE_READ_ATTEMPTS} attempts: "
        f"{lastError}; not judging the client without it"
    )


class SupervisorTerminated(SystemExit):
    """Raised by the SIGTERM/SIGHUP handler so cleanup runs; the exit code is 128 + signal."""

    def __init__(self, signalNumber: int):
        super().__init__(128 + signalNumber)
        self.signalNumber = signalNumber


def _raiseTerminated(signalNumber: int, frame) -> None:
    # A second signal must not interrupt the cleanup before the client group got its SIGKILL.
    for number in _TERMINATION_SIGNALS:
        signal.signal(number, signal.SIG_IGN)
    raise SupervisorTerminated(signalNumber)


@contextlib.contextmanager
def _terminateOnSignals() -> Iterator[None]:
    """Turn SIGTERM and SIGHUP into SupervisorTerminated, restoring the handlers on exit."""
    # signal.signal only works in the main thread.
    if threading.current_thread() is not threading.main_thread():
        yield
        return
    previous = {number: signal.signal(number, _raiseTerminated) for number in _TERMINATION_SIGNALS}
    try:
        yield
    finally:
        # A signal arriving mid-restore must not hit the raising handler.
        for number in _TERMINATION_SIGNALS:
            signal.signal(number, signal.SIG_IGN)
        for number, handler in previous.items():
            # None means the handler was installed from C and cannot be restored.
            signal.signal(number, signal.SIG_DFL if handler is None else handler)


def _dieWithParent() -> Optional[Callable[[], None]]:
    """Return a preexec_fn that makes the child get SIGKILL when the supervisor dies, if supported.

    preexec_fn is not thread-safe, so this is only used from the main thread: the supervisor never
    starts clients or probes from other threads.
    """
    if sys.platform != "linux":
        return None
    # Loaded here because the forked child must not do anything beyond the calls below.
    libc = ctypes.CDLL(None, use_errno=True)
    parentPid = os.getpid()

    def setDeathSignal() -> None:
        if libc.prctl(_PR_SET_PDEATHSIG, int(signal.SIGKILL), 0, 0, 0) != 0:
            raise OSError(ctypes.get_errno(), "prctl(PR_SET_PDEATHSIG) failed")
        # The supervisor may have died between the fork and the prctl, which then never signals.
        if os.getppid() != parentPid:
            os._exit(1)

    return setDeathSignal


@dataclass(frozen=True)
class Watchdog:
    """How long a pair may run before the client is killed, and how the kill is carried out."""

    timeoutSeconds: float  # 0 disables the watchdog.
    pollSeconds: float = POLL_SECONDS
    graceSeconds: float = KILL_GRACE_SECONDS
    reapSeconds: float = KILL_REAP_SECONDS


@dataclass(frozen=True)
class _Launch:
    # The return code of run.sh: negative only if run.sh itself was signalled, while a client killed
    # by a signal makes run.sh exit with 128 + the signal. None when the watchdog killed the client
    # or it was not reaped: the code of a killed wrapper means nothing.
    exitCode: Optional[int]
    unreapedPid: Optional[int] = None

    @property
    def killed(self) -> bool:
        return self.exitCode is None

    @property
    def cleanExit(self) -> bool:
        return self.exitCode == 0


def _unreapedMessage(pid: int) -> str:
    return (
        f"client process {pid} did not exit after SIGKILL (stuck in an uninterruptible driver "
        "call?); not relaunching onto a GPU it may still hold"
    )


def _killpg(process: subprocess.Popen, signalNumber: int) -> None:
    with contextlib.suppress(ProcessLookupError):
        os.killpg(process.pid, signalNumber)


def _reap(process: subprocess.Popen, reapSeconds: float) -> bool:
    try:
        process.wait(timeout=reapSeconds)
    except subprocess.TimeoutExpired:
        return False
    return True


def _killGroup(process: subprocess.Popen, graceSeconds: float, reapSeconds: float) -> bool:
    """Kill the process group; return whether its leader was reaped."""
    _killpg(process, signal.SIGTERM)
    try:
        process.wait(timeout=graceSeconds)
    except subprocess.TimeoutExpired:
        pass
    finally:
        # Also when the grace wait is interrupted, and for a child that ignored SIGTERM while
        # the leader exited.
        _killpg(process, signal.SIGKILL)
    return _reap(process, reapSeconds)


def _findHungPair(dbPath: PathLike, sessionFloor: int, timeoutSeconds: float) -> Optional[Pair]:
    try:
        with contextlib.closing(openDatabase(dbPath, STATE_BUSY_TIMEOUT_SECONDS)) as connection:
            pairs = getStuckPairs(connection, sessionFloor, time.time() - timeoutSeconds)
    except (BenchmarkDatabaseError, sqlite3.Error):
        # A read error only delays the detection to the next poll, unlike the state reads that
        # decide whether to launch or stop, so it is deliberately skipped.
        return None
    return pairs[0] if pairs else None


def _warnHungPair(pair: Pair, timeoutSeconds: float) -> None:
    running = time.time() - (pair.startTime or time.time())
    printWarning(
        f"problem {pair.problemIndex}, solution {pair.libraryIndex} {pair.solutionName} "
        f"has been running for {running:.0f}s, over the {timeoutSeconds:g}s limit; "
        "killing the client"
    )


def _watch(
    process: subprocess.Popen, dbPath: PathLike, sessionFloor: int, watchdog: Watchdog
) -> _Launch:
    while True:
        try:
            return _Launch(process.wait(timeout=watchdog.pollSeconds))
        except subprocess.TimeoutExpired:
            pass
        hung = _findHungPair(dbPath, sessionFloor, watchdog.timeoutSeconds)
        if hung is None:
            continue
        _warnHungPair(hung, watchdog.timeoutSeconds)
        reaped = _killGroup(process, watchdog.graceSeconds, watchdog.reapSeconds)
        return _Launch(None, None if reaped else process.pid)


def _runLaunch(
    command: List[str], cwd: PathLike, dbPath: PathLike, sessionFloor: int, watchdog: Watchdog
) -> _Launch:
    # Own session so the whole process tree can be killed; it then no longer gets Ctrl-C.
    process = subprocess.Popen(
        command, cwd=cwd, start_new_session=True, preexec_fn=_dieWithParent()
    )
    try:
        if watchdog.timeoutSeconds <= 0:
            return _Launch(process.wait())
        return _watch(process, dbPath, sessionFloor, watchdog)
    except BaseException:
        if not _killGroup(process, watchdog.graceSeconds, watchdog.reapSeconds):
            printWarning(_unreapedMessage(process.pid))
        raise


@dataclass(frozen=True)
class GpuProbe:
    command: List[str]
    device: int


class WarmUpState:
    """Devices that must be warmed up before the next client launch, and the missing-probe warning.

    Process-wide on purpose: the bad GPU state outlives one superviseClient call, because the next
    config, group or Ductile evaluation launches its first client later.
    """

    def __init__(self) -> None:
        self._devices: Set[int] = set()
        self._missingProbeWarned = False

    def request(self, device: int) -> None:
        self._devices.add(device)

    def isRequested(self, device: int) -> bool:
        return device in self._devices

    def complete(self, device: int) -> None:
        self._devices.discard(device)

    def warnMissingProbeOnce(self, searched: str) -> None:
        if self._missingProbeWarned:
            return
        self._missingProbeWarned = True
        printWarning(
            f"{PROBE_NAME} not found in {searched}; clients are restarted without the "
            "post-crash GPU warm-up (rebuild the client to get it)"
        )


# A module instance rather than one passed in by ClientWriter: runClient is a free function called
# per group or evaluation, so it would need its own global anyway.
_warmUpState = WarmUpState()


def _searchGpuProbe(
    clientExecutable: PathLike, device: int
) -> Tuple[Optional[GpuProbe], List[Path]]:
    directories = [Path(clientExecutable).parent, DEFAULT_CLIENT_PATH.parent]
    for directory in directories:
        probe = directory / PROBE_NAME
        if probe.is_file():
            return GpuProbe([str(probe), "--device-idx", str(device)], device), directories
    return None, directories


def findGpuProbe(clientExecutable: PathLike, device: int) -> Optional[GpuProbe]:
    """Locate the probe next to the client, else in the default build directory.

    The fallback covers wrapper scripts passed as the client. Without a probe, None is returned
    and one warning is printed per process.
    """
    probe, directories = _searchGpuProbe(clientExecutable, device)
    if probe is None:
        _warmUpState.warnMissingProbeOnce(" or ".join(str(directory) for directory in directories))
    return probe


class _Verdict(enum.Enum):
    DONE = enum.auto()
    RESTART = enum.auto()
    STOP = enum.auto()


def _sessionEnded(before: _DbState, after: _DbState) -> bool:
    return after.lastSessionId > before.lastSessionId and not after.lastSessionDied


def _signalName(signalNumber: int) -> Optional[str]:
    try:
        return signal.Signals(signalNumber).name
    except ValueError:
        return None


def _describeExit(launch: _Launch, sessionEnded: bool) -> str:
    if launch.killed:
        return "killed by the watchdog"
    code = launch.exitCode
    signalNumber = 0
    if code < 0:
        signalNumber = -code
    elif code > 128 and not sessionEnded:
        # run.sh reports a signal-killed client as 128 + the signal; after an ended session the
        # code is the validation failure count instead.
        signalNumber = code - 128
    name = _signalName(signalNumber)
    if name is None:
        return f"exit code {code}"
    return f"killed by signal {signalNumber} ({name})"


def _exitedAbnormally(before: _DbState, after: _DbState, launch: _Launch) -> bool:
    if launch.killed or launch.exitCode < 0:
        return True
    # A non-zero exit code of a cleanly ended session is only the validation failure count.
    return not launch.cleanExit and not _sessionEnded(before, after)


def _runProbeOnce(probe: GpuProbe, cwd: PathLike) -> Optional[str]:
    """Run the probe once; return why it failed, or None if it passed."""
    try:
        returncode = subprocess.run(
            probe.command, cwd=cwd, timeout=PROBE_TIMEOUT_SECONDS, preexec_fn=_dieWithParent()
        ).returncode
    except subprocess.TimeoutExpired:
        return f"timed out after {PROBE_TIMEOUT_SECONDS:g}s"
    except OSError as error:
        return str(error)
    if returncode == 0:
        return None
    if returncode < 0:
        name = _signalName(-returncode)
        return f"killed by signal {-returncode}" + (f" ({name})" if name else "")
    return f"exit code {returncode}"


def _warmUp(probe: GpuProbe, cwd: PathLike) -> Optional[str]:
    """Run the probe until it passes; return an error if every attempt failed, else None."""
    device = probe.device
    for attempt in range(1, WARMUP_ATTEMPTS + 1):
        reason = _runProbeOnce(probe, cwd)
        if reason is None:
            print(
                f"# gpu warm-up on device {device}, attempt {attempt}/{WARMUP_ATTEMPTS}: passed",
                flush=True,
            )
            return None
        printWarning(
            f"gpu warm-up on device {device}, attempt {attempt}/{WARMUP_ATTEMPTS} failed ({reason})"
        )
    return (
        f"gpu warm-up failed on device {device} after {WARMUP_ATTEMPTS} attempts; "
        "not benchmarking on a possibly unhealthy gpu"
    )


def _deleteHint(dbPath: PathLike) -> str:
    return (
        f"If the benchmark configuration changed since {dbPath} was created, "
        "delete it to start over"
    )


def _judge(
    before: _DbState, after: _DbState, launch: _Launch, stopOnCleanExit: bool, dbPath: PathLike
) -> Tuple[_Verdict, str]:
    if launch.unreapedPid is not None:
        return _Verdict.STOP, _unreapedMessage(launch.unreapedPid)
    sessionStarted = after.lastSessionId > before.lastSessionId
    sessionEnded = _sessionEnded(before, after)
    how = _describeExit(launch, sessionEnded)
    if not launch.killed and not launch.cleanExit and not sessionStarted:
        # E.g. the client rejected the database as not matching its configuration.
        return (
            _Verdict.STOP,
            f"client failed ({how}) before starting a benchmark session; see its output above. "
            f"{_deleteHint(dbPath)}",
        )
    if after.notBenchmarked == 0 and (after.total > 0 or launch.cleanExit):
        return _Verdict.DONE, ""
    if launch.cleanExit:
        return (
            _Verdict.STOP,
            f"client exited normally but {after.notBenchmarked} pairs in {dbPath} "
            "were not benchmarked",
        )
    if after.latestPairSessionId <= before.lastSessionId:
        # Progress is a pair carrying a newer session id, not the NOT_BENCHMARKED total, which a
        # disabled indirect option can raise at session start.
        return (
            _Verdict.STOP,
            f"client failed ({how}) without benchmarking any pair; see its output above. "
            f"{_deleteHint(dbPath)}",
        )
    if stopOnCleanExit and sessionEnded:
        # ExitOnFails=2 asks the client to stop at the first failure; do not resume it.
        return (
            _Verdict.STOP,
            f"client stopped early ({how}) with {after.notBenchmarked} pairs not benchmarked",
        )
    return (
        _Verdict.RESTART,
        f"client failed ({how}); restarting to resume, {after.notBenchmarked} pairs left",
    )


class SupervisionOutcome(enum.Enum):
    COMPLETED = enum.auto()
    # Stopped, but a client session of this call confirmed the database matches the configuration.
    STOPPED_AFTER_SESSION = enum.auto()
    # Stopped before any launch of this call started a session.
    STOPPED_BEFORE_SESSION = enum.auto()


@dataclass(frozen=True)
class SupervisionResult:
    restarts: int
    outcome: SupervisionOutcome
    # Why the supervision stopped; empty when it completed.
    message: str = ""


def _stopped(restarts: int, sessionStarted: bool, message: str) -> SupervisionResult:
    outcome = (
        SupervisionOutcome.STOPPED_AFTER_SESSION
        if sessionStarted
        else SupervisionOutcome.STOPPED_BEFORE_SESSION
    )
    return SupervisionResult(restarts, outcome, message)


def superviseClient(
    command: List[str],
    cwd: PathLike,
    dbPath: PathLike,
    watchdog: Watchdog,
    stopOnCleanExit: bool,
    probe: Optional[GpuProbe] = None,
) -> SupervisionResult:
    """Run the client until every pair is benchmarked, restarting it after crashes and hangs.

    The loop is bounded because each restart requires progress (a pair touched by the new session),
    so a persistent database fault that makes the client exit non-zero cannot loop forever. With a
    probe, the device is warmed up before the first launch that follows an abnormal client exit.
    SIGTERM and SIGHUP kill the client group and raise SupervisorTerminated (a SystemExit).
    """
    restarts = 0
    sessionStarted = False
    with _terminateOnSignals():
        while True:
            if probe is not None and _warmUpState.isRequested(probe.device):
                error = _warmUp(probe, cwd)
                if error:
                    return _stopped(restarts, sessionStarted, error)
                _warmUpState.complete(probe.device)
            try:
                before = _readState(dbPath)
            except _StateUnreadable as unreadable:
                return _stopped(restarts, sessionStarted, str(unreadable))
            launch = _runLaunch(command, cwd, dbPath, before.lastSessionId, watchdog)
            try:
                after = _readState(dbPath)
            except _StateUnreadable as unreadable:
                if probe is not None:
                    # The exit cannot be judged, so assume the GPU state is bad.
                    _warmUpState.request(probe.device)
                return _stopped(restarts, sessionStarted, str(unreadable))
            if probe is not None and _exitedAbnormally(before, after, launch):
                _warmUpState.request(probe.device)
            sessionStarted |= after.lastSessionId > before.lastSessionId
            verdict, message = _judge(before, after, launch, stopOnCleanExit, dbPath)
            if verdict == _Verdict.DONE:
                return SupervisionResult(restarts, SupervisionOutcome.COMPLETED)
            if verdict == _Verdict.STOP:
                return _stopped(restarts, sessionStarted, message)
            printWarning(message)
            restarts += 1


def removeStaleResults(csvPath: PathLike) -> None:
    # A CSV from an earlier run must not be mistaken for results.
    Path(csvPath).unlink(missing_ok=True)


def removeDatabase(dbPath: PathLike) -> None:
    """Delete the database and its SQLite write-ahead log and shared memory files."""
    for suffix in ("", "-wal", "-shm"):
        Path(str(dbPath) + suffix).unlink(missing_ok=True)


def reportDatabaseResults(dbPath: PathLike, csvPath: PathLike) -> int:
    """Write the results CSV and print the summary; return 1 if any pair is not clean."""
    try:
        summary = summarize(dbPath)
        writeResultsCsv(dbPath, csvPath)
    except (BenchmarkDatabaseError, sqlite3.Error) as error:
        printWarning(error)
        removeStaleResults(csvPath)
        return 1
    print(formatSummary(summary), flush=True)
    unclean = (
        summary.counts[RunStatus.UNKNOWN]
        + summary.counts[RunStatus.FAILURE]
        + summary.counts[RunStatus.INDIRECTLY_FAILED]
        + summary.counts[RunStatus.INDIRECT_VERIFICATION_FAILURE]
        + summary.notBenchmarkedCount
    )
    return 1 if unclean else 0
