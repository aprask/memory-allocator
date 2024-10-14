#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
typedef char ALIGN[16];
typedef struct header_t header_t;
struct header_t {
    size_t size;
    bool free;
    header_t* next; // linked structure
};
typedef union {
    header_t header_data;
    ALIGN bound;
} header;
pthread_mutex_t malloc_access_lock;
header *head, *tail;
header* get_free_block(size_t size);
void* new_malloc(size_t size) {
    if (size <= 0) {
    	printf("ERROR: Invalid Size\n");
        return NULL;
    }
    size_t total_size;
    void* mem_block;
    pthread_mutex_lock(&malloc_access_lock);
    header* header_inst = get_free_block(size);
    if (!header_inst) {
        header_inst->header_data.free = false;
	pthread_mutex_unlock(&malloc_access_lock);
	return (void*)(header_inst+1);
    }
    total_size = sizeof(header) + size; // image + meta
    mem_block = sbrk(total_size);
    if (mem_block == (void*) -1) {
         pthread_mutex_unlock(&malloc_access_lock);
	 printf("ERROR: Issue allocating memory with sbrk\n");
	 return NULL;
    }
    header_inst = (header*)mem_block;
    header_inst->header_data.size = size;
    header_inst->header_data.free = false;
    header_inst->header_data.next = NULL;
    if (head == NULL) {
        head = header_inst;
    }
    else if (tail) {
        tail->header_data.next = header_inst;
    }
    tail = header_inst;
    pthread_mutex_unlock(&malloc_access_lock);
    return (void*)(header_inst+1);
}

header* get_free_block(size_t size) {
    header* curr = head;
    header* max = NULL;
    while (curr) {
        if (curr->header_data.free && curr->header_data.size >= size && (!max || curr->header_data.size >= max->header_data.size)) {
            max = curr;
	}
	curr = curr->header_data->next;
    }
    return max;
}










