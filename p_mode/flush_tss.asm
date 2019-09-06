bits 32
global flush_tss
flush_tss:
	mov ax, 0x2B
	ltr ax
	ret

; 0x28 as the TSS is the 6th entry in the GDT (5th index) but we set
; the bottom two bits so that Ring Privillege Level is 3
; making it 0x2B
