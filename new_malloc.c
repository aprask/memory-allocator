#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
typedef char ALIGN[16];
typedef struct {
    size_t size;
    bool free;
    header_t* next; // linked structure
} header_t;

union header {
    header_t header_data;
    ALIGN bound;
};
