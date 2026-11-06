#ifndef CONFIG_CONTROLLER_H
#define CONFIG_CONTROLLER_H

#include <functional>
#include <string>
#include "input_validator.h"
#include "sim_config.h"

// Control-flow entry point for user-entered configuration (FR-10, SEC-01).
// UI/CLI code submits raw input with an application callback; current() changes
// only after that callback accepts a validated SimConfig.
class ConfigController {
public:
    // Validates raw text and asks the caller to apply the complete candidate.
    // The active config changes only when the caller accepts it.
    bool submit(const RawConfig& raw,
                const std::function<bool(const SimConfig&)>& apply,
                std::string& feedback);

    const SimConfig& current() const { return active_; }
    unsigned long acceptedCount() const { return accepted_; }
    unsigned long rejectedCount() const { return rejected_; }

private:
    SimConfig active_;          // defaults until a valid config is submitted
    unsigned long accepted_ = 0;
    unsigned long rejected_ = 0;
};

#endif
