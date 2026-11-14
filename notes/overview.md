# Lab 2 Overview

There are three main components to lab 2.

First, we have the `Scheduler`, which maintains a `readyQ` of processes that are ready to run.

Next, we have the `DES`, a class representing a Discrete Event Simulator. The `DES` maintains an `eventQ`, a queue of `Event` objects.

Finally, we have the `Simulation` in `main.cpp`. The simulation sits between the `Scheduler` and `DES`.

## Process

`Process` represents a process and contains relevant information such as its `remaining_cpu_time`, `cpu_burst`, and `io_burst`.

## Scheduler

The `Scheduler` maintains a `readyQ`, a queue of `Process` objects. We can `add_process` and `get_next_process`.

`Scheduler` is a generic interface. `FCFS`, `LCFS`, `SRTF`, `RR`, `PRIO`, and `PREPRIO` extends the `Scheduler` base class.

## Event

An `Event` object represents a process' transition from one state to another. It contains relevant information such as `timestamp` (time where the event takes place), `process` (a pointer to a `Process` object), and `transition` (e.g., `READY_TO_RUNNING`).

The `Simulation` processes the events in the `eventQ`, and the simulation ends when there are no events in the `eventQ`.

## DES

The `DES` class represents a Discrete Event Simulator. The `DES` maintains an `eventQ`, a queue of `Event` objects. We can `get_event` (get next event), `put_event`, and `rm_event` from the `eventQ`.

## Simulation

The `Simulation` is where the simulation actually takes place. It processes each event in the `eventQ` and takes necessary actions until the `eventQ` is empty.
