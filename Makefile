CC=gcc
LD=ld
AS=nasm
CC_OPTS=-c -m32 -Wall -Wextra -nostdlib -fno-builtin -nodefaultlibs
LD_OPTS=-melf_i386 
AS_OPTS=-f elf32
GRUB_URL=ftp://alpha.gnu.org/gnu/grub/grub-0.97-i386-pc.tar.gz
# Bootstrap code taken from wiki.osdev.org/Bare_bones
all: x86term
floppy: floppy.img
clean:
	rm -f *.o *.bin *.img x86term
loader.o: loader.s
	$(AS) $(AS_OPTS) -o $@ $?
gdt.o: gdt.c gdt.h
	$(CC) $(CC_OPTS) -o $@ gdt.c
x86term.o: x86term.c gdt.h
	$(CC) $(CC_OPTS) -o $@ x86term.c
x86term: loader.o gdt.o x86term.o
	$(LD) $(LD_OPTS) -T linker.ld -o $@ $?
test: x86term
	qemu -kernel $?
floppy.img: x86term
	cat stage1 stage2 > floppy.img
	dd if=/dev/zero append of=floppy.img bs=1 count=200
	cat x86term >> floppy.img
floppytest: floppy.img
	bochs -q 'boot:a' 'floppya: 1_44=floppy.img, status=inserted'
