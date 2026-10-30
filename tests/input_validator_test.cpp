#include <cstdlib>
#include <iostream>
#include <string>
#include "config_controller.h"
#include "input_validator.h"

#define CHECK(cond) do { if (!(cond)) { \
    std::cerr << "CHECK failed: " #cond " (line " << __LINE__ << ")\n"; std::exit(1); } } while (0)

// Approved defaults: 1000 ms tick, GREEN 10, YELLOW 2, arrival every 3 ticks.
static RawConfig good() { return RawConfig{"1000", "10", "2", "3", "12"}; }

static bool sameConfig(const SimConfig& a, const SimConfig& b) {
    return a.tickMs == b.tickMs && a.greenTicks == b.greenTicks && a.yellowTicks == b.yellowTicks
        && a.arrivalInterval == b.arrivalInterval && a.maxQueue == b.maxQueue;
}

// Replace field `idx` (0..4) of a good config with `value`.
static RawConfig with(int idx, const std::string& value) {
    RawConfig r = good();
    std::string* f[] = {&r.tickMs, &r.greenTicks, &r.yellowTicks, &r.arrivalInterval, &r.maxQueue};
    *f[idx] = value;
    return r;
}

int main() {
    // ---- Defaults (approved specification) ----
    const SimConfig d;
    CHECK(d.tickMs == 1000);
    CHECK(d.greenTicks == 10);
    CHECK(d.yellowTicks == 2);
    CHECK(d.arrivalInterval == 3);
    CHECK(isInRange(d));
    ConfigController fresh;
    CHECK(sameConfig(fresh.current(), d));
    CHECK(fresh.acceptedCount() == 0 && fresh.rejectedCount() == 0);

    // ---- Valid inputs ----
    ValidationResult r = InputValidator::validate(good());
    CHECK(r.ok && r.errors.empty());
    CHECK(sameConfig(r.config, d));
    RawConfig b = good(); b.greenTicks = "  15\t"; b.maxQueue = "+7";   // blanks and + tolerated
    r = InputValidator::validate(b);
    CHECK(r.ok && r.config.greenTicks == 15 && r.config.maxQueue == 7);

    // ---- Boundary values for every field: min-1 / min / max / max+1 ----
    struct Bound { int lo, hi; };
    const Bound bounds[5] = {
        {SimLimits::kTickMsMin, SimLimits::kTickMsMax},
        {SimLimits::kGreenMin, SimLimits::kGreenMax},
        {SimLimits::kYellowMin, SimLimits::kYellowMax},
        {SimLimits::kArrivalMin, SimLimits::kArrivalMax},
        {SimLimits::kMaxQueueMin, SimLimits::kMaxQueueMax}};
    for (int i = 0; i < 5; ++i) {
        CHECK(InputValidator::validate(with(i, std::to_string(bounds[i].lo))).ok);
        CHECK(InputValidator::validate(with(i, std::to_string(bounds[i].hi))).ok);
        CHECK(!InputValidator::validate(with(i, std::to_string(bounds[i].lo - 1))).ok);
        CHECK(!InputValidator::validate(with(i, std::to_string(bounds[i].hi + 1))).ok);
    }

    // ---- Invalid / malformed inputs (every field) ----
    const char* bad[] = {"", "   ", "abc", "5x", "x5", "5.5", "1e3", "-", "+", "--5", "0x10",
                         "-5", "0", "99999999999999999999", "123456789012345678901234567890",
                         "5 5", "\x01\x02", "\xff\xfe", "5\n6", "NaN", "null"};
    for (int i = 0; i < 5; ++i) {
        for (const char* v : bad) {
            r = InputValidator::validate(with(i, v));
            CHECK(!r.ok);
            CHECK(r.errors.size() == 1);
            CHECK(!r.summary().empty());
        }
    }
    RawConfig big = good(); big.greenTicks = std::string(100000, '9');
    r = InputValidator::validate(big);
    CHECK(!r.ok && r.summary().size() < 300);                 // echoed input is truncated

    r = InputValidator::validate(RawConfig{"a", "b", "c", "d", "e"});
    CHECK(!r.ok && r.errors.size() == 5);                     // all errors reported together
    CHECK(sameConfig(r.config, d));                           // invalid result carries no values

    // ---- ConfigController: last valid configuration is preserved ----
    ConfigController ctl;
    std::string fb;
    CHECK(!ctl.submit(RawConfig{"abc", "10", "2", "3", "12"}, fb));
    CHECK(!fb.empty() && fb.find("Tick interval") != std::string::npos);
    CHECK(sameConfig(ctl.current(), d) && ctl.rejectedCount() == 1);   // defaults kept

    CHECK(ctl.submit(RawConfig{"2000", "20", "3", "4", "8"}, fb));
    const SimConfig lastValid = ctl.current();
    CHECK(lastValid.tickMs == 2000 && lastValid.greenTicks == 20 && lastValid.yellowTicks == 3);
    CHECK(lastValid.arrivalInterval == 4 && lastValid.maxQueue == 8);
    CHECK(ctl.acceptedCount() == 1);

    // Many kinds of invalid submissions, including ones where most fields are valid.
    const RawConfig invalidSubmissions[] = {
        RawConfig{"2000", "999", "3", "4", "8"},      // one field out of range
        RawConfig{"2000", "20", "3", "4", ""},        // one field empty
        RawConfig{"2000", "20", "3", "4x", "8"},      // trailing garbage
        RawConfig{"99", "20", "3", "4", "8"},         // just below tick minimum
        RawConfig{"", "", "", "", ""},                // everything empty
        RawConfig{"a", "b", "c", "d", "e"},           // everything malformed
        RawConfig{"1000", "10", "2", "3", std::string(5000, '1')}};
    unsigned long rejected = ctl.rejectedCount();
    for (const RawConfig& bad2 : invalidSubmissions) {
        CHECK(!ctl.submit(bad2, fb));
        CHECK(!fb.empty());
        CHECK(sameConfig(ctl.current(), lastValid));  // active config never overwritten
        CHECK(ctl.rejectedCount() == ++rejected);
    }
    CHECK(ctl.acceptedCount() == 1);
    CHECK(isInRange(ctl.current()));

    // A later valid submission replaces it normally.
    CHECK(ctl.submit(good(), fb) && sameConfig(ctl.current(), d) && ctl.acceptedCount() == 2);

    std::cout << "TC-10 input validation passed.\n";
    return 0;
}
