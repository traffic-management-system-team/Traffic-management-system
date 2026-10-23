# Traffic Signal Manager — Integration Notes

## 1. Shared Signal Coordination

All traffic signals at the same intersection must use the same `SignalCoordinator` instance. The coordinator manages four approaches, initially all RED, and prevents a second approach from becoming GREEN while another is already GREEN.

Example:

```cpp
SignalCoordinator coordinator;

TrafficSignal north(coordinator, 0);
TrafficSignal south(coordinator, 1);
TrafficSignal east(coordinator, 2);
TrafficSignal west(coordinator, 3);
```

Standalone `TrafficSignal` objects do not coordinate with each other and must not represent multiple approaches of the same intersection.

## 2. Timing Ownership

The simulation or integration layer controls when signal transitions occur. `TrafficSignal` and `SignalCoordinator` do not implement timers or automatically change signal states.

The integration layer must request valid transitions and handle rejected transitions without assuming that the state changed.

## 3. Reset Behavior

Resetting a standalone signal sets it to RED. Resetting a signal connected to a shared coordinator sets every approach managed by that coordinator to RED.

The integration layer must treat this as an intersection-wide reset.

## 4. Issue #5 Integration

`TrafficManager` can use a `TrafficSignal` reference and check `canMove()` before dequeuing a vehicle.

- If `canMove()` is false, do not dequeue a vehicle.
- If the road queue is empty, do not process a vehicle.
- If movement is permitted and the queue is not empty, dequeue the next vehicle.

All approaches of one intersection must share the same coordinator.

## 5. Local Test Results

Debug-style build and test:

```powershell
g++ -std=c++17 -Wall -Wextra -pedantic src\TrafficSignal.cpp tests\test_traffic.cpp -o tests\test_traffic.exe
.\tests\test_traffic.exe
```

Release-style build with `NDEBUG` and test:

```powershell
g++ -std=c++17 -DNDEBUG -Wall -Wextra -pedantic src\TrafficSignal.cpp tests\test_traffic.cpp -o tests\test_traffic_release.exe
.\tests\test_traffic_release.exe
```

Both local runs printed `All TrafficSignal tests passed!`.

Full project integration testing is still pending.