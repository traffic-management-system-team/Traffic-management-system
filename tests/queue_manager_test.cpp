
#include "queue_manager.h"

#include <cassert>
#include <iostream>

int main() {
    QueueManager queue;
    Vehicle output(999, VehicleType::Car, "Lane X");

    // Test 1: Unknown road should be empty
    assert(queue.isEmpty("Road A"));
    assert(queue.getSize("Road A") == 0);
    assert(!queue.dequeue("Road A", output));

    // Test 2: Add vehicles to two different roads
    Vehicle car(101, VehicleType::Car, "Lane A");
    Vehicle bus(102, VehicleType::Bus, "Lane B");
    Vehicle bike(103, VehicleType::Motorcycle, "Lane A");

    queue.enqueue("Road A", car);
    queue.enqueue("Road B", bus);
    queue.enqueue("Road A", bike);

    assert(queue.getSize("Road A") == 2);
    assert(queue.getSize("Road B") == 1);

    // Test 3: FIFO order on Road A
    assert(queue.dequeue("Road A", output));
    assert(output.getId() == 101);

    assert(queue.dequeue("Road A", output));
    assert(output.getId() == 103);

    // Test 4: Road B remains unaffected
    assert(queue.getSize("Road B") == 1);
    assert(queue.dequeue("Road B", output));
    assert(output.getId() == 102);

    // Test 5: Empty-queue safety after removal
    assert(queue.isEmpty("Road A"));
    assert(queue.isEmpty("Road B"));
    assert(!queue.dequeue("Road A", output));

    std::cout << "All multi-road QueueManager tests passed.\n";

    return 0;
}
