MBALIGN equ 1 << 0
MEMINFO equ 1 << 1 
MBFLAGS equ MBALIGN | MEMINFO
MAGIC equ 0x1BADB002
CHECKSUM equ -(MAGIC + MBFLAGS)


section .multiboot
align 4
    dd MAGIC
    dd MBFLAGS
    dd CHECKSUM

section .bss
align 16
stack_bottom:
resb 16384 ; 16KB
stack_top:

section .text
global _start:function (_start.end - _start)
_start:
    mov esp, stack_top
    extern axio_main ; Kernal Main
    call axio_main
    
.hang: hlt
    jmp .hang 
.end: