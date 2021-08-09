[BITS 32]

		GLOBAL	api_initmalloc

[SECTION .text]

api_initmalloc:	; void api_initmalloc(void);
		PUSH	EBX

		MOV		EDX,8
		MOV		EBX,[CS:0x0020]	; start address of heap area where struct MEMMAN will be located
		MOV		EAX,EBX
		ADD		EAX,32*1024		; add 32KB (size of struct MEMMAN)
		MOV		ECX,[CS:0x0000]	; data segment size
		SUB		ECX,EAX			; size of heap area
		INT		0x40

		POP		EBX
		RET

