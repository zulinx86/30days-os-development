LINKERPATH = ../

hello5.hrb: hello5.asm $(LINKERPATH)app.ld
	nasm -f elf32 -o hello5.obj hello5.asm
	i386-elf-gcc -march=i486 -m32 -nostdlib -T $(LINKERPATH)app.ld -o hello5.hrb hello5.obj

.PHONY: clean
clean:
	rm -f *.obj *.hrb
