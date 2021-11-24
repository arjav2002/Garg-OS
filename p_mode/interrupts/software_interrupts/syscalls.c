#include "system.h"
#include "screen.h"
#include "kheap.h"

#include <stdint.h>

#define CPUID_OP	0x0FA2

#define N_SYSCALLS	1
#define FIRST_INTNO	48

extern void syscall0();

void **req_handlers;

void syscalls_install() {
	req_handlers = (void**)kmalloc(sizeof(void*) * N_SYSCALLS);
	memset(req_handlers, 0, sizeof(void*) * N_SYSCALLS);
	
	// 0x08 = 00000000 00001000b  	0xEE = 11101110b
	// selector comments:
	// the segment with index = 1 in the GDT, null descriptor is index zero
	// in the GDT, with ring privillege = 0.
	// flag comments:
	// present, ring privillege zero(11b), set zero for gate, 32 bit interrupt gate type(1110b)  
	idt_set_gate(FIRST_INTNO, (unsigned)syscall0, 0x08, 0xEE);
}

void syscall_handler(regs_t *r) {
	if(r->int_no >= FIRST_INTNO && req_handlers[r->int_no - FIRST_INTNO]) {
		void (*fnc)() = req_handlers[r->int_no - FIRST_INTNO];
		fnc(*r);
		outb(0x20, 0x20);	// End of Interrupt signal
		return;
	}
	
	printf("Unsupported syscall: %x\nSystem halted.", r->int_no);
	for(;;);
}

void register_syscall_handler(uint32_t syscall_no, void (*fnc)()) {
	req_handlers[syscall_no] = fnc;
}
