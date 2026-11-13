#pragma once

#include <list>
#include <memory>
#include <string>

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

struct Process;
struct Event;

class DES {
private:
    std::list<std::unique_ptr<Event>> eventQ;

public:
    DES();
    ~DES();
    std::unique_ptr<Event> get_event();
    void put_event(std::unique_ptr<Event> event);
    void rm_event(Process *p, int curr_timestamp);
    int get_next_event_timestamp();
    int get_event_timestamp(Process *p);
};
