# Simulation Statistics Contract

`SimModel::statistics()` returns a read-only value snapshot derived from the
existing C++ simulation. It does not advance ticks, dequeue vehicles, or mutate
the model. The future `SimulationService` must serialize this read with model
updates; this value type does not provide synchronization.

## Implemented fields

| Field | C++ type | Meaning and calculation | Update/reset behavior |
|---|---|---|---|
| `tickCount` | `std::uint64_t` | Number of logical ticks accepted by `SimModel::tick()`. | Increments once per running tick; reset sets it to zero. |
| `vehiclesGenerated` | `std::uint64_t` | Vehicles successfully created and enqueued. Capacity-rejected arrivals are not counted. | Increments on successful arrival; reset sets it to zero. |
| `vehiclesProcessed` | `std::uint64_t` | Vehicles successfully processed by the signal-gated traffic managers. | Increments once per processed vehicle; reset sets it to zero. |
| `vehiclesWaiting` | `std::size_t` | Sum of the current four queue lengths. | Instantaneous; changes only when a vehicle is enqueued, processed, or removed by reset. |
| `totalWaitingTicks` | `std::uint64_t` | Sum of `processingTick - arrivalTick` for processed vehicles. | Increments when a vehicle is processed; reset sets it to zero. |
| `averageWaitingTicks` | `double` | `totalWaitingTicks / vehiclesProcessed`; zero when no vehicle has been processed. It includes completed trips only, not vehicles still queued. | Recomputed in each snapshot; reset returns it to zero. |
| `processedPerLogicalTick` | `double` | Cumulative `vehiclesProcessed / tickCount`; zero before the first tick. This is a logical-tick rate, not wall-clock throughput. | Recomputed in each snapshot; reset returns it to zero. |
| `approaches` | `std::array<ApproachStatistics, 4>` | Per-approach generated, processed, and currently waiting counts. Array order is North, South, East, West. | Event counters increment on successful arrival/processing; queue value is read from the actual queue. All reset to zero. |

Each `ApproachStatistics` has `vehiclesGenerated` (`std::uint64_t`),
`vehiclesProcessed` (`std::uint64_t`), and `vehiclesWaiting`
(`std::size_t`). Global generated/processed values equal the sum of their
per-approach event counters. The current waiting total equals the sum of
per-approach queue lengths.

`reset()` clears all statistics and queues, restores North GREEN, and returns
the model to Stopped. It preserves the accepted `SimConfig`.

## Example

After 13 default logical ticks, the model has generated one vehicle per
approach; North and South have been processed, while East and West are queued:

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

The JSON is an illustrative REST representation, not a serializer implemented
in the C++ core. `nlohmann/json` serialization and endpoint naming belong to
the API integration. Signal states, active phase, simulation run state,
vehicle IDs/types, and FIFO vehicle arrays are not statistics fields; the
state/snapshot API owns those values.
