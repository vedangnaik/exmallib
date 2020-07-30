#include "exmallib.h"

void* baseOfBlockLL = NULL;


/*
exmalloc is my implementation of malloc. It uses the two helpers functions findFreeBlock and getMemoryFromOS. 
*/
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
            void* ptrToNewMem = getMemoryFromOS(size + BLOCKINFOSIZE);
            if (ptrToNewMem == NULL) {
                // whoops, sbrk failed
                return NULL;
            } else {
                // great, got a new block
                blockInfo* newBlock = ptrToNewMem;
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


/*
exfree is my implementation of free. Given a pointer to the block of memory that needs freedom, it first uses the helper memPtrToBlockInfoPtr to get the address of blockInfo of the block. It then sets the 'free' parameter of the blockInfo to 1.
*/
void exfree(void* ptrToMem) {
    if (!ptrToMem) {
        // Welp, someone gave you an invalid pointer
        return;
    } else {
        blockInfo* blockInfoOfMem = memPtrToBlockInfoPtr(ptrToMem);
        blockInfoOfMem->free = 1;
    }
}


/*
exrealloc is my implementation of realloc. It currently works quite naively: if the block is being shrunk, then nothing is done (lmao). If not, a new block with the requested amount of space is created and everything is copied over. It can be made more efficient. TODO
*/
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


/*
excalloc is my implementation of calloc. Unlike the spec, however, excalloc does not do any checking for overflows or anything. TODO
*/
void* excalloc(unsigned int numObjs, size_t sizeOfObjs) {
    size_t size = numObjs * sizeOfObjs;
    void* ptrToMem = exmalloc(size);
    memset(ptrToMem, 0, size);
    return ptrToMem;
}