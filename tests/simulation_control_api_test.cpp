#include "simulation_control_api.h"

#include <cassert>
#include <iostream>

namespace {
ApiRequest command(const std::string& path) { return ApiRequest{"POST", path, true, {}}; }
ApiJsonField integer(std::int64_t value) { return ApiJsonField{ApiJsonType::Integer, value}; }
}

int main() {
    SimulationService service;
    SimulationControlApi api(service);

    auto result = api.handle(command("/api/simulation/pause"));
    assert(result.statusCode == 409);
    assert(result.body.find("INVALID_STATE") != std::string::npos);

    result = api.handle(command("/api/simulation/start"));
    assert(result.statusCode == 200);
    assert(result.body.find("\"state\":\"running\"") != std::string::npos);

    result = api.handle(command("/api/simulation/start"));
    assert(result.statusCode == 409);

    result = api.handle(command("/api/simulation/pause"));
    assert(result.statusCode == 200);
    assert(result.body.find("\"state\":\"paused\"") != std::string::npos);

    result = api.handle(command("/api/simulation/resume"));
    assert(result.statusCode == 200);

    result = api.handle(command("/api/simulation/reset"));
    assert(result.statusCode == 200);
    assert(result.body.find("\"state\":\"stopped\"") != std::string::npos);
    assert(result.body.find("\"tickMs\":1000") != std::string::npos);
    result = api.handle(command("/api/simulation/reset"));
    assert(result.statusCode == 200);

    ApiRequest config{"PUT", "/api/simulation/config", true, {
        {"tickMs", integer(750)}, {"greenTicks", integer(12)},
        {"yellowTicks", integer(3)}, {"arrivalInterval", integer(4)},
        {"maxQueue", integer(15)}}};
    result = api.handle(config);
    assert(result.statusCode == 200);
    assert(result.body.find("\"tickMs\":750") != std::string::npos);

    config.objectBody["tickMs"].type = ApiJsonType::String;
    result = api.handle(config);
    assert(result.statusCode == 400);

    config.objectBody["tickMs"] = integer(750);
    config.objectBody["greenTicks"] = integer(0);
    result = api.handle(config);
    assert(result.statusCode == 400);
    assert(result.body.find("VALIDATION_FAILED") != std::string::npos);
    assert(result.body.find("\"details\"") != std::string::npos);

    config.objectBody["greenTicks"] = integer(12);
    config.objectBody.erase("maxQueue");
    result = api.handle(config);
    assert(result.statusCode == 400);
    config.objectBody["maxQueue"] = integer(15);
    config.objectBody["unexpected"] = integer(1);
    result = api.handle(config);
    assert(result.statusCode == 400);
    config.objectBody.erase("unexpected");

    ApiRequest malformed{"PUT", "/api/simulation/config", false, {}};
    result = api.handle(malformed);
    assert(result.statusCode == 400);
    assert(result.body.find("MALFORMED_JSON") != std::string::npos);

    result = api.handle(command("/api/simulation/reset"));
    assert(result.statusCode == 200);
    assert(result.body.find("\"tickMs\":750") != std::string::npos);

    result = api.handle(command("/api/simulation/unknown"));
    assert(result.statusCode == 404);

    std::cout << "All simulation_control_api tests passed.\n";
    return 0;
}
