; nasmfunc
; TAB=4

[BITS 32]

		GLOBAL	io_hlt


[SECTION .text]

io_hlt:	; void io_hlt(void);
		HLT
		RET
