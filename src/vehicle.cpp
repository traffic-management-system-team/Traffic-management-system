
#include "Vehicle.h"

#include <stdexcept>
#include <utility>

std::string toString(VehicleType type) {
    switch (type) {
        case VehicleType::Car:
            return "Car";

        case VehicleType::Motorcycle:
            return "Motorcycle";

        case VehicleType::Bus:
            return "Bus";

        case VehicleType::Truck:
            return "Truck";

        case VehicleType::Emergency:
            return "Emergency";
    }

    return "Unknown";
}

Vehicle::Vehicle(int id, VehicleType type, std::string lane)
    : id_(id), type_(type), lane_(std::move(lane)) {

    if (id_ <= 0) {
        throw std::invalid_argument("Vehicle ID must be positive.");
    }

    if (lane_.empty()) {
        throw std::invalid_argument("Vehicle lane must not be empty.");
    }
}

int Vehicle::getId() const noexcept {
    return id_;
}

VehicleType Vehicle::getType() const noexcept {
    return type_;
}

const std::string& Vehicle::getLane() const noexcept {
    return lane_;
}
