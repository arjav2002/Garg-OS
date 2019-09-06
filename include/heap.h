#ifndef HEAP_H
#define HEAP_H

#include "process.h"
#include <stdint.h>

// heap.c contains logic for freeing and allocating memory
// given a certain process (and hence its directory)
// assumes directory of the process passed
// is current directory
typedef struct block_struct {
	uint16_t is_free;
	uint32_t size;
}__attribute__((packed)) block;

void* malloc(uint32_t bytes, process* proc, uint8_t align);
void free(process* proc, void* mem_ptr);
void initialise_heap(process* proc);
void print_heap_summuary(process* proc);

#endif
