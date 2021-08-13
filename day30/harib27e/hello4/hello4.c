#include "libapi.h"

void HariMain(void)
{
	api_putstr((unsigned char *) "hello, world\n");
	api_end();
}
