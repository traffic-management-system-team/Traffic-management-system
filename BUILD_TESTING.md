# Build and Test Instructions

## Requirements

- C++17-compatible compiler (`g++` recommended).
- Run commands from the repository root in Windows CMD.

## PR #15 — Vehicle Manager

Build:

```cmd
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src\vehicle.cpp src\vehicle_manager.cpp tests\vehicle_manager_test.cpp -o vehicle_manager_test.exe
```

Run:

```cmd
vehicle_manager_test.exe
```

Expected output:

```text
All VehicleManager tests passed.
```

## PR #16 — Queue Manager

The Queue Manager depends on the `Vehicle` type supplied by PR #15.

### Multi-road Queue Manager test

Debug:

```cmd
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src\vehicle.cpp src\queue_manager.cpp tests\queue_manager_test.cpp -o queue_manager_test.exe
queue_manager_test.exe
```

Release:

```cmd
g++ -std=c++17 -DNDEBUG -O2 -Wall -Wextra -Wpedantic -Iinclude src\vehicle.cpp src\queue_manager.cpp tests\queue_manager_test.cpp -o queue_manager_test_release.exe
queue_manager_test_release.exe
```

### Empty-queue safety test

Debug:

```cmd
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src\vehicle.cpp src\queue_manager.cpp tests\queue_empty_safety_test.cpp -o queue_empty_safety_test.exe
queue_empty_safety_test.exe
```

Release:

```cmd
g++ -std=c++17 -DNDEBUG -O2 -Wall -Wextra -Wpedantic -Iinclude src\vehicle.cpp src\queue_manager.cpp tests\queue_empty_safety_test.cpp -o queue_empty_safety_test_release.exe
queue_empty_safety_test_release.exe
```

### Queue-length test

Debug:

```cmd
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src\vehicle.cpp src\queue_manager.cpp tests\queue_length_test.cpp -o queue_length_test.exe
queue_length_test.exe
```

Release:

```cmd
g++ -std=c++17 -DNDEBUG -O2 -Wall -Wextra -Wpedantic -Iinclude src\vehicle.cpp src\queue_manager.cpp tests\queue_length_test.cpp -o queue_length_test_release.exe
queue_length_test_release.exe
```

## Integration status

These commands verify the Vehicle Manager and Queue Manager modules together through the shared Vehicle implementation. They do not test a Simulation Controller or arrival scheduler, which are not implemented in these two PR branches.

Vehicle-arrival scheduling and controller-level vehicle-to-queue assignment still require integration work.
