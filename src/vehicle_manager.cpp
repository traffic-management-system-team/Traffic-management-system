
#include "vehicle_manager.h"

#include <limits>
#include <stdexcept>

Vehicle VehicleManager::createVehicle(
    VehicleType type,
    const std::string& lane
) {
    if (nextId_ == std::numeric_limits<int>::max()) {
        throw std::overflow_error("Vehicle ID limit reached.");
    }

    Vehicle vehicle(nextId_, type, lane);

    auto result = vehicles_.emplace(nextId_, vehicle);

    if (!result.second) {
        throw std::runtime_error("Duplicate vehicle ID detected.");
    }

    ++nextId_;

    return vehicle;
}

const Vehicle* VehicleManager::findVehicle(int id) const {
    auto it = vehicles_.find(id);

    if (it == vehicles_.end()) {
        return nullptr;
    }

    return &it->second;
}

bool VehicleManager::removeVehicle(int id) {
    return vehicles_.erase(id) > 0;
}

std::size_t VehicleManager::getVehicleCount() const noexcept {
    return vehicles_.size();
}
