#include "simulation_state_serializer.h"

#include <array>
#include <cstddef>
#include <utility>

namespace {

using Json = nlohmann::json;

constexpr std::array<const char*, SimModel::kRoads> kRoadIds = {
    "north", "east", "south", "west"
};

constexpr std::array<const char*, SimModel::kRoads> kRoadNames = {
    "North", "East", "South", "West"
};

constexpr std::array<const char*, kSimulationApproachCount>
    kStatisticsRoadIds = {
        "north", "south", "east", "west"
    };

const char* stateToString(SimModel::State state) noexcept {
    switch (state) {
        case SimModel::State::Stopped:
            return "stopped";
        case SimModel::State::Running:
            return "running";
        case SimModel::State::Paused:
            return "paused";
    }

    return "unknown";
}

const char* signalToString(SimModel::Signal signal) noexcept {
    switch (signal) {
        case SimModel::Signal::Red:
            return "red";
        case SimModel::Signal::Yellow:
            return "yellow";
        case SimModel::Signal::Green:
            return "green";
    }

    return "unknown";
}

Json serializeApproaches(const SimulationSnapshot& snapshot) {
    Json approaches = Json::array();

    for (std::size_t i = 0; i < snapshot.queues.size(); ++i) {
        const auto& queue = snapshot.queues[i];
        Json vehicles = Json::array();

        for (const auto& vehicle : queue.vehicles) {
            vehicles.push_back({
                {"id", vehicle.id},
                {"type", toString(vehicle.type)},
                {"lane", vehicle.lane}
            });
        }

        approaches.push_back({
            {"id", kRoadIds[i]},
            {"name", kRoadNames[i]},
            {"signal", signalToString(snapshot.signals[i])},
            {"queueLength", queue.vehicles.size()},
            {"vehicles", std::move(vehicles)}
        });
    }

    return approaches;
}

Json serializeApproachStatistics(const SimulationStatistics& statistics) {
    Json approaches = Json::array();

    for (std::size_t i = 0; i < statistics.approaches.size(); ++i) {
        const auto& approach = statistics.approaches[i];
        approaches.push_back({
            {"id", kStatisticsRoadIds[i]},
            {"vehiclesGenerated", approach.vehiclesGenerated},
            {"vehiclesProcessed", approach.vehiclesProcessed},
            {"vehiclesWaiting", approach.vehiclesWaiting}
        });
    }

    return approaches;
}

} // namespace

nlohmann::json serializeSimulationStateData(
    const SimulationSnapshot& snapshot
) {
    const bool activeApproachIsValid =
        snapshot.activeApproach < snapshot.signals.size();

    const char* activeApproach = activeApproachIsValid
        ? kRoadIds[snapshot.activeApproach]
        : "unknown";
    const char* activeSignal = activeApproachIsValid
        ? signalToString(snapshot.signals[snapshot.activeApproach])
        : "unknown";
    const auto& statistics = snapshot.statistics;

    return {
        {"state", stateToString(snapshot.state)},
        {"tickCount", statistics.tickCount},
        {"activeApproach", activeApproach},
        {"activeSignal", activeSignal},
        {"approaches", serializeApproaches(snapshot)},
        {"config", {
            {"tickMs", snapshot.config.tickMs},
            {"greenTicks", snapshot.config.greenTicks},
            {"yellowTicks", snapshot.config.yellowTicks},
            {"arrivalInterval", snapshot.config.arrivalInterval},
            {"maxQueue", snapshot.config.maxQueue}
        }},
        {"statistics", {
            {"tickCount", statistics.tickCount},
            {"vehiclesGenerated", statistics.vehiclesGenerated},
            {"vehiclesProcessed", statistics.vehiclesProcessed},
            {"vehiclesWaiting", statistics.vehiclesWaiting},
            {"totalWaitingTicks", statistics.totalWaitingTicks},
            {"averageWaitingTicks", statistics.averageWaitingTicks},
            {"processedPerLogicalTick", statistics.processedPerLogicalTick},
            {"approaches", serializeApproachStatistics(statistics)}
        }}
    };
}
