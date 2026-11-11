# TC-15 Maintainability Review (NFR-04 and NFR-05)

## Review scope

This review evaluates the active C++ backend boundaries relevant to TC-15. It is a source/interface and CMake dependency review, not a claim that passing tests alone prove maintainability. The reviewed branch is `dinesh/issue-33-api-contract-tests`. The initial REST contract test suite passed at commit `162336e206390e74ac04be5bc2`. Later commits added the independently tested statistics manager, dashboard aggregate metrics, responsive dashboard assertions, and CI; fresh verification of those later changes is pending.

## 1. Module boundaries and public interfaces

| Area | Interface / implementation evidence | Responsibility |
|---|---|---|
| Vehicle | `include/vehicle.h`, `include/vehicle_manager.h`; `src/vehicle.cpp`, `src/vehicle_manager.cpp` | Vehicle value/type and ID/lifetime registry |
| Queue | `include/queue_manager.h`; `src/queue_manager.cpp` | Per-road FIFO storage, guarded dequeue, independent FIFO snapshots |
| Signal | `src/TrafficSignal.h`, `src/TrafficSignal.cpp` | Individual signal state transitions and shared four-approach coordinator |
| Traffic movement | `src/TrafficManager.h`, `src/TrafficManager.cpp` | Signal-gated dequeue for one configured approach |
| Simulation model | `include/simulation_model.h`, `src/simulation_model.cpp` | Logical tick ordering, arrivals, movement coordination and signal phases; delegates counters/aggregate statistics to `SimulationStatisticsManager` |
| Service | `include/simulation_service.h`, `src/simulation_service.cpp` | Authoritative model owner, scheduler worker, synchronization and copied snapshots |
| Validation/configuration/control | `include/input_validator.h`, `include/config_controller.h`, `include/simulation_control_api.h`; matching `src/` implementations | Framework-neutral input/configuration and command policy |
| Snapshot DTO/statistics | `include/simulation_dto.h`, `include/simulation_statistics.h` | Plain copied state/statistics structures; no HTTP types |
| Statistics aggregation | `include/simulation_statistics_manager.h`, `src/simulation_statistics_manager.cpp` | Independent generated/processed counters, waiting-time aggregation, per-approach mapping, reset and snapshot calculations |
| State serialization | `include/simulation_state_serializer.h`, `src/simulation_state_serializer.cpp` | Convert a supplied snapshot to JSON data; no model ownership |
| HTTP host/adapter | `include/simulation_http_server.h`, `include/simulation_http_adapter.h`; `src/simulation_http_server.cpp`, `src/simulation_http_adapter.cpp` | Listener lifecycle, route registration, HTTP/JSON translation and common response envelopes |

The public `SimulationService` interface exposes start, pause, resume, reset, configuration, state/statistics reads and a value-copy `SimulationSnapshot`. Its implementation uses a private `Impl` owned by `std::unique_ptr`; the worker thread, mutex, condition variable and `SimModel` are hidden from callers. The service header has no Crow or nlohmann-json include.

`SimulationControlApi` accepts framework-neutral `ApiRequest` data and returns `ApiResponse`; it does not own an HTTP listener, scheduler or `SimModel`. `SimulationHttpAdapter` is the framework boundary: it maps Crow requests into the framework-neutral API and forwards the API response. The state route obtains a copied snapshot from the same service and calls the serializer.

## 2. CMake target boundaries and build evidence

The active CMake graph has these production boundaries:

- `tms_core`: simulation, vehicle, queue, traffic signal, service, configuration and framework-neutral control code. It links Threads; it does not link Crow or nlohmann-json.
- `tms_state_serializer`: serializer code linked to `tms_core` and nlohmann-json.
- `tms_http`: HTTP adapter/server linked to `tms_core` and Crow; it privately links the state serializer.
- `tms_api_server`: executable linked to `tms_http`.

With `BUILD_TESTING=ON`, CMake registers eleven core test targets, including `simulation_statistics_manager_test`. With `BUILD_API_SERVER=ON`, it adds three API-enabled targets: `simulation_http_server_test`, `simulation_state_serializer_test`, and `simulation_api_contract_test`, for fourteen total. With `BUILD_API_SERVER=OFF`, the core and its eleven tests can be configured without fetching Crow, Asio or nlohmann-json.

The API contract tests instantiate a real `SimulationService` and `SimulationHttpServer` bound to `127.0.0.1` and port `0`; they do not substitute mocked route handlers. The deterministic model test also calls `SimModel::tick()` directly to avoid wall-clock timing in logical-tick assertions.

## 3. Review findings

**Positive separation**

1. The simulation service is usable without constructing an HTTP server. The worker lifecycle and model synchronization are encapsulated behind its public interface.
2. Control/configuration policy is separated from Crow route registration. The control API uses framework-neutral request/response structs.
3. JSON serialization consumes a DTO snapshot rather than reaching into the model. The DTO and statistics types do not depend on Crow or JSON.
4. The CMake graph keeps the HTTP framework and JSON serialization dependencies out of `tms_core`. Core targets have independent test executables.
5. The HTTP integration tests and new contract tests start the actual server on loopback with an ephemeral port, and pass the service by reference.
6. Statistics aggregation is now encapsulated in `SimulationStatisticsManager`; `SimModel` provides tick count and per-approach queue lengths to its snapshot API rather than owning aggregate counter arrays and formulas.

**Maintainability gaps**

1. `SimModel` remains the orchestration point for vehicle creation, queue updates, traffic-manager calls and signal-phase advancement. This is the aggregate coordination boundary; it now delegates statistics counter ownership and aggregation to `SimulationStatisticsManager`.
2. Queue-length observations are passed to the statistics manager in the established North/South/East/West approach order. The independent manager test verifies the mapping and aggregate totals.
3. The HTTP adapter header includes Crow and is intentionally tied to the HTTP target. This is acceptable at the boundary; it must not be included from `tms_core` or core public interfaces.
4. The review is a focused code/interface inspection, not an automated architecture-dependency rule or a formal maintainability metric.

## 4. TC-15 assessment

- **NFR-04 — modular implementation completed; latest-head module tests passed.** Vehicle, queue, signal, movement, service, validation/control, statistics aggregation and serialization have identifiable module boundaries. Statistics counters, waiting-time totals and aggregate snapshot calculations live in the separately testable `SimulationStatisticsManager`. `SimModel` remains the intended intersection coordinator rather than implementing the statistics calculations itself.
- **NFR-05 — independently buildable/testable core modules.** Core production code remains a separate CMake target with separate module/service/statistics-manager test executables. The API integration tests use public service/server interfaces rather than requiring access to HTTP adapter internals. Latest-head API-enabled builds passed 14/14 tests in Windows and Ubuntu Debug/Release; the core-only sanitizer build passed all 11 core tests.


## 5. Module test and build verification status

The older local full-project execution at commit `162336e206390e74ac04be5bc2fe66ee17f3a1c2` (13/13 API-enabled Debug, 13/13 API-enabled Release and 10/10 core-only Debug) predates the statistics-manager refactor and is retained as historical evidence only.

**Latest verified implementation commit:** `ce670996581b752816d3043d60ee1e18bcc62d48`. GitHub Actions PR run [#38](https://github.com/traffic-management-system-team/Traffic-management-system/actions/runs/42949329618) completed with all seven jobs successful:

- Windows Debug: configure/build passed; CTest 14/14.
- Windows Release: configure/build passed; CTest 14/14.
- Ubuntu Debug: configure/build passed; CTest 14/14.
- Ubuntu Release: configure/build passed; CTest 14/14.
- Frontend: `npm run type-check` passed; Vitest 36/36 across four test files; `npm run build` passed.
- Core-only Clang ASan/UBSan build and CTest: 11/11 passed.
- Clang static-analysis job completed successfully. It emitted two enum-cast diagnostics in the deliberate invalid-enum negative tests (`tests/test_traffic.cpp:112` and `tests/simulation_statistics_manager_test.cpp:72`). The workflow job passed, but this is not described as a warning-free analysis.

The API-enabled matrix runs the 11 core tests plus three API-enabled tests (14 total) in all four platform/configuration combinations. The sanitizer/static-analysis jobs are scoped to the core-only target set.

The statistics-manager unit test covers counter accumulation, per-approach ordering, snapshot aggregation, the generated = processed + waiting invariant, invalid enum handling and reset. The independent TC-15 source/interface review records module responsibility and CMake boundaries.

## 6. Out of scope / evidence limits

The frontend dashboard is implemented and has Vitest component/hook coverage for four-approach signal/queue display, FIFO data, aggregate metrics, snapshot refresh, adaptive polling, keyboard form submission, accessible signal names and responsive CSS structure. The latest-head frontend type-check, all 36 tests, and production build passed in run #38.

A full cross-browser Playwright suite and manual screen-reader/accessibility audit are not present. No real-browser latency/frame-rate benchmark is claimed for NFR-02. The Windows/Ubuntu Debug/Release matrix passed for NFR-06. The Linux ASan/UBSan job passed 11/11 core tests and the Clang static-analysis job completed successfully, with the two test-code enum-cast diagnostics recorded above. No ThreadSanitizer run is included.

