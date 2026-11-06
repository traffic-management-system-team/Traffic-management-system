# Build and Test Instructions

## 1. Requirements

- A C++ compiler with C++17 support (`g++` recommended).
- CMake 3.16 or newer.
- Qt 6 Widgets development files to build and run the desktop GUI.
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

The project uses C++17, CMake, and Qt 6 Widgets. The simulation model and all
eight module tests can be built without Qt by disabling the GUI target.

### 9.1 Debug tests without Qt

```powershell
cmake -S . -B build\debug -DBUILD_GUI=OFF -DBUILD_TESTING=ON
cmake --build build\debug --config Debug
ctest --test-dir build\debug -C Debug --output-on-failure
```

### 9.2 Release tests without Qt

```powershell
cmake -S . -B build\release -DBUILD_GUI=OFF -DBUILD_TESTING=ON
cmake --build build\release --config Release
ctest --test-dir build\release -C Release --output-on-failure
```

### 9.3 TC-10 input validation and safe configuration handling

`input_validator_test` covers every configuration field's minimum and maximum
boundaries, missing values, malformed and non-integer values, out-of-range
values, field-specific diagnostic keys and error codes, and simultaneous
errors. It also verifies that invalid submissions preserve the active
configuration, backend rejection does not commit a candidate, and accepted
and rejected submission counts are correct.

`gui_model_test` exercises the controller-to-model application path. It checks
successful application, invalid-input and model-rejection preservation,
rejection while Running and Paused, and acceptance after reset. Existing
pause/resume and reset regressions remain in the test.

The Qt-independent Debug and Release commands above build and run all eight
registered core tests, including `input_validator_test` and `gui_model_test`.

#### Recorded verification — 6 November 2026

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

### 9.4 Build and launch the Qt 6 application

Install a C++17 compiler, CMake, and Qt 6 Widgets development files. If Qt is
not in the default CMake package search path, set `CMAKE_PREFIX_PATH` to the
Qt installation prefix.

```powershell
cmake -S . -B build\qt -DBUILD_GUI=ON -DBUILD_TESTING=ON -DCMAKE_PREFIX_PATH="<Qt 6 prefix>"
cmake --build build\qt --config Debug
.\build\qt\Debug\TrafficManagementSystem.exe
```

The timer interval defaults to 1000 ms and can be configured. GREEN defaults
to 10 logical ticks, YELLOW to 2, and arrivals to every 3 ticks, with the first
arrival on North at tick 3. Model tests advance logical ticks without sleeping.

Configuration is accepted only while the simulation is stopped at logical
tick zero. Invalid values are rejected without replacing the last accepted
configuration. Accepted tick interval, signal durations, arrival interval, and
maximum per-approach queue length are applied to timer and model behavior.

Each accepted running tick increments the logical tick, processes existing
vehicles through the signal-gated `TrafficManager`, creates the scheduled
arrival if capacity permits, then advances the signal phase. Arrivals become
eligible for processing on the following tick. Arrival rotation is North,
South, East, West, repeating at the configured interval.

Reset drains every approach queue, removes queued vehicles from the
`VehicleManager`, clears statistics and arrival records, restores North GREEN,
and returns the simulation to Stopped. An explicit Start resumes ticks.

### 9.5 CMake test targets

CTest registers `vehicle_manager_test`, `queue_manager_test`,
`queue_empty_safety_test`, `queue_length_test`, `traffic_signal_test`,
`traffic_manager_test`, `gui_model_test`, and `input_validator_test`.
