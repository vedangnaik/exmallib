/*! \file exmallib.h
The main header file for the exmallib project. It contains the declarations of all functions and global variables required by exmalloc, exrealloc, exfree, etc.
*/


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>


#define ALIGNTO 8 /*!< The ALIGNTO constant defines the number of bytes to which exmalloc, exrealloc, etc. align the pointers they return. This is left configurable as different architectures support/require different alignments. */


// blockInfo struct and defined size
typedef struct blockInfo {
    size_t size; /*!< The size in bytes of the memory block.*/
    struct blockInfo* next; /*!< The pointer to the next blockInfo in the linked list of blockInfos.*/
    int free; /*!< Indicates whether the block is free or not.*/
} blockInfo;


#define BLOCKINFOSIZE sizeof(blockInfo) /*!< Quite self-explnatory. The size of a blockInfo struct is stored as a constant since it will never change. */


extern void* baseOfBlockLL; /*!< The global base pointer of the linked list of blockInfos that store the information of all memory blocks that have been exmalloc'd, exrealloc'd, etc. IMPORTANTLY, contiguous nodes in this linked list are also contiguous in memory (like a static array).*/


// implemented in helpers.c
blockInfo* memPtrToBlockInfoPtr(void* ptrToMem);
void printBlockInfoLL();
blockInfo* getLastLLNode();
blockInfo* getFreeBlock(size_t size);
void* getMemoryFromOS(size_t size);
void splitBlock(void* ptrToMem, size_t size);
size_t alignSize(size_t size);


// implemented in exmallib.c
void* exmalloc(size_t size);
void exfree(void* ptrToMem);
void* exrealloc(void* ptrToMem, size_t newSize);
void* excalloc(unsigned int numObjs, size_t sizeOfObjs);