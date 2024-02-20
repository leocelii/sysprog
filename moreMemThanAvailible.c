#include <stdio.h>
#include "mymalloc.h"

int main(){
    //attemps to allocate more memory than is currently availible
    void *ptr1 = malloc(100); // allocates some memory
    void *ptr2 = malloc(500000); // attempts to allocate 500k byte (should not work)
    return 0;
}
