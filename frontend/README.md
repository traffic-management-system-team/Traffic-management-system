# Frontend application

## Prerequisites

- Node.js 22.12 or newer (Node 22.17 is used by the frontend validation workflow).
- npm 10 or newer.
- The C++ REST API server is required for live data. It listens at `http://127.0.0.1:18080` by default.

## Install and run

From the repository root:

```powershell
cd frontend
npm ci
Copy-Item .env.example .env.local
# Edit API_PROXY_TARGET in .env.local only if the backend listens elsewhere.
npm run dev
```

In another terminal, start the backend server using the CMake build instructions in [REST API documentation](../docs/REST_API.md). Vite serves the frontend and forwards same-origin `/api/*` requests to `API_PROXY_TARGET`. This variable is used only by the Node-side Vite configuration and is not exposed in the browser bundle. Do not add a `VITE_` prefix or enable permissive CORS. The proxy preserves browser-facing `Host` and `Origin` headers to remain compatible with backend origin validation.

## Automated quality checks

```powershell
npm run type-check
npm test
npm run build
```

The GitHub Actions Issue #33 workflow runs all three checks. The Issue #34 workflow additionally builds the C++ REST API and runs Chromium browser tests against that live server.

## Implemented dashboard and controls

The React application renders a responsive intersection dashboard from the authoritative `GET /api/simulation/state` snapshot:

- Four approach cards (North, East, South, West), with a descriptive RED/YELLOW/GREEN signal state and active-approach marker.
- Each approach's queue in backend FIFO order, vehicle ID/type/lane, queue length and configured capacity.
- Aggregate generated, processed, waiting and average-wait statistics, plus the logical tick and simulation state.
- Per-approach arrivals/processed activity since the previous snapshot.
- Start, Pause, Resume and Reset controls, plus the five-field configuration form. The form keeps the last backend-accepted values separate from draft values and respects backend configuration locks.

The backend snapshot hook refreshes every second while running, every five seconds while paused, and every ten seconds while stopped. It stops polling while a command is pending and refreshes immediately after lifecycle/configuration operations and state conflicts.

## Responsive and accessibility behavior

The dashboard uses responsive grid breakpoints: statistics render in two columns on narrow screens and four columns at the small-screen breakpoint; approach cards move from one column to two and then four at larger breakpoints. The header/navigation adapts below 768 px. Controls and configuration fields have accessible names, status/signal indicators have descriptive labels, keyboard form submission is supported, focus-visible styles are defined, and reduced-motion preferences are respected.

## Frontend test coverage and its limits

- `App.test.tsx`: lifecycle actions and refresh, API conflict handling, configuration validation/accepted-value preservation, loading/duplicate-action behavior and keyboard submission.
- `IntersectionDashboard.test.tsx`: four approach/signal display, FIFO order, capacity, aggregate statistics on refreshed snapshots, per-approach activity and accessible signal labels.
- `useBackendStatus.test.ts`: adaptive polling at running/paused/stopped intervals and interval changes across state transitions.
- `client.test.ts`: typed REST envelope, endpoint, HTTP, network and malformed-response behavior.

These Vitest/jsdom tests cover component, hook and API-client behavior. They do not replace browser integration tests against the real C++ service.

## Real-backend browser tests

The Issue #34 Playwright suite exercises the dashboard through Chromium and the Vite same-origin proxy, with requests reaching the real C++ REST API. It verifies configuration submission, generated vehicle IDs/counts, descriptive signal labels, lifecycle controls, keyboard submission/activation, and mobile/tablet/desktop layouts. It does not mock `/api` responses.

For local execution, build and start the C++ API server using the instructions in [REST API documentation](../docs/REST_API.md). Keep it running at `http://127.0.0.1:18080`, then run from `frontend/`:

```powershell
npm ci
npm install --no-save --package-lock=false @playwright/test@1.56.1
npx playwright install chromium
$env:API_PROXY_TARGET = "http://127.0.0.1:18080"
npm run test:e2e
```

The Playwright runner is installed separately from the application dependency lockfile so the existing `npm ci` installation remains unchanged. The CI workflow pins the same runner version and installs Chromium with its operating-system dependencies. Automated browser coverage currently targets Chromium; a deployment-environment smoke test and a broader manual accessibility audit remain useful additional checks.
