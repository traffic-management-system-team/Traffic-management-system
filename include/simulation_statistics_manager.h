#ifndef SIMULATION_STATISTICS_MANAGER_H
#define SIMULATION_STATISTICS_MANAGER_H

#include "simulation_statistics.h"

#include <array>
#include <cstddef>
#include <cstdint>

// Owns aggregate traffic counters independently of the simulation lifecycle,
// queue implementation, and HTTP/serialization layers.
class SimulationStatisticsManager {
public:
    bool recordGenerated(SimulationApproach approach) noexcept;
    bool recordProcessed(
        SimulationApproach approach,
        std::uint64_t waitingTicks
    ) noexcept;

    void reset() noexcept;

    std::uint64_t generatedCount() const noexcept;
    std::uint64_t processedCount() const noexcept;
    std::uint64_t totalWaitingTicks() const noexcept;

    // waitingByApproach uses SimulationApproach order:
    // North, South, East, West.
    SimulationStatistics snapshot(
        std::uint64_t tickCount,
        const std::array<std::size_t, kSimulationApproachCount>&
            waitingByApproach
    ) const noexcept;

private:
    static bool indexOf(
        SimulationApproach approach,
        std::size_t& index
    ) noexcept;

    std::array<std::uint64_t, kSimulationApproachCount> generated_{};
    std::array<std::uint64_t, kSimulationApproachCount> processed_{};
    std::uint64_t totalWaitingTicks_ = 0;
};

#endif
