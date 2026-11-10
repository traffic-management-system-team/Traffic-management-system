#include "simulation_http_server.h"
#include "simulation_state_serializer.h"

#include <asio.hpp>
#include <nlohmann/json.hpp>

#include <algorithm>
#include <atomic>
#include <array>
#include <chrono>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace {

using Json = nlohmann::json;

struct HttpResponse {
    int status = 0;
    std::string contentType;
    std::string body;
};

void check(bool condition, const char* expression, int line) {
    if (!condition) {
        std::cerr << "CHECK FAILED at line " << line
                  << ": " << expression << '\n';
        std::exit(EXIT_FAILURE);
    }
}

#define CHECK(condition) check((condition), #condition, __LINE__)

HttpResponse sendRequest(
    std::uint16_t port,
    const std::string& method,
    const std::string& path,
    const std::string& body = {},
    const std::string& contentType = {}
) {
    asio::io_context context;
    asio::ip::tcp::socket socket(context);
    socket.connect(asio::ip::tcp::endpoint(
        asio::ip::make_address("127.0.0.1"),
        port
    ));

    std::ostringstream request;
    request << method << ' ' << path << " HTTP/1.1\r\n"
            << "Host: 127.0.0.1:" << port << "\r\n"
            << "Connection: close\r\n";
    if (!contentType.empty()) {
        request << "Content-Type: " << contentType << "\r\n";
    }
    request << "Content-Length: " << body.size() << "\r\n\r\n"
            << body;

    const std::string bytes = request.str();
    asio::write(socket, asio::buffer(bytes));

    asio::streambuf responseBuffer;
    asio::read_until(socket, responseBuffer, "\r\n\r\n");
    std::istream responseStream(&responseBuffer);

    std::string statusLine;
    std::getline(responseStream, statusLine);
    if (statusLine.size() < 12 || statusLine.substr(0, 5) != "HTTP/") {
        throw std::runtime_error("Invalid HTTP response status line.");
    }

    HttpResponse response;
    response.status = std::stoi(statusLine.substr(9, 3));
    std::size_t contentLength = 0;

    std::string header;
    while (std::getline(responseStream, header) && header != "\r") {
        const std::size_t separator = header.find(':');
        if (separator == std::string::npos) {
            continue;
        }

        std::string name = header.substr(0, separator);
        std::string value = header.substr(separator + 1);
        std::transform(name.begin(), name.end(), name.begin(),
            [](unsigned char character) {
                return static_cast<char>(std::tolower(character));
            });
        while (!value.empty() && value.front() == ' ') {
            value.erase(value.begin());
        }
        if (!value.empty() && value.back() == '\r') {
            value.pop_back();
        }

        if (name == "content-length") {
            contentLength = static_cast<std::size_t>(std::stoull(value));
        } else if (name == "content-type") {
            response.contentType = value;
        }
    }

    const std::size_t alreadyRead = responseBuffer.size();
    if (alreadyRead < contentLength) {
        asio::read(
            socket,
            responseBuffer,
            asio::transfer_exactly(contentLength - alreadyRead)
        );
    }

    response.body.resize(contentLength);
    responseStream.read(
        response.body.data(),
        static_cast<std::streamsize>(contentLength)
    );
    return response;
}

HttpResponse post(std::uint16_t port, const std::string& path) {
    return sendRequest(port, "POST", path);
}

HttpResponse putConfig(std::uint16_t port, const Json& config) {
    return sendRequest(
        port,
        "PUT",
        "/api/simulation/config",
        config.dump(),
        "application/json"
    );
}

Json responseJson(const HttpResponse& response) {
    CHECK(response.contentType.find("application/json") == 0);
    return Json::parse(response.body);
}

Json getStateData(std::uint16_t port) {
    const HttpResponse response = sendRequest(
        port, "GET", "/api/simulation/state"
    );
    CHECK(response.status == 200);

    const Json envelope = responseJson(response);
    CHECK(envelope.at("success") == true);
    CHECK(envelope.at("error").is_null());
    CHECK(envelope.at("data").is_object());
    return envelope.at("data");
}

void checkError(
    const HttpResponse& response,
    int expectedStatus,
    const std::string& expectedCode
) {
    CHECK(response.status == expectedStatus);
    const Json envelope = responseJson(response);
    CHECK(envelope.at("success") == false);
    CHECK(envelope.at("data").is_null());
    CHECK(envelope.at("error").at("code") == expectedCode);
    CHECK(envelope.at("error").at("message").is_string());
}

SimulationSnapshot snapshotFromModel(const SimModel& model) {
    constexpr std::array<const char*, SimModel::kRoads> roadIds = {
        "north", "east", "south", "west"
    };

    SimulationSnapshot snapshot;
    snapshot.state = model.state();
    snapshot.activeApproach = model.activeApproach();
    snapshot.statistics = model.statistics();
    snapshot.config = model.config();

    for (int road = 0; road < SimModel::kRoads; ++road) {
        const std::size_t index = static_cast<std::size_t>(road);
        snapshot.signals[index] = model.signal(road);
        snapshot.queues[index].roadId = roadIds[index];

        const auto vehicles = model.queuedVehicles(road);
        for (const auto& vehicle : vehicles) {
            snapshot.queues[index].vehicles.push_back({
                vehicle.getId(),
                vehicle.getType(),
                vehicle.getLane()
            });
        }
    }

    return snapshot;
}

bool waitForTickCount(
    SimulationService& service,
    std::uint64_t minimum,
    std::chrono::milliseconds timeout
) {
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    while (std::chrono::steady_clock::now() < deadline) {
        if (service.getSnapshot().statistics.tickCount >= minimum) {
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return service.getSnapshot().statistics.tickCount >= minimum;
}

void testDeterministicLogicalTicksAndFifoVehicleIds() {
    SimModel model(SimConfig{100, 60, 1, 1, 20});
    model.start();

    // Drive the model directly: these assertions do not depend on wall-clock time.
    for (int tick = 0; tick < 8; ++tick) {
        CHECK(model.tick());
    }

    const auto snapshot = snapshotFromModel(model);
    const Json data = serializeSimulationStateData(snapshot);

    CHECK(snapshot.statistics.tickCount == 8);
    CHECK(snapshot.statistics.vehiclesGenerated == 8);
    CHECK(snapshot.statistics.vehiclesProcessed == 2);
    CHECK(snapshot.statistics.vehiclesWaiting == 6);
    CHECK(snapshot.statistics.totalWaitingTicks == 2);
    CHECK(snapshot.statistics.averageWaitingTicks == 1.0);
    CHECK(snapshot.statistics.processedPerLogicalTick == 0.25);

    CHECK(model.queuedVehicles(0).empty());

    const auto east = model.queuedVehicles(1);
    const auto south = model.queuedVehicles(2);
    const auto west = model.queuedVehicles(3);

    CHECK(east.size() == 2);
    CHECK(east[0].getId() == 3);
    CHECK(east[1].getId() == 7);
    CHECK(south.size() == 2);
    CHECK(south[0].getId() == 2);
    CHECK(south[1].getId() == 6);
    CHECK(west.size() == 2);
    CHECK(west[0].getId() == 4);
    CHECK(west[1].getId() == 8);

    const auto& approaches = data.at("approaches");
    CHECK(approaches.size() == 4);
    CHECK(approaches[0].at("id") == "north");
    CHECK(approaches[0].at("queueLength") == 0);
    CHECK(approaches[1].at("id") == "east");
    CHECK(approaches[1].at("queueLength") == 2);
    CHECK(approaches[1].at("vehicles")[0].at("id") == 3);
    CHECK(approaches[1].at("vehicles")[1].at("id") == 7);
    CHECK(approaches[2].at("id") == "south");
    CHECK(approaches[2].at("queueLength") == 2);
    CHECK(approaches[2].at("vehicles")[0].at("id") == 2);
    CHECK(approaches[2].at("vehicles")[1].at("id") == 6);
    CHECK(approaches[3].at("id") == "west");
    CHECK(approaches[3].at("queueLength") == 2);
    CHECK(approaches[3].at("vehicles")[0].at("id") == 4);
    CHECK(approaches[3].at("vehicles")[1].at("id") == 8);

    const auto& statistics = data.at("statistics");
    CHECK(statistics.at("vehiclesGenerated") == 8);
    CHECK(statistics.at("vehiclesProcessed") == 2);
    CHECK(statistics.at("vehiclesWaiting") == 6);
    CHECK(statistics.at("totalWaitingTicks") == 2);
    CHECK(statistics.at("averageWaitingTicks") == 1.0);

    // A paused model cannot advance until explicitly resumed.
    model.togglePause();
    CHECK(model.state() == SimModel::State::Paused);
    CHECK(!model.tick());
    CHECK(model.statistics().tickCount == 8);

    model.reset();
    CHECK(model.state() == SimModel::State::Stopped);
    CHECK(model.statistics().tickCount == 0);
    CHECK(model.statistics().vehiclesGenerated == 0);
    CHECK(model.statistics().vehiclesProcessed == 0);
    CHECK(model.statistics().vehiclesWaiting == 0);
    CHECK(model.config().tickMs == 100);
    for (int road = 0; road < SimModel::kRoads; ++road) {
        CHECK(model.queuedVehicles(road).empty());
    }
    CHECK(model.signal(0) == SimModel::Signal::Green);
}

void testRestSnapshotExposesActualFifoVehiclesAndConsistentCounts() {
    SimulationService service;
    SimulationHttpServer server(service, "127.0.0.1", 0);
    std::string error;
    CHECK(server.start(error));
    CHECK(server.port() != 0);

    const Json config = {
        {"tickMs", 100},
        {"greenTicks", 60},
        {"yellowTicks", 1},
        {"arrivalInterval", 1},
        {"maxQueue", 20}
    };
    const HttpResponse configured = putConfig(server.port(), config);
    CHECK(configured.status == 200);

    CHECK(post(server.port(), "/api/simulation/start").status == 200);
    CHECK(waitForTickCount(service, 8, std::chrono::seconds(5)));

    // The successful pause response is the acknowledgement that no tick is
    // in flight; the following state reads must remain identical.
    const HttpResponse paused = post(server.port(), "/api/simulation/pause");
    CHECK(paused.status == 200);
    CHECK(service.getSnapshot().state == SimModel::State::Paused);

    const SimulationSnapshot expectedSnapshot = service.getSnapshot();
    const Json expectedData = serializeSimulationStateData(expectedSnapshot);
    const Json actualData = getStateData(server.port());
    CHECK(actualData == expectedData);

    const auto& approaches = actualData.at("approaches");
    CHECK(approaches.size() == 4);

    std::set<int> queuedIds;
    std::size_t totalQueued = 0;
    std::size_t nonEmptyApproaches = 0;
    for (const auto& approach : approaches) {
        const std::string roadId = approach.at("id").get<std::string>();
        const auto& vehicles = approach.at("vehicles");
        const std::size_t queueLength =
            approach.at("queueLength").get<std::size_t>();

        CHECK(queueLength == vehicles.size());
        totalQueued += queueLength;
        if (queueLength != 0) {
            ++nonEmptyApproaches;
        }

        int previousId = 0;
        for (const auto& vehicle : vehicles) {
            const int id = vehicle.at("id").get<int>();
            CHECK(id > previousId);
            CHECK(queuedIds.insert(id).second);
            CHECK(vehicle.at("lane") == roadId);
            CHECK(vehicle.at("type").is_string());
            previousId = id;
        }
    }

    CHECK(nonEmptyApproaches >= 3);
    CHECK(totalQueued == actualData.at("statistics").at("vehiclesWaiting"));
    CHECK(actualData.at("statistics").at("vehiclesGenerated") ==
          actualData.at("statistics").at("vehiclesProcessed").get<std::uint64_t>() +
          totalQueued);

    const auto& approachStatistics =
        actualData.at("statistics").at("approaches");
    CHECK(approachStatistics.size() == 4);
    for (const auto& approachStats : approachStatistics) {
        const std::string roadId = approachStats.at("id").get<std::string>();
        const auto found = std::find_if(
            approaches.begin(),
            approaches.end(),
            [&roadId](const Json& approach) {
                return approach.at("id") == roadId;
            }
        );
        CHECK(found != approaches.end());
        CHECK(approachStats.at("vehiclesWaiting") == found->at("queueLength"));
    }

    for (int read = 0; read < 5; ++read) {
        CHECK(getStateData(server.port()) == actualData);
    }

    CHECK(server.stop());
}

void testRejectedConfigurationPreservesLastAcceptedValues() {
    SimulationService service;
    SimulationHttpServer server(service, "127.0.0.1", 0);
    std::string error;
    CHECK(server.start(error));

    const Json acceptedConfig = {
        {"tickMs", 250},
        {"greenTicks", 12},
        {"yellowTicks", 3},
        {"arrivalInterval", 4},
        {"maxQueue", 15}
    };
    CHECK(putConfig(server.port(), acceptedConfig).status == 200);
    CHECK(getStateData(server.port()).at("config") == acceptedConfig);

    Json invalidConfig = acceptedConfig;
    invalidConfig["greenTicks"] = 0;
    checkError(
        putConfig(server.port(), invalidConfig),
        400,
        "VALIDATION_FAILED"
    );
    CHECK(getStateData(server.port()).at("config") == acceptedConfig);

    const Json replacementConfig = {
        {"tickMs", 300},
        {"greenTicks", 20},
        {"yellowTicks", 4},
        {"arrivalInterval", 5},
        {"maxQueue", 25}
    };

    CHECK(post(server.port(), "/api/simulation/start").status == 200);
    checkError(
        putConfig(server.port(), replacementConfig),
        409,
        "INVALID_STATE"
    );
    CHECK(getStateData(server.port()).at("config") == acceptedConfig);

    CHECK(post(server.port(), "/api/simulation/pause").status == 200);
    checkError(
        putConfig(server.port(), replacementConfig),
        409,
        "INVALID_STATE"
    );
    CHECK(getStateData(server.port()).at("config") == acceptedConfig);

    CHECK(post(server.port(), "/api/simulation/reset").status == 200);
    const Json resetData = getStateData(server.port());
    CHECK(resetData.at("state") == "stopped");
    CHECK(resetData.at("tickCount") == 0);
    CHECK(resetData.at("config") == acceptedConfig);

    CHECK(server.stop());
}


void testDeterministicArrivalsSignalTransitionsAndSignalGatedMovement() {
    SimModel model(SimConfig{100, 2, 1, 1, 20});
    model.start();

    CHECK(model.signal(0) == SimModel::Signal::Green);
    CHECK(model.signal(1) == SimModel::Signal::Red);
    CHECK(model.signal(2) == SimModel::Signal::Red);
    CHECK(model.signal(3) == SimModel::Signal::Red);

    // Tick 1 creates exactly one North vehicle with its initial lane/queue state.
    CHECK(model.tick());
    const auto northAfterFirstTick = model.queuedVehicles(0);
    CHECK(northAfterFirstTick.size() == 1);
    CHECK(northAfterFirstTick[0].getId() == 1);
    CHECK(northAfterFirstTick[0].getLane() == "north");
    CHECK(model.statistics().vehiclesGenerated == 1);
    CHECK(model.statistics().vehiclesProcessed == 0);
    CHECK(model.queueLength(0) == 1);

    // Tick 2 processes North while it is GREEN, then creates a South vehicle.
    CHECK(model.tick());
    CHECK(model.queuedVehicles(0).empty());
    const auto southAfterSecondTick = model.queuedVehicles(2);
    CHECK(southAfterSecondTick.size() == 1);
    CHECK(southAfterSecondTick[0].getId() == 2);
    CHECK(southAfterSecondTick[0].getLane() == "south");
    CHECK(model.statistics().vehiclesProcessed == 1);
    CHECK(model.signal(0) == SimModel::Signal::Yellow);
    CHECK(model.signal(2) == SimModel::Signal::Red);

    // Tick 3 leaves the South vehicle queued while South is still RED.
    // The tick then completes the North YELLOW -> RED / South RED -> GREEN transition.
    CHECK(model.tick());
    CHECK(model.queuedVehicles(2).size() == 1);
    CHECK(model.queuedVehicles(2)[0].getId() == 2);
    CHECK(model.queuedVehicles(1).size() == 1);
    CHECK(model.queuedVehicles(1)[0].getId() == 3);
    CHECK(model.statistics().vehiclesProcessed == 1);
    CHECK(model.signal(0) == SimModel::Signal::Red);
    CHECK(model.signal(2) == SimModel::Signal::Green);

    // Tick 4 processes the waiting South vehicle only after South becomes GREEN.
    CHECK(model.tick());
    CHECK(model.queuedVehicles(2).empty());
    CHECK(model.statistics().vehiclesProcessed == 2);
    CHECK(model.queuedVehicles(3).size() == 1);
    CHECK(model.queuedVehicles(3)[0].getId() == 4);

    // The next signal phases are deterministic: South YELLOW, then East GREEN.
    CHECK(model.tick());
    CHECK(model.signal(2) == SimModel::Signal::Yellow);
    CHECK(model.queuedVehicles(1).size() == 1);
    CHECK(model.tick());
    CHECK(model.signal(2) == SimModel::Signal::Red);
    CHECK(model.signal(1) == SimModel::Signal::Green);

    // East is processed only while GREEN; West waits through RED until its turn.
    CHECK(model.tick());
    CHECK(model.queuedVehicles(1).size() == 1);
    CHECK(model.queuedVehicles(1)[0].getId() == 7);
    CHECK(model.statistics().vehiclesProcessed == 3);
    CHECK(model.tick());
    CHECK(model.queuedVehicles(1).empty());
    CHECK(model.signal(1) == SimModel::Signal::Yellow);
    const auto westAfterEighthTick = model.queuedVehicles(3);
    CHECK(westAfterEighthTick.size() == 2);
    CHECK(westAfterEighthTick[0].getId() == 4);
    CHECK(westAfterEighthTick[1].getId() == 8);

    CHECK(model.tick());
    CHECK(model.signal(1) == SimModel::Signal::Red);
    CHECK(model.signal(3) == SimModel::Signal::Green);
    const auto westAfterNinthTick = model.queuedVehicles(3);
    CHECK(westAfterNinthTick.size() == 2);
    CHECK(westAfterNinthTick[0].getId() == 4);
    CHECK(westAfterNinthTick[1].getId() == 8);

    // On tick 10, West is already GREEN at tick entry, so only its
    // oldest queued vehicle (ID 4) may be processed.
    CHECK(model.tick());
    const auto westAfterTenthTick = model.queuedVehicles(3);
    CHECK(westAfterTenthTick.size() == 1);
    CHECK(westAfterTenthTick[0].getId() == 8);
    CHECK(model.statistics().vehiclesProcessed == 5);

    const auto statistics = model.statistics();
    CHECK(statistics.tickCount == 10);
    CHECK(statistics.vehiclesGenerated == 10);
    CHECK(statistics.vehiclesProcessed == 5);
    CHECK(statistics.vehiclesWaiting == 5);
    CHECK(statistics.totalWaitingTicks == 14);
}

void validateConcurrentSnapshot(const Json& data) {
    const auto require = [](bool condition, const char* message) {
        if (!condition) {
            throw std::runtime_error(message);
        }
    };

    require(data.is_object(), "Snapshot data must be an object.");
    require(data.at("state").is_string(), "Snapshot state must be a string.");
    require(data.at("tickCount").is_number_unsigned() ||
            data.at("tickCount").is_number_integer(),
            "Snapshot tickCount must be an integer.");
    require(data.at("config").is_object(), "Snapshot config must be an object.");

    const auto& approaches = data.at("approaches");
    require(approaches.is_array() && approaches.size() == 4,
            "Snapshot must contain four approaches.");

    std::set<int> allVehicleIds;
    std::size_t totalWaiting = 0;
    std::size_t activeSignals = 0;
    std::string activeApproach;
    std::string activeSignal;

    for (const auto& approach : approaches) {
        const std::string roadId = approach.at("id").get<std::string>();
        const std::string signal = approach.at("signal").get<std::string>();
        const auto& vehicles = approach.at("vehicles");
        const std::size_t queueLength =
            approach.at("queueLength").get<std::size_t>();

        require(vehicles.is_array(), "Approach vehicles must be an array.");
        require(queueLength == vehicles.size(),
                "Queue length must match its vehicle array.");
        require(signal == "red" || signal == "yellow" || signal == "green",
                "Approach signal must be a known state.");
        if (signal != "red") {
            ++activeSignals;
            activeApproach = roadId;
            activeSignal = signal;
        }

        totalWaiting += queueLength;
        int previousId = 0;
        for (const auto& vehicle : vehicles) {
            const int id = vehicle.at("id").get<int>();
            require(id > previousId, "Vehicle IDs must be FIFO-increasing per lane.");
            require(allVehicleIds.insert(id).second,
                    "A vehicle ID must not occur in more than one queue.");
            require(vehicle.at("lane") == roadId,
                    "Vehicle lane must match its approach.");
            previousId = id;
        }
    }

    require(activeSignals == 1,
            "Exactly one approach must be active (GREEN or YELLOW).");
    require(data.at("activeApproach") == activeApproach,
            "Active approach must match the non-RED approach.");
    require(data.at("activeSignal") == activeSignal,
            "Active signal must match its approach signal state.");

    const auto& statistics = data.at("statistics");
    require(statistics.at("vehiclesWaiting").get<std::size_t>() == totalWaiting,
            "Waiting statistic must match queue contents.");
    require(statistics.at("vehiclesGenerated").get<std::uint64_t>() ==
            statistics.at("vehiclesProcessed").get<std::uint64_t>() + totalWaiting,
            "Generated count must equal processed plus waiting vehicles.");
    require(statistics.at("approaches").is_array() &&
            statistics.at("approaches").size() == 4,
            "Statistics must contain four approach entries.");
}

void testConcurrentHttpReadsCommandsAndTicksRemainConsistent() {
    SimulationService service;
    SimulationHttpServer server(service, "127.0.0.1", 0, 8);
    std::string error;
    CHECK(server.start(error));
    CHECK(server.port() != 0);

    const Json acceptedConfig = {
        {"tickMs", 100},
        {"greenTicks", 2},
        {"yellowTicks", 1},
        {"arrivalInterval", 1},
        {"maxQueue", 20}
    };
    CHECK(putConfig(server.port(), acceptedConfig).status == 200);
    CHECK(post(server.port(), "/api/simulation/start").status == 200);
    CHECK(waitForTickCount(service, 1, std::chrono::seconds(5)));

    std::atomic<bool> begin{false};
    std::mutex failureMutex;
    std::string failure;
    const auto recordFailure = [&](const std::string& message) {
        std::lock_guard<std::mutex> lock(failureMutex);
        if (failure.empty()) {
            failure = message;
        }
    };

    std::vector<std::thread> readers;
    for (int reader = 0; reader < 5; ++reader) {
        readers.emplace_back([&] {
            while (!begin.load(std::memory_order_acquire)) {
                std::this_thread::yield();
            }
            try {
                for (int request = 0; request < 20; ++request) {
                    validateConcurrentSnapshot(getStateData(server.port()));
                }
            } catch (const std::exception& exception) {
                recordFailure(exception.what());
            }
        });
    }

    std::thread controller([&] {
        while (!begin.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }
        try {
            for (int cycle = 0; cycle < 3; ++cycle) {
                // Configuration is locked while the scheduler is running.
                const Json rejectedConfig = {
                    {"tickMs", 250},
                    {"greenTicks", 12},
                    {"yellowTicks", 3},
                    {"arrivalInterval", 4},
                    {"maxQueue", 15}
                };
                const HttpResponse locked = putConfig(server.port(), rejectedConfig);
                if (locked.status != 409) {
                    throw std::runtime_error(
                        "Running-state configuration update must return HTTP 409."
                    );
                }

                const HttpResponse pauseResponse =
                    post(server.port(), "/api/simulation/pause");
                if (pauseResponse.status != 200) {
                    throw std::runtime_error("Pause command was not acknowledged.");
                }

                const Json paused = getStateData(server.port());
                validateConcurrentSnapshot(paused);
                if (paused.at("state") != "paused") {
                    throw std::runtime_error(
                        "Pause acknowledgement must correspond to PAUSED state."
                    );
                }

                // The pause acknowledgement is synchronous with the scheduler
                // mutex. Immediate API snapshots must remain byte-for-byte stable.
                for (int read = 0; read < 8; ++read) {
                    const Json stable = getStateData(server.port());
                    if (stable != paused) {
                        throw std::runtime_error(
                            "Simulation changed after pause acknowledgement."
                        );
                    }
                }

                const HttpResponse resetResponse =
                    post(server.port(), "/api/simulation/reset");
                if (resetResponse.status != 200) {
                    throw std::runtime_error("Reset command was not acknowledged.");
                }

                const Json reset = getStateData(server.port());
                validateConcurrentSnapshot(reset);
                if (reset.at("state") != "stopped" ||
                    reset.at("tickCount") != 0 ||
                    reset.at("statistics").at("vehiclesGenerated") != 0 ||
                    reset.at("statistics").at("vehiclesProcessed") != 0 ||
                    reset.at("statistics").at("vehiclesWaiting") != 0) {
                    throw std::runtime_error(
                        "Reset acknowledgement must correspond to a clean STOPPED state."
                    );
                }
                for (const auto& approach : reset.at("approaches")) {
                    if (approach.at("queueLength") != 0 ||
                        !approach.at("vehicles").empty()) {
                        throw std::runtime_error(
                            "Reset acknowledgement must clear every queue."
                        );
                    }
                }
                if (reset.at("config") != acceptedConfig) {
                    throw std::runtime_error(
                        "Reset must preserve the last accepted configuration."
                    );
                }

                if (putConfig(server.port(), acceptedConfig).status != 200) {
                    throw std::runtime_error(
                        "Configuration must be accepted after reset."
                    );
                }
                if (post(server.port(), "/api/simulation/start").status != 200) {
                    throw std::runtime_error("Restart after reset must succeed.");
                }
                if (!waitForTickCount(service, 1, std::chrono::seconds(5))) {
                    throw std::runtime_error(
                        "Service scheduler did not resume after restart."
                    );
                }
            }

            const HttpResponse finalPause =
                post(server.port(), "/api/simulation/pause");
            if (finalPause.status != 200) {
                throw std::runtime_error("Final pause command was not acknowledged.");
            }
            const Json finalPaused = getStateData(server.port());
            for (int read = 0; read < 8; ++read) {
                if (getStateData(server.port()) != finalPaused) {
                    throw std::runtime_error(
                        "Final paused snapshot changed after acknowledgement."
                    );
                }
            }
        } catch (const std::exception& exception) {
            recordFailure(exception.what());
        }
    });

    begin.store(true, std::memory_order_release);
    controller.join();
    for (auto& reader : readers) {
        reader.join();
    }

    CHECK(failure.empty());
    CHECK(service.getState() == SimModel::State::Paused);
    CHECK(server.stop());
}

} // namespace

int main() {
    testDeterministicLogicalTicksAndFifoVehicleIds();
    testDeterministicArrivalsSignalTransitionsAndSignalGatedMovement();
    testRestSnapshotExposesActualFifoVehiclesAndConsistentCounts();
    testRejectedConfigurationPreservesLastAcceptedValues();
    testConcurrentHttpReadsCommandsAndTicksRemainConsistent();

    std::cout << "All simulation_api_contract tests passed.\n";
    return EXIT_SUCCESS;
}
