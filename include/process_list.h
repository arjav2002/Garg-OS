#ifndef PROCESS_LIST_H
#define PROCESS_LIST_H

#include "process.h"

#include <stddef.h>

struct element_struct {
	process *proc;
	struct element_struct *next;
};

typedef struct element_struct element;

extern uint32_t list_size;
// only to be called in the scheduler
void add_element(process* proc);
void init_list(process* kernel_proc);

#endif
