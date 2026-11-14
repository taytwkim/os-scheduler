# OS Scheduler Simulation

I am revisiting one of the labs from my Operating Systems class (NYU Fall 2024, Professor Hubertus Franke) for fun and to brush up on my C++.

This lab implements a simulation of process scheduling under different policies: First Come First Served, Last Come First Served, Round Robin, Priority, and Preemptive Priority.

The input is a text file describing each process, including its arrival time, total CPU time, and CPU and IO burst limits. After the simulation, the program prints statistics such as total elapsed time, CPU utilization, and IO utilization. Verbose output also logs each process’s state transitions.

When a process starts running, the simulation generates a random CPU burst. Once the burst finishes, the process blocks for IO, unless it has completed its total CPU time. The simulation then generates a random IO burst, after which the process becomes ready again.

Some schedulers use a time quantum to limit how long a process can run at once. If its CPU burst exceeds the quantum, the process is preempted after the quantum and later resumes with the remaining CPU burst.

Priority schedulers use priority levels to determine which process runs next. The Preemptive Priority scheduler also allows a newly ready process with a higher priority to preempt the currently running process.

See `notes/` for more documentation.

## Usage

`make` at the top level directory to compile. `make clean` to remove the executable.

```bash
make 
make clean
```

How to run the program:

```
./scheduler [-v] [-s<schedspec>] input_file rfile
```

* `-v`: print verbose traces to follow state transitions
* `-s<schedspec>`: scheduler spec

Arguments can be provided in any order.

`rfile` is a file containing random numbers. It is used to seed a uniform repeatable behavior.

### Scheduling Policies

The simulation supports following scheduling policies:

```
–s [ FLS | R<num> | P<num>[:<maxprio>] | E<num>[:<maxprios>] ]

    F: FCFS (First Come First Served)
    L: LCFS (Last Come First Served)
    S: SRTF (Shorting Remaining Time First)
    R: RR   (Round Robin)
    P: PRIO (Priority)
    E: PREPRIO  (Preemptive Priority)

e.g.,
    ./sched –sR10 -> RR, time quantum = 10
    ./sched -sE10:5 -> PREPRIO, time quantum=10, maxprios=5
```

`input/` contains input files. `refout/` has reference output files. `refout-vp` has verbose reference output files.

### Testing

There are scripts for testing and grading.

```bash
# write results to myout
scripts/runit.sh myout ./scheduler

# compare myout with reference output
scripts/gradeit.sh refout myout
```

Expected result:

```
in    F    L    S   R2   R5   P2   P5:3 E2:5  E4
00    1    1    1    1    1    1    1    1    1
01    1    1    1    1    1    1    1    1    1
02    1    1    1    1    1    1    1    1    1
03    1    1    1    1    1    1    1    1    1
04    1    1    1    1    1    1    1    1    1
05    1    1    1    1    1    1    1    1    1
06    1    1    1    1    1    1    1    1    1
07    1    1    1    1    1    1    1    1    1

SUM   8    8    8    8    8    8    8    8    8 
72 out of 72 correct
```