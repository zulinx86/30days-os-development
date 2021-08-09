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

void inthandler27(int *esp)
{
	io_out8(PIC0_OCW2, 0x67); /* notify PIC of completion of IRQ-07 reception */
	return;
}
