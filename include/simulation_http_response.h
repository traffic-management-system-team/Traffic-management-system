#ifndef SIMULATION_HTTP_RESPONSE_H
#define SIMULATION_HTTP_RESPONSE_H

#include <crow.h>
#include <nlohmann/json.hpp>

#include <string>

class SimulationHttpOriginPolicy {
public:
    bool configureFromEnvironment(std::string& error);
    bool isCrossOriginBrowserRequest(const crow::request& request) const;

private:
    std::string expectedOrigin_;
    bool hasConfiguredOrigin_ = false;
};

crow::response makeJsonSuccessResponse(const nlohmann::json& data);
crow::response makeJsonErrorResponse(
    int status,
    const std::string& code,
    const std::string& message
);
crow::response makeJsonMethodNotAllowedResponse(const std::string& allow);

template <typename Handler>
crow::response executeHttpHandlerSafely(Handler&& handler) {
    try {
        return handler();
    } catch (...) {
        return makeJsonErrorResponse(
            500,
            "INTERNAL_SERVER_ERROR",
            "An unexpected internal error occurred."
        );
    }
}

#endif
