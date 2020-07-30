/*! \file helpers.c
The helpers file for the exmallib project. It contains the implementation of all the helper functions used by exmalloc, exrealloc, etc, along with various linked list traversal and convinence functions.
*/


#include "exmallib.h"


/*! 
    \brief "Converts" a pointer to memory created by exmalloc et al. to a pointer to the blockInfo of that block. 
    \details To do so, the size of a blockInfo struct is subtracted from the ptrToMem; this works since exmalloc et al. store the blockInfo for a block direcly behind the pointer they return.
    \param ptrToMem A pointer to a block of allocated memory
    \returns A pointer to the blockInfo of the memory pointed to by \p ptrToMem
*/
blockInfo* memPtrToBlockInfoPtr(void* ptrToMem) {
    return ptrToMem - BLOCKINFOSIZE;
}


/*! 
    \brief Finds a free block which is larger than or equal to \p size in the global linked list of blockInfo's.
    \param size The minimum size of a free block sought.
    \returns A pointer to the blockInfo of a free block with size greater than or equal to \p size.
    \returns NULL, if no such block if found. 
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


/*! 
    \brief Moves the system break point up by \p size units.
    \details This function can be seen as a naive way of getting more memory from the OS, although memory allocaed this way cannot be freed/reused later.
    \param size The number of bytes by which to move the system break point.
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


/*! 
    \brief A helper function which prints out each node of the blockInfo linked list.
    \details This function is useful for preliminary debugging and visualization of the memory allocation process.
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


/*! 
    \brief Splits the block of memory at \p ptrToMem into two parts, with the first part being \p size bytes large.
    \details This function implements "shrinking" of a block, in a sense. The block of memory at \p ptrToMem is split into two. The first block remains in place, but shrunk down to \p size bytes. A new blockInfo struct is made for the second block, and it is marked free. 
    \param ptrToMem A pointer to a block of allocated memory.
    \param size The size, in bytes, to which to shrink the memory block down to.
    \returns The same pointer \p ptrToMem which it was passed, with parameters now updated.
    \returns NULL, if the size of the concerned memory block minus \p size is less than or equal to BLOCKINFOSIZE.
*/
void* splitBlock(void* ptrToMem, size_t size) {
    if (!ptrToMem) { return NULL; }

    blockInfo* block = memPtrToBlockInfoPtr(ptrToMem);
    if (block->size - size <= BLOCKINFOSIZE) {
        // The new memory block requires BLOCKINFOSIZE bytes to store the blockInfo struct. If the new size leaves less than or equal to BLOCKINFOSIZE bytes free later, it can't be split.
        return NULL;
    }

    // Create the new block at the appropriate location and add it to the end of the linked list
    blockInfo* newBlock = ptrToMem + BLOCKINFOSIZE + size;
    newBlock->size = block->size - size;
    newBlock->next = NULL;
    newBlock->free = 1;
    blockInfo* lastBlock = getLastLLNode();
    lastBlock->next = newBlock;

    // Now, we can resize the old block and update the info in blockInfo
    block->size = size;

    return ptrToMem;
}


/*! 
    \brief Gets the last node of the blockInfo linked list.
    \returns A pointer to the last node of the blockInfo linked list.
*/
blockInfo* getLastLLNode() {
    blockInfo* curr = baseOfBlockLL;
    while (curr->next) { curr = curr->next; }
    return curr;
}