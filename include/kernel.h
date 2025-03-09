#ifndef _KERNEL_H_
#define _KERNEL_H_

#include <main.h>

enum KernelError {
    ERROR_NONE = 0,
    ERROR_MEMORY_ALLOCATION,
    ERROR_INVALID_PROGRAM,
    ERROR_EXECUTION_FAILED,
    ERROR_INVALID_SYSCALL,
    ERROR_STACK_OVERFLOW,
    ERROR_INVALID_PARAMETER
};

class SyscallHandler {
public:
    virtual uint32_t HandleSyscall(uint32_t syscall_number, uint32_t arg1, uint32_t arg2, uint32_t arg3) = 0;
};

class Kernel : public SyscallHandler {
private:
    volatile uint16_t* video_buffer;
    unsigned int current_pos;
    unsigned int current_line;
    KernelError last_error;
    
    void outb(uint16_t port, uint8_t value);
    
public:
    Kernel();
    virtual uint32_t HandleSyscall(uint32_t syscall_number, uint32_t arg1, uint32_t arg2, uint32_t arg3) override;
    uint16_t VideoEntry(char character, uint8_t foreground, uint8_t background);
    void ClearVideoBuffer(uint8_t foreground, uint8_t background);
    void UpdateCursor();
    void ScrollUp();
    void PrintString(const char* str, uint8_t foreground, uint8_t background);
    void PrintCharacter(char character, uint8_t foreground, uint8_t background);
    char ReadChar();
    
    void SetError(KernelError error);
    KernelError GetLastError();
    const char* GetErrorMessage(KernelError error);
    void PrintError();
};

#endif