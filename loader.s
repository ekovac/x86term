global loader                           ; making entry point visible to linker
global gdt
global flush_segments
extern kmain                            ; kmain is defined in kmain.cpp
BITS 32
; setting up the Multiboot header - see GRUB docs for details
MODULEALIGN equ  1<<0                   ; align loaded modules on page boundaries
MEMINFO     equ  1<<1                   ; provide memory map
FLAGS       equ  MODULEALIGN | MEMINFO  ; this is the Multiboot 'flag' field
MAGIC       equ    0x1BADB002           ; 'magic number' lets bootloader find the header
CHECKSUM    equ -(MAGIC + FLAGS)        ; checksum required
 
section .text
 
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM
 
; reserve initial kernel stack space
STACKSIZE equ 0x4000                    ; that's a lot.
_start: 
loader:
    mov  esp, stack + STACKSIZE         ; set up the stack
    push eax                            ; Multiboot magic number
    push ebx                            ; Multiboot info structure
 
    call kmain                          ; call kernel proper
 
    cli
.hang:
    hlt                                 ; halt machine should kernel return
    jmp  .hang

flush_segments:
    mov ax, 0x10
    mov dx, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret
section .bss

align 4
stack:
    resb STACKSIZE                      ; reserve 16k stack on a doubleword boundary
gdt:
    resb 0x0800                         ; reserve 4 8-byte entries for gdt;
