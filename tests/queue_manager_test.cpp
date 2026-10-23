
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

    // Test 1: Unknown road should be empty
    CHECK(queue.isEmpty("Road A"));
    CHECK(queue.getSize("Road A") == 0);
    CHECK(!queue.dequeue("Road A", output));

    // Test 2: Add vehicles to two different roads
    Vehicle car(101, VehicleType::Car, "Lane A");
    Vehicle bus(102, VehicleType::Bus, "Lane B");
    Vehicle bike(103, VehicleType::Motorcycle, "Lane A");

    queue.enqueue("Road A", car);
    queue.enqueue("Road B", bus);
    queue.enqueue("Road A", bike);

    CHECK(queue.getSize("Road A") == 2);
    CHECK(queue.getSize("Road B") == 1);

    // Test 3: FIFO order on Road A
    CHECK(queue.dequeue("Road A", output));
    CHECK(output.getId() == 101);

    CHECK(queue.dequeue("Road A", output));
    CHECK(output.getId() == 103);

    // Test 4: Road B remains unaffected
    CHECK(queue.getSize("Road B") == 1);
    CHECK(queue.dequeue("Road B", output));
    CHECK(output.getId() == 102);

    // Test 5: Empty-queue safety after removal
    CHECK(queue.isEmpty("Road A"));
    CHECK(queue.isEmpty("Road B"));
    CHECK(!queue.dequeue("Road A", output));

    std::cout << "All multi-road QueueManager tests passed.\n";
    return EXIT_SUCCESS;
}
