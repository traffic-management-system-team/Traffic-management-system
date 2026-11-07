#ifndef SIMULATION_SERVICE_H
#define SIMULATION_SERVICE_H

#include "input_validator.h"
#include "simulation_dto.h"

#include <memory>
#include <string>

class SimulationService {
public:
    SimulationService();
    ~SimulationService();

    SimulationService(const SimulationService&) = delete;
    SimulationService& operator=(const SimulationService&) = delete;
    SimulationService(SimulationService&&) = delete;
    SimulationService& operator=(SimulationService&&) = delete;

    // Starts the simulation and its service-owned scheduler.
    bool start();

    // Pauses after any currently executing tick has completed.
    bool pause();

    // Resumes a paused simulation.
    bool resume();

    // Stops the scheduler's logical activity and resets the model.
    void reset();

    // Validates raw configuration and applies it only if SimModel accepts it.
    bool setConfig(const RawConfig& raw, std::string& feedback);

    // Returns one consistent, independent copy of the complete simulation.
    SimulationSnapshot getSnapshot() const;

    // Convenience reads; use the same serialized execution boundary.
    SimulationStatistics getStatistics() const;
    SimModel::State getState() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

#endif
