# Build and Test Instructions

## 1. Requirements

- A C++ compiler with C++17 support (`g++` recommended).
- CMake 3.16 or newer.
- Git and network access to fetch the pinned Crow, Asio, and JSON dependencies for API builds.
- Windows Command Prompt (CMD).
- Run all commands from the repository root.
- Queue Manager tests use the `Vehicle` module integrated through PR #15.

---

## 2. PR #15 - Vehicle Manager

### Build

```cmd
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src\vehicle.cpp src\vehicle_manager.cpp tests\vehicle_manager_test.cpp -o vehicle_manager_test.exe
```

### Run

```cmd
vehicle_manager_test.exe
```

### Expected result

```text
All VehicleManager tests passed.
```

---

## 3. PR #16 - Queue Manager

Queue Manager depends on the `Vehicle` type and implementation supplied by PR #15. PR #15 has been merged into `main`. The Queue Manager tests compile against the integrated files `include/vehicle.h` and `src/vehicle.cpp`.

Do not add duplicate copies of the Vehicle module to PR #16.

### 3.1 Multi-road queue test - Debug

Build:

```cmd
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src\vehicle.cpp src\queue_manager.cpp tests\queue_manager_test.cpp -o queue_manager_test.exe
```

Run:

```cmd
queue_manager_test.exe
```

Verified result:

```text
All multi-road QueueManager tests passed.
```

### 3.2 Multi-road queue test - Release

Build:

```cmd
g++ -std=c++17 -DNDEBUG -O2 -Wall -Wextra -Wpedantic -Iinclude src\vehicle.cpp src\queue_manager.cpp tests\queue_manager_test.cpp -o queue_manager_release_test.exe
```

Run:

```cmd
queue_manager_release_test.exe
```

Verified result:

```text
All multi-road QueueManager tests passed.
```

### 3.3 Empty-queue safety test - Debug

Build:

```cmd
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src\vehicle.cpp src\queue_manager.cpp tests\queue_empty_safety_test.cpp -o queue_empty_safety_test.exe
```

Run:

```cmd
queue_empty_safety_test.exe
```

Verified result:

```text
TC-13 empty-queue safety passed.
```

### 3.4 Empty-queue safety test - Release

Build:

```cmd
g++ -std=c++17 -DNDEBUG -O2 -Wall -Wextra -Wpedantic -Iinclude src\vehicle.cpp src\queue_manager.cpp tests\queue_empty_safety_test.cpp -o queue_empty_safety_release_test.exe
```

Run:

```cmd
queue_empty_safety_release_test.exe
```

Verified result:

```text
TC-13 empty-queue safety passed.
```

### 3.5 Queue-length test - Debug

Build:

```cmd
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src\vehicle.cpp src\queue_manager.cpp tests\queue_length_test.cpp -o queue_length_test.exe
```

Run:

```cmd
queue_length_test.exe
```

Verified result:

```text
Queue length tests passed.
```

### 3.6 Queue-length test - Release

Build:

```cmd
g++ -std=c++17 -DNDEBUG -O2 -Wall -Wextra -Wpedantic -Iinclude src\vehicle.cpp src\queue_manager.cpp tests\queue_length_test.cpp -o queue_length_release_test.exe
```

Run:

```cmd
queue_length_release_test.exe
```

Verified result:

```text
Queue length tests passed.
```

All six Queue Manager test configurations above were built and executed successfully in the rebased PR #16 working tree on 24 October 2026.

---

## 4. Release-safe test checks

The Queue Manager test files use a `CHECK` mechanism rather than relying on standard `assert()` for test verification. This keeps test checks active when compiling with `-DNDEBUG`.

Both Debug and Release configurations should be tested when changing Queue Manager behavior.

---

## 5. Queue Manager API

The Queue Manager exposes adapter methods while retaining its existing queue API for backward compatibility.

The adapter methods are:

- `addVehicle(roadId, vehicle)` - delegates to `enqueue` and returns `true` if the operation completes normally. Allocation failures may propagate as exceptions.
- `dequeueVehicle(roadId, vehicle)` - delegates to `dequeue`, returning `false` if the road queue is missing or empty.
- `getQueueLength(roadId)` - delegates to `getSize` and returns the current queue length without modifying the queue.

The underlying queue operations are:

- `enqueue(roadId, vehicle)`
- `dequeue(roadId, vehicle)`
- `getSize(roadId)`

The SAD describes `addVehicle(vehicle)` and `dequeueVehicle(roadId)` at a conceptual interface level. The C++ adapter retains `roadId` for insertion and uses an output parameter for dequeueing to fit the existing per-road queue implementation.

Confirm any interface differences with the team or reviewer against the agreed SAD contract.

---

## 6. Responsibility boundary

Queue Manager manages FIFO queues and does not inspect traffic-signal state. Any requirement to allow vehicle processing only when a signal permits it must be enforced by the simulation or controller layer before requesting vehicle processing.

This documents the current implementation boundary; it does not claim that signal-permission behavior has been verified end to end.

---

## 7. Test coverage

The Queue Manager tests cover:

- Multiple independent road queues.
- FIFO queue behavior.
- Empty or nonexistent queue handling.
- Queue-length reporting.
- Checks that remain active in Release builds using `-DNDEBUG`.

These tests validate Queue Manager behavior in isolation. They do not establish that the complete traffic-management application has been integrated or that signal-permission behavior has been verified end to end.

---

## 8. Integration notes

- PR #16 uses the Vehicle module integrated through PR #15.
- Build the Queue Manager tests against the current integrated source files.
- Do not commit duplicate copies of `include/vehicle.h` or `src/vehicle.cpp` in PR #16.
- Re-run all six Queue Manager test configurations after changes to the Queue Manager implementation or its dependencies.
- Verify the integrated API against the agreed SAD interpretation during review.

---

## 9. CMake build and deterministic simulation tests

The active application backend uses C++17, CMake, and the optional Crow REST
server. The core-only configuration does not fetch Crow, Asio, or nlohmann-json.
No Qt installation or desktop build target is required.

### 9.1 Debug core tests

```powershell
cmake -S . -B build\debug -DBUILD_API_SERVER=OFF -DBUILD_TESTING=ON
cmake --build build\debug --config Debug
ctest --test-dir build\debug -C Debug --output-on-failure
```

### 9.2 Release core tests

```powershell
cmake -S . -B build\release -DBUILD_API_SERVER=OFF -DBUILD_TESTING=ON
cmake --build build\release --config Release
ctest --test-dir build\release -C Release --output-on-failure
```

#### Current web-backend verification - 9 November 2026

| Configuration | Build | CTest |
|---|---|---|
| Debug, `BUILD_API_SERVER=ON` | Passed | 12/12 passed |
| Release, `BUILD_API_SERVER=ON` | Passed | 12/12 passed |
| Debug, `BUILD_API_SERVER=OFF` | Passed | 10/10 passed |

These counts are recorded local execution results. The saved LastTest.log
files do not include a source commit ID or source hash, so they cannot prove
which exact source revision was built or whether every then-current uncommitted
change was included. They are not Issue #10 acceptance results. No tests were
rerun during Issue #12 documentation preparation. Treat the counts as
provenance-limited local records, not revision-bound acceptance evidence.

The active CMake configuration contains no Qt package lookup or desktop
application target.

### 9.3 TC-10 input validation and safe configuration handling

`input_validator_test` covers every configuration field's minimum and maximum
boundaries, missing values, malformed and non-integer values, out-of-range
values, field-specific diagnostic keys and error codes, and simultaneous
errors. It also verifies that invalid submissions preserve the active
configuration, backend rejection does not commit a candidate, and accepted
and rejected submission counts are correct.

`gui_model_test` retains its historical target name but exercises the
controller-to-model application path without Qt. It checks successful
application, invalid-input and model-rejection preservation, rejection while
Running and Paused, and acceptance after reset. Existing pause/resume and reset
regressions remain in the test.

The core-only Debug and Release commands above build and run all ten registered
core tests, including `input_validator_test` and `gui_model_test`.

#### Recorded verification - 6 November 2026

| Configuration | Result |
|---|---|
| Debug | All eight core test executables compiled with GCC 6.3.0 and passed. |
| Release | All eight core test executables compiled with GCC 6.3.0 and passed. |
| CMake/CTest through CMake Tools | Blocked: the CMake Tools configure action reported `Unable to configure the project`; no CTest run is claimed. |

The Debug and Release results above are direct GCC fallback runs using the
same core source and test lists registered in `CMakeLists.txt`; they are not
CMake/CTest results. The CMake commands in sections 9.1 and 9.2 remain the
reproducible CMake verification path.

For single-configuration generators, set `-DCMAKE_BUILD_TYPE=Debug` or
`Release` at configure time and omit `--config` and `-C` from the build and
CTest commands. The test checks remain active in Release builds.

### 9.4 Build and run the REST API backend

The API server is enabled by default. Its build and run commands, including
Debug, Release, and core-only configurations, are maintained in
[REST API documentation](docs/REST_API.md). Starting the server does not start
the simulation; use the documented start endpoint to begin ticks. The selected
browser frontend stack is React, TypeScript, Vite and Tailwind CSS. It remains
unimplemented; browser integration and acceptance checks are listed in the
Issue #12 demo-readiness checklist.

The legacy Qt prototype is retained at `legacy/qt-prototype/gui_main.cpp` for
historical reference; it is not a CMake target or the application frontend.

Configuration is accepted only while the simulation is stopped at logical
tick zero. Invalid values are rejected without replacing the last accepted
configuration. Accepted tick interval, signal durations, arrival interval, and
maximum per-approach queue length are applied to the service scheduler and model behavior.

Each accepted running tick increments the logical tick, processes existing
vehicles through the signal-gated `TrafficManager`, creates the scheduled
arrival if capacity permits, then advances the signal phase. Arrivals become
eligible for processing on the following tick. Arrival rotation is North,
South, East, West, repeating at the configured interval.

Reset drains every approach queue, removes queued vehicles from the
`VehicleManager`, clears statistics and arrival records, restores North GREEN,
and returns the simulation to Stopped. An explicit Start resumes ticks.

### 9.5 CMake test targets

CTest registers ten core tests, including `simulation_service_test` and
`simulation_control_api_test`, as well as `gui_model_test` (a retained legacy
name for core model regression coverage). With `BUILD_API_SERVER=ON`, CTest
also registers `simulation_http_server_test` and
`simulation_state_serializer_test`.

## 10. Issue #12 documentation and demo readiness

The target frontend stack is React, TypeScript, Vite and Tailwind CSS. This
selection does not mean the browser application exists. Frontend source,
browser/API integration, UI responsiveness, keyboard accessibility and
end-to-end acceptance remain pending.

Previously inspected local CTest records show Debug 12/12, Release 12/12, and
core-only Debug 10/10 passing. Their logs contain no source commit ID or source
hash and cannot conclusively tie all four uncommitted changes in the separate
Issue #9 worktree to those builds. They are not Issue #10 acceptance results.
No tests were rerun for this documentation work.

Issue #10 remains open and owned by another team member. Await its actual
revision-bound C++ regression results, frontend/browser integration, control
and configuration demonstrations, accessibility/responsiveness checks, and
end-to-end demo evidence. Do not mark Issue #12 complete from documentation
preparation alone. See the Issue #12 Demo Readiness checklist.
