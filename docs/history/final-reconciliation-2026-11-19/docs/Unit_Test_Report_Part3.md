
# Unit Test Report — Part 3

**Repository:** https://github.com/traffic-management-system-team/Traffic-management-system
**Verified commit:** 42a57c45311551058c41ccf54fec8c2ca88215d2
**Environment/date:** Windows 11 Home build 26200, PowerShell 7.6.9; 2026-11-15

## Scope and method

Records C++ unit/regression and frontend unit/component results at the verified source. Integration/browser evidence is in System_Validation_Report_Part3.md; coverage is in the two coverage reports. Evidence source: root FINAL_PROJECT_VERIFICATION_REPORT.md. No tests were rerun while drafting.

Environment: CMake 4.4.4, MSVC 19.51.36256.0, Windows SDK 10.0.26100.0; Node.js 22.17.0, npm 10.9.2, Vitest 5.1.0. C++ used API-enabled Release CMake/CTest. Frontend used Vitest; coverage run used 20-second timeout due to instrumentation overhead.

## C++ unit and regression

**PASS — 15/15 passed; 0 failed.**

| CTest target | Scope |
|---|---|
| vehicle_manager_test | Vehicle identity/lifecycle |
| queue_manager_test | Queue operations/order |
| queue_empty_safety_test | Empty queues |
| queue_length_test | Queue accounting |
| traffic_signal_test | Signal transitions/coordination |
| traffic_manager_test | Signal-gated FIFO processing |
| gui_model_test | Historical target name; core model, not Qt GUI |
| input_validator_test | Configuration validation |
| security_reliability_test | Safety/reliability |
| simulation_service_test | Shared service/lifecycle |
| simulation_statistics_manager_test | Statistics/snapshot/reset |
| simulation_control_api_test | Control API |
| simulation_http_server_test | HTTP routes/errors |
| simulation_state_serializer_test | State/config serialization |
| simulation_api_contract_test | Integrated API contract/FIFO/validation |

Recorded commands:

    cmake -S . -B build/release -G "Visual Studio 18 2026" -A x64 -DBUILD_API_SERVER=ON -DBUILD_TESTING=ON -DCMAKE_CONFIGURATION_TYPES=Release
    cmake --build build/release --config Release --parallel 2
    ctest --test-dir build/release -C Release --output-on-failure

Build and CTest succeeded. MSVC warnings C4244/C4267/C4996 came from fetched Crow headers; no build errors.

## Frontend unit/component tests

**PASS — 38/38 across four files.**

| File | Tests | Coverage |
|---|---:|---|
| frontend/src/api/client.test.ts | 8 | Requests/responses/errors |
| frontend/src/hooks/useBackendStatus.test.ts | 4 | Polling/state/lifecycle/config |
| frontend/src/components/IntersectionDashboard.test.tsx | 10 | State/signals/queues/stats |
| frontend/src/App.test.tsx | 16 | App/config/controls/loading/errors |
| **Total** | **38** | |

Recorded commands:

    npm run type-check
    npm test
    npm run build

Coverage-instrumented Vitest also passed 38/38 with testTimeout 20000. An earlier instrumented attempt at the default 5 seconds timed out in two UI tests; the longer timeout resolved instrumentation overhead without source/test changes. Uninstrumented final verification was 38/38.

## Traceability and limits

TC-01–TC-16 are retained in Test_Plan/Test_Case_Document_Part3.md and mapped through docs/sprint-1/RTM_TC_Mapping.csv. Unit suites do not establish exhaustive security, full accessibility, cross-browser or deployment readiness. An initial GCC 6.3.0 coverage attempt failed because the C++17 dependency build could not locate <optional>; a subsequent separate MSVC 19.51.36256.0 x64 Debug run instrumented all 15 C++ test executables and CTest passed 15/15. Measured project-owned C++ line/block coverage was 1,090/1,219 (89.42%); test-source coverage was 2,649/2,725 (97.21%); third-party Crow, Asio and nlohmann-json coverage was 5,707/9,497 (60.09%). src/api_server_main.cpp was built but not measured. C++ branch coverage: NOT RUN: the Microsoft Cobertura export provided no branch outcome measurements, and its root branch-rate=1 is not a coverage result. Evidence is retained in coverage/msvc-debug-42a57c45311551058c41ccf54fec8c2ca88215d2/ (cpp-msvc-debug-summary.md, cpp-msvc-debug.coverage, cpp-msvc-debug.cobertura.xml and ctest-coverage-run.log). Previous static-analysis/sanitizer evidence is revision-scoped, not a new run at this commit.

**Conclusion:** Recorded CTest and Vitest passed; type-check and production build passed. No tests were rerun for this report.
