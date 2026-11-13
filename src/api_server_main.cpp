#include "simulation_http_server.h"

#include <asio/ip/address.hpp>

#include <cstdlib>
#include <iostream>
#include <string>

int main() {
    SimulationService service;

    std::string bindAddress = "127.0.0.1";
    if (const char* configuredAddress =
            std::getenv("TMS_HTTP_BIND_ADDRESS")) {
        bindAddress = configuredAddress;
    }

    SimulationHttpServer server(service, bindAddress);

    std::string error;
    if (!server.start(error)) {
        std::cerr << "Could not start REST server: " << error << '\n';
        return EXIT_FAILURE;
    }

    const asio::ip::address address = asio::ip::make_address(bindAddress);
    const std::string displayAddress = address.is_v6()
        ? "[" + bindAddress + "]"
        : bindAddress;

    if (!address.is_loopback()) {
        std::cerr
            << "WARNING: REST API is listening on a non-loopback interface. "
            << "The server does not provide authentication or TLS; restrict "
            << "network access with firewall and reverse-proxy controls.\n";
    }

    std::cout << "Traffic Management System REST API listening on "
              << "http://" << displayAddress << ":" << server.port()
              << " (press Ctrl+C to stop).\n";

    if (!server.waitForShutdown(error)) {
        std::cerr << "REST server stopped unexpectedly: " << error << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
