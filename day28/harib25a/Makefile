SHELL := /bin/bash
APPS := a hello3 hello4 hello5 winhelo1 winhelo2 winhelo3 star1 stars1 stars2 lines walk noodle beepdown color1 color2 sosu1 sosu2 sosu3
APP_HRBS := $(foreach APP,$(APPS),$(APP)/$(APP).hrb)

.PHONY: default
default:
	make img

.PHONY: img
img:
	make -C libapi
	make -C mylib
	for APP in $(APPS); do make -C $$APP; done
	make -r haribote.img

.PHONY: run
run:
	make img
	qemu-system-i386 -drive file=haribote.img,if=floppy,format=raw -boot a -m 32M

haribote.img: haribote/ipl10.bin haribote/haribote.sys $(APP_HRBS)
	mformat -f 1440 -C -B haribote/ipl10.bin -i haribote.img
	mcopy -i haribote.img haribote/haribote.sys ::haribote.sys
	mcopy -i haribote.img $(APP_HRBS) ::

.PHONY: clean
clean:
	for APP in $(APPS); do make -C $$APP clean; done
	make -C libapi clean
	make -C mylib clean
	rm -f *.img
