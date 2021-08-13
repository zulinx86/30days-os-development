[BITS 32]

		GLOBAL	api_openwin

[SECTION .text]

api_openwin:	; int api_openwin(unsigned char *buf, int xsize, int ysize, int col_inv, unsigned char *title);
		PUSH	EDI
		PUSH	ESI
		PUSH	EBX
		MOV		EDX,5
		MOV		EBX,[ESP+16]	; buf
		MOV		ESI,[ESP+20]	; xsize
		MOV		EDI,[ESP+24]	; ysize
		MOV		EAX,[ESP+28]	; col_inv
		MOV		ECX,[ESP+32]	; title
		INT		0x40
		POP		EBX
		POP		ESI
		POP		EDI
		RET

