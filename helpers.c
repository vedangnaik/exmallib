#include "exmallib.h"


/* 
    This function "converts" a pointer to memory created by exmalloc et al. to a pointer to the blockInfo of that block. TBH, it just subtracts the size of a blockInfo struct from the pointer, since exmalloc et al. store the blockInfo for a block direcly behind the pointer they return 
*/

blockInfo* memPtrToBlockInfoPtr(void* ptrToMem) {
    return ptrToMem - BLOCKINFOSIZE;
}