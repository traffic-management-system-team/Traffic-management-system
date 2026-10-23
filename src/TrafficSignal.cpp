#include "TrafficSignal.h"

#include <stdexcept>

namespace {
bool isValidState(SignalState state) {
    return state == SignalState::RED ||
           state == SignalState::YELLOW ||
           state == SignalState::GREEN;
}

bool isValidTransition(
    SignalState current,
    SignalState next
) {
    return
        (current == SignalState::RED &&
         next == SignalState::GREEN) ||
        (current == SignalState::GREEN &&
         next == SignalState::YELLOW) ||
        (current == SignalState::YELLOW &&
         next == SignalState::RED);
}
}

SignalCoordinator::SignalCoordinator()
    : states{
        SignalState::RED,
        SignalState::RED,
        SignalState::RED,
        SignalState::RED
    } {}

SignalState SignalCoordinator::getState(
    std::size_t approach
) const {
    if (approach >= APPROACH_COUNT) {
        throw std::out_of_range("Invalid approach index");
    }

    return states[approach];
}

bool SignalCoordinator::canMove(
    std::size_t approach
) const {
    return getState(approach) == SignalState::GREEN;
}

bool SignalCoordinator::changeState(
    std::size_t approach,
    SignalState newState
) {
    if (approach >= APPROACH_COUNT ||
        !isValidState(newState)) {
        return false;
    }

    SignalState current = states[approach];

    if (!isValidTransition(current, newState)) {
        return false;
    }

    // Do not allow another approach to be GREEN.
    if (newState == SignalState::GREEN) {
        for (std::size_t i = 0; i < APPROACH_COUNT; ++i) {
            if (i != approach &&
                states[i] == SignalState::GREEN) {
                return false;
            }
        }
    }

    states[approach] = newState;
    return true;
}

void SignalCoordinator::reset() {
    states.fill(SignalState::RED);
}

TrafficSignal::TrafficSignal(SignalState initialState)
    : state(initialState),
      coordinator(nullptr),
      approach(0) {
    if (!isValidState(initialState)) {
        throw std::invalid_argument(
            "Invalid initial traffic signal state"
        );
    }
}

TrafficSignal::TrafficSignal(
    SignalCoordinator& sharedCoordinator,
    std::size_t approachIndex
)
    : state(SignalState::RED),
      coordinator(&sharedCoordinator),
      approach(approachIndex) {
    // Validate that the approach exists.
    state = coordinator->getState(approach);
}

SignalState TrafficSignal::getState() const {
    return coordinator
        ? coordinator->getState(approach)
        : state;
}

bool TrafficSignal::canMove() const {
    return coordinator
        ? coordinator->canMove(approach)
        : state == SignalState::GREEN;
}

bool TrafficSignal::changeState(SignalState newState) {
    if (!isValidState(newState)) {
        return false;
    }

    if (coordinator) {
        return coordinator->changeState(approach, newState);
    }

    if (!isValidTransition(state, newState)) {
        return false;
    }

    state = newState;
    return true;
}

void TrafficSignal::reset() {
    if (coordinator) {
        coordinator->reset();
    } else {
        state = SignalState::RED;
    }
}