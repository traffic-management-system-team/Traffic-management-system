#include "gui_model.h"
#include "config_controller.h"

#include <cstdlib>
#include <iostream>
#include <string>

#define CHECK(condition) do { \
    if (!(condition)) { \
        std::cerr << "CHECK failed: " #condition \
                  << " (line " << __LINE__ << ")\n"; \
        std::exit(EXIT_FAILURE); \
    } \
} while (false)

namespace {
bool sameConfig(const SimConfig& a, const SimConfig& b) {
    return a.tickMs == b.tickMs &&
           a.greenTicks == b.greenTicks &&
           a.yellowTicks == b.yellowTicks &&
           a.arrivalInterval == b.arrivalInterval &&
           a.maxQueue == b.maxQueue;
}

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
    CHECK(sameConfig(model.config(), config));

    model.start();
    runTicks(model, 1);

    CHECK(!model.setConfig(config));
    CHECK(sameConfig(model.config(), config));
}

void testConfigurationControllerWithModel() {
    ConfigController controller;
    SimModel model;
    const SimConfig defaults;
    const RawConfig acceptedInput{"2000", "5", "3", "4", "8"};
    const RawConfig nextInput{"3000", "6", "4", "5", "9"};
    std::string feedback;

    CHECK(controller.submit(
        acceptedInput,
        [&model](const SimConfig& candidate) {
            return model.setConfig(candidate);
        },
        feedback
    ));
    const SimConfig accepted = controller.current();
    CHECK(sameConfig(accepted, model.config()));
    CHECK(accepted.tickMs == 2000 && accepted.greenTicks == 5);
    CHECK(controller.acceptedCount() == 1 && controller.rejectedCount() == 0);

    CHECK(!controller.submit(
        RawConfig{"2000", "0", "3", "4", "8"},
        [&model](const SimConfig& candidate) {
            return model.setConfig(candidate);
        },
        feedback
    ));
    CHECK(sameConfig(controller.current(), accepted));
    CHECK(sameConfig(model.config(), accepted));
    CHECK(controller.acceptedCount() == 1 && controller.rejectedCount() == 1);

    model.start();
    CHECK(!controller.submit(
        nextInput,
        [&model](const SimConfig& candidate) {
            return model.setConfig(candidate);
        },
        feedback
    ));
    CHECK(model.state() == SimModel::State::Running);
    CHECK(sameConfig(controller.current(), accepted));
    CHECK(sameConfig(model.config(), accepted));
    CHECK(controller.acceptedCount() == 1 && controller.rejectedCount() == 2);

    runTicks(model, 1);
    model.togglePause();
    CHECK(model.state() == SimModel::State::Paused);
    CHECK(!controller.submit(
        nextInput,
        [&model](const SimConfig& candidate) {
            return model.setConfig(candidate);
        },
        feedback
    ));
    CHECK(sameConfig(controller.current(), accepted));
    CHECK(sameConfig(model.config(), accepted));
    CHECK(controller.acceptedCount() == 1 && controller.rejectedCount() == 3);

    model.reset();
    CHECK(model.state() == SimModel::State::Stopped);
    CHECK(model.tickCount() == 0);
    CHECK(sameConfig(model.config(), accepted));
    CHECK(controller.submit(
        nextInput,
        [&model](const SimConfig& candidate) {
            return model.setConfig(candidate);
        },
        feedback
    ));

    CHECK(sameConfig(controller.current(), model.config()));
    CHECK(model.config().tickMs == 3000 && model.config().greenTicks == 6);
    CHECK(controller.acceptedCount() == 2 && controller.rejectedCount() == 3);
    CHECK(!sameConfig(controller.current(), defaults));
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
    testPauseResumeRegression();
    testCustomConfigurationTiming();
    testMaximumQueueCapacity();
    testConfigurationChanges();
    testConfigurationControllerWithModel();
    testInvalidRoads();
}
