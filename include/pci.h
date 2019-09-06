#ifndef PCI_H
#define PCI_H

#define PCI_CONFIG_ADDR		0xCF8
#define PCI_CONFIG_DATA		0xCFC

#include <stdint.h>

extern uint32_t pci_config_read(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint8_t amount_of_bytes);
extern void pci_config_write(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t to_write, uint8_t amount_of_bytes);
extern uint16_t get_vendorID(uint8_t bus, uint8_t device, uint8_t function);
extern uint8_t get_header_type(uint8_t bus, uint8_t device, uint8_t function);
extern uint8_t get_base_class(uint8_t bus, uint8_t device, uint8_t function);
extern uint8_t get_sub_class(uint8_t bus, uint8_t device, uint8_t function);
extern uint8_t get_secondary_bus(uint8_t bus, uint8_t device, uint8_t function);
extern uint8_t get_subordinate_bus(uint8_t bus, uint8_t device, uint8_t function);
extern uint8_t get_progif(uint8_t bus, uint8_t device, uint8_t function);
extern void check_function(uint8_t bus, uint8_t device, uint8_t function);
extern void check_device(uint8_t bus, uint8_t device);
extern void check_bus(uint8_t bus);
extern void check_all_buses();

#endif
