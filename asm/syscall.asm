global syscall
global isr128

extern syscall_interrupt_handler

section .text

syscall:
    push ebp
    mov ebp, esp
    push ebx
    push esi
    push edi
    
    mov eax, [ebp + 8]
    mov ebx, [ebp + 12]
    mov ecx, [ebp + 16]
    mov edx, [ebp + 20]
    int 0x80
    
    pop edi
    pop esi
    pop ebx
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

    mov [esp + 28], eax

    popad
    iret