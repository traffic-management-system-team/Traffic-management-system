#ifndef VEHICLE_H
#define VEHICLE_H

#include <string>

enum class VehicleType {
    Car,
    Motorcycle,
    Bus,
    Truck,
    Emergency
};

std::string toString(VehicleType type);

class Vehicle {
public:
    Vehicle(int id, VehicleType type, std::string lane);

    int getId() const noexcept;
    VehicleType getType() const noexcept;
    const std::string& getLane() const noexcept;

private:
    int id_;
    VehicleType type_;
    std::string lane_;
};

#endif