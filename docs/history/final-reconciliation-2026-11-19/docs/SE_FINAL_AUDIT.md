# Software Engineering Final Audit

## A. EXECUTIVE SUMMARY

Audit date 19 November 2026 (Asia/Calcutta). Demonstration date supplied by student 20 November 2026.

Actual project is Traffic Management System (Simulation). Branch is feature/professional-traffic-ui at commit 5cbdc012591221ccf300a9656b55bb1c5c38a2d2. Seven preexisting modified frontend files and untracked verification report, coverage and redesign screenshots were present. This audit examines that working tree, not a clean committed redesign. Only this report was authored during the audit. Existing tests generate ignored build/test outputs. The separate Issue #27 integration worktree was not inspected.

**Verdict INSUFFICIENT EVIDENCE for complete university compliance.** The application passes the executed functional checks. Missing outcome-video evidence, outdated final SRS/SAD/Test Plan and unmeasured C++ branch coverage prevent a complete-deliverables claim. This verdict does not identify a confirmed normal-flow application failure.

Strengths include a modular C++17 core, synchronized service, real REST/browser integration, negative and boundary tests, measured frontend coverage, four substantive reports, assigned GitHub stories and individual PR evidence. Main evaluation risks are document alignment and submission evidence.

The university criteria in the supplied brief are the audit authority. The original official rubric itself was not supplied separately. No arbitrary coverage threshold, production deployment, authentication, extra sprint or WCAG certification is treated as mandatory.

## B. MANDATORY COMPLIANCE MATRIX

| Requirement | Status | Evidence | Issue | Required action |
|---|---|---|---|---|
| Updated IEEE-style SRS | PARTIAL | docs/SRS/SRS_Traffic_Management_System.docx Â§Â§1â€“3, Appendix B | Recognizable IEEE-style sections and 24 IDs, but dated 6 Oct v1.3 and describes local desktop communication | Update final platform, interfaces, movement representation and references |
| Updated SAD | PARTIAL | docs/SAD/SAD_Traffic_Management_System.docx Â§Â§3.3â€“3.6, 4.4; docs/Architecture.md | Historical desktop design omits HTTP host, service scheduler, serializer, React and two pages | Reconcile final SAD and diagrams with current architecture |
| Updated Test Plan | PARTIAL | docs/Test_Plan/Test_Plan_Traffic_Management_System.docx Â§Â§6â€“7 | Desktop environment, October schedule and draft Part-1 status | Record final web environment, actual schedule and execution scope |
| Updated Test Case Document | PARTIAL | docs/Test_Plan/Test_Case_Document_Part3.md | TC-01â€“16 and ST-01â€“08 present, baseline 42a57c4 predates current UI | Refresh revision-scoped evidence |
| Unit Test Report | PARTIAL | docs/Unit_Test_Report_Part3.md | Substantive but 38 tests at old source, now 33 | Add current revision and results |
| System Validation Report | PARTIAL | docs/System_Validation_Report_Part3.md | Substantive historical REST/browser evidence, lacks final two-page revision | Add current browser run and scope |
| Code Coverage Report | PARTIAL | docs/Code_Coverage_Report_Part3.md | Historical metrics and absent raw frontend package | Update current frontend measurements and preserve reproduction details |
| Branch Coverage Report | PARTIAL | docs/Branch_Coverage_Report_Part3.md | Frontend historical measurement; C++ explicitly NOT RUN | Measure C++ branches or disclose outstanding mandatory evidence |
| Four final reports under docs | PASS | Four paths above | All readable and nonempty | Keep their limitations visible |
| Assigned backlog with points | PASS | GitHub organization project 1, 22 items | All Done, assigned and pointed, later work has no sprint | Reconcile sprint placement and stale checklists |
| Individual PR/commit history | PASS | PRs #15, #17, #19, #25, #39, #45, #47, #51, #53 | Contributions exist, ownership differs from some integrating authors | Explain contribution versus integration |
| Sprint outcome videos | NOT VERIFIED | No local video files or links found in docs/README | Actual submissions and durations inaccessible | Supply accessible Sprint 1/2 videos and confirm required 1/2 minute formats |
| Design deviations documented | PARTIAL | Design_README.md; docs/Architecture.md | Web migration disclosed, final formal documents remain old | Record final revision and rationale |
| Working application | PASS | Fresh TypeScript/build/Vitest/CTest/Playwright runs in D | Limited to executed environment/scenarios | Rehearse same startup on demo machine |
| Explain implementation/SDLC | PARTIAL | Source, tests, project and K | Oral understanding cannot be established by repository | Team rehearse project-specific answers |

## C. REQUIREMENTS TRACEABILITY MATRIX

The actual SRS DOCX text was extracted directly from its Word XML. It contains purpose, scope, actors, environment, interfaces, constraints, revision history, FR-01â€“12, NFR-01â€“08 and SEC-01â€“04. PDF visual formatting and institutional template conformance were not independently certified. Appendix B uses obsolete names such as MainWindow.cpp, Statistics.cpp and SimulationController.cpp.

PASS below means implemented and supported for the inspected/tested scope, not exhaustive correctness. Current executable evidence is in D. Existing CSV evidence is historical and is not silently treated as current.

| ID | Requirement | Implementation file/function | Test case / file | Result | Remarks |
|---|---|---|---|---|---|
| FR-01 | The system shall create vehicles according to the configured arrival mechanism. | src/simulation_model.cpp; src/vehicle_manager.cpp | TC-01: tests/vehicle_manager_test.cpp::main; tests/simulation_service_test.cpp::testSnapshotAndFifo | PASS | Current unit/integration suites pass for tested scope. |
| FR-02 | The system shall assign each generated vehicle to the appropriate road/lane queue. | src/simulation_model.cpp; src/queue_manager.cpp | TC-02: tests/queue_manager_test.cpp::main; tests/simulation_service_test.cpp::testSnapshotAndFifo | PASS | Current unit/integration suites pass for tested scope. |
| FR-03 | The system shall maintain each road/lane queue using FIFO ordering. | src/queue_manager.cpp | TC-03: tests/queue_manager_test.cpp::main; tests/test_traffic_manager.cpp::testFifoOrder | PASS | Current unit/integration suites pass for tested scope. |
| FR-04 | The system shall maintain and display the traffic signal state for each controlled direction. | src/TrafficSignal.cpp; src/simulation_model.cpp; src/simulation_state_serializer.cpp | TC-04: tests/test_traffic.cpp::testIndependentSignal; testSharedCoordinator; tests/simulation_state_serializer_test.cpp::testStateAndApproachSerialization | PASS | Current unit/integration suites pass for tested scope. |
| FR-05 | The system shall allow vehicles to leave a queue only when movement is permitted by the active signal state. | src/TrafficManager.cpp | TC-05: tests/test_traffic_manager.cpp::testRedSignalKeepsVehicleQueued; testYellowSignalKeepsVehicleQueued; testGreenSignalProcessesOneVehicle | PASS | Current unit/integration suites pass for tested scope. |
| FR-06 | The system shall update queue lengths when vehicles enter or leave queues. | src/queue_manager.cpp | TC-06: tests/queue_length_test.cpp::main; tests/queue_manager_test.cpp::main | PASS | Current unit/integration suites pass for tested scope. |
| FR-07 | The system shall calculate traffic statistics including processed vehicle count and waiting-time information. | src/simulation_model.cpp; include/simulation_statistics.h; src/simulation_state_serializer.cpp | TC-07: tests/simulation_service_test.cpp::testSnapshotAndFifo; tests/simulation_state_serializer_test.cpp::testConfigurationAndStatisticsSerialization | PASS | Current unit/integration suites pass for tested scope. |
| FR-08 | The system shall display the simulation state and statistics through the GUI. | frontend/src/App.tsx; frontend/src/components/IntersectionDashboard.tsx; frontend/src/styles/index.css | TC-08: frontend/src/components/IntersectionDashboard.test.tsx; frontend/src/App.test.tsx | PASS | Current unit/integration suites pass for tested scope. |
| FR-09 | The system shall allow the user to start, pause/resume and reset the simulation. | src/simulation_service.cpp; src/simulation_control_api.cpp; src/simulation_http_server.cpp | TC-09: tests/simulation_service_test.cpp::testInitialStateAndTransitions; tests/simulation_control_api_test.cpp; tests/simulation_http_server_test.cpp | PASS | Current unit/integration suites pass for tested scope. |
| FR-10 | The system shall reject invalid simulation configuration values and prevent unsafe queue operations. | src/input_validator.cpp; src/config_controller.cpp; src/simulation_control_api.cpp; src/queue_manager.cpp | TC-10: tests/input_validator_test.cpp; tests/simulation_control_api_test.cpp; tests/queue_empty_safety_test.cpp; tests/simulation_http_server_test.cpp | PASS | Current unit/integration suites pass for tested scope. |
| FR-11 | The system shall keep vehicles that cannot currently move in their respective queues. | src/TrafficManager.cpp | TC-11: tests/test_traffic_manager.cpp::testRedSignalKeepsVehicleQueued; testYellowSignalKeepsVehicleQueued | PASS | Current unit/integration suites pass for tested scope. |
| FR-12 | The system shall update the GUI after relevant simulation-state changes. | frontend/src/hooks/useBackendStatus.ts; frontend/src/App.tsx; frontend/src/components/IntersectionDashboard.tsx | TC-12: frontend/src/hooks/useBackendStatus.test.ts; frontend/src/App.test.tsx; frontend/src/components/IntersectionDashboard.test.tsx | PASS | Current unit/integration suites pass for tested scope. |
| NFR-01 | The main simulation state and controls shall be visible without navigating through multiple screens. | Browser frontend source is absent | TC-08: No browser layout/navigation test source | PASS | Current unit/integration suites pass for tested scope. |
| NFR-02 | Normal simulation updates shall complete without noticeable GUI freezing on the target development system. | frontend/src/hooks/useBackendStatus.ts; frontend/src/components/IntersectionDashboard.tsx; frontend/src/styles/index.css | TC-14: frontend/src/hooks/useBackendStatus.test.ts; frontend/src/components/IntersectionDashboard.test.tsx | PARTIAL | No quantified target-device latency or frame benchmark. |
| NFR-03 | The application shall handle empty queues and invalid inputs without crashing. | src/queue_manager.cpp safely handles empty/missing queues; input/API paths reject invalid values | TC-13: tests/queue_empty_safety_test.cpp::main; tests/input_validator_test.cpp; tests/simulation_control_api_test.cpp | PASS | Current unit/integration suites pass for tested scope. |
| NFR-04 | Traffic signal, queue, vehicle and statistics logic shall be separated into maintainable modules. | include/simulation_statistics_manager.h; src/simulation_statistics_manager.cpp; src/simulation_model.cpp | TC-15: tests/simulation_statistics_manager_test.cpp; docs/sprint-1/TC-15_Maintainability_Review.md | PASS | Current unit/integration suites pass for tested scope. |
| NFR-05 | Core simulation operations shall be implementable and verifiable as independent functions/modules. | CMakeLists.txt; tms_core production target; separate API/serializer targets | TC-15: Independent core test targets including simulation_statistics_manager_test; API contract tests use the public service/server interface | PASS | Current unit/integration suites pass for tested scope. |
| NFR-06 | The source code shall avoid unnecessary platform-specific dependencies where practical. | CMakeLists.txt; .github/workflows/issue-33-validation.yml | Review: GitHub Actions Windows/Ubuntu Debug and Release matrix | PASS | Prior PR #53 CI verifies Windows/Linux Debug/Release; fresh local run is Windows only. |
| NFR-07 | Queue operations shall preserve FIFO ordering and signal rules shall be consistently enforced. | src/queue_manager.cpp; src/TrafficSignal.cpp; src/TrafficManager.cpp | TC-03, TC-05: tests/queue_manager_test.cpp::main; tests/test_traffic_manager.cpp::testFifoOrder and signal tests; tests/test_traffic.cpp | PASS | Current unit/integration suites pass for tested scope. |
| NFR-08 | GUI controls shall remain usable during normal simulation execution. | frontend/src/App.tsx; frontend/src/components/IntersectionDashboard.tsx; frontend/src/components/ConfigurationForm.tsx; frontend/src/styles/index.css | TC-14: frontend/src/App.test.tsx; frontend/src/components/IntersectionDashboard.test.tsx; frontend/src/hooks/useBackendStatus.test.ts | PASS | Current unit/integration suites pass for tested scope. |
| SEC-01 | The system shall validate all user-entered simulation configuration values before passing them to the simulation modules. | src/input_validator.cpp; src/config_controller.cpp; API validation before configuration applies | TC-10: tests/input_validator_test.cpp; tests/simulation_control_api_test.cpp; tests/simulation_http_server_test.cpp | PASS | Current unit/integration suites pass for tested scope. |
| SEC-02 | The system shall prevent dequeue, removal or access operations on an empty queue. | src/queue_manager.cpp checks missing/empty queues before dequeue access | TC-13: tests/queue_empty_safety_test.cpp::main; tests/queue_manager_test.cpp::main | PASS | Current unit/integration suites pass for tested scope. |
| SEC-03 | The system shall reject invalid traffic-signal state transitions. | src/TrafficSignal.cpp permits only RED->GREEN, GREEN->YELLOW, YELLOW->RED; coordinator requires other approaches RED before GREEN | TC-04: tests/test_traffic.cpp::testIndependentSignal checks RED->YELLOW rejection; testSharedCoordinator checks representative conflicting GREEN attempts | PASS | Current unit/integration suites pass for tested scope. |
| SEC-04 | The implementation shall avoid unsafe memory access and unchecked buffer operations. | All 18 active include headers and 18 src implementation files listed in docs/sprint-1/TC-16_Source_Memory_Safety_Review.md | TC-16: TC-16 completed source review; Clang ASan/UBSan and Clang Static Analyzer | PARTIAL | Prior sanitizer/static review exists; no exhaustive memory-safety proof. |


SRS interface Â§3.1.1 says movement shall be observable. Queue arrivals/removals are observable, but continuous movement coordinates are absent. Classify that wording PARTIAL until the SRS defines the delivered state-based representation. SRS Â§3.1.3 says local desktop with no network communication; final UI uses localhost HTTP. C++ owns the simulation but the React presentation uses TypeScript. These are documented Agile changes, not evidence that the historical SRS has been updated. NFR-01 retains controls/state on both pages, though detailed statistics and queues occupy separate pages.

### Executed test-case mapping

| TC | Requirement | Scenario and expected result | Test file | Actual result/status |
|---|---|---|---|---|
| TC-01 | FR-01 | Configured arrivals create vehicles | tests/vehicle_manager_test.cpp; simulation_api_contract_test.cpp | CTest targets pass |
| TC-02 | FR-02 | Assign correct road/lane | tests/simulation_service_test.cpp; simulation_api_contract_test.cpp | Pass |
| TC-03 | FR-03,NFR-07 | Front vehicle dequeues first | tests/queue_manager_test.cpp; test_traffic_manager.cpp; frontend/e2e/simulation.spec.ts | Pass |
| TC-04 | FR-04,SEC-03 | Legal signal sequence; reject invalid/conflicting transitions | tests/test_traffic.cpp | Pass |
| TC-05 | FR-05 | Only GREEN permits processing | tests/test_traffic_manager.cpp | Pass |
| TC-06 | FR-06 | Counts match enqueue/dequeue/reset | tests/queue_length_test.cpp; simulation_api_contract_test.cpp | Pass |
| TC-07 | FR-07 | Accurate processed/waiting/throughput | tests/simulation_statistics_manager_test.cpp | Pass |
| TC-08 | FR-08,NFR-01 | Display backend state/queues/signals/stats | frontend/src/App.test.tsx; components/IntersectionDashboard.test.tsx; e2e/simulation.spec.ts | Pass for assertions |
| TC-09 | FR-09 | Start/Pause/Resume/Reset state transitions | tests/simulation_control_api_test.cpp; frontend/e2e/simulation.spec.ts | Pass |
| TC-10 | FR-10,SEC-01 | Invalid/boundary input rejected; accepted config retained | tests/input_validator_test.cpp; frontend/src/App.test.tsx | Pass |
| TC-11 | FR-11 | RED/YELLOW retain waiting vehicles | tests/test_traffic_manager.cpp | Pass |
| TC-12 | FR-12 | Poll and refresh changes | frontend/src/hooks/useBackendStatus.test.ts; e2e/simulation.spec.ts | Pass |
| TC-13 | NFR-03,SEC-02 | Empty queue safely returns failure | tests/queue_empty_safety_test.cpp | Pass |
| TC-14 | NFR-02,NFR-08 | Usable controls and 390/768/1440 widths | frontend/e2e/simulation.spec.ts | Pass usability assertions; quantitative performance NOT VERIFIED |
| TC-15 | NFR-04,NFR-05 | Separate independently testable modules | CMakeLists.txt add_tms_test; docs/sprint-1/TC-15_Maintainability_Review.md | Source boundary review and CTest support PASS |
| TC-16 | SEC-04 | Memory/buffer review plus analysis | docs/sprint-1/TC-16_Source_Memory_Safety_Review.md; CI core sanitizers | Historical review/CI verified, fresh sanitizer run NOT RUN |

No significant FR lacks an associated executable suite. Tests do not prove every permutation. Supplemental ST-01â€“07 correspond to actual API/browser assertions. ST-08 is explicitly unexecuted broader accessibility/browser testing and is not an added university mandate.

## D. TEST EXECUTION RESULTS

Commands ran on the audit working tree on 19 Nov. Windows/PowerShell, installed dependencies and existing Release build tree were used.

| Command | Collected | Passed | Failed | Skipped | Outcome |
|---|---:|---:|---:|---:|---|
| frontend: npm run type-check | N/A | N/A | 0 | N/A | Exit 0 |
| frontend: npm test | 33 in 4 files | 33 | 0 | 0 | Exit 0 |
| frontend: npm run build | N/A | N/A | 0 | N/A | Exit 0; Vite 28 modules |
| ctest --test-dir build/release -C Release --output-on-failure, sandbox | 15 executables | 13 | 2 | 0 | HTTP server/contract exit 0xc0000409 |
| Same CTest command with local socket permission | 15 executables | 15 | 0 | 0 | Exit 0, 9.29 s |
| frontend: npm run test:e2e, API absent | 3 | 1 | 2 | 0 | ECONNREFUSED 127.0.0.1:18080, proxy 502 |
| Same Playwright command after starting Release/tms_api_server.exe | 3 | 3 | 0 | 0 | Exit 0, 28.7 s |
| Existing Vitest V8 coverage run | 33 | 33 | 0 | 0 | Exit 0, 51.21 s |
| git diff --check | N/A | N/A | 0 | N/A | Exit 0 |

C++ counts denote CTest executables, each containing multiple custom assertions. They are not 15 total assertion scenarios. Sandbox failure followed by success with loopback access is environmental evidence, not a confirmed product defect. The first browser run lacked the required API process. The successful rerun verified actual backend lifecycle/configuration/FIFO/statistics, navigation continuity and responsive widths. No fresh manual browser/assistive-technology inspection is claimed.

Coverage command from frontend:
npx --no-install vitest run --coverage --coverage.provider=v8 --coverage.reportsDirectory=$env:TEMP/tms-se-audit-coverage --coverage.include=src/**/*.ts --coverage.include=src/**/*.tsx --testTimeout=20000

Browser command used TMS_SCREENSHOT_DIR=$env:TEMP/tms-se-audit-browser to preserve existing repository captures. The audit starts the documented API and lets Playwright start Vite. Existing dependencies suffice. The incremental command cmake --build build/release --config Release --parallel 2 emitted only the MSBuild 18.9.1 banner and stalled for several minutes. It was interrupted. A fresh native rebuild is therefore NOT VERIFIED in this audit. CTest success uses existing Release binaries. The current frontend production build succeeded. Prior PR #53 CI provides independent native build evidence for its committed source.

Current GitHub PR #53 checks independently show eight SUCCESS conclusions at head 88c9a00, merged by pranav9360 on 16 Nov. They verify that committed source, not these uncommitted visual changes. CI files are .github/workflows/issue-33-validation.yml and issue-34-browser-e2e.yml. CI covers backend matrix, frontend, core analysis/sanitizers and Chromium. Automated deployment/CD is absent and is not required by the supplied rubric.

## E. COVERAGE ASSESSMENT

Fresh V8 results on current working tree, all frontend src TS/TSX included:
- Statements 282/311 = 90.67%.
- Lines 271/297 = 91.24%.
- Functions 85/91 = 93.40%.
- Branch counters 263/326 = 80.67%.

Uncovered examples reported by V8 include App.tsx 134,153â€“161; main.tsx 6â€“12; api/client.ts 103,115; api/errors.ts 51; ConfigurationForm.tsx 82; SimulationMonitor.tsx 48,52,288. Lower branch scopes include BackendConnectionCard 62.16%, SectionNavigation 50%, useBackendStatus 69.38%. ControlCentre branch coverage is 87.5%; SimulationMonitor 86.51%. These are coverage gaps, not confirmed bugs. CSS, native C++, Playwright execution and deployed behavior are outside V8 coverage. The provider is installed locally but absent from package.json, so a clean install needs the version-matched provider described in the existing report. Raw new output was directed to temporary storage. A later lookup under the sandbox TEMP path did not locate it, so durable availability of that new raw package is NOT VERIFIED; the command output and exact counters are retained here. No coverage threshold is imposed.

Historical C++ XML independently recalculated during this audit:
coverage/msvc-debug-42a57c45311551058c41ccf54fec8c2ca88215d2/cpp-msvc-debug.cobertura.xml
Project src/include union = 1,090 covered of 1,219 represented lines = 89.42%. All sampled branch flags are False. Its root branch-rate must not be interpreted as 100% branch coverage. Raw evidence is local/untracked and absent from git ls-files. The report excludes api_server_main.cpp and separates third-party/test sources. HTTP server line coverage is 72.99%; service 87.31%; response 90.17%, with defensive failure paths uncovered. Backend source has no working-tree changes, but this measurement remains revision-scoped to 42a57c4 and was not independently recollected.

**C++ branch coverage NOT VERIFIED / NOT MEASURED.** Overall whole-project branch coverage cannot be stated from frontend counters. Reproducing native instrumentation needs MSVC collector/setup and separate instrumented builds. Existing reports describe the instrumentation sequence but no single checked-in coverage script/configuration is present.

## F. DOCUMENTATION CONSISTENCY

| Location | Exact mismatch | Evaluation impact | Suggested correction / Agile interpretation |
|---|---|---|---|
| SRS DOCX Â§Â§2.4â€“2.6,3.1.3 | Desktop C/C++ GUI communication versus React/Crow HTTP | Updated SRS mandatory | Document web platform and C++ core boundary; justified migration |
| SRS Appendix B | MainWindow.cpp, Statistics.cpp, SimulationController.cpp references obsolete | Traceability difficult to defend | Map actual files from C; retain original as version history |
| SRS Â§3.1.1 | Observable movement versus stationary queue markers | Scope clarification needed | Define queue-state visualization and observed dequeue behavior |
| SAD Â§Â§3.3â€“3.6,4.4 | Desktop component/API diagrams omit service/HTTP/serializer/browser | Final architecture document incomplete | Add current architecture, six endpoint contracts and ownership |
| diagrams/component/component.mmd | Direct GUI/domain arrows and Queue-to-Statistics versus service-owned snapshot and model-driven statistics | Diagram does not explain runtime implementation | Version final diagram and keep historical one |
| diagrams/sequence/SD-03_Reset_Simulation.mmd | resetComplete callback/clearQueues design versus REST acknowledgement and model dequeue cleanup | Conceptual diagram differs from code | Add current sequence; disclose conceptual vs concrete interfaces |
| SAD Â§4.2 caption | Vehicle Arrival section caption says Start Simulation | Minor inconsistency | Correct caption when documents are updated |
| Test Plan Â§Â§6â€“7 | Desktop framework and Oct dates, no actual Nov 2â€“6 versus later testing schedule | SDLC timing evidence incomplete | Record actual dates, spillover and final web test environment |
| README Verification, Unit Report | 38 tests historical, current 33 | Can sound contradictory in viva | State revision and 10-to-5 monitor test replacement |
| Four Part3 reports | Baseline 42a57c4 predates PR #53/two pages | Reports exist but not fully final | Append current-run baseline and results |
| docs/Architecture.md Status | Full Playwright suite described as future work | Chromium suite now exists and passes | Separate completed Chromium from unexecuted cross-browser scope |
| RTM.csv / RTM_TC_Mapping.csv | Some evidence says 14 CTests/36 Vitest and old hashes | Traceability status stale | Keep historical records and add current validation row |
| include/sim_config.h comments | arrival says one vehicle per road, timing says per axis | Actual model generates one rotating approach per arrival interval and phases by approach | Correct wording after audit approval |
| GitHub issues #8,#10,#12 and others | Done/closed while body checklist still unchecked | Completion evidence hard to scan | Reconcile checklists with actual PR evidence |

No database, schema or persistent storage is used in the active implementation. Database compliance is NOT APPLICABLE. No live sensor integration is claimed. Active architecture is C++17 model/modules â†’ synchronized SimulationService â†’ adapter/API/serializer/Crow â†’ same-origin React client. Inspecting TrafficSignal::changeState confirms REDâ†’GREENâ†’YELLOWâ†’RED and other directions RED before a new GREEN. SimModel::tick processes allowed queues, rotates arrivals, updates statistics, then advances phases. QueueManager snapshots copy FIFO order. Reset clears activity but retains config. Average waiting ticks count completed vehicles; processed/tick is cumulative. Frontend polls every 1/5/10 seconds for running/paused/stopped. These decisions can be explained accurately in viva.

A scoped search of src/include/frontend source and workflows for credential-related terms found no exposed credential. This is not a comprehensive secret-history scan.

## G. GITHUB AND SPRINT AUDIT

Live GitHub API access succeeded after network permission. Project https://github.com/orgs/traffic-management-system-team/projects/1 is private, titled Traffic Management System â€“ Sprint Backlog. 22 items, all Done, each assigned and pointed. All 22 inspected issues are closed.

Sprint 1 (19â€“23 Oct) has #1=2, #2=3, #3=5, #4=3, #5=5, #6=5 points, total 23.
Sprint 2 (26â€“30 Oct) has #7=3, #8=3, #9=5, #10=5, #11=3, #12=5 points, total 24.
Later #26â€“35 have points but no sprint iteration, total 44. No additional sprint is automatically mandatory. Completion dates extend beyond the planned testing window 2â€“6 Nov, including PR #39 on 8 Nov and #53 on 16 Nov. Explain actual spillover honestly. Never backdate evidence.

| Member/account | Assigned stories and points | PR evidence | Local reachable author commits | Review/integration and limitations |
|---|---|---|---:|---|
| Pranav / pranav9360 | #1(2),#9(5),#12(5),#29(3),#31(5), total20 | #20,#36,#37,#41â€“44,#52,#53 merged |27| Merged #53 and approved #25/#51. Integrating author on statistics/config differs from assigned Nitish |
| Dinesh / Dinesh07-18 | #2(3),#3(5),#11(3),#26(5),#27(5),#33(5),#35(3), total29 | #15,#16,#25,#38,#40,#46,#49 merged |14| #25 approved by Pranav. #15 has no formal reviews. #50 closed unmerged |
| Gowtham / gowth139am-lab | #4(3),#5(5),#10(5),#30(5),#34(5), total23 | #17,#18,#45,#48,#51 merged |9 across two author spellings| #51 approved by Pranav. #17 Copilot quota comment is not an actual completed review |
| Nitish / nitish5052 | #6(5),#7(3),#8(3),#28(5),#32(3), total19 | #19,#39,#47 merged; #21,#23,#24 closed unmerged |4 across two emails| #19 contains authored GUI commit 5a7065b and no formal reviews. Explain how #36/#37 integrated his assigned work |

Counts are author records reachable from local HEAD and include merge/documentation commits. They are not lines-of-code or productivity scores. PR authorship, assignment and integration must not be conflated. PR #15 contains Dinesh-authored Vehicle Manager and release-safe checks; #17 contains Gowtham signal commits and review-feedback changes. Contributor code evidence is therefore stronger than name-only document tables. Individual oral understanding remains NOT VERIFIED.

GitHub formal reviews are selective rather than universal. Verified approval on #25 and #51, no formal reviews on #15,#19,#53. Do not claim every PR was independently approved.

### Sprint videos

No mp4/mov/webm/avi/mkv or video submission links were found through repository file and docs/README searches. Docs/Issue_12_Demo_Readiness.md explicitly leaves final recording as a handoff check. Actual Sprint 1 and Sprint 2 video files/content/durations are **NOT VERIFIED**. A separate LMS, Drive or private submission could exist outside this repository. GitHub project/issue/PR data inspected does not establish video delivery. Provide both required sprint outcome submissions, confirm which requires 1 minute and which 2 minutes against the actual rubric, and verify actual metadata/playback. No duration is fabricated.

## H. CRITICAL ISSUES

P0-1 Video evidence is unavailable. A mandatory submission cannot be certified. Locate the real Sprint 1/2 files/links before the viva and verify content/duration/access.
P0-2 Final SRS/SAD/Test Plan updates are incomplete. Historical documents are present, but final web alignment must be demonstrable. Record approved deviations and final requirements/interfaces.
P0-3 Branch Coverage Report discloses no native branch measurement. Obtain actual C++ branch data if feasible or present the limitation explicitly. Frontend 80.67% is not the C++ result.
No confirmed application blocker remains in the successful normal-flow test run.

## I. IMPORTANT BUT NON-BLOCKING ISSUES

P1 final four testing reports and RTM need current revision/results, including 33 versus 38 test explanation, frontend coverage and two-page UI. Raw native coverage evidence is untracked and should be preserved in the approved submission mechanism.
P1 later work has no sprint assignment and actual testing extends beyond Nov 2â€“6. Prepare a factual timeline and explain spillover.
P1 role assignment and PR integration authors differ in statistics/configuration work. Explain original work, feedback and final integration using commits.
P1 current redesign is uncommitted. State precisely which working tree is demonstrated and what CI actually verified.
P2 stale Architecture.md Playwright wording, issue checklists, model comments and SAD caption.

## J. OPTIONAL IMPROVEMENTS

Broader browser/screen-reader testing, quantitative latency benchmarks, repeatable coverage scripts and deployment automation would improve evidence. They are not extra mandatory university criteria. No production traffic fidelity, physical sensors, database, automated CD or authentication is required by the supplied brief.

## K. VIVA PREPARATION

1. What problem does the project solve? It demonstrates controlled intersection traffic using FIFO queues, signal rules and measurable waiting/processing behavior. It is an educational simulation.
2. What are the main requirements? FR-01â€“12 cover arrivals, lane assignment, FIFO, signals, permitted movement, queue lengths/statistics, UI, controls, validation, blocked vehicles and refresh. Refer to SRS Â§3.2.
3. Why FIFO? QueueManager uses per-road queues and removes only the front. Snapshots copy the queue to preserve ordered vehicle display. See queue_manager.cpp and FIFO tests.
4. How are conflicting greens prevented? SignalCoordinator rejects a new GREEN unless every other approach is RED and allows only the legal state cycle.
5. What happens each tick? SimModel::tick checks running state, increments logical time, processes permitted queued vehicles, records wait/processed stats, generates a rotating arrival when due, then advances signal phase.
6. Who owns state and concurrency? One SimulationService owns the model and scheduler. A mutex serializes commands, ticks, configuration and snapshots.
7. What architecture did you choose? Layered presentation, HTTP adapter/API, synchronized application service and reusable simulation modules. The serializer is separate from the core.
8. Why migrate from desktop GUI? The repository records the desktop baseline and current web design. React now presents backend snapshots via Crow REST. Exact team/business rationale and approval detail require team confirmation.
9. What does Reset preserve? It clears ticks/queues/statistics, returns stopped and restores North GREEN, while retaining accepted configuration.
10. How do you validate configuration? Central SimLimits bounds and InputValidator reject missing/type/range errors. Changes are accepted only before simulation activity; rejected changes preserve the prior config.
11. What does average wait mean? Total completed-vehicle waiting ticks divided by processed vehicles. Throughput is cumulative processed/logical tick, not vehicles/second.
12. How did you test it? C++ CTest module/API tests, Vitest client/hooks/components and real-backend Chromium Playwright. Fresh results are 15/15 targets, 33/33 tests and 3/3 browser tests.
13. What is your coverage? Fresh frontend lines91.24%, branches80.67%. Historical native lines89.42%. Native branches have not been measured. Explain scopes before quoting numbers.
14. Where is CI/CD? GitHub Actions workflows under .github/workflows perform backend matrix, frontend, core sanitizers/static analysis and real-API browser CI. Deployment is manual; automated CD is absent.
15. What did each member contribute and how did sprints evolve? Cite assigned pointed project items and individual PRs in G. Distinguish author work from Pranav integration. Sprint 1/2 iteration records exist; later migration work has no sprint placement. Videos and personal explanation require team-provided evidence.

## L. FINAL VERDICT

**INSUFFICIENT EVIDENCE** to certify every mandatory university deliverable. Executed application tests pass and individual GitHub contributions are evidenced. The final document alignment, native branch measurement, current final-report baseline and sprint video submissions are incomplete or unavailable.

### Prioritized action plan

| Priority | Location | Explanation | Required fix | Why evaluation matters | Verify afterward |
|---|---|---|---|---|---|
| P0 | Sprint1/2 submission locations, absent from repo | Outcome videos inaccessible | Locate or record actual required submissions with correct timing | Explicit mandatory deliverable | Play both, check durations and reviewer access |
| P0 | docs/SRS, docs/SAD, docs/Test_Plan DOCX/PDF | Final implementation differs from historical records | Update final version and trace deviations | Demonstrates SDLC alignment | Cross-check all IDs, final diagrams, interfaces and tests |
| P0 | docs/Branch_Coverage_Report_Part3.md | C++ branches unmeasured | Collect actual native branch outcomes or disclose unresolved scope | Required branch report needs real evidence | Inspect raw counts and revision; never infer from branch-rate=1 |
| P1 | Four docs/*Report_Part3.md and RTM CSVs | Old revision/38 tests/old frontend percentages | Add final revision-scoped runs and fresh metrics | Reports must represent delivered implementation | Compare to D/E and current source |
| P1 | GitHub project1 issues26â€“35 | Later work lacks sprint dates; schedule slipped | Document actual timeline and any authorized spillover | Agile process must be explainable | Show dated PRs and actual board iterations |
| P1 | Issues7/8, PRs23/24/36/37 | Assigned owner and merging author differ | Prepare truthful commit-level ownership explanation | Individual contributions assessed | Show actual PR diffs and history |
| P1 | Current dirty feature branch | CI verifies committed baseline only | Record exact demo source state and approved validation evidence | Reproducibility | Match demo files with recorded test results |
| P2 | Architecture.md, SAD caption, sim_config.h, issue checklists | Stale words/comments/checklists | Correct after audit approval | Avoid contradictory viva claims | Review final document/code references |

### Answers to the ten final questions

1. **Implementation versus SRS** â€” Core FR behavior is consistent for tested cases. Platform/network wording, visual movement and obsolete file references need final SRS reconciliation.
2. **Architecture versus SAD** â€” Modular responsibilities align, but formal SAD does not describe the implemented web/service/REST architecture.
3. **Mandatory documents present** â€” Named SRS, SAD, Test Plan, Test Case Document and four reports exist. Their final-version completeness is PARTIAL. Video submissions are not verified.
4. **Four reports complete** â€” No. They are substantive, historical reports. Current frontend revision/results and native branch measurement remain gaps.
5. **Tests actually passed** â€” Yes, fresh typecheck/build, 33 Vitest, 15 CTest and 3 Playwright passed. Environmental failed first attempts are recorded in D.
6. **Branch coverage properly documented** â€” Its limitation is honest, but current C++ branch coverage is unavailable. Fresh frontend branch coverage is80.67%.
7. **Sprint and contribution records sufficient** â€” Strong assigned/pointed stories and individual PR evidence exist. Later iteration placement, stale completion checklists and integration ownership explanations need clarification.
8. **Required videos present** â€” NOT VERIFIED. No local files or accessible linked submissions were found.
9. **Exactly missing** â€” Accessible sprint video evidence/durations, final aligned formal documents, native branch measurements, final revision-scoped reports/RTM and complete later-work sprint timeline.
10. **Before viva** â€” First secure videos and final document alignment, disclose native branch limits, prepare the measured testing summary and individual PR links, and rehearse documented API+frontend startup plus lifecycle/configuration/FIFO demonstration.
