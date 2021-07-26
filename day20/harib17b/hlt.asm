[BITS 32]

	MOV		AL,'A'
	CALL	0x155
fin:
	HLT
	JMP		fin
