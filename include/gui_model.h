
#ifndef GUI_MODEL_H
#define GUI_MODEL_H

#include "TrafficManager.h"
#include "TrafficSignal.h"
#include "queue_manager.h"
#include "vehicle_manager.h"
#include "sim_config.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <unordered_map>

class SimModel {
public:
    enum class State { Stopped, Running, Paused };
    enum class Signal { Red, Yellow, Green };

    static constexpr int kRoads = 4;

    explicit SimModel(const SimConfig& config = SimConfig{});

    State state() const noexcept { return state_; }
    Signal signal(int road) const;
    std::size_t queueLength(int road) const;

    std::uint64_t tickCount() const noexcept { return ticks_; }
    std::uint64_t generatedCount() const noexcept { return generatedCount_; }
    std::uint64_t processedCount() const noexcept { return processedCount_; }

    std::uint64_t totalWaitingTicks() const noexcept {
        return totalWaitingTicks_;
    }

    const SimConfig& config() const noexcept {
        return config_;
    }

    bool setConfig(const SimConfig& config);

    void start() noexcept;
    void togglePause() noexcept;
    void reset();
    bool tick();

private:
    void advanceSignalPhase();

    SimConfig config_;

    State state_ = State::Stopped;
    std::uint64_t ticks_ = 0;
    std::uint64_t phaseElapsedTicks_ = 0;
    std::uint64_t generatedCount_ = 0;
    std::uint64_t processedCount_ = 0;
    std::uint64_t totalWaitingTicks_ = 0;

    std::size_t nextArrivalApproach_ = 0;
    std::size_t currentGreenApproach_ = 0;

    SignalCoordinator signalCoordinator_;
    VehicleManager vehicleManager_;
    QueueManager queueManager_;

    std::array<TrafficSignal, kRoads> signals_;
    std::array<TrafficManager, kRoads> trafficManagers_;

    std::unordered_map<int, std::uint64_t> arrivalTicksByVehicle_;
};

#endif
