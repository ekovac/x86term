CC=gcc
LD=ld
AS=nasm
CC_OPTS=-g -c -m32 -Wall -Wextra -nostdlib -fno-builtin -nodefaultlibs -Iinclude/
LD_OPTS=-melf_i386 
AS_OPTS=-f elf32
GRUB_URL=ftp://alpha.gnu.org/gnu/grub/grub-0.97-i386-pc.tar.gz
OBJECTS=obj/loader.o obj/base.o obj/display.o obj/interrupt_handlers.o obj/interrupts.o obj/keys.o obj/pic.o obj/main.o obj/segments.o obj/tables.o obj/serial.o 
# Bootstrap code taken from wiki.osdev.org/Bare_bones
all: x86term
floppy: floppy.img
stage1:
	wget -qO - $(GRUB_URL) | tar zx grub-0.97-i386-pc/boot/grub/stage1 grub-0.97-i386-pc/boot/grub/stage2 --strip-components 3
clean:
	rm -f *.o *.bin *.img x86term
obj/interrupt_handlers.o: src/interrupt_handlers.s
	$(AS) $(AS_OPTS) $?  -o $@
obj/loader.o: src/loader.s
	$(AS) $(AS_OPTS) $? -o $@
obj/tables.o: src/tables.s
	$(AS) $(AS_OPTS) $? -o $@
obj/%.o: src/%.c
	$(CC) $(CC_OPTS) -o $@ $?
x86term: $(OBJECTS)
	$(LD) $(LD_OPTS) -T linker.ld -o $@ $(OBJECTS)
test: x86term
	qemu -serial stdio -kernel $?
floppy.img: x86term stage1
	cat stage1 stage2 > floppy.img.tmp
	dd if=/dev/zero conv=notrunc oflag=append of=floppy.img.tmp bs=1 count=750
	cat x86term >> floppy.img.tmp
	mv floppy.img.tmp floppy.img
floppytest: floppy.img
	bochs -qf bochs.cfg
