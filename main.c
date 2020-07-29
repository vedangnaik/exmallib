#include "exmallib.h"

void main() {
    size_t old = (size_t)sbrk(0);
    int* a = exmalloc(10 * sizeof(int));
    a[5] = 24;
    size_t new = (size_t)sbrk(0);
    int* b = exrealloc(a, 20 * sizeof(int));
    size_t re = (size_t)sbrk(0);
    printf("Old system break point: %d\n", old);
    printf("New system break point: %d\n", new);
    printf("System break point after realloc: %d\n", re);
    printf("Difference between system breaks: %d\n", re-old);
    printf("Data preserved: %d\n", b[5]);
}