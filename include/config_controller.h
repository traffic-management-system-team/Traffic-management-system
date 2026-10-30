#ifndef CONFIG_CONTROLLER_H
#define CONFIG_CONTROLLER_H

#include <string>
#include "input_validator.h"
#include "sim_config.h"

// Control-flow entry point for user-entered configuration (FR-10, SEC-01).
// UI/CLI code calls submit(); simulation modules only ever read current(),
// which always holds a validated SimConfig. Parsing never happens in the simulation.
class ConfigController {
public:
    // Validates raw text. On success the active config is replaced and true is
    // returned. On failure the active config is left unchanged and `feedback`
    // contains clear messages for the user.
    bool submit(const RawConfig& raw, std::string& feedback);

    const SimConfig& current() const { return active_; }
    unsigned long acceptedCount() const { return accepted_; }
    unsigned long rejectedCount() const { return rejected_; }

private:
    SimConfig active_;          // defaults until a valid config is submitted
    unsigned long accepted_ = 0;
    unsigned long rejected_ = 0;
};

#endif
