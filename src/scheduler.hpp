#pragma once

#include <deque>
#include <list>

#include "process.hpp"

class Scheduler {
public:
    virtual ~Scheduler() = default;
    virtual void add_process(Process* p) = 0;
    virtual Process* get_next_process() = 0;
};

// ==================== //

class FCFS_Scheduler : public Scheduler {
private:
    std::deque<Process*> readyQ;

public:
    FCFS_Scheduler() = default;
    void add_process(Process* p) override;
    Process* get_next_process() override;
};

// ==================== //

class LCFS_Scheduler : public Scheduler {
private:
    std::deque<Process*> readyQ;

public:
    LCFS_Scheduler() = default;
    void add_process(Process* p) override;
    Process* get_next_process() override;
};

// ==================== //

class SRTF_Scheduler : public Scheduler {
private:
    std::list<Process*> readyQ;

public:
    SRTF_Scheduler() = default;
    void add_process(Process* p) override;
    Process* get_next_process() override;
};

// ==================== //

class RR_Scheduler : public Scheduler {
private:
    std::deque<Process*> readyQ;

public:
    RR_Scheduler() = default;
    void add_process(Process* p) override;
    Process* get_next_process() override;
};