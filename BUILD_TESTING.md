# Build and Test Instructions

## Requirements

- A C++17 compiler.
- CMake 3.16 or newer.
- Qt 6 Widgets to build and run the desktop GUI.
- Run commands from the repository root.

The project registers eight CTest targets:

1. `vehicle_manager_test`
2. `queue_manager_test`
3. `queue_empty_safety_test`
4. `queue_length_test`
5. `traffic_signal_test`
6. `traffic_manager_test`
7. `gui_model_test`
8. `input_validator_test`

The test targets link the shared `tms_core` library. GUI tests exercise the
simulation model directly and do not require a Qt event loop.

## Debug tests

Configure and build all eight tests without requiring Qt:

```powershell
cmake -S . -B build\debug -DBUILD_GUI=OFF -DBUILD_TESTING=ON
cmake --build build\debug --config Debug
ctest --test-dir build\debug -C Debug --output-on-failure
```

## Release tests

Configure and build all eight tests in Release mode:

```powershell
cmake -S . -B build\release -DBUILD_GUI=OFF -DBUILD_TESTING=ON
cmake --build build\release --config Release
ctest --test-dir build\release -C Release --output-on-failure
```

These commands use CMake's multi-configuration workflow (such as the Visual
Studio generator). For a single-configuration generator, use separate build
directories and set `-DCMAKE_BUILD_TYPE=Debug` or `-DCMAKE_BUILD_TYPE=Release`
at configure time; then omit `--config` and `-C` from the build and CTest
commands.

## Qt 6 GUI

Install Qt 6 with its Widgets component and make its CMake package available to
CMake (for example, by setting `CMAKE_PREFIX_PATH` to the Qt installation).
Then configure and build the GUI in Debug mode:

```powershell
cmake -S . -B build\gui -DBUILD_GUI=ON -DBUILD_TESTING=ON
cmake --build build\gui --config Debug
```

Run the `TrafficManagementSystem` executable from the generated build directory.
The GUI's default logical tick interval is 1000 ms. Configuration is accepted
only before the first simulation tick; invalid values are shown in the GUI and
do not replace the active configuration.

## Test coverage

The eight tests cover vehicle management, FIFO multi-road queues, empty-queue
safety, queue lengths, safe traffic-signal transitions, signal-gated traffic
processing, deterministic simulation timing and controls, and configuration
validation. The `gui_model_test` checks the North → South → East → West signal
cycle, GREEN/YELLOW durations, the default arrival schedule, queue capacity,
pause/resume behavior, reset behavior, and configuration changes.
