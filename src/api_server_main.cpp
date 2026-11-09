#include "simulation_http_server.h"

#include <cstdlib>
#include <iostream>
#include <string>

int main() {
    SimulationService service;
    SimulationHttpServer server(service);

    std::string error;
    if (!server.start(error)) {
        std::cerr << "Could not start REST server: " << error << '\n';
        return EXIT_FAILURE;
    }

    std::cout << "Traffic Management System REST API listening on "
              << "http://127.0.0.1:" << server.port()
              << " (press Ctrl+C to stop).\n";

    if (!server.waitForShutdown(error)) {
        std::cerr << "REST server stopped unexpectedly: " << error << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
