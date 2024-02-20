#include <stdio.h>
#include "mymalloc.h"


int main() {
    //attempt to free an address outside the heap
    int x;
    free(&x);
    return 0;
}
