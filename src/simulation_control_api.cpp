
#include "simulation_control_api.h"

#include "input_validator.h"

#include <exception>
#include <limits>
#include <sstream>

namespace {
std::string escapeJson(const std::string& value) {
    std::ostringstream out;
    for (unsigned char ch : value) {
        switch (ch) {
        case '"': out << "\\\""; break;
        case '\\': out << "\\\\"; break;
        case '\b': out << "\\b"; break;
        case '\f': out << "\\f"; break;
        case '\n': out << "\\n"; break;
        case '\r': out << "\\r"; break;
        case '\t': out << "\\t"; break;
        default:
            if (ch < 0x20) {
                const char* hex = "0123456789abcdef";
                out << "\\u00" << hex[ch >> 4] << hex[ch & 0x0f];
            } else {
                out << static_cast<char>(ch);
            }
        }
    }
    return out.str();
}

std::string configJson(const SimConfig& c) {
    std::ostringstream out;
    out << "{\"tickMs\":" << c.tickMs
        << ",\"greenTicks\":" << c.greenTicks
        << ",\"yellowTicks\":" << c.yellowTicks
        << ",\"arrivalInterval\":" << c.arrivalInterval
        << ",\"maxQueue\":" << c.maxQueue << '}';
    return out.str();
}

ApiResponse response(int status, const std::string& body) {
    return ApiResponse{status, "application/json", body};
}

ApiResponse success(const std::string& data) {
    return response(200, "{\"success\":true,\"data\":" + data + ",\"error\":null}");
}

ApiResponse error(int status, const std::string& code,
                  const std::string& message,
                  const std::string& details = {}) {
    std::string payload =
        "{\"success\":false,\"data\":null,\"error\":{\"code\":\"" +
        escapeJson(code) + "\",\"message\":\"" + escapeJson(message) + "\"";

    if (!details.empty()) {
        payload += ",\"details\":" + details;
    }

    payload += "}}";
    return response(status, payload);
}

bool hasOnlyConfigFields(const std::map<std::string, ApiJsonField>& body) {
    static const char* fields[] = {
        "tickMs", "greenTicks", "yellowTicks", "arrivalInterval", "maxQueue"
    };

    if (body.size() != 5) {
        return false;
    }

    for (const char* field : fields) {
        if (body.find(field) == body.end()) {
            return false;
        }
    }

    return true;
}

std::string validationDetails(const ValidationResult& validation) {
    std::ostringstream out;
    out << '[';

    for (std::size_t i = 0; i < validation.issues.size(); ++i) {
        if (i) {
            out << ',';
        }

        const auto& issue = validation.issues[i];

        out << "{\"field\":\"" << escapeJson(issue.field)
            << "\",\"code\":\"" << escapeJson(issue.code)
            << "\",\"message\":\"" << escapeJson(issue.message) << "\"}";
    }

    out << ']';
    return out.str();
}

bool getInteger(const std::map<std::string, ApiJsonField>& body,
                const char* name, std::int64_t& value) {
    const auto it = body.find(name);

    if (it == body.end() || it->second.type != ApiJsonType::Integer) {
        return false;
    }

    value = it->second.integerValue;
    return true;
}
}

ApiResponse SimulationControlApi::handle(const ApiRequest& request) const noexcept {
    try {
        if (!request.jsonValid) {
            return error(
                400,
                "MALFORMED_JSON",
                "Request body contains malformed JSON."
            );
        }

        if (request.method == "POST" &&
            request.path == "/api/simulation/start") {
            if (!service_.start()) {
                return error(
                    409,
                    "INVALID_STATE",
                    "Simulation can only start when stopped."
                );
            }

            return success("{\"state\":\"running\"}");
        }

        if (request.method == "POST" &&
            request.path == "/api/simulation/pause") {
            if (!service_.pause()) {
                return error(
                    409,
                    "INVALID_STATE",
                    "Simulation can only pause when running."
                );
            }

            return success("{\"state\":\"paused\"}");
        }

        if (request.method == "POST" &&
            request.path == "/api/simulation/resume") {
            if (!service_.resume()) {
                return error(
                    409,
                    "INVALID_STATE",
                    "Simulation can only resume when paused."
                );
            }

            return success("{\"state\":\"running\"}");
        }

        if (request.method == "POST" &&
            request.path == "/api/simulation/reset") {
            service_.reset();

            const auto snapshot = service_.getSnapshot();

            return success(
                "{\"state\":\"stopped\",\"config\":" +
                configJson(snapshot.config) + "}"
            );
        }

        if (request.method == "PUT" &&
            request.path == "/api/simulation/config") {

            if (!hasOnlyConfigFields(request.objectBody)) {
                return error(
                    400,
                    "INVALID_REQUEST",
                    "All five configuration fields are required; unknown fields are not allowed."
                );
            }

            std::int64_t tickMs = 0;
            std::int64_t greenTicks = 0;
            std::int64_t yellowTicks = 0;
            std::int64_t arrivalInterval = 0;
            std::int64_t maxQueue = 0;

            if (!getInteger(request.objectBody, "tickMs", tickMs) ||
                !getInteger(request.objectBody, "greenTicks", greenTicks) ||
                !getInteger(request.objectBody, "yellowTicks", yellowTicks) ||
                !getInteger(request.objectBody, "arrivalInterval", arrivalInterval) ||
                !getInteger(request.objectBody, "maxQueue", maxQueue)) {

                return error(
                    400,
                    "INVALID_REQUEST",
                    "All configuration fields must be JSON integers (not strings, booleans, or floating-point values)."
                );
            }

            const auto intMin =
                static_cast<std::int64_t>(std::numeric_limits<int>::min());

            const auto intMax =
                static_cast<std::int64_t>(std::numeric_limits<int>::max());

            if (tickMs < intMin || tickMs > intMax ||
                greenTicks < intMin || greenTicks > intMax ||
                yellowTicks < intMin || yellowTicks > intMax ||
                arrivalInterval < intMin || arrivalInterval > intMax ||
                maxQueue < intMin || maxQueue > intMax) {

                return error(
                    400,
                    "VALIDATION_FAILED",
                    "Configuration integer is outside the supported representation."
                );
            }

            RawConfig raw{
                std::to_string(tickMs),
                std::to_string(greenTicks),
                std::to_string(yellowTicks),
                std::to_string(arrivalInterval),
                std::to_string(maxQueue)
            };

            std::string feedback;

            // Validate values before checking whether the simulation can accept them.
            const auto validation = InputValidator::validate(raw);

            if (!validation.ok) {
                const std::string details = validationDetails(validation);

                return error(
                    400,
                    "VALIDATION_FAILED",
                    validation.summary(),
                    details
                );
            }

            // Valid configuration rejected by the service means a state conflict.
            if (!service_.setConfig(raw, feedback)) {
                return error(
                    409,
                    "INVALID_STATE",
                    "Configuration cannot be changed in the current simulation state."
                );
            }

            return success(
                "{\"config\":" +
                configJson(SimConfig{
                    static_cast<int>(tickMs),
                    static_cast<int>(greenTicks),
                    static_cast<int>(yellowTicks),
                    static_cast<int>(arrivalInterval),
                    static_cast<int>(maxQueue)
                }) + "}"
            );
        }

        return error(
            404,
            "NOT_FOUND",
            "The requested API endpoint was not found."
        );

    } catch (...) {
        return error(
            500,
            "INTERNAL_ERROR",
            "An unexpected server error occurred."
        );
    }
}
