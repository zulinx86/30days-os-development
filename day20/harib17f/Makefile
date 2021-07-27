OBJS_BOOTPACK = nasmfunc.obj $(patsubst %.c,%.obj,$(wildcard *.c))
HDRS_BOOTPACK = mystdio.h bootpack.h

default:
	make img

ipl10.bin: ipl10.asm
	nasm -o ipl10.bin -l ipl10.lst ipl10.asm

asmhead.bin: asmhead.asm
	nasm -o asmhead.bin -l asmhead.lst asmhead.asm

nasmfunc.obj: nasmfunc.asm
	nasm -f elf32 -o nasmfunc.obj -l nasmfunc.lst nasmfunc.asm

%.obj: %.c
	i386-elf-gcc -c -march=i486 -m32 -nostdlib -Wall -o $@ $<

bootpack.hrb: $(OBJS_BOOTPACK) $(HDRS_BOOTPACK) hrb.ld
	i386-elf-gcc -march=i486 -m32 -nostdlib -Wall -T hrb.ld -Wl,-Map=bootpack.map -o bootpack.hrb $(OBJS_BOOTPACK)

hello.hrb: hello.asm
	nasm -o hello.hrb -l hello.lst hello.asm

haribote.sys: asmhead.bin bootpack.hrb
	cat asmhead.bin bootpack.hrb > haribote.sys

haribote.img: ipl10.bin haribote.sys hello.hrb
	mformat -f 1440 -C -B ipl10.bin -i haribote.img
	mcopy -i haribote.img haribote.sys ::haribote.sys
	mcopy -i haribote.img hello.hrb ::hello.hrb

img:
	make -r haribote.img

run:
	make img
	qemu-system-i386 -drive file=haribote.img,if=floppy,format=raw -boot a -m 32M

clean:
	rm -rf *.bin *.lst *.img *.sys *.obj *.hrb *.map
