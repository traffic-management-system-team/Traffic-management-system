#include "config_controller.h"
#include "gui_model.h"

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
void runTicks(SimModel& model, int count) {
    for (int i = 0; i < count; ++i) CHECK(model.tick());
}

void testInvalidConfigurationPreservesLastAccepted() {
    ConfigController controller;
    std::string feedback;
    CHECK(controller.submit(RawConfig{"1500", "8", "2", "4", "6"}, feedback));
    const SimConfig accepted = controller.current();

    CHECK(!controller.submit(RawConfig{"250", "0", "2x", "4", "0"}, feedback));
    CHECK(!feedback.empty());
    CHECK(controller.current().tickMs == accepted.tickMs);
    CHECK(controller.current().greenTicks == accepted.greenTicks);
    CHECK(controller.current().yellowTicks == accepted.yellowTicks);
    CHECK(controller.current().arrivalInterval == accepted.arrivalInterval);
    CHECK(controller.current().maxQueue == accepted.maxQueue);
}

void testInvalidSignalTransitionsAreRejected() {
    TrafficSignal signal;
    CHECK(!signal.changeState(static_cast<SignalState>(99)));
    CHECK(signal.getState() == SignalState::RED);
    CHECK(!signal.changeState(SignalState::YELLOW));
    CHECK(signal.getState() == SignalState::RED);
    CHECK(signal.changeState(SignalState::GREEN));
    CHECK(!signal.changeState(SignalState::GREEN));
    CHECK(signal.getState() == SignalState::GREEN);
}

void testEmptyQueueProcessingIsSafe() {
    TrafficSignal signal;
    CHECK(signal.changeState(SignalState::GREEN));
    QueueManager queues;
    TrafficManager manager(signal, queues, "north");
    Vehicle output(71, VehicleType::Bus, "sentinel");

    CHECK(!manager.processNextVehicle("north", output));
    CHECK(output.getId() == 71);
    CHECK(queues.getQueueLength("north") == 0);
}

void testSignalGatesEachApproach() {
    SignalCoordinator coordinator;
    TrafficSignal north(coordinator, 0);
    TrafficSignal south(coordinator, 1);
    QueueManager queues;
    TrafficManager northManager(north, queues, "north");
    TrafficManager southManager(south, queues, "south");
    queues.addVehicle("north", Vehicle(1, VehicleType::Car, "north"));
    queues.addVehicle("south", Vehicle(2, VehicleType::Car, "south"));
    Vehicle processed(99, VehicleType::Bus, "sentinel");

    CHECK(north.changeState(SignalState::GREEN));
    CHECK(!southManager.processNextVehicle("south", processed));
    CHECK(queues.getQueueLength("south") == 1);
    CHECK(northManager.processNextVehicle("north", processed));
    CHECK(processed.getId() == 1);
}

void testCapacityAndResetReliability() {
    SimConfig config;
    config.arrivalInterval = 1;
    config.greenTicks = 1;
    config.yellowTicks = 10;
    config.maxQueue = 1;
    SimModel model(config);
    model.start();
    runTicks(model, 25);

    for (int road = 0; road < SimModel::kRoads; ++road) {
        CHECK(model.queueLength(road) <= 1);
    }

    model.reset();
    CHECK(model.state() == SimModel::State::Stopped);
    CHECK(model.tickCount() == 0);
    CHECK(model.generatedCount() == 0);
    CHECK(model.processedCount() == 0);
    CHECK(model.totalWaitingTicks() == 0);
    CHECK(model.signal(0) == SimModel::Signal::Green);
    for (int road = 0; road < SimModel::kRoads; ++road) {
        CHECK(model.queueLength(road) == 0);
    }
}
}

int main() {
    testInvalidConfigurationPreservesLastAccepted();
    testInvalidSignalTransitionsAreRejected();
    testEmptyQueueProcessingIsSafe();
    testSignalGatesEachApproach();
    testCapacityAndResetReliability();
    std::cout << "Security and reliability checks passed.\n";
}
