#include "exmallib.h"

void* baseOfBlockLL = NULL;

// This function scans the linked list of blockInfo's
// to see if any of them are free and bigger than size.
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

// This function gets the OS to allocate the process size
// units more memory by using the sbrk syscall.
void* getMemoryFromOS(size_t size) {
    void* p = sbrk(0);
    // Ask for BLOCKINFOSIZE more memory since you need
    // to store the blockInfo struct here too.
    void* request = sbrk(size);
    if (request == (void*) -1) {
        // sbrk failed, oops
        return NULL;
    } else {
        // Great, you got some more memory. Return the pointer
        // to it.
        return request;
    }
}

// The actual exmalloc function which does memory allocation.
void* exmalloc(size_t size) {
    if (size <= 0) { return NULL; }

    if (!baseOfBlockLL) {
        // Welp, no base node. create a new block here and instantiate
        // the base. Make sure to ask for enough memory to store
        // the blockInfo struct for this block.
        void* ptrToNewMem = getMemoryFromOS(size + BLOCKINFOSIZE);
        if (ptrToNewMem == NULL) {
            // Whoops, sbrk failed.
            return NULL;
        } else {
            // Great, you got your memory. Create the blockInfo
            // struct for it and put it at the location returned.
            // Make this block the base.
            blockInfo* base = ptrToNewMem;
            base->size = size;
            base->next = NULL;
            base->free = 0;
            baseOfBlockLL = base;

            // return the pointer + the size of the blockInfo. Otherwise
            // the blockInfo will get overwritten by the user.
            return ptrToNewMem + BLOCKINFOSIZE;
        }
    } else {
        // Great, some blocks have been requested before
        // check them first to see if you can find any free ones
        blockInfo* usableBlock = findFreeBlock(size);
        if (usableBlock) {
            // great, you found a block which is big enough. 
            // give it's address to the user and update its parameters
            usableBlock->size = size;
            usableBlock->free = 0;
            return usableBlock + 1;
        } else {
            // welp, no free block is big enough. Hence, get some
            // more memory, make a new one, and stick it to the end.
            void* ptrToNewMem = getMemoryFromOS(size);
            if (ptrToNewMem == NULL) {
                // whoops, sbrk failed
                return NULL;
            } else {
                // great, got a new block
                blockInfo* newBlock = ptrToNewMem - BLOCKINFOSIZE;
                newBlock->size = size;
                newBlock->next = NULL;
                newBlock->free = 0;
                // traverse the linked list to get to the end and add
                // this guy to the end
                blockInfo* curr = baseOfBlockLL;
                while (curr->next) {
                    curr = curr->next;
                }
                curr->next = newBlock;
                
                return ptrToNewMem;
            }
        }
    }
}