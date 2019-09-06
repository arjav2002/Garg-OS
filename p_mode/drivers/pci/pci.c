#include "pci.h"
#include "uhci.h"
#include "system.h"
#include "screen.h"
#include <stdint.h>

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

void pci_config_write(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t to_write, uint8_t amount_of_bytes) {
	uint32_t address;
	uint32_t lbus = (uint32_t) bus;
	uint32_t lslot = (uint32_t) slot;
	uint32_t lfunc = (uint32_t) func;
	/* 31		30 - 24 	23 - 16 	15 - 11 	10 - 8 		7 - 0
	Enable Bit 	Reserved 	Bus Number 	Device Number 	Function Number	Register Offset */
	// offset is into the 256 byte configuration space
	/* create configuration address */
	address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
	// write out the address
	outdw(PCI_CONFIG_ADDR, address);
	uint32_t tmp = indw(PCI_CONFIG_DATA);
	if(amount_of_bytes == 1) {
		uint8_t *ptr = (uint8_t*)(&tmp);
		ptr += offset & 3;
		*ptr = (uint8_t)to_write;
	}
	else if(amount_of_bytes == 2) {
		uint16_t *ptr = (uint16_t*)(&tmp);
		ptr += offset && 2;
		*ptr = (uint16_t)to_write;
	}
	outdw(PCI_CONFIG_ADDR, address);
	outdw(PCI_CONFIG_DATA, tmp);
}

uint16_t get_vendorID(uint8_t bus, uint8_t device, uint8_t function) {
	return (uint16_t)(pci_config_read(bus, device, function, 0, 1));
}

uint8_t get_header_type(uint8_t bus, uint8_t device, uint8_t function) {
	return (uint8_t)(pci_config_read(bus, device, function, 0xE, 1));
}

uint8_t get_base_class(uint8_t bus, uint8_t device, uint8_t function) {
	return (uint8_t)(pci_config_read(bus, device, function, 0xB, 1));
}

uint8_t get_sub_class(uint8_t bus, uint8_t device, uint8_t function) {
	return (uint8_t)(pci_config_read(bus, device, function, 0xA, 1));
}

uint8_t get_secondary_bus(uint8_t bus, uint8_t device, uint8_t function) {
	return (uint8_t)(pci_config_read(bus, device, function, 0x19, 1));
}

uint8_t get_subordinate_bus(uint8_t bus, uint8_t device, uint8_t function) {
	return (uint8_t)(pci_config_read(bus, device, function, 0x1A, 1));
}

// secondary bus is the first bus in the hierarchy that the bridge device gives access to
// subordinate bus is the last bus in the hierarchy

uint8_t get_progif(uint8_t bus, uint8_t device, uint8_t function) {
	return (uint8_t)(pci_config_read(bus, device, function, 0x9, 1));
}

void check_bus(uint8_t bus);

void check_function(uint8_t bus, uint8_t device, uint8_t function) {
	uint8_t base_class = 0;
	uint8_t sub_class = 0;
	uint8_t secondary_bus = 0;
	uint8_t prog_if = 0;

	base_class = get_base_class(bus, device, function);
	sub_class = get_sub_class(bus, device, function);
	prog_if = get_progif(bus, device, function);
	switch(base_class) {
	case 0x02:
		switch(sub_class) {
		case 0x00:
			break;
		}
		break;
	case 0x03:
		switch(sub_class) {
		case 0x00:
			switch(prog_if) {
			case 0x00:
				break;
			}
		}
		break;
	case 0x0C:
		switch(sub_class) {
		case 0x03:
			switch(get_progif(bus, device, function)) {
			case 0x00:
				add_uhci(bus, device, function);
			}
			break;
		}
		break;
	}

	/*if( (base_class == 0x06) && (sub_class == 0x04) ) {
		secondary_bus = get_secondary_bus(bus, device, function);
		check_bus(secondary_bus);
	}
	Needed to be uncommented in case we stop using the Brute Force method.	
	*/
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
