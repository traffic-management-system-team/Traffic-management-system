
#include "vehicle_manager.h"

#include <cassert>
#include <iostream>

int main() {
    VehicleManager manager;

    // Test 1: Automatically assign unique IDs
    Vehicle car = manager.createVehicle(VehicleType::Car, "North");
    Vehicle bus = manager.createVehicle(VehicleType::Bus, "East");

    assert(car.getId() == 1);
    assert(bus.getId() == 2);
    assert(manager.getVehicleCount() == 2);

    // Test 2: Find existing vehicles
    assert(manager.findVehicle(1) != nullptr);
    assert(manager.findVehicle(2) != nullptr);
    assert(manager.findVehicle(1)->getType() == VehicleType::Car);

    // Test 3: Missing vehicle
    assert(manager.findVehicle(999) == nullptr);

    // Test 4: Remove a vehicle
    assert(manager.removeVehicle(1));
    assert(manager.findVehicle(1) == nullptr);
    assert(manager.getVehicleCount() == 1);

    // Test 5: Cannot remove the same vehicle twice
    assert(!manager.removeVehicle(1));

    // Test 6: IDs are not reused after removal
    Vehicle truck = manager.createVehicle(VehicleType::Truck, "South");
    assert(truck.getId() == 3);
    assert(manager.getVehicleCount() == 2);

    std::cout << "All VehicleManager tests passed.\n";
    return 0;
}
