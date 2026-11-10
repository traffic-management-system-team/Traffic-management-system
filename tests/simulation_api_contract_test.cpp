#include "simulation_http_server.h"
#include "simulation_state_serializer.h"

#include <asio.hpp>
#include <nlohmann/json.hpp>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
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

} // namespace

int main() {
    testDeterministicLogicalTicksAndFifoVehicleIds();
    testRestSnapshotExposesActualFifoVehiclesAndConsistentCounts();
    testRejectedConfigurationPreservesLastAcceptedValues();

    std::cout << "All simulation_api_contract tests passed.\n";
    return EXIT_SUCCESS;
}
