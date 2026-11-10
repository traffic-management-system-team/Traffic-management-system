# Traffic Management System — Part-1 Checked Final v1.3

All five Part-1 documents are at current version 1.3 and each contains the complete revision history v1.0–v1.3.

Version distribution in every document:
- v1.0 — Daruvuri Pranav Sai (P1)
- v1.1 — GOWTHAM S (P2)
- v1.2 — Dinesh (P3)
- v1.3 — C NITISH (P4)

Cross-document consistency was corrected for FR/NFR/SEC IDs, test cases, architecture/design references, and UML coverage.

## Current architecture

The current target is a browser-based web application backed by the C++17
simulation core and a Crow HTTP server. The backend exposes JSON REST APIs for
simulation state, lifecycle control, and configuration. It owns one
`SimulationService`; `tms_core` remains independent of Crow and nlohmann-json.

The selected browser frontend stack is React, TypeScript, Vite and Tailwind
CSS. This is an architecture decision only: frontend source and browser
functionality are not implemented in this repository. The earlier Qt desktop
prototype and Part-1 GUI-oriented specifications remain historical context.

See [the current architecture](docs/Architecture.md),
[REST API routes and build instructions](docs/REST_API.md),
[build and test instructions](BUILD_TESTING.md), and the
[Issue #12 demo-readiness checklist](docs/Issue_12_Demo_Readiness.md).
