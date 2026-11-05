#ifndef SIMULATION_STATISTICS_H
#define SIMULATION_STATISTICS_H

#include <array>
#include <cstddef>
#include <cstdint>

enum class SimulationApproach : std::size_t {
    North,
    South,
    East,
    West,
    Count
};

constexpr std::size_t kSimulationApproachCount =
    static_cast<std::size_t>(SimulationApproach::Count);

struct ApproachStatistics {
    std::uint64_t vehiclesGenerated = 0;
    std::uint64_t vehiclesProcessed = 0;
    std::size_t vehiclesWaiting = 0;
};

struct SimulationStatistics {
    std::uint64_t tickCount = 0;
    std::uint64_t vehiclesGenerated = 0;
    std::uint64_t vehiclesProcessed = 0;
    std::size_t vehiclesWaiting = 0;
    std::uint64_t totalWaitingTicks = 0;
    double averageWaitingTicks = 0.0;
    double processedPerLogicalTick = 0.0;
    std::array<ApproachStatistics, kSimulationApproachCount> approaches{};
};

#endif
