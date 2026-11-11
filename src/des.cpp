#include "des.hpp"

#include <iostream>

#include "event.hpp"

std::string transition_to_string(Transition t) {
    switch (t) {
        case CREATED_TO_READY:  return "CREATED -> READY";
        case RUNNING_TO_READY:  return "RUNNING -> READY";
        case BLOCKED_TO_READY:  return "BLOCKED -> READY";
        case READY_TO_RUNNING:  return "READY -> RUNNING";
        case RUNNING_TO_BLOCKED:return "RUNNING -> BLOCKED";
    }
    return "UNKNOWN";
}

Event* DES::get_event() {
    if (eventQ.empty()) {
        return nullptr;
    }

    Event* evt = eventQ.front();
    eventQ.pop_front();
    return evt;
}

void DES::put_event(Event *event) {
    auto it = eventQ.begin();

    while (it != eventQ.end() &&
        (*it)->timestamp <= event->timestamp) {
        ++it;
    }

    eventQ.insert(it, event);
}

// void DES::rm_event() {

// }

// void DES::print_eventQ() {
//     std::cout << "Printing eventQ:\n";
//     auto it = eventQ.begin();

//     while (it != eventQ.end()){
//         std::cout << (*it)->to_string() << "\n";
//         ++it;
//     }
// }

int DES::get_next_event_time() {
    if (eventQ.empty()) {
        return -1;
    }

    Event* evt = eventQ.front();
    return evt->timestamp;
}