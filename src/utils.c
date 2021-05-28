#include "utils.h"
#include <stdio.h>

void *xzalloc(size_t count, size_t size){
    /* calloc initializes the new memory to 0  so there is no need to
     * explicitly do it here */
    void *ptr = calloc(count, size);
    if(ptr == NULL){
        fprintf(stderr, "Error: Failed to reserve memory");
        abort();
    }
    return ptr;
}
