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

The browser frontend is a separate pending component and is not implemented in
this repository. No frontend framework is selected here. The earlier Qt
desktop prototype and Part-1 GUI-oriented specifications are historical and
are not the current application architecture.

See [the current architecture](docs/Architecture.md),
[REST API routes and build instructions](docs/REST_API.md), and
[build and test instructions](BUILD_TESTING.md).
