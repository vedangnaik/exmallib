/*! \file main.c
The testing file for the exmallib library. It contains the main function.
*/


#include "exmallib.h"



void main() {
    size_t old = (size_t)sbrk(0);
    int* a = exmalloc(40 * sizeof(char));
    exfree(a);
    int* b = exmalloc(80 * sizeof(char));
    exfree(b);
    int* c = exmalloc(144 * sizeof(char));
    // printBlockInfoLL();
    size_t new = (size_t)sbrk(0);

    printf("Old system break point: %d\n", old);
    printf("New system break point: %d\n", new);
    printf("Difference between system breaks: %d\n", new-old);
}