# Current Web Architecture

## Status and scope

The selected target architecture is a React, TypeScript, Vite and Tailwind CSS
browser frontend communicating with the C++17 backend over JSON REST APIs.
This repository implements the simulation core and Crow API host; it does
**not** contain frontend source or browser functionality. React, TypeScript,
Vite and Tailwind CSS are selected technologies, not implemented components.
Frontend integration and acceptance testing remain pending.

The GUI-based Part-1 SRS, SAD, test plan, sprint records, and UML diagrams are
historical design artifacts for the earlier desktop prototype. They remain
available for project history, but do not describe the current target
architecture; see [Design Deliverables](../Design_README.md). The Qt prototype
source is archived under `legacy/qt-prototype/` and is not part of active builds.
The `include/gui_model.h` compatibility header and `gui_model_test` target remain
for existing model-source compatibility and core regression coverage.

## Runtime layers

```text
Browser frontend (pending; separate component)
    | HTTP + JSON
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

- **Frontend (selected, pending):** React, TypeScript, Vite and Tailwind CSS
  are the target stack. The browser dashboard, controls, configuration form,
  accessible interactions and API integration are not implemented here.
- **HTTP host and adapter:** One Crow server registers the six approved
  endpoints. The adapter translates control/configuration HTTP requests into
  the existing framework-neutral `SimulationControlApi` calls. The state route
  serializes a copied snapshot from the same service.
- **Service layer:** One shared `SimulationService` is the authoritative
  simulation owner and sole scheduler/tick source. It synchronizes commands,
  configuration, ticks, and snapshots.
- **Simulation core:** `tms_core` contains the simulation model, validation,
  configuration, and control API logic. It has no Crow or nlohmann-json
  dependency.
- **State serializer:** The separate `tms_state_serializer` target uses
  nlohmann-json and accepts a `const SimulationSnapshot&`. It returns state
  data only; the HTTP layer supplies the common response envelope.

Starting the HTTP host does not start or reset the simulation. The start
endpoint begins simulation ticks. The HTTP server is stopped and joined before
the shared service is destroyed.

## Browser deployment

For the initial deployment, serve the browser frontend and REST API through the
same public origin. A reverse proxy should serve the frontend and route requests
under `/api/` to the Crow backend. Browser clients call paths such as
`/api/simulation/state` on that origin; the proxy forwards those API requests
to the backend. The selected React, TypeScript, Vite and Tailwind CSS frontend
remains pending implementation and browser verification.

The backend rejects cross-origin browser requests. Hosting the frontend and
backend on separate origins therefore requires an explicitly approved CORS
policy or another approved deployment arrangement, such as a same-origin
reverse proxy. Do not enable permissive CORS as a workaround.

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
lifecycle. Details, response formats, and build commands are in the
[REST API documentation](REST_API.md).

## Build boundaries

The API server is enabled by `BUILD_API_SERVER` and is independent of Qt.
Configuring with `BUILD_API_SERVER=OFF` builds and tests the core without Crow,
Asio, or nlohmann-json. Qt is not an active build dependency or target.
See [build and test instructions](../BUILD_TESTING.md).
