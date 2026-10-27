#include "config_controller.h"

bool ConfigController::submit(const RawConfig& raw, std::string& feedback) {
    const ValidationResult r = InputValidator::validate(raw);
    if (!r.ok) {
        feedback = r.summary();
        ++rejected_;
        return false;               // nothing reaches the simulation
    }
    active_ = r.config;
    feedback = "Configuration accepted.";
    ++accepted_;
    return true;
}
