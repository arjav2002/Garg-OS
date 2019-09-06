#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

#include "system.h"
// all addresses are virtual addresses
// of the directory associated with
// the process, other than paging structures
// themselves, all of which lie in the kernel
// directory virtual address space
// heap should end on a page end
typedef struct process_struct {
	uint32_t heap_stack_boundary;
	uint32_t heap_size;
	uint32_t *page_directory;
	uint32_t pid;
	regs_t last_state_regs;
	uint8_t priority_level;
} process;
// 0 -> highest priority level

#endif
