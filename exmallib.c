/*! \file exmallib.c
The main implementation file for the exmallib project. It contains the implementations of exmalloc, exrealloc, exfree, etc.
*/


#include "exmallib.h"


void* baseOfBlockLL = NULL;


/*! 
    \brief exmalloc is my implementation of malloc.
    \details 
    \param size The size in bytes of memory to be allocated. 
    \returns A pointer to the start of the block of allocated memory.
*/
void* exmalloc(size_t size) {
    if (size <= 0) { return NULL; }

    if (!baseOfBlockLL) {
        // Welp, no base node. Create a new block here and instantiate the base. Make sure to ask for enough memory to store the blockInfo struct for this block.
        void* ptrToNewMem = getMemoryFromOS(size + BLOCKINFOSIZE);
        if (ptrToNewMem == NULL) { return NULL; }
        
        // Great, you got your memory. Create the blockInfo struct for it and put it at the location returned. Make this block the base.
        blockInfo* base = ptrToNewMem;
        base->size = size;
        base->next = NULL;
        base->free = 0;
        baseOfBlockLL = base;

        // Return the pointer + the size of the blockInfo. Otherwise the blockInfo will get overwritten by the user.
        return ptrToNewMem + BLOCKINFOSIZE;

    } else {
        // Great, some blocks have been requested before. Check them first to see if you can find any free ones.
        blockInfo* usableBlock = findFreeBlock(size);
        if (usableBlock) {
            // Great, you found a block which is big enough. Split it down to size first, then mark it not free.
            usableBlock->free = 0;
            void* ptrToMem = splitBlock(usableBlock + 1, size);
            return ptrToMem;

        } else {
            // Welp, no free block is big enough. Hence, get some more memory, make a new one, and stick it to the end.
            void* ptrToNewMem = getMemoryFromOS(size + BLOCKINFOSIZE);
            if (ptrToNewMem == NULL) { return NULL; }

            // Great, got a new block
            blockInfo* newBlock = ptrToNewMem;
            newBlock->size = size;
            newBlock->next = NULL;
            newBlock->free = 0;
            // Add this blockInfo to the end of the linked list
            blockInfo* lastBlock = getLastLLNode();
            lastBlock->next = newBlock;
            
            return ptrToNewMem;
        }
    }
}


/*! 
    \brief exfree is my implementation of free.
    \details exfree frees the block of memory pointed to my \p ptrToMem by setting the free parameter of it's blockInfo struct to 1.
    \param ptrToMem A pointer to a block of allocated memory.
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


/*! 
    \brief exrealloc is my implementation of realloc.
    \details exrealloc currently works quite naively: if the block is being shrunk, then nothing is done. If not, a new block with the requested amount of space is created and everything is copied over. It can be made more efficient.
    \param ptrToMem A pointer to a block of allocated memory.
    \param newSize The new size in bytes to which to resize the memory block pointed to by \p ptrToMem   
    \returns A pointer to the start of the resized block of memory.
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


/*! 
    \brief excalloc is my implementation of calloc.
    \param numObjs The number of objects to allocate
    \param sizeOfObjs The size of each object
    \returns A pointer to the start of the block of zero'd out memory.
*/
void* excalloc(unsigned int numObjs, size_t sizeOfObjs) {
    size_t size = numObjs * sizeOfObjs;
    void* ptrToMem = exmalloc(size);
    memset(ptrToMem, 0, size);
    return ptrToMem;
}