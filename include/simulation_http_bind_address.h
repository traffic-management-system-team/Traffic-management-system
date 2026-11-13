#ifndef SIMULATION_HTTP_BIND_ADDRESS_H
#define SIMULATION_HTTP_BIND_ADDRESS_H

#include <asio/ip/address.hpp>

#include <string>

namespace SimulationHttpBindAddress {

inline constexpr const char* DefaultAddress = "127.0.0.1";

inline bool parse(const std::string& value, asio::ip::address& address) {
    asio::error_code error;
    address = asio::ip::make_address(value, error);
    return !error;
}

inline std::string resolve(const char* configuredAddress) {
    return configuredAddress == nullptr
        ? std::string(DefaultAddress)
        : std::string(configuredAddress);
}

inline std::string nonLoopbackWarning(const asio::ip::address& address) {
    if (address.is_loopback()) {
        return {};
    }

    return
        "WARNING: REST API is listening on a non-loopback interface. "
        "The server does not provide authentication or TLS; restrict "
        "network access with firewall and reverse-proxy controls.\n";
}

inline std::string displayHost(const asio::ip::address& address) {
    const std::string value = address.to_string();
    return address.is_v6() ? "[" + value + "]" : value;
}

} // namespace SimulationHttpBindAddress

#endif
