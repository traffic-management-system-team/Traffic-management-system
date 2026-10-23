# Build and Test Instructions

## Requirements

- A C++ compiler with C++17 support (`g++` recommended).
- Run the commands from the repository root.

The commands below use Windows Command Prompt syntax and produce `.exe` files.

## PR #15 — Vehicle Manager

Build:

```cmd
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src\vehicle.cpp src\vehicle_manager.cpp tests\vehicle_manager_test.cpp -o vehicle_manager_test.exe
```

Run:

```cmd
vehicle_manager_test.exe
```

Recorded local result:

```text
All VehicleManager tests passed.
```

## PR #16 — Queue Manager

**Dependency:** Queue Manager uses the `Vehicle` type provided by PR #15. The following commands require `include\vehicle.h` and `src\vehicle.cpp` to be present in the working tree. They were tested locally with those two files temporarily taken from the `dinesh/vehicle-manager-pr` branch. They should be rerun after the Vehicle module is available through the project's agreed integration workflow.

Build and run the main multi-road queue test:

```cmd
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src\vehicle.cpp src\queue_manager.cpp tests\queue_manager_test.cpp -o queue_manager_test.exe
queue_manager_test.exe
```

Recorded local result:

```text
All multi-road QueueManager tests passed.
```

Build and run the empty-queue safety test:

```cmd
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src\vehicle.cpp src\queue_manager.cpp tests\queue_empty_safety_test.cpp -o queue_empty_safety_test.exe
queue_empty_safety_test.exe
```

Recorded local result:

```text
TC-13 empty-queue safety passed.
```

Build and run the queue-length test:

```cmd
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src\vehicle.cpp src\queue_manager.cpp tests\queue_length_test.cpp -o queue_length_test.exe
queue_length_test.exe
```

Recorded local result:

```text
TC-06 queue length update passed.
```

## Scope and CI status

These are local build/test results, not GitHub Actions results. GitHub status checks have not been verified as configured. After PR #15 is merged or the Vehicle dependency is otherwise integrated, rerun all Queue Manager commands against that integrated codebase and update this document with the new results.
