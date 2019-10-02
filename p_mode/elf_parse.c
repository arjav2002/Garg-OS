#include "elf.h"
#include "pci.h"
#include "uhci.h"

uint16_t* cursor = (uint16_t*) 0xB8000;

#define KERNEL_SECS_TO_LOAD	0x80

void putc(uint8_t c) {
	*cursor = 0x9000 | c;
	cursor++;
}

void puts(uint8_t* str) {
	uint32_t i = -1;
	while(str[++i]) putc(str[i]);
}

void puth(uint32_t a) {
	puts("0x");

	if(a == 0) {
		putc(48);
		return;
	}

	uint32_t b = 0x10000000;

	while(b > a) {
		b /= 0x10;
	}

	while(b) {
		uint8_t d = a / b;
		if(d > 9) {
			d -= 9;
			putc(64 + d);
		}
		else {
			putc((a / b) + 48);
		}
		a %= b;
		b /= 0x10;
	}
}

void memcpy(uint8_t* dest, uint8_t* src, uint32_t n_bytes) {
	uint32_t i =0;
	for(; i < n_bytes; i++)	dest[i] = src[i];
}

static inline uint8_t
inb(uint16_t port)
{
	uint8_t data;

	asm volatile("in %1,%0" : "=a" (data) : "d" (port));
	return data;
}

static inline void
insl(uint32_t port, void *addr, uint32_t cnt)
{ 	/* cld clears direction flag
	 * rep means "repeat while CX != 0"
	 * insl is GAS syntax for INSD (input double word string)
	 * INSD reads from ES:(E)DI
	 * Since DF is cleared, EDI is incremented
	 * D=EDI, c=ECX, d=EDX
	 */
	asm volatile("cld; rep insl" :
		 "=D" (addr), "=c" (cnt) :
		 "d" (port), "0" (addr), "1" (cnt) : // "0" and "1" probably refer to the 0th and 1st index in output operrands
		 "memory", "cc");
}

static inline void
outb(uint16_t port, uint8_t data)
{
	asm volatile("out %0,%1" : : "a" (data), "d" (port));
}

void waitdisk()
{
  // Wait for disk ready.
  while((inb(0x1F7) & 0xC0) != 0x40)
    ;
}

#include <stdint.h>

static uint32_t inline indw(uint16_t port) {
	uint32_t result;
	__asm__("in %%dx, %%eax": "=a"(result) : "d"(port));
	return result;
}

static void inline outdw(uint16_t port, uint32_t dword) {
	__asm__("out %%eax, %%dx"::"d"(port), "a"(dword));
}

static uint8_t inline inw(uint16_t port) {
	uint16_t result; 
	__asm__("in %%dx, %%ax" : "=a" (result) : "d" (port)); 
	return result;
}

static void inline outw(uint16_t port, uint16_t word) {
	__asm__("out %%ax, %%dx" : :"a" (word), "d" (port));
}


uint8_t bus0, device0, function0;
uint32_t io_base0, *fl_base0;

uint32_t pci_config_read(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint8_t amount_of_bytes) {
	uint32_t address;
	uint32_t lbus = (uint32_t) bus;
	uint32_t lslot = (uint32_t) slot;
	uint32_t lfunc = (uint32_t) func;
	uint32_t tmp = 0;
	/* 31		30 - 24 	23 - 16 	15 - 11 	10 - 8 		7 - 0
	Enable Bit 	Reserved 	Bus Number 	Device Number 	Function Number	Register Offset */
	// offset is into the 256 byte configuration space
	/* create configuration address */
	address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
	// write out the address
	outdw(PCI_CONFIG_ADDR, address);
	// read in the data
	// (offset & 2) * 8 = 0 will choose the first word of the 32 bit register
	tmp = indw(PCI_CONFIG_DATA);
	if(amount_of_bytes == 1) return (tmp >> (((offset & 3) * 8))) & 0xFF;
	if(amount_of_bytes == 2) return (tmp >> ((offset & 2) * 8)) & 0xFFFF;
	return tmp;
}

// for now, using single variables
void add_uhci(uint8_t bus, uint8_t device, uint8_t function) {
	bus0 = bus;
	device0 = device;
	function0 = function;
	io_base0 = pci_config_read(bus, device, function, USBBASE_UHCI, 2) & 0xFFF0;
	//reading BAR4

	fl_base0 = (uint32_t*)0x6000;
	outw(io_base0 + USBSTS_UHCI, inw(io_base0 + USBSTS_UHCI) & 0xFFBF); // setting HCHalted to zero
	outw(io_base0 + USBCMD_UHCI, inw(io_base0 + USBCMD_UHCI) & 0xFFFE); // setting USBCMD Run/Stop bit to zero
	outdw(io_base0 + FLBASEADDR_UHCI, (uint32_t)fl_base0); // setting frame list base address register
	outw(io_base0 + FRNUM_UHCI, 0);
}

void read_sector(uint32_t sector_start, void* dest) {
	 // Issue command.
	waitdisk();
	outb(0x1F2, 1);   // count = 1
	outb(0x1F3, sector_start);
	outb(0x1F4, sector_start >> 8);
	outb(0x1F5, sector_start >> 16);
	outb(0x1F6, (sector_start >> 24) | 0xE0);
	outb(0x1F7, 0x20);  // cmd 0x20 - read sectors

	// Read data.
	waitdisk();
	insl(0x1F0, dest, 512/4);
}

uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
	uint32_t address;
	uint32_t lbus = (uint32_t) bus;
	uint32_t lslot = (uint32_t) slot;
	uint32_t lfunc = (uint32_t) func;
	uint16_t tmp = 0;
	/* 31		30 - 24 	23 - 16 	15 - 11 	10 - 8 		7 - 0
	Enable Bit 	Reserved 	Bus Number 	Device Number 	Function Number	Register Offset */
	// offset is into the 256 byte configuration space
	/* create configuration address */
	address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
	// write out the address
	outdw(PCI_CONFIG_ADDR, address);
	// read in the data
	// (offset & 2) * 8 = 0 will choose the first word of the 32 bit register
	tmp = (uint16_t)((indw(PCI_CONFIG_DATA) >> ((offset & 2) * 8)) & 0xffff);
	return tmp;
}

uint16_t get_vendorID(uint8_t bus, uint8_t device, uint8_t function) {
	return pci_config_read_word(bus, device, function, 1);
}

uint8_t get_header_type(uint8_t bus, uint8_t device, uint8_t function) {
	return (uint8_t)(0xFF & pci_config_read_word(bus, device, function, 0xE));
}

uint8_t get_base_class(uint8_t bus, uint8_t device, uint8_t function) {
	return (uint8_t)( (pci_config_read_word(bus, device, function, 0xB) >> 8) & 0xff);
}

uint8_t get_sub_class(uint8_t bus, uint8_t device, uint8_t function) {
	return (uint8_t)( pci_config_read_word(bus, device, function, 0xA) & 0xff );
}

uint8_t get_secondary_bus(uint8_t bus, uint8_t device, uint8_t function) {
	return (uint8_t)( (pci_config_read_word(bus, device, function, 0x19) >> 8 ) & 0xFF );
}

uint8_t get_subordinate_bus(uint8_t bus, uint8_t device, uint8_t function) {
	return (uint8_t)( pci_config_read_word(bus, device, function, 0x1A) & 0xFF);
}

// secondary bus is the first bus in the hierarchy that the bridge device gives access to
// subordinate bus is the last bus in the hierarchy

uint8_t get_progif(uint8_t bus, uint8_t device, uint8_t function) {
	return (uint8_t)( ( pci_config_read_word(bus, device, function, 0x9) >> 8 ) & 0xFF );
}

void check_bus(uint8_t bus);

void check_function(uint8_t bus, uint8_t device, uint8_t function) {
	uint8_t base_class = 0;
	uint8_t sub_class = 0;
	uint8_t secondary_bus = 0;

	base_class = get_base_class(bus, device, function);
	sub_class = get_sub_class(bus, device, function);
	/*if( (base_class == 0x06) && (sub_class == 0x04) ) {
		secondary_bus = get_secondary_bus(bus, device, function);
		check_bus(secondary_bus);
	}
	Needed to be uncommented in case we stop using the Brute Force method.
	*/
	if(base_class == PCI_UHCI_BASE && sub_class == PCI_UHCI_SUB_CLASS && get_progif(bus, device, function) == PCI_UHCI_PROG_IF) {
		add_uhci(bus, device, function);
		puts("ur mom ge xD");
	}

}

void check_device(uint8_t bus, uint8_t device) {
	uint8_t function = 0;
	uint8_t vendorID = get_vendorID(bus, device, function);
	if(vendorID == 0xFFFF) return;
	check_function(bus, device, function);
	uint8_t header_type = get_header_type(bus, device, function);
	if( (header_type & 0x80) != 0) {
		// multi function device, so check remaining functions
		for(function = 1; function < 8; function++) {
			if(get_vendorID(bus, device, function) != 0xFFFF) {
				check_function(bus, device, function);
			}
		}
	}
}

void check_bus(uint8_t bus) {
	uint8_t device;
	for(device = 0; device < 32; device++) check_device(bus, device);
}

void check_all_buses() {
	uint16_t bus;
	uint8_t device;

	for(bus = 0; bus < 256; bus++) {
		for(device = 0; device < 32; device++) {
			check_device((uint8_t)bus, device);
		}
	}
}

void parse_ELF(uint32_t to_load_from) {
	check_all_buses();

	uint32_t buffer_start = 0xA000;
	to_load_from /= 512;
	uint32_t i;
	for(i = 0; i < KERNEL_SECS_TO_LOAD; i++)
		read_sector(to_load_from + i, (void*)(buffer_start + i*512));

	Elf32_Ehdr* main_hdr = (Elf32_Ehdr*) buffer_start;
	// if this is correct everything else must be correct anyway pfffttt...
	if(main_hdr->e_ident[EI_MAG0] != EHDR_MAG0) {
		puts("Incorrect MAG0 ");
		puth(main_hdr->e_ident[EI_MAG0]);
		return;
	}
	Elf32_Phdr* program_header_table = (Elf32_Phdr*)((uint32_t)main_hdr + (uint32_t)main_hdr->e_phoff);
	Elf32_Half ph_entry_num = main_hdr->e_phnum;
	for(Elf32_Half i = 0; i < ph_entry_num; i ++) {
		Elf32_Phdr program_header = program_header_table[i];
		if(program_header.p_type == PT_NULL) continue;
		memcpy((uint8_t*)program_header.p_paddr, (uint8_t*)((uint32_t)main_hdr + (uint32_t)program_header.p_offset), program_header.p_filesz);
		if(program_header.p_memsz > program_header.p_filesz) {
			// zero out undefined memory
			uint8_t *my_ptr = (uint8_t*)program_header.p_paddr + program_header.p_filesz;
			uint32_t i =0;
			for(; i < program_header.p_memsz-program_header.p_filesz; i++) my_ptr[i] = (uint8_t)0;
		}
	}

	for(;;);
}
