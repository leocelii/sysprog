#include <stdio.h>
#include "mymalloc.h"


int main() {
    // attempts to free address in the heap but not of an object
    void *ptr = malloc(10); // this allocates memory
    free(ptr +5); //attempts to free object in the middle of the allocated memory
}
