# Simulation State Data Contract

Issue #27 provides a framework-independent serializer for a copied
`SimulationSnapshot`. It does not own a service, create an HTTP response
envelope, register a route, or depend on Crow. Issue #9 owns the HTTP host and
wraps this data as:

```json
{
  "success": true,
  "data": {},
  "error": null
}
```

The serializer interface is:

```cpp
nlohmann::json serializeSimulationStateData(
    const SimulationSnapshot& snapshot
);
```

## Data object

The returned JSON object contains:

- `state`: `stopped`, `running`, or `paused`.
- `tickCount`, `activeApproach`, and `activeSignal`.
- `approaches`: all approaches in North, East, South, West order. Each includes
  `id`, `name`, `signal`, `queueLength`, and a FIFO `vehicles` array. Each
  vehicle includes its `id`, `type`, and `lane`.
- `config`: the accepted `tickMs`, `greenTicks`, `yellowTicks`,
  `arrivalInterval`, and `maxQueue`.
- `statistics`: tick count, generated/processed/waiting counts,
  `totalWaitingTicks`, `averageWaitingTicks`, `processedPerLogicalTick`, and
  per-approach counters.

Statistics retain their existing North, South, East, West order. The
`averageWaitingTicks` value is the model's cumulative `totalWaitingTicks`
divided by cumulative `vehiclesProcessed`, or zero when no vehicles have been
processed. Waiting vehicles are excluded until processed.
`processedPerLogicalTick` is a logical-tick rate, not a wall-clock rate.

The serializer reads only the supplied snapshot. `SimulationService` remains
responsible for obtaining a consistent snapshot while holding its existing
synchronization boundary. `GET /api/simulation/state` obtains that snapshot from
the single shared service and wraps the serialized object in the common Issue #9
HTTP success envelope. The serializer itself remains framework-independent and
returns data only.
