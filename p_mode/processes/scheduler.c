#include "scheduler.h"
#include "process_list.h"
#include "paging.h"
#include "system.h"
#include "screen.h"
#include "timer.h"
#include "kheap.h"

int cur_proc_index = -1;
int scheduler_active = 0;
int is_first_run = 1;

int time_slice(uint8_t priority_level) {
	switch(priority_level) {
	case 0:
		return 100;
	case 1:
		return 50;
	case 2:
		return 25;
	case 3:
		return 10;
	default:
		return 5;
	}
}

void init_scheduler(void* kernel_func) {
	init_process_list();
	process* kernel_proc = proc_arr;
	kernel_proc->priority_level = 0;
	kernel_proc->pid = 0;
	kernel_proc->page_directory = kernel_directory;
	kernel_proc->last_state_regs.ds = 0x10;
	kernel_proc->last_state_regs.cs = 0x08;
	kernel_proc->last_state_regs.eip = (uint32_t) kernel_func;
	kernel_proc->last_state_regs.ss = 0x10;
	kernel_proc->last_state_regs.es = 0x10;
	kernel_proc->last_state_regs.fs = 0x10;
	kernel_proc->last_state_regs.gs = 0x10;
	kernel_proc->last_state_regs.eflags = 0x202;
	kernel_proc->last_state_regs.useresp = (uint32_t)kmalloc(0x1000) + 0x1000;
	kernel_proc->last_state_regs.ebp = kernel_proc->last_state_regs.useresp;
	set_time_slice(40);
	cur_proc_index = 0;
}

// this takes a bit of time to kick in after activating the scheduler, so
// some statements right after activating the scheduler could be run.
int start_next_process(regs_t* current_proc_regs) {
	if(scheduler_active) {
		int i = cur_proc_index;
		do {
			i++;
			if(i >= MAX_PROC) i = 0;
			if(proc_arr[i].pid != -1) {
				if(!is_first_run) proc_arr[cur_proc_index].last_state_regs = *current_proc_regs;
				else is_first_run = 0;
				*current_proc_regs = proc_arr[i].last_state_regs;
				switch_page_directory(proc_arr[i].page_directory);
				cur_proc_index = i;
			}
		} while(cur_proc_index != i);
		return time_slice(proc_arr[cur_proc_index].priority_level);	
	} else return 50;
}
