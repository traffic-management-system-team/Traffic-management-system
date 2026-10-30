#ifndef INPUT_VALIDATOR_H
#define INPUT_VALIDATOR_H

#include <string>
#include <vector>
#include "sim_config.h"

// Raw, untrusted text exactly as the user typed it (GUI fields, CLI, file).
struct RawConfig {
    std::string tickMs;
    std::string greenTicks;
    std::string yellowTicks;
    std::string arrivalInterval;
    std::string maxQueue;
};

struct ValidationResult {
    bool ok = false;
    SimConfig config;                 // meaningful only when ok == true
    std::vector<std::string> errors;  // one clear message per invalid field

    std::string summary() const;      // all errors joined, for display to the user
};

class InputValidator {
public:
    // Never throws and never touches the simulation. Reports every invalid field.
    static ValidationResult validate(const RawConfig& raw);

    // Strict whole-number parse: optional sign, digits only, surrounding blanks allowed.
    static bool parseInt(const std::string& text, long long& out);
};

#endif
