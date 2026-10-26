#include "gui_model.h"

SimModel::Signal SimModel::signal(int road) const {
    if (road < 0 || road >= kRoads) return Signal::Red;
    const unsigned long t = ticks_ % 20;      // 20-tick cycle
    const bool northSouth = (road % 2 == 0);
    if (northSouth) {
        if (t < 8) return Signal::Green;      // 0-7
        if (t < 10) return Signal::Yellow;    // 8-9
        return Signal::Red;
    }
    if (t >= 10 && t < 18) return Signal::Green;   // 10-17
    if (t >= 18) return Signal::Yellow;            // 18-19
    return Signal::Red;
}

std::size_t SimModel::queueLength(int road) const {
    if (road < 0 || road >= kRoads) return 0;
    return queues_[road].size();
}

void SimModel::start() {
    if (state_ == State::Stopped) state_ = State::Running;
}

void SimModel::togglePause() {
    if (state_ == State::Running) state_ = State::Paused;
    else if (state_ == State::Paused) state_ = State::Running;
}

void SimModel::reset() {
    for (auto& q : queues_) q.clear();
    state_ = State::Stopped;
    ticks_ = 0;
    nextId_ = 1;
}

bool SimModel::tick() {
    if (state_ != State::Running) return false;
    for (int r = 0; r < kRoads; ++r) {
        // Vehicle arrival (deterministic pattern so tests are repeatable).
        if ((ticks_ + static_cast<unsigned long>(r) * 2) % 3 == 0 && queues_[r].size() < kMaxQueue)
            queues_[r].push_back(static_cast<int>(nextId_++));
        // Vehicle processing only when the signal permits it.
        if (signal(r) == Signal::Green && !queues_[r].empty())
            queues_[r].pop_front();
    }
    ++ticks_;
    return true;
}