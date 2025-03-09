global syscall
global isr128
extern syscall_interrupt_handler

section .text
syscall:
    push ebp
    mov ebp, esp
    pushad
    mov eax, [ebp + 8]    ; syscall number
    mov ebx, [ebp + 12]   ; arg1
    mov ecx, [ebp + 16]   ; arg2
    mov edx, [ebp + 20]   ; arg3
    int 0x80
    mov [esp + 28], eax   ; Save return value
    popad
    pop ebp
    ret

isr128:
    pushad
    push edx
    push ecx
    push ebx
    push eax
    call syscall_interrupt_handler
    add esp, 16
    mov [esp + 28], eax   ; Save return value
    popad
    iret
