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

NOTE: 
*/
void* getMemoryFromOS(size_t size) {
    void* p = sbrk(0);
    int paddingRequired = (long)p % ALIGNTO;
    if (paddingRequired != 0) {
        // The cuurrent top of the heap is misaligned. Thus, we move the top of the heap into alignment and then start allocating more memory
        sbrk(paddingRequired);
    }
    // The top of the heap is now aligned. We can go ahead and allocate the memory. 'size' is rounded up a multiple of ALIGNTO in order to not waste space; since the system break point will always be aligned, 1-7 bytes in between blocks will be wasted and inaccessible otherwise. 
    size_t alignedSize = ALIGNTO * ceil((double)size / ALIGNTO);
    void* request = sbrk(alignedSize);
    if (request == (void*) -1) {
        // sbrk failed, oops
        return NULL;
    } else {
        // Great, you got some more memory. Return the pointer to it.
        return request;
    }
}


/*
A helper function to print out each node of the blockInfo linked list. Useful for priliminary debugging and visualization of memory allocation.
*/
void printBlockInfoLL() {
    blockInfo* curr = baseOfBlockLL;
    while (curr) {
        printf("blockInfo@    %p\n", curr);
        printf("Block size    %d\n", curr->size);
        printf("Block next    %p\n", curr->next);
        printf("Block free?   %s\n", (curr->free == 0) ? "No" : "Yes");
        printf("Actual mem@   %p\n\n", curr + 1);

        curr = curr->next;
    }
}