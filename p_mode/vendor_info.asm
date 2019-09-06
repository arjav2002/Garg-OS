bits 32

global print_vendor_info
extern putc
extern puti

print_vendor_info:
pushad
	mov eax, 0x0
	cpuid

	ebx_loop:
		or ebx, ebx
		jz edx_loop
		pushad
			push ebx
			call putc
			add esp, 4
		popad
		shr ebx, 8
		jmp ebx_loop
	edx_loop:
		or edx, edx
		jz ecx_loop
		pushad
			push edx
			call putc
			add esp, 4
		popad
		shr edx, 8
		jmp edx_loop
	ecx_loop:
		or ecx, ecx
		jz exit_loop
		pushad
			push ecx
			call putc
			add esp, 4
		popad
		shr ecx, 8
		jmp ecx_loop
	exit_loop:
popad
ret
