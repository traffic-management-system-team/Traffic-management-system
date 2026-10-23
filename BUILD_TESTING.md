# Build and Test Instructions

## 1. Requirements

- A C++ compiler with C++17 support (`g++` recommended).
- Windows Command Prompt (CMD).
- Run all commands from the repository root.
- The Queue Manager tests require the `Vehicle` module from PR #15.

The commands below use Windows CMD syntax and generate `.exe` files in the repository root.

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

### Recorded local result

```text
All VehicleManager tests passed.
```

This is a recorded local result. Rerun the command against the latest integrated code before treating it as a fresh verification.

---

## 3. PR #16 - Queue Manager

### Dependency

Queue Manager depends on the `Vehicle` type and implementation supplied by PR #15.

The commands below require these files to exist in the working tree:

- `include\vehicle.h`
- `src\vehicle.cpp`

The Queue Manager tests were previously run locally with these two dependency files temporarily taken from the `dinesh/vehicle-manager-pr` branch. They are dependency files for local testing and must not be committed into PR #16 as duplicate copies.

After PR #15 is merged or the team agrees on another integration method, rerun these tests against the integrated code.

### 3.1 Multi-road queue test - Debug

Build:

```cmd
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src\vehicle.cpp src\queue_manager.cpp tests\queue_manager_test.cpp -o queue_manager_test.exe
```

Run:

```cmd
queue_manager_test.exe
```

Recorded result:

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

Recorded result: passed in the previous local test run.

### 3.3 Empty-queue safety test - Debug

Build:

```cmd
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src\vehicle.cpp src\queue_manager.cpp tests\queue_empty_safety_test.cpp -o queue_empty_safety_test.exe
```

Run:

```cmd
queue_empty_safety_test.exe
```

Recorded result:

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

Recorded result: passed in the previous local test run.

### 3.5 Queue-length test - Debug

Build:

```cmd
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src\vehicle.cpp src\queue_manager.cpp tests\queue_length_test.cpp -o queue_length_test.exe
```

Run:

```cmd
queue_length_test.exe
```

Recorded result:

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

Recorded result: passed in the previous local test run.

### Release-safe assertions

The Queue Manager test files use a `CHECK` mechanism rather than relying on standard `assert()` for test verification. This ensures that the checks remain active when compiling with `-DNDEBUG`.

---

## 4. SAD API Compatibility and Scope

The Queue Manager exposes the following adapter methods while retaining the existing API for backward compatibility:

| Adapter method | Behavior |
|---|---|
| `addVehicle(roadId, vehicle)` | Delegates to `enqueue` and returns `true` if the operation completes normally. Allocation failures may propagate as exceptions. |
| `dequeueVehicle(roadId, vehicle)` | Delegates to `dequeue`, returning `false` if the road queue is missing or empty. |
| `getQueueLength(roadId)` | Delegates to `getSize` and returns the current queue length without modifying the queue. |

The existing methods remain available:

- `enqueue(roadId, vehicle)`
- `dequeue(roadId, vehicle)`
- `isEmpty(roadId)`
- `getSize(roadId)`

The SAD describes `addVehicle(vehicle)` and `dequeueVehicle(roadId)` at a conceptual interface level. The C++ adapter retains `roadId` for insertion and uses an output parameter for dequeueing to fit the existing per-road queue implementation.

### Traffic-signal permission boundary

Queue Manager manages FIFO queues and does not inspect traffic-signal state. Any requirement to allow vehicle processing only when a signal permits it must be enforced by the simulation/controller layer before requesting vehicle processing.

This responsibility split must be confirmed with the team/reviewer against the SAD contract. It is documented here as the current implementation boundary, not as an already approved deviation.

---

## 5. Test Scope

The local Queue Manager tests cover:

- Multiple independent road queues.
- FIFO insertion and removal.
- Empty or nonexistent queue handling.
- Queue-length reporting.
- The SAD-compatible adapter methods.
- Checks that remain active in Release builds using `-DNDEBUG`.

These tests validate Queue Manager behavior in isolation. They do not establish that the complete traffic-management application has been integrated or that signal-permission behavior has been verified end to end.

---

## 6. Integration and CI Status

The recorded results above are local compiler/test results, not GitHub Actions results.

PR #16 depends on the `Vehicle` module in PR #15. The dependency is not resolved merely by temporarily copying the Vehicle files into the local working tree.

After the team integrates the Vehicle module through the agreed workflow:

1. Run all six Queue Manager Debug and Release build/test commands again.
2. Record the fresh outputs and any compiler warnings or errors.
3. Verify the integrated API against the agreed SAD interpretation.
4. Check GitHub status checks separately; do not assume CI has run or passed unless the repository confirms it.
