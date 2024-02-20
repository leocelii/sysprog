#include <stdio.h>
#include "mymalloc.h"

int main(){
    //attepmt to free address of non-allocated object
    int *p = malloc(sizeof(int));
    free(p); // free the memory
    free(p); // attemps to free p again(should not work since memory was already allocated)
    return 0;
}
