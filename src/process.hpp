#pragma once

#include <iomanip>
#include <iostream>

struct Process {
    int pid;

    int arrival_time;
    int total_cpu_time;
    int max_cpu_burst;
    int max_io_burst;
    int static_prio;

    int state_start_time;
    int remaining_cpu_time;
    int curr_cpu_burst;
    int curr_io_burst;
    int dynamic_prio;

    int finish_time;
    int turnaround_time;
    int io_time;
    int cpu_wait_time;

    Process(int pid, int arrival, int total, int cpu, int io, int static_prio)
        : pid(pid),
          arrival_time(arrival),
          total_cpu_time(total),
          max_cpu_burst(cpu),
          max_io_burst(io),
          static_prio(static_prio),
          state_start_time(arrival),
          remaining_cpu_time(total_cpu_time),
          dynamic_prio(static_prio-1){}

    void print_process_stats() {
        std::cout
            << std::setfill('0') << std::setw(4) << pid << ": "
            << std::setfill(' ')
            << std::setw(4) << arrival_time << " "
            << std::setw(4) << total_cpu_time << " "
            << std::setw(4) << max_cpu_burst << " "
            << std::setw(4) << max_io_burst << " "
            << static_prio << " | "
            << std::setw(5) << finish_time << " "
            << std::setw(5) << turnaround_time << " "
            << std::setw(5) << io_time << " "
            << std::setw(5) << cpu_wait_time << '\n';
    }
};