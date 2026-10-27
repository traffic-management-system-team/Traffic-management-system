#include <cstdlib>
#include <iostream>
#include <string>
#include "config_controller.h"
#include "input_validator.h"

#define CHECK(cond) do { if (!(cond)) { \
    std::cerr << "CHECK failed: " #cond " (line " << __LINE__ << ")\n"; std::exit(1); } } while (0)

static RawConfig good() { return RawConfig{"500", "8", "2", "3", "12"}; }

int main() {
    // Valid input becomes structured data.
    ValidationResult r = InputValidator::validate(good());
    CHECK(r.ok && r.errors.empty());
    CHECK(r.config.tickMs == 500 && r.config.greenTicks == 8 && r.config.yellowTicks == 2);
    CHECK(r.config.arrivalInterval == 3 && r.config.maxQueue == 12);

    // Surrounding blanks and a leading + are tolerated.
    RawConfig b = good(); b.greenTicks = "  10\t"; b.maxQueue = "+7";
    r = InputValidator::validate(b);
    CHECK(r.ok && r.config.greenTicks == 10 && r.config.maxQueue == 7);

    // Boundaries: min and max accepted, one outside rejected.
    CHECK(InputValidator::validate(RawConfig{"100", "1", "1", "1", "1"}).ok);
    CHECK(InputValidator::validate(RawConfig{"5000", "60", "10", "20", "50"}).ok);
    CHECK(!InputValidator::validate(RawConfig{"99", "8", "2", "3", "12"}).ok);
    CHECK(!InputValidator::validate(RawConfig{"500", "61", "2", "3", "12"}).ok);
    CHECK(!InputValidator::validate(RawConfig{"500", "8", "0", "3", "12"}).ok);
    CHECK(!InputValidator::validate(RawConfig{"500", "8", "2", "21", "12"}).ok);
    CHECK(!InputValidator::validate(RawConfig{"500", "8", "2", "3", "51"}).ok);

    // Malformed input is rejected, never crashes, and gives a message.
    const char* bad[] = {"", "   ", "abc", "5x", "x5", "5.5", "1e3", "-", "+", "--5", "0x10",
                         "-5", "99999999999999999999", "123456789012345678901234567890",
                         "5 5", "\x01\x02", "\xff\xfe", "5\n6", "NaN", "null"};
    for (const char* v : bad) {
        RawConfig x = good(); x.tickMs = v;
        r = InputValidator::validate(x);
        CHECK(!r.ok);
        CHECK(r.errors.size() == 1);
        CHECK(!r.summary().empty());
    }

    // Very long input is rejected and echoed in truncated form only.
    RawConfig big = good(); big.greenTicks = std::string(100000, '9');
    r = InputValidator::validate(big);
    CHECK(!r.ok && r.summary().size() < 300);

    // All invalid fields are reported together.
    r = InputValidator::validate(RawConfig{"a", "b", "c", "d", "e"});
    CHECK(!r.ok && r.errors.size() == 5);

    // Invalid result never carries values: config stays at defaults.
    CHECK(r.config.tickMs == SimConfig().tickMs);

    // Integration: only validated config becomes the active configuration.
    ConfigController ctl;
    std::string fb;
    CHECK(ctl.current().greenTicks == 8);                       // defaults at start
    CHECK(!ctl.submit(RawConfig{"abc", "8", "2", "3", "12"}, fb));
    CHECK(!fb.empty() && fb.find("Tick interval") != std::string::npos);
    CHECK(ctl.current().tickMs == 500 && ctl.rejectedCount() == 1);   // unchanged
    CHECK(ctl.submit(RawConfig{"200", "3", "1", "2", "4"}, fb));
    CHECK(ctl.current().tickMs == 200 && ctl.current().greenTicks == 3);
    CHECK(ctl.current().maxQueue == 4 && ctl.acceptedCount() == 1);
    CHECK(isInRange(ctl.current()));
    // A later invalid submission must not overwrite the good config.
    CHECK(!ctl.submit(RawConfig{"200", "999", "1", "2", "4"}, fb));
    CHECK(ctl.current().greenTicks == 3 && ctl.rejectedCount() == 2);

    std::cout << "TC-10 input validation passed.\n";
    return 0;
}
