#include "simulation_http_server.h"

#include "simulation_control_api.h"
#include "simulation_http_adapter.h"
#include "simulation_http_response.h"

#include <asio.hpp>

#include <chrono>
#include <csignal>
#include <cstdio>
#include <future>
#include <exception>
#include <iostream>
#include <mutex>
#include <thread>
#include <utility>

class SimulationHttpServer::Impl {
public:
    Impl(
        SimulationService& service,
        std::string bindAddress,
        std::uint16_t requestedPort,
        unsigned int concurrency
    )
        : service_(service),
          api_(service),
          adapter_(api_, service_, originPolicy_),
          bindAddress_(std::move(bindAddress)),
          requestedPort_(requestedPort),
          concurrency_(concurrency) {
        adapter_.registerRoutes(app_);
    }

    ~Impl() {
        if (!stop()) {
            std::fprintf(
                stderr,
                "SimulationHttpServer: HTTP worker did not shut down cleanly.\n"
            );
        }
    }

    bool start(std::string& error) {
        std::lock_guard<std::mutex> lock(lifecycleMutex_);
        error.clear();

        // Validate the numeric bind address before launching Crow's worker.
        // Crow can otherwise keep waiting for startup after an invalid address
        // fails inside its asynchronous server thread.
        try {
            (void)asio::ip::make_address(bindAddress_);
        } catch (const std::exception&) {
            error = "Invalid HTTP server bind address.";
            return false;
        }

        if (!originPolicy_.configureFromEnvironment(error)) {
            return false;
        }

        if (startedOnce_) {
            error = "The HTTP server can only be started once.";
            return false;
        }
        startedOnce_ = true;

        app_.bindaddr(bindAddress_)
            .port(requestedPort_)
            .concurrency(concurrency_)
            .loglevel(crow::LogLevel::Warning)
            .signal_clear()
            .signal_add(SIGINT)
            .signal_add(SIGTERM);
#ifdef SIGBREAK
        app_.signal_add(SIGBREAK);
#endif

        try {
            worker_ = app_.run_async();
        } catch (const std::exception& exception) {
            std::fprintf(
                stderr,
                "SimulationHttpServer: failed to launch HTTP worker: %s\n",
                exception.what()
            );
            error = "Failed to launch the HTTP server.";
            return false;
        } catch (...) {
            error = "Failed to launch the HTTP server.";
            return false;
        }

        const auto deadline =
            std::chrono::steady_clock::now() + startupTimeout_;
        while (std::chrono::steady_clock::now() < deadline) {
            if (worker_.wait_for(std::chrono::milliseconds(0)) ==
                std::future_status::ready) {
                finishWorkerLocked(
                    error,
                    "The HTTP server stopped during startup."
                );
                if (error.empty()) {
                    error = "The HTTP server stopped during startup.";
                }
                return false;
            }

            const auto remaining = std::chrono::duration_cast<
                std::chrono::milliseconds>(
                deadline - std::chrono::steady_clock::now()
            );
            const auto waitTime = remaining < startupPollInterval_
                ? remaining
                : startupPollInterval_;

            if (app_.wait_for_server_start(waitTime) ==
                std::cv_status::no_timeout) {
                boundPort_ = app_.port();
                running_ = true;
                return true;
            }
        }

        app_.stop();
        worker_.wait();
        finishWorkerLocked(error, "Timed out while starting the HTTP server.");
        if (error.empty()) {
            error = "Timed out while starting the HTTP server.";
        }
        return false;
    }

    bool stop() noexcept {
        std::lock_guard<std::mutex> lock(lifecycleMutex_);
        if (!worker_.valid()) {
            running_ = false;
            return true;
        }

        app_.stop();
        worker_.wait();
        std::string error;
        const bool clean = finishWorkerLocked(
            error,
            "The HTTP server worker failed during shutdown."
        );
        running_ = false;
        return clean;
    }

    bool waitForShutdown(std::string& error) {
        std::unique_lock<std::mutex> lock(lifecycleMutex_);
        error.clear();
        if (!worker_.valid()) {
            error = "The HTTP server is not running.";
            return false;
        }

        while (worker_.wait_for(std::chrono::milliseconds(50)) !=
               std::future_status::ready) {
            lock.unlock();
            std::this_thread::yield();
            lock.lock();
            if (!worker_.valid()) {
                error = "The HTTP server was stopped.";
                return false;
            }
        }

        return finishWorkerLocked(error, "The HTTP server worker failed.");
    }

    std::uint16_t port() const noexcept {
        std::lock_guard<std::mutex> lock(lifecycleMutex_);
        return boundPort_;
    }

    bool isRunning() const noexcept {
        std::lock_guard<std::mutex> lock(lifecycleMutex_);
        return running_;
    }

private:
    bool finishWorkerLocked(std::string& error, const char* failureMessage) {
        if (!worker_.valid()) {
            running_ = false;
            return true;
        }

        try {
            worker_.get();
            running_ = false;
            return true;
        } catch (const std::exception& exception) {
            std::fprintf(
                stderr,
                "SimulationHttpServer: %s: %s\n",
                failureMessage,
                exception.what()
            );
        } catch (...) {
            std::fprintf(
                stderr,
                "SimulationHttpServer: %s\n",
                failureMessage
            );
        }

        running_ = false;
        error = failureMessage;
        return false;
    }

    SimulationService& service_;
    SimulationControlApi api_;
    SimulationHttpOriginPolicy originPolicy_;
    SimulationHttpAdapter adapter_;
    crow::SimpleApp app_;
    std::string bindAddress_;
    std::uint16_t requestedPort_;
    unsigned int concurrency_;
    std::uint16_t boundPort_ = 0;
    std::future<void> worker_;
    mutable std::mutex lifecycleMutex_;
    bool startedOnce_ = false;
    bool running_ = false;
    const std::chrono::milliseconds startupTimeout_{3000};
    const std::chrono::milliseconds startupPollInterval_{25};
};


SimulationHttpServer::SimulationHttpServer(
    SimulationService& service,
    std::string bindAddress,
    std::uint16_t port,
    unsigned int concurrency
)
    : impl_(std::make_unique<Impl>(
          service,
          std::move(bindAddress),
          port,
          concurrency
      )) {
}

SimulationHttpServer::~SimulationHttpServer() = default;

bool SimulationHttpServer::start(std::string& error) {
    return impl_->start(error);
}

bool SimulationHttpServer::stop() noexcept {
    return impl_->stop();
}

bool SimulationHttpServer::waitForShutdown(std::string& error) {
    return impl_->waitForShutdown(error);
}

std::uint16_t SimulationHttpServer::port() const noexcept {
    return impl_->port();
}

bool SimulationHttpServer::isRunning() const noexcept {
    return impl_->isRunning();
}
