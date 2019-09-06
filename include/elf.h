#ifndef ELF_H
#define ELF_H

#include <stdint.h>

#define Elf32_Addr	uint32_t
#define Elf32_Half	uint16_t
#define Elf32_Off	uint32_t
#define Elf32_Sword	uint32_t
#define Elf32_Word	uint32_t

#define EI_NIDENT	16
#define EI_MAG0		0
#define EI_MAG1		1
#define EI_MAG2		2
#define EI_MAG3		3
#define EI_CLASS	4
#define EI_DATA		5
#define EI_VERSION	6
#define EI_PAD		7

#define EHDR_MAG0	0x7f
#define EHDR_MAG1	'E'
#define EHDR_MAG2	'L'
#define EHDR_MAG3	'F'
#define ELF_CLASS32	1
typedef struct {
	uint8_t		e_ident[EI_NIDENT];
	Elf32_Half	e_type;
	Elf32_Half	e_machine;
	Elf32_Word	e_version;
	Elf32_Addr	e_entry;
	Elf32_Off	e_phoff;
	Elf32_Off	e_shoff;
	Elf32_Word	e_flags;
	Elf32_Half	e_ehsize;
	Elf32_Half	e_phentsize;
	Elf32_Half	e_phnum;
	Elf32_Half	e_shentsize;
	Elf32_Half	e_shnum;
	Elf32_Half	e_shstrndx;
} Elf32_Ehdr; 

typedef struct {
	Elf32_Word 	sh_name;
	Elf32_Word 	sh_type;
	Elf32_Word 	sh_flags;
	Elf32_Addr 	sh_addr;
	Elf32_Off 	sh_offset;
	Elf32_Word 	sh_size;
	Elf32_Word 	sh_link;
	Elf32_Word 	sh_info;
	Elf32_Word 	sh_addralign;
	Elf32_Word 	sh_entsize;
} Elf32_Shdr;

#define PT_NULL		0

typedef struct {
	Elf32_Word 	p_type;
	Elf32_Off 	p_offset;
	Elf32_Addr 	p_vaddr;
	Elf32_Addr 	p_paddr;
	Elf32_Word 	p_filesz;
	Elf32_Word 	p_memsz;
	Elf32_Word 	p_flags;
	Elf32_Word 	p_align;
} Elf32_Phdr;

#endif
