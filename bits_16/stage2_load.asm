N_SECS_TO_LOAD equ 0x0F
bits 16
load_stage2:
	mov ah, 0x02
	mov al, N_SECS_TO_LOAD
	mov ch, 0x00 ; cylinder head number
	mov cl, 0x02 ; start from sector
	mov dh, 0x00
	mov dl, [BOOT_DRIVE]
	mov bx, STAGE2_OFFSET
	
	push ax
	xor ax, ax
	mov es, ax
	pop ax
	
	int 0x13
	jc load_stage2_error
	
	cmp al, N_SECS_TO_LOAD
	jne load_stage2_error
	ret
	
load_stage2_error:
	mov bx, READ_ERROR_MESSAGE
	call print_string
	jmp $
	
READ_ERROR_MESSAGE: db "Could not load stage 2", 0xa, 0xd, 0x0


	
