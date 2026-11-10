# Issue #12 Demo Readiness

## Current state

The target web frontend stack is React, TypeScript, Vite and Tailwind CSS. The
initial frontend scaffold, typed API client and tests, responsive shell,
lifecycle controls, configuration form, and live backend state request are
implemented under `frontend/`. The full browser dashboard,
signal/queue/statistics display, browser accessibility/responsiveness review
and end-to-end acceptance remain outstanding.

Issue #10 remains open and assigned to another team member. This checklist is
a preparation and evidence tracker, not a claim that Issue #10 or Issue #12
acceptance is complete.

## Checklist

| Demonstration item | Evidence needed | Status |
|---|---|---|
| Backend startup | Start the API server from a documented build and record revision, configuration and successful listener startup. | Pending revision-bound acceptance evidence |
| Initial stopped state | Load initial state from GET /api/simulation/state; show stopped state and four approach states from the real response. | Connection shell and controls display real state; full four-approach dashboard pending |
| Backend-driven dashboard | Render state, signals, queues and statistics from actual API responses, not hard-coded sample state. | Pending dashboard implementation |
| Start, pause, resume and reset | Operate browser controls and verify each resulting API state transition. | Controls call backend; UI tests verify state refresh; end-to-end demo evidence pending |
| Configuration update | Submit valid values through browser and show accepted values applied by backend. | Form uses accepted configuration from backend response; live demo evidence pending |
| Configuration validation | Submit malformed, missing and out-of-range values; show clear feedback and verify prior accepted configuration is retained. | Field/general errors and preservation have UI tests; end-to-end demo evidence pending |
| Four signals and transitions | Show North, South, East and West signal states and transitions sourced from API responses. | Backend state and serializer exist; browser display pending |
| Vehicle arrivals and FIFO queues | Show arrivals assigned to appropriate queues, FIFO order, and blocked vehicles retained until movement is permitted. | Backend behaviors have source tests; browser demonstration pending |
| Generated, processed and waiting statistics | Show counts and waiting-time information refreshed from actual API responses. | Backend calculation/serialization exist; browser display pending |
| Invalid requests and errors | Demonstrate malformed requests, invalid configuration and lifecycle conflicts with appropriate API errors and usable UI feedback. | UI tests cover validation and conflict feedback; live backend demonstration pending |
| Responsive layout | Verify dashboard and controls at agreed viewport sizes without obstructing state or operation. | Shell, controls and form use responsive layouts; browser/device verification and full dashboard pending |
| Keyboard accessibility | Operate controls and inspect focus order, visible focus, labels and error announcements by keyboard. | Controls/form are labelled and keyboard-operable with focus/error states; manual accessibility audit pending |
| End-to-end acceptance demo | Record a browser session against backend covering setup, initial state, configuration, lifecycle, signals, queues, statistics and errors. | Pending Issue #10 evidence and acceptance |

## Evidence and acceptance dependencies

The previously inspected local CTest logs record Debug 12/12, Release 12/12,
and core-only Debug 10/10 passing. Their logs contain no source commit ID or
source hash; they cannot conclusively prove the tested builds included all
four uncommitted changes in the separate Issue #9 worktree. They are local test
records, not Issue #10 acceptance results. No C++ tests were rerun during this
documentation preparation.

Issue #10 evidence still awaited:

- C++ regression results tied to an identified source revision and build.
- Frontend/browser integration against the real REST API.
- Revision-bound browser evidence for start, pause, resume and reset controls.
- Revision-bound configuration validation and visible error feedback evidence.
- Signal, queue and statistics display.
- Responsiveness and keyboard accessibility results.
- End-to-end demonstration evidence.

Do not mark Issue #12 complete until its documentation is reviewed and the
required implementation and acceptance evidence is available.
