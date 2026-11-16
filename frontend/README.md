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

## Control Centre and Simulation Monitor

The React application has two hash-addressable workspace pages and keeps one authoritative `GET /api/simulation/state` snapshot across navigation:

- **Control Centre** — backend status, lifecycle controls, five configuration values, six performance metrics and compact four-approach summaries.
- **Simulation Monitor** — a top-down intersection with actual queue vehicles, traffic signals, full FIFO queue lists, compact lifecycle controls and snapshot-detected activity.

The monitor represents each actual queued vehicle as a stationary marker on its inbound approach, in FIFO order. It shows up to six nearest vehicles per approach on the road scene and lists every vehicle and lane below. There are no fabricated vehicles or movement coordinates. The API does not expose a phase countdown or event stream; the activity log only reports changes detected between observed snapshots and can miss changes between polls.

The configuration form keeps backend-accepted values separate from its draft, displays the ranges already enforced by the C++ validator, and respects backend configuration locks. There is no reset-to-defaults action because the API does not provide one; Reset only resets simulation activity while retaining configuration.

The backend snapshot hook refreshes every second while running, every five seconds while paused, and every ten seconds while stopped. It stops polling while a command is pending and refreshes immediately after lifecycle/configuration operations and state conflicts.

## Responsive and accessibility behavior

The Control Centre metrics and approach summaries reflow from two columns on narrow screens to denser grids at larger breakpoints. Monitor queue cards move from one to two to four columns. The header/navigation adapts below 768 px. Controls and configuration fields have accessible names, signal states have text labels, keyboard form submission is supported, focus-visible styles are defined, and reduced-motion preferences are respected.

## Frontend test coverage and its limits

- `App.test.tsx`: lifecycle actions and refresh, API conflict handling, configuration validation/accepted-value preservation, loading/duplicate-action behavior and keyboard submission.
- `IntersectionDashboard.test.tsx`: top-down monitor, backend queue/FIFO display, front-vehicle labeling and snapshot-derived event behavior.
- `useBackendStatus.test.ts`: adaptive polling at running/paused/stopped intervals and interval changes across state transitions.
- `client.test.ts`: typed REST envelope, endpoint, HTTP, network and malformed-response behavior.

These Vitest/jsdom tests cover component, hook and API-client behavior. They do not replace browser integration tests against the real C++ service.

## Real-backend browser tests

The Issue #34 Playwright suite exercises both pages through Chromium and the Vite same-origin proxy, with requests reaching the real C++ REST API. It verifies configuration submission and restoration, generated vehicle IDs/counts, signal changes in the rendered scene, lifecycle controls, navigation continuity, event detection, keyboard submission/activation, and mobile/tablet/desktop layouts. It does not mock `/api` responses.

For local execution, build and start the C++ API server using the instructions in [REST API documentation](../docs/REST_API.md). Keep it running at `http://127.0.0.1:18080`, then run from `frontend/`:

```powershell
npm ci
npx playwright install chromium
$env:API_PROXY_TARGET = "http://127.0.0.1:18080"
npm run test:e2e
```

The lockfile installs `@playwright/test` 1.65.0, matching the browser runner used by CI. Chromium is installed with the runner's matching Playwright browser revision. Automated browser coverage currently targets Chromium; a deployment-environment smoke test and a broader manual accessibility audit remain useful additional checks.
