global loader                           ; making entry point visible to linker
extern gdt
extern interrupt_handler
global set_gdt
extern kmain                            ; kmain is defined in kmain.cpp
extern print_byte
extern print_bytes
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
GDTSIZE equ 0x0800
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

gdtr DW 0
     DD 0
idtr DW 0
     DD 0
set_gdt:
    mov eax, [esp+8]
    mov [gdtr+2], eax
    mov eax, [esp+12]
    mov [gdtr], eax
    lgdt [gdtr]
    jmp 0x08:flush_segments
    ret

flush_segments:
    mov ax, 0x10
    mov dx, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret
simple_isr:
    pushad
    call interrupt_handler
    popad

section .bss

align 4
stack:
    resb STACKSIZE                      ; reserve 16k stack on a doubleword boundary
