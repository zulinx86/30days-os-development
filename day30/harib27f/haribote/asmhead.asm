; haribote-os
; TAB=4

VBEMODE	EQU	0x105
; 0x100 :  640 x  400 x 8 bit color
; 0x101 :  640 x  480 x 8 bit color
; 0x103 :  800 x  600 x 8 bit color
; 0x105 : 1024 x  768 x 8 bit color
; 0x107 : 1280 x 1024 x 8 bit color

BOTPAK	EQU	0x00280000	; where bootpack is loaded
DSKCAC	EQU	0x00100000	; where disk cache is placed
DSKCAC0	EQU	0x00008000	; where disk cache is placed in real mode

; Boot Info

CYLS	EQU	0x0ff0		; #cylinder which IPL has loaded
LEDS	EQU	0x0ff1		; LED state of keyboard
VMODE	EQU	0x0ff2		; video mode (N bit color)
SCRNX	EQU	0x0ff4		; resolution for x-axis
SCRNY	EQU	0x0ff6		; resolution for y-axis
VRAM	EQU	0x0ff8		; where graphic buffer is

; Program

		ORG		0xc200

		; check the existence of VBE
		MOV		AX,0x9000
		MOV		ES,AX
		MOV		DI,0
		MOV		AX,0x4f00
		INT		0x10
		CMP		AX,0x004f
		JNE		scrn320

		; check if VBE is greater than or equal to version 2.0
		MOV		AX,[ES:DI+4]
		CMP		AX,0x0200
		JB		scrn320		; if ([0x9000:0x0004] < 0x0200) goto scrn320

		; check if the display mode is available
		MOV		CX,VBEMODE
		MOV		AX,0x4f01
		INT		0x10
		CMP		AX,0x004f
		JNE		scrn320

		; check info about the display mode
		CMP		BYTE [ES:DI+0x19],8
		JNE		scrn320		; 8 bit color or not
		CMP		BYTE [ES:DI+0x1b],4
		JNE		scrn320		; palette mode or not
		MOV		AX,[ES:DI+0x00]
		AND		AX,0x0080
		JZ		scrn320

		; switch to the display mode
		MOV		BX,VBEMODE+0x4000
		MOV		AX,0x4f02
		INT		0x10
		MOV		BYTE [VMODE],8
		MOV		AX,[ES:DI+0x12]
		MOV		[SCRNX],AX
		MOV		AX,[ES:DI+0x14]
		MOV		[SCRNY],AX
		MOV		EAX,[ES:DI+0x28]
		MOV		[VRAM],EAX
		JMP		keystatus

scrn320:
		MOV		AL,0x13		; VGA graphics (320 x 200 x 8 bit color)
		MOV		AH,0x00
		INT		0x10
		MOV		BYTE [VMODE],8
		MOV		WORD [SCRNX],320
		MOV		WORD [SCRNY],200
		MOV		DWORD [VRAM],0x000a0000

keystatus:
		MOV		AH,0x02		; function to get the LED state of keyboard
		INT		0x16		; BIOS interruption for keyboard functions
		MOV		[LEDS],AL

		; ignore interruptions
		MOV		AL,0xff
		OUT		0x21,AL		; io_out8(PIC0_IMR, 0xff);
		NOP
		OUT		0xa1,AL		; io_out8(PIC1_IMR, 0xff);
		CLI

		; set A20 gate
		CALL	waitkbdout	; wait_KBC_sendready();
		MOV		AL,0xd1
		OUT		0x64,AL		; io_out8(PORT_KEYCMD, KEYCMD_WRITE_OUTPUT);
		CALL	waitkbdout	; wait_KBC_sendready();
		MOV		AL,0xdf
		OUT		0x60,AL		; io_out8(PORT_KEYDAT, KBC_OUTPUT_A20G_ENABLE);
		CALL	waitkbdout	; wait_KBC_sendready();

		; go to protected mode
		LGDT	[GDTR0]		; load temporal GDT
		MOV		EAX,CR0
		AND		EAX,0x7fffffff	; reset bit 31 (prohibit paging)
		OR		EAX,0x00000001	; reset bit 0 (go to protected mode)
		MOV		CR0,EAX
		JMP		pipelineflush
pipelineflush:
		MOV		AX,1*8
		MOV		DS,AX
		MOV		ES,AX
		MOV		FS,AX
		MOV		GS,AX
		MOV		SS,AX

		; transfer bootpack
		; memcpy(bootpack, BOTPAK, 512 * 1024 / 4);
		MOV		ESI,bootpack	; source address
		MOV		EDI,BOTPAK		; destination address
		MOV		ECX,512*1024/4
		CALL	memcpy

		; trasnfer boot sector data
		; memcpy(0x7c00, DSKCAC, 512 / 4);
		MOV		ESI,0x7c00
		MOV		EDI,DSKCAC
		MOV		ECX,512/4
		CALL	memcpy

		; trasnfer disk data (except for boot sector)
		; memcpy(DSKCAC0 + 512, DSKCAC + 512, cyls * 512 * 8 * 2 / 4 - 512 / 4);
		MOV		ESI,DSKCAC0+512
		MOV		EDI,DSKCAC+512
		MOV		ECX,0
		MOV		CL,BYTE [CYLS]
		IMUL	ECX,512*18*2/4
		SUB		ECX,512/4
		CALL	memcpy

		; go to bootpack
		MOV		EBX,BOTPAK
		MOV		ECX,[EBX+16]	; (size of .data section + 3) / 4
		ADD		ECX,3
		SHR		ECX,2
		JZ		skip
		MOV		ESI,[EBX+20]	; file offset of .data section
		ADD		ESI,EBX
		MOV		EDI,[EBX+12]	; address of .data section
		CALL	memcpy
skip:
		MOV		ESP,[EBX+12]
		JMP		DWORD 2*8:0x0000001b

waitkbdout:
		IN		AL,0x64
		AND		AL,0x02
		JNZ		waitkbdout
		RET

memcpy:
		MOV		EAX,[ESI]
		ADD		ESI,4
		MOV		[EDI],EAX
		ADD		EDI,4
		SUB		ECX,1
		JNZ		memcpy
		RET

		ALIGNB	16
GDT0:
		TIMES	8 DB 0
		DW		0xffff,0x0000,0x9200,0x00cf	; 32 bit readable/writable segment
		DW		0xffff,0x0000,0x9a28,0x0047	; 32 bit executable segment (for bootpack)
		DW		0

GDTR0:
		DW		8*3-1
		DW		GDT0

		ALIGNB	16
bootpack:
