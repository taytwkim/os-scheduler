# Scheduling Policies

The scheduler maintains a `readyQ`, a queue of processes that are ready to run. The scheduling algorithm chooses which process gets to run next.

## FCFS (First Come First Served)

Processes are scheduled in the order they arrive.

## LCFS (Last Come Last Served)

The most recently arrived process is scheduled first.

## SRTF (Shortest Remaining Time First)

The process with the shortest remaining CPU time is scheduled.

## RR (Round Robin)

In Round Robin, processes are scheduled in a FCFS manner. Each process gets a fixed time slice (called a `time quantum`) to run.

If a process doesn't finish during its time slice, it is preempted (transitions from run to ready state) and placed at the back of the queue, and the next process is scheduled.

It may be a confusing to understand how `time quantum` works together with `cpu_burst`. `cpu_burst` represents how long the process needs to run on the CPU before it needs IO. If the `time quantum` is shorter than the `cpu_burst`, the process gets preempted before it finishes the current CPU burst.

A new `cpu_burst` is not generated when the process is preempted; it is recomputed only when the current `cpu_burst` has been consumed. When the preempted process runs again, it will continue with the remaining `cpu_burst`.

## PRIO (Priority)

Each process is assigned a *static priority* (randomly assigned in our case). 

The scheduler picks the process with the highest priority to run. With every preemption the dynamic priority of a process decreases by one. Processes with the same priority level are scheduled in a round robin manner.

PRIO should accept a `time quantum` and optionally the number of priority levels, `maxprio`. The scheduler has exactly `maxprio` priority levels `[0..maxprio-1]`, `maxprio-1` being the highest.

In PRIO, we have the `activeQ` and `expiredQ` instead of a single `readyQ`. `activeQ` and `expiredQ` can be represented as a vector of deques `vector<deque<Process*>>`. We maintain a queue for each priority level (this is called MLQ, Multi Level Queueing). For example, if `maxprio` = 3, we have three `activeQ`s and three `expiredQ`s.

When a process’s dynamic priority becomes -1, it is reset to `static_priority-1` and the process is enqueued into the `expiredQ`. When the `activeQ` becomes empty, active and expired queues are swapped. This ensures that every process will get at least one chance to run before we start resetting priority.

## PREPRIO (Preemptive Priority)

This is similar to PRIO, but with preemption. When a process with a higher priority becomes ready, it can preempt the currently running process, even if the current process has not finished its CPU burst.