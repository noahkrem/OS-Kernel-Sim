
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

// Create a process and put it on the appropriate ready queue
// Reports: success or failure, the pid of created process on success
int create(int priority);

int fork();

#endif