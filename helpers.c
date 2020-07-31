/*! \file helpers.c
The helpers file for the exmallib project. It contains the implementation of all the helper functions used by exmalloc, exrealloc, etc, along with various linked list traversal and convinence functions. 

Helper functions always assume that the sizes they pass are aligned. It is the job of exmalloc, etc. to pass their helpers aligned sizes.
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
    \brief Gets a free block which is larger than or equal to \p alignedSize in the global linked list of blockInfo's, if one exists.
    \details The function scans the linked list for free blocks. If a free block is found and is large enough, it is returned. If it is not large enough but the next block is free too, they are both merged and then checked again. This is done recursively until a block is found or the end of the linked list is reached.
    \param alignedSize The minimum size of a free block sought. It is ASSUMED to be aligned.
    \returns A pointer to the blockInfo of a free block with size greater than or equal to \p alignedSize.
    \returns NULL, if no such block if found. 
*/
blockInfo* getFreeBlock(size_t alignedSize) {
    blockInfo* curr = baseOfBlockLL;
    while (curr != NULL) {
        if (curr->free == 1) {
            if (curr->size >= alignedSize) {
                // A large enough free block has been found. Return it.
                break;
            } else if (curr->next && curr->next->free == 1) {
                // This block isn't big enough but the neighbour is free too. Thus, they can be merged. Point the current block to neighbour's neighbour, then overwrite the ex-neighbour.
                curr->size += curr->next->size + BLOCKINFOSIZE;
                curr->next = curr->next->next;
                // The next-next neighbour might also be free, so call the function recursively to check. Probably overkill lmao
                return getFreeBlock(alignedSize);
            }
        }
        curr = curr->next;
    }
    return curr;
}


/*! 
    \brief Moves the system break point up by \p alignedSize units.
    \details This function can be seen as a naive way of getting more memory from the OS, although memory allocaed this way cannot be freed/reused later.
    \param alignedSize The number of bytes by which to move the system break point. It is ASSUMED to be aligned.
*/
void* getMemoryFromOS(size_t alignedSize) {
    void* p = sbrk(0);
    int paddingRequired = (long)p % ALIGNTO;
    if (paddingRequired != 0) {
        // The cuurrent top of the heap is misaligned. Thus, we move the top of the heap into alignment and then start allocating more memory. Ideally this should only be a one-time thing.
        sbrk(paddingRequired);
    }
    // Since size is assumed to be aligned, it is passed directly. The system break point will thus remain aligned after being moved.
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
        printf("blockInfo@    %d\n", curr);
        printf("Block size    %d\n", curr->size);
        printf("Block next    %d\n", curr->next);
        printf("Block free?   %s\n", (curr->free == 0) ? "No" : "Yes");
        printf("Actual mem@   %d\n\n", curr + 1);

        curr = curr->next;
    }
}


/*! 
    \brief Splits the block of memory at \p ptrToMem into two parts, with the first part being \p alignedSize bytes large.
    \details This function implements "shrinking" of a block, in a sense. The block of memory at \p ptrToMem is split into two. The first block remains in place, but shrunk down to \p alignedSize bytes. A new blockInfo struct is made for the second block, and it is marked free. IMPORTANTLY, this new split-off block is added directly after the shrunked one, to preserve memory contiguity. 
    \param ptrToMem A pointer to a block of allocated memory.
    \param alignedSize The size, in bytes, to which to shrink the memory block down to. It is ASSUMED to be aligned.
    \returns NULL
*/
void splitBlock(void* ptrToMem, size_t alignedSize) {
    if (!ptrToMem) { return; }

    blockInfo* block = memPtrToBlockInfoPtr(ptrToMem);
    if (block->size - alignedSize <= BLOCKINFOSIZE) {
        // The new memory block requires BLOCKINFOSIZE bytes to store the blockInfo struct. If the new size leaves less than or equal to BLOCKINFOSIZE bytes free, it can't be split.
        return;
    }

    // Create the new block at the appropriate location. Mark it free and set it's next parameter to the immediate next block in the linked list.
    blockInfo* newBlock = ptrToMem + alignedSize;
    newBlock->size = block->size - alignedSize - BLOCKINFOSIZE;
    newBlock->next = block->next;
    newBlock->free = 1;

    // Now, we can update the blockInfo. It will now point to the new block just created, which also appears ahead of it in memory.
    block->size = alignedSize;
    block->next = newBlock;
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


/*! 
    \brief Rounds up \p size to the nearest multiple of \p ALIGNTO.
    \param size The size in bytes to be rounded up.
    \returns THe smallest multiple of ALIGNTO larger than or equal to \p size
*/
size_t alignSize(size_t size) {
    return ALIGNTO * ceil((double)size / ALIGNTO);
}