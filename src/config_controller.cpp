#include "config_controller.h"

bool ConfigController::submit(
    const RawConfig& raw,
    const std::function<bool(const SimConfig&)>& apply,
    std::string& feedback) {
    const ValidationResult r = InputValidator::validate(raw);
    if (!r.ok) {
        feedback = r.summary();
        ++rejected_;
        return false;
    }
    if (!isInRange(r.config)) {
        feedback = "Configuration rejected: values are outside the allowed limits.";
        ++rejected_;
        return false;
    }
    if (!apply || !apply(r.config)) {
        feedback = "The simulation rejected the configuration.";
        ++rejected_;
        return false;
    }

    active_ = r.config;
    feedback = "Configuration accepted.";
    ++accepted_;
    return true;
}
