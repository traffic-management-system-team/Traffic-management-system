# Issue #28 implementation handoff

## Delivered in this branch package

- Framework-neutral `SimulationControlApi` handler module.
- Five agreed method/path handlers and common JSON response envelopes.
- Lowercase state responses and idempotent reset with retained configuration.
- Strict integer-only handling for the five configuration fields.
- Delegation to `SimulationService::setConfig()` and the existing validator/controller path.
- Field-level validation diagnostics forwarded from `ValidationIssue` where available.
- CTest coverage for transitions, reset, configuration success/rejection, malformed JSON signal, missing/extra fields, and unknown routes.
- API contract updated with the host-adapter handoff interface.

## Boundary with Issue #9

This module does not start a server, register Crow routes, parse raw JSON bytes, or select/pin Crow. The Issue #9 HTTP adapter must parse JSON, preserve JSON scalar types when populating `ApiRequest::objectBody`, set `jsonValid=false` for malformed JSON, invoke `SimulationControlApi::handle`, and forward the returned status/content-type/body. This keeps HTTP host lifecycle and framework dependencies outside Issue #28 as agreed.

## Build verification performed

Verified locally on Windows using MSYS2 UCRT64:

- CMake 4.4.5
- GNU C++ 16.2.0
- C++17
- BUILD_GUI=OFF
- BUILD_TESTING=ON
- Build: successful
- CTest: 10/10 tests passed

The project has not been verified here with Windows MSVC/Clang-CL, and the Crow host integration is not included in this Issue #28 implementation.