#include "bootpack.h"

struct FIFO8 mousefifo;

void inthandler2c(int *esp)
{
	unsigned char data;
	io_out8(PIC1_OCW2, 0x64); /* notify PIC1 of completion of IRQ-12 reception */
	io_out8(PIC0_OCW2, 0x62); /* notify PIC0 of completion of IRQ-02 reception */
	data = io_in8(PORT_KEYDAT);
	fifo8_put(&mousefifo, data);
	return;
}

#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4

void enable_mouse(struct MOUSE_DEC *mdec)
{
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
	mdec->phase = 0;
	return;	/* when it succeeds, ACK(0xfa) is sent */
}

int mouse_decode(struct MOUSE_DEC *mdec, unsigned char data)
{
	if (mdec->phase == 0) {
		if (data == 0xfa) {
			mdec->phase = 1;
		}
		return 0;
	} else if (mdec->phase == 1) {
		if ((data & 0xc8) == 0x08) {
			mdec->buf[0] = data;
			mdec->phase = 2;
		}
		return 0;
	} else if (mdec->phase == 2) {
		mdec->buf[1] = data;
		mdec->phase = 3;
		return 0;
	} else if (mdec->phase == 3) {
		mdec->buf[2] = data;
		mdec->phase = 1;

		mdec->btn = mdec->buf[0] & 0x07;
		mdec->x = mdec->buf[1];
		mdec->y = mdec->buf[2];
		if ((mdec->buf[0] & 0x10) != 0)
			mdec->x |= 0xffffff00;
		if ((mdec->buf[0] & 0x20) != 0)
			mdec->y |= 0xffffff00;
		mdec->y = -mdec->y;
		return 1;
	}
	return -1;
}
