#ifndef SIMULATION_STATE_SERIALIZER_H
#define SIMULATION_STATE_SERIALIZER_H

#include "simulation_dto.h"

#include <nlohmann/json.hpp>

nlohmann::json serializeSimulationStateData(
    const SimulationSnapshot& snapshot
);

#endif
