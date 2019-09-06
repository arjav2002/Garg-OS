#ifndef KHEAP_H
#define KHEAP_H

#include <stdint.h>

// these methods are to be used very judiciously for only
// permanent things because they will not be made a part of
// the heap and freeing process
uint32_t* kmalloc_a(uint32_t size);
uint32_t* kmalloc_p(uint32_t size, uint32_t* physAddr);
uint32_t* kmalloc_ap(uint32_t size, uint32_t* physAddr);
uint32_t* kmalloc(uint32_t size);

uint32_t place_addr;

#endif
