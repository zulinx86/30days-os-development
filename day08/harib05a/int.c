#include "bootpack.h"

void init_pic(void)
{
	io_out8(PIC0_IMR,  0xff  ); /* ignore all interrupts */
	io_out8(PIC1_IMR,  0xff  ); /* ignore all interrupts */

	io_out8(PIC0_ICW1, 0x11  ); /* edge trigger mode */
	io_out8(PIC0_ICW2, 0x20  ); /* receive IRQ0-7 at INT20-27 */
	io_out8(PIC0_ICW3, 1 << 2); /* connect PIC1 with IRQ2 */
	io_out8(PIC0_ICW4, 0x01  ); /* non-buffer mode */

	io_out8(PIC1_ICW1, 0x11   ); /* edge trigger mode */
	io_out8(PIC1_ICW2, 0x28   ); /* receive IRQ8-15 at INT28-2f */
	io_out8(PIC1_ICW3, 2      ); /* conenct PIC1 with IRQ2 */
	io_out8(PIC1_ICW4, 0x01   ); /* non-buffer mode */

	io_out8(PIC0_IMR,  0xfb   ); /* ignore interrupts except for PIC1 (11111011) */
	io_out8(PIC1_IMR,  0xff   ); /* ignore all interrupts (11111111) */

	return;
}

#define PORT_KEYDAT		0x0060
struct FIFO8 keyfifo;

void inthandler21(int *esp)
{
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	unsigned char data;
	io_out8(PIC0_OCW2, 0x61); /* notify PIC of completion of IRQ-01 reception */
	data = io_in8(PORT_KEYDAT);
	fifo8_put(&keyfifo, data);
	return;
}

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

void inthandler27(int *esp)
{
	io_out8(PIC0_OCW2, 0x67); /* notify PIC of completion of IRQ-07 reception */
	return;
}
