#include "simulation_statistics_manager.h"

#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

namespace {

void check(bool condition, const char* expression, int line) {
    if (!condition) {
        std::cerr << "CHECK FAILED at line " << line
                  << ": " << expression << '\n';
        std::exit(EXIT_FAILURE);
    }
}

#define CHECK(expression) check((expression), #expression, __LINE__)

void testIndependentCountersAndSnapshot() {
    SimulationStatisticsManager manager;

    CHECK(manager.recordGenerated(SimulationApproach::North));
    CHECK(manager.recordGenerated(SimulationApproach::North));
    CHECK(manager.recordGenerated(SimulationApproach::South));
    CHECK(manager.recordGenerated(SimulationApproach::East));

    CHECK(manager.recordProcessed(SimulationApproach::North, 4));
    CHECK(manager.recordProcessed(SimulationApproach::South, 7));
    CHECK(manager.recordProcessed(SimulationApproach::East, 2));

    const std::array<std::size_t, kSimulationApproachCount> waiting = {
        1, // North: 2 generated, 1 processed
        0, // South: 1 generated, 1 processed
        0, // East: 1 generated, 1 processed
        0  // West: no vehicles
    };

    const SimulationStatistics snapshot = manager.snapshot(10, waiting);

    CHECK(snapshot.tickCount == 10);
    CHECK(snapshot.vehiclesGenerated == 4);
    CHECK(snapshot.vehiclesProcessed == 3);
    CHECK(snapshot.vehiclesWaiting == 1);
    CHECK(snapshot.vehiclesGenerated ==
          snapshot.vehiclesProcessed + snapshot.vehiclesWaiting);
    CHECK(snapshot.totalWaitingTicks == 13);
    CHECK(std::abs(snapshot.averageWaitingTicks - (13.0 / 3.0)) < 1e-12);
    CHECK(std::abs(snapshot.processedPerLogicalTick - 0.3) < 1e-12);

    CHECK(snapshot.approaches[0].vehiclesGenerated == 2);
    CHECK(snapshot.approaches[0].vehiclesProcessed == 1);
    CHECK(snapshot.approaches[0].vehiclesWaiting == 1);

    CHECK(snapshot.approaches[1].vehiclesGenerated == 1);
    CHECK(snapshot.approaches[1].vehiclesProcessed == 1);
    CHECK(snapshot.approaches[1].vehiclesWaiting == 0);

    CHECK(snapshot.approaches[2].vehiclesGenerated == 1);
    CHECK(snapshot.approaches[2].vehiclesProcessed == 1);
    CHECK(snapshot.approaches[2].vehiclesWaiting == 0);

    CHECK(snapshot.approaches[3].vehiclesGenerated == 0);
    CHECK(snapshot.approaches[3].vehiclesProcessed == 0);
    CHECK(snapshot.approaches[3].vehiclesWaiting == 0);
}

void testInvalidApproachAndEmptySnapshot() {
    SimulationStatisticsManager manager;
    const auto invalid = static_cast<SimulationApproach>(255);

    CHECK(!manager.recordGenerated(invalid));
    CHECK(!manager.recordProcessed(invalid, 99));

    const std::array<std::size_t, kSimulationApproachCount> emptyWaiting{};
    const SimulationStatistics empty = manager.snapshot(0, emptyWaiting);

    CHECK(empty.tickCount == 0);
    CHECK(empty.vehiclesGenerated == 0);
    CHECK(empty.vehiclesProcessed == 0);
    CHECK(empty.vehiclesWaiting == 0);
    CHECK(empty.totalWaitingTicks == 0);
    CHECK(empty.averageWaitingTicks == 0.0);
    CHECK(empty.processedPerLogicalTick == 0.0);
}

void testResetClearsEveryCounter() {
    SimulationStatisticsManager manager;
    CHECK(manager.recordGenerated(SimulationApproach::West));
    CHECK(manager.recordProcessed(SimulationApproach::West, 12));

    manager.reset();

    CHECK(manager.generatedCount() == 0);
    CHECK(manager.processedCount() == 0);
    CHECK(manager.totalWaitingTicks() == 0);

    const std::array<std::size_t, kSimulationApproachCount> emptyWaiting{};
    const auto snapshot = manager.snapshot(0, emptyWaiting);
    CHECK(snapshot.vehiclesGenerated == 0);
    CHECK(snapshot.vehiclesProcessed == 0);
    CHECK(snapshot.totalWaitingTicks == 0);
}

} // namespace

int main() {
    testIndependentCountersAndSnapshot();
    testInvalidApproachAndEmptySnapshot();
    testResetClearsEveryCounter();

    std::cout << "All SimulationStatisticsManager tests passed.\n";
    return EXIT_SUCCESS;
}
