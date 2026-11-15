# Build and Test Instructions

This guide describes the active C++17 backend and React/TypeScript frontend. The archived Qt prototype is not part of the current build.

## 1. Prerequisites

- C++17 compiler.
- CMake 3.16 or newer.
- Git and network access to fetch the pinned Crow, Asio and JSON dependencies when building the API server.
- Node.js 22.12 or newer and npm 10 or newer for the frontend.
- Windows examples use PowerShell and the Visual Studio CMake generator. Equivalent CMake commands work with supported generators on other platforms.

Run backend commands from the repository root.

## 2. Configure, build and test the C++ backend

### Windows — API-enabled Release build

    cmake -S . -B build/release -G "Visual Studio 18 2026" -A x64 -DBUILD_API_SERVER=ON -DBUILD_TESTING=ON -DCMAKE_CONFIGURATION_TYPES=Release
    cmake --build build/release --config Release --parallel 2
    ctest --test-dir build/release -C Release --output-on-failure

### Windows — API-enabled Debug build

    cmake -S . -B build/debug -G "Visual Studio 18 2026" -A x64 -DBUILD_API_SERVER=ON -DBUILD_TESTING=ON -DCMAKE_CONFIGURATION_TYPES=Debug
    cmake --build build/debug --config Debug --parallel 2
    ctest --test-dir build/debug -C Debug --output-on-failure

### Core-only build (without the HTTP server)

Use BUILD_API_SERVER=OFF to build and test the framework-neutral simulation core without fetching or linking the Crow/Asio HTTP dependencies:

    cmake -S . -B build/core-only -DBUILD_API_SERVER=OFF -DBUILD_TESTING=ON
    cmake --build build/core-only --config Release
    ctest --test-dir build/core-only -C Release --output-on-failure

For a single-configuration generator, set CMAKE_BUILD_TYPE=Debug or Release during configuration and omit --config and -C from build and CTest commands.

## 3. Start the C++ REST API

After a successful Windows Release build:

    .\build\release\Release\tms_api_server.exe

The default listener is http://127.0.0.1:18080. Starting the HTTP server does not start simulation ticks; use the Start endpoint or browser control to start the simulation.

The server has no built-in authentication or TLS. Keep it loopback-only for local development. Do not bind it to a public/untrusted interface without an appropriately secured deployment boundary, firewall restrictions and TLS-terminating reverse proxy. See [REST API security and deployment notes](docs/REST_API.md).

## 4. Build and test the frontend

Open a second terminal:

    cd frontend
    npm ci
    npm run type-check
    npm test
    npm run build

To launch the development frontend:

    Copy-Item .env.example .env.local
    npm run dev

Edit API_PROXY_TARGET in .env.local only if the backend is not at http://127.0.0.1:18080. Vite proxies same-origin /api/* requests to the backend. Keep the API target in the Node-side API_PROXY_TARGET setting; do not expose it through a VITE_ variable.

## 5. Run real-backend browser acceptance

Keep the C++ REST API running. From frontend/:

    npx playwright install chromium
    $env:API_PROXY_TARGET = "http://127.0.0.1:18080"
    npm run test:e2e

The Playwright suite uses Chromium and sends requests through the Vite same-origin proxy to the real C++ API; it does not mock the API routes.

## 6. Recorded Issue #10 verification

The reviewed Issue #10 evidence was merged through [PR #51](https://github.com/traffic-management-system-team/Traffic-management-system/pull/51).

| Verification | Recorded result |
|---|---|
| Windows MSVC 19.52.37029 API-enabled Release build | Passed |
| CTest Release suite | 15/15 passed from a clean checkout of source commit 2eadfbe1797c8394c9253423726eac712b0fa1a6 |
| Frontend type-check | Passed |
| Vitest | 38/38 passed across four test files |
| Vite production build | Passed |
| Playwright real-backend Chromium suite | 3/3 passed |
| GitHub Actions | Both workflows passed; 8 jobs successful on the reviewed PR source |

The clean-checkout C++ rerun is tied to PR source commit 2eadfbe. The report transparently distinguishes the earlier local browser run (which initially used an uncommitted test-file state) from the later committed test source and CI evidence. Do not describe the 15/15 local Windows rerun as having been executed on the later merge commit unless a new run is performed on that commit.

MSVC emitted warnings in fetched Crow dependency headers; the build and tests completed successfully. Automated browser evidence targets Chromium and the specified responsive widths. It is not a full manual screen-reader/WCAG audit or a cross-browser deployment certification.

## 7. Test targets

With BUILD_API_SERVER=ON, the 15 registered CTest targets in the recorded Issue #10 Release run were:

- vehicle_manager_test
- queue_manager_test
- queue_empty_safety_test
- queue_length_test
- traffic_signal_test
- traffic_manager_test
- gui_model_test (historical target name; core model regression, not a Qt GUI)
- input_validator_test
- security_reliability_test
- simulation_service_test
- simulation_statistics_manager_test
- simulation_control_api_test
- simulation_http_server_test
- simulation_state_serializer_test
- simulation_api_contract_test

The frontend test suite covers API client behavior, backend status/polling, dashboard components and application interactions. The browser suite adds end-to-end coverage against the real backend.

## 8. Related documentation

- [REST API](docs/REST_API.md)
- [Frontend setup and test commands](frontend/README.md)
- [Architecture](docs/Architecture.md)
- [Issue #10 regression report](docs/Issue_10_CPP_Regression_Report.md)
- [Issue #12 demo-readiness checklist](docs/Issue_12_Demo_Readiness.md)
