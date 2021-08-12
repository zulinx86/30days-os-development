#include "libapi.h"
#include "mystdio.h"
#include "mystdlib.h"

#define INVALID	-0x7fffffff

unsigned char *skipspace(unsigned char *p);
int getnum(unsigned char **p, int priority);

void HariMain(void)
{
	int i;
	unsigned char s[30], *p;

	api_cmdline(s, 30);
	for (p = s; *p > ' '; p++) {} /* skip until a space occurs */
	i = getnum(&p, 9);
	if (i == INVALID) {
		api_putstr((unsigned char *) "Error!\n");
	} else {
		mysprintf(s, (unsigned char *) "= %d = 0x%x\n", i, i);
		api_putstr(s);
	}
	api_end();
}

unsigned char *skipspace(unsigned char *p)
{
	for (; *p == ' '; p++) {}
	return p;
}

int getnum(unsigned char **pp, int priority)
{
	unsigned char *p = *pp;
	int i = INVALID, j;
	p = skipspace(p);

	/* unary operator */
	if (*p == '+') {
		p = skipspace(p + 1);
		i = getnum(&p, 0);
	} else if (*p == '-') {
		p = skipspace(p + 1);
		i = getnum(&p, 0);
		if (i != INVALID)
			i = -i;
	} else if (*p == '~') {
		p = skipspace(p + 1);
		i = getnum(&p, 0);
		if (i != INVALID)
			i = ~i;
	} else if (*p == '(') {
		p = skipspace(p + 1);
		i = getnum(&p, 9);
		if (*p == ')')
			p = skipspace(p + 1);
		else
			i = INVALID;
	} else if ('0' <= *p && *p <= '9') {
		i = mystrtol(p, &p);
	} else {
		i = INVALID;
	}

	/* binary operator */
	for (;;) {
		if (i == INVALID)
			break;

		p = skipspace(p);
		if (*p == '+' && priority > 2) {
			p = skipspace(p + 1);
			j = getnum(&p, 2);
			if (j != INVALID)
				i += j;
			else
				i = INVALID;
		} else if (*p == '-' && priority > 2) {
			p = skipspace(p + 1);
			j = getnum(&p, 2);
			if (j != INVALID)
				i -= j;
			else
				i = INVALID;
		} else if (*p == '*' && priority > 1) {
			p = skipspace(p + 1);
			j = getnum(&p, 1);
			if (j != INVALID)
				i *= j;
			else
				i = INVALID;
		} else if (*p == '/' && priority > 1) {
			p = skipspace(p + 1);
			j = getnum(&p, 1);
			if (j != INVALID && j != 0)
				i /= j;
			else
				i = INVALID;
		} else if (*p == '%' && priority > 1) {
			p = skipspace(p + 1);
			j = getnum(&p, 1);
			if (j != INVALID && j != 0)
				i %= j;
			else
				i = INVALID;
		} else if (*p == '<' && p[1] == '<' && priority > 3) {
			p = skipspace(p + 2);
			j = getnum(&p, 3);
			if (j != INVALID && j != 0)
				i <<= j;
			else
				i = INVALID;
		} else if (*p == '>' && p[1] == '>' && priority > 3) {
			p = skipspace(p + 2);
			j = getnum(&p, 3);
			if (j != INVALID && j != 0)
				i >>= j;
			else
				i = INVALID;
		} else if (*p == '&' && priority > 4) {
			p = skipspace(p + 1);
			j = getnum(&p, 4);
			if (j != INVALID)
				i &= j;
			else
				i = INVALID;
		} else if (*p == '^' && priority > 5) {
			p = skipspace(p + 1);
			j = getnum(&p, 5);
			if (j != INVALID)
				i ^= j;
			else
				i = INVALID;
		} else if (*p == '|' && priority > 6) {
			p = skipspace(p + 1);
			j = getnum(&p, 6);
			if (j != INVALID)
				i |= j;
			else
				i = INVALID;
		} else {
			break;
		}
	}

	p = skipspace(p);
	*pp = p;
	return i;
}
