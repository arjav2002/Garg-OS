bits 16
print_string:		; prints null terminated string stored in bx
	mov al, [bx]
	or al, al
	jz print_string_done
	mov ah, 0x0e
	int 0x10
	inc bx
	jmp print_string
print_string_done:
	ret