#include "libapi.h"

void HariMain(void)
{
	static unsigned char s[9] = { 0xb2, 0xdb, 0xca, 0xc6, 0xce, 0xcd, 0xc4, 0x0a, 0x00 };
	api_putstr(s);
	api_end();
}
