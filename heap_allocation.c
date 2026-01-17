#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include "heap_allocation.h"

block_t * head = NULL;

block_t * find_free_block(size_t size) {
    if(size == 0) return NULL;
    block_t * current = head;   
    block_t * tmp = NULL;
    size_t k = 0;
    bool first = true;
    while(current) {
        printf("Status %d, Requested size : %zu, Available size: %zu\n", current->free, size, current->size - current->used);
        if(current->free && current->size >= size) {
            if((current->size / size < k) || first) {
                if(first) first = false;
                k = current->size / size;
                tmp = current;
            }
        } 
        current = current->next;
    }
    
    return tmp;
}

block_t * find_last_block() {
    block_t * current = head;

    while(current && current->next) current= current->next;

    return current;
}

void split_block(block_t * block, size_t size) {
    block_t * new_block = (block_t *) ((char *) (block + 1) + size);

    new_block->free = 1;
    new_block->size = block->size - size - sizeof(block_t);
    new_block->used = 0;

    block->size = size;
    new_block->next = block->next;
    new_block->prev = block;
    if(new_block->next) {
        new_block->next->prev = new_block;
    }
    block->next = new_block;
    puts("Block was successfully splitted");
}

block_t * new_block(size_t size) {
    if(size == 0) return NULL;
    size_t block_size = ((size + 4095) / 4096) * 4096 * 10;
    block_size = (block_size + 7) & ~7;
    printf("Requesting new block from memory ...\n");
    block_t * block = NULL;
         block = mmap(NULL, block_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if(block == MAP_FAILED) return NULL;
    block->size = block_size - sizeof(block_t);


    block->free = 0;
    if(!head) {
        head = block;
    }
    else {
        block_t * prev = find_last_block();
        prev->next = block;
        block->prev = prev;
    }
    

    return block;
}

void * request_memory(size_t size) {
    if(size == 0) {
        return NULL;
    }

    block_t * block = find_free_block(size);
    if(!block) {
        block = new_block(size);
    }
      if(block->size / 2 >= size) split_block(block, size);
      block->free = 0;
    block->used = size;
    return (void *) (block + 1);
}

void combine_blocks(block_t * block1, block_t * block2) {
    if(!block1 || !block2) return;
    block1->size += block2->size + sizeof(block_t);
    block1->next = block2->next;
    if(block1->next) {
        block1->next->prev = block1;
    }
    block1->free = 1;
    block1->used = 0;
    puts("Successfully combined two blocks into one");
}

void request_free_impl(void ** ptr_ref) {
    void *ptr = *ptr_ref;
    if(!ptr) return;

    block_t * block = (block_t *) ptr - 1;
    if(block->free == 1) return;
    block->free = 1;
    block->used = 0;
    block_t * current = NULL;
    if(block->next && block->next->free) {
        puts("Combining two blocks...");
        combine_blocks(block, block->next);
    }
    
    
    if(block->prev && block->prev->free) {
        puts("Combining two blocks...");
        combine_blocks(block->prev, block);
        current = block->prev;
    } else current = block;


   if(current->free && current->size > 100000) {
    *ptr_ref = NULL;
    block_t * next = current->next;
    if(current->prev) current->prev->next = next;
    else head = next;
    if(next) {
        next->prev = current->prev;
    }
    
    printf("Unmapping block of size %zu\n", current->size + sizeof(block_t));
    munmap(current, current->size + sizeof(block_t));
   }
    
}   
    
void print_blocks(void) {
   
    block_t * current = head; 
    printf("%p\n", head);  
     if(!current) {
        fprintf(stderr, "No blocks allocated\n");
        return;
    }
    while(current) {
        printf("Address: %p, Used %zu/%zu, Free:%s\n", current, current->used, current->size, current->free ? "true" : "false");
        current = current->next;
    }

}


void * request_realloc(void * ptr, size_t size) {
    if(!ptr) return request_memory(size);
    if(!size) {
        request_free_impl(&ptr);
        return NULL;
    }

    block_t * block = (block_t *) ptr - 1;

    if(block->size >= size) {
        block->used = size;
        fprintf(stdout, "Block was not changed, memory is enough in this block, exitting ...\n");
        return ptr;
    }

    void * new_ptr = request_memory(size);

    if(!new_ptr) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    memmove(new_ptr, ptr, block->used);

    request_free_impl(&ptr);

    return new_ptr;


}