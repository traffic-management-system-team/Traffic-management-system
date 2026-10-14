
#include "TrafficSignal.h"

TrafficSignal::TrafficSignal(SignalState initialState)
    : state(initialState) {}

SignalState TrafficSignal::getState() const {
    return state;
}

bool TrafficSignal::canMove() const {
    return state == SignalState::GREEN;
}

bool TrafficSignal::changeState(SignalState newState) {
    bool validTransition =
        (state == SignalState::RED &&
         newState == SignalState::GREEN) ||
        (state == SignalState::GREEN &&
         newState == SignalState::YELLOW) ||
        (state == SignalState::YELLOW &&
         newState == SignalState::RED);

    if (!validTransition) {
        return false;
    }

    state = newState;
    return true;
}

void TrafficSignal::reset() {
    state = SignalState::RED;
}
