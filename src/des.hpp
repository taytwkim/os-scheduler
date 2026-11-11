#pragma once

#include "scheduler.hpp"

enum State {
    CREATED,
    READY,
    RUNNING,
    BLOCKED
};

enum Transition {
    CREATED_TO_READY,
    RUNNING_TO_READY,
    BLOCKED_TO_READY,
    READY_TO_RUNNING,
    RUNNING_TO_BLOCKED
};

struct Event {
    Process *process;
    Transition transition;
    int timestamp;

    Event(Process *proc, Transition transition, int timestamp)
        : process(proc),
          transition(transition),
          timestamp(timestamp) {}
};

class DES {
private:
    std::vector<Event *> eventQ;

public:
    Event *get_event();
    void put_event();
    void rm_event();
};