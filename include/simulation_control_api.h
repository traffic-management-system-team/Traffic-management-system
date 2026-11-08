#ifndef SIMULATION_CONTROL_API_H
#define SIMULATION_CONTROL_API_H

#include "simulation_service.h"

#include <cstdint>
#include <map>
#include <string>

// Framework-neutral values populated by the HTTP adapter after JSON parsing.
// Keeping JSON parsing and the Crow server in Issue #9 avoids coupling tms_core
// to a particular HTTP/JSON framework.
enum class ApiJsonType { Integer, Number, String, Boolean, Null, Array, Object };
struct ApiJsonField {
    ApiJsonType type = ApiJsonType::Null;
    std::int64_t integerValue = 0;
};

struct ApiRequest {
    std::string method;
    std::string path;
    bool jsonValid = true; // Set false by the host if a supplied body is malformed.
    std::map<std::string, ApiJsonField> objectBody;
};

struct ApiResponse {
    int statusCode = 500;
    std::string contentType = "application/json";
    std::string body;
};

// Implements the five Issue #28 endpoint behaviours without owning an HTTP
// listener, route registration, scheduler, or SimModel.
class SimulationControlApi {
public:
    explicit SimulationControlApi(SimulationService& service) : service_(service) {}
    ApiResponse handle(const ApiRequest& request) const noexcept;

private:
    SimulationService& service_;
};

#endif
