#include <serial/com.h>
#include <cstddef>

namespace Controllers {

SerialController::SerialController(uint16_t port) : port(port) {
    Initialize();
}

void SerialController::Initialize() {
    outb(port + 1, 0x00);
    outb(port + 3, 0x80);
    outb(port + 0, 0x03);
    outb(port + 1, 0x00);
    outb(port + 3, 0x03);
    outb(port + 2, 0xC7);
    outb(port + 4, 0x0B);
}

void SerialController::WriteChar(char c) {
    while ((inb(port + 5) & 0x20) == 0);
    outb(port, c);
}

void SerialController::WriteString(const char* str) {
    if (!str) return;
    for (size_t i = 0; str[i] != '\0'; i++) {
        WriteChar(str[i]);
    }
}

uint8_t SerialController::ReadChar() {
    while ((inb(port + 5) & 0x01) == 0);
    return inb(port);
}

void SerialController::WriteHEX(uint8_t value) {
    const char* hex_digits = "0123456789ABCDEF";
    WriteChar(hex_digits[(value >> 4) & 0x0F]);
    WriteChar(hex_digits[value & 0x0F]);
}

void SerialController::outb(uint16_t port, uint8_t value) {
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

uint8_t SerialController::inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

}
