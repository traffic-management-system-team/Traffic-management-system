#include "simulation_service.h"

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <vector>

#define CHECK(condition) do { \
    if (!(condition)) { \
        std::cerr << "CHECK failed: " #condition \
                  << " (line " << __LINE__ << ")\n"; \
        std::exit(EXIT_FAILURE); \
    } \
} while (false)

namespace {

using namespace std::chrono_literals;

RawConfig fastConfig() {
    return {
        "100", // tickMs
        "60",  // greenTicks
        "1",   // yellowTicks
        "1",   // arrivalInterval
        "50"   // maxQueue
    };
}

RawConfig invalidConfig() {
    return {
        "not-a-number",
        "60",
        "1",
        "1",
        "50"
    };
}

bool waitForTickCount(
    SimulationService& service,
    std::uint64_t minimum,
    std::chrono::milliseconds timeout = 2000ms
) {
    const auto deadline = std::chrono::steady_clock::now() + timeout;

    while (std::chrono::steady_clock::now() < deadline) {
        if (service.getSnapshot().statistics.tickCount >= minimum) {
            return true;
        }
        std::this_thread::sleep_for(2ms);
    }

    return service.getSnapshot().statistics.tickCount >= minimum;
}

void testInitialStateAndTransitions() {
    SimulationService service;

    CHECK(service.getState() == SimModel::State::Stopped);
    const auto initialSnapshot = service.getSnapshot();
    CHECK(initialSnapshot.activeApproach == 0);
    CHECK(initialSnapshot.signals[0] == SimModel::Signal::Green);
    CHECK(!service.pause());
    CHECK(!service.resume());

    std::string feedback;
    CHECK(service.setConfig(fastConfig(), feedback));

    CHECK(service.start());
    CHECK(!service.start());

    CHECK(waitForTickCount(service, 2));

    CHECK(service.pause());
    CHECK(service.getState() == SimModel::State::Paused);
    CHECK(!service.pause());

    const auto pausedTicks =
        service.getSnapshot().statistics.tickCount;

    std::this_thread::sleep_for(100ms);

    CHECK(service.getSnapshot().statistics.tickCount == pausedTicks);

    CHECK(service.resume());
    CHECK(!service.resume());
    CHECK(waitForTickCount(service, pausedTicks + 1));

    service.reset();

    const auto resetSnapshot = service.getSnapshot();
    CHECK(resetSnapshot.state == SimModel::State::Stopped);
    CHECK(resetSnapshot.activeApproach == 0);
    CHECK(resetSnapshot.signals[0] == SimModel::Signal::Green);
    CHECK(resetSnapshot.statistics.tickCount == 0);
    CHECK(resetSnapshot.statistics.vehiclesGenerated == 0);
    CHECK(resetSnapshot.statistics.vehiclesProcessed == 0);
    CHECK(resetSnapshot.statistics.vehiclesWaiting == 0);

    CHECK(service.start());
    CHECK(waitForTickCount(service, 1));
}

void testSnapshotActiveApproach() {
    SimulationService service;
    std::string feedback;
    const RawConfig shortSignalPhase{
        "100", // tickMs
        "1",   // greenTicks
        "1",   // yellowTicks
        "20",  // arrivalInterval
        "50"   // maxQueue
    };

    CHECK(service.setConfig(shortSignalPhase, feedback));
    CHECK(service.start());

    const auto deadline = std::chrono::steady_clock::now() + 2000ms;
    while (std::chrono::steady_clock::now() < deadline) {
        const auto snapshot = service.getSnapshot();
        if (snapshot.statistics.tickCount >= 2) {
            CHECK(snapshot.activeApproach == 2);
            CHECK(snapshot.signals[2] == SimModel::Signal::Green);
            service.reset();
            return;
        }
        std::this_thread::sleep_for(2ms);
    }

    CHECK(false && "Simulation did not reach the second approach.");
}
void testConfiguration() {
    SimulationService service;
    std::string feedback;

    const auto original = service.getSnapshot().config;

    CHECK(!service.setConfig(invalidConfig(), feedback));
    CHECK(!feedback.empty());

    CHECK(service.getSnapshot().config.tickMs == original.tickMs);

    CHECK(service.setConfig(fastConfig(), feedback));
    CHECK(feedback == "Configuration accepted.");

    const auto configured = service.getSnapshot().config;
    CHECK(configured.tickMs == 100);
    CHECK(configured.greenTicks == 60);
    CHECK(configured.yellowTicks == 1);
    CHECK(configured.arrivalInterval == 1);
    CHECK(configured.maxQueue == 50);

    CHECK(service.start());
    CHECK(waitForTickCount(service, 1));

    // SimModel only accepts configuration while stopped and pristine.
    CHECK(!service.setConfig(fastConfig(), feedback));

    service.reset();
}

void testModelActiveApproachTransition() {
    SimModel model(SimConfig{100, 1, 1, 1, 50});

    CHECK(model.activeApproach() == 0);
    CHECK(model.signal(0) == SimModel::Signal::Green);

    model.start();

    CHECK(model.tick());
    CHECK(model.activeApproach() == 0);
    CHECK(model.signal(0) == SimModel::Signal::Yellow);

    CHECK(model.tick());
    CHECK(model.activeApproach() == 2);
    CHECK(model.signal(0) == SimModel::Signal::Red);
    CHECK(model.signal(2) == SimModel::Signal::Green);
}
void testSnapshotAndFifo() {
    SimulationService service;
    std::string feedback;

    CHECK(service.setConfig(fastConfig(), feedback));
    CHECK(service.start());
    CHECK(waitForTickCount(service, 12));

    const SimulationSnapshot first = service.getSnapshot();

    CHECK(first.state == SimModel::State::Running);

    CHECK(first.statistics.vehiclesProcessed > 0);
    CHECK(first.statistics.averageWaitingTicks ==
          static_cast<double>(first.statistics.totalWaitingTicks) /
              static_cast<double>(first.statistics.vehiclesProcessed));

    CHECK(
        first.statistics.vehiclesGenerated ==
        first.statistics.vehiclesProcessed +
        first.statistics.vehiclesWaiting
    );

    std::size_t totalQueued = 0;

    for (const auto& queue : first.queues) {
        totalQueued += queue.vehicles.size();

        for (std::size_t i = 1; i < queue.vehicles.size(); ++i) {
            CHECK(
                queue.vehicles[i - 1].id <
                queue.vehicles[i].id
            );
        }

        for (const auto& vehicle : queue.vehicles) {
            CHECK(vehicle.lane == queue.roadId);
        }
    }

    CHECK(totalQueued == first.statistics.vehiclesWaiting);

    // Other approaches should accumulate vehicles while North is green.
    CHECK(!first.queues[1].vehicles.empty());
    CHECK(!first.queues[2].vehicles.empty());
    CHECK(!first.queues[3].vehicles.empty());

    const auto originalTickCount = first.statistics.tickCount;

    CHECK(waitForTickCount(service, originalTickCount + 2));

    // The earlier snapshot must remain an independent copy.
    CHECK(first.statistics.tickCount == originalTickCount);
}



void testSnapshotReadsAreNonMutating() {
    SimulationService service;
    std::string feedback;

    CHECK(service.setConfig(fastConfig(), feedback));
    CHECK(service.start());
    CHECK(waitForTickCount(service, 12));
    CHECK(service.pause());

    const auto baseline = service.getSnapshot();

    CHECK(baseline.state == SimModel::State::Paused);

    for (int iteration = 0; iteration < 25; ++iteration) {
        const auto current = service.getSnapshot();

        CHECK(current.state == baseline.state);
        CHECK(current.activeApproach == baseline.activeApproach);
        CHECK(current.signals == baseline.signals);

        CHECK(current.statistics.tickCount == baseline.statistics.tickCount);
        CHECK(current.statistics.vehiclesGenerated == baseline.statistics.vehiclesGenerated);
        CHECK(current.statistics.vehiclesProcessed == baseline.statistics.vehiclesProcessed);
        CHECK(current.statistics.vehiclesWaiting == baseline.statistics.vehiclesWaiting);
        CHECK(current.statistics.totalWaitingTicks == baseline.statistics.totalWaitingTicks);
        CHECK(current.statistics.averageWaitingTicks == baseline.statistics.averageWaitingTicks);
        CHECK(current.statistics.processedPerLogicalTick == baseline.statistics.processedPerLogicalTick);

        CHECK(current.config.tickMs == baseline.config.tickMs);
        CHECK(current.config.greenTicks == baseline.config.greenTicks);
        CHECK(current.config.yellowTicks == baseline.config.yellowTicks);
        CHECK(current.config.arrivalInterval == baseline.config.arrivalInterval);
        CHECK(current.config.maxQueue == baseline.config.maxQueue);

        for (std::size_t road = 0; road < baseline.queues.size(); ++road) {
            const auto& expected = baseline.queues[road];
            const auto& actual = current.queues[road];

            CHECK(actual.roadId == expected.roadId);
            CHECK(actual.vehicles.size() == expected.vehicles.size());

            for (std::size_t vehicle = 0; vehicle < expected.vehicles.size(); ++vehicle) {
                CHECK(actual.vehicles[vehicle].id == expected.vehicles[vehicle].id);
                CHECK(actual.vehicles[vehicle].type == expected.vehicles[vehicle].type);
                CHECK(actual.vehicles[vehicle].lane == expected.vehicles[vehicle].lane);
            }
        }

        for (std::size_t approach = 0;
             approach < baseline.statistics.approaches.size();
             ++approach) {
            const auto& expected = baseline.statistics.approaches[approach];
            const auto& actual = current.statistics.approaches[approach];

            CHECK(actual.vehiclesGenerated == expected.vehiclesGenerated);
            CHECK(actual.vehiclesProcessed == expected.vehiclesProcessed);
            CHECK(actual.vehiclesWaiting == expected.vehiclesWaiting);
        }
    }
}
void testProcessedVehicleRemoved() {
    SimulationService service;
    std::string feedback;

    CHECK(service.setConfig(fastConfig(), feedback));
    CHECK(service.start());

    // Tick 1 generates vehicles for the approaches.
    CHECK(waitForTickCount(service, 1));

    const auto before = service.getSnapshot();

    // The first approach (North) should have a waiting vehicle.
    CHECK(!before.queues[0].vehicles.empty());

    const int vehicleId = before.queues[0].vehicles.front().id;
    const auto processedBefore = before.statistics.vehiclesProcessed;

    // Tick 2 processes the waiting North vehicle.
    CHECK(waitForTickCount(service, 2));

    const auto after = service.getSnapshot();

    CHECK(after.statistics.vehiclesProcessed > processedBefore);

    // The processed vehicle must no longer exist in any waiting queue.
    bool vehicleStillQueued = false;

    for (const auto& queue : after.queues) {
        for (const auto& vehicle : queue.vehicles) {
            if (vehicle.id == vehicleId) {
                vehicleStillQueued = true;
            }
        }
    }

    CHECK(!vehicleStillQueued);
}

void testConcurrentSnapshotsAndStateChanges() {
    SimulationService service;
    std::string feedback;

    CHECK(service.setConfig(fastConfig(), feedback));
    CHECK(service.start());

    std::atomic<bool> readerFailed{false};

    std::vector<std::thread> readers;

    for (int i = 0; i < 3; ++i) {
        readers.emplace_back([&service, &readerFailed] {
            for (int iteration = 0; iteration < 100; ++iteration) {
                const auto snapshot = service.getSnapshot();

                std::size_t queued = 0;
                for (const auto& queue : snapshot.queues) {
                    queued += queue.vehicles.size();
                }

                if (queued != snapshot.statistics.vehiclesWaiting) {
                    readerFailed.store(true);
                    return;
                }
            }
        });
    }

    for (int i = 0; i < 5; ++i) {
        CHECK(service.pause());
        CHECK(service.resume());
    }

    for (auto& reader : readers) {
        reader.join();
    }

    CHECK(!readerFailed.load());
}

void testCleanDestruction() {
    {
        SimulationService service;
        std::string feedback;

        CHECK(service.setConfig(fastConfig(), feedback));
        CHECK(service.start());
        CHECK(waitForTickCount(service, 2));
    }

    // Reaching this point verifies destruction returned after worker join.
}

} // namespace

int main() {
    testInitialStateAndTransitions();
    testSnapshotActiveApproach();
    testConfiguration();
    testModelActiveApproachTransition();
    testSnapshotAndFifo();
    testSnapshotReadsAreNonMutating();
    testProcessedVehicleRemoved();
    testConcurrentSnapshotsAndStateChanges();
    testCleanDestruction();

    std::cout << "All SimulationService tests passed.\n";
    return EXIT_SUCCESS;
}
