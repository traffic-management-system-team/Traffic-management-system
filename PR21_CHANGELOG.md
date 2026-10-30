# PR #21 Changelog

## Changes relative to the latest `main`

- Added the CMake build for the shared traffic-management core, optional Qt 6
  Widgets application, and eight registered CTest targets.
- Integrated PR #20's GUI-facing simulation model and deterministic signal,
  arrival, queue, pause, and reset behavior.
- Added `SimConfig`, `InputValidator`, and `ConfigController` for validated
  configuration values and feedback; connected accepted values to the model
  and GUI timer.
- Enforced configured per-approach queue capacity in actual vehicle-arrival
  processing, with invalid or running-state configuration changes rejected.
- Updated the traffic-signal integration notes to document configurable timing
  defaults and reset behavior.
- Replaced outdated test instructions with accurate CMake Debug/Release
  commands and the complete list of eight test targets.
- Added a direct pause/resume regression test that confirms ticks remain
  stopped while paused, resume from the preserved phase, and transition on the
  expected logical tick.
