#ifndef SIMULATION_DTO_H
#define SIMULATION_DTO_H

#include "sim_config.h"
#include "simulation_model.h"
#include "simulation_statistics.h"
#include "vehicle.h"

#include <array>
#include <cstddef>
#include <string>
#include <vector>

// Independent copy of a currently waiting vehicle.
struct VehicleSnapshot {
    int id = 0;
    VehicleType type = VehicleType::Car;
    std::string lane;
};

// Vehicles are stored in FIFO order.
struct QueueSnapshot {
    std::string roadId;
    std::vector<VehicleSnapshot> vehicles;
};

// Combined, consistent simulation snapshot.
// Road-index order: North, East, South, West.
// Statistics retain their existing approach ordering.
struct SimulationSnapshot {
    SimModel::State state = SimModel::State::Stopped;
    std::size_t activeApproach = 0;
    std::array<SimModel::Signal, SimModel::kRoads> signals{};
    std::array<QueueSnapshot, SimModel::kRoads> queues{};
    SimulationStatistics statistics;
    SimConfig config;
};

#endif
