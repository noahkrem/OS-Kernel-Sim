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

Receive (R) - Receive a message, block until one arrives.

Reply (Y) - Unblocks sender and delivers reply.

New Semaphore (N) - Initialize the named semaphore with the value given. IDs can take a value from 0 to 4. This can only be done once for a semaphore, subsequent attempts result in error.

Semaphore P (P) - Execute the semaphore P operation on behalf of the running process. Assume sempahores IDs numbered 0 through 4.

Semaphore V (V) - Execute the semaphore V operation on behalf of the running process. Assume sempahores IDs numbered 0 through 4.

Procinfo (I) - Dump complete state information of process to screen.

Totalinfo (T) - Display all process queues and their contents.


## Other Info

Note that the above commands must be in upper case. Each command consists of a single character, and if other information is needed, the program will prompt the user.

To run this code, make the executable using the "make" command. Then, execute "./sim" at the command line. 