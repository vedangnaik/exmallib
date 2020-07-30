#include "exmallib.h"

void main() {
    size_t old = (size_t)sbrk(0);
    int* a = exmalloc(1 * sizeof(char));
    size_t new = (size_t)sbrk(0);

    printf("Old system break point: %d\n", old);
    printf("New system break point: %d\n", new);
    printf("Difference between system breaks: %d\n", new-old);
}