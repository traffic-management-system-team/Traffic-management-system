# Branch Coverage Report â€” Part 3

## Final submission reconciliation 19 November 2026

Reconciled 19 November 2026 for the 20 November 2026 viva. Source baseline 5cbdc012591221ccf300a9656b55bb1c5c38a2d2, branch feature/professional-traffic-ui, plus the seven existing uncommitted frontend redesign files. This documentation change does not commit or approve the redesign. Execution results below belong to the 19 November audit working tree. Historical results retain their original dates and revisions. No new execution is implied by this reconciliation.

## Current branch coverage status

Fresh frontend V8 branch counters are 263/326 = 80.67%, from 33 passing tests on the 19 November audit working tree. Historical 79.90% is retained below for 42a57c4 and its 38 tests. These measurements describe different frontend revisions.

| Frontend file/scope | Branch percent | Scope |
|---|---|---|
| BackendConnectionCard | 62.16% | Loading/error/compact/connection conditionals |
| SectionNavigation | 50% | Current-page branches |
| useBackendStatus | 69.38% | Polling, command and error outcomes |
| ControlCentre | 87.50% | Snapshot/approach availability |
| SimulationMonitor | 86.51% | Signal, queue and detected-event branches |

These are V8 source branch counters, not C++ control-flow branches. Fresh raw frontend branch-map availability is NOT VERIFIED, so no new per-line branch listing beyond recorded audit evidence is invented.

**C++ branch coverage remains NOT MEASURED.** Microsoft collector Cobertura line entries carry branch=False and supply no branch outcomes. The XML root branch-rate=1 cannot establish branch coverage. Historical native 89.42% is line/block coverage only. There is no verified complete-project branch percentage. This mandatory evidence gap remains open. A future native branch-capable instrumented run must identify compiler/tool, source revision, branch numerator/denominator and excluded scopes. No threshold is invented.

Full commands, initial failures, successful reruns, environment and scope are in [Final verification evidence](FINAL_VERIFICATION_EVIDENCE_2026-11-19.md). This is a documentation refresh of verified recorded evidence, not a new test run.

## Preserved historical report

The original report follows verbatim. All its dates, counts, PASS labels, conclusions and source claims apply to its original revision. The final submission status is the reconciliation above. An unchanged backup is also in docs/history/final-reconciliation-2026-11-19/docs/Branch_Coverage_Report_Part3.md.

# Branch Coverage Report â€” Part 3

**Repository:** https://github.com/traffic-management-system-team/Traffic-management-system
**Source commit:** `42a57c45311551058c41ccf54fec8c2ca88215d2`
**Date:** 2026-11-16
**Method:** Vitest V8 branch counters for frontend. C++ branch coverage was not measured.

## Frontend branch coverage

**PARTIAL â€” frontend measured; C++ NOT RUN.** Vitest/V8 covered 155 of 194 frontend branch counters: **79.90%**. The test suite completed 38/38 tests.

| File | Covered | Total | Percent |
|---|---:|---:|---:|
| `frontend/src/App.tsx` | 15 | 16 | 93.75% |
| `frontend/src/main.tsx` | 0 | 2 | 0% |
| `frontend/src/api/client.ts` | 24 | 27 | 88.89% |
| `frontend/src/api/endpoints.ts` | 0 | 0 | N/A |
| `frontend/src/api/errors.ts` | 13 | 15 | 86.67% |
| `frontend/src/api/types.ts` | 0 | 0 | N/A |
| `frontend/src/components/BackendConnectionCard.tsx` | 16 | 26 | 61.54% |
| `frontend/src/components/BrandMark.tsx` | 0 | 0 | N/A |
| `frontend/src/components/ConfigurationForm.tsx` | 26 | 30 | 86.67% |
| `frontend/src/components/IntersectionDashboard.tsx` | 15 | 16 | 93.75% |
| `frontend/src/components/SectionNavigation.tsx` | 0 | 0 | N/A |
| `frontend/src/components/SimulationControls.tsx` | 12 | 13 | 92.31% |
| `frontend/src/components/StatusBadge.tsx` | 0 | 0 | N/A |
| `frontend/src/hooks/useBackendStatus.ts` | 34 | 49 | 69.38% |
| **Total** | **155** | **194** | **79.90%** |

Vitest reported no applicable separate branch-true counter (total zero); this is not an additional metric.

## Uncovered frontend branch locations

The generated coverage map lists `App.tsx` line 28; `main.tsx` line 8; `api/client.ts` lines 66, 97, 115; `BackendConnectionCard.tsx` lines 17, 39, 49, 54, 88, 90, 94, 97; `ConfigurationForm.tsx` lines 81, 89, 199, 200; `IntersectionDashboard.tsx` line 151; `SimulationControls.tsx` line 94; and `useBackendStatus.ts` lines 53, 54, 62, 86, 106, 120, 141, 155, 164, 208.

The larger gaps include the unmounted application entrypoint, conditional paths in `BackendConnectionCard`, and polling/lifecycle/error paths in `useBackendStatus`. Locations are test opportunities, not defect claims. The underlying frontend line, statement and function metrics are documented in `docs/Code_Coverage_Report_Part3.md`.

## C++ branch coverage

**NOT RUN â€” no C++ branch result is available.** The MSVC coverage run measured C++ line/block execution only. Microsoft.CodeCoverage.Console's Cobertura export marked the line entries `branch=False` and did not provide branch outcome measurements. The XML root `branch-rate=1` is an exporter field and must not be interpreted as a coverage result. No C++ branch percentage is calculated, estimated or inferred.

The C++ run used MSVC 19.51.36256.0 x64 and Microsoft.CodeCoverage.Console 18.9.0-preview.26325.2 in a separate Debug build. Its raw `.coverage` and Cobertura XML are retained under `coverage/msvc-debug-42a57c45311551058c41ccf54fec8c2ca88215d2/`. The export limitation means C++ branch coverage remains outstanding despite the measured C++ line/block results.

## Conclusion

Frontend branch coverage is measured at **79.90%** (155/194). C++ branch coverage is **NOT RUN** and remains an explicit evidence gap.
