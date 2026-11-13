#include "scheduler.hpp"

void FCFS_Scheduler::add_process(Process* p) {
    readyQ.push_back(p);
};

Process* FCFS_Scheduler::get_next_process() {
    if (readyQ.empty()) {
        return nullptr;
    }

    Process *p = readyQ.front();
    readyQ.pop_front();
    return p;
};

// ==================== //

void LCFS_Scheduler::add_process(Process* p) {
    readyQ.push_back(p);
};

Process* LCFS_Scheduler::get_next_process() {
    if (readyQ.empty()) {
        return nullptr;
    }

    Process *p = readyQ.back();
    readyQ.pop_back();
    return p;
};

// ==================== //

void SRTF_Scheduler::add_process(Process* p) {
    auto it = readyQ.begin();

    while (it != readyQ.end() && (*it)->remaining_cpu_time <= p->remaining_cpu_time) {
        ++it;
    }

    readyQ.insert(it, p);
};

Process* SRTF_Scheduler::get_next_process() {
    if (readyQ.empty()) {
        return nullptr;
    }

    Process *p = readyQ.front();
    readyQ.pop_front();
    return p;
};

// ==================== //

void RR_Scheduler::add_process(Process* p) {
    readyQ.push_back(p);
};

Process* RR_Scheduler::get_next_process() {
    if (readyQ.empty()) {
        return nullptr;
    }

    Process *p = readyQ.front();
    readyQ.pop_front();
    return p;
};