#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#ifndef REALMALLOC
#include "mymalloc.h"
#endif

#define OBJECTS 120
#define NUM_RUNS 50

int main () {
	//creating structures to record time
	struct timeval startTime, endTime;
	double elapsedTime;

	//Array to store the pointers to the allocated objects using malloc
	void *pointers[OBJECTS] = { NULL };

	//Random numb generator
	srand(time(NULL));

	//Randomly choose between allocating and deallocating
	//setup recording structures for elapsed time of each task
	int allocatedCount = 0;
	int currentNumAlloc =  0;
	int mallocTaskCount = 0;
	double mallocAvgTime = 0;
	int freeTaskCount = 0;
	double freeAvgTime = 0;

	while (allocatedCount < OBJECTS) {
		if (rand() > RAND_MAX/2) {
			if (mallocTaskCount < NUM_RUNS) {
				gettimeofday(&startTime, NULL);
				pointers[currentNumAlloc] = malloc(1);
				gettimeofday(&endTime, NULL);
				elapsedTime = (endTime.tv_sec - startTime.tv_sec) * 1000.0; //convert seconds to milliseconds
				elapsedTime += (endTime.tv_usec - startTime.tv_usec) / 1000.0; // convert microseconds to milliseconds
				mallocAvgTime += elapsedTime;
				mallocTaskCount++;
			}
			else {
				pointers[currentNumAlloc] = malloc(1);
			}

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
				if (freeTaskCount < NUM_RUNS) {
					gettimeofday(&startTime, NULL);
					free(pointers[currentNumAlloc - 1]);
					gettimeofday(&endTime, NULL);
					elapsedTime = (endTime.tv_sec - startTime.tv_sec) * 1000.0;
					elapsedTime += (endTime.tv_usec - startTime.tv_usec) / 1000.0;
					freeAvgTime += elapsedTime;
					freeTaskCount++;
 					pointers[currentNumAlloc - 1] = NULL;
					currentNumAlloc--;
				}
				else {
					free(pointers[currentNumAlloc - 1]);
					pointers[currentNumAlloc - 1] = NULL;
					currentNumAlloc--;
				}
			}
		}
	}
	// In finding the average of the 50 runs for each task (mymalloc, myfree), it is possible that only one of
	// the tasks will reach 50, and the other will be in the 40's most likely. This is because it is likely that
	// all 120 allocations of a 1-byte object will be completed, before 50 runs of both free and malloc.
	// Therefore, while malloc will reach 50 runs, because it needs to allocate 120 times, free may not reach 50
	// as per the random choice between the allocating and deallocating. Howevever, it is likely that free runs
	// for a number of times in the 40's, and perhaps very close to 50, if not 50. To address this concern, 
	// instead we calculated the average of the mymalloc and myfree runs, using not 50, but using a count 
	// of the number of times each function was used/called. Through a number of trial runs, we've determined 
	// that myfree was called usually in the high 40's, and somtimes it would reach and exceed 50. We figured
	// this would not make a difference in reporting accuracy, as the average time to run say, for example, 47 
	// trials, will be roughly the same as the average time to run all 50 trials. 
	
	printf("The average time needed for a successful call to the mymalloc function is: %f milliseconds.\n", (mallocAvgTime / mallocTaskCount)); 
	printf("The average time needed for a successful call to the myfree function is: %f milliseconds.\n", (freeAvgTime / freeTaskCount)); 

	// free any remaining allocated objects
	for (int i = 0; i < OBJECTS; i++) {
		if (pointers[i] != NULL) {
			free(pointers[i]);
		}
	}
	return 0;

}
