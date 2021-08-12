[BITS 32]

		GLOBAL	api_fsize

[SECTION .text]

api_fsize:
		MOV		EDX,24
		MOV		EAX,[ESP+4]		; fhandle
		MOV		ECX,[ESP+8]		; mode
		INT		0x40
		RET

