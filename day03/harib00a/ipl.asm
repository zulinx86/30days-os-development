; haribote-ipl
; TAB=4

	ORG	0x7c00

; Description for FAT12 floppy disk

	JMP	SHORT entry
	DB	0x90				; NOP

	DB	"HARIBOTE"			; OEM Manufacturer

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
	DB	"HARIBOTE OS"		; 11-byte volume label
	DB	"FAT12   "			; FAT type
	TIMES	18 DB 0


; Program

entry:
	MOV	AX,0
	MOV	SS,AX
	MOV	SP,0x7c00
	MOV	DS,AX
	MOV ES,AX

	; Read sectors
	MOV	AX,0x0820
	MOV	ES,AX		; segment for output buffer
	MOV	CH,0		; cylinder (0 - 79)
	MOV	DH,0		; head (0 - 1)
	MOV	CL,2		; sector (1 - 18)

	MOV	AH,0x02		; function to read
	MOV	AL,1		; number of sectors
	MOV	BX,0		; offset address for output buffer
	MOV	DL,0x00		; drive number
	INT	0x13		; BIOS interruption for mass storage access
	JC	error


fin:
	HLT
	JMP	fin

error:
	MOV	SI,msg
putloop:
	MOV	AL,[SI]		; character code
	ADD	SI,1
	CMP	AL,0
	JE	fin
	MOV	AH,0x0e		; function to display a character
	MOV	BX,15		; color code
	INT	0x10		; BIOS interruption for video display functions
	JMP	putloop
msg:
	DB	0x0a, 0x0a
	DB	"load error"
	DB	0x0a
	DB	0x00

	TIMES 0x1fe-($-$$) DB 0

	DB	0x55, 0xaa
