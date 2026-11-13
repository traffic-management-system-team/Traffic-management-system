# Security and Reliability Validation — Issue #11

## Purpose

This package adds regression coverage for unsafe signal coordination, queue processing, and simulation invariants. It is layered on the current `tms_core` implementation. It does not replace the dedicated input-validation, queue, HTTP, API, service, or sanitizer test suites.

## Traceability and automated coverage

| Requirement | Coverage | Expected result |
|---|---|---|
| SEC-01 / TC-10 | `input_validator_test`, `simulation_control_api_test`, `simulation_http_server_test` | Malformed, wrong-type, missing and out-of-range configuration is rejected; a rejected update does not overwrite accepted configuration. |
| SEC-02 / TC-13 / NFR-03 | `queue_empty_safety_test`, `security_reliability_test` | Empty and unknown queues fail safely; output objects remain unchanged. |
| SEC-03 / TC-04 | `traffic_signal_test`, `security_reliability_test` | Invalid enum values, invalid transitions and invalid approach indexes are rejected or reported without mutating signal state. |
| SEC-03 / FR-05 / TC-05 / TC-11 | `traffic_manager_test`, `security_reliability_test` | RED/YELLOW or mismatched approaches cannot dequeue vehicles; GREEN processing preserves FIFO order. |
| FR-06 / NFR-07 | `simulation_api_contract_test`, `security_reliability_test` | Per-approach queue capacity is respected and reset clears queues, counters and waiting-time state. |
| SEC-04 / TC-16 | Manual source review plus compiler/static-analysis/sanitizer jobs | Findings and limitations must be recorded separately; automated tools are not proof of memory safety. |

## New regression target

`security_reliability_test` is registered against `tms_core` and exercises:

1. Invalid signal enum values and invalid coordinator indexes.
2. Constructor rejection of an invalid approach index.
3. Single-GREEN coordination and legal RED → GREEN → YELLOW → RED transitions.
4. Fail-closed queue processing for empty queues, wrong approaches and non-GREEN signals.
5. FIFO processing after the signal permits movement.
6. Invalid simulation configuration rejection.
7. Queue-capacity bounds over repeated logical ticks.
8. Reset invariants for state, signal, queues and statistics.

Existing targets remain the source of truth for detailed configuration parsing, HTTP request boundaries, API contracts, and empty-queue behavior; those tests are deliberately not duplicated here.

## Validation commands

Configure and run the complete registered suite:

```sh
cmake -S . -B build/issue-11 -DBUILD_API_SERVER=ON -DBUILD_TESTING=ON
cmake --build build/issue-11 --parallel
ctest --test-dir build/issue-11 --output-on-failure
```

Also run the repository CI configurations for Windows and Ubuntu Debug/Release, the core ASan/UBSan suite, and Clang static analysis. Record the exact commit and workflow result for each run. Do not mark a check PASS unless the corresponding run completed successfully.

## Scope and limitations

The tests exercise deterministic core behavior; they are not a formal proof of absence of defects, and they do not claim a penetration test, fuzzing campaign, thread-sanitizer run, or manual GUI audit. TC-16 requires a separate source-review record with files inspected, concrete findings, and residual risks.
