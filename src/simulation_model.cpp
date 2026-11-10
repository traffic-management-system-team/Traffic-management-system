#include "simulation_model.h"

#include <stdexcept>
#include <string>

namespace {
constexpr const char* kRoadIds[SimModel::kRoads] = {
    "north", "east", "south", "west"
};

constexpr std::size_t kArrivalOrder[SimModel::kRoads] = {
    0, 2, 1, 3
};

constexpr std::size_t kStatisticsRoadOrder[kSimulationApproachCount] = {
    0, 2, 1, 3
};

constexpr std::size_t kRoadStatisticsOrder[SimModel::kRoads] = {
    0, 2, 1, 3
};

std::size_t nextApproach(std::size_t approach) {
    switch (approach) {
        case 0: return 2;
        case 2: return 1;
        case 1: return 3;
        case 3: return 0;
        default: throw std::out_of_range("Invalid traffic approach");
    }
}
}

SimModel::SimModel(const SimConfig& config)
    : config_(config),
      signals_{{
          TrafficSignal(signalCoordinator_, 0),
          TrafficSignal(signalCoordinator_, 1),
          TrafficSignal(signalCoordinator_, 2),
          TrafficSignal(signalCoordinator_, 3)
      }},
      trafficManagers_{{
          TrafficManager(signals_[0], queueManager_, kRoadIds[0]),
          TrafficManager(signals_[1], queueManager_, kRoadIds[1]),
          TrafficManager(signals_[2], queueManager_, kRoadIds[2]),
          TrafficManager(signals_[3], queueManager_, kRoadIds[3])
      }} {
    if (!isInRange(config_)) {
        throw std::invalid_argument("Invalid simulation configuration");
    }

    if (!signals_[currentGreenApproach_].changeState(SignalState::GREEN)) {
        throw std::logic_error("Unable to initialize North GREEN");
    }
}

std::uint64_t SimModel::generatedCount() const noexcept {
    return statisticsManager_.generatedCount();
}

std::uint64_t SimModel::processedCount() const noexcept {
    return statisticsManager_.processedCount();
}

bool SimModel::setConfig(const SimConfig& config) {
    if (!isInRange(config) || state_ != State::Stopped || ticks_ != 0 ||
        generatedCount() != 0) {
        return false;
    }
    config_ = config;
    return true;
}

SimModel::Signal SimModel::signal(int road) const {
    if (road < 0 || road >= kRoads) {
        return Signal::Red;
    }

    switch (signals_[static_cast<std::size_t>(road)].getState()) {
        case SignalState::GREEN: return Signal::Green;
        case SignalState::YELLOW: return Signal::Yellow;
        case SignalState::RED: return Signal::Red;
    }

    throw std::logic_error("Traffic signal has an invalid state");
}

std::size_t SimModel::queueLength(int road) const {
    if (road < 0 || road >= kRoads) {
        return 0;
    }
    return queueManager_.getQueueLength(
        kRoadIds[static_cast<std::size_t>(road)]
    );
}

SimulationStatistics SimModel::statistics() const {
    std::array<std::size_t, kSimulationApproachCount> waitingByApproach{};

    for (std::size_t approach = 0;
         approach < kSimulationApproachCount;
         ++approach) {
        const std::size_t road = kStatisticsRoadOrder[approach];
        waitingByApproach[approach] =
            queueManager_.getQueueLength(kRoadIds[road]);
    }

    return statisticsManager_.snapshot(ticks_, waitingByApproach);
}

void SimModel::start() noexcept {
    if (state_ == State::Stopped) {
        state_ = State::Running;
    }
}

void SimModel::togglePause() noexcept {
    if (state_ == State::Running) {
        state_ = State::Paused;
    } else if (state_ == State::Paused) {
        state_ = State::Running;
    }
}

void SimModel::reset() {
    for (int road = 0; road < kRoads; ++road) {
        Vehicle removed(1, VehicleType::Car, kRoadIds[road]);
        while (queueManager_.dequeueVehicle(kRoadIds[road], removed)) {
            if (!vehicleManager_.removeVehicle(removed.getId())) {
                throw std::logic_error("Queued vehicle is missing from manager");
            }
        }
    }

    arrivalTicksByVehicle_.clear();
    state_ = State::Stopped;
    ticks_ = 0;
    phaseElapsedTicks_ = 0;
    statisticsManager_.reset();
    nextArrivalApproach_ = 0;
    currentGreenApproach_ = 0;

    signalCoordinator_.reset();
    if (!signals_[currentGreenApproach_].changeState(SignalState::GREEN)) {
        throw std::logic_error("Unable to restore North GREEN on reset");
    }
}

bool SimModel::tick() {
    if (state_ != State::Running) {
        return false;
    }

    ++ticks_;

    for (int road = 0; road < kRoads; ++road) {
        Vehicle processed(1, VehicleType::Car, kRoadIds[road]);
        if (!trafficManagers_[static_cast<std::size_t>(road)]
                 .processNextVehicle(kRoadIds[road], processed)) {
            continue;
        }

        const auto arrival = arrivalTicksByVehicle_.find(processed.getId());
        if (arrival == arrivalTicksByVehicle_.end()) {
            throw std::logic_error("Processed vehicle has no arrival record");
        }
        if (!vehicleManager_.removeVehicle(processed.getId())) {
            throw std::logic_error("Processed vehicle is missing from manager");
        }

        const auto statisticsApproach = static_cast<SimulationApproach>(
            kRoadStatisticsOrder[static_cast<std::size_t>(road)]
        );
        if (!statisticsManager_.recordProcessed(
                statisticsApproach,
                ticks_ - arrival->second
            )) {
            throw std::logic_error("Processed vehicle has an invalid approach");
        }
        arrivalTicksByVehicle_.erase(arrival);
    }

    if (ticks_ % static_cast<std::uint64_t>(config_.arrivalInterval) == 0) {
        const std::size_t road = kArrivalOrder[nextArrivalApproach_];
        if (queueLength(static_cast<int>(road)) <
            static_cast<std::size_t>(config_.maxQueue)) {
            const Vehicle vehicle = vehicleManager_.createVehicle(
                VehicleType::Car,
                kRoadIds[road]
            );
            if (!queueManager_.addVehicle(kRoadIds[road], vehicle)) {
                vehicleManager_.removeVehicle(vehicle.getId());
                throw std::runtime_error("Unable to enqueue arriving vehicle");
            }
            arrivalTicksByVehicle_.emplace(vehicle.getId(), ticks_);
            const auto statisticsApproach = static_cast<SimulationApproach>(
                kRoadStatisticsOrder[road]
            );
            if (!statisticsManager_.recordGenerated(statisticsApproach)) {
                throw std::logic_error("Generated vehicle has an invalid approach");
            }
        }
        nextArrivalApproach_ =
            (nextArrivalApproach_ + 1) % kRoads;
    }

    advanceSignalPhase();
    return true;
}

void SimModel::advanceSignalPhase() {
    ++phaseElapsedTicks_;
    const SignalState currentState =
        signals_[currentGreenApproach_].getState();

    if (currentState == SignalState::GREEN &&
        phaseElapsedTicks_ == static_cast<std::uint64_t>(config_.greenTicks)) {
        if (!signals_[currentGreenApproach_].changeState(SignalState::YELLOW)) {
            throw std::logic_error("Unable to enter YELLOW signal phase");
        }
        phaseElapsedTicks_ = 0;
        return;
    }

    if (currentState == SignalState::YELLOW &&
        phaseElapsedTicks_ == static_cast<std::uint64_t>(config_.yellowTicks)) {
        if (!signals_[currentGreenApproach_].changeState(SignalState::RED)) {
            throw std::logic_error("Unable to end YELLOW signal phase");
        }
        currentGreenApproach_ = nextApproach(currentGreenApproach_);
        if (!signals_[currentGreenApproach_].changeState(SignalState::GREEN)) {
            throw std::logic_error("Unable to grant next GREEN signal");
        }
        phaseElapsedTicks_ = 0;
    }
}

std::vector<Vehicle> SimModel::queuedVehicles(int road) const {
    if (road < 0 || road >= kRoads) {
        return {};
    }

    return queueManager_.snapshot(
        kRoadIds[static_cast<std::size_t>(road)]
    );
}
