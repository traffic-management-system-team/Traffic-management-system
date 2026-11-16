# Code Coverage Report â€” Part 3

**Repository:** https://github.com/traffic-management-system-team/Traffic-management-system
**Source commit:** `42a57c45311551058c41ccf54fec8c2ca88215d2`
**Measured:** 2026-11-16
**Environment:** Windows 11; Visual Studio Community 2026 18.9.2; MSVC 19.51.36256.0 x64; CMake 4.4.4; Node.js 22.17.0; npm 10.9.2; Vitest 5.1.0/V8.

## Scope and methodology

Frontend coverage was collected from the existing Vitest suite using the V8 provider. C++ line/block coverage was collected in a separate x64 Debug build using MSVC profiling instrumentation and Microsoft.CodeCoverage.Console. The normal Release build was left untouched. CTest ran serially under the collector.

C++ toolchain and setup:

- Compiler: MSVC 19.51.36256.0 x64.
- Collector: Microsoft.CodeCoverage.Console 18.9.0-preview.26325.2 (console banner 18.9.0.0).
- Separate build: `build/coverage-msvc-debug-project`, Visual Studio 18 2026 generator, Debug, `BUILD_API_SERVER=ON`, `BUILD_TESTING=ON`, linker flags `/PROFILE /DEBUG`.
- All 15 CTest executables had matching PDB files and were instrumented before the complete CTest run.
- The collector exported raw `.coverage` data and Cobertura XML. The exported per-line hit data represents collected instrumentation. Source line totals are deduplicated by repository file and line across test executables.

The frontend provider was installed temporarily with `npm install --no-save --package-lock=false @vitest/coverage-v8@5.1.0`; the coverage run used a 20-second test timeout. The frontend suite passed 38/38. This scope matches `frontend/src/**/*.{ts,tsx}` and excludes C++, Playwright and runtime paths not exercised by Vitest.

## Measured coverage

### Frontend

**PASS â€” measured by Vitest/V8 from 38 passing tests.**

| Metric | Covered / total | Percent |
|---|---:|---:|
| Lines | 204 / 225 | 90.67% |
| Statements | 208 / 230 | 90.43% |
| Functions | 59 / 62 | 95.16% |
| Branches | 155 / 194 | 79.90% |

### C++

**PASS â€” measured C++ project-owned source line/block coverage: 1,090 / 1,219 = 89.42%.** This is a deduplicated union of reported executable lines in repository `src/` and `include/` files represented in the PDB/Cobertura output across the 15 instrumented CTest executables. It describes those test executions; it does not represent all production code or all possible runtime paths.

| Scope | Covered / total unique source lines | Rate | Interpretation |
|---|---:|---:|---|
| Project-owned `src/` and `include/` | 1,090 / 1,219 | 89.42% | Primary C++ line/block result for measured test execution |
| Test sources | 2,649 / 2,725 | 97.21% | Test-code coverage; not product coverage |
| Third-party Crow, Asio and nlohmann-json | 5,707 / 9,497 | 60.09% | Dependency lines; excluded from project-owned result |
| `src/api_server_main.cpp` | Not measured | NOT RUN | Built, but not instrumented or launched; only the 15 CTest executables were instrumented and executed |

The Cobertura root aggregate (18,601 / 32,499) combines repeated copies of source lines compiled into different test executables and mixes project, test and dependency code. It is not used as the project coverage result.

## Project-owned C++ per-file results

| Source file | Covered / total | Line coverage | Uncovered reported lines |
|---|---:|---:|---|
| `include/config_controller.h` | 3 / 3 | 100% | â€” |
| `include/sim_config.h` | 3 / 3 | 100% | â€” |
| `include/simulation_control_api.h` | 1 / 1 | 100% | â€” |
| `include/simulation_http_adapter.h` | 2 / 2 | 100% | â€” |
| `include/simulation_http_bind_address.h` | 17 / 17 | 100% | â€” |
| `include/simulation_http_response.h` | 6 / 6 | 100% | â€” |
| `include/simulation_model.h` | 12 / 12 | 100% | â€” |
| `src/config_controller.cpp` | 16 / 19 | 84.21% | 14â€“16 |
| `src/input_validator.cpp` | 62 / 62 | 100% | â€” |
| `src/queue_manager.cpp` | 44 / 44 | 100% | â€” |
| `src/simulation_control_api.cpp` | 101 / 116 | 87.07% | 15â€“21, 24â€“26, 78, 91, 279â€“280, 285 |
| `src/simulation_http_adapter.cpp` | 111 / 122 | 90.98% | 83â€“84, 88, 104â€“106, 110, 112, 115, 138, 216 |
| `src/simulation_http_response.cpp` | 156 / 173 | 90.17% | 23, 31, 64, 69, 78, 92, 106, 115, 124, 137, 148, 151, 160, 174, 182, 214â€“215 |
| `src/simulation_http_server.cpp` | 100 / 137 | 72.99% | 39, 64â€“65, 84â€“94, 102, 107â€“108, 111, 129, 131â€“135, 140â€“141, 144, 181, 184, 195, 204, 226â€“227, 235â€“236, 241â€“242, 247, 249â€“251 |
| `src/simulation_model.cpp` | 122 / 137 | 89.05% | 29, 53, 85, 130, 145, 165, 168, 178, 192â€“193, 200, 219, 228, 232, 240 |
| `src/simulation_service.cpp` | 117 / 134 | 87.31% | 129â€“132, 179â€“180, 185â€“189, 194â€“195, 197, 247â€“249 |
| `src/simulation_state_serializer.cpp` | 39 / 41 | 95.12% | 34, 47 |
| `src/simulation_statistics_manager.cpp` | 62 / 62 | 100% | â€” |
| `src/TrafficManager.cpp` | 13 / 16 | 81.25% | 30â€“32 |
| `src/TrafficSignal.cpp` | 63 / 66 | 95.45% | 120, 137â€“138 |
| `src/vehicle_manager.cpp` | 20 / 22 | 90.91% | 12, 20 |
| `src/vehicle.cpp` | 20 / 24 | 83.33% | 13, 25, 32, 36 |

Counts are source lines represented by the coverage/PDB report; blank, comment-only and non-emitted lines are not in the denominator.

## Uncovered regions and limitations

These are measured line-coverage gaps, not established defects:

- `src/simulation_http_server.cpp`: rejected second start, worker-launch exceptions/startup exit, timeout cleanup, stopped-worker polling, and worker completion/exception paths (reported ranges include 63â€“65, 84â€“111, 129â€“144, 180â€“204, 225â€“251).
- `src/simulation_http_response.cpp`: malformed host/IP/port parsing and canonicalization, plus environment-read failure (23â€“184, 214â€“215).
- `src/config_controller.cpp`: configuration rejection accounting (14â€“16).
- `src/simulation_control_api.cpp`: JSON escaping and generic unexpected-error response (15â€“26, 279â€“285).
- `src/simulation_model.cpp`: defensive invariant/error paths for queue/arrival bookkeeping, enqueue/stat recording and prohibited transitions (reported gaps include 129â€“130, 163â€“178, 191â€“200, 218â€“232).
- `src/simulation_service.cpp`: scheduler exception/failure handling and statistics forwarding (129â€“132, 179â€“197, 247â€“249).
- `src/TrafficManager.cpp` and `src/TrafficSignal.cpp`: reset and signal-reset paths (30â€“32, 120, 137â€“138).
- `src/api_server_main.cpp` was built but not measured because the run covered only CTest executables.
- C++ branch coverage is **NOT RUN**. The Cobertura export marks line entries `branch=False`; its root `branch-rate=1` is an exporter field and is not a branch-coverage measurement. No C++ branch percentage is claimed.
- This is test-suite coverage and excludes production paths not executed by those tests. It must not be interpreted as complete production-code or runtime-path coverage.

Build warnings included third-party Crow header warnings C4244, C4267 and C4702, and linker warning LNK4075 because `/PROFILE` disables incremental linking. CMake also emitted an unused `FETCHCONTENT_FULLY_DISCONNECTED` warning while local dependency source overrides were used.

## Evidence locations

All C++ raw evidence, logs, export and per-file summary are in:

`coverage/msvc-debug-42a57c45311551058c41ccf54fec8c2ca88215d2/`

Files:

- `cpp-msvc-debug.coverage` â€” raw Microsoft coverage data.
- `cpp-msvc-debug.cobertura.xml` â€” exported line coverage data.
- `cpp-msvc-debug-summary.md` â€” run identity, scope, per-file results and uncovered regions.
- `cmake-configure-project.log`, `cmake-build-debug.log`, `ctest-list.log` â€” valid build configuration/build and registered tests.
- `instrumentation.log`, `ctest-coverage-run.log`, `collector.log`, `coverage-export.log` â€” instrumentation, execution and export logs.
- `cmake-configure.log` â€” retained diagnostic log from an initial configure attempt that was not built or tested; the valid run used `build/coverage-msvc-debug-project`.

The frontend coverage measurements and uncovered locations are retained in this report from the previously generated Vitest/V8 results. Raw frontend coverage output is not present in this verification worktree; only the C++ raw coverage package listed above is available here.

## Conclusion

Frontend line and branch coverage are measured at 90.67% and 79.90%, respectively. C++ project-owned line/block coverage is measured at 89.42% for the 15 CTest executable runs, with test-source and third-party figures reported separately. C++ branch coverage remains NOT RUN.
