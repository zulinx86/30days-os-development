#include "libapi.h"
#include "mystring.h"
#include "mystdlib.h"

void waittimer(int timer, int time);
void end(unsigned char *s);

void HariMain(void)
{
	unsigned char winbuf[256 * 112], txtbuf[100 * 1024];
	unsigned char s[32], *p, *r;
	int win, timer, i, j, t = 120, l = 192 / 4, o = 4, q = 7, note_old = 0;

	/* octave 16 list (C ~ B) */
	static int tonetable[12] = {
		1071618315, 1135340056, 1202850889, 1274376125, 1350154473, 1430438836,
		1515497155, 1605613306, 1701088041, 1802240000, 1909406767, 2022946002
	};
	static int notetable[7] = { +9, +11, +0, +2, +4, +5, +7 };

	/* analyze command line */
	api_cmdline(s, 30);
	for (p = s; *p > ' '; p++) {}
	for (; *p == ' '; p++) {}

	i = mystrlen(p);
	if (i > 12) {
file_error:
		end((unsigned char *) "File open error.\n");
	}
	if (i == 0)
		end(0);
	
	/* window set up */
	win = api_openwin(winbuf, 256, 112, -1, (unsigned char *) "mmplay");
	api_putstrwin(win, 128, 32, 0, i, p);
	api_boxfillwin(win, 8, 60, 247,  76, 7);
	api_boxfillwin(win, 6, 86, 249, 105, 7);

	/* file load */
	i = api_fopen(p);
	if (i == 0)
		goto file_error;
	j = api_fsize(i, 0);
	if (j >= 100 * 1024)
		j = 100 * 1024 - 1;
	api_fread(txtbuf, j, i);
	api_fclose(i);
	txtbuf[j] = 0;
	r = txtbuf;

	i = 0;
	for (p = txtbuf; *p != 0; p++) {
		if (i == 0 && *p > ' ') {
			if (*p == '/') {
				if (p[1] == '*') { /* start of comment block */
					i = 1;
				} else if (p[1] == '/') { /* start of comment line */
					i = 2;
				} else {
					*r = *p;
					if ('a' <= *p && *p <= 'z')
						*r += 'A' - 'a';
					r++;
				}
			} else if (*p == 0x22) { /* double quote */
				*r = *p;
				r++;
				i = 3;
			} else {
				*r = *p;
				r++;
			}
		} else if (i == 1 && *p == '*' && p[1] == '/') { /* end of comment block */
			p++;
			i = 0;
		} else if (i == 2 && *p == 0x0a) { /* end of comment line */
			i = 0;
		} else if (i == 3) {
			*r = *p;
			r++;
			if (*p == 0x22) { /* end of double quote */
				i = 0;
			} else if (*p == '%') {
				p++;
				*r = *p;
				r++;
			}
		}
	}
	*r = 0;

	/* timer set up */
	timer = api_alloctimer();
	api_inittimer(timer, 128);

	/* main */
	p = txtbuf;
	for (;;) {
		if (('A' <= *p && *p <= 'G') || *p == 'R') { /* note */
			if (*p == 'R') {
				i = 0;
				s[0] = 0;
			} else {
				i = o * 12 + notetable[*p - 'A'] + 12;
				s[0] = 'O';
				s[1] = '0' + o;
				s[2] = *p;
				s[3] = ' ';
				s[4] = 0;
			}

			p++;
			if (*p == '+' || *p == '-' || *p == '#') {
				s[3] = *p;
				if (*p == '-')
					i--;
				else
					i++;
				p++;
			}

			if (i != note_old) {
				api_boxfillwin(win + 1, 32, 36, 63, 51, 8);
				if (s[0] != 0)
					api_putstrwin(win + 1, 32, 36, 10, 4, s);
				api_refreshwin(win, 32, 36, 64, 52);

				if (28 <= note_old && note_old <= 107)
					api_boxfillwin(win, (note_old - 28) * 3 + 8, 60, (note_old - 28) * 3 + 10, 76, 7);

				if (28 <= i && i <= 107)
					api_boxfillwin(win, (i - 28) * 3 + 8, 60, (i - 28) * 3 + 10, 76, 4);

				if (s[0] != 0)
					api_beep(tonetable[i % 12] >> (17 - i / 12));
				else
					api_beep(0);

				note_old = i;
			}

			if ('0' <= *p && *p <= '9')
				i = 192 / mystrtol(p, &p);
			else
				i = l;

			for (; *p == '.'; ) {
				p++;
				i += i / 2;
			}
			i *= (60 * 100 / 48);
			i /= t;
			if (s[0] != 0 && q < 8 && *p != '&') {
				j = i * q / 8;
				waittimer(timer, j);
				api_boxfillwin(win, 32, 36, 63, 51, 8);
				if (28 <= note_old && note_old <= 107) {
					api_boxfillwin(win, (note_old - 28) * 3 + 8, 60, (note_old - 28) * 3 + 10,  76, 7);
				}
				note_old = 0;
				api_beep(0);
			} else {
				j = 0;
				if (*p == '&') {
					p++;
				}
			}
			waittimer(timer, i - j);
		} else if (*p == '<') { /* octave-- */
			p++;
			o--;
		} else if (*p == '>') { /* octave++ */
			p++;
			o++;
		} else if (*p == 'O') { /* set octave */
			o = mystrtol(p + 1, &p);
		} else if (*p == 'Q') { /* set Q parameter */
			q = mystrtol(p + 1, &p);
		} else if (*p == 'L') { /* set default note length */
			l = mystrtol(p + 1, &p);
			if (l == 0)
				goto syntax_error;
			l = 192 / l;
			for (; *p == '.'; ) {
				p++;
				l += l / 2;
			}
		} else if (*p == 'T') { /* set tempo */
			t = mystrtol(p + 1, &p);
		} else if (*p == '$') {
			if (p[1] == 'K') { /* karaoke */
				p += 2;
				for (; *p != 0x22; p++) {
					if (*p == 0)
						goto syntax_error;
				}
				p++;
				for (i = 0; i < 32; i++) {
					if (*p == 0)
						goto syntax_error;

					if (*p == 0x22)
						break;

					if (*p == '%') {
						s[i] = p[1];
						p += 2;
					} else {
						s[i] = *p;
						p++;
					}
				}

				if (i > 30)
					end((unsigned char *) "karaoke too long.\n");

				api_boxfillwin(win + 1, 8, 88, 247, 103, 7);
				s[i] = 0;
				if (i != 0)
					api_putstrwin(win + 1, 128 - i * 4, 88, 0, i, s);

				api_refreshwin(win, 8, 88, 248, 104);
			}

			for (; *p != ';'; p++) {
				if (*p == 0)
					goto syntax_error;
			}
			p++;
		} else if (*p == 0) {
			p = txtbuf;
		} else {
syntax_error:
			end((unsigned char *) "mml syntax error.\n");
		}
	}
}


void waittimer(int timer, int time)
{
	int i;
	api_settimer(timer, time);
	for (;;) {
		i = api_getkey(1);
		if (i == 'Q' || i == 'q') {
			api_beep(0);
			api_end();
		}
		if (i == 128)
			return;
	}
}

void end(unsigned char *s)
{
	if (s != 0)
		api_putstr(s);
	api_beep(0);
	api_end();
}
