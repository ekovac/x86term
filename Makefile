CC=gcc
LD=ld
AS=nasm
CC_OPTS=-m32 -Wall -Wextra -nostdlib -fno-builtin -nostartfiles -nodefaultlibs
LD_OPTS=-melf_i386 
AS_OPTS=-f elf
# Bootstrap code taken from wiki.osdev.org/Bare_bones
all: tvtypewriter.bin
clean:
	rm *.o *.bin
loader.o: loader.s
	$(AS) $(AS_OPTS) -o $@ $?
tvtypewriter.o: tvtypewriter.c
	$(CC) $(CC_OPTS) -o $@ $?
tvtypewriter.bin: tvtypewriter.o loader.o 
	$(LD) $(LD_OPTS) -T linker.ld -o $@ $?

