CC=gcc
LD=ld
AS=nasm
LDFLAGS+=-melf_i386 
ASFLAGS+=-f elf32
GRUB_URL=ftp://alpha.gnu.org/gnu/grub/grub-0.97-i386-pc.tar.gz
LIBVTERM=../libvterm/
CFLAGS+=-g -std=c99 -c -m32 -Wall -nostdlib -fno-builtin -nodefaultlibs -Iinclude/ -I$(LIBVTERM)/include/
VTERMCFILES=$(wildcard $(LIBVTERM)/src/*.c)
VTERMOFILES=$(VTERMCFILES:.c=.o)
TBLFILES=$(wildcard $(LIBVTERM)/src/encoding/*.tbl)
INCFILES=$(TBLFILES:.tbl=.inc)
OBJECTS=obj/loader.o obj/base.o obj/display.o obj/interrupt_handlers.o obj/interrupts.o obj/keys.o obj/pic.o obj/main.o obj/segments.o obj/tables.o obj/serial.o obj/stub.o obj/ringbuf.o obj/termhandlers.o obj/timer.o
# Bootstrap code taken from wiki.osdev.org/Bare_bones
all: x86term
floppy: floppy.img
clean:
	rm -f obj/*.o *.bin *.img x86term $(LIBVTERM)/src/*.o x86term.strip
obj/interrupt_handlers.o: src/interrupt_handlers.s
	$(AS) $(ASFLAGS) $?  -o $@
obj/loader.o: src/loader.s
	$(AS) $(ASFLAGS) $? -o $@
obj/tables.o: src/tables.s
	$(AS) $(ASFLAGS) $? -o $@
$(LIBVTERM)/src/%.o: $(LIBVTERM)/src/%.c 
	$(CC) $(CFLAGS) $? -o $@
$(LIBVTERM)/src/encoding.o: $(LIBVTERM)/src/encoding.c $(INCFILES)
	$(CC) $(CFLAGS) $? -o $@	
$(LIBVTERM)/src/encoding/%.inc: $(LIBVTERM)/src/encoding/%.tbl
	perl -C $(LIBVTERM)/tbl2inc_c.pl $< > $@
obj/vterm.o: $(VTERMOFILES)
	$(LD) -r $(LDFLAGS) $^ -o $@
obj/%.o: src/%.c
	$(CC) $(CFLAGS) -o $@ $?
x86term: $(OBJECTS) obj/vterm.o
	$(LD) $(LDFLAGS) -T linker.ld -o $@ $^
test: x86term
	./run-shell
floppytest: x86term floppy.img
	qemu -serial stdio -fda floppy.img
debug: x86term
	qemu -s -S -serial stdio -kernel $?
floppydebug: floppy.img
	qemu -s -S -serial stdio -fda floppy.img
# Floppy target depends on mtools, syslinux
x86term.strip: x86term
	strip x86term -o x86term.strip
floppy.img: x86term.strip
	dd if=/dev/zero bs=512 count=2880 of=floppy.img.tmp
	/sbin/mkfs.msdos -n x86term floppy.img.tmp
	syslinux --install floppy.img.tmp
	mcopy -i floppy.img.tmp x86term.strip ::x86term
	mcopy -i floppy.img.tmp syslinux.cfg /usr/lib/syslinux/mboot.c32 ::
	mv floppy.img.tmp floppy.img
