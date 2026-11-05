#include "gui_model.h"

#include <cstdlib>
#include <iostream>

#define CHECK(condition) do { \
    if (!(condition)) { \
        std::cerr << "CHECK failed: " #condition \
                  << " (line " << __LINE__ << ")\n"; \
        std::exit(EXIT_FAILURE); \
    } \
} while (false)

namespace {
void runTicks(SimModel& model, int count) {
    for (int i = 0; i < count; ++i) {
        CHECK(model.tick());
    }
}

void checkOnlyExpectedGreen(const SimModel& model, int expectedRoad) {
    int greenCount = 0;
    for (int road = 0; road < SimModel::kRoads; ++road) {
        if (model.signal(road) == SimModel::Signal::Green) {
            ++greenCount;
            CHECK(road == expectedRoad);
        }
    }
    CHECK(greenCount == 1);
}

bool sameStatistics(
    const SimulationStatistics& left,
    const SimulationStatistics& right
) {
    if (left.tickCount != right.tickCount ||
        left.vehiclesGenerated != right.vehiclesGenerated ||
        left.vehiclesProcessed != right.vehiclesProcessed ||
        left.vehiclesWaiting != right.vehiclesWaiting ||
        left.totalWaitingTicks != right.totalWaitingTicks ||
        left.averageWaitingTicks != right.averageWaitingTicks ||
        left.processedPerLogicalTick != right.processedPerLogicalTick) {
        return false;
    }

    for (std::size_t i = 0; i < kSimulationApproachCount; ++i) {
        if (left.approaches[i].vehiclesGenerated !=
                right.approaches[i].vehiclesGenerated ||
            left.approaches[i].vehiclesProcessed !=
                right.approaches[i].vehiclesProcessed ||
            left.approaches[i].vehiclesWaiting !=
                right.approaches[i].vehiclesWaiting) {
            return false;
        }
    }
    return true;
}

void testInitialStatistics() {
    SimModel model;
    const SimulationStatistics stats = model.statistics();

    CHECK(stats.tickCount == 0);
    CHECK(stats.vehiclesGenerated == 0);
    CHECK(stats.vehiclesProcessed == 0);
    CHECK(stats.vehiclesWaiting == 0);
    CHECK(stats.totalWaitingTicks == 0);
    CHECK(stats.averageWaitingTicks == 0.0);
    CHECK(stats.processedPerLogicalTick == 0.0);
    for (const auto& approach : stats.approaches) {
        CHECK(approach.vehiclesGenerated == 0);
        CHECK(approach.vehiclesProcessed == 0);
        CHECK(approach.vehiclesWaiting == 0);
    }
}

void testStatisticsContract() {
    SimModel model;
    model.start();

    runTicks(model, 3);
    SimulationStatistics stats = model.statistics();
    CHECK(stats.tickCount == 3);
    CHECK(stats.vehiclesGenerated == 1);
    CHECK(stats.vehiclesProcessed == 0);
    CHECK(stats.vehiclesWaiting == 1);
    CHECK(stats.approaches[static_cast<std::size_t>(
              SimulationApproach::North)].vehiclesGenerated == 1);
    CHECK(stats.approaches[static_cast<std::size_t>(
              SimulationApproach::North)].vehiclesWaiting == 1);

    runTicks(model, 1);
    stats = model.statistics();
    CHECK(stats.tickCount == 4);
    CHECK(stats.vehiclesGenerated == 1);
    CHECK(stats.vehiclesProcessed == 1);
    CHECK(stats.vehiclesWaiting == 0);
    CHECK(stats.totalWaitingTicks == 1);
    CHECK(stats.averageWaitingTicks == 1.0);
    CHECK(stats.processedPerLogicalTick == 0.25);
    CHECK(stats.approaches[static_cast<std::size_t>(
              SimulationApproach::North)].vehiclesProcessed == 1);

    runTicks(model, 9);
    stats = model.statistics();
    CHECK(stats.tickCount == 13);
    CHECK(stats.vehiclesGenerated == 4);
    CHECK(stats.vehiclesProcessed == 2);
    CHECK(stats.vehiclesWaiting == 2);
    CHECK(stats.totalWaitingTicks == 8);
    CHECK(stats.averageWaitingTicks == 4.0);
    CHECK(stats.approaches[static_cast<std::size_t>(
              SimulationApproach::North)].vehiclesGenerated == 1);
    CHECK(stats.approaches[static_cast<std::size_t>(
              SimulationApproach::North)].vehiclesProcessed == 1);
    CHECK(stats.approaches[static_cast<std::size_t>(
              SimulationApproach::South)].vehiclesGenerated == 1);
    CHECK(stats.approaches[static_cast<std::size_t>(
              SimulationApproach::South)].vehiclesProcessed == 1);
    CHECK(stats.approaches[static_cast<std::size_t>(
              SimulationApproach::East)].vehiclesWaiting == 1);
    CHECK(stats.approaches[static_cast<std::size_t>(
              SimulationApproach::West)].vehiclesWaiting == 1);

    const SimulationStatistics beforeReads = model.statistics();
    const std::uint64_t tickBeforeReads = model.tickCount();
    const std::size_t northQueueBeforeReads = model.queueLength(0);
    for (int i = 0; i < 10; ++i) {
        CHECK(sameStatistics(beforeReads, model.statistics()));
    }
    CHECK(model.tickCount() == tickBeforeReads);
    CHECK(model.queueLength(0) == northQueueBeforeReads);

    model.togglePause();
    const SimulationStatistics pausedStats = model.statistics();
    CHECK(!model.tick());
    CHECK(sameStatistics(pausedStats, model.statistics()));
    model.togglePause();
    CHECK(model.tick());
    CHECK(model.statistics().tickCount == 14);

    const SimConfig configBeforeReset = model.config();
    model.reset();
    CHECK(model.config().tickMs == configBeforeReset.tickMs);
    CHECK(model.config().greenTicks == configBeforeReset.greenTicks);
    CHECK(model.config().yellowTicks == configBeforeReset.yellowTicks);
    CHECK(model.config().arrivalInterval == configBeforeReset.arrivalInterval);
    CHECK(model.config().maxQueue == configBeforeReset.maxQueue);
    stats = model.statistics();
    CHECK(stats.tickCount == 0);
    CHECK(stats.vehiclesGenerated == 0);
    CHECK(stats.vehiclesProcessed == 0);
    CHECK(stats.vehiclesWaiting == 0);
    CHECK(stats.totalWaitingTicks == 0);
    CHECK(stats.averageWaitingTicks == 0.0);
    CHECK(stats.processedPerLogicalTick == 0.0);
    for (const auto& approach : stats.approaches) {
        CHECK(approach.vehiclesGenerated == 0);
        CHECK(approach.vehiclesProcessed == 0);
        CHECK(approach.vehiclesWaiting == 0);
    }
}

void testInitialStateAndSignalTiming() {
    SimModel model;
    CHECK(model.state() == SimModel::State::Stopped);
    CHECK(model.tickCount() == 0);
    checkOnlyExpectedGreen(model, 0);

    model.start();
    CHECK(model.state() == SimModel::State::Running);
    runTicks(model, 9);
    CHECK(model.tickCount() == 9);
    checkOnlyExpectedGreen(model, 0);

    runTicks(model, 1);
    CHECK(model.tickCount() == 10);
    CHECK(model.signal(0) == SimModel::Signal::Yellow);
    for (int road = 1; road < SimModel::kRoads; ++road) {
        CHECK(model.signal(road) == SimModel::Signal::Red);
    }

    runTicks(model, 1);
    CHECK(model.signal(0) == SimModel::Signal::Yellow);
    runTicks(model, 1);
    CHECK(model.tickCount() == 12);
    checkOnlyExpectedGreen(model, 2);

    runTicks(model, 10);
    CHECK(model.tickCount() == 22);
    CHECK(model.signal(2) == SimModel::Signal::Yellow);
    runTicks(model, 2);
    CHECK(model.tickCount() == 24);
    checkOnlyExpectedGreen(model, 1);

    runTicks(model, 12);
    CHECK(model.tickCount() == 36);
    checkOnlyExpectedGreen(model, 3);

    runTicks(model, 12);
    CHECK(model.tickCount() == 48);
    checkOnlyExpectedGreen(model, 0);

    for (int i = 0; i < 60; ++i) {
        CHECK(model.tick());
        int greenCount = 0;
        int activeCount = 0;
        for (int road = 0; road < SimModel::kRoads; ++road) {
            const SimModel::Signal state = model.signal(road);
            greenCount += state == SimModel::Signal::Green;
            activeCount += state != SimModel::Signal::Red;
        }
        CHECK(greenCount <= 1);
        CHECK(activeCount == 1);
    }
}

void testArrivalScheduleAndProcessing() {
    SimModel model;
    model.start();

    runTicks(model, 2);
    CHECK(model.generatedCount() == 0);
    for (int road = 0; road < SimModel::kRoads; ++road) {
        CHECK(model.queueLength(road) == 0);
    }

    runTicks(model, 1);
    CHECK(model.tickCount() == 3);
    CHECK(model.generatedCount() == 1);
    CHECK(model.queueLength(0) == 1);
    CHECK(model.queueLength(1) == 0);
    CHECK(model.queueLength(2) == 0);
    CHECK(model.queueLength(3) == 0);

    runTicks(model, 1);
    CHECK(model.processedCount() == 1);
    CHECK(model.totalWaitingTicks() == 1);
    CHECK(model.queueLength(0) == 0);

    runTicks(model, 2);
    CHECK(model.tickCount() == 6);
    CHECK(model.generatedCount() == 2);
    CHECK(model.queueLength(2) == 1);
    CHECK(model.queueLength(0) == 0);

    runTicks(model, 3);
    CHECK(model.tickCount() == 9);
    CHECK(model.generatedCount() == 3);
    CHECK(model.queueLength(1) == 1);

    runTicks(model, 3);
    CHECK(model.tickCount() == 12);
    CHECK(model.generatedCount() == 4);
    CHECK(model.queueLength(3) == 1);

    runTicks(model, 1);
    CHECK(model.processedCount() == 2);
    CHECK(model.queueLength(2) == 0);
    CHECK(model.queueLength(3) == 1);
}

void testPauseAndReset() {
    SimModel model;
    CHECK(!model.tick());
    model.togglePause();
    CHECK(model.state() == SimModel::State::Stopped);

    model.start();
    runTicks(model, 6);
    model.togglePause();
    CHECK(model.state() == SimModel::State::Paused);
    const std::uint64_t pausedAt = model.tickCount();
    CHECK(!model.tick());
    CHECK(model.tickCount() == pausedAt);

    model.reset();
    CHECK(model.state() == SimModel::State::Stopped);
    CHECK(model.tickCount() == 0);
    CHECK(model.generatedCount() == 0);
    CHECK(model.processedCount() == 0);
    CHECK(model.totalWaitingTicks() == 0);
    checkOnlyExpectedGreen(model, 0);
    for (int road = 0; road < SimModel::kRoads; ++road) {
        CHECK(model.queueLength(road) == 0);
    }

    model.start();
    runTicks(model, 2);
    CHECK(model.generatedCount() == 0);
    runTicks(model, 1);
    CHECK(model.generatedCount() == 1);
    CHECK(model.queueLength(0) == 1);
    CHECK(model.queueLength(1) == 0);
    CHECK(model.queueLength(2) == 0);
    CHECK(model.queueLength(3) == 0);

    model.reset();
    CHECK(model.tickCount() == 0);
    CHECK(model.generatedCount() == 0);
    CHECK(model.processedCount() == 0);
    CHECK(model.totalWaitingTicks() == 0);
    checkOnlyExpectedGreen(model, 0);
}

void testPauseResumeRegression() {
    SimModel model;
    model.start();
    runTicks(model, 6);

    model.togglePause();
    CHECK(model.state() == SimModel::State::Paused);
    const std::uint64_t pausedAt = model.tickCount();

    for (int i = 0; i < 5; ++i) {
        CHECK(!model.tick());
        CHECK(model.tickCount() == pausedAt);
    }
    checkOnlyExpectedGreen(model, 0);

    model.togglePause();
    CHECK(model.state() == SimModel::State::Running);
    runTicks(model, 3);
    CHECK(model.tickCount() == pausedAt + 3);
    checkOnlyExpectedGreen(model, 0);

    runTicks(model, 1);
    CHECK(model.tickCount() == pausedAt + 4);
    CHECK(model.signal(0) == SimModel::Signal::Yellow);

    runTicks(model, 2);
    CHECK(model.tickCount() == pausedAt + 6);
    checkOnlyExpectedGreen(model, 2);
}

void testCustomConfigurationTiming() {
    SimConfig config;
    config.greenTicks = 2;
    config.yellowTicks = 1;
    config.arrivalInterval = 2;
    config.maxQueue = 5;

    SimModel model(config);
    model.start();

    runTicks(model, 2);

    CHECK(model.signal(0) == SimModel::Signal::Yellow);
    CHECK(model.generatedCount() == 1);
    CHECK(model.queueLength(0) == 1);

    runTicks(model, 1);
    checkOnlyExpectedGreen(model, 2);

    runTicks(model, 1);
    CHECK(model.generatedCount() == 2);
    CHECK(model.queueLength(2) == 1);

    runTicks(model, 2);
    CHECK(model.generatedCount() == 3);
    checkOnlyExpectedGreen(model, 1);
}

void testMaximumQueueCapacity() {
    SimConfig config;
    config.greenTicks = 1;
    config.yellowTicks = 10;
    config.arrivalInterval = 1;
    config.maxQueue = 1;

    SimModel model(config);
    model.start();

    runTicks(model, 5);

    CHECK(model.generatedCount() == 4);
    CHECK(model.queueLength(0) == 1);

    for (int road = 0; road < SimModel::kRoads; ++road) {
        CHECK(model.queueLength(road) <= 1);
    }

    runTicks(model, 25);

    for (int road = 0; road < SimModel::kRoads; ++road) {
        CHECK(model.queueLength(road) <= 1);
    }
}

void testConfigurationChanges() {
    SimModel model;

    SimConfig config;
    config.greenTicks = 5;
    config.yellowTicks = 2;
    config.arrivalInterval = 2;
    config.maxQueue = 5;

    CHECK(model.setConfig(config));
    CHECK(model.config().greenTicks == 5);
    CHECK(model.config().yellowTicks == 2);
    CHECK(model.config().arrivalInterval == 2);
    CHECK(model.config().maxQueue == 5);

    SimConfig invalidConfig = config;
    invalidConfig.maxQueue = 0;

    CHECK(!model.setConfig(invalidConfig));

    model.start();
    runTicks(model, 1);

    CHECK(!model.setConfig(config));
}

void testInvalidRoads() {
    const SimModel model;
    CHECK(model.queueLength(-1) == 0);
    CHECK(model.queueLength(99) == 0);
    CHECK(model.signal(-1) == SimModel::Signal::Red);
    CHECK(model.signal(99) == SimModel::Signal::Red);
}
}

int main() {
    testInitialStatistics();
    testStatisticsContract();
    testInitialStateAndSignalTiming();
    testArrivalScheduleAndProcessing();
    testPauseAndReset();
    testPauseResumeRegression();
    testCustomConfigurationTiming();
    testMaximumQueueCapacity();
    testConfigurationChanges();
    testInvalidRoads();
}
