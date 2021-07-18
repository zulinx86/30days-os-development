default:
	make img

%.bin: %.asm
	nasm -o $@ -l $(<:.asm=.lst) $<

bootpack.hrb: bootpack.c hrb.ld
	i386-elf-gcc -march=i486 -m32 -nostdlib -T hrb.ld -o bootpack.hrb bootpack.c

haribote.sys: asmhead.bin bootpack.hrb Makefile
	cat asmhead.bin bootpack.hrb > haribote.sys

haribote.img: ipl10.bin haribote.sys
	mformat -f 1440 -C -B ipl10.bin -i haribote.img
	mcopy -i haribote.img haribote.sys ::haribote.sys

img:
	make -r haribote.img

run:
	make img
	qemu-system-i386 -drive file=haribote.img,if=floppy,format=raw -boot a

clean:
	rm -rf *.bin *.lst *.hrb *.sys *.img
