# Traffic Management System

A browser-based traffic intersection simulation built with a C++17 simulation backend and a React, TypeScript, Vite and Tailwind CSS frontend.

The backend owns the simulation state and exposes JSON REST endpoints. The frontend renders live intersection state and sends lifecycle/configuration commands through the API.

## Current implementation

- Four-way intersection with North, East, South and West approaches.
- Traffic signal phases and signal-gated vehicle processing.
- Per-approach FIFO vehicle queues and queue-capacity handling.
- Vehicle arrival, processing and waiting statistics.
- Simulation lifecycle controls: Start, Pause, Resume and Reset.
- Configuration form with backend validation and preservation of the last accepted configuration after rejected updates.
- Crow-based REST API with state snapshots, lifecycle routes and configuration route.
- Responsive browser dashboard with accessible signal labels, keyboard-operable controls and adaptive polling.

The backend is organized around a shared SimulationService. The tms_core simulation library remains independent of Crow and the JSON serializer. The browser communicates with the API through same-origin /api requests.

## Technology

| Layer | Technology |
|---|---|
| Simulation core | C++17 |
| Build system | CMake |
| HTTP API | Crow / Standalone Asio |
| State serialization | nlohmann-json (separate serializer target) |
| Frontend | React, TypeScript, Vite, Tailwind CSS |
| Frontend tests | Vitest |
| Browser acceptance | Playwright / Chromium |
| CI | GitHub Actions |

## Build and run

See [REST API documentation](docs/REST_API.md) for backend dependencies, CMake configuration, server startup and deployment-boundary guidance.

For the browser app, follow [frontend setup](frontend/README.md). In brief:

1. Build and start the C++ API server.
2. Install frontend dependencies with npm ci from frontend/.
3. Configure API_PROXY_TARGET in .env.local if the API is not at its default local address.
4. Start the Vite development server with npm run dev.

The server starts with the simulation stopped. Use the browser controls or API Start route to begin simulation ticks.

## Verification status

The Issue #10 regression and browser-acceptance evidence was reviewed and merged through [PR #51](https://github.com/traffic-management-system-team/Traffic-management-system/pull/51).

Recorded results on the reviewed PR source:
- Windows MSVC Release build succeeded; 15/15 CTest targets passed from a clean checkout.
- Frontend type-check passed.
- Vitest: 38/38 tests passed.
- Production frontend build passed.
- Playwright: 3/3 Chromium tests passed against the real C++ REST backend.
- Both GitHub Actions workflows completed successfully, with 8 jobs passing.

The automated browser checks cover lifecycle controls, configuration validation and error feedback, live state/statistics, responsive widths (390, 768 and 1440 px), and keyboard skip-link behavior. A full manual screen-reader/WCAG audit, broader cross-browser matrix, and deployment-environment smoke test are not claimed by these results.

## Project documentation

- [Current architecture](docs/Architecture.md)
- [REST API and backend build/run instructions](docs/REST_API.md)
- [Frontend setup, tests and browser acceptance](frontend/README.md)
- [Build and test instructions](BUILD_TESTING.md)
- [Issue #10 regression and web acceptance report](docs/Issue_10_CPP_Regression_Report.md)
- [Issue #12 demo-readiness checklist](docs/Issue_12_Demo_Readiness.md)
- [Security and reliability validation](docs/Security_Reliability_Validation.md)

## Part-1 design records

The Part-1 SRS, SAD, Test Plan, sprint backlog, work-distribution documents and original UML diagrams are retained as historical project/design deliverables. They describe the earlier GUI-oriented desktop prototype and should be read alongside—not as a replacement for—the current web architecture. See [Design Deliverables](Design_README.md).

The legacy Qt prototype is archived under legacy/qt-prototype/; it is not the active application frontend.
