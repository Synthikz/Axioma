global syscall
global isr128
global irq1_handler

extern syscall_interrupt_handler
extern double_fault_handler
extern default_interrupt_handler

extern keyboard_interrupt_handler

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
    popad
    pop ebp
    ret

irq1_handler:
    cli
    pushad
    call keyboard_interrupt_handler
    popad
    sti
    iret

isr128:
    cli
    pushad        
    mov eax, [esp + 32]    ; syscall number
    mov ebx, [esp + 36]    ; arg1
    mov ecx, [esp + 40]    ; arg2
    mov edx, [esp + 44]    ; arg3

    push edx               ; arg3
    push ecx               ; arg2
    push ebx               ; arg1
    push eax               ; syscall number
    call syscall_interrupt_handler
    add esp, 16 

    mov [esp + 28], eax 

    popad
    sti
    iret

isr8:
    cli   
    push ebp  
    mov ebp, esp  
    pushad     
    call double_fault_handler
    popad
    pop ebp        
    hlt