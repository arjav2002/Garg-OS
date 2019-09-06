#include "process_list.h"
#include "process.h"
#include "heap.h"
#include "kheap.h"

uint32_t list_size = 0;
element* kernel_handle = NULL;

void init_list(process* kernel_proc) {
	kernel_handle = (element*) kmalloc(sizeof(element));
	kernel_handle->proc = kernel_proc;
	kernel_handle->next = kernel_handle;
	list_size = 1;
}

void add_element(process* proc) {
	element* new_element = (element*) malloc(sizeof(element), kernel_handle->proc, 0);
	new_element->proc = proc;
	new_element->next = kernel_handle->next;
	kernel_handle->next = new_element;
	list_size++;
}
