#pragma once

#include <deque>
#include <string>

enum Scheduler_Type {
    FCFS,
    LCFS,
    SRTF,
    RR,
    PRIO,
    PREPRIO
};

struct Process {
    int arrival_time;
    int total_cpu_time;
    int cpu_burst;
    int io_burst;

    Process(int arrival, int total, int cpu, int io)
        : arrival_time(arrival),
          total_cpu_time(total),
          cpu_burst(cpu),
          io_burst(io) {}
        
    std::string to_string() const {
        return std::to_string(arrival_time) + " " +
               std::to_string(total_cpu_time) + " " +
               std::to_string(cpu_burst) + " " +
               std::to_string(io_burst);
    }
};

class Scheduler {
public:
    virtual ~Scheduler() = default;
    virtual void add_process(Process* p) = 0;
    virtual Process* get_next_process() = 0;
};

class FCFS_Scheduler : public Scheduler {
private:
    std::deque<Process*> readyQ;

public:
    FCFS_Scheduler() = default;
    void add_process(Process* p) override;
    Process* get_next_process() override;
};