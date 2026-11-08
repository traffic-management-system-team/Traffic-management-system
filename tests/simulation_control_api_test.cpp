
#include "simulation_control_api.h"

#include <cstdlib>
#include <iostream>
#include <string>

namespace {

ApiRequest command(const std::string& path) {
    return ApiRequest{"POST", path, true, {}};
}

ApiJsonField integer(std::int64_t value) {
    return ApiJsonField{ApiJsonType::Integer, value};
}

void check(bool condition, const char* expression, int line) {
    if (!condition) {
        std::cerr << "CHECK FAILED at line " << line
                  << ": " << expression << '\n';
        std::exit(EXIT_FAILURE);
    }
}

#define CHECK(condition) check((condition), #condition, __LINE__)

ApiRequest validConfigRequest() {
    return ApiRequest{"PUT", "/api/simulation/config", true, {
        {"tickMs", integer(750)},
        {"greenTicks", integer(12)},
        {"yellowTicks", integer(3)},
        {"arrivalInterval", integer(4)},
        {"maxQueue", integer(15)}
    }};
}

} // namespace

int main() {
    SimulationService service;
    SimulationControlApi api(service);

    // Pause is invalid when the simulation is stopped.
    auto result = api.handle(command("/api/simulation/pause"));
    CHECK(result.statusCode == 409);
    CHECK(result.body.find("INVALID_STATE") != std::string::npos);

    // Start the simulation.
    result = api.handle(command("/api/simulation/start"));
    CHECK(result.statusCode == 200);
    CHECK(result.body.find("\"state\":\"running\"") != std::string::npos);

    // Starting an already running simulation must fail.
    result = api.handle(command("/api/simulation/start"));
    CHECK(result.statusCode == 409);

    // Pause the simulation.
    result = api.handle(command("/api/simulation/pause"));
    CHECK(result.statusCode == 200);
    CHECK(result.body.find("\"state\":\"paused\"") != std::string::npos);

    // Resume the simulation.
    result = api.handle(command("/api/simulation/resume"));
    CHECK(result.statusCode == 200);

    // Reset the simulation.
    result = api.handle(command("/api/simulation/reset"));
    CHECK(result.statusCode == 200);
    CHECK(result.body.find("\"state\":\"stopped\"") != std::string::npos);
    CHECK(result.body.find("\"tickMs\":1000") != std::string::npos);

    // Reset must be idempotent.
    result = api.handle(command("/api/simulation/reset"));
    CHECK(result.statusCode == 200);

    // Apply a valid configuration while stopped.
    ApiRequest config = validConfigRequest();

    result = api.handle(config);
    CHECK(result.statusCode == 200);
    CHECK(result.body.find("\"tickMs\":750") != std::string::npos);
    CHECK(result.body.find("\"greenTicks\":12") != std::string::npos);
    CHECK(result.body.find("\"yellowTicks\":3") != std::string::npos);
    CHECK(result.body.find("\"arrivalInterval\":4") != std::string::npos);
    CHECK(result.body.find("\"maxQueue\":15") != std::string::npos);

    // Wrong JSON type must be rejected.
    config.objectBody["tickMs"].type = ApiJsonType::String;

    result = api.handle(config);
    CHECK(result.statusCode == 400);

    // Restore tickMs and test an out-of-range value.
    config.objectBody["tickMs"] = integer(750);
    config.objectBody["greenTicks"] = integer(0);

    result = api.handle(config);
    CHECK(result.statusCode == 400);
    CHECK(result.body.find("VALIDATION_FAILED") != std::string::npos);
    CHECK(result.body.find("\"details\"") != std::string::npos);

    // Missing configuration field must be rejected.
    config.objectBody["greenTicks"] = integer(12);
    config.objectBody.erase("maxQueue");

    result = api.handle(config);
    CHECK(result.statusCode == 400);

    // Unknown configuration field must be rejected.
    config.objectBody["maxQueue"] = integer(15);
    config.objectBody["unexpected"] = integer(1);

    result = api.handle(config);
    CHECK(result.statusCode == 400);

    config.objectBody.erase("unexpected");

    // Malformed JSON must be rejected.
    ApiRequest malformed{"PUT", "/api/simulation/config", false, {}};

    result = api.handle(malformed);
    CHECK(result.statusCode == 400);
    CHECK(result.body.find("MALFORMED_JSON") != std::string::npos);

    // A valid configuration must be rejected while running.
    result = api.handle(command("/api/simulation/start"));
    CHECK(result.statusCode == 200);

    result = api.handle(config);
    CHECK(result.statusCode == 409);
    CHECK(result.body.find("INVALID_STATE") != std::string::npos);

    // Invalid values must still be classified as validation errors,
    // even when the simulation is running.
    ApiRequest invalidConfig = validConfigRequest();
    invalidConfig.objectBody["greenTicks"] = integer(0);

    result = api.handle(invalidConfig);
    CHECK(result.statusCode == 400);
    CHECK(result.body.find("VALIDATION_FAILED") != std::string::npos);

    // Pause and verify that configuration is also rejected while paused.
    result = api.handle(command("/api/simulation/pause"));
    CHECK(result.statusCode == 200);

    result = api.handle(config);
    CHECK(result.statusCode == 409);
    CHECK(result.body.find("INVALID_STATE") != std::string::npos);

    // Reset must preserve the previously accepted configuration.
    result = api.handle(command("/api/simulation/reset"));
    CHECK(result.statusCode == 200);
    CHECK(result.body.find("\"state\":\"stopped\"") != std::string::npos);
    CHECK(result.body.find("\"tickMs\":750") != std::string::npos);
    CHECK(result.body.find("\"greenTicks\":12") != std::string::npos);
    CHECK(result.body.find("\"yellowTicks\":3") != std::string::npos);
    CHECK(result.body.find("\"arrivalInterval\":4") != std::string::npos);
    CHECK(result.body.find("\"maxQueue\":15") != std::string::npos);

    // Unknown endpoint must return 404.
    result = api.handle(command("/api/simulation/unknown"));
    CHECK(result.statusCode == 404);

    std::cout << "All simulation_control_api tests passed.\n";
    return EXIT_SUCCESS;
}
