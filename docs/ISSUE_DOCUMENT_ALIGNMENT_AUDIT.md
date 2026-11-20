# GitHub issue and final document alignment audit

Audit date: 19 November 2026. Repository: [Traffic Management System](https://github.com/traffic-management-system-team/Traffic-management-system). Workspace: `C:\Users\prana\Downloads\Traffic-management-final-verification`. Inspected branch: `feature/professional-traffic-ui`; HEAD: `5cbdc012591221ccf300a9656b55bb1c5c38a2d2`, plus the existing uncommitted frontend and documentation work.

The audit found **22 actual issues: 18 FULL MATCH and 4 PARTIAL MATCH**. No whole issue is classified MISMATCH or NOT VERIFIED; individual evidence limits remain explicitly NOT VERIFIED. The final documents accurately describe the implemented browser/C++ REST system. Remaining partial classifications concern original desktop execution evidence and inconsistencies in the live issue records, including dependency/history wording and test-count/determinism claims. No new application failure was established by this read-only audit.

## Scope and decision rules

GitHub was queried live for all open and closed issues, full current bodies, assignments, labels, dates, project fields, closing PRs and cross-referenced/connected PR events. The issue list returned 22 issues; the GraphQL issue connection and every queried issue timeline reported no further page. Project 1 also returned 22 items, all Done. The gaps between #12 and #26 are not missing issue records; PRs and issues share GitHub numbering. A closed issue, checked checkbox, Done project status or merged PR was never sufficient by itself to prove acceptance.

“Original requested functionality” below summarizes the request in the **current retrieved issue body**, with the original desktop decision and migration history distinguished where supported. Bodies were edited during migration; an immutable creation-time wording archive for every issue was not recovered. Appendix A preserves the exact current bodies, including unchecked boxes, contradictory statements and historical run descriptions. This prevents a current-body audit from silently claiming to reconstruct every original October sentence.

FULL MATCH means the current delivered behavior and final-document mapping are supported for the issue's stated scope, with revision-bound results and explicit limits. PARTIAL MATCH means a material record contradiction, delivery-history discrepancy or unverified requested completion item remains. MISMATCH would mean contrary current behavior/documentation for the requested functionality. NOT VERIFIED would mean insufficient evidence to assess the issue as a whole. These are alignment judgments, not fresh execution certificates or university acceptance/sign-off.

Acceptance conclusions use inspected source/tests, recorded execution evidence and verified remote PR/CI metadata. **No builds, tests, browser interactions, coverage collection, commits or GitHub edits were performed by this audit.** Source/test presence establishes implemented checks, not a newly observed pass. An overall FULL MATCH does not assert quantitative performance, universal memory safety, all-browser accessibility or every historical build.

Only this new report is authorized for creation. Application files, tests, existing documents, coverage and the existing verification report are preserved. The separate Issue #27 integration worktree was not accessed.

## Authoritative final documents and diagram keys

The actual FINAL_REVIEW DOCX paragraphs and tables were read; the Test Plan was opened by exact filename to exclude the temporary Word lock file. The corresponding final Mermaid sources were compared with the active source. This audit did not re-render or alter the Word documents.

- [SRS FINAL_REVIEW](history/sprint-3-approved-2026-11-19/docs/SRS/SRS_Traffic_Management_System_FINAL_REVIEW_2026-11-19.docx), revision 1.4, 19 November 2026. SHA-256: `2fc0b76ea055382008178c51ab565f7d69b38935eede3ea6e3331056140d982c`.
- [SAD FINAL_REVIEW](history/sprint-3-approved-2026-11-19/docs/SAD/SAD_Traffic_Management_System_FINAL_REVIEW_2026-11-19.docx), revision 1.4, 19 November 2026. SHA-256: `ef59b0790500fc5d693965c9a4c65778a33d35c71eb6ad4dba6cb57534e4e5b7`.
- [Test Plan FINAL_REVIEW](history/sprint-3-approved-2026-11-19/docs/Test_Plan/Test_Plan_Traffic_Management_System_FINAL_REVIEW_2026-11-19.docx), revision 1.4, 19 November 2026. SHA-256: `3b6b241565b77b2d23c811519ab041d4ed8c90a07d8d093460fa23254ccb3b3a`.

Document references use the internal numbered sections. SRS §3.2 and its landscape traceability tables retain FR-01–FR-12, NFR-01–NFR-08 and SEC-01–SEC-04. The Plan §4 delegates detailed TC-01–TC-16 definitions to [Test Case Document](Test_Plan/Test_Case_Document_Part3.md) and [RTM case mapping](sprint-1/RTM_TC_Mapping.csv).

SAD §5 contains four rendered diagrams. Audit shorthand:

- **D1 Component:** React pages → shared hook/client → Crow host/adapter → control API/service → C++ modules, plus copied snapshot/serializer path. Source: [component.mmd](../diagrams/final-submission/component.mmd).
- **D2 Arrival Processing:** permitted FIFO departure and wait recording, rotating/capacity-limited arrival, then signal advancement. Source: [arrival-processing.mmd](../diagrams/final-submission/arrival-processing.mmd).
- **D3 Lifecycle:** request → shared service/mutex → model transition/reset → refreshed snapshot. Source: [lifecycle.mmd](../diagrams/final-submission/lifecycle.mmd).
- **D4 Configuration:** complete typed request → canonical validation → authoritative model state lock → accepted/rejected result → refresh. Source: [configuration.mmd](../diagrams/final-submission/configuration.mmd).

Additional SRS/TC mappings beyond an issue's own Traceability section are audit-derived from the implementation; they are not represented as extra IDs originally written by the issue author. Particularly, #29 contains no explicit SRS mapping.

## Recorded verification and its limits

The [final evidence ledger](FINAL_VERIFICATION_EVIDENCE_2026-11-19.md) and [final Test Plan](history/sprint-3-approved-2026-11-19/docs/Test_Plan/Test_Plan_Traffic_Management_System_FINAL_REVIEW_2026-11-19.docx) record:

- 19 November audit working tree: TypeScript and production build passed; Vitest **33/33** across four files; real-API Chromium Playwright **3/3**; CTest **15/15 using existing Release binaries** after permitted loopback access.
- The attempted fresh C++ rebuild stalled and was interrupted. A successful fresh native rebuild of the present working tree is **NOT VERIFIED**.
- Frontend recorded coverage: lines 271/297 = **91.24%**, branches 263/326 = **80.67%**. The ledger says durable availability of the fresh temporary raw coverage package is not verified. This audit did not reproduce those measurements.
- Native **89.42% line/block coverage** belongs to historical revision 42a57c4; C++ branch coverage is **NOT MEASURED**. There is no measured complete-project branch percentage.
- Historical #33: 14 API-enabled/11 core CTest and 36 Vitest tests apply to that older implementation. Historical #10/#51: 15 CTest and 38 Vitest tests belong to its recorded revision. The later #53/19 November 33-test frontend suite must not be conflated with either.
- Rechecked live: [PR #53](https://github.com/traffic-management-system-team/Traffic-management-system/pull/53) merged 16 November at `5cbdc012591221ccf300a9656b55bb1c5c38a2d2`; all **eight CI jobs pass** for PR head `88c9a001bd978c1326849aa542ac95f7b018fc4c`. [Backend/frontend/static/sanitizer workflow run](https://github.com/traffic-management-system-team/Traffic-management-system/actions/runs/43848601261) and [real-API Chromium workflow run](https://github.com/traffic-management-system-team/Traffic-management-system/actions/runs/43848600954). Those checks do not validate the subsequent uncommitted redesign.
- NFR-02 has functional responsiveness evidence, without a quantified target-device benchmark. SEC-04 has bounded source review and historical core analysis/sanitizers, not exhaustive proof. Full cross-browser and manual assistive-technology audits remain outside the verified scope.

The workflow files are [.github/workflows/issue-33-validation.yml](../.github/workflows/issue-33-validation.yml) and [.github/workflows/issue-34-browser-e2e.yml](../.github/workflows/issue-34-browser-e2e.yml). They implement CI validation; no automatic production deployment is established by these files.

## One row per issue

All issue titles, assignees, points and iteration fields below come from the live API/project. “Backlog/unassigned” means no project iteration, not an approved Sprint 3. Source paths in evidence cells are repository-relative.

| Issue and actual title | Assignee · points · iteration | Original requested functionality | Relevant SRS IDs | SAD component or diagram | Test case IDs | Actual implementation evidence | Acceptance criteria satisfied | Final document accuracy | Classification | Linked PR evidence |
|---|---|---|---|---|---|---|---|---|---|---|
| [#1](https://github.com/traffic-management-system-team/Traffic-management-system/issues/1) Finalize GUI framework and simulation timing parameters | pranav9360 · 2 SP · Sprint 1 | Select desktop framework and agree arrivals, logical ticks and signal timing. | FR-01, FR-04, FR-09 | §2/4; D1, D2, D3 | TC-01, TC-04, TC-09 | Issue decision comment dated 23 Oct explicitly selects C++17/Qt 6 Widgets, 1-second ticks, 10 GREEN/2 YELLOW ticks and North-first rotating arrivals every 3 ticks. `include/sim_config.h`, `src/simulation_model.cpp::tick/advanceSignalPhase/reset`; `tests/gui_model_test.cpp` preserve these defaults/semantics. | YES for the four decision ACs: framework/timing decisions and their visibility are verified. Choosing a framework does not prove the Qt application ran. | Accurate with evolution qualification: SRS §§2.2–2.3/3.3/4 and SAD §§2/4/7 explain the browser replacement, retained C++ rules and historical Qt. Final browser is intentionally different from the original desktop choice; instructor approval remains unverified. | **FULL MATCH** | Closing: [PR #20](https://github.com/traffic-management-system-team/Traffic-management-system/pull/20) merged |
| [#2](https://github.com/traffic-management-system-team/Traffic-management-system/issues/2) Implement Vehicle Manager | Dinesh07-18 · 3 SP · Sprint 1 | Create valid vehicle identities/records and pass them to correct queues. | FR-01, FR-02 | §2 component table; D1, D2 | TC-01, TC-02 | `src/vehicle.cpp`, `src/vehicle_manager.cpp::createVehicle`, `src/simulation_model.cpp::tick`; `tests/vehicle_manager_test.cpp`, `tests/queue_manager_test.cpp`, `tests/simulation_service_test.cpp::testSnapshotAndFifo`. | YES: configured arrivals create valid ID/type/lane values and enqueue them. CMake target and recorded execution exist. Actual arrival scheduling belongs to SimModel, rather than a second clock inside VehicleManager. | Accurate: SRS FR-01/02 and RTM; SAD VehicleManager/Model separation and arrival sequence; Plan §§2–4 cover core unit/integration tests. | **FULL MATCH** | Closing: [PR #15](https://github.com/traffic-management-system-team/Traffic-management-system/pull/15) merged; [PR #14](https://github.com/traffic-management-system-team/Traffic-management-system/pull/14) closed unmerged; not delivery proof |
| [#3](https://github.com/traffic-management-system-team/Traffic-management-system/issues/3) Implement Queue Manager with FIFO and empty-queue safety | Dinesh07-18 · 5 SP · Sprint 1 | Centralized FIFO enqueue/dequeue, queue lengths and empty-queue protection. | FR-02, FR-03, FR-06; NFR-03, NFR-07; SEC-02 | §2/4; D1, D2 | TC-02, TC-03, TC-06, TC-13 | `src/queue_manager.cpp::enqueue/dequeue/getSize/snapshot`; `tests/queue_manager_test.cpp`, `tests/queue_length_test.cpp`, `tests/queue_empty_safety_test.cpp`. | YES: correct-key assignment, FIFO front removal, changing counts and safe missing/empty queue behavior are implemented and covered. Snapshot drains a queue copy only. | Accurate: SRS requirement/RTM entries and §3.3; SAD queue component/arrival sequence; Plan unit and negative-test scope. | **FULL MATCH** | Closing: [PR #16](https://github.com/traffic-management-system-team/Traffic-management-system/pull/16) merged |
| [#4](https://github.com/traffic-management-system-team/Traffic-management-system/issues/4) Implement Traffic Signal Manager | gowth139am-lab · 3 SP · Sprint 1 | Maintain/query each approach's signals, reject illegal transitions and expose state for GUI display. | FR-04; SEC-03 | §2/4; D1, D2 | TC-04; TC-08 for display | `src/TrafficSignal.cpp::getState/changeState`; shared SignalCoordinator rejects invalid enums/transitions/conflicting GREEN. `src/simulation_state_serializer.cpp` and `SimulationMonitor.tsx` render copied signal values. `tests/test_traffic.cpp`, serializer and monitor tests. | YES: all four ACs have implementation/test evidence despite unchecked issue boxes. | Accurate: SRS FR-04/SEC-03 and §3.1; SAD signal coordination/serializer; Plan signal/boundary/browser scope. | **FULL MATCH** | Closing: [PR #17](https://github.com/traffic-management-system-team/Traffic-management-system/pull/17) merged |
| [#5](https://github.com/traffic-management-system-team/Traffic-management-system/issues/5) Implement signal-gated traffic processing | gowth139am-lab · 5 SP · Sprint 1 | Permit FIFO departure only under the matching GREEN signal; retain blocked vehicles; return useful processing status. | FR-05, FR-11; NFR-07 | §4; D1, D2 | TC-03, TC-05, TC-11 | `src/TrafficManager.cpp::processNextVehicle` returns bool and checks road identity, GREEN and nonempty queue before dequeue. `src/simulation_model.cpp::tick` consumes that status for statistics. `tests/test_traffic_manager.cpp`, `tests/security_reliability_test.cpp`. | YES: four ACs and safe integration are supported; RED/YELLOW/wrong-road cases retain vehicles. Unchecked boxes do not negate the source evidence. | Accurate: SRS §§3.2–3.3 and RTM, SAD arrival sequence and GREEN guard, Plan core negative/integration tests. | **FULL MATCH** | Closing: [PR #18](https://github.com/traffic-management-system-team/Traffic-management-system/pull/18) merged; [PR #17](https://github.com/traffic-management-system-team/Traffic-management-system/pull/17) merged cross-reference |
| [#6](https://github.com/traffic-management-system-team/Traffic-management-system/issues/6) Create initial GUI traffic visualization and controls | nitish5052 · 5 SP · Sprint 1 | Initial desktop intersection/lane, signal/queue visualization and Start/Pause/Resume/Reset foundation; keep full statistics a separate issue. | FR-08, FR-09, FR-12; NFR-01, NFR-08 | §2/7; D1, D3 | TC-08, TC-09, TC-12, TC-14 | Historical PR #19 changes `include/gui_model.h`, `src/gui_main.cpp`, `src/gui_model.cpp`, `tests/gui_model_test.cpp`; PR #20 subsequently replaces the Win32-only GUI with Qt. Qt source survives in `legacy/qt-prototype/`. Current App/ControlCentre/SimulationMonitor/SimulationControls and hook fulfill the migrated behavior; current component/E2E tests cover it. | CURRENT WEB ACs YES: lanes, authoritative signals/queues, lifecycle and refresh are present; statistics now has its separate backend module. ORIGINAL desktop build/run DoD NOT VERIFIED: no #6 comments, PR #19 has no usable run evidence, and PR #20 explicitly says Qt could not be configured/launched. | Final SRS/SAD accurately describe the delivered web replacement and archive. Plan does not establish an executed original desktop GUI. Do not claim that current browser passes retrospectively prove original desktop runtime acceptance. | **PARTIAL MATCH** | No automatic/project PR link. [PR #19](https://github.com/traffic-management-system-team/Traffic-management-system/pull/19) historical file correlation, then [PR #20](https://github.com/traffic-management-system-team/Traffic-management-system/pull/20) desktop replacement; both merged |
| [#7](https://github.com/traffic-management-system-team/Traffic-management-system/issues/7) Define backend-neutral simulation statistics contract | nitish5052 · 3 SP · Sprint 2 | Backend-neutral generated/processed/queue/throughput/waiting statistics contract usable by REST without frontend calculations. | FR-07, FR-08 | §2/3/4; D1, D2 | TC-07, TC-08 | `src/simulation_statistics_manager.cpp::snapshot`, `src/simulation_model.cpp::tick/statistics`, `src/simulation_state_serializer.cpp`; dedicated statistics tests and `docs/Simulation_Statistics_Contract.md`. | YES: counts, per-approach metrics, completed-vehicle waiting, zero handling, cumulative throughput and tick/event updates are in the core and consumed by REST. Assignment is Nitish; final integration PR #36 is authored by Pranav. | Accurate: SRS FR-07/08 and definitions, SAD statistics component/delegation, Plan unit/integration scope. Original ownership and later integration must remain distinct. | **FULL MATCH** | Closing: [PR #36](https://github.com/traffic-management-system-team/Traffic-management-system/pull/36) merged; [PR #23](https://github.com/traffic-management-system-team/Traffic-management-system/pull/23) closed unmerged; not delivery proof |
| [#8](https://github.com/traffic-management-system-team/Traffic-management-system/issues/8) Implement input validation and safe configuration handling | nitish5052 · 3 SP · Sprint 2 | Central C++ configuration defaults/bounds, safe complete validation, state-dependent acceptance and preservation of last accepted settings without Qt. | FR-10; SEC-01; NFR-03 | §2/3/4; D1, D4 | TC-09, TC-10, TC-13 | `include/sim_config.h`, `src/input_validator.cpp`, `src/config_controller.cpp`, `src/simulation_model.cpp::setConfig`; `tests/input_validator_test.cpp`, control API/model/service tests. Raw field diagnostics come from InputValidator; HTTP JSON type/schema handling belongs to #28/#33. | YES for the nine core ACs: validated configuration only; malformed/missing/non-integer/range errors; accepted-value preservation; authoritative stopped/tick-zero/generated-zero lock; lifecycle/reset regressions; independent core build and stable validation result. TC-10 is updated. | Accurate: SRS §3.3 gives all five actual bounds/defaults and state locks; SAD validation/service layers; Plan negative/boundary cases. #8 currently has no Story Points section, but its actual project field is 3. | **FULL MATCH** | Closing: [PR #37](https://github.com/traffic-management-system-team/Traffic-management-system/pull/37) merged; [PR #21](https://github.com/traffic-management-system-team/Traffic-management-system/pull/21) closed unmerged; not delivery proof; [PR #24](https://github.com/traffic-management-system-team/Traffic-management-system/pull/24) closed unmerged; not delivery proof |
| [#9](https://github.com/traffic-management-system-team/Traffic-management-system/issues/9) Integrate C++ simulation modules with REST API | pranav9360 · 5 SP · Sprint 2 | Integrate C++17/CMake/Crow host with one shared service, agreed six endpoints/envelope, independent core and retained legacy GUI history. | FR-01–FR-12; NFR-04, NFR-05 | §§2–6; D1–D4 | TC-01–TC-15, especially TC-15 | `src/api_server_main.cpp` constructs service before server; `src/simulation_http_server.cpp`, adapter/control/serializer and CMake target boundaries; HTTP/API contract tests; `docs/REST_API.md` records pinned dependencies and MSVC commands/results. | YES in the integrated repository: six endpoints and shared envelope, serialized access, stopped startup, separate host/service shutdown, type-preserving JSON adapter, retained archived Qt and independent core. Actual host integration is PR #41, not service-only PR #38. | Accurate: SRS localhost/browser interface, SAD component/endpoint tables and all four sequences, Plan API/concurrency/build scope. Qt is archived and not an active build dependency. | **FULL MATCH** | Closing: [PR #41](https://github.com/traffic-management-system-team/Traffic-management-system/pull/41) merged; [PR #38](https://github.com/traffic-management-system-team/Traffic-management-system/pull/38) merged cross-reference; [PR #39](https://github.com/traffic-management-system-team/Traffic-management-system/pull/39) merged cross-reference |
| [#10](https://github.com/traffic-management-system-team/Traffic-management-system/issues/10) Run C++ regression and web acceptance tests | gowth139am-lab · 5 SP · Sprint 2 | Execute C++ regressions and real-backend browser acceptance; record environment, errors, responsive/keyboard checks and blockers. | FR-01–FR-12; NFR-01, NFR-02, NFR-03, NFR-07, NFR-08 | §6 and D1–D4 tested boundaries | TC-01–TC-15; TC-14 responsiveness | `docs/Issue_10_CPP_Regression_Report.md`, `cpp-test-results-issue10-release.txt`, PR #51; `frontend/e2e/simulation.spec.ts`; current final evidence ledger. Real API and browser checks include IDs/counts, lifecycle, validation/retention, errors and viewport/keyboard paths. | YES for the six execution/reporting ACs at recorded revisions. PR #51 records historical 38/38 Vitest and 15/15 CTest; later #53/19 Nov record 33/33 and 15/15 existing binaries. Current fresh native rebuild remains unverified; no new test execution is asserted by this audit. | Accurate: Plan §§2/3/6/8/9 explicitly separates dates, revisions, existing binaries and interrupted rebuild. NFR-02 is functional evidence only, not a measured latency guarantee. | **FULL MATCH** | [PR #42](https://github.com/traffic-management-system-team/Traffic-management-system/pull/42) merged cross-reference; [PR #51](https://github.com/traffic-management-system-team/Traffic-management-system/pull/51) merged cross-reference; [PR #52](https://github.com/traffic-management-system-team/Traffic-management-system/pull/52) merged cross-reference; No closing/project PR field; [PR #51](https://github.com/traffic-management-system-team/Traffic-management-system/pull/51) is the substantive acceptance evidence |
| [#11](https://github.com/traffic-management-system-team/Traffic-management-system/issues/11) Perform security and reliability validation | Dinesh07-18 · 3 SP · Sprint 2 | Validate rejection of invalid input/signals, empty-queue safety and C++ memory/buffer review; record defect disposition. | SEC-01–SEC-04; NFR-03 | §3/4/6/7; D1, D2, D4 | TC-04, TC-10, TC-13, TC-16 | `tests/security_reliability_test.cpp`, validator/signal/empty-queue tests; `docs/Security_Reliability_Validation.md`; `docs/sprint-1/TC-16_Source_Memory_Safety_Review.md` at 92d7ab0 explicitly records findings F01–F06 and says no separate defect was opened because no reproducible defect was established. | YES for four bounded validation ACs and the repository's explicit no-actionable-defect disposition. BUT issue DoD simultaneously checks the defect item, says it must stay open, and concludes it is intentionally unchecked. Its completion record is internally contradictory. | Final documents accurately limit SEC-04 to reviewed paths/historical tools and retain PARTIAL assurance. They do not fix the contradictory GitHub checklist. This is a record mismatch, not evidence of an undisclosed confirmed memory defect. | **PARTIAL MATCH** | Closing: [PR #25](https://github.com/traffic-management-system-team/Traffic-management-system/pull/25) merged; [PR #22](https://github.com/traffic-management-system-team/Traffic-management-system/pull/22) closed unmerged; not delivery proof; [PR #50](https://github.com/traffic-management-system-team/Traffic-management-system/pull/50) closed unmerged; not delivery proof |
| [#12](https://github.com/traffic-management-system-team/Traffic-management-system/issues/12) Finalize web migration RTM, evidence, and demonstration | pranav9360 · 5 SP · Sprint 2 | Reconcile web migration RTM/SAD/Test Plan/README, preserve history, link evidence/limits and provide real-backend demo checklist. | FR-01–FR-12; NFR-01–NFR-08; SEC-01–SEC-04 | §§1–7; D1–D4 | TC-01–TC-16 | FINAL_REVIEW SRS/SAD/Plan; `docs/sprint-1/RTM.csv`, `RTM_TC_Mapping.csv`, `docs/Test_Plan/Test_Case_Document_Part3.md`; `docs/Issue_12_Demo_Readiness.md`, final evidence ledger and sprint change log; PRs #42/#52. | YES for six documentation/readiness ACs in the present working tree: each requirement has a status, history/limits and demo checklist exist. The requested checklist exists; actual video submission is a separate NOT VERIFIED handoff item, not an invented extra issue AC. | Accurate: final documents include two-page UI and correct NFR-01 browser-source mapping. Historical 38-test demo notes are explicitly revision-bound elsewhere; they must not be quoted as today's count. Final document changes remain uncommitted. | **FULL MATCH** | Closing: [PR #42](https://github.com/traffic-management-system-team/Traffic-management-system/pull/42) merged; [PR #52](https://github.com/traffic-management-system-team/Traffic-management-system/pull/52) merged cross-reference |
| [#26](https://github.com/traffic-management-system-team/Traffic-management-system/issues/26) Add Crow API host and serialized simulation service | Dinesh07-18 · 5 SP · Backlog/unassigned | Crow host/dependency verification plus single-owner synchronized SimulationService, scheduler, lifecycle and snapshots, independent of Qt. | NFR-04, NFR-05; FR-09/10 supporting | §§2/4/6; D1, D3, D4 | TC-09, TC-10, TC-12, TC-15 | `src/simulation_service.cpp::Impl` owns model/mutex/condition variable/one worker, copies snapshots and joins worker; `tests/simulation_service_test.cpp`; host integration in #41 and MSVC evidence in `docs/REST_API.md`/Issue #10 report. Current CMake pins Crow/Asio/json with FetchContent. | CURRENT integrated ACs supported, including later MSVC evidence. At #38 delivery its own description explicitly says MSVC v143 was NOT verified and routes were outside #26; issue closes 7 Nov before host #41 merges 9 Nov. Description still names Crow/vcpkg, while implemented dependency acquisition is FetchContent. | SAD §6 and referenced REST guide correctly describe current pinned dependencies/service. The issue's vcpkg wording and closure-time qualification are not reconciled by treating it as fully verified on 7 Nov. No current service functional failure established. | **PARTIAL MATCH** | Closing: [PR #38](https://github.com/traffic-management-system-team/Traffic-management-system/pull/38) merged |
| [#27](https://github.com/traffic-management-system-team/Traffic-management-system/issues/27) Expose read-only simulation state and vehicle snapshots | Dinesh07-18 · 5 SP · Backlog/unassigned | Copy-only consistent GET snapshot with four named signals, actual FIFO IDs/types, configuration, tick and defined statistics; separate data serializer from host/envelope. | FR-01–FR-08, FR-11, FR-12 | §2/3/4; D1 snapshot path; D2 context | TC-01–TC-08, TC-11, TC-12; TC-15 boundary | `src/queue_manager.cpp::snapshot` copies FIFO; service copies under mutex; `src/simulation_state_serializer.cpp::serializeSimulationStateData` accepts DTO only; adapter provides GET/envelope. Serializer/service/API contract tests verify counts/order/non-mutation. | YES for all seven ACs in integrated code: IDs/types/counts/config/stats, cardinal identity/order, copied consistent data, documented average, non-mutating reads and shared service. Serializer remains Crow-independent and json is outside tms_core. | Accurate: SRS §§3.1/3.3, SAD DTO/serializer/component/sequence descriptions, Plan integration/contract scope. Audited only this workspace and remote records; the separate #27 worktree was not accessed. | **FULL MATCH** | Closing: [PR #40](https://github.com/traffic-management-system-team/Traffic-management-system/pull/40) merged |
| [#28](https://github.com/traffic-management-system-team/Traffic-management-system/issues/28) Add simulation control and configuration REST endpoints | nitish5052 · 5 SP · Backlog/unassigned | Five command/config endpoints via same service; defined transition errors, preserved configuration, acknowledged pause/reset and single tick source. | FR-09, FR-10; SEC-01 | §2/3/4; D1, D3, D4 | TC-09, TC-10; TC-15 integration | `src/simulation_control_api.cpp::handle`, adapter, service and `SimModel::setConfig/reset`; `tests/simulation_control_api_test.cpp`, HTTP/API contract/service tests; `docs/Simulation_Control_API_Contract.md`. | YES for nine ACs: lifecycle conflicts documented/tested; reset retains config; complete five-integer validation delegates canonical bounds; malformed/type/range/state errors; shared lock and successful-application interval behavior. HTTP commands do not manually tick. | Accurate: SAD six endpoint table and lifecycle/config sequences; SRS configuration/lifecycle requirements; Plan negative/concurrency/integration scope. | **FULL MATCH** | Closing: [PR #39](https://github.com/traffic-management-system-team/Traffic-management-system/pull/39) merged |
| [#29](https://github.com/traffic-management-system-team/Traffic-management-system/issues/29) Scaffold React TypeScript Vite frontend and API client | pranav9360 · 3 SP · Backlog/unassigned | React/TypeScript/Vite/Tailwind scaffold, lockfile, typed client, configurable local proxy, responsive shell and consistent API errors without simulation rules. | FR-08, FR-09, FR-10, FR-12; NFR-04, NFR-06, NFR-08 (derived mapping) | §2/3/6; D1 | TC-08, TC-09, TC-10, TC-12, TC-15 (derived) | `frontend/package.json`, `package-lock.json`, `vite.config.ts`, `src/api/client.ts/types.ts/errors.ts`, `src/hooks/useBackendStatus.ts`, `frontend/README.md`; client/hook tests. Proxy /api preserves Host/Origin with changeOrigin false. | YES: documented npm ci/dev/type-check/test/build path, pinned lockfile, real envelope types, configurable API_PROXY_TARGET, backend-populated state, loading/network/error normalization and setup guide. | Accurate: SRS §2.2/3.1, SAD browser/hook/client/proxy and dependency boundary, Plan frontend framework/unit scope. Issue itself supplies no SRS IDs; mappings here are audit-derived. | **FULL MATCH** | Closing: [PR #43](https://github.com/traffic-management-system-team/Traffic-management-system/pull/43) merged |
| [#30](https://github.com/traffic-management-system-team/Traffic-management-system/issues/30) Build responsive intersection, signal, and vehicle queue dashboard | gowth139am-lab · 5 SP · Backlog/unassigned | Responsive four-approach actual signal/FIFO dashboard with non-color text cues, IDs/types/count/capacity and backend-only arrivals/departures. | FR-02, FR-03, FR-04, FR-06, FR-08, FR-11, FR-12; NFR-08 | §2/3/7; D1 | TC-02–TC-06, TC-08, TC-11, TC-12, TC-14 | `frontend/src/components/SimulationMonitor.tsx` scene/QueueCard; ControlCentre approach summary; `IntersectionDashboard.test.tsx`; E2E paused-snapshot ID/order/count comparison, yellow-signal checks and viewport/keyboard paths. | YES for five ACs in migrated two-page layout: all approaches named, textual/accessible signals, complete FIFO detail and actual capacity; markers reflect snapshots only. Scene limits six markers per approach while full queue lists retain all IDs. | Accurate: SRS §3.1 and SAD §§2/3/7 explicitly distinguish stationary queue markers from movement coordinates and describe full FIFO lists. Polling may miss brief transitions; no continuous-motion/countdown requirement is invented. | **FULL MATCH** | Closing: [PR #45](https://github.com/traffic-management-system-team/Traffic-management-system/pull/45) merged; [PR #43](https://github.com/traffic-management-system-team/Traffic-management-system/pull/43) merged cross-reference |
| [#31](https://github.com/traffic-management-system-team/Traffic-management-system/issues/31) Add web simulation controls and configuration form | pranav9360 · 5 SP · Backlog/unassigned | Accessible backend-driven lifecycle and complete five-field configuration form, separate draft/accepted values, field feedback, state locks and loading/disabled states. | FR-09, FR-10; SEC-01; NFR-08 | §2/3/4; D1, D3, D4 | TC-09, TC-10, TC-14 | `SimulationControls.tsx`, `ConfigurationForm.tsx`, `useBackendStatus.ts`, typed client; App tests verify endpoints, all fields, validation/conflict retention, keyboard submit and duplicate prevention; E2E uses real API. Both pages reuse lifecycle controls. | YES for eight ACs: authoritative command states, full submission, backend field details when provided, retained accepted values/draft on rejection, explicit state locks, no local ticks, usable responsive/loading controls and backend authority. Frontend displays bounds as guidance, not a separate acceptance engine. | Accurate: SRS §3.1/3.3, SAD service/configuration sequence, Plan browser/API validation scope. Configuration intentionally resides on Control Centre. | **FULL MATCH** | Closing: [PR #44](https://github.com/traffic-management-system-team/Traffic-management-system/pull/44) merged |
| [#32](https://github.com/traffic-management-system-team/Traffic-management-system/issues/32) Display live simulation statistics in the dashboard | nitish5052 · 3 SP · Backlog/unassigned | Display API-generated/processed/waiting/queue metrics, backend average-wait definition and accurate refresh/reset without client-side counters. | FR-07, FR-08 | §2/3/4; D1, D2 | TC-07, TC-08, TC-12, TC-14 | `ControlCentre.tsx::PerformanceMetrics` reads six snapshot fields and states completed-vehicle average/cumulative per-tick throughput; service/statistics serializer provides zero-safe values. Core statistics/API tests plus App/browser snapshot refresh/reset evidence. | YES for five ACs: snapshot-only statistics, zero/completed-vehicle definition, refresh/reset, no fabricated increments, responsive metric grid. Client .toFixed only formats values; it does not calculate simulation statistics. | Accurate: SRS definitions/FR-07/08 and two-page feature allocation; SAD statistics component; Plan unit/API/browser scope. Aggregate metrics are on Control Centre rather than requiring duplication on Monitor. | **FULL MATCH** | Closing: [PR #47](https://github.com/traffic-management-system-team/Traffic-management-system/pull/47) merged; [PR #43](https://github.com/traffic-management-system-team/Traffic-management-system/pull/43) merged cross-reference |
| [#33](https://github.com/traffic-management-system-team/Traffic-management-system/issues/33) Test REST API contracts, validation, and concurrency | Dinesh07-18 · 5 SP · Backlog/unassigned | Actual REST/service contract, validation/FIFO/statistics/concurrency tests, independent modules, traceability, frontend regressions and cross-platform/static/sanitizer CI. | FR-01–FR-12; NFR-01–NFR-08; SEC-01–SEC-04 | §4/6/7; D1–D4 | TC-01–TC-16, especially TC-15 | `tests/simulation_api_contract_test.cpp`, service/HTTP/control/serializer/statistics tests; `CMakeLists.txt`; `.github/workflows/issue-33-validation.yml`; RTM/TC-15; client/App/hook/monitor tests. Current CMake registers 12 core + 3 API = 15 targets. | SUBSTANTIVE coverage supported, but literal current counts do NOT match checked AC7 (11 core/14 total) and issue's 36-test historical snapshot. AC4 needs qualification: deterministic model tick tests exist, but service/scheduler tests use waitForTickCount/sleep_for and bounded real-time waits. Old queued/running CI wording is historical; PR #53 currently has all eight checks passing. | Final Plan correctly records current 15/33/3 and historical scope, rather than copying 14/36. Final SRS correctly keeps NFR-02/SEC-04 PARTIAL. No evidence supports claiming every service logical-tick assertion is wall-clock-free. See M4/M5. | **PARTIAL MATCH** | Closing: [PR #46](https://github.com/traffic-management-system-team/Traffic-management-system/pull/46) merged |
| [#34](https://github.com/traffic-management-system-team/Traffic-management-system/issues/34) Test browser integration, responsiveness, and accessibility | gowth139am-lab · 5 SP · Backlog/unassigned | Components plus real-C++-API browser integration, actual IDs/counts/signals/stats/config/lifecycle, responsive widths and keyboard/non-color cues in validation workflow. | FR-04, FR-07–FR-10, FR-12; NFR-01, NFR-02, NFR-08 | §2/6/7; D1, D3, D4 | TC-07–TC-10, TC-12, TC-14 | Four Vitest files cover client/hook/App/Monitor. `frontend/e2e/simulation.spec.ts` has three real-API Chromium tests; `.github/workflows/issue-34-browser-e2e.yml` builds/starts API then runs Playwright. PR #48 and later #53 CI verified. | YES for five scoped ACs: components, real API, 390/768/1440 widths on both pages, keyboard form/start/skip-link paths and textual/accessible signal state, documented workflow. Full cross-browser/WCAG/screen-reader audit is not established or claimed. | Accurate: Plan §§2–4/8/9, SRS §3.1 and NFR-01/02/08 mapping. NFR-02 remains PARTIAL for absent quantitative target-system performance evidence; this is not failure of the stated basic responsive/browser AC. | **FULL MATCH** | Closing: [PR #48](https://github.com/traffic-management-system-team/Traffic-management-system/pull/48) merged |
| [#35](https://github.com/traffic-management-system-team/Traffic-management-system/issues/35) Review REST API security and operational error handling | Dinesh07-18 · 3 SP · Backlog/unassigned | Local REST exposure, safe malformed/oversized/invalid inputs, generic errors, credential/log review and synchronized lifecycle/shutdown evidence without adding auth/persistence requirements. | SEC-01–SEC-04; NFR-03, NFR-05 | §3/4/6/7; D1, D3, D4 | TC-04, TC-10, TC-13, TC-15, TC-16; HTTP negative cases | `api_server_main.cpp`, HTTP bind/origin/server/adapter/response code, service exception boundaries and `patches/`; HTTP tests cover loopback/default/configured origins, malformed JSON, 64 KiB bounds, generic sensitive-detail suppression and active-request shutdown. `docs/REST_API.md` security review and TC-16 record findings/limits. | YES for the five bounded review/test ACs: loopback default/explicit broader bind, rejected input preserves config, stable generic errors, credential-free reviewed API configuration and fixed diagnostics, documented synchronization/shutdown review. No exhaustive all-history secret scan or formal safety proof is asserted. | Accurate: SRS localhost/security limits and SAD origin/body/lifecycle boundaries. Host/origin checks are not authentication; no built-in TLS/auth/database is claimed. Plan covers tested scope and retains SEC-04 assurance limits. | **FULL MATCH** | Closing: [PR #49](https://github.com/traffic-management-system-team/Traffic-management-system/pull/49) merged |

## Later work and contribution boundaries

The actual project assigns Sprint 1 to 19–23 October (#1–6, 23 points) and Sprint 2 to 26–30 October (#7–12, 24 points). #26–35 total 44 points and have no iteration assigned. Total: **91 points**. Completion extended beyond those dates; the original Sprint 2 allocation does not establish October delivery of the later web implementation.

The current project assignee is an ownership field. It is not interchangeable with final PR author or merger. In particular, #7 and #8 are assigned to nitish5052 while their merged completion PRs #36 and #37 are authored by pranav9360. Earlier unmerged drafts remain unmerged, rather than being counted as separate delivered implementations.

Later merged sequence, verified from live PR metadata (dates UTC):

- 2026-11-05T20:08:30Z: [PR #36](https://github.com/traffic-management-system-team/Traffic-management-system/pull/36) — Implement simulation statistics contract; author `pranav9360`; merge `801bc894b648f640f62e860d1cb3ac8de3b10628`.
- 2026-11-06T10:42:02Z: [PR #37](https://github.com/traffic-management-system-team/Traffic-management-system/pull/37) — Implement safe configuration validation and handling; author `pranav9360`; merge `2b39b51df42ea9c1031d237217537f20d52f44fb`.
- 2026-11-07T12:54:04Z: [PR #38](https://github.com/traffic-management-system-team/Traffic-management-system/pull/38) — Implement SimulationService with synchronized scheduler; author `Dinesh07-18`; merge `fdb26370b8b685bb866006a763e5b2f60724d8ec`.
- 2026-11-08T11:04:37Z: [PR #39](https://github.com/traffic-management-system-team/Traffic-management-system/pull/39) — Implement Simulation Control API Handlers (#28); author `nitish5052`; merge `e56998eb177272b63a04f368215257de143271e9`.
- 2026-11-09T16:05:24Z: [PR #41](https://github.com/traffic-management-system-team/Traffic-management-system/pull/41) — Issue #9: Integrate Web-Based REST Simulation Backend Closes #9; author `pranav9360`; merge `04b4a1cf59403a4af5263225aaa02078ba0c6d18`.
- 2026-11-10T04:37:46Z: [PR #42](https://github.com/traffic-management-system-team/Traffic-management-system/pull/42) — docs: finalize Issue #12 RTM and web migration readiness; author `pranav9360`; merge `19ce562176a386b88466743e1f92ed493a2863cc`.
- 2026-11-10T06:41:51Z: [PR #43](https://github.com/traffic-management-system-team/Traffic-management-system/pull/43) — feat(frontend): scaffold React app and typed API client; author `pranav9360`; merge `d61f5e7c72f88d1c6b34602dd83071a5a11fa263`.
- 2026-11-10T08:44:34Z: [PR #40](https://github.com/traffic-management-system-team/Traffic-management-system/pull/40) — Add read-only simulation state API; author `Dinesh07-18`; merge `2622442cc468bb008f360a7f8ee69fcacb14c6c4`.
- 2026-11-10T09:14:22Z: [PR #44](https://github.com/traffic-management-system-team/Traffic-management-system/pull/44) — feat(frontend): add simulation controls and configuration; author `pranav9360`; merge `94a303e319cb6d6cc8350d958703006bfa42143d`.
- 2026-11-10T15:25:55Z: [PR #45](https://github.com/traffic-management-system-team/Traffic-management-system/pull/45) — feat(frontend): implement Issue 30 intersection dashboard; author `gowth139am-lab`; merge `51b9461b65b8ec847b7edc2a07e9f672f65bf60e`.
- 2026-11-11T14:52:11Z: [PR #46](https://github.com/traffic-management-system-team/Traffic-management-system/pull/46) — test(api): add focused REST contract coverage for #33; author `Dinesh07-18`; merge `fea4a94e2c4c4376131dc8c839966d2d30a6cfa6`.
- 2026-11-12T15:16:44Z: [PR #47](https://github.com/traffic-management-system-team/Traffic-management-system/pull/47) — feat(dashboard): display live simulation statistics; author `nitish5052`; merge `d097a2c1378811d9f09e2f59aba0a4df0ebbac8d`.
- 2026-11-13T10:22:00Z: [PR #49](https://github.com/traffic-management-system-team/Traffic-management-system/pull/49) — Harden REST API error handling and security boundaries; author `Dinesh07-18`; merge `c481afd3a3fbfae6422e2696fca3d7e7bc88f786`.
- 2026-11-13T11:45:55Z: [PR #25](https://github.com/traffic-management-system-team/Traffic-management-system/pull/25) — Add security and reliability validation - Issue #11; author `Dinesh07-18`; merge `b3a970450c362b1a916d5708124216507a6269f3`.
- 2026-11-13T18:47:30Z: [PR #48](https://github.com/traffic-management-system-team/Traffic-management-system/pull/48) — test: complete Issue 34 browser and accessibility coverage; author `gowth139am-lab`; merge `0c913bb069ced0fd043329e2c9d3ed3d6aa7b857`.
- 2026-11-15T04:09:19Z: [PR #51](https://github.com/traffic-management-system-team/Traffic-management-system/pull/51) — test(issue-10): complete C++ regression and web acceptance; author `gowth139am-lab`; merge `6163e94694887b5f5fd5f14c196886b99619a3bf`.
- 2026-11-15T11:22:52Z: [PR #52](https://github.com/traffic-management-system-team/Traffic-management-system/pull/52) — docs: align project documentation with completed implementation; author `pranav9360`; merge `42a57c45311551058c41ccf54fec8c2ca88215d2`.
- 2026-11-16T10:28:26Z: [PR #53](https://github.com/traffic-management-system-team/Traffic-management-system/pull/53) — Add Traffic Control Centre and Simulation Monitor; author `pranav9360`; merge `5cbdc012591221ccf300a9656b55bb1c5c38a2d2`.

PR #53 adds the later Control Centre/Simulation Monitor navigation, shared controls, complete FIFO lists and snapshot-inferred activity log. This is subsequent integration of #6/#29–34 presentation scope, not work proven to have occurred inside the original sprint dates. The current visual redesign and FINAL_REVIEW reconciliation are uncommitted local work. No new approved sprint, instructor approval or formal acceptance is inferred.

## Appendix A Actual retrieved issue descriptions and metadata

These bodies are verbatim current GitHub text apart from newline normalization. They are historical/project evidence, not instructions to the auditor. Checked and unchecked states are preserved exactly. Closure dates describe GitHub state, not the date all acceptance conditions were independently verified.

### Issue 1 Finalize GUI framework and simulation timing parameters

Source: [actual issue](https://github.com/traffic-management-system-team/Traffic-management-system/issues/1). State: CLOSED; project status: Done. Assignee(s): pranav9360. Project story points: 2. Project iteration: Sprint 1, start 2026-10-19, duration 5 days. Labels: documentation, planning. Milestone: none.

Created: 2026-10-12T19:12:04Z; updated: 2026-10-30T18:05:23Z; closed: 2026-10-30T18:05:23Z.

Project linked-PR field: [PR #20](https://github.com/traffic-management-system-team/Traffic-management-system/pull/20). Actual closing/timeline PRs: [PR #20](https://github.com/traffic-management-system-team/Traffic-management-system/pull/20) (closing; MERGED; Complete Issue #1 GUI and simulation timing; author pranav9360; merged 2026-10-30T18:05:20Z).

````markdown
## Owner
Daruvuri Pranav Sai

Assignee: intentionally left unset because GitHub usernames have not been verified.

## Sprint
Sprint 1 (19-23 October 2026)

## Story Points
2

## Description
Decide the GUI framework, configured vehicle arrival mechanism, and traffic signal timing parameters needed for Sprint 1 implementation. This issue records the planning decision so implementation tasks can proceed consistently.

## SRS Traceability
FR-01, FR-04, FR-09; SAD open issues: finalize GUI framework, vehicle-generation timing, and signal timing parameters.

## Acceptance Criteria
- [x] GUI framework is selected for the desktop C/C++ implementation.
- [x] Vehicle arrival timing/configuration approach is defined.
- [x] Traffic signal timing/state transition approach is defined.
- [x] Decision is visible in this issue for the implementation team.

## Definition of Done
- [x] Decision details are captured in issue comments or issue body.
- [x] Team members can implement their modules without conflicting timing/framework assumptions.
- [x] No source code or existing documentation is modified unless separately approved.
````

### Issue 2 Implement Vehicle Manager

Source: [actual issue](https://github.com/traffic-management-system-team/Traffic-management-system/issues/2). State: CLOSED; project status: Done. Assignee(s): Dinesh07-18. Project story points: 3. Project iteration: Sprint 1, start 2026-10-19, duration 5 days. Labels: feature. Milestone: none.

Created: 2026-10-12T19:12:07Z; updated: 2026-11-13T12:12:10Z; closed: 2026-10-24T13:34:38Z.

Project linked-PR field: [PR #14](https://github.com/traffic-management-system-team/Traffic-management-system/pull/14), [PR #15](https://github.com/traffic-management-system-team/Traffic-management-system/pull/15). Actual closing/timeline PRs: [PR #15](https://github.com/traffic-management-system-team/Traffic-management-system/pull/15) (closing; MERGED; Implement Vehicle Manager; author Dinesh07-18; merged 2026-10-24T13:34:37Z); [PR #14](https://github.com/traffic-management-system-team/Traffic-management-system/pull/14) (cross-reference/connection; CLOSED UNMERGED; Implement Vehicle Manager; author Dinesh07-18).

````markdown
## Owner
Dinesh

Assignee: intentionally left unset because GitHub usernames have not been verified.

## Sprint
Sprint 1 (19-23 October 2026)

## Story Points
3

## Description
Implement the Vehicle Manager responsible for creating vehicle records/objects with valid initial state and preparing them for assignment to the appropriate road/lane queue.

## SRS Traceability
FR-01, FR-02; TC-01, TC-02.

## Acceptance Criteria
- [x] A vehicle can be created according to the configured arrival mechanism.
- [x] Each generated vehicle has valid initial state needed by queue and traffic modules.
- [x] Vehicle data can be passed to queue assignment logic.

## Definition of Done
- [x] Vehicle Manager code builds successfully.
- [x] Evidence for TC-01 and TC-02 is recorded in the issue or linked test notes.
- [x] Implementation follows the SAD module separation for Vehicle Manager responsibilities.
````

### Issue 3 Implement Queue Manager with FIFO and empty-queue safety

Source: [actual issue](https://github.com/traffic-management-system-team/Traffic-management-system/issues/3). State: CLOSED; project status: Done. Assignee(s): Dinesh07-18. Project story points: 5. Project iteration: Sprint 1, start 2026-10-19, duration 5 days. Labels: feature, testing. Milestone: none.

Created: 2026-10-12T19:12:09Z; updated: 2026-11-13T12:08:58Z; closed: 2026-10-24T16:15:42Z.

Project linked-PR field: [PR #16](https://github.com/traffic-management-system-team/Traffic-management-system/pull/16). Actual closing/timeline PRs: [PR #16](https://github.com/traffic-management-system-team/Traffic-management-system/pull/16) (closing; MERGED; Implement Queue Manager and safety tests; author Dinesh07-18; merged 2026-10-24T16:15:41Z).

````markdown
## Owner
Dinesh

Assignee: intentionally left unset because GitHub usernames have not been verified.

## Sprint
Sprint 1 (19-23 October 2026)

## Story Points
5

## Description
Implement road/lane queue operations using FIFO ordering, including enqueue, safe dequeue/access, queue-length reporting, and empty-queue guards.

## SRS Traceability
FR-02, FR-03, FR-06; NFR-03, NFR-07; SEC-02; TC-02, TC-03, TC-06, TC-13.

## Acceptance Criteria
- [x] Generated vehicles can be added to the correct road/lane queue.
- [x] FIFO order is preserved when vehicles are processed.
- [x] Queue length updates when vehicles enter or leave queues.
- [x] Empty queue access/dequeue is handled safely without crashing.

## Definition of Done
- [x] Queue Manager code builds successfully.
- [x] TC-03, TC-06, and TC-13 evidence is recorded.
- [x] Queue operations are centralized through the Queue Manager interface described in the SAD.
````

### Issue 4 Implement Traffic Signal Manager

Source: [actual issue](https://github.com/traffic-management-system-team/Traffic-management-system/issues/4). State: CLOSED; project status: Done. Assignee(s): gowth139am-lab. Project story points: 3. Project iteration: Sprint 1, start 2026-10-19, duration 5 days. Labels: feature. Milestone: none.

Created: 2026-10-12T19:12:12Z; updated: 2026-10-23T18:48:24Z; closed: 2026-10-23T18:48:24Z.

Project linked-PR field: [PR #17](https://github.com/traffic-management-system-team/Traffic-management-system/pull/17). Actual closing/timeline PRs: [PR #17](https://github.com/traffic-management-system-team/Traffic-management-system/pull/17) (closing; MERGED; Implement Traffic Signal Manager (#4); author gowth139am-lab; merged 2026-10-23T19:03:17Z).

````markdown
## Owner
GOWTHAM S

Assignee: intentionally left unset because GitHub usernames have not been verified.

## Sprint
Sprint 1 (19-23 October 2026)

## Story Points
3

## Description
Implement the Traffic Signal Manager for maintaining signal states for each controlled direction and validating supported traffic-signal transitions.

## SRS Traceability
FR-04; SEC-03; TC-04.

## Acceptance Criteria
- [ ] Signal state can be maintained for each controlled direction.
- [ ] Current signal state can be queried by simulation/traffic logic.
- [ ] Invalid signal transitions are rejected.
- [ ] Signal state can be displayed by the GUI layer.

## Definition of Done
- [ ] Traffic Signal Manager code builds successfully.
- [ ] TC-04 evidence is recorded.
- [ ] Signal state logic remains separated from GUI and queue logic as described in the SAD.
````

### Issue 5 Implement signal-gated traffic processing

Source: [actual issue](https://github.com/traffic-management-system-team/Traffic-management-system/issues/5). State: CLOSED; project status: Done. Assignee(s): gowth139am-lab. Project story points: 5. Project iteration: Sprint 1, start 2026-10-19, duration 5 days. Labels: feature, integration. Milestone: none.

Created: 2026-10-12T19:12:17Z; updated: 2026-10-25T17:06:07Z; closed: 2026-10-25T17:06:07Z.

Project linked-PR field: [PR #18](https://github.com/traffic-management-system-team/Traffic-management-system/pull/18). Actual closing/timeline PRs: [PR #18](https://github.com/traffic-management-system-team/Traffic-management-system/pull/18) (closing; MERGED; Implement signal-gated traffic processing (#5); author gowth139am-lab; merged 2026-10-26T14:38:52Z); [PR #17](https://github.com/traffic-management-system-team/Traffic-management-system/pull/17) (cross-reference/connection; MERGED; Implement Traffic Signal Manager (#4); author gowth139am-lab; merged 2026-10-23T19:03:17Z).

````markdown
## Owner
GOWTHAM S

Assignee: intentionally left unset because GitHub usernames have not been verified.

## Sprint
Sprint 1 (19-23 October 2026)

## Story Points
5

## Description
Implement traffic processing logic that checks signal state before allowing vehicles to leave a queue, while keeping blocked vehicles in their respective queues.

## SRS Traceability
FR-05, FR-11; NFR-07; TC-05, TC-11.

## Acceptance Criteria
- [ ] Vehicles leave a queue only when movement is permitted by the active signal state.
- [ ] Vehicles on non-permitted roads remain queued.
- [ ] Processing respects FIFO ordering provided by Queue Manager.
- [ ] Processing returns a useful status/count for controller and statistics use.

## Definition of Done
- [ ] Traffic processing integrates with Queue Manager and Traffic Signal Manager.
- [ ] TC-05 and TC-11 evidence is recorded.
- [ ] No unsafe queue operations are introduced by traffic processing.
````

### Issue 6 Create initial GUI traffic visualization and controls

Source: [actual issue](https://github.com/traffic-management-system-team/Traffic-management-system/issues/6). State: CLOSED; project status: Done. Assignee(s): nitish5052. Project story points: 5. Project iteration: Sprint 1, start 2026-10-19, duration 5 days. Labels: feature. Milestone: none.

Created: 2026-10-12T19:12:20Z; updated: 2026-10-29T09:09:51Z; closed: 2026-10-29T09:09:51Z.

Project linked-PR field: empty. Actual closing/timeline PRs: none returned.

````markdown
## Owner
C NITISH

Assignee: intentionally left unset because GitHub usernames have not been verified.

## Sprint
Sprint 1 (19-23 October 2026)

## Story Points
5

## Description
Create the initial GUI foundation with basic traffic visualization and simulation controls only. This includes intersection/lane display, visible signal/vehicle/queue state, and Start, Pause/Resume, and Reset controls. Full statistics implementation remains in the separate Statistics Manager issue.

## SRS Traceability
FR-08, FR-09, FR-12; NFR-01, NFR-08; TC-08, TC-09, TC-12.

## Acceptance Criteria
- [ ] GUI shows the intersection and incoming traffic lanes.
- [ ] GUI shows current traffic signal state and basic vehicle/queue state.
- [ ] Start, Pause/Resume, and Reset controls are available.
- [ ] GUI refreshes after relevant simulation-state changes.
- [ ] Statistics calculations are not implemented in this issue beyond any basic placeholder needed for layout.

## Definition of Done
- [ ] Initial GUI builds and runs.
- [ ] TC-08, TC-09, and TC-12 evidence is recorded for the implemented basic GUI scope.
- [ ] Full statistics work remains tracked by the Statistics Manager issue.
````

### Issue 7 Define backend-neutral simulation statistics contract

Source: [actual issue](https://github.com/traffic-management-system-team/Traffic-management-system/issues/7). State: CLOSED; project status: Done. Assignee(s): nitish5052. Project story points: 3. Project iteration: Sprint 2, start 2026-10-26, duration 5 days. Labels: feature. Milestone: none.

Created: 2026-10-12T19:12:23Z; updated: 2026-11-05T20:08:32Z; closed: 2026-11-05T20:08:32Z.

Project linked-PR field: [PR #36](https://github.com/traffic-management-system-team/Traffic-management-system/pull/36). Actual closing/timeline PRs: [PR #36](https://github.com/traffic-management-system-team/Traffic-management-system/pull/36) (closing; MERGED; Implement simulation statistics contract; author pranav9360; merged 2026-11-05T20:08:30Z); [PR #23](https://github.com/traffic-management-system-team/Traffic-management-system/pull/23) (cross-reference/connection; CLOSED UNMERGED; Start Statistics Manager work (FR-07, FR-08, TC-07); author nitish5052).

````markdown
## Owner
C NITISH

## Sprint history
Sprint 2 (26-30 October 2026); preserved from the original backlog.

## Story Points
3

## Description
Define and complete the backend-neutral statistics contract for the existing C++ simulation so REST consumers can report processed vehicles, throughput, queue metrics, and waiting-time information. Preserve the existing Statistics Manager implementation where it already satisfies these requirements; do not duplicate statistics in the frontend.

## SRS Traceability
FR-07, FR-08; TC-07; web API state contract.

## Acceptance Criteria
- [ ] The C++ statistics interface exposes generated and processed vehicle counts, queue counts/metrics, throughput, and waiting-time data needed by clients.
- [ ] Values remain derived from the simulation core and update at the correct logical tick/vehicle-processing events.
- [ ] The interface is usable by the REST API without coupling simulation logic to React or HTTP rendering.
- [ ] Existing C++ unit tests continue to validate the statistics behavior; uncovered contract behavior has regression tests.
- [ ] The API integration can consume this contract for dashboard display.

## Definition of Done
- [ ] Core statistics contract and tests are reviewed against the current implementation.
- [ ] TC-07 evidence and any remaining gaps are recorded.
````

### Issue 8 Implement input validation and safe configuration handling

Source: [actual issue](https://github.com/traffic-management-system-team/Traffic-management-system/issues/8). State: CLOSED; project status: Done. Assignee(s): nitish5052. Project story points: 3. Project iteration: Sprint 2, start 2026-10-26, duration 5 days. Labels: feature, testing. Milestone: none.

Created: 2026-10-12T19:12:25Z; updated: 2026-11-06T10:42:04Z; closed: 2026-11-06T10:42:04Z.

Project linked-PR field: [PR #24](https://github.com/traffic-management-system-team/Traffic-management-system/pull/24), [PR #37](https://github.com/traffic-management-system-team/Traffic-management-system/pull/37). Actual closing/timeline PRs: [PR #37](https://github.com/traffic-management-system-team/Traffic-management-system/pull/37) (closing; MERGED; Implement safe configuration validation and handling; author pranav9360; merged 2026-11-06T10:42:02Z); [PR #21](https://github.com/traffic-management-system-team/Traffic-management-system/pull/21) (cross-reference/connection; CLOSED UNMERGED; Add InputValidator and configuration handling (#8); author nitish5052); [PR #24](https://github.com/traffic-management-system-team/Traffic-management-system/pull/24) (cross-reference/connection; CLOSED UNMERGED; Implement InputValidator and configuration handling (Issue #8); author nitish5052).

````markdown
## Description
Keep configuration validation and configuration-state guarantees in the reusable C++17 core. This issue owns the canonical configuration contract and its regression tests; REST routing and React presentation are handled by #28, #33, and #31 respectively.

Reuse the existing `SimConfig`, `InputValidator`, `ConfigController`, and `SimModel::setConfig` implementation. Do not duplicate validation rules in HTTP handlers or the frontend.

## Scope
- Define configuration defaults, field names, and valid ranges centrally.
- Reject missing, malformed, non-integer, and out-of-range values with field-specific diagnostics.
- Ensure rejected or state-disallowed updates never replace the last accepted model configuration.
- Preserve existing simulation, reset, and pause/resume regression coverage.
- Keep the core validation/test path independent of Qt.

## Acceptance Criteria
- [ ] Configuration defaults and bounds are defined centrally in the C++ core.
- [ ] Missing, malformed, non-integer, and out-of-range values are rejected with field-specific diagnostics.
- [ ] Only a complete validated `SimConfig` can be applied to the model.
- [ ] Invalid or state-disallowed updates leave the last accepted configuration unchanged.
- [ ] State-dependent acceptance is determined by authoritative backend model state, not client-reported state.
- [ ] Regression tests cover boundaries, rejected updates, configuration preservation, stopped/running/paused behavior, reset, and existing pause/resume behavior.
- [ ] Core tests can build and run without requiring Qt.
- [ ] A stable core validation result is available for the REST layer to translate into the documented API error contract; HTTP routes/status codes are covered by #28 and #33.
- [ ] TC-10 evidence is updated to reflect core validation and backend-facing behavior; Qt GUI runtime is not a web-migration acceptance requirement.

## SRS Traceability
FR-10; SEC-01; NFR-03; TC-10.

## Definition of Done
- [ ] Core validation path builds and is covered by regression tests.
- [ ] Existing simulation regression tests remain intact.
- [ ] Validation behavior and configuration bounds are documented.
- [ ] No Qt GUI runtime dependency is introduced for core validation.
````

### Issue 9 Integrate C++ simulation modules with REST API

Source: [actual issue](https://github.com/traffic-management-system-team/Traffic-management-system/issues/9). State: CLOSED; project status: Done. Assignee(s): pranav9360. Project story points: 5. Project iteration: Sprint 2, start 2026-10-26, duration 5 days. Labels: testing, integration. Milestone: none.

Created: 2026-10-12T19:12:27Z; updated: 2026-11-09T16:09:59Z; closed: 2026-11-09T16:09:59Z.

Project linked-PR field: [PR #41](https://github.com/traffic-management-system-team/Traffic-management-system/pull/41). Actual closing/timeline PRs: [PR #41](https://github.com/traffic-management-system-team/Traffic-management-system/pull/41) (closing; MERGED; Issue #9: Integrate Web-Based REST Simulation Backend Closes #9; author pranav9360; merged 2026-11-09T16:05:24Z); [PR #38](https://github.com/traffic-management-system-team/Traffic-management-system/pull/38) (cross-reference/connection; MERGED; Implement SimulationService with synchronized scheduler; author Dinesh07-18; merged 2026-11-07T12:54:04Z); [PR #39](https://github.com/traffic-management-system-team/Traffic-management-system/pull/39) (cross-reference/connection; MERGED; Implement Simulation Control API Handlers (#28); author nitish5052; merged 2026-11-08T11:04:37Z).

````markdown
## Owner
Daruvuri Pranav Sai

## Sprint history
Sprint 2 (26-30 October 2026); preserved from the original backlog.

## Story Points
5

## Description
Integrate the existing C++17 simulation modules with the HTTP/JSON REST API while retaining the current simulation behavior and CMake build. The API must use a single well-defined, synchronized execution model; HTTP handlers must not concurrently mutate or inspect the model unsafely.

## SRS Traceability
FR-01 through FR-12; NFR-04, NFR-05; TC-15.

## Acceptance Criteria
- [ ] Existing Simulation Model, Vehicle Manager, Queue Manager, Traffic Signal Manager, Traffic Manager, Input Validator, SimConfig, and Configuration Controller remain C++ and functional.
- [ ] The API host builds with the existing C++17/CMake project and verified Windows-compatible dependency integration.
- [ ] Model access is serialized through a single simulation execution/ownership boundary; state snapshots do not dequeue or mutate vehicles.
- [ ] Start/pause/resume/reset/configuration operations and simulation ticking preserve current core semantics; invalid or unsafe runtime configuration does not replace accepted configuration.
- [ ] The Qt implementation is retained during migration and is not removed by this integration task.
- [ ] Core tests remain runnable independently of the HTTP host.

## Issue #9 Integration Specification

### HTTP host defaults
- HTTP framework: Crow.
- JSON library: nlohmann-json.
- Bind address: 127.0.0.1 (loopback).
- Port: 18080.
- These are the agreed initial defaults; do not introduce a different host or port as part of the integration without an agreed change.

### Ownership and architecture
- Issue #9 owns the application entry point (`main()`), Crow application creation, the single shared `SimulationService` instance, HTTP server startup/shutdown, CMake integration, route/handler integration, and overall API integration testing.
- Issue #26 owns `SimulationService`, synchronized model access, scheduler, and simulation lifecycle.
- Issue #27 owns the framework-neutral, read-only simulation-state handler and its response data/serialization.
- Issue #28 owns the framework-neutral simulation-control handlers.
- The production application must use exactly one Crow server and one shared `SimulationService`; all endpoint handlers use that same service instance.
- HTTP handlers must not create their own `SimulationService`, access `SimModel` directly, or duplicate scheduler/model logic.
- The core library and its tests must remain buildable/runnable independently of the Crow HTTP host. Crow and nlohmann-json dependencies belong to the HTTP-host target, not the framework-neutral core.

### API endpoints
- `GET /api/simulation/state` — Issue #27.
- `POST /api/simulation/start` — Issue #28.
- `POST /api/simulation/pause` — Issue #28.
- `POST /api/simulation/resume` — Issue #28.
- `POST /api/simulation/reset` — Issue #28.
- `PUT /api/simulation/config` — Issue #28.

### Shared response contract
All six endpoints use the common JSON envelope. Success responses contain `success: true`, endpoint-specific content under `data`, and `error: null`. Error responses contain `success: false`, `data: null`, and an `error` object with `code` and a clear `message`. Configuration validation errors may additionally include field-specific `details`. The state endpoint's existing state fields must be placed under `data`.

### Lifecycle
- Starting the HTTP server must not start, pause, resume, or reset the simulation.
- The service is created once and remains alive while the HTTP server can process requests.
- The simulation initially remains stopped and starts only when a client calls the start endpoint.
- Shut down the HTTP server cleanly before destroying the shared service. HTTP-server shutdown and simulation reset/shutdown are separate concerns.

### Integration boundary
- Issue #9 adapts HTTP requests to the framework-neutral handlers and forwards their responses through Crow.
- For control requests, the adapter parses JSON and preserves JSON scalar types when constructing the Issue #28 request representation; malformed JSON must be reported as invalid JSON.
- The exact route-registration/handler adapter function signature is to be supplied and agreed by Pranav; do not guess or implement an unapproved signature.

## Definition of Done
- [ ] Integrated C++ build and API smoke-test evidence is recorded.
- [ ] TC-15 evidence and any integration defects are linked.
````

### Issue 10 Run C++ regression and web acceptance tests

Source: [actual issue](https://github.com/traffic-management-system-team/Traffic-management-system/issues/10). State: CLOSED; project status: Done. Assignee(s): gowth139am-lab. Project story points: 5. Project iteration: Sprint 2, start 2026-10-26, duration 5 days. Labels: testing. Milestone: none.

Created: 2026-10-12T19:12:31Z; updated: 2026-11-15T11:15:44Z; closed: 2026-11-15T11:15:44Z.

Project linked-PR field: empty. Actual closing/timeline PRs: [PR #42](https://github.com/traffic-management-system-team/Traffic-management-system/pull/42) (cross-reference/connection; MERGED; docs: finalize Issue #12 RTM and web migration readiness; author pranav9360; merged 2026-11-10T04:37:46Z); [PR #51](https://github.com/traffic-management-system-team/Traffic-management-system/pull/51) (cross-reference/connection; MERGED; test(issue-10): complete C++ regression and web acceptance; author gowth139am-lab; merged 2026-11-15T04:09:19Z); [PR #52](https://github.com/traffic-management-system-team/Traffic-management-system/pull/52) (cross-reference/connection; MERGED; docs: align project documentation with completed implementation; author pranav9360; merged 2026-11-15T11:22:52Z).

````markdown
## Owner
GOWTHAM S

## Sprint history
Sprint 2 (26-30 October 2026); preserved from the original backlog.

## Story Points
5

## Description
Execute existing C++ simulation regression tests and verify web acceptance behavior against the real REST backend. Preserve historical GUI test evidence where relevant, but test the replacement web UI for newly delivered presentation and interaction behavior.

## SRS Traceability
FR-01 through FR-12; TC-01 through TC-15; web acceptance and integration criteria.

## Acceptance Criteria
- [ ] All existing C++ test targets are executed and pass/fail results are recorded.
- [ ] Regression coverage verifies deterministic ticks, signal sequence/states, arrivals, queue capacity, generated/processed/waiting statistics, reset, and accepted/invalid configuration behavior.
- [ ] Web tests use actual backend state to verify controls, configuration/validation feedback, signal/queue/statistics display, and API error handling.
- [ ] Pause/resume, reset, and safe state access are tested across the API/UI boundary.
- [ ] Browser viewport and keyboard-accessibility checks are recorded for the responsive web UI.
- [ ] Defects are tracked and blocking failures are clearly identified; do not close or alter historical PRs as part of this test task.

## Definition of Done
- [ ] Test commands, environment, results, and remaining gaps are recorded.
- [ ] Core regression and web acceptance evidence are linked or summarized.
````

### Issue 11 Perform security and reliability validation

Source: [actual issue](https://github.com/traffic-management-system-team/Traffic-management-system/issues/11). State: CLOSED; project status: Done. Assignee(s): Dinesh07-18. Project story points: 3. Project iteration: Sprint 2, start 2026-10-26, duration 5 days. Labels: testing. Milestone: none.

Created: 2026-10-12T19:12:33Z; updated: 2026-11-13T12:09:37Z; closed: 2026-11-13T11:45:57Z.

Project linked-PR field: [PR #25](https://github.com/traffic-management-system-team/Traffic-management-system/pull/25). Actual closing/timeline PRs: [PR #25](https://github.com/traffic-management-system-team/Traffic-management-system/pull/25) (closing; MERGED; Add security and reliability validation - Issue #11; author Dinesh07-18; merged 2026-11-13T11:45:55Z); [PR #22](https://github.com/traffic-management-system-team/Traffic-management-system/pull/22) (cross-reference/connection; CLOSED UNMERGED; Initial security and reliability validation - Issue #11; author Dinesh07-18); [PR #50](https://github.com/traffic-management-system-team/Traffic-management-system/pull/50) (cross-reference/connection; CLOSED UNMERGED; Complete security and reliability evidence for Issue #11; author Dinesh07-18).

````markdown
## Owner
Dinesh

Assignee: intentionally left unset because GitHub usernames have not been verified.

## Sprint
Sprint 2 (26-30 October 2026)

## Story Points
3

## Description
Validate security and reliability behavior covering invalid configuration input, invalid signal transitions, empty-queue safety, and C/C++ memory/buffer safety review.

## SRS Traceability
SEC-01 through SEC-04; NFR-03; TC-04, TC-10, TC-13, TC-16.

## Acceptance Criteria
- [x] Invalid inputs are checked for safe rejection.
- [x] Invalid signal transitions are checked for safe rejection.
- [x] Empty queue operations are checked for safe handling.
- [x] Source is reviewed for unchecked buffers, invalid pointer use, and unsafe memory operations.

## Definition of Done
- [x] TC-13 and TC-16 evidence is recorded, along with relevant TC-04/TC-10 observations.
- [x] Any security or reliability defects are opened as separate issues. (No separate defect issue link or explicit disposition was verified while updating this checklist; keep this item open until that is confirmed.)
- [x] No tested invalid condition causes an application crash without a recorded defect.

## Completion Evidence
- Security/reliability validation report: `docs/Security_Reliability_Validation.md`
- TC-16 source and memory-safety review: `docs/sprint-1/TC-16_Source_Memory_Safety_Review.md`
- Regression tests: `tests/security_reliability_test.cpp`
- Requirement traceability: `docs/sprint-1/RTM.csv`
- Latest reviewed PR head: `e57b212eeb5d49a5c9f8e97fcc16de9132f34abc`
- Latest-head GitHub Actions run #77: all seven jobs passed, including ASan/UBSan and Clang Static Analyzer.
- Independent PR #25 review: approved by `pranav9360` on the reviewed head.

The remaining conditional DoD item is intentionally left unchecked until any applicable defect issues are linked or the absence of actionable defects is explicitly confirmed.
````

### Issue 12 Finalize web migration RTM, evidence, and demonstration

Source: [actual issue](https://github.com/traffic-management-system-team/Traffic-management-system/issues/12). State: CLOSED; project status: Done. Assignee(s): pranav9360. Project story points: 5. Project iteration: Sprint 2, start 2026-10-26, duration 5 days. Labels: documentation, testing. Milestone: none.

Created: 2026-10-12T19:12:36Z; updated: 2026-11-10T04:37:47Z; closed: 2026-11-10T04:37:47Z.

Project linked-PR field: [PR #42](https://github.com/traffic-management-system-team/Traffic-management-system/pull/42). Actual closing/timeline PRs: [PR #42](https://github.com/traffic-management-system-team/Traffic-management-system/pull/42) (closing; MERGED; docs: finalize Issue #12 RTM and web migration readiness; author pranav9360; merged 2026-11-10T04:37:46Z); [PR #52](https://github.com/traffic-management-system-team/Traffic-management-system/pull/52) (cross-reference/connection; MERGED; docs: align project documentation with completed implementation; author pranav9360; merged 2026-11-15T11:22:52Z).

````markdown
## Owner
Daruvuri Pranav Sai

## Sprint history
Sprint 2 (26-30 October 2026); preserved from the original backlog.

## Story Points
5

## Description
Finalize requirement traceability, implementation/test evidence, and demonstration readiness for the approved React/TypeScript/Vite frontend and C++17 HTTP/JSON API migration. Update only documentation that must describe the approved web architecture; retain completed history and identify migration gaps rather than rewriting unrelated project records.

## SRS Traceability
All SRS functional, non-functional, and security requirements; RTM; SAD; Test Plan; README; approved web migration plan.

## Acceptance Criteria
- [ ] Each SRS FR/NFR/SEC requirement has an implementation or test-evidence status that reflects the web architecture.
- [ ] RTM, SAD, Test Plan, README, and migration plan are checked for consistency, with scoped updates proposed/applied only where authorized.
- [ ] Completed Qt and C++ work remains documented as historical evidence; outstanding Qt-specific delivery requirements are traced to web equivalents.
- [ ] API, frontend, integration, responsiveness/accessibility, and C++ regression evidence is linked or summarized.
- [ ] Remaining risks, unverified behaviors, and migration limitations are explicitly identified.
- [ ] A final demonstration checklist covers the real backend-driven dashboard, configuration, controls, signals, queues, and statistics.

## Definition of Done
- [ ] RTM/evidence review and demo checklist are recorded.
- [ ] Documentation conflicts and unresolved coverage gaps are tracked without erasing sprint history.
````

### Issue 26 Add Crow API host and serialized simulation service

Source: [actual issue](https://github.com/traffic-management-system-team/Traffic-management-system/issues/26). State: CLOSED; project status: Done. Assignee(s): Dinesh07-18. Project story points: 5. Project iteration: not assigned. Labels: feature, integration. Milestone: none.

Created: 2026-11-04T15:31:09Z; updated: 2026-11-13T12:13:00Z; closed: 2026-11-07T13:59:05Z.

Project linked-PR field: [PR #38](https://github.com/traffic-management-system-team/Traffic-management-system/pull/38). Actual closing/timeline PRs: [PR #38](https://github.com/traffic-management-system-team/Traffic-management-system/pull/38) (closing; MERGED; Implement SimulationService with synchronized scheduler; author Dinesh07-18; merged 2026-11-07T12:54:04Z).

````markdown
## Description
Add a C++17 Crow REST host and a `SimulationService` that owns and serializes access to the existing C++ simulation model. Verify the pinned Crow/vcpkg dependency with the supported MSVC/CMake toolchain before wiring routes.

The service is the single production tick source and the only path for HTTP commands and state reads. Keep the existing simulation logic; do not duplicate simulation behavior. Keep the core/API build independent of Qt. The existing Qt application may remain as a separate legacy target, but Qt is not required by the web application.

## Scope
- Establish one authoritative `SimModel` instance owned by `SimulationService`.
- Serialize commands, configuration changes, ticks, and snapshots through one synchronization boundary.
- Establish one authoritative backend tick scheduler.
- Define service lifecycle, pause acknowledgement, interval changes, and clean shutdown.
- Verify Crow and dependencies before adopting them permanently.

## Acceptance Criteria
- [x] Crow and pinned transitive dependencies configure and build with the supported C++17/MSVC/CMake toolchain; exact commands and versions are recorded.
- [x] The API target configures/builds without requiring Qt; existing core targets remain available.
- [x] A single worker/tick source owns the model; commands and snapshots are serialized against it.
- [x] HTTP handlers cannot obtain mutable model/manager references or mutate the model outside the service.
- [x] Worker shutdown stops and joins cleanly; pause acknowledgement and interval scheduling behavior are documented and tested.
- [x] Existing `SimModel::tick()` deterministic semantics and simulation timing remain unchanged.
- [x] Service lifecycle and failure behavior are documented and covered by tests.

## Traceability
SAD backend/service architecture; NFR-04, NFR-05.

## Story Points
5

## Sprint
Backlog; future sprint placement and dates are not assigned.
````

### Issue 27 Expose read-only simulation state and vehicle snapshots

Source: [actual issue](https://github.com/traffic-management-system-team/Traffic-management-system/issues/27). State: CLOSED; project status: Done. Assignee(s): Dinesh07-18. Project story points: 5. Project iteration: not assigned. Labels: feature, integration. Milestone: none.

Created: 2026-11-04T15:31:59Z; updated: 2026-11-13T12:10:23Z; closed: 2026-11-10T08:44:36Z.

Project linked-PR field: [PR #40](https://github.com/traffic-management-system-team/Traffic-management-system/pull/40). Actual closing/timeline PRs: [PR #40](https://github.com/traffic-management-system-team/Traffic-management-system/pull/40) (closing; MERGED; Add read-only simulation state API; author Dinesh07-18; merged 2026-11-10T08:44:34Z).

````markdown
## Description
Add an additive, copy-only snapshot API to the existing C++ model/queue boundary and expose the current simulation state through `GET /api/simulation/state`. Return actual authoritative backend state; never inspect a FIFO queue by dequeuing it or expose internal mutable containers.

## Scope
- Simulation running/paused/stopped status and current tick.
- Active signal and all four approach signal states.
- Queue lengths and FIFO-ordered vehicle snapshots.
- Configuration and simulation counters/statistics.

## Acceptance Criteria
- [x] Response includes all four named approaches and their RED/YELLOW/GREEN states.
- [x] Each approach includes its current queue count and FIFO-ordered queued vehicle IDs/types from the actual backend.
- [x] Response includes tick, stopped/running/paused state, accepted configuration, generated and processed counts, total waiting ticks, and a documented average-wait definition.
- [x] Queue counts agree with serialized vehicle arrays; reading a snapshot never drains or reorders a queue.
- [x] Snapshot values are copied as one consistent state through `SimulationService` from #26.
- [x] JSON field names, cardinal ordering, error schema, and endpoint contract are documented and tested.
- [x] Snapshot reads do not mutate simulation state or counters.

## Issue #9 Integration Contract

### Agreed architecture
- Issue #27 provides the read-only state data functionality and its serialization.
- Issue #9 owns the Crow HTTP host, route integration, HTTP response envelope, and server lifecycle.
- Issue #9 supplies the same shared `SimulationService` instance used by all API handlers. Issue #27 must not create or own a service, server, or application entry point.
- State reads must use one synchronized `SimulationSnapshot` obtained through `SimulationService::getSnapshot()`; they must not access `SimModel` or mutable queue containers directly.

### Approved state serializer interface
The agreed proposed interface is:

```cpp
nlohmann::json serializeSimulationStateData(
    const SimulationSnapshot& snapshot
);
```

- This is a Crow-independent serializer: it must not include Crow types or register routes.
- It accepts an already-copied `SimulationSnapshot`; it must not create a `SimulationService` or fetch the snapshot itself.
- It returns only the state **data object**, not the outer HTTP response envelope.
- The serializer uses nlohmann-json. This dependency is permitted for the state serializer/HTTP integration target, but must not be introduced into `tms_core`.
- Issue #9 wraps the returned data in the shared success envelope: `{"success":true,"data":...,"error":null}`.
- Error responses from the state endpoint must use the shared error envelope: `{"success":false,"data":null,"error":{"code":"...","message":"..."}}`.

### Explicitly undecided
The exact Crow route-registration and handler-adapter function signature is not specified by this issue. Pranav will provide and approve that integration signature separately. Do not guess or implement a route-registration signature before that approval.

## Traceability
FR-01 through FR-08, FR-11, FR-12; TC-01 through TC-08, TC-11, TC-12.

## Story Points
5

## Sprint
Backlog; future sprint placement and dates are not assigned.
````

### Issue 28 Add simulation control and configuration REST endpoints

Source: [actual issue](https://github.com/traffic-management-system-team/Traffic-management-system/issues/28). State: CLOSED; project status: Done. Assignee(s): nitish5052. Project story points: 5. Project iteration: not assigned. Labels: feature, testing, integration. Milestone: none.

Created: 2026-11-04T15:32:19Z; updated: 2026-11-08T11:04:38Z; closed: 2026-11-08T11:04:38Z.

Project linked-PR field: [PR #39](https://github.com/traffic-management-system-team/Traffic-management-system/pull/39). Actual closing/timeline PRs: [PR #39](https://github.com/traffic-management-system-team/Traffic-management-system/pull/39) (closing; MERGED; Implement Simulation Control API Handlers (#28); author nitish5052; merged 2026-11-08T11:04:37Z).

````markdown
## Description
Implement REST commands using `SimulationService` from #26. Expose start, pause, resume, reset, and configuration update operations over HTTP/JSON. Configuration must continue through the canonical C++ `InputValidator`, `ConfigController`, and `SimConfig` path from #8.

## Proposed endpoints
- `POST /api/simulation/start`
- `POST /api/simulation/pause`
- `POST /api/simulation/resume`
- `POST /api/simulation/reset`
- `PUT /api/simulation/config`

Coordinate the request/response and error schema with #27 and #33 before finalizing route contracts. Do not introduce a competing `/api/v1` route prefix without team agreement.

## Acceptance Criteria
- [ ] Start, pause, resume, and reset are explicit state transitions; repeated-command behavior is defined and tested.
- [ ] API requests never manually advance logical ticks; only the service's single tick source calls the model tick.
- [ ] Pause acknowledgement prevents later ticks until resume; reset restores the model reset state and retains the accepted configuration.
- [ ] Configuration accepts complete validated settings only when the model permits; invalid or disallowed values do not overwrite the last accepted configuration.
- [ ] Accepted settings affect model behavior and the backend scheduler interval only after successful application.
- [ ] Malformed JSON, missing/wrong-type fields, validation failures, and invalid state transitions return documented JSON errors and appropriate HTTP status codes.
- [ ] State-changing routes use the same synchronization boundary as snapshots and worker ticks.
- [ ] Canonical validation rules are reused from #8; route handlers do not duplicate field bounds or parsing rules.
- [ ] Endpoint behavior is covered by integration tests under #33.

## Traceability
FR-09, FR-10; SEC-01; TC-09, TC-10.

## Story Points
5

## Sprint
Backlog; future sprint placement and dates are not assigned.
````

### Issue 29 Scaffold React TypeScript Vite frontend and API client

Source: [actual issue](https://github.com/traffic-management-system-team/Traffic-management-system/issues/29). State: CLOSED; project status: Done. Assignee(s): pranav9360. Project story points: 3. Project iteration: not assigned. Labels: feature, integration. Milestone: none.

Created: 2026-11-04T15:32:48Z; updated: 2026-11-10T06:41:52Z; closed: 2026-11-10T06:41:52Z.

Project linked-PR field: [PR #43](https://github.com/traffic-management-system-team/Traffic-management-system/pull/43). Actual closing/timeline PRs: [PR #43](https://github.com/traffic-management-system-team/Traffic-management-system/pull/43) (closing; MERGED; feat(frontend): scaffold React app and typed API client; author pranav9360; merged 2026-11-10T06:41:51Z).

````markdown
## Description
Create the web frontend using React, TypeScript, Vite, and Tailwind CSS. Add a typed HTTP/JSON API client and local development proxy for the C++ backend. Do not implement simulation rules in the client.

## Scope
- Frontend project scaffold and dependency lockfile.
- Typed API client for state, configuration, commands, and errors.
- Configurable backend connection and Vite development proxy.
- Basic responsive application layout and centralized API error handling.

## Acceptance Criteria
- [ ] Frontend dependencies are locked and scripts provide install, development, type-check, test, and production-build commands.
- [ ] Vite starts locally and proxies `/api` requests to the configured backend without permissive cross-origin defaults.
- [ ] TypeScript API types match the agreed backend state, command, configuration, and error contracts from #27, #28, and #33.
- [ ] UI state is populated from real API responses; no hard-coded traffic state or duplicate simulation logic is introduced.
- [ ] API client handles loading, network failures, and documented backend errors consistently.
- [ ] Setup and build instructions are documented.

## Story Points
3

## Sprint
Backlog; future sprint placement and dates are not assigned.
````

### Issue 30 Build responsive intersection, signal, and vehicle queue dashboard

Source: [actual issue](https://github.com/traffic-management-system-team/Traffic-management-system/issues/30). State: CLOSED; project status: Done. Assignee(s): gowth139am-lab. Project story points: 5. Project iteration: not assigned. Labels: accessibility, feature. Milestone: none.

Created: 2026-11-04T15:33:17Z; updated: 2026-11-10T15:25:56Z; closed: 2026-11-10T15:25:56Z.

Project linked-PR field: [PR #45](https://github.com/traffic-management-system-team/Traffic-management-system/pull/45). Actual closing/timeline PRs: [PR #45](https://github.com/traffic-management-system-team/Traffic-management-system/pull/45) (closing; MERGED; feat(frontend): implement Issue 30 intersection dashboard; author gowth139am-lab; merged 2026-11-10T15:25:55Z); [PR #43](https://github.com/traffic-management-system-team/Traffic-management-system/pull/43) (cross-reference/connection; MERGED; feat(frontend): scaffold React app and typed API client; author pranav9360; merged 2026-11-10T06:41:51Z).

````markdown
## Description
Build the primary dashboard showing all four approaches and actual backend signal and queue data. Preserve the existing intersection/signal/queue functionality while delivering it in the web client.

## Acceptance Criteria
- [ ] North, South, East, and West are distinctly labeled and rendered from backend snapshots.
- [ ] RED, YELLOW, and GREEN states use clear text and accessible visual indicators; color is not the only signal.
- [ ] Each approach displays actual FIFO vehicle IDs/types and queue count/capacity from the backend.
- [ ] Vehicle arrivals and processing are visualized only when reflected in successive backend responses; the browser does not generate or remove vehicles itself.
- [ ] Layout is usable at mobile, tablet, and desktop viewports and supports keyboard navigation.

## Traceability
FR-02, FR-03, FR-04, FR-06, FR-08, FR-11, FR-12; TC-02 through TC-06, TC-08, TC-11, TC-12.

## Story Points
5

## Sprint
Backlog; future sprint placement and dates are not assigned.

## Labels
feature, accessibility
````

### Issue 31 Add web simulation controls and configuration form

Source: [actual issue](https://github.com/traffic-management-system-team/Traffic-management-system/issues/31). State: CLOSED; project status: Done. Assignee(s): pranav9360. Project story points: 5. Project iteration: not assigned. Labels: accessibility, feature. Milestone: none.

Created: 2026-11-04T15:33:35Z; updated: 2026-11-10T09:14:24Z; closed: 2026-11-10T09:14:24Z.

Project linked-PR field: [PR #44](https://github.com/traffic-management-system-team/Traffic-management-system/pull/44). Actual closing/timeline PRs: [PR #44](https://github.com/traffic-management-system-team/Traffic-management-system/pull/44) (closing; MERGED; feat(frontend): add simulation controls and configuration; author pranav9360; merged 2026-11-10T09:14:22Z).

````markdown
## Description
Provide web controls for Start, Pause, Resume, and Reset plus configuration fields for tick interval, GREEN/YELLOW durations, arrival interval, and maximum queue capacity. The frontend must use the API client from #29 and endpoints/contracts delivered by #28.

## Scope
- Simulation control buttons.
- Configuration form and draft-state management.
- Backend validation and state-error display.
- Accepted configuration and authoritative simulation-state display.
- Accessible loading, disabled, and error states.

## Acceptance Criteria
- [ ] Controls call the matching REST endpoints and reflect authoritative backend state.
- [ ] Configuration submits the complete settings object through the API client.
- [ ] Backend validation feedback is displayed, including field-level errors where provided.
- [ ] Rejected values do not replace the last accepted configuration shown to the user.
- [ ] Draft form values are distinguished from the last accepted server configuration.
- [ ] Configuration lock/state rules and errors are clear; controls do not simulate ticks locally.
- [ ] Controls and form are responsive, keyboard accessible, and have appropriate disabled/loading states.
- [ ] Client-side validation is used only for user guidance; backend validation remains authoritative.

## Traceability
FR-09, FR-10; SEC-01; TC-09, TC-10.

## Story Points
5

## Sprint
Backlog; future sprint placement and dates are not assigned.
````

### Issue 32 Display live simulation statistics in the dashboard

Source: [actual issue](https://github.com/traffic-management-system-team/Traffic-management-system/issues/32). State: CLOSED; project status: Done. Assignee(s): nitish5052. Project story points: 3. Project iteration: not assigned. Labels: feature. Milestone: none.

Created: 2026-11-04T15:33:53Z; updated: 2026-11-12T15:16:46Z; closed: 2026-11-12T15:16:46Z.

Project linked-PR field: [PR #47](https://github.com/traffic-management-system-team/Traffic-management-system/pull/47). Actual closing/timeline PRs: [PR #47](https://github.com/traffic-management-system-team/Traffic-management-system/pull/47) (closing; MERGED; feat(dashboard): display live simulation statistics; author nitish5052; merged 2026-11-12T15:16:44Z); [PR #43](https://github.com/traffic-management-system-team/Traffic-management-system/pull/43) (cross-reference/connection; MERGED; feat(frontend): scaffold React app and typed API client; author pranav9360; merged 2026-11-10T06:41:51Z).

````markdown
## Description
Display statistics supplied by the C++ API, including generated/processed counts, waiting-time metrics, and current queue metrics. The browser must not become a second statistics or simulation engine.

## Acceptance Criteria
- [ ] Generated, processed, and waiting statistics are rendered from API snapshots.
- [ ] Average waiting-time definition and zero-processed behavior match the backend contract.
- [ ] Statistics refresh after backend state changes and reset accurately on backend reset.
- [ ] UI does not fabricate or independently increment statistics.
- [ ] Values remain understandable and legible at responsive viewport sizes.

## Traceability
FR-07, FR-08; TC-07, TC-08.

## Story Points
3

## Sprint
Backlog; future sprint placement and dates are not assigned.

## Labels
feature
````

### Issue 33 Test REST API contracts, validation, and concurrency

Source: [actual issue](https://github.com/traffic-management-system-team/Traffic-management-system/issues/33). State: CLOSED; project status: Done. Assignee(s): Dinesh07-18. Project story points: 5. Project iteration: not assigned. Labels: testing, integration. Milestone: none.

Created: 2026-11-04T15:34:28Z; updated: 2026-11-11T16:07:54Z; closed: 2026-11-11T16:07:54Z.

Project linked-PR field: [PR #46](https://github.com/traffic-management-system-team/Traffic-management-system/pull/46). Actual closing/timeline PRs: [PR #46](https://github.com/traffic-management-system-team/Traffic-management-system/pull/46) (closing; MERGED; test(api): add focused REST contract coverage for #33; author Dinesh07-18; merged 2026-11-11T14:52:11Z).

````markdown
## Description

Complete the Issue #33 backend REST/service contract coverage, test traceability and module-level verification. Tests must exercise the actual backend service/API, not only mocked route handlers. The existing browser dashboard is also documented and its component/hook coverage is included in the validation workflow.

## Scope

- REST request/response and error contracts.
- Configuration validation, accepted-value preservation and state locks.
- Consistent state snapshots, FIFO vehicle data, deterministic signal/tick behavior and statistics.
- Concurrent commands, reads, ticks, pause/reset acknowledgement and worker lifecycle.
- TC-01–TC-16 traceability and TC-15 maintainability review.
- Frontend dashboard component/hook regression tests.
- Cross-platform backend build/test, static analysis, and sanitizer validation via GitHub Actions.

## Acceptance Criteria

- [x] Tests cover `GET /api/simulation/state`, start, pause, resume, reset, and configuration update, including success/error status and schema.
- [x] Tests verify actual FIFO vehicle IDs/counts and consistent signal, statistics and configuration snapshots.
- [x] Tests verify malformed JSON, missing/wrong-type fields, out-of-range configuration, accepted-config preservation and configuration lock rules.
- [x] Deterministic service/model tests avoid wall-clock sleeps for logical-tick assertions.
- [x] Concurrency tests exercise overlapping reads, commands and ticks; pause/reset acknowledgements and worker shutdown behavior are covered.
- [x] Test server binds to loopback/ephemeral ports.
- [x] All **11 core CTest targets** remain registered. With `BUILD_API_SERVER=ON`, the **3 API-enabled targets** are added for **14 total**; with `BUILD_API_SERVER=OFF`, the core suite contains 11 tests.
- [x] API tests use the agreed contract from #27/#28 and do not duplicate the core validator's unit-test responsibility from #8.
- [x] Statistics aggregation is separated behind `SimulationStatisticsManager` and independently unit-tested; the simulation model delegates aggregation to it.
- [x] TC-01 through TC-16 remain traceable in the existing Sprint 1 RTM, with test target, function/case, expected result, execution result and evidence. Uncovered scopes remain explicitly described.
- [x] TC-15 documents module/interface boundaries, independent targets and the maintainability review.
- [x] Frontend component/hook tests verify the implemented four-approach dashboard, signal/queue/statistics display, refresh polling, keyboard form submission and responsive grid behavior.
- [x] GitHub Actions verifies backend Debug/Release on Windows and Ubuntu, frontend typecheck/tests/build, Clang static analysis, and Clang ASan/UBSan for core tests. All seven jobs passed in PR run #38 on implementation commit ce670996581b752816d3043d60ee1e18bcc62d48; the three subsequent commits are documentation-only. Run #44 repeats checks on the current documentation head and is running; six jobs have passed and the final Ubuntu Debug CTest step remains in progress.

## Implemented frontend scope (not a missing feature)

The repository already has the responsive four-approach intersection dashboard, signal indicators, FIFO queue cards, aggregate generated/processed/waiting/average-wait statistics, lifecycle controls, configuration form and adaptive REST polling. Component/hook tests are in `frontend/src/components/IntersectionDashboard.test.tsx`, `frontend/src/App.test.tsx` and `frontend/src/hooks/useBackendStatus.test.ts`. These are Vitest/jsdom tests, not a full cross-browser Playwright suite or manual screen-reader audit.

## Revision-bound verification

The earlier local C++ results (Debug 13/13, Release 13/13, core-only Debug 10/10) apply only to source revision 162336e206390e74ac04be5bc2fe66ee17f3a1c2; they predate the statistics-manager refactor.

**Latest fully tested implementation/CMake commit:** ce670996581b752816d3043d60ee1e18bcc62d48. GitHub Actions PR run [#38](https://github.com/traffic-management-system-team/Traffic-management-system/actions/runs/42949329618) passed all seven jobs:
- Windows Debug and Release: build plus CTest 14/14 each.
- Ubuntu Debug and Release: build plus CTest 14/14 each.
- Frontend: type-check passed, 36/36 Vitest tests across four files, production build passed.
- Core-only Clang ASan/UBSan: CTest 11/11 passed.
- Clang static-analysis job completed successfully with two enum-cast diagnostics in deliberate invalid-enum negative tests (tests/test_traffic.cpp:112 and tests/simulation_statistics_manager_test.cpp:72); no warning-free claim is made.

Current branch head: b9e558d2101f32c1a5e540b9ffdabe5a513c2920. The three commits after ce67099 are documentation-only changes to the RTM and TC-15 review; they do not change executable source, tests, CMake or frontend files. [Run #44](https://github.com/traffic-management-system-team/Traffic-management-system/actions/runs/42950065245) is the repeat workflow for this documentation-only head and is queued.

A full local backend matrix has not been rerun on the latest refactor in the user's local Windows environment. The prior local full-suite results are revision-bound to 162336e; the complete current implementation matrix passed in GitHub Actions. The statistics-manager module was also separately compiled/run in a local Linux harness with GCC 14.2 warnings-as-errors and Clang 17 ASan/UBSan.

## Explicit limits

- No quantified real-browser latency/frame-rate benchmark is claimed for NFR-02.
- Full cross-browser Playwright E2E and a manual screen-reader/accessibility audit are not included.
- Sanitizer/static-analysis results are not claimed until their workflow jobs pass.
- Cross-platform verification is not claimed until both Windows and Ubuntu jobs pass.

## Traceability

FR-01 through FR-12; NFR-01 through NFR-08; SEC-01 through SEC-04; TC-01 through TC-16. Detailed case mapping: `docs/sprint-1/RTM_TC_Mapping.csv`. TC-15 review: `docs/sprint-1/TC-15_Maintainability_Review.md`.

## Story Points

5

## Sprint

Backlog; future sprint placement and dates are not assigned.
````

### Issue 34 Test browser integration, responsiveness, and accessibility

Source: [actual issue](https://github.com/traffic-management-system-team/Traffic-management-system/issues/34). State: CLOSED; project status: Done. Assignee(s): gowth139am-lab. Project story points: 5. Project iteration: not assigned. Labels: accessibility, testing, integration. Milestone: none.

Created: 2026-11-04T15:36:41Z; updated: 2026-11-13T18:47:31Z; closed: 2026-11-13T18:47:31Z.

Project linked-PR field: [PR #48](https://github.com/traffic-management-system-team/Traffic-management-system/pull/48). Actual closing/timeline PRs: [PR #48](https://github.com/traffic-management-system-team/Traffic-management-system/pull/48) (closing; MERGED; test: complete Issue 34 browser and accessibility coverage; author gowth139am-lab; merged 2026-11-13T18:47:30Z).

````markdown
## Description
Add frontend component and browser-level integration tests that exercise the web application against the real C++ REST API, including responsive and accessible behavior.

## Acceptance Criteria
- [ ] Component tests cover state rendering, controls, configuration, validation feedback, and API error/loading states.
- [ ] Browser integration runs against the real API and verifies actual vehicle IDs/counts, signal changes, statistics, configuration, pause/resume, and reset.
- [ ] Responsive behavior is checked at mobile, tablet, and desktop viewport sizes.
- [ ] Keyboard and accessible non-color signal-state behavior are checked.
- [ ] Frontend tests/build are documented and run in the project validation workflow; mocked API tests do not replace end-to-end coverage.

## Traceability
FR-04, FR-07 through FR-10, FR-12; NFR-01, NFR-02, NFR-08; TC-07 through TC-10, TC-12, TC-14.

## Story Points
5

## Sprint
Backlog; future sprint placement and dates are not assigned.

## Labels
testing, accessibility, integration
````

### Issue 35 Review REST API security and operational error handling

Source: [actual issue](https://github.com/traffic-management-system-team/Traffic-management-system/issues/35). State: CLOSED; project status: Done. Assignee(s): Dinesh07-18. Project story points: 3. Project iteration: not assigned. Labels: testing, integration. Milestone: none.

Created: 2026-11-04T15:38:17Z; updated: 2026-11-13T12:10:52Z; closed: 2026-11-13T10:22:01Z.

Project linked-PR field: [PR #49](https://github.com/traffic-management-system-team/Traffic-management-system/pull/49). Actual closing/timeline PRs: [PR #49](https://github.com/traffic-management-system-team/Traffic-management-system/pull/49) (closing; MERGED; Harden REST API error handling and security boundaries; author Dinesh07-18; merged 2026-11-13T10:22:00Z).

````markdown
## Description
Review and test security and reliability boundaries introduced by the local HTTP API, without adding unnecessary authentication or persistence requirements.

## Acceptance Criteria
- [x] The API binds to loopback by default; broader exposure is explicitly configured and documented.
- [x] Malformed, out-of-range, and oversized requests fail safely without corrupting model state or replacing accepted configuration.
- [x] Client errors and internal failures return stable JSON error responses without leaking sensitive implementation details.
- [x] No credentials or secrets are required, committed, or written to logs.
- [x] Synchronization, shutdown, and error paths are reviewed and evidence/findings recorded; defects are linked rather than silently waived.

## SRS Traceability
SEC-01 through SEC-04; NFR-03, NFR-05.

## Story Points
3

## Sprint
Backlog; future sprint placement and dates are not assigned.

## Labels
testing, integration
````

## Appendix B Verified original framework decision

Issue #1's actual comments are important because its body says the decision must be visible. The 23 October comment selects Qt 6 Widgets/C++17, CMake, one controlled GREEN approach, North → South → East → West, 1-second ticks, 10 GREEN/2 YELLOW ticks and one North-first rotating arrival per three ticks. It explicitly leaves exact within-tick ordering and processing capacity for integration agreement. The 26 October implementation comment records 7/7 core Debug/Release results and says Qt was unavailable, so desktop build/launch was not performed.

- [2026-10-23T14:55:20Z, pranav9360](https://github.com/traffic-management-system-team/Traffic-management-system/issues/1#issuecomment-6367786254).
- [2026-10-26T18:27:22Z, pranav9360](https://github.com/traffic-management-system-team/Traffic-management-system/issues/1#issuecomment-6424108237).

PR #19's live title/body are abbreviated and its issue has no comments or returned PR association. Its changed-file list supplies historical GUI/model evidence. PR #20's actual description identifies the transition from Win32-only to Qt and explicitly retains the Qt build/launch limitation. The web migration and current browser validation do not remove that historical evidence boundary.

## Final mismatches and unresolved evidence

**M1 — #6 original desktop runtime completion is NOT VERIFIED.** The web replacement implements the requested behavior and is described accurately, but the original “initial GUI builds and runs” DoD cannot be established from available run evidence. PR #20 explicitly says Qt could not be configured/launched. Retain that historical limitation; do not convert browser success into a desktop execution claim.

**M2 — #11 contradicts its own defect checklist.** The defect DoD is checked while its parenthetical says to keep it open; the final paragraph calls it intentionally unchecked. The repository's TC-16 review explicitly provides a no-reproducible-defect disposition. The code/review evidence supports bounded acceptance, but the issue text does not accurately reflect that disposition. This is not evidence that an actual confirmed defect was waived.

**M3 — #26 dependency wording and closure-time evidence differ from delivery.** Its description refers to Crow/vcpkg; the actual pinned dependency workflow is CMake FetchContent. Its closing PR #38 explicitly did not verify MSVC v143 and excluded REST routes, while #26 closed on 7 November; host integration #41 merged 9 November and later MSVC verification exists. Final SAD/REST documentation describes the current implementation correctly. Current integrated acceptance can be supported; full MSVC/Crow host acceptance at #26 closure cannot.

**M4 — #33 checked test-count AC is stale.** Its checked acceptance condition says 11 core plus 3 API = 14, and its recorded frontend baseline is 36. Current CMake has 12 core plus 3 API = 15; current recorded frontend is 33 after later changes. Earlier 14/36 results are valid only at their stated historical source. Final Plan uses 15/33/3 correctly. A larger backend suite is not a regression; the issue needs a revision-qualified historical label rather than being read as current truth. Its old “running/queued” CI passages likewise are not current status.

**M5 — #33 deterministic-test wording needs scope qualification.** Direct logical-model tests (including `testDeterministicLogicalTicksAndFifoVehicleIds` and `testDeterministicArrivalsSignalTransitionsAndSignalGatedMovement`) advance ticks without sleeping. Service tests such as `testInitialStateAndTransitions` and `testSnapshotActiveApproach` use real scheduler waits/`sleep_for`, including logical tick observations. The blanket claim about service/model logical-tick assertions avoiding wall-clock sleeps is too broad. Scheduler/concurrency integration legitimately requires timing; distinguish it from deterministic model testing.

**M6 — historical assignment prose is stale.** Several Sprint 1/2 issue bodies still say assignees are intentionally unset, although the live issue and project records contain real assignees. The audit table uses live assignments. #7/#8 ownership versus integration authorship is also explicitly distinguished. This administrative inconsistency does not invalidate otherwise evidenced functionality.

**M7 — final-document source basis is local, not a newly merged submission baseline.** FINAL_REVIEW describes HEAD plus existing local redesign/reconciliation. Eight passing #53 CI checks validate its committed PR head, not the present uncommitted additions. Fresh native rebuilding, new sanitizer execution, quantified NFR-02 assurance and comprehensive SEC-04 assurance are not established by this audit. The final documents state those limitations accurately.

**M8 — university history/sign-off and videos remain NOT VERIFIED.** Later #26–35 have no assigned iteration; no approved Sprint 3 is established. Formal instructor approval of the web migration and sprint video submission/durations are not proven by issue closure or screenshots. #12's explicit AC asks for a demonstration checklist, which exists; do not falsely mark that issue's checklist AC failed merely because a separate course recording requirement remains outstanding.

There is **no confirmed current application-versus-FINAL_REVIEW architectural or functional MISMATCH** in the inspected scope. There are four PARTIAL MATCH issues (#6, #11, #26, #33), the cross-cutting record/evidence qualifications above, and no basis for claiming all university compliance obligations complete. No existing source, test, document, issue or PR was corrected by this audit.
