global syscall
global isr128
extern syscall_interrupt_handler

section .text

; Manejo de la llamada al sistema desde el modo de usuario
syscall:
    ; Guardamos los registros que vamos a usar
    pushad

    ; Obtenemos el número de syscall y los argumentos
    mov eax, [esp + 4]  ; Número de la syscall
    mov ebx, [esp + 8]  ; Primer argumento
    mov ecx, [esp + 12] ; Segundo argumento
    mov edx, [esp + 16] ; Tercer argumento

    ; Llamamos al manejador de la interrupción de la syscall
    int 0x80

    ; Guardamos el valor de retorno de la syscall
    mov [esp + 28], eax  ; Guardamos el valor de retorno en la pila

    ; Restauramos los registros
    popad

    ; Retornamos al código que llamó a la syscall
    ret

; Interrupción 128 (usada por la syscall)
isr128:
    ; Guardamos los registros que vamos a usar
    pushad

    ; Pasamos los registros al manejador de la interrupción
    push edx
    push ecx
    push ebx
    push eax

    ; Llamamos al manejador de la interrupción en C (syscall_interrupt_handler)
    call syscall_interrupt_handler

    ; Restauramos los registros después de la llamada
    add esp, 16

    ; Restauramos el valor de retorno de la syscall
    mov [esp + 28], eax

    ; Restauramos todos los registros
    popad

    ; Terminamos la interrupción
    iret
