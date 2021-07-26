[BITS 32]

	MOV		AL,'A'
	CALL	2*8:0x155
fin:
	HLT
	JMP		fin
