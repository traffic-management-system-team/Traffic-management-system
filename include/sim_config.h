#ifndef SIM_CONFIG_H
#define SIM_CONFIG_H

// Structured, already-validated simulation configuration (FR-10).
// Plain data only: no parsing and no simulation logic lives here.
struct SimConfig {
    int tickMs          = 500;  // timer interval in milliseconds
    int greenTicks      = 8;    // green duration per axis, in ticks
    int yellowTicks     = 2;    // yellow duration per axis, in ticks
    int arrivalInterval = 3;    // one vehicle per road every N ticks
    int maxQueue        = 12;   // maximum queued vehicles per road
};

namespace SimLimits {
    constexpr int kTickMsMin = 100,        kTickMsMax = 5000;
    constexpr int kGreenMin = 1,           kGreenMax = 60;
    constexpr int kYellowMin = 1,          kYellowMax = 10;
    constexpr int kArrivalMin = 1,         kArrivalMax = 20;
    constexpr int kMaxQueueMin = 1,        kMaxQueueMax = 50;
}

// Range check used as a second line of defence by the simulation module.
inline bool isInRange(const SimConfig& c) {
    using namespace SimLimits;
    return c.tickMs >= kTickMsMin && c.tickMs <= kTickMsMax
        && c.greenTicks >= kGreenMin && c.greenTicks <= kGreenMax
        && c.yellowTicks >= kYellowMin && c.yellowTicks <= kYellowMax
        && c.arrivalInterval >= kArrivalMin && c.arrivalInterval <= kArrivalMax
        && c.maxQueue >= kMaxQueueMin && c.maxQueue <= kMaxQueueMax;
}

#endif
