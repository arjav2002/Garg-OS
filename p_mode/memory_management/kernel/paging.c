#include "paging.h"
#include "kheap.h"
#include "system.h"
#include "screen.h"
#include "process_list.h"
#include "heap.h"

#include <stdint.h>
#include <stddef.h>

uint32_t* current_dir = NULL;

// frames is the bitset containing data of whether frame is allocated
uint32_t *frames;
uint32_t nframes = PAGES_PER_TABLE * TABLES_PER_DIRECTORY;

// returns index in frames array where information about the frame is stored for a certain frame number
uint32_t INDEX_FROM_BIT(uint32_t a) { return a/32; }
// returns offset into the value in frames at some index for a certain frame number
uint32_t OFFSET_FROM_BIT(uint32_t a) { return a%32; }
uint32_t INDEX_FROM_ADDRESS(uint32_t a) { return INDEX_FROM_BIT(a/PAGE_SIZE); }
uint32_t OFFSET_FROM_ADDRESS(uint32_t a) { return OFFSET_FROM_BIT(a/PAGE_SIZE); }

extern uint32_t place_addr;

void write_bitset(uint32_t phys_addr, uint32_t toWrite);
int32_t is_reserved_frame(uint32_t phys_addr);
uint32_t get_bitset(uint32_t phys_addr);

// all directories and page tables will be stored in identity mapped pages
// directories and page tables will only be allocated with kernel_directory as active

uint32_t* kernel_directory;

void init_paging() {
	// kernel_directory is the OS's directory for its own processes
	kernel_directory = kmalloc_a(sizeof(uint32_t*) * TABLES_PER_DIRECTORY); 
	memset(kernel_directory, 0, sizeof(uint32_t*) * TABLES_PER_DIRECTORY);
	frames = kmalloc_a(sizeof(uint32_t) * INDEX_FROM_BIT(nframes));
	memset(frames, 0, INDEX_FROM_BIT(sizeof(uint32_t) * nframes));
	
	uint32_t i = 0;
	while(i < KERNEL_MEM_END) {
		alloc_page(kernel_directory, i, 1, 1);
		i += PAGE_SIZE;
	}
	switch_page_directory(kernel_directory);
	register_fault_handler(14, page_fault_handler);
	enable_paging();
}

void enable_paging() {
	write_cr0(read_cr0() | 0x80000000);
}

void switch_page_directory(uint32_t* dir) {
	current_dir = dir;
	write_cr3((uint32_t)(current_dir));
}

// last 12 bits of vaddr will be ignored
// returns physical address of the frame allocated
uint32_t alloc_page(uint32_t *directory, uint32_t vaddr, uint32_t is_writeable, uint32_t is_kernel) {
	return alloc_page_pr(directory, vaddr, first_free_frame(), is_writeable, is_kernel, 0);
}

uint32_t first_free_frame() {
	int32_t i, j;
	
	for(i = 0; i < INDEX_FROM_BIT(nframes); i++) {
		if(frames[i] != 0xFFFFFFFF) {
			for(j = 0; j < 32; j++) {
				if((frames[i] & (1 << j)) == 0) {
					return ((i * 32) + j) * PAGE_SIZE;
				}
			}
		}
	}
	puts("No free frames! System Halted!\n");
	for(;;);

}

// adds identity mapped pages to the kernel
void add_frame_to_kernel_dir(uint32_t frame_addr) {
	uint32_t table_index = TABLE_INDEX_FROM_VIRTUAL_ADDRESS(frame_addr);
	if((kernel_directory[table_index] & 0x1) == 0) {
		kernel_directory[table_index] = (uint32_t)kmalloc_a(sizeof(uint32_t) * PAGES_PER_TABLE);
		kernel_directory[table_index] |= 0x1 + 0x2; // present, writeable, user mode access not allowed
	}
	uint32_t page_index = PAGE_INDEX_FROM_VIRTUAL_ADDRESS(frame_addr);
	uint32_t* page_table = (uint32_t*) (kernel_directory[table_index] & 0xFFFFF000);
	if(page_table[page_index] & 0x1) return;
	page_table[page_index] = frame_addr | (0x1 + 0x2); // present, writeable, user mode access not allowed
}

// alloc_page_protected_mode
uint32_t alloc_page_pr(uint32_t *directory, uint32_t vaddr, uint32_t phys_addr, uint32_t is_writeable, uint32_t is_kernel, uint32_t is_privileged) {
	// check if phys_addr already allocated or is requested frame reserved and the request is not privileged
	// this will result in false if frame is reserved and request is privileged
	// or if neither is frame privileged nor is the requestkernel_directory
	if(frames[INDEX_FROM_ADDRESS(phys_addr)] & (1 << OFFSET_FROM_ADDRESS(phys_addr))) {
		puts("\nPhysical page: ");
		puth(phys_addr);
		puts(" is not free.\n");
		return -1;
	}
	
	uint32_t tableIndex = TABLE_INDEX_FROM_VIRTUAL_ADDRESS(vaddr);
	uint32_t pageIndex = PAGE_INDEX_FROM_VIRTUAL_ADDRESS(vaddr);
	// if table is not present in memory
	if((directory[tableIndex] & 0x1) == 0) {
		uint32_t* table_addr = directory == kernel_directory? kmalloc_a(sizeof(uint32_t)*PAGES_PER_TABLE) : kmalloc(sizeof(uint32_t) * PAGES_PER_TABLE);
		directory[tableIndex] = (uint32_t)table_addr | ((is_writeable?2:0) | (is_kernel?0:4) | 1);
		memset((void*)(directory[tableIndex] & 0xFFFFF000), 0, PAGES_PER_TABLE * sizeof(uint32_t));
		// table accessible by all
		// table is writeable
		// table is present in memory
		phys_addr = first_free_frame();
		if(phys_addr == -1) {
			printf("No free frames left after allocating table_index %d of directory %x\n", tableIndex, directory);
			return -1;
		}
	}
	uint32_t table_entry = phys_addr;
	table_entry |= ((is_writeable)?2:0) + ((is_kernel)?0:4) + 1;
	uint32_t* table_ptr = (uint32_t*) (directory[tableIndex] & 0xFFFFF000); // removing flags for getting actual address of table
	table_ptr[pageIndex] = table_entry;
	write_bitset(phys_addr, 1);
	if(directory != kernel_directory) add_frame_to_kernel_dir(phys_addr);
	return phys_addr;
}

void free_page(uint32_t* directory, uint32_t vaddr) {
	uint32_t tableIndex = TABLE_INDEX_FROM_VIRTUAL_ADDRESS(vaddr);
	uint32_t pageIndex = PAGE_INDEX_FROM_VIRTUAL_ADDRESS(vaddr);
	
	uint32_t* table = (uint32_t*) directory[tableIndex];
	uint32_t phys_addr = table[pageIndex] & ~(0xFFFFF000);
	write_bitset(phys_addr, 0);
}

void write_bitset(uint32_t phys_addr, uint32_t toWrite) {
	uint32_t a = toWrite? (1 << OFFSET_FROM_ADDRESS(phys_addr)) : ~(1 << OFFSET_FROM_ADDRESS(phys_addr));
	if(toWrite) {
		frames[INDEX_FROM_ADDRESS(phys_addr)] |= a;
	} else {
		frames[INDEX_FROM_ADDRESS(phys_addr)] &= a;
	}
}

void page_fault_handler(regs_t r) {
	uint32_t errcode = r.err_code;
	uint32_t cr2 = read_cr2();
	
	printf("\nFaulty Directory: %x", read_cr3());
	printf("\nThe fault occurred at: %x", cr2);
	printf("\nThe page was %spresent\n", (errcode & 1)?"":"not ");
	printf("The fault was a %s", (errcode & 2)?"write\n":"read\n");
	printf("Processor was running in %s", (errcode & 4)?"user mode\n":"kernel mode\n");
	printf("Reserved bits were %soverwritten.\n", (errcode & 8)?"being ":"not being ");
	printf("The fault %sduring an instruction fetch.\n", (errcode & 16)?"occurred ":"did not occur ");
	printf("Bitset at that address: %d", get_bitset(cr2));
	printf("\nPage for that address: %x", get_page(current_dir, cr2));
	print_regs(r);
	printf("\nSystem Halted!");
	for(;;);
}

void print_dir(uint32_t* directory) {
	uint32_t i, j, virtual_addr = 0;
	directory = (uint32_t*)((uint32_t)directory & 0xFFFFF000);
	for(i = 0; i < TABLES_PER_DIRECTORY; i++) {
		uint32_t *table = (uint32_t*)(directory[i] & 0xFFFFF000);
		if(table) {
			for(j = 0; j < PAGES_PER_TABLE; j++) {
				if(table[j]) {
					printf("Page for Virtual Mapping %x is %x\n", virtual_addr, table[j]);
				}
				virtual_addr += PAGE_SIZE;
			}
		} else virtual_addr += PAGE_SIZE * PAGES_PER_TABLE;
	}
}

uint32_t get_page(uint32_t* directory, uint32_t vaddr) {
	uint32_t* table = (uint32_t*) directory[TABLE_INDEX_FROM_VIRTUAL_ADDRESS(vaddr)];
	table = (uint32_t*)((uint32_t)table & 0xFFFFF000); // removal of flags
	uint32_t page = table[PAGE_INDEX_FROM_VIRTUAL_ADDRESS(vaddr)];
	return page;
}

uint32_t get_bitset(uint32_t phys_addr) {
	return ((frames[INDEX_FROM_ADDRESS(phys_addr)] & (1 << OFFSET_FROM_ADDRESS(phys_addr))) >> OFFSET_FROM_ADDRESS(phys_addr));
}
