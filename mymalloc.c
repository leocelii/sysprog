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
    // struct Node* prev;  // Pointer to the previous chunk's metadata *****************
} chunkNode;

void initializeMemory() {
    chunkNode *memStart = (chunkNode*)memory;
    memStart->size = MEMSIZE - HEADERSIZE;
    memStart->allocated = 0;
}
void *mymalloc(size_t size, char *file, int line){
    if (size == 0) {
        return NULL; //if mymalloc(0, .., ..), then return NULL
    }

    //if memory array has not been intialized, and mymalloc is called for the first time, then this step will initialize the memory array
    chunkNode *memoryInitialized = (chunkNode*)memory;
    if (memoryInitialized->size <= 0 || memoryInitialized->size > MEMSIZE || (memoryInitialized->allocated != 0 && memoryInitialized->allocated != 1)) {
    initializeMemory(); //initialize memory array;
    }
    
    int newByteSize = (size + 7) & ~7; //ensures that requested size is rounded up to the nearest multiple of 8
    size = (newByteSize < 16) ? 16 : newByteSize; //ensures that the size is at least minimum chunk size of 16, 8 min for metadata, 8 min. for payload
    
    char *memoryStart = (char*)memory; //setup for byte-width pointer arithmetic
    while (memoryStart < ((char*)memory + MEMSIZE)) { //within range of memory
        chunkNode *currentNode = (chunkNode*)memoryStart; //memoryStart gets casted to a pointer of type chunkNode to access the metadata at the current chunk
        if(currentNode->allocated == 0 && currentNode->size >= size){ //if the current chunk's metadata represents a free chunk, whose size is greater than/equalto the requested size, than the requested chunk has been found
            if (currentNode->size > size && ((currentNode->size - size - HEADERSIZE) >= HEADERSIZE)) {  //if possible, split into two chunks
                chunkNode *newNode = (chunkNode *)(memoryStart + size + HEADERSIZE);
                newNode->size = currentNode->size - size - HEADERSIZE;
                newNode->allocated = 0;
                currentNode->size = size;
            }
            currentNode->allocated = 1;
            return memoryStart + HEADERSIZE;
    
        }
        memoryStart += ((currentNode->size) + HEADERSIZE);
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
    chunkNode *current_header = (chunkNode*)((char*)ptr - HEADERSIZE);
    char *memoryStart = (char*)memory;
    char *memoryEnd = memoryStart + MEMSIZE;

    // Validate if the pointer points to a valid memory chunk
    if (((char*)current_header < memoryStart) || ((char*)current_header >= memoryEnd) || (current_header->allocated != 1)) {
        // Report error: Attempting to free memory that was not allocated by malloc or attempting to free memory that is out of bounds and therefore also not allocated by malloc.
        fprintf(stderr, "Attempting to free memory that was not malloced in file '%s' on line %d\n", file, line);
        return;
    }

    // Mark the chunk as free
    current_header->allocated = 0;
    

    // Coalesce free chunks if possible
    // Coalescing with the previous chunk
    //chunkNode *prev_header = (chunkNode*)((char*)current_header - ); //how do we find the address of the previous chunk (header/metadata)?

    
    if ((char*)current_header > (char*)memory + HEADERSIZE) { //checks if first chunk/ if it is, there is no previous chunk.
        chunkNode* prev_header = (chunkNode*)memory; //simple but less efficient way of iterating through all previous chunks until we find the immediate previous
        while ((char*)prev_header + HEADERSIZE + prev_header->size != (char*)current_header) {
            prev_header = (chunkNode*)((char*)prev_header + HEADERSIZE + prev_header->size);
        }
        if (prev_header->allocated == 0) {
            // Coalesce with previous chunk
            prev_header->size += current_header->size + HEADERSIZE;
            current_header = prev_header; // Update header to point to the coalesced chunk
        }
        // Coalescing with the next chunk
    }

    if ((char*)current_header + current_header->size + HEADERSIZE < (char*)memory + MEMSIZE) { // checks if first chunk/ if its is there is no previous chunk.
        chunkNode *next_header = (chunkNode*)((char*)current_header + current_header->size + HEADERSIZE); //current chunk header plus CHUNKSIZE = next chunk, CHUNKSIZE = (payload size + headersize) 
        if (((char*)next_header < ((char*)memory + MEMSIZE)) && next_header->allocated == 0) {
            // Coalesce with next chunk
            current_header->size += next_header->size + HEADERSIZE;
            next_header = (chunkNode *)((char*)current_header + current_header->size + HEADERSIZE);
        }
    }









//chunkNode *prev_header = header->prev;
  //  if (prev_header != NULL && prev_header->allocated == 0) {
    //    // Coalesce with previous chunk
      //  prev_header->size += header->size + sizeof(chunkNode);
        //header = prev_header; // Update header to point to the coalesced chunk
    //}

    // Coalescing with the next chunk
    //chunkNode *next_header = (chunkNode*)((char*)header + header->size + sizeof(chunkNode));
    //if ((char*)next_header < memoryEnd && next_header->allocated == 0) {
        // Coalesce with next chunk
      //  header->size += next_header->size + sizeof(chunkNode);
    //}

    // Update the prev pointer of the next chunk, if it exists
    //chunkNode *next_next_header = (chunkNode*)((char*)header + header->size + sizeof(chunkNode));
    //if ((char*)next_next_header < memoryEnd) {
      //  next_next_header->prev = header;
    //}
// }

