# TC-16 — Complete C++ Source and Memory-Safety Review

## Review result

**Manual source review completed for the active C++ implementation at commit `92d7ab0bb6f5bb544c90cd3a0adef8b6c14baf70`.** No confirmed out-of-bounds access, unchecked C buffer operation, or use-after-free defect was identified in the reviewed paths. This is a bounded source review, not a formal proof of memory safety.

## Reviewed production file inventory

All production headers under `include/`:
- `config_controller.h`
- `input_validator.h`
- `queue_manager.h`
- `sim_config.h`
- `simulation_control_api.h`
- `simulation_dto.h`
- `simulation_http_adapter.h`
- `simulation_http_bind_address.h`
- `simulation_http_limits.h`
- `simulation_http_response.h`
- `simulation_http_server.h`
- `simulation_model.h`
- `simulation_service.h`
- `simulation_state_serializer.h`
- `simulation_statistics.h`
- `simulation_statistics_manager.h`
- `vehicle.h`
- `vehicle_manager.h`

All production implementations under `src/`:
- `TrafficManager.cpp`, `TrafficManager.h`
- `TrafficSignal.cpp`, `TrafficSignal.h`
- `api_server_main.cpp`
- `config_controller.cpp`
- `input_validator.cpp`
- `queue_manager.cpp`
- `simulation_control_api.cpp`
- `simulation_http_adapter.cpp`
- `simulation_http_response.cpp`
- `simulation_http_server.cpp`
- `simulation_model.cpp`
- `simulation_service.cpp`
- `simulation_state_serializer.cpp`
- `simulation_statistics_manager.cpp`
- `vehicle.cpp`
- `vehicle_manager.cpp`

The retired `legacy/qt-prototype/gui_main.cpp` is excluded from the active build and is not represented as production-reviewed. Test sources are not production implementation.

## Findings — bounds, parsing, and buffers

- Configuration parsing bounds the input length before numeric accumulation, accepts only sign/digits, and validates configured ranges before conversion to `int`.
- Signal coordinator indexes are checked before fixed-array access. Model road indexes are checked before array access.
- HTTP request bodies have a configured maximum (default 65,536 bytes); Crow parses JSON and the adapter copies fields into value-owned request data.
- No raw C string copy/format/scan routines or manual owning `new[]`/`delete[]` pairs were found in active production source.
- HTTP JSON envelopes/state serialization use nlohmann-json; the framework-neutral API explicitly escapes JSON string values.

**Disposition:** No confirmed unchecked-buffer or out-of-bounds defect found in the reviewed active source. Malformed/oversized input and bounds behavior have automated tests.

## Findings — ownership and lifetimes

- `SimulationService` owns its implementation via `unique_ptr`; the implementation owns the model, config controller, mutex, condition variable and scheduler thread.
- `SimulationHttpServer` owns its implementation, but the implementation holds a non-owning `SimulationService&`. The service must outlive the server. `api_server_main.cpp` constructs service before server, so reverse destruction destroys server first.
- `SimulationHttpAdapter` holds references to API, service and origin policy; `SimulationHttpServer::Impl` owns these dependencies and the adapter.
- `SimulationControlApi` holds a non-owning service reference; its owner must outlive it.
- Each `TrafficManager` holds non-owning references to a `TrafficSignal` and `QueueManager`. Coordinated `TrafficSignal` holds a non-owning pointer to `SignalCoordinator`. `SimModel` member declaration order constructs dependencies before referents and reverse destruction destroys referents first.
- `VehicleManager::findVehicle` returns a borrowed pointer into an unordered map. Erasing that vehicle invalidates the pointer. Callers must not retain it across removal.
- `Vehicle::getLane()` returns a reference into the Vehicle; it must not outlive that object. Queue/service snapshots copy values instead of exposing internal queue storage.
- Crow JSON parsing values are local to request conversion; parsed scalar values are copied into `ApiRequest`.

**Disposition:** These are non-owning lifetime contracts, not observed dangling-pointer defects. Current construction/destruction ordering was checked. Consider a value/optional return for `findVehicle` if future callers need to retain lookup results.

## Findings — thread safety

- `SimModel`, `QueueManager`, `SignalCoordinator`, `VehicleManager`, `ConfigController` and `SimulationStatisticsManager` do not independently synchronize access. They are not safe for arbitrary concurrent use.
- Production model access through `SimulationService::Impl` is serialized by one mutex: scheduler ticks, commands, configuration changes, snapshots, statistics and state reads all take that lock.
- The service owns one scheduler thread. Destruction sets the shutdown flag under lock, notifies the condition variable, and joins the worker before model state is destroyed.
- `SimulationHttpServer::Impl` protects lifecycle state/future with a lifecycle mutex and waits for its async server worker during stop. Crow owns request-worker execution.
- Direct standalone use of lower-level model/managers or `ConfigController` outside the service must be externally serialized.
- No ThreadSanitizer run is claimed; ASan/UBSan do not detect data races.

**Disposition:** The production synchronization boundary is the service mutex and is exercised by service/HTTP concurrency tests. Lower-level classes remain intentionally unsynchronized.

## Findings — error paths and residual limitations

- Queue dequeue checks map membership and emptiness before accessing `front()`; snapshots copy the queue.
- Vehicle creation checks ID exhaustion and duplicate insertion; Vehicle construction rejects non-positive IDs and empty lanes.
- Configuration is applied only after validation and model acceptance.
- API handlers return generic error envelopes for unexpected exceptions.
- A simulation tick can throw after earlier mutations if an internal invariant fails (for example, missing arrival/vehicle records). The service catches the failure and pauses the scheduler but does not roll back that partially completed tick.
- Statistics use unsigned 64-bit counters without saturation; theoretical wrap after extremely long operation is not explicitly handled.

| ID | Finding | Disposition |
|---|---|---|
| TC16-F01 | Server/API/adapter/manager/signal use non-owning references/pointers. | Current owner lifetime ordering checked; contracts documented. |
| TC16-F02 | `findVehicle` pointer is invalid after removal of its element. | Borrowed-pointer contract documented; no current production caller retains it across removal. |
| TC16-F03 | Core model/managers are not independently thread-safe. | Service mutex serializes production access; external direct use requires synchronization. |
| TC16-F04 | No TSan/fuzzing run; sanitizers are not proof of race freedom. | Limitation disclosed; no unsupported claim. |
| TC16-F05 | Statistics counters do not saturate at uint64 maximum. | Extreme-duration limitation documented; no observed test failure. |
| TC16-F06 | Internal tick invariant failure can leave partial tick mutations before scheduler pauses. | Recovery limitation documented; transactional tick rollback is not implemented. |

No separate defect issue was opened because F01–F06 are documented design constraints/limitations and no reproducible defect was established. If future changes violate these contracts or reproduce partial-tick corruption, create a linked defect issue.

## Automated evidence

Reviewed branch commit: `92d7ab0bb6f5bb544c90cd3a0adef8b6c14baf70`.

GitHub Actions workflow run **#76** completed successfully with all seven jobs:
- [Workflow run #76](https://github.com/traffic-management-system-team/Traffic-management-system/actions/runs/43413265356)
- Core ASan/UBSan job **131847186111** — success.
- Clang Static Analyzer job **131847187003** — success.
- Ubuntu Debug job **131847186202** — success.
- Ubuntu Release job **131847183908** — success.
- Windows Debug job **131847239063** — success.
- Windows Release job **131847244118** — success.
- Frontend type-check/tests/build job **131847185380** — success.

The static-analysis job completed successfully; that result is not represented as a claim of zero warnings. Job logs remain the authoritative execution evidence.

## Conclusion and limits

TC-16 manual review is complete for the active `include/` and `src/` production C++ files at the reviewed commit. Ownership/lifetime assumptions, synchronization boundaries, findings and limitations are recorded. This is not formal verification and does not claim exhaustive dependency review, TSan, fuzzing, or proof of absence of all defects.
