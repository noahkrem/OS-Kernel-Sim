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


