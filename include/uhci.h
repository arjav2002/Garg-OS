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

struct frame_list_ptr {
    uint8_t valid   :1;
    uint8_t qh_td   :1;
    uint8_t reserved    :2;
    uint32_t flp    :28;
} __packed;
typedef struct frame_list_ptr flptr_t;

struct transfer_descriptor {
    uint8_t terminate   :1;
    uint8_t select      :1;
    uint8_t depth_breadth_first :1;
    uint8_t reserved    :1; 
    uint32_t link_pointer :28;
    uint16_t act_len    :11;
    uint8_t reserved2   :5;
    uint8_t status      :8;
    uint8_t ioc         :1;
    uint8_t ios         :1;
    uint8_t ls          :1;
    uint8_t n_errors    :2;
    uint8_t spd         :1;
    uint8_t reserved3   :2;
    uint8_t pid         :8;
    uint8_t device_addr :7;
    uint8_t end_pt      :4;
    uint8_t data_toggle :1;
    uint8_t reserved    :1;
    uint16_t max_len    :11;
    uint32_t buffer_ptr :32;
} __packed;
typedef struct transfer_descriptor td_t;

struct queue_head {
    uint8_t last_qh     :1;
    uint8_t qh_td_lptr  :1;
    uint8_t reserved    :2;
    uint32_t qhlp       :28;
    uint8_t terminate   :1;
    uint8_t qh_td_elptr :1;
    uint8_t reserved2   :2;
    uint32_t qelp       :28;
} __packed;
typedef struct queue_head qh_t;

#endif
