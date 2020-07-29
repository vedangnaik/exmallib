#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void* baseOfBlockLL = NULL;

typedef struct blockInfo {
    size_t size;
    struct blockInfo* next;
    int free;
} blockInfo;
#define BLOCKINFOSIZE sizeof(blockInfo)


blockInfo* findFreeBlock(size_t size) {
    blockInfo* curr = baseOfBlockLL;
    while (curr && (curr->size < size || !curr->free)) {
        curr = curr->next;
    }
    return curr;
}


void* getMemoryFromOS(size_t size) {
    void* p = sbrk(0);
    void* request = sbrk(size + BLOCKINFOSIZE);
    if (request == (void*) -1) {
        // sbrk failed, oops
        return NULL;
    } else {
        // great, you got some more memory. However, give them
        // the pointer one blockSize up since you need to store
        // the blockInfo struct for this block there
        return request + BLOCKINFOSIZE;
    }
}

void* exmalloc(size_t size) {
    if (size <= 0) { return NULL; }

    if (!baseOfBlockLL) {
        // Welp, no base node. create a new block here
        // and instantiate the base
        void* ptrToNewMem = getMemoryFromOS(size);
        if (ptrToNewMem == NULL) {
            // whoops, sbrk failed
            return NULL;
        } else {
            // great, you got your memory. Create the blockInfo
            // struct for it and put it at the location returned,
            // minus the size.
            blockInfo* base = ptrToNewMem - BLOCKINFOSIZE;
            base->size = size;
            base->next = NULL;
            base->free = 0;
            return ptrToNewMem;
        }
    } else {
        // great, some blocks have been requested before
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

void main() {
    int numToAllocate = 10;
    size_t spaceToAllocate = numToAllocate * sizeof(int);
    
    // Code to print out sizes of a blockInfo and it's components
    // separately, just for my peace of mind. Note that they may not
    // add up depending on your machine; the total space is 'padded'
    // in order to ensure 'aligned memory access'.
    // printf("Sizeof blockInfo: %d\n", BLOCKINFOSIZE);
    // printf("Sizeof size var: %d\n", sizeof(size_t));
    // printf("Sizeof next block pointer: %d\n", sizeof(blockInfo*));
    // printf("Sizeof free var: %d\n\n", sizeof(int));

    size_t old = (size_t)sbrk(0);
    int* arr = exmalloc(10 * sizeof(int));
    size_t new = (size_t)sbrk(0);

    printf("Old system break point: %d\n", old);
    printf("New system break point: %d\n", new);
    printf("Sizeof blockInfo + new memory: %d\n", BLOCKINFOSIZE + spaceToAllocate);
    printf("Difference between system breaks: %d\n", new-old);
    printf("Malloc'd location: %d\n", arr);
}