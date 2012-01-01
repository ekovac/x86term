CC=gcc
LD=ld
AS=nasm
LDFLAGS=-melf_i386 
ASFLAGS=-f elf32
GRUB_URL=ftp://alpha.gnu.org/gnu/grub/grub-0.97-i386-pc.tar.gz
LIBVTERM=../libvterm/
CFLAGS=-g -std=c99 -c -m32 -Wall -Wextra -nostdlib -fno-builtin -nodefaultlibs -Iinclude/ -I$(LIBVTERM)/include/
VTERMCFILES=$(wildcard $(LIBVTERM)/src/*.c)
VTERMOFILES=$(VTERMCFILES:.c=.o)
TBLFILES=$(wildcard $(LIBVTERM)/src/encoding/*.tbl)
INCFILES=$(TBLFILES:.tbl=.inc)
OBJECTS=obj/loader.o obj/base.o obj/display.o obj/interrupt_handlers.o obj/interrupts.o obj/keys.o obj/pic.o obj/main.o obj/segments.o obj/tables.o obj/serial.o obj/stub.o obj/ringbuf.o obj/termhandlers.o
# Bootstrap code taken from wiki.osdev.org/Bare_bones
all: x86term
floppy: floppy.img
stage1:
	wget -qO - $(GRUB_URL) | tar zx grub-0.97-i386-pc/boot/grub/stage1 grub-0.97-i386-pc/boot/grub/stage2 --strip-components 3
clean:
	rm -f obj/*.o *.bin *.img x86term
obj/interrupt_handlers.o: src/interrupt_handlers.s
	$(AS) $(ASFLAGS) $?  -o $@
obj/loader.o: src/loader.s
	$(AS) $(ASFLAGS) $? -o $@
obj/tables.o: src/tables.s
	$(AS) $(ASFLAGS) $? -o $@
$(LIBVTERM)/src/%.o: $(LIBVTERM)/src/%.c 
	$(CC) $(CFLAGS) $? -o $@
$(LIBVTERM)/src/encoding.o: $(LIBVTERM)/src/encoding.c $(INCFILES)
	
$(LIBVTERM)/src/encoding/%.inc: $(LIBVTERM)/src/encoding/%.tbl
	perl -C $(LIBVTERM)/tbl2inc_c.pl $< > $@
obj/vterm.o: $(VTERMOFILES)
	$(LD) -r $(LDFLAGS) $^ -o $@
obj/%.o: src/%.c
	$(CC) $(CFLAGS) -o $@ $?
x86term: $(OBJECTS) obj/vterm.o
	$(LD) $(LDFLAGS) -T linker.ld -o $@ $^
test: x86term
	qemu -serial stdio -kernel $?
debug: x86term
	qemu -s -S -serial stdio -kernel $?
floppydebug: floppy.img
	qemu -s -S -serial stdio -fda floppy.img
floppy.img: x86term stage1
	cat stage1 stage2 > floppy.img.tmp
	dd if=/dev/zero conv=notrunc oflag=append of=floppy.img.tmp bs=1 count=750
	cat x86term >> floppy.img.tmp
	mv floppy.img.tmp floppy.img
bochs: floppy.img
	bochs -qf bochs.cfg 'gdbstub: enabled=0'
bochsdebug: floppy.img
	bochs -qf bochs.cfg
