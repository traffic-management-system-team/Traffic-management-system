#include "TrafficManager.h"
#include "TrafficSignal.h"
#include "simulation_model.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#define CHECK(condition) do { \
    if (!(condition)) { \
        std::cerr << "CHECK failed: " #condition \
                  << " (line " << __LINE__ << ")\n"; \
        std::exit(EXIT_FAILURE); \
    } \
} while (false)

namespace {
void testCoordinatorRejectsInvalidValuesAndIndexes() {
    SignalCoordinator coordinator;
    const int invalidValue = 99;
    const auto invalidState = static_cast<SignalState>(invalidValue);

    CHECK(!coordinator.changeState(0, invalidState));
    CHECK(coordinator.getState(0) == SignalState::RED);
    CHECK(!coordinator.changeState(4, SignalState::GREEN));
    CHECK(!coordinator.canMove(0));

    bool threw = false;
    try {
        (void)coordinator.getState(4);
    } catch (const std::out_of_range&) {
        threw = true;
    }
    CHECK(threw);

    threw = false;
    try {
        TrafficSignal invalidApproach(coordinator, 4);
        (void)invalidApproach;
    } catch (const std::out_of_range&) {
        threw = true;
    }
    CHECK(threw);
}

void testCoordinatorEnforcesSafeSignalTransitions() {
    SignalCoordinator coordinator;
    TrafficSignal north(coordinator, 0);
    TrafficSignal east(coordinator, 1);

    CHECK(north.changeState(SignalState::GREEN));
    CHECK(!east.changeState(SignalState::GREEN));
    CHECK(east.getState() == SignalState::RED);
    CHECK(!north.changeState(SignalState::GREEN));
    CHECK(!north.changeState(SignalState::RED));
    CHECK(north.changeState(SignalState::YELLOW));
    CHECK(!east.changeState(SignalState::GREEN));
    CHECK(north.changeState(SignalState::RED));
    CHECK(east.changeState(SignalState::GREEN));
    CHECK(coordinator.canMove(1));
    CHECK(!coordinator.canMove(0));
}

void testQueueAndSignalProcessingFailSafely() {
    TrafficSignal signal;
    QueueManager queues;
    TrafficManager manager(signal, queues, "north");
    Vehicle output(71, VehicleType::Bus, "sentinel");

    CHECK(!manager.processNextVehicle("north", output));
    CHECK(!manager.processNextVehicle("south", output));
    CHECK(output.getId() == 71);
    CHECK(output.getLane() == "sentinel");

    const Vehicle first(1, VehicleType::Car, "north");
    const Vehicle second(2, VehicleType::Bus, "north");
    CHECK(queues.addVehicle("north", first));
    CHECK(queues.addVehicle("north", second));
    CHECK(!manager.processNextVehicle("north", output));
    CHECK(queues.getQueueLength("north") == 2);
    CHECK(output.getId() == 71);

    CHECK(signal.changeState(SignalState::GREEN));
    CHECK(manager.processNextVehicle("north", output));
    CHECK(output.getId() == 1);
    CHECK(manager.processNextVehicle("north", output));
    CHECK(output.getId() == 2);
    CHECK(!manager.processNextVehicle("north", output));
    CHECK(queues.getQueueLength("north") == 0);
}

void testModelRejectsInvalidConfiguration() {
    SimConfig invalid;
    invalid.maxQueue = 0;
    bool threw = false;
    try {
        SimModel model(invalid);
        (void)model;
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    CHECK(threw);
}

void testModelCapacityAndResetInvariants() {
    SimConfig config;
    config.arrivalInterval = 1;
    config.greenTicks = 1;
    config.yellowTicks = 1;
    config.maxQueue = 1;
    SimModel model(config);

    model.start();
    for (int tick = 0; tick < 80; ++tick) {
        CHECK(model.tick());
        for (int road = 0; road < SimModel::kRoads; ++road) {
            CHECK(model.queueLength(road) <= 1);
        }
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
        CHECK(model.queuedVehicles(road).empty());
    }
    CHECK(!model.tick());
}
}

int main() {
    testCoordinatorRejectsInvalidValuesAndIndexes();
    testCoordinatorEnforcesSafeSignalTransitions();
    testQueueAndSignalProcessingFailSafely();
    testModelRejectsInvalidConfiguration();
    testModelCapacityAndResetInvariants();
    std::cout << "Issue #11 security and reliability regression checks passed.\n";
    return EXIT_SUCCESS;
}
