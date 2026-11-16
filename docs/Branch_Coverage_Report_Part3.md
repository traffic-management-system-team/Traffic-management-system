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
