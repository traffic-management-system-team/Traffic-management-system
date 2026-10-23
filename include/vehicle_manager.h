
#ifndef VEHICLE_MANAGER_H
#define VEHICLE_MANAGER_H

#include "Vehicle.h"

#include <cstddef>
#include <unordered_map>

class VehicleManager {
public:
    Vehicle createVehicle(VehicleType type, const std::string& lane);

    const Vehicle* findVehicle(int id) const;
    bool removeVehicle(int id);

    std::size_t getVehicleCount() const noexcept;

private:
    int nextId_ = 1;
    std::unordered_map<int, Vehicle> vehicles_;
};

#endif
