#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <sys/resource.h>
#include <unistd.h>

#include "des.hpp"
#include "process.hpp"
#include "event.hpp"
#include "scheduler.hpp"

std::unique_ptr<Scheduler> SCHEDULER;
DES SIMULATOR;
std::vector<std::unique_ptr<Process>> PROCESSES;
Process *CURR_RUNNING_PROCESS;

std::string sched_type;
int quantum = 10000;    // default
int max_prio = 4;       // default
char *inputFileName;
char *randFileName;
bool verbose = false;

std::vector<int> randVals; 
int numRandVals;
int randIndex;

// Read rfile and store random numbers in a vector.
void parseRandomFile()
{
    std::ifstream file(randFileName);
    std::string line;

    std::getline(file, line);
    numRandVals = std::stoi(line);
    
    while (std::getline(file, line)) {
        randVals.push_back(std::stoi(line));
    }
}

// Get next random number, wrap around if necessary.
int myrandom(int burst)
{
    int val = 1 + (randVals[randIndex % numRandVals] % burst);
    ++randIndex;
    return val;
}

// Read input file.
// There is one process per line. 
// Create a Process object and a "CREATED -> READY" event.
void parseInputFile()
{
    std::ifstream file(inputFileName);
    std::string line;
    int pid = 0;

    while (std::getline(file, line)) {
        // { Arrival Time, Total CPU Time, CPU Burst, IO Burst }
        std::vector<int> processData;

        std::istringstream ss(line);
        std::string value;

        while (ss >> value) {
            processData.push_back(std::stoi(value));
        }

        // Compute random priority.
        int prio = myrandom(max_prio);

        PROCESSES.push_back(std::make_unique<Process>(pid, processData[0],
            processData[1], processData[2], processData[3], prio));
        
        Process *proc = PROCESSES.back().get();
        
        ++pid;
        
        SIMULATOR.put_event(std::make_unique<Event>(
            proc, CREATED_TO_READY, processData[0]));
    }
}

void Simulation()
{
    int current_timestamp;  // Timestamp of the current event.

    // Needed to compute IO utilization.
    int num_io = 0;                 // How many processes are currently performing IO.
    int io_start_timestamp = 0;     // Time since at least one process is performing IO.
    int io_busy_time = 0;           // Time where at least one process is performing IO.

    // We first process the next event.
    // Some events set call_scheduler to true
    // so that the scheduler can schedule a new process.
    bool call_scheduler = false;

    while (auto evt = SIMULATOR.get_event()) {
        Process* proc = evt->proc;
        current_timestamp = evt->timestamp;
        int transition = evt->transition;
        int timeInPrevState = current_timestamp - proc->curr_state_start_time;
        proc->curr_state_start_time = current_timestamp;

        if (verbose) {
            std::cout << current_timestamp << " "
                << proc->pid << " "
                << std::setfill(' ') << std::setw(2) << timeInPrevState
                << ": ";
        }
        
        switch (transition) {
            case CREATED_TO_READY: {
                SCHEDULER->add_process(proc);
                
                if (verbose) {
                    std::cout << "CREATED -> READY\n";
                }

                if (sched_type == "PREPRIO" && CURR_RUNNING_PROCESS) {
                    bool cond1 = proc->dynamic_prio > CURR_RUNNING_PROCESS->dynamic_prio;

                    // The current running process could have a pending event now.
                    // If so, don't preempt it. It will be blocked anyway.
                    int event_timestamp = SIMULATOR.get_event_timestamp(CURR_RUNNING_PROCESS);
                    bool cond2 = event_timestamp > current_timestamp;

                    if (verbose) {
                        std::cout << "    --> PrioPreempt Cond1=" << cond1
                            << " Cond2=" << cond2
                            << " (" << event_timestamp - current_timestamp << ") --> "
                            << (cond1 && cond2 ? "YES" : "NO") << '\n';
                    }

                    if (cond1 && cond2) {
                        SIMULATOR.rm_event(CURR_RUNNING_PROCESS);

                        // Preempt current running process.
                        SIMULATOR.put_event(std::make_unique<Event>(
                            CURR_RUNNING_PROCESS, RUNNING_TO_READY, current_timestamp));
                    }
                }
                
                call_scheduler = true;

                break;
            }
            case RUNNING_TO_READY: {
                CURR_RUNNING_PROCESS = nullptr;

                proc->remaining_cpu_time -= timeInPrevState;
                proc->curr_cpu_burst -= timeInPrevState;

                if (verbose) {
                    std::cout << "RUNNG -> READY"
                        << "  cb=" << proc->curr_cpu_burst
                        << " rem=" << proc->remaining_cpu_time
                        << " prio=" << proc->dynamic_prio
                        << '\n';
                }

                proc->dynamic_prio--;

                if (sched_type == "RR") {
                    // Round Robin doesn't really use priority
                    // so reset it.
                    proc->dynamic_prio = proc->static_prio - 1;
                };

                SCHEDULER->add_process(proc);
                call_scheduler = true;
                
                break;
            }
            case BLOCKED_TO_READY: {
                proc->io_time = proc->io_time + timeInPrevState;
                proc->dynamic_prio = proc->static_prio - 1;
                SCHEDULER->add_process(proc);

                if (verbose) {
                    std::cout << "BLOCK -> READY\n";
                }

                if (sched_type == "PREPRIO" && CURR_RUNNING_PROCESS) {
                    bool cond1 = proc->dynamic_prio > CURR_RUNNING_PROCESS->dynamic_prio;
                    int event_timestamp = SIMULATOR.get_event_timestamp(CURR_RUNNING_PROCESS);
                    bool cond2 = event_timestamp > current_timestamp;

                    if (verbose) {
                        std::cout << "    --> PrioPreempt Cond1=" << cond1
                            << " Cond2=" << cond2
                            << " (" << event_timestamp - current_timestamp << ") --> "
                            << (cond1 && cond2 ? "YES" : "NO") << '\n';
                    }

                    if (cond1 && cond2) {
                        SIMULATOR.rm_event(CURR_RUNNING_PROCESS);
                        SIMULATOR.put_event(std::make_unique<Event>(
                            CURR_RUNNING_PROCESS, RUNNING_TO_READY, current_timestamp));
                    }
                }
                
                // IO is idle.
                if (num_io == 1) {
                    int time = current_timestamp - io_start_timestamp;
                    io_busy_time += time;
                }

                --num_io;

                call_scheduler = true;

                break;
            }
            case READY_TO_RUNNING: {
                proc->cpu_wait_time = proc->cpu_wait_time + timeInPrevState;

                if (proc->curr_cpu_burst == 0) {
                    proc->curr_cpu_burst = std::min(
                        myrandom(proc->max_cpu_burst),
                        proc->remaining_cpu_time
                    );
                }

                if (proc->curr_cpu_burst > quantum) {
                    // Create event for preemption.
                    SIMULATOR.put_event(std::make_unique<Event>(
                        proc, RUNNING_TO_READY, current_timestamp + quantum));
                }
                else {
                    // Create event for blocking.
                    SIMULATOR.put_event(std::make_unique<Event>(
                        proc, RUNNING_TO_BLOCKED, current_timestamp + proc->curr_cpu_burst));
                }

                if (verbose) {
                    std::cout << "READY -> RUNNG"
                        << " cb=" << proc->curr_cpu_burst
                        << " rem=" << proc->remaining_cpu_time
                        << " prio=" << proc->dynamic_prio
                        << '\n';
                }
                
                break;
            }
            case RUNNING_TO_BLOCKED: {                
                CURR_RUNNING_PROCESS = nullptr;
                proc->remaining_cpu_time -= timeInPrevState;
                proc->curr_cpu_burst = 0;

                if (proc->remaining_cpu_time == 0) {
                    proc->finish_time = current_timestamp;
                    proc->turnaround_time = current_timestamp - proc->arrival_time;

                    if (verbose) {
                        std::cout << "DONE\n";
                    }
                }
                else {
                    int io_burst = myrandom(proc->max_io_burst);
                    proc->curr_io_burst = io_burst;

                    SIMULATOR.put_event(std::make_unique<Event>(
                        proc, BLOCKED_TO_READY, current_timestamp + io_burst
                    ));

                    if (num_io == 0) {
                        io_start_timestamp = current_timestamp;
                    }

                    ++num_io;

                    if (verbose) {
                        std::cout << "RUNNG -> BLOCK"
                            << " ib=" << proc->curr_io_burst
                            << " rem=" << proc->remaining_cpu_time
                            << '\n';
                    }
                }

                call_scheduler = true;
                break;
            }
        }

        if (call_scheduler) {
            if (SIMULATOR.get_next_event_timestamp() == current_timestamp) {
                // if there are more events scheduled at the current timestamp,
                // process them first before scheduling a new process.
                // Note that call_scheduler is not reset here.
                continue;
            }

            call_scheduler = false;
            
            if (CURR_RUNNING_PROCESS == nullptr) {
                CURR_RUNNING_PROCESS = SCHEDULER->get_next_process();

                if (CURR_RUNNING_PROCESS == nullptr) {
                    continue;
                }

                // New READY -> RUNNING event.
                SIMULATOR.put_event(std::make_unique<Event>(
                    CURR_RUNNING_PROCESS, READY_TO_RUNNING, current_timestamp));
            }
        }
    }

    // Aggregate and print final stats
    std::cout << sched_type;
    
    if (quantum < 10000) {
        std::cout << " " << quantum;
    }
    
    std::cout << "\n";
    
    int cpu_busy_time = 0;
    int total_turnaround_time = 0;
    int total_wait_time = 0;

    for (const auto& p : PROCESSES) {
        p->print_process_stats();
        cpu_busy_time += p->total_cpu_time;
        total_turnaround_time += p->turnaround_time;
        total_wait_time += p->cpu_wait_time;
    }

    double cpu_util = 100.0 *  cpu_busy_time / current_timestamp;
    double io_util = 100.0 * io_busy_time / current_timestamp;
    double avg_turnaround = 1.0 * total_turnaround_time / PROCESSES.size();
    double avg_wait = 1.0 * total_wait_time / PROCESSES.size();
    double throughput = 100.0 * PROCESSES.size() / current_timestamp;
    
    std::cout << "SUM: " << current_timestamp << " "
            << std::fixed << std::setprecision(2)
            << cpu_util << " "
            << io_util << " "
            << avg_turnaround << " "
            << avg_wait << " "
            << std::setprecision(3)
            << throughput << '\n';
}

int main(int argc, char *argv[])
{
    // Usage: <program> [-v] [-s<schedspec>] inputfile randfile
    // –s [ FLS | R<num> | P<num>[:<maxprio>] | E<num>[:<maxprio>] ]
    
    int option;

    while ((option = getopt(argc, argv, "vs:")) != -1) {
        switch (option) {
            case 'v': {
                verbose = true;
                break;
            }
            case 's': {
                std::stringstream ss(optarg);

                // For E10:5, first = "E10" and second = "5"
                std::string first, second;

                std::getline(ss, first, ':');

                int n = (int) first.size();
                std::string q = first.substr(1, n-1);

                if ((int) q.size() > 0) {
                    quantum = std::stoi(q);
                }

                if (std::getline(ss, second)) {
                    max_prio = std::stoi(second);
                }
                
                switch (first[0]) {
                    case 'F': {
                        sched_type = "FCFS";
                        SCHEDULER = std::make_unique<FCFS_Scheduler>();
                        break;
                    }
                    case 'L': { 
                        sched_type = "LCFS";
                        SCHEDULER = std::make_unique<LCFS_Scheduler>();
                        break;
                    }
                    case 'S': {
                        sched_type = "SRTF";
                        SCHEDULER = std::make_unique<SRTF_Scheduler>();
                        break;
                    }
                    case 'R': {
                        sched_type = "RR";
                        SCHEDULER = std::make_unique<FCFS_Scheduler>();
                        break;
                    }
                    case 'P': {
                        sched_type = "PRIO";
                        SCHEDULER = std::make_unique<PRIO_Scheduler>(max_prio);
                        break;
                    }
                    case 'E': {
                        sched_type = "PREPRIO";
                        SCHEDULER = std::make_unique<PRIO_Scheduler>(max_prio);
                        break;
                    }

                    default:
                        std::cerr << "Invalid scheduler type.\n";
                        return EXIT_FAILURE;
                }

                break;
            }
            case '?':
                // getopt returns "?" for unknown options. 
                std::cerr << "Invalid argument.\n";
                return EXIT_FAILURE;
        }
    }

    if (optind + 2 > argc) {
        std::cerr << "Missing arguments.\n";
        return EXIT_FAILURE;
    }

    inputFileName = argv[optind];
    randFileName = argv[optind+1];
    
    parseRandomFile();
    parseInputFile();

    Simulation();
    
    return EXIT_SUCCESS;
}
