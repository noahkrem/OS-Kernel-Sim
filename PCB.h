
#ifndef _PCB_H_
#define _PCB_H_
#include <stdbool.h>

/*
#define LIST_SUCCESS 0
#define LIST_FAIL -1
*/

/*
enum ListOutOfBounds {
    LIST_OOB_START,
    LIST_OOB_END
};
*/

typedef struct PCB_s PCB;
struct PCB_s{

};

// Create a process and put it on the appropriate ready queue.
// Reports: success or failure, the pid of created process on success.
int create(int priority);

// Copy the currently running process and put it on the ready Q corresponding to the
// original process' priority. Attempting to Fork the "init" process (see below) should fail.
// Reports: Success or failure, the pid of the resulting process on success.
int fork();

// Kill the named process and remove it from the system.
// Reports: Action taken as well as success or failure.
int kill(int pid);

// Kill the currently running process.
// Reports: Process scheduling information (which process now gets control of the cpu).
void exit();

// 

#endif