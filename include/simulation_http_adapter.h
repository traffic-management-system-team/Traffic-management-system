#ifndef SIMULATION_HTTP_ADAPTER_H
#define SIMULATION_HTTP_ADAPTER_H

#include "simulation_control_api.h"
#include "simulation_http_response.h"

#include <crow.h>

class SimulationHttpAdapter {
public:
    SimulationHttpAdapter(
        SimulationControlApi& api,
        const SimulationHttpOriginPolicy& originPolicy
    )
        : api_(api), originPolicy_(originPolicy) {}

    void registerRoutes(crow::SimpleApp& app);

private:
    crow::response handle(
        const crow::request& request,
        const char* method,
        const char* path
    ) const;

    SimulationControlApi& api_;
    const SimulationHttpOriginPolicy& originPolicy_;
};

#endif
