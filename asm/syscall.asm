global syscall
global isr128

extern syscall_interrupt_handler

section .text

; Syscall function for programs to use
syscall:
    mov eax, [esp + 4]   ; syscall number
    mov ebx, [esp + 8]   ; arg1
    mov ecx, [esp + 12]  ; arg2
    mov edx, [esp + 16]  ; arg3
    int 0x80
    ret

; Interrupt service routine for syscall (int 0x80)
isr128:
    pushad                  ; Push all registers

    ; Call C++ handler
    push edx                ; arg3
    push ecx                ; arg2  
    push ebx                ; arg1
    push eax                ; syscall number
    call syscall_interrupt_handler
    add esp, 16             ; Clean up arguments

    mov [esp + 28], eax     ; Store return value in the original EAX position on stack

    popad                   ; Restore all registers
    iret                    ; Return from interrupt