# TC-16 — Source and Memory-Safety Review

## Review status

**In progress — not yet accepted as complete.** This document is a review record, not a claim that the code is free of memory-safety defects.

## Initial reviewed areas

The following current implementation areas were inspected while preparing the Issue #11 regression tests:

- `src/TrafficSignal.cpp` and `src/TrafficSignal.h`: signal-state validation, approach bounds checks, coordinator access, and transition checks.
- `src/TrafficManager.cpp` and `src/TrafficManager.h`: approach identity checks, signal gating, empty-queue checks, and dequeue behavior.
- `src/queue_manager.cpp` and `include/queue_manager.h`: map lookup before dequeue, queue emptiness checks, and snapshot-by-copy behavior.
- `src/simulation_model.cpp` and `include/simulation_model.h`: configuration validation, approach-index use, queue-capacity checks, vehicle bookkeeping, and reset behavior.

## Initial findings

1. The coordinator checks approach indexes before indexing its fixed-size state array. The checked accessor throws `std::out_of_range`; state changes return false for invalid indexes.
2. Signal transitions validate enum values and legal transition sequences before changing state. The coordinator only grants GREEN when every other approach is RED.
3. Traffic processing verifies the requested road matches the manager's approach, checks the signal, and checks queue emptiness before dequeue.
4. Queue dequeue checks both map membership and queue emptiness before reading the front element. Snapshot returns a copy rather than exposing the internal queue.
5. The simulation constructor rejects out-of-range configuration. Arrival insertion checks the configured per-road queue capacity, and reset clears queued vehicles and associated statistics.

## Residual risks and work still required

- This initial pass does not cover every active source file, third-party dependency, compiler path, or runtime configuration. Complete the wider source review before marking TC-16 complete.
- The coordinator and queue/model objects are not independently thread-safe; callers must preserve the service-level synchronization contract.
- The simulation model stores non-owning references/pointers inside traffic managers/signals. Their lifetime safety depends on member construction/destruction order and must be reviewed alongside any future refactor.
- ASan/UBSan and static-analysis results are supporting evidence only. They do not prove absence of use-after-free, data races, integer issues, or logic defects.
- No ThreadSanitizer or fuzzing result is claimed by this review.

## Acceptance evidence to add

Before closing TC-16, record the complete active-source file list reviewed, any findings and their dispositions, the exact commit SHA, and successful CI run identifiers for the sanitizer and static-analysis jobs.
