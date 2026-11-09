#include "simulation_http_response.h"

#include <asio/ip/address_v6.hpp>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <string>

namespace {

bool isValidIpv4Address(const std::string& host) {
    std::size_t componentStart = 0;
    unsigned int componentCount = 0;

    while (componentStart <= host.size()) {
        const std::size_t separator = host.find('.', componentStart);
        const std::size_t componentEnd =
            separator == std::string::npos ? host.size() : separator;
        const std::size_t componentLength = componentEnd - componentStart;
        if (componentLength == 0 || componentLength > 3 ||
            (componentLength > 1 && host[componentStart] == '0')) {
            return false;
        }

        unsigned int value = 0;
        for (std::size_t i = componentStart; i < componentEnd; ++i) {
            const unsigned char character =
                static_cast<unsigned char>(host[i]);
            if (character < '0' || character > '9') {
                return false;
            }
            value = value * 10U + static_cast<unsigned int>(character - '0');
        }
        if (value > 255U) {
            return false;
        }

        ++componentCount;
        if (separator == std::string::npos) {
            break;
        }
        componentStart = separator + 1;
    }

    return componentCount == 4;
}

bool isNumericDottedHost(const std::string& host) {
    bool hasDot = false;
    for (const unsigned char character : host) {
        if (character == '.') {
            hasDot = true;
        } else if (character < '0' || character > '9') {
            return false;
        }
    }
    return hasDot;
}

bool parseOrigin(const std::string& value, std::string& canonical) {
    if (value.empty() ||
        value.find_first_of(" \t\r\n,\\") != std::string::npos) {
        return false;
    }

    const std::size_t schemeEnd = value.find("://");
    if (schemeEnd == std::string::npos) {
        return false;
    }

    std::string scheme = value.substr(0, schemeEnd);
    std::transform(
        scheme.begin(), scheme.end(), scheme.begin(),
        [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); }
    );
    if (scheme != "http" && scheme != "https") {
        return false;
    }

    const std::string authority = value.substr(schemeEnd + 3);
    if (authority.empty() ||
        authority.find_first_of(" \t\r\n/?#@,\\") != std::string::npos) {
        return false;
    }

    std::string host;
    std::string portText;
    if (authority.front() == '[') {
        const std::size_t closingBracket = authority.find(']');
        if (closingBracket == std::string::npos || closingBracket == 1) {
            return false;
        }
        const std::string addressText =
            authority.substr(1, closingBracket - 1);
        asio::error_code addressError;
        const asio::ip::address_v6 address =
            asio::ip::make_address_v6(addressText, addressError);
        if (addressError) {
            return false;
        }
        host = "[" + address.to_string() + "]";
        const std::string suffix = authority.substr(closingBracket + 1);
        if (!suffix.empty()) {
            if (suffix.front() != ':' || suffix.size() == 1) {
                return false;
            }
            portText = suffix.substr(1);
        }
    } else {
        const std::size_t colon = authority.find(':');
        if (colon != std::string::npos) {
            if (authority.find(':', colon + 1) != std::string::npos ||
                colon == 0 || colon + 1 == authority.size()) {
                return false;
            }
            host = authority.substr(0, colon);
            portText = authority.substr(colon + 1);
        } else {
            host = authority;
        }

        if (host.empty() || host.front() == '.' || host.back() == '.') {
            return false;
        }
        if (isNumericDottedHost(host)) {
            if (!isValidIpv4Address(host)) {
                return false;
            }
        }

        std::size_t labelLength = 0;
        bool previousWasHyphen = false;
        for (const unsigned char ch : host) {
            if (ch == '.') {
                if (labelLength == 0 || labelLength > 63 || previousWasHyphen) {
                    return false;
                }
                labelLength = 0;
                previousWasHyphen = false;
                continue;
            }
            const bool asciiAlphaNumeric =
                (ch >= 'a' && ch <= 'z') ||
                (ch >= 'A' && ch <= 'Z') ||
                (ch >= '0' && ch <= '9');
            if (!asciiAlphaNumeric && ch != '-') {
                return false;
            }
            if (labelLength == 0 && ch == '-') {
                return false;
            }
            ++labelLength;
            if (labelLength > 63) {
                return false;
            }
            previousWasHyphen = ch == '-';
        }
        if (labelLength == 0 || labelLength > 63 || previousWasHyphen) {
            return false;
        }
        std::transform(
            host.begin(), host.end(), host.begin(),
            [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); }
        );
    }

    const unsigned int defaultPort = scheme == "https" ? 443U : 80U;
    unsigned int port = defaultPort;
    if (!portText.empty()) {
        port = 0;
        for (const unsigned char ch : portText) {
            if (!std::isdigit(ch)) {
                return false;
            }
            port = port * 10U + static_cast<unsigned int>(ch - '0');
            if (port > 65535U) {
                return false;
            }
        }
        if (port == 0U) {
            return false;
        }
    }

    canonical = scheme + "://" + host;
    if (port != defaultPort) {
        canonical += ":" + std::to_string(port);
    }
    return true;
}

std::string lowercase(std::string value) {
    std::transform(
        value.begin(), value.end(), value.begin(),
        [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); }
    );
    return value;
}

} // namespace

bool SimulationHttpOriginPolicy::configureFromEnvironment(std::string& error) {
    error.clear();
    expectedOrigin_.clear();
    hasConfiguredOrigin_ = false;

#ifdef _WIN32
    char* configuredOrigin = nullptr;
    std::size_t configuredOriginLength = 0;
    if (_dupenv_s(
            &configuredOrigin, &configuredOriginLength, "TMS_PUBLIC_ORIGIN"
        ) != 0) {
        error = "Unable to read TMS_PUBLIC_ORIGIN.";
        return false;
    }
    if (configuredOrigin == nullptr) {
        return true;
    }
    const std::string configuredOriginValue(configuredOrigin);
    std::free(configuredOrigin);
#else
    const char* configuredOrigin = std::getenv("TMS_PUBLIC_ORIGIN");
    if (configuredOrigin == nullptr) {
        return true;
    }
    const std::string configuredOriginValue(configuredOrigin);
#endif

    if (!parseOrigin(configuredOriginValue, expectedOrigin_)) {
        error = "TMS_PUBLIC_ORIGIN must be an HTTP(S) origin with no path.";
        return false;
    }

    hasConfiguredOrigin_ = true;
    return true;
}

bool SimulationHttpOriginPolicy::isCrossOriginBrowserRequest(
    const crow::request& request
) const {
    const std::string& origin = request.get_header_value("Origin");
    if (!origin.empty()) {
        std::string normalizedOrigin;
        std::string expectedOrigin = expectedOrigin_;
        if ((!hasConfiguredOrigin_ &&
             !parseOrigin(
                 "http://" + request.get_header_value("Host"),
                 expectedOrigin
             )) ||
            !parseOrigin(origin, normalizedOrigin) ||
            normalizedOrigin != expectedOrigin) {
            return true;
        }
    }

    const std::string fetchSite =
        lowercase(request.get_header_value("Sec-Fetch-Site"));
    return !fetchSite.empty() &&
           fetchSite != "same-origin" &&
           fetchSite != "none";
}

crow::response makeJsonSuccessResponse(const nlohmann::json& data) {
    crow::response response(200);
    response.set_header("Content-Type", "application/json");
    response.body =
        "{\"success\":true,\"data\":" + data.dump() +
        ",\"error\":null}";
    return response;
}

crow::response makeJsonErrorResponse(
    int status,
    const std::string& code,
    const std::string& message
) {
    crow::response response(status);
    response.set_header("Content-Type", "application/json");
    response.body =
        "{\"success\":false,\"data\":null,\"error\":{"
        "\"code\":" + nlohmann::json(code).dump() +
        ",\"message\":" + nlohmann::json(message).dump() + "}}";
    return response;
}

crow::response makeJsonMethodNotAllowedResponse(const std::string& allow) {
    crow::response response = makeJsonErrorResponse(
        405,
        "METHOD_NOT_ALLOWED",
        "The HTTP method is not allowed for this API endpoint."
    );
    response.set_header("Allow", allow);
    return response;
}
