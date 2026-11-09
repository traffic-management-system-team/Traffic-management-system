#include "simulation_service.h"

#include "config_controller.h"

#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <exception>
#include <mutex>
#include <thread>

namespace {

constexpr const char* kRoadIds[SimModel::kRoads] = {
    "north", "east", "south", "west"
};

} // namespace

class SimulationService::Impl {
public:
    Impl()
        : worker_(&Impl::run, this) {
    }

    ~Impl() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            shuttingDown_ = true;
        }

        condition_.notify_all();

        if (worker_.joinable()) {
            worker_.join();
        }
    }

    bool start() {
        std::lock_guard<std::mutex> lock(mutex_);

        if (model_.state() != SimModel::State::Stopped) {
            return false;
        }

        model_.start();
        condition_.notify_all();
        return true;
    }

    bool pause() {
        std::lock_guard<std::mutex> lock(mutex_);

        if (model_.state() != SimModel::State::Running) {
            return false;
        }

        // The scheduler uses the same mutex, so any active tick
        // has completed before this transition occurs.
        model_.togglePause();
        condition_.notify_all();
        return true;
    }

    bool resume() {
        std::lock_guard<std::mutex> lock(mutex_);

        if (model_.state() != SimModel::State::Paused) {
            return false;
        }

        model_.togglePause();
        condition_.notify_all();
        return true;
    }

    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);

        model_.reset();
        condition_.notify_all();
    }

    bool setConfig(const RawConfig& raw, std::string& feedback) {
        std::lock_guard<std::mutex> lock(mutex_);

        const bool accepted = configController_.submit(
            raw,
            [this](const SimConfig& candidate) {
                return model_.setConfig(candidate);
            },
            feedback
        );

        condition_.notify_all();
        return accepted;
    }

    SimulationSnapshot getSnapshot() const {
        std::lock_guard<std::mutex> lock(mutex_);

        SimulationSnapshot snapshot;
        snapshot.state = model_.state();
        snapshot.activeApproach = model_.activeApproach();
        snapshot.statistics = model_.statistics();
        snapshot.config = model_.config();

        for (int road = 0; road < SimModel::kRoads; ++road) {
            const auto index = static_cast<std::size_t>(road);

            snapshot.signals[index] = model_.signal(road);
            snapshot.queues[index].roadId = kRoadIds[index];

            const auto vehicles = model_.queuedVehicles(road);
            snapshot.queues[index].vehicles.reserve(vehicles.size());

            for (const auto& vehicle : vehicles) {
                snapshot.queues[index].vehicles.push_back({
                    vehicle.getId(),
                    vehicle.getType(),
                    vehicle.getLane()
                });
            }
        }

        return snapshot;
    }

    SimulationStatistics getStatistics() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return model_.statistics();
    }

    SimModel::State getState() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return model_.state();
    }

private:
    void run() {
        std::unique_lock<std::mutex> lock(mutex_);

        while (!shuttingDown_) {
            condition_.wait(lock, [this] {
                return shuttingDown_ ||
                       model_.state() == SimModel::State::Running;
            });

            if (shuttingDown_) {
                break;
            }

            const auto interval =
                std::chrono::milliseconds(model_.config().tickMs);
            const auto deadline =
                std::chrono::steady_clock::now() + interval;

            const bool interrupted = condition_.wait_until(
                lock,
                deadline,
                [this] {
                    return shuttingDown_ ||
                           model_.state() != SimModel::State::Running;
                }
            );

            if (shuttingDown_) {
                break;
            }

            if (interrupted) {
                continue;
            }

            try {
                if (model_.state() == SimModel::State::Running) {
                    model_.tick();
                }
            } catch (const std::exception& error) {
                std::fprintf(
                    stderr,
                    "SimulationService: scheduler stopped after tick failure: %s\n",
                    error.what()
                );

                if (model_.state() == SimModel::State::Running) {
                    model_.togglePause();
                }
            } catch (...) {
                std::fprintf(
                    stderr,
                    "SimulationService: scheduler stopped after an unknown tick failure.\n"
                );

                if (model_.state() == SimModel::State::Running) {
                    model_.togglePause();
                }
            }
        }
    }

    // Exactly one model instance, owned by the service.
    SimModel model_;
    ConfigController configController_;

    // All model and configuration access is serialized by this mutex.
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    bool shuttingDown_ = false;

    // One scheduler thread for the lifetime of the service.
    std::thread worker_;
};

SimulationService::SimulationService()
    : impl_(std::make_unique<Impl>()) {
}

SimulationService::~SimulationService() = default;

bool SimulationService::start() {
    return impl_->start();
}

bool SimulationService::pause() {
    return impl_->pause();
}

bool SimulationService::resume() {
    return impl_->resume();
}

void SimulationService::reset() {
    impl_->reset();
}

bool SimulationService::setConfig(
    const RawConfig& raw,
    std::string& feedback
) {
    return impl_->setConfig(raw, feedback);
}

SimulationSnapshot SimulationService::getSnapshot() const {
    return impl_->getSnapshot();
}

SimulationStatistics SimulationService::getStatistics() const {
    return impl_->getStatistics();
}

SimModel::State SimulationService::getState() const {
    return impl_->getState();
}