OBJS_BOOTPACK = bootpack.obj nasmfunc.obj hankaku.obj mystdio.obj mystring.obj graphic.obj dsctbl.obj int.obj fifo.obj keyboard.obj mouse.obj memory.obj sheet.obj timer.obj mtask.obj window.obj console.obj file.obj
HDRS_BOOTPACK = mystdio.h mystring.h bootpack.h
APPS = hello.hrb hello2.hrb hello3.hrb a.hrb hello4.hrb hello5.hrb winhello.hrb winhelo2.hrb winhelo3.hrb star1.hrb stars.hrb stars2.hrb lines.hrb walk.hrb

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

hello2.hrb: hello2.asm
	nasm -o hello2.hrb -l hello2.lst hello2.asm

a_nasm.obj: a_nasm.asm
	nasm -f elf32 -o a_nasm.obj -l a_nasm.lst a_nasm.asm

a.hrb: a.obj a_nasm.obj app.ld
	i386-elf-gcc -march=i486 -m32 -nostdlib -T app.ld -o a.hrb a.obj a_nasm.obj

hello3.hrb: hello3.obj a_nasm.obj app.ld
	i386-elf-gcc -march=i486 -m32 -nostdlib -T app.ld -o hello3.hrb hello3.obj a_nasm.obj

hello4.hrb: hello4.obj a_nasm.obj app.ld
	i386-elf-gcc -march=i486 -m32 -nostdlib -T app.ld -o hello4.hrb hello4.obj a_nasm.obj

hello5.hrb: hello5.asm
	nasm -f elf32 -o hello5.obj -l hello5.lst hello5.asm
	i386-elf-gcc -march=i486 -m32 -nostdlib -T app.ld -o hello5.hrb hello5.obj

winhello.hrb: winhello.obj a_nasm.obj app.ld
	i386-elf-gcc -march=i486 -m32 -nostdlib -T app.ld -o winhello.hrb winhello.obj a_nasm.obj

winhelo2.hrb: winhelo2.obj a_nasm.obj app.ld
	i386-elf-gcc -march=i486 -m32 -nostdlib -T app.ld -o winhelo2.hrb winhelo2.obj a_nasm.obj

winhelo3.hrb: winhelo3.obj a_nasm.obj app.ld
	i386-elf-gcc -march=i486 -m32 -nostdlib -T app.ld -o winhelo3.hrb winhelo3.obj a_nasm.obj

star1.hrb: star1.obj a_nasm.obj app.ld
	i386-elf-gcc -march=i486 -m32 -nostdlib -T app.ld -o star1.hrb star1.obj a_nasm.obj

stars.hrb: stars.obj a_nasm.obj app.ld
	i386-elf-gcc -march=i486 -m32 -nostdlib -T app.ld -o stars.hrb stars.obj a_nasm.obj

stars2.hrb: stars2.obj a_nasm.obj app.ld
	i386-elf-gcc -march=i486 -m32 -nostdlib -T app.ld -o stars2.hrb stars2.obj a_nasm.obj

lines.hrb: lines.obj a_nasm.obj app.ld
	i386-elf-gcc -march=i486 -m32 -nostdlib -T app.ld -o lines.hrb lines.obj a_nasm.obj

walk.hrb: walk.obj a_nasm.obj app.ld
	i386-elf-gcc -march=i486 -m32 -nostdlib -T app.ld -o walk.hrb walk.obj a_nasm.obj

haribote.sys: asmhead.bin bootpack.hrb
	cat asmhead.bin bootpack.hrb > haribote.sys

haribote.img: ipl10.bin haribote.sys $(APPS)
	mformat -f 1440 -C -B ipl10.bin -i haribote.img
	mcopy -i haribote.img haribote.sys ::haribote.sys
	mcopy -i haribote.img $(APPS) ::

img:
	make -r haribote.img

run:
	make img
	qemu-system-i386 -drive file=haribote.img,if=floppy,format=raw -boot a -m 32M

clean:
	rm -rf *.bin *.lst *.img *.sys *.obj *.hrb *.map
