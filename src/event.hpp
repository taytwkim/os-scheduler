#pragma once

#include "process.hpp"
#include "des.hpp"

struct Event {
    Process *proc;
    Transition transition;
    int timestamp;

    Event(Process *proc, Transition transition, int timestamp)
        : proc(proc),
          transition(transition),
          timestamp(timestamp) {}
};