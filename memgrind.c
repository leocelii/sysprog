
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#ifndef REALMALLOC
#include "mymalloc.h"
#endif

#define OBJECTS 120

int main () {
	//Array to store the pointers to the allocated objects using malloc
	void *pointers[OBJECTS] = { NULL };

	//Random numb generator
	srand(time(NULL));

	//Randomly choose between allocating and deallocating
	int allocatedCount = 0;
	int currentNumAlloc =  0;
	while (allocatedCount < OBJECTS) {
		if (rand() > RAND_MAX/2) {
			pointers[currentNumAlloc] = malloc(1);
			if (pointers[currentNumAlloc] != NULL) {
				allocatedCount++;
				currentNumAlloc++;
			}
			else {
				fprintf(stderr, "Failed to allocate memory\n");
		      		break;
			}
		}
		else {
			//Deallocate previously allocated object
			if (currentNumAlloc > 0) {
				free(pointers[currentNumAlloc - 1]);
				pointers[currentNumAlloc - 1] = NULL;
				currentNumAlloc--;
			}
		}
	}
	

	// free any remaining allocated objects
	for (int i = 0; i < OBJECTS; i++) {
		if (pointers[i] != NULL) {
			free(pointers[i]);
		}
	}
	return 0;

}
