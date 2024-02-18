#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mymalloc.h"

#define MEMSIZE 4096
#define HEADERSIZE 8
#define OBJECTS 64
#define OBJSIZE (MEMSIZE / OBJECTS - HEADERSIZE)
#define MEMLENGTH 512
static double memory[MEMLENGTH];

typedef struct Node {
    int size;
    int allocated;
} chunkNode;

void initializeMemory() {
    chunkNode *memStart = (chunkNode *)memory;
    memStart->size = MEMSIZE;
    memStart->allocated = 0;
}
void *mymalloc(size_t size, char *file, int line){
    if (size == 0) {
        return NULL; //if mymalloc(0, .., ..), then return NULL
    }

    //if memory array has not been intialized, and mymalloc is called for the first time, then this step will initialize the memory array
    static int initialized = 0; //static makes it so, it retains its value as 1, during every call to malloc, after the first.
    if (!initialized) {
    initializeMemory(); //initialize memory array;
    initialized = 1; //subsequent calls to mymalloc will already have the memory array initialized, so this step will be skipped. 
    }
    
    int newByteSize = (size + 7) & -7; //ensures that requested size is rounded up to the nearest multiple of 8
    size = (newByteSize < 16) ? 16: size; //ensures that the size is at least minimum chunk size of 16, 8 min for metadata, 8 min. for payload
    
    char *memoryStart = (char *)memory; //setup for byte-width pointer arithmetic
    while (memoryStart < ((char*)memory + MEMSIZE)) { //within range of memory
        chunkNode *currentNode = (chunkNode *)memoryStart; //memoryStart gets casted to a pointer of type chunkNode to access the metadata at the current chunk
        if(currentNode->allocated == 0 && currentNode->size >= size){ 
            //if the current chunk's metadata represents a free chunk, whose size is greater than/equalto the requested size, than the requested chunk has been found
            if (currentNode->size > size) { 
                chunkNode *newNode = (chunkNode *)(memoryStart + size);
                newNode->size = currentNode->size - size;
                newNode->allocated = 0;
                currentNode->size = size;
            }
            currentNode->allocated = 1;
            return memoryStart + sizeof(chunkNode);
    
        }
        memoryStart += ((currentNode->size));
    }
    return NULL; //if no space is found that matches the requested amount of bytes, return NULL or "space unavailable"
}

void myfree(void *ptr, char *file, int line) {
    // Check if the pointer is NULL
    if (ptr == NULL) {
        // Report error: Attempting to free a NULL pointer
        fprintf(stderr, "Pointer is NULL in file '%s' on line %d\n", file, line);
        return;
    }

    // Calculate the address of the chunk header
    chunkNode *header = (chunkNode*)((char*)ptr);
    char *memoryStart = (char*)memory;
    char *memoryEnd = memoryStart + MEMSIZE;

    // Validate if the pointer points to a valid memory chunk
    if ((ptr <= memoryStart) || (ptr >= memoryEnd) || (header->allocated != 1)) {
        // Report error: Attempting to free memory that was not allocated by malloc or attempting to free memory that is out of bounds and therefore also not allocated by malloc.
        fprintf(stderr, "Attempting to free memory that was not malloced in file '%s' on line %d\n", file, line);
        return;
    }

    // Mark the chunk as free
    header->allocated = 0;

    // Coalesce free chunks if possible
    // Coalescing with the previous chunk
    
    chunkNode *prev_header = (chunkNode*)((char*)header - sizeof(chunkNode));
    if (prev_header >= (char*)memory && prev_header->allocated == 0) {
        // Coalesce with previous chunk
        prev_header->size += header->size;
        header = prev_header; // Update header to point to the coalesced chunk
    }

    // Coalescing with the next chunk
    chunkNode *next_header = (chunkNode*)((char*)header + header->size);
    if ((char*)next_header < (char*)memory + MEMLENGTH && next_header->allocated == 0) {
        // Coalesce with next chunk
        header->size += next_header->size;
    }
}
