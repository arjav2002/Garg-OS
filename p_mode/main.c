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

void divideByZero(regs_t r);
extern void print_vendor_info();
void call_cpuid(uint32_t op, uint32_t* eax, uint32_t* edx);

extern uint32_t* CPUID_available;
extern element* kernel_handle;

struct idt_ptr idtptemp;
extern void read_idtr();
extern void jmp_user_mode();
extern void **fault_handlers;

void main() {
	init_video();
	init_tss();
	idt_install();
	register_fault_handler(0, divideByZero);

	init_paging();
	timer_install();
	set_timer_freq(100);
	keyboard_install();
	register_fault_handler(0, divideByZero);
	printf("Thank you for choosing Garg-OS!\n");
	
	check_all_buses();

	// call only after doing all the kmallocs
	init_scheduler();
	/*
	regs_t my_regs;
	my_regs.ds = 0x23;
	my_regs.es = 0x23;
	my_regs.fs = 0x23;
	my_regs.gs = 0x23;
	my_regs.cs = 0x1B;
	my_regs.ss = 0x23;
	my_regs.eip = KERNEL_MEM_END;
	my_regs.eflags = get_eflags();
	uint32_t code_size = 0x20, stack_size = 0x100;
	my_regs.useresp = KERNEL_MEM_END + code_size + stack_size - 4;
	add_process(stack_size, code_size, my_regs, (uint8_t*)user_func);*/
}

void divideByZero(regs_t r) {
	printf("You are a dumbo. Don't divide by zero. System halted!\n");
}
