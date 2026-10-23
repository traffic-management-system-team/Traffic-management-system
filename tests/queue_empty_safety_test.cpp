
#include "Queue_manager.h"

#include <cassert>
#include <iostream>

int main() {
    QueueManager queue;
    Vehicle output(999, VehicleType::Car, "Test");

    // TC-13: Empty-queue safety
    assert(queue.isEmpty("UnknownRoad"));
    assert(queue.getSize("UnknownRoad") == 0);
    assert(!queue.dequeue("UnknownRoad", output));

    // Also verify a queue after its final vehicle is removed.
    Vehicle car(1, VehicleType::Car, "North");
    queue.enqueue("North", car);

    assert(queue.dequeue("North", output));
    assert(queue.isEmpty("North"));
    assert(queue.getSize("North") == 0);
    assert(!queue.dequeue("North", output));

    std::cout << "TC-13 empty-queue safety passed.\n";

    return 0;
}
