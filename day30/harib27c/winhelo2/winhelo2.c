#include "libapi.h"

void HariMain(void)
{
	int win;
	unsigned char buf[150 * 50];
	win = api_openwin((unsigned char *) buf, 150, 50, -1, (unsigned char *) "hello");
	api_boxfillwin(win, 8, 36, 141, 43, 3);
	api_putstrwin(win, 28, 28, 0, 12, (unsigned char *) "hello, world");
	for (;;) {
		if (api_getkey(1) == 0x0a)
			break;
	}
	api_end();
}
