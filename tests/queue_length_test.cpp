
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

    CHECK(queue.getSize("North") == 0);

    Vehicle car(1, VehicleType::Car, "Lane A");
    Vehicle bus(2, VehicleType::Bus, "Lane B");

    queue.enqueue("North", car);
    CHECK(queue.getSize("North") == 1);

    queue.enqueue("North", bus);
    CHECK(queue.getSize("North") == 2);

    CHECK(queue.dequeue("North", output));
    CHECK(queue.getSize("North") == 1);

    CHECK(queue.dequeue("North", output));
    CHECK(queue.getSize("North") == 0);

    CHECK(queue.isEmpty("North"));

    std::cout << "Queue length tests passed.\n";
    return EXIT_SUCCESS;
}
