# Security and Reliability Validation — Issue #11

## Scope and method

This review records evidence for SEC-01 through SEC-04, NFR-03, and TC-04, TC-10, TC-13, and TC-16 against the current implementation. Existing tests are reused where they already exercise the requirement; this change does not introduce a duplicate security test target.

The source review covered the active C++ implementation and public interfaces for configuration parsing/application, signal coordination, traffic/queue processing, vehicle storage, simulation model/service lifecycle, HTTP request handling and state serialization. The retired `legacy/qt-prototype` is not part of the active CMake build and is excluded from the active implementation findings.

## Automated evidence

| Requirement / test | Evidence in current source | Validation focus |
|---|---|---|
| SEC-01 / TC-10 — configuration input | `tests/input_validator_test.cpp`, `tests/simulation_control_api_test.cpp`, `tests/simulation_api_contract_test.cpp` | Empty, whitespace, malformed, non-integer, very long, boundary and out-of-range values; all five fields; invalid values do not overwrite last accepted configuration; backend rejection does not commit a candidate. |
| SEC-02 / TC-13 — empty queue | `tests/queue_empty_safety_test.cpp`, `tests/queue_manager_test.cpp`, `tests/test_traffic_manager.cpp` | Missing and empty roads return false/zero, do not remove an item, and leave the caller's output vehicle unchanged. |
| SEC-03 / TC-04 — signal transitions | `tests/test_traffic.cpp`, `tests/test_traffic_manager.cpp` | Valid RED→GREEN→YELLOW→RED cycle; invalid/repeated transitions rejected; conflicting approaches cannot be GREEN together; invalid enum and invalid approach inputs rejected without mutating valid states. |
| NFR-03 | Same tests above | Invalid configuration and empty-queue paths are handled as rejected operations, not unchecked access. |
| Lifecycle / reliability supporting evidence | `tests/simulation_service_test.cpp`, `tests/simulation_api_contract_test.cpp`, `tests/simulation_http_server_test.cpp` | Serialized scheduler/model access, consistent snapshots, concurrent operations, shutdown and HTTP boundary behavior. |

## TC-16 manual source review

### Findings

1. **No unsafe C string or raw allocation APIs found in the reviewed active implementation.** The source search/review found no use of `strcpy`, `strcat`, `sprintf`, `gets`, `malloc`, `calloc`, `realloc`, or manual `free` in the active C++ implementation. No `reinterpret_cast` or `const_cast` use was found in the reviewed active paths. This is a bounded source review, not a proof that every possible defect is absent.
2. **Input parsing is bounded.** `InputValidator::parseInt` rejects numeric text longer than 12 characters before accumulating digits, accepts only ASCII decimal digits after optional sign/whitespace, and therefore bounds the integer accumulation. Error display copies at most 12 input characters and replaces non-printable bytes.
3. **Queue access checks precede access.** `QueueManager::dequeue` uses `find`, verifies the road exists and the queue is non-empty, then reads `front()` and pops. Snapshot operates on a local queue copy and returns value-owned vehicles.
4. **Vehicle storage uses standard containers and value ownership.** `VehicleManager` stores `Vehicle` values in an `unordered_map`; insertion checks ID exhaustion and duplicate insertion. `findVehicle` returns a pointer to a stored value or null. That pointer is non-owning and must not be retained across erase or operations that may rehash the map.
5. **Non-owning references have explicit lifetime requirements.** `TrafficSignal` stores a pointer to a `SignalCoordinator`; `TrafficManager` stores references to its signal and queue manager. Their referents must outlive the wrapper/manager. `SimModel` member declaration/construction order satisfies this relationship: coordinator and queue are constructed before signals/managers and destroyed after them. Callers constructing these classes independently must preserve the same lifetime rule.
6. **Model indexing is guarded at external boundaries.** Signal coordinator validates approach indices before array access. `SimModel::signal`, `queueLength`, and `queuedVehicles` validate road indices before indexing or return safe fallback values. Internal approach progression uses a closed switch with an exception for an unexpected value.
7. **HTTP request storage is bounded.** The pinned Crow parser patch caps request-body storage at 64 KiB before JSON parsing; adapter tests cover body-size boundaries and stable errors.
8. **Scheduler and server lifecycle are joined/serialized.** `SimulationService` serializes model access under one mutex and joins its worker on destruction. The HTTP server owns and joins its worker; the service lifetime must exceed the server lifetime.

### Findings requiring care (not silently waived)

- `VehicleManager::findVehicle` exposes a non-owning pointer into an `unordered_map`. Current reviewed call sites use it for immediate lookup; callers must not cache it across mutation/rehash. A future API could return an optional value/snapshot if long-lived retention becomes necessary.
- The raw pointer/reference members in `TrafficSignal` and `TrafficManager` are non-owning by design. Current `SimModel` member ordering keeps their dependencies alive. Standalone users must honor the documented lifetime contract.
- No ThreadSanitizer run is claimed. Existing synchronization/concurrency tests and ASan/UBSan do not substitute for TSan.
- Static analysis previously completed successfully but reported two intentional invalid-enum-cast diagnostics in negative tests. They are disclosed as test inputs, not described as warning-free analysis.

No confirmed memory-safety defect was identified in this bounded review. The above lifetime constraints are recorded as maintenance risks; they are not evidence of a currently reproducible crash.

## Reproduction

From the repository root, use the existing CMake/CTest targets. For the core-only configuration:

```powershell
cmake -S . -B build\issue11-core-debug -DBUILD_API_SERVER=OFF -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build\issue11-core-debug --config Debug
ctest --test-dir build\issue11-core-debug -C Debug --output-on-failure

cmake -S . -B build\issue11-core-release -DBUILD_API_SERVER=OFF -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build\issue11-core-release --config Release
ctest --test-dir build\issue11-core-release -C Release --output-on-failure
```

The API-enabled Windows/Ubuntu Debug/Release matrix, frontend checks, ASan/UBSan and static analysis are supplied by the repository's existing CI workflow. Results for this PR must be taken from the current PR head's workflow, not copied from a previous commit.

## Conclusion

The existing implementation and tests cover the invalid-configuration, invalid-transition and empty-queue acceptance areas. This review records the missing TC-16 evidence and the non-owning lifetime constraints. Final completion is conditional on the new negative-path tests and the full current-head CI matrix passing, plus independent review of this evidence.
