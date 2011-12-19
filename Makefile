CC=gcc
LD=ld
AS=nasm
CC_OPTS=-c -m32 -Wall -Wextra -nostdlib -fno-builtin -nodefaultlibs
LD_OPTS=-melf_i386 
AS_OPTS=-f elf32
GRUB_URL=ftp://alpha.gnu.org/gnu/grub/grub-0.97-i386-pc.tar.gz
# Bootstrap code taken from wiki.osdev.org/Bare_bones
all: tvtypewriter.bin
clean:
	rm *.o *.bin
loader.o: loader.s
	$(AS) $(AS_OPTS) -o $@ $?
tvtypewriter.o: tvtypewriter.c
	$(CC) $(CC_OPTS) -o $@ $?
tvtypewriter.bin: loader.o tvtypewriter.o 
	$(LD) $(LD_OPTS) -T linker.ld -o $@ $?
test: tvtypewriter.bin
	qemu -kernel $?
