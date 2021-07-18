; hello-os
; TAB=4

; Description for FAT12 floppy disk

	DB	0xeb, 0x4e, 0x90	; Jump Instruction
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

	DB	0xb8, 0x00, 0x00, 0x8e, 0xd0, 0xbc, 0x00, 0x7c
	DB	0x8e, 0xd8, 0x8e, 0xc0, 0xbe, 0x74, 0x7c, 0x8a
	DB	0x04, 0x83, 0xc6, 0x01, 0x3c, 0x00, 0x74, 0x09
	DB	0xb4, 0x0e, 0xbb, 0x0f, 0x00, 0xcd, 0x10, 0xeb
	DB	0xee, 0xf4, 0xeb, 0xfd

; Message

	DB	0x0a, 0x0a
	DB	"hello, world"
	DB	0x0a
	DB	0x00

	TIMES 0x1fe-($-$$) DB 0

	DB	0x55, 0xaa

; After boot sector

	DB	0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00
	TIMES	4600 DB 0
	DB	0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00
	TIMES	1469432 DB 0
