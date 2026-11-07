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
    CHECK(resetSnapshot.statistics.tickCount == 0);
    CHECK(resetSnapshot.statistics.vehiclesGenerated == 0);
    CHECK(resetSnapshot.statistics.vehiclesProcessed == 0);
    CHECK(resetSnapshot.statistics.vehiclesWaiting == 0);

    CHECK(service.start());
    CHECK(waitForTickCount(service, 1));
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

void testSnapshotAndFifo() {
    SimulationService service;
    std::string feedback;

    CHECK(service.setConfig(fastConfig(), feedback));
    CHECK(service.start());
    CHECK(waitForTickCount(service, 12));

    const SimulationSnapshot first = service.getSnapshot();

    CHECK(first.state == SimModel::State::Running);

    CHECK(first.statistics.vehiclesProcessed > 0);

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
    testConfiguration();
    testSnapshotAndFifo();
    testProcessedVehicleRemoved();
    testConcurrentSnapshotsAndStateChanges();
    testCleanDestruction();

    std::cout << "All SimulationService tests passed.\n";
    return EXIT_SUCCESS;
}
