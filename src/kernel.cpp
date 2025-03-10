#include <axioma/kernel.h>
#include <axioma/addresses.h>
#include <video/vga.h>

SyscallHandler* g_syscall_handler = nullptr;

extern "C" uint32_t syscall_interrupt_handler(uint32_t syscall_number, uint32_t arg1, uint32_t arg2, uint32_t arg3) {
    Controllers::SerialController serial;
    serial.WriteString("syscall_interrupt_handler called\n");
    
    if (g_syscall_handler) {
        return g_syscall_handler->HandleSyscall(syscall_number, arg1, arg2, arg3);
    }
    serial.WriteString("No syscall handler installed!\n");
    return 0;
}

static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

Kernel::Kernel() : vga_controller() {
    g_syscall_handler = this;
}

uint32_t Kernel::HandleSyscall(uint32_t syscall_number, uint32_t arg1, uint32_t arg2, uint32_t arg3) {
    Controllers::SerialController serial;
    serial.WriteString("Syscall received: ");
    serial.WriteHEX(syscall_number);
    serial.WriteString("\nArg1: ");
    serial.WriteHEX(arg1);
    serial.WriteString("\nArg2: ");
    serial.WriteHEX(arg2);
    serial.WriteString("\n");

    switch (syscall_number) {
        case 0: { // SYS_PRINT
            const char* message = reinterpret_cast<const char*>(arg1);
            if (!message) {
                serial.WriteString("Error: null message pointer\n");
                return static_cast<uint32_t>(-1);
            }
            uint8_t foreground = arg2 & 0xFF;
            uint8_t background = (arg2 >> 8) & 0xFF;
            vga_controller.PrintString(message, foreground, background);
            return 0;
        }
        case 1: { // SYS_READ_CHAR
            Controllers::KeyboardController keyboard;
            char key = keyboard.GetInput();
            return static_cast<uint32_t>(key);
        }
        case 2: { // SYS_EXIT
            vga_controller.PrintString("\n[Program completed]\n", LIGHT_RED, BLUE);
            return 0xFFFFFFFF;
        }
        case 3: { // SYS_CLEAR_SCREEN
            uint8_t foreground = arg1 & 0xFF;
            uint8_t background = (arg1 >> 8) & 0xFF;
            vga_controller.ClearVideoBuffer(foreground, background);
            return 0;
        }
        default:
            vga_controller.PrintString("\n[ERROR] Invalid syscall\n", LIGHT_RED, BLUE);
            return static_cast<uint32_t>(-1);
    }
}
