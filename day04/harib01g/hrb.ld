OUTPUT_FORMAT("binary");

SECTIONS
{
	.head 0x0: {
		LONG(64 * 1024)			/* stack + .data + heap */
		LONG(0x69726148)		/* signature "Hari" */
		LONG(0)					/* size of mmarea */
		LONG(0x310000)			/* position where .data is transferred */
		LONG(SIZEOF(.data))		/* .data size */
		LONG(LOADADDR(.data))	/* file offset of .data */
		LONG(0xE9000000)
		LONG(HariMain - 0x20)	/* entry address */
		LONG(0)					/* position where heap region */
	}

	.text : { *(.text) }

	.data 0x310000 : AT ( ADDR(.text) + SIZEOF(.text) ) {
		*(.data)
		*(.rodata*)
		*(.bss)
	}

	/DISCARD/ : { *(.eh_frame) }
}
