# Traffic Signal Manager — Integration Notes

## 1. Shared Signal Coordination

All traffic signals at the same intersection must use the same `SignalCoordinator` instance.

The coordinator manages four approaches, initially all RED. An approach may become GREEN only when all other approaches are RED. The required transition sequence is:

**GREEN → YELLOW → RED → next approach GREEN**

A new approach must not become GREEN while any other approach is GREEN or YELLOW.

Example:

```cpp
SignalCoordinator coordinator;

TrafficSignal north(coordinator, 0);
TrafficSignal south(coordinator, 1);
TrafficSignal east(coordinator, 2);
TrafficSignal west(coordinator, 3);
```

All four signals must share this same coordinator instance. Standalone `TrafficSignal` objects do not coordinate with each other and must not be used to represent multiple approaches of the same intersection.

## 2. Timing Ownership

The simulation or integration layer is responsible for deciding when signal transitions occur.

`TrafficSignal` and `SignalCoordinator` do not implement timers or automatically change signal states. The simulation layer must request transitions in the required order:

1. Change the active GREEN signal to YELLOW.
2. Change that signal from YELLOW to RED.
3. Only after the previous approach is RED, request GREEN for the next approach.

If a transition is rejected, the integration layer must handle the failure and must not assume that the signal state changed.

The GUI-facing simulation model in `src/gui_model.cpp` owns the Sprint 1 phase
timer. It advances one logical second per tick, holds GREEN for 10 ticks and
YELLOW for 2 ticks, then rotates North → South → East → West. The model starts
and resets with North GREEN. Its deterministic tick ordering is documented in
`BUILD_TESTING.md`; model tests advance ticks directly without sleeping.

## 3. Reset Behavior

Resetting a standalone signal sets that signal to RED.

Resetting a signal connected to a shared coordinator resets every approach managed by that coordinator to RED.

The integration layer must treat this as an intersection-wide reset. The simulation controller is responsible for coordinating reset operations and resuming the signal cycle safely.

`SimModel` resets the shared coordinator and then explicitly restores North
GREEN. It also drains every queue and removes queued vehicles from the
`VehicleManager` before clearing its counters and restarting the arrival
schedule.

## 4. Issue #5 — TrafficManager Integration Contract

`TrafficManager` uses a `TrafficSignal` reference to determine whether a vehicle may be processed.

The expected processing behavior is:

- If `canMove()` returns `false`, do not dequeue a vehicle.
- If the selected road queue is empty, do not process a vehicle.
- If `canMove()` returns `true` and the queue is not empty, dequeue the next vehicle.
- Preserve the Queue Manager's FIFO ordering.
- All approaches of the same intersection must use signals connected to the same `SignalCoordinator`.

The simulation or integration layer owns signal timing and must ensure that signal transitions follow GREEN → YELLOW → RED before another approach becomes GREEN.

Resetting a coordinated signal resets all approaches, so `TrafficManager` and the simulation controller must treat this operation as intersection-wide rather than as a reset of only one road.

`SimModel` owns one shared coordinator and four `TrafficSignal` instances.
Each road's `TrafficManager` references its corresponding signal and the shared
`QueueManager`; vehicle arrivals are created by `VehicleManager` and enqueued
through the Queue Manager API.

## 5. Local Test Results

The following commands were used for local testing.

### Debug-style build

```powershell
g++ -std=c++17 -Wall -Wextra -pedantic src\TrafficSignal.cpp tests\test_traffic.cpp -o tests\test_traffic.exe
.\tests\test_traffic.exe
```

### Release-style build with `NDEBUG`

```powershell
g++ -std=c++17 -DNDEBUG -Wall -Wextra -pedantic src\TrafficSignal.cpp tests\test_traffic.cpp -o tests\test_traffic_release.exe
.\tests\test_traffic_release.exe
```

Earlier local runs printed:

`All TrafficSignal tests passed!`

The complete Debug and Release test commands, including the deterministic
simulation-model tests, are documented in `BUILD_TESTING.md`. Results from the
current implementation must be recorded only after running those commands.