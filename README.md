<img src="https://r2cdn.perplexity.ai/pplx-full-logo-primary-dark%402x.png" style="height:64px;margin-right:32px"/>

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

Your OS-Kernel-Sim README can be enhanced for a resume by highlighting technical skills, leadership, and results. Here’s an upgrade that emphasizes your expertise in operating systems, process management, concurrency, and interactive system design.

***

# OS-Kernel-Sim

OS-Kernel-Sim is a comprehensive, interactive operating system simulation designed to model key OS functionalities in real time. Developed as a command-line application, the simulator features robust process management, inter-process communication, and concurrency control—providing hands-on experience with low-level systems programming and kernel principles.

## Key Features

- **Interactive Process Lifecycle Management:**
Supports creating, forking, killing, and exiting processes, with dynamic scheduling and priority-aware queuing.
- **Inter-Process Communication (IPC):**
Implements synchronous messaging with send, receive, and reply commands, enforcing blocking semantics for realistic process coordination.
- **Semaphore-Based Concurrency:**
Models synchronization primitives with named semaphores (IDs 0-4), including atomic P (wait) and V (signal) operations to safely control resource access.
- **Kernel-Level Reporting:**
Enables real-time inspection of process and system states, with detailed process info dumps and visualization of all scheduling queues.
- **Error Handling \& Robust Input Validation:**
Ensures stability by managing invalid actions and providing clear command prompts for user input.


## Supported Commands

- **C** - Create a new process in the ready queue
- **F** - Fork the running process (priority-aware)
- **K** - Kill a named process
- **E** - Exit (terminate) the current process
- **Q** - Simulate process time quantum expiration
- **S** - Send a message to another process, block until replied
- **R** - Receive a message, block until available
- **Y** - Reply to sender, unblocking it
- **N** - Initialize a named semaphore (ID 0-4)
- **P** - Perform semaphore wait (P) on running process
- **V** - Perform semaphore signal (V) on running process
- **I** - Display full state of any process
- **T** - Display all process queues and their contents


## Usage Highlights

- All commands use uppercase, single-character inputs for speed and precision.
- Context-aware prompts ensure smooth workflow and minimize errors.
- Error handling covers command misuse and semaphore reinitialization attempts.


## Getting Started

1. Build the simulator:

```
make
```

2. Run interactively:

```
./sim
```


***

Would you prefer the README to mention any specific programming languages, optimization strategies, or teamwork aspects?

