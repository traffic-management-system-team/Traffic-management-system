#include "simulation_http_server.h"
#include "simulation_http_bind_address.h"

#include <cstdlib>
#include <iostream>
#include <string>

int main() {
    SimulationService service;

    const std::string bindAddress = SimulationHttpBindAddress::resolve(
        std::getenv("TMS_HTTP_BIND_ADDRESS")
    );

    SimulationHttpServer server(service, bindAddress);

    std::string error;
    if (!server.start(error)) {
        std::cerr << "Could not start REST server: " << error << '\n';
        return EXIT_FAILURE;
    }

    asio::ip::address address;
    if (!SimulationHttpBindAddress::parse(bindAddress, address)) {
        std::cerr << "Could not start REST server: Invalid HTTP server bind address.\n";
        return EXIT_FAILURE;
    }

    const std::string warning =
        SimulationHttpBindAddress::nonLoopbackWarning(address);
    if (!warning.empty()) {
        std::cerr << warning;
    }

    const std::string displayAddress =
        SimulationHttpBindAddress::displayHost(address);

    std::cout << "Traffic Management System REST API listening on "
              << "http://" << displayAddress << ":" << server.port()
              << " (press Ctrl+C to stop).\n";

    if (!server.waitForShutdown(error)) {
        std::cerr << "REST server stopped unexpectedly: " << error << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
