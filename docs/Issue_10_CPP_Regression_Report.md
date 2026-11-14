# Issue #10 — C++ Regression and Web Acceptance Report

## 1. Scope

This report records the completed local Windows regression and browser acceptance run for Issue #10. It retains the earlier MinGW 8/8 run as historical Stage 1 evidence and records the later API-enabled Release run separately.

## 2. Environment

| Item | Recorded environment |
|---|---|
| Operating system | Windows (PowerShell) |
| CMake | 4.4.5 |
| CMake generator | Visual Studio 18 2026 |
| C++ compiler | MSVC 19.52.37029 (detected by CMake) |
| Windows SDK | 10.0.26100 |
| CMake configuration | Release, `BUILD_API_SERVER=ON`, `BUILD_TESTING=ON` |
| Node.js | v24.9.0 |
| npm | 11.6.0 |
| Playwright | `@playwright/test` 1.65.0 |
| Browser | Chromium |
| Branch | `gowtham/issue-10-regression` |
| Local HEAD at test time | `de01f7bd6223b48437e43f5fdc8cffc6ca28a611` |

The `cl` command was not available in the ordinary PowerShell PATH. CMake nevertheless detected MSVC 19.52.37029 and built through the Visual Studio generator/MSBuild. The compiler version above is taken from CMake's successful configure output, not from a direct `cl` invocation.

## 3. C++ regression build and test results

### Configuration and build commands

```powershell
cmake -S . -B build-issue10-e2e -DBUILD_API_SERVER=ON -DBUILD_TESTING=ON -DFETCHCONTENT_FULLY_DISCONNECTED=ON
cmake --build build-issue10-e2e --config Release --parallel 2
ctest --test-dir build-issue10-e2e -C Release --output-on-failure
```

`FETCHCONTENT_FULLY_DISCONNECTED=ON` reused the already-fetched dependencies after the Crow request-body-limit patch had been applied in the generated dependency tree. No project source or patch file was changed to work around that generated-build-directory issue.

### CTest summary

- Configuration: Release, API server enabled, testing enabled.
- Registered C++ tests: 15.
- Passed: 15.
- Failed: 0.
- Not run: 0.
- Result: **100% passed**.
- Recorded run duration: **9.59 seconds**.

### Individual results

| Test target | Result |
|---|---|
| `vehicle_manager_test` | PASS |
| `queue_manager_test` | PASS |
| `queue_empty_safety_test` | PASS |
| `queue_length_test` | PASS |
| `traffic_signal_test` | PASS |
| `traffic_manager_test` | PASS |
| `gui_model_test` | PASS |
| `input_validator_test` | PASS |
| `security_reliability_test` | PASS |
| `simulation_service_test` | PASS |
| `simulation_statistics_manager_test` | PASS |
| `simulation_control_api_test` | PASS |
| `simulation_http_server_test` | PASS |
| `simulation_state_serializer_test` | PASS |
| `simulation_api_contract_test` | PASS |

The registered suite includes core vehicle/queue/traffic behavior, validation and security/reliability, simulation service and statistics, control API, HTTP server, state serialization, and API contract coverage. The API contract tests include rejection of invalid configuration while preserving the last accepted values, as well as configuration/state-transition checks.

### C++ build observations

- The API server target `tms_api_server` built successfully.
- MSVC emitted conversion/deprecation warnings in the fetched Crow headers during HTTP compilation. These were warnings; the build completed successfully.
- The earlier CMake attempt failed because the generated Crow source had already received the local request-body-limit patch and CMake attempted to apply it again. Restoring only the generated Crow source to its pinned commit and reconfiguring with `FETCHCONTENT_FULLY_DISCONNECTED=ON` resolved this. The tracked patch and project CMake files were not changed.

## 4. Real-backend API and browser acceptance

### API server startup and smoke check

The API server was built and started with:

```powershell
.\build-issue10-e2e\Release\tms_api_server.exe
```

Startup output confirmed the listener at `http://127.0.0.1:18080`. A request to `GET /api/simulation/state` returned `success = true`, simulation state `stopped`, active approach `north`, active signal `green`, and `tickCount = 0`.

### Playwright command

From `frontend/`:

```powershell
npx playwright install chromium
$env:API_PROXY_TARGET = "http://127.0.0.1:18080"
npm run test:e2e
```

Result: **3 passed, 0 failed**, one worker, **13.0 seconds**.

| Browser test | Result |
|---|---|
| Configures, runs, pauses, resumes and resets the real simulation | PASS |
| Dashboard usable at mobile, tablet and desktop widths | PASS |
| Keyboard access to the main content | PASS |

The lifecycle browser test uses the real C++ backend through the Vite same-origin proxy; it does not mock `/api` responses. It verifies:

- Initial reset and stopped state.
- Invalid Pause while stopped returns HTTP 409 with the `INVALID_STATE` error envelope.
- A valid configuration is accepted through keyboard form submission.
- An invalid `greenTicks = 0` configuration returns HTTP 400 with `VALIDATION_FAILED` and a `greenTicks` validation detail.
- The browser displays the field/global error feedback, marks the field invalid, retains the draft, and preserves the last accepted value (`greenTicks = 10`) in both the UI and authoritative backend snapshot.
- Correcting the field clears the error and the backend accepts the corrected configuration.
- Start, running state, vehicle generation, processed/waiting statistics, changing statistics, backend-observed yellow signal phase, visible signal label, Pause, Resume, and Reset.
- Reset returns generated, processed, and waiting statistics to zero.
- Responsive viewport checks at 390 px, 768 px, and 1440 px with no horizontal overflow.
- Keyboard skip-link focus, visibility, and navigation to the main content.

## 5. Frontend checks

Commands were run from `frontend/`:

```powershell
npm ci
npm run type-check
npm test
npm run build
```

| Check | Result |
|---|---|
| `npm ci` | PASS; 0 vulnerabilities reported |
| `npm run type-check` | PASS |
| `npm test` | PASS; 38 tests across 4 test files |
| `npm run build` | PASS; TypeScript check and Vite production build completed |
| `npm run test:e2e` | PASS; 3 real-backend Chromium tests |

Vitest file results:

| Test file | Tests |
|---|---:|
| `src/api/client.test.ts` | 8 |
| `src/hooks/useBackendStatus.test.ts` | 4 |
| `src/components/IntersectionDashboard.test.tsx` | 10 |
| `src/App.test.tsx` | 16 |
| **Total** | **38** |

## 6. Historical Stage 1 evidence

The original `cpp-test-results.txt` and the initial version of this report record an earlier Windows run using MinGW GCC 6.3.0, CMake 4.4.5, MinGW Makefiles, GUI disabled, and testing enabled. That run executed 8 registered tests and passed 8/8.

That 8/8 result is retained as historical Stage 1 evidence. It is not the final API-enabled CTest count. The later MSVC Release configuration registered and passed 15/15 tests, including the additional API/HTTP/serializer targets.

## 7. Defects, failures, and limitations

- No failing C++ tests, frontend unit tests, type checks, production build checks, or Playwright tests were observed in the recorded final run.
- The build emitted third-party Crow conversion/deprecation warnings under MSVC; there were no build errors.
- Automated browser coverage in this run targeted Chromium only.
- The keyboard accessibility check verifies the skip-link keyboard path. It is not a full manual screen-reader, assistive-technology, or WCAG audit.
- No deployment-environment smoke test or cross-browser matrix is included in this local evidence.
- `cl` was not directly callable from the ordinary PowerShell PATH; MSVC was detected and invoked successfully through CMake/MSBuild.

## 8. Evidence files and source state

- Historical C++ output: `cpp-test-results.txt`.
- Current API-enabled Release CTest output: `cpp-test-results-issue10-release.txt`.
- Browser test source updated for the real-backend invalid-configuration and stopped-Pause checks: `frontend/e2e/simulation.spec.ts`.
- Configure diagnostic log from the initial failed Crow patch attempt: `cmake-issue10-configure.log` (diagnostic only; not required as acceptance evidence).
- Local branch at test time: `gowtham/issue-10-regression`.
- Local HEAD at test time: `de01f7bd6223b48437e43f5fdc8cffc6ca28a611`. The Playwright source file had uncommitted changes at test time, so the browser evidence reflects the working-tree version of that file rather than the committed HEAD alone.

## 9. Conclusion and handoff

- C++ API-enabled Release regression: **PASS (15/15)**.
- Frontend unit/type/build checks: **PASS (38/38 unit tests; type-check and build passed)**.
- Real-backend Chromium browser acceptance: **PASS (3/3)**.
- No blocking failures were observed in the recorded local run.

Issue #10 evidence is ready for a focused test/evidence pull request and independent team review. Keep Issue #10 open until the evidence is reviewed and the team confirms the acceptance criteria and handoff.
