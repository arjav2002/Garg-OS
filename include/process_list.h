#ifndef PROCESS_LIST_H
#define PROCESS_LIST_H

#include "process.h"

#define MAX_PROC 32

#include <stddef.h>

struct element_struct {
	process *proc;
	struct element_struct *next;
};

typedef struct element_struct element;

extern uint32_t list_size;
// only to be called in the scheduler
int add_process(process proc);
void init_process_list();

#endif
