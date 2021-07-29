OUTPUT_FORMAT("binary");

SECTIONS
{
	.head 0x0 : {
		LONG(128 * 1024)		/*  0: stack + .data + heap */
		LONG(0x69726148)		/*  4: signature "Hari" */
		LONG(0)					/*  8: size of mmarea */
		LONG(0x0400)			/* 12: position where .data and stack are transferred */
		LONG(SIZEOF(.data))		/* 16: size of .data */
		LONG(LOADADDR(.data))	/* 20: file offset of .data */
		LONG(0xE9000000)
		LONG(HariMain - 0x20)	/* 28: entry address */
		LONG(24 * 1024)			/* 32: position of heap region */
	}

	.text : { *(.text) }

	.data 0x0400 : AT ( ADDR(.text) + SIZEOF(.text) ) {
		*(.data)
		*(.rodata*)
		*(.bss)
	}

	/DISCARD/ : { *(.eh_frame) }
}
