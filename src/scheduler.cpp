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

// ==================== //

bool PRIO_Scheduler::activeQIsEmpty() {
    for (int i = 0; i < (int) activeQ.size(); ++i) {
        if (!activeQ[i].empty()) {
            return false;
        }
    }
    return true;
};

bool PRIO_Scheduler::expiredQIsEmpty() {
    for (int i = 0; i < (int) expiredQ.size(); ++i) {
        if (!expiredQ[i].empty()) {
            return false;
        }
    }
    return true;
};

void PRIO_Scheduler::add_process(Process* p) {
    if (p->dynamic_prio < 0) {
        p->dynamic_prio = p->static_prio - 1;
        expiredQ[p->dynamic_prio].push_back(p);
        return;
    }

    activeQ[p->dynamic_prio].push_back(p);
};

Process* PRIO_Scheduler::get_next_process() {
    if (activeQIsEmpty()) {
        if (expiredQIsEmpty()) {
            return nullptr;
        }
        activeQ.swap(expiredQ);
    }

    for (int i = (int) activeQ.size() - 1; i >= 0; --i) {
        if (!activeQ[i].empty()) {
            Process *p = activeQ[i].front();
            activeQ[i].pop_front();
            return p;
        }
    }

    return nullptr;
};