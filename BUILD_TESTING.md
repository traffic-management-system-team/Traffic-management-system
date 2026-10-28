# Build and Test Instructions

## 1. Requirements

- A C++ compiler with C++17 support (`g++` recommended).
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

## 9. PR #8 - InputValidator and configuration handling (TC-10)

`InputValidator` checks user-entered simulation configuration text before it reaches any simulation module. `ConfigController::submit` is the entry point: it replaces the active `SimConfig` only when every field is valid, and otherwise returns clear feedback and leaves the active config unchanged. Parsing stays separate from core simulation logic.

### 9.1 TC-10 - Debug

Build:

```cmd
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src\input_validator.cpp src\config_controller.cpp tests\input_validator_test.cpp -o input_validator_test.exe
```

Run:

```cmd
input_validator_test.exe
```

Verified result:

```text
TC-10 input validation passed.
```

### 9.2 TC-10 - Release

Build:

```cmd
g++ -std=c++17 -DNDEBUG -O2 -Wall -Wextra -Wpedantic -Iinclude src\input_validator.cpp src\config_controller.cpp tests\input_validator_test.cpp -o input_validator_release_test.exe
```

Run:

```cmd
input_validator_release_test.exe
```

Verified result:

```text
TC-10 input validation passed.
```

### 9.3 Coverage

- Valid values become a structured `SimConfig`.
- Boundary values: minimum and maximum accepted, one outside each rejected.
- Malformed input rejected without crashing: empty, blank, letters, decimals, exponents, signs only, hex, overflow, control characters, very long strings.
- Every invalid field reported with its own message.
- A rejected submission leaves the active configuration unchanged.

These tests validate the input path in isolation. They do not claim the GUI or simulation modules have been wired to `ConfigController` end to end.

### 9.4 Approved defaults and build integration

- Approved defaults verified by the TC-10 tests: tick interval 1000 ms, GREEN 10 logical ticks, YELLOW 2 logical ticks, vehicle arrival interval 3 logical ticks.
- Invalid submissions never overwrite the active valid configuration; this is covered by the last-valid-configuration checks in the TC-10 tests.
- CMake registration of `src/input_validator.cpp`, `src/config_controller.cpp` and `tests/input_validator_test.cpp` is deferred to the PR #20 integration. This PR adds no build system.
