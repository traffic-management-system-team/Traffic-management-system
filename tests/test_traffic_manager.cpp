
#include "TrafficManager.h"

#include <iostream>
#include <stdexcept>

namespace {
int testsPassed = 0;
int testsFailed = 0;

void check(bool condition, const char* description) {
    if (condition) {
        ++testsPassed;
        std::cout << "[PASS] " << description << '\n';
    } else {
        ++testsFailed;
        std::cerr << "[FAIL] " << description << '\n';
    }
}

void testRedSignalKeepsVehicleQueued() {
    TrafficSignal signal(SignalState::RED);
    QueueManager queue;
    TrafficManager manager(signal, queue);

    Vehicle first(1, VehicleType::Car, "north");
    Vehicle output(99, VehicleType::Bus, "placeholder");

    queue.enqueue("north", first);

    bool processed = manager.processNextVehicle("north", output);

    check(!processed, "RED signal blocks vehicle processing");
    check(queue.getSize("north") == 1,
          "RED signal leaves vehicle in queue");
    check(output.getId() == 99,
          "RED signal leaves output vehicle unchanged");
}

void testYellowSignalKeepsVehicleQueued() {
    TrafficSignal signal(SignalState::RED);
    check(signal.changeState(SignalState::GREEN),
          "Transition RED to GREEN");
    check(signal.changeState(SignalState::YELLOW),
          "Transition GREEN to YELLOW");

    QueueManager queue;
    TrafficManager manager(signal, queue);

    Vehicle first(2, VehicleType::Motorcycle, "east");
    Vehicle output(99, VehicleType::Bus, "placeholder");

    queue.enqueue("east", first);

    bool processed = manager.processNextVehicle("east", output);

    check(!processed, "YELLOW signal blocks vehicle processing");
    check(queue.getSize("east") == 1,
          "YELLOW signal leaves vehicle in queue");
    check(output.getId() == 99,
          "YELLOW signal leaves output vehicle unchanged");
}

void testGreenSignalProcessesOneVehicle() {
    TrafficSignal signal(SignalState::RED);
    check(signal.changeState(SignalState::GREEN),
          "Transition RED to GREEN");

    QueueManager queue;
    TrafficManager manager(signal, queue);

    Vehicle first(10, VehicleType::Car, "south");
    Vehicle output(99, VehicleType::Bus, "placeholder");

    queue.enqueue("south", first);

    bool processed = manager.processNextVehicle("south", output);

    check(processed, "GREEN signal processes a waiting vehicle");
    check(output.getId() == 10,
          "Processed vehicle has the expected ID");
    check(queue.isEmpty("south"),
          "Processed vehicle is removed from queue");
}

void testEmptyQueueIsSafe() {
    TrafficSignal signal(SignalState::RED);
    check(signal.changeState(SignalState::GREEN),
          "Transition RED to GREEN");

    QueueManager queue;
    TrafficManager manager(signal, queue);

    Vehicle output(99, VehicleType::Bus, "placeholder");

    bool processed = manager.processNextVehicle("empty-road", output);

    check(!processed, "Empty queue does not process a vehicle");
    check(output.getId() == 99,
          "Empty queue leaves output vehicle unchanged");
}

void testFifoOrder() {
    TrafficSignal signal(SignalState::RED);
    check(signal.changeState(SignalState::GREEN),
          "Transition RED to GREEN");

    QueueManager queue;
    TrafficManager manager(signal, queue);

    queue.enqueue("west", Vehicle(1, VehicleType::Car, "west"));
    queue.enqueue("west", Vehicle(2, VehicleType::Bus, "west"));
    queue.enqueue("west", Vehicle(3, VehicleType::Truck, "west"));

    Vehicle output(99, VehicleType::Car, "placeholder");

    bool first = manager.processNextVehicle("west", output);
    check(first && output.getId() == 1,
          "FIFO processes the first vehicle first");

    bool second = manager.processNextVehicle("west", output);
    check(second && output.getId() == 2,
          "FIFO processes the second vehicle next");

    bool third = manager.processNextVehicle("west", output);
    check(third && output.getId() == 3,
          "FIFO processes the third vehicle last");

    check(queue.isEmpty("west"),
          "FIFO test leaves the queue empty");
}

void testSharedCoordinatorGatesEachApproach() {
    SignalCoordinator coordinator;
    TrafficSignal north(coordinator, 0);
    TrafficSignal east(coordinator, 1);

    QueueManager queue;
    TrafficManager northManager(north, queue);
    TrafficManager eastManager(east, queue);

    queue.enqueue("north", Vehicle(1, VehicleType::Car, "north"));
    queue.enqueue("east", Vehicle(2, VehicleType::Car, "east"));

    Vehicle output(99, VehicleType::Bus, "placeholder");

    check(north.changeState(SignalState::GREEN),
          "North approach becomes GREEN");

    check(northManager.processNextVehicle("north", output),
          "GREEN north approach processes its vehicle");

    check(!eastManager.processNextVehicle("east", output),
          "RED east approach cannot process its vehicle");

    check(queue.getSize("east") == 1,
          "East vehicle remains queued while its signal is RED");

    check(north.changeState(SignalState::YELLOW),
          "North approach changes to YELLOW");

    check(!northManager.processNextVehicle("north", output),
          "YELLOW north approach cannot process another vehicle");

    check(north.changeState(SignalState::RED),
          "North approach changes to RED");

    check(east.changeState(SignalState::GREEN),
          "East approach becomes GREEN after north is RED");

    check(eastManager.processNextVehicle("east", output),
          "GREEN east approach processes its vehicle");
}
}

int main() {
    testRedSignalKeepsVehicleQueued();
    testYellowSignalKeepsVehicleQueued();
    testGreenSignalProcessesOneVehicle();
    testEmptyQueueIsSafe();
    testFifoOrder();
    testSharedCoordinatorGatesEachApproach();

    std::cout << "\nTests passed: " << testsPassed
              << "\nTests failed: " << testsFailed << '\n';

    if (testsFailed != 0) {
        return 1;
    }

    std::cout << "All TrafficManager tests passed!\n";
    return 0;
}
