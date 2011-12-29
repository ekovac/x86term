global kb_isr
global serial_isr
extern interrupt_handler

kb_isr:
    cli
    pushad
    push byte 1
    call interrupt_handler
    add esp, 4
    popad
    sti
    iret
serial_isr:
    cli
    pushad
    push byte 4
    call interrupt_handler
    add esp, 4
    popad
    sti
    iret
