# Traffic Management System — Design Deliverables

This page separates the original Part-1 design baseline from the implemented web application. Both are retained for project traceability, but they describe different stages of the project.

## Final submission revisions dated 19 November 2026

Reviewed SRS, SAD and Test Plan revision 1.5 Markdown, Word and PDF editions are linked from [Final submission status](docs/FINAL_SUBMISSION_STATUS_2026-11-20.md). The reviewed Word/PDF filenames contain FINAL_REVIEW_SPRINT3. All 18 pages were visually inspected; the original v1.3 documents remain the historical baseline. The Sprint 3 label is retrospective. Historical results and limitations remain in docs/FINAL_VERIFICATION_EVIDENCE_2026-11-19.md.

## 1. Original Part-1 design baseline (historical)

The Part-1 v1.3 SRS, SAD, Test Plan, sprint backlog and work-distribution documents were prepared for the earlier GUI-oriented desktop prototype. They are retained as the approved historical design records and should not be interpreted as a description of the current web deployment architecture.

### Design ownership / version history

| Version | Contributor | Contribution |
|---|---|---|
| 1.0 | Daruvuri Pranav Sai | Initial SAD baseline and requirement alignment |
| 1.1 | GOWTHAM S | Security and testability considerations |
| 1.2 | Dinesh | Architecture and component model |
| 1.3 | C NITISH | Detailed design and UML sequence diagrams |

### Historical UML diagrams

| ID | Diagram | Main flow |
|---|---|---|
| UC-01 | Use-case diagram | Operator interaction with the original GUI prototype |
| CD-01 | Component diagram | GUI, controller and simulation components |
| SD-01 | Vehicle-arrival sequence | Vehicle creation and queue insertion |
| SD-02 | Vehicle-processing sequence | Signal check and FIFO vehicle processing |
| SD-03 | Reset sequence | Reset flow and state restoration |

Editable Mermaid sources and rendered diagrams are stored under diagrams/. The original SRS, SAD and Test Plan are stored under docs/SRS/, docs/SAD/ and docs/Test_Plan/.

## 2. Current implemented application

The active application is a browser-based system:

- Frontend: React, TypeScript, Vite and Tailwind CSS.
- Backend: C++17 simulation core hosted by a Crow HTTP server.
- API: JSON endpoints for state snapshots, Start, Pause, Resume, Reset and configuration updates.
- Simulation ownership: one shared SimulationService owns the simulation model and scheduler.
- Core boundary: tms_core remains independent of Crow and nlohmann-json; state serialization is kept in a separate target.

The browser dashboard displays four intersection approaches, signal states, FIFO vehicle queues, queue capacity, aggregate statistics and per-approach activity. It provides lifecycle controls and a validated configuration form. The frontend consumes authoritative backend snapshots rather than hard-coded simulation state.

See [Current Web Architecture](docs/Architecture.md), [REST API](docs/REST_API.md) and [Frontend Application](frontend/README.md).

## 3. Verification and remaining demonstration checks

The reviewed Issue #10 evidence records:
- Windows MSVC Release build and CTest: 15/15 passed from a clean checkout of the PR source commit.
- Frontend type-check and production build: passed.
- Vitest: 38/38 passed.
- Playwright against the real C++ backend: 3/3 passed.
- GitHub Actions: all 8 jobs passed on the reviewed PR source.

These are automated verification results, not a claim of exhaustive accessibility or deployment testing. A full manual screen-reader/WCAG audit, broader cross-browser matrix, deployment-environment smoke test and final recorded demonstration remain separate handoff checks. See [Issue #12 Demo Readiness](docs/Issue_12_Demo_Readiness.md).

## 4. Interpretation

Use the Part-1 documents to understand the original requirements, design decisions and team contribution history. Use the current architecture and implementation documents to understand how the delivered web application is structured and operated. Where the historical desktop design differs from the current web implementation, the current architecture documents describe the active system.
