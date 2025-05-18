bits 64

extern GDTR
global flush_gdt

section .text
flush_gdt:

    lgdt [GDTR]
    push 0x08
    push .reload_cs
    retfq

.reload_cs:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret
