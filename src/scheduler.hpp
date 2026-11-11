#pragma once

#include <deque>

#include "process.hpp"

class Scheduler {
public:
    virtual ~Scheduler() = default;
    virtual void add_process(Process* p) = 0;
    virtual Process* get_next_process() = 0;
    // virtual void print_readyQ() = 0;
};

class FCFS_Scheduler : public Scheduler {
private:
    std::deque<Process*> readyQ;

public:
    FCFS_Scheduler() = default;
    void add_process(Process* p) override;
    Process* get_next_process() override;
    // void print_readyQ() override;
};