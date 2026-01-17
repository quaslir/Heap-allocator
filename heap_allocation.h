#include <stddef.h>
#include <stdbool.h>
#define request_free(ptr) request_free_impl((void **)(&(ptr)))
typedef struct block {
    struct block * next;
    struct block * prev;
    size_t size;
    size_t used;
    bool free;
} block_t;

block_t * find_free_block(size_t size);
block_t * find_last_block();
void split_block(block_t * block, size_t size);
void split_block(block_t * block, size_t size);
void * request_memory(size_t size);
void combine_blocks(block_t * block1, block_t * block2);
void request_free_impl(void ** ptr_ref);
void print_blocks(void);
void * request_realloc(void * ptr, size_t size);

