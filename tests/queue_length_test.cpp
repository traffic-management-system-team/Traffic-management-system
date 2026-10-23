
#include "queue_manager.h"

#include <cassert>
#include <iostream>

int main() {
    QueueManager queue;

    Vehicle car(1, VehicleType::Car, "North");
    Vehicle bus(2, VehicleType::Bus, "North");
    Vehicle output(999, VehicleType::Car, "Test");

    // TC-06: Queue length updates correctly.
    assert(queue.getSize("North") == 0);

    queue.enqueue("North", car);
    assert(queue.getSize("North") == 1);

    queue.enqueue("North", bus);
    assert(queue.getSize("North") == 2);

    assert(queue.dequeue("North", output));
    assert(queue.getSize("North") == 1);

    assert(queue.dequeue("North", output));
    assert(queue.getSize("North") == 0);

    std::cout << "TC-06 queue length update passed.\n";

    return 0;
}
