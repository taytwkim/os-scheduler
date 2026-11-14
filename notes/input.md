# Input and Output Formatting

## Input

*  Sample Input File
``` 
0    100 10 10 
500  100 20 10 
```

* Annotated Version
```
AT   TC  CB IO
0    100 10 10 
500  100 20 10 
```
Each line represents a process. In the example above, two processes will be scheduled. 

* `AT` (Arrival Time): the time at which the process arrives / is created.
* `TC` (Total CPU Time): total CPU time the process requires
* `CB` (CPU Burst): the upper limit of `cpu_burst`
* `IO` (IO Burst): the upper limit of `io_burst`

## Output

* Sample Output File
```
FCFS
0000:    0  100   10   10 2 |   201   201   101     0
0001:  500  100   20   10 4 |   627   127    27     0
SUM: 627 31.90 20.41 164.00 0.00 0.319
```

* Annotated Version
```
 PID    AT   TC   CB   IO PRIO      FT    TT    IT    CW
0000:    0  100   10   10    2 |   201   201   101     0
0001:  500  100   20   10    4 |   627   127    27     0

SUM:
     Total_Sim_Time    CPU_Utilization    IO_Utilization
                627              31.90             20.41             

AVG_Turnaround_Time      Avg_Wait_Time        Throughput
             164.00               0.00             0.319
```

#### Process Summary

* `PRIO`: static priority assigned to the process (relevant only for PRIO and PREPRIO)
* `TT` (Turnaround Time): FT - AT
* `IT` (IO Time): time in blocked state
* `CW` (CPU Wait Time): time in ready state

#### Simulation Summary

* `Total_Sim_Time`: finishing time of the last event
* `CPU_Utilization`: percentage (0.0 – 100.0) of time at least one process is running
* `IO_Utilization`: percentage (0.0 – 100.0) of time at least one process is performing IO
* `AVG_Turnaround_Time`: average of `TT`
* `Avg_Wait_Time`: average of `CW`
* `Throughput`: throughput of number processes per 100 time units
    * throughput = (number of completed processes) / total time * 100
    * e.g., if our simulation runs for 500 time units and during that time, 20 processes are completed, throughput is 4. 
