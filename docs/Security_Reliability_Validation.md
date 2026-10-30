# Security and Reliability Validation

Issue: #11 - Perform security and reliability validation

## Traceability

- SEC-01 and TC-10: malformed and out-of-range configuration is rejected;
  rejection preserves the last accepted configuration.
- SEC-02, NFR-03, and TC-13: empty-queue processing is safe and leaves the
  output vehicle unchanged.
- SEC-03 and TC-04: invalid signal values and transitions are rejected without
  changing the active state.
- SEC-04 and TC-16: manually review source for unchecked buffers, unsafe
  pointers, and unsafe memory operations. Automated checks do not replace this
  required review.
- FR-05, FR-11, and TC-05/TC-11: only the permitted approach processes vehicles;
  vehicles on another approach remain queued.
- FR-06 and NFR-07: configured per-approach capacity and reset invariants are
  checked against the simulation model.

## Automated checks

`tests/security_reliability_test.cpp` exercises the checks above except for the
manual SEC-04 source review. Run `security_reliability_test` through CTest in
Debug and Release alongside the existing targets.

## Limits and evidence

Record source-review findings for TC-16 separately. This test does not prove
absence of all memory-safety defects and does not verify the Qt GUI runtime.
Report GUI runtime verification separately when Qt 6 Widgets is available.
