#ifndef SYSTEM_H
#define SYSTEM_H

#define KERNEL_STACK_SIZE 0x10000

#include <stdint.h>

extern uint8_t inb(uint16_t port);
extern uint32_t indw(uint16_t port);
extern void outb(uint16_t port, uint8_t byte);
extern uint8_t inw(uint16_t port);
extern void outw(uint16_t port, uint16_t word);
extern void outdw(uint16_t port, uint32_t dword);
extern void memcpy(void* dest, void* src, uint32_t n);
extern void memset(void* dest, uint8_t c, uint32_t n);
extern void idt_set_gate(uint8_t num, unsigned long base, uint16_t sel, uint8_t flags);
extern void isrs_install();
extern void idt_install();
extern void irq_install();
extern void irq_uninstall_handler(uint8_t irq);
extern void keyboard_install();
extern void timer_install();
extern void init_tss();
extern uint32_t get_eflags();
extern uint32_t get_esp();

typedef struct regs {
	uint32_t gs, fs, es, ds;
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // esp in this one is useless
	uint32_t int_no, err_code;
	uint32_t eip, cs, eflags, useresp, ss; // useresp is esp of the process being run when the interrupt is raised
} regs_t ;

/*ss
esp
eflags
cs
eip*/

typedef struct gdt_entry_struct {
	uint16_t low_limit;
	uint16_t low_base;
	uint8_t mid_base;
	uint8_t access_byte;
	uint8_t flags_limit;
	uint8_t high_base;
}__attribute__((packed)) gdt_entry;

typedef struct gdt_descriptor_struct {
	uint16_t limit;
	uint32_t base;
}__attribute__((packed)) gdt_descriptor;

typedef struct tss_struct {
	uint32_t prev_tss;   // The previous TSS - if we used hardware task switching this would form a linked list.
	uint32_t esp0;       // The stack pointer to load when we change to kernel mode.
	uint32_t ss0;        // The stack segment to load when we change to kernel mode.
	uint32_t esp1;       // everything below here is unusued now.. 
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;         
	uint32_t cs;        
	uint32_t ss;        
	uint32_t ds;        
	uint32_t fs;       
	uint32_t gs;         
	uint32_t ldt;      
	uint16_t trap;
	uint16_t iomap_base;
}__attribute__((packed)) tss_t;

extern void irq_install_handler(uint8_t irq, void (*handler)(struct regs *r));
extern void register_fault_handler(uint32_t fault_no, void (*fnc)());
extern void get_gdt_descriptor(gdt_descriptor* gdt);
extern void flush_tss();
extern void copy_regs(regs_t* new_regs, regs_t* r);
extern regs_t get_current_regs();
extern void print_regs(regs_t regs);

#endif
