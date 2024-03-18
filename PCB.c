
#include "PCB.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

static PCB *CURRENT = NULL;
static PCB *INIT = NULL;
static unsigned int PID_CURR = 0;
static bool initMade = false;
static unsigned int SEM_NUM = 0;

static sem_t sem_array[NUM_SEMAPHORE]; 
static List * ready_lists[NUM_READY_LIST];      // 0 - high priority, 1 - normal priority, 2 - low priority
static List * waiting_lists[NUM_WAITING_LIST];  // 0 - waiting for send, 1 - waiting for reply

// Create a process and put it on the appropriate ready queue.
// Reports: success or failure, the pid of created process on success.
int create(int priority) {

    // Check that the given priority is valid
    if (priority < 0 || priority > 2 && initMade == true) {
        return -1;
    }

    PCB *newPCB = malloc(sizeof(PCB));
    // If allocation fails
    if (newPCB == NULL) {
        printf("Error: Memory allocation failed\n");
        return -1;
    }

    // Set member variables
    newPCB->pid = PID_CURR;
    PID_CURR++;
    newPCB->priority = priority;
    newPCB->waitState = 2;
    newPCB->msg_src = -1;

    // If there are no processes currently running
    if (CURRENT == NULL) {
        newPCB->state = RUNNING;
        CURRENT = newPCB;
    }
    // If the currently running process is the init process
    else if (CURRENT->priority == 3) {
        INIT->state = READY;
        newPCB->state = RUNNING;
        if(List_append(ready_lists[CURRENT->priority], CURRENT) != -1) {
            CURRENT = newPCB;
        }
        else {
            return -1;
        }
    }
    else {
        newPCB->state = READY;
        if(List_append(ready_lists[newPCB->priority], newPCB) == -1) {
            return -1;
        }
    }

    // Return pid of the created process
    return newPCB->pid;
}

// Copy the currently running process and put it on the ready Q corresponding to the
// original process' priority. Attempting to Fork the "init" process (see below) should fail.
// Reports: Success or failure, the pid of the resulting process on success.
// Returns -1 on failure, the pid on success
int fork() {
    
    if (CURRENT == NULL || CURRENT->pid == 0) {
        return -1;
    }

    // Create the new process
    PCB *newPCB = malloc(sizeof(PCB));
    newPCB->pid = PID_CURR;
    PID_CURR++;
    newPCB->priority = CURRENT->priority;
    newPCB->state = READY;

    // Enqueue the new process
    if(List_append(ready_lists[newPCB->priority], newPCB) == -1) {
        return -1;
    }
    else {
        return newPCB->pid;  
    }
}

// Kill the named process and remove it from the system.
// Reports: Action taken as well as success or failure.
int kill(int pid) {

    PCB *toKill = NULL;

    if (CURRENT != NULL) {
        if (CURRENT->pid == pid) {
            toKill = CURRENT;
            CURRENT = nextProcess();
            freeProcess(toKill);
            printf("Process %i killed\n", pid);
            return 1;
        }
    }

    toKill = findProcess(pid); // Find the desired process. The list's "current" node is to be removed now

    if (toKill != NULL) {
        // If the process is on a ready queue 
        if (toKill->state == READY) {
            List_remove(ready_lists[toKill->priority]);
            freeProcess(toKill);
        // If the process is on a waiting queue
        } else {
            // If the process is waiting on a send
            if (toKill->waitState == WAITING_SEND) {
                List_remove(waiting_lists[0]);
                freeProcess(toKill);
            }
            if (toKill->waitState == WAITING_RECEIVE) {
                List_remove(waiting_lists[1]);
                freeProcess(toKill);
            }
        }
        printf("Process %i killed\n", pid);
        return 1;
    }

    // If we reach this line, process removal has failed
    printf("Error: PCB not found\n");
    return -1;
}

// Kill the currently running process.
// Reports: Process scheduling information (which process now gets control of the cpu).
void exit_proc() {

    if (CURRENT != NULL) {
        kill(CURRENT->pid);
        printf("--Current process: \n");
        procinfo_helper(CURRENT);
    }
}

// Time quantum of the running process expires.
// Reports: Action taken (process scheduling information).
void quantum() {
    
    if(CURRENT == NULL) {
        return;
    }

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

    printf("Expired process: \n");
    procinfo_helper(temp);

    printf("New current process: \n");
    procinfo_helper(CURRENT);
}

// Send a message to another process, block until reply.
// Reports: success or failure, scheduling information, and reply source and text (once
//  reply arrives).
int send(int pid, char *msg) {
    
    // If we try to send to the currently running process, operation fails
    if (CURRENT->pid == pid) {
        printf("Error: Cannot send to currently running process\n");
        return -1;
    }

    // Look for the target
    PCB* target = findProcess(pid);
    if (target == NULL) {
        printf("Error: PCB not found\n");
        return -1;
    }

    // If the target already has a message queued
    if (target->proc_message != NULL) {
        printf("Error: Target already has a message queued\n");
        return -1;
    }

    // We must not block the current process
    if (target->state != BLOCKED && CURRENT == INIT) {
        printf("Error: Cannot block the init process\n");
        return -1;
    }

    // Check if the receiving process is blocked
    if (target->state == BLOCKED) {
        // Check if the receiving process is waiting for a send
        if (target->waitState == WAITING_SEND) {
            
            // Give the target process the message
            printf("Setting proc_msg...\n"); // Testing...
            target->proc_message = msg;
            target->msg_src = CURRENT->pid;

            List_remove(waiting_lists[1]);  // Remove target process from the waiting queue (it is already waiting_list[1]'s current process)
            if (List_append(ready_lists[target->priority], target) == -1) {
                return -1;
            } 
            // Output necessary reception message
            printf("Message received from process %i\n", target->msg_src);
            printf("Received Message: %s\n", target->proc_message);
            target->state = READY;
            target->msg_src = -1;
            target->proc_message = NULL;

            // If the currently running process is the init process, make the target the newly running process
            if (CURRENT == INIT) {
                dequeue(ready_lists[target->priority]);
                target->state = RUNNING;
                CURRENT = target;
            }

            // Return success
            return 1;
        }

    }
    // If the target process is not blocked, or is waiting for a receive:

    if (target->pid == CURRENT->msg_src) {
        printf("Error: Target process is waiting for a receive from current process\n");
        return -1;
    }

    // Move the current process to waiting list
    CURRENT->state = BLOCKED;
    CURRENT->waitState = WAITING_RECEIVE;
    if(List_append(waiting_lists[0], CURRENT) == -1) {
        return -1;
    }

    // Give the target process the message
    printf("Setting proc_msg...\n"); // Testing...
    target->proc_message = msg;
    target->msg_src = CURRENT->pid;

    printf("Blocking process: \n");
    procinfo_helper(CURRENT);
            
    // Run the next process in the queue
    CURRENT = nextProcess();
    CURRENT->state = RUNNING;

    printf("New current process: \n");
    procinfo_helper(CURRENT);
    
    return 1;
}

// Receive a message, block until one arrives
// Reports: Scheduling information, message text, source of message.
void receive() {

    if (CURRENT == INIT) {
        // We should never block the init process
        if (CURRENT->proc_message == NULL) {
            printf("Error: Cannot block the init process\n");
            return;
        }
    }
    
    // If the new process has a message, print it 
    if(CURRENT->proc_message != NULL) {  
        
        printf("Message received from process %i\n", CURRENT->msg_src);
        printf("Received Message: %s\n", CURRENT->proc_message);

        // Move the source process back onto a ready list
        PCB *src_proc = findProcess(CURRENT->msg_src);
        src_proc->state = READY;
        List_remove(waiting_lists[0]);  // Dequeue sender process. Cannot use dequeue() here
        List_append(ready_lists[src_proc->priority], src_proc);

        // Clear the message information from the current process
        CURRENT->proc_message = NULL;
        CURRENT->msg_src = -1;

        // If the current process is the init process, make the source process the new current
        if (CURRENT == INIT) {
            dequeue(ready_lists[src_proc->priority]);
            src_proc->state = RUNNING;
            CURRENT = src_proc;
        }

        return;
    }
    // If there's no messages to receive, move the process to the waiting list
    else {
        
        // Move current process to the waiting list
        CURRENT->state = BLOCKED;
        CURRENT->waitState = WAITING_SEND;
        List_append(waiting_lists[1], CURRENT);
        printf("Blocking process: \n");
        procinfo_helper(CURRENT);


        // Run the next process in the queue
        CURRENT = nextProcess();
        CURRENT->state = RUNNING;
        printf("New current process: \n");
        procinfo_helper(CURRENT);
        
        return;
    }
}

// Unblocks sender and delivers reply.
// Reports: Success or failure.
int reply(int pid, char *msg) {
    
    // Find the target in a list
    PCB* target = findProcess(pid);
    
    // If we cannot find the given pid, operation fails
    if(target == NULL) {
        printf("Error: PCB not found\n");
        return -1;
    }

    // If the target doesn't currently hold a message, reply with a message
    if(target->proc_message == NULL) {
        
        // printf("Setting proc_msg...\n"); // Testing...
        target->proc_message = msg;

        // If the target was waiting for a reply, remove it from the list
        if(target->state == BLOCKED && target->waitState == WAITING_RECEIVE) {
            target->state = READY;
            if(List_append(ready_lists[target->priority], target) == -1) {
                return -1;
            }
            List_remove(waiting_lists[0]);
        }
        return 1;
    }
    else {
        printf("Error: This PCB already has a message queued \n");
        return -1;
    }
}

// Initialize the named semaphore with the value given. IDs can take a value from 0 to 4. 
//  This can only be done once for a semaphore - subsequent attempts result in error.
// Reports: Action taken as well as success or failure.
int new_Sem(int sem_id, unsigned int init) {

    // Check for valid semaphore ID
    if (sem_id > 4 || sem_id < 0) {
        printf("Error: Not a valid semaphore ID\n");
        return -1;
    }
    // Check that we have not created too many semaphores
    if (SEM_NUM >= NUM_SEMAPHORE) {
        printf("Error: Created too many semaphores\n");
        return -1;
    }
    // Check that we have not already created a semaphore with the given ID
    if (sem_array[sem_id].sem_value != -1) {
        printf("Error: This semaphore has already been created!\n");
        return -1;
    }

    sem_array[sem_id].sem_value = init;
    sem_array[sem_id].pList = List_create();

}

// Execute the semaphore P operation on behalf of the running process. Assume semaphore 
//  IDs to be numbered 0 through 4.
// Reports: Action taken (blocked or not) as well as success or failure.
int sem_P(int sem_id) {
    // Check for valid semaphore ID
    if (sem_id > 4 || sem_id < 0) {
        printf("Error: Not a valid semaphore ID\n");
        return -1;
    }

    // Decrement semaphore value
    sem_array[sem_id].sem_value--;

    // If the semaphore value is less than 0, add the process to the waiting list
    if(sem_array[sem_id].sem_value < 0) {
        if(List_append(sem_array[sem_id].pList, CURRENT) == -1) {
            return -1;
        }
        CURRENT->state = BLOCKED;
        CURRENT = nextProcess();
        CURRENT->state = RUNNING;
    }
}

// Execute the semaphore V operation on behalf of the running process. Assume semaphore 
//  IDs to be numbered 0 through 4.
int sem_V(int sem_id) {
    // Check for valid semaphore ID
    if (sem_id > 4 || sem_id < 0) {
        printf("Error: Not a valid semaphore ID\n");
        return -1;
    }

    // Increment semaphore value
    sem_array[sem_id].sem_value++;

    // If the semaphore value is zero or less, wake up a process from the waiting list
    if(sem_array[sem_id].sem_value <= 0) {
        PCB* temp = (PCB *)dequeue(sem_array[sem_id].pList);
        temp->state = READY;
        if(List_append(ready_lists[temp->priority], temp) == -1) {
            return -1;
        }
    }
}

// Dump complete state information of process to screen.
void procinfo(int pid) {

    printf("---PROCESS INFO---\n");

    PCB *temp = NULL;

    // Either the process is the currently running process, or it is stored in a list
    if (CURRENT == NULL || CURRENT->pid != pid) {
        temp = findProcess(pid);
    } else if (CURRENT->pid == pid) {
        temp = CURRENT;
    }

    if (temp != NULL) {
        procinfo_helper(temp);
    }
    else {
        printf("Error: Process not found\n");
    }
}

// Display all process queues and their contents
void totalinfo() {
    
    printf("---TOTAL INFO---\n");

    // Display the currently running process
    if (CURRENT != NULL) {
        printf("--Current Process:\n");
        procinfo_helper(CURRENT);
    }

    // Display the ready lists
    for (int i = 0; i <= 2; i++) {
        List_first(ready_lists[i]);
        printf("--Ready List %i:\n", i);
        while (ready_lists[i]->current != NULL) {
            // Print process info
            PCB *processPointer = ready_lists[i]->current->item;
            procinfo_helper(processPointer);
            // Advance
            ready_lists[i]->current = ready_lists[i]->current->next;
        }
    }

    // Display the waiting lists
    for (int i = 0; i <= 1; i++) {
        List_first(waiting_lists[i]);
        printf("--Waiting List %i:\n", i);
        while (waiting_lists[i]->current != NULL) {
            // Print process info
            PCB *processPointer = waiting_lists[i]->current->item;
            procinfo_helper(processPointer);
            // Advance
            waiting_lists[i]->current = waiting_lists[i]->current->next;
        }
    }

}


// PRIVATE FUNCTIONS

// Dequeue from list
static void * dequeue(List * list) {
    List_first(list);
    void *ret = List_remove(list);
    return ret;
}

// Initialize all lists
void initProgram(List * readyTop, List * readyNorm, List * readyLow, List * readyInit, List * waitingSend, List * waitingReceive) {

    ready_lists[0] = readyTop;
    ready_lists[1] = readyNorm;
    ready_lists[2] = readyLow;
    ready_lists[3] = readyInit;

    waiting_lists[0] = waitingSend;
    waiting_lists[1] = waitingReceive;

    for (int i = 0; i < 5; i++) {
        sem_array[i].sem_value = -1;    // Set to one, to later check semaphore initialization
    }

    // Initialize the special init process
    INIT = malloc(sizeof(PCB));
    INIT->pid = PID_CURR;
    PID_CURR++;
    INIT->priority = 3;
    INIT->state = RUNNING;
    CURRENT = INIT;
    initMade = true;

    // Start the input loop
    while(1) {
        checkInput();
    }
}

static void checkInput() {
    char input;
    char msg[256];
    int int_input;
    int rv;
    scanf("%c", &input);
    printf("---------------------------------------------------------------------------\n");
    switch (input) {
        case 'C':
            printf("Enter process priority (0 = high, 1 = norm, 2 = low): ");
            scanf("%d", &int_input);
            if (create(int_input) == -1) {
                printf("Failure: Could not create\n");
            }
            else {
                printf("New process ID: %i\n", PID_CURR-1);
                printf("Success: Create complete\n");
            }
            break;
        case 'F':
            rv = fork();
            if(rv == -1) {
                printf("Failure: Could not fork\n");
            }
            else {
                printf("New process ID: %i\n", rv);
                printf("Success: Fork complete\n");
            }
            break;
        case 'K':
            printf("Enter process ID: ");
            scanf("%d", &int_input);
            if (int_input > PID_CURR || int_input < 1) {
                printf("Failure: Invalid input\n");
            } 
            else if (kill(int_input) == -1) {
                printf("Failure: Could not kill\n");
            }
            else {
                printf("Success: Kill complete\n");
            }
            break;
        case 'E':
            exit_proc();
            break;
        case 'Q':
            quantum();
            break;
        case 'S':
            printf("Enter process ID of receiver: ");
            scanf("%d", &int_input);
            printf("Enter a message: ");
            scanf("%s", msg);
            if(send(int_input, msg) == -1) {
                printf("Failure: Could not send\n");
            }
            else {
                printf("Success: Send complete\n");
            }
            break;
        case 'R':
            receive();
            break;
        case 'Y':
            printf("Enter process ID to reply to: ");
            scanf("%d", &int_input);
            printf("Enter message: ");
            scanf("%s", msg);
            // unblock sender
            if (reply(int_input, msg) == -1) {
                printf("Failure: Could not reply\n");
            }
            else {
                printf("Success: Reply complete\n");
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
            break;
        case 'T':
            totalinfo();
            break;
    } 

}

// Free a process control block
static void freeProcess(PCB *process) {
    
    process->proc_message = NULL;
    free(process);
    
    process = NULL;
}

static PCB* nextProcess() {
    
    if (List_count(ready_lists[0]) != 0) {
        return dequeue(ready_lists[0]);
    }
    else if(List_count(ready_lists[1]) != 0) {
        return dequeue(ready_lists[1]);
    }
    else if(List_count(ready_lists[2]) != 0) {
        return dequeue(ready_lists[2]);
    }
    else {
        INIT->state = RUNNING;
        return INIT;
    }
}

// Search the relevant queues for the given pid.
// The queue's current node will now be the desired process.
static PCB* findProcess(int pid) {

    // If we search for the init process
    if (pid == 0) {
        return INIT;
    }

    // Search the ready lists
    for (int i = 0; i <= 2; i++) {
        List_first(ready_lists[i]);
        while (ready_lists[i]->current != NULL) {
            // Check for a match
            PCB *processPointer = ready_lists[i]->current->item;
            if (processPointer->pid == pid)
                return processPointer;
            // If no match, advance
            ready_lists[i]->current = ready_lists[i]->current->next;
        }
        // printf("Match not found in ready list %i...\n", i);  // Testing
    }

    // Search the waiting lists
    for (int i = 0; i <= 1; i++) {
        List_first(waiting_lists[i]);
        while (waiting_lists[i]->current != NULL) {
            // Check for a match
            PCB *processPointer = waiting_lists[i]->current->item;
            if (processPointer->pid == pid)
                return processPointer;
            // If no match, advance
            waiting_lists[i]->current = waiting_lists[i]->current->next;
        }
        // printf("Match not found in waiting list %i...\n", i);    // Testing
    }

    // If we reach this line, process with the given pid was not found
    return NULL;
}

// Helper function to print process information to the screen
void procinfo_helper(PCB *process) {

    printf("    Process ID:         %i\n", process->pid);
    printf("    Process Priority:   %i\n", process->priority);
    printf("    Process State:      ");
    if (process->state == RUNNING) {
        printf("RUNNING\n");
    } else if (process->state == READY) {
        printf("READY\n");
    } else if (process->state == BLOCKED) {
        printf("BLOCKED\n");
    } else {
        printf("BLOCKED\n");
    }
    printf("    Process Message:    %s\n", process->proc_message);  // Testing...
    printf("\n");
}