; nasmfunc
; TAB=4

[BITS 32]

		GLOBAL	io_hlt,io_cli,io_sti,io_stihlt
		GLOBAL	io_in8,io_in16,io_in32
		GLOBAL	io_out8,io_out16,io_out32
		GLOBAL	io_load_eflags,io_store_eflags
		GLOBAL	load_gdtr,load_idtr
		GLOBAL	load_cr0,store_cr0
		GLOBAL	load_tr
		GLOBAL	asm_inthandler0d
		GLOBAL	asm_inthandler20,asm_inthandler21
		GLOBAL	asm_inthandler27,asm_inthandler2c
		GLOBAL	memtest_sub
		GLOBAL	farjmp,farcall
		GLOBAL	asm_hrb_api,start_app
		EXTERN	inthandler0d
		EXTERN	inthandler20,inthandler21
		EXTERN	inthandler27,inthandler2c
		EXTERN	hrb_api

[SECTION .text]

io_hlt:		; void io_hlt(void);
		HLT
		RET

io_cli:		; void io_cli(void);
		CLI
		RET

io_sti:		; void io_sti(void);
		STI
		RET

io_stihlt:	; void io_stihlt(void);
		STI
		HLT
		RET

io_in8:		; int io_in8(int port);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AL,DX
		RET

io_in16:	; int io_in16(int port);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AX,DX
		RET

io_in32:	; int io_in32(int port);
		MOV		EDX,[ESP+4]		; port
		IN		EAX,DX
		RET

io_out8:	; void io_out8(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		AL,[ESP+8]		; data
		OUT		DX,AL
		RET

io_out16:	; void io_out16(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		AX,[ESP+8]		; data
		OUT		DX,AX
		RET

io_out32:	; void io_out32(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,[ESP+8]		; data
		OUT		DX,EAX
		RET

io_load_eflags:		; int io_load_eflags(void);
		PUSHFD		; PUSH EFLAGS という意味
		POP		EAX
		RET

io_store_eflags:	; void io_store_eflags(int eflags);
		MOV		EAX,[ESP+4]
		PUSH	EAX
		POPFD		; POP EFLAGS という意味
		RET

load_gdtr:			; void load_gdtr(int limit, int addr);
		MOV		AX,[ESP+4]
		MOV		[ESP+6],AX
		LGDT	[ESP+6]
		RET

load_idtr:			; void load_idtr(int limit, int addr);
		MOV		AX,[ESP+4]
		MOV		[ESP+6],AX
		LIDT	[ESP+6]
		RET

load_cr0:			; int load_cr0(void);
		MOV		EAX,CR0
		RET

store_cr0:			; void store_cr0(int cr0);
		MOV		EAX,[ESP+4]
		MOV		CR0,EAX
		RET

load_tr:			; void load_tr(int tr);
		LTR		[ESP+4]
		RET

asm_inthandler20:
		PUSH	ES
		PUSH	DS
		PUSHAD

		MOV		AX,SS
		CMP		AX,1*8
		JNE		.from_app

		MOV		EAX,ESP
		PUSH	SS
		PUSH	EAX
		; registers when interrupted
		; [ESP+ 0]: ESP
		; [ESP+ 4]: SS
		; [ESP+ 8]: EDI
		; [ESP+12]: ESI
		; [ESP+16]: EBP
		; [ESP+20]: ESP
		; [ESP+24]: EBX
		; [ESP+28]: EDX
		; [ESP+32]: ECX
		; [ESP+36]: EAX
		; [ESP+40]: DS
		; [ESP+44]: ES

		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	inthandler20

		ADD		ESP,8
		POPAD
		POP		DS
		POP		ES

		IRETD
.from_app:
		; set ECX as ESP of OS
		MOV		EAX,1*8
		MOV		DS,AX
		MOV		ECX,[0x0fe4]

		ADD		ECX,-8
		MOV		[ECX+4],SS
		MOV		[ECX  ],ESP
		MOV		SS,AX
		MOV		ES,AX
		MOV		ESP,ECX
		; stack of OS
		; [ESP +0],ESP
		; [ESP +4],SS

		CALL	inthandler20

		POP		ECX
		POP		EAX
		MOV		SS,AX
		MOV		ESP,ECX
		POPAD
		POP		DS
		POP		ES

		IRETD

asm_inthandler0d:
		STI
		PUSH	ES
		PUSH	DS
		PUSHAD

		MOV		AX,SS
		CMP		AX,1*8
		JNE		.from_app

		MOV		EAX,ESP
		PUSH	SS
		PUSH	EAX
		; registers when interrupted
		; [ESP+ 0]: ESP
		; [ESP+ 4]: SS
		; [ESP+ 8]: EDI
		; [ESP+12]: ESI
		; [ESP+16]: EBP
		; [ESP+20]: ESP
		; [ESP+24]: EBX
		; [ESP+28]: EDX
		; [ESP+32]: ECX
		; [ESP+36]: EAX
		; [ESP+40]: DS
		; [ESP+44]: ES

		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	inthandler0d

		ADD		ESP,8
		POPAD
		POP		DS
		POP		ES
		ADD		ESP,4

		IRETD
.from_app:
		CLI

		; set ECX as ESP of OS
		MOV		EAX,1*8
		MOV		DS,AX
		MOV		ECX,[0x0fe4]

		ADD		ECX,-8
		MOV		[ECX+4],SS
		MOV		[ECX  ],ESP
		MOV		SS,AX
		MOV		ES,AX
		MOV		ESP,ECX
		; stack of OS
		; [ESP +0],ESP
		; [ESP +4],SS

		STI
		CALL	inthandler0d
		CLI

		CMP		EAX,0
		JNE		.kill

		POP		ECX
		POP		EAX
		MOV		SS,AX
		MOV		ESP,ECX

		POPAD
		POP		DS
		POP		ES
		ADD		ESP,4

		IRETD
.kill:
		MOV		EAX,1*8
		MOV		ES,AX
		MOV		SS,AX
		MOV		DS,AX
		MOV		FS,AX
		MOV		GS,AX
		MOV		ESP,[0x0fe4]
		STI
		POPAD
		RET


asm_inthandler21:
		PUSH	ES
		PUSH	DS
		PUSHAD

		MOV		AX,SS
		CMP		AX,1*8
		JNE		.from_app

		MOV		EAX,ESP
		PUSH	SS
		PUSH	EAX
		; registers when interrupted
		; [ESP+ 0]: ESP
		; [ESP+ 4]: SS
		; [ESP+ 8]: EDI
		; [ESP+12]: ESI
		; [ESP+16]: EBP
		; [ESP+20]: ESP
		; [ESP+24]: EBX
		; [ESP+28]: EDX
		; [ESP+32]: ECX
		; [ESP+36]: EAX
		; [ESP+40]: DS
		; [ESP+44]: ES

		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	inthandler21

		ADD		ESP,8
		POPAD
		POP		DS
		POP		ES

		IRETD
.from_app:
		; set ECX as ESP of OS
		MOV		EAX,1*8
		MOV		DS,AX
		MOV		ECX,[0x0fe4]

		ADD		ECX,-8
		MOV		[ECX+4],SS
		MOV		[ECX  ],ESP
		MOV		SS,AX
		MOV		ES,AX
		MOV		ESP,ECX
		; stack of OS
		; [ESP +0],ESP
		; [ESP +4],SS

		CALL	inthandler21

		POP		ECX
		POP		EAX
		MOV		SS,AX
		MOV		ESP,ECX
		POPAD
		POP		DS
		POP		ES

		IRETD

asm_inthandler27:
		PUSH	ES
		PUSH	DS
		PUSHAD

		MOV		AX,SS
		CMP		AX,1*8
		JNE		.from_app

		MOV		EAX,ESP
		PUSH	SS
		PUSH	EAX
		; registers when interrupted
		; [ESP+ 0]: ESP
		; [ESP+ 4]: SS
		; [ESP+ 8]: EDI
		; [ESP+12]: ESI
		; [ESP+16]: EBP
		; [ESP+20]: ESP
		; [ESP+24]: EBX
		; [ESP+28]: EDX
		; [ESP+32]: ECX
		; [ESP+36]: EAX
		; [ESP+40]: DS
		; [ESP+44]: ES

		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	inthandler27

		ADD		ESP,8
		POPAD
		POP		DS
		POP		ES

		IRETD
.from_app:
		; set ECX as ESP of OS
		MOV		EAX,1*8
		MOV		DS,AX
		MOV		ECX,[0x0fe4]

		ADD		ECX,-8
		MOV		[ECX+4],SS
		MOV		[ECX  ],ESP
		MOV		SS,AX
		MOV		ES,AX
		MOV		ESP,ECX
		; stack of OS
		; [ESP +0],ESP
		; [ESP +4],SS

		CALL	inthandler27

		POP		ECX
		POP		EAX
		MOV		SS,AX
		MOV		ESP,ECX
		POPAD
		POP		DS
		POP		ES

		IRETD

asm_inthandler2c:
		PUSH	ES
		PUSH	DS
		PUSHAD

		MOV		AX,SS
		CMP		AX,1*8
		JNE		.from_app

		MOV		EAX,ESP
		PUSH	SS
		PUSH	EAX
		; registers when interrupted
		; [ESP+ 0]: ESP
		; [ESP+ 4]: SS
		; [ESP+ 8]: EDI
		; [ESP+12]: ESI
		; [ESP+16]: EBP
		; [ESP+20]: ESP
		; [ESP+24]: EBX
		; [ESP+28]: EDX
		; [ESP+32]: ECX
		; [ESP+36]: EAX
		; [ESP+40]: DS
		; [ESP+44]: ES

		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	inthandler2c

		ADD		ESP,8
		POPAD
		POP		DS
		POP		ES

		IRETD
.from_app:
		; set ECX as ESP of OS
		MOV		EAX,1*8
		MOV		DS,AX
		MOV		ECX,[0x0fe4]

		ADD		ECX,-8
		MOV		[ECX+4],SS
		MOV		[ECX  ],ESP
		MOV		SS,AX
		MOV		ES,AX
		MOV		ESP,ECX
		; stack of OS
		; [ESP +0],ESP
		; [ESP +4],SS

		CALL	inthandler2c

		POP		ECX
		POP		EAX
		MOV		SS,AX
		MOV		ESP,ECX
		POPAD
		POP		DS
		POP		ES

		IRETD

memtest_sub:		; unsigned int memtest_sub(unsigned int start, unsigned end)
		PUSH	EDI
		PUSH	ESI
		PUSH	EBX
		MOV		EAX,[ESP+12+4]			; i = start;
		MOV		ESI,0xaa55aa55			; pat0 = 0xaa55aa55;
		MOV		EDI,0x55aa55aa			; pat1 = 0x55aa55aa;
mts_loop:
		MOV		EBX,EAX
		ADD		EBX,0xffc				; p = i + 0xffc;
		MOV		EDX,[EBX]				; old = *p;
		MOV		[EBX],ESI				; *p = pat0;
		XOR		DWORD [EBX],0xffffffff	; *p ^= 0xffffffff;
		CMP		EDI,[EBX]				; if (*p != pat1) goto mts_fin;
		JNE		mts_fin
		XOR		DWORD [EBX],0xffffffff	; *p ^= 0xffffffff;
		CMP		ESI,[EBX]				; if (*p != pat0) goto mts_fin;
		JNE		mts_fin
		MOV		[EBX],EDX				; *p = old;
		ADD		EAX,0x1000				; i += 0x1000;
		CMP		EAX,[ESP+12+8]			; if (i <= end) goto mts_loop;
		JBE		mts_loop
		POP		EBX
		POP		ESI
		POP		EDI
		RET
mts_fin:
		MOV		[EBX],EDX				; *p = old;
		POP		EBX
		POP		ESI
		POP		EDI
		RET

farjmp:				; void farjmp(int eip, int cs);
		JMP		FAR [ESP+4]				; eip, cs
		RET

farcall:			; void farcall(int eip, int cs);
		CALL	FAR [ESP+4]
		RET

asm_hrb_api:
		PUSH	DS
		PUSH	ES
		PUSHAD
		; [ESP+ 0]: EDI
		; [ESP+ 4]: ESI
		; [ESP+ 8]: EBP
		; [ESP+12]: ESP
		; [ESP+16]: EBX
		; [ESP+20]: EDX
		; [ESP+24]: ECX
		; [ESP+28]: EAX
		; [ESP+32]: DS
		; [ESP+36]: ES

		MOV		EAX,1*8
		MOV		DS,AX
		MOV		ECX,[0x0fe4]	; ESP of OS
		ADD		ECX,-40
		MOV		[ECX+32],ESP	; ESP from app
		MOV		[ECX+36],SS		; SS  from app

		MOV		EDX,[ESP   ]	; EDI from app
		MOV		EBX,[ESP+ 4]	; ESI from app
		MOV		[ECX   ],EDX
		MOV		[ECX+ 4],EBX
		MOV		EDX,[ESP+ 8]	; EBP from app
		MOV		EBX,[ESP+12]	; ESP from app
		MOV		[ECX+ 8],EDX
		MOV		[ECX+12],EBX
		MOV		EDX,[ESP+16]	; EBX from app
		MOV		EBX,[ESP+20]	; EDX from app
		MOV		[ECX+16],EDX
		MOV		[ECX+20],EBX
		MOV		EDX,[ESP+24]	; ECX from app
		MOV		EBX,[ESP+28]	; EAX from app
		MOV		[ECX+24],EDX
		MOV		[ECX+28],EBX

		MOV		ES,AX
		MOV		SS,AX
		MOV		ESP,ECX
		; [ESP+ 0]: EDI
		; [ESP+ 4]: ESI
		; [ESP+ 8]: EBP
		; [ESP+12]: ESP
		; [ESP+16]: EBX
		; [ESP+20]: EDX
		; [ESP+24]: ECX
		; [ESP+28]: EAX
		; [ESP+32]: ESP
		; [ESP+36]: SS

		STI
		CALL	hrb_api
		
		MOV		ECX,[ESP+32]	; restore ESP
		MOV		EAX,[ESP+36]	; restore SS
		CLI
		MOV		SS,AX
		MOV		ESP,ECX
		POPAD
		POP		ES
		POP		DS

		IRETD

start_app:			; void start_app(int eip, int cs, int esp, int ds);
		PUSHAD
		MOV		EAX,[ESP+36]	; EIP for app
		MOV		ECX,[ESP+40]	; CS  for app
		MOV		EDX,[ESP+44]	; ESP for app
		MOV		EBX,[ESP+48]	; DS/SS for app
		; [ESP+ 0]: EDI
		; [ESP+ 4]: ESI
		; [ESP+ 8]: EBP
		; [ESP+12]: ESP
		; [ESP+16]: EBX
		; [ESP+20]: EDX
		; [ESP+24]: ECX
		; [ESP+28]: EAX
		; [ESP+32]: return address 
		; [ESP+36]: EIP for app (set to EAX)
		; [ESP+40]: CS  for app (set to ECX)
		; [ESP+44]: ESP for app (set to EDX)
		; [ESP+48]: DS/SS for app (set to EBX)

		MOV		[0x0fe4],ESP	; save ESP of OS
		
		CLI
		MOV		ES,BX
		MOV		SS,BX
		MOV		DS,BX
		MOV		FS,BX
		MOV		GS,BX
		MOV		ESP,EDX
		STI

		PUSH	ECX				; push cs for far-call
		PUSH	EAX				; push eip for far-call
		CALL	FAR [ESP]		; call app

		; return from app
		MOV		EAX,1*8			; DS/SS for OS

		CLI
		MOV		ES,AX
		MOV		SS,AX
		MOV		DS,AX
		MOV		FS,AX
		MOV		GS,AX
		MOV		ESP,[0x0fe4]
		STI

		POPAD
		RET

