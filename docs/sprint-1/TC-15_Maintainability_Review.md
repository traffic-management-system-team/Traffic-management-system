# TC-15 Maintainability Review (NFR-04 and NFR-05)

## Review scope

This review evaluates the active C++ backend boundaries relevant to TC-15. It is a source/interface and CMake dependency review, not a claim that passing tests alone prove maintainability. The reviewed branch is `dinesh/issue-33-api-contract-tests`. The executable test source was verified at commit `162336e206390e74ac04be5bc2fe66ee17f3a1c2`; subsequent PR commits updated only traceability documentation.

## 1. Module boundaries and public interfaces

| Area | Interface / implementation evidence | Responsibility |
|---|---|---|
| Vehicle | `include/vehicle.h`, `include/vehicle_manager.h`; `src/vehicle.cpp`, `src/vehicle_manager.cpp` | Vehicle value/type and ID/lifetime registry |
| Queue | `include/queue_manager.h`; `src/queue_manager.cpp` | Per-road FIFO storage, guarded dequeue, independent FIFO snapshots |
| Signal | `src/TrafficSignal.h`, `src/TrafficSignal.cpp` | Individual signal state transitions and shared four-approach coordinator |
| Traffic movement | `src/TrafficManager.h`, `src/TrafficManager.cpp` | Signal-gated dequeue for one configured approach |
| Simulation model | `include/simulation_model.h`, `src/simulation_model.cpp` | Logical tick ordering, arrivals, movement coordination, signal phases and aggregate statistics |
| Service | `include/simulation_service.h`, `src/simulation_service.cpp` | Authoritative model owner, scheduler worker, synchronization and copied snapshots |
| Validation/configuration/control | `include/input_validator.h`, `include/config_controller.h`, `include/simulation_control_api.h`; matching `src/` implementations | Framework-neutral input/configuration and command policy |
| Snapshot DTO/statistics | `include/simulation_dto.h`, `include/simulation_statistics.h` | Plain copied state and statistics structures; no HTTP types |
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

With `BUILD_TESTING=ON`, CMake registers ten core test targets. With `BUILD_API_SERVER=ON`, it adds three API-enabled targets: `simulation_http_server_test`, `simulation_state_serializer_test`, and `simulation_api_contract_test`, for thirteen total. With `BUILD_API_SERVER=OFF`, the core and its ten tests can be configured without fetching Crow, Asio or nlohmann-json.

The API contract tests instantiate a real `SimulationService` and `SimulationHttpServer` bound to `127.0.0.1` and port `0`; they do not substitute mocked route handlers. The deterministic model test also calls `SimModel::tick()` directly to avoid wall-clock timing in logical-tick assertions.

## 3. Review findings

**Positive separation**

1. The simulation service is usable without constructing an HTTP server. The worker lifecycle and model synchronization are encapsulated behind its public interface.
2. Control/configuration policy is separated from Crow route registration. The control API uses framework-neutral request/response structs.
3. JSON serialization consumes a DTO snapshot rather than reaching into the model. The DTO and statistics types do not depend on Crow or JSON.
4. The CMake graph keeps the HTTP framework and JSON serialization dependencies out of `tms_core`. Core targets have independent test executables.
5. The HTTP integration tests and new contract tests start the actual server on loopback with an ephemeral port, and pass the service by reference.

**Maintainability gaps**

1. `SimModel` remains the orchestration point for vehicle creation, queue updates, traffic-manager calls, signal phase advancement and statistics accumulation. This is a deliberate aggregate boundary, but it is relatively broad and should remain a focus of future decomposition.
2. Aggregate statistics are calculated by `SimModel::statistics()`; `simulation_statistics.h` defines data types, not an independent statistics manager. NFR-04's “statistics logic separated into maintainable modules” is therefore only partially demonstrated.
3. The HTTP adapter header includes Crow and is intentionally tied to the HTTP target. This is acceptable at the boundary; it must not be included from `tms_core` or core public interfaces.
4. The review is a focused code/interface inspection, not an automated architecture-dependency rule or a formal maintainability metric.

## 4. TC-15 assessment

- **NFR-04 — partially verified.** Vehicle, queue, signal, movement, service, validation/control and serialization responsibilities have identifiable interfaces and implementation files. The aggregate simulation model still coordinates several of them, and statistics calculations are not an independent module. These are recorded gaps rather than hidden by the passing test count.
- **NFR-05 — verified for the represented core modules.** Core production code is a separate CMake target with separate module/service test executables. The independent API-enabled test targets integrate through public service/server interfaces rather than requiring tests to reach into the HTTP adapter's internal state. The final revision-bound Debug/Release results are in `RTM_TC_Mapping.csv`.


## 6. Revision-bound build and test verification

**Source revision tested:** `162336e206390e74ac04be5bc2fe66ee17f3a1c2`

**Environment:** Windows x64, Clang 23.1.5 via `clang-cl`, CMake/Ninja Multi-Config.

| Build mode | Configure/build | CTest result | Scope |
|---|---|---:|---|
| API-enabled Debug | Passed | 13/13 passed | 10 core tests + 3 API-enabled tests |
| API-enabled Release | Passed | 13/13 passed | 10 core tests + 3 API-enabled tests |
| Core-only Debug, `BUILD_API_SERVER=OFF` | Configure and build passed | 10/10 passed | Core only; API dependencies are not required |

The API-enabled suite preserves all ten core test registrations and adds three API-enabled tests. The new `simulation_api_contract_test` uses a real loopback HTTP server with an ephemeral port and the actual `SimulationService`; the deterministic logical-tick test drives `SimModel::tick()` directly without wall-clock sleeps for tick-count assertions.

These are locally executed test results reported from the checked-out revision. No CI workflow result is claimed. No ThreadSanitizer, AddressSanitizer, UndefinedBehaviorSanitizer, static-analysis report, or second-platform build was produced by this review.


## 5. Out of scope for this review

This review does not establish browser GUI usability/responsiveness, cross-platform portability, or comprehensive memory-safety assurance. Those are tracked separately in the RTM and are not marked as satisfied by this document.
