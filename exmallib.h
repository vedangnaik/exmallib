#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

// blockInfo struct and defined size
typedef struct blockInfo {
    size_t size;
    struct blockInfo* next;
    int free;
} blockInfo;
#define BLOCKINFOSIZE sizeof(blockInfo)

// global base of blockInfo linked list
extern void* baseOfBlockLL;

// exmalloc (malloc) and helper functions
blockInfo* findFreeBlock(size_t size);
void* getMemoryFromOS(size_t size);
void* exmalloc(size_t size);

// dealloc (free)
void dealloc(void* ptrToMem);