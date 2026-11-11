#pragma once

#include <list>

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
    RUNNING_TO_BLOCKED,
};

std::string transition_to_string(Transition t);
struct Event;

class DES {
private:
    std::list<Event*> eventQ;

public:
    Event *get_event();
    void put_event(Event *event);
    // void rm_event();
    int get_next_event_time();
    // void print_eventQ();
};