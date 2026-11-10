#include "simulation_statistics_manager.h"

bool SimulationStatisticsManager::indexOf(
    SimulationApproach approach,
    std::size_t& index
) noexcept {
    index = static_cast<std::size_t>(approach);
    return index < kSimulationApproachCount;
}

bool SimulationStatisticsManager::recordGenerated(
    SimulationApproach approach
) noexcept {
    std::size_t index = 0;
    if (!indexOf(approach, index)) {
        return false;
    }
    ++generated_[index];
    return true;
}

bool SimulationStatisticsManager::recordProcessed(
    SimulationApproach approach,
    std::uint64_t waitingTicks
) noexcept {
    std::size_t index = 0;
    if (!indexOf(approach, index)) {
        return false;
    }
    ++processed_[index];
    totalWaitingTicks_ += waitingTicks;
    return true;
}

void SimulationStatisticsManager::reset() noexcept {
    generated_.fill(0);
    processed_.fill(0);
    totalWaitingTicks_ = 0;
}

std::uint64_t SimulationStatisticsManager::generatedCount() const noexcept {
    std::uint64_t total = 0;
    for (const auto count : generated_) {
        total += count;
    }
    return total;
}

std::uint64_t SimulationStatisticsManager::processedCount() const noexcept {
    std::uint64_t total = 0;
    for (const auto count : processed_) {
        total += count;
    }
    return total;
}

std::uint64_t SimulationStatisticsManager::totalWaitingTicks() const noexcept {
    return totalWaitingTicks_;
}

SimulationStatistics SimulationStatisticsManager::snapshot(
    std::uint64_t tickCount,
    const std::array<std::size_t, kSimulationApproachCount>& waitingByApproach
) const noexcept {
    SimulationStatistics result;
    result.tickCount = tickCount;
    result.vehiclesGenerated = generatedCount();
    result.vehiclesProcessed = processedCount();
    result.totalWaitingTicks = totalWaitingTicks_;

    for (std::size_t approach = 0;
         approach < kSimulationApproachCount;
         ++approach) {
        auto& approachStats = result.approaches[approach];
        approachStats.vehiclesGenerated = generated_[approach];
        approachStats.vehiclesProcessed = processed_[approach];
        approachStats.vehiclesWaiting = waitingByApproach[approach];
        result.vehiclesWaiting += waitingByApproach[approach];
    }

    if (result.vehiclesProcessed != 0) {
        result.averageWaitingTicks =
            static_cast<double>(result.totalWaitingTicks) /
            static_cast<double>(result.vehiclesProcessed);
    }

    if (tickCount != 0) {
        result.processedPerLogicalTick =
            static_cast<double>(result.vehiclesProcessed) /
            static_cast<double>(tickCount);
    }

    return result;
}
