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

void inthandler21(int *esp)
{
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	boxfill8(binfo->vram, binfo->scrnx, COL8_000000, 0, 0, 32 * 8 - 1, 15);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, "INT 21 (IRQ-1) : PS/2 keyboard");
	for (;;)
		io_hlt();
}

void inthandler2c(int *esp)
{
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	boxfill8(binfo->vram, binfo->scrnx, COL8_000000, 0, 0, 32 * 8 - 1, 15);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, "INT 2C (IRQ-12) : PS/2 mouse");
	for (;;)
		io_hlt();
}

void inthandler27(int *esp)
{
	io_out8(PIC0_OCW2, 0x67); /* notify PIC of completion */
	return;
}