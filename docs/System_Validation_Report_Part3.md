# System Validation Report — Part 3

## Final submission reconciliation 19 November 2026

Reconciled 19 November 2026 for the 20 November 2026 viva. Source baseline 5cbdc012591221ccf300a9656b55bb1c5c38a2d2, branch feature/professional-traffic-ui, plus the seven existing uncommitted frontend redesign files. This documentation change does not commit or approve the redesign. Execution results below belong to the 19 November audit working tree. Historical results retain their original dates and revisions. No new execution is implied by this reconciliation.

## Current system validation results

The 19 November audit's real-backend Chromium suite passed 3/3 after starting the local C++ API. It exercises both Control Centre and Simulation Monitor, valid/invalid configuration, preservation of accepted config, lifecycle Start/Pause/Resume/Reset, live statistics, actual backend FIFO order, signals, detected activity, navigation continuity, keyboard skip link and widths 390/768/1440 without horizontal overflow. Configuration is restored after the integration test. Expected conflicts and rejected validation are exercised.

The first browser attempt failed 2/3 because no API listened at 127.0.0.1:18080, yielding proxy 502/ECONNREFUSED. This was an environment setup failure. After API startup the suite passed 3/3 in 28.7 seconds. These are automated checks. Fresh manual visual, screen-reader, broad browser and deployed smoke testing are not claimed. Existing redesign screenshots were produced in the earlier visual phase and are not fresh audit captures.

Backend state remains authoritative. The map shows actual stationary queue markers rather than physical trajectories. Activity is inferred between snapshots and can miss brief events. These are implementation limits reflected in the final SRS/SAD. Existing Release binaries powered this run. Fresh native rebuild completion remains NOT VERIFIED.

Full commands, initial failures, successful reruns, environment and scope are in [Final verification evidence](FINAL_VERIFICATION_EVIDENCE_2026-11-19.md). This is a documentation refresh of verified recorded evidence, not a new test run.

## Preserved historical report

The original report follows verbatim. All its dates, counts, PASS labels, conclusions and source claims apply to its original revision. The final submission status is the reconciliation above. An unchanged backup is also in docs/history/final-reconciliation-2026-11-19/docs/System_Validation_Report_Part3.md.


# System Validation Report — Part 3

**Repository:** https://github.com/traffic-management-system-team/Traffic-management-system
**Verified commit:** 42a57c45311551058c41ccf54fec8c2ca88215d2
**Environment/date:** Windows 11 Home build 26200, PowerShell 7.6.9; 2026-11-15
**Recorded result:** PASS for listed local REST and Chromium scenarios.

## Scope and methodology

The final C++17/Crow backend and React/TypeScript frontend ran together through Vite's same-origin proxy. API bound to loopback 127.0.0.1:18080. Playwright Chromium used the real backend, not API mocks. Environment: CMake 4.4.4, MSVC 19.51.36256.0, Windows SDK 10.0.26100.0, Node.js 22.17.0, npm 10.9.2, Playwright 1.65.0, Chromium 158.0.8127.18. Evidence is in root FINAL_PROJECT_VERIFICATION_REPORT.md; no tests were rerun for this report.

## REST API results

| Method / endpoint | Expected | Recorded result |
|---|---|---|
| GET /api/simulation/state | Authoritative current state; initial stopped state after reset. | **PASS — HTTP 200:** stopped, tick 0, empty queues, zero statistics. |
| PUT /api/simulation/config (valid) | Return accepted configuration. | **PASS — HTTP 200:** tickMs 500, greenTicks 5, yellowTicks 2, arrivalInterval 2, maxQueue 7 accepted. |
| PUT /api/simulation/config (invalid tickMs 99) | Reject with validation details; retain prior config. | **PASS — HTTP 400:** VALIDATION_FAILED and 100–5000 range; later state retained tickMs 500. Browser retained invalid draft greenTicks 0 while accepted value remained 10. |
| POST /api/simulation/start | Start stopped simulation. | **PASS — HTTP 200:** running. |
| POST /api/simulation/pause | Pause running simulation. | **PASS — HTTP 200:** paused; tick stayed 16 over two seconds. |
| POST /api/simulation/resume | Resume paused simulation. | **PASS — HTTP 200:** running; tick advanced 16 to 19. |
| POST /api/simulation/reset | Stop; clear tick/queues/stats; retain accepted config. | **PASS — HTTP 200:** stopped tick 0; queues and generated/processed/waiting zero; config retained. |

Expected HTTP 409 conflicts were exercised for Pause while stopped and stale-state Start. UI displayed errors, refreshed authoritative state where appropriate and remained usable.

## End-to-end validation

Recorded Playwright result: **3/3 Chromium tests passed** against C++ API through Vite proxy.

| Scenario | Expected | Actual |
|---|---|---|
| Initial stopped state | UI loads backend snapshot. | **PASS.** |
| Valid/invalid configuration | Accept valid; show field/general error for invalid; preserve accepted config and draft. | **PASS.** HTTP 400 with validation details; prior values preserved. |
| Start, arrivals, FIFO and processing | Backend progresses; vehicles remain FIFO and process only when permitted. | **PASS.** Live snapshots showed generated/processed vehicles and ascending IDs. |
| Signal transitions | Backend phases progress and are displayed. | **PASS.** Green/yellow transitions sampled and shown. |
| Statistics | API and UI counts update. | **PASS.** Generated/processed/waiting values changed during run. |
| Pause/resume | Pause halts; resume continues. | **PASS.** Tick held at 16 while paused and reached 19 after resume. |
| Reset | Stop, clear queues/stats, retain configuration. | **PASS.** Response/snapshot confirmed. |
| API errors | Show errors and keep UI usable. | **PASS.** Expected 409 cases handled. |

## Responsive/accessibility

Automated viewport checks at 390 px, 768 px and 1440 px found no horizontal overflow and kept primary content visible. Not a manual visual review. Browser records include keyboard skip-link and automated accessibility assertions. Full manual WCAG/screen-reader and cross-browser evaluation **NOT RUN**.

## Errors and cleanup

No uncaught page errors or failed network requests. One console “Failed to load resource” event was for the intentionally induced HTTP 409. MSVC emitted Crow header conversion/deprecation warnings; no build error. API/Vite processes were stopped; no listener remained on 18080 or 5173.

**Conclusion:** Recorded API and real-backend Chromium scenarios passed. Evidence is local Windows/Chromium only; no deployed smoke test or full accessibility/cross-browser certification is claimed.
