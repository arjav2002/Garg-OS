#ifndef UHCI_H
#define UHCI_H

#define USBBASE_UHCI	0x20

#define USBCMD_UHCI	0x00
#define USBSTS_UHCI	0x02
#define FLBASEADDR_UHCI	0x08
#define FRNUM_UHCI	0x06

void add_uhci(uint8_t bus, uint8_t device, uint8_t function);

#endif
