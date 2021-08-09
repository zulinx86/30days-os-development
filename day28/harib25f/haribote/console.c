#include "mystdio.h"
#include "mystring.h"
#include "bootpack.h"

void console_task(struct SHEET *sheet, unsigned int memtotal)
{
	struct TASK *task = task_now();
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	int i, *fat = (int *) memman_alloc_4k(memman, 4 * 2880);
	struct CONSOLE cons;
	struct FILEHANDLE fhandle[8];
	unsigned char cmdline[30];
	unsigned char *nihongo = (unsigned char *) *((int *) 0x0fe8);

	cons.sht = sheet;
	cons.cur_x = 8;
	cons.cur_y = 28;
	cons.cur_c = -1;
	task->cons = &cons;
	task->cmdline = cmdline;

	if (sheet != 0) {
		cons.timer = timer_alloc();
		timer_init(cons.timer, &task->fifo, 1);
		timer_settime(cons.timer, 50);
	}

	file_readfat(fat, (unsigned char *) (ADR_DISKIMG + 0x00000200));
	for (i = 0; i < 8; i++)
		fhandle[i].buf = 0;
	task->fhandle = fhandle;
	task->fat = fat;
	if (nihongo[4096] != 0xff)
		task->langmode = 1;
	else
		task->langmode = 0;
	task->langbyte1 = 0;

	cons_putchar(&cons, '>', 1);
	for (;;) {
		io_cli();
		if (fifo32_status(&task->fifo) == 0) {
			task_sleep(task);
			io_sti();
		} else {
			i = fifo32_get(&task->fifo);
			io_sti();

			if (i <= 1 && cons.sht != 0) {
				if (i != 0) {
					timer_init(cons.timer, &task->fifo, 0);
					if (cons.cur_c >= 0)
						cons.cur_c = COL8_FFFFFF;
				} else {
					timer_init(cons.timer, &task->fifo, 1);
					if (cons.cur_c >= 0)
						cons.cur_c = COL8_000000;
				}
				timer_settime(cons.timer, 50);
			}

			if (i == 2) { /* cursor on */
				cons.cur_c = COL8_FFFFFF;
			}

			if (i == 3) { /* cursor off */
				if (cons.sht != 0)
					boxfill8(cons.sht->buf, cons.sht->bxsize, COL8_000000, 
						cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
				cons.cur_c = -1;
			}

			if (i == 4) { /* exit by mouse click */
				cmd_exit(&cons, fat);
			}
			
			if (256 <= i && i < 512) {
				if (i == 8 + 256) { /* backspace */
					if (cons.cur_x > 16) {
						cons_putchar(&cons, ' ', 0);
						cons.cur_x -= 8;
					}
				} else if (i == 10 + 256) { /* enter */
					cons_putchar(&cons, ' ', 0);
					cmdline[cons.cur_x / 8 - 2] = 0;
					cons_newline(&cons);
					cons_runcmd(cmdline, &cons, fat, memtotal);
					if (cons.sht == 0)
						cmd_exit(&cons, fat);
					cons_putchar(&cons, '>', 1);
				} else {
					if (cons.cur_x < 240) {
						cmdline[cons.cur_x / 8 - 2] = i - 256;
						cons_putchar(&cons, i - 256, 1);
					}
				}
			}

			if (cons.sht != 0) {
				if (cons.cur_c >= 0)
					boxfill8(cons.sht->buf, cons.sht->bxsize, cons.cur_c, 
						cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);

				sheet_refresh(cons.sht, cons.cur_x, cons.cur_y, cons.cur_x + 8, cons.cur_y + 16);
			}
		}
	}
}

void cons_putchar(struct CONSOLE *cons, unsigned char c, char move)
{
	unsigned char s[2];
	s[0] = c;
	s[1] = 0;

	if (s[0] == 0x09) { /* tab */
		for (;;) {
			if (cons->sht != 0)
				putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL8_FFFFFF, COL8_000000, (unsigned char *) " ", 1);
			cons->cur_x += 8;

			if (cons->cur_x == 8 + 240)
				cons_newline(cons);

			if (((cons->cur_x - 8) & 0x1f) == 0)
				break;
		}
	} else if (s[0] == 0x0a) { /* new line */
		cons_newline(cons);
	} else if (s[0] == 0x0d) { /* carriage return */
	} else { /* normal character */
		if (cons->sht != 0)
			putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL8_FFFFFF, COL8_000000, s, 1);
		
		if (move != 0) {
			cons->cur_x += 8;
			if (cons->cur_x == 8 + 240)
				cons_newline(cons);
		}
	}
	return;
}

void cons_newline(struct CONSOLE *cons)
{
	int x, y;
	struct SHEET *sheet = cons->sht;
	struct TASK *task = task_now();

	if (cons->cur_y < 28 + 112) {
		cons->cur_y += 16;
	} else {
		if (sheet != 0) {
			for (y = 28; y < 28 + 112; y++) {
				for (x = 8; x < 8 + 240; x++) {
					sheet->buf[x + y * sheet->bxsize] = sheet->buf[x + (y + 16) * sheet->bxsize];
				}
			}
			for (y = 28 + 112; y < 28 + 128; y++) {
				for (x = 8; x < 8 + 240; x++) {
					sheet->buf[x + y * sheet->bxsize] = COL8_000000;
				}
			}
			sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
		}
	}
	cons->cur_x = 8;
	if (task->langmode == 1 && task->langbyte1 != 0)
		cons->cur_x += 8;
	return;
}

void cons_putstr(struct CONSOLE *cons, unsigned char *s)
{
	for (; *s != 0; s++)
		cons_putchar(cons, *s, 1);
	return;
}

void cons_putnstr(struct CONSOLE *cons, unsigned char *s, int l)
{
	int i;
	for (i = 0; i < l; i++)
		cons_putchar(cons, s[i], 1);
	return;
}

void cons_runcmd(unsigned char *cmdline, struct CONSOLE *cons, int *fat, unsigned int memtotal)
{
	if (mystrcmp(cmdline, (unsigned char *) "mem") == 0 && cons->sht != 0) {
		cmd_mem(cons, memtotal);
	} else if (mystrcmp(cmdline, (unsigned char *) "cls") == 0 && cons->sht != 0) {
		cmd_cls(cons);
	} else if (mystrcmp(cmdline, (unsigned char *) "dir") == 0 && cons->sht != 0) {
		cmd_dir(cons);
	} else if (mystrcmp(cmdline, (unsigned char *) "exit") == 0) {
		cmd_exit(cons, fat);
	} else if (mystrncmp(cmdline, (unsigned char *) "start ", 6) == 0) {
		cmd_start(cons, cmdline, memtotal);
	} else if (mystrncmp(cmdline, (unsigned char *) "ncst ", 5) == 0) {
		cmd_ncst(cons, cmdline, memtotal);
	} else if (mystrncmp(cmdline, (unsigned char *) "langmode ", 9) == 0) {
		cmd_langmode(cons, cmdline);
	} else if (cmdline[0] != 0) {
		if (cmd_app(cons, fat, cmdline) == 0)
			cons_putstr(cons, (unsigned char *) "Bad command.\n\n");
	}
	return;
}

void cmd_mem(struct CONSOLE *cons, unsigned int memtotal)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	unsigned char s[60];
	mysprintf(s, (unsigned char *) "total: %dMB\nfree : %dKB\n\n", memtotal / (1024 * 1024), memman_total(memman) / 1024);
	cons_putstr(cons, s);
	return;
}

void cmd_cls(struct CONSOLE *cons)
{
	int x, y;
	struct SHEET *sheet = cons->sht;
	for (y = 28; y < 28 + 128; y++) {
		for (x = 8; x < 8 + 240; x++) {
			sheet->buf[x + y * sheet->bxsize] = COL8_000000;
		}
	}
	sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
	cons->cur_y = 28;
	return;
}

void cmd_dir(struct CONSOLE *cons)
{
	struct FILEINFO *finfo = (struct FILEINFO *) (ADR_DISKIMG + 0x00002600);
	int i, j;
	unsigned char s[30];
	for (i = 0; i < 224; i++) {
		if (finfo[i].name[0] == 0x00) break;
		if (finfo[i].name[0] != 0xe5) {
			if ((finfo[i].type & 0x18) == 0) {
				mysprintf(s, (unsigned char *) "filename.txt   %7d\n", finfo[i].size);
				for (j = 0; j < 8; j++)
					s[j] = finfo[i].name[j];
				s[9] = finfo[i].ext[0];
				s[10] = finfo[i].ext[1];
				s[11] = finfo[i].ext[2];
				cons_putstr(cons, s);
			}
		}
	}
	cons_newline(cons);
	return;
}

void cmd_exit(struct CONSOLE *cons, int *fat)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TASK *task = task_now();
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
	struct FIFO32 *fifo = (struct FIFO32 *) *((int *) 0xfec);
	if (cons->sht != 0)
		timer_cancel(cons->timer);
	memman_free_4k(memman, (int) fat, 4 * 2880);
	io_cli();
	if (cons->sht != 0)
		fifo32_put(fifo, cons->sht - shtctl->sheets0 + 768);
	else
		fifo32_put(fifo, task - taskctl->tasks0 + 1024);
	io_sti();
	for (;;)
		task_sleep(task);
}

void cmd_start(struct CONSOLE *cons, unsigned char *cmdline, int memtotal)
{
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
	struct SHEET *sht = open_console(shtctl, memtotal);
	struct FIFO32 *fifo = &sht->task->fifo;
	int i;
	sheet_slide(sht, 32, 4);
	sheet_updown(sht, shtctl->top);
	for (i = 6; cmdline[i] != 0; i++)
		fifo32_put(fifo, cmdline[i] + 256);
	fifo32_put(fifo, 10 + 256); /* enter */
	cons_newline(cons);
	return;
}

void cmd_ncst(struct CONSOLE *cons, unsigned char *cmdline, int memtotal)
{
	struct TASK *task = open_constask(0, memtotal);
	struct FIFO32 *fifo = &task->fifo;
	int i;
	for (i = 5; cmdline[i] != 0; i++)
		fifo32_put(fifo, cmdline[i] + 256);
	fifo32_put(fifo, 10 + 256); /* enter */
	cons_newline(cons);
	return;
}

void cmd_langmode(struct CONSOLE *cons, unsigned char *cmdline)
{
	struct TASK *task = task_now();
	unsigned char mode = cmdline[9] - '0';
	if (mode <= 1)
		task->langmode = mode;
	else
		cons_putstr(cons, (unsigned char *) "mode number error.\n");
	cons_newline(cons);
	return;
}

int cmd_app(struct CONSOLE *cons, int *fat, unsigned char *cmdline)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct FILEINFO *finfo;
	char name[18];
	unsigned char *p, *q;
	struct TASK *task = task_now();
	int i, segsize, datasize, esp, datahrb;
	struct SHTCTL *shtctl;
	struct SHEET *sht;

	for (i = 0; i < 13; i++) {
		if (cmdline[i] <= ' ') break;
		name[i] = cmdline[i];
	}
	name[i] = 0;

	finfo = file_search(name, (struct FILEINFO *) (ADR_DISKIMG + 0x00002600), 224);
	if (finfo == 0 && name[i - 1] != '.') {
		name[i] = '.';
		name[i + 1] = 'H';
		name[i + 2] = 'R';
		name[i + 3] = 'B';
		name[i + 4] = 0;
		finfo = file_search(name, (struct FILEINFO *) (ADR_DISKIMG + 0x00002600), 224);
	}

	if (finfo != 0) {
		p = (unsigned char *) memman_alloc_4k(memman, finfo->size);
		file_loadfile(finfo->clustno, finfo->size, p, fat, (unsigned char *) (ADR_DISKIMG + 0x00003e00));
		if (finfo->size >= 36 && mystrncmp((unsigned char *)(p + 4), (unsigned char *) "Hari", 4) == 0 && *p == 0x00) {
			segsize  = *((int *) (p + 0x0000));
			esp      = *((int *) (p + 0x000c));
			datasize = *((int *) (p + 0x0010));
			datahrb  = *((int *) (p + 0x0014));
			
			q = (unsigned char *) memman_alloc_4k(memman, segsize);
			task->ds_base = (int) q;
			
			set_segmdesc(task->ldt + 0, finfo->size - 1, (int) p, AR_CODE32_ER + 0x60);
			set_segmdesc(task->ldt + 1, segsize - 1,     (int) q, AR_DATA32_RW + 0x60);
			
			for (i = 0; i < datasize; i++)
				q[esp + i] = p[datahrb + i];

			start_app(0x1b, 0 * 8 + 4, esp, 1 * 8 + 4, &(task->tss.esp0));
			shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
			for (i = 0; i < MAX_SHEETS; i++) {
				sht = &(shtctl->sheets0[i]);
				if ((sht->flags & (SHEET_USE | SHEET_APP)) == (SHEET_USE | SHEET_APP) && sht->task == task)
					sheet_free(sht);
			}
			for (i = 0; i < 8; i++) {
				if (task->fhandle[i].buf != 0) {
					memman_free_4k(memman, (int) task->fhandle[i].buf, task->fhandle[i].size);
					task->fhandle[i].buf = 0;
				}
			}
			timer_cancelall(&task->fifo);
			memman_free_4k(memman, (int) q, finfo->size);
			task->langbyte1 = 0;
		} else {
			cons_putstr(cons, (unsigned char *) ".hrb file format error.\n");
		}
		memman_free_4k(memman, (int) p, finfo->size);
		cons_newline(cons);
		return 1;
	}
	return 0;
}

int *hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax)
{
	struct TASK *task = task_now();
	int i, ds_base = task->ds_base;
	struct CONSOLE *cons = (struct CONSOLE *) task->cons;
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
	struct SHEET *sht;
	struct FIFO32 *sys_fifo = (struct FIFO32 *) *((int *) 0x0fec);
	struct FILEINFO *finfo;
	struct FILEHANDLE *fh;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	int *reg = &eax + 1;
	/* 
	 * reg[0]:EDI, reg[1]:ESI, reg[2]:EBP, reg[3]:ESP
	 * reg[4]:EBX, reg[5]:EDX, reg[6]:ECX, reg[7]:EAX
	 */

	if (edx == 1) { /* api_putchar */
		cons_putchar(cons, eax & 0xff, 1);
	} else if (edx == 2) { /* api_putstr */
		cons_putstr(cons, (unsigned char *) ebx + ds_base);
	} else if (edx == 3) { /* api_putnstr */
		cons_putnstr(cons, (unsigned char *) ebx + ds_base, ecx);
	} else if (edx == 4) { /* api_end */
		return &(task->tss.esp0);
	} else if (edx == 5) { /* api_openwin */
		sht = sheet_alloc(shtctl);
		sht->task = task;
		sht->flags |= SHEET_APP;
		sheet_setbuf(sht, (unsigned char *) ebx + ds_base, esi, edi, eax);
		make_window8((unsigned char *) ebx + ds_base, esi, edi, (unsigned char *) ecx + ds_base, 0);
		sheet_slide(sht, ((shtctl->xsize - esi) / 2) & ~3, (shtctl->ysize - edi) / 2);
		sheet_updown(sht, shtctl->top);
		reg[7] = (int) sht;
	} else if (edx == 6) { /* api_putstrwin */
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		putfonts8_asc(sht->buf, sht->bxsize, esi, edi, eax, (unsigned char *) ebp + ds_base);
		if ((ebx & 1) == 0)
			sheet_refresh(sht, esi, edi, esi + ecx * 8, edi + 16);
	} else if (edx == 7) { /* api_boxfillwin */
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		boxfill8(sht->buf, sht->bxsize, ebp, eax, ecx, esi, edi);
		if ((ebx & 1) == 0)
			sheet_refresh(sht, eax, ecx, esi + 1, edi + 1);
	} else if (edx == 8) { /* api_initmalloc */
		memman_init((struct MEMMAN *) (ebx + ds_base));
		ecx &= 0xfffffff0; /* 16 byte alignment */
		memman_free((struct MEMMAN *) (ebx + ds_base), eax, ecx);
	} else if (edx == 9) { /* api_malloc */
		ecx = (ecx + 0x0f) & 0xfffffff0; /* round up in 16 byte unit */
		reg[7] = memman_alloc((struct MEMMAN *) (ebx + ds_base), ecx);
	} else if (edx == 10) { /* api_free */
		ecx = (ecx + 0x0f) & 0xfffffff0; /* round up in 16 byte unit */
		memman_free((struct MEMMAN *) (ebx + ds_base), eax, ecx);
	} else if (edx == 11) { /* api_point */
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		sht->buf[sht->bxsize * edi + esi] = eax;
		if ((ebx & 1) == 0)
			sheet_refresh(sht, esi, edi, esi + 1, edi + 1);
	} else if (edx == 12) { /* api_refreshwin */
		sht = (struct SHEET *) ebx;
		sheet_refresh(sht, eax, ecx, esi, edi);
	} else if (edx == 13) { /* api_linewin */
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		hrb_api_linewin(sht, eax, ecx, esi, edi, ebp);
		if ((ebx & 1) == 0)
			sheet_refresh(sht, eax, ecx, esi + 1, edi + 1);
	} else if (edx == 14) { /* api_closewin */
		sheet_free((struct SHEET *) ebx);
	} else if (edx == 15) { /* api_getkey */
		for (;;) {
			io_cli();
			if (fifo32_status(&task->fifo) == 0) {
				if (eax != 0) {
					task_sleep(task);
				} else {
					io_sti();
					reg[7] = -1;
					return 0;
				}
			}

			i = fifo32_get(&task->fifo);
			io_sti();
			
			if (i <= 1) {
				timer_init(cons->timer, &task->fifo, 1);
				timer_settime(cons->timer, 50);
			} else if (i == 2) {
				cons->cur_c = COL8_FFFFFF;
			} else if (i == 3) {
				cons->cur_c = -1;
			} else if (i == 4) {
				timer_cancel(cons->timer);
				io_cli();
				fifo32_put(sys_fifo, cons->sht - shtctl->sheets0 + 2024);
				cons->sht = 0;
				io_sti();
			} else if (i >= 256) {
				reg[7] = i - 256;
				return 0;
			}
		}
	} else if (edx == 16) { /* api_alloctimer */
		reg[7] = (int) timer_alloc();
		((struct TIMER *) reg[7])->cancel_flag = 1;
	} else if (edx == 17) { /* api_inittimer */
		timer_init((struct TIMER *) ebx, &task->fifo, eax + 256);
	} else if (edx == 18) { /* api_settimer */
		timer_settime((struct TIMER *) ebx, eax);
	} else if (edx == 19) { /* api_freetimer */
		timer_free((struct TIMER *) ebx);
	} else if (edx == 20) { /* api_beep */
		if (eax == 0) {
			i = io_in8(0x61);
			io_out8(0x61, i & 0x0d);
		} else {
			i = 1193180000 / eax;
			io_out8(0x43, 0xb6);
			io_out8(0x42, i & 0xff);
			io_out8(0x42, i >> 8);
			i = io_in8(0x61);
			io_out8(0x61, (i | 0x03) & 0x0f);
		}
	} else if (edx == 21) { /* api_fopen */
		for (i = 0; i < 8; i++) {
			if (task->fhandle[i].buf == 0)
				break;
		}

		fh = &task->fhandle[i];
		reg[7] = 0;
		if (i < 8) {
			finfo = file_search((char *) ebx + ds_base, (struct FILEINFO *) (ADR_DISKIMG + 0x00002600), 224);
			if (finfo != 0) {
				reg[7] = (int) fh;
				fh->buf = (unsigned char *) memman_alloc_4k(memman, finfo->size);
				fh->size = finfo->size;
				fh->pos = 0;
				file_loadfile(finfo->clustno, finfo->size, fh->buf, task->fat, (unsigned char *) (ADR_DISKIMG + 0x00003e00));
			}
		}
	} else if (edx == 22) { /* api_fclose */
		fh = (struct FILEHANDLE *) eax;
		memman_free_4k(memman, (int) fh->buf, fh->size);
		fh->buf = 0;
	} else if (edx == 23) { /* api_fseek */
		fh = (struct FILEHANDLE *) eax;
		if (ecx == 0)
			fh->pos = ebx;
		else if (ecx == 1)
			fh->pos += ebx;
		else if (ecx == 2)
			fh->pos = fh->size + ebx;

		if (fh->pos < 0)
			fh->pos = 0;
		if (fh->pos > fh->size)
			fh->pos = fh->size;
	} else if (edx == 24) { /* api_fsize */
		fh = (struct FILEHANDLE *) eax;
		if (ecx == 0)
			reg[7] = fh->size;
		else if (ecx == 1)
			reg[7] = fh->pos;
		else if (ecx == 2)
			reg[7] = fh->pos - fh->size;
	} else if (edx == 25) { /* api_fread */
		fh = (struct FILEHANDLE *) eax;
		for (i = 0; i < ecx; ++i) {
			if (fh->pos == fh->size)
				break;
			*((char *) ebx + ds_base + i) = fh->buf[fh->pos];
			fh->pos++;
		}
		reg[7] = i;
	} else if (edx == 26) { /* api_cmdline */
		i = 0;
		for (;;) {
			*((char *) ebx + ds_base + i) = task->cmdline[i];
			if (task->cmdline[i] == 0)
				break;
			if (i >= ecx)
				break;
			i++;
		}
		reg[7] = i;
	}

	return 0;
}

int *inthandler0c(int *esp)
{
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	unsigned char s[30];
	cons_putstr(cons, (unsigned char *) "\nINT 0C :\n Stack Exception.\n");
	mysprintf(s, (unsigned char *) "EIP = %08x\n", esp[11]);
	cons_putstr(cons, s);
	return &(task->tss.esp0);
}

int *inthandler0d(int *esp)
{
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	unsigned char s[30];
	cons_putstr(cons, (unsigned char *) "\nINT 0D :\n General Protected Exception.\n");
	mysprintf(s, (unsigned char *) "EIP = %08x\n", esp[11]);
	cons_putstr(cons, s);
	return &(task->tss.esp0);
}

void hrb_api_linewin(struct SHEET *sht, int x0, int y0, int x1, int y1, int col)
{
	int i, x, y, len, dx, dy;

	dx = x1 - x0;
	dy = y1 - y0;
	x = x0 << 10;
	y = y0 << 10;
	if (dx < 0) dx = -dx;
	if (dy < 0) dy = -dy;

	if (dx >= dy) {
		len = dx + 1;
		if (x0 > x1) dx = -1024;
		else dx = 1024;
		if (y0 <= y1) dy = ((y1 - y0 + 1) << 10) / len;
		else dy = (y1 - y0 - 1) << 10 / len;
	} else {
		len = dy + 1;
		if (y0 > y1) dy = -1024;
		else dy = 1024;
		if (x0 <= x1) dx = ((x1 - x0 + 1) << 10) / len;
		else dx = ((x1 - x0 - 1) << 10) / len;
	}

	for (i = 0; i < len; i++) {
		sht->buf[(y >> 10) * sht->bxsize + (x >> 10)] = col;
		x += dx;
		y += dy;
	}

	return;
}
