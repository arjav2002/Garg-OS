bits 32
global load_regs
global get_eflags
global get_esp
load_regs:
	
get_eflags:
	pushf
	pop eax
	ret
get_esp:
	push esp
	pop eax
	ret
