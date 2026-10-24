
#include "vehicle_manager.h"

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
    VehicleManager manager;

    // Test 1: Automatically assign unique IDs
    Vehicle car = manager.createVehicle(VehicleType::Car, "North");
    Vehicle bus = manager.createVehicle(VehicleType::Bus, "East");

    CHECK(car.getId() == 1);
    CHECK(bus.getId() == 2);
    CHECK(manager.getVehicleCount() == 2);

    // Test 2: Find existing vehicles
    CHECK(manager.findVehicle(1) != nullptr);
    CHECK(manager.findVehicle(2) != nullptr);
    CHECK(manager.findVehicle(1)->getType() == VehicleType::Car);

    // Test 3: Missing vehicle
    CHECK(manager.findVehicle(999) == nullptr);

    // Test 4: Remove a vehicle
    CHECK(manager.removeVehicle(1));
    CHECK(manager.findVehicle(1) == nullptr);
    CHECK(manager.getVehicleCount() == 1);

    // Test 5: Cannot remove the same vehicle twice
    CHECK(!manager.removeVehicle(1));

    // Test 6: IDs are not reused after removal
    Vehicle truck = manager.createVehicle(VehicleType::Truck, "South");
    CHECK(truck.getId() == 3);
    CHECK(manager.getVehicleCount() == 2);

    std::cout << "All VehicleManager tests passed.\n";
    return EXIT_SUCCESS;
}
