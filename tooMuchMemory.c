#include <stdio.h>
#include "mymalloc.h"


int main(){
    // program attemps to allocate too much memory
    void *ptr = malloc(1000000000); // Attempt to allocate 1 billion bytes
    return 0;
}
