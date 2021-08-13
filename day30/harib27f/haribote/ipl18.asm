; haribote-ipl
; TAB=4

CYLS	EQU		18

		ORG		0x7c00

; Description for FAT12 floppy disk

		JMP		SHORT entry
		DB		0x90				; NOP
	
		DB		"HARIBOTE"			; OEM Manufacturer
	
		; BIOS Parameter Block (BPB)
		; FAT12/16/32 common fileds
		DW		512					; Number of bytes per sector
		DB		1					; Number of sectors per cluster
		DW		1					; Number of reserved sectors
		DB		2					; Number of file allocation tables
		DW		224					; Root entries
		DW		2880				; Number of sectors (2880 means 1.4MB floppy)
		DB		0xf0				; Media type (0xf0 means removable)
		DW		9					; Sectors per file allocation table
		DW		18					; Sectors per track
		DW		2					; Number of heads
		DD		0					; Count of hidden sectors
		DD		2880				; Number of sectors
		; Fields for FAT12/16
		DB		0					; Drive number used by disk BIOS
		DB		0					; Reserved
		DB		0x29				; Extended boot signature
		DD		0xffffffff			; Volume serial number
		DB		"HARIBOTE OS"		; 11-byte volume label
		DB		"FAT12   "			; FAT type
		TIMES	18 DB 0


; Program

entry:
		MOV		AX,0
		MOV		SS,AX
		MOV		SP,0x7c00
		MOV		DS,AX
	
		; Read floppy disk
		MOV		AX,0x0820
		MOV		ES,AX			; segment for output buffer
		MOV		CH,0			; cylinder (0 - 79)
		MOV		DH,0			; head (0 - 1)
		MOV		CL,2			; sector (1 - 18)
		MOV		BX,18*2*CYLS-1	; number of sectors to read
		CALL	readfast		; read disk fast

; Go to haribote.sys
		MOV		BYTE [0x0ff0],CYLS	; remember the number of cylinders which IPL has been loaded
		JMP		0xc200


error:
		MOV		SI,msg
putloop:
		MOV		AL,[SI]			; character code
		ADD		SI,1
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e			; function to display a character
		MOV		BX,15			; color code
		INT		0x10			; BIOS interruption for video display functions
		JMP		putloop
fin:
		HLT
		JMP		fin
msg:
		DB		0x0a, 0x0a
		DB		"load error"
		DB		0x0a
		DB		0x00

readfast:
		; ES: destination address
		; CH: cylinder, DH: head, CL: sector, BX: number of sectors to read

		MOV		AX,ES
		SHL		AX,3
		AND		AH,0x7f
		MOV		AL,128
		SUB		AL,AH
		; AL = 128 - ((ES / 2^5) % 128);
		; Number of sectors from ES to the nearest 64 KiB (= 128 * 512 B) boundary

		MOV		AH,BL
		CMP		BH,0
		JE		.skip1
		MOV		AH,18
		; if (BX > 0x0100) { AH = 18; }
		; else { AH = BL; }
.skip1:
		CMP		AL,AH
		JBE		.skip2
		MOV		AL,AH
		; AL = min(AL, AH);

.skip2:
		MOV		AH,19
		SUB		AH,CL
		CMP		AL,AH
		JBE		.skip3
		MOV		AL,AH
		; AL = min(AL, 19 - CL);
.skip3:

		PUSH	BX
		MOV		SI,0			; the number of failures
retry:
		MOV		AH,0x02			; function to read disk
		MOV		BX,0			; destination address [ES:BX]
		MOV		DL,0			; A drive
		PUSH	ES
		PUSH	DX
		PUSH	CX
		PUSH	AX
		INT		0x13			; BIOS interruption for disk functions
		JNC		next			; if not error, go to next

		ADD		SI,1
		CMP		SI,5
		JAE		error

		MOV		AH,0x00			; function to reset
		MOV		DL,0x00			; A drive
		INT		0x13			; BIOS interruption for disk functions

		POP		AX
		POP		CX
		POP		DX
		POP		ES
		JMP		retry

next:
		POP		AX
		POP		CX
		POP		DX
		POP		BX				; set ES value to BX

		; ready for new ES value
		SHR		BX,5			; 16 byte unit -> 512 byte unit
		MOV		AH,0
		ADD		BX,AX			; BX += AL;
		SHL		BX,5			; 512 byte unit -> 16 byte unit
		MOV		ES,BX			; set BX value to ES

		; ready for new BX value
		POP		BX
		SUB		BX,AX

		; if there are no any sectors to read, go to .ret
		JZ		.ret

		ADD		CL,AL
		CMP		CL,18
		JBE		readfast
		
		; go to next head
		MOV		CL,1
		ADD		DH,1
		CMP		DH,2
		JB		readfast

		; go to next cylinder
		MOV		DH,0
		ADD		CH,1
		JMP		readfast

.ret:
		RET

		TIMES	0x1fe-($-$$) DB 0
	
		DB		0x55, 0xaa
