# Frontend scaffold

## Prerequisites

- Node.js 22.12 or newer (Node 22.17 was used for verification).
- npm 10 or newer.
- The C++ API server is optional for UI-shell work. For live connection status, start it on the configured target (default `http://127.0.0.1:18080`).

## Install and run

From the repository root:

```powershell
cd frontend
npm install
Copy-Item .env.example .env.local
# Edit API_PROXY_TARGET in .env.local if the backend listens elsewhere.
npm run dev
```

Vite serves the frontend and forwards same-origin `/api/*` requests to `API_PROXY_TARGET`. This variable is used only by the Node-side Vite configuration and is not exposed in the browser bundle. The default is `http://127.0.0.1:18080`. Do not add a `VITE_` prefix or enable permissive CORS. The proxy preserves the browser-facing `Host` and `Origin` headers to remain compatible with backend origin validation; deployments should serve the frontend and API through one public origin as described in [the architecture](../docs/Architecture.md#browser-deployment).

## Quality checks and production build

```powershell
npm run type-check
npm test
npm run build
```

`npm ci` can be run for reproducible installation using the checked-in
`package-lock.json`. The production output is written to `frontend/dist/` and is not served by the C++ CMake targets.

## Implemented scope

The initial React shell displays a loading, backend error, or connected state from `GET /api/simulation/state`; it includes a responsive header/navigation shell and a retry action. A centralized typed API client covers the six documented state, lifecycle and configuration endpoints and parses the common `{ success, data, error }` envelope. It preserves backend error codes, messages and validation details and normalizes HTTP, network and malformed-response errors.

This scaffold does not implement the full intersection dashboard, browser lifecycle/configuration controls, signal and queue presentation, or statistics UI. Those features remain for Issues #30–#32. Browser accessibility/responsiveness and end-to-end acceptance also require dedicated verification; see the [Issue #12 demo-readiness checklist](../docs/Issue_12_Demo_Readiness.md).
