# Traffic Management System — Design Deliverables

> **Historical baseline:** The Part-1 v1.3 documents and diagrams below describe
> the original GUI-oriented desktop prototype. They are retained as project
> history and are not the current application architecture. The current target
> is documented in [Architecture](docs/Architecture.md).

## Design Ownership / Version History

| Version | Contributor | Contribution |
|---|---|---|
| 1.0 | Daruvuri Pranav Sai | Initial SAD baseline and requirement alignment |
| 1.1 | GOWTHAM S | Security and testability considerations |
| 1.2 | Dinesh | Architecture and component model |
| 1.3 | C NITISH | Detailed design and UML sequence diagrams |

## UML Diagrams

| ID | Diagram | Main Flow |
|---|---|---|
| UC-01 | Historical use-case diagram | Operator interaction with the GUI prototype |
| CD-01 | Historical component diagram | GUI, controller and simulation components |
| SD-01 | Historical vehicle-arrival sequence | Vehicle creation and queue insertion |
| SD-02 | Historical vehicle-processing sequence | Signal check and FIFO vehicle processing |
| SD-03 | Historical reset sequence | Reset flow and state restoration |

## Traceability

The historical diagrams are mapped to the Part-1 SRS requirements and SAD
components. Editable source and rendered diagrams remain under `diagrams/`.
The Part-1 SRS, SAD, test plan, sprint backlog, and work-distribution
documents in `docs/` are retained unchanged as historical records.
