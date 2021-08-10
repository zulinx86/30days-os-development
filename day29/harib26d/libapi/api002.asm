[BITS 32]

		GLOBAL	api_putstr

[SECTION .text]

api_putstr:	; void api_putstr(unsigned char *s);
		PUSH	EBX
		MOV		EDX,2
		MOV		EBX,[ESP+8]
		INT		0x40
		POP		EBX
		RET

