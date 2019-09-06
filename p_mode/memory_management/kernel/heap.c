#include "heap.h"
#include "paging.h"
#include "process.h"
#include "screen.h"

#include <stdint.h>
#include <stddef.h>
// returns address to pointer in process memory space
void* alloc_new_page(process* proc) {
	alloc_page(proc->page_directory, (proc->heap_size + proc->heap_stack_boundary), 1, proc->page_directory == kernel_directory);
	return (void*) (proc->heap_size + proc->heap_stack_boundary);
}

// makes *info block smaller and equal to new_bytes
// return pointer to new block created
block* divide_block(block* info, uint32_t new_bytes) {
	if(new_bytes > info->size - sizeof(block)) return NULL;
	block* new_info = (block*)((uint32_t)info + sizeof(block) + new_bytes);
	new_info->size = info->size - sizeof(block) - new_bytes;
	info->size = new_bytes;
	return new_info;
}

void* get_free_block(process* proc, uint32_t bytes, uint8_t align) {
	uint32_t start = proc->heap_stack_boundary;
	uint32_t size = proc->heap_size;
	uint32_t i;
	for(i = 0; i < size;) {
		uint32_t effective_addr = start + i;
		block *info = (block*) effective_addr;
		if(info->is_free && info->size >= bytes) {
			if(align) {
				// if current memory area is aligned then return that
				if(effective_addr + sizeof(block) % 0x1000 == 0) return (void*)effective_addr;
				// else find next aligned address
				uint32_t next_aligned_addr;
				uint32_t block_end = effective_addr + sizeof(block) + info->size;
				for(next_aligned_addr = (((effective_addr + sizeof(block)) >> 12) + 0x1) << 12; block_end - next_aligned_addr >= bytes; next_aligned_addr += 0x1000) {
					// check if block is big enough to divide block there.
					if(next_aligned_addr - effective_addr <= 2*sizeof(block)) continue;
					// assuring that new_bytes > 0
					uint32_t new_bytes = next_aligned_addr - effective_addr - 2*sizeof(block);
					block* new_info = divide_block(info, new_bytes);
					if(new_info != NULL) {
						new_info->is_free = 0;
						if(new_info->size > bytes + sizeof(block)) {
							block* info_2 = (block*)((uint32_t)new_info + sizeof(block) + bytes);
							info_2->is_free = 1;
							info_2->size = block_end - (uint32_t)info_2 - sizeof(block);
							new_info->size = bytes;
						}
						return (void*) new_info;
					}
				}
				// if block is not big enough to be divided go, to next block.
			}
			else {
				info->is_free = 0;
				// divide block if big enough to accomodate
				block* new_info = divide_block(info, bytes);
				if(new_info != NULL) new_info->is_free = 1;
				return (void*)effective_addr;
			}
		}
		i += sizeof(block) + info->size;
	}
	return NULL;
}

void unite_free_blocks(process* proc) {
	uint32_t start = proc->heap_stack_boundary;
	uint32_t size = proc->heap_size;
	uint32_t effective_addr = start;
	block *prev_info = (block*) effective_addr;
	
	uint32_t i;
	for(i = prev_info->size + sizeof(block); i < size;) {
		uint32_t effective_addr = start + i;
		block *info = (block*) effective_addr;
		uint8_t merge_flag = 1;
		if(info->is_free && prev_info->is_free) {
			merge_flag = 0;
			prev_info->size += sizeof(block) + info->size;
		}
		i += sizeof(block) + info->size;
		if(merge_flag) prev_info = info;
	}
}

void initialise_heap(process* proc) {
	uint32_t first_block_addr = proc->heap_stack_boundary;
	block* info = (block*) first_block_addr;
	info->size = proc->heap_size - sizeof(block);
	info->is_free = 1;
}

void print_heap_summuary(process* proc) {
	printf("\nHeap size: %x\n", proc->heap_size);
	uint32_t start = proc->heap_stack_boundary;
	uint32_t size = proc->heap_size;
	uint32_t i;
	for(i = 0; i < size;) {
		block* info = (block*) (i + start);
		printf("Effective Addr: %x\tInfo is_free: %d\tsize: %x\n", (uint32_t)info, info->is_free, info->size);
		i += sizeof(block) + info->size;
	}
}

void* malloc(uint32_t bytes, process* proc, uint8_t align) {
	if(bytes == 0) return NULL;
	void* get_free_block_addr = NULL;
	do {
		get_free_block_addr = get_free_block(proc, bytes, align);
		if(get_free_block_addr == NULL) {
			alloc_new_page(proc);
			uint32_t page_start_addr = proc->heap_stack_boundary + proc->heap_size;
			block* info = (block*) page_start_addr;
			info->size = PAGE_SIZE - sizeof(block);
			info->is_free = 1;
			proc->heap_size += PAGE_SIZE;
			unite_free_blocks(proc);
		}
	} while(get_free_block_addr == NULL);
	return (void*) ((uint32_t)get_free_block_addr + sizeof(block));
}

void free(process* proc, void* mem_ptr) {
	block* info = (block*) ((uint32_t)(mem_ptr) - sizeof(block));
	info->is_free = 1;
	unite_free_blocks(proc);
}
