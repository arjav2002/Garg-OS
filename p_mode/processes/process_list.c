#include "kheap.h"
#include "system.h"
#include "process_list.h"
#include "screen.h"
#include "heap.h"

process* proc_arr;

uint32_t list_size;

void init_process_list() {
	proc_arr = (process*) kmalloc(sizeof(process)*MAX_PROC);
	list_size = 0;
	for(uint8_t i = 0; i < MAX_PROC; i++) proc_arr[i].pid = -1;
}

int add_process(process proc) {
	for(int i = 0; i < MAX_PROC; i++) {
		if(proc_arr[i].pid == -1) {
			proc_arr[i] = proc;
			proc_arr[i].pid = i;
			return i;
		}
	}
	printf("No space for any other process!\n");
	return -1;
}

void show_all_procs() {
	printf("ALL PROCESSES:\n");
	for(int i = 0; i < MAX_PROC; i++) if(proc_arr[i].pid != -1) printf("PROC ID: %d\n", proc_arr[i].pid); 
}

void remove_process(process proc) {
	for(int i = 0; i < MAX_PROC; i++) if(proc_arr[i].pid == proc.pid) proc_arr[i].pid = -1;
}
