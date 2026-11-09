# Simulation Statistics Contract

## Ownership and access

`SimulationStatistics` is a backend-neutral value type declared in
`include/simulation_statistics.h`. The authoritative values live in the
existing `SimModel` in `include/simulation_model.h`; no second model or
statistics service is introduced. The legacy `include/gui_model.h` remains as
a compatibility header for older source consumers and includes the neutral
model interface. The active REST backend links `tms_core`, which has no
dependency on Qt, Crow, or nlohmann-json.

`SimModel::statistics()` returns a copied snapshot. It does not advance ticks,
dequeue vehicles, or mutate counters or queues. It is not internally
synchronized. The existing `SimulationService` owns the single production `SimModel` and
serializes snapshot reads with tick, command, and reset
operations using its existing synchronization boundary. The service provides
the copied snapshot used by Issue #27's serializer; JSON field names,
transport status, and route behavior belong to the API boundary, not this core
contract.

## Implemented fields and formulas

| Field | C++ type | Definition | Update behavior |
|---|---|---|---|
| `tickCount` | `std::uint64_t` | Number of logical ticks accepted by `SimModel::tick()`. | Increments once for each running tick; reset sets it to zero. |
| `vehiclesGenerated` | `std::uint64_t` | Sum of successful per-approach vehicle enqueues. Arrivals rejected by queue capacity are excluded. | Derived from per-approach event counters; reset clears those counters. |
| `vehiclesProcessed` | `std::uint64_t` | Sum of vehicles successfully processed by the signal-gated traffic managers. | Derived from per-approach event counters; reset clears those counters. |
| `vehiclesWaiting` | `std::size_t` | Sum of the four current approach queue lengths. | Read from the actual queues in each snapshot. |
| `totalWaitingTicks` | `std::uint64_t` | Sum, over completed vehicles, of `processingTick - arrivalTick`. | Increments once when a vehicle is processed; reset sets it to zero. |
| `averageWaitingTicks` | `double` | `totalWaitingTicks / vehiclesProcessed`; zero when no vehicle has been processed. Queued vehicles are not included until processed. | Calculated for each snapshot; reset returns it to zero. |
| `processedPerLogicalTick` | `double` | `vehiclesProcessed / tickCount`; zero when `tickCount == 0`. | Calculated for each snapshot; reset returns it to zero. |
| `approaches` | `std::array<ApproachStatistics, 4>` | Per-approach values in **North, South, East, West** order. | Generated/processed counts are event counters; waiting counts are read from the corresponding queues. |

Each `ApproachStatistics` contains:

- `vehiclesGenerated` (`std::uint64_t`): successful enqueues at that approach.
- `vehiclesProcessed` (`std::uint64_t`): successfully processed vehicles at
  that approach.
- `vehiclesWaiting` (`std::size_t`): current queue length for that approach.

The per-approach generated and processed counters are the sole stored event
counters. The existing `SimModel::generatedCount()` and
`SimModel::processedCount()` accessors, and the snapshot's global totals, sum
those arrays rather than maintaining duplicate global counters. The waiting
total is the sum of the actual queue lengths.

`processedPerLogicalTick` is a **logical simulation rate**: the cumulative
number of processed vehicles divided by the cumulative logical tick count
since reset. It is not wall-clock throughput, does not have units of vehicles
per second, and must not be presented or serialized under a per-second label.
No wall-clock timing is collected by this contract. The SRS defines throughput
as vehicles processed “during a period” but does not define a wall-clock period;
the logical-tick denominator is the deterministic period supported by the
existing model. A wall-clock metric would require a separately agreed
requirement and design.

## Reset behavior

`SimModel::reset()` clears the tick count, global and per-approach statistics,
waiting-time accumulator, and all queues; restores North GREEN; and returns the
model to Stopped. It preserves the accepted `SimConfig`.

## Snapshot consistency and thread safety

In the current single-threaded model, the returned value contains the model's
current counters and queue lengths and performs no mutations. A future
concurrent caller must not read the model directly while it is being updated.
The `SimulationService` in Issue #26 is responsible for serializing the
complete snapshot read with ticks and commands; the statistics value type and
`SimModel::statistics()` add no mutex and claim no cross-thread safety.

## Example

For the default configuration after 13 logical ticks, the current model
generates one vehicle per approach. North and South have been processed; one
East and one West vehicle remain queued:

```json
{
  "tickCount": 13,
  "vehiclesGenerated": 4,
  "vehiclesProcessed": 2,
  "vehiclesWaiting": 2,
  "totalWaitingTicks": 8,
  "averageWaitingTicks": 4.0,
  "processedPerLogicalTick": 0.1538461538,
  "approaches": [
    { "name": "North", "vehiclesGenerated": 1, "vehiclesProcessed": 1, "vehiclesWaiting": 0 },
    { "name": "South", "vehiclesGenerated": 1, "vehiclesProcessed": 1, "vehiclesWaiting": 0 },
    { "name": "East", "vehiclesGenerated": 1, "vehiclesProcessed": 0, "vehiclesWaiting": 1 },
    { "name": "West", "vehiclesGenerated": 1, "vehiclesProcessed": 0, "vehiclesWaiting": 1 }
  ]
}
```

This example documents the statistics value contract. Signal states, active
phase, run state, vehicle IDs/types, and FIFO vehicle snapshots are provided
by the broader state contract and REST integration in Issues #27/#28, not by
`SimulationStatistics` alone.
