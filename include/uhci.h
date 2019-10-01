#ifndef UHCI_H
#define UHCI_H

#define PCI_UHCI_BASE   0x0C
#define PCI_UHCI_SUB_CLASS  0x03
#define PCI_UHCI_PROG_IF    0x00

#define USBBASE_UHCI	0x20

#define USBCMD_UHCI	0x00
#define USBSTS_UHCI	0x02
#define FLBASEADDR_UHCI	0x08
#define FRNUM_UHCI	0x06

void add_uhci(uint8_t bus, uint8_t device, uint8_t function);

#endif
