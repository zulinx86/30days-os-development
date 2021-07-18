; hello-os
; TAB=4

	ORG	0x7c00

; Description for FAT12 floppy disk

	JMP	SHORT entry
	DB	0x90				; NOP

	DB	"HELLOIPL"			; OEM Manufacturer

	; BIOS Parameter Block (BPB)
	; FAT12/16/32 common fileds
	DW	512					; Number of bytes per sector
	DB	1					; Number of sectors per cluster
	DW	1					; Number of reserved sectors
	DB	2					; Number of file allocation tables
	DW	224					; Root entries
	DW	2880				; Number of sectors (2880 means 1.4MB floppy)
	DB	0xf0				; Media type (0xf0 means removable)
	DW	9					; Sectors per file allocation table
	DW	18					; Sectors per track
	DW	2					; Number of heads
	DD	0					; Count of hidden sectors
	DD	2880				; Number of sectors
	; Fields for FAT12/16
	DB	0					; Drive number used by disk BIOS
	DB	0					; Reserved
	DB	0x29				; Extended boot signature
	DD	0xffffffff			; Volume serial number
	DB	"HELLO-OS   "		; 11-byte volume label
	DB	"FAT12   "			; FAT type
	TIMES	18 DB 0


; Program

entry:
	MOV	AX,0
	MOV	SS,AX
	MOV	SP,0x7c00
	MOV	DS,AX
	MOV ES,AX

	MOV	SI,msg
putloop:
	MOV	AL,[SI]
	ADD	SI,1
	CMP	AL,0
	JE	fin
	MOV	AH,0x0e
	MOV	BX,15
	INT	0x10
	JMP	putloop
fin:
	HLT
	JMP	fin

msg:
	DB	0x0a, 0x0a
	DB	"hello, world"
	DB	0x0a
	DB	0x00

	TIMES 0x1fe-($-$$) DB 0

	DB	0x55, 0xaa
