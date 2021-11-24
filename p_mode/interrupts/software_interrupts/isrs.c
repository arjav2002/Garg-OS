#include "system.h"
#include "screen.h"
#include "kheap.h"

#include <stdint.h>

#define CPUID_OP	0x0FA2

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

void **fault_handlers;

void isrs_install() {
	fault_handlers = (void**)kmalloc(sizeof(void*) * 32);
	memset(fault_handlers, 0, sizeof(void*) * 32);
	
	// 0x08 = 00000000 00001000b  	0x8E = 10001110b
	// selector comments:
	// the segment with index = 1 in the GDT, null descriptor is index zero
	// in the GDT, with ring privillege = 0.
	// flag comments:
	// present, ring privillege zero(00b), set zero for gate, 32 bit interrupt gate type(1110b)  
	idt_set_gate(0, (unsigned)isr0, 0x08, 0x8E);
	idt_set_gate(1, (unsigned)isr1, 0x08, 0x8E);
	idt_set_gate(2, (unsigned)isr2, 0x08, 0x8E);
	idt_set_gate(3, (unsigned)isr3, 0x08, 0x8E);
	idt_set_gate(4, (unsigned)isr4, 0x08, 0x8E);
	idt_set_gate(5, (unsigned)isr5, 0x08, 0x8E);
	idt_set_gate(6, (unsigned)isr6, 0x08, 0x8E);
	idt_set_gate(7, (unsigned)isr7, 0x08, 0x8E);
	idt_set_gate(8, (unsigned)isr8, 0x08, 0x8E);
	idt_set_gate(9, (unsigned)isr9, 0x08, 0x8E);
	idt_set_gate(10, (unsigned)isr10, 0x08, 0x8E);
	idt_set_gate(11, (unsigned)isr11, 0x08, 0x8E);
	idt_set_gate(12, (unsigned)isr12, 0x08, 0x8E);
	idt_set_gate(13, (unsigned)isr13, 0x08, 0x8E);
	idt_set_gate(14, (unsigned)isr14, 0x08, 0x8E);
	idt_set_gate(15, (unsigned)isr15, 0x08, 0x8E);
	idt_set_gate(16, (unsigned)isr16, 0x08, 0x8E);
	idt_set_gate(17, (unsigned)isr17, 0x08, 0x8E);
	idt_set_gate(18, (unsigned)isr18, 0x08, 0x8E);
	idt_set_gate(19, (unsigned)isr19, 0x08, 0x8E);
	idt_set_gate(20, (unsigned)isr20, 0x08, 0x8E);
	idt_set_gate(21, (unsigned)isr21, 0x08, 0x8E);
	idt_set_gate(22, (unsigned)isr22, 0x08, 0x8E);
	idt_set_gate(23, (unsigned)isr23, 0x08, 0x8E);
	idt_set_gate(24, (unsigned)isr24, 0x08, 0x8E);
	idt_set_gate(25, (unsigned)isr25, 0x08, 0x8E);
	idt_set_gate(26, (unsigned)isr26, 0x08, 0x8E);
	idt_set_gate(27, (unsigned)isr27, 0x08, 0x8E);
	idt_set_gate(28, (unsigned)isr28, 0x08, 0x8E);
	idt_set_gate(29, (unsigned)isr29, 0x08, 0x8E);
	idt_set_gate(30, (unsigned)isr30, 0x08, 0x8E);
	idt_set_gate(31, (unsigned)isr31, 0x08, 0x8E);
}

uint8_t* exception_messages[] = {
	"Division By Zero\n",
	"Debug\n",
	"Non Maskable Interrupt\n",
	"Breakpoint Exception\n",
	"Into Detected Overflow Exception\n",
	"Out of Bounds Exception\n",
	"Invalid Opcode Exception\n",
	"No Coprocessor Exception\n",
	"Double Fault Exception\n",
	"Coprocessor Segment Overrun Exception\n",
	"Bad TSS Exception\n",
	"Segment Not Present Exception\n",
	"Stack Fault Exception\n",
	"General Protection Fault Exception\n",
	"Page Fault Exception\n",
	"Unknown Interrupt Exception\n",
	"Coprocessor Fault Exception\n",
	"Alignment Check Exception (486+)\n",
	"Machine Check Exception (Pentium/586+)\n",
	"Reserved Exception 20\n",
	"Reserved Exception 21\n",
	"Reserved Exception 22\n",
	"Reserved Exception 23\n",
	"Reserved Exception 24\n",
	"Reserved Exception 25\n",
	"Reserved Exception 26\n",
	"Reserved Exception 27\n",
	"Reserved Exception 28\n",
	"Reserved Exception 29\n",
	"Reserved Exception 30\n",
	"Reserved Exception 31\n",
	"Reserved Exception 32\n"
};

void fault_handler(regs_t *r) {
	if(r->int_no < 32) {
		puts(exception_messages[r->int_no]);
		if(fault_handlers[r->int_no]) {
			void (*fnc)() = fault_handlers[r->int_no];
			fnc(*r);
			return;
		}
	}
	outb(0x20, 0x20);	// End of Interrupt signal

	printf("The state of regs:\nCS:\t%x\nEIP:\t%x\n", r->cs, r->eip);

	for(;;);
}

void register_fault_handler(uint32_t fault_no, void (*fnc)()) {
	fault_handlers[fault_no] = fnc;
}
