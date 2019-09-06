#include "screen.h"
#include "kheap.h"

#include <stdint.h>

uint32_t place_addr = 0xF0000;

uint32_t kmalloc_all(uint32_t size, int align, uint32_t* physAddr) {
	if(align && (place_addr % 0x1000)) while(place_addr % 0x1000) place_addr++;
	if(physAddr) {
		*physAddr = place_addr;
	}
	uint32_t toRet = place_addr;
	place_addr += size;
	return toRet;
}

// return memory block aligned with 0x1000
uint32_t* kmalloc_a(uint32_t size) {
	return (uint32_t*)kmalloc_all(size, 1, 0);
}

uint32_t* kmalloc_ap(uint32_t size, uint32_t* physAddr) {
	return (uint32_t*)kmalloc_all(size, 1, physAddr);
}

uint32_t* kmalloc(uint32_t size) {
	return (uint32_t*)kmalloc_all(size, 0, 0);
}
