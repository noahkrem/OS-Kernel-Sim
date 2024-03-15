
#include "PCB.h"
#include <stdio.h>

static PCB* CURRENT = NULL;
static sem_t sem_array[NUM_SEMAPHORE]; 
static List * ready_lists[NUM_READY_LIST];
static List * waiting_lists[NUM_WAITING_LIST];

// Create a process and put it on the appropriate ready queue.
// Reports: success or failure, the pid of created process on success.
int create(int priority) {

}

// Copy the currently running process and put it on the ready Q corresponding to the
//  original process' priority. Attempting to Fork the "init" process (see below) should fail.
// Reports: Success or failure, the pid of the resulting process on success.
int fork() {
    
}

// Kill the named process and remove it from the system.
// Reports: Action taken as well as success or failure.
int kill(int pid) {

}

// Kill the currently running process.
// Reports: Process scheduling information (which process now gets control of the cpu).
void exit_proc() {

}

// Time quantum of the running process expires.
// Reports: Action taken (process scheduling information).
void quantum() {

}

// Send a message to another process, block until reply.
// Reports: success or failure, scheduling information, and reply source and text (once
//  reply arrives).
int send(int pid, char *msg) {

}

// Receive a message, block until one arrives
// Reports: Scheduling information, message text, source of message.
void receive() {

}

// Unblocks sender and delivers reply.
// Reports: Success or failure.
int reply(int pid, char *msg) {

}

// Initialize the named semaphore with the value given. IDs can take a value from 0 to 4. 
//  This can only be done once for a semaphore - subsequent attempts result in error.
// Reports: Action taken as well as success or failure.
int new_Sem(int sem_id, unsigned int init) {

}

// Execute the semaphore P operation on behalf of the running process. Assume semaphore 
//  IDs to be numbered 0 through 4.
// Reports: Action taken (blocked or not) as well as success or failure.
int sem_P(int sem_id) {
    sem_array[sem_id].sem_value--;
    if(sem_array[sem_id].sem_value < 0) {
        List_append(sem_array[sem_id].pList, CURRENT);
        CURRENT->state = BLOCKED;
    }
}

// Execute the semaphore V operation on behalf of the running process. Assume semaphore 
//  IDs to be numbered 0 through 4.
int sem_V(int sem_id) {
    sem_array[sem_id].sem_value++;
    if(sem_array[sem_id].sem_value <= 0) {
        PCB* temp = (PCB *)dequeue(sem_array[sem_id].pList);
        temp->state = READY;
    }
}

// Dump complete state information of process to screen.
void procinfo(int pid) {

}

// Display all process queues and their contents
void totalinfo() {
    
}


// PRIVATE FUNCTIONS

// Dequeue from list
static void * dequeue(List * list) {
    List_first(list);
    void *ret = List_remove(list);
    return ret;
}

// Initialize all lists
void initProgram(List * readyTop, List * readyNorm, List * readyLow, List * waitingSend, List * waitingReceive) {

    ready_lists[0] = readyTop;
    ready_lists[1] = readyNorm;
    ready_lists[2] = readyLow;

    waiting_lists[0] = waitingSend;
    waiting_lists[1] = waitingReceive;

    while(1) {
        while(readyListEmpty) {
            checkInput();
        }
    }
}

void checkInput() {
    char input;
    int pid_input;
    scanf("%c", &input);
    switch (input) {
        case 'C':
            int priority;
            printf("Please enter a priority number for the process (0 = high, 1 = norm, 2 = low)\n");
            scanf("%d", &priority);
            if(create(priority) == -1) {
                printf("failure\n");
            }
            else {
                printf("success\n");
            }
            break;
        case 'F':
            if(fork() == -1) {
                printf("failure\n");
            }
            else {
                printf("success\n");
            }
            break;
        case 'K':
            printf("Please enter the pid of the process you want to delete\n");
            scanf("%d", &pid_input);
            if(kill(pid_input) == -1) {
                printf("failure\n");
            }
            else {
                printf("succes\n");
            }
            break;
        case 'E':
            exit_proc();
            printf("report info\n");
            break;
        case 'Q':
            quantum();
            printf("process scheduling info\n");
            break;
        case 'S':
            printf("Please enter the pid of the process you want to send a message to\n");
            scanf("%d", &pid_input);
            if(kill(pid_input) == -1) {
                printf("failure\n");
            }
            else {
                printf("succes\n");
            }
            // Need to put process into waiting reply queue
            printf("scheduling info\n");
            break;
    } 

    bool readyListEmpty(){
        if((ready_lists[0]) == 0 && List_count(ready_lists[1]) == 0 && List_count(ready_lists[2]) == 0) {
            return true;
        }
        else {
            return false;
        }
    }
}
