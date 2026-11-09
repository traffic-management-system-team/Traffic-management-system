#ifndef SIMULATION_HTTP_LIMITS_H
#define SIMULATION_HTTP_LIMITS_H

#include <cstddef>

#ifndef TMS_HTTP_MAX_REQUEST_BODY_BYTES
#define TMS_HTTP_MAX_REQUEST_BODY_BYTES 65536
#endif

namespace SimulationHttpLimits {
inline constexpr std::size_t MaxRequestBodyBytes =
    TMS_HTTP_MAX_REQUEST_BODY_BYTES;
}

#endif
