#include "mystdio.h"
#include "mystring.h"
#include "bootpack.h"


#define KEYCMD_LED	0xed

void HariMain(void)
{
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	char s[40];
	int i, j, x, y;

	struct FIFO32 fifo;
	int fifobuf[128];
	
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	unsigned int memtotal;

	struct TASK *task_a, *task_cons;
	struct TIMER *timer;

	struct SHTCTL *shtctl;
	struct SHEET *sht, *sht_back, *sht_mouse, *sht_win, *sht_cons;
	char *buf_back, buf_mouse[256], *buf_win, *buf_cons;
	struct MOUSE_DEC mdec;
	int mx, my, mmx = -1, mmy = -1;

	static char keytable0[0x80] = {
		0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0,   0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0,   0,   'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0,   0,   ']', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0x5c, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0x5c, 0,  0
	};
	static char keytable1[0x80] = {
		0,   0,   '!', 0x22, '#', '$', '%', '&', 0x27, '(', ')', '~', '=', '~', 0,   0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '`', '{', 0,   0,   'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', '+', '*', 0,   0,   '}', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   '_', 0,   0,   0,   0,   0,   0,   0,   0,   0,   '|', 0,   0
	};
	int cursor_x, cursor_c;
	int key_to = 0, key_shift = 0, key_ctrl = 0, key_leds = (binfo->leds >> 4) & 7, keycmd_wait = -1;
	struct FIFO32 keycmd;
	int keycmd_buf[32];
	struct CONSOLE *cons;

	init_gdtidt();
	init_pic();
	io_sti();

	fifo32_init(&fifo, 128, fifobuf, 0);
	init_pit();
	init_keyboard(&fifo, 256);
	enable_mouse(&fifo, 512, &mdec);
	io_out8(PIC0_IMR, 0xf8); /* enable PIT, PIC1 and PS/2 keyboard (11111000) */
	io_out8(PIC1_IMR, 0xef); /* enable PS/2 mouse (11101111) */
	fifo32_init(&keycmd, 32, keycmd_buf, 0);

	memtotal = memtest(0x00400000, 0xbfffffff);
	memman_init(memman);
	memman_free(memman, 0x00001000, 0x0009e000);
	memman_free(memman, 0x00400000, memtotal - 0x00400000);

	init_palette();
	shtctl = shtctl_init(memman, binfo->vram, binfo->scrnx, binfo->scrny);
	*((int *) 0x0fe4) = (int) shtctl;
	task_a = task_init(memman);
	fifo.task = task_a;
	task_run(task_a, 1, 2);

	/* background */
	sht_back = sheet_alloc(shtctl);
	buf_back = (char *) memman_alloc_4k(memman, binfo->scrnx * binfo->scrny);
	sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny, -1);
	init_screen(buf_back, binfo->scrnx, binfo->scrny);

	/* mouse */
	sht_mouse = sheet_alloc(shtctl);
	sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99);
	init_mouse_cursor8(buf_mouse, 99);
	mx = (binfo->scrnx - 16) / 2;
	my = (binfo->scrny - 28 - 16) / 2;

	/* task A */
	sht_win = sheet_alloc(shtctl);
	buf_win = (char *) memman_alloc_4k(memman, 144 * 52);
	sheet_setbuf(sht_win, buf_win, 144, 52, -1);
	make_window8(buf_win, 144, 52, "task_a", 1);
	make_textbox8(sht_win, 8, 28, 128, 16, COL8_FFFFFF);
	cursor_x = 8;
	cursor_c = COL8_FFFFFF;
	timer = timer_alloc();
	timer_init(timer, &fifo, 1);
	timer_settime(timer, 50);

	/* console task */
	sht_cons = sheet_alloc(shtctl);
	buf_cons = (char *) memman_alloc_4k(memman, 256 * 165);
	sheet_setbuf(sht_cons, buf_cons, 256, 165, -1);
	make_window8(buf_cons, 256, 165, "console", 0);
	make_textbox8(sht_cons, 8, 28, 240, 128, COL8_000000);
	task_cons = task_alloc();
	task_cons->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024 - 12;
	task_cons->tss.eip = (int) &console_task;
	task_cons->tss.es = 1 * 8;
	task_cons->tss.cs = 2 * 8;
	task_cons->tss.ss = 1 * 8;
	task_cons->tss.ds = 1 * 8;
	task_cons->tss.gs = 1 * 8;
	task_cons->tss.gs = 1 * 8;
	*((int *) (task_cons->tss.esp + 4)) = (int) sht_cons;
	*((int *) (task_cons->tss.esp + 8)) = memtotal;
	task_run(task_cons, 2, 2);

	sheet_slide(sht_back,   0,   0);
	sheet_slide(sht_cons,  32,   4);
	sheet_slide(sht_win,    8,  56);
	sheet_slide(sht_mouse, mx,  my);
	sheet_updown(sht_back,  0);
	sheet_updown(sht_cons,  1);
	sheet_updown(sht_win,   2);
	sheet_updown(sht_mouse, 3);

	for (;;) {
		if (fifo32_status(&keycmd) > 0 && keycmd_wait < 0) {
			keycmd_wait = fifo32_get(&keycmd);
			wait_KBC_sendready();
			io_out8(PORT_KEYDAT, keycmd_wait);
		}

		io_cli();
		if (fifo32_status(&fifo) == 0) {
			task_sleep(task_a);
			io_sti();
		} else {
			i = fifo32_get(&fifo);
			io_sti();

			if (256 <= i && i < 512) {
				if (i < 256 + 0x80) {
					if (key_shift == 0) {
						s[0] = keytable0[i - 256];
					} else {
						s[0] = keytable1[i - 256];
					}
				} else {
					s[0] = 0;
				}

				if (key_ctrl && s[0] == 'C' && task_cons->tss.ss0 != 0) {
					cons = (struct CONSOLE *) *((int *) 0x0fec);
					cons_putstr(cons, "\nBreak(key) :\n");
					io_cli();
					task_cons->tss.eax = (int) &(task_cons->tss.esp0);
					task_cons->tss.eip = (int) asm_end_app;
					io_sti();
					s[0] = 0;
				}

				if ('A' <= s[0] && s[0] <= 'Z') {
					if (((key_leds & 4) == 0 && key_shift == 0) ||
						((key_leds & 4) != 0 && key_shift != 0)) {
						s[0] += 0x20;
					}
				}

				if (s[0] != 0) { /* normal character */
					if (key_to == 0) {
						if (cursor_x < 128) {
							s[1] = 0;
							putfonts8_asc_sht(sht_win, cursor_x, 28, COL8_000000, COL8_FFFFFF, s, 1);
							cursor_x += 8;
						}
					} else {
						fifo32_put(&task_cons->fifo, s[0] + 256);
					}
				}

				if (i == 256 + 0x0e) { /* backspace */
					if (key_to == 0) {
						if (cursor_x > 8) {
							putfonts8_asc_sht(sht_win, cursor_x, 28, COL8_000000, COL8_FFFFFF, " ", 1);
							cursor_x -= 8;
						}
					} else {
						fifo32_put(&task_cons->fifo, 8 + 256);
					}
				}

				if (i == 256 + 0x1c) { /* enter */
					if (key_to != 0) fifo32_put(&task_cons->fifo, 10 + 256);
				}

				if (i == 256 + 0x0f) { /* tab */
					if (key_ctrl) {
						if (shtctl->top > 2)
							sheet_updown(shtctl->sheets[1], shtctl->top - 1);
					} else {
						if (key_to == 0) {
							key_to = 1;
							make_wtitle8(buf_win,  sht_win->bxsize,  "task_a",  0);
							make_wtitle8(buf_cons, sht_cons->bxsize, "console", 1);
							cursor_c = -1;
							boxfill8(sht_win->buf, sht_win->bxsize, COL8_FFFFFF, cursor_x, 28, cursor_x + 7, 43);
							fifo32_put(&task_cons->fifo, 2);
						} else {
							key_to = 0;
							make_wtitle8(buf_win,  sht_win->bxsize,  "task_a",  1);
							make_wtitle8(buf_cons, sht_cons->bxsize, "console", 0);
							cursor_c = COL8_000000;
							fifo32_put(&task_cons->fifo, 3);
						}
						sheet_refresh(sht_win,  0, 0, sht_win->bxsize,  21);
						sheet_refresh(sht_cons, 0, 0, sht_cons->bxsize, 21);
					}
				}

				if (i == 256 + 0x2a) { /* left shift on */
					key_shift |= 1;
				}
				if (i == 256 + 0x36) { /* right shift on */
					key_shift |= 2;
				}
				if (i == 256 + 0xaa) { /* left shift off */
					key_shift &= ~1;
				}
				if (i == 256 + 0xb6) { /* right shift off */
					key_shift &= ~2;
				}

				if (i == 256 + 0x1d) { /* ctrl on */
					key_ctrl = 1;
				}
				if (i == 256 + 0x9d) { /* ctrl off */
					key_ctrl = 0;
				}

				if (i == 256 + 0x3a) { /* CapsLock */
					key_leds ^= 4;
					fifo32_put(&keycmd, KEYCMD_LED);
					fifo32_put(&keycmd, key_leds);
				}
				if (i == 256 + 0x45) { /* NumLock */
					key_leds ^= 2;
					fifo32_put(&keycmd, KEYCMD_LED);
					fifo32_put(&keycmd, key_leds);
				}
				if (i == 256 + 0x46) { /* ScrollLock */
					key_leds ^= 1;
					fifo32_put(&keycmd, KEYCMD_LED);
					fifo32_put(&keycmd, key_leds);
				}

				if (i == 256 + 0xfa) { /* keyboard got the data successfully */
					keycmd_wait = -1;
				}
				if (i == 256 + 0xfe) { /* keyboard didn't get the data successfully */
					wait_KBC_sendready();
					io_out8(PORT_KEYDAT, keycmd_wait);
				}

				if (cursor_c >= 0)
					boxfill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43);
				sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44);
			} else if (512 <= i && i < 768) {
				if (mouse_decode(&mdec, i - 512) != 0) {
					mx += mdec.x;
					my += mdec.y;
					if (mx < 0) mx = 0;
					if (my < 0) my = 0;
					if (mx > binfo->scrnx - 1) mx = binfo->scrnx - 1;
					if (my > binfo->scrny - 1) my = binfo->scrny - 1;
					sheet_slide(sht_mouse, mx, my);

					if ((mdec.btn & 0x01) != 0) {
						if (mmx < 0) {
							for (j = shtctl->top - 1; j > 0; j--) {
								sht = shtctl->sheets[j];
								x = mx - sht->vx0;
								y = my - sht->vy0;
								if (0 <= x && x < sht->bxsize && 0 <= y && y < sht->bysize) {
									if (sht->buf[y * sht->bxsize + x] != sht->col_inv) {
										sheet_updown(sht, shtctl->top - 1);
										if (3 <= x && x < sht->bxsize - 3 && 3 <= y && y < 21) {
											mmx = mx;
											mmy = my;
										}
										if (sht->bxsize - 21 <= x && x < sht->bxsize - 5 && 5 <= y && y < 19) {
											if (sht->task != 0) {
												cons = (struct CONSOLE *) *((int *) 0x0fec);
												cons_putstr(cons, "\nBreak(mouse) :\n");
												io_cli();
												task_cons->tss.eax = (int) &(task_cons->tss.esp0);
												task_cons->tss.eip = (int) asm_end_app;
												io_sti();
											}
										}
										break;
									}
								}
							}
						} else {
							x = mx - mmx;
							y = my - mmy;
							sheet_slide(sht, sht->vx0 + x, sht->vy0 + y);
							mmx = mx;
							mmy = my;
						}
					} else {
						mmx = -1;
					}
				}
			} else if (i <= 1) {
				if (i != 0) {
					timer_init(timer, &fifo, 0);
					if (cursor_c >= 0) cursor_c = COL8_000000;
				} else {
					timer_init(timer, &fifo, 1);
					if (cursor_c >= 0) cursor_c = COL8_FFFFFF;
				}
				timer_settime(timer, 50);
				if (cursor_c >= 0) {
					boxfill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43);
					sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44);
				}
			}
		}
	}
}

