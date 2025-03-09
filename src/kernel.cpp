#include <axioma/kernel.h>
#include <axioma/addresses.h>
#include <video/vga.h>

SyscallHandler* g_syscall_handler = nullptr;

extern "C" uint32_t syscall_interrupt_handler(uint32_t syscall_number, uint32_t arg1, uint32_t arg2, uint32_t arg3) {
    if (g_syscall_handler) {
        return g_syscall_handler->HandleSyscall(syscall_number, arg1, arg2, arg3);
    }
    return 0;
}

static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

Kernel::Kernel() : vga_controller() { // Initialize vga_controller in the constructor
    g_syscall_handler = this;
}

uint32_t Kernel::HandleSyscall(uint32_t syscall_number, uint32_t arg1, uint32_t arg2, uint32_t arg3) {
    (void)arg3;

    switch (syscall_number) {
        case 0: {
            uint8_t foreground = arg2 & 0xFF;
            uint8_t background = (arg2 >> 8) & 0xFF;
            vga_controller.PrintString(reinterpret_cast<const char*>(arg1), foreground, background);
            return 0;
        }
        case 1: {
            uint8_t scan_code = inb(Addresses::keyboard_port);
            return static_cast<uint32_t>(scan_code);
        }
        case 2: {
            vga_controller.PrintString("\n[Program completed]\n", LIGHT_RED, BLUE);
            return 0;
        }
        case 3: {
            uint8_t foreground = arg1 & 0xFF;
            uint8_t background = (arg1 >> 8) & 0xFF;
            vga_controller.ClearVideoBuffer(foreground, background);
            return 0;
        }
        default:
            return static_cast<uint32_t>(-1);
    }
}
