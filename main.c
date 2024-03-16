#include "List.h"
#include "PCB.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>



// void* dequeue(List list) {
//     list.List_first();
//     Node tempNode = list.List_remove();
//     return tempNode;
// }

// void enqueue(List list, void* item) {
//     List_append(item);
// }

int main(int argc, char *argv[]) {
    
    List* ready_top = List_create();
    List* ready_norm = List_create();
    List* ready_low = List_create();
    List* ready_init = List_create();
    List* waiting_send = List_create();
    List* waiting_receive = List_create();

    initProgram(ready_top, ready_norm, ready_low, ready_init, waiting_send, waiting_receive);

    printf("\nskipped\n");

    return 0;    
}