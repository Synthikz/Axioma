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
    KernelError last_error;
    
public:
    Kernel();
    virtual uint32_t HandleSyscall(uint32_t syscall_number, uint32_t arg1, uint32_t arg2, uint32_t arg3) override;
    
    void SetError(KernelError error);
    KernelError GetLastError();
    const char* GetErrorMessage(KernelError error);
    void PrintError();
    void EnableInterrupts();
};

#endif