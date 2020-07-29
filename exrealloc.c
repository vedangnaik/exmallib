#include "exmallib.h"

void* exrealloc(void* ptrToMem, size_t newSize) {
    if (ptrToMem == NULL) { return exmalloc(newSize); }
    if (newSize <= 0) { return NULL; }
    
    blockInfo* block = memPtrToBlockInfoPtr(ptrToMem);
    if (block->size >= newSize) {
        // We will do nothing right now. Just return the current pointer.
        // Actually freeing up memory will be implemented later.
        return ptrToMem;
    } else {
        // We will have to make a new block and copy the data in this one.
        // Mark the current block as free and exmalloc a new one, then memcpy  
        void* newPtrToMem = exmalloc(newSize);
        if (newPtrToMem == NULL) { return NULL; }
        memcpy(newPtrToMem, ptrToMem, block->size);
        block->free = 1;
        return newPtrToMem;
    }
}