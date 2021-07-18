; haribote-os
; TAB=4

; Boot Info

CYLS	EQU	0x0ff0
LEDS	EQU	0x0ff1
VMODE	EQU	0x0ff2
SCRNX	EQU	0x0ff4
SCRNY	EQU	0x0ff6
VRAM	EQU	0x0ff8

; Program

	ORG	0xc200

	MOV	AL,0x13		; VGA graphics (320 x 200 x 8 bit color)
	MOV	AH,0x00		; function to set video mode
	INT	0x10		; BIOS interruption for video display functions

	MOV	BYTE [VMODE],8
	MOV	WORD [SCRNX],320
	MOV	WORD [SCRNY],200
	MOV	DWORD [VRAM],0x000a0000

	MOV	AH,0x02		; function to get the LED state of keyboard
	INT	0x16		; BIOS interruption for keyboard functions
	MOV	[LEDS],AL

fin:
	HLT
	JMP	fin
