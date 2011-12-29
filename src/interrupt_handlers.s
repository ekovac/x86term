global kb_isr
global serial_isr
extern kb_handler
extern serial_handler

kb_isr:
    cli
    pushad
    call kb_handler
    popad
    sti
    iret
serial_isr:
    cli
    pushad
    call serial_handler
    popad
    sti
    iret
