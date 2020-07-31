/*! \file exmallib.c
The main implementation file for the exmallib project. It contains the implementations of exmalloc, exrealloc, exfree, etc. These functions will ensure that aligned chunks of memory are being asked for before invoking any helpers functions.
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
    
    size_t alignedSize = alignSize(size);
    if (!baseOfBlockLL) {
        // Welp, no base node. Create a new block here and instantiate the base. Make sure to ask for enough memory to store the blockInfo struct for this block.
        void* ptrToNewMem = getMemoryFromOS(alignedSize + BLOCKINFOSIZE);
        if (ptrToNewMem == NULL) { return NULL; }
        
        // Great, you got your memory. Create the blockInfo struct for it and put it at the location returned. Make this block the base.
        blockInfo* base = ptrToNewMem;
        base->size = alignedSize;
        base->next = NULL;
        base->free = 0;
        baseOfBlockLL = base;

        // Return the pointer + the size of the blockInfo. Otherwise the blockInfo will get overwritten by the user.
        return ptrToNewMem + BLOCKINFOSIZE;

    } else {
        // Great, some blocks have been requested before. Check them first to see if you can find any free ones.
        blockInfo* usableBlock = getFreeBlock(alignedSize);
        if (usableBlock) {
            // Great, you found a block which is big enough. Split it down to size first, then mark it not free. exrealloc can be used here, but that would likely be inefficient due to the addition of a stack frame.
            usableBlock->free = 0;
            splitBlock(usableBlock + 1, alignedSize);
            return usableBlock;

        } else {
            // Welp, no free block is big enough. Hence, get some more memory, make a new one, and stick it to the end.
            void* ptrToNewMem = getMemoryFromOS(alignedSize + BLOCKINFOSIZE);
            if (ptrToNewMem == NULL) { return NULL; }

            // Great, got a new block
            blockInfo* newBlock = ptrToNewMem;
            newBlock->size = alignedSize;
            newBlock->next = NULL;
            newBlock->free = 0;
            // Add this blockInfo to the end of the linked list
            blockInfo* lastBlock = getLastLLNode();
            lastBlock->next = newBlock;
            
            return ptrToNewMem + BLOCKINFOSIZE;
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
    \details exrealloc aligns \p newSize before splitting the block of memory. This 
    \param ptrToMem A pointer to a block of allocated memory.
    \param newSize The new size in bytes to which to resize the memory block pointed to by \p ptrToMem   
    \returns A pointer to the start of the resized block of memory.
*/
void* exrealloc(void* ptrToMem, size_t newSize) {
    if (ptrToMem == NULL) { return exmalloc(newSize); }
    if (newSize <= 0) { return NULL; }
    
    blockInfo* block = memPtrToBlockInfoPtr(ptrToMem);
    size_t alignedNewSize = alignSize(newSize);
    if (block->size > alignedNewSize) {
        // The actual return value of splitBlock is ignored, since it doesn't matter whether it failed or not; there's more than or equal to alignedNewSize bytes of memory anyway. 
        splitBlock(ptrToMem, alignedNewSize);
        return ptrToMem;
    }
    else if (block->size == alignedNewSize) {
        // Since all block sizes and alignedNewSize are multiples of ALIGNTO, we can bypass an equals-to case immediately here, instead of having a stack frame for split be called and wasting time. 
        return ptrToMem;
    } else {
        // We will have to make a new block and copy the data in this one. Mark the current block as free and exmalloc a new one, then memcpy. exmalloc can be passed alignedNewSize or newSize; it will do the alignement again anyway. It's possible that an already-aligned value will be faster, but idk rn.
        void* ptrToNewMem = exmalloc(alignedNewSize);
        if (ptrToNewMem == NULL) { return NULL; }
        memcpy(ptrToNewMem, ptrToMem, block->size);
        block->free = 1;
        return ptrToNewMem;
    }
}


/*! 
    \brief excalloc is my implementation of calloc.
    \param numObjs The number of objects to allocate.
    \param sizeOfObjs The size of each object.
    \returns A pointer to the start of the block of zero'd out memory.
*/
void* excalloc(unsigned int numObjs, size_t sizeOfObjs) {
    size_t size = numObjs * sizeOfObjs;
    void* ptrToMem = exmalloc(size);
    memset(ptrToMem, 0, size);
    return ptrToMem;
}