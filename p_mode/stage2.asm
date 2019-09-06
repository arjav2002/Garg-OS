bits 32
extern stage2_end
extern parse_ELF

cli
; enabling A20 line for 32 bit address mode using 8042 keyboard controller

; disable the keyboard
call Wait_8042_command
mov al, 0xAD
out 0x64, al

; tell keyboard controller we want to read input
call Wait_8042_command
mov al, 0xD0
out 0x64, al

; read one byte of input
call Wait_8042_data
in al, 0x60
push eax

; tell keyboard controller we want to write output
call Wait_8042_command
mov al, 0xD1
out 0x64, al

; read, enable bit 1, and write it to the keyboard controller
call Wait_8042_command
pop eax
or al, 2
out 0x60, al

; enable the keyboard
call Wait_8042_command
mov al, 0xAE
out 0x64, al

call Wait_8042_command

; load elf
push dword stage2_end
call parse_ELF
add esp, 4

;jmp $

; jmp to kernel
jmp KERNEL_OFFSET

jmp $

global KERNEL_OFFSET
KERNEL_OFFSET equ 0x20000 ; If changed, update linker.ld too

; wait till keyboard controller is ready to receive command
Wait_8042_command:
	in al, 0x64
	test al, 2	; ands al and 2
	jnz	Wait_8042_command	; jmp if ZF not equal to 0 (result is 0)
	ret
	
; wait till keyboard controller is ready to receive data
Wait_8042_data:
	in al, 0x64
	test al, 1
	jz Wait_8042_data
	ret
