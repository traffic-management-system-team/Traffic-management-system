# Issue #12 — Final Demo Readiness

## Current status

The web application implementation and automated acceptance evidence are complete and integrated into main. Issue #10 was independently reviewed and its evidence PR #51 was merged.

This checklist separates completed automated verification from the remaining manual handoff/demo activities. Closing the implementation issues does not by itself prove that a final presentation video or deployment demonstration has been recorded.

## Readiness checklist

| Item | Evidence / expected behavior | Status |
|---|---|---|
| Backend startup | C++ REST server builds and starts on the documented local listener | Automated/local verification recorded |
| Initial stopped state | Browser reads the real API snapshot and displays the stopped simulation | Covered by real-backend browser test |
| Four-way dashboard | North, South, East and West approaches render signal state, queue and vehicle details | Implemented; component and browser evidence recorded |
| Lifecycle controls | Start, Pause, Resume and Reset operate through backend APIs | Covered by real-backend browser test |
| Configuration update | Valid configuration is submitted and accepted values come from backend response | Covered by real-backend browser test |
| Configuration validation | Invalid green duration returns validation error; UI shows feedback and preserves last accepted value | Covered by real-backend browser test |
| Signals and transitions | Signal labels and phase changes are sourced from backend state | Covered by backend and browser tests |
| Vehicle arrivals and FIFO queues | Vehicles are generated, queued FIFO and processed only when signal permits | Covered by C++ regression and frontend component/browser tests |
| Statistics | Generated, processed, waiting and average-wait values update from backend snapshots | Covered by C++ regression and frontend tests; live values exercised in browser test |
| API errors | Invalid lifecycle/configuration requests return defined error responses and UI feedback | Covered by API and browser tests |
| Responsive layout | Browser checks at 390 px, 768 px and 1440 px; no horizontal overflow in tested views | Playwright checks passed |
| Keyboard accessibility | Skip-link keyboard focus/activation and keyboard form operation | Automated checks passed; limited scope |
| Manual screen-reader/WCAG audit | Review focus order, announcements, contrast and screen-reader behavior with assistive technology | Still to perform if required by course rubric |
| Cross-browser verification | Smoke-test supported browsers beyond Chromium | Not included in recorded acceptance run |
| Deployment smoke test | Verify the intended demo/deployment environment, API origin and network boundary | Still to perform |
| Final demonstration recording | Record a complete browser session covering setup, initial state, configuration, lifecycle, signals, queues, statistics and validation feedback | Recording not verified in repository |

## Recorded automated verification

Issue #10 report and PR #51 record:

- Windows MSVC 19.52.37029 API-enabled Release build succeeded.
- Clean-checkout CTest run on source commit 2eadfbe1797c8394c9253423726eac712b0fa1a6: 15/15 passed.
- Frontend TypeScript type-check passed.
- Vitest: 38/38 passed.
- Production build passed.
- Playwright real-backend Chromium suite: 3/3 passed.
- GitHub Actions on the reviewed PR source: both workflows passed, 8 jobs successful.

See [Issue #10 C++ Regression and Web Acceptance Report](Issue_10_CPP_Regression_Report.md), [test output](../cpp-test-results-issue10-release.txt) and [frontend browser test](../frontend/e2e/simulation.spec.ts).

## Suggested live demonstration sequence

1. Start the backend and frontend; show the initial stopped state.
2. Explain the four approaches and their signal indicators.
3. Submit a valid configuration and show the accepted settings.
4. Submit an invalid value and show the validation message and retained accepted configuration.
5. Start the simulation; show vehicle arrivals, FIFO queues, signal transitions and changing statistics.
6. Pause and resume the simulation.
7. Reset; show the cleared queues/statistics and restored initial state.
8. Briefly show the responsive layout and keyboard navigation.

## Safety and deployment note

The API server has no built-in authentication or TLS. For a local classroom demonstration, keep it bound to loopback. For any shared or public deployment, use an approved same-origin reverse proxy, TLS termination, firewall restrictions and an appropriate access-control boundary. Do not expose the development API directly to an untrusted network.

## Final handoff

Before submission, confirm the course rubric's required video format/location, perform the live demonstration on the intended machine, and retain the recording link alongside the project report. Do not mark manual accessibility or deployment checks complete without performing them.
