
#include "PCB.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

static PCB* CURRENT = NULL;
static unsigned int PID_CURR = 0;

static sem_t sem_array[NUM_SEMAPHORE]; 
static List * ready_lists[NUM_READY_LIST];
static List * waiting_lists[NUM_WAITING_LIST];

// Create a process and put it on the appropriate ready queue.
// Reports: success or failure, the pid of created process on success.
// NOTE: MAY WANT TO DO EXTRA ERROR CHECKING FOR LIST APPEND
int create(int priority) {

    // Check that the given priority is valid
    if (priority < 0 || priority > 2) {
        return -1;
    }

    PCB *newPCB = malloc(sizeof(PCB));
    // If allocation fails
    if (newPCB == NULL) {
        return -1;
    }

    // Set member variables
    newPCB->pid = PID_CURR;
    PID_CURR++;
    newPCB->priority = priority;

    // If there are no processes currently running
    if (CURRENT == NULL) {
        newPCB->state = RUNNING;
        CURRENT = newPCB;
    }
    // If the currently running process is lower priority than the new process
    else if (CURRENT->priority > newPCB->priority || CURRENT->pid == 0) {
        newPCB->state = RUNNING;
        List_append(ready_lists[CURRENT->priority], CURRENT);
        CURRENT = newPCB;
    }
    // If the currently running process is higher priority than the new process
    else {
        newPCB->state = READY;
        List_append(ready_lists[newPCB->priority], newPCB);
    }

    // Return pid of the created process
    return newPCB->pid;
}

// Copy the currently running process and put it on the ready Q corresponding to the
// original process' priority. Attempting to Fork the "init" process (see below) should fail.
// Reports: Success or failure, the pid of the resulting process on success.
// Returns -1 on failure, the pid on success
int fork() {
    if(CURRENT == NULL || CURRENT->pid == 0) {
        return -1;
    }
    PCB *newPCB = malloc(sizeof(PCB));
    newPCB->pid = PID_CURR;
    PID_CURR++;
    newPCB->priority = CURRENT->priority;

   if(List_append(ready_lists[newPCB->priority], newPCB) == -1) {
        PID_CURR--;
        free(newPCB);
        return -1;   
   }

   return newPCB->pid;  
}

// Kill the named process and remove it from the system.
// Reports: Action taken as well as success or failure.
// NOTE: MAY WANT TO DO MORE TESTING ON FREEPROCESS
int kill(int pid) {

    PCB *toKill = findProcess(pid);

    if (toKill != NULL) {
        freeProcess(toKill);
        printf("Process %i killed\n", pid);
        return 1;
    }

    // If we reach this line, process removal has failed
    return -1;
}

// Kill the currently running process.
// Reports: Process scheduling information (which process now gets control of the cpu).
void exit_proc() {

}

// Time quantum of the running process expires.
// Reports: Action taken (process scheduling information).
void quantum() {
    if(CURRENT == NULL) {
        return;
    }
    printf("\n");
    printf("Process %d's time quantum has expired\n", CURRENT->pid);
    PCB* temp = CURRENT;
    CURRENT = nextProcess();

    if(CURRENT == NULL) {
        CURRENT = temp;
        return;
    }

    if(CURRENT->pid == 0) {
        List_append(ready_lists[CURRENT->priority], CURRENT);
        CURRENT = temp;
    }
    else {
        CURRENT->state = RUNNING;
        temp->state = READY;
        List_append(ready_lists[temp->priority], temp);
    }

    printf("Current process is now: %d\n", CURRENT->pid);
    printf("\n");

    printf("Highest Priority Processes: \n");
    traverseList(ready_lists[0]);
    printf("\n");
    printf("Normal Priority Processes: \n");
    traverseList(ready_lists[1]);
    printf("\n");
    printf("Loweset Priority Processes: \n");
    traverseList(ready_lists[2]);
    printf("\n");



    
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

    PCB *temp = findProcess(pid);

    if (temp != NULL) {
        printf("---Process Information---\n");
        printf("Process ID:         %i\n", temp->pid);
        printf("Process Priority:   %i\n", temp->priority);
        printf("Process State:      ");
        if (temp->state == RUNNING) {
            printf("RUNNING\n");
        } else if (temp->state == READY) {
            printf("READY\n");
        } else {
            printf("BLOCKED\n");
        }
    }
    else {
        printf("Error: Process not found\n");
    }
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

bool readyListEmpty() {
    if((ready_lists[0]) == 0 && List_count(ready_lists[1]) == 0 && List_count(ready_lists[2]) == 0) {
        return true;
    }
    else {
        return false;
    }
}

// Initialize all lists
void initProgram(List * readyTop, List * readyNorm, List * readyLow, List * waitingSend, List * waitingReceive) {

    ready_lists[0] = readyTop;
    ready_lists[1] = readyNorm;
    ready_lists[2] = readyLow;

    waiting_lists[0] = waitingSend;
    waiting_lists[1] = waitingReceive;

    create(2);

    while(1) {
        checkInput();
    }
}

static void checkInput() {
    char input;
    int int_input;
    int rv;
    scanf("%c", &input);
    switch (input) {
        case 'C':
            printf("Please enter a priority number for the process (0 = high, 1 = norm, 2 = low)\n");
            scanf("%d", &int_input);
            if(create(int_input) == -1) {
                printf("failure\n");
            }
            else {
                printf("success\n");
            }
            break;
        case 'F':
            rv = fork();
            if(rv == -1) {
                printf("failure\n");
            }
            else {
                printf("success\n");
                printf("PID: %d\n", rv);
            }
            break;
        case 'K':
            printf("Please enter the pid of the process you want to delete\n");
            scanf("%d", &int_input);
            if(kill(int_input) == -1) {
                printf("failure\n");
            }
            else {
                printf("success\n");
            }
            break;
        case 'E':
            exit_proc();
            printf("report info\n");
            break;
        case 'Q':
            quantum();
            break;
        case 'S':
            printf("Please enter the pid of the process you want to send a message to\n");
            scanf("%d", &int_input);
            if(kill(int_input) == -1) {
                printf("failure\n");
            }
            else {
                printf("success\n");
            }
            // Need to put process into waiting reply queue
            printf("scheduling info\n");
            break;
        case 'R':
            receive();
            // Put into waiting queue
            break;
        case 'Y':
            printf("Please enter the pid to reply to\n");
            scanf("%d", &int_input);
            // unblock sender
            if(reply(int_input, CURRENT->reply_msg) == -1) {
                printf("failure\n");
            }
            else {
                printf("success\n");
            }
            break;
        case 'N':
            printf("Please enter the semaphore ID of the new semaphore\n");
            scanf("%d", &int_input);
            // need to figure out number
            if(new_Sem(int_input, 0) == -1) {
                printf("failure\n");
            }
            else {
                printf("success\n");
            }
            break;
        case 'P':
            printf("Please enter the ID of the semaphore\n");
            scanf("%d", &int_input);
            if(sem_P(int_input) == -1) {
                printf("failure\n");
            }
            else {
                printf("success\n");
            }
            break;
        case 'V':
            printf("Please enter the ID of the semaphore\n");
            scanf("%d", &int_input);
            if(sem_V(int_input) == -1) {
                printf("failure\n");
            }
            else {
                printf("success\n");
            }
            break;
        case 'I':
            printf("Please enter the pid of the process\n");
            scanf("%d", &int_input);
            procinfo(int_input);
        case 'T':
            totalinfo();
            break;
    } 

}

// Search a list for a specific pid
// NOTE: NOT ENTIRELY SURE IF THE COMPARISON WILL WORK
static bool pComparator(void * process, void * pComparisonArg) {

    PCB *processPointer = (PCB *)process;
    unsigned int intPointer = (uintptr_t)pComparisonArg;

    if (processPointer->pid == intPointer) {
        return true;
    }
    
    return false;
}

// Free a process control block
static void freeProcess(PCB *process) {
    
    process->send_msg = NULL;
    process->reply_msg = NULL;
    free(process);
    
    process = NULL;
}

static void traverseList(List * pList) {
    if(List_count(pList) == 0) {
        return;
    }
    List_first(pList);
    PCB *curr = (PCB*)List_curr(pList);
    printf("PID: %d\n", curr->pid);
    if(List_next(pList) != NULL) {
        PCB *curr = (PCB*)List_curr(pList);
        printf("PID: %d\n", curr->pid);
    }
}

static PCB* nextProcess() {
    if(List_count(ready_lists[0]) != 0) {
        return dequeue(ready_lists[0]);
    }
    else if(List_count(ready_lists[1]) != 0) {
        return dequeue(ready_lists[1]);
    }
    else if(List_count(ready_lists[2]) != 0) {
        return dequeue(ready_lists[2]);
    }
    else {
        return NULL;
    }
}

// Search the relevant queues for the given pid
static PCB *findProcess(int pid) {

    PCB *temp = NULL;
    
    temp = List_search(ready_lists[0], pComparator, &pid);
    if (temp != NULL) {
        return temp;
    } 

    temp = List_search(ready_lists[1], pComparator, &pid);
    if (temp != NULL) {
        return temp;
    }

    temp = List_search(ready_lists[2], pComparator, &pid);
    if (temp != NULL) {
        return temp;
    }

    temp = List_search(waiting_lists[0], pComparator, &pid);
    if (temp != NULL) {
        return temp;
    }

    temp = List_search(waiting_lists[1], pComparator, &pid);
    if (temp != NULL) {
        return temp;
    }

    // If we reach this line, process with the given pid was not found
    return NULL;
}
