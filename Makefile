CC=gcc
LD=ld
AS=nasm
CC_OPTS=-g -c -m32 -Wall -Wextra -nostdlib -fno-builtin -nodefaultlibs
LD_OPTS=-melf_i386 
AS_OPTS=-f elf32
GRUB_URL=ftp://alpha.gnu.org/gnu/grub/grub-0.97-i386-pc.tar.gz
# Bootstrap code taken from wiki.osdev.org/Bare_bones
all: x86term
floppy: floppy.img
stage1:
	wget -qO - $(GRUB_URL) | tar zx grub-0.97-i386-pc/boot/grub/stage1 grub-0.97-i386-pc/boot/grub/stage2 --strip-components 3
clean:
	rm -f *.o *.bin *.img x86term
keys.o: keys.c keys.h
	$(CC) $(CC_OPTS) -o $@ keys.c
pic.o: pic.h pic.c
	$(CC) $(CC_OPTS) -o $@ pic.c
iobase.o: iobase.h iobase.c
	$(CC) $(CC_OPTS) -o $@ iobase.c
loader.o: loader.s
	$(AS) $(AS_OPTS) -o $@ $?
x86term.o: x86term.c
	$(CC) $(CC_OPTS) -o $@ x86term.c
x86term: loader.o x86term.o pic.o iobase.o keys.o
	$(LD) $(LD_OPTS) -T linker.ld -o $@ loader.o pic.o iobase.o keys.o x86term.o 
test: x86term
	qemu -serial stdio -kernel $?
floppy.img: x86term stage1
	cat stage1 stage2 > floppy.img.tmp
	dd if=/dev/zero conv=notrunc oflag=append of=floppy.img.tmp bs=1 count=750
	cat x86term >> floppy.img.tmp
	mv floppy.img.tmp floppy.img
floppytest: floppy.img
	bochs -qf bochs.cfg
debug: floppy.img
	bochs -qf bochs.cfg 'magic_break: enabled=1'
