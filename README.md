# OS-Kernel-Sim

The goal of this assignment is to create an interactive operating system simulation, which supports a few basic OS functions. The simulation is keyboard driven, and makes reports to the screen.

## Commands

The following commands have been implemented:

Create (C) - Create a process, put it on the ready queue

Fork (F) - Copy the currently running process, put it on the ready queue corresponding to the orginal process' priority.

Kill (K) - Kill the named process and remove it from the system.

Exit (E) - Kill the currently running process.

Quantum (Q) - Time quantum of running process expires

Send (S) - Send a message to another process, block until reply.
