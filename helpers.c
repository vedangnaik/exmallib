#include "exmallib.h"


/* 
memPtrToBlockInfoPtr "converts" a pointer to memory created by exmalloc et al. to a pointer to the blockInfo of that block. To do this, it subtracts the size of a blockInfo struct from the pointer; this works since exmalloc et al. store the blockInfo for a block direcly behind the pointer they return. 
*/
blockInfo* memPtrToBlockInfoPtr(void* ptrToMem) {
    return ptrToMem - BLOCKINFOSIZE;
}


/* 
findFreeBlock traverses the linked list of blockInfo's created in the past to find one which is free and is attached to memory equal to or greater than 'size'.
*/
blockInfo* findFreeBlock(size_t size) {
    blockInfo* curr = baseOfBlockLL;
    while (curr != NULL) {
        if (curr->size >= size && curr->free == 1) {
            break;
        }
        curr = curr->next;
    }
    return curr;
}


/*
getMemoryFromOS moves the system break point up by 'size' units. It can be seen as a naive way of getting more memory from the OS, although memory allocaed this way cannot be freed/reused later.
*/
void* getMemoryFromOS(size_t size) {
    void* p = sbrk(0);
    void* request = sbrk(size);
    if (request == (void*) -1) {
        // sbrk failed, oops
        return NULL;
    } else {
        // Great, you got some more memory. Return the pointer to it.
        return request;
    }
}