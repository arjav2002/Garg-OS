#ifndef PAGING_H
#define PAGING_H

#define TABLES_PER_DIRECTORY 	1024
#define PAGES_PER_TABLE		1024
#define PAGE_SIZE		4096
#define KERNEL_MEM_END		0xFF0000

#define TABLE_INDEX_FROM_VIRTUAL_ADDRESS(a) a >> 22
#define PAGE_INDEX_FROM_VIRTUAL_ADDRESS(a) (a >> 12) & 0x3FF
// 0x3FF = 0011 1111 1111b

#include "system.h"
#include <stdint.h>

void init_paging();

uint32_t* kernel_directory;

extern void write_cr3(uint32_t a);
extern void write_cr0(uint32_t a);
extern uint32_t read_cr0();
extern uint32_t read_cr2();
extern uint32_t read_cr3();
uint32_t upper_bound;

void switch_page_directory(uint32_t* page_dir);
void enable_paging();
uint32_t alloc_page(uint32_t* directory, uint32_t vaddr, uint32_t is_writeable, uint32_t is_kernel);
uint32_t alloc_page_pr(uint32_t *directory, uint32_t vaddr, uint32_t phys_addr, uint32_t is_writeable, uint32_t is_kernel, uint32_t is_privileged);
void free_page(uint32_t* directory, uint32_t vaddr);
uint32_t first_free_frame();
uint32_t* get_new_kernel_mapped_dir();

uint32_t get_page(uint32_t* directory, uint32_t vaddr);
void print_dir(uint32_t* directory);

void page_fault_handler(regs_t r);

#endif
