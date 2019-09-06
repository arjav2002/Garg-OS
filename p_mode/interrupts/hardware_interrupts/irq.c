#include "system.h"
#include "screen.h"

#include <stdint.h>

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

static void *irq_routines[16] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};

void irq_install_handler(uint8_t irq, void (*handler)(struct regs *r)) {
	irq_routines[irq] = handler;
}

void irq_uninstall_handler(uint8_t irq) {
	irq_routines[irq] = 0;
}

void irq_remap(void) {
	outb(0x20, 0x11);	// sending Initialization Command Word 1 to slave
	outb(0xA0, 0x11);	// sending Initialization Command Word 1 to master
	outb(0x21, 0x20);	// remap IRQ 0 to INT 32 (master) base (vector offset)
	outb(0xA1, 0x28);	// remap IRQ 8 to INT 40 (slave)  base (vector offset)
	outb(0x21, 0x04);	// tell master there is slave at IRQ2 (0100)
	outb(0xA1, 0x02);	// tell slave their master thinks it is at IRQ2
	outb(0x21, 0x01);	// this is 8086, master
	outb(0xA1, 0x01);	// this is 8086, slave
	outb(0x21, 0x0);	// don't ignore anything, master
	outb(0xA1, 0x0);	// don't ignore anything, slave
}

void irq_install() {
	irq_remap();
	// 0x08 = 00000000 00001000b  	0x8E = 10001110b
	// selector comments:
	// the segment with index = 1 in the GDT, null descriptor is index zero
	// in the GDT, with ring privillege = 0.
	// flag comments:
	// present, ring privillege zero(00b), set zero for gate, 32 bit interrupt gate type(1110b)  

	idt_set_gate(32, (unsigned)irq0, 0x08, 0x8E);
	idt_set_gate(33, (unsigned)irq1, 0x08, 0x8E);
	idt_set_gate(34, (unsigned)irq2, 0x08, 0x8E);
	idt_set_gate(35, (unsigned)irq3, 0x08, 0x8E);
	idt_set_gate(36, (unsigned)irq4, 0x08, 0x8E);
	idt_set_gate(37, (unsigned)irq5, 0x08, 0x8E);
	idt_set_gate(38, (unsigned)irq6, 0x08, 0x8E);
	idt_set_gate(39, (unsigned)irq7, 0x08, 0x8E);
	idt_set_gate(40, (unsigned)irq8, 0x08, 0x8E);
	idt_set_gate(41, (unsigned)irq9, 0x08, 0x8E);
	idt_set_gate(42, (unsigned)irq10, 0x08, 0x8E);
	idt_set_gate(43, (unsigned)irq11, 0x08, 0x8E);
	idt_set_gate(44, (unsigned)irq12, 0x08, 0x8E);
	idt_set_gate(45, (unsigned)irq13, 0x08, 0x8E);
	idt_set_gate(46, (unsigned)irq14, 0x08, 0x8E);
	idt_set_gate(47, (unsigned)irq15, 0x08, 0x8E);
}

void irq_handler(regs_t *r) {
	void (*handler)(struct regs *r);
	
	handler = irq_routines[r->int_no - 32];
	if(handler) {
		handler(r);
	}
	
	if(r->int_no >= 40) {
		outb(0xA0, 0x20);
	}
	
	outb(0x20, 0x20);
}
