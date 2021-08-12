APP = gview
OBJS = bmp.obj jpeg.obj

LIBAPIPATH := ../libapi/
MYLIBPATH := ../mylib/
MYLIBOBJS := $(MYLIBPATH)mystdio.obj $(MYLIBPATH)mystring.obj $(MYLIBPATH)mystdlib.obj

.PHONY: default
default:
	make -r $(APP).hrb

$(APP).obj: $(APP).c
	i386-elf-gcc -c -march=i486 -m32 -nostdlib -Wall -I$(MYLIBPATH) -I$(LIBAPIPATH) -o $(APP).obj $(APP).c

bmp.obj: bmp.asm
	nasm -f elf32 -o $@ $<

jpeg.obj: jpeg.c
	i386-elf-gcc -c -march=i486 -m32 -nostdlib -Wall -o $@ $<

$(APP).hrb: $(APP).obj $(OBJS) $(LIBAPIPATH)libapi.h $(LIBAPIPATH)libapi.a $(LINKERPATH)app.ld
	i386-elf-gcc -march=i486 -m32 -nostdlib -T app.ld -o $(APP).hrb $(APP).obj $(OBJS) $(MYLIBOBJS) -L$(LIBAPIPATH) -static -lapi

.PHONY: clean
clean:
	rm -f *.obj *.hrb
