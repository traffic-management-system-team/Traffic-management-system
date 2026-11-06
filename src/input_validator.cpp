#include "input_validator.h"

namespace {
constexpr const char* kAsciiWhitespace = " \t\n\r\f\v";

std::string shown(const std::string& s) {
    std::string out;
    for (std::size_t i = 0; i < s.size() && i < 12; ++i) {
        const unsigned char c = static_cast<unsigned char>(s[i]);
        out += (c >= 32 && c < 127) ? static_cast<char>(c) : '?';
    }
    if (s.size() > 12) out += "...";
    return out;
}

void checkField(const char* name, const std::string& text, int lo, int hi,
                const char* field, int& target, ValidationResult& result) {
    const bool missing =
        text.find_first_not_of(kAsciiWhitespace) == std::string::npos;
    long long v = 0;
    if (!InputValidator::parseInt(text, v)) {
        const std::string message =
            std::string(name) + " must be a whole number between " +
            std::to_string(lo) + " and " + std::to_string(hi) +
            " (got '" + shown(text) + "').";
        result.errors.push_back(message);
        result.issues.push_back(ValidationIssue{
            field, missing ? "MISSING_VALUE" : "INVALID_INTEGER", message
        });
        return;
    }
    if (v < lo || v > hi) {
        const std::string message =
            std::string(name) + " must be between " + std::to_string(lo) +
            " and " + std::to_string(hi) + " (got " + std::to_string(v) + ").";
        result.errors.push_back(message);
        result.issues.push_back(ValidationIssue{
            field, "OUT_OF_RANGE", message
        });
        return;
    }
    target = static_cast<int>(v);
}

} // namespace

std::string ValidationResult::summary() const {
    std::string s;
    for (const auto& e : errors) {
        if (!s.empty()) s += "\n";
        s += e;
    }
    return s;
}

bool InputValidator::parseInt(const std::string& text, long long& out) {
    std::size_t b = text.find_first_not_of(kAsciiWhitespace);
    if (b == std::string::npos) return false;
    const std::size_t e = text.find_last_not_of(kAsciiWhitespace) + 1;
    if (b == e || e - b > 12) return false;          // empty or absurdly long

    bool neg = false;
    if (text[b] == '+' || text[b] == '-') {
        neg = (text[b] == '-');
        ++b;
        if (b == e) return false;                    // sign only
    }
    long long v = 0;
    for (std::size_t i = b; i < e; ++i) {
        if (text[i] < '0' || text[i] > '9') return false;
        v = v * 10 + (text[i] - '0');                // <= 11 digits, cannot overflow
    }
    out = neg ? -v : v;
    return true;
}

ValidationResult InputValidator::validate(const RawConfig& raw) {
    using namespace SimLimits;
    ValidationResult r;
    SimConfig c;
    checkField("Tick interval (ms)", raw.tickMs, kTickMsMin, kTickMsMax,
               "tickMs", c.tickMs, r);
    checkField("Green duration (ticks)", raw.greenTicks, kGreenMin, kGreenMax,
               "greenTicks", c.greenTicks, r);
    checkField("Yellow duration (ticks)", raw.yellowTicks, kYellowMin, kYellowMax,
               "yellowTicks", c.yellowTicks, r);
    checkField("Arrival interval (ticks)", raw.arrivalInterval, kArrivalMin, kArrivalMax,
               "arrivalInterval", c.arrivalInterval, r);
    checkField("Max queue length", raw.maxQueue, kMaxQueueMin, kMaxQueueMax,
               "maxQueue", c.maxQueue, r);
    r.ok = r.errors.empty();
    if (r.ok) r.config = c;
    return r;
}
