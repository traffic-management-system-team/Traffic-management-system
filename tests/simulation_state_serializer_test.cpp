#include "simulation_state_serializer.h"

#include <cstdlib>
#include <iostream>
#include <string>

namespace {

void check(bool condition, const char* expression, int line) {
    if (!condition) {
        std::cerr << "CHECK FAILED at line " << line
                  << ": " << expression << '\n';
        std::exit(EXIT_FAILURE);
    }
}

#define CHECK(condition) check((condition), #condition, __LINE__)

SimulationSnapshot makeSnapshot() {
    SimulationSnapshot snapshot;
    snapshot.state = SimModel::State::Paused;
    snapshot.activeApproach = 2;
    snapshot.signals = {{
        SimModel::Signal::Red,
        SimModel::Signal::Yellow,
        SimModel::Signal::Green,
        SimModel::Signal::Red
    }};
    snapshot.queues = {{
        {"north", {{101, VehicleType::Car, "north"},
                   {102, VehicleType::Bus, "north"}}},
        {"east", {}},
        {"south", {{201, VehicleType::Truck, "south"}}},
        {"west", {{301, VehicleType::Emergency, "west"}}}
    }};
    snapshot.config = SimConfig{250, 12, 3, 5, 20};

    auto& statistics = snapshot.statistics;
    statistics.tickCount = 12;
    statistics.vehiclesGenerated = 8;
    statistics.vehiclesProcessed = 4;
    statistics.vehiclesWaiting = 4;
    statistics.totalWaitingTicks = 24;
    statistics.averageWaitingTicks = 6.0;
    statistics.processedPerLogicalTick = 1.0 / 3.0;
    statistics.approaches = {{
        {2, 1, 1},
        {2, 1, 1},
        {2, 1, 1},
        {2, 1, 1}
    }};
    return snapshot;
}

void testStateAndApproachSerialization() {
    const SimulationSnapshot snapshot = makeSnapshot();
    const auto data = serializeSimulationStateData(snapshot);

    CHECK(data.is_object());
    CHECK(!data.contains("success"));
    CHECK(!data.contains("error"));
    CHECK(data.at("state") == "paused");
    CHECK(data.at("tickCount") == 12);
    CHECK(data.at("activeApproach") == "south");
    CHECK(data.at("activeSignal") == "green");

    const auto& approaches = data.at("approaches");
    CHECK(approaches.is_array());
    CHECK(approaches.size() == 4);
    CHECK(approaches[0].at("id") == "north");
    CHECK(approaches[0].at("name") == "North");
    CHECK(approaches[0].at("signal") == "red");
    CHECK(approaches[1].at("id") == "east");
    CHECK(approaches[1].at("signal") == "yellow");
    CHECK(approaches[2].at("id") == "south");
    CHECK(approaches[2].at("signal") == "green");
    CHECK(approaches[3].at("id") == "west");
    CHECK(approaches[3].at("signal") == "red");

    for (std::size_t i = 0; i < approaches.size(); ++i) {
        CHECK(approaches[i].at("queueLength") ==
              approaches[i].at("vehicles").size());
    }

    CHECK(approaches[0].at("queueLength") == 2);
    CHECK(approaches[0].at("vehicles")[0].at("id") == 101);
    CHECK(approaches[0].at("vehicles")[0].at("type") == "Car");
    CHECK(approaches[0].at("vehicles")[0].at("lane") == "north");
    CHECK(approaches[0].at("vehicles")[1].at("id") == 102);
    CHECK(approaches[0].at("vehicles")[1].at("type") == "Bus");
    CHECK(approaches[1].at("vehicles").empty());
    CHECK(approaches[2].at("vehicles")[0].at("id") == 201);
    CHECK(approaches[2].at("vehicles")[0].at("type") == "Truck");
    CHECK(approaches[3].at("vehicles")[0].at("id") == 301);
    CHECK(approaches[3].at("vehicles")[0].at("type") == "Emergency");
}

void testConfigurationAndStatisticsSerialization() {
    const auto data = serializeSimulationStateData(makeSnapshot());

    CHECK(data.at("config").at("tickMs") == 250);
    CHECK(data.at("config").at("greenTicks") == 12);
    CHECK(data.at("config").at("yellowTicks") == 3);
    CHECK(data.at("config").at("arrivalInterval") == 5);
    CHECK(data.at("config").at("maxQueue") == 20);

    const auto& statistics = data.at("statistics");
    CHECK(statistics.at("tickCount") == 12);
    CHECK(statistics.at("vehiclesGenerated") == 8);
    CHECK(statistics.at("vehiclesProcessed") == 4);
    CHECK(statistics.at("vehiclesWaiting") == 4);
    CHECK(statistics.at("totalWaitingTicks") == 24);
    CHECK(statistics.at("averageWaitingTicks") == 6.0);
    CHECK(statistics.at("averageWaitingTicks") ==
          static_cast<double>(statistics.at("totalWaitingTicks")) /
              static_cast<double>(statistics.at("vehiclesProcessed")));
    CHECK(statistics.at("processedPerLogicalTick") == 1.0 / 3.0);

    const auto& approaches = statistics.at("approaches");
    CHECK(approaches.size() == 4);
    CHECK(approaches[0].at("id") == "north");
    CHECK(approaches[1].at("id") == "south");
    CHECK(approaches[2].at("id") == "east");
    CHECK(approaches[3].at("id") == "west");
    for (const auto& approach : approaches) {
        CHECK(approach.at("vehiclesGenerated") == 2);
        CHECK(approach.at("vehiclesProcessed") == 1);
        CHECK(approach.at("vehiclesWaiting") == 1);
    }
}

void testSerializationUsesOnlyAndDoesNotMutateSnapshot() {
    SimulationSnapshot snapshot = makeSnapshot();
    const auto serialized = serializeSimulationStateData(snapshot);

    CHECK(snapshot.state == SimModel::State::Paused);
    CHECK(snapshot.activeApproach == 2);
    CHECK(snapshot.statistics.tickCount == 12);
    CHECK(snapshot.queues[0].vehicles[0].id == 101);

    snapshot.activeApproach = 0;
    snapshot.config.tickMs = 999;
    snapshot.statistics.tickCount = 999;
    const auto second = serializeSimulationStateData(snapshot);

    CHECK(serialized.at("activeApproach") == "south");
    CHECK(serialized.at("config").at("tickMs") == 250);
    CHECK(serialized.at("tickCount") == 12);
    CHECK(second.at("activeApproach") == "north");
    CHECK(second.at("activeSignal") == "red");
    CHECK(second.at("config").at("tickMs") == 999);
    CHECK(second.at("tickCount") == 999);
}

} // namespace

int main() {
    testStateAndApproachSerialization();
    testConfigurationAndStatisticsSerialization();
    testSerializationUsesOnlyAndDoesNotMutateSnapshot();

    std::cout << "All simulation_state_serializer tests passed.\n";
    return EXIT_SUCCESS;
}
