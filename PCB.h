
#ifndef _PCB_H_
#define _PCB_H_
#include <stdbool.h>
#include "List.h"

/*
#define LIST_SUCCESS 0
#define LIST_FAIL -1
*/

#define NUM_SEMAPHORE 5

enum ProcessState {
    RUNNING,
    READY,
    BLOCKED
};

typedef struct PCB_s PCB;
struct PCB_s {
    int pid;
    int priority;
    ProcessState state;
    char *send_msg;
    char *reply_msg;
};

typedef struct semaphore_t sem_t;
struct semaphore_t {
    int sem_value;
    List plist; // Processes blocked on this semaphore
};

sem_t sem_array[NUM_SEMAPHORE]; 

// Create a process and put it on the appropriate ready queue.
// Reports: success or failure, the pid of created process on success.
int create(int priority);

// Copy the currently running process and put it on the ready Q corresponding to the
//  original process' priority. Attempting to Fork the "init" process (see below) should fail.
// Reports: Success or failure, the pid of the resulting process on success.
int fork();

// Kill the named process and remove it from the system.
// Reports: Action taken as well as success or failure.
int kill(int pid);

// Kill the currently running process.
// Reports: Process scheduling information (which process now gets control of the cpu).
void exit();

// Time quantum of the running process expires.
// Reports: Action taken (process scheduling information).
void quantum();

// Send a message to another process, block until reply.
// Reports: success or failure, scheduling information, and reply source and text (once
//  reply arrives).
int send(int pid, char *msg);

// Receive a message, block until one arrives
// Reports: Scheduling information, message text, source of message.
void receive();

// Unblocks sender and delivers reply.
// Reports: Success or failure.
int reply(int pid, char *msg);

// Initialize the named semaphore with the value given. IDs can take a value from 0 to 4. 
//  This can only be done once for a semaphore - subsequent attempts result in error.
// Reports: Action taken as well as success or failure.
int new_Sem(int semaphore, unsigned int init);

// Execute the semaphore P operation on behalf of the running process. Assume semaphore 
//  IDs to be numbered 0 through 4.
// Reports: Action taken (blocked or not) as well as success or failure.
int sem_P(int sem_id);

// Execute the semaphore V operation on behalf of the running process. Assume semaphore 
//  IDs to be numbered 0 through 4.
int sem_V(int sem_id);

// Dump complete state information of process to screen.
void procinfo(int pid);

// Display all process queues and their contents
void totalinfo();


#endif