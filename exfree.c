#include "exmallib.h"

// The free function
void exfree(void* ptrToMem) {
    if (!ptrToMem) {
        // Welp, someone gave you an invalid pointer
        return;
    } else {
        // You got your pointer. Assuming the memory was allocated
        // with exmalloc(), the blockSize struct for this block is 
        // direcly BLOCKINFOSIZE behind you. Hence, use the helper
        // function to get the blockInfo's address
        blockInfo* blockInfoOfMem = memPtrToBlockInfoPtr(ptrToMem);
        blockInfoOfMem->free = 1;
    }
}