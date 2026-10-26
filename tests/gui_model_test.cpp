#include <cstdlib>
#include <iostream>
#include "gui_model.h"

#define CHECK(cond) do { if (!(cond)) { \
    std::cerr << "CHECK failed: " #cond " (line " << __LINE__ << ")\n"; std::exit(1); } } while (0)

static void runTicks(SimModel& m, int n) { for (int i = 0; i < n; ++i) m.tick(); }

int main() {
    SimModel m;
    // TC-08/TC-12: initial state is Stopped and nothing advances until Start.
    CHECK(m.state() == SimModel::State::Stopped);
    CHECK(!m.tick());
    CHECK(m.tickCount() == 0);

    // Start runs the simulation; queues change.
    m.start();
    CHECK(m.state() == SimModel::State::Running);
    runTicks(m, 6);
    CHECK(m.tickCount() == 6);

    // TC-09: Pause freezes state, Resume continues.
    m.togglePause();
    CHECK(m.state() == SimModel::State::Paused);
    const unsigned long frozen = m.tickCount();
    CHECK(!m.tick());
    CHECK(m.tickCount() == frozen);
    m.togglePause();
    CHECK(m.state() == SimModel::State::Running);
    CHECK(m.tick());

    // Signals: never green on both axes at the same time.
    for (int i = 0; i < 60; ++i) {
        m.tick();
        const bool ns = m.signal(0) == SimModel::Signal::Green || m.signal(2) == SimModel::Signal::Green;
        const bool ew = m.signal(1) == SimModel::Signal::Green || m.signal(3) == SimModel::Signal::Green;
        CHECK(!(ns && ew));
    }

    // Queues stay within the display cap.
    for (int r = 0; r < SimModel::kRoads; ++r) CHECK(m.queueLength(r) <= SimModel::kMaxQueue);

    // Reset restores the initial state.
    m.reset();
    CHECK(m.state() == SimModel::State::Stopped);
    CHECK(m.tickCount() == 0);
    for (int r = 0; r < SimModel::kRoads; ++r) CHECK(m.queueLength(r) == 0);

    // Out-of-range road ids are safe.
    CHECK(m.queueLength(-1) == 0 && m.queueLength(99) == 0);
    CHECK(m.signal(99) == SimModel::Signal::Red);

    std::cout << "All GUI model tests passed.\n";
    return 0;
}