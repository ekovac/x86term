global kb_isr
global serial_isr
extern kb_handler
extern serial_handler

kb_isr:
    cli
    pushad
    push byte 1
    call kb_handler
    add esp, 4
    popad
    sti
    iret
serial_isr:
    cli
    pushad
    push byte 4
    call serial_handler
    add esp, 4
    popad
    sti
    iret
