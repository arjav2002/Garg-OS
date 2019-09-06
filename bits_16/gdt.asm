; GDT
gdt_start:

gdt_null:
	dd 0x0
	dd 0x0
	
gdt_code:
	dw 0xffff		; limit (0:15)
	dw 0x0			; base (0:15)
	db 0x0			; base (16:23)
	db 10011010b	; access byte (present, ring 0, code/data segment, executable, can be executed from zero privillege only, readable, 0)
	db 11001111b 	; flags	limit (16:19)
	db 0x0			; base (24:31)
	
gdt_data:
	dw 0xffff	
	dw 0x0
	db 0x0
	db 10010010b
	db 11001111b
	db 0x0
	
gdt_code_user:
	dw 0xffff		; limit (0:15)
	dw 0x0			; base (0:15)
	db 0x0			; base (16:23)
	db 11111010b	; access byte (present, ring level 3, executable, can only be executed from ring 3, readable, not accessed currently)
	db 11001111b 	; flags	limit (16:19)
	db 0x0			; base (24:31)

gdt_data_user:
	dw 0xffff	
	dw 0x0
	db 0x0
	db 11110010b
	db 11001111b
	db 0x0

gdt_tss_kernel:	; to be initialised correctly by C code
	dd 0
	dd 0
gdt_end:

gdt_descriptor:
	dw gdt_end - gdt_start - 1
	dd gdt_start
	
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
