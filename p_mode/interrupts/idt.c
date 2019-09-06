#include "idt.h"
#include "screen.h"
#include "kheap.h"
#include "system.h"

#include <stdint.h>

// an idt entry is called a gate
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
	idt[num].base_hi = (base >> 16) & 0xFFFF;
	idt[num].base_lo = base & 0xFFFF;
	idt[num].sel = sel;
	idt[num].flags = flags;
	idt[num].always0 = 0;
}

void idt_install() {
	idt = (struct idt_entry*)kmalloc_a(sizeof(struct idt_entry) * 0x100);
	memset(idt, 0, sizeof(struct idt_entry) * 0x100);
	
	idtp.limit = (sizeof(struct idt_entry) * 0x100) - 1;
	idtp.base = (uint32_t)idt;

	isrs_install();
	irq_install();

	idt_load();
}
