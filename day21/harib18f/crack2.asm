[BITS 32]
		MOV		EAX,1*8
		MOV		DS,AX
		MOV		BYTE [0x00102600],0
		RETF
