#include "screen.h"
#include "system.h"
#include "paging.h"
#include "scheduler.h"
#include "idt.h"
#include "timer.h"
#include "heap.h"
#include "pci.h"
#include <stdint.h>
#include "process_list.h"
#include "kheap.h"

void divideByZero(regs_t r);
extern void print_vendor_info();
void call_cpuid(uint32_t op, uint32_t* eax, uint32_t* edx);

extern uint32_t* CPUID_available;

struct idt_ptr idtptemp;
extern void read_idtr();
extern void jmp_user_mode();
extern void **fault_handlers;

extern int scheduler_active;

process proc2, proc3;

void lol2() {
	initialise_heap(&proc2);
	int lol2 = 0;
	uint32_t* mptr = malloc(50, &proc2, 0);
	*mptr = 5;
	while(1) {
		lol2++;
		if(lol2 >= 50000000) {
			printf("Value @ %x: %x\n", mptr, *mptr);
			lol2 = 0;
		}
	}
}

void lol3() {
	initialise_heap(&proc3);
	uint32_t* mptr = malloc(50, &proc3, 0);
	*mptr = 6;
	int lol3 = 0;
	while(1) {
		lol3++;
		if(lol3 >= 50000000) {
			printf("Value @ %x: %x\n", mptr, *mptr);
			lol3 = 0;
		}
	}
}

void lol() {
	int lol = 0;
	
	proc2.page_directory = get_new_kernel_mapped_dir();
	proc2.priority_level = 0;
	regs_t regis;
	regis.ds = 0x10;
	regis.cs = 0x08;
	regis.eip = (uint32_t) lol2;
	regis.ss = 0x10;
	regis.es = 0x10;
	regis.fs = 0x10;
	regis.gs = 0x10;
	regis.eflags = 0x202;
	uint32_t size = 0x1000;
	alloc_page(proc2.page_directory, KERNEL_MEM_END, 1, 1);
	alloc_page(proc2.page_directory, KERNEL_MEM_END + 0x1000, 1, 1);
	regis.useresp = (uint32_t)KERNEL_MEM_END + 0x1000;
	regis.ebp = regis.useresp;
	proc2.heap_stack_boundary = regis.ebp;
	proc2.heap_size = 0x1000;
	proc2.last_state_regs = regis;
	add_process(proc2);
	
	proc3.page_directory = get_new_kernel_mapped_dir();
	proc3.priority_level = 0;
	regis.ds = 0x10;
	regis.cs = 0x08;
	regis.eip = (uint32_t) lol3;
	regis.ss = 0x10;
	regis.es = 0x10;
	regis.fs = 0x10;
	regis.gs = 0x10;
	regis.eflags = 0x202;
	alloc_page(proc3.page_directory, KERNEL_MEM_END, 1, 1);
	alloc_page(proc3.page_directory, KERNEL_MEM_END + 0x1000, 1, 1);
	regis.useresp = (uint32_t)KERNEL_MEM_END + 0x1000;
	regis.ebp = regis.useresp;
	proc3.heap_stack_boundary = regis.ebp;
	proc3.heap_size = 0x1000;
	proc3.last_state_regs = regis;
	add_process(proc3);
	
	while(1);
}


void main() {
	init_video();
	init_tss();
	idt_install();

	init_paging();
	timer_install();
	set_timer_freq(100);
	keyboard_install();
	register_fault_handler(0, divideByZero);
	printf("Thank you for choosing Garg-OS!\n");
	//check_all_buses();
	init_scheduler(lol);
	scheduler_active = 1;
	/*process proc2;
	proc2.page_directory = get_new_kernel_mapped_dir();
	proc2.priority_level = 0;
	regs_t regis;
	regis.ds = 0x10;
	regis.cs = 0x08;
	regis.eip = (uint32_t) lol2;
	regis.ss = 0x10;
	regis.es = 0x10;
	regis.fs = 0x10;
	regis.gs = 0x10;
	regis.eflags = 0x202;
	regis.useresp = (uint32_t)kmalloc(0x1000) + 0x1000;
	regis.ebp = regis.useresp;
	proc2.last_state_regs = regis;
	add_process(proc2);*/
	/*regis.useresp = (uint32_t)kmalloc(0x1000) + 0x1000;
	regis.ebp = regis.useresp;
	proc2.last_state_regs = regis;
	proc2.last_state_regs.eip = (uint32_t) lol3;
	add_process(proc2);*/
	//print_heap_summuary(proc_arr+sizeof(process));
	//void* ptr = malloc(5000, &proc2, 0);
}

void divideByZero(regs_t r) {
	printf("Don't divide by zero. System halted!\n");
}
