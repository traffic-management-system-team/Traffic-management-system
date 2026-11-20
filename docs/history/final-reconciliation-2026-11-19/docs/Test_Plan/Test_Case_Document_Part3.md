
# Test Case Document — Part 3

**Project:** Traffic Management System
**Baseline:** 42a57c45311551058c41ccf54fec8c2ca88215d2 (PR #52)
**Prepared:** 2026-11-15

## Scope and source

The original Test Plan v1.3 dated 06-Oct-2026 remains unchanged in Test_Plan_Traffic_Management_System.docx and its PDF. It defines TC-01–TC-16 against the initial desktop/GUI baseline. This companion retains those IDs and SRS mappings while describing validation of the final React/TypeScript/Vite browser application and C++17/Crow backend. It does not replace the SRS or claim the archived Qt prototype is the current frontend.

Mappings follow docs/sprint-1/RTM_TC_Mapping.csv and docs/sprint-1/RTM.csv. Detailed execution evidence is in the root FINAL_PROJECT_VERIFICATION_REPORT.md. No tests were rerun for these documents.

Environment: Windows 11 Home build 26200, PowerShell 7.6.9, CMake 4.4.4, MSVC 19.51.36256.0, Windows SDK 10.0.26100.0, Node.js 22.17.0, npm 10.9.2, Playwright 1.65.0, Chromium 158.0.8127.18. CTest 15/15, Vitest 38/38, Playwright 3/3 were recorded at the verified commit.

## Original test cases updated for final implementation

| ID | SRS mapping | Test and expected result | Actual evidence / status |
|---|---|---|---|
| TC-01 | FR-01 | Advance logical ticks; vehicles are generated with valid identity and approach under configured arrival behavior. | API contract/service tests passed; live API observed arrivals. **PASS** |
| TC-02 | FR-02 | Generated vehicles are assigned to intended approach queues and lane identity. | Deterministic API contract case and live snapshots passed/observed. **PASS** |
| TC-03 | FR-03, NFR-07 | Enqueue several vehicles and verify insertion order through snapshots and dequeue. | queue_manager_test, traffic_manager FIFO case and API contract passed; live IDs ascending. **PASS** |
| TC-04 | FR-04, SEC-03 | Advance phases; verify backend signal state, serialization, legal transitions and tested rejection cases. | traffic_signal, API contract and serializer tests passed. Negative coverage is limited to assertions present in tests. **PASS for tested cases** |
| TC-05 | FR-05, NFR-07 | RED/YELLOW vehicles remain queued; processing occurs FIFO when movement is permitted on GREEN. | traffic_manager and API contract cases passed; live processing observed. **PASS** |
| TC-06 | FR-06 | Compare queue length with actual queued vehicles after arrivals, dequeue and reset. | queue_length and API snapshot consistency cases passed; reset cleared four queues. **PASS** |
| TC-07 | FR-07 | Compare generated/processed/waiting and waiting-time statistics with deterministic state and serialization. | Service, statistics, serializer and API contract tests passed; live values changed and reset. **PASS for tested backend calculations/serialization** |
| TC-08 | FR-08, NFR-01 | Load dashboard from backend snapshot; show state, approaches, signals, queues and aggregate statistics. | Dashboard/App tests and real-backend browser checks passed. **PASS for tested dashboard behavior** |
| TC-09 | FR-09 | Exercise Start/Pause/Resume/Reset and verify UI follows API state. | C++ control/service tests and Playwright passed; pause held ticks, resume advanced, reset cleared state and retained config. **PASS** |
| TC-10 | FR-10, SEC-01 | Submit valid/invalid config; invalid values return details and do not replace accepted config. | Validator, API/HTTP and Playwright passed. HTTP 400 VALIDATION_FAILED returned field detail; accepted values stayed unchanged. **PASS for tested cases** |
| TC-11 | FR-11 | Blocked approaches do not process contrary to signal rules. | Traffic manager/API contract tests passed; live signal-gated processing observed. **PASS for tested backend behavior** |
| TC-12 | FR-12 | Trigger state changes and verify browser refreshes display. | Polling/App/dashboard tests passed; Playwright observed lifecycle, signal, queue and statistic changes. **PASS for tested scenarios** |
| TC-13 | NFR-03, SEC-02 | Empty queue operations fail safely without invalid access/state corruption. | queue_empty_safety, security_reliability and related tests passed. **PASS for tested cases** |
| TC-14 | NFR-02, NFR-08 | Check 390/768/1440 px usability and overflow; inspect keyboard/focus path. | Automated viewport checks passed; recorded keyboard skip-link checks passed. Full manual WCAG/screen-reader and cross-browser audits **NOT RUN**. |
| TC-15 | NFR-04, NFR-05 | Review module boundaries and independently testable targets/core separation. | Independent targets and source review support modularity; API-enabled CTest 15/15 passed. **PASS for inspected/tested scope** |
| TC-16 | SEC-04 | Review memory/buffer safety and available static/runtime analysis. | Existing review/prior CI recorded Clang analysis and ASan/UBSan; those runs are revision-scoped and were not repeated here. Ordinary CTest is not a sanitizer run. **PASS with limitation** |

## Supplemental web/API acceptance scenarios

| ID | Scenario / expected result | Actual evidence |
|---|---|---|
| ST-01 | GET state returns stopped, tick 0, empty queues and zero stats after reset. | HTTP 200, expected snapshot. **PASS** |
| ST-02 | Valid PUT returns accepted server config. | HTTP 200, values matched. **PASS** |
| ST-03 | Invalid PUT returns validation details; accepted config and draft preserved. | HTTP 400; API kept tickMs 500; UI retained invalid greenTicks draft while accepted remained 10. **PASS** |
| ST-04 | Start/Pause/Resume/Reset refresh authoritative state. | All HTTP 200; pause held ticks, resume advanced, reset cleared counters/queues. **PASS** |
| ST-05 | Arrivals/FIFO/signals/statistics progress on backend and appear in UI. | Live snapshots showed arrivals/processing, ascending FIFO IDs, signal changes and changing stats. **PASS** |
| ST-06 | Lifecycle conflict shows useful error and refreshes state. | Expected HTTP 409 handled; UI remained usable. **PASS** |
| ST-07 | Mobile/tablet/desktop layouts avoid overflow. | Automated 390/768/1440 px checks passed. **PASS** |
| ST-08 | Full manual WCAG/screen-reader and cross-browser matrix. | Not executed. **NOT RUN** |

## Limitations and conclusion

Recorded final Windows Release CTest: 15/15; frontend type-check/build passed; Vitest 38/38; real-backend Chromium Playwright 3/3. Instrumented frontend suite passed 38/38 with a 20-second timeout; two tests timed out during an earlier 5-second coverage attempt due to instrumentation overhead. No tests/source were changed.

Frontend coverage was measured. An initial GCC 6.3.0 coverage attempt failed because the C++17 dependency build could not locate <optional>. A subsequent separate MSVC 19.51.36256.0 x64 Debug coverage run instrumented all 15 C++ test executables, and CTest passed 15/15. Measured project-owned C++ line/block coverage was 1,090/1,219 (89.42%). Test-source coverage was 2,649/2,725 (97.21%), and third-party Crow, Asio and nlohmann-json coverage was 5,707/9,497 (60.09%); these scopes are reported separately. src/api_server_main.cpp was built but not measured. C++ branch coverage: NOT RUN. The Microsoft Cobertura export did not provide branch outcome measurements; its root branch-rate=1 is not a branch coverage result. Evidence is retained in coverage/msvc-debug-42a57c45311551058c41ccf54fec8c2ca88215d2/ (cpp-msvc-debug-summary.md, cpp-msvc-debug.coverage, cpp-msvc-debug.cobertura.xml and ctest-coverage-run.log). NFR-06 portability evidence is in earlier CI/RTM records but was not repeated in this Windows run. Manual accessibility, cross-browser and deployed-environment checks remain NOT RUN.

TC-01–TC-16 retain original IDs and traceability while reflecting current web/backend architecture. Recorded automated tests passed within the stated scope.

## Supplemental portability review

| Mapping | Review and expected evidence | Actual evidence / status |
|---|---|---|
| NFR-06 | Review supported-platform build/test evidence; successful supported-platform CI should configure, build and pass the registered test suite. | docs/sprint-1/RTM_TC_Mapping.csv records GitHub Actions run #38 at PR head ce670996581b752816d3043d60ee1e18bcc62d48 with Windows and Ubuntu Debug/Release CTest passing 14/14. This was not rerun at verified commit 42a57c45311551058c41ccf54fec8c2ca88215d2. **NOT RUN at this baseline; prior CI record only.** |