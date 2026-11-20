# Software Requirements Specification

## Document control

Traffic Management System (Simulation), final submission revision 1.5, 19 November 2026. This IEEE-style specification describes the delivered browser interface and C++ simulation. It reconciles the October desktop design with the implementation and retains all 24 requirement IDs.

| Version | Date | Recorded contribution |
|---|---|---|
| 1.0 | 6 Oct 2026 | Daruvuri Pranav Sai, initial baseline |
| 1.1 | 6 Oct 2026 | GOWTHAM S, testing/security contribution |
| 1.2 | 6 Oct 2026 | Dinesh, architecture and module contribution |
| 1.3 | 6 Oct 2026 | C NITISH, design/GUI and traceability contribution |
| 1.4 | 19 Nov 2026 | Final submission reconciliation against implemented source and recorded audit evidence. Formal team/instructor sign-off pending. |
| 1.5 | 19 Nov 2026 | Applied the team-approved retrospective Sprint 3 documentation classification; preserved original sprint assignments, requirements, diagrams, testing evidence and limitations. No historical formal planning or instructor approval verified. |

The 1.0–1.3 rows reproduce the historical document revision records. They do not independently prove individual authorship of every document edit. Original documents and their complete wording are retained under docs/history/final-reconciliation-2026-11-19/. GitHub implementation authorship is described separately in the sprint change log.

## 1 Introduction

### 1.1 Purpose and scope

The system is an educational four-way traffic simulation for an operator and evaluator. It generates vehicles, maintains road queues in FIFO order, coordinates Red/Yellow/Green phases, processes permitted vehicles and displays queue and waiting statistics. It provides Start, Pause, Resume, Reset and validated configuration controls.

### 1.2 Definitions and references

FIFO means First In First Out. A logical tick is one backend simulation update. Waiting ticks measure how long a completed vehicle was queued. Throughput is cumulative processed vehicles divided by logical ticks. References are the original SRS v1.3, current formal SAD revision 1.5, final Test Plan revision 1.5, docs/REST_API.md, docs/Simulation_Statistics_Contract.md and docs/sprint-1/RTM.csv. University requirements were supplied in the audit brief.

## 2 Overall description

### 2.1 Product perspective and users

The browser is the graphical interface. React and TypeScript render authoritative C++ state through JSON REST requests. The student/operator changes settings and lifecycle state. The evaluator observes behavior. Developers maintain modules and tests. The active application uses no database, live sensors or physical traffic lights.

### 2.2 Operating environment and dependencies

The simulation uses C++17 and CMake. Crow hosts HTTP with Standalone Asio. A separate nlohmann-json serializer converts snapshots. React, TypeScript, Vite and Tailwind CSS supply the browser frontend. Vitest and Playwright support frontend verification. GitHub Actions supplies CI. The local API defaults to 127.0.0.1:18080 and Vite to 127.0.0.1:5173. Vite proxies /api requests to the API. Same-origin deployment guidance is in docs/REST_API.md. The API has no built-in authentication/TLS and is intended for trusted local use by default.

### 2.3 Constraints and assumptions

C++ owns simulation rules, queues, signals, validation, time and statistics. TypeScript owns presentation and requests. The original C/C++ requirement remains satisfied for the simulation core, while the presentation technology evolved to a browser stack. This revision documents that deviation and does not invent instructor approval. No external traffic network is required, although browser/backend communication uses localhost HTTP. A clean dependency build may require network downloads. Simulated departures are observable through FIFO removal and changing counts. Stationary queue markers represent actual backend entries. The system provides no continuous vehicle trajectories, physical positions or phase countdown.

## 3 Specific requirements

### 3.1 External interfaces

Control Centre displays connection/state/tick status, lifecycle controls, six aggregate metrics, accepted/draft configuration with validation feedback, and four-approach summary. Simulation Monitor displays a North/East/South/West junction, real queue markers, current signals, active approach, full FIFO lists with IDs/type/lane/capacity, shared lifecycle controls and a bounded activity log inferred from consecutive snapshots. The road scene shows up to six nearest queued vehicles per approach. Both pages share one snapshot hook and preserve simulation continuity through hash navigation. State and lifecycle controls are visible on each page. More detailed operational data is grouped by page.

The UI updates after commands and through state-dependent polling. Events detected between polls are observations, not a complete server event history. Brief transitions may be missed. Unknown/unavailable state and rejected operations produce explicit feedback. Layouts are exercised at 390, 768 and 1440 pixels.

Software interfaces are browser → typed API client → Crow/HTTP adapter → SimulationControlApi → SimulationService → SimModel/modules. The state route copies a synchronized snapshot, serializes it, then wraps the response in the common success/data/error envelope. The six supported endpoints are listed in the SAD. Unsupported paths/methods are handled as errors.

### 3.2 Functional nonfunctional and security requirements

Original IDs and requirement statements remain stable. The interface and environment sections above clarify their final web interpretation.

| ID | Requirement |
|---|---|
| FR-01 | The system shall create vehicles according to the configured arrival mechanism. |
| FR-02 | The system shall assign each generated vehicle to the appropriate road/lane queue. |
| FR-03 | The system shall maintain each road/lane queue using FIFO ordering. |
| FR-04 | The system shall maintain and display the traffic signal state for each controlled direction. |
| FR-05 | The system shall allow vehicles to leave a queue only when movement is permitted by the active signal state. |
| FR-06 | The system shall update queue lengths when vehicles enter or leave queues. |
| FR-07 | The system shall calculate traffic statistics including processed vehicle count and waiting-time information. |
| FR-08 | The system shall display the simulation state and statistics through the GUI. |
| FR-09 | The system shall allow the user to start, pause/resume and reset the simulation. |
| FR-10 | The system shall reject invalid simulation configuration values and prevent unsafe queue operations. |
| FR-11 | The system shall keep vehicles that cannot currently move in their respective queues. |
| FR-12 | The system shall update the GUI after relevant simulation-state changes. |
| NFR-01 | The main simulation state and controls shall be visible without navigating through multiple screens. |
| NFR-02 | Normal simulation updates shall complete without noticeable GUI freezing on the target development system. |
| NFR-03 | The application shall handle empty queues and invalid inputs without crashing. |
| NFR-04 | Traffic signal, queue, vehicle and statistics logic shall be separated into maintainable modules. |
| NFR-05 | Core simulation operations shall be implementable and verifiable as independent functions/modules. |
| NFR-06 | The source code shall avoid unnecessary platform-specific dependencies where practical. |
| NFR-07 | Queue operations shall preserve FIFO ordering and signal rules shall be consistently enforced. |
| NFR-08 | GUI controls shall remain usable during normal simulation execution. |
| SEC-01 | The system shall validate all user-entered simulation configuration values before passing them to the simulation modules. |
| SEC-02 | The system shall prevent dequeue, removal or access operations on an empty queue. |
| SEC-03 | The system shall reject invalid traffic-signal state transitions. |
| SEC-04 | The implementation shall avoid unsafe memory access and unchecked buffer operations. |

### 3.3 Configuration and edge conditions

The five integer fields are tickMs 100–5000, greenTicks 1–60, yellowTicks 1–10, arrivalInterval 1–20 and maxQueue 1–50. Canonical defaults in include/sim_config.h are 1000, 10, 2, 3 and 12 respectively. Accepted runtime values can differ after configuration. Updates require stopped state, tick zero and no generated vehicles. Rejected input preserves accepted configuration. Reset clears simulation activity while retaining configuration. Queue capacity suppresses generation for a full approach. One vehicle is generated on a rotating approach at each arrival interval. Empty/missing queues fail safely. RED and YELLOW prevent dequeue by TrafficManager. Invalid transitions and conflicting GREEN requests are rejected.

### 3.4 Verification limits

NFR-02 has functional browser responsiveness evidence but no quantified latency benchmark. SEC-04 has source review and historical core sanitizer/static-analysis evidence with stated limits. These requirements are not redefined as quantitative guarantees. The final evidence ledger identifies executed versus unavailable measurements.

## 4 Agile evolution and traceability

The original C++ desktop GUI proposal evolved into a React/TypeScript browser interface communicating over localhost REST with the retained C++17 simulation backend. The migration enables browser-based operation while reusing the implemented simulation rules. On 19 November 2026, the team approved documenting issues #26–35 as Sprint 3 — Web Migration (Retrospective Classification), using the observed 4–14 November 2026 IST activity window. These ten issues add 44 estimated points to the original 47, giving 22 issues and 91 points, a 93.62% scope increase. Original Sprint 1/2 assignments and all 24 requirement IDs and statements remain unchanged. This present-day classification does not verify an original planning meeting, iteration assignment or instructor approval.

PR #53 added the two-page presentation on 16 November, after the migration cohort's observed window. The current uncommitted visual redesign changes presentation layout; lifecycle, configuration and FIFO contracts remain traced. Original Sprint 2 work continued into November without changing its original allocation. Actual dates, assignments and integrating authors appear in docs/SPRINT_CHANGE_LOG_FINAL.md.

Backlog scope increased by 44 story points; a historical rise in the burndown chart has not been independently verified.

## 5 Submission evidence

Reconciled 19 November 2026 for the 20 November 2026 viva. Source baseline 5cbdc012591221ccf300a9656b55bb1c5c38a2d2, branch feature/professional-traffic-ui, plus the seven existing uncommitted frontend redesign files. This documentation change does not commit or approve the redesign. Execution results below belong to the 19 November audit working tree. Historical results retain their original dates and revisions. No new execution is implied by this reconciliation.

Formal sign-off and sprint video submissions are NOT VERIFIED. The final evidence ledger and reconciliation register record remaining gaps.
