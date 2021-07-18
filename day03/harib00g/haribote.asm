; haribote-os
; TAB=4

	ORG	0xc200

	MOV	AL,0x13		; VGA graphics (640 x 480 x 8 bit color)
	MOV	AH,0x00		; function to set video mode
	INT	0x10		; BIOS interruption for video display functions
fin:
	HLT
	JMP	fin
