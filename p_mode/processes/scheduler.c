#include "process_list.h"
#include "scheduler.h"
#include "kheap.h"
#include "paging.h"
#include "heap.h"
#include "screen.h"
#include "timer.h"

element* current_element;
extern element* kernel_handle;
extern uint32_t place_addr;
// memory layout of a process:- Kernel mapping, code, stack, heap
// memory layout of kernel process:- OS-IMAGE & CODE, KMALLOC (ends at place_addr), TSS allocated stack, heap until KERNEL_MEM_END is reached
uint32_t init_proc(process* proc, uint8_t priority_level, uint32_t* page_directory, uint32_t stack_size, uint32_t code_size, regs_t registers, uint8_t* code) {
	proc->priority_level = priority_level;
	proc->page_directory = page_directory;
	proc->pid = list_size;
	proc->last_state_regs = registers;
	proc->heap_stack_boundary = (page_directory == kernel_directory?place_addr : KERNEL_MEM_END) + code_size + stack_size;

	uint32_t heap_size;
	if(page_directory != kernel_directory) heap_size = (((proc->heap_stack_boundary >> 12) + 0x1) << 12) - proc->heap_stack_boundary; // covers the leftover page where stack ends
	else heap_size = KERNEL_MEM_END - proc->heap_stack_boundary;
	proc->heap_size = heap_size;

	uint32_t i = KERNEL_MEM_END;
	// allocates all memory specific to the process
	for(; i < (heap_size + proc->heap_stack_boundary); i += PAGE_SIZE) {
		uint32_t phys_addr = alloc_page(page_directory, i, 1, (page_directory == kernel_directory));
		if(phys_addr == -1) {
			printf("\nOut of pages while initialising process. System halted. PID = %d", proc->pid);
			for(;;);
		}
		if(code_size > 0) {
			uint32_t amount_copied = code_size > PAGE_SIZE? PAGE_SIZE : code_size;
			memcpy((void*)phys_addr, (void*)code, amount_copied);
			code += amount_copied;
			code_size -= amount_copied;
		}
	}
	if(page_directory != kernel_directory) map_kernel_into_dir(page_directory);
	
	// the size available for heap will atmost be ( heap_size requested - sizeof(block) ) 
	initialise_heap(proc);
	return proc->pid;
}

int start_next_process(regs_t* registers) {
	// stop current process
	process* current_proc = current_element->proc;
	current_proc->last_state_regs = *registers;
	// switch to next process
	current_element = current_element->next;
	// start current process
	uint32_t time_slice;
	switch(current_element->proc->priority_level) {
	case 0:
		time_slice = PRIORITY0_TIME_SLICE_MILLIS;
		break;
	case 1:
		time_slice = PRIORITY1_TIME_SLICE_MILLIS;
		break;
	case 2:
		time_slice = PRIORITY2_TIME_SLICE_MILLIS;
		break;
	case 3:
		time_slice = PRIORITY3_TIME_SLICE_MILLIS;
	}
	copy_regs(&(current_element->proc->last_state_regs), registers);
	switch_page_directory(current_element->proc->page_directory);
	return time_slice;
}


void init_scheduler() {
	process* kernel_proc = (process*) kmalloc(sizeof(process));
	regs_t kernel_regs; // this is safe because the regs get set in the timer if kernel is the only process for some time.
	kernel_proc->pid = init_proc(kernel_proc, 0, kernel_directory, 0, 0, kernel_regs, NULL); 
	init_list(kernel_proc);
	current_element = kernel_handle;
	set_time_slice(PRIORITY0_TIME_SLICE_MILLIS);
}

uint32_t add_process(uint32_t stack_size, uint32_t code_size, regs_t registers, uint8_t* code, uint8_t priority_level) {
	process* new_proc = (process*) malloc(sizeof(process), kernel_handle->proc, 0);
	uint32_t* page_directory = (uint32_t*) malloc(TABLES_PER_DIRECTORY * sizeof(uint32_t), kernel_handle->proc, 1);
	memset(page_directory, 0, TABLES_PER_DIRECTORY * sizeof(uint32_t));
	new_proc->pid = init_proc(new_proc, priority_level, page_directory, stack_size, code_size, registers, code);
	add_element(new_proc);
}
