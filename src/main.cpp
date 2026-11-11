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
DES Simulator;
Process *CURR_RUNNING_PROCESS;
std::vector<Process *> PROCESSES;

int quantum = 10000;
int max_prio = 4;
char *inputFileName;
char *randFileName;
bool verbose = false;
std::string sched_type;

std::vector<int> randVals; 
int numRandVals;
int randOffset;

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

int myrandom(int burst) 
{
    int val = 1 + (randVals[randOffset % numRandVals] % burst);
    ++randOffset;
    return val;
}

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

        int prio = myrandom(max_prio);

        Process *proc = new Process(pid, processData[0], 
            processData[1], processData[2], processData[3], prio);
        
        PROCESSES.push_back(proc);
        ++pid;
        
        Event *evt = new Event(proc, CREATED_TO_READY, processData[0]);
        Simulator.put_event(evt);
    }
}

void Simulation()
{
    Event* evt;
    int current_timestamp;

    int num_io = 0;
    int io_start_timestamp = 0;
    int io_busy_time = 0;
    bool call_scheduler = false;

    while ((evt = Simulator.get_event())) {
        Process* proc = evt->proc;
        current_timestamp = evt->timestamp;
        int transition = evt->transition;
        int timeInPrevState = current_timestamp - proc->state_start_time;
        proc->state_start_time = current_timestamp;

        delete evt;
        evt = nullptr;

        std::cout << current_timestamp << " "
                << proc->pid << " "
                << std::setfill(' ') << std::setw(2) << timeInPrevState
                << ": ";
        
        switch (transition) {
            case CREATED_TO_READY: {
                std::cout << "CREATED -> READY\n";
                SCHEDULER->add_process(proc);
                call_scheduler = true;
                break;
            }
            case RUNNING_TO_READY: {                
                SCHEDULER->add_process(proc);

                std::cout << "RUNNG -> READY"
                        << "  cb=" << proc->curr_cpu_burst
                        << " rem=" << proc->remaining_cpu_time
                        << " prio=" << proc->dynamic_prio
                        << '\n';

                call_scheduler = true;
                break;
            }
            case BLOCKED_TO_READY: {
                std::cout << "BLOCK -> READY\n";
                proc->io_time = proc->io_time + timeInPrevState;
                SCHEDULER->add_process(proc);
                call_scheduler = true;

                if (num_io == 1) {
                    int time = current_timestamp - io_start_timestamp;
                    io_busy_time += time;
                }

                --num_io;

                break;
            }
            case READY_TO_RUNNING: {
                proc->cpu_wait_time = proc->cpu_wait_time + timeInPrevState;
                
                int burst = std::min(
                    myrandom(proc->max_cpu_burst),
                    proc->remaining_cpu_time
                );

                proc->curr_cpu_burst = burst;
                Event *evt = new Event(proc, RUNNING_TO_BLOCKED,
                     current_timestamp + burst);
                Simulator.put_event(evt);

                std::cout << "READY -> RUNNG"
                        << " cb=" << proc->curr_cpu_burst
                        << " rem=" << proc->remaining_cpu_time
                        << " prio=" << proc->dynamic_prio
                        << '\n';
                
                break;
            }
            case RUNNING_TO_BLOCKED: {                
                CURR_RUNNING_PROCESS = nullptr;
                proc->remaining_cpu_time -= timeInPrevState;

                if (proc->remaining_cpu_time == 0) {
                    proc->finish_time = current_timestamp;
                    proc->turnaround_time = current_timestamp - proc->arrival_time;
                    std::cout << "DONE\n";
                } 
                else {
                    int burst = myrandom(proc->max_io_burst);
                    proc->curr_io_burst = burst;

                    Simulator.put_event(new Event(
                        proc, BLOCKED_TO_READY, current_timestamp + burst
                    ));

                    std::cout << "RUNNG -> BLOCK"
                            << " ib=" << proc->curr_io_burst
                            << " rem=" << proc->remaining_cpu_time
                            << '\n';
                    
                    if (num_io == 0) {
                        io_start_timestamp = current_timestamp;
                    }

                    ++num_io;
                }

                call_scheduler = true;
                break;
            }
        }

        if (call_scheduler) {
            if (Simulator.get_next_event_time() == current_timestamp) {
                continue;
            }

            call_scheduler = false;

            if (CURR_RUNNING_PROCESS == nullptr) {
                CURR_RUNNING_PROCESS = SCHEDULER->get_next_process();

                if (CURR_RUNNING_PROCESS == nullptr) {
                    continue;
                }

                Event *evt = new Event(CURR_RUNNING_PROCESS,
                     READY_TO_RUNNING, current_timestamp);
                
                Simulator.put_event(evt);
            }
        }
    }

    std::cout << sched_type << "\n";
    
    int cpu_busy_time = 0;
    int total_turnaround_time = 0;
    int total_wait_time = 0;

    for (Process *p : PROCESSES) {
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

                switch (first[0]) {
                    case 'F': {
                        sched_type = "FCFS";
                        SCHEDULER = std::make_unique<FCFS_Scheduler>();
                        break;
                    }
                    // case 'L': sched_type = LCFS; break;
                    // case 'S': sched_type = SRTF; break;
                    // case 'R': sched_type = RR; break;
                    // case 'P': sched_type = PRIO; break;
                    // case 'E': sched_type = PREPRIO; break;
                    default:
                        std::cerr << "Invalid scheduler type.\n";
                        return EXIT_FAILURE;
                }

                if (std::getline(ss, second)) {
                    max_prio = std::stoi(second);
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

    // std::cout << "Options: \n"
    //           << "Scheduler: " << std::to_string(sched_type) << "\n"
    //           << "Quantum: " << std::to_string(quantum) << "\n"
    //           << "Max Prio: " << std::to_string(max_prio) << "\n"
    //           << "Input File Name: " << inputFileName << "\n"
    //           << "Rand File Name: " << randFileName << "\n"
    //           << "Verbose: " << std::to_string(verbose) << "\n";
    
    parseRandomFile();
    parseInputFile();

    // SCHEDULER->print_readyQ();
    // Simulator.print_eventQ();

    Simulation();
    
    return EXIT_SUCCESS;
}