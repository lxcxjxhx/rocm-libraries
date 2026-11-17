# Reference-GEMM stack publication drafts

Prepared November 17, 2026 from live PR bodies and validated local stack tip `4135e6d96f2df606404ab51af3214a8b2f4a3427` on develop `bc60c82f1d732aa8bb50c869ac3ad2e1a586e599`.

No text in this file has been published during this implementation. Code pushes remain pending authorization. Resolve `<FLOAT8_ISSUE_NUMBER>` after filing the issue, then add the resulting PR links and published-head CI results. The descriptions distinguish integrated local validation from earlier published-head evidence.

## Float8 bug issue

TensileLite Float8 narrowing applies input scales incorrectly in slow Vector and MultipleBuffer conversion paths

Discovered while reviewing [PR #15275](https://github.com/ROCm/rocm-libraries/pull/15275), at published head `b6828c46316720641f2e3f15a1320201d4f1c34b`. Both defects also exist below that PR.

1. With Half storage narrowed to Float8 compute inputs and Vector ScaleAB, the GPU narrows first and applies both vectors after reduction. The slow CPU reference omits the vector belonging to the narrowed Half operand.
2. For the same narrowing with Scalar ScaleAB and MultipleBuffer split-K (GSU 2/4), the main kernel consumes the Half-side scale before narrowing. The conversion invocation supplies it again, applying that scale twice.

The correction applies both vectors after the slow reduction and binds the already-consumed narrowed-side scalar to a unit scale in the conversion invocation only. The main kernel's scalar binding and the other operand's scale remain intact. Keep the Scalar-only fallback introduced by #15275; Vector remains fast-eligible.

Independent witnesses:

- Vector, E4M3 OCP or fnuz: `Half(0.9)` narrows to `0.875`. With the narrowed-side vector scale `3` and the other side `1`, the final Float8 output is `2.5` after rounding `2.625` to even. The old slow reference omits the narrowed-side vector and returns `0.875`. Scalar scaling has a different order and returns `2.75`, so narrowing before all scales is not a valid universal fix.
- Actual gfx942 split-K witness: `H × F8N`, both compute inputs `F8N`, output `F8N`, NN, M=N=16, K=256, alpha=1, beta=0, Scalar scales A=3/B=0.5, MultipleBuffer GSU=2. At output index 0, the baseline GPU gives `8` instead of the independent expected `2.75`. The final production conversion binding gives `2.75`.

Numerical evidence: 1,728 gfx942 cases executed; baseline 1,696 passed and 32 failed, fixed production runtime 1,728 passed. The other 1,696 numerical output arrays match baseline. An independent CPU matrix passed 2,304 cases across OCP/fnuz types, scale modes, layouts and epilogues. The user also confirmed the same baseline/proposal counts on MI350X gfx950, ROCm 7.2.1; that gfx950 run validated the conversion-call proposal, not the final integrated production patch.

Regression coverage calls production SolveGemmCPU and inspects production conversion arguments, including grouped argument construction. Final integrated gfx942 execution passed all 1,728 nongrouped and 384 grouped cases at stack tip `4135e6d96f2df606404ab51af3214a8b2f4a3427`. Final production gfx950 execution remains a qualification task.

The dedicated fix branch is `users/alvasile/tl-fix-float8-input-scaling`, to be inserted between #15274 and #15275. Its new PR must close this issue and link #15275 as the discovery PR.

## PR #15765

Title: `fix(tensilelite): accumulate Int8 dot products before float epilogues`

````markdown
JIRA ID : N/A
Fixes #15764

## Motivation

TensileLite’s CPU reference supplies expected results for GEMM validation. For Int8 inputs with float alpha/beta, its slow implementation accumulates in float and can lose integer terms before scaling. The reproducer in #15764 returns 8,388,608 instead of 8,388,609.

Investigating [newling’s comment on #15266](https://github.com/ROCm/rocm-libraries/pull/15266#discussion_r4554390239) exposed this existing defect.

## Technical Details

Separate the Int8 reduction type from the float epilogue type. Accumulate with unsigned 32-bit arithmetic for defined wraparound, interpret the completed sum as signed Int32, then convert to float. Alpha/beta scaling and the remaining epilogue stay unchanged.

Add 17 gtest cases to the existing tensilelite-tests target. Coverage includes the 2^24 boundary, fractional beta, negative sums, integer-alpha controls, Int32/Int8/Half/BFloat16 outputs, float AMAX-D, and signed wraparound. Both path preferences are exercised; Int8 falls back to the slow reference.

This PR supplies the slow-reference fix immediately above #15259 and below #15266 in the reference-GEMM stack. It changes no GPU kernels or public interfaces and adds no feature flag.

Downstream integration has separate owners: #15266 (`users/alvasile/tl-speedup-16-cpu-gemm-driver-int8`) updates the driver oracle documentation and restores large-K driver coverage without the former float-per-K precision restriction; #15274 (`users/alvasile/tl-speedup-24-reference-fast-path-int8`) checks both paths against the independent 16,790,292 exact sum and refreshes its integer-reduction comments. The driver retains its checked Int32 range; #15274 describes defined wrapping for the fast and corrected slow float-alpha reductions without changing inherited slow integer-alpha arithmetic. These downstream changes are outside this PR's own diff.

## Test Plan

Configure from `projects/hipblaslt`:

```sh
cmake --preset tensilelite -S . -B "$BUILD" -DTENSILELITE_BUILD_TESTING=ON
cmake --build "$BUILD" --target tensilelite-tests cpu-gemm-driver tensilelite-client --parallel 8
ctest --test-dir "$BUILD/tensilelite/tests" -L '^standard$' \
  --output-on-failure --parallel 8 --no-tests=error
```

Install the `tests` and `tensilelite` components to a separate prefix, provide the ROCm runtime dependencies, and run the same standard label under `bin/hipblaslt-tensilelite` with `LD_LIBRARY_PATH` unset. Run focused reference tests by invoking `tensilelite-tests` directly; CTest groups gtests into category suites. Run root and hipBLASLt component pre-commit checks on the changed files.

Run `tensilelite-tests --gtest_filter='*ReferenceInt8*'` to select this PR's 17 integer-reduction regressions.

## Test Result

- Unpatched speedup-09 base: 6 passed / 6 failed in the initial 12-case regression suite.
- Final suite with the fix temporarily disabled: 6 passed / 11 failed.
- Fixed suite: all 17 cases passed over 10 repetitions.
- Full gtest run: 900 passed / 9 existing skips; one pre-existing disabled test was not executed.
- Standard CTest suite: passed.
- CPU GEMM driver CTests: 297/297 passed.
- Scoped pre-commit completed, but its hooks skip these C++/CMake paths. Manual clang-format and diff checks passed.

Integrated local validation on November 17, 2026, at stack tip `4135e6d96f2df606404ab51af3214a8b2f4a3427`, rebased onto develop `bc60c82f1d732aa8bb50c869ac3ad2e1a586e599`:

- Release builds of `tensilelite-tests`, `cpu-gemm-driver` and `tensilelite-client` passed.
- Standard build-tree and installed CTest each passed **925/925 registrations**, comprising 924 driver cases and the standard gtest suite. Installed execution used `LD_LIBRARY_PATH` unset and local ROCm runtime dependencies.
- All ten independent AMAX-D gtests passed at each OpenMP thread limit of 1, 3 and 8.
- The independent CPU Float8 matrix passed all 2,304 cases across both scale profiles, with zero slow or fast/fallback mismatches.
- gfx942 executed all 1,728 nongrouped and 384 grouped Float8 GPU cases with zero numerical failures, unexecuted cases or nonfinite outputs. The 32 baseline failures are corrected; the other 1,696 nongrouped numerical output arrays equal baseline.
- Scoped root and hipBLASLt hooks passed. The root C++/CMake hooks exclude these paths; formatting was checked separately for the new C++ edits.

These are integrated local results, including the later correctness and AMAX-D PRs. They do not establish separate CI results for each intermediate branch. No code has been pushed or run through shared CI at these local heads. Final production gfx950 validation, including the Float8 correction and dedicated gradient/MX-tail qualification, remains pending. No new performance measurement is claimed.

Integrated local validation on November 17, 2026, at stack tip `4135e6d96f2df606404ab51af3214a8b2f4a3427`, rebased onto develop `bc60c82f1d732aa8bb50c869ac3ad2e1a586e599`:

- Release builds of `tensilelite-tests`, `cpu-gemm-driver` and `tensilelite-client` passed.
- Standard build-tree and installed CTest each passed **925/925 registrations**, comprising 924 driver cases and the standard gtest suite. Installed execution used `LD_LIBRARY_PATH` unset and local ROCm runtime dependencies.
- All ten independent AMAX-D gtests passed at each OpenMP thread limit of 1, 3 and 8.
- The independent CPU Float8 matrix passed all 2,304 cases across both scale profiles, with zero slow or fast/fallback mismatches.
- gfx942 executed all 1,728 nongrouped and 384 grouped Float8 GPU cases with zero numerical failures, unexecuted cases or nonfinite outputs. The 32 baseline failures are corrected; the other 1,696 nongrouped numerical output arrays equal baseline.
- Scoped root and hipBLASLt hooks passed. The root C++/CMake hooks exclude these paths; formatting was checked separately for the new C++ edits.

These are integrated local results, including the later correctness and AMAX-D PRs. They do not establish separate CI results for each intermediate branch. No code has been pushed or run through shared CI at these local heads. Final production gfx950 validation, including the Float8 correction and dedicated gradient/MX-tail qualification, remains pending. No new performance measurement is claimed.

Integrated local validation on November 17, 2026, at stack tip `4135e6d96f2df606404ab51af3214a8b2f4a3427`, rebased onto develop `bc60c82f1d732aa8bb50c869ac3ad2e1a586e599`:

- Release builds of `tensilelite-tests`, `cpu-gemm-driver` and `tensilelite-client` passed.
- Standard build-tree and installed CTest each passed **925/925 registrations**, comprising 924 driver cases and the standard gtest suite. Installed execution used `LD_LIBRARY_PATH` unset and local ROCm runtime dependencies.
- All ten independent AMAX-D gtests passed at each OpenMP thread limit of 1, 3 and 8.
- The independent CPU Float8 matrix passed all 2,304 cases across both scale profiles, with zero slow or fast/fallback mismatches.
- gfx942 executed all 1,728 nongrouped and 384 grouped Float8 GPU cases with zero numerical failures, unexecuted cases or nonfinite outputs. The 32 baseline failures are corrected; the other 1,696 nongrouped numerical output arrays equal baseline.
- Scoped root and hipBLASLt hooks passed. The root C++/CMake hooks exclude these paths; formatting was checked separately for the new C++ edits.

These are integrated local results, including the later correctness and AMAX-D PRs. They do not establish separate CI results for each intermediate branch. No code has been pushed or run through shared CI at these local heads. Final production gfx950 validation, including the Float8 correction and dedicated gradient/MX-tail qualification, remains pending. No new performance measurement is claimed.

Integrated local validation on November 17, 2026, at stack tip `4135e6d96f2df606404ab51af3214a8b2f4a3427`, rebased onto develop `bc60c82f1d732aa8bb50c869ac3ad2e1a586e599`:

- Release builds of `tensilelite-tests`, `cpu-gemm-driver` and `tensilelite-client` passed.
- Standard build-tree and installed CTest each passed **925/925 registrations**, comprising 924 driver cases and the standard gtest suite. Installed execution used `LD_LIBRARY_PATH` unset and local ROCm runtime dependencies.
- All ten independent AMAX-D gtests passed at each OpenMP thread limit of 1, 3 and 8.
- The independent CPU Float8 matrix passed all 2,304 cases across both scale profiles, with zero slow or fast/fallback mismatches.
- gfx942 executed all 1,728 nongrouped and 384 grouped Float8 GPU cases with zero numerical failures, unexecuted cases or nonfinite outputs. The 32 baseline failures are corrected; the other 1,696 nongrouped numerical output arrays equal baseline.
- Scoped root and hipBLASLt hooks passed. The root C++/CMake hooks exclude these paths; formatting was checked separately for the new C++ edits.

These are integrated local results, including the later correctness and AMAX-D PRs. They do not establish separate CI results for each intermediate branch. No code has been pushed or run through shared CI at these local heads. Final production gfx950 validation, including the Float8 correction and dedicated gradient/MX-tail qualification, remains pending. No new performance measurement is claimed.

Integrated local validation on November 17, 2026, at stack tip `4135e6d96f2df606404ab51af3214a8b2f4a3427`, rebased onto develop `bc60c82f1d732aa8bb50c869ac3ad2e1a586e599`:

- Release builds of `tensilelite-tests`, `cpu-gemm-driver` and `tensilelite-client` passed.
- Standard build-tree and installed CTest each passed **925/925 registrations**, comprising 924 driver cases and the standard gtest suite. Installed execution used `LD_LIBRARY_PATH` unset and local ROCm runtime dependencies.
- All ten independent AMAX-D gtests passed at each OpenMP thread limit of 1, 3 and 8.
- The independent CPU Float8 matrix passed all 2,304 cases across both scale profiles, with zero slow or fast/fallback mismatches.
- gfx942 executed all 1,728 nongrouped and 384 grouped Float8 GPU cases with zero numerical failures, unexecuted cases or nonfinite outputs. The 32 baseline failures are corrected; the other 1,696 nongrouped numerical output arrays equal baseline.
- Scoped root and hipBLASLt hooks passed. The root C++/CMake hooks exclude these paths; formatting was checked separately for the new C++ edits.

These are integrated local results, including the later correctness and AMAX-D PRs. They do not establish separate CI results for each intermediate branch. No code has been pushed or run through shared CI at these local heads. Final production gfx950 validation, including the Float8 correction and dedicated gradient/MX-tail qualification, remains pending. No new performance measurement is claimed.

## Submission Checklist

- [ ] Look over the contributing guidelines at https://github.com/ROCm/TheRock/blob/main/GOVERNANCE.md#pull-requests.

## Risk level

Low (2/5): narrowly changes float-alpha Int8 CPU-reference results, including AMAX-D. Other data types and integer-alpha modes retain their existing arithmetic. Shared automated checks remain pending; no architecture-specific kernel changes require a GPU sweep.
````

## PR #15259

Title: `perf(tensilelite): take the slow-path reference lock only for AMAX-D`

````markdown
JIRA ID : AIHPBLAS-5257

## Motivation

tensilelite-client, the C++ program that runs and validates generated GPU kernels, checks GPU results against a CPU reference GEMM (matrix multiply). Problems that the vectorized fast path does not support use a scalar slow path, an OpenMP loop over output elements. Each element entered one global critical section, even when the problem had no AMAX-D output (the maximum absolute value of D, which some problems also return). In a gfx950 per-config timing run, the slow path was 741 s of the 1,080 s of reference GEMM time, with most threads waiting on that lock.

## Technical Details

Production code (`client/src/Reference.cpp`): in `SolveCPU`'s slow path, each thread keeps its own AMAX-D maximum inside the parallel region. After the work-shared loop, each thread merges it into the result once, under the critical section, and only when the problem outputs AMAX-D. Problems without AMAX-D take no lock. A per-thread maximum is used instead of an OpenMP `reduction(max:...)`, because the accumulator type can be a class type (`Half`, `BFloat16`) that the built-in reduction rejects. The maximum does not depend on the merge order, so D and AMAX-D are unchanged.

Test driver (`client/cpu_gemm_driver.cpp`): cpu-gemm-driver, the standalone CPU-reference test program, gains `--outputAmaxD`. The problem then gets an AMAX-D output, and `--validate` compares it with max |D| of the driver's own scalar GEMM, which it computes independently of the client code. The value starts at -1, so a reference that never writes it fails.

Tests (`tests/CMakeLists.txt`): 14 `CPUGemm.slow_*AmaxD*` CTest cases. They cover f32, f16 and bf16 (plain, and with transA, beta, bias, ReLU, alpha vector and vector scale A/B), plus f64, tf32, f8, bf8, f4 MX32, batched, alpha 0 and a 1x1 problem with a negative element. Each numerical case runs with OpenMP limits of 1, 3 and 8. They run on the slow path; the separate final stack PR adds fast AMAX-D support.

No GPU code changes.

## Test Plan

Configure from `projects/hipblaslt`:

```sh
cmake --preset tensilelite -S . -B "$BUILD" -DTENSILELITE_BUILD_TESTING=ON
cmake --build "$BUILD" --target tensilelite-tests cpu-gemm-driver tensilelite-client --parallel 8
ctest --test-dir "$BUILD/tensilelite/tests" -L '^standard$' \
  --output-on-failure --parallel 8 --no-tests=error
```

Install the `tests` and `tensilelite` components to a separate prefix, provide the ROCm runtime dependencies, and run the same standard label under `bin/hipblaslt-tensilelite` with `LD_LIBRARY_PATH` unset. Run focused reference tests by invoking `tensilelite-tests` directly; CTest groups gtests into category suites. Run root and hipBLASLt component pre-commit checks on the changed files.

## Test Result

Time saved: not measured in isolation on the common tests. On cpu-gemm-driver alone, each step was timed against its parent commit on the original unsplit branch: fp32 `--transA` slow path with `--validate` on a shared host, `KMP_BLOCKTIME=0`, 3 interleaved runs each. The recorded environment set `OMP_NUM_THREADS=25`; the actual OpenMP team size for those historical runs is unverified.

| Problem | Before (ms) | After (ms) |
|---|---:|---:|
| 1024³, no AMAX-D (lock removed) | 1,495-2,574 | 151-156 |
| 2048³, no AMAX-D (lock removed) | 7,014-9,098 | 1,248-2,052 |
| 1024³, `--outputAmaxD` (per-thread maximum) | 1,619-2,913 | 206-333 |
| 2048³, `--outputAmaxD` (per-thread maximum) | 7,273-10,697 | 1,205-1,739 |

The closest common-test evidence is combined: in a gfx950 replay of the 59 configs that used the slow path, slow-path time fell from 546.6 s to 0.53 s after this change and the later fast-path changes together.

Other results:

- D was bit-identical (max diff 0), and AMAX-D was identical (152 and 242).
- The new cases also pass with the old `Reference.cpp`. Merging only thread 0's maximum failed 9 of 14, and dropping the absolute value failed 5 of 14.

[Math CI build 7](https://math-ci.amd.com/job/rocm-libraries/job/tensilelite-unit-codecov/job/hipblaslt/job/PR-15259/7/consoleText) tested commit `571ecae3fc4814ff951461d1d14e48f780975511`. All 14 new AMAX-D cases passed. The full CTest run reported 1,185 passed tests, four skipped tests, and one disabled test, with pre-existing exclusions in the coverage configuration.

Integrated local validation on November 17, 2026, at stack tip `4135e6d96f2df606404ab51af3214a8b2f4a3427`, rebased onto develop `bc60c82f1d732aa8bb50c869ac3ad2e1a586e599`:

- Release builds of `tensilelite-tests`, `cpu-gemm-driver` and `tensilelite-client` passed.
- Standard build-tree and installed CTest each passed **925/925 registrations**, comprising 924 driver cases and the standard gtest suite. Installed execution used `LD_LIBRARY_PATH` unset and local ROCm runtime dependencies.
- All ten independent AMAX-D gtests passed at each OpenMP thread limit of 1, 3 and 8.
- The independent CPU Float8 matrix passed all 2,304 cases across both scale profiles, with zero slow or fast/fallback mismatches.
- gfx942 executed all 1,728 nongrouped and 384 grouped Float8 GPU cases with zero numerical failures, unexecuted cases or nonfinite outputs. The 32 baseline failures are corrected; the other 1,696 nongrouped numerical output arrays equal baseline.
- Scoped root and hipBLASLt hooks passed. The root C++/CMake hooks exclude these paths; formatting was checked separately for the new C++ edits.

These are integrated local results, including the later correctness and AMAX-D PRs. They do not establish separate CI results for each intermediate branch. No code has been pushed or run through shared CI at these local heads. Final production gfx950 validation, including the Float8 correction and dedicated gradient/MX-tail qualification, remains pending. No new performance measurement is claimed.

## Related

- Work tracking: AIHPBLAS-5257
- OpenMP thread policy: [#15257](https://github.com/ROCm/rocm-libraries/pull/15257).

## Device / Architecture Coverage

Client host code (CPU reference) and its test driver; no kernel or GPU behavior changes. It is arch-independent, so passing PR checks are sufficient. The f4 MX32 case is registered only on non-Windows builds, like the existing MX tests.

## Submission Checklist

- [x] Look over the contributing guidelines at https://github.com/ROCm/TheRock/blob/main/GOVERNANCE.md#pull-requests.

## Risk level

Low (2/5): the slow-path reference keeps the same D and AMAX-D values and takes the lock once per thread instead of once per element. The 14 new cases check AMAX-D against an independent GEMM.
````

## PR #15266

Title: `test(tensilelite): cpu-gemm-driver Int8 inputs with separate C/D and alpha types`

````markdown
JIRA ID : AIHPBLAS-5257

## Motivation

tensilelite-client, the C++ program that runs generated GPU kernels, checks their results against a CPU reference, a CPU computation of the same matrix multiply (GEMM). The reference has a general slow path and a blocked, multithreaded fast path. Int8-input problems always take the slow path, and they are its largest cost in the gfx950 common tests: 951 calls and 293.7 s of replay time, measured in the original branch's investigation. `cpu-gemm-driver`, the test program for the CPU reference, could not express Int8 inputs or a C/D type different from the alpha type. Before the fast path can take Int8 inputs, the slow path needs tests it passes on these problems.

## Technical Details

Test code only; the CPU reference itself is unchanged.

- `client/cpu_gemm_driver.cpp` gains `--type i8`, `--typeCD` (f32, f64, f16, bf16, i8, i32) and `--typeAlpha` (f32, f64, i32). For Int8 inputs it accepts exactly the slow path's 6 type ids: `I8_I8_I32`, `I8_I32_I32`, `I8_I32_S`, `I8_I8_S`, `I8_H_S` and `I8_B_S`. Int8 data covers the full range [-128, 127].
- The driver's own expected result accumulates Int8 products exactly in integers and fails if a sum leaves the int32 range. It runs the epilogue in the alpha type and casts to C/D with the reference's `SaturateCast` rules: Int8 from float rounds half to even, then clamps; Int8 from int32 only clamps. Int8 results are compared exactly.
- `tests/CMakeLists.txt` adds 88 CTest cases: 84 `CPUGemm.slow_I8_*` cases (each type id in 4 transposes, with beta, bias, combined features, ScaleAB scalar and vector, and batched) and 4 rejected-option cases.

The stdout header also prints `TypeCD` and `TypeAlpha`. The preceding #15765 fixes float-per-K precision loss by completing the integer reduction before the float epilogue. All six type combinations have K = 1044 coverage in NN/NT/TN/TT. Ordinary cases use the larger F32 dimensions; integer-alpha ScaleAB cases retain smaller dimensions so completed sums and integer epilogue intermediates fit the driver's checked Int32 range.

## Test Plan

Configure from `projects/hipblaslt`:

```sh
cmake --preset tensilelite -S . -B "$BUILD" -DTENSILELITE_BUILD_TESTING=ON
cmake --build "$BUILD" --target tensilelite-tests cpu-gemm-driver tensilelite-client --parallel 8
ctest --test-dir "$BUILD/tensilelite/tests" -L '^standard$' \
  --output-on-failure --parallel 8 --no-tests=error
```

Install the `tests` and `tensilelite` components to a separate prefix, provide the ROCm runtime dependencies, and run the same standard label under `bin/hipblaslt-tensilelite` with `LD_LIBRARY_PATH` unset. Run focused reference tests by invoking `tensilelite-tests` directly; CTest groups gtests into category suites. Run root and hipBLASLt component pre-commit checks on the changed files.

## Test Result

At published head `4554dab796dee28e4e640b7f688c4d0f12d5b375`, these CI summaries passed:

- [Multi-Arch CI Summary](https://github.com/ROCm/rocm-libraries/actions/runs/42914665250/job/130356281758).
- [Component CI Summary](https://github.com/ROCm/rocm-libraries/actions/runs/42914663179/job/130319662977).
- [Math CI Summary](https://math-ci.amd.com/job/rocm-libraries/job/status-gate/job/hipblaslt/job/PR-15266/lastBuild/stages/).

Integrated local validation on November 17, 2026, at stack tip `4135e6d96f2df606404ab51af3214a8b2f4a3427`, rebased onto develop `bc60c82f1d732aa8bb50c869ac3ad2e1a586e599`:

- Release builds of `tensilelite-tests`, `cpu-gemm-driver` and `tensilelite-client` passed.
- Standard build-tree and installed CTest each passed **925/925 registrations**, comprising 924 driver cases and the standard gtest suite. Installed execution used `LD_LIBRARY_PATH` unset and local ROCm runtime dependencies.
- All ten independent AMAX-D gtests passed at each OpenMP thread limit of 1, 3 and 8.
- The independent CPU Float8 matrix passed all 2,304 cases across both scale profiles, with zero slow or fast/fallback mismatches.
- gfx942 executed all 1,728 nongrouped and 384 grouped Float8 GPU cases with zero numerical failures, unexecuted cases or nonfinite outputs. The 32 baseline failures are corrected; the other 1,696 nongrouped numerical output arrays equal baseline.
- Scoped root and hipBLASLt hooks passed. The root C++/CMake hooks exclude these paths; formatting was checked separately for the new C++ edits.

These are integrated local results, including the later correctness and AMAX-D PRs. They do not establish separate CI results for each intermediate branch. No code has been pushed or run through shared CI at these local heads. Final production gfx950 validation, including the Float8 correction and dedicated gradient/MX-tail qualification, remains pending. No new performance measurement is claimed.

## Related

- Work tracking: AIHPBLAS-5257
- [#15764](https://github.com/ROCm/rocm-libraries/issues/15764): float-alpha Int8 slow-reference precision bug.
- [#15765](https://github.com/ROCm/rocm-libraries/pull/15765): integer-reduction fix immediately after [#15259](https://github.com/ROCm/rocm-libraries/pull/15259) and before this PR.

## Device / Architecture Coverage

Host-only test code: a CPU test driver and its CTest registrations. No kernel, client or device behavior changes, so passing PR checks are sufficient.

## Submission Checklist

- [x] Look over the contributing guidelines at https://github.com/ROCm/TheRock/blob/main/GOVERNANCE.md#pull-requests.

## Risk level

Low (1/5): test-only change to the CPU-reference test driver; the client, the CPU reference and device behavior are unchanged.
````

## PR #15274

Title: `perf(tensilelite): run Int8-input GEMMs on the CPU reference fast path`

````markdown
JIRA ID : AIHPBLAS-5257

## Motivation

tensilelite-client, the C++ program that runs generated GPU kernels, checks their results against a CPU reference, a CPU computation of the same matrix multiply (GEMM). The reference has a blocked, multithreaded fast path, `solveCPUFast`, and a per-element slow path. `isFastPathEligible` decides which one a problem takes. Int8-input problems always took the slow path. In the original branch's investigation, the 4111^3 Int8 problems of `streamk/sk_I8gemm_quick.yaml` took 45-73 s each there, or 9.5-11.6 s with the slow-path locking change made earlier in this stack. Same-size problems on the fast path took 0.47-1.21 s.

## Technical Details

Production change in `client/src/Reference.cpp`, which the client uses to validate results:

- `solveCPUFast` gets a separate multiply-accumulate type (`MacT`), used for the K-loop sums, from its epilogue type (`AccumT`), used for alpha, beta, bias, activation and the output. Existing instantiations keep `MacT = AccumT`.
- `isFastPathEligible` accepts Int8 inputs for exactly the slow path's 6 type ids: `I8_I8_I32`, `I8_I32_I32`, `I8_I32_S`, `I8_I8_S`, `I8_H_S` and `I8_B_S`. MX with Int8 stays on the slow path.
- For these, the K loop uses an Int32 reduction, separate from the Int32 or float epilogue. The epilogue runs in the slow path's type (int32 or float), in the slow path's order. The output uses the slow path's `SaturateCast`: Int8 rounds half to even and then clamps to [-128, 127].

Behavior change: eligible Int8 problems are validated by the blocked CPU fast path. Integer dot-product accumulation is separate from the Int32 or float epilogue, which retains the reference’s operation order and saturating output conversion. [#15765](https://github.com/ROCm/rocm-libraries/pull/15765) supplies the corresponding integer reduction for the slow path’s float-alpha Int8 cases, fixing the precision loss tracked in [#15764](https://github.com/ROCm/rocm-libraries/issues/15764).

Tests: `cpu-gemm-driver`, the CPU-reference test program, gains `--int8Min` and `--int8Max`. `tests/CMakeLists.txt` adds 84 fast-path cases next to the slow-path Int8 cases added earlier in this stack, 16 narrow-range cases and 3 option checks. `tests/ReferenceFastPath_test.cpp` adds 8 gtests covering exact sums, wrapping, C/D aliasing and fast/slow behavior.

The fast reduction uses unsigned arithmetic for defined modulo-2^32 accumulation, followed by signed Int32 interpretation. The corrected slow float-alpha reduction in #15765 uses the same policy. Slow integer-alpha arithmetic retains its existing behavior. The regression checks both reference paths against the independent sum 16,790,292, and C/D aliasing preserves the original C value for the beta term.

## Test Plan

Configure from `projects/hipblaslt`:

```sh
cmake --preset tensilelite -S . -B "$BUILD" -DTENSILELITE_BUILD_TESTING=ON
cmake --build "$BUILD" --target tensilelite-tests cpu-gemm-driver tensilelite-client --parallel 8
ctest --test-dir "$BUILD/tensilelite/tests" -L '^standard$' \
  --output-on-failure --parallel 8 --no-tests=error
```

Install the `tests` and `tensilelite` components to a separate prefix, provide the ROCm runtime dependencies, and run the same standard label under `bin/hipblaslt-tensilelite` with `LD_LIBRARY_PATH` unset. Run focused reference tests by invoking `tensilelite-tests` directly; CTest groups gtests into category suites. Run root and hipBLASLt component pre-commit checks on the changed files.

## Test Result

Time saved: not measured in isolation on a test config. The closest isolated figure is `cpu-gemm-driver` on `sk_I8gemm_quick`'s 4 problem types at 4111^3: CPU reference time 36,842 ms to 2,077 ms in total (about 34.8 s), one run each, against the commit before it, on the original branch's shared gfx950 MI355X host, 25 cores, `OMP_NUM_THREADS=25`. The combined gfx950 measurement of all client changes (184 configs, cores 0-24, interleaved) shows `sk_I8gemm_quick` at 323 s pytest wall time with the old client and 6 s with the new one, a figure that also includes the slow-path locking change made earlier in this stack.

Other results recorded on the original branch:

- The 8 driver runs above were exact (maximum difference 0).
- All `CPUGemm.*` tests passed (440, at 1, 3, 4 and 32 threads); full CTest 1091 of 1091. The 361 earlier `CPUGemm` commands gave the same exit code and output as before.
- Temporary mutations (no `SaturateCast`, folded epilogue order, float MAC, an off-by-one in the last row) each made new tests fail.

Published head `f58bbb3388853e13cdf85bda0917485d590c4ddc`: [Multi-Arch CI Summary](https://github.com/ROCm/rocm-libraries/actions/runs/42914666073/job/130346019278) and [Component CI Summary](https://github.com/ROCm/rocm-libraries/actions/runs/42914663937/job/130326273026) completed successfully at this head. GitHub's [Math CI Summary](https://math-ci.amd.com/job/rocm-libraries/job/status-gate/job/hipblaslt/job/PR-15274/lastBuild/stages/) status also reports success for this head.

Integrated local validation on November 17, 2026, at stack tip `4135e6d96f2df606404ab51af3214a8b2f4a3427`, rebased onto develop `bc60c82f1d732aa8bb50c869ac3ad2e1a586e599`:

- Release builds of `tensilelite-tests`, `cpu-gemm-driver` and `tensilelite-client` passed.
- Standard build-tree and installed CTest each passed **925/925 registrations**, comprising 924 driver cases and the standard gtest suite. Installed execution used `LD_LIBRARY_PATH` unset and local ROCm runtime dependencies.
- All ten independent AMAX-D gtests passed at each OpenMP thread limit of 1, 3 and 8.
- The independent CPU Float8 matrix passed all 2,304 cases across both scale profiles, with zero slow or fast/fallback mismatches.
- gfx942 executed all 1,728 nongrouped and 384 grouped Float8 GPU cases with zero numerical failures, unexecuted cases or nonfinite outputs. The 32 baseline failures are corrected; the other 1,696 nongrouped numerical output arrays equal baseline.
- Scoped root and hipBLASLt hooks passed. The root C++/CMake hooks exclude these paths; formatting was checked separately for the new C++ edits.

These are integrated local results, including the later correctness and AMAX-D PRs. They do not establish separate CI results for each intermediate branch. No code has been pushed or run through shared CI at these local heads. Final production gfx950 validation, including the Float8 correction and dedicated gradient/MX-tail qualification, remains pending. No new performance measurement is claimed.

## Related

- Work tracking: AIHPBLAS-5257
- [#15764](https://github.com/ROCm/rocm-libraries/issues/15764) — slow-reference precision loss for Int8 inputs with float alpha/beta.
- [#15765](https://github.com/ROCm/rocm-libraries/pull/15765) — integer reduction for those slow-reference cases.
- [#15266](https://github.com/ROCm/rocm-libraries/pull/15266) — preceding CPU GEMM driver Int8 coverage.

## Device / Architecture Coverage

Client host code: the CPU reference that validates GPU results. It is arch-independent; it applies to Int8 problems on every arch. No kernel or device code changes. Passing PR checks are sufficient.

## Submission Checklist

- [x] Look over the contributing guidelines at https://github.com/ROCm/TheRock/blob/main/GOVERNANCE.md#pull-requests.

## Risk level

Moderate (3/5): the CPU reference determines pass/fail for validated Int8 problems. Tests cover the six eligible type IDs and the integer-reduction and epilogue behavior.
````

## PR #15275

Title: `perf(tensilelite): run Float8-output GEMMs on the CPU reference fast path`

````markdown
JIRA ID : AIHPBLAS-5257

## Motivation

`tensilelite-client` validates generated GPU kernels against a CPU matrix-multiply reference. Float8 and BFloat8 outputs always used its per-element slow path. This PR allows those 8-bit outputs on the existing blocked, multithreaded fast path, reducing CPU validation time.

## Technical Details

- Fast eligibility covers the slow reference's 24 Float8-output type IDs: Float8/BFloat8 inputs and hybrids within one encoding family, plus mixed Half/Float8 inputs. OCP is the Open Compute Project encoding; fnuz is the variant without negative zero. These combinations use float alpha, beta and accumulation.
- Scalar scaling falls back when Half storage is narrowed to Float8 for multiplication. Vector narrowing remains eligible and relies on the preceding Float8 input-scaling correctness PR, which applies both vectors after the dot product.
- Both paths use `SaturateCast` for output conversion. Finite values round to nearest even and saturate at the largest finite value. OCP E4M3 and both fnuz formats turn either infinity into NaN; OCP E5M2 preserves signed infinity. All four preserve NaN.

Blocked summation and the folded scale calculation can move a result by one Float8 step. The client's relative tolerances, 0.125 for Float8 and 0.25 for BFloat8, cover that difference.

The CPU test driver adds Float8 outputs, exact comparisons and rejection of one-sided NaN. The PR adds 207 driver CTests and gtests for eligibility, Scalar fallback, Vector results and output-cast limits, including non-finite inputs.

## Test Plan

Configure from `projects/hipblaslt`:

```sh
cmake --preset tensilelite -S . -B "$BUILD" -DTENSILELITE_BUILD_TESTING=ON
cmake --build "$BUILD" --target tensilelite-tests cpu-gemm-driver tensilelite-client --parallel 8
ctest --test-dir "$BUILD/tensilelite/tests" -L '^standard$' \
  --output-on-failure --parallel 8 --no-tests=error
```

Install the `tests` and `tensilelite` components to a separate prefix, provide the ROCm runtime dependencies, and run the same standard label under `bin/hipblaslt-tensilelite` with `LD_LIBRARY_PATH` unset. Run focused reference tests by invoking `tensilelite-tests` directly; CTest groups gtests into category suites. Run root and hipBLASLt component pre-commit checks on the changed files.

## Test Result

Published head `b6828c46316720641f2e3f15a1320201d4f1c34b`: [Multi-Arch CI](https://github.com/ROCm/rocm-libraries/actions/runs/42914665491), [TensileLite coverage](https://github.com/ROCm/rocm-libraries/actions/runs/42914663544/job/130174298919), Math CI [preliminary](https://math-ci.amd.com/job/rocm-libraries/job/preliminary/job/hipblaslt/job/PR-15275/7/stages/) and [tensilelite-unit-codecov](https://math-ci.amd.com/job/rocm-libraries/job/tensilelite-unit-codecov/job/hipblaslt/job/PR-15275/7/stages/) passed. These results cover the published source before the local review fixes.

Original-branch timing measurements: `streamk/sk_f8gemm_quick.yaml` run-phase pytest wall time fell from 58.5 s to 11.4 s, saving 47.1 s; CPU reference time fell from 51.3 s to 4.1 s. This was one run per side on a shared gfx950 MI355X host with 25 cores and `OMP_NUM_THREADS=25`, replaying the run phase with the adjacent clients. The replay passed. `gemm/gfx950/f8f16mix_f8s.yaml` did not change because `UseScaleCD` excludes the fast path. These are historical measurements; no timing rerun is claimed for the review fixes.

On that original branch and host, `cpu-gemm-driver` at 4111^3 for `F8_F8_S` fell from 11,225 ms to 582 ms (TT) and 11,613 ms to 724 ms (NT), with maximum difference 0. All 647 driver cases passed at 1, 3, 4 and 32 threads; the full CTest run passed 1,300 of 1,300 tests.

Integrated local validation on November 17, 2026, at stack tip `4135e6d96f2df606404ab51af3214a8b2f4a3427`, rebased onto develop `bc60c82f1d732aa8bb50c869ac3ad2e1a586e599`:

- Release builds of `tensilelite-tests`, `cpu-gemm-driver` and `tensilelite-client` passed.
- Standard build-tree and installed CTest each passed **925/925 registrations**, comprising 924 driver cases and the standard gtest suite. Installed execution used `LD_LIBRARY_PATH` unset and local ROCm runtime dependencies.
- All ten independent AMAX-D gtests passed at each OpenMP thread limit of 1, 3 and 8.
- The independent CPU Float8 matrix passed all 2,304 cases across both scale profiles, with zero slow or fast/fallback mismatches.
- gfx942 executed all 1,728 nongrouped and 384 grouped Float8 GPU cases with zero numerical failures, unexecuted cases or nonfinite outputs. The 32 baseline failures are corrected; the other 1,696 nongrouped numerical output arrays equal baseline.
- Scoped root and hipBLASLt hooks passed. The root C++/CMake hooks exclude these paths; formatting was checked separately for the new C++ edits.

These are integrated local results, including the later correctness and AMAX-D PRs. They do not establish separate CI results for each intermediate branch. No code has been pushed or run through shared CI at these local heads. Final production gfx950 validation, including the Float8 correction and dedicated gradient/MX-tail qualification, remains pending. No new performance measurement is claimed.

## Related

- Work tracking: AIHPBLAS-5257
- Depends on [#15274](https://github.com/ROCm/rocm-libraries/pull/15274) through the preceding Float8 input-scaling correctness PR. That PR owns the slow Vector scaling and split-K conversion fixes.

## Device / Architecture Coverage

This PR changes the CPU reference and test driver; it contains no GPU-kernel changes. Host numerical tests cover both OCP and fnuz encodings. Final-production gfx950 validation of the preceding Float8 input-scaling correctness PR remains outstanding.

## Submission Checklist

- [x] Look over the contributing guidelines at https://github.com/ROCm/TheRock/blob/main/GOVERNANCE.md#pull-requests.

## Risk level

Moderate (3/5): the CPU reference decides pass/fail for 24 Float8-output type ids. Blocked summation can move a result by one Float8 step within the client's tolerances. Scalar Half-to-Float8 narrowing falls back; Vector narrowing relies on the preceding correctness PR.
````

## PR #15276

Title: `perf(tensilelite): run complex GEMMs on the CPU reference fast path`

````markdown
JIRA ID : AIHPBLAS-5257

## Motivation

tensilelite-client, the C++ program that runs generated GPU kernels, checks their results against a CPU reference, a CPU computation of the same matrix multiply (GEMM). The reference has a blocked, multithreaded fast path and a per-element slow path. Complex-number problems always took the slow path. That affects 5 common configs: `gemm/bufferLoadStore0`, `gemm/cgemm`, `gemm/zgemm`, `streamk/sk_cgemm_quick` and `streamk/sk_zgemm_quick`. The cost is small, about 1 s in total for them after the earlier slow-path changes in this stack. Complex types were one of the remaining reasons the original branch's investigation found for problems leaving the fast path.

## Technical Details

Production change in `client/src/Reference.cpp`, which the client uses to validate results:

- `isFastPathEligible` accepts exactly the slow path's complex type ids, `C_C_C` (complex float) and `Z_Z_Z` (complex double), including a complex-conjugate operation on A, B or both. `SolveGemmCPU` dispatches them to `solveCPUFast<std::complex<float>>` or `<std::complex<double>>`.
- `ShadowBuffer`, the fast path's copy of A and B, holds the conjugate of each element when the problem conjugates that operand. The epilogue computes `alpha * sum + beta * C` and writes D in place.
- Complex problems with bias, ScaleAB, scaleAlphaVec, activation or gate residual stay on the slow path, which reads those inputs as zero. Complex with MX and other complex type combinations also stay there.

The tested finite-result cases match bitwise. Non-finite cases, including overflow from finite inputs, can differ between reference paths.

Tests: `cpu-gemm-driver`, the CPU-reference test program, gains `--type c|z`, `--conjA` and `--conjB`. `tests/CMakeLists.txt` adds 53 CTest cases: fast/slow pairs, injected-failure cases and 7 option checks. `tests/ReferenceFastPath_test.cpp` adds 4 gtests: eligibility, conjugation in all 4 combinations, C/D aliasing, and a bitwise fast-versus-slow comparison on finite-result random data.

## Test Plan

Configure from `projects/hipblaslt`:

```sh
cmake --preset tensilelite -S . -B "$BUILD" -DTENSILELITE_BUILD_TESTING=ON
cmake --build "$BUILD" --target tensilelite-tests cpu-gemm-driver tensilelite-client --parallel 8
ctest --test-dir "$BUILD/tensilelite/tests" -L '^standard$' \
  --output-on-failure --parallel 8 --no-tests=error
```

Install the `tests` and `tensilelite` components to a separate prefix, provide the ROCm runtime dependencies, and run the same standard label under `bin/hipblaslt-tensilelite` with `LD_LIBRARY_PATH` unset. Run focused reference tests by invoking `tensilelite-tests` directly; CTest groups gtests into category suites. Run root and hipBLASLt component pre-commit checks on the changed files.

## Test Result

At published head `5b4f85f05f2977d06f67cf5405eff41da6800f93`, the [Math CI TensileLite unit/CTest job](https://math-ci.amd.com/job/rocm-libraries/job/tensilelite-unit-codecov/job/hipblaslt/job/PR-15276/7/stages/) passed. [TheRock’s gfx94X common-test job](https://github.com/ROCm/rocm-libraries/actions/runs/42914667575/job/130304234756) ran and passed all five complex configs: `gemm/cgemm`, `gemm/zgemm`, `gemm/bufferLoadStore0`, `streamk/sk_cgemm_quick` and `streamk/sk_zgemm_quick`.

Time saved: about 0.53 s of CPU reference time over the 5 complex configs (975.7 ms to 445.0 ms in total), with wall-time changes within noise. Measured on the original branch on a shared gfx950 MI355X host, 25 cores, `OMP_NUM_THREADS=25`, one run per side, replaying the run phase with the previous commit's client and this one.

Other results recorded on the original branch, same host:

- `cpu-gemm-driver`: complex float 256x256x4096 NN 62.2 ms to 37.9 ms; complex double 128x128x4096 with both operands conjugated 32.0 ms to 13.5 ms. All exact.
- In the replay, all 315 complex reference calls took the fast path, and all 5 configs passed.
- All `CPUGemm.*` tests passed (700, at 1, 3, 4 and 32 threads); full CTest 1356 of 1356. The 647 earlier `CPUGemm` commands gave the same output.

Integrated local validation on November 17, 2026, at stack tip `4135e6d96f2df606404ab51af3214a8b2f4a3427`, rebased onto develop `bc60c82f1d732aa8bb50c869ac3ad2e1a586e599`:

- Release builds of `tensilelite-tests`, `cpu-gemm-driver` and `tensilelite-client` passed.
- Standard build-tree and installed CTest each passed **925/925 registrations**, comprising 924 driver cases and the standard gtest suite. Installed execution used `LD_LIBRARY_PATH` unset and local ROCm runtime dependencies.
- All ten independent AMAX-D gtests passed at each OpenMP thread limit of 1, 3 and 8.
- The independent CPU Float8 matrix passed all 2,304 cases across both scale profiles, with zero slow or fast/fallback mismatches.
- gfx942 executed all 1,728 nongrouped and 384 grouped Float8 GPU cases with zero numerical failures, unexecuted cases or nonfinite outputs. The 32 baseline failures are corrected; the other 1,696 nongrouped numerical output arrays equal baseline.
- Scoped root and hipBLASLt hooks passed. The root C++/CMake hooks exclude these paths; formatting was checked separately for the new C++ edits.

These are integrated local results, including the later correctness and AMAX-D PRs. They do not establish separate CI results for each intermediate branch. No code has been pushed or run through shared CI at these local heads. Final production gfx950 validation, including the Float8 correction and dedicated gradient/MX-tail qualification, remains pending. No new performance measurement is claimed.

## Related

- Work tracking: AIHPBLAS-5257
- Depends on [#15275](https://github.com/ROCm/rocm-libraries/pull/15275).

## Device / Architecture Coverage

Client host code: the CPU reference that validates GPU results. It is arch-independent; it applies to complex problems on every arch that runs them. No kernel or device code changes. Passing PR checks are sufficient.

## Submission Checklist

- [x] Look over the contributing guidelines at https://github.com/ROCm/TheRock/blob/main/GOVERNANCE.md#pull-requests.

## Risk level

Low (2/5): the CPU reference for complex problems moves to the fast path, with bitwise-identical results in the tested finite-result cases. Overflow and other non-finite behavior can differ between the paths.
````

## PR #15277

Title: `perf(tensilelite): write E and run gradient GEMMs on the CPU reference fast path`

````markdown
JIRA ID : AIHPBLAS-5257

## Motivation

tensilelite-client, the C++ program that runs generated GPU kernels, checks their results against a CPU reference, a CPU computation of the same matrix multiply (GEMM). The reference has a blocked, multithreaded fast path and a per-element slow path. 2 problem kinds always took the slow path. `useE` problems also write E, an extra output holding each value before the activation function. Gradient problems (`useGradient`) multiply by the activation's derivative at E and can reduce a bias gradient, a sum over rows or columns. In the original branch's investigation they made 18 and 47 slow-path calls in the gfx950 common tests.

## Technical Details

Production change in `client/src/Reference.cpp`, which the client uses to validate results:

- `useE`: the fast-path epilogue stores E as the slow path does. It stores the value after alpha, scales, `beta * C` and bias, before the activation, converted with `SetValue` to E's own type at E's own strides. Problems whose alpha and beta types differ stay on the slow path, which throws for them. The alpha/beta type lookup is factored into `getAlphaBetaTypes` for this check.
- Gradient: the fast path adds no bias and does not write E. With an activation it multiplies by the activation of E. A bias gradient is reduced by the slow path's own `ReductionCPU`, through the new `reduceBiasGradient`, from D, A or B. Bias sources whose shape `ReductionCPU` does not support stay on the slow path (`isBiasReductionShapeSupported`, shared with it).

Both are in one PR because they share the test driver's output comparison code. Eligible `useE` and gradient problems are now validated by the fast path. Floating-point epilogue values can differ in their last bits when evaluation order differs. Non-HPA `H_H_H` and `B_B_S` gradient problems with a bias output remain on the slow path, preserving Half/BFloat16 accumulation. HPA means higher-precision accumulation; those configurations remain fast-eligible and reduce in float.

Tests: `cpu-gemm-driver`, the CPU-reference test program, gains `--useE`, `--gradient` and `--biasSrc d|a|b`. `tests/CMakeLists.txt` adds 70 CTest cases: 64 fast/slow cases and 6 option checks. `tests/ReferenceFastPath_test.cpp` adds 8 gtests on eligibility, E contents, finite-data gradient equality and non-HPA/HPA bias-gradient accumulation.

## Test Plan

Configure from `projects/hipblaslt`:

```sh
cmake --preset tensilelite -S . -B "$BUILD" -DTENSILELITE_BUILD_TESTING=ON
cmake --build "$BUILD" --target tensilelite-tests cpu-gemm-driver tensilelite-client --parallel 8
ctest --test-dir "$BUILD/tensilelite/tests" -L '^standard$' \
  --output-on-failure --parallel 8 --no-tests=error
```

Install the `tests` and `tensilelite` components to a separate prefix, provide the ROCm runtime dependencies, and run the same standard label under `bin/hipblaslt-tensilelite` with `LD_LIBRARY_PATH` unset. Run focused reference tests by invoking `tensilelite-tests` directly; CTest groups gtests into category suites. Run root and hipBLASLt component pre-commit checks on the changed files.

## Test Result

Published PR head: `46ce0b0c3a676b3878f652e4dbf136bdb1f30f9f`.

- [Math CI C++ coverage, build 7](https://math-ci.amd.com/job/rocm-libraries/job/tensilelite-unit-codecov/job/hipblaslt/job/PR-15277/7/): 1,674 CTests passed, 4 skipped and 1 disabled; no failures. Math CI tested merge `103c270621709258b11dd7fa5daff5cadb0b4ab9`, combining this PR head with its then-current #15276 base `d72ce11dbab4b6ad1f84b42e0350e542dfac0749`.
- [TheRock gfx942 common tests](https://github.com/ROCm/rocm-libraries/actions/runs/42914664204/job/130385137352): 164 passed and 570 skipped. The three gradient configs passed: `fp16_gradient_bias.yaml`, `fp16_gradient_activation.yaml` and `fp8bf8nss_gradient_bias_b.yaml`.
- [gfx1250-strict common tests](https://github.com/ROCm/rocm-libraries/actions/runs/42914662388/job/130174079118): 236 passed, 482 skipped and 16 expected failures; the job passed.

Time saved: each part was measured against the commit before it on the original branch, on a shared gfx950 MI355X host, 25 cores, `OMP_NUM_THREADS=25`, one run per side, replaying the run phase of the affected configs:

| Part | Configs | CPU reference time | Pytest wall time, summed |
|---|---|---|---|
| `useE` | 3 | 6,712.6 ms to 2,561.1 ms (-4.15 s) | 17.8 s to 13.6 s |
| Gradient | 4 | 4,369.3 ms to 410.9 ms (-3.96 s) | 22.7 s to 18.7 s |

Other results recorded on the original branch, same host:

- All 18 `useE` calls and all 47 gradient calls took the fast path, and no slow-path call remained in the 4 gradient configs. All configs passed.
- `cpu-gemm-driver`, 1031x1031x4103: `useE` 919.9 ms to 54.5 ms; gradient with bias from D 802.3 ms to 48.8 ms. D, E and bias exact.
- All `CPUGemm.*` tests passed (770, at 1, 3, 4 and 32 threads); full CTest 1431 of 1431.

Integrated local validation on November 17, 2026, at stack tip `4135e6d96f2df606404ab51af3214a8b2f4a3427`, rebased onto develop `bc60c82f1d732aa8bb50c869ac3ad2e1a586e599`:

- Release builds of `tensilelite-tests`, `cpu-gemm-driver` and `tensilelite-client` passed.
- Standard build-tree and installed CTest each passed **925/925 registrations**, comprising 924 driver cases and the standard gtest suite. Installed execution used `LD_LIBRARY_PATH` unset and local ROCm runtime dependencies.
- All ten independent AMAX-D gtests passed at each OpenMP thread limit of 1, 3 and 8.
- The independent CPU Float8 matrix passed all 2,304 cases across both scale profiles, with zero slow or fast/fallback mismatches.
- gfx942 executed all 1,728 nongrouped and 384 grouped Float8 GPU cases with zero numerical failures, unexecuted cases or nonfinite outputs. The 32 baseline failures are corrected; the other 1,696 nongrouped numerical output arrays equal baseline.
- Scoped root and hipBLASLt hooks passed. The root C++/CMake hooks exclude these paths; formatting was checked separately for the new C++ edits.

These are integrated local results, including the later correctness and AMAX-D PRs. They do not establish separate CI results for each intermediate branch. No code has been pushed or run through shared CI at these local heads. Final production gfx950 validation, including the Float8 correction and dedicated gradient/MX-tail qualification, remains pending. No new performance measurement is claimed.

## Related

- Work tracking: AIHPBLAS-5257
- Depends on #15276: [complex CPU-reference fast path](https://github.com/ROCm/rocm-libraries/pull/15276).

## Device / Architecture Coverage

Client host code: the CPU reference that validates GPU results. It is arch-independent; it applies to `useE` and gradient problems on every arch that runs them. No kernel or device code changes. Passing PR checks are sufficient.

## Submission Checklist

- [x] Look over the contributing guidelines at https://github.com/ROCm/TheRock/blob/main/GOVERNANCE.md#pull-requests.

## Risk level

Moderate (3/5): the CPU reference decides pass/fail for `useE` and gradient problems, including a new bias-gradient path. Values can differ from the slow path in the last bits. This is the largest of the fast-path changes.
````

## PR #15278

Title: `perf(tensilelite): run MX GEMMs with a partial last MX block on the CPU reference fast path`

````markdown
JIRA ID : AIHPBLAS-5257

## Motivation

tensilelite-client checks generated GPU matrix multiplies against a CPU reference. MX, or microscaling, formats share a scale across a block along K, the summation dimension. The blocked CPU fast path rejected problems with a partial final MX block, forcing them onto the per-element slow reference. This change allows those problems to use the fast path's existing loads that zero elements beyond K.

## Technical Details

- Remove the fast-path eligibility restriction that K must be divisible by both MX block sizes. Blocks must still be multiples of the fast path's 8-element K tile; each tile uses one scale from each side.
- The production slow reference already stops its final segment at K, from [#13630](https://github.com/ROCm/rocm-libraries/pull/13630). The driver now stops its own expected-result sum at K and accepts equal-block tails with either execution preference. It continues rejecting asymmetric blocks on slow execution.
- Add 18 fast-tail driver cases and 15 equal-block slow counterparts, and convert two old rejection cases into positive slow-tail checks. Two new gtests use independent sums over `k < K` with nonzero padding beyond K. The exact vector assertion also rejects NaN output and reports the case settings.
- All 38 negative driver cases require exit code 1 and the intended diagnostic. The Float8 and complex injected-failure cases use the shared registration helper, which includes them in the standard and installed suites.
- Forced fast execution now reports the first eligibility rejection reason. Optional diagnostic output leaves ordinary client fallback silent and preserves every eligibility condition. Two driver cases check the AMAX-D and MX block-size messages.

## Test Plan

Configure from `projects/hipblaslt`:

```sh
cmake --preset tensilelite -S . -B "$BUILD" -DTENSILELITE_BUILD_TESTING=ON
cmake --build "$BUILD" --target tensilelite-tests cpu-gemm-driver tensilelite-client --parallel 8
ctest --test-dir "$BUILD/tensilelite/tests" -L '^standard$' \
  --output-on-failure --parallel 8 --no-tests=error
```

Install the `tests` and `tensilelite` components to a separate prefix, provide the ROCm runtime dependencies, and run the same standard label under `bin/hipblaslt-tensilelite` with `LD_LIBRARY_PATH` unset. Run focused reference tests by invoking `tensilelite-tests` directly; CTest groups gtests into category suites. Run root and hipBLASLt component pre-commit checks on the changed files.

## Test Result

Published head `e24bb5950c6eacb30a5eb2e201d7e5fba9af6848`: [Math CI's gfx950 common suite](https://math-ci.amd.com/job/rocm-libraries/job/preliminary/job/hipblaslt/job/PR-15278/7/execution/node/1115/log) passed with 239 passed and 495 skipped, including `gemm/gfx950/mxfp4_mxfp4_bf16_tn_act.yaml`. [GitHub Actions on gfx942](https://github.com/ROCm/rocm-libraries/actions/runs/42914665365/job/130345001958) passed with 164 passed and 570 skipped; it skipped that gfx950-only configuration.

Integrated local validation on November 17, 2026, at stack tip `4135e6d96f2df606404ab51af3214a8b2f4a3427`, rebased onto develop `bc60c82f1d732aa8bb50c869ac3ad2e1a586e599`:

- Release builds of `tensilelite-tests`, `cpu-gemm-driver` and `tensilelite-client` passed.
- Standard build-tree and installed CTest each passed **925/925 registrations**, comprising 924 driver cases and the standard gtest suite. Installed execution used `LD_LIBRARY_PATH` unset and local ROCm runtime dependencies.
- All ten independent AMAX-D gtests passed at each OpenMP thread limit of 1, 3 and 8.
- The independent CPU Float8 matrix passed all 2,304 cases across both scale profiles, with zero slow or fast/fallback mismatches.
- gfx942 executed all 1,728 nongrouped and 384 grouped Float8 GPU cases with zero numerical failures, unexecuted cases or nonfinite outputs. The 32 baseline failures are corrected; the other 1,696 nongrouped numerical output arrays equal baseline.
- Scoped root and hipBLASLt hooks passed. The root C++/CMake hooks exclude these paths; formatting was checked separately for the new C++ edits.

These are integrated local results, including the later correctness and AMAX-D PRs. They do not establish separate CI results for each intermediate branch. No code has been pushed or run through shared CI at these local heads. Final production gfx950 validation, including the Float8 correction and dedicated gradient/MX-tail qualification, remains pending. No new performance measurement is claimed.

## Related

- Work tracking: [AIHPBLAS-5257](https://amd-hub.atlassian.net/browse/AIHPBLAS-5257).
- Depends on [#15277](https://github.com/ROCm/rocm-libraries/pull/15277).
- Existing slow-tail clamp: [#13630](https://github.com/ROCm/rocm-libraries/pull/13630).

## Device / Architecture Coverage

CPU reference and driver changes; no GPU kernel code changes. The published-head gfx950 run covers the original MX-tail change. The local review fixes have CPU numerical validation; gfx950 validation of the final pushed head remains pending.

## Submission Checklist

- [x] Look over the contributing guidelines at https://github.com/ROCm/TheRock/blob/main/GOVERNANCE.md#pull-requests.

## Risk level

Moderate (3/5): more MX problems use the blocked CPU reference. Changing accumulation order can change reference values and affect GPU validation tolerances. Equal-block slow execution remains available and clamps the final segment at K; asymmetric slow blocks remain rejected.
````

## New float8 PR

Title: `fix(tensilelite): correct scaling around Float8 input conversion`

````markdown
JIRA ID : AIHPBLAS-5257
Fixes #<FLOAT8_ISSUE_NUMBER>

## Motivation

Half inputs narrowed to Float8 reveal two inherited scaling defects. The slow Vector reference omits the narrowed operand’s scale. MultipleBuffer split-K applies a Scalar scale a second time in conversion after the main kernel has consumed it. These defects were found while reviewing #15275 and are present below that PR.

## Technical Details

Apply both Vector scales after the narrowed dot product. For MultipleBuffer conversion only, bind a unit Scalar scale for an operand narrowed from Half to Float8. Preserve the main kernel’s original scale and the other operand’s scale. The shared invocation builder handles ordinary and grouped arguments.

Add three independent slow-reference gtests and three conversion-argument gtests, including grouped construction, to the existing standard suite. The tests fail before their respective corrections and pass after them. The Scalar-only fast fallback belongs to the following #15275 PR; Vector remains eligible there.

## Test Plan

Configure from `projects/hipblaslt`:

```sh
cmake --preset tensilelite -S . -B "$BUILD" -DTENSILELITE_BUILD_TESTING=ON
cmake --build "$BUILD" --target tensilelite-tests cpu-gemm-driver tensilelite-client --parallel 8
ctest --test-dir "$BUILD/tensilelite/tests" -L '^standard$' \
  --output-on-failure --parallel 8 --no-tests=error
```

Install the `tests` and `tensilelite` components to a separate prefix, provide the ROCm runtime dependencies, and run the same standard label under `bin/hipblaslt-tensilelite` with `LD_LIBRARY_PATH` unset. Run focused reference tests by invoking `tensilelite-tests` directly; CTest groups gtests into category suites. Run root and hipBLASLt component pre-commit checks on the changed files.

Run `tensilelite-tests --gtest_filter='ReferenceFloat8Scaling.*:Float8ConversionArguments.*'`. Execute nongrouped and grouped GPU matrices with both Half operand directions, Scalar/Vector/no scaling, four layouts, GSU 1/2/4, MultipleBuffer/MultipleBufferSingleKernel and both epilogues. Compare against an independent arithmetic model.

## Test Result

Integrated local validation on November 17, 2026, at stack tip `4135e6d96f2df606404ab51af3214a8b2f4a3427`, rebased onto develop `bc60c82f1d732aa8bb50c869ac3ad2e1a586e599`:

- Release builds of `tensilelite-tests`, `cpu-gemm-driver` and `tensilelite-client` passed.
- Standard build-tree and installed CTest each passed **925/925 registrations**, comprising 924 driver cases and the standard gtest suite. Installed execution used `LD_LIBRARY_PATH` unset and local ROCm runtime dependencies.
- All ten independent AMAX-D gtests passed at each OpenMP thread limit of 1, 3 and 8.
- The independent CPU Float8 matrix passed all 2,304 cases across both scale profiles, with zero slow or fast/fallback mismatches.
- gfx942 executed all 1,728 nongrouped and 384 grouped Float8 GPU cases with zero numerical failures, unexecuted cases or nonfinite outputs. The 32 baseline failures are corrected; the other 1,696 nongrouped numerical output arrays equal baseline.
- Scoped root and hipBLASLt hooks passed. The root C++/CMake hooks exclude these paths; formatting was checked separately for the new C++ edits.

These are integrated local results, including the later correctness and AMAX-D PRs. They do not establish separate CI results for each intermediate branch. No code has been pushed or run through shared CI at these local heads. Final production gfx950 validation, including the Float8 correction and dedicated gradient/MX-tail qualification, remains pending. No new performance measurement is claimed.

## Related

- Discovery PR: [#15275](https://github.com/ROCm/rocm-libraries/pull/15275).
- Stack base: [#15274](https://github.com/ROCm/rocm-libraries/pull/15274); next PR: #15275.
- Work tracking: [AIHPBLAS-5257](https://amd-hub.atlassian.net/browse/AIHPBLAS-5257).

## Device / Architecture Coverage

Host tests exercise both OCP and fnuz Float8 encodings where applicable. Numerical GPU execution in this implementation used gfx942. The user confirmed the earlier Float8 proposal on gfx950 at remote head `b6828c46316720641f2e3f15a1320201d4f1c34b`; final production gfx950 qualification remains pending.

## Submission Checklist

- [x] Look over the contributing guidelines at https://github.com/ROCm/TheRock/blob/main/GOVERNANCE.md#pull-requests.

## Risk level

Moderate (3/5): changes CPU expected results and GPU conversion argument binding for scaled narrowing. Controls retain identical numerical arrays on gfx942. Shared CI and final production gfx950 results remain required.
````

## New bias PR

Title: `fix(tensilelite): share bias reduction and report unsupported shapes`

````markdown
JIRA ID : AIHPBLAS-5257
Fixes #16120

## Motivation

The slow reference constructs exceptions without throwing them when bias-gradient reduction rejects a shape, and its D-source error path can free the workspace twice. Unsupported batched reductions can therefore appear successful or fail during cleanup. This inherited problem was identified during #15277 review and is tracked in #16120.

## Technical Details

Use automatic workspace ownership and share `reduceBiasGradient` between reference paths. Propagate actual reduction errors for A, B and D sources, guard unsupported complex reductions, and report the rejected dimension/batch count. Preserve the current 2-D or single-batch limitation.

Apply [Newling’s suggestion](https://github.com/newling/rocm-libraries/commit/66106f99d916da886beecc39e464681596074b6d), with a regression covering every bias source through both requested paths, checking the exception and unchanged bias output. The regression failed on the unpatched A-source path because no exception was thrown; this run did not separately reproduce the D double-free.

## Test Plan

Configure from `projects/hipblaslt`:

```sh
cmake --preset tensilelite -S . -B "$BUILD" -DTENSILELITE_BUILD_TESTING=ON
cmake --build "$BUILD" --target tensilelite-tests cpu-gemm-driver tensilelite-client --parallel 8
ctest --test-dir "$BUILD/tensilelite/tests" -L '^standard$' \
  --output-on-failure --parallel 8 --no-tests=error
```

Install the `tests` and `tensilelite` components to a separate prefix, provide the ROCm runtime dependencies, and run the same standard label under `bin/hipblaslt-tensilelite` with `LD_LIBRARY_PATH` unset. Run focused reference tests by invoking `tensilelite-tests` directly; CTest groups gtests into category suites. Run root and hipBLASLt component pre-commit checks on the changed files.

Run `tensilelite-tests --gtest_filter='ReferenceFastPath.BatchedBiasGradientReportsUnsupportedShape:ReferenceFastPath.*Gradient*'`. All 55 focused tests passed after the original fix; the final integrated standard suite also passes.

## Test Result

Integrated local validation on November 17, 2026, at stack tip `4135e6d96f2df606404ab51af3214a8b2f4a3427`, rebased onto develop `bc60c82f1d732aa8bb50c869ac3ad2e1a586e599`:

- Release builds of `tensilelite-tests`, `cpu-gemm-driver` and `tensilelite-client` passed.
- Standard build-tree and installed CTest each passed **925/925 registrations**, comprising 924 driver cases and the standard gtest suite. Installed execution used `LD_LIBRARY_PATH` unset and local ROCm runtime dependencies.
- All ten independent AMAX-D gtests passed at each OpenMP thread limit of 1, 3 and 8.
- The independent CPU Float8 matrix passed all 2,304 cases across both scale profiles, with zero slow or fast/fallback mismatches.
- gfx942 executed all 1,728 nongrouped and 384 grouped Float8 GPU cases with zero numerical failures, unexecuted cases or nonfinite outputs. The 32 baseline failures are corrected; the other 1,696 nongrouped numerical output arrays equal baseline.
- Scoped root and hipBLASLt hooks passed. The root C++/CMake hooks exclude these paths; formatting was checked separately for the new C++ edits.

These are integrated local results, including the later correctness and AMAX-D PRs. They do not establish separate CI results for each intermediate branch. No code has been pushed or run through shared CI at these local heads. Final production gfx950 validation, including the Float8 correction and dedicated gradient/MX-tail qualification, remains pending. No new performance measurement is claimed.

## Related

- Bug: [#16120](https://github.com/ROCm/rocm-libraries/issues/16120), assigned to Alex-Vasile.
- Discovery PR: [#15277](https://github.com/ROCm/rocm-libraries/pull/15277).
- Stack base: [#15278](https://github.com/ROCm/rocm-libraries/pull/15278); AMAX-D follows this PR.

## Device / Architecture Coverage

Host tests exercise both OCP and fnuz Float8 encodings where applicable. Numerical GPU execution in this implementation used gfx942. The user confirmed the earlier Float8 proposal on gfx950 at remote head `b6828c46316720641f2e3f15a1320201d4f1c34b`; final production gfx950 qualification remains pending.

## Submission Checklist

- [x] Look over the contributing guidelines at https://github.com/ROCm/TheRock/blob/main/GOVERNANCE.md#pull-requests.

## Risk level

Moderate (3/5): shared reference cleanup and error paths change. Supported reductions retain their numerical behavior; unsupported shapes now report failure. This PR adds no batched bias-gradient support.
````

## New amax PR

Title: `feat(tensilelite): support AMAX-D in the CPU reference fast path`

````markdown
JIRA ID : AIHPBLAS-5257


## Motivation

AMAX-D problems still use the per-element slow reference after #15259 improves its locking. This PR adds AMAX-D to the blocked CPU fast path as the final stack feature, addressing Newling’s request without expanding #15259.

## Technical Details

Compute an absolute maximum locally within each tile, update one maximum per OpenMP worker, then merge worker maxima after the parallel loop. Sample after activation or gradient multiplication, before gate residual and the final output cast, matching the slow reference. Complex AMAX-D and ScaleCD combinations retain their fallback.

Add ten gtests with independent D and AMAX-D expectations covering real/Int8/Float8 types, layouts, batches, activation, gradients, output narrowing and zero/NaN comparison behavior. Add positive fast driver coverage. Replace the former AMAX-D rejection case with an unsupported Scalar Half-to-Float8 narrowing case to retain diagnostic coverage.

## Test Plan

Configure from `projects/hipblaslt`:

```sh
cmake --preset tensilelite -S . -B "$BUILD" -DTENSILELITE_BUILD_TESTING=ON
cmake --build "$BUILD" --target tensilelite-tests cpu-gemm-driver tensilelite-client --parallel 8
ctest --test-dir "$BUILD/tensilelite/tests" -L '^standard$' \
  --output-on-failure --parallel 8 --no-tests=error
```

Install the `tests` and `tensilelite` components to a separate prefix, provide the ROCm runtime dependencies, and run the same standard label under `bin/hipblaslt-tensilelite` with `LD_LIBRARY_PATH` unset. Run focused reference tests by invoking `tensilelite-tests` directly; CTest groups gtests into category suites. Run root and hipBLASLt component pre-commit checks on the changed files.

Run `tensilelite-tests --gtest_filter='ReferenceAmaxD.*'` with `OMP_THREAD_LIMIT` and `OMP_NUM_THREADS` each set to 1, 3 and 8. Run standard driver cases matching `AmaxD` in both build and installed trees.

## Test Result

Integrated local validation on November 17, 2026, at stack tip `4135e6d96f2df606404ab51af3214a8b2f4a3427`, rebased onto develop `bc60c82f1d732aa8bb50c869ac3ad2e1a586e599`:

- Release builds of `tensilelite-tests`, `cpu-gemm-driver` and `tensilelite-client` passed.
- Standard build-tree and installed CTest each passed **925/925 registrations**, comprising 924 driver cases and the standard gtest suite. Installed execution used `LD_LIBRARY_PATH` unset and local ROCm runtime dependencies.
- All ten independent AMAX-D gtests passed at each OpenMP thread limit of 1, 3 and 8.
- The independent CPU Float8 matrix passed all 2,304 cases across both scale profiles, with zero slow or fast/fallback mismatches.
- gfx942 executed all 1,728 nongrouped and 384 grouped Float8 GPU cases with zero numerical failures, unexecuted cases or nonfinite outputs. The 32 baseline failures are corrected; the other 1,696 nongrouped numerical output arrays equal baseline.
- Scoped root and hipBLASLt hooks passed. The root C++/CMake hooks exclude these paths; formatting was checked separately for the new C++ edits.

These are integrated local results, including the later correctness and AMAX-D PRs. They do not establish separate CI results for each intermediate branch. No code has been pushed or run through shared CI at these local heads. Final production gfx950 validation, including the Float8 correction and dedicated gradient/MX-tail qualification, remains pending. No new performance measurement is claimed.

## Related

- Feature request: [Newling’s AMAX-D review on #15259](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5879565945).
- Final stack PR, after `users/alvasile/tl-fix-reference-bias-gradient-cleanup`.
- Work tracking: [AIHPBLAS-5257](https://amd-hub.atlassian.net/browse/AIHPBLAS-5257).

## Device / Architecture Coverage

Host tests exercise both OCP and fnuz Float8 encodings where applicable. Numerical GPU execution in this implementation used gfx942. The user confirmed the earlier Float8 proposal on gfx950 at remote head `b6828c46316720641f2e3f15a1320201d4f1c34b`; final production gfx950 qualification remains pending.

## Submission Checklist

- [x] Look over the contributing guidelines at https://github.com/ROCm/TheRock/blob/main/GOVERNANCE.md#pull-requests.

## Risk level

Moderate (3/5): extends blocked-reference eligibility and adds a parallel maximum reduction. Independent numerical checks cover the output stage and worker limits. Performance benefit has not been measured for this final change.
````

## Remaining reply draft 1

**Draft reply to Tony — not posted:**

Target: Tony: https://github.com/ROCm/rocm-libraries/pull/15275#discussion_r4568213252

Added +Inf, -Inf and NaN to all four cast tables, with explicit expectations for each reference path and `std::isnan` checks for NaN. OCP E4M3 and both fnuz types convert either infinity to NaN; OCP E5M2 preserves signed infinity. All four preserve NaN. The cast test and all 35 reference gtests pass locally.

Current validation supplement: the integrated standard build and installed suites each pass 925/925 CTests at `4135e6d96f2df606404ab51af3214a8b2f4a3427`. Code remains local and shared CI has not run this head.

## Remaining reply draft 2

**Draft reply to Newling — not posted:**

Target: Newling: a general comment on https://github.com/ROCm/rocm-libraries/pull/15275 referring to https://github.com/newling/rocm-libraries/commit/aef34bafaad8296981db27b07f6f2fe96bd6258e

Added the storage, compute-input, output and alpha/beta type selectors to the driver's runtime exception message, preserving the original exception text and exit code 1. The mixed Float8/Half error now identifies the incompatible default compute-input types. Both supported alternatives pass for both encodings and operand directions, and all 675 driver CTests pass locally.

Current validation supplement: the integrated standard build and installed suites each pass 925/925 CTests at `4135e6d96f2df606404ab51af3214a8b2f4a3427`. Code remains local and shared CI has not run this head.

## Remaining reply draft 3

**Draft reply to newling — not posted:**

Target: Newling: a general comment on https://github.com/ROCm/rocm-libraries/pull/15276 referring to the complex C/D alias finding

@newling Agreed. #15276 now inherits the shared C/D aliasing correction from #15274. I added ComplexInPlaceCDPreservesBetaInput for complex float and complex double, covering your `(11,1)` example and a case with imaginary A/B values and complex alpha/beta. Both cases failed on the old fast path and pass with the inherited fix. The production correction stays in #15274; this PR adds the complex regression.

Current validation supplement: the integrated standard build and installed suites each pass 925/925 CTests at `4135e6d96f2df606404ab51af3214a8b2f4a3427`. Code remains local and shared CI has not run this head.

## Remaining reply draft 4

##### Draft reply to Tony — not posted

Reply to [Tony's non-HPA inline comment](https://github.com/ROCm/rocm-libraries/pull/15277#discussion_r4568214232):

> Agreed. I've implemented the guard locally for non-HPA gradient+bias problems with `H_H_H` or `B_B_S`, preserving the slow path's Half/BFloat16 accumulation. The regression reproduced bias 4096 versus 2048 for Half and 512 versus 256 for BFloat16. With the guard, both requested paths return 2048 and 256. HPA configurations remain fast-eligible and retain float accumulation. All 48 focused reference tests and 795 CPU-driver CTests passed. The code is committed locally and has not been pushed.

Current validation supplement: the integrated standard build and installed suites each pass 925/925 CTests at `4135e6d96f2df606404ab51af3214a8b2f4a3427`. Code remains local and shared CI has not run this head.

## Remaining reply draft 5

##### Draft reply to Newling — not posted

Post on #15277, referring to [Newling's non-HPA finding](https://github.com/newling/agent-public-reviews/blob/ede5c5f065835ae571ce0647b4e07af2c6cb8d64/rocm-libraries/review_pr15277_correctness_20261113.md#actionable-items):

> @newling, I've taken the non-HPA eligibility guard from [de7815c2e50](https://github.com/newling/rocm-libraries/commit/de7815c2e5066f6b2ff76e589732a3b8a671e402) in #15277. The new regression reproduced the Half and BFloat16 bias-sum differences before the fix; afterward, both requested paths preserve the narrow sums. HPA controls retain fast eligibility and float accumulation, and the eligibility checks cover all three bias sources. All 48 focused reference tests and 795 CPU-driver CTests passed locally. The code is committed locally and has not been pushed. The separate cleanup/error-reporting fix remains tracked in #16120.

Current validation supplement: the integrated standard build and installed suites each pass 925/925 CTests at `4135e6d96f2df606404ab51af3214a8b2f4a3427`. Code remains local and shared CI has not run this head.

## Remaining reply draft 6

##### Draft reply to Newling — not posted

Target: a comment on [#15278](https://github.com/ROCm/rocm-libraries/pull/15278), replying to the linked exact-tail assertion finding.

Thanks, @newling. I applied your direct vector comparison and kept the per-case context with `SCOPED_TRACE`. The all-NaN control passed with the old assertion and failed with the corrected one. After removing the control, all 50 focused reference tests and 813 standard CPU-driver tests passed locally. The change is committed locally and has not been pushed.

Current validation supplement: the integrated standard build and installed suites each pass 925/925 CTests at `4135e6d96f2df606404ab51af3214a8b2f4a3427`. Code remains local and shared CI has not run this head.

## Remaining reply draft 7

##### Draft reply to Tony — not posted

Target: [Tony's driver thread](https://github.com/ROCm/rocm-libraries/pull/15278#discussion_r4568214899); also addresses [his CMake thread](https://github.com/ROCm/rocm-libraries/pull/15278#discussion_r4568214917).

You're right: the production slow reference already clamps its final MX segment at K, from #13630. The over-read claim was incorrect for equal blocks. I removed the driver's obsolete K restriction, corrected its help and comments, converted the two old rejection cases into positive slow-tail tests and added 15 slow counterparts for the equal-block fast cases. All 828 standard driver tests and 50 focused reference tests pass locally. The asymmetric-block rejection remains. I've also drafted the description correction and removed the historical scratch-build failures as evidence of a current production over-read; I have not re-established the precise cause/version of that old experiment. The code is committed locally and has not been pushed.

Current validation supplement: the integrated standard build and installed suites each pass 925/925 CTests at `4135e6d96f2df606404ab51af3214a8b2f4a3427`. Code remains local and shared CI has not run this head.

## Remaining reply draft 8

##### Draft reply to Newling — not posted

Target: a comment on [#15278](https://github.com/ROCm/rocm-libraries/pull/15278), replying to the [linked slow-tail suggestion](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15278.md#remove-the-obsolete-equal-block-slow-tail-rejection).

Thanks, @newling. I applied the equal-block slow-tail correction from your suggestion and added 15 slow counterparts for the existing equal-block fast cases. The converted K=33 test reproduced the driver rejection before the fix and passed afterward. All 828 standard driver tests and 50 focused reference tests pass locally. The asymmetric-block restriction remains, and the description correction is drafted. The code is committed locally and has not been pushed.

Current validation supplement: the integrated standard build and installed suites each pass 925/925 CTests at `4135e6d96f2df606404ab51af3214a8b2f4a3427`. Code remains local and shared CI has not run this head.

## Remaining reply draft 9

##### Draft reply to Newling — not posted

Target: a comment on [#15278](https://github.com/ROCm/rocm-libraries/pull/15278), replying to the [linked shared negative-test suggestion](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15278.md#share-negative-test-checking-across-the-stack).

Thanks, @newling. The stack already has the shared diagnostic checker, so I migrated the remaining 11 negative tests to it. All 36 now require exit code 1 and the expected message. I also moved the four Float8/complex injected-failure cases through the shared registration helper so they enter the standard and installed suites. An unrelated argument error passed the old registration and failed the corrected one; the installed checker also rejected a deliberately incorrect expected diagnostic. All 832 standard driver tests passed in both build and installed trees, and all 50 focused installed reference tests passed locally. The changes are committed locally and have not been pushed.

Current validation supplement: the integrated standard build and installed suites each pass 925/925 CTests at `4135e6d96f2df606404ab51af3214a8b2f4a3427`. Code remains local and shared CI has not run this head.

## Remaining reply draft 10

##### Draft reply to Newling — not posted

Target: a comment on [#15278](https://github.com/ROCm/rocm-libraries/pull/15278), replying to the [linked eligibility-diagnostic suggestion](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15278.md#expose-the-eligibility-reason-to-the-driver).

Thanks, @newling. I applied the optional rejection-reason output and included it in the driver's `--tryFastPath` error. Every existing eligibility condition is preserved, including the Scalar Half-to-Float8 and non-HPA bias-gradient fallbacks added during review. Normal client fallback stays silent. The new AMAX-D and MX block-4 cases check the specific messages through the shared diagnostic checker. The AMAX-D case failed with the old generic error and passed after the change. All 834 standard driver tests passed in both build and installed trees, and all 50 focused installed reference tests passed locally. The changes are committed locally and have not been pushed.

Current validation supplement: the integrated standard build and installed suites each pass 925/925 CTests at `4135e6d96f2df606404ab51af3214a8b2f4a3427`. Code remains local and shared CI has not run this head.

## Remaining reply draft 11

##### Draft reply to Tony — not posted

Target: a comment on [#15278](https://github.com/ROCm/rocm-libraries/pull/15278), responding to the CI/Test Result paragraph in [Tony's review](https://github.com/ROCm/rocm-libraries/pull/15278#pullrequestreview-5897491239).

Thanks, Tony. I rechecked the CI evidence for published head `e24bb5950c6eacb30a5eb2e201d7e5fba9af6848`. [Math CI on gfx950](https://math-ci.amd.com/job/rocm-libraries/job/preliminary/job/hipblaslt/job/PR-15278/7/execution/node/1115/log) passed the affected `mxfp4_mxfp4_bf16_tn_act.yaml` configuration, with 239 passed and 495 skipped overall. [GitHub Actions on gfx942](https://github.com/ROCm/rocm-libraries/actions/runs/42914665365/job/130345001958) passed with 164 passed and 570 skipped, but skipped that gfx950-only configuration.

The refreshed description draft links those results and separately records local validation through `dafba8ce355d`: 834 driver tests passed in both build and installed suites, and all 50 focused installed reference tests passed. Those review commits have not been pushed or run by shared CI. The draft removes the stale "not built or tested" and "checks are pending" statements and the production over-read claim for equal blocks. The precise cause/version of the old scratch-build experiment remains unverified.

Current validation supplement: the integrated standard build and installed suites each pass 925/925 CTests at `4135e6d96f2df606404ab51af3214a8b2f4a3427`. Code remains local and shared CI has not run this head.

## New integration replies

Target: [Newling’s AMAX-D request on #15259](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5879565945).

@newling I implemented AMAX-D in a separate final stack branch, after the bias-gradient correctness PR. It uses tile and worker maxima, merged after the parallel loop, and samples at the slow path’s epilogue stage. Ten independent gtests pass at worker limits 1, 3 and 8, and the integrated build/installed suites each pass 925 CTests at `4135e6d96f2df606404ab51af3214a8b2f4a3427`. #15259 stays focused on the slow-path lock optimization. The implementation is committed locally; the new PR and shared CI remain pending.

Target: [Tony’s large-K note on #15266](https://github.com/ROCm/rocm-libraries/pull/15266#discussion_r4568212035).

I integrated #15765 below this branch, removed the float-per-K precision restriction, and added K=1044 cases for all six Int8 type combinations and all four layouts. Integer-alpha ScaleAB cases keep smaller output-range dimensions so the driver’s checked Int32 epilogue remains valid. The matching driver/test comments and description draft now describe the completed integer reduction. The integrated build and installed suites each pass 925 CTests; code and the description refresh remain local.

Target: [Tony’s exact-sum note on #15274](https://github.com/ROCm/rocm-libraries/pull/15274#discussion_r4568212613).

With #15765 below this branch, the regression now checks both references against 16,790,292 and retains the independent old-float-summation witness. The comments specify wrapping for the fast reduction and corrected slow float-alpha reduction; slow integer-alpha arithmetic keeps its inherited behavior. The C/D alias correction is also retained through the restack. The integrated standard build and installed suites each pass 925 CTests. These commits remain local.
