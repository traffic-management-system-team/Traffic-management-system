#include "simulation_http_server.h"
#include "simulation_http_limits.h"

#include <nlohmann/json.hpp>

#include <asio.hpp>

#include <algorithm>
#include <cmath>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace {

bool setPublicOrigin(const char* value) {
#ifdef _WIN32
    return _putenv_s("TMS_PUBLIC_ORIGIN", value == nullptr ? "" : value) == 0;
#else
    if (value == nullptr) {
        return unsetenv("TMS_PUBLIC_ORIGIN") == 0;
    }
    return setenv("TMS_PUBLIC_ORIGIN", value, 1) == 0;
#endif
}

class ScopedPublicOriginEnvironment {
public:
    ScopedPublicOriginEnvironment() {
#ifdef _WIN32
        char* value = nullptr;
        std::size_t length = 0;
        if (_dupenv_s(&value, &length, "TMS_PUBLIC_ORIGIN") != 0) {
            throw std::runtime_error("Unable to capture TMS_PUBLIC_ORIGIN.");
        }
        if (value != nullptr) {
            hadOriginalValue_ = true;
            originalValue_ = value;
            std::free(value);
        }
#else
        const char* value = std::getenv("TMS_PUBLIC_ORIGIN");
        if (value != nullptr) {
            hadOriginalValue_ = true;
            originalValue_ = value;
        }
#endif
    }

    ~ScopedPublicOriginEnvironment() {
        setPublicOrigin(hadOriginalValue_ ? originalValue_.c_str() : nullptr);
    }

private:
    std::string originalValue_;
    bool hadOriginalValue_ = false;
};

struct HttpResponse {
    int status = 0;
    std::string contentType;
    std::string allow;
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

std::string lower(std::string value) {
    for (char& character : value) {
        if (character >= 'A' && character <= 'Z') {
            character = static_cast<char>(character - 'A' + 'a');
        }
    }
    return value;
}

HttpResponse sendRequest(
    std::uint16_t port,
    const std::string& method,
    const std::string& path,
    const std::string& body = {},
    const std::string& contentType = {},
    const std::string& origin = {},
    const std::string& fetchSite = {},
    const std::string& forwardedProto = {},
    const std::string& forwardedHost = {},
    bool chunkedTransfer = false
) {
    asio::io_context context;
    asio::ip::tcp::socket socket(context);
    socket.connect(asio::ip::tcp::endpoint(
        asio::ip::make_address("127.0.0.1"),
        port
    ));

    std::ostringstream request;
    request << method << ' ' << path << " HTTP/1.1\r\n"
            << "Host: 127.0.0.1:" << port << "\r\n";
    if (!contentType.empty()) {
        request << "Content-Type: " << contentType << "\r\n";
    }
    if (!origin.empty()) {
        request << "Origin: " << origin << "\r\n";
    }
    if (!fetchSite.empty()) {
        request << "Sec-Fetch-Site: " << fetchSite << "\r\n";
    }
    if (!forwardedProto.empty()) {
        request << "X-Forwarded-Proto: " << forwardedProto << "\r\n";
    }
    if (!forwardedHost.empty()) {
        request << "X-Forwarded-Host: " << forwardedHost << "\r\n";
    }
    if (chunkedTransfer) {
        request << "Transfer-Encoding: chunked\r\n";
    } else {
        request << "Content-Length: " << body.size() << "\r\n";
    }

    request << "Connection: close\r\n\r\n";

    if (chunkedTransfer) {
        constexpr std::size_t chunkSize = 4096;
        for (std::size_t offset = 0; offset < body.size();) {
            const std::size_t length = std::min(
                chunkSize, body.size() - offset
            );

            std::ostringstream chunkLength;
            chunkLength << std::hex << length;
            request << chunkLength.str() << "\r\n";
            request.write(
                body.data() + offset,
                static_cast<std::streamsize>(length)
            );
            request << "\r\n";
            offset += length;
        }
        request << "0\r\n\r\n";
    } else {
        request << body;
    }

    const std::string requestData = request.str();
    asio::write(socket, asio::buffer(requestData));

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

        const std::string name = lower(header.substr(0, separator));
        std::string value = header.substr(separator + 1);
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
        } else if (name == "allow") {
            response.allow = value;
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

HttpResponse sendJsonRequest(
    std::uint16_t port,
    const std::string& method,
    const std::string& path,
    const std::string& body
) {
    return sendRequest(port, method, path, body, "application/json");
}

const std::string kValidConfig =
    "{\"tickMs\":100,\"greenTicks\":12,\"yellowTicks\":3,"
    "\"arrivalInterval\":4,\"maxQueue\":15}";

nlohmann::json getStateData(std::uint16_t port) {
    const HttpResponse response = sendRequest(
        port,
        "GET",
        "/api/simulation/state"
    );
    CHECK(response.status == 200);
    CHECK(response.contentType == "application/json");

    const auto envelope = nlohmann::json::parse(response.body);
    CHECK(envelope.at("success") == true);
    CHECK(envelope.at("error").is_null());
    CHECK(envelope.at("data").is_object());
    return envelope.at("data");
}

void checkStateDataContract(const nlohmann::json& state) {
    const std::vector<std::string> expectedApproaches = {
        "north", "east", "south", "west"
    };
    const std::vector<std::string> expectedStatisticsOrder = {
        "north", "south", "east", "west"
    };
    const std::vector<std::string> validSignals = {
        "red", "yellow", "green"
    };

    CHECK(state.at("state").is_string());
    CHECK(state.at("tickCount").is_number_unsigned() ||
          state.at("tickCount").is_number_integer());
    CHECK(state.at("activeApproach").is_string());
    CHECK(state.at("activeSignal").is_string());

    const auto& approaches = state.at("approaches");
    CHECK(approaches.is_array());
    CHECK(approaches.size() == 4);

    for (std::size_t i = 0; i < approaches.size(); ++i) {
        const auto& approach = approaches[i];

        CHECK(approach.at("id") == expectedApproaches[i]);
        CHECK(approach.at("name").is_string());
        CHECK(approach.at("signal").is_string());
        CHECK(std::find(
            validSignals.begin(),
            validSignals.end(),
            approach.at("signal").get<std::string>()
        ) != validSignals.end());

        CHECK(approach.at("queueLength").is_number());
        CHECK(approach.at("vehicles").is_array());
        CHECK(approach.at("queueLength") == approach.at("vehicles").size());

        for (const auto& vehicle : approach.at("vehicles")) {
            CHECK(vehicle.at("id").is_number());
            CHECK(vehicle.at("type").is_string());
            CHECK(vehicle.at("lane").is_string());
        }
    }

    const auto& config = state.at("config");
    CHECK(config.at("tickMs").is_number());
    CHECK(config.at("greenTicks").is_number());
    CHECK(config.at("yellowTicks").is_number());
    CHECK(config.at("arrivalInterval").is_number());
    CHECK(config.at("maxQueue").is_number());

    const auto& statistics = state.at("statistics");
    CHECK(statistics.at("tickCount") == state.at("tickCount"));
    CHECK(statistics.at("vehiclesGenerated").is_number());
    CHECK(statistics.at("vehiclesProcessed").is_number());
    CHECK(statistics.at("vehiclesWaiting").is_number());
    CHECK(statistics.at("totalWaitingTicks").is_number());
    CHECK(statistics.at("averageWaitingTicks").is_number());
    CHECK(statistics.at("processedPerLogicalTick").is_number());

    const auto processed =
        statistics.at("vehiclesProcessed").get<double>();
    const auto totalWaiting =
        statistics.at("totalWaitingTicks").get<double>();
    const auto averageWaiting =
        statistics.at("averageWaitingTicks").get<double>();

    const double expectedAverage =
        processed == 0.0 ? 0.0 : totalWaiting / processed;

    CHECK(std::abs(averageWaiting - expectedAverage) < 1e-9);

    const auto& approachStatistics = statistics.at("approaches");
    CHECK(approachStatistics.is_array());
    CHECK(approachStatistics.size() == 4);

    for (std::size_t i = 0; i < approachStatistics.size(); ++i) {
        const auto& approach = approachStatistics[i];
        CHECK(approach.at("id") == expectedStatisticsOrder[i]);
        CHECK(approach.at("vehiclesGenerated").is_number());
        CHECK(approach.at("vehiclesProcessed").is_number());
        CHECK(approach.at("vehiclesWaiting").is_number());
    }
}

void testAcceptedConfiguredOrigin(const std::string& origin) {
    CHECK(setPublicOrigin(origin.c_str()));
    SimulationService service;
    SimulationHttpServer server(service, "127.0.0.1", 0);
    std::string error;
    CHECK(server.start(error));

    const HttpResponse response = sendRequest(
        server.port(), "GET", "/api/simulation/state", {}, {},
        origin, "same-origin"
    );
    CHECK(response.status == 200);
    CHECK(server.stop());
    CHECK(setPublicOrigin(nullptr));
}

void testConfiguredPublicOrigin() {
    CHECK(setPublicOrigin(nullptr));
    {
        SimulationService service;
        SimulationHttpServer server(service, "127.0.0.1", 0);
        std::string error;
        CHECK(server.start(error));

        const std::string localOrigin =
            "http://127.0.0.1:" + std::to_string(server.port());
        HttpResponse response = sendRequest(
            server.port(), "GET", "/api/simulation/state", {}, {},
            localOrigin, "same-origin"
        );
        CHECK(response.status == 200);
        CHECK(sendRequest(server.port(), "GET", "/api/simulation/state").status == 200);
        CHECK(server.stop());
    }

    CHECK(setPublicOrigin("https://traffic.example:8443"));
    {
        SimulationService service;
        SimulationHttpServer server(service, "127.0.0.1", 0);
        std::string error;
        CHECK(server.start(error));

        HttpResponse response = sendRequest(
            server.port(), "GET", "/api/simulation/state", {}, {},
            "https://traffic.example:8443", "same-origin",
            "http", "attacker.example"
        );
        CHECK(response.status == 200);

        const std::vector<std::string> mismatchedOrigins = {
            "http://traffic.example:8443",
            "https://attacker.example:8443",
            "https://traffic.example:9443"
        };
        for (const auto& origin : mismatchedOrigins) {
            response = sendRequest(
                server.port(), "GET", "/api/simulation/state", {}, {},
                origin, "same-origin", "https", "traffic.example:8443"
            );
            CHECK(response.status == 403);
            CHECK(response.body.find("\"code\":\"CROSS_ORIGIN_REQUEST\"") !=
                  std::string::npos);
        }
        // Cross-origin rejection takes precedence over oversized-body rejection.
        const std::string oversizedBody(
            SimulationHttpLimits::MaxRequestBodyBytes + 1, 'x'
        );
        response = sendRequest(
            server.port(), "GET", "/api/simulation/state",
            oversizedBody, {}, "https://attacker.example:8443",
            "same-origin", "https", "traffic.example:8443"
        );
        CHECK(response.status == 403);
        CHECK(response.contentType == "application/json");

        const auto oversizedOriginEnvelope =
            nlohmann::json::parse(response.body);
        CHECK(oversizedOriginEnvelope.at("success") == false);
        CHECK(oversizedOriginEnvelope.at("data").is_null());
        CHECK(oversizedOriginEnvelope.at("error").at("code") ==
              "CROSS_ORIGIN_REQUEST");
        CHECK(oversizedOriginEnvelope.at("error").at("message").is_string());
        CHECK(sendRequest(server.port(), "GET", "/api/simulation/state").status == 200);
        CHECK(server.stop());
    }

    const std::string overlongLabel = std::string(64, 'a') + ".example";
    const std::vector<std::string> invalidOrigins = {
        "https://traffic.example/path",
        "https://traffic.example:99999",
        "https://[::::]",
        "https://" + overlongLabel,
        "https://999.999.999.999"
    };
    for (const auto& invalidOrigin : invalidOrigins) {
        CHECK(setPublicOrigin(invalidOrigin.c_str()));
        SimulationService service;
        SimulationHttpServer server(service, "127.0.0.1", 0);
        std::string error;
        CHECK(!server.start(error));
        CHECK(!error.empty());
        CHECK(!server.isRunning());
    }
    CHECK(setPublicOrigin(nullptr));

    const std::string maxLengthLabel = std::string(63, 'a') + ".example";
    testAcceptedConfiguredOrigin("https://" + maxLengthLabel);
    testAcceptedConfiguredOrigin("https://192.0.2.1");
    testAcceptedConfiguredOrigin("https://[2001:db8::1]:8443");
}

void testUnsupportedMethodsUseJsonEnvelope() {
    SimulationService service;
    SimulationHttpServer server(service, "127.0.0.1", 0);
    std::string error;
    CHECK(server.start(error));

    const auto checkMethodNotAllowed = [&](const HttpResponse& response,
                                           const std::string& allow) {
        CHECK(response.status == 405);
        CHECK(response.contentType == "application/json");
        CHECK(response.allow == allow);
        const auto envelope = nlohmann::json::parse(response.body);
        CHECK(envelope.at("success") == false);
        CHECK(envelope.at("data").is_null());
        CHECK(envelope.at("error").at("code") == "METHOD_NOT_ALLOWED");
        CHECK(envelope.at("error").at("message").is_string());
    };

    checkMethodNotAllowed(
        sendRequest(server.port(), "POST", "/api/simulation/state"), "GET"
    );
    // An oversized POST must return 405 before body-size validation.
    const std::string oversizedPostBody(
        SimulationHttpLimits::MaxRequestBodyBytes + 1, 'x'
    );
    checkMethodNotAllowed(
        sendRequest(
            server.port(), "POST", "/api/simulation/state",
            oversizedPostBody
        ),
        "GET"
    );
    checkMethodNotAllowed(
        sendRequest(server.port(), "GET", "/api/simulation/config"), "PUT"
    );

    const HttpResponse unknown =
        sendRequest(server.port(), "GET", "/api/simulation/not-found");
    CHECK(unknown.status == 404);
    CHECK(unknown.contentType == "application/json");
    const auto unknownEnvelope = nlohmann::json::parse(unknown.body);
    CHECK(unknownEnvelope.at("success") == false);
    CHECK(unknownEnvelope.at("data").is_null());
    CHECK(unknownEnvelope.at("error").at("code") == "NOT_FOUND");
    CHECK(unknownEnvelope.at("error").at("message").is_string());

    const HttpResponse crossOriginUnknown = sendRequest(
        server.port(),
        "GET",
        "/api/not-found",
        {},
        {},
        "https://attacker.example",
        "same-origin"
    );
    CHECK(crossOriginUnknown.status == 403);
    CHECK(crossOriginUnknown.contentType == "application/json");
    const auto crossOriginEnvelope =
        nlohmann::json::parse(crossOriginUnknown.body);
    CHECK(crossOriginEnvelope.at("success") == false);
    CHECK(crossOriginEnvelope.at("data").is_null());
    CHECK(crossOriginEnvelope.at("error").at("code") ==
          "CROSS_ORIGIN_REQUEST");

    CHECK(server.stop());
}

void testControlRoutesAndResponsePassthrough() {
    SimulationService service;
    SimulationHttpServer server(service, "127.0.0.1", 0);
    std::string error;
    CHECK(server.start(error));
    CHECK(error.empty());
    CHECK(server.isRunning());
    CHECK(server.port() != 0);

    auto state = getStateData(server.port());
    checkStateDataContract(state);
    CHECK(state.at("state") == "stopped");
    CHECK(state.at("activeApproach") == "north");
    CHECK(state.at("activeSignal") == "green");
    CHECK(state.at("approaches").size() == 4);

    HttpResponse response = post(server.port(), "/api/simulation/pause");
    CHECK(response.status == 409);
    CHECK(response.contentType == "application/json");
    CHECK(response.body ==
          "{\"success\":false,\"data\":null,\"error\":{\"code\":\"INVALID_STATE\","
          "\"message\":\"Simulation can only pause when running.\"}}");

    response = post(server.port(), "/api/simulation/start");
    CHECK(response.status == 200);
    CHECK(response.contentType == "application/json");
    CHECK(response.body ==
          "{\"success\":true,\"data\":{\"state\":\"running\"},\"error\":null}");
    state = getStateData(server.port());
    checkStateDataContract(state);
    CHECK(state.at("state") == "running");
    CHECK(state.at("activeApproach") == "north");

    response = post(server.port(), "/api/simulation/start");
    CHECK(response.status == 409);
    CHECK(response.body.find("\"code\":\"INVALID_STATE\"") != std::string::npos);

    response = post(server.port(), "/api/simulation/pause");
    CHECK(response.status == 200);
    CHECK(response.body ==
          "{\"success\":true,\"data\":{\"state\":\"paused\"},\"error\":null}");
    state = getStateData(server.port());
    checkStateDataContract(state);
    CHECK(state.at("state") == "paused");

    const auto pausedSnapshot = state;
    for (int i = 0; i < 5; ++i) {
        const auto repeatedRead = getStateData(server.port());
        checkStateDataContract(repeatedRead);
        CHECK(repeatedRead == pausedSnapshot);
    }

    response = post(server.port(), "/api/simulation/resume");
    CHECK(response.status == 200);
    CHECK(response.body ==
          "{\"success\":true,\"data\":{\"state\":\"running\"},\"error\":null}");
    state = getStateData(server.port());
    checkStateDataContract(state);
    CHECK(state.at("state") == "running");

    response = post(server.port(), "/api/simulation/reset");
    CHECK(response.status == 200);
    CHECK(response.body ==
          "{\"success\":true,\"data\":{\"state\":\"stopped\",\"config\":"
          "{\"tickMs\":1000,\"greenTicks\":10,\"yellowTicks\":2,"
          "\"arrivalInterval\":3,\"maxQueue\":12}},\"error\":null}");
    state = getStateData(server.port());
    checkStateDataContract(state);
    CHECK(state.at("state") == "stopped");

    response = sendJsonRequest(
        server.port(),
        "PUT",
        "/api/simulation/config",
        kValidConfig
    );
    CHECK(response.status == 200);
    CHECK(response.contentType == "application/json");
    CHECK(response.body ==
          "{\"success\":true,\"data\":{\"config\":{\"tickMs\":100,"
          "\"greenTicks\":12,\"yellowTicks\":3,\"arrivalInterval\":4,"
          "\"maxQueue\":15}},\"error\":null}");

    response = post(server.port(), "/api/simulation/reset");
    CHECK(response.status == 200);
    CHECK(response.body.find("\"tickMs\":100") != std::string::npos);

    response = sendJsonRequest(
        server.port(),
        "POST",
        "/api/simulation/config",
        kValidConfig
    );
    CHECK(response.status == 404 || response.status == 405);

    CHECK(server.stop());
    CHECK(!server.isRunning());

    bool connectionFailed = false;
    try {
        post(server.port(), "/api/simulation/start");
    } catch (const std::exception&) {
        connectionFailed = true;
    }
    CHECK(connectionFailed);
}

void testMalformedAndWrongTypeJson() {
    SimulationService service;
    SimulationHttpServer server(service, "127.0.0.1", 0);
    std::string error;
    CHECK(server.start(error));

    HttpResponse response = sendJsonRequest(
        server.port(),
        "PUT",
        "/api/simulation/config",
        "{\"tickMs\":"
    );
    CHECK(response.status == 400);
    CHECK(response.body.find("\"code\":\"MALFORMED_JSON\"") !=
          std::string::npos);

    const std::vector<std::string> wrongTypes = {
        "\"100\"", "100.0", "true", "null", "[]", "{}",
        "9223372036854775808",
        "999999999999999999999999999999999999999"
    };
    for (const std::string& value : wrongTypes) {
        const std::string body =
            "{\"tickMs\":" + value +
            ",\"greenTicks\":12,\"yellowTicks\":3,"
            "\"arrivalInterval\":4,\"maxQueue\":15}";
        response = sendJsonRequest(
            server.port(),
            "PUT",
            "/api/simulation/config",
            body
        );
        CHECK(response.status == 400);
        CHECK(response.contentType == "application/json");
        CHECK(response.body.find("\"success\":false") != std::string::npos);
    }

    response = sendJsonRequest(
        server.port(),
        "PUT",
        "/api/simulation/config",
        "{\"tickMs\":100,\"greenTicks\":0,\"yellowTicks\":3,"
        "\"arrivalInterval\":4,\"maxQueue\":15}"
    );
    CHECK(response.status == 400);
    CHECK(response.body.find("\"code\":\"VALIDATION_FAILED\"") !=
          std::string::npos);
    CHECK(response.body.find("\"details\"") != std::string::npos);

    response = sendJsonRequest(
        server.port(),
        "PUT",
        "/api/simulation/config",
        "{\"tickMs\":100,\"greenTicks\":12,\"yellowTicks\":3,"
        "\"arrivalInterval\":4}"
    );
    CHECK(response.status == 400);
    CHECK(response.body.find("\"code\":\"INVALID_REQUEST\"") !=
          std::string::npos);

    CHECK(server.stop());
}

void testContentTypeAndBodyRequirements() {
    SimulationService service;
    SimulationHttpServer server(service, "127.0.0.1", 0);
    std::string error;
    CHECK(server.start(error));

    HttpResponse response = sendRequest(
        server.port(),
        "POST",
        "/api/simulation/start",
        {},
        "application/x-www-form-urlencoded"
    );
    CHECK(response.status == 415);
    CHECK(response.body.find("\"code\":\"UNSUPPORTED_MEDIA_TYPE\"") !=
          std::string::npos);

    response = sendRequest(
        server.port(),
        "POST",
        "/api/simulation/start",
        {},
        {},
        "https://attacker.example",
        "cross-site"
    );
    CHECK(response.status == 403);
    CHECK(response.body.find("\"code\":\"CROSS_ORIGIN_REQUEST\"") !=
          std::string::npos);

    response = sendRequest(
        server.port(),
        "GET",
        "/api/simulation/state",
        {},
        {},
        "https://attacker.example",
        "cross-site"
    );
    CHECK(response.status == 403);
    CHECK(response.body.find("\"code\":\"CROSS_ORIGIN_REQUEST\"") !=
          std::string::npos);

    response = post(server.port(), "/api/simulation/start");
    CHECK(response.status == 200);
    response = sendRequest(
        server.port(),
        "POST",
        "/api/simulation/reset",
        {},
        "text/plain"
    );
    CHECK(response.status == 415);
    CHECK(post(server.port(), "/api/simulation/pause").status == 200);
    CHECK(post(server.port(), "/api/simulation/reset").status == 200);

    response = sendRequest(
        server.port(),
        "PUT",
        "/api/simulation/config",
        kValidConfig
    );
    CHECK(response.status == 415);
    response = sendRequest(
        server.port(),
        "PUT",
        "/api/simulation/config",
        kValidConfig,
        "text/plain"
    );
    CHECK(response.status == 415);

    response = sendJsonRequest(
        server.port(),
        "PUT",
        "/api/simulation/config",
        {}
    );
    CHECK(response.status == 400);
    CHECK(response.body.find("\"code\":\"INVALID_REQUEST\"") !=
          std::string::npos);

    response = sendRequest(
        server.port(),
        "PUT",
        "/api/simulation/config",
        kValidConfig,
        "Application/JSON; charset=utf-8"
    );
    CHECK(response.status == 200);
    CHECK(server.stop());
}

void testRequestBodySizeLimit() {
    SimulationService service;
    SimulationHttpServer server(service, "127.0.0.1", 0);
    std::string error;
    CHECK(server.start(error));

    HttpResponse response = sendJsonRequest(
        server.port(),
        "PUT",
        "/api/simulation/config",
        kValidConfig
    );
    CHECK(response.status == 200);

    std::string atLimit = kValidConfig;
    CHECK(atLimit.size() < SimulationHttpLimits::MaxRequestBodyBytes);
    atLimit.append(
        SimulationHttpLimits::MaxRequestBodyBytes - atLimit.size(),
        ' '
    );
    response = sendJsonRequest(
        server.port(),
        "PUT",
        "/api/simulation/config",
        atLimit
    );
    CHECK(response.status == 200);

    response = sendJsonRequest(
        server.port(),
        "PUT",
        "/api/simulation/config",
        atLimit + "x"
    );
    CHECK(response.status == 413);
    CHECK(response.contentType == "application/json");
    CHECK(response.body.find("\"code\":\"PAYLOAD_TOO_LARGE\"") !=
          std::string::npos);
    // Normal state GET must return the success envelope.
    const auto stateBefore = getStateData(server.port());

    // An oversized POST to a valid control route must be rejected before execution.
    const std::string oversizedControlBody(
        SimulationHttpLimits::MaxRequestBodyBytes + 1, 'x'
    );
    response = sendRequest(
        server.port(), "POST", "/api/simulation/start",
        oversizedControlBody
    );
    CHECK(response.status == 413);
    CHECK(response.contentType == "application/json");

    auto oversizedControlEnvelope = nlohmann::json::parse(response.body);
    CHECK(oversizedControlEnvelope.at("success") == false);
    CHECK(oversizedControlEnvelope.at("data").is_null());
    CHECK(oversizedControlEnvelope.at("error").at("code") ==
          "PAYLOAD_TOO_LARGE");
    CHECK(getStateData(server.port()) == stateBefore);

    response = sendRequest(
        server.port(), "GET", "/api/simulation/state"
    );
    CHECK(response.status == 200);
    CHECK(response.contentType == "application/json");

    auto envelope = nlohmann::json::parse(response.body);
    CHECK(envelope.at("success") == true);
    CHECK(envelope.at("error").is_null());
    CHECK(envelope.at("data") == stateBefore);

    // Exactly 64 KiB is accepted.
    const std::string stateBodyAtLimit(
        SimulationHttpLimits::MaxRequestBodyBytes, ' '
    );

    response = sendRequest(
        server.port(), "GET", "/api/simulation/state",
        stateBodyAtLimit
    );
    CHECK(response.status == 200);
    CHECK(response.contentType == "application/json");

    envelope = nlohmann::json::parse(response.body);
    CHECK(envelope.at("success") == true);
    CHECK(envelope.at("error").is_null());
    CHECK(envelope.at("data") == stateBefore);

    // One byte above the limit is rejected.
    response = sendRequest(
        server.port(), "GET", "/api/simulation/state",
        stateBodyAtLimit + " "
    );
    CHECK(response.status == 413);
    CHECK(response.contentType == "application/json");

    envelope = nlohmann::json::parse(response.body);
    CHECK(envelope.at("success") == false);
    CHECK(envelope.at("data").is_null());
    CHECK(envelope.at("error").at("code") == "PAYLOAD_TOO_LARGE");
    CHECK(envelope.at("error").at("message").is_string());

    // Chunked transfer: exactly 64 KiB is accepted.
    response = sendRequest(
        server.port(), "GET", "/api/simulation/state",
        stateBodyAtLimit, {}, {}, {}, {}, {}, true
    );
    CHECK(response.status == 200);
    CHECK(response.contentType == "application/json");

    envelope = nlohmann::json::parse(response.body);
    CHECK(envelope.at("success") == true);
    CHECK(envelope.at("error").is_null());
    CHECK(envelope.at("data") == stateBefore);

    // Chunked transfer: one byte above the limit is rejected.
    response = sendRequest(
        server.port(), "GET", "/api/simulation/state",
        stateBodyAtLimit + " ", {}, {}, {}, {}, {}, true
    );
    CHECK(response.status == 413);
    CHECK(response.contentType == "application/json");

    envelope = nlohmann::json::parse(response.body);
    CHECK(envelope.at("success") == false);
    CHECK(envelope.at("data").is_null());
    CHECK(envelope.at("error").at("code") == "PAYLOAD_TOO_LARGE");
    CHECK(envelope.at("error").at("message").is_string());
    // All requests above must leave simulation state unchanged.
    CHECK(getStateData(server.port()) == stateBefore);
    CHECK(server.stop());
}

void testConcurrentRequestsAndServiceScheduler() {
    SimulationService service;
    SimulationHttpServer server(service, "127.0.0.1", 0, 8);
    std::string error;
    CHECK(server.start(error));

    HttpResponse response = sendJsonRequest(
        server.port(),
        "PUT",
        "/api/simulation/config",
        kValidConfig
    );
    CHECK(response.status == 200);
    response = post(server.port(), "/api/simulation/start");
    CHECK(response.status == 200);
    std::this_thread::sleep_for(std::chrono::milliseconds(150));

    const auto runConcurrentStateReads = [&] {
        std::atomic<int> successCount{0};
        std::mutex errorMutex;
        std::string requestError;
        std::vector<std::thread> requests;

        for (int i = 0; i < 12; ++i) {
            requests.emplace_back([&] {
                try {
                    const auto state = getStateData(server.port());
                    checkStateDataContract(state);
                    CHECK(state.at("state") == "running");
                    ++successCount;
                } catch (const std::exception& exception) {
                    std::lock_guard<std::mutex> lock(errorMutex);
                    requestError = exception.what();
                }
            });
        }

        for (auto& request : requests) {
            request.join();
        }

        CHECK(requestError.empty());
        CHECK(successCount == 12);
    };

    runConcurrentStateReads();

    const auto runConcurrentCommands = [&](const std::string& path) {
        std::atomic<int> successCount{0};
        std::atomic<int> conflictCount{0};
        std::mutex errorMutex;
        std::string requestError;
        std::vector<std::thread> requests;
        for (int i = 0; i < 12; ++i) {
            requests.emplace_back([&] {
                try {
                    const HttpResponse result = post(server.port(), path);
                    if (result.status == 200) {
                        ++successCount;
                    } else if (result.status == 409) {
                        ++conflictCount;
                    } else {
                        std::lock_guard<std::mutex> lock(errorMutex);
                        requestError = "Unexpected HTTP status.";
                    }
                } catch (const std::exception& exception) {
                    std::lock_guard<std::mutex> lock(errorMutex);
                    requestError = exception.what();
                }
            });
        }
        for (auto& request : requests) {
            request.join();
        }
        CHECK(requestError.empty());
        CHECK(successCount == 1);
        CHECK(conflictCount == 11);
    };

    runConcurrentCommands("/api/simulation/pause");
    runConcurrentCommands("/api/simulation/resume");

    response = post(server.port(), "/api/simulation/reset");
    CHECK(response.status == 200);
    CHECK(server.stop());
}


void testServerShutdownDoesNotResetSimulation() {
    SimulationService service;
    SimulationHttpServer server(service, "127.0.0.1", 0);
    std::string error;
    CHECK(server.start(error));
    CHECK(post(server.port(), "/api/simulation/start").status == 200);
    CHECK(service.getState() == SimModel::State::Running);
    CHECK(server.stop());
    CHECK(service.getState() == SimModel::State::Running);
    service.reset();
}

void testStartupFailureAndShutdown() {
    SimulationService service;
    SimulationHttpServer first(service, "127.0.0.1", 0);
    std::string error;
    CHECK(first.start(error));

    SimulationHttpServer second(service, "not-an-ip-address", 0);
    CHECK(!second.start(error));
    CHECK(!error.empty());
    CHECK(first.stop());
    CHECK(!first.isRunning());
}

void testShutdownWhileWaiterAndRequestsAreActive() {
    SimulationService service;
    SimulationHttpServer server(service, "127.0.0.1", 0, 8);
    std::string error;
    CHECK(server.start(error));
    CHECK(post(server.port(), "/api/simulation/start").status == 200);

    std::atomic<int> completed{0};
    std::atomic<int> disconnected{0};
    std::atomic<bool> unexpectedStatus{false};
    std::vector<std::thread> requests;
    for (int i = 0; i < 32; ++i) {
        requests.emplace_back([&] {
            try {
                const HttpResponse response =
                    post(server.port(), "/api/simulation/pause");
                if (response.status == 200 || response.status == 409) {
                    ++completed;
                } else {
                    unexpectedStatus = true;
                }
            } catch (const std::exception&) {
                ++disconnected;
            }
        });
    }

    std::thread stopper([&] {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (!server.stop()) {
            unexpectedStatus = true;
        }
    });

    const bool shutdownCompleted = server.waitForShutdown(error);
    stopper.join();
    for (auto& request : requests) {
        request.join();
    }

    CHECK(!shutdownCompleted);
    CHECK(error == "The HTTP server was stopped.");
    CHECK(!server.isRunning());
    CHECK(completed + disconnected == 32);
    CHECK(!unexpectedStatus);
    service.reset();
    CHECK(service.getState() == SimModel::State::Stopped);
}

} // namespace


void runTest(const char* name, void (*test)()) {
    std::cerr << "[ RUN      ] " << name << '\n' << std::flush;
    test();
    std::cerr << "[       OK ] " << name << '\n' << std::flush;
}

int main() {
    ScopedPublicOriginEnvironment publicOriginEnvironment;
    CHECK(setPublicOrigin(nullptr));
    runTest("configured public origin", testConfiguredPublicOrigin);
    runTest("unsupported methods", testUnsupportedMethodsUseJsonEnvelope);
    runTest("control routes and response passthrough", testControlRoutesAndResponsePassthrough);
    runTest("malformed and wrong-type JSON", testMalformedAndWrongTypeJson);
    runTest("content type and body requirements", testContentTypeAndBodyRequirements);
    runTest("request body size limit", testRequestBodySizeLimit);
    runTest("concurrent requests and service scheduler", testConcurrentRequestsAndServiceScheduler);
    runTest("startup failure and shutdown", testStartupFailureAndShutdown);
    runTest("server shutdown preserves simulation", testServerShutdownDoesNotResetSimulation);
    runTest("shutdown with active waiters and requests", testShutdownWhileWaiterAndRequestsAreActive);

    std::cout << "All simulation_http_server tests passed.\n";
    return EXIT_SUCCESS;
}
