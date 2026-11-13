# Implement OS Scheduling Policies

I am revisiting one of the labs from my Operating Systems class (NYU Fall 2024, Professor Hubertus Franke) for fun and to brush up on my C++.

This lab implements a process scheduling simulation for different policies.

## Usage

`make` at the top level directory to compile. `make clean` to remove the executable.

```bash
make 
make clean
```

How to run the program:

```
./scheduler [-v] [-s<schedspec>] input_file rfile

[-v] print verbose traces to follow state transitions

[-s<schedspec>] scheduler configuration
```

Arguments can be provided in any order.

`rfile` is a file containing random numbers. It is used to seed a uniform repeatable behavior.

Scheduling algorithm can be configured:

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

The `input/` directory contains input files. `refout/` has reference output files. `refout-vp` has verbose reference output files.

There are scripts for grading.

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