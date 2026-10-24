#include "queue_manager.h"
#include <cstdlib>
#include <iostream>

#define CHECK(condition)                                                   \
    do {                                                                   \
        if (!(condition)) {                                                \
            std::cerr << "FAIL: " << #condition                            \
                      << " at " << __FILE__ << ":" << __LINE__ << '\n';    \
            return EXIT_FAILURE;                                           \
        }                                                                  \
    } while (false)

int main() {
    QueueManager queue;
    Vehicle output(999, VehicleType::Car, "Lane X");

    // Test 1: Unknown road should have an empty queue.
    CHECK(queue.isEmpty("UnknownRoad"));
    CHECK(queue.getSize("UnknownRoad") == 0);
    CHECK(!queue.dequeue("UnknownRoad", output));

    // Test 2: Queue becomes empty after removing its only vehicle.
    Vehicle car(1, VehicleType::Car, "Lane A");
    queue.enqueue("North", car);

    CHECK(queue.dequeue("North", output));
    CHECK(output.getId() == 1);
    CHECK(queue.isEmpty("North"));
    CHECK(queue.getSize("North") == 0);
    CHECK(!queue.dequeue("North", output));

    std::cout << "TC-13 empty-queue safety passed.\n";
    return EXIT_SUCCESS;
}