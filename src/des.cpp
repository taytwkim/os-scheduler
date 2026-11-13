#include "des.hpp"

#include <utility>

#include "event.hpp"

DES::DES() = default;
DES::~DES() = default;

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

std::unique_ptr<Event> DES::get_event() {
    if (eventQ.empty()) {
        return nullptr;
    }

    auto evt = std::move(eventQ.front());
    eventQ.pop_front();
    return evt;
}

void DES::put_event(std::unique_ptr<Event> event) {
    auto it = eventQ.begin();

    while (it != eventQ.end() &&
        (*it)->timestamp <= event->timestamp) {
        ++it;
    }

    eventQ.insert(it, std::move(event));
}

void DES::rm_event(Process *p, int curr_timestamp) {
    auto it = eventQ.begin();

    while (it != eventQ.end()) {
        if ((*it)->proc == p && curr_timestamp < (*it)->timestamp) {
            eventQ.erase(it);
            return;
        }
        ++it;
    }

}

int DES::get_event_timestamp(Process *p) {
    for (const auto& event : eventQ) {
        if (event->proc == p) {
            return event->timestamp;
        }
    }
    return -1;
}

int DES::get_next_event_timestamp() {
    if (eventQ.empty()) {
        return -1;
    }

    const auto& evt = eventQ.front();
    return evt->timestamp;
}
