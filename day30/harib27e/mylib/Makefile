OBJS = mystdio.obj mystring.obj mystdlib.obj

.PHONY: default
default: $(wildcard *.c) $(wildcard *.h)
	make $(OBJS)

%.obj: %.c
	i386-elf-gcc -c -march=i486 -m32 -nostdlib -Wall -o $@ $<

.PHONY: clean
clean:
	rm -f *.obj
