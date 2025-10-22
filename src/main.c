/*

Filename: main.c

Author(s): Noah Kremler & Hugo Kwon

Description: Please see the "Other Info" section in our README for more information on how to run this code. 

*/


#include "List.h"
#include "PCB.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>



int main(int argc, char *argv[]) {
    
    List* ready_top = List_create();
    List* ready_norm = List_create();
    List* ready_low = List_create();
    List* ready_init = List_create();
    List* waiting_send = List_create();
    List* waiting_receive = List_create();

    initProgram(ready_top, ready_norm, ready_low, ready_init, waiting_send, waiting_receive);

    return 0;    
}