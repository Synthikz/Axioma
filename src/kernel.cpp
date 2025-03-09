#include <kernel.h>
#include <addresses.h>

SyscallHandler* g_syscall_handler = nullptr;

extern "C" uint32_t syscall_interrupt_handler(uint32_t syscall_number, uint32_t arg1, uint32_t arg2, uint32_t arg3) {
    if (g_syscall_handler) {
        return g_syscall_handler->HandleSyscall(syscall_number, arg1, arg2, arg3);
    }
    return 0;
}

Kernel::Kernel() {
    video_buffer = Addresses::video_memory_color;
    ClearVideoBuffer(WHITE, BLUE);
    current_line = 0;
    current_pos = 0;
    g_syscall_handler = this;
}

uint16_t Kernel::VideoEntry(char character, uint8_t foreground, uint8_t background) {
    uint16_t fg = foreground & 0x0F;
    uint16_t bg = (background & 0x0F) << 4;
    return static_cast<uint16_t>(character) | ((fg | bg) << 8);
}

void Kernel::ClearVideoBuffer(uint8_t foreground, uint8_t background) {
    for (uint16_t i = 0; i < 25; ++i) {
        for (uint16_t j = 0; j < 80; ++j) {
            video_buffer[i * 80 + j] = VideoEntry(' ', foreground, background);
        }
    }
    current_line = 0;
    current_pos = 0;
}

void Kernel::UpdateCursor() {
    uint16_t position = current_line * 80 + current_pos;
    outb(0x3D4, 0x0F);
    outb(0x3D5, static_cast<uint8_t>(position & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, static_cast<uint8_t>((position >> 8) & 0xFF));
}

void Kernel::ScrollUp() {
    for (uint16_t i = 1; i < 25; ++i) {
        for (uint16_t j = 0; j < 80; ++j) {
            video_buffer[(i - 1) * 80 + j] = video_buffer[i * 80 + j];
        }
    }
    for (uint16_t j = 0; j < 80; ++j) {
        video_buffer[(25 - 1) * 80 + j] = VideoEntry(' ', WHITE, BLUE);
    }
    current_line = 24;
    current_pos = 0;
}

void Kernel::PrintString(const char* str, uint8_t foreground, uint8_t background) {
    if (!str) return;
    for (uint16_t i = 0; str[i] != '\0'; i++) {
        PrintCharacter(str[i], foreground, background);
    }
    UpdateCursor();
}

void Kernel::PrintCharacter(char character, uint8_t foreground, uint8_t background) {
    if (character == '\n') {
        current_pos = 0;
        current_line++;
    } else if (character == '\b') {
        if (current_pos > 0) {
            current_pos--;
            video_buffer[current_line * 80 + current_pos] = VideoEntry(' ', foreground, background);
        }
    } else {
        video_buffer[current_line * 80 + current_pos] = VideoEntry(character, foreground, background);
        current_pos++;
    }
    if (current_pos >= 80) {
        current_pos = 0;
        current_line++;
    }
    if (current_line >= 25) {
        ScrollUp();
    }
    UpdateCursor();
}

void Kernel::outb(uint16_t port, uint8_t value) {
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

char Kernel::ReadChar() {
    Keyboard kbd;
    return kbd.GetInput();
}

uint32_t Kernel::HandleSyscall(uint32_t syscall_number, uint32_t arg1, uint32_t arg2, uint32_t arg3) {
    (void)arg3;

    switch (syscall_number) {
        case 0: {
            uint8_t foreground = arg2 & 0xFF;
            uint8_t background = (arg2 >> 8) & 0xFF;
            PrintString(reinterpret_cast<const char*>(arg1), foreground, background);
            return 0;
        }
        case 1: {
            char key = ReadChar();
            return static_cast<uint32_t>(key);
        }
        case 2: {
            PrintString("\n[Program completed]\n", LIGHT_RED, BLUE);
            return 0;
        }
        case 3: {
            uint8_t foreground = arg1 & 0xFF;
            uint8_t background = (arg1 >> 8) & 0xFF;
            ClearVideoBuffer(foreground, background);
            return 0;
        }
        default:
            return static_cast<uint32_t>(-1);
    }
}
