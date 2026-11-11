#include "scheduler.hpp"
#include <iostream>

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

// void FCFS_Scheduler::print_readyQ() {
//     std::cout << "Printing readyQ:\n";
//     auto it = readyQ.begin();

//     while (it != readyQ.end()){
//         std::cout << (*it)->to_string() << "\n";
//         ++it;
//     }
// }