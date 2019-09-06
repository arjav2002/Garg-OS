C = i686-elf-gcc
ASM = nasm
ELF = -f elf -g
BIN = -f bin -g
BITS_16 = bits_16
FREESTANDING = -ffreestanding
CINCLUDE = -Iinclude -I.
ASMINCLUDE = -i$(BITS_16)/

C_INCLUDE_DIR = include
OBJDIR = bin
P_MODE = p_mode
INTERRUPTS = $(P_MODE)/interrupts
SOFTWARE_INTERRUPTS = $(INTERRUPTS)/software_interrupts
HARDWARE_INTERRUPTS = $(INTERRUPTS)/hardware_interrupts
DRIVERS = $(P_MODE)/drivers
SCREEN = $(DRIVERS)/screen
TIMER = $(DRIVERS)/timer
PCI = $(DRIVERS)/pci
USB = $(DRIVERS)/usb
KEYBOARD = $(DRIVERS)/keyboard
MEMORY_MANAGEMENT = $(P_MODE)/memory_management
KERNEL_MEM_MANAGE = $(MEMORY_MANAGEMENT)/kernel
USER_MEM_MANAGE = $(MEMORY_MANAGEMENT)/user_apps
PROCESSES = $(P_MODE)/processes
LD = i686-elf-ld
LINKER = linker.ld
STAGE2_LINKER = stage2linker.ld
OBJECTS = $(OBJDIR)/kernel_entry.o $(OBJDIR)/registers.o $(OBJDIR)/system.o $(OBJDIR)/pci.o $(OBJDIR)/uhci.o $(OBJDIR)/cr_util.o $(OBJDIR)/kheap.o $(OBJDIR)/paging.o $(OBJDIR)/screen.o $(OBJDIR)/isrs.o $(OBJDIR)/idt.o $(OBJDIR)/idtc.o $(OBJDIR)/irq.o $(OBJDIR)/timer.o $(OBJDIR)/kb.o $(OBJDIR)/vendor_info.o $(OBJDIR)/main.o $(OBJDIR)/flush_tss.o $(OBJDIR)/scheduler.o $(OBJDIR)/heap.o $(OBJDIR)/memAPI.o $(OBJDIR)/process_list.o
FILES_TO_LINK = $(OBJECTS)
FILES_TO_CAT = $(OBJDIR)/boot.bin $(OBJDIR)/stage2.bin $(OBJDIR)/kernel.o $(OBJDIR)/nullbytes.bin
STAGE2_OBJFILES = $(OBJDIR)/stage2_asm.o $(OBJDIR)/elf_parse.o

all: $(OBJDIR)/os_image.iso $(OBJDIR)/boot.bin $(OBJDIR)/stage2.bin $(OBJDIR)/nullbytes.bin $(OBJDIR)/kernel.o $(OBJECTS)

$(OBJDIR)/boot.bin : $(BITS_16)/boot.asm $(BITS_16)/gdt.asm $(BITS_16)/switch_to_pm.asm $(BITS_16)/print.asm $(BITS_16)/stage2_load.asm
	$(ASM) $(BITS_16)/boot.asm $(BIN) -o $(OBJDIR)/boot.bin $(ASMINCLUDE)

$(OBJDIR)/stage2_asm.o : $(P_MODE)/stage2.asm
	$(ASM) $(P_MODE)/stage2.asm $(ELF) -o $(OBJDIR)/stage2_asm.o

$(OBJDIR)/elf_parse.o : $(P_MODE)/elf_parse.c
	$(C) -c $(P_MODE)/elf_parse.c $(FREESTANDING) -o $(OBJDIR)/elf_parse.o $(CINCLUDE) 

$(OBJDIR)/stage2.bin : $(STAGE2_OBJFILES)
	$(LD) -o $(OBJDIR)/stage2.bin -T $(STAGE2_LINKER) $(STAGE2_OBJFILES) --oformat binary

$(OBJDIR)/idt.o : $(INTERRUPTS)/idt.asm
	$(ASM) $(INTERRUPTS)/idt.asm $(ELF) -o $(OBJDIR)/idt.o

$(OBJDIR)/cr_util.o : $(P_MODE)/cr_util.asm
	$(ASM) $(P_MODE)/cr_util.asm $(ELF) -o $(OBJDIR)/cr_util.o

$(OBJDIR)/vendor_info.o : $(P_MODE)/vendor_info.asm
	$(ASM) $(P_MODE)/vendor_info.asm $(ELF) -o $(OBJDIR)/vendor_info.o

$(OBJDIR)/kernel_entry.o : $(P_MODE)/kernel_entry.asm
	$(ASM) $(P_MODE)/kernel_entry.asm $(ELF) -o $(OBJDIR)/kernel_entry.o

$(OBJDIR)/flush_tss.o : $(P_MODE)/flush_tss.asm
	$(ASM) $(P_MODE)/flush_tss.asm $(ELF) -o $(OBJDIR)/flush_tss.o

$(OBJDIR)/nullbytes.bin : nullbytes.asm
	$(ASM) nullbytes.asm $(BIN) -o $(OBJDIR)/nullbytes.bin

$(OBJDIR)/registers.o : $(P_MODE)/registers.asm
	$(ASM) $(P_MODE)/registers.asm $(ELF) -o $(OBJDIR)/registers.o

$(OBJDIR)/main.o : $(P_MODE)/main.c $(C_INCLUDE_DIR)/screen.h $(C_INCLUDE_DIR)/system.h $(C_INCLUDE_DIR)/paging.h $(C_INCLUDE_DIR)/idt.h $(C_INCLUDE_DIR)/timer.h
	$(C) -c $(P_MODE)/main.c $(FREESTANDING) -o $(OBJDIR)/main.o $(CINCLUDE)

$(OBJDIR)/system.o : $(P_MODE)/system.c $(C_INCLUDE_DIR)/system.h $(C_INCLUDE_DIR)/screen.h $(C_INCLUDE_DIR)/kheap.h
	$(C) -c $(P_MODE)/system.c $(FREESTANDING) -o $(OBJDIR)/system.o $(CINCLUDE)

$(OBJDIR)/screen.o : $(SCREEN)/screen.c $(C_INCLUDE_DIR)/screen.h $(C_INCLUDE_DIR)/system.h
	$(C) -c $(SCREEN)/screen.c $(FREESTANDING) -o $(OBJDIR)/screen.o $(CINCLUDE)

$(OBJDIR)/timer.o : $(TIMER)/timer.c $(C_INCLUDE_DIR)/system.h $(C_INCLUDE_DIR)/screen.h $(C_INCLUDE_DIR)/timer.h
	$(C) -c $(TIMER)/timer.c $(FREESTANDING) -o $(OBJDIR)/timer.o  $(CINCLUDE)

$(OBJDIR)/kb.o : $(KEYBOARD)/kb.c $(C_INCLUDE_DIR)/kb.h $(C_INCLUDE_DIR)/screen.h $(C_INCLUDE_DIR)/system.h $(C_INCLUDE_DIR)/screen_control.h
	$(C) -c $(KEYBOARD)/kb.c $(FREESTANDING) -o $(OBJDIR)/kb.o  $(CINCLUDE)

$(OBJDIR)/idtc.o : $(INTERRUPTS)/idt.c $(C_INCLUDE_DIR)/idt.h $(C_INCLUDE_DIR)/screen.h $(C_INCLUDE_DIR)/kheap.h $(C_INCLUDE_DIR)/system.h
	$(C) -c $(INTERRUPTS)/idt.c $(FREESTANDING) -o $(OBJDIR)/idtc.o $(CINCLUDE)

$(OBJDIR)/isrs.o : $(SOFTWARE_INTERRUPTS)/isrs.c $(C_INCLUDE_DIR)/system.h $(C_INCLUDE_DIR)/screen.h $(C_INCLUDE_DIR)/kheap.h
	$(C) -c $(SOFTWARE_INTERRUPTS)/isrs.c $(FREESTANDING) -o $(OBJDIR)/isrs.o $(CINCLUDE)

$(OBJDIR)/irq.o : $(HARDWARE_INTERRUPTS)/irq.c
	$(C) -c $(HARDWARE_INTERRUPTS)/irq.c $(FREESTANDING) -o $(OBJDIR)/irq.o $(CINCLUDE)

$(OBJDIR)/pci.o : $(PCI)/pci.c
	$(C) -c $(PCI)/pci.c $(FREESTANDING) -o $(OBJDIR)/pci.o $(CINCLUDE)

$(OBJDIR)/uhci.o : $(USB)/uhci.c
	$(C) -c $(USB)/uhci.c $(FREESTANDING) -o $(OBJDIR)/uhci.o $(CINCLUDE)

$(OBJDIR)/kheap.o : $(KERNEL_MEM_MANAGE)/kheap.c
	$(C) -c $(KERNEL_MEM_MANAGE)/kheap.c $(FREESTANDING) -o $(OBJDIR)/kheap.o $(CINCLUDE)

$(OBJDIR)/paging.o : $(KERNEL_MEM_MANAGE)/paging.c
	$(C) -c $(KERNEL_MEM_MANAGE)/paging.c $(FREESTANDING) -o $(OBJDIR)/paging.o $(CINCLUDE)

$(OBJDIR)/heap.o : $(KERNEL_MEM_MANAGE)/heap.c
	$(C) -c $(KERNEL_MEM_MANAGE)/heap.c $(FREESTANDING) -o $(OBJDIR)/heap.o $(CINCLUDE)

$(OBJDIR)/memAPI.o : $(USER_MEM_MANAGE)/memAPI.c
	$(C) -c $(USER_MEM_MANAGE)/memAPI.c $(FREESTANDING) -o $(OBJDIR)/memAPI.o $(CINCLUDE)

$(OBJDIR)/scheduler.o : $(PROCESSES)/scheduler.c
	$(C) -c $(PROCESSES)/scheduler.c $(FREESTANDING) -o $(OBJDIR)/scheduler.o $(CINCLUDE)

$(OBJDIR)/process_list.o : $(PROCESSES)/process_list.c
	$(C) -c $(PROCESSES)/process_list.c $(FREESTANDING) -o $(OBJDIR)/process_list.o $(CINCLUDE)

$(OBJDIR)/kernel.o : $(LINKER) $(FILES_TO_LINK)
	$(LD) -o $(OBJDIR)/kernel.o -T $(LINKER) $(FILES_TO_LINK)

$(OBJDIR)/kernel.bin : $(LINKER) $(FILES_TO_LINK)
	$(LD) -o $(OBJDIR)/kernel.bin -T $(LINKER) $(FILES_TO_LINK) --oformat binary

$(OBJDIR)/os_image.iso : $(FILES_TO_CAT)
	cat $(FILES_TO_CAT) > $(OBJDIR)/os_image.iso 
