# REST API server

## Architecture

The Crow REST host is the C++ backend for the web application and is a separate
CMake target from `tms_core`. One `SimulationService` is constructed in
`main()` and shared with the host for both state snapshots and control
operations:

```text
Crow HTTP server
    -> SimulationHttpAdapter
        -> SimulationControlApi
            -> SimulationService
                -> one SimModel and its service-owned scheduler
```

`SimulationHttpAdapter` only translates the HTTP/JSON boundary for the five
control/configuration routes. It does not access `SimModel`, validate
configuration values, mutate simulation state, or advance ticks. It constructs
the existing `ApiRequest`, invokes `SimulationControlApi::handle()`, then
forwards the returned status code, content type, and response body without
re-encoding them. The state route obtains a consistent copied snapshot from the
same service and passes it to `serializeSimulationStateData`; a shared HTTP
response helper wraps the resulting data in the common envelope.

The `SimulationService` remains the sole production simulation owner and tick
source. Its mutex continues to serialize HTTP commands and configuration
updates with scheduler ticks and snapshots. Crow uses its own worker pool for
requests; this does not add a simulation scheduler.

The server defaults to `127.0.0.1:18080`. The standalone executable
accepts an explicit bind address through `TMS_HTTP_BIND_ADDRESS`; when unset,
it remains loopback-only. The value must be a numeric IPv4 or IPv6 address and
is validated before the worker starts. For example, on PowerShell:

```powershell
$env:TMS_HTTP_BIND_ADDRESS = "192.0.2.10"
.\build\issue9-rest-debug\Debug\tms_api_server.exe
```

Use an address assigned to the host. Setting `0.0.0.0` binds all IPv4
interfaces; an IPv6 wildcard address similarly broadens exposure. The
executable prints the actual listener address and emits a warning when the
address is not loopback. This is not authentication: the API has no built-in
authentication or TLS. Do not expose it to an untrusted network; restrict
access with host firewall rules and a properly configured TLS-terminating
reverse proxy.

The server owns the Crow worker and waits for the listener to bind before
reporting startup success. Startup failures are returned to the executable;
HTTP responses never include internal exception details. On Ctrl+C or
Ctrl+Break, Crow stops accepting requests and the executable joins the server
worker. If the server is stopped explicitly, call
`SimulationHttpServer::stop()` and confirm it returns true before destroying
the service. The service must outlive the HTTP server.

The simulation remains stopped when the HTTP server starts. Only the start
route begins ticks. The HTTP server is stopped and joined before its shared
`SimulationService` is destroyed; stopping the server does not reset or stop
the simulation.

Unexpected failures at the HTTP adapter, state serialization, server worker,
and service scheduler boundaries return generic client errors where an HTTP
response is possible. Runtime diagnostics use fixed messages rather than
printing exception text, which can contain implementation details.

## Dependencies and CMake

The HTTP target fetches these exact upstream commits with CMake
`FetchContent`:

| Dependency | Version | Pinned commit |
|---|---|---|
| [Crow](https://github.com/CrowCpp/Crow/releases/tag/v1.3.5) | 1.3.5 | `fcf0c360b15fc245d884c71340b08ac20403ba36` |
| [Standalone Asio](https://github.com/chriskohlhoff/asio/releases/tag/asio-1-36-0) | 1.36.0 | `231cb29bab30f82712fcd54faaea42424cc6e710` |
| [nlohmann-json](https://github.com/nlohmann/json/releases/tag/v3.12.0) | 3.12.0 | `55f93686c01528224f448c19128836e7df245f72` |

Crow parses incoming HTTP request JSON. The separate Issue #27
`tms_state_serializer` target uses nlohmann-json to serialize a supplied
`SimulationSnapshot`; it is linked to the HTTP host, but not to `tms_core`. Crow
examples, upstream tests, installation, Boost, TLS, and compression are disabled
for the HTTP-only target. A small,
version-controlled patch to Crow's HTTP parser limits request-body storage to
64 KiB before the adapter parses JSON. Oversized bodies receive HTTP 413 with
the standard JSON error envelope. CMake applies the patch to the pinned Crow
source; configuration therefore needs access to GitHub to fetch the pinned
sources.

`BUILD_API_SERVER=OFF` skips Crow and Asio entirely, leaving `tms_core` and its
tests buildable without the HTTP dependencies. Qt is not an active dependency
or build target. The React, TypeScript, Vite and Tailwind CSS frontend scaffold
is maintained separately in `frontend/`; the full browser dashboard and
controls are still pending.

## Browser deployment

The recommended initial deployment serves the browser frontend and REST backend
through the same public origin. Put a reverse proxy in front of both: serve the
frontend from that origin and route `/api/` requests to the Crow backend. The
browser calls the existing API endpoints on the shared origin. The
React/TypeScript/Vite scaffold and typed API client are implemented. The
complete dashboard, simulation controls, and remaining browser acceptance work
are pending under subsequent issues.

The backend rejects cross-origin browser requests, and this policy is not
weakened here. For a TLS-terminating reverse proxy, set the backend process
variable before startup, for example in PowerShell:

```powershell
$env:TMS_PUBLIC_ORIGIN = "https://traffic.example.com"
.\build\issue9-rest-debug\Debug\tms_api_server.exe
```

Set it to the browser-facing origin, including scheme, host, and any
non-default port. The backend compares the request `Origin` to this configured
value; forwarded headers are not used to derive the expected origin. Invalid
values (including paths or malformed ports) prevent server startup. If unset,
local development retains the default behavior of matching `Origin` to the
request host over HTTP. Requests without `Origin` retain their existing
behavior, subject to the `Sec-Fetch-Site` check.

Separate-origin frontend hosting requires an explicitly approved CORS policy or
another approved deployment arrangement, such as the same-origin reverse proxy
described above. Do not enable permissive CORS without approval.

## Build and run on Windows

Use a Visual Studio Developer PowerShell/Command Prompt with CMake and Git on
`PATH`. The following uses the CMake-selected Visual Studio generator. No Qt
installation or GUI build option is required:

```powershell
cmake -S . -B build\issue9-rest-debug -G "Visual Studio 18 2026" -A x64 `
  -DBUILD_API_SERVER=ON -DBUILD_TESTING=ON `
  -DCMAKE_CONFIGURATION_TYPES=Debug
cmake --build build\issue9-rest-debug --config Debug
ctest --test-dir build\issue9-rest-debug -C Debug --output-on-failure

cmake -S . -B build\issue9-rest-release -G "Visual Studio 18 2026" -A x64 `
  -DBUILD_API_SERVER=ON -DBUILD_TESTING=ON `
  -DCMAKE_CONFIGURATION_TYPES=Release
cmake --build build\issue9-rest-release --config Release
ctest --test-dir build\issue9-rest-release -C Release --output-on-failure
```

Run the API server from PowerShell:

```powershell
.\build\issue9-rest-debug\Debug\tms_api_server.exe
```

The server listens on `http://127.0.0.1:18080` by default. To bind to a
specific interface, set `TMS_HTTP_BIND_ADDRESS` before launch; for example,
`$env:TMS_HTTP_BIND_ADDRESS = "192.0.2.10"`. Only do this when network
exposure is intentional and firewall/reverse-proxy controls are in place.
Stop it with Ctrl+C/Ctrl+Break. To build only the existing backend without
Crow, configure and test it independently:

```powershell
cmake -S . -B build\issue9-core-only -G "Visual Studio 18 2026" -A x64 `
  -DBUILD_API_SERVER=OFF -DBUILD_TESTING=ON `
  -DCMAKE_CONFIGURATION_TYPES=Debug
cmake --build build\issue9-core-only --config Debug
ctest --test-dir build\issue9-core-only -C Debug --output-on-failure
```

## Routes

The five control routes delegate to Issue #28's `SimulationControlApi`. The
read-only state route uses the shared service snapshot and Issue #27 serializer:

| Method | Path | Purpose |
|---|---|---|
| `GET` | `/api/simulation/state` | Read a consistent state and vehicle snapshot |
| `POST` | `/api/simulation/start` | Start a stopped simulation |
| `POST` | `/api/simulation/pause` | Pause a running simulation |
| `POST` | `/api/simulation/resume` | Resume a paused simulation |
| `POST` | `/api/simulation/reset` | Stop and reset while retaining configuration |
| `PUT` | `/api/simulation/config` | Replace all five configuration values |

The state route returns the common success envelope; `data` contains the
serializer's state object. Errors use the same common error envelope as the
control API.

Example state response (vehicle arrays abbreviated):

```json
{
  "success": true,
  "data": {
    "state": "stopped",
    "tickCount": 0,
    "activeApproach": "north",
    "activeSignal": "green",
    "approaches": [
      {"id": "north", "signal": "green", "queueLength": 0, "vehicles": []},
      {"id": "east", "signal": "red", "queueLength": 0, "vehicles": []},
      {"id": "south", "signal": "red", "queueLength": 0, "vehicles": []},
      {"id": "west", "signal": "red", "queueLength": 0, "vehicles": []}
    ],
    "config": {"tickMs": 1000, "greenTicks": 10, "yellowTicks": 2, "arrivalInterval": 3, "maxQueue": 12},
    "statistics": {"vehiclesGenerated": 0, "vehiclesProcessed": 0, "totalWaitingTicks": 0, "averageWaitingTicks": 0.0}
  },
  "error": null
}
```

Example requests:

```http
GET /api/simulation/state
```

```http
POST /api/simulation/start
```

```http
PUT /api/simulation/config
Content-Type: application/json

{"tickMs":1000,"greenTicks":10,"yellowTicks":2,"arrivalInterval":3,"maxQueue":12}
```

Successful requests and errors use the envelopes documented in
[Simulation Control API Contract](Simulation_Control_API_Contract.md). The
adapter preserves API status codes, JSON bodies, and content types. Invalid
JSON is marked as malformed for the existing API handler. Configuration JSON
must use integer scalar values; strings, booleans, nulls, arrays, objects, and
floating-point values remain distinct and are rejected by the existing API
validation path. JSON integers outside the API's safe signed representation
are retained as integer-typed invalid values so they cannot be accepted as
configuration.

The accepted configuration ranges are `tickMs` 100-5000, `greenTicks` 1-60,
`yellowTicks` 1-10, `arrivalInterval` 1-20, and `maxQueue` 1-50. The HTTP
adapter does not duplicate these checks; Issue #28's existing validation path
remains authoritative.

The HTTP adapter accepts a request body only when its media type is
`application/json` (case-insensitive; optional parameters such as `charset`
are allowed). Bodyless control calls without a `Content-Type` remain valid as
specified by the API contract. Non-JSON media types, including browser form
content types, are rejected with HTTP 415 before invoking the control API.
Configuration calls with an empty JSON body continue to receive the API's
existing HTTP 400 invalid-request response. Requests carrying a browser
`Origin` must match the configured public origin when `TMS_PUBLIC_ORIGIN` is
set, or the request's HTTP origin in the default local configuration.
`Sec-Fetch-Site` must be `same-origin` or `none`; cross-origin browser requests
receive HTTP 403. This preserves headerless API clients while rejecting
cross-origin form and fetch submissions. Unsupported methods on a registered
API path receive HTTP 405 with the common JSON error envelope and an `Allow`
header; unknown API paths remain 404 responses.

## Tests and verification

`simulation_http_server_test` exercises real loopback HTTP startup, all six
routes and the common state envelope, response forwarding, malformed JSON,
wrong JSON types, invalid configuration/state transitions, concurrent control
requests while the
service scheduler is active, invalid-bind startup failure, concurrent
shutdown, post-shutdown unavailability, JSON media-type enforcement, and body
limits below, at, and above 64 KiB. Crow's parser stores no more than the
configured limit and skips JSON parsing for an oversized body. The existing
service, API, and core tests remain registered with CTest. Tests use
always-active checks rather than `assert()`.

Validation on Windows with Visual Studio 18 / MSVC 19.51.36256 and Windows SDK
10.0.26100.0:

| Configuration | Build | CTest |
|---|---|---|
| Debug, `BUILD_API_SERVER=ON` | Passed | 12/12 passed |
| Release, `BUILD_API_SERVER=ON` | Passed | 12/12 passed |
| Debug, `BUILD_API_SERVER=OFF` | Passed | 10/10 passed |

The current CMake configuration has no Qt package lookup or desktop GUI target.
The React, TypeScript, Vite and Tailwind CSS frontend scaffold is maintained in
`frontend/` and is not included in these backend builds. Its typed client uses
the same-origin `/api` path; see the [frontend setup guide](../frontend/README.md).
The lifecycle controls and configuration form are implemented in the frontend;
the full browser dashboard, live backend demonstration, and acceptance evidence
remain pending.

The saved CTest records do not contain a source commit ID or source hash. They
cannot conclusively identify the exact source revision or prove whether all
then-current uncommitted changes were included. These are local test records,
not Issue #10 acceptance results. No tests were rerun during Issue #12
documentation preparation.

A prior verification note reports an independent API-server launch and live
HTTP requests for initial state, lifecycle operations, configuration and error
handling. The underlying run record is not present in this worktree, and no
source revision is attached to the report. Treat it as an unverified historical
report, not revision-bound evidence or Issue #10 acceptance.
