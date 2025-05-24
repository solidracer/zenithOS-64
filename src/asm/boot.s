bits 64

section .boot
    dd 0x314E455A
    align 8
global info
info:
    dq 0x0 ; fb
    dd 0x0 ; width
    dd 0x0 ; height
    dd 0x0 ; ppsl
    dd 0x0 ; pixel_format
    align 8
global memmap
memmap:
    dq 0x0
    dq 0x0
    dq 0x0

section .boot.text
global _start
_start:
    cli
    mov rsp, stack - 0xFFFFFFFF80000000
    extern low_entry
    call low_entry
section .text
global high_entry
high_entry:
    mov rsp, stack
    extern kmain
    call kmain
.halt:
    hlt
    jmp .halt

section .bss
    align 16
    resb 0x4000
stack:
