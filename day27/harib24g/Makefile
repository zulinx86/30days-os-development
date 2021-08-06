APPS = a/a.hrb hello3/hello3.hrb hello4/hello4.hrb hello5/hello5.hrb \
	winhelo1/winhelo1.hrb winhelo2/winhelo2.hrb winhelo3/winhelo3.hrb \
	star1/star1.hrb stars1/stars1.hrb stars2/stars2.hrb lines/lines.hrb \
	walk/walk.hrb noodle/noodle.hrb beepdown/beepdown.hrb color1/color1.hrb color2/color2.hrb

.PHONY: default
default:
	make img

.PHONY: img
img:
	make -C haribote
	make -C libapi
	make -C a
	make -C hello3
	make -C hello4
	make -C hello5
	make -C winhelo1
	make -C winhelo2
	make -C winhelo3
	make -C star1
	make -C stars1
	make -C stars2
	make -C lines
	make -C walk
	make -C noodle
	make -C beepdown
	make -C color1
	make -C color2
	make -r haribote.img

.PHONY: run
run:
	make img
	qemu-system-i386 -drive file=haribote.img,if=floppy,format=raw -boot a -m 32M

haribote.img: haribote/ipl10.bin haribote/haribote.sys $(APPS)
	mformat -f 1440 -C -B haribote/ipl10.bin -i haribote.img
	mcopy -i haribote.img haribote/haribote.sys ::haribote.sys
	mcopy -i haribote.img $(APPS) ::

.PHONY: clean
clean:
	make -C haribote clean
	make -C libapi clean
	make -C a clean
	make -C hello3 clean
	make -C hello4 clean
	make -C hello5 clean
	make -C winhelo1 clean
	make -C winhelo2 clean
	make -C winhelo3 clean
	make -C star1 clean
	make -C stars1 clean
	make -C stars2 clean
	make -C lines clean
	make -C walk clean
	make -C noodle clean
	make -C beepdown clean
	make -C color1 clean
	make -C color2 clean
	rm -f *.img
