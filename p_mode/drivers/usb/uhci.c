#include "pci.h"
#include "uhci.h"
#include "screen.h"
#include "kheap.h"
#include "system.h"

uint8_t bus0, device0, function0;
uint32_t io_base0, *fl_base0;
// fl_base is an array of frame pointers
// a frame represents a window in time

// for now, using single variables
void add_uhci(uint8_t bus, uint8_t device, uint8_t function) {
	bus0 = bus;
	device0 = device;
	function0 = function;
	io_base0 = pci_config_read(bus, device, function, USBBASE_UHCI, 2) & 0xFFF0;
	// reading BAR4 for address

	fl_base0 = kmalloc_a(4 * 1024);
	for(uint32_t i = 0; i < 1024; i++) fl_base0[i] = 1;
	outdw(io_base0 + FLBASEADDR_UHCI, (uint32_t)fl_base0);
	outw(io_base0 + FRNUM_UHCI, 0);
}
