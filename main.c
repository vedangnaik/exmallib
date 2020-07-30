/*! \file main.c
The main testing file for the exmallib project. It contains the main function.
*/


#include "exmallib.h"



void main() {
    size_t old = (size_t)sbrk(0);
    int* a = exmalloc(80 * sizeof(char));
    exfree(a);
    int* b = exmalloc(20 * sizeof(char));
    printBlockInfoLL();
    size_t new = (size_t)sbrk(0);

    printf("Old system break point: %d\n", old);
    printf("New system break point: %d\n", new);
    printf("Difference between system breaks: %d\n", new-old);
}