# Current Web Architecture

## Status and scope

The selected target architecture is a React, TypeScript, Vite and Tailwind CSS
browser frontend communicating with the C++17 backend over JSON REST APIs. The
repository contains a typed API client and a responsive browser dashboard with
four signal/queue approaches, FIFO vehicle details, aggregate statistics,
lifecycle controls and a configuration form. The app consumes authoritative
backend snapshots and polls at a state-dependent interval.

Automated Vitest component/hook tests cover dashboard rendering, snapshot
refreshes, FIFO display, signal labels, controls, keyboard form submission and
adaptive polling. A full cross-browser Playwright end-to-end suite and manual
accessibility audit are deployment-level validation still to be performed;
they are not missing dashboard implementation.

The GUI-based Part-1 SRS, SAD, test plan, sprint records, and UML diagrams are
historical design artifacts for the earlier desktop prototype. They remain
available for project history, but do not describe the current target
architecture; see [Design Deliverables](../Design_README.md). The Qt prototype
source is archived under `legacy/qt-prototype/` and is not part of active
builds. The `include/gui_model.h` compatibility header and `gui_model_test`
target remain for existing model-source compatibility and core regression
coverage.

## Runtime layers

```text
React + TypeScript + Vite + Tailwind frontend
    | same-origin /api requests (Vite proxy in development)
    v
Crow HTTP host
    |-- control/configuration requests --> SimulationHttpAdapter
    |                                         -> SimulationControlApi
    |                                         -> SimulationService
    |
    `-- state request ----------------------> SimulationService snapshot
                                              -> state serializer
                                              -> common JSON envelope
                                                    |
                                                    v
                                                 tms_core
```

- **Frontend dashboard (implemented):** `frontend/` contains the
  React/TypeScript/Vite/Tailwind application, typed API client,
  connection/loading/error shell, four-approach intersection dashboard,
  signal indicators, FIFO vehicle queues, aggregate generated/processed/waiting
  and average-wait metrics, per-approach activity, lifecycle controls and
  configuration form. It fetches `/api/simulation/state`, uses backend command
  responses, and polls the authoritative snapshot every second while running,
  every five seconds while paused, and every ten seconds while stopped.
  Responsive CSS breakpoints, focus-visible styles, descriptive signal labels,
  keyboard form submission and reduced-motion support are implemented.
  Vitest component/hook tests cover these behaviors; full cross-browser E2E and
  manual accessibility audits remain deployment-level evidence.
- **HTTP host and adapter:** One Crow server registers the six approved endpoints. The adapter translates control/configuration HTTP requests into the existing framework-neutral `SimulationControlApi` calls. The state route serializes a copied snapshot from the same service.
- **Service layer:** One shared `SimulationService` is the authoritative simulation owner and sole scheduler/tick source. It synchronizes commands, configuration, ticks, and snapshots.
- **Simulation core:** `tms_core` contains the simulation model, validation, configuration, statistics aggregation and control API logic. It has no Crow or nlohmann-json dependency.
- **State serializer:** The separate `tms_state_serializer` target uses nlohmann-json and accepts a `const SimulationSnapshot&`. It returns state data only; the HTTP layer supplies the common response envelope.

Starting the HTTP host does not start or reset the simulation. The start
endpoint begins simulation ticks. The HTTP server is stopped and joined before
the shared service is destroyed.

## Browser deployment

For the initial deployment, serve the browser frontend and REST API through the
same public origin. A reverse proxy should serve the frontend and route
requests under `/api/` to the Crow backend. Browser clients call paths such as
`/api/simulation/state` on that origin; the proxy forwards those API requests
to the backend.

During Vite development, `frontend/vite.config.ts` proxies `/api` to the
server-side `API_PROXY_TARGET`, defaulting to `http://127.0.0.1:18080`. The
proxy keeps `changeOrigin` disabled so the backend's host/origin validation
sees the browser-facing host and origin. Configure the target in
`frontend/.env.local` (see [frontend setup](../frontend/README.md)); it is not
exposed as a `VITE_` client variable. Do not enable permissive CORS as a
workaround.

The backend rejects cross-origin browser requests. Production deployment must
therefore use a same-origin reverse proxy or another explicitly approved
arrangement.

## REST boundary

The API provides:

| Method | Path | Responsibility |
|---|---|---|
| GET | `/api/simulation/state` | Read a consistent simulation snapshot |
| POST | `/api/simulation/start` | Start a stopped simulation |
| POST | `/api/simulation/pause` | Pause a running simulation |
| POST | `/api/simulation/resume` | Resume a paused simulation |
| POST | `/api/simulation/reset` | Reset simulation while retaining accepted configuration |
| PUT | `/api/simulation/config` | Replace simulation configuration |

Issue #27 owns read-only state serialization; Issue #28 owns control and
configuration behavior; Issue #9 integrates the HTTP host and application
lifecycle. Details, response formats, and build commands are in the [REST API
documentation](REST_API.md).

## Build boundaries

The API server is enabled by `BUILD_API_SERVER` and is independent of Qt.
Configuring with `BUILD_API_SERVER=OFF` builds and tests the core without Crow,
Asio, or nlohmann-json. Qt is not an active build dependency or target.
Frontend setup is separate from CMake and documented in
[frontend/README.md](../frontend/README.md). See [build and test
instructions](../BUILD_TESTING.md).
