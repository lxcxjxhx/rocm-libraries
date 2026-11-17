# Reference-GEMM stack: GitHub reviews and comments

Snapshot collected: 2026-11-16 15:12:53 UTC.

Current stack: [#16153](https://github.com/ROCm/rocm-libraries/stack/16153), listed bottom → top. Collection used historical stack [#15515](https://github.com/ROCm/rocm-libraries/stack/15515). At collection, the worktree checked out `users/alvasile/tl-speedup-09-reference-slow-path-amax-d` at `571ecae3fc4814ff951461d1d14e48f780975511`.

This archive contains **12 review submissions, 21 general comments, and 15 inline comments in 14 threads**, plus **14 per-PR review documents and one shared assessment** linked by newling. Review bodies, replies, suggestions, and bot comments are copied in full. Empty review bodies retain their submission metadata. Thread resolution and outdated status are recorded as returned by GitHub.

Each item links to its original source. Bodies reflect the versions available at collection time. Relative Markdown links in imported review documents are expanded to their original GitHub locations. The linked reviews retain their own reviewed commits, findings, and validation claims.

Newling’s two posts on #15259 contain reviews of all seven PRs: [initial round, November 12](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5867396457) and [final correctness round and newling’s own remarks, November 14](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5899752492). Both original posts appear under #15259; each linked per-PR document appears under its corresponding PR. The [shared testing and design assessment](#shared-assessment) is included in full at the end.

| PR | Title | Reviews | General comments | Inline comments | Linked per-PR reviews |
| --- | --- | ---: | ---: | ---: | ---: |
| [#15259](#pr-15259) | perf(tensilelite): take the slow-path reference lock only for AMAX-D | 4 | 3 | 2 | 2 |
| [#15266](#pr-15266) | test(tensilelite): cpu-gemm-driver Int8 inputs with separate C/D and alpha types | 3 | 3 | 4 | 2 |
| [#15274](#pr-15274) | perf(tensilelite): run Int8-input GEMMs on the CPU reference fast path | 1 | 3 | 3 | 2 |
| [#15275](#pr-15275) | perf(tensilelite): run Float8-output GEMMs on the CPU reference fast path | 1 | 3 | 2 | 2 |
| [#15276](#pr-15276) | perf(tensilelite): run complex GEMMs on the CPU reference fast path | 1 | 3 | 0 | 2 |
| [#15277](#pr-15277) | perf(tensilelite): write E and run gradient GEMMs on the CPU reference fast path | 1 | 3 | 2 | 2 |
| [#15278](#pr-15278) | perf(tensilelite): run MX GEMMs with a partial last MX block on the CPU reference fast path | 1 | 3 | 2 | 2 |

<a id="future-work"></a>

## Future work

### Implementation, integration and publication — completed November 17, 2026

All agreed source changes are committed and published, and the stack has been rebased onto fetched develop `bc60c82f1d732aa8bb50c869ac3ad2e1a586e599`. The validated code tip is `4135e6d96f2df606404ab51af3214a8b2f4a3427`. The main checkout and its develop branch remain at `1598298cf22ff879e3bb59b5b3f0bf3b9b3df265`. The Float8 issue, three draft PRs, stack links, eleven descriptions and fourteen approved replies are published and verified below. Shared CI and final production gfx950 qualification remain open.

- [x] Integrate #15257's OpenMP setter deletion below #15259, retaining the slow path's per-thread maximum and synchronized merge.
- [x] Insert #15765 locally between #15259 and #15266.
- [x] Apply #15266's branch-owned oracle/documentation corrections and large-K coverage: K=1044 for six Int8 combinations and four layouts. Ordinary cases use F32 sizes; integer-alpha ScaleAB cases keep smaller sizes within the driver's checked Int32 epilogue range.
- [x] Apply #15274's exact-sum test to both paths, asserting 16,790,292 and retaining the old float-summation witness. Preserve the C/D alias correction. Scope defined modulo-2^32 wrapping to the fast reduction and corrected slow float-alpha reduction; inherited slow integer-alpha arithmetic remains unchanged.
- [x] Insert the existing Float8 correctness branch between #15274 and #15275 and restack every descendant. Preserve #15275's Scalar-only narrowing fallback and Vector eligibility, including its helper rename.
- [x] Implement and commit the bias-gradient cleanup above #15278, using Newling's suggestion and regressions; PR #16150 closes #16120.
- [x] Implement and commit AMAX-D support at the end of the stack. Replace the old AMAX-D rejection test with positive coverage and retain diagnostic coverage using Scalar Half-to-Float8 narrowing.
- [x] Fetch develop and run `gh stack rebase --remote origin`; the final fetched and rebased trunk is `bc60c82f1d732aa8bb50c869ac3ad2e1a586e599`. Preserve the main develop checkout, verify ancestry, and rerun the affected build/tests.
- [x] Complete integrated build-tree/installed CPU checks and gfx942 nongrouped/grouped GPU execution, including the non-HPA gradient guard and MX-tail host tests.
- [x] Prepare and publish [complete issue, PR and reply text](tl-reference-gemm-publication-drafts.md) for the Float8 issue, three new PRs, eight existing descriptions, remaining review replies and new integration replies.

### Publication receipt — November 17, 2026

- [x] File [Float8 issue #16148](https://github.com/ROCm/rocm-libraries/issues/16148), link discovery PR #15275, and add the correctness-PR backlink.
- [x] Create draft PR [#16149](https://github.com/ROCm/rocm-libraries/pull/16149) for Float8 input scaling (`Fixes #16148`), draft PR [#16150](https://github.com/ROCm/rocm-libraries/pull/16150) for bias-gradient cleanup (`Fixes #16120`), and draft PR [#16151](https://github.com/ROCm/rocm-libraries/pull/16151) for AMAX-D.
- [x] Publish all twelve branches with `gh stack push --remote origin`; verify every remote head, base, and the agreed order.
- [x] Register [stack #16153](https://github.com/ROCm/rocm-libraries/stack/16153), including #15765 between #15259 and #15266 and Float8 correctness between #15274 and #15275. Bias cleanup follows #15278; AMAX-D is last.
- [x] Publish and read back eleven PR descriptions and fourteen approved replies. Preserve the existing PR readiness states; the three new PRs are drafts.

GitHub rejected inserting PRs below the top of existing stack #15515. Supported `gh stack unstack 15515` and `gh stack link --base develop --remote origin` commands rebuilt the registration as #16153. #15257 was verified unstacked before inclusion. The old stack link remains collection history. No review threads were resolved, approvals submitted, or PRs merged.

| PR | Published reply |
| --- | --- |
| [#15275](https://github.com/ROCm/rocm-libraries/pull/15275) | [Tony — Float8 cast nonfinite cases](https://github.com/ROCm/rocm-libraries/pull/15275#discussion_r4604838396) |
| [#15275](https://github.com/ROCm/rocm-libraries/pull/15275) | [Newling — Float8 driver selectors](https://github.com/ROCm/rocm-libraries/pull/15275#issuecomment-6958973018) |
| [#15276](https://github.com/ROCm/rocm-libraries/pull/15276) | [Newling — complex C/D aliasing](https://github.com/ROCm/rocm-libraries/pull/15276#issuecomment-6958973633) |
| [#15277](https://github.com/ROCm/rocm-libraries/pull/15277) | [Tony — non-HPA bias-gradient fallback](https://github.com/ROCm/rocm-libraries/pull/15277#discussion_r4604839593) |
| [#15277](https://github.com/ROCm/rocm-libraries/pull/15277) | [Newling — non-HPA bias-gradient fallback](https://github.com/ROCm/rocm-libraries/pull/15277#issuecomment-6958975655) |
| [#15278](https://github.com/ROCm/rocm-libraries/pull/15278) | [Newling — MX-tail NaN assertion](https://github.com/ROCm/rocm-libraries/pull/15278#issuecomment-6958976360) |
| [#15278](https://github.com/ROCm/rocm-libraries/pull/15278) | [Tony — equal-block slow tails](https://github.com/ROCm/rocm-libraries/pull/15278#discussion_r4604840794) |
| [#15278](https://github.com/ROCm/rocm-libraries/pull/15278) | [Newling — equal-block slow tails](https://github.com/ROCm/rocm-libraries/pull/15278#issuecomment-6958978253) |
| [#15278](https://github.com/ROCm/rocm-libraries/pull/15278) | [Newling — shared negative-test registration](https://github.com/ROCm/rocm-libraries/pull/15278#issuecomment-6958979108) |
| [#15278](https://github.com/ROCm/rocm-libraries/pull/15278) | [Newling — fast-path rejection diagnostics](https://github.com/ROCm/rocm-libraries/pull/15278#issuecomment-6958979825) |
| [#15278](https://github.com/ROCm/rocm-libraries/pull/15278) | [Tony — Test Result evidence](https://github.com/ROCm/rocm-libraries/pull/15278#issuecomment-6958980491) |
| [#15259](https://github.com/ROCm/rocm-libraries/pull/15259) | [Newling — separate AMAX-D PR](https://github.com/ROCm/rocm-libraries/pull/15259#issuecomment-6958981235) |
| [#15266](https://github.com/ROCm/rocm-libraries/pull/15266) | [Tony — large-K Int8 integration](https://github.com/ROCm/rocm-libraries/pull/15266#discussion_r4604842893) |
| [#15274](https://github.com/ROCm/rocm-libraries/pull/15274) | [Tony — both-path exact-sum regression](https://github.com/ROCm/rocm-libraries/pull/15274#discussion_r4604843542) |

The [companion publication record](tl-reference-gemm-publication-drafts.md) contains the exact verified issue, description, and reply bodies. Publication snapshots and read-backs are in `/tmp/tl-reference-publication-20261117/`. The last code commit is `4135e6d96f2df606404ab51af3214a8b2f4a3427`; later commits update only the two Markdown records.

Published branch order, bottom to top (code heads; later AMAX-D tip commits update documentation only):

| PR | Branch | Code head |
| --- | --- | --- |
| [#15257](https://github.com/ROCm/rocm-libraries/pull/15257) | `users/alvasile/tl-speedup-07-client-openmp-runtime-policy` | `3b0923a82c82586559c41db86b01a315a1ba45c2` |
| [#15259](https://github.com/ROCm/rocm-libraries/pull/15259) | `users/alvasile/tl-speedup-09-reference-slow-path-amax-d` | `7fadc6a06dd9e9d3fa82ddea6556a3beb3a1010f` |
| [#15765](https://github.com/ROCm/rocm-libraries/pull/15765) | `users/alvasile/fix-int8-slow-reference-accumulation` | `8d6f17d0fdcab834fa9eda9f25756b3a4eb2a817` |
| [#15266](https://github.com/ROCm/rocm-libraries/pull/15266) | `users/alvasile/tl-speedup-16-cpu-gemm-driver-int8` | `f5ff9cce4a7a882f0f35f53b8ba04a3a99d21ae8` |
| [#15274](https://github.com/ROCm/rocm-libraries/pull/15274) | `users/alvasile/tl-speedup-24-reference-fast-path-int8` | `203104dfb2f5bbf6693f93048463cfb1122cc4ea` |
| [#16149](https://github.com/ROCm/rocm-libraries/pull/16149) | `users/alvasile/tl-fix-float8-input-scaling` | `3534538e4c370dcba2be89a5a0dd10f585280632` |
| [#15275](https://github.com/ROCm/rocm-libraries/pull/15275) | `users/alvasile/tl-speedup-25-reference-fast-path-float8-output` | `0c69cd859220ab316535b4c7651a176a0d744511` |
| [#15276](https://github.com/ROCm/rocm-libraries/pull/15276) | `users/alvasile/tl-speedup-26-reference-fast-path-complex` | `386b9bb7f56d7ea682ac4389c76a1a1c6128966a` |
| [#15277](https://github.com/ROCm/rocm-libraries/pull/15277) | `users/alvasile/tl-speedup-27-reference-fast-path-use-e-gradient` | `c87b8c01e1e52334442ad726d9b3460c24acc003` |
| [#15278](https://github.com/ROCm/rocm-libraries/pull/15278) | `users/alvasile/tl-speedup-28-reference-fast-path-mx-k-tail` | `8eda434fc2a8e40743726e8abdecf365af961bca` |
| [#16150](https://github.com/ROCm/rocm-libraries/pull/16150) | `users/alvasile/tl-fix-reference-bias-gradient-cleanup` | `549fa73dd2dea12842739454119697b2d735c667` |
| [#16151](https://github.com/ROCm/rocm-libraries/pull/16151) | `users/alvasile/tl-speedup-reference-fast-path-amax-d` | `4135e6d96f2df606404ab51af3214a8b2f4a3427` |

#15257 is the existing prerequisite and now the first member of stack #16153, targeting develop. Local and GitHub stack membership and bases match the chain above, and every descendant contains its predecessor.

### Shared CI snapshot at publication verification — November 17, 2026, 19:57 UTC

The new draft PRs have queued checks, and the existing PRs show a mixture of passed, pending, cancelled and failed contexts. Two examined failures report cancellations: [#15259's policy check](https://github.com/ROCm/rocm-libraries/actions/runs/44161453975/job/134316117938) reports `pre-commit=cancelled`; [#15765's clang-tidy summary](https://github.com/ROCm/rocm-libraries/actions/runs/44160728514/job/134313514245) reports cancelled setup and clang-tidy jobs. The remaining failed summaries have not been diagnosed. Required shared-CI execution and successful final-head results remain open. The complete check rollups and these two logs are saved in `/tmp/tl-reference-publication-20261117/`.

### Remaining execution checklist

1. Verify that required shared CI executes every new regression on the published heads, including build-tree and installed standard driver suites. Confirm affected-architecture results.
2. Complete final production gfx950 qualification. Repeat the Float8 matrix against the final runtime patch, with actual nongrouped and grouped execution. Retain dedicated gradient/non-HPA and MX-tail qualification against the final published stack. The user's earlier gfx950 result confirmed the proposal at the pinned remote head, not this final integrated implementation.
3. Verify each addressed review thread against the published source and test/CI evidence, resolve proven fixes, and obtain reviewer sign-off. Threads remain open after publication.

The AMAX-D race-detector follow-up remains removed at the user's request. Earlier posted replies and ticket comments are historical records.

### 1. AMAX-D fast path — published as final draft PR #16151

- [x] Implement locally on `users/alvasile/tl-speedup-reference-fast-path-amax-d`, after the bias-gradient correctness branch.
- [x] Add ten independent D/AMAX-D gtests and positive fast-driver cases. Replace `CPUGemm.fast_AmaxD_Rejected` with an unsupported Scalar narrowing diagnostic case.
- [x] Create and publish final draft PR [#16151](https://github.com/ROCm/rocm-libraries/pull/16151).
- [ ] Verify its shared CI.

**Resolution for #15259:** [Newling's AMAX-D request](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5879565945) is implemented in this separate final branch. #15259 retains its slow-path lock/per-thread-maximum scope.

Each tile computes a local absolute maximum, each worker accumulates its own maximum, and the main thread merges after the parallel loop. Sampling follows activation/gradient multiplication and precedes gate residual and the final output cast. Complex AMAX-D and ScaleCD combinations retain fallback. Coverage includes real, Int8 and both Float8 encodings; transposes, batching, activation, gradients, output narrowing, zero and NaN comparison behavior. All ten gtests pass at each OpenMP limit 1, 3 and 8. No performance result is claimed for the new feature.

### 2. PR #15765 insertion and owned downstream edits — published

- [x] Place [#15765](https://github.com/ROCm/rocm-libraries/pull/15765) immediately after #15259 and before #15266 in the local stack.
- [x] Commit the driver/oracle wording and large-K test edits on #15266, preserving the Int32 output-range constraints. [Source feedback](https://github.com/ROCm/rocm-libraries/pull/15266#discussion_r4568212035).
- [x] Commit the both-path exact-sum regression and integer-reduction comments on #15274. The shared helper is renamed by #15275, which carries the regression through that rename. [Source feedback](https://github.com/ROCm/rocm-libraries/pull/15274#discussion_r4568212613).
- [x] Draft #15765's refreshed description with its own fix/tests and the separate downstream #15266/#15274 responsibilities.
- [x] Publish and verify GitHub stack membership/base changes and the final descriptions.

### 3. Develop fetch/rebase — completed and verified before publication

Immediately before publication, a fresh fetch verified develop unchanged at `bc60c82f1d732aa8bb50c869ac3ad2e1a586e599`. The first completed full rebase used `951ff1461d4ba7e425fe1f50de53c4e94b26e323`. Develop then advanced by the TensileLite C++20/helper-cache commit. A fresh fetch and second full rebase used `bc60c82f1d732aa8bb50c869ac3ad2e1a586e599`; the build, installed suite and numerical probes were repeated against that code.

```sh
git fetch origin develop
gh stack rebase --remote origin
```

`--remote origin` disambiguates this checkout's remotes. Because local develop is checked out in the main worktree, `gh stack` rebased onto `origin/develop` and preserved local develop. Fetch negotiation also used `fetch.hideRefs=worktrees/` and `fetch.hideRefs=refs/backup/`. Both full-rebase logs and the fetched/validated SHAs are retained in [the evidence directory](/tmp/tl-reference-implementation-20261117/).

### 4. gfx950 Float8 proposal validation — completed; final patch qualification pending

- [x] User confirmed all three findings at remote head `b6828c46316720641f2e3f15a1320201d4f1c34b`: HIP device 0, MI350X (`gfx950:sramecc+:xnack-`), ROCm 7.2.1, OCP F8/B8.
- [x] Baseline: 1,728 executed, 1,696 passed, 32 numerical failures, zero unexecuted. Conversion-call proposal: 1,728 executed/passed, zero failures or unexecuted cases; the user's other 1,696 output bytes were unchanged.
- [ ] Repeat against the final production patch and grouped invocation path on gfx950.

The user-provided result establishes the operation order and proposal on gfx950. It does not replace the final-runtime qualification listed above.

### 5. Float8 input-scaling correctness — issue #16148 and draft PR #16149 published

- [x] Implement the slow Vector correction and GPU MultipleBuffer conversion binding correction on `users/alvasile/tl-fix-float8-input-scaling`.
- [x] Demonstrate the six regressions failing before their respective fixes and passing after; include grouped argument construction in the existing shared standard suite.
- [x] Insert the branch locally between #15274 and #15275. Preserve Scalar-only fallback on #15275 and leave Vector eligible.
- [x] Repeat final integrated gfx942 GPU execution: 1,728 nongrouped and 384 grouped cases pass, with no unexecuted or numerical failures. The 32 baseline failures are corrected and the other 1,696 numerical arrays match baseline.
- [x] File [#16148](https://github.com/ROCm/rocm-libraries/issues/16148) and create [#16149](https://github.com/ROCm/rocm-libraries/pull/16149) with `Fixes #16148` and discovery link #15275; link the PR back from the issue.
- [x] Publish and verify the stack relationship.
- [ ] Verify shared CI and final production gfx950 results.

Both defects predate [#15275](https://github.com/ROCm/rocm-libraries/pull/15275) and are present below it. The slow reference now applies both Vector scales after narrowing and reduction. The conversion invocation substitutes a unit scalar only for the already-consumed narrowed operand; the main kernel's binding and the other operand's scale stay intact. The original local red/green results are preserved in [the earlier evidence](/tmp/tl-float8-scaling-fix-20261116/README.md). Current integrated evidence is below.

### 6. Bias-gradient cleanup — draft PR #16150 published

- [x] File [#16120](https://github.com/ROCm/rocm-libraries/issues/16120), link discovery [#15277](https://github.com/ROCm/rocm-libraries/pull/15277), and assign `Alex-Vasile`; completed earlier and verified by read-back.
- [x] Implement [Newling's suggestion](https://github.com/newling/rocm-libraries/commit/66106f99d916da886beecc39e464681596074b6d) on `users/alvasile/tl-fix-reference-bias-gradient-cleanup`, sharing reduction, giving workspace automatic ownership, throwing actual reduction errors and guarding complex reduction.
- [x] Add `BatchedBiasGradientReportsUnsupportedShape` for A/B/D and both path preferences; check the dimension/batch diagnostic and unchanged bias. The original unpatched run failed at A because no exception was thrown; it did not separately reproduce the D double-free. All 55 focused tests passed after the fix, and the final standard suites pass.
- [x] Create [#16150](https://github.com/ROCm/rocm-libraries/pull/16150) after #15278 with `Fixes #16120`; AMAX-D PR #16151 follows it.
- [ ] Verify shared-CI execution against the published code.

The batching limitation is preserved. [Tony](https://github.com/ROCm/rocm-libraries/pull/15277#discussion_r4602603298) and [Newling](https://github.com/ROCm/rocm-libraries/pull/15277#issuecomment-6954240221) already received replies linking the existing ticket and separate-PR plan. Their replies require no duplicate publication. The non-HPA accumulation guard remains owned by #15277 and survives this integration.

### Integrated validation evidence — November 17, 2026

| Validation on code tip `4135e6d96f2df606404ab51af3214a8b2f4a3427` | Result |
| --- | --- |
| Release tests, driver and client build | Passed |
| Standard build-tree CTest | 925/925 passed: 924 drivers and the standard gtest suite |
| Standard installed CTest, `LD_LIBRARY_PATH` unset | 925/925 passed, with local ROCm runtime dependencies |
| Independent AMAX-D gtests, limits 1/3/8 | 10/10 passed at each limit |
| Independent CPU Float8 matrix, both scale profiles | 2,304 passed; zero slow or fast/fallback mismatches |
| gfx942 nongrouped Float8 matrix | 1,728 executed/passed; no failures, unexecuted or nonfinite outputs |
| gfx942 grouped Float8 matrix | 384 executed/passed; no failures, unexecuted or nonfinite outputs |
| Previously passing nongrouped control arrays | All 1,696 numerically equal to baseline; all 32 baseline failures fixed |
| Scoped root/component hooks and whitespace | Passed; root C++/CMake hooks exclude these paths |
| Manual formatting of this turn's new C++ ranges | Passed with clang-format 18.1.4 after branch-owned corrections |

The gtest suite's internal exclusions retain their existing behavior; the 925 totals count CTest registrations. The tests include the non-HPA/HPA guard, complex aliasing and finite-data comparisons, exact MX tails with nonzero padding/NaN rejection, negative-test diagnostics, Float8 scaling/casts and Int8 exact sums/wrapping. Passing shared CI for the newly published heads has not yet been verified; shared-CI execution remains a required follow-up.

The saved nongrouped kernel libraries used the previous `FusedGemmA2A` serialization. An artifact-only migration verified every legacy fusion value was false and translated both the problem field and predicate to current `FusedA2A: Disabled`, retaining the disabled-fusion constraint and the original kernel objects. No production serializer or kernel was changed for this migration. The final production probe was invoked without its optional prototype replacement argument.

Grouped execution uses newly generated libraries. The harness follows client setup by reserving required host workspace and giving each problem its own GSU synchronizer slot. Coverage includes both Half directions, Scalar/Vector, four layouts, GSU 1/2/4, MB/MBSK, two epilogues and two distinct grouped problems.

Raw logs, XML, probe/build recipes, manifests, JSONL output and [machine-readable final results](/tmp/tl-reference-implementation-20261117/latest-validation-summary.json) are in `/tmp/tl-reference-implementation-20261117/`. The `latest-*` files record the final develop `bc60c82f1d732aa8bb50c869ac3ad2e1a586e599` run; earlier files preserve the red/green and first integrated runs. Final gfx950 and shared-CI checks remain explicitly open above. No current performance or race-detector result is claimed.

The per-PR walkthroughs below retain their dated review/discussion history. The implementation and publication receipt above supersede older statements that integration, these source edits, or the approved descriptions/replies were pending. Archived review text and older draft wording are preserved unchanged.

<a id="pr-15259"></a>

## PR #15259 — perf(tensilelite): take the slow-path reference lock only for AMAX-D

[GitHub PR](https://github.com/ROCm/rocm-libraries/pull/15259) · State: **OPEN**

Branch: `users/alvasile/tl-speedup-09-reference-slow-path-amax-d`. Base: `develop`. Head at collection: `571ecae3fc4814ff951461d1d14e48f780975511`.

<a id="pr-15259-feedback-assessment"></a>

### Feedback assessment and recommended actions — November 16, 2026

Assessed current PR **#15259** at `571ecae3fc4814ff951461d1d14e48f780975511`. Its head and all nine native review/comment entries still match the archive. Each native entry and both linked per-PR reviews now have an evaluation immediately after the quoted text.

**Recommendation:** retain the per-thread AMAX-D implementation and its tests. The description update with exact-head evidence is complete. Link the already implemented OpenMP cleanup in [#15257](https://github.com/ROCm/rocm-libraries/pull/15257); keep race detection as a follow-up. Resolve the fast-path AMAX-D request through the [new PR planned at the end of this stack](#future-work). The [required installed CPU reference suite](#pr-15259-host-ci-spec) is implemented and validated locally in #15259; publication and a passing PR CI run remain required before merging. I found no blocking correctness issue in this PR’s feedback. This assessment concerns these entries and their implications for #15259.

| Feedback | Disposition | Recommended action |
| --- | --- | --- |
| Stale “checks pending” sentence and unclear 942-test run | Completed: PR description updated November 16 | Replaced with linked current-head results and removed the unlinked 942-test claim. [Reply to Tony](#pr-15259-draft-reply-tony-description) is posted and verified. |
| Per-element OpenMP setter | Agreed: addressed by #15257, pending integration | Keep the cleanup on #15257 and verify the combined code preserves its deletion and #15259’s critical merge. [Reply to Tony](#pr-15259-draft-reply-tony-openmp) is posted and verified. |
| OMP_NUM_THREADS used as proof of team size | Completed: PR description updated November 16 | Test Plan now uses OMP_THREAD_LIMIT=1/3/8; the historical OMP_NUM_THREADS=25 setting is qualified because the actual team size is unverified. Broader policy remains on #15257. [Reply to newling](#pr-15259-draft-reply-newling-thread-counts) is posted and verified. |
| No reliable race test for the critical merge | Removed from remaining work at user request, November 17 | Critical retained. The earlier [reply to Tony](https://github.com/ROCm/rocm-libraries/pull/15259#discussion_r4588246818) and [AIHPBLAS-5035 comment](https://amd-hub.atlassian.net/browse/AIHPBLAS-5035?focusedCommentId=706371) remain historical records; this stack no longer schedules a race-detector follow-up. |
| Add AMAX-D to the fast path | Resolution recorded: new PR in this stack | Implement and test the feature in a new PR added to the end of the stack; see [Future work](#future-work). Keep #15259 focused on the slow-path optimization. |
| Policy-bot test filename warning | Ignored per user instruction | No follow-up action. |
| Failed project coverage targets | Ignored per user instruction | No follow-up action for #15259. |
| AMAX-D driver tests are absent from the required installed suite | Implemented and validated locally; shared PR CI pending publication | Both trees pass all 325 driver cases, including 42 AMAX-D configurations. The relocated installation also passes 24 focused gtests and propagates an injected numerical failure. See [implementation and validation](#pr-15259-host-ci-spec). |

#### Evidence checked

- Source and incremental diff: the only shared maximum update is the [critical merge](https://github.com/ROCm/rocm-libraries/blob/571ecae3fc4814ff951461d1d14e48f780975511/projects/hipblaslt/tensilelite/client/src/Reference.cpp#L2298); [each thread’s maximum](https://github.com/ROCm/rocm-libraries/blob/571ecae3fc4814ff951461d1d14e48f780975511/projects/hipblaslt/tensilelite/client/src/Reference.cpp#L1947) is private and the final output follows the parallel-region join.
- [Math CI coverage log, build 7](https://math-ci.amd.com/job/rocm-libraries/job/tensilelite-unit-codecov/job/hipblaslt/job/PR-15259/7/consoleText): checks out this exact head, passes all 14 AMAX-D cases, and reports **1,185 passed, four skipped, one disabled**. The lane also retains pre-existing exclusions from `tox.ini`.
- A standalone OpenMP runtime probe confirmed that `OMP_NUM_THREADS=4` becomes a 64-thread team after the outer setter; `OMP_THREAD_LIMIT=1/3/8` constrains it to 1/3/8. Calling the setter inside the active region leaves that region’s team size unchanged.
- Current relevant Math CI checks, Component CI Summary and `codecov/patch` pass. The rollup still contains failed `FFM gfx1250 / tensilelite`, `codecov/project/TensileLite-CPP` and `codecov/project/hipBLASLt` checks. The gfx1250 config cited by Tony is marked `ffm_fail` in the source; I did not independently re-diagnose that failed job.
- The initial feedback assessment used the downloaded current-head CI log. The CI integration subsequently built both test executables and reran the numerical suites locally; results are recorded below. Sanitizer runs and performance timings remain unverified here.

#### Shared assessment: recommendations that apply to #15259

The [shared testing and design review](#shared-assessment) correctly treats the 14 AMAX-D cases as distinct output checks. Keep them: the independent scalar result and the -1 sentinel test the write and the absolute maximum. Their passing against the old implementation is expected for an optimization that preserves numerical output.

Its thread-limit and test-selection advice applies here. At the archived head, the installed CTest file omitted the later `CPUGemm` registrations; the local implementation now exports them into the same file. The reference gtests use categories when the shared test helper is available. Use the binary directly for focused reference checks. The downloaded Math CI log confirms the driver cases already have shared-CI coverage execution. The user requested the required installed suite in this branch; the [implementation and validation](#pr-15259-host-ci-spec) below replace the earlier separate-PR recommendation.

The shared document’s 788-driver-test total describes the full stack, not this PR. Its aliasing, scaled narrowing, complex, gradient and MX-tail findings belong to later layers. Keep those fixes and any numerical-policy decisions on the PRs that own them.

#### Review questions

1. **Changed functionality:** ordinary slow-path elements stop taking an unnecessary global lock; AMAX-D elements collect a private maximum and merge once per thread. The test driver gains AMAX-D validation.
2. **Appropriate test level:** CPU numerical checks plus controlled OpenMP teams are appropriate; the 14 cases are present and passed in shared CI. GPU qualification is unnecessary for this unchanged numerical path.
3. **Tests and flags:** tests are present. A new flag is unnecessary for this output-preserving lock optimization. The missing race-sanitizer lane remains a documented testing gap.
4. **Adjacent coverage:** retain the combined epilogues, batched maximum, alpha-zero sentinel and negative 1×1 case; exercise controlled teams of 1, 3 and 8 when validating a local change. Broader fast-path AMAX-D coverage belongs to the [new PR planned at the end of this stack](#future-work).

<a id="pr-15259-host-ci-spec"></a>

#### Required CPU reference CI in #15259 — implementation and validation

**Agreed implementation:** reuse the required TheRock standard CTest lane. The earlier dedicated Component CI workflow and custom report runner were superseded after comparing the existing reference tests. The existing SDK setup, artifact selection and CTest failure handling cover this integration.

##### Existing coverage and the actual gap

The AMAX-D cases use the same `add_cpu_gemm_test` helper as the existing fast/slow transpose, bias, activation, scaling, FP8 and MX driver cases. Math CI's unfiltered build-tree `coverage-cpp` command already runs these cases: [#15259 build 7](https://math-ci.amd.com/job/rocm-libraries/job/tensilelite-unit-codecov/job/hipblaslt/job/PR-15259/7/consoleText) contains all 14 passing AMAX-D results at the current head.

`ReferenceFastPath` and `ReferenceMX*` instead live in the `tensilelite-tests` gtest binary. The [current #15259 gfx942 job](https://github.com/ROCm/rocm-libraries/actions/runs/42914662655/job/130434041172) runs them through `tensilelite-tests_standard_suite` using the installed CTest file. Its result feeds the required `Multi-Arch CI Summary`.

The driver registrations are added after that installed CTest file is generated, and the driver executable has no corresponding test installation. They therefore share a packaging/registration gap with the other driver tests; AMAX-D requires no different numerical test framework.

**Execution overlap:** installing these registrations adds execution in TheRock while Math CI continues running them from its coverage-instrumented build tree. The reference gtests already have this overlap. The added execution validates the installed artifact and feeds the required gate; it is not a migration that removes the coverage run. Within a single CTest invocation, assigning several category labels to one driver entry does not execute it several times. Export each registration once, and keep additional thread configurations limited to AMAX-D.

##### Smaller implementation

1. Install `cpu-gemm-driver` beside `tensilelite-tests` under `bin/hipblaslt-tensilelite/`, in the tests component. Set its `THEROCK_INSTALL_RPATH_ORIGIN` consistently with the existing gtest target so the relocated executable finds its libraries.
2. Export driver registrations into the same installed `CTestTestfile.cmake`, with executable paths relative to that directory. Extend the existing registration helper and preserve arguments and expected-failure properties for the driver rejection tests. Apply the standard/comprehensive/full labels through the existing CTest machinery.
3. Express the AMAX-D limits 1, 3 and 8 as native CTest configurations. Use the existing AMAX-D entries for limit 8 and add explicitly named variants only for limits 1 and 3. Set `OMP_THREAD_LIMIT`, `OMP_NUM_THREADS` and `OMP_DYNAMIC=FALSE` through test properties in both build-tree and installed registrations. This gives three configurations per AMAX-D case; each other driver case keeps one registration per tree. Numerical failures use the existing CTest exit status.
4. Update the testing documentation and inventory to identify the driver coverage in the required installed lane.

The [pinned TheRock build](https://github.com/ROCm/TheRock/blob/647a8c52a206f2e7d0e9c63cbf0af0ded084f08b/math-libs/BLAS/CMakeLists.txt) already enables the TensileLite client and C++ tests on Linux and supplies their dependencies. Its [artifact manifest](https://github.com/ROCm/TheRock/blob/647a8c52a206f2e7d0e9c63cbf0af0ded084f08b/math-libs/BLAS/artifact-blas.toml) already includes `bin/hipblaslt-tensilelite/**`. No new workflow, dispatcher output, dependency bootstrap or report runner is needed for this route.

Primary implementation files: `projects/hipblaslt/tensilelite/tests/CMakeLists.txt` and its categories configuration, plus the testing documents and inventory. Verify the installed driver runs after relocation, installed discovery selects every AMAX-D configuration, rejection tests preserve their expected-failure behavior, and an injected numerical failure makes CTest fail. Then require a real #15259 PR run showing these installed cases execute and `Multi-Arch CI Summary` passes.

At the pinned TheRock version, ordinary component PRs select `standard`; the current branch's job confirms that selection. The [test matrix](https://github.com/ROCm/TheRock/blob/647a8c52a206f2e7d0e9c63cbf0af0ded084f08b/build_tools/github_actions/fetch_test_configurations.py) omits the C++ stage in `quick`. This plan uses the existing required standard lane; quick-tier promotion would also require changing TheRock's selector.

##### Implementation and local validation — November 16, 2026

- Install both executables and export all driver registrations into the existing relocatable test file. The shared registration helper lives in `tests/cmake/CPUGemmTests.cmake`; `tests/cpu_gemm_test_categories.yaml` uses native CTest regular expressions for standard/comprehensive/full labels.
- Preserve every original driver name and argument, including all nine expected-failure registrations. The 297 original cases remain; only the 28 additional AMAX-D configurations increase discovery to 325 unique driver entries. Each ordinary case uses limit 8; each of the 14 AMAX-D cases uses limits 8, 1 and 3. Both trees have matching arguments, labels, environment, processor accounting, timeout and expected-failure properties.
- A Release build using the local ROCm 7.2.4 SDK and Clang 22 produced `cpu-gemm-driver` and `tensilelite-tests`, with OpenMP and mxDataGenerator enabled and YAML disabled. All **325 build-tree driver cases passed**, with **42 AMAX-D configurations**, no failures and no skips.
- Installed the test and host-library components into a private prefix, supplied the SDK's OpenMP runtime there, then renamed the prefix. Without build-tree `LD_LIBRARY_PATH`, all **325 relocated installed driver cases passed**, with the same 42 AMAX-D configurations and no skips. All **24 focused reference gtests passed** from that installation. `ldd` confirmed the host and OpenMP libraries resolve from the relocated prefix; HIP and amd_smi still resolve from the SDK.
- Temporarily added `--injectValidationFailure` to the installed passing `CPUGemm.fast_F32_Sanity` registration. The driver reported a D mismatch and CTest exited **8** with one failed test. Restored the generated file byte for byte and reran the case successfully. All nine registered rejection tests retained `WILL_FAIL` and passed in both full runs.
- Scoped root and hipBLASLt pre-commit hooks passed. The component hook selected no Python unit/characterization tests for these CMake and documentation changes. YAML parsing, inventory ID uniqueness and `git diff --check` passed.

Evidence is preserved locally in `/tmp/tl-amaxd-installed-ci-20261116/`: the build log, discovery JSON, relocated dependency output, three numerical logs/JUnit reports, and injected/restored validation logs. These host checks validate packaging and numerical failure handling; they do not establish GPU performance or race-detector coverage.

**Shared CI status:** the implementation is local and has not been pushed. The old-head PR checks do not validate it. Publish this commit, then require the #15259 standard TensileLite job to report the installed driver cases and a passing `Multi-Arch CI Summary` before merging. Math CI's coverage run remains enabled. The race-detector follow-up was subsequently removed from this remaining-work plan at the user's request on November 17.

<a id="pr-15259-reply-newling-installed-ci"></a>

##### Posted reply to newling — Installed CPU reference CI

In response to the installed-test gap in [newling's shared assessment](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_cpu_reference_stack_15259_15278.md#ci-and-validation-limits). [Posted reply](https://github.com/ROCm/rocm-libraries/pull/15259#issuecomment-6924405520), November 16, 2026; exact body and target PR verified by reading it back.

> @newling I've implemented the installed-test coverage in this branch. `cpu-gemm-driver` now installs beside `tensilelite-tests`, with its registrations in the existing standard CTest suite used by TheRock's required `Multi-Arch CI Summary`. Math CI keeps its coverage run.
>
> Each ordinary driver case has one registration. The 14 AMAX-D cases run at `OMP_THREAD_LIMIT=1`, `3` and `8`; the original names use limit 8. Multiple tier labels do not repeat a case within one CTest invocation.
>
> Locally, all 325 driver cases passed in both the build tree and a relocated installation, including all 42 AMAX-D configurations. The 24 focused reference gtests passed from the installation too. An injected numerical mismatch made installed CTest fail, and the restored case passed.
>
> The commit is still local, so the updated shared PR CI run remains pending publication. ThreadSanitizer/Archer validation remains a follow-up under AIHPBLAS-5035.

**Feedback queue:** a fresh GitHub review/thread lookup after implementation found no additional undecided human feedback for #15259. The two unresolved Tony threads are the already agreed race-detector follow-up and the setter deletion owned by #15257. The next branch step is publication and verification of the required installed suite; the stack insertion and fast-path feature remain recorded in Future work.

#### Recommended PR-description rewrite

The agreed Test Result paragraph has been published on #15259, replacing the stale automation sentence and unlinked 942-test claim. The agreed thread-count correction is also published: Test Plan now uses limits of 1, 3 and 8, selects AMAX-D CTests explicitly and invokes focused reference gtests directly; Test Result qualifies the historical OMP_NUM_THREADS=25 setting, and Related links #15257. The broader description draft below remains advisory; its remaining wording has not been applied. Exact historical performance and mutation numbers need links to their source runs; they have not been independently verified here. The repository-required headings are retained.

```markdown
JIRA ID : AIHPBLAS-5257

## Motivation

tensilelite-client validates generated GPU results against a CPU reference matrix multiplication. Unsupported fast-path problems use a scalar OpenMP loop. That loop took a global lock for every output element, including problems without AMAX-D, the maximum absolute output value.

## Technical Details

- The slow reference computes a private AMAX-D maximum for each thread and merges it once under the existing critical section. Problems without AMAX-D take no lock. The maximum is computed at the same stage as before, preserving numerical behavior and support for Half and BFloat16 accumulators.
- cpu-gemm-driver gains `--outputAmaxD`. Validation compares the result with an independently computed scalar GEMM and starts the output at -1 to detect a missing write.
- Fourteen Linux CTest cases cover data types, combined epilogues, batching, zero alpha and a negative 1×1 result. The fast path continues to reject AMAX-D. No GPU code changes.

## Test Plan

From `projects/hipblaslt/tensilelite`, configure with `cmake --preset tensilelite -S .. -B <build> -DTENSILELITE_BUILD_TESTING=ON` and build with `cmake --build <build> --parallel`.

Run the AMAX-D cases with `OMP_THREAD_LIMIT` set to 1, 3 and 8; this branch's explicit 64-thread request overrides `OMP_NUM_THREADS`:

    OMP_THREAD_LIMIT=8 OMP_NUM_THREADS=4 KMP_BLOCKTIME=0 ctest --test-dir <build>/tensilelite/tests -R '^CPUGemm\..*AmaxD' --output-on-failure

Run focused reference tests directly, including configurations that register gtests by category:

    OMP_THREAD_LIMIT=8 OMP_NUM_THREADS=4 KMP_BLOCKTIME=0 <build>/tensilelite/tests/tensilelite-tests --gtest_filter='ReferenceFastPath.*:ReferenceMX*.*:*ReferenceMX*/*'

Run repository hooks on the three changed files.

## Test Result

[Math CI build 7](https://math-ci.amd.com/job/rocm-libraries/job/tensilelite-unit-codecov/job/hipblaslt/job/PR-15259/7/consoleText) checks out commit `571ecae3fc4814ff951461d1d14e48f780975511` and passes all 14 AMAX-D cases. Its full CTest run has 1,185 passed tests, four skipped tests and one disabled test, with pre-existing exclusions in the coverage configuration.

Component CI Summary, the relevant Math CI checks and `codecov/patch` pass. The check rollup retains failed gfx1250 emulation and two project-coverage checks. Numerical validation is confirmed here; historical performance and mutation results need their own linked artifacts, and no race-sanitizer result is supplied.

## Related

- OpenMP idle behavior and thread policy: [#15257](https://github.com/ROCm/rocm-libraries/pull/15257).
- Race-detection gap: AIHPBLAS-5035 in [hipBLASLt's testing guidance](https://github.com/ROCm/rocm-libraries/blob/571ecae3fc4814ff951461d1d14e48f780975511/projects/hipblaslt/TESTING.md#L966).

## Device / Architecture Coverage

CPU reference and test-driver changes. The f4 MX32 case and the Float8 cases follow their existing non-Windows test registration. GPU kernels and dispatch are unchanged.

## Submission Checklist

- [x] Look over the contributing guidelines at https://github.com/ROCm/TheRock/blob/main/GOVERNANCE.md#pull-requests.

## Risk level

Low (2/5): the numerical operation is unchanged, the shared maximum remains protected, and independent AMAX-D validation passed in shared CI.
```

### Reviews (4)

#### newling — COMMENTED — review 5867396457

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5867396457) · [@newling](https://github.com/newling) · Submitted: 2026-11-12 01:01:00 UTC

Reviewed commit: `571ecae3fc4814ff951461d1d14e48f780975511`.

> Hi Alex, I do intend to look at these myself tomorrow sometime. For now, though, here is some feedback from agent reviews. I haven't looked at them yet, so feel free to wait for me to filter out the ones that look not useful.
>
> | PR | Link to review | Number of findings | Main finding |
> | --- | --- | ---: | --- |
> | #15259 | [Review](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15259.md) | 0 | No correctness defect found. |
> | #15266 | [Review](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15266.md) | 1 | Valid Int32 scalar values are rejected. |
> | #15274 | [Review](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15274.md) | 1 | Aliased C/D gives incorrect Int8 results. |
> | #15275 | [Review](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15275.md) | 1 | Scaled Half-to-Float8 inputs change reference results. |
> | #15276 | [Review](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15276.md) | 1 | Aliased C/D gives incorrect complex results. |
> | #15277 | [Review](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15277.md) | 2 | Batched bias-gradient fallback can double-free. |
> | #15278 | [Review](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15278.md) | 2 | The MX-tail test passes with all-NaN output. |
>
> The counts cover actionable findings; the reviews also include optional suggestions. Each review links to a suggestion branch with proposed fixes and tests.
>
> [Cross-PR assessment of testing and design](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_cpu_reference_stack_15259_15278.md).
>

**Evaluation and recommendation — November 16, 2026:** Accept the #15259 conclusion: the per-thread maximum preserves the original computation, and the final shared update remains synchronized. The [merge](https://github.com/ROCm/rocm-libraries/blob/571ecae3fc4814ff951461d1d14e48f780975511/projects/hipblaslt/tensilelite/client/src/Reference.cpp#L2294) occurs after the work-sharing loop and the output is written after the parallel region ends. Keep the 14 AMAX-D tests. The other six rows in this post belong to their respective PRs; they do not justify changing #15259. The linked initial review has one optional cleanup, assessed below.

#### newling — APPROVED — review 5879565945

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5879565945) · [@newling](https://github.com/newling) · Submitted: 2026-11-12 18:05:18 UTC

Reviewed commit: `571ecae3fc4814ff951461d1d14e48f780975511`.

> LGTM. Follow-up thought: Should we also add amax-D to the fast path?

**Evaluation and recommendation — November 16, 2026:** Resolution recorded in [Future work](#future-work): implement fast-path AMAX-D in a new PR added to the end of this stack. Keep #15259 focused on the slow-path lock optimization. The current [eligibility predicate](https://github.com/ROCm/rocm-libraries/blob/571ecae3fc4814ff951461d1d14e48f780975511/projects/hipblaslt/tensilelite/client/src/Reference.cpp#L1230) explicitly rejects AMAX-D. The new PR needs independent D/AMAX-D checks across batching, data types and epilogues, and must reduce at the same stage as the slow path: before D scaling, gate residual and the final D cast. The feature remains planned; this entry records where the feedback will be handled.

#### tony-davis — APPROVED — review 5897487182

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5897487182) · [@tony-davis](https://github.com/tony-davis) · Submitted: 2026-11-13 20:21:10 UTC

Reviewed commit: `571ecae3fc4814ff951461d1d14e48f780975511`.

> Hi Alex! Thanks for this one, and for the timing table. That's the perf evidence I'd want for a change like this.
>
> I spent most of my time on the concurrency side, since that's where my worry was. On develop, the per-element `omp critical` only ever wrapped the AMAX-D compare-and-update. The `dPtr`, `ws` and E writes were already outside it, and each iteration writes its own index. So `amaxD` was the only state the lock protected, and it's still behind the critical in the new per-thread merge. The merge runs after the `omp for` barrier and before the region closes, so `SetValue` sees the final value. NaN handling is the same too (`NaN > x` is false in both versions, so a NaN element is skipped either way), and a max doesn't care about merge order. I think it's correct.
>
> I also like that the new cases came with mutation evidence (merging only thread 0 failed 9 of 14, dropping the abs failed 5 of 14). Codecov shows every changed line in `Reference.cpp` covered, so the new CTest cases ran in CI on this head.
>
> Two small things, both non-blocking:
>
> 1. Test Result still ends with "Not yet run on this branch. Automated GitHub checks are pending." The checks have finished, and I wasn't sure what "not yet run" referred to. Could you replace it with what actually ran on this head (and say whether the 942-of-942 CTest run was on this branch or the unsplit one)?
> 2. If the merge's critical ever went away, I don't think any test would catch it reliably. That's the known no-TSAN-lane gap (AIHPBLAS-5035), not something this PR created. There's an inline note.
>
> CI looks fine to me. The required checks all pass. `FFM gfx1250 / tensilelite` has one failure, `gsuaad_gfx1250`, but that config is already marked `ffm_fail` and has no AMAX-D output; the emulator timeouts show up on other PRs too. The two codecov project checks are repo-wide targets, and the patch check passes.
>
> Approving.
>
> <!-- rocm-pr-quality:attestation v1 -->
> > **Reviewed with the ROCm PR-quality skill.**
> > - Skill: `rocm-pr-quality` (base) - overlays: `hipblaslt-pr-quality`
> > - Action: `review` - Result: **APPROVED** (`0` BLOCKING; `0` IMPORTANT)
> > - Risk: `2`
> > - Reviewed commit: `571ecae3fc4814ff951461d1d14e48f780975511`
> > - Attestation: `2add4029`
> >
> > <sub>Machine-checkable attestation. The digest ties this review to this PR; later commits are
> > allowed. The reviewed commit is informational (what the review looked at).</sub>

**Evaluation and recommendation — November 16, 2026:** Accept the concurrency analysis. The description request is addressed: its Test Result now links the [exact-head CI log](https://math-ci.amd.com/job/rocm-libraries/job/tensilelite-unit-codecov/job/hipblaslt/job/PR-15259/7/consoleText), which proves all 14 AMAX-D cases ran and passed; its full CTest result is **1,185 passed, four skipped, one disabled**. The stale “checks pending” sentence and unlinked 942-test claim were removed, and the live body was verified after publication. Thread-local maxima, the implicit loop barrier, the retained critical section and the parallel-region join preserve the shared maximum and final output. NaNs continue to lose the greater-than comparisons. Existing mutation counts need their own run/commit links before being described as current-head evidence. Line coverage alone would not prove which tests ran; the downloaded log does. Race-sanitizer coverage remains the documented follow-up.

<a id="pr-15259-draft-reply-tony-description"></a>

##### Posted reply to Tony — Test Result update

In response to [Tony’s description feedback](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5897487182). [Posted reply](https://github.com/ROCm/rocm-libraries/pull/15259#issuecomment-6924519806), November 16, 2026; exact body and target verified by reading it back.

> Updated Test Result with [Math CI build 7](https://math-ci.amd.com/job/rocm-libraries/job/tensilelite-unit-codecov/job/hipblaslt/job/PR-15259/7/consoleText), which tested `571ecae3fc48`. All 14 new AMAX-D cases passed; the full CTest run reported 1,185 passed, four skipped and one disabled, with pre-existing exclusions in the coverage configuration.
>
> I removed the unlinked 942-test claim and the stale “checks pending” sentence.

#### newling — COMMENTED — review 5899752492

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5899752492) · [@newling](https://github.com/newling) · Submitted: 2026-11-14 00:15:55 UTC

Reviewed commit: `571ecae3fc4814ff951461d1d14e48f780975511`.

> This is a nice stack, it's very clear what each PR contributes. I'm happy with overall. Can you please take a look at the following (final) agent review I ran? I've commented at the bottom what I think of the agent's review findings
>
> Agent table:
>
> | PR | Link to review | Number of findings | Main finding |
> | --- | --- | ---: | --- |
> | #15259 | [Review](https://github.com/newling/agent-public-reviews/blob/ede5c5f065835ae571ce0647b4e07af2c6cb8d64/rocm-libraries/review_pr15259_correctness_20261113.md) | 0 | No correctness defect found. |
> | #15266 | [Review](https://github.com/newling/agent-public-reviews/blob/ede5c5f065835ae571ce0647b4e07af2c6cb8d64/rocm-libraries/review_pr15266_correctness_20261113.md) | 1 | CLI parsing loses valid Int32 scalar values. |
> | #15274 | [Review](https://github.com/newling/agent-public-reviews/blob/ede5c5f065835ae571ce0647b4e07af2c6cb8d64/rocm-libraries/review_pr15274_correctness_20261113.md) | 2 | Aliased C/D gives incorrect Int8 results; large dot products trigger signed overflow. |
> | #15275 | [Review](https://github.com/newling/agent-public-reviews/blob/ede5c5f065835ae571ce0647b4e07af2c6cb8d64/rocm-libraries/review_pr15275_correctness_20261113.md) | 1 | Scaled Half-to-Float8 inputs change reference results. |
> | #15276 | [Review](https://github.com/newling/agent-public-reviews/blob/ede5c5f065835ae571ce0647b4e07af2c6cb8d64/rocm-libraries/review_pr15276_correctness_20261113.md) | 1 | Aliased C/D gives incorrect complex results. |
> | #15277 | [Review](https://github.com/newling/agent-public-reviews/blob/ede5c5f065835ae571ce0647b4e07af2c6cb8d64/rocm-libraries/review_pr15277_correctness_20261113.md) | 2 | Inherited batched bias-gradient fallback can double-free; non-HPA bias sums can change substantially. |
> | #15278 | [Review](https://github.com/newling/agent-public-reviews/blob/ede5c5f065835ae571ce0647b4e07af2c6cb8d64/rocm-libraries/review_pr15278_correctness_20261113.md) | 1 | The MX-tail test passes with all-NaN output. |
>
> My thoughts:
>
> #15266 has a easy float->double fix, might as well just take
> #15274 has a fix suggestion that looks like a code simplification, so might as well take too
> #15275 has a change to the slow path to make its semantics agree with the fast path. This one I'm not sure about, might need to probe what the GPU implementation actually does
> #15276 looks like the same fix suggested as #15274
> #15277 The 2 fixes combined overall simplify the code, so I wouldn't ask questions unless this breaks a downstream test
> #15278 Tightens a test, looks fine to me.

**Evaluation and recommendation — November 16, 2026:** Accept the #15259 result: this focused round proposes no fix for its unchanged head. It reuses earlier numerical validation, so it supplies no additional performance or sanitizer result. Keep newling’s proposed changes for #15266–#15278 with those PRs. The initial round’s OpenMP cleanup remains a useful suggestion even though this later, correctness-only round lists none.

### Inline review threads (2 threads, 2 comments)

#### Thread 1

`projects/hipblaslt/tensilelite/client/src/Reference.cpp` · Current line: 2298 · Original line: 2298 · Side: RIGHT

Status: Unresolved · Current diff.

##### tony-davis — comment 4568211288

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15259#discussion_r4568211288) · [@tony-davis](https://github.com/tony-davis) · Created: 2026-11-13 20:21:10 UTC

[Part of review 5897487182](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5897487182)

> (non-blocking) This critical is now the only thing keeping the merge correct, and I don't think the tests would catch it going away. All threads arrive here together after the `omp for` barrier, so a lost update would only fail a case when the dropped thread happened to hold the max. There's no TSAN lane yet (AIHPBLAS-5035 in hipBLASLt's `TESTING.md`), so this isn't on you. If you have a clang build with Archer handy, one TSAN run of the 14 `CPUGemm.slow_*AmaxD*` cases would be a nice line for the Test Result.

**Evaluation and agreed resolution — November 16, 2026:** Valid coverage gap; retain the critical section and handle the sanitizer request as a non-blocking validation follow-up. A value test cannot reliably detect every lost update after synchronization is removed, and an ordinary passing run does not establish race freedom. The branch’s [testing guidance](https://github.com/ROCm/rocm-libraries/blob/571ecae3fc4814ff951461d1d14e48f780975511/projects/hipblaslt/TESTING.md#L1682) records the missing TSAN run mode under [AIHPBLAS-5035](https://amd-hub.atlassian.net/browse/AIHPBLAS-5035). The agreed run of the instrumented reference with an OpenMP-aware detector such as Archer is recorded in [Future work](#future-work). No sanitizer result is claimed for #15259.

<a id="pr-15259-posted-reply-tony-race"></a>

##### Posted reply to Tony — Race-detection follow-up

[Published reply 4588246818](https://github.com/ROCm/rocm-libraries/pull/15259#discussion_r4588246818), in response to [Tony’s race-detection feedback](https://github.com/ROCm/rocm-libraries/pull/15259#discussion_r4568211288). Posted November 16, 2026; body and parent comment verified.

> Agreed—the critical merge stays in place. I haven’t run ThreadSanitizer with Archer on these cases, so I’m leaving race detection as a documented follow-up under AIHPBLAS-5035. I’ll record the OpenMP-aware sanitizer run in the future-work section.

##### Posted comment on AIHPBLAS-5035

[Published ticket comment 706371](https://amd-hub.atlassian.net/browse/AIHPBLAS-5035?focusedCommentId=706371). Posted November 16, 2026; body verified by reading it back. The ticket remains Open and unassigned.

> A concrete follow-up for this ticket came from [PR #15259](https://github.com/ROCm/rocm-libraries/pull/15259) and [Tony’s review comment](https://github.com/ROCm/rocm-libraries/pull/15259#discussion_r4568211288).
>
> The slow CPU reference computes a private AMAX-D maximum per thread and merges it under a critical section. The 14 numerical tests check the resulting value, but can still pass if synchronization is removed and a racing update happens to preserve the maximum.
>
> Please include the 14 `CPUGemm.slow_*AmaxD*` cases in an OpenMP-aware ThreadSanitizer run, using Archer with a compatible instrumented build. These are build-tree CPU driver tests; running the installed quick tier alone does not select them.
>
> Use `OMP_THREAD_LIMIT=1`, `3` and `8`, and record the observed team sizes. As a detector check, temporarily remove the merge synchronization and confirm that the multi-thread run reports the race; then restore it and require a clean run. Link the tested commit, commands and logs here.
>
> The critical section remains in #15259. No ThreadSanitizer/Archer run is claimed for that PR; this is the recorded validation follow-up.

#### Thread 2

`projects/hipblaslt/tensilelite/client/src/Reference.cpp` · Current line: 2256 · Original line: 2256 · Side: RIGHT

Status: Unresolved · Current diff.

##### tony-davis — comment 4568211307

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15259#discussion_r4568211307) · [@tony-davis](https://github.com/tony-davis) · Created: 2026-11-13 20:21:10 UTC

[Part of review 5897487182](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5897487182)

> (non-blocking) Now that the critical is gone, I think this per-element call can go too. Inside the parallel region it only sets the thread count for nested regions, and the loop body doesn't open any. The same call already runs before the region at line 1943.
>
> ```suggestion
> ```

**Evaluation and agreed resolution — November 16, 2026:** Correct suggestion, already implemented in [#15257](https://github.com/ROCm/rocm-libraries/pull/15257). The call at [line 2256](https://github.com/ROCm/rocm-libraries/blob/571ecae3fc4814ff951461d1d14e48f780975511/projects/hipblaslt/tensilelite/client/src/Reference.cpp#L2256) changes a worker’s setting for future nested regions; it cannot resize the active team, and this loop starts no nested parallel region. #15257 removes this call together with the outer setter, the reduction setter and the 64-thread define. The agreed resolution is to keep the cleanup on #15257 and link it in the posted reply below. #15257 is open, so this item is addressed there, pending integration. Verify the combined result preserves the setter deletion, per-thread maximum and critical merge.

<a id="pr-15259-draft-reply-tony-openmp"></a>

##### Posted reply to Tony — Per-element OpenMP setter

In response to [Tony’s OpenMP setter feedback](https://github.com/ROCm/rocm-libraries/pull/15259#discussion_r4568211307). [Posted reply](https://github.com/ROCm/rocm-libraries/pull/15259#discussion_r4589423954), November 16, 2026; exact body and target verified by reading it back.

> This per-element setter is removed in [#15257](https://github.com/ROCm/rocm-libraries/pull/15257), together with the other fixed 64-thread setters. I’m keeping the OpenMP thread-policy cleanup there and keeping this PR focused on the AMAX-D lock restructuring.

### Newling’s linked reviews and cross-PR remarks

Posted on #15259: [initial round](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5867396457) · [final round](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5899752492). The [shared testing and design assessment](#shared-assessment) applies to this PR too.

Newling’s original stack-wide remarks and full tables are copied in the Reviews subsection above.

#### Initial agent review, linked November 12

[Original on GitHub](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15259.md) · [Newling’s source post](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5867396457)

Source revision: `30df5bab4294f839ef85a91c732c36c92c0821ca`. File: `rocm-libraries/review_pr15259.md`.

> This is a review from an agent with an automatic prompt from the reviewer
>
> **PR reviewed:** [ROCm/rocm-libraries#15259](https://github.com/ROCm/rocm-libraries/pull/15259)
>
> **Submitted head:** `571ecae3fc4814ff951461d1d14e48f780975511`. **Incremental base:** `e8b44f8b6b0e8d9ffffc294203d6ef063a85f40d`.
>
> Independent review of the incremental change and its interaction with the CPU-reference stack, emphasizing test adequacy, runtime, overlap, design reuse, and unsupported cases. Inputs were the public descriptions, code, repository guidance and CI status/logs; target-PR discussions and existing reviews were not read.
>
> **Suggestion branch:** [review/pr15259-suggestions](https://github.com/newling/rocm-libraries/tree/review/pr15259-suggestions), based on the submitted head above; final head [f8ae33f65755293d932e7066102d1b3378d71146](https://github.com/newling/rocm-libraries/commit/f8ae33f65755293d932e7066102d1b3378d71146).
>
> ## Tests
>
> Submitted head: `cpu-gemm-driver`, `tensilelite-tests`, and `tensilelite-client` built successfully; **297 driver CTests and 24 focused reference gtests passed**. The driver suite took 2.29 s with four CTest workers. Suggestion implementation: the same targets built; **297 driver CTests and 24 focused reference gtests passed**. Later comment-only commits, where present, do not change that execution result.
>
> Validation used a Release ROCm compiler build. Controlled runs set `OMP_THREAD_LIMIT=8`, `OMP_NUM_THREADS=4`, and `KMP_BLOCKTIME=0`; the original stack-tip driver timing predates the thread-limit setting. These are host numerical tests, not GPU-kernel performance measurements. Root/component hook invocations and full-stack whitespace checks were clean; the formatting hooks opt out of these C++/CMake paths, and no Python tests were selected by the component hook. No local GPU or sanitizer run is claimed.
>
> The gtests were run directly with `--gtest_filter=ReferenceFastPath.*:ReferenceMX*.*:*ReferenceMX*/*`. In the category-based build, a CTest name regex containing `ReferenceFastPath` does not select them. Driver CTests are build-tree tests and are not installed into the TheRock test package; Math CI's unfiltered `coverage-cpp` lane runs them but is informational. The gtest categories include standard/comprehensive/full, not quick.
>
> ## Summary
>
> The important optimization is removing synchronization from ordinary slow-path output writes. Per-thread AMAX-D maxima retain one small merge per participating thread and work with class accumulator types that do not support OpenMP's built-in max reduction. I found no correctness defect in that change.
>
> The 14 added tests are appropriately scoped: ordinary and combined epilogues, several accumulator/storage types, batching, zero alpha, and a negative 1×1 value. The driver's independent GEMM and the initial AMAX-D sentinel help detect a missing write. These tests exercise a distinct output contract and are not duplicated by the later fast-path tests.
>
> ## Actionable items
>
> None for the lock/max restructuring.
>
> ## Suggestions
>
> ### Remove the redundant OpenMP policy update inside the output loop
>
> [client/src/Reference.cpp:2256](https://github.com/ROCm/rocm-libraries/blob/571ecae3fc4814ff951461d1d14e48f780975511/projects/hipblaslt/tensilelite/client/src/Reference.cpp#L2256) calls `omp_set_num_threads(64)` for every output after the parallel region has already started. It cannot resize that region and is unrelated to computing the maximum. Suggestion commit [f8ae33f6575](https://github.com/newling/rocm-libraries/commit/f8ae33f65755293d932e7066102d1b3378d71146) removes it; the outer thread policy is retained.
>
> The test descriptions' `OMP_NUM_THREADS=1/3/32` runs do not establish those team sizes: the outer call overrides that setting. Use `OMP_THREAD_LIMIT` or record the observed team size. All 14 AMAX-D cases passed locally with limits of 1, 3, and 8. This strengthens the merge-coverage evidence without adding a permanent test matrix.
>
> ## Scope decisions
>
> The published performance numbers refer to earlier unsplit work and common-test replay. I did not reproduce those GPU-workload timings. The source change removes the identified serialization, and local test runtimes are reported separately from performance claims. A broader redesign of the fixed 64-thread policy is independent of this optimization.
>
> ## Suggestion branch handoff
>
> | Suggestion commit | Item |
> | --- | --- |
> | [f8ae33f6575](https://github.com/newling/rocm-libraries/commit/f8ae33f65755293d932e7066102d1b3378d71146) | avoid resetting OpenMP policy for every output |
>
> The suggestion branches are published. Each branch contains its own corrections on its own submitted head, not the other review branches' fixes. Integrate shared changes in stack order, including the alias fix once and later `TypeInfo` call sites. PR-description corrections are reported for the author to make; none were published. See [the cross-PR assessment](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_cpu_reference_stack_15259_15278.md) for test overlap, CI limits and integration notes.
>

**Evaluation and recommendation — November 16, 2026:** Accept the correctness assessment and keep the test matrix. Its proposed one-line deletion is exactly the existing cleanup in [#15257](https://github.com/ROCm/rocm-libraries/pull/15257); I inspected suggestion commit f8ae33f6575 and confirmed it changes only that setter. Keep the broader thread policy on #15257. The team-size caveat is correct: the [outer call](https://github.com/ROCm/rocm-libraries/blob/571ecae3fc4814ff951461d1d14e48f780975511/projects/hipblaslt/tensilelite/client/src/Reference.cpp#L1943) requests 64 threads despite OMP_NUM_THREADS. A standalone GCC/OpenMP probe observed 64 threads with OMP_NUM_THREADS=4, then 1, 3 and 8 with the respective OMP_THREAD_LIMIT settings. That probe verifies runtime semantics; the review’s reported 14-case runs at those limits remain its own evidence. Use controlled limits for future validation and record the observed team size for performance runs. The CTest category warning is configuration-dependent and supported by the CMake code: use the gtest binary directly for focused reference tests. At the archived head, the driver tests were build-tree registrations outside the installed test-file generation. The agreed CI integration now exports them into the existing required TheRock standard suite, with local validation recorded in [implementation and validation](#pr-15259-host-ci-spec); shared PR CI remains pending publication.

**Agreed resolution — November 16, 2026:** The live #15259 description has been corrected and verified without changing its head. Test Plan constrains AMAX-D runs with OMP_THREAD_LIMIT=1/3/8, explicitly selects those CTests with an error when no tests match, and runs focused reference gtests directly for category-based builds. The historical timing paragraph retains OMP_NUM_THREADS=25 as the recorded environment setting and states that the actual team size is unverified. Related now links #15257, which owns the thread-policy change. At description-edit time, the documented command snippets passed Bash syntax checks with the build placeholder replaced; the numerical matrix had not yet been independently rerun. The subsequent CI integration passed all 42 local AMAX-D configurations, as recorded above.

<a id="pr-15259-draft-reply-newling-thread-counts"></a>

##### Posted reply to newling — Thread-count validation

In response to the thread-count warning in [newling’s initial linked review](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15259.md), posted in [review 5867396457](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5867396457). [Posted reply](https://github.com/ROCm/rocm-libraries/pull/15259#issuecomment-6924521499), November 16, 2026; exact body and target verified by reading it back.

> Updated the Test Plan to constrain the AMAX-D runs with `OMP_THREAD_LIMIT=1`, `3` and `8`. I also qualified the historical `OMP_NUM_THREADS=25` timing setting: the actual team size for those runs is unverified. The broader thread-policy change is in [#15257](https://github.com/ROCm/rocm-libraries/pull/15257).

#### Final correctness review, linked November 14 (document dated November 13)

[Original on GitHub](https://github.com/newling/agent-public-reviews/blob/ede5c5f065835ae571ce0647b4e07af2c6cb8d64/rocm-libraries/review_pr15259_correctness_20261113.md) · [Newling’s source post](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5899752492)

Source revision: `ede5c5f065835ae571ce0647b4e07af2c6cb8d64`. File: `rocm-libraries/review_pr15259_correctness_20261113.md`.

> This is a review from an agent with an automatic prompt from the reviewer
>
> **PR reviewed:** [ROCm/rocm-libraries#15259](https://github.com/ROCm/rocm-libraries/pull/15259)
>
> **Submitted head:** `571ecae3fc4814ff951461d1d14e48f780975511`. **Incremental diff base:** `e8b44f8b6b0e8d9ffffc294203d6ef063a85f40d`.
>
> **Scope:** Focused follow-up on consequential CPU-reference errors and regression-test failures, including the existing review discussion. Head and discussion were unchanged at the 2026-11-13 23:09 UTC check. Earlier validation is reused where the tested code is unchanged.
>
> **Suggestion branch:** [review/pr15259-correctness-20261113](https://github.com/newling/rocm-libraries/tree/review/pr15259-correctness-20261113), based on the submitted head; final head [571ecae3fc4](https://github.com/newling/rocm-libraries/commit/571ecae3fc4814ff951461d1d14e48f780975511). Commit-to-item mapping is given below.
>
> ## Tests
>
> Reused the earlier validation at the unchanged submitted head: the driver, reference tests and client built; 297 driver CTests and 24 focused reference tests passed. The new review branch has no code changes. These are CPU tests; performance and GPU execution were not re-evaluated.
>
> ## Summary
>
> The change removes the lock from independent output writes and retains synchronization when merging the maximum absolute output value, AMAX-D. Each worker keeps its own maximum and merges it once. The existing tests exercise the maximum as well as the output, including a negative one-element result and an initialized maximum sentinel.
>
> I found no consequential correctness or cleanup defect in this change.
>
> ## Actionable items
>
> None.
>
> ## Suggestions
>
> None in this focused review.
>
> **Suggestion summary:** [review/pr15259-correctness-20261113](https://github.com/newling/rocm-libraries/tree/review/pr15259-correctness-20261113) remains at the submitted head. No fix commits or implementation omissions.
>

**Evaluation and recommendation — November 16, 2026:** Accept “no consequential correctness defect found.” The reviewed head matches the current PR and the protected per-thread merge matches the source. There are no fix commits to import. The report explicitly reuses older numerical validation; count it as a follow-up assessment, not another independent test or performance run. The exact-head Math CI log supplies the directly checked test execution evidence.

### General discussion comments (3, including bots)

#### therock-pr-bot[bot] — comment 6692553222

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15259#issuecomment-6692553222) · [@therock-pr-bot[bot]](https://github.com/apps/therock-pr-bot) · Created: 2026-11-06 18:40:01 UTC · Updated: 2026-11-10 22:32:08 UTC

> <!-- therock-pr-bot-policy-check -->
> ### ✅ All Checks Passed — Ready for Review
>
> | Check | Status | Details |
> |---|:---:|---|
> | 📝 **PR Description** | ✅ Pass | — |
> | ⛔ **Forbidden Files** | ✅ Pass | — |
> | 🧪 **Unit Test** | ⚠️ Warning | **Error:** Source/code files changed without an accompanying unit test.<br>**Expected:** add at least one test file named like `test_<name>.py` / `test_<name>.cpp` (or `<name>_test.*`).<br>**Current:** code file(s) changed: `projects/hipblaslt/tensilelite/client/cpu_gemm_driver.cpp`, `projects/hipblaslt/tensilelite/client/src/Reference.cpp`; no test file found |
> | 🔎 **pre-commit** | ✅ Pass | — |
> | 🚫 **Draft PR** | 🔜 To Be Enabled | — |
> | 🚩 **Feature Flag** | 🔜 To Be Enabled | — |
> | 📊 **Code Coverage** | 🔜 To Be Enabled | — |
> | 🤖 **therock-pr-bot** | ✅ Pass | — |
>
> > 🎉 All checks passed! This PR is ready for review.
>
> 📖 **Need help?** See the [Policy FAQ](https://github.com/ROCm/rocm-libraries/blob/develop/docs/LIBRARIES_PR_BOT_FAQ.md) for details on every check and how to fix failures.
>
> 🙋 **[Wish to Override Policy?](https://github.com/ROCm/rocm-libraries/blob/develop/docs/LIBRARIES_PR_BOT_FAQ.md#-wish-to-override-the-policy-process-and-get-unblocked)**

**Evaluation and agreed resolution — November 16, 2026:** Ignored per user instruction. The unit-test warning is a filename heuristic: the actual diff adds 14 tests through [CMake registrations](https://github.com/ROCm/rocm-libraries/blob/571ecae3fc4814ff951461d1d14e48f780975511/projects/hipblaslt/tensilelite/tests/CMakeLists.txt#L677). The [Math CI log](https://math-ci.amd.com/job/rocm-libraries/job/tensilelite-unit-codecov/job/hipblaslt/job/PR-15259/7/consoleText) confirms they execute successfully. No follow-up action is planned for this warning.

#### therock-pr-bot[bot] — comment 6692554360

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15259#issuecomment-6692554360) · [@therock-pr-bot[bot]](https://github.com/apps/therock-pr-bot) · Created: 2026-11-06 18:40:04 UTC · Updated: 2026-11-10 22:32:10 UTC

> <!-- therock-pr-bot-fix-policies -->
> 🎉 All checks passed! This PR is ready for review.

**Evaluation and recommendation — November 16, 2026:** Acknowledgment only; it asks for no change. Its “ready for review” message confirms the policy bot’s result, while the source review and test evidence determine how to handle the technical feedback.

#### codecov[bot] — comment 6787721731

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15259#issuecomment-6787721731) · [@codecov[bot]](https://github.com/apps/codecov) · Created: 2026-11-11 05:16:06 UTC · Updated: 2026-11-14 01:11:30 UTC

> ## [Codecov](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15259?dropdown=coverage&src=pr&el=h1&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) Report
> :white_check_mark: All modified and coverable lines are covered by tests.
>
> :x: Your project check has failed because the head coverage (52.90%) is below the target coverage (80.00%). You can increase the head coverage or adjust the [target](https://docs.codecov.com/docs/commit-status#target) coverage.
>
> <details><summary>Additional details and impacted files</summary>
>
>
>
> ```diff
> @@             Coverage Diff             @@
> ##           develop   #15259      +/-   ##
> ===========================================
> - Coverage    77.69%   77.60%   -0.09%
> ===========================================
>   Files         3061     3002      -59
>   Lines       543959   521646   -22313
>   Branches     86453    81316    -5137
> ===========================================
> - Hits        422614   404812   -17802
> + Misses       90322    88982    -1340
> + Partials     31023    27852    -3171
> ```
>
> | [Flag](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15259/flags?src=pr&el=flags&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | Coverage Δ | | *Carryforward flag |
> |---|---|---|---|
> | [TensileLite-CPP](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15259/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `52.90% <100.00%> (+0.09%)` | :arrow_up: | |
> | [TensileLite-Unit](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15259/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `83.84% <ø> (ø)` | | |
> | [hipBLAS](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15259/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `90.62% <ø> (ø)` | | Carriedforward from [ffab621](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/ffab621a55274338588213b0a1b9505d317cca4c?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipBLASLt](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15259/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `70.47% <ø> (+0.04%)` | :arrow_up: | |
> | [hipCUB](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15259/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `82.68% <ø> (+2.46%)` | :arrow_up: | Carriedforward from [ffab621](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/ffab621a55274338588213b0a1b9505d317cca4c?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipDNN](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15259/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `87.34% <ø> (ø)` | | Carriedforward from [ffab621](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/ffab621a55274338588213b0a1b9505d317cca4c?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipFFT](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15259/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `42.65% <ø> (ø)` | | Carriedforward from [ffab621](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/ffab621a55274338588213b0a1b9505d317cca4c?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipRAND](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15259/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `76.12% <ø> (ø)` | | Carriedforward from [ffab621](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/ffab621a55274338588213b0a1b9505d317cca4c?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipSOLVER](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15259/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `69.67% <ø> (ø)` | | Carriedforward from [ffab621](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/ffab621a55274338588213b0a1b9505d317cca4c?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipSPARSE](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15259/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `87.03% <ø> (ø)` | | Carriedforward from [ffab621](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/ffab621a55274338588213b0a1b9505d317cca4c?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocALUTION](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15259/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `86.47% <ø> (ø)` | | Carriedforward from [ffab621](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/ffab621a55274338588213b0a1b9505d317cca4c?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocBLAS](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15259/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `69.16% <ø> (ø)` | | Carriedforward from [ffab621](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/ffab621a55274338588213b0a1b9505d317cca4c?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocFFT](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15259/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `48.81% <ø> (ø)` | | Carriedforward from [ffab621](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/ffab621a55274338588213b0a1b9505d317cca4c?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocRAND](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15259/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `57.32% <ø> (ø)` | | Carriedforward from [ffab621](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/ffab621a55274338588213b0a1b9505d317cca4c?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocSOLVER](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15259/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `76.66% <ø> (ø)` | | Carriedforward from [ffab621](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/ffab621a55274338588213b0a1b9505d317cca4c?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocSPARSE](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15259/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `83.86% <ø> (ø)` | | Carriedforward from [ffab621](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/ffab621a55274338588213b0a1b9505d317cca4c?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocSPARSE-unit](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15259/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `79.82% <ø> (ø)` | | Carriedforward from [ffab621](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/ffab621a55274338588213b0a1b9505d317cca4c?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocThrust](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15259/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `93.15% <ø> (ø)` | | Carriedforward from [ffab621](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/ffab621a55274338588213b0a1b9505d317cca4c?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
>
> *This pull request uses carry forward flags. [Click here](https://docs.codecov.io/docs/carryforward-flags?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) to find out more.
>
> | [Files with missing lines](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15259?dropdown=coverage&src=pr&el=tree&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | Coverage Δ | |
> |---|---|---|
> | [...cts/hipblaslt/tensilelite/client/src/Reference.cpp](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15259?src=pr&el=tree&filepath=projects%2Fhipblaslt%2Ftensilelite%2Fclient%2Fsrc%2FReference.cpp&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm#diff-cHJvamVjdHMvaGlwYmxhc2x0L3RlbnNpbGVsaXRlL2NsaWVudC9zcmMvUmVmZXJlbmNlLmNwcA==) | `42.61% <100.00%> (+1.08%)` | :arrow_up: |
>
> ... and [154 files with indirect coverage changes](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15259/indirect-changes?src=pr&el=tree-more&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm)
> </details>
> <details><summary> :rocket: New features to boost your workflow: </summary>
>
> - :package: [JS Bundle Analysis](https://docs.codecov.com/docs/javascript-bundle-analysis): Save yourself from yourself by tracking and limiting bundle sizes in JS merges.
> </details>

**Evaluation and agreed resolution — November 16, 2026:** Ignored per user instruction; no follow-up action for #15259. The patch check passes, while the TensileLite-CPP project flag is 52.90% against an 80% target; that measures the entire flag’s coverage, not just this patch. The live rollup also has a failed hipBLASLt project coverage check. The [TensileLite testing guidance](https://github.com/ROCm/rocm-libraries/blob/571ecae3fc4814ff951461d1d14e48f780975511/projects/hipblaslt/tensilelite/TESTING.md#L753) identifies this coverage report as advisory. The new AMAX-D tests already run and cover the intended output behavior.

<a id="pr-15266"></a>

## PR #15266 — test(tensilelite): cpu-gemm-driver Int8 inputs with separate C/D and alpha types

[GitHub PR](https://github.com/ROCm/rocm-libraries/pull/15266) · State: **OPEN**

Branch: `users/alvasile/tl-speedup-16-cpu-gemm-driver-int8`. Base: `users/alvasile/tl-speedup-09-reference-slow-path-amax-d`. Head at collection: `4554dab796dee28e4e640b7f688c4d0f12d5b375`.

### Feedback walkthrough — November 16, 2026

#### Preserve Int32 alpha/beta during CLI parsing — agreed fix

The [initial linked review](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15266.md#preserve-the-full-advertised-int32-scalar-range-during-cli-parsing) and [final correctness review](https://github.com/newling/agent-public-reviews/blob/ede5c5f065835ae571ce0647b4e07af2c6cb8d64/rocm-libraries/review_pr15266_correctness_20261113.md#preserve-int32-scalars-before-validating-or-using-them) identify the same test-driver bug. Alpha/beta were parsed as float, losing precision before the Int32 domain check and before either the reference or oracle used them. Valid beta `2147483647` rounded up and was rejected; some other integers above `2^24` could silently change. The reproduction failed on the rebased branch at `3c54ff8ac10294fb5840cedf0b45e237a5babc67`.

**Agreed implementation:** apply [newling's suggestion 2fcafc2eee3](https://github.com/newling/rocm-libraries/commit/2fcafc2eee3f322abd349beeda112823931c8591). Parse and carry alpha/beta as double through `runGemm`, then use the existing conversions to the selected scalar type. Keep the integer and range checks in double. Add `CPUGemm.slow_I8_I32_I32_Int32BetaBoundary` through the shared registration helper before category labeling, so it runs in both the build-tree and installed standard suites. Source formatting and the CMake insertion point are adapted to this branch's local CI integration.

[Reply to newling](https://github.com/ROCm/rocm-libraries/pull/15266#issuecomment-6925033977) was posted before implementation and verified by reading it back:

> @newling Agreed. I’m going to apply your scalar-parsing fix in #15266: preserve alpha and beta as double until conversion to the selected scalar type, and add the Int32 boundary regression. I reproduced the rejection of valid beta `2147483647` on this branch.

**Status:** implemented and validated locally. The original driver fails the new regression with exit 1; the fixed regression passes. All **390 driver CTests passed in both the build tree and a relocated installation**, with no failures or skips. All **24 focused reference gtests passed** from the installation. Discovery confirms the 389 existing driver commands and properties are preserved; only the boundary test is added, with matching installed/build-tree arguments and properties. Twelve manual controls confirm that fractional, out-of-range and non-finite alpha/beta values remain rejected. Scoped root and component pre-commit hooks passed; the component hook selected no Python unit/characterization tests. The fix has not been pushed. Logs, reports, discovery JSON, the source suggestion and verified reply are preserved in `/tmp/tl-pr15266-scalar-fix-20261116/`.

#### Use the existing type metadata — agreed cleanup

Newling's [initial review](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15266.md#use-the-existing-type-metadata) proposes replacing the driver's private `TypeTraits` table with `TensileLite::TypeInfo<T>::Enum`. Source inspection confirms that `TypeInfo` already covers all 11 locally specialized types with the same enum values, including packed `Float4x2`.

**Agreed implementation:** applied [suggestion 088d585c895](https://github.com/newling/rocm-libraries/commit/088d585c895cff36e459ee0409c1b812edd9da81). Added the explicit `Tensile/DataTypes.hpp` include, removed all 11 local specializations and replaced the four enum lookups with `TypeInfo<T>::Enum`. The mappings are unchanged. When restacking the later PRs, convert any newly introduced `TypeTraits` uses and specializations to the shared metadata too; that integration is still pending.

**Status:** implemented and validated locally. The Release build of `cpu-gemm-driver` and `tensilelite-tests` succeeded, and all **390 existing driver CTests passed** in the build tree with no failures or skips. Scoped root and component pre-commit hooks passed; the component hook selected no Python unit/characterization tests. No test registrations or installation rules changed. The cleanup has not been pushed. The source suggestion, patch, build log, CTest log, JUnit report and hook logs are preserved in `/tmp/tl-pr15266-typeinfo-cleanup-20261116/`.

**[Reply to newling](https://github.com/ROCm/rocm-libraries/pull/15266#issuecomment-6925521343):** posted after the local commit and verified by reading it back.

> @newling Applied your [TypeInfo cleanup](https://github.com/newling/rocm-libraries/commit/088d585c895cff36e459ee0409c1b812edd9da81) in #15266. The driver now uses `TypeInfo<T>::Enum` instead of its private type table. The build and all 390 driver CTests passed locally. The commit is local for now.

#### Require the intended diagnostic in rejected-option tests — agreed fix

[Tony's inline comment](https://github.com/ROCm/rocm-libraries/pull/15266#discussion_r4568212077) and [newling's linked review](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15266.md#require-the-intended-diagnostic-in-rejected-option-tests) identify the same coverage gap. `CPUGemm.I8_MixedA_Rejected`, `CPUGemm.I8_UnsupportedCD_Rejected`, `CPUGemm.I8_NonIntegerAlpha_Rejected` and `CPUGemm.F32_TypeCD_Rejected` use `WILL_FAIL`, so an unrelated ordinary nonzero exit can satisfy them.

**Agreed implementation:** adapted the shared failure checker from [suggestion 66ef187d435](https://github.com/newling/rocm-libraries/commit/66ef187d43535cacaba49a328b77b0388909291e) here. The existing `add_cpu_gemm_test` helper now accepts `EXPECTED_ERROR` and runs `cmake/CheckCpuGemmFailure.cmake`, requiring exit code **1** and each case's diagnostic. The four registrations include stable diagnostic prefixes and no longer use `WILL_FAIL`. Arguments, environment, processor limits, 120-second timeouts and category labels are preserved. The checker installs alongside the relocated CTest suite; its installed command uses relative driver/checker paths and finds CMake through PATH. Later PRs can reuse this facility for their negative cases. Changes for later branches in the full stack-tip suggestion were not applied.

Adding `PASS_REGULAR_EXPRESSION` while retaining `WILL_FAIL` is unsuitable: a two-case control with the installed CTest version failed on the intended diagnostic and passed on unrelated text because the result is inverted. The agreed wrapper checks both conditions and reports success only when both hold. Property-interaction evidence is in `/tmp/tl-pr15266-typeinfo-cleanup-20261116/ctest-property-control/`.

**[Reply in Tony's thread](https://github.com/ROCm/rocm-libraries/pull/15266#discussion_r4589967020):** posted before implementation, mentioning newling's shared checker, and verified by reading it back.

> Agreed. I’ll adapt @newling’s shared failure checker here so these four cases require exit code 1 and their intended diagnostic. It will use the existing registration helper and run in both the build-tree and relocated installed suites. I checked `PASS_REGULAR_EXPRESSION` with `WILL_FAIL`: together they invert the intended result, so I’ll use the wrapper.

**Status:** implemented and validated locally. The Release configure/build succeeded. All **390 driver CTests passed in both the build tree and a relocated installation**, with no failures or skips. Discovery confirms that only these four commands and their `WILL_FAIL` settings changed; all other driver commands and properties are preserved. Installed/build-tree arguments, diagnostics and settings match, and the installed CTest file has no source/build paths. Temporary unknown-option controls passed all four old registrations but correctly failed all four strengthened cases in both locations. Matching-diagnostic controls with exit codes 0 and 2 also correctly failed. No driver registrations were added. Scoped root and component pre-commit hooks passed; the component hook selected no Python unit/characterization tests. Evidence, including the suggestion, verified reply, discovery snapshots, JUnit reports, controls and hook logs, is in `/tmp/tl-pr15266-rejection-diagnostics-20261116/`. The code has not been pushed.

#### Explain the Int8 reduction and epilogue casts — agreed documentation fix

[Newling's inline request](https://github.com/ROCm/rocm-libraries/pull/15266#discussion_r4554390239), seconded by [Tony's review](https://github.com/ROCm/rocm-libraries/pull/15266#pullrequestreview-5897488069), asks for a short pseudocode paragraph with explicit casts. The existing prose describes `MacT` and `AccumT` and the output-cast helper, but does not show the Int8 calculation in one place.

**Agreed implementation:** added a short Int8-specific pseudocode block beside the golden GEMM documentation. It shows exact integer dot-product accumulation with an int32-range check, one conversion to the selected alpha/epilogue type, separate alpha/beta and bias steps, activation and final `SaturateCast` to the C/D storage type. It names the two integer-alpha modes and four float-alpha modes, and explains the oracle's int64 accumulation for overflow detection. The existing explanations of scaling, activation, checked integer epilogues and storage conversion remain. The block explicitly distinguishes the driver's oracle from the current float-alpha slow reference: [#15764](https://github.com/ROCm/rocm-libraries/issues/15764) tracks that reduction difference, and [#15765](https://github.com/ROCm/rocm-libraries/pull/15765) remains the planned fix in this stack.

**[Reply to newling](https://github.com/ROCm/rocm-libraries/pull/15266#discussion_r4590172300):** posted before implementation and verified by reading it back.

> Agreed. I’ll add a short Int8 pseudocode block showing the integer dot product, the conversion to the selected alpha/epilogue type, and the final C/D storage conversion. I’ll spell out which modes use Int32 versus float alpha/beta, and distinguish the driver’s oracle from the slow-reference issue tracked by #15764 and addressed in #15765.

**Status:** implemented locally as source comments only. The pseudocode was checked against the golden reduction, epilogue and storage-cast implementations and all six type registrations. All noncomment source lines are identical before and after the change. Scoped root and component pre-commit hooks passed; the component hook selected no Python unit/characterization tests. Validation evidence, hook logs and the verified reply are preserved in `/tmp/tl-pr15266-int8-pseudocode-20261116/`. The change has not been pushed.

#### Link the small-K test limit to the tracked reference bug — agreed documentation fix

[Tony's inline suggestion](https://github.com/ROCm/rocm-libraries/pull/15266#discussion_r4568212035) asks for `(#15764)` beside the float-accumulation limit in the Int8 CMake commentary. The comment currently explains the `2^24` precision limit and K = 151, but lacks the issue link.

**Agreed implementation:** applied Tony's exact one-line suggestion to link the limit to [#15764](https://github.com/ROCm/rocm-libraries/issues/15764). The test dimensions remain K = 151 while [#15765](https://github.com/ROCm/rocm-libraries/pull/15765) is pending insertion into the stack. The user also requested a follow-up under #15765 to refresh this description as part of integrating its integer-reduction fix.

**[Reply to Tony](https://github.com/ROCm/rocm-libraries/pull/15266#discussion_r4590247429):** posted before implementation and verified by reading it back.

> Agreed. I’ll add #15764 to this comment and keep K = 151 for now. I’ll record a follow-up under #15765 to update this description as part of integrating its integer-reduction fix.

**Status:** implemented locally as a CMake comment change. All noncomment CMake lines are identical, preserving test dimensions, arguments and registrations. Scoped root and component pre-commit hooks passed; the component hook selected no Python unit/characterization tests. Validation evidence, hook logs and the verified reply are preserved in `/tmp/tl-pr15266-small-k-link-20261116/`. The change has not been pushed.

#### Refresh the PR's test results and related links — agreed and published

[Tony's review](https://github.com/ROCm/rocm-libraries/pull/15266#pullrequestreview-5897488069) asks to replace the stale claims that this split branch has not been built/tested and automated checks are pending, and add #15764/#15765 to Related. The live PR body still contains those claims.

**Agreed implementation:** updated Test Result with links to the verified passing [Multi-Arch CI Summary](https://github.com/ROCm/rocm-libraries/actions/runs/42914665250/job/130356281758), [Component CI Summary](https://github.com/ROCm/rocm-libraries/actions/runs/42914663179/job/130319662977) and [Math CI Summary](https://math-ci.amd.com/job/rocm-libraries/job/status-gate/job/hipblaslt/job/PR-15266/lastBuild/stages/) for published head `4554dab796dee28e4e640b7f688c4d0f12d5b375`. The two linked GitHub jobs were independently checked to have that head and successful conclusions. Separately recorded the local review fixes at `f20b0195c60a`: Release builds, all 390 driver CTests passing in both the build tree and relocated installation, failure controls and scoped hooks. Their JUnit reports were reread to confirm counts and absence of failures/skips. The body explicitly says the local fixes are not pushed. Related now links [#15764](https://github.com/ROCm/rocm-libraries/issues/15764), [#15765](https://github.com/ROCm/rocm-libraries/pull/15765) and the preceding #15259, and explains the planned insertion. No per-case CI count was inferred from summary statuses.

**[Reply to Tony](https://github.com/ROCm/rocm-libraries/pull/15266#issuecomment-6927257494):** posted after updating the body and verified by reading it back.

> @tony-davis Updated Test Result with links to the passing CI summaries at `4554dab` and the separately identified local validation: 390 driver tests in both the build tree and relocated installation. Related now links #15764 and #15765 and explains where #15765 will be inserted in the stack. The code fixes remain local.

**Status:** published and verified. Only Test Result and Related changed, and the PR head remained unchanged. The exact body and reply were shown before publication. `gh pr edit` failed on the retired Projects API; a guarded REST body update succeeded. Snapshots, payload, CI job metadata, proposed/verified body and verified reply are preserved in `/tmp/tl-pr15266-description-refresh-20261116/`.

**Walkthrough status:** all human feedback items on #15266 have an agreed resolution. The source fixes remain local, and #15765's stack insertion and documentation follow-up remain recorded in Future work.

### Reviews (3)

#### newling — COMMENTED — review 5879596049

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15266#pullrequestreview-5879596049) · [@newling](https://github.com/newling) · Submitted: 2026-11-12 18:25:27 UTC

Reviewed commit: `4554dab796dee28e4e640b7f688c4d0f12d5b375`.

*No body text; review metadata is retained above.*

#### Alex-Vasile — COMMENTED — review 5895076688

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15266#pullrequestreview-5895076688) · [@Alex-Vasile](https://github.com/Alex-Vasile) · Submitted: 2026-11-13 17:06:52 UTC

Reviewed commit: `4554dab796dee28e4e640b7f688c4d0f12d5b375`.

*No body text; review metadata is retained above.*

#### tony-davis — APPROVED — review 5897488069

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15266#pullrequestreview-5897488069) · [@tony-davis](https://github.com/tony-davis) · Submitted: 2026-11-13 20:21:13 UTC

Reviewed commit: `4554dab796dee28e4e640b7f688c4d0f12d5b375`.

> Hi Alex! Thanks for splitting this out. Pinning the slow path for all six Int8 type ids before the fast path lands is exactly the order I'd want. I read the driver's golden against `Reference.cpp` at the base, and it follows the slow path's epilogue step by step (alpha, then the ScaleAB scales, then `alpha * sum`, then beta * C and bias) and its `SaturateCast` rules. The one reordering is `scaleAlphaVec`, and the comment owns it.
>
> It's great that newling's question turned up a real slow-path bug (#15764), with a fix already in #15765. I'd second newling's ask for a short pseudocode paragraph in the driver header. Something like "sum = sum over k of int32(a) * int32(b); value = TypeAlpha(sum); D = SaturateCast<TypeCD>(alpha * value + beta * C + bias)" would answer it in a few lines.
>
> I'm approving. A few non-blocking things:
>
> 1. Test Result still says the split branch hasn't been built or tested and that checks are pending. The required checks are green at 4554dab, and Math CI's `tensilelite-unit-codecov` job passed too (I believe it runs the whole `tensilelite-tests` CTest suite, so these 64 cases included). It'd help to say so, and to link #15764 and #15765 in Related.
> 2. Two small inline notes on the CMake side.
>
> Merge-wise this sits on #15259, so it waits on that one.
>
> <!-- rocm-pr-quality:attestation v1 -->
> > **Reviewed with the ROCm PR-quality skill.**
> > - Skill: `rocm-pr-quality` (base) - overlays: `hipblaslt-pr-quality`
> > - Action: `review` - Result: **APPROVED** (`0` BLOCKING; `0` IMPORTANT)
> > - Risk: `1`
> > - Reviewed commit: `4554dab796dee28e4e640b7f688c4d0f12d5b375`
> > - Attestation: `1785880c`
> >
> > <sub>Machine-checkable attestation. The digest ties this review to this PR; later commits are
> > allowed. The reviewed commit is informational (what the review looked at).</sub>

### Inline review threads (3 threads, 4 comments)

#### Thread 1

`projects/hipblaslt/tensilelite/client/cpu_gemm_driver.cpp` · Current line: 167 · Original line: 167 · Side: RIGHT

Status: Unresolved · Current diff.

##### newling — comment 4554390239

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15266#discussion_r4554390239) · [@newling](https://github.com/newling) · Created: 2026-11-12 18:07:11 UTC · Updated: 2026-11-12 18:25:27 UTC

[Part of review 5879596049](https://github.com/ROCm/rocm-libraries/pull/15266#pullrequestreview-5879596049)

> I thought that in general int8 GEMM worked like
>
> Accumulate like int32_t(a_i) * int32_t(b_i)
> Cast accumulation to float
> perform epilogue (including alpha, beta scaling)
> cast back to int32_t
>
> i.e. that alpha, beta were generally float. But maybe that's incorrect?
>
> I think a paragraph with pseudocode with explicit casting of how the int gemms work would be useful, also to understand what the different template variable types are here

##### Alex-Vasile — comment 4566293308

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15266#discussion_r4566293308) · [@Alex-Vasile](https://github.com/Alex-Vasile) · Created: 2026-11-13 17:06:52 UTC

[Reply to comment 4554390239](https://github.com/ROCm/rocm-libraries/pull/15266#discussion_r4554390239) · [Part of review 5895076688](https://github.com/ROCm/rocm-libraries/pull/15266#pullrequestreview-5895076688)

> Looking into your comment prompted a new issue: #15764 tracks an existing bug in the slow CPU reference.
>
> Your integer-dot-product → float conversion → float epilogue ordering is correct for the float-alpha Int8 modes. The slow reference instead accumulates in float, losing individual terms at 2^24. With 1024 products of (-128)*(-128), two trailing 1*1 products, and alpha=0.5, it returns 8,388,608 instead of 8,388,609.
>
> Draft PR #15765 adds regression tests and separates the integer reduction from the float epilogue. The tests reproduce the failure before the fix and pass afterward. I’ve requested your review and kept the PR as a draft.
>
> This fixes the reference behavior; your request for explicit-cast pseudocode in this driver remains relevant.
>

#### Thread 2

`projects/hipblaslt/tensilelite/tests/CMakeLists.txt` · Current line: 736 · Original line: 736 · Side: RIGHT

Status: Unresolved · Current diff.

##### tony-davis — comment 4568212035

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15266#discussion_r4568212035) · [@tony-davis](https://github.com/tony-davis) · Created: 2026-11-13 20:21:13 UTC

[Part of review 5897488069](https://github.com/ROCm/rocm-libraries/pull/15266#pullrequestreview-5897488069)

> (non-blocking) Now that this is a tracked bug, I'd point at it here. That way whoever lands #15765 knows the K = 151 limit can go, and a large-K slow-path case could come back.
>
> ```suggestion
> # element, which is exact only while |sum| < 2^24 (#15764). Keep K small enough:
> ```

#### Thread 3

`projects/hipblaslt/tensilelite/tests/CMakeLists.txt` · Current line: 797 · Original line: 797 · Side: RIGHT

Status: Unresolved · Current diff.

##### tony-davis — comment 4568212077

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15266#discussion_r4568212077) · [@tony-davis](https://github.com/tony-davis) · Created: 2026-11-13 20:21:13 UTC

[Part of review 5897488069](https://github.com/ROCm/rocm-libraries/pull/15266#pullrequestreview-5897488069)

> (non-blocking) `WILL_FAIL` passes on any non-zero exit, so I think all four of these would also pass on the base driver, where `--typeCD` and `i8` are just unknown options. Adding the expected error text as a third field and using `PASS_REGULAR_EXPRESSION` would show each case is rejected for the reason in its name. It's the file's existing pattern, so I'm happy to leave it for a follow-up.

### Newling’s linked reviews and cross-PR remarks

Posted on #15259: [initial round](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5867396457) · [final round](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5899752492). The [shared testing and design assessment](#shared-assessment) applies to this PR too.

**Newling’s own remark about #15266** ([original post](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5899752492)):

> #15266 has a easy float->double fix, might as well just take

#### Initial agent review, linked November 12

[Original on GitHub](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15266.md) · [Newling’s source post](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5867396457)

Source revision: `30df5bab4294f839ef85a91c732c36c92c0821ca`. File: `rocm-libraries/review_pr15266.md`.

> This is a review from an agent with an automatic prompt from the reviewer
>
> **PR reviewed:** [ROCm/rocm-libraries#15266](https://github.com/ROCm/rocm-libraries/pull/15266)
>
> **Submitted head:** `4554dab796dee28e4e640b7f688c4d0f12d5b375`. **Incremental base:** `571ecae3fc4814ff951461d1d14e48f780975511`.
>
> Independent review of the incremental change and its interaction with the CPU-reference stack, emphasizing test adequacy, runtime, overlap, design reuse, and unsupported cases. Inputs were the public descriptions, code, repository guidance and CI status/logs; target-PR discussions and existing reviews were not read.
>
> **Suggestion branch:** [review/pr15266-suggestions](https://github.com/newling/rocm-libraries/tree/review/pr15266-suggestions), based on the submitted head above; final head [088d585c895cff36e459ee0409c1b812edd9da81](https://github.com/newling/rocm-libraries/commit/088d585c895cff36e459ee0409c1b812edd9da81).
>
> ## Tests
>
> Submitted head: `cpu-gemm-driver`, `tensilelite-tests`, and `tensilelite-client` built successfully; **361 driver CTests and 24 focused reference gtests passed**. The driver suite took 2.29 s with four CTest workers. Suggestion implementation: the same targets built; **362 driver CTests and 24 focused reference gtests passed**. Later comment-only commits, where present, do not change that execution result.
>
> Validation used a Release ROCm compiler build. Controlled runs set `OMP_THREAD_LIMIT=8`, `OMP_NUM_THREADS=4`, and `KMP_BLOCKTIME=0`; the original stack-tip driver timing predates the thread-limit setting. These are host numerical tests, not GPU-kernel performance measurements. Root/component hook invocations and full-stack whitespace checks were clean; the formatting hooks opt out of these C++/CMake paths, and no Python tests were selected by the component hook. No local GPU or sanitizer run is claimed.
>
> The gtests were run directly with `--gtest_filter=ReferenceFastPath.*:ReferenceMX*.*:*ReferenceMX*/*`. In the category-based build, a CTest name regex containing `ReferenceFastPath` does not select them. Driver CTests are build-tree tests and are not installed into the TheRock test package; Math CI's unfiltered `coverage-cpp` lane runs them but is informational. The gtest categories include standard/comprehensive/full, not quick.
>
> ## Summary
>
> Separating input storage, output storage, and scalar/epilogue types is necessary to test the six existing Int8 reference combinations. Integer golden accumulation with an explicit int32-range check and exact output comparison is a useful oracle. This PR establishes the slow-path baseline; the next PR's corresponding fast tests exercise a different implementation, so those pairs are justified.
>
> The 64 added driver cases are inexpensive and cover dispatch, transpose, scalar types, epilogue operations and batching. They miss an important scalar boundary and currently check rejected options only through a nonzero exit status.
>
> ## Actionable items
>
> ### Preserve the full advertised Int32 scalar range during CLI parsing
>
> At [client/cpu_gemm_driver.cpp:1145](https://github.com/ROCm/rocm-libraries/blob/4554dab796dee28e4e640b7f688c4d0f12d5b375/projects/hipblaslt/tensilelite/client/cpu_gemm_driver.cpp#L1145) and [client/cpu_gemm_driver.cpp:1292](https://github.com/ROCm/rocm-libraries/blob/4554dab796dee28e4e640b7f688c4d0f12d5b375/projects/hipblaslt/tensilelite/client/cpu_gemm_driver.cpp#L1292), alpha and beta are parsed as `float` before checking whether they fit Int32. `2147483647` rounds to `2147483648` and is rejected even though it is valid.
>
> Minimal driver reproduction:
>
> ```sh
> "$BUILD_DIR/tensilelite/client/cpu-gemm-driver" -M 1 -N 1 -K 0 \
>   --type i8 --typeCD i32 --typeAlpha i32 --alpha 0 --beta 2147483647 --validate
> ```
>
> Suggestion commit [2fcafc2eee3](https://github.com/newling/rocm-libraries/commit/2fcafc2eee3f322abd349beeda112823931c8591) carries parsed scalars as double until conversion to the selected scalar type and adds this boundary case. It passes without weakening integer-domain validation.
>
> ## Suggestions
>
> ### Use the existing type metadata
>
> [client/cpu_gemm_driver.cpp:80](https://github.com/ROCm/rocm-libraries/blob/4554dab796dee28e4e640b7f688c4d0f12d5b375/projects/hipblaslt/tensilelite/client/cpu_gemm_driver.cpp#L80) duplicates the type-to-enum mapping in private `TypeTraits` specializations. `TensileLite::TypeInfo<T>::Enum` already owns this mapping, including packed Float4. Suggestion commit [088d585c895](https://github.com/newling/rocm-libraries/commit/088d585c895cff36e459ee0409c1b812edd9da81) replaces the private table. Later stack commits that add `TypeTraits` call sites should use `TypeInfo` when integrating this change.
>
> ### Require the intended diagnostic in rejected-option tests
>
> [tests/CMakeLists.txt:784](https://github.com/ROCm/rocm-libraries/blob/4554dab796dee28e4e640b7f688c4d0f12d5b375/projects/hipblaslt/tensilelite/tests/CMakeLists.txt#L784) uses `WILL_FAIL`; an unrelated runtime error also satisfies the test. The shared correction is implemented once at the stack tip in #15278's suggestion branch ([66ef187d435](https://github.com/newling/rocm-libraries/commit/66ef187d43535cacaba49a328b77b0388909291e)). It requires exit code 1 and the intended diagnostic, including for this PR's cases. Keep one helper instead of adding a separate failure runner for each PR.
>
> ## Scope decisions
>
> The six-type dispatch matrix is useful coverage and should remain. The existing full-range Int8-to-Int8 cases often saturate; #15274's narrow-range cases complement them by making incorrect in-range arithmetic observable. That is a useful extension rather than duplicate coverage.
>
> ## Suggestion branch handoff
>
> | Suggestion commit | Item |
> | --- | --- |
> | [2fcafc2eee3](https://github.com/newling/rocm-libraries/commit/2fcafc2eee3f322abd349beeda112823931c8591) | preserve int32 scalar values in cpu-gemm-driver |
> | [088d585c895](https://github.com/newling/rocm-libraries/commit/088d585c895cff36e459ee0409c1b812edd9da81) | use TypeInfo in the CPU GEMM driver |
>
> The suggestion branches are published. Each branch contains its own corrections on its own submitted head, not the other review branches' fixes. Integrate shared changes in stack order, including the alias fix once and later `TypeInfo` call sites. PR-description corrections are reported for the author to make; none were published. See [the cross-PR assessment](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_cpu_reference_stack_15259_15278.md) for test overlap, CI limits and integration notes.
>

#### Final correctness review, linked November 14 (document dated November 13)

[Original on GitHub](https://github.com/newling/agent-public-reviews/blob/ede5c5f065835ae571ce0647b4e07af2c6cb8d64/rocm-libraries/review_pr15266_correctness_20261113.md) · [Newling’s source post](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5899752492)

Source revision: `ede5c5f065835ae571ce0647b4e07af2c6cb8d64`. File: `rocm-libraries/review_pr15266_correctness_20261113.md`.

> This is a review from an agent with an automatic prompt from the reviewer
>
> **PR reviewed:** [ROCm/rocm-libraries#15266](https://github.com/ROCm/rocm-libraries/pull/15266)
>
> **Submitted head:** `4554dab796dee28e4e640b7f688c4d0f12d5b375`. **Incremental diff base:** `571ecae3fc4814ff951461d1d14e48f780975511`.
>
> **Scope:** Focused follow-up on consequential CPU-reference errors and regression-test failures, including the existing review discussion. Head and discussion were unchanged at the 2026-11-13 23:09 UTC check. Earlier validation is reused where the tested code is unchanged.
>
> **Suggestion branch:** [review/pr15266-correctness-20261113](https://github.com/newling/rocm-libraries/tree/review/pr15266-correctness-20261113), based on the submitted head; final head [2fcafc2eee3](https://github.com/newling/rocm-libraries/commit/2fcafc2eee3f322abd349beeda112823931c8591). Commit-to-item mapping is given below.
>
> ## Tests
>
> Reused validation for the unchanged code: submitted head, 361 driver CTests and 24 reference tests passed; scalar-fix commit, 362 driver CTests and 24 reference tests passed. The driver, tests and client built in both cases. The boundary command below failed on the submitted implementation and passed with the fix. No GPU execution is needed for this CLI parsing defect.
>
> ## Summary
>
> The driver can now choose Int8 input storage, C/D storage, and alpha/beta types separately. Its integer oracle is useful for checking the supported combinations. The remaining consequential issue is that the command line can lose the requested integer scalar before either the reference or the oracle sees it.
>
> ## Actionable items
>
> ### Preserve Int32 scalars before validating or using them
>
> [client/cpu_gemm_driver.cpp:1145](https://github.com/ROCm/rocm-libraries/blob/4554dab796dee28e4e640b7f688c4d0f12d5b375/projects/hipblaslt/tensilelite/client/cpu_gemm_driver.cpp#L1145) parses alpha and beta as float, and [client/cpu_gemm_driver.cpp:1292](https://github.com/ROCm/rocm-libraries/blob/4554dab796dee28e4e640b7f688c4d0f12d5b375/projects/hipblaslt/tensilelite/client/cpu_gemm_driver.cpp#L1292) checks the already-rounded values. For example, valid Int32 beta `2147483647` becomes `2147483648` and is rejected. Other integers above `2^24` can silently become a neighboring integer; both the calculation and its oracle then receive the changed value, so validation cannot detect the parsing error.
>
> ```sh
> "$BUILD_DIR/tensilelite/client/cpu-gemm-driver" -M 1 -N 1 -K 0 \
>   --type i8 --typeCD i32 --typeAlpha i32 --alpha 0 --beta 2147483647 --validate
> ```
>
> Carry parsed scalars in a type that represents the Int32 range exactly until converting to the selected scalar type. Fix [2fcafc2eee3](https://github.com/newling/rocm-libraries/commit/2fcafc2eee3f322abd349beeda112823931c8591) uses double and adds the boundary regression. This is a defect in test-driver input handling; it does not change GPU arithmetic.
>
> ## Suggestions
>
> None in this focused review.
>
> ## Scope decisions
>
> The legacy float-alpha Int8 reduction loses precision above `2^24`; that separate numerical issue is tracked by [#15764](https://github.com/ROCm/rocm-libraries/issues/15764) and [#15765](https://github.com/ROCm/rocm-libraries/pull/15765). The latter remains outside this stack. It does not address the CLI scalar conversion above.
>
> **Suggestion summary:** [review/pr15266-correctness-20261113](https://github.com/newling/rocm-libraries/tree/review/pr15266-correctness-20261113) contains only [2fcafc2eee3](https://github.com/newling/rocm-libraries/commit/2fcafc2eee3f322abd349beeda112823931c8591) for scalar parsing and its regression. The broader naming and diagnostic changes from earlier reviews are omitted from this review.
>

### General discussion comments (3, including bots)

#### therock-pr-bot[bot] — comment 6692557003

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15266#issuecomment-6692557003) · [@therock-pr-bot[bot]](https://github.com/apps/therock-pr-bot) · Created: 2026-11-06 18:40:11 UTC · Updated: 2026-11-10 22:31:50 UTC

> <!-- therock-pr-bot-policy-check -->
> ### ❌ PR Check — Action Required
>
> | Check | Status | Details |
> |---|:---:|---|
> | 📝 **PR Description** | ✅ Pass | — |
> | ⛔ **Forbidden Files** | ✅ Pass | — |
> | 🧪 **Unit Test** | ⚠️ Warning | **Error:** Source/code files changed without an accompanying unit test.<br>**Expected:** add at least one test file named like `test_<name>.py` / `test_<name>.cpp` (or `<name>_test.*`).<br>**Current:** code file(s) changed: `projects/hipblaslt/tensilelite/client/cpu_gemm_driver.cpp`; no test file found |
> | 🔎 **pre-commit** | ❌ Fail | **Error:** Check concluded with `cancelled`. |
> | 🚫 **Draft PR** | 🔜 To Be Enabled | — |
> | 🚩 **Feature Flag** | 🔜 To Be Enabled | — |
> | 📊 **Code Coverage** | 🔜 To Be Enabled | — |
>
> > ⚠️ **1 policy check(s) failed.** Please address the issues above before this PR can be Reviewed.
> >
> > 🚫 **Please fix the failed policies**
> > - ❌ pre-commit
> >
> > The **`Not ready to Review`** label was added to this PR. Once all policies pass, the label is removed automatically.
>
> 📖 **Need help?** See the [Policy FAQ](https://github.com/ROCm/rocm-libraries/blob/develop/docs/LIBRARIES_PR_BOT_FAQ.md) for details on every check and how to fix failures.
>
> 🙋 **[Wish to Override Policy?](https://github.com/ROCm/rocm-libraries/blob/develop/docs/LIBRARIES_PR_BOT_FAQ.md#-wish-to-override-the-policy-process-and-get-unblocked)**

#### therock-pr-bot[bot] — comment 6692557969

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15266#issuecomment-6692557969) · [@therock-pr-bot[bot]](https://github.com/apps/therock-pr-bot) · Created: 2026-11-06 18:40:14 UTC · Updated: 2026-11-09 21:35:17 UTC

> <!-- therock-pr-bot-fix-policies -->
> 🎉 All checks passed! This PR is ready for review.

#### codecov[bot] — comment 6787993699

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15266#issuecomment-6787993699) · [@codecov[bot]](https://github.com/apps/codecov) · Created: 2026-11-11 05:33:49 UTC · Updated: 2026-11-11 07:32:09 UTC

> ## [Codecov](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15266?dropdown=coverage&src=pr&el=h1&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) Report
> :white_check_mark: All modified and coverable lines are covered by tests.
>
> <details><summary>Additional details and impacted files</summary>
>
>
>
> ```diff
> @@                                     Coverage Diff                                     @@
> ##           users/alvasile/tl-speedup-09-reference-slow-path-amax-d   #15266      +/-   ##
> ===========================================================================================
> + Coverage                                                    77.60%   77.61%   +0.01%
> ===========================================================================================
>   Files                                                         3002     3002
>   Lines                                                       521646   521646
>   Branches                                                     81316    81316
> ===========================================================================================
> + Hits                                                        404812   404852      +40
> + Misses                                                       88982    88974       -8
> + Partials                                                     27852    27820      -32
> ```
>
> | [Flag](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15266/flags?src=pr&el=flags&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | Coverage Δ | | *Carryforward flag |
> |---|---|---|---|
> | [TensileLite-CPP](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15266/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `53.02% <ø> (+0.12%)` | :arrow_up: | |
> | [TensileLite-Unit](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15266/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `83.84% <ø> (ø)` | | |
> | [hipBLAS](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15266/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `90.62% <ø> (ø)` | | Carriedforward from [571ecae](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/571ecae3fc4814ff951461d1d14e48f780975511?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipBLASLt](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15266/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `70.48% <ø> (+0.01%)` | :arrow_up: | |
> | [hipCUB](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15266/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `82.68% <ø> (ø)` | | Carriedforward from [571ecae](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/571ecae3fc4814ff951461d1d14e48f780975511?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipDNN](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15266/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `87.34% <ø> (ø)` | | Carriedforward from [571ecae](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/571ecae3fc4814ff951461d1d14e48f780975511?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipFFT](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15266/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `42.65% <ø> (ø)` | | Carriedforward from [571ecae](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/571ecae3fc4814ff951461d1d14e48f780975511?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipRAND](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15266/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `76.12% <ø> (ø)` | | Carriedforward from [571ecae](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/571ecae3fc4814ff951461d1d14e48f780975511?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipSOLVER](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15266/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `69.67% <ø> (ø)` | | Carriedforward from [571ecae](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/571ecae3fc4814ff951461d1d14e48f780975511?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipSPARSE](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15266/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `87.03% <ø> (ø)` | | Carriedforward from [571ecae](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/571ecae3fc4814ff951461d1d14e48f780975511?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocALUTION](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15266/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `86.47% <ø> (ø)` | | Carriedforward from [571ecae](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/571ecae3fc4814ff951461d1d14e48f780975511?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocBLAS](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15266/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `69.16% <ø> (ø)` | | Carriedforward from [571ecae](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/571ecae3fc4814ff951461d1d14e48f780975511?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocFFT](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15266/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `48.81% <ø> (ø)` | | Carriedforward from [571ecae](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/571ecae3fc4814ff951461d1d14e48f780975511?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocRAND](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15266/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `57.32% <ø> (ø)` | | Carriedforward from [571ecae](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/571ecae3fc4814ff951461d1d14e48f780975511?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocSOLVER](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15266/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `76.66% <ø> (ø)` | | Carriedforward from [571ecae](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/571ecae3fc4814ff951461d1d14e48f780975511?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocSPARSE](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15266/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `83.86% <ø> (ø)` | | Carriedforward from [571ecae](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/571ecae3fc4814ff951461d1d14e48f780975511?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocSPARSE-unit](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15266/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `79.82% <ø> (ø)` | | Carriedforward from [571ecae](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/571ecae3fc4814ff951461d1d14e48f780975511?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocThrust](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15266/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `93.15% <ø> (ø)` | | Carriedforward from [571ecae](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/571ecae3fc4814ff951461d1d14e48f780975511?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
>
> *This pull request uses carry forward flags. [Click here](https://docs.codecov.io/docs/carryforward-flags?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) to find out more.
> [see 9 files with indirect coverage changes](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15266/indirect-changes?src=pr&el=tree-more&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm)
> </details>
> <details><summary> :rocket: New features to boost your workflow: </summary>
>
> - :snowflake: [Test Analytics](https://docs.codecov.com/docs/test-analytics): Detect flaky tests, report on failures, and find test suite problems.
> - :package: [JS Bundle Analysis](https://docs.codecov.com/docs/javascript-bundle-analysis): Save yourself from yourself by tracking and limiting bundle sizes in JS merges.
> </details>

<a id="pr-15274"></a>

## PR #15274 — perf(tensilelite): run Int8-input GEMMs on the CPU reference fast path

[GitHub PR](https://github.com/ROCm/rocm-libraries/pull/15274) · State: **OPEN**

Branch: `users/alvasile/tl-speedup-24-reference-fast-path-int8`. Base: `users/alvasile/tl-speedup-16-cpu-gemm-driver-int8`. Head at collection: `f58bbb3388853e13cdf85bda0917485d590c4ddc`.

### Feedback walkthrough — November 16, 2026

#### Preserve the original C when C/D share storage — implemented locally

Newling's [initial review](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15274.md#read-the-original-c-before-writing-an-aliased-d) and final correctness review identify an intermediate-write bug. The published head remains `f58bbb3388853e13cdf85bda0917485d590c4ddc`, with the tile-to-D copy preceding the C read at [Reference.cpp:1816](https://github.com/ROCm/rocm-libraries/blob/f58bbb3388853e13cdf85bda0917485d590c4ddc/projects/hipblaslt/tensilelite/client/src/Reference.cpp#L1816). For Int32 C/D with an Int32 epilogue, the shadow C buffer can reference C directly. If C and D share storage, writing the dot product destroys the original beta input.

**Agreed implementation:** adapted [suggestion 1024648658c](https://github.com/newling/rocm-libraries/commit/1024648658c17b9f3dfafb3aa41929817081c84f). Removed the intermediate tile-to-D copy, read the dot product directly from `cReg` during the epilogue, preserve the original C and write only the final D. Added `Int8InPlaceCDPreservesBetaInput` with A=2, B=3, initial C=D=5 and alpha=beta=1. Its independent expected result is 11. Apply the common production correction once in #15274 and retain #15276's later complex regression when integrating the stack.

**Validation:** the new regression was built against the unpatched local branch (`4201f388e99`) and failed with actual result 12 versus expected 11. After the fix, Release builds of `cpu-gemm-driver` and `tensilelite-tests` succeeded; all 31 focused fast-path/MX gtests and all 468 standard driver CTests passed, with no failures or skips. Both suites also passed from a relocated installation with `LD_LIBRARY_PATH` unset and GPU visibility disabled. The new gtest is covered by the existing standard-tier wildcard registration used by the required installed CTest lane. JUnit/gtest XML reports and build logs are saved under `/tmp/tl-pr15274-alias-fix-20261116/`. Scoped root/component hooks and the staged whitespace check passed; the C++ formatting hook excludes these paths, and the component affected-test hook selected no Python tests. This branch already includes #15259's installed-suite CI integration, but its parent remains `3c54ff8ac10`; the later #15266 fixes have not been restacked into it. The current archive preserves those notes. The aliasing fix remains local.

**Reply posted to newling** ([GitHub](https://github.com/ROCm/rocm-libraries/pull/15274#issuecomment-6927414612); exact body reread and verified):

> @newling Agreed — I’ll take your aliasing fix from 1024648658c and add the 1×1 regression with expected result 11. The epilogue will read the tile sum from cReg and preserve the original C until the final D write. I’ll carry the shared correction once from #15274 and retain #15276’s complex regression when integrating the stack.

The requested #15765 description follow-up is recorded in [Future work](#future-work).

#### Define wrapping for overflowing Int8 dot products — implemented locally

[Tony's dispatch comment](https://github.com/ROCm/rocm-libraries/pull/15274#discussion_r4568212559) and [newling's suggestion a20ddeb0e6f](https://github.com/newling/rocm-libraries/commit/a20ddeb0e6f2e7b9e0af960d8b96730f8b3441f4) identify signed overflow in the Int8 K reduction. A 1×1×140,000 problem with A=B=-128 has exact dot product 2,293,760,000, exceeding INT32_MAX. The unpatched `innerFastPathReduction` adds into signed `int32_t`, which makes overflowing sums undefined in C++.

**Agreed implementation:** adapted newling's unsigned multiply-add approach inside the existing reduction. Int32 MACs use `uint32_t` arithmetic modulo `2^32`, and `memcpy` preserves the signed result bits for the epilogue. Other MAC types retain their existing arithmetic. Added `Int8DotProductWrapsAtInt32Boundary` for both overflow directions and both Int32 and float alpha types, with independently derived expected signed results -2,001,207,296 and 2,019,127,296. The earlier alias fix keeps the epilogue conversion directly from `cReg` and preserves the original C input. Inherited integer epilogue arithmetic is outside this dot-product correction.

**Related wording feedback addressed:** the solver/dispatch comments describe modulo `2^32` reduction and the separate epilogue. The CMake comment now states the wrapping policy, addressing [Tony's “exact at any K” nit](https://github.com/ROCm/rocm-libraries/pull/15274#discussion_r4568212578) and newling's earlier [3b524a30398](https://github.com/newling/rocm-libraries/commit/3b524a30398c6e136a3d049dc78fae2053ce3d97)/[8886ad2d584](https://github.com/newling/rocm-libraries/commit/8886ad2d584b154fc7d986ce49f7b20e56661d7d) boundary corrections. The temporary float slow-path precision caveat remains until #15765 is integrated.

**Validation:** the value regression passes on the ordinary optimized unpatched build (`a581be344872`), where this compiler happens to wrap. Signed-overflow instrumentation of that same `Reference.cpp` nevertheless reproduces `2147467264 + 16384 cannot be represented in type 'int'` at line 1421. The instrumentation uses newling's linked sanitizer recipe, adapted to the saved local source snapshot; it covers only `Reference.cpp`, with the remaining objects uninstrumented. The instrumented fixed reference passes the same regression, including both overflow directions and both scalar types. Release builds of `cpu-gemm-driver` and `tensilelite-tests` succeeded; all 32 focused fast-path/MX gtests and all 468 standard driver CTests passed, with no failures or skips. Both suites also passed from a relocated installation with `LD_LIBRARY_PATH` unset and GPU visibility disabled. The new regression runs under the existing standard-tier gtest wildcard used by the required installed CTest lane. Scoped root/component hooks and the staged whitespace check passed; C++ formatting/CMake lint hooks exclude these paths, and the component affected-test hook selected no Python tests. Evidence is saved under `/tmp/tl-pr15274-int8-overflow-20261116/`.

**Reply posted to Tony** ([GitHub](https://github.com/ROCm/rocm-libraries/pull/15274#discussion_r4590824622); exact body and parent thread reread and verified):

> Agreed. I’ll take newling’s unsigned multiply-add approach, preserving the wrapped Int32 result for the epilogue, and add K = 140,000 regressions for both overflow directions and Int32/float alpha. I’ll also correct the “exact at any K” wording and record the #15765 description follow-up. The earlier alias fix removed the intermediate D copy, so the epilogue now reads directly from cReg.

**Reply posted on the related wording thread** ([GitHub](https://github.com/ROCm/rocm-libraries/pull/15274#discussion_r4590866584); exact body and parent thread reread and verified):

> Updated locally with the overflow fix: the comment now says the fast path sums modulo 2^32 with a signed Int32 result.

The requested #15765 follow-up records describing its own contents and the separately owned downstream integration updates; see [Future work](#future-work). The code remains local.

#### Extend the large-sum regression to the slow path after #15765 — agreed integration follow-up

[Tony's comment](https://github.com/ROCm/rocm-libraries/pull/15274#discussion_r4568212613) asks to strengthen `Int8FloatEpilogueStartsFromExactSum` once #15765 is below this branch. The 1×1×1044 test currently checks the fast result against the independently derived dot product 16,790,292 and asserts that per-K float summation loses precision for the chosen inputs. The slow path still uses that per-K float reduction here, so a correct slow-result assertion would currently fail.

**Agreed resolution:** record the test extension under #15765's downstream integration work, with the edit committed on #15274 after it inherits the corrected slow reference. Run the case through `solveInt8FastAndSlow<int32_t>` and assert both results equal the independent `exactSum`, retaining the old-float-reduction premise. The user also confirmed that #15266's K = 151/description updates remain owned by #15266 after its restack. Both tasks are recorded in [Future work](#future-work). #15765 has not been inserted or restacked in this step, so the source changes remain pending.

**Reply posted to Tony** ([GitHub](https://github.com/ROCm/rocm-libraries/pull/15274#discussion_r4591042132); exact body and parent thread reread and verified):

> Agreed. Once #15274 is restacked onto #15765, I’ll update this test on #15274 to use solveInt8FastAndSlow<int32_t> and check both results against exactSum. I’ll retain the independent expected value and the assertion showing the old float-per-K precision loss. I’ve recorded this as a downstream integration follow-up.


#### Link the slow-reference bug and dependent fix in the PR description — published

[Tony's main review](https://github.com/ROCm/rocm-libraries/pull/15274#pullrequestreview-5897488728) asks to link #15764/#15765 and explain how they relate. Before the update, Related listed only AIHPBLAS-5257 and the body contained neither issue/PR link.

**Agreed implementation:** at the user's request, published the final relationship wording immediately, without pending-insertion wording. Related now links #15764 as the slow-reference precision bug, #15765 as the corresponding integer-reduction fix, and #15266 as the preceding driver coverage. Replaced the temporary large-sum fast/slow divergence paragraph, described the K loop as an Int32 reduction separate from the epilogue, and removed the stale large-K value-change clause from Risk level. The separately approved Test Result update is recorded below.

**Published behavior paragraph:**

> Behavior change: eligible Int8 problems are validated by the blocked CPU fast path. Integer dot-product accumulation is separate from the Int32 or float epilogue, which retains the reference’s operation order and saturating output conversion. [#15765](https://github.com/ROCm/rocm-libraries/pull/15765) supplies the corresponding integer reduction for the slow path’s float-alpha Int8 cases, fixing the precision loss tracked in [#15764](https://github.com/ROCm/rocm-libraries/issues/15764).

**Verification:** re-read the live body and asserted exact equality with the prepared Markdown after a guarded REST PATCH. Confirmed unchanged code head `f58bbb3388853e13cdf85bda0917485d590c4ddc` and base `users/alvasile/tl-speedup-16-cpu-gemm-driver-int8`; all three added links resolve. Before/after snapshots are saved under `/tmp/tl-pr15274-final-description-20261116/`. The source-comment and test follow-ups remain owned by the descendants in [Future work](#future-work).

#### Update Test Result with verified branch-specific evidence — published

[Tony's main review](https://github.com/ROCm/rocm-libraries/pull/15274#pullrequestreview-5897488728) also asks to replace the stale claims that this split branch has not been built/tested and automated checks are pending. The user approved the update, which is now published.

**Agreed implementation:** replaced those claims with links to the successful [Multi-Arch CI Summary](https://github.com/ROCm/rocm-libraries/actions/runs/42914666073/job/130346019278), [Component CI Summary](https://github.com/ROCm/rocm-libraries/actions/runs/42914663937/job/130326273026) and [Math CI Summary](https://math-ci.amd.com/job/rocm-libraries/job/status-gate/job/hipblaslt/job/PR-15274/lastBuild/stages/) for published head `f58bbb3388853e13cdf85bda0917485d590c4ddc`. Independently verified that the two GitHub runs completed successfully at that exact head; the Math CI context reports success on the same head. The body reports those specific results without inferring per-case CI counts from summaries.

Separately identified the locally committed review fixes at `38ab457d2794`: Release driver/gtest builds, 32 focused fast-path/MX gtests and 468 driver CTests passing in both the build tree and relocated installation, plus the targeted signed-overflow instrumentation failing before the fix and passing afterward. The instrumentation covers only `Reference.cpp`. The JUnit/gtest XML reports were reread to confirm counts and absence of failures/skips. Kept the existing original-branch benchmark measurements and test/mutation results identified as historical evidence. The review code has not been pushed.

**Verification:** a fresh body/head/base read matched the saved snapshot before the guarded REST PATCH. The subsequent read matched the prepared body exactly and confirmed unchanged published code head `f58bbb3388853e13cdf85bda0917485d590c4ddc` and base `users/alvasile/tl-speedup-16-cpu-gemm-driver-int8`. Only Test Result changed. Before/after snapshots and the exact body are saved under `/tmp/tl-pr15274-test-result-update-20261116/`.

#### Select the fast-path gtests explicitly in Test Plan — published

Newling's [linked initial review](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15274.md#tests) notes that a CTest name regex containing `ReferenceFastPath` does not select these gtests in the category-based build. The previous Test Plan claimed that `-R 'CPUGemm\..*I8|ReferenceFastPath'` ran both the Int8 driver cases and fast-path gtests.

**Verified locally:** CTest's JSON inventory selects 141 driver entries with that regex and zero gtest suites. The gtests are registered as `tensilelite-tests_standard_suite`, `tensilelite-tests_comprehensive_suite` and `tensilelite-tests_full_suite`; none matches the regex. The inventory is saved as `/tmp/tl-pr15274-test-result-update-20261116/ctest-inventory.json`.

**Agreed implementation:** published the corrected manual Test Plan. It now selects the Int8 driver CTests with `--no-tests=error`, then invokes `tensilelite-tests` directly using the validated filter `ReferenceFastPath.*:ReferenceMX*.*:*ReferenceMX*/*`. It sets `OMP_THREAD_LIMIT=8`, `OMP_NUM_THREADS=4` and `KMP_BLOCKTIME=0` for these runs. This selects the intended gtests regardless of whether CTest uses category suites or individual discovery. The required installed CI integration is already present locally from #15259; this item corrects the documented manual command.

**Verification:** guarded the REST PATCH against a fresh body/head/base read, then re-read the published body and asserted exact equality with the prepared Markdown. Published code head `f58bbb3388853e13cdf85bda0917485d590c4ddc` and base `users/alvasile/tl-speedup-16-cpu-gemm-driver-int8` are unchanged. Only Test Plan changed, and its shell block passes `bash -n`. Exact text and before/after snapshots are saved under `/tmp/tl-pr15274-test-plan-update-20261116/`.

**Walkthrough status:** all feedback items for #15274 now have an agreed resolution. The code fixes remain local, and the source-comment/test changes after #15765 integration remain recorded in [Future work](#future-work). A fresh read of GitHub reviews, inline comments and discussion comments found no additional human feedback. Continue with #15275 below.

### Reviews (1)

#### tony-davis — APPROVED — review 5897488728

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15274#pullrequestreview-5897488728) · [@tony-davis](https://github.com/tony-davis) · Submitted: 2026-11-13 20:21:15 UTC

Reviewed commit: `f58bbb3388853e13cdf85bda0917485d590c4ddc`.

> Hi Alex! This one reads really well. I like that the Int8 fast path doesn't try to be clever about the epilogue. It follows the slow path's order step by step and reuses the slow path's own `SaturateCast`, `Activation` and `GetValue`, so most of the agreement comes by construction. Gating on the exact `TypedGemm_I8_*` ids (which include the compute-input types) is the right check. And the driver throwing when `--tryFastPath` isn't eligible means the `fast_*` cases can't quietly fall back to the slow path. `Int8FloatEpilogueFollowsSlowPathOrder` asserting its own premise is a nice touch. That's a test that would catch someone "simplifying" the epilogue later.
>
> I'm approving, with two things I'd like to see before this merges:
>
> 1. Please link #15764 and #15765, and say how they fit. This PR deliberately departs from the slow path for float-alpha Int8 once partial sums pass 2^24, which is exactly the bug in #15764. Until #15765 lands, the expected value for a large-K `I8_*_S` problem depends on which path it takes. Sparse validation, an ineligible feature or `tryFastPath` off all still go to the slow path. Once it lands, the paths agree again, and a comment and a test here can tighten (inline).
> 2. The int32 overflow case (inline on the dispatch). #15765 sums in `uint32_t` so wraparound is defined. This path sums in `int32_t`, where overflow is undefined behavior. I bet both wrap in practice, but I'd rather the two paths agree there by construction.
>
> Also non-blocking: Test Result still says the split branch hasn't been built or tested. The required checks are green at f58bbb3. Math CI's `tensilelite-unit-codecov` (which runs the CTest suite) and `preliminary` (the GPU common tests against this reference) both passed. Worth saying so.
>
> This sits on #15266 and #15259, so it waits on those.
>
> <!-- rocm-pr-quality:attestation v1 -->
> > **Reviewed with the ROCm PR-quality skill.**
> > - Skill: `rocm-pr-quality` (base) - overlays: `hipblaslt-pr-quality`
> > - Action: `review` - Result: **APPROVED** (`0` BLOCKING; `2` IMPORTANT)
> > - Risk: `3`
> > - Reviewed commit: `f58bbb3388853e13cdf85bda0917485d590c4ddc`
> > - Attestation: `01650863`
> >
> > <sub>Machine-checkable attestation. The digest ties this review to this PR; later commits are
> > allowed. The reviewed commit is informational (what the review looked at).</sub>

### Inline review threads (3 threads, 3 comments)

#### Thread 1

`projects/hipblaslt/tensilelite/client/src/Reference.cpp` · Current line: 3491 · Original line: 3491 · Side: RIGHT

Status: Unresolved · Current diff.

##### tony-davis — comment 4568212559

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15274#discussion_r4568212559) · [@tony-davis](https://github.com/tony-davis) · Created: 2026-11-13 20:21:15 UTC

[Part of review 5897488728](https://github.com/ROCm/rocm-libraries/pull/15274#pullrequestreview-5897488728)

> Both Int8 instantiations sum the K loop in `int32_t`. With Int8 data that only overflows past roughly K = 131,072 at the extremes (128 * 128 * K), so it's unlikely in today's configs. But signed overflow is undefined behavior in C++, and #15765 makes the slow path's float-alpha sum wrap through `uint32_t` on purpose, since that's what the GPU's 32-bit accumulator does.
>
> I'd use the same approach here: `MacT = uint32_t` for the Int8 path, and convert the finished sum through `int32_t` at the "Copy from cReg back" step. Then the fast and slow paths agree at overflow by construction, not by compiler habit. A 1 x 1 x 140,000 gtest is cheap if you want to lock it. If you'd rather defer it, a ticket and a line in the description would be fine with me.

#### Thread 2

`projects/hipblaslt/tensilelite/tests/CMakeLists.txt` · Current line: 737 · Original line: 737 · Side: RIGHT

Status: Unresolved · Current diff.

##### tony-davis — comment 4568212578

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15274#discussion_r4568212578) · [@tony-davis](https://github.com/tony-davis) · Created: 2026-11-13 20:21:15 UTC

[Part of review 5897488728](https://github.com/ROCm/rocm-libraries/pull/15274#pullrequestreview-5897488728)

> (non-blocking) Small wording nit, tied to the overflow note on the dispatch.
>
> ```suggestion
> # (The fast path accumulates in int32_t, exact until int32 overflow; its results above 2^24
> ```

#### Thread 3

`projects/hipblaslt/tensilelite/tests/ReferenceFastPath_test.cpp` · Current line: 386 · Original line: 386 · Side: RIGHT

Status: Unresolved · Current diff.

##### tony-davis — comment 4568212613

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15274#discussion_r4568212613) · [@tony-davis](https://github.com/tony-davis) · Created: 2026-11-13 20:21:15 UTC

[Part of review 5897488728](https://github.com/ROCm/rocm-libraries/pull/15274#pullrequestreview-5897488728)

> (non-blocking) This is the one Int8 test that checks only the fast path, because the slow path gets this case wrong today (#15764). Once #15765 is under this branch, I think it can go through `solveFastAndSlow` and check `dSlow[0] == exactSum` too.

### Newling’s linked reviews and cross-PR remarks

Posted on #15259: [initial round](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5867396457) · [final round](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5899752492). The [shared testing and design assessment](#shared-assessment) applies to this PR too.

**Newling’s own remark about #15274** ([original post](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5899752492)):

> #15274 has a fix suggestion that looks like a code simplification, so might as well take too

#### Initial agent review, linked November 12

[Original on GitHub](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15274.md) · [Newling’s source post](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5867396457)

Source revision: `30df5bab4294f839ef85a91c732c36c92c0821ca`. File: `rocm-libraries/review_pr15274.md`.

> This is a review from an agent with an automatic prompt from the reviewer
>
> **PR reviewed:** [ROCm/rocm-libraries#15274](https://github.com/ROCm/rocm-libraries/pull/15274)
>
> **Submitted head:** `f58bbb3388853e13cdf85bda0917485d590c4ddc`. **Incremental base:** `4554dab796dee28e4e640b7f688c4d0f12d5b375`.
>
> Independent review of the incremental change and its interaction with the CPU-reference stack, emphasizing test adequacy, runtime, overlap, design reuse, and unsupported cases. Inputs were the public descriptions, code, repository guidance and CI status/logs; target-PR discussions and existing reviews were not read.
>
> **Suggestion branch:** [review/pr15274-suggestions](https://github.com/newling/rocm-libraries/tree/review/pr15274-suggestions), based on the submitted head above; final head [8886ad2d584b154fc7d986ce49f7b20e56661d7d](https://github.com/newling/rocm-libraries/commit/8886ad2d584b154fc7d986ce49f7b20e56661d7d).
>
> ## Tests
>
> Submitted head: `cpu-gemm-driver`, `tensilelite-tests`, and `tensilelite-client` built successfully; **440 driver CTests and 30 focused reference gtests passed**. The driver suite took 2.74 s with four CTest workers. Suggestion implementation: the same targets built; **440 driver CTests and 31 focused reference gtests passed**. Later comment-only commits, where present, do not change that execution result.
>
> Validation used a Release ROCm compiler build. Controlled runs set `OMP_THREAD_LIMIT=8`, `OMP_NUM_THREADS=4`, and `KMP_BLOCKTIME=0`; the original stack-tip driver timing predates the thread-limit setting. These are host numerical tests, not GPU-kernel performance measurements. Root/component hook invocations and full-stack whitespace checks were clean; the formatting hooks opt out of these C++/CMake paths, and no Python tests were selected by the component hook. No local GPU or sanitizer run is claimed.
>
> The gtests were run directly with `--gtest_filter=ReferenceFastPath.*:ReferenceMX*.*:*ReferenceMX*/*`. In the category-based build, a CTest name regex containing `ReferenceFastPath` does not select them. Driver CTests are build-tree tests and are not installed into the TheRock test package; Math CI's unfiltered `coverage-cpp` lane runs them but is informational. The gtest categories include standard/comprehensive/full, not quick.
>
> ## Summary
>
> Separating the integer K-loop accumulator from the epilogue type is a sound way to reuse the tiled implementation. It avoids forcing Int8 multiplication through float and retains the slow epilogue's operation order and saturating casts. The six type-ID gate keeps dispatch explicit.
>
> The 79 added driver cases complement the earlier slow baseline. The narrow-range tests are especially valuable because saturation otherwise hides many arithmetic errors. The six new gtests target rounding, large sums and edge conditions; those are worth keeping alongside the broader dispatch matrix. The missing aliasing case is a correctness gap.
>
> ## Actionable items
>
> ### Read the original C before writing an aliased D
>
> At [client/src/Reference.cpp:1816](https://github.com/ROCm/rocm-libraries/blob/f58bbb3388853e13cdf85bda0917485d590c4ddc/projects/hipblaslt/tensilelite/client/src/Reference.cpp#L1816), the tile sum is copied to D before the epilogue reads C. With Int32 C/D, the shadow buffer can refer directly to C. If C and D share storage, the copy destroys the beta input.
>
> A 1×1 Int8 GEMM with A=2, B=3, C=D initially 5 and alpha=beta=1 returns **12** on the submitted fast path instead of **11** on the slow path. The counterexample was reproduced against the submitted stack tip; this PR first admits the affected Int8 types.
>
> Suggestion commit [1024648658c](https://github.com/newling/rocm-libraries/commit/1024648658c17b9f3dfafb3aa41929817081c84f) removes the intermediate tile-to-D copy, takes the epilogue input directly from `cReg`, and writes only the final result. `Int8InPlaceCDPreservesBetaInput` is a compact regression with an independently known answer. This fixes the alias and eliminates redundant memory work.
>
> ## Suggestions
>
> ### Bound the claim of exact integer accumulation
>
> [tests/CMakeLists.txt:737](https://github.com/ROCm/rocm-libraries/blob/f58bbb3388853e13cdf85bda0917485d590c4ddc/projects/hipblaslt/tensilelite/tests/CMakeLists.txt#L737) says int32 accumulation is exact at any K. Every intermediate sum must fit in Int32, and the float slow path can lose precision once sums exceed 2^24. Suggestion commits [3b524a30398](https://github.com/newling/rocm-libraries/commit/3b524a30398c6e136a3d049dc78fae2053ce3d97) and [8886ad2d584](https://github.com/newling/rocm-libraries/commit/8886ad2d584b154fc7d986ce49f7b20e56661d7d) state those boundaries in the solver and test comments. No overflow policy is changed.
>
> ## Scope decisions
>
> The slow/fast driver pairs intentionally exercise different implementations. Do not replace the integer golden oracle with a fast-versus-slow-only comparison: above 2^24, the more precise fast result is intentionally different from the float slow result.
>
> #15276 exposes the same intermediate-write defect for complex C/D. Its suggestion contains the same production correction plus a complex regression. Apply the production change once across the stack and retain both regressions.
>
> The exact native probe source and build recipe for the numerical counterexamples are preserved in the [cross-PR appendix](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_cpu_reference_stack_15259_15278.md#appendix-native-counterexample-source-and-setup). Committed regressions cover the corresponding contracts.
>
> ## Suggestion branch handoff
>
> | Suggestion commit | Item |
> | --- | --- |
> | [1024648658c](https://github.com/newling/rocm-libraries/commit/1024648658c17b9f3dfafb3aa41929817081c84f) | apply the Int8 epilogue before writing aliased D |
> | [3b524a30398](https://github.com/newling/rocm-libraries/commit/3b524a30398c6e136a3d049dc78fae2053ce3d97) | state the limits of exact Int8 accumulation |
> | [8886ad2d584](https://github.com/newling/rocm-libraries/commit/8886ad2d584b154fc7d986ce49f7b20e56661d7d) | bound the integer reference test claim |
>
> The suggestion branches are published. Each branch contains its own corrections on its own submitted head, not the other review branches' fixes. Integrate shared changes in stack order, including the alias fix once and later `TypeInfo` call sites. PR-description corrections are reported for the author to make; none were published. See [the cross-PR assessment](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_cpu_reference_stack_15259_15278.md) for test overlap, CI limits and integration notes.
>

#### Final correctness review, linked November 14 (document dated November 13)

[Original on GitHub](https://github.com/newling/agent-public-reviews/blob/ede5c5f065835ae571ce0647b4e07af2c6cb8d64/rocm-libraries/review_pr15274_correctness_20261113.md) · [Newling’s source post](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5899752492)

Source revision: `ede5c5f065835ae571ce0647b4e07af2c6cb8d64`. File: `rocm-libraries/review_pr15274_correctness_20261113.md`.

> This is a review from an agent with an automatic prompt from the reviewer
>
> **PR reviewed:** [ROCm/rocm-libraries#15274](https://github.com/ROCm/rocm-libraries/pull/15274)
>
> **Submitted head:** `f58bbb3388853e13cdf85bda0917485d590c4ddc`. **Incremental diff base:** `4554dab796dee28e4e640b7f688c4d0f12d5b375`.
>
> **Scope:** Focused follow-up on consequential CPU-reference errors and regression-test failures, including the existing review discussion. Head and discussion were unchanged at the 2026-11-13 23:09 UTC check. Earlier validation is reused where the tested code is unchanged.
>
> **Suggestion branch:** [review/pr15274-correctness-20261113](https://github.com/newling/rocm-libraries/tree/review/pr15274-correctness-20261113), based on the submitted head; final head [a20ddeb0e6f](https://github.com/newling/rocm-libraries/commit/a20ddeb0e6f2e7b9e0af960d8b96730f8b3441f4). Commit-to-item mapping is given below.
>
> ## Tests
>
> Earlier validation at the unchanged submitted head passed 440 driver CTests and 30 reference tests. The new suggestion branch rebuilt the driver, tests and client and passed all 440 driver CTests and 32 reference tests.
>
> Fresh checks linked the same new regressions against submitted and fixed `Reference.cpp` implementations. The submitted code returned 12 in the alias test and triggered the signed-overflow sanitizer at line 1421; the fixed implementation passed both. Sanitizer instrumentation covered `Reference.cpp`, with the rest of the build uninstrumented. This is CPU correctness evidence, with no new GPU or performance comparison.
>
> ## Summary
>
> An integer dot-product accumulator avoids losing Int8 products through float rounding before the epilogue applies alpha, beta and output conversion. That separation is useful. The implementation still needs to preserve the original C input and define what happens when the dot product exceeds signed 32-bit range.
>
> ## Actionable items
>
> ### Read the original C before writing an aliased D
>
> At [client/src/Reference.cpp:1816](https://github.com/ROCm/rocm-libraries/blob/f58bbb3388853e13cdf85bda0917485d590c4ddc/projects/hipblaslt/tensilelite/client/src/Reference.cpp#L1816), the intermediate dot-product tile is copied to D before the epilogue reads C. For Int32 C/D and an Int32 epilogue, the C shadow buffer can directly reference that storage. Supplying the same buffer for C and D therefore destroys the beta input.
>
> A 1×1 GEMM with A=2, B=3, initial C=D=5 and alpha=beta=1 must give **11**. The submitted implementation gives **12**. Fix [1024648658c](https://github.com/newling/rocm-libraries/commit/1024648658c17b9f3dfafb3aa41929817081c84f) reads the dot product directly from the tile, reads C, then writes the final D. `Int8InPlaceCDPreservesBetaInput` checks the independently known answer.
>
> ### Define wrapping arithmetic for overflowing Int8 dot products
>
> [client/src/Reference.cpp:1420](https://github.com/ROCm/rocm-libraries/blob/f58bbb3388853e13cdf85bda0917485d590c4ddc/projects/hipblaslt/tensilelite/client/src/Reference.cpp#L1420) adds into signed `int32_t`. A supported 1×1×140,000 problem with A=B=-128 and alpha=1, beta=0 exceeds its range. The sanitizer reports `2147467264 + 16384 cannot be represented in type 'int'`. Ordinary compiler wraparound does not make that C++ operation defined, so the reference can depend on optimization behavior.
>
> Fix [a20ddeb0e6f](https://github.com/newling/rocm-libraries/commit/a20ddeb0e6f2e7b9e0af960d8b96730f8b3441f4) performs the multiply-add through `uint32_t` and preserves the resulting signed bits before the epilogue. This follows the wrapping approach in [HostNumerics](https://github.com/ROCm/rocm-libraries/blob/064a46fc55678f21bc71588c112df6a7836a94dc/shared/hostnumerics/src/detail/reference_arithmetic.hpp#L31) and the policy proposed in #15765. It keeps the change inside the existing reduction rather than adding another numerical backend.
>
> `Int8DotProductWrapsAtInt32Boundary` checks both overflow directions and both Int32 and float scalar types. Its independent expected results are -2,001,207,296 and 2,019,127,296. The [standalone sanitizer recipe](https://github.com/newling/agent-public-reviews/blob/ede5c5f065835ae571ce0647b4e07af2c6cb8d64/rocm-libraries/review_pr15274_overflow_reproducer_20261113.py) reproduces the submitted failure and fixed pass using that committed test.
>
> ## Suggestions
>
> None in this focused review.
>
> ## Scope decisions
>
> [#15765](https://github.com/ROCm/rocm-libraries/pull/15765) is still a sibling of this stack. Until its slow float-alpha reduction fix is integrated, large sums can produce different reference answers when eligibility or validation density selects the slow path. Keep the independent integer oracle; comparing only fast and slow answers would conceal which result is correct. The new fix covers dot-product overflow; inherited integer epilogue arithmetic is retained.
>
> The alias correction also applies to complex GEMMs in #15276. Integrate the common production fix once and retain both type-specific regressions.
>
> **Suggestion summary:** [review/pr15274-correctness-20261113](https://github.com/newling/rocm-libraries/tree/review/pr15274-correctness-20261113) contains [1024648658c](https://github.com/newling/rocm-libraries/commit/1024648658c17b9f3dfafb3aa41929817081c84f) for C/D aliasing and [a20ddeb0e6f](https://github.com/newling/rocm-libraries/commit/a20ddeb0e6f2e7b9e0af960d8b96730f8b3441f4) for dot-product overflow. The slow-path fix in #15765 has not been incorporated or certified by this review.
>

### General discussion comments (3, including bots)

#### therock-pr-bot[bot] — comment 6692547564

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15274#issuecomment-6692547564) · [@therock-pr-bot[bot]](https://github.com/apps/therock-pr-bot) · Created: 2026-11-06 18:39:43 UTC · Updated: 2026-11-10 22:31:55 UTC

> <!-- therock-pr-bot-policy-check -->
> ### ❌ PR Check — Action Required
>
> | Check | Status | Details |
> |---|:---:|---|
> | 📝 **PR Description** | ✅ Pass | — |
> | ⛔ **Forbidden Files** | ✅ Pass | — |
> | 🧪 **Unit Test** | ✅ Pass | — |
> | 🔎 **pre-commit** | ❌ Fail | **Error:** Check concluded with `cancelled`. |
> | 🚫 **Draft PR** | 🔜 To Be Enabled | — |
> | 🚩 **Feature Flag** | 🔜 To Be Enabled | — |
> | 📊 **Code Coverage** | 🔜 To Be Enabled | — |
>
> > ⚠️ **1 policy check(s) failed.** Please address the issues above before this PR can be Reviewed.
> >
> > 🚫 **Please fix the failed policies**
> > - ❌ pre-commit
> >
> > The **`Not ready to Review`** label was added to this PR. Once all policies pass, the label is removed automatically.
>
> 📖 **Need help?** See the [Policy FAQ](https://github.com/ROCm/rocm-libraries/blob/develop/docs/LIBRARIES_PR_BOT_FAQ.md) for details on every check and how to fix failures.
>
> 🙋 **[Wish to Override Policy?](https://github.com/ROCm/rocm-libraries/blob/develop/docs/LIBRARIES_PR_BOT_FAQ.md#-wish-to-override-the-policy-process-and-get-unblocked)**

#### therock-pr-bot[bot] — comment 6692786316

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15274#issuecomment-6692786316) · [@therock-pr-bot[bot]](https://github.com/apps/therock-pr-bot) · Created: 2026-11-06 18:51:49 UTC · Updated: 2026-11-09 21:35:20 UTC

> <!-- therock-pr-bot-fix-policies -->
> 🎉 All checks passed! This PR is ready for review.

#### codecov[bot] — comment 6788004815

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15274#issuecomment-6788004815) · [@codecov[bot]](https://github.com/apps/codecov) · Created: 2026-11-11 05:34:35 UTC · Updated: 2026-11-11 06:52:02 UTC

> ## [Codecov](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15274?dropdown=coverage&src=pr&el=h1&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) Report
> :x: Patch coverage is `91.66667%` with `16 lines` in your changes missing coverage. Please review.
>
> | [Files with missing lines](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15274?dropdown=coverage&src=pr&el=tree&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | Patch % | Lines |
> |---|---|---|
> | [...cts/hipblaslt/tensilelite/client/src/Reference.cpp](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15274?src=pr&el=tree&filepath=projects%2Fhipblaslt%2Ftensilelite%2Fclient%2Fsrc%2FReference.cpp&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm#diff-cHJvamVjdHMvaGlwYmxhc2x0L3RlbnNpbGVsaXRlL2NsaWVudC9zcmMvUmVmZXJlbmNlLmNwcA==) | 91.67% | [7 Missing and 9 partials :warning: ](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15274?src=pr&el=tree&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
>
> <details><summary>Additional details and impacted files</summary>
>
>
>
> ```diff
> @@                                  Coverage Diff                                  @@
> ##           users/alvasile/tl-speedup-16-cpu-gemm-driver-int8   #15274      +/-   ##
> =====================================================================================
> + Coverage                                              77.61%   77.62%   +0.01%
> =====================================================================================
>   Files                                                   3002     3002
>   Lines                                                 521646   521749     +103
>   Branches                                               81316    81342      +26
> =====================================================================================
> + Hits                                                  404852   404959     +107
> + Misses                                                 88974    88960      -14
> - Partials                                               27820    27830      +10
> ```
>
> | [Flag](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15274/flags?src=pr&el=flags&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | Coverage Δ | | *Carryforward flag |
> |---|---|---|---|
> | [TensileLite-CPP](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15274/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `53.22% <91.67%> (+0.20%)` | :arrow_up: | |
> | [TensileLite-Unit](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15274/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `83.84% <ø> (ø)` | | |
> | [hipBLAS](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15274/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `90.62% <ø> (ø)` | | Carriedforward from [4554dab](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/4554dab796dee28e4e640b7f688c4d0f12d5b375?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipBLASLt](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15274/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `70.44% <ø> (-0.04%)` | :arrow_down: | |
> | [hipCUB](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15274/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `82.68% <ø> (ø)` | | Carriedforward from [4554dab](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/4554dab796dee28e4e640b7f688c4d0f12d5b375?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipDNN](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15274/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `87.34% <ø> (ø)` | | Carriedforward from [4554dab](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/4554dab796dee28e4e640b7f688c4d0f12d5b375?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipFFT](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15274/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `42.65% <ø> (ø)` | | Carriedforward from [4554dab](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/4554dab796dee28e4e640b7f688c4d0f12d5b375?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipRAND](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15274/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `76.12% <ø> (ø)` | | Carriedforward from [4554dab](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/4554dab796dee28e4e640b7f688c4d0f12d5b375?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipSOLVER](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15274/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `69.67% <ø> (ø)` | | Carriedforward from [4554dab](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/4554dab796dee28e4e640b7f688c4d0f12d5b375?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipSPARSE](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15274/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `87.03% <ø> (ø)` | | Carriedforward from [4554dab](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/4554dab796dee28e4e640b7f688c4d0f12d5b375?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocALUTION](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15274/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `86.47% <ø> (ø)` | | Carriedforward from [4554dab](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/4554dab796dee28e4e640b7f688c4d0f12d5b375?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocBLAS](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15274/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `69.16% <ø> (ø)` | | Carriedforward from [4554dab](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/4554dab796dee28e4e640b7f688c4d0f12d5b375?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocFFT](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15274/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `48.81% <ø> (ø)` | | Carriedforward from [4554dab](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/4554dab796dee28e4e640b7f688c4d0f12d5b375?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocRAND](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15274/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `57.32% <ø> (ø)` | | Carriedforward from [4554dab](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/4554dab796dee28e4e640b7f688c4d0f12d5b375?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocSOLVER](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15274/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `76.66% <ø> (ø)` | | Carriedforward from [4554dab](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/4554dab796dee28e4e640b7f688c4d0f12d5b375?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocSPARSE](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15274/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `83.86% <ø> (ø)` | | Carriedforward from [4554dab](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/4554dab796dee28e4e640b7f688c4d0f12d5b375?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocSPARSE-unit](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15274/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `79.82% <ø> (ø)` | | Carriedforward from [4554dab](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/4554dab796dee28e4e640b7f688c4d0f12d5b375?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocThrust](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15274/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `93.15% <ø> (ø)` | | Carriedforward from [4554dab](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/4554dab796dee28e4e640b7f688c4d0f12d5b375?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
>
> *This pull request uses carry forward flags. [Click here](https://docs.codecov.io/docs/carryforward-flags?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) to find out more.
>
> | [Files with missing lines](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15274?dropdown=coverage&src=pr&el=tree&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | Coverage Δ | |
> |---|---|---|
> | [...cts/hipblaslt/tensilelite/client/src/Reference.cpp](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15274?src=pr&el=tree&filepath=projects%2Fhipblaslt%2Ftensilelite%2Fclient%2Fsrc%2FReference.cpp&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm#diff-cHJvamVjdHMvaGlwYmxhc2x0L3RlbnNpbGVsaXRlL2NsaWVudC9zcmMvUmVmZXJlbmNlLmNwcA==) | `46.59% <91.67%> (+2.53%)` | :arrow_up: |
>
> ... and [8 files with indirect coverage changes](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15274/indirect-changes?src=pr&el=tree-more&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm)
> </details>
> <details><summary> :rocket: New features to boost your workflow: </summary>
>
> - :snowflake: [Test Analytics](https://docs.codecov.com/docs/test-analytics): Detect flaky tests, report on failures, and find test suite problems.
> - :package: [JS Bundle Analysis](https://docs.codecov.com/docs/javascript-bundle-analysis): Save yourself from yourself by tracking and limiting bundle sizes in JS merges.
> </details>

<a id="pr-15275"></a>

## PR #15275 — perf(tensilelite): run Float8-output GEMMs on the CPU reference fast path

[GitHub PR](https://github.com/ROCm/rocm-libraries/pull/15275) · State: **OPEN**

Branch: `users/alvasile/tl-speedup-25-reference-fast-path-float8-output`. Base: `users/alvasile/tl-speedup-24-reference-fast-path-int8`. Head at collection: `b6828c46316720641f2e3f15a1320201d4f1c34b`.

### Feedback walkthrough — November 16, 2026

#### Scaled Half-to-Float8 inputs — numerical results and proposed fix

[Tony's ScaleAB comment](https://github.com/ROCm/rocm-libraries/pull/15275#discussion_r4568213239) and [newling's final linked review](https://github.com/newling/agent-public-reviews/blob/ede5c5f065835ae571ce0647b4e07af2c6cb8d64/rocm-libraries/review_pr15275_correctness_20261113.md#keep-scaled-half-to-float8-inputs-out-of-the-fast-path-until-the-formulas-agree) concern Half storage narrowed to Float8 before multiplication. Newling's [d930b5107f1 suggestion](https://github.com/newling/rocm-libraries/commit/d930b5107f10b667c55bfe4a9454e4f4fa8f9d4b) changes eligibility and adds a regression; it contains no slow-path formula edit.

**Pinned validation:** tested published Reference.cpp, reference gtests and CPU driver source at `b6828c46316720641f2e3f15a1320201d4f1c34b`. The local #15275 head remains `04197cbc20eb22675d88cf8b5725db691746ed5d`. Generated kernels using the review worktree at `d6af002f7a65`; verified its entire TensileLite Python package, runtime source and runtime headers have no differences from the published head. Hardware: GPU 0, gfx942 MI300X; ROCm 7.2.4 / Clang 22. Source proposals were tested in temporary copies; production code and GitHub text were not changed by this investigation.

**CPU matrix:** all 24 admitted Float8-output type IDs, OCP and fnuz, scaling disabled/Scalar/Vector, shapes 1×1×1 and 16×16×256, NN/NT/TN/TT, alpha=1/beta=0 and alpha=0.5/beta=1, and two scalar-scale profiles including non-unit scales on both operands: **2,304 cases per version**. Values include both signs, nonuniform vectors and nonzero values at the first and last K positions. Compared every output exactly against an independent arithmetic model with explicit scale/conversion order and HIP host FP8 codecs.

**gfx942 GPU matrix:** all 12 fnuz IDs, the same scale modes/layouts/epilogues, shape 16×16×256, GSU 1/2/4, and `MultipleBuffer`/`MultipleBufferSingleKernel`: **1,728 solution cases per version, all executed**. Each record includes actual kernel names, synchronized execution and downloaded outputs. GSU=1 configurations share normal main kernels, so this is a count of solution cases, not distinct kernels. The installed hipBLASLt API initially returned no algorithms for the affected cases; those attempts are excluded. This matrix does not cover every tile, edge shape or feature epilogue.

| Case | Published slow reference | Published fast reference | Measured gfx942 behavior | Required change |
| --- | --- | --- | --- | --- |
| Scaling disabled, all admitted types | Matches independent model | Matches independent model | Both accumulation algorithms match | Preserve eligibility |
| Scalar, Half narrowed to Float8 | Correct: scale Half before narrowing | Wrong order: narrow before scaling | Normal kernel and `MultipleBufferSingleKernel` match slow; `MultipleBuffer` GSU 2/4 applies the narrowed side's scalar scale twice | Scalar-only fast fallback; fix GPU conversion-call scale binding |
| Vector, Half narrowed to Float8 | Omits the narrowed operand's vector scale | Correct: applies both vectors after multiplication | All tested GPU paths match fast | Correct slow Vector scaling; retain Vector fast eligibility |
| Other admitted combinations, Scalar/Vector | Matches independent model | Matches independent model | Both accumulation algorithms match | Preserve eligibility and use as controls |

**Measured failures:** published CPU fast execution disagrees in **112 Scalar cases**, while slow execution disagrees in **96 Vector cases**. Some inputs/output casts mask the same defect, so numerical failure counts are lower than affected eligibility counts. Published gfx942 kernels disagree in **32 solution cases / 8,192 output elements**, exclusively Scalar Half-to-Float8 on either operand with `MultipleBuffer` and GSU 2 or 4, across all four layouts and both epilogues. For the 1×1 witness Half A=0.9, Float8 B=1, scaleA=3/scaleB=1, Scalar slow gives **2.75**, Scalar fast **2.5**; Vector CPU fast and the independent model give **2.5**, while legacy Vector slow gives **0.875**. The GPU matrix confirms the Vector operation order at 16×16×256; no 1×1 GPU execution is claimed. A 16×16×256 Scalar case with scaleA=3/scaleB=0.5 gives **2.75** in the first element on the normal/MBSK paths and **8** with the broken MB conversion.

**Tested reference proposal:** for Float8 output, reject fast eligibility only when ScaleAB is **Scalar** and Half storage narrows to Float8 on either side. In the slow Vector epilogue, remove the input-size gates and apply both vector scales. This proposal passes **all 2,304 CPU cases**, with **128 Scalar fallback cases / 2,176 still eligible**, plus the published source's **15 ReferenceFastPath gtests and 202 Float8-output driver cases**, including the two expected injected-validation failures. An indiscriminate Scalar-and-Vector fallback would preserve the demonstrated slow Vector defect.

**Tested GPU proposal:** retain both original scale pointers for the main kernel. For the conversion invocation only, replace the pointer for a Scalar scale already consumed before narrowing with null; the existing helper treats null as scale 1. Replaced only that invocation in the temporary native probe and reran the entire matrix: **all 1,728 GPU cases match the independent model**. This is validation of the argument-binding change, not a production runtime patch. The owning change is in [`outputConversionCallArgs`](https://github.com/ROCm/rocm-libraries/blob/b6828c46316720641f2e3f15a1320201d4f1c34b/projects/hipblaslt/tensilelite/src/ContractionSolution.cpp#L4590), which currently forwards both scale pointers; [`KernelWriterConversion.py`](https://github.com/ROCm/rocm-libraries/blob/b6828c46316720641f2e3f15a1320201d4f1c34b/projects/hipblaslt/tensilelite/tensilelite/KernelWriterConversion.py#L742) folds both into alpha. Shared conversion helper names omit A/B storage and MAC types, so changing their formula per input type would also require synchronized generator/runtime naming changes. Per-problem argument binding avoids that collision.

**gfx950 confirmation — user-provided results, November 16, 2026:** the user ran the remote baseline at `b6828c46316720641f2e3f15a1320201d4f1c34b` on HIP device 0, an AMD Instinct MI350X reporting `gfx950:sramecc+:xnack-`, with ROCm 7.2.1 and OCP F8/B8 types. These results confirm all three findings for OCP on gfx950, complementing the fnuz measurements on gfx942 with ROCm 7.2.4.

| gfx950 run | Executed | Passed | Numerical failures | Unexecuted |
| --- | --- | --- | --- | --- |
| Pinned remote baseline | 1,728 | 1,696 | 32 | 0 |
| Conversion-call proposal | 1,728 | 1,728 | 0 | 0 |

**A — Scalar narrowing:** normal kernels scale Half before Float8 narrowing, supporting the Scalar-only fast fallback. **B — Vector narrowing:** GPU results match narrowing first and applying both vectors afterward, supporting correction of slow Vector scaling while retaining Vector fast eligibility. **C — MultipleBuffer split-K:** all 32 failures apply the consumed Half-side Scalar scale again at GSU 2/4. Replacing only the conversion invocation's narrowed-side scale pointer corrects every failure; the other **1,696 cases retain identical output bytes**. This entry records the user's run summary; it does not claim an additional local gfx950 execution.

**Agreed ownership:** put the pre-existing slow Vector omission and GPU conversion-binding defect in one new correctness PR immediately above #15274 and below #15275. Implement it on `users/alvasile/tl-fix-float8-input-scaling`, with regressions independent of #15275. Keep the Scalar-only fast fallback, its eligibility/scaling tests and the CMake `noscale` explanation in #15275. [Future work](#future-work) records the GitHub issue, issue/PR links, PR creation and stack insertion. The issue must link #15275 as the discovery PR; the new correctness PR must close that issue.

**Speedup scope:** the affected scaled-narrowing combinations are absent from the reported `sk_f8gemm_quick` cases: Float8-output cases store Float8 on both sides, while the mixed Half/Float8 case uses Float output with scaling disabled. The proposed narrow guard leaves those cases eligible. No performance rerun is claimed.

**Artifacts:** [gfx942/CPU experiment details and reproduction commands](/tmp/tl-pr15275-numerical-20261116/README.md), [gfx942/CPU machine-readable summary](/tmp/tl-pr15275-numerical-20261116/summary.json), full CPU/GPU JSONL output arrays, temporary source proposals, build recipes and test logs are saved in `/tmp/tl-pr15275-numerical-20261116/`. The [remote-based gfx950 handoff](/home/alvasile/rocm-libraries/tl-pr15275-gfx950-validation-handoff.md) also records the user's returned gfx950 results. **Both architectures confirmed the findings. The pre-existing fixes are implemented locally below #15275; the Scalar fast guard is implemented locally; final-production gfx950 validation and stack insertion remain pending.**

#### Scalar fast fallback — implemented locally November 17, 2026

**Agreed and implemented:** #15275 now rejects fast eligibility only for Scalar scaling when Half storage is narrowed to Float8 for multiplication and C/D are Float8. Vector narrowing remains eligible. The preceding correctness PR owns the slow Vector formula and GPU conversion-binding fixes; those corrections are not duplicated here. The CMake explanation now describes the final Scalar/Vector policy. [Tony's ScaleAB thread](https://github.com/ROCm/rocm-libraries/pull/15275#discussion_r4568213239) is addressed by these local changes and the agreed correctness-PR dependency; publication and stack insertion remain pending.

**Regression evidence:** the Scalar test failed before the guard, returning 1.25 instead of the independently specified 1.375 for both encodings and operand directions. It passes after the guard. The Vector test uses signed, nonuniform vectors and independent output literals and remains fast-eligible. The eligibility sweep covers all 24 admitted type IDs in no-scale, Scalar and Vector modes.

**Validation:** all 35 branch reference gtests and all 675 CPU-driver CTests pass. A temporary combined CPU reference, constructed by applying only commit `6d06af3ff59c224c92a468920120797edd2212fa`'s slow Vector patch to the guarded #15275 source, passes 38 reference gtests and all 2,304 independent CPU cases on both default and forced-slow paths. Exactly 128 Scalar narrowing cases fall back and all 128 Vector narrowing cases stay eligible. On #15275 before insertion, all 2,304 default results pass while forced-slow Vector retains the known 96 errors owned by the preceding correctness PR. No stack metadata or base was changed by this validation. [Commands, raw results and scope](/tmp/tl-pr15275-scalar-guard-20261117/README.md) and [summary](/tmp/tl-pr15275-scalar-guard-20261117/summary.json).

#### Non-finite output casts — implemented locally November 17, 2026

**Agreed and implemented:** [Tony's cast-coverage request](https://github.com/ROCm/rocm-libraries/pull/15275#discussion_r4568213252) is covered by adding positive infinity, negative infinity and NaN to each of the four existing output-cast tables. Each reference path is checked against the explicit expected result. OCP E4M3 and both fnuz types turn either infinity into NaN; OCP E5M2 preserves signed infinity. All four preserve NaN. The helper checks NaN with `std::isnan` on each path, and the source comment now distinguishes finite saturation from non-finite conversion.

**Validation:** the added NaN cases first exposed the helper's invalid NaN equality assertions. After correcting the helper, the focused cast test and all 35 reference gtests pass. The test remains in the existing standard-tier C++ test registration. No production code or CI registration changed. Shared CI has not run this unpublished change. [Local test logs, XML and summary](/tmp/tl-pr15275-nonfinite-casts-20261117/README.md).

**Draft reply to Tony — not posted:**

> Added +Inf, -Inf and NaN to all four cast tables, with explicit expectations for each reference path and `std::isnan` checks for NaN. OCP E4M3 and both fnuz types convert either infinity to NaN; OCP E5M2 preserves signed infinity. All four preserve NaN. The cast test and all 35 reference gtests pass locally.

#### PR description — updated on GitHub November 17, 2026

**Completed:** [Tony's Test Result request](https://github.com/ROCm/rocm-libraries/pull/15275#pullrequestreview-5897489454) is reflected in the [published description](https://github.com/ROCm/rocm-libraries/pull/15275). It names passing Multi-Arch, coverage and Math CI jobs at remote head `b6828c46316720641f2e3f15a1320201d4f1c34b`, records the local Scalar guard at `d9b351947565` and non-finite cast tests at `f6b685979d48`, and states the limits of the temporary combined CPU validation. The original-branch timing measurements retain their hardware, thread and single-run context. The eligibility, cast and dependency wording now describes the agreed final policy. The focused Test Plan runs the gtest binary directly because shared CTest groups those tests by category. Read-back matched the intended description exactly; the remote head, base and title were preserved. Code remains unpublished. [Exact description and publication records](/tmp/tl-pr15275-description-20261117/description.md).

#### Type selectors in driver runtime errors — implemented locally November 17, 2026

[Newling's initial linked review](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15275.md#include-the-failing-type-configuration-in-runtime-errors) suggests including the failing storage, compute-input, output and alpha/beta type selectors in exception diagnostics. His suggestion is [aef34bafaad](https://github.com/newling/rocm-libraries/commit/aef34bafaad8296981db27b07f6f2fe96bd6258e).

**Agreed and implemented:** the driver's exception message now appends `typeA`, `typeB`, `computeInputA`, `computeInputB`, `typeCD` and `typeAlpha`, with their resolved defaults. It retains the original exception text and exit code 1. The unsupported `--typeA f8 --typeB f16 --typeCD f8` combination now identifies its incompatible default multiplication types directly in stderr. The change adds no type-ID allowlist and leaves dispatch decisions in the reference.

**Validation:** all 675 driver CTests pass, with zero skipped or disabled tests. Focused CLI checks cover both Half operand directions and both OCP/fnuz encodings: four unsupported combinations produce the exact six-selector diagnostic and exit code 1, while eight supported multiplication-type alternatives pass numerical validation. [Build, commands, raw outputs and results](/tmp/tl-pr15275-runtime-diagnostic-20261117/README.md).

**Draft reply to Newling — not posted:**

> Added the storage, compute-input, output and alpha/beta type selectors to the driver's runtime exception message, preserving the original exception text and exit code 1. The mixed Float8/Half error now identifies the incompatible default compute-input types. Both supported alternatives pass for both encodings and operand directions, and all 675 driver CTests pass locally.

**Walkthrough status:** all feedback items for #15275 now have an agreed resolution. The code changes remain local; the correctness PR's issue, PR creation, stack insertion and shared-CI/final-production gfx950 validation remain in [Future work](#future-work). A fresh read of #15275's reviews, inline comments and discussion comments, plus Newling's reviews posted on #15259, found no additional human feedback. The next item is [complex C/D aliasing on #15276](#pr-15276).

### Reviews (1)

#### tony-davis — CHANGES_REQUESTED — review 5897489454

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15275#pullrequestreview-5897489454) · [@tony-davis](https://github.com/tony-davis) · Submitted: 2026-11-13 20:21:18 UTC

Reviewed commit: `b6828c46316720641f2e3f15a1320201d4f1c34b`.

> Hi Alex! Thanks for this one. Taking 47 seconds off `sk_f8gemm_quick` is a nice win. Reusing the slow path's `SaturateCast` for the write-back means the cast itself can't drift between the paths, and the cast test at the limits (ties, subnormals, values that round past the max) locks it down well. I also like the driver fix for one-sided NaN. Before this, `diff > tolerance` was false for a NaN, so the driver couldn't catch a NaN output at all. I checked your tolerance argument against `AlmostEqual` in `Reference.hpp`, and `|a - b| < 0.125 * (|a| + |b| + 1)` does cover one E4M3 step everywhere (0.25 for E5M2), including near zero.
>
> One thing I think needs to change before this merges. Four of the 24 ids store one operand as Half and quantize it to Float8 for the MAC (`HF8_FP8_FP8_S`, `F8H_FP8_FP8_S` and the two fnuz ones). With ScaleAB on, the slow and fast paths compute different things there. The slow path scales the Half value before quantizing it (Scalar) or drops that side's scale (Vector); see `getElement` and the `sizeof(AType) <= sizeof(ComputeInputTypeA)` guards. The fast path quantizes first and multiplies both scales into alpha. The CMake comment at line 837 already describes this, but the eligibility check still lets those problems through, so the expected value depends on the path. In Vector mode that's a factor of scaleA, not one F8 step.
>
> I don't see a current Tests/common config that hits it (the Float8-output Float8 x Half configs all set `UseScaleCD`, which the fast path rejects), so it's latent. I'd still rather the fast path reject it. There's a sketch inline.
>
> Two non-blocking notes. Test Result still says the branch hasn't been built or tested; the required checks are green at b6828c4, and Math CI's `tensilelite-unit-codecov` and `preliminary` passed. And a couple of non-finite cases in the cast test would pin what Inf and NaN turn into (inline).
>
> This sits on #15274 (with #15266 and #15259 below it), so it waits on those too.

### Inline review threads (2 threads, 2 comments)

#### Thread 1

`projects/hipblaslt/tensilelite/client/src/Reference.cpp` · Current line: 1357 · Original line: 1357 · Side: RIGHT

Status: Unresolved · Current diff.

##### tony-davis — comment 4568213239

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15275#discussion_r4568213239) · [@tony-davis](https://github.com/tony-davis) · Created: 2026-11-13 20:21:17 UTC · Updated: 2026-11-13 20:21:18 UTC

[Part of review 5897489454](https://github.com/ROCm/rocm-libraries/pull/15275#pullrequestreview-5897489454)

> I think this is the place for a ScaleAB guard. For the four Half-storage, Float8-MAC ids, the fast path pre-quantizes the Half operand without its scale (`ShadowBuffer` at line 296) and then multiplies both scales into alpha. The slow path quantizes `scaleA * a` (Scalar) or ignores that side's scale (Vector), so with ScaleAB the two disagree by much more than one F8 step. Something like this (untested) would send those problems to the slow path:
>
> ```cpp
> // With ScaleAB, the slow path scales an operand stored wider than its
> // compute-input type before quantizing it (Scalar) or drops its scale (Vector);
> // the fast path multiplies both scales into alpha instead.
> auto storedWider = [](rocisa::DataType storage, rocisa::DataType compute) {
>     return DataTypeInfo::Get(storage).elementSize > DataTypeInfo::Get(compute).elementSize;
> };
> if(!problem.useScaleAB().empty()
>    && (storedWider(problem.a().dataType(), problem.computeInputTypeA())
>        || storedWider(problem.b().dataType(), problem.computeInputTypeB())))
> {
>     return rejectFast("scaleAB_with_quantized_operand");
> }
> ```
>
> That would also move the existing Half and Float output versions of these problems to the slow path, which I think is the same bug. If you'd rather keep this PR to Float8 outputs, gating it on `isFloat8Output` plus a follow-up ticket is fine with me. An `EXPECT_FALSE(isFastPathEligible(...))` for one of the four ids with `setUseScaleAB("Scalar")` would lock it in, and then the CMake comment at line 837 can say they're rejected rather than untested.

#### Thread 2

`projects/hipblaslt/tensilelite/tests/ReferenceFastPath_test.cpp` · Current line: 617 · Original line: 617 · Side: RIGHT

Status: Unresolved · Current diff.

##### tony-davis — comment 4568213252

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15275#discussion_r4568213252) · [@tony-davis](https://github.com/tony-davis) · Created: 2026-11-13 20:21:18 UTC

[Part of review 5897489454](https://github.com/ROCm/rocm-libraries/pull/15275#pullrequestreview-5897489454)

> (non-blocking) Since this test pins the cast at its limits, I'd add `INFINITY`, `-INFINITY` and `NAN` too (NaN needs an `isnan` check rather than `EXPECT_EQ`). OCP E4M3 has no infinity, so I'm honestly not sure whether +Inf becomes 448 or NaN here, and a test would answer that for the next person. Both paths share `SaturateCast`, so this is documentation more than path agreement.

### Newling’s linked reviews and cross-PR remarks

Posted on #15259: [initial round](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5867396457) · [final round](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5899752492). The [shared testing and design assessment](#shared-assessment) applies to this PR too.

**Newling’s own remark about #15275** ([original post](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5899752492)):

> #15275 has a change to the slow path to make its semantics agree with the fast path. This one I'm not sure about, might need to probe what the GPU implementation actually does

#### Initial agent review, linked November 12

[Original on GitHub](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15275.md) · [Newling’s source post](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5867396457)

Source revision: `30df5bab4294f839ef85a91c732c36c92c0821ca`. File: `rocm-libraries/review_pr15275.md`.

> This is a review from an agent with an automatic prompt from the reviewer
>
> **PR reviewed:** [ROCm/rocm-libraries#15275](https://github.com/ROCm/rocm-libraries/pull/15275)
>
> **Submitted head:** `b6828c46316720641f2e3f15a1320201d4f1c34b`. **Incremental base:** `f58bbb3388853e13cdf85bda0917485d590c4ddc`.
>
> Independent review of the incremental change and its interaction with the CPU-reference stack, emphasizing test adequacy, runtime, overlap, design reuse, and unsupported cases. Inputs were the public descriptions, code, repository guidance and CI status/logs; target-PR discussions and existing reviews were not read.
>
> **Suggestion branch:** [review/pr15275-suggestions](https://github.com/newling/rocm-libraries/tree/review/pr15275-suggestions), based on the submitted head above; final head [aef34bafaad8296981db27b07f6f2fe96bd6258e](https://github.com/newling/rocm-libraries/commit/aef34bafaad8296981db27b07f6f2fe96bd6258e).
>
> ## Tests
>
> Submitted head: `cpu-gemm-driver`, `tensilelite-tests`, and `tensilelite-client` built successfully; **647 driver CTests and 32 focused reference gtests passed**. The driver suite took 4.18 s with four CTest workers. Suggestion implementation: the same targets built; **647 driver CTests and 33 focused reference gtests passed**. Later comment-only commits, where present, do not change that execution result.
>
> Validation used a Release ROCm compiler build. Controlled runs set `OMP_THREAD_LIMIT=8`, `OMP_NUM_THREADS=4`, and `KMP_BLOCKTIME=0`; the original stack-tip driver timing predates the thread-limit setting. These are host numerical tests, not GPU-kernel performance measurements. Root/component hook invocations and full-stack whitespace checks were clean; the formatting hooks opt out of these C++/CMake paths, and no Python tests were selected by the component hook. No local GPU or sanitizer run is claimed.
>
> The gtests were run directly with `--gtest_filter=ReferenceFastPath.*:ReferenceMX*.*:*ReferenceMX*/*`. In the category-based build, a CTest name regex containing `ReferenceFastPath` does not select them. Driver CTests are build-tree tests and are not installed into the TheRock test package; Math CI's unfiltered `coverage-cpp` lane runs them but is informational. The gtest categories include standard/comprehensive/full, not quick.
>
> ## Summary
>
> Reusing the float accumulator and the existing saturating output cast keeps the fast implementation small. The 207 added driver cases are already reduced by responsibility: four homogeneous IDs get the full feature matrix; hybrid/cross-output IDs get a smaller matrix. The two new gtests exercise eligibility and conversion limits. This is useful coverage at a reasonable runtime.
>
> However, the `noscale` subset deliberately avoids a combination that the production eligibility check still accepts. The resulting gap is about conversion semantics, not merely the number of tests.
>
> ## Actionable items
>
> ### Keep scaled Half-to-Float8 narrowing out of the fast path until its semantics match
>
> [client/src/Reference.cpp:1354](https://github.com/ROCm/rocm-libraries/blob/b6828c46316720641f2e3f15a1320201d4f1c34b/projects/hipblaslt/tensilelite/client/src/Reference.cpp#L1354) admits Float8 outputs with a Half storage operand narrowed to Float8 and Scalar/Vector ScaleAB. [tests/CMakeLists.txt:837](https://github.com/ROCm/rocm-libraries/blob/b6828c46316720641f2e3f15a1320201d4f1c34b/projects/hipblaslt/tensilelite/tests/CMakeLists.txt#L837) explicitly omits scaling tests for these IDs because the paths handle it differently.
>
> For a 1×1 problem with Half A=0.9, Float8 B=1, scaleA=3, scaleB=1, alpha=1 and beta=0, the scalar slow path gives **2.75** while the fast path gives **2.5**: scaling occurs on opposite sides of the narrowing conversion. Vector scaling is more substantial: the slow path omits that operand's scale while the fast path applies it. This is not covered by an explanation limited to last-bit reassociation.
>
> Suggestion commit [d930b5107f1](https://github.com/newling/rocm-libraries/commit/d930b5107f10b667c55bfe4a9454e4f4fa8f9d4b) rejects these combinations from fast eligibility and adds `ScaledHalfToFloat8KeepsSlowPathConversionOrder`, covering both operand directions and both scale modes. The ordinary solver request then preserves the slow behavior. The driver, when explicitly forced to use fast, reports ineligibility.
>
> ## Suggestions
>
> ### Include the failing type configuration in runtime errors
>
> At [client/cpu_gemm_driver.cpp:1585](https://github.com/ROCm/rocm-libraries/blob/b6828c46316720641f2e3f15a1320201d4f1c34b/projects/hipblaslt/tensilelite/client/cpu_gemm_driver.cpp#L1585), `--typeA f8 --typeB f16 --typeCD f8` reaches the reference and prints only `Data type not implemented.` The missing piece is the compute-input combination. Suggestion commit [aef34bafaad](https://github.com/newling/rocm-libraries/commit/aef34bafaad8296981db27b07f6f2fe96bd6258e) appends storage, compute-input, output, and scalar selectors to runtime errors without duplicating the reference's type-ID allowlist. This command now identifies `computeInputA=f8, computeInputB=f16`; choosing both compute inputs as Half or both as the matching Float8 encoding gives the supported alternatives.
>
> The shared negative-test helper in #15278 ([66ef187d435](https://github.com/newling/rocm-libraries/commit/66ef187d43535cacaba49a328b77b0388909291e)) strengthens the existing rejection and injected-failure cases. Its additional eligibility-diagnostic change exposes the fast-path reason to the driver.
>
> ## Scope decisions
>
> The suggestion deliberately retains the pre-existing Vector ScaleAB omission on the slow path; it does not claim to correct that formula. A separate change can define and align scaling/conversion semantics across output types. Until then, enabling those cases on the fast path changes reference behavior substantially.
>
> No wholesale expansion of the 24-ID matrix is needed. A compact conversion-order regression is more useful than repeating all transpose and feature combinations for this boundary.
>
> The exact native probe source and build recipe for the numerical counterexamples are preserved in the [cross-PR appendix](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_cpu_reference_stack_15259_15278.md#appendix-native-counterexample-source-and-setup). Committed regressions cover the corresponding contracts.
>
> ## Suggestion branch handoff
>
> | Suggestion commit | Item |
> | --- | --- |
> | [d930b5107f1](https://github.com/newling/rocm-libraries/commit/d930b5107f10b667c55bfe4a9454e4f4fa8f9d4b) | retain scaled narrow-input Float8 reference semantics |
> | [aef34bafaad](https://github.com/newling/rocm-libraries/commit/aef34bafaad8296981db27b07f6f2fe96bd6258e) | identify the type combination in driver runtime errors |
>
> The suggestion branches are published. Each branch contains its own corrections on its own submitted head, not the other review branches' fixes. Integrate shared changes in stack order, including the alias fix once and later `TypeInfo` call sites. PR-description corrections are reported for the author to make; none were published. See [the cross-PR assessment](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_cpu_reference_stack_15259_15278.md) for test overlap, CI limits and integration notes.
>

#### Final correctness review, linked November 14 (document dated November 13)

[Original on GitHub](https://github.com/newling/agent-public-reviews/blob/ede5c5f065835ae571ce0647b4e07af2c6cb8d64/rocm-libraries/review_pr15275_correctness_20261113.md) · [Newling’s source post](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5899752492)

Source revision: `ede5c5f065835ae571ce0647b4e07af2c6cb8d64`. File: `rocm-libraries/review_pr15275_correctness_20261113.md`.

> This is a review from an agent with an automatic prompt from the reviewer
>
> **PR reviewed:** [ROCm/rocm-libraries#15275](https://github.com/ROCm/rocm-libraries/pull/15275)
>
> **Submitted head:** `b6828c46316720641f2e3f15a1320201d4f1c34b`. **Incremental diff base:** `f58bbb3388853e13cdf85bda0917485d590c4ddc`.
>
> **Scope:** Focused follow-up on consequential CPU-reference errors and regression-test failures, including the existing review discussion. Head and discussion were unchanged at the 2026-11-13 23:09 UTC check. Earlier validation is reused where the tested code is unchanged.
>
> **Suggestion branch:** [review/pr15275-correctness-20261113](https://github.com/newling/rocm-libraries/tree/review/pr15275-correctness-20261113), based on the submitted head; final head [d930b5107f1](https://github.com/newling/rocm-libraries/commit/d930b5107f10b667c55bfe4a9454e4f4fa8f9d4b). Commit-to-item mapping is given below.
>
> ## Tests
>
> Reused validation for the unchanged implementations: submitted head, 647 driver CTests and 32 reference tests passed; retained fix, 647 driver CTests and 33 reference tests passed. The driver, tests and client built. A direct CPU probe demonstrated the scaling discrepancy below; the regression checks both operand directions and scalar/vector scaling. No new GPU or performance run was made.
>
> ## Summary
>
> Float accumulation followed by the existing Float8 output conversion is a small extension of the tiled reference. The material exception is a stored Half operand that is first narrowed to Float8 for multiplication: moving its scale across that conversion changes the numerical operation.
>
> ## Actionable items
>
> ### Keep scaled Half-to-Float8 inputs out of the fast path until the formulas agree
>
> [client/src/Reference.cpp:1354](https://github.com/ROCm/rocm-libraries/blob/b6828c46316720641f2e3f15a1320201d4f1c34b/projects/hipblaslt/tensilelite/client/src/Reference.cpp#L1354) admits Float8-output configurations with a Half input narrowed to Float8 and ScaleAB enabled. The corresponding driver tests explicitly omit scaling at [tests/CMakeLists.txt:837](https://github.com/ROCm/rocm-libraries/blob/b6828c46316720641f2e3f15a1320201d4f1c34b/projects/hipblaslt/tensilelite/tests/CMakeLists.txt#L837).
>
> For a 1×1 GEMM with Half A=0.9, Float8 B=1, scaleA=3, scaleB=1, alpha=1 and beta=0, the slow reference returns **2.75**, while the fast reference returns **2.5**. The slow path scales before quantizing A; the fast path quantizes before scaling. Vector scaling exposes a larger semantic difference: the legacy slow path omits the narrowed operand's scale while the fast path applies it. The expected reference result can therefore change with path selection, beyond ordinary last-bit reassociation.
>
> Fix [d930b5107f1](https://github.com/newling/rocm-libraries/commit/d930b5107f10b667c55bfe4a9454e4f4fa8f9d4b) excludes these combinations from fast eligibility. `ScaledHalfToFloat8KeepsSlowPathConversionOrder` checks fallback through the normal solver, using both operand directions and both scale modes. The original probe is preserved in the [native counterexample source and setup](https://github.com/newling/agent-public-reviews/blob/ede5c5f065835ae571ce0647b4e07af2c6cb8d64/rocm-libraries/review_cpu_reference_stack_15259_15278.md#appendix-native-counterexample-source-and-setup).
>
> ## Suggestions
>
> None in this focused review.
>
> ## Scope decisions
>
> The guard preserves current behavior, including the legacy vector-scale omission. Correcting that formula requires an explicit change shared by both paths; it should not happen implicitly in a performance extension. Current common configurations use ScaleCD, which already excludes fast execution, so this is a demonstrated supported-case defect rather than an observed failure in those configurations.
>
> The existing requested-change review raises this same discrepancy. It remains unaddressed at the submitted head.
>
> **Suggestion summary:** [review/pr15275-correctness-20261113](https://github.com/newling/rocm-libraries/tree/review/pr15275-correctness-20261113) contains only [d930b5107f1](https://github.com/newling/rocm-libraries/commit/d930b5107f10b667c55bfe4a9454e4f4fa8f9d4b), the eligibility guard and regression. A broader scaling-policy change is left open.
>

### General discussion comments (3, including bots)

#### therock-pr-bot[bot] — comment 6692569561

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15275#issuecomment-6692569561) · [@therock-pr-bot[bot]](https://github.com/apps/therock-pr-bot) · Created: 2026-11-06 18:40:50 UTC · Updated: 2026-11-10 22:31:45 UTC

> <!-- therock-pr-bot-policy-check -->
> ### ❌ PR Check — Action Required
>
> | Check | Status | Details |
> |---|:---:|---|
> | 📝 **PR Description** | ✅ Pass | — |
> | ⛔ **Forbidden Files** | ✅ Pass | — |
> | 🧪 **Unit Test** | ✅ Pass | — |
> | 🔎 **pre-commit** | ❌ Fail | **Error:** Check concluded with `cancelled`. |
> | 🚫 **Draft PR** | 🔜 To Be Enabled | — |
> | 🚩 **Feature Flag** | 🔜 To Be Enabled | — |
> | 📊 **Code Coverage** | 🔜 To Be Enabled | — |
>
> > ⚠️ **1 policy check(s) failed.** Please address the issues above before this PR can be Reviewed.
> >
> > 🚫 **Please fix the failed policies**
> > - ❌ pre-commit
> >
> > The **`Not ready to Review`** label was added to this PR. Once all policies pass, the label is removed automatically.
>
> 📖 **Need help?** See the [Policy FAQ](https://github.com/ROCm/rocm-libraries/blob/develop/docs/LIBRARIES_PR_BOT_FAQ.md) for details on every check and how to fix failures.
>
> 🙋 **[Wish to Override Policy?](https://github.com/ROCm/rocm-libraries/blob/develop/docs/LIBRARIES_PR_BOT_FAQ.md#-wish-to-override-the-policy-process-and-get-unblocked)**

#### therock-pr-bot[bot] — comment 6692570903

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15275#issuecomment-6692570903) · [@therock-pr-bot[bot]](https://github.com/apps/therock-pr-bot) · Created: 2026-11-06 18:40:54 UTC · Updated: 2026-11-10 16:00:24 UTC

> <!-- therock-pr-bot-fix-policies -->
> 🎉 All checks passed! This PR is ready for review.

#### codecov[bot] — comment 6788015987

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15275#issuecomment-6788015987) · [@codecov[bot]](https://github.com/apps/codecov) · Created: 2026-11-11 05:35:20 UTC · Updated: 2026-11-11 07:31:24 UTC

> ## [Codecov](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15275?dropdown=coverage&src=pr&el=h1&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) Report
> :x: Patch coverage is `98.46154%` with `1 line` in your changes missing coverage. Please review.
>
> | [Files with missing lines](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15275?dropdown=coverage&src=pr&el=tree&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | Patch % | Lines |
> |---|---|---|
> | [...cts/hipblaslt/tensilelite/client/src/Reference.cpp](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15275?src=pr&el=tree&filepath=projects%2Fhipblaslt%2Ftensilelite%2Fclient%2Fsrc%2FReference.cpp&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm#diff-cHJvamVjdHMvaGlwYmxhc2x0L3RlbnNpbGVsaXRlL2NsaWVudC9zcmMvUmVmZXJlbmNlLmNwcA==) | 98.46% | [0 Missing and 1 partial :warning: ](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15275?src=pr&el=tree&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
>
> <details><summary>Additional details and impacted files</summary>
>
>
>
> ```diff
> @@                                    Coverage Diff                                    @@
> ##           users/alvasile/tl-speedup-24-reference-fast-path-int8   #15275      +/-   ##
> =========================================================================================
> + Coverage                                                  77.62%   77.64%   +0.02%
> =========================================================================================
>   Files                                                       3002     3002
>   Lines                                                     521749   521809      +60
>   Branches                                                   81342    81374      +32
> =========================================================================================
> + Hits                                                      404959   405126     +167
> + Misses                                                     88960    88882      -78
> + Partials                                                   27830    27801      -29
> ```
>
> | [Flag](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15275/flags?src=pr&el=flags&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | Coverage Δ | | *Carryforward flag |
> |---|---|---|---|
> | [TensileLite-CPP](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15275/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `53.65% <98.46%> (+0.43%)` | :arrow_up: | |
> | [TensileLite-Unit](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15275/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `83.84% <ø> (ø)` | | |
> | [hipBLAS](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15275/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `90.62% <ø> (ø)` | | Carriedforward from [f58bbb3](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/f58bbb3388853e13cdf85bda0917485d590c4ddc?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipBLASLt](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15275/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `70.46% <ø> (+0.02%)` | :arrow_up: | |
> | [hipCUB](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15275/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `82.68% <ø> (ø)` | | Carriedforward from [f58bbb3](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/f58bbb3388853e13cdf85bda0917485d590c4ddc?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipDNN](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15275/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `87.34% <ø> (ø)` | | Carriedforward from [f58bbb3](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/f58bbb3388853e13cdf85bda0917485d590c4ddc?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipFFT](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15275/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `42.65% <ø> (ø)` | | Carriedforward from [f58bbb3](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/f58bbb3388853e13cdf85bda0917485d590c4ddc?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipRAND](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15275/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `76.12% <ø> (ø)` | | Carriedforward from [f58bbb3](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/f58bbb3388853e13cdf85bda0917485d590c4ddc?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipSOLVER](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15275/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `69.67% <ø> (ø)` | | Carriedforward from [f58bbb3](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/f58bbb3388853e13cdf85bda0917485d590c4ddc?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipSPARSE](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15275/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `87.03% <ø> (ø)` | | Carriedforward from [f58bbb3](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/f58bbb3388853e13cdf85bda0917485d590c4ddc?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocALUTION](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15275/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `86.47% <ø> (ø)` | | Carriedforward from [f58bbb3](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/f58bbb3388853e13cdf85bda0917485d590c4ddc?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocBLAS](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15275/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `69.16% <ø> (ø)` | | Carriedforward from [f58bbb3](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/f58bbb3388853e13cdf85bda0917485d590c4ddc?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocFFT](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15275/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `48.81% <ø> (ø)` | | Carriedforward from [f58bbb3](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/f58bbb3388853e13cdf85bda0917485d590c4ddc?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocRAND](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15275/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `57.32% <ø> (ø)` | | Carriedforward from [f58bbb3](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/f58bbb3388853e13cdf85bda0917485d590c4ddc?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocSOLVER](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15275/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `76.66% <ø> (ø)` | | Carriedforward from [f58bbb3](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/f58bbb3388853e13cdf85bda0917485d590c4ddc?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocSPARSE](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15275/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `83.86% <ø> (ø)` | | Carriedforward from [f58bbb3](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/f58bbb3388853e13cdf85bda0917485d590c4ddc?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocSPARSE-unit](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15275/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `79.82% <ø> (ø)` | | Carriedforward from [f58bbb3](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/f58bbb3388853e13cdf85bda0917485d590c4ddc?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocThrust](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15275/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `93.15% <ø> (ø)` | | Carriedforward from [f58bbb3](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/f58bbb3388853e13cdf85bda0917485d590c4ddc?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
>
> *This pull request uses carry forward flags. [Click here](https://docs.codecov.io/docs/carryforward-flags?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) to find out more.
>
> | [Files with missing lines](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15275?dropdown=coverage&src=pr&el=tree&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | Coverage Δ | |
> |---|---|---|
> | [...cts/hipblaslt/tensilelite/client/src/Reference.cpp](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15275?src=pr&el=tree&filepath=projects%2Fhipblaslt%2Ftensilelite%2Fclient%2Fsrc%2FReference.cpp&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm#diff-cHJvamVjdHMvaGlwYmxhc2x0L3RlbnNpbGVsaXRlL2NsaWVudC9zcmMvUmVmZXJlbmNlLmNwcA==) | `51.46% <98.46%> (+4.87%)` | :arrow_up: |
>
> ... and [7 files with indirect coverage changes](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15275/indirect-changes?src=pr&el=tree-more&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm)
> </details>
> <details><summary> :rocket: New features to boost your workflow: </summary>
>
> - :snowflake: [Test Analytics](https://docs.codecov.com/docs/test-analytics): Detect flaky tests, report on failures, and find test suite problems.
> - :package: [JS Bundle Analysis](https://docs.codecov.com/docs/javascript-bundle-analysis): Save yourself from yourself by tracking and limiting bundle sizes in JS merges.
> </details>

<a id="pr-15276"></a>

## PR #15276 — perf(tensilelite): run complex GEMMs on the CPU reference fast path

[GitHub PR](https://github.com/ROCm/rocm-libraries/pull/15276) · State: **OPEN**

Branch: `users/alvasile/tl-speedup-26-reference-fast-path-complex`. Base: `users/alvasile/tl-speedup-25-reference-fast-path-float8-output`. Head at collection: `5b4f85f05f2977d06f67cf5405eff41da6800f93`.

### Feedback walkthrough — November 17, 2026

#### Complex C/D aliasing — implemented locally

[Newling's linked review](https://github.com/newling/agent-public-reviews/blob/ede5c5f065835ae571ce0647b4e07af2c6cb8d64/rocm-libraries/review_pr15276_correctness_20261113.md#preserve-complex-c-until-the-epilogue-has-consumed-it) reports that writing an intermediate sum to D destroys C when they share storage. His example uses A = `(2,0)`, B = `(3,0)`, C = D = `(5,1)`, and alpha = beta = `(1,0)`: the expected result is `(11,1)`, while the reviewed fast path returns `(12,0)`.

**Agreed implementation:** switched to `users/alvasile/tl-speedup-26-reference-fast-path-complex` and added `ComplexInPlaceCDPreservesBetaInput` through `SolveGemmCPU`. It covers complex float and complex double, sharing the C/D buffer with nonzero beta, using independent expected literals. Besides Newling's `(11,1)` example, it checks A = `(2,1)`, B = `(3,-2)`, initial C = `(5,1)`, alpha = `(1,-1)`, beta = `(2,1)`, and expected D = `(16,-2)`. Both cases run through the fast and slow paths.

**Red before green:** the new regression failed on local head `e51f9dd0374926ab594f582af2ed21a96628821a` for both complex widths. Fast results were `(12,0)` and `(24,-3)` rather than `(11,1)` and `(16,-2)`; the slow-path controls passed. After snapshotting the reference-stack refs and metadata, `gh stack rebase --no-trunk --downstack` and its conflict continuations carried the existing production correction from #15274 through #15275 to #15276. The epilogue consumes `cReg`, reads the original C and writes final D. The shared production correction remains owned once by #15274, now at commit `516aacf703c`; #15276 adds the complex regression.

**Restack integration:** preserved #15266's double CLI scalar parsing and shared `TypeInfo` metadata, migrated the later Float8 call sites and complex helper, and retained all option checks with their expected diagnostics. Kept #15275's Scalar-only Half-to-Float8 fallback, Vector eligibility, non-finite output-cast tests and runtime selector diagnostics. `git range-diff` shows the numerical fixes replayed unchanged, and the archive was byte-identical to the previous #15275 head before this entry was updated. Only #15274, #15275 and #15276 heads changed; #15259, #15266, #15277, #15278 and unrelated branches were unchanged. The deferred Float8 correctness branch remains at `6d06af3ff59c224c92a468920120797edd2212fa` and has not been inserted or published.

**Validation:** Release builds of `tensilelite-tests` and `cpu-gemm-driver` succeeded. All **41 focused reference gtests** and **729 CPU driver CTests** passed, with zero failures or skips. The regression is selected by the existing standard-tier wildcard for `tensilelite-tests` and needs no new CI registration. Build logs, red/green results, XML reports, ref snapshots and range diffs are saved under `/tmp/tl-pr15276-complex-alias-20261117/`. Scoped root and hipBLASLt pre-commit hooks and the staged whitespace check passed; clang-format 18.1.4 was applied to the added C++ ranges. The component hook selected no Python tests. These are local CPU results. The published #15276 head remains `5b4f85f05f2977d06f67cf5405eff41da6800f93`; no code was pushed.

**Draft reply to newling — not posted:**

> @newling Agreed. #15276 now inherits the shared C/D aliasing correction from #15274. I added ComplexInPlaceCDPreservesBetaInput for complex float and complex double, covering your `(11,1)` example and a case with imaginary A/B values and complex alpha/beta. Both cases failed on the old fast path and pass with the inherited fix. The production correction stays in #15274; this PR adds the complex regression.

#### Refresh Test Result with completed validation — published

[Tony's first description note](https://github.com/ROCm/rocm-libraries/pull/15276#pullrequestreview-5897490033) asks to replace the claims that this split branch has not been built/tested and that all checks are pending. The pre-update snapshot confirmed those sentences in the GitHub description at published head `5b4f85f05f2977d06f67cf5405eff41da6800f93`.

**Verified evidence:** the [Math CI TensileLite unit/CTest job](https://math-ci.amd.com/job/rocm-libraries/job/tensilelite-unit-codecov/job/hipblaslt/job/PR-15276/7/stages/) reports success for this head. The [TheRock gfx94X common-test log](https://github.com/ROCm/rocm-libraries/actions/runs/42914667575/job/130304234756) records passes for all five complex configs: `gemm/cgemm.yaml`, `gemm/zgemm.yaml`, `gemm/bufferLoadStore0.yaml`, `streamk/sk_cgemm_quick.yaml` and `streamk/sk_zgemm_quick.yaml`. The complete common-test run reports 164 passed and 570 skipped; those skips are other configurations, not these five. The local results above cover the newly restacked code and regression; remote CI has not run those unpushed changes.

**Agreed implementation:** updated only Test Result on GitHub and verified an exact read-back. Replaced the stale build/test and pending-check claims with the linked CTest and five-config results at the published SHA. Added local validation at `62b23ec6fe7d2ed593b52f531765babf1b83b806`, explicitly stating that those commits remain unpushed and have not run PR CI. Preserved the original-branch timing paragraph, benchmark numbers and shared-host/single-run limitations. Other description sections and the published code head were unchanged. No review reply was posted. Before/after descriptions, the PATCH payload and read-back are saved under `/tmp/tl-pr15276-description-results-20261117/`.

**Published Test Result text:**

```markdown
## Test Result

At published head `5b4f85f05f2977d06f67cf5405eff41da6800f93`, the [Math CI TensileLite unit/CTest job](https://math-ci.amd.com/job/rocm-libraries/job/tensilelite-unit-codecov/job/hipblaslt/job/PR-15276/7/stages/) passed. [TheRock’s gfx94X common-test job](https://github.com/ROCm/rocm-libraries/actions/runs/42914667575/job/130304234756) ran and passed all five complex configs: `gemm/cgemm`, `gemm/zgemm`, `gemm/bufferLoadStore0`, `streamk/sk_cgemm_quick` and `streamk/sk_zgemm_quick`.

Local Release validation of the restacked branch at `62b23ec6fe7d2ed593b52f531765babf1b83b806` passed 41 focused reference gtests and 729 CPU driver CTests, with zero failures or skips, including the new complex C/D aliasing regression. Scoped root and hipBLASLt pre-commit checks passed. These local commits remain unpushed; PR CI has not run them.

Time saved: about 0.53 s of CPU reference time over the 5 complex configs (975.7 ms to 445.0 ms in total), with wall-time changes within noise. Measured on the original branch on a shared gfx950 MI355X host, 25 cores, `OMP_NUM_THREADS=25`, one run per side, replaying the run phase with the previous commit's client and this one.

Other results recorded on the original branch, same host:

- `cpu-gemm-driver`: complex float 256x256x4096 NN 62.2 ms to 37.9 ms; complex double 128x128x4096 with both operands conjugated 32.0 ms to 13.5 ms. All exact.
- In the replay, all 315 complex reference calls took the fast path, and all 5 configs passed.
- All `CPUGemm.*` tests passed (700, at 1, 3, 4 and 32 threads); full CTest 1356 of 1356. The 647 earlier `CPUGemm` commands gave the same output.
```

#### Remove the incorrect alpha-zero distinction — published

[Tony's second description note](https://github.com/ROCm/rocm-libraries/pull/15276#pullrequestreview-5897490033) points out that the fast path also skips the K reduction when alpha is zero. The pre-update description said: “With `alpha == 0`, the slow path skips the K loop, while the fast path computes `0 * sum`.”

**Verified source:** at the published SHA, [Reference.cpp:1708](https://github.com/ROCm/rocm-libraries/blob/5b4f85f05f2977d06f67cf5405eff41da6800f93/projects/hipblaslt/tensilelite/client/src/Reference.cpp#L1708) sets `doContraction` from `effectiveAlpha != AccumT(0)`. The A/B shadow copies are guarded by that flag at line 1733, and the ordinary K loop is guarded at line 1932. The tile sum starts at zero at line 1870. The local restacked source retains the same guards. Thus, when the selected alpha is zero, the fast path skips A/B contraction as the slow path does.

**Agreed implementation:** removed exactly that sentence from #15276's GitHub description and verified that the full body matched the proposed text on read-back. Recorded the agreement here. The remaining non-finite behavior statement is unchanged and remains available for its own assessment. No production code changed, no code was pushed and no reply was posted. The published head remains `5b4f85f05f2977d06f67cf5405eff41da6800f93`. Before/after descriptions and the PATCH/read-back records are saved under `/tmp/tl-pr15276-description-alpha0-20261117/`.

#### Link the direct dependency on #15275 — published

[Tony's third description note](https://github.com/ROCm/rocm-libraries/pull/15276#pullrequestreview-5897490033) asks for a “Depends on #15275” line in Related. The pre-update GitHub description had only the work-tracking entry there.

**Verified relationship:** #15276's base is `users/alvasile/tl-speedup-25-reference-fast-path-float8-output`, the open [#15275](https://github.com/ROCm/rocm-libraries/pull/15275). It remains the immediate dependency in the planned stack, including after the earlier correctness PRs are inserted below it.

**Agreed implementation:** added the following line under Related on GitHub, retaining the work-tracking entry:

```markdown
- Depends on [#15275](https://github.com/ROCm/rocm-libraries/pull/15275).
```

Verified that the full GitHub body matched the proposed text on read-back. The published head and base are unchanged; no code was pushed and no review reply was posted. Before/after descriptions and the PATCH/read-back records are saved under `/tmp/tl-pr15276-description-dependency-20261117/`.

#### Correct the manual command so it runs the reference gtests — published

[Newling's initial linked review, Tests section](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15276.md#tests) notes that a CTest name regex containing `ReferenceFastPath` does not select those gtests in a category-based build. #15276's pre-update Test Plan used `ctest ... -R 'CPUGemm|ReferenceFastPath|ReferenceMXFastPath'` and claimed it ran both the CPU driver tests and fast-path gtests.

**Verified selection:** the documented CTest filter selects **729 CPU driver tests and zero gtest categories** in the existing review build. Calling `tensilelite-tests` directly with `ReferenceFastPath.*:ReferenceMX*.*:*ReferenceMX*/*` selects **40 gtests**, including the complex in-place regression and six parameterized MX free-tile cases. All 40 passed in a fresh run. The six parameterized cases were outside the earlier 41-test filter, so this run supplies their current local validation. The earlier 41-test result remains an accurate report of its selected tests. Discovery records and the new XML/log are saved under `/tmp/tl-pr15276-description-dependency-20261117/`.

**Agreed implementation:** retained the configure/build steps and published separate CPU driver CTest and direct gtest commands in Test Plan:

```bash
export OMP_NUM_THREADS=4 OMP_THREAD_LIMIT=8 OMP_DYNAMIC=FALSE KMP_BLOCKTIME=0
ctest --test-dir "$BUILD/tensilelite/tests" --output-on-failure \
  -R '^CPUGemm[.]' --no-tests=error
"$BUILD/tensilelite/tests/tensilelite-tests" \
  --gtest_filter='ReferenceFastPath.*:ReferenceMX*.*:*ReferenceMX*/*'
```

The gtests' existing standard-tier registration and #15259's local installed-suite integration already cover their shared-CI execution. Verified an exact read-back of the full updated description and confirmed that only Test Plan changed. The published code head remains `5b4f85f05f2977d06f67cf5405eff41da6800f93`; no code was pushed and no reply was posted. Before/after descriptions and the PATCH/read-back records are saved under `/tmp/tl-pr15276-description-test-plan-20261117/`.

**Other linked suggestions already satisfied:** the local restack preserved #15266's shared `TypeInfo` metadata and migrated the later complex helper; there is no private `TypeTraits` table. All seven complex option rejections now use the common `EXPECTED_ERROR` helper and name the unsupported option or combination. They passed in the 729-driver run. E/gradient checks introduced by later branches remain for those branches' walkthroughs.

**Feedback refresh:** a fresh GitHub read found Tony's existing approval review, no inline comments and no human discussion comments on #15276. Bots remain outside the walkthrough per the user's earlier instruction. The immutable linked reviews are preserved below.

#### Retain the documented non-finite complex-arithmetic boundary — accepted; description narrowed

[Newling's initial review, Scope decisions](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15276.md#scope-decisions) says: “Non-finite complex multiplication remains a documented compatibility boundary. The finite-data comparison does not establish identical behavior for overflow, NaNs, or every compiler/platform.” His [final correctness review](https://github.com/newling/agent-public-reviews/blob/ede5c5f065835ae571ce0647b4e07af2c6cb8d64/rocm-libraries/review_pr15276_correctness_20261113.md#scope-decisions) also leaves that boundary unchanged.

**Assessment:** this is an explicit scope decision in the linked reviews, rather than another requested production correction. The description retains its caveat about differing non-finite results. `ComplexMatchesSlowPathExactly` uses bounded finite random inputs; the complex arithmetic and alias regressions use finite worked examples. Their passing results do not qualify overflow/NaN behavior or every compiler/platform. No new non-finite complex comparison or platform qualification is claimed here.

**Agreed and published:** retained the documented non-finite boundary and narrowed the GitHub description's equality claim to the tested finite-result cases. Newling requested no additional code or tests for this scope decision; the wording correction was our additional recommendation, approved by the user. Technical Details now says:

> The tested finite-result cases match bitwise. Non-finite cases, including overflow from finite inputs, can differ between reference paths.

The Risk level paragraph now uses the same scope and explicitly notes that overflow and other non-finite behavior can differ between paths. Verified an exact read-back of the full description and unchanged head, base and title. No code was pushed, no review reply was posted and no new numerical validation was performed. Before/after descriptions and publication records are saved under `/tmp/tl-pr15276-description-finite-20261117/`.

Broader complex-arithmetic qualification would need a separate test matrix and decision. All requested actionable corrections for #15276 are now implemented locally or published as description edits; code publication and fresh PR CI remain pending. #15277 is the next branch in the walkthrough.


### Reviews (1)

#### tony-davis — APPROVED — review 5897490033

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15276#pullrequestreview-5897490033) · [@tony-davis](https://github.com/tony-davis) · Submitted: 2026-11-13 20:21:20 UTC

Reviewed commit: `5b4f85f05f2977d06f67cf5405eff41da6800f93`.

> Hi Alex! Thanks for this one; it was a pleasant read. I went through the complex path against the slow path and I think it's faithful. The fast path sums `op(A) * op(B)` in plain K order from zero, applies `alpha * sum + beta * C` in the same order, conjugates in the shadow copy only when `aOps()`/`bOps()` ask for it, and rejects every epilogue input the slow path reads as zero. The tests back that up well: the hand-computed conjugation cases with complex alpha and beta would catch a missing or extra `conj`, and `ComplexMatchesSlowPathExactly` would catch a reordered sum. I also liked that `--tryFastPath` on an ineligible problem is an error in the driver, so the "fast" CTest cases can't quietly run the slow path.
>
> CI looks good for this change. `tensilelite-unit-codecov` ran the CTest suite at this head and passed, and TheRock's gfx94X `tensilelite-common` ran all five complex configs (`cgemm`, `zgemm`, `bufferLoadStore0`, `sk_cgemm_quick`, `sk_zgemm_quick`) against the new reference and passed. The `gfx1250-strict` failure is a timeout in `decouple_pgr_tdm_fuse_gfx1250.yaml`, which isn't complex, and #15277 passed that lane with this same code, so I don't think it's yours. `therock-pr-bot` died on a GitHub 503; a re-run should clear it.
>
> I'm approving. A few small things for the description (non-blocking):
>
> 1. Test Result still says the branch hasn't been built or tested and that checks are pending. Since this becomes the commit message, I'd swap in what actually ran at this head (the CTest job, the five complex configs) and keep the original-branch timings as timings.
> 2. I think the `alpha == 0` sentence is off. The fast path also skips the K loop when alpha is zero (`doContraction` in `solveCPUFast`), so I'd just drop it.
> 3. A "Depends on #15275" line in Related would help anyone who lands here from the commit history.
>
> Merge order is up to the stack below this, of course.
>
> <!-- rocm-pr-quality:attestation v1 -->
> > **Reviewed with the ROCm PR-quality skill.**
> > - Skill: `rocm-pr-quality` (base) - overlays: `hipblaslt-pr-quality`
> > - Action: `review` - Result: **APPROVED** (`0` BLOCKING; `1` IMPORTANT)
> > - Risk: `2`
> > - Reviewed commit: `5b4f85f05f2977d06f67cf5405eff41da6800f93`
> > - Attestation: `a346b8f9`
> >
> > <sub>Machine-checkable attestation. The digest ties this review to this PR; later commits are
> > allowed. The reviewed commit is informational (what the review looked at).</sub>

### Inline review threads (0 threads, 0 comments)

No inline review threads at collection time.

### Newling’s linked reviews and cross-PR remarks

Posted on #15259: [initial round](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5867396457) · [final round](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5899752492). The [shared testing and design assessment](#shared-assessment) applies to this PR too.

**Newling’s own remark about #15276** ([original post](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5899752492)):

> #15276 looks like the same fix suggested as #15274

#### Initial agent review, linked November 12

[Original on GitHub](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15276.md) · [Newling’s source post](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5867396457)

Source revision: `30df5bab4294f839ef85a91c732c36c92c0821ca`. File: `rocm-libraries/review_pr15276.md`.

> This is a review from an agent with an automatic prompt from the reviewer
>
> **PR reviewed:** [ROCm/rocm-libraries#15276](https://github.com/ROCm/rocm-libraries/pull/15276)
>
> **Submitted head:** `5b4f85f05f2977d06f67cf5405eff41da6800f93`. **Incremental base:** `b6828c46316720641f2e3f15a1320201d4f1c34b`.
>
> Independent review of the incremental change and its interaction with the CPU-reference stack, emphasizing test adequacy, runtime, overlap, design reuse, and unsupported cases. Inputs were the public descriptions, code, repository guidance and CI status/logs; target-PR discussions and existing reviews were not read.
>
> **Suggestion branch:** [review/pr15276-suggestions](https://github.com/newling/rocm-libraries/tree/review/pr15276-suggestions), based on the submitted head above; final head [7c3393ef929f82a8f899ded5a245816265954a1d](https://github.com/newling/rocm-libraries/commit/7c3393ef929f82a8f899ded5a245816265954a1d).
>
> ## Tests
>
> Submitted head: `cpu-gemm-driver`, `tensilelite-tests`, and `tensilelite-client` built successfully; **700 driver CTests and 35 focused reference gtests passed**. The driver suite took 4.54 s with four CTest workers. Suggestion implementation: the same targets built; **700 driver CTests and 36 focused reference gtests passed**. Later comment-only commits, where present, do not change that execution result.
>
> Validation used a Release ROCm compiler build. Controlled runs set `OMP_THREAD_LIMIT=8`, `OMP_NUM_THREADS=4`, and `KMP_BLOCKTIME=0`; the original stack-tip driver timing predates the thread-limit setting. These are host numerical tests, not GPU-kernel performance measurements. Root/component hook invocations and full-stack whitespace checks were clean; the formatting hooks opt out of these C++/CMake paths, and no Python tests were selected by the component hook. No local GPU or sanitizer run is claimed.
>
> The gtests were run directly with `--gtest_filter=ReferenceFastPath.*:ReferenceMX*.*:*ReferenceMX*/*`. In the category-based build, a CTest name regex containing `ReferenceFastPath` does not select them. Driver CTests are build-tree tests and are not installed into the TheRock test package; Math CI's unfiltered `coverage-cpp` lane runs them but is informational. The gtest categories include standard/comprehensive/full, not quick.
>
> ## Summary
>
> Using the existing tiled solver with complex accumulator types and applying conjugation during shadow-buffer loading is a coherent extension. The eligibility exclusions correctly avoid claiming support for complex epilogues whose slow helpers do not implement the corresponding values.
>
> The 53 added driver cases cover both widths, transpose, conjugation, alpha/beta and batching. The three new gtests supply direct complex arithmetic/conjugation checks and a finite-data comparison. These are complementary; an in-place beta test is the important omission.
>
> ## Actionable items
>
> ### Preserve aliased complex C through the epilogue
>
> [client/src/Reference.cpp:1958](https://github.com/ROCm/rocm-libraries/blob/5b4f85f05f2977d06f67cf5405eff41da6800f93/projects/hipblaslt/tensilelite/client/src/Reference.cpp#L1958) writes the tile sums into D before using C. When complex C and D share storage, the complex shadow buffer aliases C and the beta input is overwritten.
>
> A 1×1 complex-float example with A=(2,0), B=(3,0), C=D initially (5,1), and alpha=beta=(1,0) gives **(12,0)** on the submitted fast path rather than **(11,1)**. The probe ran against the submitted stack tip; the complex path is introduced here.
>
> Suggestion commit [7c3393ef929](https://github.com/newling/rocm-libraries/commit/7c3393ef929f82a8f899ded5a245816265954a1d) reads the sum directly from `cReg` and writes D after reading C. `ComplexInPlaceCDPreservesBetaInput` checks both complex float and complex double with nonzero imaginary components.
>
> ## Suggestions
>
> ### Correct the stale alpha-zero description
>
> The PR description says the fast path computes `0 * sum` while the slow path skips contraction. The reviewed base already contains the effective-alpha-zero guard from [#13482](https://github.com/ROCm/rocm-libraries/pull/13482). Update that claim to describe the current base. No replacement description was drafted or published.
>
> The generic driver rejection checks would benefit from the shared diagnostic helper implemented in #15278 ([66ef187d435](https://github.com/newling/rocm-libraries/commit/66ef187d43535cacaba49a328b77b0388909291e)). Complex bias/scaling/E/gradient rejections should continue identifying the unsupported feature, not just returning failure.
>
> ## Scope decisions
>
> This suggestion repeats #15274's common epilogue correction because each review branch is independently based on its own submitted head. When combining them, keep that production correction once and both type-specific regressions.
>
> Non-finite complex multiplication remains a documented compatibility boundary. The finite-data comparison does not establish identical behavior for overflow, NaNs, or every compiler/platform. No GPU execution or platform-wide complex-arithmetic qualification was performed locally.
>
> The exact native probe source and build recipe for the numerical counterexamples are preserved in the [cross-PR appendix](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_cpu_reference_stack_15259_15278.md#appendix-native-counterexample-source-and-setup). Committed regressions cover the corresponding contracts.
>
> ## Suggestion branch handoff
>
> | Suggestion commit | Item |
> | --- | --- |
> | [7c3393ef929](https://github.com/newling/rocm-libraries/commit/7c3393ef929f82a8f899ded5a245816265954a1d) | preserve aliased complex C before writing D |
>
> The suggestion branches are published. Each branch contains its own corrections on its own submitted head, not the other review branches' fixes. Integrate shared changes in stack order, including the alias fix once and later `TypeInfo` call sites. PR-description corrections are reported for the author to make; none were published. See [the cross-PR assessment](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_cpu_reference_stack_15259_15278.md) for test overlap, CI limits and integration notes.
>

#### Final correctness review, linked November 14 (document dated November 13)

[Original on GitHub](https://github.com/newling/agent-public-reviews/blob/ede5c5f065835ae571ce0647b4e07af2c6cb8d64/rocm-libraries/review_pr15276_correctness_20261113.md) · [Newling’s source post](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5899752492)

Source revision: `ede5c5f065835ae571ce0647b4e07af2c6cb8d64`. File: `rocm-libraries/review_pr15276_correctness_20261113.md`.

> This is a review from an agent with an automatic prompt from the reviewer
>
> **PR reviewed:** [ROCm/rocm-libraries#15276](https://github.com/ROCm/rocm-libraries/pull/15276)
>
> **Submitted head:** `5b4f85f05f2977d06f67cf5405eff41da6800f93`. **Incremental diff base:** `b6828c46316720641f2e3f15a1320201d4f1c34b`.
>
> **Scope:** Focused follow-up on consequential CPU-reference errors and regression-test failures, including the existing review discussion. Head and discussion were unchanged at the 2026-11-13 23:09 UTC check. Earlier validation is reused where the tested code is unchanged.
>
> **Suggestion branch:** [review/pr15276-correctness-20261113](https://github.com/newling/rocm-libraries/tree/review/pr15276-correctness-20261113), based on the submitted head; final head [7c3393ef929](https://github.com/newling/rocm-libraries/commit/7c3393ef929f82a8f899ded5a245816265954a1d). Commit-to-item mapping is given below.
>
> ## Tests
>
> Reused validation for the unchanged code: submitted head, 700 driver CTests and 35 reference tests passed; retained fix, 700 driver CTests and 36 reference tests passed. The driver, tests and client built. A direct probe against the submitted stack reproduced the alias error; the committed regression covers complex float and complex double. No new GPU or performance run was made.
>
> ## Summary
>
> The tiled solver can perform complex multiplication and conjugate operands while loading them. The central correctness gap is ordinary GEMM input preservation: the result buffer is written before a potentially aliased beta input is read.
>
> ## Actionable items
>
> ### Preserve complex C until the epilogue has consumed it
>
> At [client/src/Reference.cpp:1958](https://github.com/ROCm/rocm-libraries/blob/5b4f85f05f2977d06f67cf5405eff41da6800f93/projects/hipblaslt/tensilelite/client/src/Reference.cpp#L1958), intermediate tile sums are written to D before reading C. When C and D share storage, the complex shadow buffer refers directly to C, so this overwrites the beta input.
>
> With A=(2,0), B=(3,0), initial C=D=(5,1), and alpha=beta=(1,0), the reference should return **(11,1)**. The submitted fast path returns **(12,0)**. This changes both the real result and the preserved imaginary component.
>
> Fix [7c3393ef929](https://github.com/newling/rocm-libraries/commit/7c3393ef929f82a8f899ded5a245816265954a1d) consumes the tile sum directly and writes D only after reading C. `ComplexInPlaceCDPreservesBetaInput` uses nonzero imaginary components in A, B and C and checks both complex widths. The original probe is preserved in the [native counterexample source and setup](https://github.com/newling/agent-public-reviews/blob/ede5c5f065835ae571ce0647b4e07af2c6cb8d64/rocm-libraries/review_cpu_reference_stack_15259_15278.md#appendix-native-counterexample-source-and-setup).
>
> ## Suggestions
>
> None in this focused review.
>
> ## Scope decisions
>
> This is the same shared epilogue defect exposed by the Int8 extension in #15274. Once that production correction is integrated, keep the complex regression without applying the correction twice. This review does not change the documented non-finite complex-arithmetic boundary.
>
> **Suggestion summary:** [review/pr15276-correctness-20261113](https://github.com/newling/rocm-libraries/tree/review/pr15276-correctness-20261113) contains [7c3393ef929](https://github.com/newling/rocm-libraries/commit/7c3393ef929f82a8f899ded5a245816265954a1d), the common alias fix and complex regression. The existing approving review does not establish that the alias counterexample is fixed; the submitted code is unchanged.
>

### General discussion comments (3, including bots)

#### therock-pr-bot[bot] — comment 6692574686

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15276#issuecomment-6692574686) · [@therock-pr-bot[bot]](https://github.com/apps/therock-pr-bot) · Created: 2026-11-06 18:41:05 UTC · Updated: 2026-11-10 16:00:14 UTC

> <!-- therock-pr-bot-policy-check -->
> ### ✅ All Checks Passed — Ready for Review
>
> | Check | Status | Details |
> |---|:---:|---|
> | 📝 **PR Description** | ✅ Pass | — |
> | ⛔ **Forbidden Files** | ✅ Pass | — |
> | 🧪 **Unit Test** | ✅ Pass | — |
> | 🔎 **pre-commit** | ✅ Pass | — |
> | 🚫 **Draft PR** | 🔜 To Be Enabled | — |
> | 🚩 **Feature Flag** | 🔜 To Be Enabled | — |
> | 📊 **Code Coverage** | 🔜 To Be Enabled | — |
> | 🤖 **therock-pr-bot** | ✅ Pass | — |
>
> > 🎉 All checks passed! This PR is ready for review.
>
> 📖 **Need help?** See the [Policy FAQ](https://github.com/ROCm/rocm-libraries/blob/develop/docs/LIBRARIES_PR_BOT_FAQ.md) for details on every check and how to fix failures.
>
> 🙋 **[Wish to Override Policy?](https://github.com/ROCm/rocm-libraries/blob/develop/docs/LIBRARIES_PR_BOT_FAQ.md#-wish-to-override-the-policy-process-and-get-unblocked)**

#### therock-pr-bot[bot] — comment 6692790151

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15276#issuecomment-6692790151) · [@therock-pr-bot[bot]](https://github.com/apps/therock-pr-bot) · Created: 2026-11-06 18:52:01 UTC · Updated: 2026-11-10 16:00:15 UTC

> <!-- therock-pr-bot-fix-policies -->
> 🎉 All checks passed! This PR is ready for review.

#### codecov[bot] — comment 6787577267

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15276#issuecomment-6787577267) · [@codecov[bot]](https://github.com/apps/codecov) · Created: 2026-11-11 05:06:37 UTC · Updated: 2026-11-11 06:50:44 UTC

> ## [Codecov](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15276?dropdown=coverage&src=pr&el=h1&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) Report
> :x: Patch coverage is `85.50725%` with `10 lines` in your changes missing coverage. Please review.
>
> | [Files with missing lines](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15276?dropdown=coverage&src=pr&el=tree&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | Patch % | Lines |
> |---|---|---|
> | [...cts/hipblaslt/tensilelite/client/src/Reference.cpp](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15276?src=pr&el=tree&filepath=projects%2Fhipblaslt%2Ftensilelite%2Fclient%2Fsrc%2FReference.cpp&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm#diff-cHJvamVjdHMvaGlwYmxhc2x0L3RlbnNpbGVsaXRlL2NsaWVudC9zcmMvUmVmZXJlbmNlLmNwcA==) | 85.51% | [2 Missing and 8 partials :warning: ](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15276?src=pr&el=tree&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
>
> :x: Your project check has failed because the head coverage (70.39%) is below the target coverage (80.00%). You can increase the head coverage or adjust the [target](https://docs.codecov.com/docs/commit-status#target) coverage.
>
> <details><summary>Additional details and impacted files</summary>
>
>
>
> ```diff
> @@                                       Coverage Diff                                       @@
> ##           users/alvasile/tl-speedup-25-reference-fast-path-float8-output   #15276   +/-   ##
> ===============================================================================================
>   Coverage                                                           77.64%   77.64%
> ===============================================================================================
>   Files                                                                3002     3002
>   Lines                                                              521809   521870   +61
>   Branches                                                            81374    81395   +21
> ===============================================================================================
> + Hits                                                               405126   405184   +58
> + Misses                                                              88882    88868   -14
> - Partials                                                            27801    27818   +17
> ```
>
> | [Flag](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15276/flags?src=pr&el=flags&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | Coverage Δ | | *Carryforward flag |
> |---|---|---|---|
> | [TensileLite-CPP](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15276/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `53.78% <85.51%> (+0.13%)` | :arrow_up: | |
> | [TensileLite-Unit](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15276/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `83.84% <ø> (ø)` | | |
> | [hipBLAS](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15276/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `90.62% <ø> (ø)` | | Carriedforward from [ffab621](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/ffab621a55274338588213b0a1b9505d317cca4c?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipBLASLt](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15276/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `70.39% <ø> (-0.07%)` | :arrow_down: | |
> | [hipCUB](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15276/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `82.68% <ø> (ø)` | | Carriedforward from [ffab621](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/ffab621a55274338588213b0a1b9505d317cca4c?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipDNN](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15276/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `87.34% <ø> (ø)` | | Carriedforward from [ffab621](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/ffab621a55274338588213b0a1b9505d317cca4c?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipFFT](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15276/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `42.65% <ø> (ø)` | | Carriedforward from [ffab621](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/ffab621a55274338588213b0a1b9505d317cca4c?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipRAND](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15276/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `76.12% <ø> (ø)` | | Carriedforward from [ffab621](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/ffab621a55274338588213b0a1b9505d317cca4c?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipSOLVER](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15276/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `69.67% <ø> (ø)` | | Carriedforward from [ffab621](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/ffab621a55274338588213b0a1b9505d317cca4c?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipSPARSE](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15276/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `87.03% <ø> (ø)` | | Carriedforward from [ffab621](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/ffab621a55274338588213b0a1b9505d317cca4c?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocALUTION](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15276/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `86.47% <ø> (ø)` | | Carriedforward from [ffab621](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/ffab621a55274338588213b0a1b9505d317cca4c?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocBLAS](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15276/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `69.16% <ø> (ø)` | | Carriedforward from [ffab621](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/ffab621a55274338588213b0a1b9505d317cca4c?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocFFT](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15276/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `48.81% <ø> (ø)` | | Carriedforward from [ffab621](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/ffab621a55274338588213b0a1b9505d317cca4c?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocRAND](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15276/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `57.32% <ø> (ø)` | | Carriedforward from [ffab621](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/ffab621a55274338588213b0a1b9505d317cca4c?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocSOLVER](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15276/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `76.66% <ø> (ø)` | | Carriedforward from [ffab621](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/ffab621a55274338588213b0a1b9505d317cca4c?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocSPARSE](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15276/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `83.86% <ø> (ø)` | | Carriedforward from [ffab621](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/ffab621a55274338588213b0a1b9505d317cca4c?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocSPARSE-unit](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15276/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `79.82% <ø> (ø)` | | Carriedforward from [ffab621](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/ffab621a55274338588213b0a1b9505d317cca4c?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocThrust](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15276/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `93.15% <ø> (ø)` | | Carriedforward from [ffab621](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/ffab621a55274338588213b0a1b9505d317cca4c?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
>
> *This pull request uses carry forward flags. [Click here](https://docs.codecov.io/docs/carryforward-flags?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) to find out more.
>
> | [Files with missing lines](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15276?dropdown=coverage&src=pr&el=tree&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | Coverage Δ | |
> |---|---|---|
> | [...cts/hipblaslt/tensilelite/client/src/Reference.cpp](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15276?src=pr&el=tree&filepath=projects%2Fhipblaslt%2Ftensilelite%2Fclient%2Fsrc%2FReference.cpp&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm#diff-cHJvamVjdHMvaGlwYmxhc2x0L3RlbnNpbGVsaXRlL2NsaWVudC9zcmMvUmVmZXJlbmNlLmNwcA==) | `52.89% <85.51%> (+1.43%)` | :arrow_up: |
>
> ... and [8 files with indirect coverage changes](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15276/indirect-changes?src=pr&el=tree-more&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm)
> </details>
> <details><summary> :rocket: New features to boost your workflow: </summary>
>
> - :snowflake: [Test Analytics](https://docs.codecov.com/docs/test-analytics): Detect flaky tests, report on failures, and find test suite problems.
> - :package: [JS Bundle Analysis](https://docs.codecov.com/docs/javascript-bundle-analysis): Save yourself from yourself by tracking and limiting bundle sizes in JS merges.
> </details>

<a id="pr-15277"></a>

## PR #15277 — perf(tensilelite): write E and run gradient GEMMs on the CPU reference fast path

[GitHub PR](https://github.com/ROCm/rocm-libraries/pull/15277) · State: **OPEN**

Branch: `users/alvasile/tl-speedup-27-reference-fast-path-use-e-gradient`. Base: `users/alvasile/tl-speedup-26-reference-fast-path-complex`. Head at collection: `46ce0b0c3a676b3878f652e4dbf136bdb1f30f9f`.

### Feedback walkthrough — November 17, 2026

#### Unsupported bias-gradient cleanup — agreed separate correctness PR

**Assessment:** this defect predates #15277. Verified the discarded exceptions and D-workspace double-free at #15277's submitted head and in its published #15276 base (`5b4f85f05f2977d06f67cf5405eff41da6800f93`). Newling's patch addresses all A/B/D reduction errors using the existing helper and automatic workspace ownership. No new local reproducer or sanitizer run was performed in this walkthrough.

**Agreed resolution:** file a GitHub issue tied to #15277, reply to Tony and Newling explaining that this is pre-existing, and create a separate correctness PR at the top of this stack using Newling's suggestion. That PR must close the issue and carry direct-reference regression coverage. See [Future work](#future-work), item 6. The fix is deferred; this record does not mark the production defect resolved.

##### Filed GitHub issue — #16120

[GitHub issue #16120](https://github.com/ROCm/rocm-libraries/issues/16120) · Assignee: `Alex-Vasile` · State: Open. Published after exact-text approval and verified by read-back.

**Title:** [TensileLite] Unsupported bias-gradient reductions double-free workspace or discard errors

Found while reviewing [#15277](https://github.com/ROCm/rocm-libraries/pull/15277) at `46ce0b0c3a676b3878f652e4dbf136bdb1f30f9f`. This is a pre-existing slow-reference defect: the [faulty code is already present in its published base](https://github.com/ROCm/rocm-libraries/blob/5b4f85f05f2977d06f67cf5405eff41da6800f93/projects/hipblaslt/tensilelite/client/src/Reference.cpp#L2642).

When `ReductionCPU` returns an unsupported-shape error, the slow reference constructs `std::runtime_error` without throwing it. The D-source branch frees its workspace before discarding the error and later frees it again. The A/B branches also discard their errors.

Reproducer: call `SolveGemmCPU` directly for a Float GEMM with M=N=K=1, two batches, A=B=1, C=0, alpha=1, beta=0, gradients enabled and a bias-gradient output initialized to 99. Select D, A or B as the bias source and try both execution preferences. Newling’s direct probe reports a double-free abort for D; A/B return without reporting the unsupported shape. The driver rejects this combination before reaching the reference, so the regression must call the reference directly.

Expected: a clear unsupported-shape exception, unchanged bias and no double-free. The existing reduction limitation remains: a 2-D tensor or a 3-D tensor with one batch.

Fix plan: create a separate PR at the top of [stack #15515](https://github.com/ROCm/rocm-libraries/stack/15515), after #15278, using [Newling’s suggestion `66106f99d91`](https://github.com/newling/rocm-libraries/commit/66106f99d916da886beecc39e464681596074b6d). Share `reduceBiasGradient`, give the workspace automatic ownership and propagate reduction errors. Include direct-reference regressions for A/B/D with both execution preferences, demonstrate failure before and success after the fix, and run them in shared CI. The new PR will close this issue.

Related feedback: [Tony’s inline comment](https://github.com/ROCm/rocm-libraries/pull/15277#discussion_r4568214216) and [Newling’s linked correctness review](https://github.com/newling/agent-public-reviews/blob/ede5c5f065835ae571ce0647b4e07af2c6cb8d64/rocm-libraries/review_pr15277_correctness_20261113.md#actionable-items).

##### Posted reply to Tony — verified

[Posted reply](https://github.com/ROCm/rocm-libraries/pull/15277#discussion_r4602603298) to [Tony's inline comment](https://github.com/ROCm/rocm-libraries/pull/15277#discussion_r4568214216). Verified its exact body and parent comment by read-back:

> This is a pre-existing slow-reference issue, also present below #15277. We will track it in https://github.com/ROCm/rocm-libraries/issues/16120 and handle it in a separate PR added at the top of [stack #15515](https://github.com/ROCm/rocm-libraries/stack/15515), using [Newling’s suggestion `66106f99d91`](https://github.com/newling/rocm-libraries/commit/66106f99d916da886beecc39e464681596074b6d). That PR will fix the double-free and discarded reduction errors, add direct-reference regressions, and close the issue.

##### Posted reply to Newling — verified

[Posted on #15277](https://github.com/ROCm/rocm-libraries/pull/15277#issuecomment-6954240221), addressed to Newling and identifying the cleanup finding from his stack-level linked review. Verified the exact body by read-back:

> @newling, regarding the cleanup finding in your linked #15277 review:
>
> This is a pre-existing slow-reference issue, also present below #15277. We will track it in https://github.com/ROCm/rocm-libraries/issues/16120 and handle it in a separate PR added at the top of [stack #15515](https://github.com/ROCm/rocm-libraries/stack/15515), using [Newling’s suggestion `66106f99d91`](https://github.com/newling/rocm-libraries/commit/66106f99d916da886beecc39e464681596074b6d). That PR will fix the double-free and discarded reduction errors, add direct-reference regressions, and close the issue.

Publication records are saved under `/tmp/tl-pr15277-bias-gradient-followup-20261117/`. The issue and replies are published; the correctness PR has not been created. No production code was changed or pushed, and #15277's code head and base remain unchanged.

#### Non-HPA bias-gradient accumulation — agreed and implemented locally

[Tony's inline comment](https://github.com/ROCm/rocm-libraries/pull/15277#discussion_r4568214232) offers either keeping non-HPA gradient+bias problems on the slow path or explicitly choosing the wider float sum. [Newling's linked correctness review](https://github.com/newling/agent-public-reviews/blob/ede5c5f065835ae571ce0647b4e07af2c6cb8d64/rocm-libraries/review_pr15277_correctness_20261113.md#actionable-items) demonstrates that this difference exceeds last-bit variation and proposes the conservative eligibility guard in [de7815c2e50](https://github.com/newling/rocm-libraries/commit/de7815c2e5066f6b2ff76e589732a3b8a671e402).

**Source assessment and reproduced failure:** non-HPA means higher-precision accumulation is disabled. The slow dispatch uses Half accumulation for `H_H_H` and BFloat16 accumulation for `B_B_S`; the default `ReferenceSolution` accumulator is the D type. The fast reduction uses float. A new regression at local pre-fix head `4430f441cf54dfd80da9a29e0edaafa213a96cc8` reproduced both differences with all-one inputs and M=K=1: Half at N=4097 returned bias 4096 when fast execution was requested, versus the expected narrow sum 2048; BFloat16 at N=513 returned 512 versus 256. Explicit slow execution passed both cases, and every D element was 1. The test checks eligibility after the numerical assertions so it exposes the wrong values before the fix.

**Agreed resolution:** implemented Newling's type-specific guard in #15277. Gradient+bias problems with higher-precision accumulation disabled and type ID `H_H_H` or `B_B_S` stay on the slow path (`non_hpa_bias_reduction`). Both execution preferences now preserve bias 2048 for Half and 256 for BFloat16. HPA configurations remain fast-eligible and return the independently expected float sums narrowed once, 4096 and 512. Additional eligibility controls cover A/B/D bias sources and confirm that disabling either gradient or bias preserves fast eligibility. This item is separate from the cleanup follow-up tracked in #16120; that production fix remains deferred.

**Restack:** used `gh stack rebase --no-trunk --downstack` and continued the two conflicts through supported `gh stack` commands. #15277 now inherits #15276 at `429dbfe15dbf14d9ea3be8dd50a4d8277dda1bcc`, including its earlier stack fixes. Preserved the negative-test helper and all six E/gradient diagnostic checks. The review archive was byte-identical to its pre-restack copy before this entry changed. #15278, all lower branches and the deferred Float8 correctness branch retained their original heads; backup refs and records are in `/tmp/tl-pr15277-non-hpa-20261117/`.

**Validation:** rebuilt `tensilelite-tests`, `cpu-gemm-driver` and `tensilelite-client` in the reusable Release ROCm build. All 48 focused reference gtests passed, including the three new non-HPA/HPA/eligibility tests. All 795 standard-tier `CPUGemm.*` CTests passed with four CTest workers (29.06 s). Runs used `OMP_THREAD_LIMIT=8`, `OMP_NUM_THREADS=4` and `KMP_BLOCKTIME=0`. The new gtests are included in the existing standard-tier CTest suite's `--gtest_filter=*`; no extra registration is needed. Root and component pre-commit hooks passed; the component hook selected no Python unit/characterization tests for these changes. Formatted the changed C++ ranges with clang-format 18.1.4 and checked the diff for whitespace errors. These are local CPU tests. Shared CI has not run the unpushed guard, and no new GPU or sanitizer run is claimed. Build, red/green and hook logs are saved in `/tmp/tl-pr15277-non-hpa-20261117/`.

##### Draft reply to Tony — not posted

Reply to [Tony's non-HPA inline comment](https://github.com/ROCm/rocm-libraries/pull/15277#discussion_r4568214232):

> Agreed. I've implemented the guard locally for non-HPA gradient+bias problems with `H_H_H` or `B_B_S`, preserving the slow path's Half/BFloat16 accumulation. The regression reproduced bias 4096 versus 2048 for Half and 512 versus 256 for BFloat16. With the guard, both requested paths return 2048 and 256. HPA configurations remain fast-eligible and retain float accumulation. All 48 focused reference tests and 795 CPU-driver CTests passed. The code is committed locally and has not been pushed.

##### Draft reply to Newling — not posted

Post on #15277, referring to [Newling's non-HPA finding](https://github.com/newling/agent-public-reviews/blob/ede5c5f065835ae571ce0647b4e07af2c6cb8d64/rocm-libraries/review_pr15277_correctness_20261113.md#actionable-items):

> @newling, I've taken the non-HPA eligibility guard from [de7815c2e50](https://github.com/newling/rocm-libraries/commit/de7815c2e5066f6b2ff76e589732a3b8a671e402) in #15277. The new regression reproduced the Half and BFloat16 bias-sum differences before the fix; afterward, both requested paths preserve the narrow sums. HPA controls retain fast eligibility and float accumulation, and the eligibility checks cover all three bias sources. All 48 focused reference tests and 795 CPU-driver CTests passed locally. The code is committed locally and has not been pushed. The separate cleanup/error-reporting fix remains tracked in #16120.

##### Numerical-policy wording for the PR description — published November 17

The [published description](https://github.com/ROCm/rocm-libraries/pull/15277) now uses:

> Both are in one PR because they share the test driver's output comparison code. Eligible `useE` and gradient problems are now validated by the fast path. Floating-point epilogue values can differ in their last bits when evaluation order differs. Non-HPA `H_H_H` and `B_B_S` gradient problems with a bias output remain on the slow path, preserving Half/BFloat16 accumulation. HPA means higher-precision accumulation; those configurations remain fast-eligible and reduce in float.

#### Testing description — refreshed on GitHub November 17

**Agreed resolution:** refreshed [#15277's description](https://github.com/ROCm/rocm-libraries/pull/15277) in response to [Tony's review](https://github.com/ROCm/rocm-libraries/pull/15277#pullrequestreview-5897490540). Removed the stale statements that the split branch had not been built or tested. The Test Result now separates published-head CI from local commit `da6ea2191ea5782803b22bdba45832537352e0c0`, explicitly states that shared CI does not cover the unpushed guard, and leaves dedicated gfx950 gradient validation of that local revision pending. Retained the original-branch gfx950 measurements under a historical-results introduction.

**Verified CI evidence:** the GitHub Actions runs report PR head `46ce0b0c3a676b3878f652e4dbf136bdb1f30f9f`. [TheRock gfx942 common tests](https://github.com/ROCm/rocm-libraries/actions/runs/42914664204/job/130385137352) passed 164 configs and skipped 570; the three passing gradient configs were `fp16_gradient_bias.yaml`, `fp16_gradient_activation.yaml` and `fp8bf8nss_gradient_bias_b.yaml`. [gfx1250-strict common tests](https://github.com/ROCm/rocm-libraries/actions/runs/42914662388/job/130174079118) reported 236 passed, 482 skipped and 16 expected failures, with a successful job. [Math CI C++ coverage build 7](https://math-ci.amd.com/job/rocm-libraries/job/tensilelite-unit-codecov/job/hipblaslt/job/PR-15277/7/) reported 1,674 passed CTests, 4 skipped and 1 disabled, with no failures. Its tested merge was `103c270621709258b11dd7fa5daff5cadb0b4ab9`, combining the published PR head with the then-current #15276 base `d72ce11dbab4b6ad1f84b42e0350e542dfac0749`.

**Test Plan correction:** the description now builds the three relevant targets, runs the standard `CPUGemm.*` CTests and invokes the reference gtests directly with `--gtest_filter='ReferenceFastPath.*:ReferenceMX*.*:*ReferenceMX*/*'`. This avoids a CTest regex that misses the gtests in category-based builds. Updated the test summary to include the three new accumulation/eligibility tests, for eight gtests added by this PR, and published the agreed numerical-policy paragraph above.

**Publication verification:** showed the exact changed text, checked the live body/head/base against the preparation snapshot, patched only the PR body and read it back. Asserted exact equality with the prepared Markdown and unchanged remote head `46ce0b0c3a676b3878f652e4dbf136bdb1f30f9f` and base `users/alvasile/tl-speedup-26-reference-fast-path-complex`. The code was not pushed and the non-HPA replies remain drafts. Exact before/after bodies, the prepared description and CI logs are saved in `/tmp/tl-pr15277-non-hpa-20261117/`.

#### Direct dependency link — published November 17

**Agreed resolution:** added `- Depends on #15276: [complex CPU-reference fast path](https://github.com/ROCm/rocm-libraries/pull/15276).` to [#15277's Related section](https://github.com/ROCm/rocm-libraries/pull/15277), as requested in [Tony's review](https://github.com/ROCm/rocm-libraries/pull/15277#pullrequestreview-5897490540). Patched only this line, verified the exact complete body by read-back, and confirmed unchanged remote head `46ce0b0c3a676b3878f652e4dbf136bdb1f30f9f` and base `users/alvasile/tl-speedup-26-reference-fast-path-complex`. No source or stack-order change was made. Before/after records are saved in `/tmp/tl-pr15277-non-hpa-20261117/pr-dependency-*`.

**Newling's negative-test helper suggestion:** the six new E/gradient rejection cases already use the inherited `EXPECTED_ERROR` helper, which requires exit code 1 and the expected diagnostic. All six passed in the 795-driver-case validation above. The direct-reference cleanup regression remains required in the separate correctness PR tracked by #16120; driver rejection cannot validate fallback cleanup.

**Walkthrough status:** all human feedback items for #15277 have an agreed disposition. Fresh reads of this PR's reviews/comments and Newling's cross-PR reviews on #15259 found no additional undecided feedback. The non-HPA code remains local, its two replies remain drafts, and shared CI/dedicated gfx950 validation of that local change is pending. The next branch is #15278; start with Newling's exact-tail assertion finding, where all-NaN output can pass the current comparison.

### Reviews (1)

#### tony-davis — APPROVED — review 5897490540

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15277#pullrequestreview-5897490540) · [@tony-davis](https://github.com/tony-davis) · Submitted: 2026-11-13 20:21:22 UTC

Reviewed commit: `46ce0b0c3a676b3878f652e4dbf136bdb1f30f9f`.

> Hi Alex! Thanks for this. I walked the fast-path epilogue against the slow path step by step and I think it lines up. E is written after bias and before activation with the same `SetValue` cast, gradient mode skips the bias add and multiplies by `Activation(E)` under the same conditions, and the bias-gradient workspace is captured before gate residual, then reduced by the slow path's own `ReductionCPU`. The two carve-outs (mixed alpha/beta with `useE`, and reduction shapes `ReductionCPU` can't handle) are exactly the cases where the slow path fails, which seems like the right call. `GradientMatchesSlowPathExactly` is a really good test: gate residual on, padded E, every bias source and transpose, compared bit for bit.
>
> CI is green where it matters. `tensilelite-unit-codecov` ran the CTest suite at this head, TheRock's gfx94X `tensilelite-common` passed the three gradient configs it runs, and `gfx1250-strict` passed. `therock-pr-bot` is still red from an old cancelled `pre-commit` run; a re-run should clear it.
>
> I'm approving. Some things I'd still like to see before this merges (none blocking):
>
> 1. Please refresh Test Result. It still says the branch hasn't been built or tested, and the Test Plan's gfx950 gradient run has no result. I'd say what ran at this head and whether the gfx950 run happened or you're relying on `preliminary`.
> 2. Two notes inline: one on the non-HPA bias-gradient sum type, and one asking for a ticket on the slow path's dropped exception.
> 3. A "Depends on #15276" line in Related.
>
> <!-- rocm-pr-quality:attestation v1 -->
> > **Reviewed with the ROCm PR-quality skill.**
> > - Skill: `rocm-pr-quality` (base) - overlays: `hipblaslt-pr-quality`
> > - Action: `review` - Result: **APPROVED** (`0` BLOCKING; `1` IMPORTANT)
> > - Risk: `3`
> > - Reviewed commit: `46ce0b0c3a676b3878f652e4dbf136bdb1f30f9f`
> > - Attestation: `f0cb6a1a`
> >
> > <sub>Machine-checkable attestation. The digest ties this review to this PR; later commits are
> > allowed. The reviewed commit is informational (what the review looked at).</sub>

### Inline review threads (2 threads, 2 comments)

#### Thread 1

`projects/hipblaslt/tensilelite/client/src/Reference.cpp` · Current line: 1399 · Original line: 1399 · Side: RIGHT

Status: Unresolved · Current diff.

##### tony-davis — comment 4568214216

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15277#discussion_r4568214216) · [@tony-davis](https://github.com/tony-davis) · Created: 2026-11-13 20:21:21 UTC

[Part of review 5897490540](https://github.com/ROCm/rocm-libraries/pull/15277#pullrequestreview-5897490540)

> (non-blocking) Thanks for writing this down. I looked: in the slow path, the D branch frees `ws` and then builds a `std::runtime_error` without throwing it (`Reference.cpp:2801-2802`), so it falls through to the second `free(ws)` at line 2831. The A and B branches (2812, 2822) drop their errors the same way. That's outside this PR, and keeping these problems on the slow path seems right to me for now. But I think it deserves a ticket. Adding `throw` in those three places would fix both problems, since the throw skips the second `free`.

#### Thread 2

`projects/hipblaslt/tensilelite/client/src/Reference.cpp` · Current line: 1697 · Original line: 1697 · Side: RIGHT

Status: Unresolved · Current diff.

##### tony-davis — comment 4568214232

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15277#discussion_r4568214232) · [@tony-davis](https://github.com/tony-davis) · Created: 2026-11-13 20:21:21 UTC

[Part of review 5897490540](https://github.com/ROCm/rocm-libraries/pull/15277#pullrequestreview-5897490540)

> (non-blocking) For the two non-HPA types you mention (`H_H_H`, `B_B_S`), this sums in float while the slow path sums in Half or BF16. My worry is that over a long reduction that's more than last-bit noise, so a non-HPA gradient config added later could see the bias reference move noticeably. No common config hits it today, so I'm fine either way. If the goal is "same answer as the slow path," a one-line reject for non-HPA gradient with bias would keep that promise. Otherwise I'd say in this comment that the float sum is deliberate.

### Newling’s linked reviews and cross-PR remarks

Posted on #15259: [initial round](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5867396457) · [final round](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5899752492). The [shared testing and design assessment](#shared-assessment) applies to this PR too.

**Newling’s own remark about #15277** ([original post](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5899752492)):

> #15277 The 2 fixes combined overall simplify the code, so I wouldn't ask questions unless this breaks a downstream test

#### Initial agent review, linked November 12

[Original on GitHub](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15277.md) · [Newling’s source post](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5867396457)

Source revision: `30df5bab4294f839ef85a91c732c36c92c0821ca`. File: `rocm-libraries/review_pr15277.md`.

> This is a review from an agent with an automatic prompt from the reviewer
>
> **PR reviewed:** [ROCm/rocm-libraries#15277](https://github.com/ROCm/rocm-libraries/pull/15277)
>
> **Submitted head:** `46ce0b0c3a676b3878f652e4dbf136bdb1f30f9f`. **Incremental base:** `5b4f85f05f2977d06f67cf5405eff41da6800f93`.
>
> Independent review of the incremental change and its interaction with the CPU-reference stack, emphasizing test adequacy, runtime, overlap, design reuse, and unsupported cases. Inputs were the public descriptions, code, repository guidance and CI status/logs; target-PR discussions and existing reviews were not read.
>
> **Suggestion branch:** [review/pr15277-suggestions](https://github.com/newling/rocm-libraries/tree/review/pr15277-suggestions), based on the submitted head above; final head [26f8f56cec2a4517b7edef1ade0da921334d228a](https://github.com/newling/rocm-libraries/commit/26f8f56cec2a4517b7edef1ade0da921334d228a).
>
> ## Tests
>
> Submitted head: `cpu-gemm-driver`, `tensilelite-tests`, and `tensilelite-client` built successfully; **770 driver CTests and 40 focused reference gtests passed**. The driver suite took 5.10 s with four CTest workers. Suggestion implementation: the same targets built; **770 driver CTests and 42 focused reference gtests passed**. Later comment-only commits, where present, do not change that execution result.
>
> Validation used a Release ROCm compiler build. Controlled runs set `OMP_THREAD_LIMIT=8`, `OMP_NUM_THREADS=4`, and `KMP_BLOCKTIME=0`; the original stack-tip driver timing predates the thread-limit setting. These are host numerical tests, not GPU-kernel performance measurements. Root/component hook invocations and full-stack whitespace checks were clean; the formatting hooks opt out of these C++/CMake paths, and no Python tests were selected by the component hook. No local GPU or sanitizer run is claimed.
>
> The gtests were run directly with `--gtest_filter=ReferenceFastPath.*:ReferenceMX*.*:*ReferenceMX*/*`. In the category-based build, a CTest name regex containing `ReferenceFastPath` does not select them. Driver CTests are build-tree tests and are not installed into the TheRock test package; Math CI's unfiltered `coverage-cpp` lane runs them but is informational. The gtest categories include standard/comprehensive/full, not quick.
>
> ## Summary
>
> E storage and gradient execution fit naturally into the existing epilogue. Reusing the reference's cast and reduction code is preferable to introducing another numerical implementation. The 70 added driver cases and five new gtests cover E ordering, E padding, activation-gradient use, A/B/D reduction sources, transpose, batching without bias reduction, and sampled validation.
>
> Two material gaps remain despite those tests: the driver rejects unsupported batched bias gradients before exercising the reference's fallback, and the driver uses HPA so it does not reveal non-HPA reduction changes.
>
> ## Actionable items
>
> ### Report unsupported reduction shapes without double-freeing the workspace
>
> [client/src/Reference.cpp:1398](https://github.com/ROCm/rocm-libraries/blob/46ce0b0c3a676b3878f652e4dbf136bdb1f30f9f/projects/hipblaslt/tensilelite/client/src/Reference.cpp#L1398) explicitly routes unsupported reduction shapes to the slow path. At [client/src/Reference.cpp:2801](https://github.com/ROCm/rocm-libraries/blob/46ce0b0c3a676b3878f652e4dbf136bdb1f30f9f/projects/hipblaslt/tensilelite/client/src/Reference.cpp#L2801), that existing slow code frees the workspace and constructs, but does not throw, a `runtime_error`; it later frees the workspace again.
>
> A 1×1×1 Float GEMM with two batches, gradient enabled, and bias reduced from D aborts with a double-free both when requesting fast execution and when explicitly requesting slow execution. This is an inherited defect carried by the fallback, not a claim that this PR introduced the original free.
>
> Suggestion commit [66106f99d91](https://github.com/newling/rocm-libraries/commit/66106f99d916da886beecc39e464681596074b6d) makes the slow path use the same `reduceBiasGradient` helper, gives workspace ownership to `std::vector`, and consumes the reduction error. The diagnostic now states the 2-D/single-batch requirement and actual batch count. `BatchedBiasGradientReportsUnsupportedShape` covers A, B and D sources through both requested paths and verifies the message and unchanged bias. Commit [26f8f56cec2](https://github.com/newling/rocm-libraries/commit/26f8f56cec2a4517b7edef1ade0da921334d228a) updates the shared helper's comment.
>
> ### Preserve non-HPA reduction rounding or explicitly define a different reference policy
>
> [client/src/Reference.cpp:1642](https://github.com/ROCm/rocm-libraries/blob/46ce0b0c3a676b3878f652e4dbf136bdb1f30f9f/projects/hipblaslt/tensilelite/client/src/Reference.cpp#L1642) reduces in fast-path `AccumT`, which is float for non-HPA Half/BFloat16 cases that the slow path reduces in their narrow type. This is not bounded to last-bit variation.
>
> With an all-one Half GEMM, M=K=1 and N=4097, every D value is 1 on both paths, but the bias gradient is **2048** on slow and **4096** on fast. Repeated narrow additions stop changing at 2048; a float reduction followed by a Half conversion behaves differently.
>
> Suggestion commit [de7815c2e50](https://github.com/newling/rocm-libraries/commit/de7815c2e5066f6b2ff76e589732a3b8a671e402) retains the slow path for non-HPA `H_H_H` and `B_B_S` gradient+bias problems. `NonHpaBiasGradientKeepsNarrowAccumulation` covers the Half and BFloat16 rounding boundaries. If wider accumulation is desired instead, it needs an explicit numerical-policy decision and corresponding tests rather than a general “last bits” explanation.
>
> ## Suggestions
>
> Use the shared negative-test helper from #15278 ([66ef187d435](https://github.com/newling/rocm-libraries/commit/66ef187d43535cacaba49a328b77b0388909291e)) for the driver's unsupported gradient cases. The direct reference regression is still necessary: driver argument rejection alone cannot test fallback cleanup.
>
> ## Scope decisions
>
> The suggested fix preserves the existing one-batch reduction limitation and the selected source's reduction semantics. It adds no batched bias-gradient implementation. The allocation/error fix was checked with the deterministic reproducer and normal native tests; an instrumented sanitizer build was not run locally.
>
> The exact native probe source and build recipe for the numerical counterexamples are preserved in the [cross-PR appendix](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_cpu_reference_stack_15259_15278.md#appendix-native-counterexample-source-and-setup). Committed regressions cover the corresponding contracts.
>
> ## Suggestion branch handoff
>
> | Suggestion commit | Item |
> | --- | --- |
> | [66106f99d91](https://github.com/newling/rocm-libraries/commit/66106f99d916da886beecc39e464681596074b6d) | share bias reduction and report unsupported shapes |
> | [de7815c2e50](https://github.com/newling/rocm-libraries/commit/de7815c2e5066f6b2ff76e589732a3b8a671e402) | preserve non-HPA bias-gradient accumulation |
> | [26f8f56cec2](https://github.com/newling/rocm-libraries/commit/26f8f56cec2a4517b7edef1ade0da921334d228a) | describe the shared bias-gradient reduction |
>
> The suggestion branches are published. Each branch contains its own corrections on its own submitted head, not the other review branches' fixes. Integrate shared changes in stack order, including the alias fix once and later `TypeInfo` call sites. PR-description corrections are reported for the author to make; none were published. See [the cross-PR assessment](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_cpu_reference_stack_15259_15278.md) for test overlap, CI limits and integration notes.
>

#### Final correctness review, linked November 14 (document dated November 13)

[Original on GitHub](https://github.com/newling/agent-public-reviews/blob/ede5c5f065835ae571ce0647b4e07af2c6cb8d64/rocm-libraries/review_pr15277_correctness_20261113.md) · [Newling’s source post](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5899752492)

Source revision: `ede5c5f065835ae571ce0647b4e07af2c6cb8d64`. File: `rocm-libraries/review_pr15277_correctness_20261113.md`.

> This is a review from an agent with an automatic prompt from the reviewer
>
> **PR reviewed:** [ROCm/rocm-libraries#15277](https://github.com/ROCm/rocm-libraries/pull/15277)
>
> **Submitted head:** `46ce0b0c3a676b3878f652e4dbf136bdb1f30f9f`. **Incremental diff base:** `5b4f85f05f2977d06f67cf5405eff41da6800f93`.
>
> **Scope:** Focused follow-up on consequential CPU-reference errors and regression-test failures, including the existing review discussion. Head and discussion were unchanged at the 2026-11-13 23:09 UTC check. Earlier validation is reused where the tested code is unchanged.
>
> **Suggestion branch:** [review/pr15277-correctness-20261113](https://github.com/newling/rocm-libraries/tree/review/pr15277-correctness-20261113), based on the submitted head; final head [de7815c2e50](https://github.com/newling/rocm-libraries/commit/de7815c2e5066f6b2ff76e589732a3b8a671e402). Commit-to-item mapping is given below.
>
> ## Tests
>
> Reused validation for the unchanged implementations: submitted head, 770 driver CTests and 40 reference tests passed; retained fixes, 770 driver CTests and 42 reference tests passed. The driver, tests and client built. Direct CPU probes demonstrated the double-free and the non-HPA reduction difference below. The cleanup fix was checked by its regression and ordinary execution, without an AddressSanitizer run.
>
> ## Summary
>
> Writing the auxiliary E output and computing gradients fit into the existing epilogue. The important risks are the fallback used for unsupported bias reductions and a change in the precision of long bias-gradient sums. Both need to be distinguished from the newly supported, ordinary E-output cases.
>
> ## Actionable items
>
> ### Make unsupported bias reductions report an error without freeing twice
>
> [client/src/Reference.cpp:1398](https://github.com/ROCm/rocm-libraries/blob/46ce0b0c3a676b3878f652e4dbf136bdb1f30f9f/projects/hipblaslt/tensilelite/client/src/Reference.cpp#L1398) sends unsupported reduction shapes to the slow reference. At [client/src/Reference.cpp:2801](https://github.com/ROCm/rocm-libraries/blob/46ce0b0c3a676b3878f652e4dbf136bdb1f30f9f/projects/hipblaslt/tensilelite/client/src/Reference.cpp#L2801), that slow path frees its workspace and constructs a `runtime_error` without throwing it, then reaches a second free. A 1×1×1 float GEMM with two batches, gradients enabled and bias reduced from D aborts with `double free detected` for either path preference. Driver argument rejection does not exercise this reference fallback.
>
> This is an **inherited memory-management defect**, not a new free introduced by this PR. It is relevant because the new path deliberately relies on this fallback. Fix [66106f99d91](https://github.com/newling/rocm-libraries/commit/66106f99d916da886beecc39e464681596074b6d) gives the workspace automatic ownership, reuses the reduction helper and propagates its error. `BatchedBiasGradientReportsUnsupportedShape` checks A, B and D reduction sources through both preferences, including the diagnostic and unchanged bias.
>
> ### Preserve narrow bias-gradient accumulation unless a wider policy is deliberate
>
> At [client/src/Reference.cpp:1642](https://github.com/ROCm/rocm-libraries/blob/46ce0b0c3a676b3878f652e4dbf136bdb1f30f9f/projects/hipblaslt/tensilelite/client/src/Reference.cpp#L1642), the fast reduction uses float for non-HPA Half/BFloat16 configurations whose slow reduction accumulates in the narrow type. Here non-HPA means higher-precision accumulation is disabled.
>
> For all-one Half inputs with M=K=1 and N=4097, every D element is 1 on both paths, but the bias gradient is **2048 on slow and 4096 on fast**. Repeated Half additions stop changing at 2048; reducing in float and converting once produces a different answer. This can change the validation target by a factor of two.
>
> The conservative fix [de7815c2e50](https://github.com/newling/rocm-libraries/commit/de7815c2e5066f6b2ff76e589732a3b8a671e402) keeps the affected non-HPA gradient+bias configurations on the slow path. `NonHpaBiasGradientKeepsNarrowAccumulation` covers Half and BFloat16 boundaries. An intentional move to float accumulation is an alternative, but it needs an explicit numerical-policy decision and tests for that result.
>
> ## Suggestions
>
> None in this focused review.
>
> ## Scope decisions
>
> The cleanup correction retains the existing single-batch reduction limitation and makes rejection safe; it adds no batched reduction support. Since the memory error predates this PR, it can also be fixed in a separate dependency. It remains a real failure until that fix is integrated.
>
> No current common configuration is known to exercise the non-HPA case. The retained guard offers behavior preservation for supported inputs; the existing discussion allows a deliberate wider-sum policy instead. This review does not claim that choice has been settled. Both original numerical probes and their setup are preserved in the [native counterexample source and setup](https://github.com/newling/agent-public-reviews/blob/ede5c5f065835ae571ce0647b4e07af2c6cb8d64/rocm-libraries/review_cpu_reference_stack_15259_15278.md#appendix-native-counterexample-source-and-setup).
>
> **Suggestion summary:** [review/pr15277-correctness-20261113](https://github.com/newling/rocm-libraries/tree/review/pr15277-correctness-20261113) contains [66106f99d91](https://github.com/newling/rocm-libraries/commit/66106f99d916da886beecc39e464681596074b6d) for reduction cleanup and [de7815c2e50](https://github.com/newling/rocm-libraries/commit/de7815c2e5066f6b2ff76e589732a3b8a671e402) for the conservative accumulation guard. The author may choose a documented wider-accumulation policy instead of the second commit.
>

### General discussion comments (3, including bots)

#### therock-pr-bot[bot] — comment 6692568671

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15277#issuecomment-6692568671) · [@therock-pr-bot[bot]](https://github.com/apps/therock-pr-bot) · Created: 2026-11-06 18:40:47 UTC · Updated: 2026-11-10 22:31:43 UTC

> <!-- therock-pr-bot-policy-check -->
> ### ❌ PR Check — Action Required
>
> | Check | Status | Details |
> |---|:---:|---|
> | 📝 **PR Description** | ✅ Pass | — |
> | ⛔ **Forbidden Files** | ✅ Pass | — |
> | 🧪 **Unit Test** | ✅ Pass | — |
> | 🔎 **pre-commit** | ❌ Fail | **Error:** Check concluded with `cancelled`. |
> | 🚫 **Draft PR** | 🔜 To Be Enabled | — |
> | 🚩 **Feature Flag** | 🔜 To Be Enabled | — |
> | 📊 **Code Coverage** | 🔜 To Be Enabled | — |
>
> > ⚠️ **1 policy check(s) failed.** Please address the issues above before this PR can be Reviewed.
> >
> > 🚫 **Please fix the failed policies**
> > - ❌ pre-commit
> >
> > The **`Not ready to Review`** label was added to this PR. Once all policies pass, the label is removed automatically.
>
> 📖 **Need help?** See the [Policy FAQ](https://github.com/ROCm/rocm-libraries/blob/develop/docs/LIBRARIES_PR_BOT_FAQ.md) for details on every check and how to fix failures.
>
> 🙋 **[Wish to Override Policy?](https://github.com/ROCm/rocm-libraries/blob/develop/docs/LIBRARIES_PR_BOT_FAQ.md#-wish-to-override-the-policy-process-and-get-unblocked)**

#### therock-pr-bot[bot] — comment 6692569706

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15277#issuecomment-6692569706) · [@therock-pr-bot[bot]](https://github.com/apps/therock-pr-bot) · Created: 2026-11-06 18:40:50 UTC · Updated: 2026-11-09 21:35:06 UTC

> <!-- therock-pr-bot-fix-policies -->
> 🎉 All checks passed! This PR is ready for review.

#### codecov[bot] — comment 6788440519

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15277#issuecomment-6788440519) · [@codecov[bot]](https://github.com/apps/codecov) · Created: 2026-11-11 06:04:19 UTC · Updated: 2026-11-11 06:53:05 UTC

> ## [Codecov](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15277?dropdown=coverage&src=pr&el=h1&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) Report
> :x: Patch coverage is `77.77778%` with `28 lines` in your changes missing coverage. Please review.
>
> | [Files with missing lines](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15277?dropdown=coverage&src=pr&el=tree&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | Patch % | Lines |
> |---|---|---|
> | [...cts/hipblaslt/tensilelite/client/src/Reference.cpp](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15277?src=pr&el=tree&filepath=projects%2Fhipblaslt%2Ftensilelite%2Fclient%2Fsrc%2FReference.cpp&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm#diff-cHJvamVjdHMvaGlwYmxhc2x0L3RlbnNpbGVsaXRlL2NsaWVudC9zcmMvUmVmZXJlbmNlLmNwcA==) | 77.78% | [16 Missing and 12 partials :warning: ](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15277?src=pr&el=tree&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
>
> :x: Your project check has failed because the head coverage (54.34%) is below the target coverage (80.00%). You can increase the head coverage or adjust the [target](https://docs.codecov.com/docs/commit-status#target) coverage.
>
> <details><summary>Additional details and impacted files</summary>
>
>
>
> ```diff
> @@                                     Coverage Diff                                      @@
> ##           users/alvasile/tl-speedup-26-reference-fast-path-complex   #15277      +/-   ##
> ============================================================================================
> + Coverage                                                     77.64%   77.67%   +0.03%
> ============================================================================================
>   Files                                                          3002     3002
>   Lines                                                        521870   521981     +111
>   Branches                                                      81395    81427      +32
> ============================================================================================
> + Hits                                                         405184   405414     +230
> + Misses                                                        88868    88742     -126
> - Partials                                                      27818    27825       +7
> ```
>
> | [Flag](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15277/flags?src=pr&el=flags&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | Coverage Δ | | *Carryforward flag |
> |---|---|---|---|
> | [TensileLite-CPP](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15277/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `54.34% <77.78%> (+0.56%)` | :arrow_up: | |
> | [TensileLite-Unit](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15277/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `83.84% <ø> (ø)` | | |
> | [hipBLAS](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15277/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `90.62% <ø> (ø)` | | Carriedforward from [5b4f85f](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/5b4f85f05f2977d06f67cf5405eff41da6800f93?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipBLASLt](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15277/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `70.39% <ø> (-<0.01%)` | :arrow_down: | |
> | [hipCUB](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15277/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `82.68% <ø> (ø)` | | Carriedforward from [5b4f85f](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/5b4f85f05f2977d06f67cf5405eff41da6800f93?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipDNN](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15277/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `87.34% <ø> (ø)` | | Carriedforward from [5b4f85f](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/5b4f85f05f2977d06f67cf5405eff41da6800f93?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipFFT](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15277/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `42.65% <ø> (ø)` | | Carriedforward from [5b4f85f](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/5b4f85f05f2977d06f67cf5405eff41da6800f93?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipRAND](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15277/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `76.12% <ø> (ø)` | | Carriedforward from [5b4f85f](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/5b4f85f05f2977d06f67cf5405eff41da6800f93?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipSOLVER](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15277/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `69.67% <ø> (ø)` | | Carriedforward from [5b4f85f](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/5b4f85f05f2977d06f67cf5405eff41da6800f93?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipSPARSE](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15277/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `87.03% <ø> (ø)` | | Carriedforward from [5b4f85f](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/5b4f85f05f2977d06f67cf5405eff41da6800f93?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocALUTION](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15277/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `86.47% <ø> (ø)` | | Carriedforward from [5b4f85f](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/5b4f85f05f2977d06f67cf5405eff41da6800f93?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocBLAS](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15277/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `69.16% <ø> (ø)` | | Carriedforward from [5b4f85f](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/5b4f85f05f2977d06f67cf5405eff41da6800f93?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocFFT](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15277/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `48.81% <ø> (ø)` | | Carriedforward from [5b4f85f](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/5b4f85f05f2977d06f67cf5405eff41da6800f93?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocRAND](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15277/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `57.32% <ø> (ø)` | | Carriedforward from [5b4f85f](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/5b4f85f05f2977d06f67cf5405eff41da6800f93?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocSOLVER](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15277/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `76.66% <ø> (ø)` | | Carriedforward from [5b4f85f](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/5b4f85f05f2977d06f67cf5405eff41da6800f93?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocSPARSE](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15277/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `83.86% <ø> (ø)` | | Carriedforward from [5b4f85f](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/5b4f85f05f2977d06f67cf5405eff41da6800f93?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocSPARSE-unit](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15277/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `79.82% <ø> (ø)` | | Carriedforward from [5b4f85f](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/5b4f85f05f2977d06f67cf5405eff41da6800f93?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocThrust](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15277/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `93.15% <ø> (ø)` | | Carriedforward from [5b4f85f](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/5b4f85f05f2977d06f67cf5405eff41da6800f93?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
>
> *This pull request uses carry forward flags. [Click here](https://docs.codecov.io/docs/carryforward-flags?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) to find out more.
>
> | [Files with missing lines](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15277?dropdown=coverage&src=pr&el=tree&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | Coverage Δ | |
> |---|---|---|
> | [...cts/hipblaslt/tensilelite/client/src/Reference.cpp](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15277?src=pr&el=tree&filepath=projects%2Fhipblaslt%2Ftensilelite%2Fclient%2Fsrc%2FReference.cpp&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm#diff-cHJvamVjdHMvaGlwYmxhc2x0L3RlbnNpbGVsaXRlL2NsaWVudC9zcmMvUmVmZXJlbmNlLmNwcA==) | `58.74% <77.78%> (+5.85%)` | :arrow_up: |
>
> ... and [8 files with indirect coverage changes](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15277/indirect-changes?src=pr&el=tree-more&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm)
> </details>
> <details><summary> :rocket: New features to boost your workflow: </summary>
>
> - :snowflake: [Test Analytics](https://docs.codecov.com/docs/test-analytics): Detect flaky tests, report on failures, and find test suite problems.
> - :package: [JS Bundle Analysis](https://docs.codecov.com/docs/javascript-bundle-analysis): Save yourself from yourself by tracking and limiting bundle sizes in JS merges.
> </details>

<a id="pr-15278"></a>

## PR #15278 — perf(tensilelite): run MX GEMMs with a partial last MX block on the CPU reference fast path

[GitHub PR](https://github.com/ROCm/rocm-libraries/pull/15278) · State: **OPEN**

Branch: `users/alvasile/tl-speedup-28-reference-fast-path-mx-k-tail`. Base: `users/alvasile/tl-speedup-27-reference-fast-path-use-e-gradient`. Head at collection: `e24bb5950c6eacb30a5eb2e201d7e5fba9af6848`.

### Feedback walkthrough — November 17, 2026

#### Exact MX-tail comparison ignores NaNs — implemented locally

**Feedback:** [Newling's initial review](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15278.md#make-the-exact-mx-tail-test-reject-nans) and [final correctness review](https://github.com/newling/agent-public-reviews/blob/ede5c5f065835ae571ce0647b4e07af2c6cb8d64/rocm-libraries/review_pr15278_correctness_20261113.md#make-the-exact-tail-test-fail-when-any-output-is-nan) show that `maxAbsDiff` ignores NaN differences. Entirely invalid output can pass the new exact tail test.

**Agreed resolution:** applied [Newling's assertion change `f15ee4aacee`](https://github.com/newling/rocm-libraries/commit/f15ee4aacee6031017b93c151ab6f2b3bb618a91), with repository formatting. `PartialLastMXBlockSumsOnlyElementsBelowK` now compares the finite expected vector directly with D using `EXPECT_EQ(expected, d)`. `SCOPED_TRACE` retains K, both MX block sizes and both transpose flags. Other approximate comparisons are unchanged.

**Validation:** temporarily replaced every computed D element with NaN. The original assertion incorrectly passed; the corrected assertion failed with exit code 1, showing the finite expected vector, NaN output and case context. Removed the mutation and its temporary includes. The Release ROCm compiler build then passed all **50 focused reference gtests** and all **813 standard CPU-driver CTests**, including the 18 fast tail cases. These are host numerical checks; they add no GPU or performance evidence. Logs and the temporary control are saved under `/tmp/tl-pr15278-exact-tail-20261117/`.

Commands:

```sh
cmake --build /tmp/tl-amaxd-installed-ci-20261116/build --target tensilelite-tests cpu-gemm-driver -j 4
OMP_THREAD_LIMIT=8 OMP_NUM_THREADS=4 KMP_BLOCKTIME=0 \
  /tmp/tl-amaxd-installed-ci-20261116/build/tensilelite/tests/tensilelite-tests \
  --gtest_filter='ReferenceFastPath.*:ReferenceMX*.*:*ReferenceMX*/*'
OMP_THREAD_LIMIT=8 OMP_NUM_THREADS=4 KMP_BLOCKTIME=0 \
  ctest --test-dir /tmp/tl-amaxd-installed-ci-20261116/build/tensilelite/tests \
    -L '^standard$' -R '^CPUGemm\.' --output-on-failure --parallel 4 --no-tests=error
```

##### Draft reply to Newling — not posted

Target: a comment on [#15278](https://github.com/ROCm/rocm-libraries/pull/15278), replying to the linked exact-tail assertion finding.

Thanks, @newling. I applied your direct vector comparison and kept the per-case context with `SCOPED_TRACE`. The all-NaN control passed with the old assertion and failed with the corrected one. After removing the control, all 50 focused reference tests and 813 standard CPU-driver tests passed locally. The change is committed locally and has not been pushed.

#### Obsolete slow-tail rejection and over-read wording — implemented locally

**Feedback:** [Tony's driver comment](https://github.com/ROCm/rocm-libraries/pull/15278#discussion_r4568214899), [his CMake comment](https://github.com/ROCm/rocm-libraries/pull/15278#discussion_r4568214917) and [Newling's corresponding suggestion](https://github.com/newling/rocm-libraries/commit/b5d8bb5f9a552f26384ff3202fc100c608c10014) challenge the claim that the production slow reference reads past K for equal MX blocks.

**Assessment:** confirmed in the current local source. `ReferenceSolution::SolveCPU` clamps its final segment with `std::min(innerMXLoop, boundSize[0] - i)`, already added by [#13630](https://github.com/ROCm/rocm-libraries/pull/13630). `SlowPathHandlesPartialFinalMXSegment` explicitly exercises slow execution at K=200 with block 128 and passed in the focused suite. The driver still rejects equal-block K tails and has an obsolete explanation. Its independent expected-result loop is a separate implementation, which this PR already changes to stop at K. The historical scratch-build failure is not evidence of a current production over-read; its precise cause/version has not been re-established.

**Agreed resolution:** removed the driver's equal-block K restriction and corrected its examples, command-line help and CMake comments. Converted the two obsolete `BadK` cases into positive slow-tail tests at K=33 and K=16. Added 15 slow counterparts for the equal-block fast tail cases using a shared fast/slow loop. Kept the asymmetric-block slow rejection: one scale per maximum-sized segment still misses smaller-side scale changes. No production slow-loop change was needed. The description correction and replies below are drafts; no GitHub changes have been published for this item.

**Validation:** the converted K=33 test failed before the driver fix with the obsolete "must be a multiple" diagnostic and passed afterward. The final Release build passed all **828 standard CPU-driver CTests** and **50 focused reference gtests**. Registration checks confirmed that all 15 slow counterparts match the existing fast inputs without `--tryFastPath`, retain the standard category and expect success. Existing fast commands are unchanged. The full build tree has 832 driver registrations; four older injected-failure tests lack category labels and are outside this standard run. Logs and before/after snapshots are saved under `/tmp/tl-pr15278-slow-tail-20261117/`.

##### Draft reply to Tony — not posted

Target: [Tony's driver thread](https://github.com/ROCm/rocm-libraries/pull/15278#discussion_r4568214899); also addresses [his CMake thread](https://github.com/ROCm/rocm-libraries/pull/15278#discussion_r4568214917).

You're right: the production slow reference already clamps its final MX segment at K, from #13630. The over-read claim was incorrect for equal blocks. I removed the driver's obsolete K restriction, corrected its help and comments, converted the two old rejection cases into positive slow-tail tests and added 15 slow counterparts for the equal-block fast cases. All 828 standard driver tests and 50 focused reference tests pass locally. The asymmetric-block rejection remains. I've also drafted the description correction and removed the historical scratch-build failures as evidence of a current production over-read; I have not re-established the precise cause/version of that old experiment. The code is committed locally and has not been pushed.

##### Draft reply to Newling — not posted

Target: a comment on [#15278](https://github.com/ROCm/rocm-libraries/pull/15278), replying to the [linked slow-tail suggestion](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15278.md#remove-the-obsolete-equal-block-slow-tail-rejection).

Thanks, @newling. I applied the equal-block slow-tail correction from your suggestion and added 15 slow counterparts for the existing equal-block fast cases. The converted K=33 test reproduced the driver rejection before the fix and passed afterward. All 828 standard driver tests and 50 focused reference tests pass locally. The asymmetric-block restriction remains, and the description correction is drafted. The code is committed locally and has not been pushed.

##### Corrected PR description draft — not published

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

Configure from `projects/hipblaslt` with `cmake --preset tensilelite -S . -B "$BUILD" -DTENSILELITE_BUILD_TESTING=ON`, then run:

```sh
cmake --build "$BUILD" --target tensilelite-tests cpu-gemm-driver --parallel 4
OMP_THREAD_LIMIT=8 OMP_NUM_THREADS=4 KMP_BLOCKTIME=0 \
  ctest --test-dir "$BUILD/tensilelite/tests" -L '^standard$' -R '^CPUGemm\.' \
    --output-on-failure --parallel 4 --no-tests=error
OMP_THREAD_LIMIT=8 OMP_NUM_THREADS=4 KMP_BLOCKTIME=0 \
  "$BUILD/tensilelite/tests/tensilelite-tests" \
    --gtest_filter='ReferenceFastPath.*:ReferenceMX*.*:*ReferenceMX*/*'
```

Run the root and hipBLASLt pre-commit hooks on the changed files.

## Test Result

Published head `e24bb5950c6eacb30a5eb2e201d7e5fba9af6848`: [Math CI's gfx950 common suite](https://math-ci.amd.com/job/rocm-libraries/job/preliminary/job/hipblaslt/job/PR-15278/7/execution/node/1115/log) passed with 239 passed and 495 skipped, including `gemm/gfx950/mxfp4_mxfp4_bf16_tn_act.yaml`. [GitHub Actions on gfx942](https://github.com/ROCm/rocm-libraries/actions/runs/42914665365/job/130345001958) passed with 164 passed and 570 skipped; it skipped that gfx950-only configuration.

Local review commits through `dafba8ce355d`, Release build with the ROCm compiler: 834 standard driver CTests passed in each of the build and installed trees, including all 38 diagnostic-checked negative cases. All 50 focused installed reference gtests and the scoped root/component hooks passed. Temporary controls confirmed that the tests reject NaN output and unrelated driver errors. The slow-tail and AMAX-D diagnostic regressions failed before their respective changes and passed afterward.

The local review commits have not been pushed or run by shared CI. Installed checks used a standalone TensileLite test package with local ROCm runtime dependencies. These local checks provide host numerical and registration evidence; no new GPU or performance measurement is claimed for those commits.

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

##### Remaining negative tests and shared registration — implemented locally

[Newling's shared failure-test suggestion](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15278.md#share-negative-test-checking-across-the-stack) applies here. The stack already has `EXPECTED_ERROR` support in `add_cpu_gemm_test` and `CheckCpuGemmFailure.cmake`, requiring exit code 1 and the intended diagnostic. However, 11 driver rejection/injected-failure tests still use raw `WILL_FAIL`; an unrelated nonzero argument or validation failure can satisfy those tests. Four Float8/complex injected-failure cases also call `add_test` directly, bypassing the category/export helper; they have no standard label and do not enter the installed driver test list.

**Agreed resolution:** converted all 11 remaining negative cases to `EXPECTED_ERROR`, reusing `CheckCpuGemmFailure.cmake`. The four Float8/complex injected-failure cases now use `add_cpu_gemm_test`, which applies category labels, OpenMP limits, timeout and installed registration. All original driver arguments and test names are preserved. Both the build and installed suites now have 832 standard driver cases, including all 36 diagnostic-checked negative cases, with no raw `WILL_FAIL` cases. No new checker was added.

**Validation:** temporarily changed `tf32_MixedA_Rejected` to use an invalid input type. The original `WILL_FAIL` registration incorrectly passed that unrelated argument error. The corrected registration failed with "Missing diagnostic" and the unrelated "Unknown computeInputA" output. Restored the original argument, then ran normal validation: **832/832 standard driver CTests passed in the build tree and 832/832 passed in the installed tree; all 50 focused installed reference gtests passed**. An installed-checker control with a deliberately incorrect expected diagnostic also failed as required. These are host numerical/registration checks, with no new GPU or performance measurement.

The standalone test package was installed from the TensileLite subtree's `tests` and `tensilelite` components into `/tmp/tl-pr15278-negative-tests-20261117/install`; the ROCm compiler's `libomp.so` was staged in its runtime library directory. This checks the installed driver, checker and registrations with local ROCm runtime dependencies; it is not a full TheRock artifact run. Discovery records, test/control logs and snapshots are saved in `/tmp/tl-pr15278-negative-tests-20261117/`.

##### Draft reply to Newling — not posted

Target: a comment on [#15278](https://github.com/ROCm/rocm-libraries/pull/15278), replying to the [linked shared negative-test suggestion](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15278.md#share-negative-test-checking-across-the-stack).

Thanks, @newling. The stack already has the shared diagnostic checker, so I migrated the remaining 11 negative tests to it. All 36 now require exit code 1 and the expected message. I also moved the four Float8/complex injected-failure cases through the shared registration helper so they enter the standard and installed suites. An unrelated argument error passed the old registration and failed the corrected one; the installed checker also rejected a deliberately incorrect expected diagnostic. All 832 standard driver tests passed in both build and installed trees, and all 50 focused installed reference tests passed locally. The changes are committed locally and have not been pushed.

##### Forced fast-path rejection reasons — implemented locally

[Newling's eligibility-diagnostic suggestion](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15278.md#expose-the-eligibility-reason-to-the-driver), implemented in [his `c7540acfc31` commit](https://github.com/newling/rocm-libraries/commit/c7540acfc31bd3bfc502810669d0e598c14e3c57), applies to the current source. The predicate knows the rejection reason, but its logger was disabled behind `if(false)`. Forced fast execution reported only that the problem was ineligible.

**Agreed resolution:** added an optional `std::string* rejectionReason` output to `isFastPathEligible`. It clears the output on entry and reports the first rejection as a readable message. The driver requests it only for `--tryFastPath` and includes it in the error. Ordinary client fallback stays silent. All eligibility statements were checked against the previous commit after normalizing diagnostic strings, comments and whitespace; none changed, including the Scalar Half-to-Float8 and non-HPA bias-gradient restrictions.

**Regression coverage:** added `fast_AmaxD_Rejected` and, outside Windows, `fast_f4_MX4_Rejected` through the existing diagnostic checker. They require exit code 1 and the AMAX-D or 8-element K-tile message. Only numerical AMAX-D cases receive the existing thread-limit variants; rejection exits before the reduction. Discovery confirmed that all 832 existing driver registrations are unchanged in both build and installed trees. The future AMAX-D implementation must replace its new rejection case with positive fast-path coverage; this is recorded in Future work.

**Validation:** the forced-AMAX-D case failed before the implementation with "Missing diagnostic" and the generic eligibility error, then passed afterward. The Release ROCm compiler build rebuilt `tensilelite-tests`, `cpu-gemm-driver` and `tensilelite-client`. All **834/834 standard driver CTests passed in each of the build and installed trees**, including all **38 diagnostic-checked negatives**; all **50 focused installed reference gtests passed**. The standalone test package uses the TensileLite `tests` and `tensilelite` components plus the local ROCm OpenMP library; this is host numerical and installed-registration validation, with no new GPU or performance measurement. Evidence and the refreshed description draft are saved in `/tmp/tl-pr15278-eligibility-20261117/`.

##### Draft reply to Newling — not posted

Target: a comment on [#15278](https://github.com/ROCm/rocm-libraries/pull/15278), replying to the [linked eligibility-diagnostic suggestion](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15278.md#expose-the-eligibility-reason-to-the-driver).

Thanks, @newling. I applied the optional rejection-reason output and included it in the driver's `--tryFastPath` error. Every existing eligibility condition is preserved, including the Scalar Half-to-Float8 and non-HPA bias-gradient fallbacks added during review. Normal client fallback stays silent. The new AMAX-D and MX block-4 cases check the specific messages through the shared diagnostic checker. The AMAX-D case failed with the old generic error and passed after the change. All 834 standard driver tests passed in both build and installed trees, and all 50 focused installed reference tests passed locally. The changes are committed locally and have not been pushed.

##### Tony's published-head Test Result refresh — drafted locally

[Tony's review](https://github.com/ROCm/rocm-libraries/pull/15278#pullrequestreview-5897491239) asks the description to identify completed testing for the published head rather than say this split branch was never built or tested.

**Agreed resolution:** refreshed the description draft above. Test Result now links the published-head gfx950 and gfx942 common-suite results and gives their exact pass/skip counts. It separately records the 834 build-tree and installed driver tests and 50 focused reference tests at local code commit `dafba8ce355d`. Device / Architecture Coverage states that the published gfx950 run covers the original MX-tail change, while shared CI and gfx950 validation of the unpushed review fixes remain pending. Historical scratch-build failures and timings are excluded from current-head correctness claims. The description and reply remain unpublished.

**Evidence refresh:** live PR metadata still reports `e24bb5950c6eacb30a5eb2e201d7e5fba9af6848`. Its existing body retains the stale testing statements. Cached CI logs and SCM metadata were rechecked against that head: [Math CI's gfx950 common log](https://math-ci.amd.com/job/rocm-libraries/job/preliminary/job/hipblaslt/job/PR-15278/7/execution/node/1115/log) reports the affected `mxfp4_mxfp4_bf16_tn_act.yaml` configuration PASSED, with **239 passed and 495 skipped** overall. [GitHub Actions on gfx942](https://github.com/ROCm/rocm-libraries/actions/runs/42914665365/job/130345001958) reports **164 passed and 570 skipped**, including an explicit skip for that gfx950-only configuration. The local numerical counts match the saved test logs. The latest PR metadata, feedback lookup, description and reply drafts are saved in `/tmp/tl-pr15278-description-20261117/`.

##### Draft reply to Tony — not posted

Target: a comment on [#15278](https://github.com/ROCm/rocm-libraries/pull/15278), responding to the CI/Test Result paragraph in [Tony's review](https://github.com/ROCm/rocm-libraries/pull/15278#pullrequestreview-5897491239).

Thanks, Tony. I rechecked the CI evidence for published head `e24bb5950c6eacb30a5eb2e201d7e5fba9af6848`. [Math CI on gfx950](https://math-ci.amd.com/job/rocm-libraries/job/preliminary/job/hipblaslt/job/PR-15278/7/execution/node/1115/log) passed the affected `mxfp4_mxfp4_bf16_tn_act.yaml` configuration, with 239 passed and 495 skipped overall. [GitHub Actions on gfx942](https://github.com/ROCm/rocm-libraries/actions/runs/42914665365/job/130345001958) passed with 164 passed and 570 skipped, but skipped that gfx950-only configuration.

The refreshed description draft links those results and separately records local validation through `dafba8ce355d`: 834 driver tests passed in both build and installed suites, and all 50 focused installed reference tests passed. Those review commits have not been pushed or run by shared CI. The draft removes the stale "not built or tested" and "checks are pending" statements and the production over-read claim for equal blocks. The precise cause/version of the old scratch-build experiment remains unverified.

##### Feedback queue after refresh

A fresh GitHub lookup found one human review and the same two unresolved Tony inline threads on #15278, with no new human comments. Newling's source reviews on #15259 still link the same initial and final reviews archived below. The four Newling suggestions and Tony's slow-tail/code-comment requests are implemented locally; the description and replies are drafted. There is no additional undecided human feedback for this branch in the refreshed queue. Tony's threads remain open on the published source, which has not received the local fixes. Publication, CI on the final pushed code and the planned correctness-PR insertions remain outstanding stack work.

### Reviews (1)

#### tony-davis — COMMENTED — review 5897491239

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15278#pullrequestreview-5897491239) · [@tony-davis](https://github.com/tony-davis) · Submitted: 2026-11-13 20:21:24 UTC

Reviewed commit: `e24bb5950c6eacb30a5eb2e201d7e5fba9af6848`.

> Hi Alex! Thanks for this one. The fast-path side looks right to me. Every 8-wide K tile sits inside one MX block, the tile loads zero everything past K, and the scale index `k0 / mxBlock` picks that block's scale, so a partial last block sums only what's below K. `PartialLastMXBlockSumsOnlyElementsBelowK` is a great test: 4s in the padding past K and random power-of-two scales mean any over-read or wrong tail scale changes D, and you compare against an exact sum.
>
> I have one question before I approve, and I think it's mostly about words. The description, plus new comments in the driver and `CMakeLists.txt`, say the production slow path sums whole blocks and reads past K. But at this head the slow path clamps its last segment (`segment = std::min(innerMXLoop, boundSize[0] - i)` in `ReferenceSolution::SolveCPU`, from #13630). This PR's own updated comment on `SlowPathHandlesPartialFinalMXSegment` says both paths handle it, and that test checks the slow path gives exactly K for K = 200. The loop that did read past K was the driver's own golden, which you fix in this PR. The description's gfx950 numbers also look like rounding to me (33 to 64 elements out of 65,536 for K = 255), where I'd expect an over-read to touch nearly every element. My guess is the scratch-build experiment predates #13630. Could you check? If the slow path is fine for equal blocks, I'd fix the two comments and the Motivation and Risk text. You could probably also let the driver run K tails on the slow path and add slow twins of the new CTest cases. I bet that's a stronger check than fast against the driver golden alone.
>
> On CI: the one config this changes, `mxfp4_mxfp4_bf16_tn_act`, is gfx950-only, and TheRock only ran `tensilelite-common` on gfx94X here, where it's skipped. So I think the on-GPU check for this PR is Math CI `preliminary` (passed). It would help if Test Result said that, or recorded a gfx950 run, instead of "not built or tested". The ASAN gfx950 failure is a 30-minute timeout downloading the ASAN build, and the `gfx1250-strict` failure is the same unrelated timeout as #15276, so I don't think either is yours.
>
> I'm leaving this as a comment for now, only because the answer changes a few comments and maybe the driver check. I'm happy to approve once we sort it out. (Also non-blocking: a "Depends on #15277" line in Related.)

### Inline review threads (2 threads, 2 comments)

#### Thread 1

`projects/hipblaslt/tensilelite/client/cpu_gemm_driver.cpp` · Current line: 674 · Original line: 674 · Side: RIGHT

Status: Unresolved · Current diff.

##### tony-davis — comment 4568214899

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15278#discussion_r4568214899) · [@tony-davis](https://github.com/tony-davis) · Created: 2026-11-13 20:21:24 UTC

[Part of review 5897491239](https://github.com/ROCm/rocm-libraries/pull/15278#pullrequestreview-5897491239)

> I think this might be out of date. At this head the production slow path stops its last MX segment at K: `segment = std::min(innerMXLoop, boundSize[0] - i)` in `ReferenceSolution::SolveCPU` (`Reference.cpp:2545`, added in #13630 on Oct 30), and the comment there says the old over-read is gone. The loop that did read past K was this file's own golden, which you fix at line 441. Is the over-read you measured from before #13630, or from that golden?
>
> If the slow path is right when `mxBlockA == mxBlockB`, I'd reword this. Maybe also let K tails through on the slow path for equal blocks (the asymmetric-block rejection below is a separate, real limitation).

#### Thread 2

`projects/hipblaslt/tensilelite/tests/CMakeLists.txt` · Current line: 652 · Original line: 652 · Side: RIGHT

Status: Unresolved · Current diff.

##### tony-davis — comment 4568214917

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15278#discussion_r4568214917) · [@tony-davis](https://github.com/tony-davis) · Created: 2026-11-13 20:21:24 UTC

[Part of review 5897491239](https://github.com/ROCm/rocm-libraries/pull/15278#pullrequestreview-5897491239)

> Same question as on the driver. I think the slow path's MX loop stops at K now (`Reference.cpp:2545`), and `SlowPathHandlesPartialFinalMXSegment` checks it. If that holds, a few `slow_f4_MX32_KTail*` twins of these cases would make this a fast-versus-slow check. I bet that's stronger than fast against the driver golden alone. (non-blocking)

### Newling’s linked reviews and cross-PR remarks

Posted on #15259: [initial round](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5867396457) · [final round](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5899752492). The [shared testing and design assessment](#shared-assessment) applies to this PR too.

**Newling’s own remark about #15278** ([original post](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5899752492)):

> #15278 Tightens a test, looks fine to me.

#### Initial agent review, linked November 12

[Original on GitHub](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15278.md) · [Newling’s source post](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5867396457)

Source revision: `30df5bab4294f839ef85a91c732c36c92c0821ca`. File: `rocm-libraries/review_pr15278.md`.

> This is a review from an agent with an automatic prompt from the reviewer
>
> **PR reviewed:** [ROCm/rocm-libraries#15278](https://github.com/ROCm/rocm-libraries/pull/15278)
>
> **Submitted head:** `e24bb5950c6eacb30a5eb2e201d7e5fba9af6848`. **Incremental base:** `46ce0b0c3a676b3878f652e4dbf136bdb1f30f9f`.
>
> Independent review of the incremental change and its interaction with the CPU-reference stack, emphasizing test adequacy, runtime, overlap, design reuse, and unsupported cases. Inputs were the public descriptions, code, repository guidance and CI status/logs; target-PR discussions and existing reviews were not read.
>
> **Suggestion branch:** [review/pr15278-suggestions](https://github.com/newling/rocm-libraries/tree/review/pr15278-suggestions), based on the submitted head above; final head [c7540acfc31bd3bfc502810669d0e598c14e3c57](https://github.com/newling/rocm-libraries/commit/c7540acfc31bd3bfc502810669d0e598c14e3c57).
>
> ## Tests
>
> Submitted head: `cpu-gemm-driver`, `tensilelite-tests`, and `tensilelite-client` built successfully; **788 driver CTests and 42 focused reference gtests passed**. The driver suite took 5.21 s with four CTest workers. Suggestion implementation: the same targets built; **790 driver CTests and 42 focused reference gtests passed**. Later comment-only commits, where present, do not change that execution result.
>
> Validation used a Release ROCm compiler build. Controlled runs set `OMP_THREAD_LIMIT=8`, `OMP_NUM_THREADS=4`, and `KMP_BLOCKTIME=0`; the original stack-tip driver timing predates the thread-limit setting. These are host numerical tests, not GPU-kernel performance measurements. Root/component hook invocations and full-stack whitespace checks were clean; the formatting hooks opt out of these C++/CMake paths, and no Python tests were selected by the component hook. No local GPU or sanitizer run is claimed.
>
> The gtests were run directly with `--gtest_filter=ReferenceFastPath.*:ReferenceMX*.*:*ReferenceMX*/*`. In the category-based build, a CTest name regex containing `ReferenceFastPath` does not select them. Driver CTests are build-tree tests and are not installed into the TheRock test package; Math CI's unfiltered `coverage-cpp` lane runs them but is informational. The gtest categories include standard/comprehensive/full, not quick.
>
> ## Summary
>
> The eligibility relaxation follows the tile invariant: MX blocks remain multiples of the eight-element K tile, and loads beyond logical K are zero-filled. The 18 added driver cases cover short K, tails on one or both scale-block sides, transpose, batching and epilogue features. The exact poisoned-padding gtest is the right kind of independent arithmetic check, but its comparison has a false-negative hole.
>
> This PR also needs to describe the current slow implementation. Its base already contains a K-clamped MX loop, so the retained driver rejection and over-read narrative are stale for equal block sizes.
>
> ## Actionable items
>
> ### Make the exact MX-tail test reject NaNs
>
> At [tests/ReferenceMXFastPath_test.cpp:274](https://github.com/ROCm/rocm-libraries/blob/e24bb5950c6eacb30a5eb2e201d7e5fba9af6848/projects/hipblaslt/tensilelite/tests/ReferenceMXFastPath_test.cpp#L274), `maxAbsDiff` feeds `fabs(a-b)` to `std::max`. A NaN difference is ignored, leaving a reported maximum of zero. Replacing every computed D element with NaN still lets `PartialLastMXBlockSumsOnlyElementsBelowK` pass.
>
> Suggestion commit [f15ee4aacee](https://github.com/newling/rocm-libraries/commit/f15ee4aacee6031017b93c151ab6f2b3bb618a91) directly compares the finite expected vector with D and retains per-case context. The same all-NaN mutation fails with the fixed assertion. See the appendix for the exact negative control.
>
> ### Remove the obsolete equal-block slow-tail rejection
>
> [client/cpu_gemm_driver.cpp:672](https://github.com/ROCm/rocm-libraries/blob/e24bb5950c6eacb30a5eb2e201d7e5fba9af6848/projects/hipblaslt/tensilelite/client/cpu_gemm_driver.cpp#L672) rejects K below or not divisible by an MX block and points users to `--tryFastPath`. The reviewed base already includes [#13630](https://github.com/ROCm/rocm-libraries/pull/13630), which clamps the slow segment to remaining K.
>
> Suggestion commit [b5d8bb5f9a5](https://github.com/newling/rocm-libraries/commit/b5d8bb5f9a552f26384ff3202fc100c608c10014) removes the obsolete K restriction and its help text, and converts the two old BadK tests into positive slow-path tail tests. The corresponding CMake commentary is corrected in [66ef187d435](https://github.com/newling/rocm-libraries/commit/66ef187d43535cacaba49a328b77b0388909291e). Update the PR description's slow-path over-read and before/after claims against the current base; no replacement description was drafted.
>
> ## Suggestions
>
> ### Share negative-test checking across the stack
>
> [tests/CMakeLists.txt:1158](https://github.com/ROCm/rocm-libraries/blob/e24bb5950c6eacb30a5eb2e201d7e5fba9af6848/projects/hipblaslt/tensilelite/tests/CMakeLists.txt#L1158) and the earlier injected-failure/MX tests accept any nonzero result through `WILL_FAIL`. Suggestion commit [66ef187d435](https://github.com/newling/rocm-libraries/commit/66ef187d43535cacaba49a328b77b0388909291e) introduces one small CMake helper that requires exit code 1 and the intended diagnostic. All 36 existing negative cases pass it; a control with an unrelated expected error correctly fails. This improves error-path coverage without multiplying the feature matrix.
>
> ### Expose the eligibility reason to the driver
>
> The existing `rejectFast` reasons are hidden behind `if(false)`, while `--tryFastPath` reports only generic ineligibility. The suggestion adds an optional diagnostic output to the existing predicate and replaces the disabled logger with readable reasons. Two driver cases verify the actual messages for AMAX-D and MX blocks smaller than the fast K tile. The commit mapping below identifies this change.
>
> ## Scope decisions
>
> Asymmetric MX blocks remain rejected by the slow driver: that loop still uses the maximum block length, so the equal-block tail correction does not establish asymmetric support. One-sided MX, misaligned fast blocks and non-E8 fast scales retain their eligibility restrictions.
>
> The older approximate `maxAbsDiff` callers are not all rewritten by the new exact-vector assertion. Their general NaN handling can be hardened separately; the newly added exact tail test now catches the demonstrated failure.
>
> ## Appendix: exact NaN negative control
>
> At the submitted head, add `<algorithm>` and `<limits>` to `ReferenceMXFastPath_test.cpp`, then insert this line immediately before the assertion at line 274, after `SolveGemmCPU`:
>
> ```cpp
> std::fill(d.begin(), d.end(), std::numeric_limits<float>::quiet_NaN());
> ```
>
> Build and run only that test:
>
> ```sh
> cmake --build "$BUILD_DIR" --target tensilelite-tests -j 4
> OMP_THREAD_LIMIT=8 OMP_NUM_THREADS=4 KMP_BLOCKTIME=0 \
>   "$BUILD_DIR/tensilelite/tests/tensilelite-tests" \
>   --gtest_filter=ReferenceMXFastPath.PartialLastMXBlockSumsOnlyElementsBelowK
> ```
>
> The submitted assertion passes. With [f15ee4aacee](https://github.com/newling/rocm-libraries/commit/f15ee4aacee6031017b93c151ab6f2b3bb618a91) it fails. The mutation is a temporary control and is not part of the suggestion branch.
>
> ## Suggestion branch handoff
>
> | Suggestion commit | Item |
> | --- | --- |
> | [f15ee4aacee](https://github.com/newling/rocm-libraries/commit/f15ee4aacee6031017b93c151ab6f2b3bb618a91) | reject NaNs in the MX tail reference check |
> | [b5d8bb5f9a5](https://github.com/newling/rocm-libraries/commit/b5d8bb5f9a552f26384ff3202fc100c608c10014) | allow supported MX tails in the slow driver path |
> | [66ef187d435](https://github.com/newling/rocm-libraries/commit/66ef187d43535cacaba49a328b77b0388909291e) | require the intended CPU driver failure diagnostic |
> | [c7540acfc31](https://github.com/newling/rocm-libraries/commit/c7540acfc31bd3bfc502810669d0e598c14e3c57) | explain CPU fast-path ineligibility in the driver |
>
> The suggestion branches are published. Each branch contains its own corrections on its own submitted head, not the other review branches' fixes. Integrate shared changes in stack order, including the alias fix once and later `TypeInfo` call sites. PR-description corrections are reported for the author to make; none were published. See [the cross-PR assessment](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_cpu_reference_stack_15259_15278.md) for test overlap, CI limits and integration notes.
>

#### Final correctness review, linked November 14 (document dated November 13)

[Original on GitHub](https://github.com/newling/agent-public-reviews/blob/ede5c5f065835ae571ce0647b4e07af2c6cb8d64/rocm-libraries/review_pr15278_correctness_20261113.md) · [Newling’s source post](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5899752492)

Source revision: `ede5c5f065835ae571ce0647b4e07af2c6cb8d64`. File: `rocm-libraries/review_pr15278_correctness_20261113.md`.

> This is a review from an agent with an automatic prompt from the reviewer
>
> **PR reviewed:** [ROCm/rocm-libraries#15278](https://github.com/ROCm/rocm-libraries/pull/15278)
>
> **Submitted head:** `e24bb5950c6eacb30a5eb2e201d7e5fba9af6848`. **Incremental diff base:** `46ce0b0c3a676b3878f652e4dbf136bdb1f30f9f`.
>
> **Scope:** Focused follow-up on consequential CPU-reference errors and regression-test failures, including the existing review discussion. Head and discussion were unchanged at the 2026-11-13 23:09 UTC check. Earlier validation is reused where the tested code is unchanged.
>
> **Suggestion branch:** [review/pr15278-correctness-20261113](https://github.com/newling/rocm-libraries/tree/review/pr15278-correctness-20261113), based on the submitted head; final head [f15ee4aacee](https://github.com/newling/rocm-libraries/commit/f15ee4aacee6031017b93c151ab6f2b3bb618a91). Commit-to-item mapping is given below.
>
> ## Tests
>
> Reused submitted-head validation: 788 driver CTests and 42 reference tests passed, and the driver, tests and client built. For the retained assertion change, the seven `ReferenceMXFastPath` tests passed. Replacing all computed outputs with NaN still passed the submitted tail test and failed the fixed assertion. This is a demonstrated test blind spot, with no new GPU or performance result claimed.
>
> ## Summary
>
> The fast path loads zero beyond logical K, so an eight-element tile can cover a partial final MX scale block without summing padding. The test's nonzero padding and independent per-K expected sum are useful. Its comparison currently lets completely invalid output escape detection.
>
> ## Actionable items
>
> ### Make the exact tail test fail when any output is NaN
>
> At [tests/ReferenceMXFastPath_test.cpp:274](https://github.com/ROCm/rocm-libraries/blob/e24bb5950c6eacb30a5eb2e201d7e5fba9af6848/projects/hipblaslt/tensilelite/tests/ReferenceMXFastPath_test.cpp#L274), the test compares `maxAbsDiff(expected, d)` with zero. That helper folds `fabs(a-b)` through `std::max`; NaN differences are ignored. Filling every output element with NaN leaves the reported maximum at zero, and `PartialLastMXBlockSumsOnlyElementsBelowK` passes.
>
> The expected values in this test are finite and exact, so compare the vectors directly. Fix [f15ee4aacee](https://github.com/newling/rocm-libraries/commit/f15ee4aacee6031017b93c151ab6f2b3bb618a91) makes that change and preserves the case context. It catches the all-NaN negative control below without widening the test matrix.
>
> ## Suggestions
>
> None in this focused review.
>
> ## Scope decisions
>
> The production slow path already clamps its final segment at K, from [#13630](https://github.com/ROCm/rocm-libraries/pull/13630). I found no production over-read for equal MX block sizes in this change. The driver's inherited equal-block tail rejection and stale over-read explanation are excluded from this serious-defect list. Asymmetric slow-path block sizes remain a separate limitation.
>
> ## Appendix: assertion negative control
>
> With `<algorithm>` and `<limits>` included, insert this immediately before the tail test's final comparison, after `SolveGemmCPU`:
>
> ```cpp
> std::fill(d.begin(), d.end(), std::numeric_limits<float>::quiet_NaN());
> ```
>
> ```sh
> cmake --build "$BUILD_DIR" --target tensilelite-tests -j 4
> OMP_THREAD_LIMIT=8 OMP_NUM_THREADS=4 KMP_BLOCKTIME=0 \
>   "$BUILD_DIR/tensilelite/tests/tensilelite-tests" \
>   --gtest_filter=ReferenceMXFastPath.PartialLastMXBlockSumsOnlyElementsBelowK
> ```
>
> The submitted comparison passes; the fixed comparison fails. The temporary mutation is not included in the suggestion commit.
>
> **Suggestion summary:** [review/pr15278-correctness-20261113](https://github.com/newling/rocm-libraries/tree/review/pr15278-correctness-20261113) contains only [f15ee4aacee](https://github.com/newling/rocm-libraries/commit/f15ee4aacee6031017b93c151ab6f2b3bb618a91), the exact assertion correction. No new production tail-bounds defect was established.
>

### General discussion comments (3, including bots)

#### therock-pr-bot[bot] — comment 6692570887

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15278#issuecomment-6692570887) · [@therock-pr-bot[bot]](https://github.com/apps/therock-pr-bot) · Created: 2026-11-06 18:40:54 UTC · Updated: 2026-11-10 22:32:03 UTC

> <!-- therock-pr-bot-policy-check -->
> ### ❌ PR Check — Action Required
>
> | Check | Status | Details |
> |---|:---:|---|
> | 📝 **PR Description** | ✅ Pass | — |
> | ⛔ **Forbidden Files** | ✅ Pass | — |
> | 🧪 **Unit Test** | ✅ Pass | — |
> | 🔎 **pre-commit** | ❌ Fail | **Error:** Check concluded with `cancelled`. |
> | 🚫 **Draft PR** | 🔜 To Be Enabled | — |
> | 🚩 **Feature Flag** | 🔜 To Be Enabled | — |
> | 📊 **Code Coverage** | 🔜 To Be Enabled | — |
>
> > ⚠️ **1 policy check(s) failed.** Please address the issues above before this PR can be Reviewed.
> >
> > 🚫 **Please fix the failed policies**
> > - ❌ pre-commit
> >
> > The **`Not ready to Review`** label was added to this PR. Once all policies pass, the label is removed automatically.
>
> 📖 **Need help?** See the [Policy FAQ](https://github.com/ROCm/rocm-libraries/blob/develop/docs/LIBRARIES_PR_BOT_FAQ.md) for details on every check and how to fix failures.
>
> 🙋 **[Wish to Override Policy?](https://github.com/ROCm/rocm-libraries/blob/develop/docs/LIBRARIES_PR_BOT_FAQ.md#-wish-to-override-the-policy-process-and-get-unblocked)**

#### therock-pr-bot[bot] — comment 6692784801

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15278#issuecomment-6692784801) · [@therock-pr-bot[bot]](https://github.com/apps/therock-pr-bot) · Created: 2026-11-06 18:51:45 UTC · Updated: 2026-11-10 15:59:35 UTC

> <!-- therock-pr-bot-fix-policies -->
> 🎉 All checks passed! This PR is ready for review.

#### codecov[bot] — comment 6788312206

[Original on GitHub](https://github.com/ROCm/rocm-libraries/pull/15278#issuecomment-6788312206) · [@codecov[bot]](https://github.com/apps/codecov) · Created: 2026-11-11 05:55:42 UTC · Updated: 2026-11-11 06:50:17 UTC

> ## [Codecov](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15278?dropdown=coverage&src=pr&el=h1&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) Report
> :white_check_mark: All modified and coverable lines are covered by tests.
>
> :x: Your project check has failed because the head coverage (54.34%) is below the target coverage (80.00%). You can increase the head coverage or adjust the [target](https://docs.codecov.com/docs/commit-status#target) coverage.
>
> <details><summary>Additional details and impacted files</summary>
>
>
>
> ```diff
> @@                                       Coverage Diff                                        @@
> ##           users/alvasile/tl-speedup-27-reference-fast-path-use-e-gradient   #15278   +/-   ##
> ================================================================================================
>   Coverage                                                            77.67%   77.67%
> ================================================================================================
>   Files                                                                 3002     3002
>   Lines                                                               521981   521974    -7
>   Branches                                                             81427    81424    -3
> ================================================================================================
> + Hits                                                                405414   405419    +5
> - Misses                                                               88742    88745    +3
> + Partials                                                             27825    27810   -15
> ```
>
> | [Flag](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15278/flags?src=pr&el=flags&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | Coverage Δ | | *Carryforward flag |
> |---|---|---|---|
> | [TensileLite-CPP](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15278/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `54.34% <ø> (+<0.01%)` | :arrow_up: | |
> | [TensileLite-Unit](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15278/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `83.84% <ø> (ø)` | | |
> | [hipBLAS](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15278/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `90.62% <ø> (ø)` | | Carriedforward from [5b4f85f](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/5b4f85f05f2977d06f67cf5405eff41da6800f93?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipBLASLt](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15278/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `70.42% <ø> (+0.04%)` | :arrow_up: | |
> | [hipCUB](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15278/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `82.68% <ø> (ø)` | | Carriedforward from [5b4f85f](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/5b4f85f05f2977d06f67cf5405eff41da6800f93?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipDNN](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15278/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `87.34% <ø> (ø)` | | Carriedforward from [5b4f85f](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/5b4f85f05f2977d06f67cf5405eff41da6800f93?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipFFT](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15278/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `42.65% <ø> (ø)` | | Carriedforward from [5b4f85f](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/5b4f85f05f2977d06f67cf5405eff41da6800f93?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipRAND](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15278/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `76.12% <ø> (ø)` | | Carriedforward from [5b4f85f](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/5b4f85f05f2977d06f67cf5405eff41da6800f93?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipSOLVER](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15278/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `69.67% <ø> (ø)` | | Carriedforward from [5b4f85f](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/5b4f85f05f2977d06f67cf5405eff41da6800f93?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [hipSPARSE](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15278/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `87.03% <ø> (ø)` | | Carriedforward from [5b4f85f](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/5b4f85f05f2977d06f67cf5405eff41da6800f93?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocALUTION](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15278/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `86.47% <ø> (ø)` | | Carriedforward from [5b4f85f](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/5b4f85f05f2977d06f67cf5405eff41da6800f93?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocBLAS](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15278/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `69.16% <ø> (ø)` | | Carriedforward from [5b4f85f](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/5b4f85f05f2977d06f67cf5405eff41da6800f93?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocFFT](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15278/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `48.81% <ø> (ø)` | | Carriedforward from [5b4f85f](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/5b4f85f05f2977d06f67cf5405eff41da6800f93?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocRAND](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15278/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `57.32% <ø> (ø)` | | Carriedforward from [5b4f85f](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/5b4f85f05f2977d06f67cf5405eff41da6800f93?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocSOLVER](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15278/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `76.66% <ø> (ø)` | | Carriedforward from [5b4f85f](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/5b4f85f05f2977d06f67cf5405eff41da6800f93?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocSPARSE](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15278/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `83.86% <ø> (ø)` | | Carriedforward from [5b4f85f](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/5b4f85f05f2977d06f67cf5405eff41da6800f93?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocSPARSE-unit](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15278/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `79.82% <ø> (ø)` | | Carriedforward from [5b4f85f](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/5b4f85f05f2977d06f67cf5405eff41da6800f93?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
> | [rocThrust](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15278/flags?src=pr&el=flag&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | `93.15% <ø> (ø)` | | Carriedforward from [5b4f85f](https://app.codecov.io/gh/ROCm/rocm-libraries/commit/5b4f85f05f2977d06f67cf5405eff41da6800f93?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) |
>
> *This pull request uses carry forward flags. [Click here](https://docs.codecov.io/docs/carryforward-flags?utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) to find out more.
>
> | [Files with missing lines](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15278?dropdown=coverage&src=pr&el=tree&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm) | Coverage Δ | |
> |---|---|---|
> | [...cts/hipblaslt/tensilelite/client/src/Reference.cpp](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15278?src=pr&el=tree&filepath=projects%2Fhipblaslt%2Ftensilelite%2Fclient%2Fsrc%2FReference.cpp&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm#diff-cHJvamVjdHMvaGlwYmxhc2x0L3RlbnNpbGVsaXRlL2NsaWVudC9zcmMvUmVmZXJlbmNlLmNwcA==) | `58.78% <ø> (+0.04%)` | :arrow_up: |
>
> ... and [8 files with indirect coverage changes](https://app.codecov.io/gh/ROCm/rocm-libraries/pull/15278/indirect-changes?src=pr&el=tree-more&utm_medium=referral&utm_source=github&utm_content=comment&utm_campaign=pr+comments&utm_term=ROCm)
> </details>
> <details><summary> :rocket: New features to boost your workflow: </summary>
>
> - :snowflake: [Test Analytics](https://docs.codecov.com/docs/test-analytics): Detect flaky tests, report on failures, and find test suite problems.
> - :package: [JS Bundle Analysis](https://docs.codecov.com/docs/javascript-bundle-analysis): Save yourself from yourself by tracking and limiting bundle sizes in JS merges.
> </details>

<a id="shared-assessment"></a>

## Shared cross-PR assessment of testing and design

Applies to [#15259](#pr-15259), [#15266](#pr-15266), [#15274](#pr-15274), [#15275](#pr-15275), [#15276](#pr-15276), [#15277](#pr-15277), [#15278](#pr-15278).

#### CPU-reference stack review, linked by newling on November 12

[Original on GitHub](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_cpu_reference_stack_15259_15278.md) · [Newling’s source post](https://github.com/ROCm/rocm-libraries/pull/15259#pullrequestreview-5867396457)

Source revision: `30df5bab4294f839ef85a91c732c36c92c0821ca`. File: `rocm-libraries/review_cpu_reference_stack_15259_15278.md`.

> This is a review from an agent with an automatic prompt from the reviewer
>
> # CPU-reference stack review
>
> Reviewed ROCm/rocm-libraries #15259, #15266, #15274, #15275, #15276, #15277 and #15278 independently, in dependency order. The seven submitted heads built and passed their existing tests. Targeted probes nevertheless found missing coverage for C/D aliasing, scaled input narrowing, unsupported reduction cleanup, non-HPA bias accumulation and NaN-sensitive comparison.
>
> ## Test assessment
>
> The added tests are generally fast and useful, but they are not sufficient by themselves to establish the claimed behavior. The full submitted stack has 788 driver tests, adding 505 to the 283-test baseline. It took 5.21 s with four CTest workers. Focused reference gtests took 35–81 ms across the submitted heads. These timings describe a local host test run, not a performance replay of the affected GPU workloads.
>
> | PR / review | New driver cases | Submitted driver / reference gtests | Driver wall time | Main gap |
> | --- | ---: | ---: | ---: | --- |
> | [#15259](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15259.md) | 14 | 297 / 24 | 2.29 s | No correctness defect found; verify real OpenMP team limits |
> | [#15266](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15266.md) | 64 | 361 / 24 | 2.29 s | Int32 scalar boundary and diagnostic checking |
> | [#15274](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15274.md) | 79 | 440 / 30 | 2.74 s | Aliased Int32 C/D |
> | [#15275](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15275.md) | 207 | 647 / 32 | 4.18 s | Scaled Half-to-Float8 conversion |
> | [#15276](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15276.md) | 53 | 700 / 35 | 4.54 s | Aliased complex C/D |
> | [#15277](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15277.md) | 70 | 770 / 40 | 5.10 s | Unsupported batched reduction and non-HPA rounding |
> | [#15278](https://github.com/newling/agent-public-reviews/blob/30df5bab4294f839ef85a91c732c36c92c0821ca/rocm-libraries/review_pr15278.md) | 18 | 788 / 42 | 5.21 s | NaN false positives and obsolete slow-tail rejection |
>
> A normalized command comparison found only two duplicate groups: `fast_F32_Sanity`/`fast_f32_NN`, and the corresponding slow pair. Both already existed before this stack. No added command duplicates another added command after option-order normalization. This does not prove every pair exercises independent logic; it does show that new registrations are not simply repeated invocations.
>
> The fast/slow matrices exercise distinct implementations. Keep the independent integer/golden oracle and the focused gtests: exact rounding, saturation, aliasing, E padding, sampling and poisoned K tails serve different purposes. The Float8 matrix already reduces feature permutations for hybrid IDs. There is no measured runtime reason for wholesale deletion of these tests. The strongest improvements are compact boundary regressions and making negative tests verify their intended failure.
>
> The submitted tests did not cover the reference's batched-gradient fallback because the driver rejects those arguments first. Similarly, `noscale` exclusions in test registration did not prevent the solver from accepting scaled narrowing cases. Test exclusions need matching eligibility boundaries or a separately validated semantic policy.
>
> Use the binary directly for the focused gtests:
>
> ```sh
> cmake --build "$BUILD_DIR" --target cpu-gemm-driver tensilelite-tests tensilelite-client -j 16
> OMP_THREAD_LIMIT=8 OMP_NUM_THREADS=4 KMP_BLOCKTIME=0 \
>   ctest --test-dir "$BUILD_DIR/tensilelite/tests" -R CPUGemm -j 4 --output-on-failure
> OMP_THREAD_LIMIT=8 OMP_NUM_THREADS=4 KMP_BLOCKTIME=0 \
>   "$BUILD_DIR/tensilelite/tests/tensilelite-tests" \
>   '--gtest_filter=ReferenceFastPath.*:ReferenceMX*.*:*ReferenceMX*/*'
> ```
>
> The source explicitly requests 64 slow-path threads, overriding `OMP_NUM_THREADS`; `OMP_THREAD_LIMIT` is needed to constrain the participating team. AMAX-D passed with limits of 1, 3 and 8. A CTest regex matching `ReferenceFastPath` misses the category-registered gtests in this configuration.
>
> ## Design and unsupported behavior
>
> The useful unifications are concrete: consume tile sums directly during the epilogue, share bias-gradient reduction and its error handling, use `TypeInfo` as the type-mapping owner, and use one diagnostic-checking helper for failure tests. The tip suggestion also surfaces readable eligibility reasons through the existing predicate instead of requiring a source edit to enable logging.
>
> Preserve the explicit numerical boundaries. Scaled Half-to-Float8 inputs and non-HPA narrow bias reduction need slow fallback until a different policy is defined. The slow Vector ScaleAB omission is retained, not repaired mathematically. Slow asymmetric MX remains unsupported even though equal-block K tails already work. Complex epilogue restrictions remain. Int32 accumulation is exact only while intermediate sums fit.
>
> The MX-tail description incorrectly attributes current results to a slow-path over-read fixed in the base by #13630. The complex description's alpha-zero distinction was fixed in the base by #13482. Both descriptions need correction; no replacement descriptions were authored or posted.
>
> ## CI and validation limits
>
> The inspected CI snapshots include successful broad summaries alongside cancelled and failed optional jobs. Downloaded failure logs showed a policy check complaining about cancelled pre-commit, a gfx1250 strict GEMM pytest exceeding 1200 seconds, and a gfx950 ASan job timing out while downloading artifacts before tests began. Those logs do not establish CPU-reference regressions or local sanitizer coverage.
>
> The driver cases are not installed in the TheRock test package. They run in Math CI's unfiltered informational `coverage-cpp` lane. Focused gtests are categorized for standard/comprehensive/full, not quick. Passing ordinary installed quick lanes therefore does not establish that every new test ran. Making this host suite a dedicated gating job is a reasonable separate CI change; it is not implemented here.
>
> Local validation built all three affected host targets at every submitted head and each suggestion implementation. No device execution, GPU performance replay, Windows qualification or local sanitizer run was performed. Root/component hooks were invoked; these C++/CMake paths largely opt out of formatting, and the component Python hook selected no tests. Whitespace checks passed.
>
> ## Suggestion branch integration
>
> Each `review/prNNNN-suggestions` branch is based on that PR's exact submitted head. Its review lists the commits and validation. These are independently reviewable alternatives for applying each finding, not a combined repaired stack.
>
> Carry the common tile-to-D correction once from #15274 and retain #15276's complex regression. When carrying #15266's `TypeInfo` cleanup into later PRs, migrate the later-added `TypeTraits` references too. Apply the shared failure-test helper and readable eligibility diagnostics once from #15278, and preserve the newly added fallback reasons from #15275/#15277. Those integration edits have not been represented as a tested combined branch.
>
> The reviews and all seven suggestion branches are published. The original PRs are unchanged; no comments or formal reviews were posted to them. Build logs and other local evidence remain available separately.
>
> ## Published suggestion branches
>
> | PR | Branch | Suggestion commits |
> | --- | --- | --- |
> | [#15259](https://github.com/ROCm/rocm-libraries/pull/15259) | [Suggestions](https://github.com/newling/rocm-libraries/tree/review/pr15259-suggestions) | [f8ae33f6575](https://github.com/newling/rocm-libraries/commit/f8ae33f65755293d932e7066102d1b3378d71146) |
> | [#15266](https://github.com/ROCm/rocm-libraries/pull/15266) | [Suggestions](https://github.com/newling/rocm-libraries/tree/review/pr15266-suggestions) | [2fcafc2eee3](https://github.com/newling/rocm-libraries/commit/2fcafc2eee3f322abd349beeda112823931c8591), [088d585c895](https://github.com/newling/rocm-libraries/commit/088d585c895cff36e459ee0409c1b812edd9da81) |
> | [#15274](https://github.com/ROCm/rocm-libraries/pull/15274) | [Suggestions](https://github.com/newling/rocm-libraries/tree/review/pr15274-suggestions) | [1024648658c](https://github.com/newling/rocm-libraries/commit/1024648658c17b9f3dfafb3aa41929817081c84f), [3b524a30398](https://github.com/newling/rocm-libraries/commit/3b524a30398c6e136a3d049dc78fae2053ce3d97), [8886ad2d584](https://github.com/newling/rocm-libraries/commit/8886ad2d584b154fc7d986ce49f7b20e56661d7d) |
> | [#15275](https://github.com/ROCm/rocm-libraries/pull/15275) | [Suggestions](https://github.com/newling/rocm-libraries/tree/review/pr15275-suggestions) | [d930b5107f1](https://github.com/newling/rocm-libraries/commit/d930b5107f10b667c55bfe4a9454e4f4fa8f9d4b), [aef34bafaad](https://github.com/newling/rocm-libraries/commit/aef34bafaad8296981db27b07f6f2fe96bd6258e) |
> | [#15276](https://github.com/ROCm/rocm-libraries/pull/15276) | [Suggestions](https://github.com/newling/rocm-libraries/tree/review/pr15276-suggestions) | [7c3393ef929](https://github.com/newling/rocm-libraries/commit/7c3393ef929f82a8f899ded5a245816265954a1d) |
> | [#15277](https://github.com/ROCm/rocm-libraries/pull/15277) | [Suggestions](https://github.com/newling/rocm-libraries/tree/review/pr15277-suggestions) | [66106f99d91](https://github.com/newling/rocm-libraries/commit/66106f99d916da886beecc39e464681596074b6d), [de7815c2e50](https://github.com/newling/rocm-libraries/commit/de7815c2e5066f6b2ff76e589732a3b8a671e402), [26f8f56cec2](https://github.com/newling/rocm-libraries/commit/26f8f56cec2a4517b7edef1ade0da921334d228a) |
> | [#15278](https://github.com/ROCm/rocm-libraries/pull/15278) | [Suggestions](https://github.com/newling/rocm-libraries/tree/review/pr15278-suggestions) | [f15ee4aacee](https://github.com/newling/rocm-libraries/commit/f15ee4aacee6031017b93c151ab6f2b3bb618a91), [b5d8bb5f9a5](https://github.com/newling/rocm-libraries/commit/b5d8bb5f9a552f26384ff3202fc100c608c10014), [66ef187d435](https://github.com/newling/rocm-libraries/commit/66ef187d43535cacaba49a328b77b0388909291e), [c7540acfc31](https://github.com/newling/rocm-libraries/commit/c7540acfc31bd3bfc502810669d0e598c14e3c57) |
>
> ## Appendix: native counterexample source and setup
>
> The following exact probe source produced the aliasing, scalar-narrowing, non-HPA and invalid-gradient results cited above against the submitted stack tip. Save it as `cpu_reference_review_probes.cpp`. The committed regressions additionally cover both complex widths, both narrowed-operand directions and scale modes, both narrow reduction types, and A/B/D reduction error paths.
>
> ```cpp
> #include <Reference.hpp>
> #include <Tensile/ContractionProblem.hpp>
> #include <Tensile/DataTypes.hpp>
> #include <complex>
> #include <iostream>
> #include <string>
> #include <vector>
> using namespace TensileLite;
> using namespace TensileLite::Client;
> using DT = rocisa::DataType;
> ContractionProblemGemm problem(DT ab, DT cd, DT alpha, size_t m, size_t n, size_t k, size_t batches=1)
> {
>     auto p = ContractionProblemGemm::GEMM_Strides(false,false,ab,ab,cd,cd,m,n,k,batches,m,-1,k,-1,m,-1,m,-1,1.0);
>     p.setComputeInputTypeA(ab); p.setComputeInputTypeB(ab);
>     p.setAlphaType(alpha); p.setBetaType(alpha);
>     return p;
> }
> int main(int argc, char** argv)
> {
>     std::string mode=argv[1];
>     bool fast=std::string(argv[2])=="fast";
>     if(mode=="alias-i32")
>     {
>         auto p=problem(DT::Int8,DT::Int32,DT::Int32,1,1,1);
>         int8_t a=2,b=3; int32_t cd=5;
>         ContractionInputs in(&a,&b,&cd,&cd,int32_t(1),int32_t(1));
>         SolveGemmCPU(p,in,-1,fast); std::cout << cd << " expected 11\n";
>         return cd==11 ? 0:1;
>     }
>     if(mode=="alias-complex")
>     {
>         using T=std::complex<float>;
>         auto p=problem(DT::ComplexFloat,DT::ComplexFloat,DT::ComplexFloat,1,1,1);
>         T a(2,0),b(3,0),cd(5,1);
>         ContractionInputs in(&a,&b,&cd,&cd,T(1,0),T(1,0));
>         SolveGemmCPU(p,in,-1,fast); std::cout << cd << " expected (11,1)\n";
>         return cd==T(11,1) ? 0:1;
>     }
>     if(mode=="scale")
>     {
>         auto p=ContractionProblemGemm::GEMM_Strides(false,false,DT::Half,DT::Float8,DT::Float8,DT::Float8,1,1,1,1,1,-1,1,-1,1,-1,1,-1,0.0);
>         p.setComputeInputTypeA(DT::Float8);p.setComputeInputTypeB(DT::Float8);
>         p.setAlphaType(DT::Float);p.setBetaType(DT::Float);
>         p.setUseScaleAB("Scalar"); p.setScaleA(DT::Float,1);p.setScaleB(DT::Float,1);
>         Half a(0.9f);Float8 b(1.0f),c(0.0f),d(0.0f); float sa=3,sb=1;
>         ContractionInputs in(&a,&b,&c,&d,1.0f,0.0f);in.scaleA=&sa;in.scaleB=&sb;
>         SolveGemmCPU(p,in,-1,fast);std::cout << float(d) << " scaled before quantization: 2.75\n";
>         return float(d)==2.75f ? 0:1;
>     }
>     if(mode=="nonhpa")
>     {
>         constexpr size_t n=4097;
>         auto p=problem(DT::Half,DT::Half,DT::Half,1,n,1);
>         p.setHighPrecisionAccumulate(false);p.setUseGradient(true);p.setUseBias(1);
>         p.setBias(DT::Half,1,1,true,ContractionProblemGemm::D);
>         Half a(1.0f),bias(0.0f); std::vector<Half>b(n,Half(1.0f)),c(n,Half(0.0f)),d(n,Half(0.0f));
>         ContractionInputs in(&a,b.data(),c.data(),d.data(),Half(1.0f),Half(0.0f));in.bias=&bias;
>         SolveGemmCPU(p,in,-1,fast);std::cout << "bias=" << float(bias) << " first D=" << float(d[0]) << '\n';return 0;
>     }
>     if(mode=="invalid-gradient")
>     {
>         auto p=problem(DT::Float,DT::Float,DT::Float,1,1,1,2);
>         p.setUseGradient(true);p.setUseBias(1);p.setBias(DT::Float,1,1,true,ContractionProblemGemm::D);
>         std::vector<float>a(2,1),b(2,1),c(2,0),d(2,0);float bias=-99;
>         ContractionInputs in(a.data(),b.data(),c.data(),d.data(),1.0f,0.0f);in.bias=&bias;
>         try {SolveGemmCPU(p,in,-1,fast);std::cout << "returned success bias=" << bias << '\n';}
>         catch(std::exception const& e){std::cout << "exception: " << e.what() << '\n';return 0;}
>         return 1;
>     }
>     return 2;
> }
> ```
>
> In a Ninja build configured with `CMAKE_EXPORT_COMPILE_COMMANDS=ON`, build the normal `cpu-gemm-driver` first. Save the following as `build_native_probe.py`; it reuses that target's exact compiler and link settings, replacing only its main object. The build must correspond to the revision being checked.
>
> ```python
> from pathlib import Path
> import json, shlex, subprocess, sys
> build = Path(sys.argv[1]).resolve()
> source = Path(sys.argv[2]).resolve()
> obj = build / 'cpu-reference-review-probes.o'
> exe = build / 'cpu-reference-review-probes'
> entry = next(x for x in json.loads((build / 'compile_commands.json').read_text())
>              if x['file'].endswith('/tensilelite/client/cpu_gemm_driver.cpp'))
> args = shlex.split(entry['command'])
> args[args.index(entry['file'])] = str(source)
> args[args.index('-o') + 1] = str(obj)
> subprocess.run(args, cwd=entry['directory'], check=True)
> link = subprocess.check_output(['ninja', '-t', 'commands', 'cpu-gemm-driver'],
>                                cwd=build, text=True).splitlines()[-1]
> args = shlex.split(link)
> if args[0] == ':':
>     start = args.index('&&') + 1
>     args = args[start:args.index('&&', start)]
> old_obj = next(x for x in args if x.endswith('/cpu_gemm_driver.cpp.o'))
> args[args.index(old_obj)] = str(obj)
> args[args.index('-o') + 1] = str(exe)
> subprocess.run(args, cwd=build, check=True)
> print(exe)
> ```
>
> ```sh
> python3 build_native_probe.py "$BUILD_DIR" cpu_reference_review_probes.cpp
> OMP_THREAD_LIMIT=8 OMP_NUM_THREADS=4 KMP_BLOCKTIME=0 \
>   "$BUILD_DIR/cpu-reference-review-probes" alias-i32 fast
> ```
>
> Modes are `alias-i32`, `alias-complex`, `scale`, `nonhpa`, and `invalid-gradient`; the second argument selects `fast` or `slow`. Run the invalid-gradient case in its own process: the submitted reference aborts. Apply the relevant suggestion branch before checking its fix; the branches do not incorporate one another's changes.
>
