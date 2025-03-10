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
    push ebp            
    mov ebp, esp
    sub esp, 16       
    pushad        
    
    push edx      
    push ecx
    push ebx
    push eax
    
    call syscall_interrupt_handler
    
    add esp, 16  
    
    popad       
    mov esp, ebp
    pop ebp
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