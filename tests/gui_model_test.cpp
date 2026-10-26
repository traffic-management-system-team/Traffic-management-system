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

void testInvalidRoads() {
    const SimModel model;
    CHECK(model.queueLength(-1) == 0);
    CHECK(model.queueLength(99) == 0);
    CHECK(model.signal(-1) == SimModel::Signal::Red);
    CHECK(model.signal(99) == SimModel::Signal::Red);
}
}

int main() {
    testInitialStateAndSignalTiming();
    testArrivalScheduleAndProcessing();
    testPauseAndReset();
    testInvalidRoads();
    std::cout << "All simulation model tests passed.\n";
    return EXIT_SUCCESS;
}
