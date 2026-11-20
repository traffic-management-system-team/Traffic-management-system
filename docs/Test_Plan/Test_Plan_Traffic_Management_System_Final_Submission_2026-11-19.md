# Software Test Plan

## Document control

Traffic Management System (Simulation), final submission revision 1.5, 19 November 2026. This plan covers the final C++/REST/browser implementation and records evidence boundaries for submission.

| Version | Date | Recorded contribution |
|---|---|---|
| 1.0 | 6 Oct 2026 | Daruvuri Pranav Sai, initial baseline |
| 1.1 | 6 Oct 2026 | GOWTHAM S, testing/security contribution |
| 1.2 | 6 Oct 2026 | Dinesh, architecture and module contribution |
| 1.3 | 6 Oct 2026 | C NITISH, design/GUI and traceability contribution |
| 1.4 | 19 Nov 2026 | Final submission reconciliation against implemented source and recorded audit evidence. Formal team/instructor sign-off pending. |
| 1.5 | 19 Nov 2026 | Applied the team-approved retrospective Sprint 3 documentation classification; preserved original sprint assignments, requirements, diagrams, testing evidence and limitations. No historical formal planning or instructor approval verified. |

The 1.0–1.3 rows reproduce the historical document revision records. They do not independently prove individual authorship of every document edit. Original documents and their complete wording are retained under docs/history/final-reconciliation-2026-11-19/. GitHub implementation authorship is described separately in the sprint change log.

## 1 Scope and references

Validate FR-01–12, NFR-01–08 and SEC-01–04 against the final SRS/SAD, TC-01–16 and supplemental ST cases. Features are arrivals, FIFO assignment/dequeue, signal rules, queue length/capacity, statistics, lifecycle/configuration, web snapshot refresh and error feedback. Exclusions remain physical signals, live sensors, real-world traffic fidelity and production-scale deployment.

## 2 Test items environment and frameworks

Backend C++17 custom regression executables are registered in CMake/CTest, including vehicle, queue, signal, model, validation, service, statistics, control API, HTTP, serializer and API contract. The API-enabled suite has 15 CTest targets. Counts denote executables, not total assertions. Frontend Vitest 5.1.0 tests client, hook, components and App in Node/jsdom. Playwright 1.65.0 uses Chromium and the real Crow API through Vite. V8 coverage measures frontend TS/TSX. Historical native line instrumentation used Microsoft.CodeCoverage.Console and MSVC Debug/PDB builds.

Recorded audit environment is Windows/PowerShell with installed Node/npm dependencies, existing Release binaries and local API 127.0.0.1:18080. Vite uses 127.0.0.1:5173 and API_PROXY_TARGET. Prior GitHub Actions verifies Windows/Ubuntu Debug/Release and core Clang analysis/ASan/UBSan. Exact historical tool versions and commits remain in preserved reports. Fresh native rebuilding stalled during audit, so current CTest results use existing binaries.

## 3 Strategy and levels

Unit tests exercise queue, vehicles, signals, validator/statistics and client/hooks/components. Integration tests exercise model/service/API/serializer interactions, lifecycle/config locks, concurrent operations, origin/error handling and snapshot consistency. System validation drives actual browser controls, configuration rejection/retention, signals/queues/stats, FIFO, navigation continuity and responsive widths. Boundary/negative tests cover invalid types/ranges/missing fields, malformed requests, empty queues, invalid lifecycle/signal transitions and queue capacity. No new tests are introduced by this documentation phase.

## 4 Test cases and traceability

TC-01–16 retain original IDs. The authoritative current table is docs/Test_Plan/Test_Case_Document_Part3.md and the detailed CSV is docs/sprint-1/RTM_TC_Mapping.csv. Both identify executable test files and revision-scoped actual outcomes. NFR-01 maps to existing browser source and navigation/control tests. Functional responsiveness tests do not quantify target-device latency. NFR-06 uses historical cross-platform CI. SEC-04 uses inspected source and historical core analysis with limitations.

## 5 Entry exit suspension and defect handling

Entry requires available dependencies, a successful appropriate build or clearly identified existing binaries, known configuration and isolated local services. API server must start before real-backend browser tests. Exit for executed scope requires no unresolved failure in that scope and documented limitations. Suspend live browser validation if the API is unavailable, then restore the documented environment and rerun. Record both failed and successful attempts. Do not relabel environment failures as product defects without reproduction in the supported environment. Confirmed defects require issue/evidence links before waiver or closure.

## 6 Schedule and actual dates

The original plan was prepared 6 Oct and referenced 7 Oct submission/initial execution. University testing window supplied by the student was 2–6 Nov 2026. Historical native/frontend verification is recorded 15–16 Nov at 42a57c4, with PR #53 CI and two-page GUI on 16 Nov. Fresh audit execution occurred 19 Nov. Work and testing therefore continued after the planned window. The sprint change log records actual implementation dates and unassigned later work. No retrospective date or formal original Sprint 3 planning/instructor approval is asserted.

On 19 November 2026, the team approved Sprint 3 — Web Migration (Retrospective Classification) for issues #26–35, using their observed 4–14 November 2026 IST activity window. This is a retrospective grouping, not a verified formal planning timebox or original GitHub iteration. Original Sprint 1/2 assignments, TC-01 through TC-16, actual testing dates, revision-bound results and coverage limitations are unchanged. Regression follow-through in original #10/PR #51 occurred on 15 November, PR #53's two-page integration on 16 November, and the audit on 19 November; these later activities are not backdated into the migration window.

Backlog scope increased by 44 story points; a historical rise in the burndown chart has not been independently verified.

## 7 Responsibilities

Historical TC owners remain Dinesh TC-01–03/16, Gowtham TC-04–08 and Nitish TC-09–15, with Pranav coordinating RTM/integration evidence. This allocation describes responsibility, while actual PR authors, integration and reviews are recorded separately. Team members should explain the tests and code they contributed.

## 8 Commands and recorded results

Use BUILD_TESTING.md, frontend/README.md and docs/REST_API.md for supported setup. Commands are npm run type-check, npm test, npm run build, npm run test:e2e from frontend and ctest --test-dir build/release -C Release --output-on-failure from root. Fresh 19 Nov audit results were 33/33 Vitest, 15/15 existing Release CTest and 3/3 Chromium Playwright, with TypeScript/build passing. The interrupted native build is not a successful fresh rebuild.

## 9 Metrics coverage deliverables and limitations

Fresh frontend lines 271/297 (91.24%), statements 282/311 (90.67%), functions 85/91 (93.40%) and branches 263/326 (80.67%). Historical C++ lines 1,090/1,219 (89.42%) at 42a57c4. Native branch coverage remains NOT MEASURED. No complete project branch percentage or arbitrary passing threshold is defined. Preserve raw scope and revision identity.

Deliverables are the Test Case Document, RTM, unit/system/code/branch reports and final evidence ledger. Unexecuted scopes include fresh native rebuild completion, fresh sanitizers, broad browser/assistive-technology matrix and quantitative performance. Videos/submission durations remain NOT VERIFIED.

Reconciled 19 November 2026 for the 20 November 2026 viva. Source baseline 5cbdc012591221ccf300a9656b55bb1c5c38a2d2, branch feature/professional-traffic-ui, plus the seven existing uncommitted frontend redesign files. This documentation change does not commit or approve the redesign. Execution results below belong to the 19 November audit working tree. Historical results retain their original dates and revisions. No new execution is implied by this reconciliation.
