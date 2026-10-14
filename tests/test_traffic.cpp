
#include <cassert>
#include <iostream>
#include "../src/TrafficSignal.h"

int main() {
    TrafficSignal signal;

    // Initial state should be RED
    assert(signal.getState() == SignalState::RED);
    assert(!signal.canMove());

    // RED -> GREEN
    assert(signal.changeState(SignalState::GREEN));
    assert(signal.canMove());

    // GREEN -> YELLOW
    assert(signal.changeState(SignalState::YELLOW));
    assert(!signal.canMove());

    // YELLOW -> RED
    assert(signal.changeState(SignalState::RED));
    assert(!signal.canMove());

    // Invalid transition: RED -> YELLOW
    assert(!signal.changeState(SignalState::YELLOW));
    assert(signal.getState() == SignalState::RED);

    // Reset should restore RED
    assert(signal.changeState(SignalState::GREEN));
    signal.reset();
    assert(signal.getState() == SignalState::RED);
    assert(!signal.canMove());

    std::cout << "All TrafficSignal tests passed!\n";
    return 0;
}
