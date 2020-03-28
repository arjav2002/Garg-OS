[org 0x7c00]
[bits 16]
jmp boot_main
db 0x0

bpbOEM:					db	'Garg OS '
bpbBytesPerSector:  	DW 	512
bpbSectorsPerCluster: 	DB 	1
bpbReservedSectors: 	DW 	50
bpbNumberOfFATs: 	    DB 	2
bpbRootEntries: 	    DW 	224
bpbTotalSectors: 	    DW 	2880
bpbMedia: 	            DB 	0xF0
bpbSectorsPerFAT: 	    DW 	9
bpbSectorsPerTrack: 	DW 	18
bpbHeadsPerCylinder: 	DW 	2
bpbHiddenSectors: 	    DD 	0
bpbTotalSectorsBig:     DD 	0
bsDriveNumber: 	        DB 	0
bsUnused: 	            DB 	0
bsExtBootSignature: 	DB 	0x29
bsSerialNumber:	        DD 	0xa0a1a2a3
bsVolumeLabel: 	        DB 	"           "
bsFileSystem: 	        DB 	"        "

boot_main:
mov [BOOT_DRIVE], dl

;Rather than hardcoding the boot-drive, using the one provided by the BIOS


xor ax, ax
mov ds, ax
mov es, ax
mov ss, ax

mov bp, 0x9000
mov sp, bp

mov bx, MSG_REAL_MODE
call print_string

call load_stage2

; disable blinking text for VGA mode
	mov ax, 0x1003
	mov bl, 0x00
	int 0x10
	
;RSDP code
	mov ax, [0x40E]
	
call switch_to_pm	; never gonna return from here

jmp $

%include "print.asm"
%include "stage2_load.asm"
%include "gdt.asm"
%include "switch_to_pm.asm"
	
bits 32

BEGIN_PM:
	call STAGE2_OFFSET
	
	jmp $

STAGE2_OFFSET: equ 0x9000	; if changed, update stage2linker.ld too
MSG_REAL_MODE: db "Started in 16 bit Real Mode", 0xa, 0xd, 0x0
BOOT_DRIVE: db 0
HELLO_MSG: db "Hello, World!", 0xa, 0xd, 0x0
MSG_PRINTED: db "Message Printed!", 0xa, 0xd, 0x0
	
times 510 - ($ - $$) db 0
dw 0xaa55
