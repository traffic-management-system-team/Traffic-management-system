#include "config_controller.h"

bool ConfigController::submit(const RawConfig& raw, std::string& feedback) {
    const ValidationResult r = InputValidator::validate(raw);
    if (!r.ok) {
        feedback = r.summary();
        ++rejected_;
        return false;               // nothing reaches the simulation
    }
    if (!isInRange(r.config)) {     // defence in depth: never store out-of-range values
        feedback = "Configuration rejected: values are outside the allowed limits.";
        ++rejected_;
        return false;
    }
    active_ = r.config;
    feedback = "Configuration accepted.";
    ++accepted_;
    return true;
}
