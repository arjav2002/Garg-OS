#include "system.h"
#include "screen.h"
#include "kheap.h"
#include <stdint.h>

tss_t tss_entry;
void* default_kernel_stack_ptr;
extern void load_regs(regs_t*);

/*
 * asm("asm code" : output : input : (clobbered) regs whose value needs to be preserved from before and till after executing asm);
 * In "=a", equals means that ASM code does not care about intial value of eax register
 */

uint8_t inb(uint16_t port) {
	uint8_t result;
	__asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
	return result;
}

void outb(uint16_t port, uint8_t byte) {
	__asm__("out %%al, %%dx" : :"a" (byte), "d" (port));
}

uint8_t inw(uint16_t port) {
	uint16_t result; 
	__asm__("in %%dx, %%ax" : "=a" (result) : "d" (port)); 
	return result;
}

uint32_t indw(uint16_t port) {
	uint32_t result;
	__asm__("in %%dx, %%eax": "=a"(result) : "d"(port));
	return result;
}

void outdw(uint16_t port, uint32_t dword) {
	__asm__("out %%eax, %%dx"::"d"(port), "a"(dword));
}

void outw(uint16_t port, uint16_t word) {
	__asm__("out %%ax, %%dx" : :"a" (word), "d" (port));
}

void memcpy(void *dest, void *src, uint32_t n) {
	uint32_t i;
	uint8_t* ptr1 = (uint8_t*) dest;
	uint8_t* ptr2 = (uint8_t*) src;
	for(i = 0; i < n; i++) {
		ptr1[i] = ptr2[i];
	}
}

void memset(void* dest, uint8_t c, uint32_t n) {
	uint32_t i;
	uint8_t* ptr1 = (uint8_t*) dest;
	for(i = 0; i < n; i++) {
		ptr1[i] = c;
	}
}

void init_tss() {
	gdt_descriptor gdt_desc;
	get_gdt_descriptor(&gdt_desc);
	// GET GDT DESCRIPTOR

	gdt_entry* gdt_entries = (gdt_entry*)(gdt_desc.base);
	gdt_entry* tss_gdt_entry = &(gdt_entries[5]);
	// GET ACCESS TO THE TSS ENTRY IN THE GDT

	// CREATE A TSS ENTRY
	// INITIALISE THE GDT TSS ENTRY
	uint32_t base = (uint32_t) &tss_entry;
	uint32_t limit = (uint32_t) sizeof(tss_entry);
	tss_gdt_entry->low_limit = limit & 0xFFFF;
	tss_gdt_entry->low_base = base & 0xFFFF;
	tss_gdt_entry->mid_base = (base>>16) & 0xFF;
	tss_gdt_entry->high_base = (base>>24) & 0xFF;
	tss_gdt_entry->access_byte = 0xE9;// 11101001b
	tss_gdt_entry->flags_limit = 0x40 + ((limit>>16) & 0xF);
	
	//SET THE VALUES IN THE TSS
	default_kernel_stack_ptr = (void*) kmalloc(KERNEL_STACK_SIZE);
	memset(&tss_entry, 0, sizeof(tss_entry));
	tss_entry.ss0 = 0x10;
	tss_entry.esp0 = (uint32_t) default_kernel_stack_ptr;
	tss_entry.iomap_base = sizeof(tss_entry);
	flush_tss();
}

// each process has its own kernel stack
void set_kernel_stack(void *stack_ptr) {
	tss_entry.esp0 = (uint32_t) stack_ptr;
}

void get_gdt_descriptor(gdt_descriptor* gdt) {
	__asm__("sgdt %0\n" : : "m"(*gdt));
}

void copy_regs(regs_t* new_regs, regs_t* r) {
	r->gs = new_regs->gs;
	r->fs = new_regs->fs;
	r->ds = new_regs->ds;
	r->es = new_regs->es;
	r->eip = new_regs->eip;
	r->cs = new_regs->cs;
	r->eflags = new_regs->eflags;
	r->useresp = new_regs->useresp;
	r->ss = new_regs->ss;
	r->edi = new_regs->edi;
	r->esi = new_regs->esi;
	r->ebp = new_regs->ebp;
	r->esp = new_regs->esp;
	r->ebx = new_regs->ebx;
	r->edx = new_regs->edx;
	r->ecx = new_regs->ecx;
	r->eax = new_regs->eax;
	r->int_no = new_regs->int_no;
	r->err_code = new_regs->err_code;
}

regs_t get_current_regs() {
	regs_t current_regs;
	
	load_regs(&current_regs);

	return current_regs;
}

void print_regs(regs_t regs) {
	printf("CS: %x\tDS: %x\tES: %x\tFS: %x\nGS: %x\tEDI: %x\tESI: %x\tEBP: %x\tESP: %x\nEBX: %x\tEDX: %x\tECX: %x\tEAX: %x\nINT_NO: %x\tERR_CODE: %x\tEIP: %x\nEFLAGS: %x\tUSERESP: %x\tSS: %x\t", regs.cs, regs.ds, regs.es, regs.fs, regs.gs, regs.edi, regs.esi, regs.ebp, regs.esp, regs.ebx, regs.edx, regs.ecx, regs.eax, regs.int_no, regs.err_code, regs.eip, regs.eflags, regs.useresp, regs.ss);
}
