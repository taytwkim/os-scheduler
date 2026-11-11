#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

#include "des.hpp"
#include "scheduler.hpp"

std::vector<Process*> processes;
std::vector<int> randVals; 
int numRandVals;
int randOffset;

void parseRandomFile(char * randFileName)
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

void parseInputFile(char *inputFileName)
{
    std::ifstream file(inputFileName);
    std::string line;

    while (std::getline(file, line)) {
        // {Arrival Time, Total CPU Time, CPU Burst, IO Burst}
        std::vector<int> processData;

        std::istringstream ss(line);
        std::string value;

        while (ss >> value) {
            processData.push_back(std::stoi(value));
        }

        Process *process = new Process(processData[0], 
            processData[1], processData[2], processData[3]);
        
        processes.push_back(process);
    }
}

void Simulation()
{

}

int main(int argc, char *argv[])
{
    // Usage: <program> [-v] [-s<schedspec>] inputfile randfile
    // –s [ FLS | R<num> | P<num>[:<maxprio>] | E<num>[:<maxprio>] ]

    Scheduler_Type sched_type;
    int quantum = 10000;
    int max_prio = 4;
    char *inputFileName;
    char *randFileName;
    bool verbose = false;
    
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
                quantum = std::stoi(q);

                switch (first[0]) {
                    case 'F': sched_type = FCFS; break;
                    case 'L': sched_type = LCFS; break;
                    case 'S': sched_type = SRTF; break;
                    case 'R': sched_type = RR; break;
                    case 'P': sched_type = PRIO; break;
                    case 'E': sched_type = PREPRIO; break;
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

    std::cout << "Options: \n"
              << "Scheduler: " << std::to_string(sched_type) << "\n"
              << "Quantum: " << std::to_string(quantum) << "\n"
              << "Max Prio: " << std::to_string(max_prio) << "\n"
              << "Input File Name: " << inputFileName << "\n"
              << "Rand File Name: " << randFileName << "\n"
              << "Verbose: " << std::to_string(verbose) << "\n";
    
    parseInputFile(inputFileName);

    parseRandomFile(randFileName);
    
    return EXIT_SUCCESS;
}