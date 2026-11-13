#ifndef SIMULATION_HTTP_SERVER_H
#define SIMULATION_HTTP_SERVER_H

#include "simulation_service.h"
#include "simulation_http_bind_address.h"

#include <cstdint>
#include <memory>
#include <string>

class SimulationHttpServer {
public:
    explicit SimulationHttpServer(
        SimulationService& service,
        std::string bindAddress = SimulationHttpBindAddress::DefaultAddress,
        std::uint16_t port = 18080,
        unsigned int concurrency = 4
    );
    ~SimulationHttpServer();

    SimulationHttpServer(const SimulationHttpServer&) = delete;
    SimulationHttpServer& operator=(const SimulationHttpServer&) = delete;
    SimulationHttpServer(SimulationHttpServer&&) = delete;
    SimulationHttpServer& operator=(SimulationHttpServer&&) = delete;

    bool start(std::string& error);
    bool stop() noexcept;
    bool waitForShutdown(std::string& error);

    std::uint16_t port() const noexcept;
    bool isRunning() const noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

#endif
