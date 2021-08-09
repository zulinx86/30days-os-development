#include "libapi.h"

unsigned int rand(void) {
	static unsigned int x = 10;
	unsigned int a = 48271, m = 2147483647;
	x = (a * x) % m;
    return x;
}

void HariMain(void)
{
	char *buf;
	int win, i, x, y;
	api_initmalloc();
	buf = api_malloc(150 * 100);
	win = api_openwin(buf, 150, 100, -1, "stars");
	api_boxfillwin(win,  6, 26, 143, 93, 0);
	for (i = 0; i < 50; i++) {
		x = (rand() % 137) +  6;
		y = (rand() %  67) + 26;
		api_point(win, x, y, 3);
	}
	for (;;) {
		if (api_getkey(1) == 0x0a)
			break;
	}
	api_end();
}

