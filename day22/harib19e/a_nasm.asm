[BITS 32]

		GLOBAL	api_putchar
		GLOBAL	api_putstr
		GLOBAL	api_end

[SECTION .text]

api_putchar:	; void api_putchar(int c);
		MOV		EDX,1
		MOV		AL,[ESP+4]
		INT		0x40
		RET

api_putstr:	; void api_putstr(char *s);
		PUSH	EBX
		MOV		EDX,2
		MOV		EBX,[ESP+8]
		INT		0x40
		POP		EBX
		RET

api_end:		; void api_end(void);
		MOV		EDX,4
		INT		0x40
