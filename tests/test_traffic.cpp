#include <iostream>
#include <stdexcept>
#include "../src/TrafficSignal.h"

namespace {
int failures = 0;

void check(bool condition, const char* description) {
    if (!condition) {
        std::cerr << "FAIL: " << description << '\n';
        ++failures;
    }
}

void testIndependentSignal() {
    TrafficSignal signal;

    check(signal.getState() == SignalState::RED,
          "Default signal starts RED");
    check(!signal.canMove(),
          "RED signal cannot move");

    check(signal.changeState(SignalState::GREEN),
          "RED -> GREEN succeeds");
    check(signal.canMove(),
          "GREEN signal permits movement");

    check(signal.changeState(SignalState::YELLOW),
          "GREEN -> YELLOW succeeds");
    check(!signal.canMove(),
          "YELLOW signal does not permit movement");

    check(signal.changeState(SignalState::RED),
          "YELLOW -> RED succeeds");

    check(!signal.changeState(SignalState::YELLOW),
          "RED -> YELLOW is rejected");

    signal.reset();
    check(signal.getState() == SignalState::RED,
          "Reset returns signal to RED");
}

void testSharedCoordinator() {
    SignalCoordinator coordinator;
    TrafficSignal north(coordinator, 0);
    TrafficSignal south(coordinator, 1);

    check(north.changeState(SignalState::GREEN),
          "First approach can become GREEN");

    check(!south.changeState(SignalState::GREEN),
          "Second approach cannot become GREEN simultaneously");

    check(north.changeState(SignalState::YELLOW),
          "First approach can change to YELLOW");

    check(south.changeState(SignalState::GREEN),
          "Second approach can become GREEN after first leaves GREEN");

    coordinator.reset();

    check(north.getState() == SignalState::RED &&
          south.getState() == SignalState::RED,
          "Coordinator reset sets all approaches to RED");
}

void testInvalidInitialState() {
    bool threw = false;

    try {
        TrafficSignal signal(static_cast<SignalState>(99));
    } catch (const std::invalid_argument&) {
        threw = true;
    }

    check(threw, "Invalid initial state is rejected");
}

void testInvalidApproach() {
    SignalCoordinator coordinator;
    bool threw = false;

    try {
        TrafficSignal invalid(coordinator, 4);
    } catch (const std::out_of_range&) {
        threw = true;
    }

    check(threw, "Invalid approach index is rejected");
}
}

int main() {
    testIndependentSignal();
    testSharedCoordinator();
    testInvalidInitialState();
    testInvalidApproach();

    if (failures != 0) {
        std::cerr << failures << " test(s) failed.\n";
        return 1;
    }

    std::cout << "All TrafficSignal tests passed!\n";
    return 0;
}