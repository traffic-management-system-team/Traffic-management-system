#include "simulation_http_adapter.h"

#include "simulation_http_response.h"

#include <algorithm>
#include <cstdint>
#include <exception>
#include <limits>
#include <string>

namespace {

#define TMS_ALL_HTTP_METHODS \
    crow::HTTPMethod::Delete, crow::HTTPMethod::Get, \
    crow::HTTPMethod::Head, crow::HTTPMethod::Post, \
    crow::HTTPMethod::Put, crow::HTTPMethod::Connect, \
    crow::HTTPMethod::Options, crow::HTTPMethod::Trace, \
    crow::HTTPMethod::Patch, crow::HTTPMethod::Purge, \
    crow::HTTPMethod::Copy, crow::HTTPMethod::Lock, \
    crow::HTTPMethod::MkCol, crow::HTTPMethod::Move, \
    crow::HTTPMethod::Propfind, crow::HTTPMethod::Proppatch, \
    crow::HTTPMethod::Search, crow::HTTPMethod::Unlock, \
    crow::HTTPMethod::Bind, crow::HTTPMethod::Rebind, \
    crow::HTTPMethod::Unbind, crow::HTTPMethod::Acl, \
    crow::HTTPMethod::Report, crow::HTTPMethod::MkActivity, \
    crow::HTTPMethod::Checkout, crow::HTTPMethod::Merge, \
    crow::HTTPMethod::MSearch, crow::HTTPMethod::Notify, \
    crow::HTTPMethod::Subscribe, crow::HTTPMethod::Unsubscribe, \
    crow::HTTPMethod::MkCalendar, crow::HTTPMethod::Link, \
    crow::HTTPMethod::Unlink, crow::HTTPMethod::Source

bool isJsonContentType(const std::string& contentType) {
    const std::size_t parameterStart = contentType.find(';');
    std::string mediaType = contentType.substr(0, parameterStart);
    const std::size_t first = mediaType.find_first_not_of(" \t");
    if (first == std::string::npos) {
        return false;
    }

    const std::size_t last = mediaType.find_last_not_of(" \t");
    mediaType = mediaType.substr(first, last - first + 1);
    std::transform(
        mediaType.begin(),
        mediaType.end(),
        mediaType.begin(),
        [](unsigned char character) {
            if (character >= 'A' && character <= 'Z') {
                return static_cast<char>(character - 'A' + 'a');
            }
            return static_cast<char>(character);
        }
    );
    return mediaType == "application/json";
}

ApiJsonField mapJsonValue(const crow::json::rvalue& value) {
    switch (value.t()) {
        case crow::json::type::Null:
            return {ApiJsonType::Null, 0};
        case crow::json::type::False:
        case crow::json::type::True:
            return {ApiJsonType::Boolean, 0};
        case crow::json::type::String:
            return {ApiJsonType::String, 0};
        case crow::json::type::List:
            return {ApiJsonType::Array, 0};
        case crow::json::type::Object:
            return {ApiJsonType::Object, 0};
        case crow::json::type::Number:
            if (value.nt() == crow::json::num_type::Floating_point ||
                value.nt() ==
                    crow::json::num_type::Double_precision_floating_point) {
                return {ApiJsonType::Number, 0};
            }

            if (value.nt() == crow::json::num_type::Unsigned_integer) {
                std::uint64_t integer = 0;
                try {
                    integer = value.u();
                } catch (const std::exception&) {
                    return {
                        ApiJsonType::Integer,
                        (std::numeric_limits<std::int64_t>::max)()
                    };
                }
                if (integer >
                    static_cast<std::uint64_t>(
                        (std::numeric_limits<std::int64_t>::max)())) {
                    return {
                        ApiJsonType::Integer,
                        (std::numeric_limits<std::int64_t>::max)()
                    };
                }
                return {
                    ApiJsonType::Integer,
                    static_cast<std::int64_t>(integer)
                };
            }

            try {
                return {ApiJsonType::Integer, value.i()};
            } catch (const std::exception&) {
                return {
                    ApiJsonType::Integer,
                    (std::numeric_limits<std::int64_t>::max)()
                };
            }
        case crow::json::type::Function:
            return {ApiJsonType::Object, 0};
    }

    return {ApiJsonType::Null, 0};
}

ApiRequest makeApiRequest(
    const crow::request& request,
    const char* method,
    const char* path
) {
    ApiRequest apiRequest;
    apiRequest.method = method;
    apiRequest.path = path;

    if (request.body.empty()) {
        return apiRequest;
    }

    const crow::json::rvalue body = crow::json::load(request.body);
    if (!body) {
        apiRequest.jsonValid = false;
        return apiRequest;
    }

    if (body.t() != crow::json::type::Object) {
        return apiRequest;
    }

    for (auto field = body.begin(); field != body.end(); ++field) {
        apiRequest.objectBody[static_cast<std::string>(field->key())] =
            mapJsonValue(*field);
    }

    return apiRequest;
}

} // namespace

void SimulationHttpAdapter::registerRoutes(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/api/simulation/start")
        .methods(TMS_ALL_HTTP_METHODS)
    ([this](const crow::request& request) {
        return handle(request, "POST", "/api/simulation/start");
    });

    CROW_ROUTE(app, "/api/simulation/pause")
        .methods(TMS_ALL_HTTP_METHODS)
    ([this](const crow::request& request) {
        return handle(request, "POST", "/api/simulation/pause");
    });

    CROW_ROUTE(app, "/api/simulation/resume")
        .methods(TMS_ALL_HTTP_METHODS)
    ([this](const crow::request& request) {
        return handle(request, "POST", "/api/simulation/resume");
    });

    CROW_ROUTE(app, "/api/simulation/reset")
        .methods(TMS_ALL_HTTP_METHODS)
    ([this](const crow::request& request) {
        return handle(request, "POST", "/api/simulation/reset");
    });

    CROW_ROUTE(app, "/api/simulation/config")
        .methods(TMS_ALL_HTTP_METHODS)
    ([this](const crow::request& request) {
        return handle(request, "PUT", "/api/simulation/config");
    });
}

#undef TMS_ALL_HTTP_METHODS

crow::response SimulationHttpAdapter::handle(
    const crow::request& request,
    const char* method,
    const char* path
) const {
    if (originPolicy_.isCrossOriginBrowserRequest(request)) {
        return makeJsonErrorResponse(
            403,
            "CROSS_ORIGIN_REQUEST",
            "Cross-origin browser requests are not allowed."
        );
    }

    if (std::string(crow::method_name(request.method)) != method) {
        return makeJsonMethodNotAllowedResponse(method);
    }

    if (request.body_too_large) {
        return makeJsonErrorResponse(
            413,
            "PAYLOAD_TOO_LARGE",
            "Request body exceeds the maximum allowed size."
        );
    }

    const std::string& contentType =
        request.get_header_value("Content-Type");
    if ((!request.body.empty() || !contentType.empty()) &&
        !isJsonContentType(contentType)) {
        return makeJsonErrorResponse(
            415,
            "UNSUPPORTED_MEDIA_TYPE",
            "Request body must use the application/json media type."
        );
    }

    const ApiResponse apiResponse =
        api_.handle(makeApiRequest(request, method, path));

    crow::response response(apiResponse.statusCode);
    response.set_header("Content-Type", apiResponse.contentType);
    response.body = apiResponse.body;
    return response;
}
