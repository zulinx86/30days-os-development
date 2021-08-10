MYLIBPATH := ../mylib/
OBJS_BOOTPACK := bootpack.obj nasmfunc.obj hankaku.obj graphic.obj dsctbl.obj int.obj fifo.obj keyboard.obj mouse.obj memory.obj sheet.obj timer.obj mtask.obj window.obj console.obj file.obj $(MYLIBPATH)mystdio.obj $(MYLIBPATH)mystring.obj
HDRS_BOOTPACK := $(MYLIBPATH)mystdio.h $(MYLIBPATH)mystring.h bootpack.h

.PHONY: default
default:
	make -r ipl20.bin
	make -r haribote.sys

%.bin: %.asm
	nasm -o $@ $<

%.obj: %.c
	i386-elf-gcc -c -march=i486 -m32 -nostdlib -Wall -I$(MYLIBPATH) -o $@ $<

nasmfunc.obj: nasmfunc.asm
	nasm -f elf32 -o nasmfunc.obj nasmfunc.asm

bootpack.hrb: $(OBJS_BOOTPACK) $(HDRS_BOOTPACK) hrb.ld
	i386-elf-gcc -march=i486 -m32 -nostdlib -Wall -T hrb.ld -Wl,-Map=bootpack.map -o bootpack.hrb $(OBJS_BOOTPACK)

haribote.sys: asmhead.bin bootpack.hrb
	cat asmhead.bin bootpack.hrb > haribote.sys

.PHONY: clean
clean:
	rm -rf *.bin *.sys *.obj *.hrb *.map

