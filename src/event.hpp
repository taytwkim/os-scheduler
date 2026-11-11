#pragma once

#include "process.hpp"
#include "des.hpp"
#include <string>

struct Event {
    Process *proc;
    Transition transition;
    int timestamp;

    Event(Process *proc, Transition transition, int timestamp)
        : proc(proc),
          transition(transition),
          timestamp(timestamp) {}
    
    std::string to_string() const {
        return "Process: " + std::to_string(proc->pid) + " ; " +
               "Transition: " + transition_to_string(transition) + " ; " +
               "Timestamp: " + std::to_string(timestamp);
    }
};