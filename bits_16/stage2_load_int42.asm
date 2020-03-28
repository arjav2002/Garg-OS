N_SECS_TO_LOAD equ 0x000F
load_stage2:

	mov ax,WORD STAGE2_OFFSET
	mov cx,WORD 0x0001 ;AH = 42h has 0th sector as bootloader unlike AH=02h
	push ax
	pop di

	
	;reset state
	push ax
	xor ax,ax
	mov es,ax

 	int 13h
	jc load_stage2_error
	pop ax

	pusha
	push ds
	push si 

	mov word [DAP_START_SECTOR] , cx
	mov word [DAP_NUM_SECTORS] , WORD N_SECS_TO_LOAD
	mov word [DAP_OFFSET], di
	mov word [DAP_SEGMENT], es
	xor ax,ax
	mov ds, ax
	mov si, DAP
	
	;mov dl, 0x00 
	;0x80 for hard drive
	;0x00 for first floppy 
	;rather than forcing dl to a value, using the one provided by bios
	mov dl, [BOOT_DRIVE]
	mov ah, 0x42
	int 13h
	jc load_stage2_error

	pop si
	pop ds
	popa
	ret

load_stage2_error:
	mov bx, READ_ERROR_MESSAGE
	call print_string
	jmp $
	
READ_ERROR_MESSAGE: db "Could not load stage 2", 0xa, 0xd, 0x0

