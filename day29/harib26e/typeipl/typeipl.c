#include "libapi.h"

void HariMain(void)
{
	int fh;
	unsigned char c;
	fh = api_fopen((unsigned char *)"ipl10.asm");
	if (fh != 0) {
		for (;;) {
			if (api_fread(&c, 1, fh) == 0)
				break;
			api_putchar(c);
		}
	}
	api_end();
}
