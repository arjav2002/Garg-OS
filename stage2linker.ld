SECTIONS
{
	.text 0x9000 :
	{
		code = .; _code = .; __code = .;
		*(.text)
		. = ALIGN(512);
	}

	.data :
	{
		data = .; _data = .; __data = .;
		*(.data)
		*(.rodata)
		. = ALIGN(512);
	}

	.bss :
	{
		bss = .; _bss = .; __bss = .;
		*(.bss)
		. = ALIGN(512);
	}
	stage2_end = . - 0x9000 + 0x200; _stage2_end = . - 0x9000 + 0x200; __stage2_end = . - 0x9000 + 0x200;
}
