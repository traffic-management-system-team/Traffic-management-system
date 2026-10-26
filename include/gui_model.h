#ifndef GUI_MODEL_H
#define GUI_MODEL_H

#include <cstddef>
#include <deque>

// GUI-facing simulation state for the basic GUI foundation (FR-08, FR-09, FR-12).
// Roads: 0 = North, 1 = East, 2 = South, 3 = West.
// Signal permission is enforced HERE (controller layer), not in the queue code.
class SimModel {
public:
    enum class State { Stopped, Running, Paused };
    enum class Signal { Red, Yellow, Green };

    static constexpr int kRoads = 4;
    static constexpr std::size_t kMaxQueue = 12;

    State state() const { return state_; }
    Signal signal(int road) const;
    std::size_t queueLength(int road) const;
    unsigned long tickCount() const { return ticks_; }

    void start();        // Stopped -> Running
    void togglePause();  // Running <-> Paused (ignored when Stopped)
    void reset();        // back to initial Stopped state, queues cleared
    bool tick();         // advance one step; returns true if state changed

private:
    // TODO(queue-integration): replace these deques with QueueManager calls
    // (addVehicle / dequeueVehicle / getQueueLength) once headers are confirmed.
    std::deque<int> queues_[kRoads];
    State state_ = State::Stopped;
    unsigned long ticks_ = 0;
    unsigned long nextId_ = 1;
};

#endif