; haribote-os
; TAB=4


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

		MOV		AL,0x13		; VGA graphics (320 x 200 x 8 bit color)
		MOV		AH,0x00		; function to set video mode
		INT		0x10		; BIOS interruption for video display functions

		MOV		BYTE [VMODE],8
		MOV		WORD [SCRNX],320
		MOV		WORD [SCRNY],200
		MOV		DWORD [VRAM],0x000a0000

		MOV		AH,0x02		; function to get the LED state of keyboard
		INT		0x16		; BIOS interruption for keyboard functions
		MOV		[LEDS],AL

		; ignore interruptions
		MOV		AL,0xff
		OUT		0x21,AL
		NOP
		OUT		0xa1,AL
		CLI

		; set A20 gate
		CALL	waitkbdout
		MOV		AL,0xd1
		OUT		0x64,AL
		CALL	waitkbdout
		MOV		AL,0xdf
		OUT		0x60,AL
		CALL	waitkbdout

		; go to protected mode
		LGDT	[GDTR0]
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
		MOV		ESI,bootpack	; source address
		MOV		EDI,BOTPAK		; destination address
		MOV		ECX,512*1024/4
		CALL	memcpy

		; trasnfer boot sector data
		MOV		ESI,0x7c00
		MOV		EDI,DSKCAC
		MOV		ECX,512/4
		CALL	memcpy

		; trasnfer disk data (except for boot sector)
		MOV		ESI,DSKCAC0+512
		MOV		EDI,DSKCAC+512
		MOV		ECX,0
		MOV		CL,BYTE [CYLS]
		IMUL	ECX,512*18*2/4
		SUB		ECX,512/4
		CALL	memcpy

		; go to bootpack
		MOV		EBX,BOTPAK
		MOV		ECX,[EBX+16]
		ADD		ECX,3			; ECX += 3
		SHR		ECX,2			; ECX /= 4
		JZ		skip
		MOV		ESI,[EBX+20]
		ADD		ESI,EBX
		MOV		EDI,[EBX+12]
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
