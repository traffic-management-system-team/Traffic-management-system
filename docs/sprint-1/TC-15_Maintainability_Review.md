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

- **NFR-04 — modular implementation completed; latest test verification pending.** Vehicle, queue, signal, movement, service, validation/control, statistics aggregation and serialization have identifiable module boundaries. Statistics counters, waiting-time totals and aggregate snapshot calculations live in the separately testable `SimulationStatisticsManager`. `SimModel` remains the intended intersection coordinator rather than implementing the statistics calculations itself.
- **NFR-05 — independently buildable/testable core modules.** Core production code remains a separate CMake target with separate module/service/statistics-manager test executables. The API integration tests use public service/server interfaces rather than requiring access to HTTP adapter internals. The latest revision adds one core test target; fresh results are pending as recorded in `RTM_TC_Mapping.csv`.


## 5. Module test and build verification status

The prior full-project execution at commit `162336e206390e74ac04be5bc2fe66ee17f3a1c2` passed API-enabled Debug 13/13, API-enabled Release 13/13 and core-only Debug 10/10. That execution predates the addition of `SimulationStatisticsManager`, its standalone unit test, the dashboard aggregate metrics and the CI workflow. It is **not** evidence for the current branch head.

The refactor adds one core CTest target, so the expected current counts are eleven core targets, three API-enabled targets, fourteen API-enabled tests total. Fresh Debug, Release, core-only and frontend runs must complete on the current branch head before those counts are marked passed. The Windows/Ubuntu CI matrix, full-core Clang ASan/UBSan job and clang-tidy clang-analyzer job are still queued in GitHub Actions run #28.

The statistics-manager unit test independently exercises counter accumulation, per-approach ordering, snapshot aggregation, the generated = processed + waiting invariant, invalid enum handling and reset. On the current head (`6463e3e1fff028f99ed6db2eaf4360847d8e950c`), the exact manager source/header/test were also compiled and executed in an isolated Linux harness: GCC 14.2 with `-std=c++17 -Wall -Wextra -Wpedantic -Werror` passed, and Clang 17 with AddressSanitizer + UndefinedBehaviorSanitizer and leak detection enabled passed. This is focused manager-module evidence only; it is not a full CMake/CTest or full-core sanitizer run.

## 6. Out of scope / evidence limits

The frontend dashboard is implemented and has Vitest component/hook coverage for four-approach signal/queue display, FIFO data, aggregate metrics, snapshot refresh, adaptive polling, keyboard form submission, accessible signal names and responsive CSS structure. The frontend quality checks are run by the new GitHub Actions job; until that job passes for the final head, no fresh workflow result is claimed.

A full cross-browser Playwright suite and manual screen-reader/accessibility audit are not present. No real-browser latency/frame-rate benchmark is claimed for NFR-02. The Windows/Ubuntu workflow is intended to supply NFR-06 cross-platform build evidence when it passes. The Linux ASan/UBSan job supplies dynamic evidence for SEC-04 when it passes, but does not replace a static-analysis report or ThreadSanitizer run.

