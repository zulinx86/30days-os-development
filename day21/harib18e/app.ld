OUTPUT_FORMAT("binary");

SECTIONS
{
	.head 0x0 : {
		LONG(128 * 1024)		/* 0x00: stack + .data + heap */
		LONG(0x69726148)		/* 0x04: signature "Hari" */
		LONG(0)					/* 0x08: size of mmarea */
		LONG(0x0400)			/* 0x0c: initial value of ESP and position where .data are transferred */
		LONG(SIZEOF(.data))		/* 0x10: size of .data */
		LONG(LOADADDR(.data))	/* 0x14: file offset of .data */
		LONG(0xE9000000)		/* 0x18: JMP instruction */
		LONG(HariMain - 0x20)	/* 0x1c: entry address */
		LONG(24 * 1024)			/* 0x20: position of heap region */
	}

	.text : { *(.text) }

	.data 0x0400 : AT ( ADDR(.text) + SIZEOF(.text) ) {
		*(.data)
		*(.rodata*)
		*(.bss)
	}

	/DISCARD/ : { *(.eh_frame) }
}
