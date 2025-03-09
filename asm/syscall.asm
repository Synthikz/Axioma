global syscall
global isr128
global isr8
extern syscall_interrupt_handler
extern double_fault_handler

section .text

; --- SYSCALL HANDLER (INT 0x80) ---
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

; --- INTERRUPT HANDLER (SYSCALL - ISR 128) ---
isr128:
    cli                  ; Disable interrupts
    push ebp            
    mov ebp, esp
    sub esp, 16         ; Align stack
    pushad              ; Save all registers
    
    push edx            ; Push syscall arguments
    push ecx
    push ebx
    push eax
    
    call syscall_interrupt_handler
    
    add esp, 16         ; Clean up arguments
    
    popad               ; Restore registers
    mov esp, ebp
    pop ebp
    sti                 ; Re-enable interrupts
    iret

; --- DOUBLE FAULT HANDLER (INT 8) ---
isr8:
    cli                  ; Deshabilitar interrupciones
    push ebp             ; Save EBP for stack alignment
    mov ebp, esp         ; Set EBP to current stack pointer
    pushad               ; Guardar el estado de la CPU
    call double_fault_handler
    popad
    pop ebp              ; Restore EBP
    hlt                  ; Detener el sistema (double fault es crítico)
