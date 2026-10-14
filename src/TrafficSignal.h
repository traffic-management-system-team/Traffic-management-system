
#ifndef TRAFFIC_SIGNAL_H
#define TRAFFIC_SIGNAL_H

enum class SignalState {
    RED,
    YELLOW,
    GREEN
};

class TrafficSignal {
private:
    SignalState state;

public:
    explicit TrafficSignal(
        SignalState initialState = SignalState::RED
    );

    SignalState getState() const;
    bool canMove() const;
    bool changeState(SignalState newState);
    void reset();
};

#endif
