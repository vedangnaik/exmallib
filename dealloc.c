#include "exmallib.h"

// The free function
void dealloc(void* ptrToMem) {
    if (!ptrToMem) {
        // Welp, someone gave you an invalid pointer
        return;
    } else {
        // You got your pointer. Assuming the memory was allocated
        // with exmalloc(), the blockSize struct for this block is 
        // direcly BLOCKINFOSIZE behind you. Hence, subtract.
        // Now, set the free indicator to true
        blockInfo* blockInfoOfMem = ptrToMem - BLOCKINFOSIZE;
        blockInfoOfMem->free = 1;
    }
}