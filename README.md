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

Arguments can be in any order.

`rfile` is a file containing random numbers. It is used to seed a uniform repeatable behavior.

Scheduling algorithm can be configured:

```
–s [ FLS | R<num> | P<num>[:<maxprio>] | E<num>[:<maxprios>] ]

    F: FCFS
    L: LCFS
    S: SRTF
    R: RR
    P: PRIO 
    E: PREPRIO

e.g.,
    ./sched –sR10 -> RR, time quantum = 10
    ./sched -sE10:5 -> PREPRIO, time quantum=10, maxprios=5
```

The `input/` directory contains input files. `refout/` has reference output files. `refout-vp` has verbose reference output files.

```
scripts/runit.sh myout ./scheduler
```

```
scripts/gradeit.sh refout-vp myout
```