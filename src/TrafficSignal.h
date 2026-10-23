#ifndef TRAFFIC_SIGNAL_H
#define TRAFFIC_SIGNAL_H

#include <array>
#include <cstddef>

enum class SignalState {
    RED,
    YELLOW,
    GREEN
};

// Coordinates all approaches at the intersection.
class SignalCoordinator {
private:
    static constexpr std::size_t APPROACH_COUNT = 4;
    std::array<SignalState, APPROACH_COUNT> states;

public:
    SignalCoordinator();

    SignalState getState(std::size_t approach) const;
    bool canMove(std::size_t approach) const;

    bool changeState(
        std::size_t approach,
        SignalState newState
    );

    void reset();
};

// Controls one approach, optionally through a shared coordinator.
class TrafficSignal {
private:
    SignalState state;
    SignalCoordinator* coordinator;
    std::size_t approach;

public:
    explicit TrafficSignal(
        SignalState initialState = SignalState::RED
    );

    TrafficSignal(
        SignalCoordinator& sharedCoordinator,
        std::size_t approachIndex
    );

    SignalState getState() const;
    bool canMove() const;
    bool changeState(SignalState newState);
    void reset();
};

#endif