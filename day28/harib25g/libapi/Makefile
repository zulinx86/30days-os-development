
default:
	make -r libapi.a

%.obj: %.asm
	nasm -f elf32 -o $@ -l $(patsubst %.obj,%.lst,$@) $<

libapi.a: $(patsubst %.asm,%.obj,$(wildcard *.asm))
	ar -rc $@ libapi.a $^
	i386-elf-ranlib libapi.a

clean:
	rm -rf *.obj *.lst *.a
