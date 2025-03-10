#include <video/vga.h>
#include <axioma/addresses.h> 

namespace Controllers {

    VGAController::VGAController() 
        : video_buffer(Addresses::video_memory_color), current_pos(0), current_line(0) {}

    void VGAController::PrintString(const char* str, uint8_t foreground, uint8_t background) {
        if (!str) return;
        for (uint16_t i = 0; str[i] != '\0'; i++) {
            PrintChar(str[i], foreground, background);
        }
        UpdateCursor();
    }

    void VGAController::ClearVideoBuffer(uint8_t foreground, uint8_t background) {
        for (uint16_t i = 0; i < 25; ++i) {
            for (uint16_t j = 0; j < 80; ++j) {
                video_buffer[i * 80 + j] = VideoEntry(' ', foreground, background);
            }
        }
        current_pos = 0;
        current_line = 0;
        UpdateCursor();
    }

    void VGAController::UpdateCursor() {
        uint16_t position = current_line * 80 + current_pos;
        outb(0x3D4, 0x0F);
        outb(0x3D5, static_cast<uint8_t>(position & 0xFF));
        outb(0x3D4, 0x0E);
        outb(0x3D5, static_cast<uint8_t>((position >> 8) & 0xFF));
    }

    void VGAController::ScrollUp() {
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

    void VGAController::PrintChar(char character, uint8_t foreground, uint8_t background) {
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

    void VGAController::outb(uint16_t port, uint8_t value) {
        asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
    }

    uint16_t VGAController::VideoEntry(char character, uint8_t foreground, uint8_t background) {
        uint8_t attribute = ((background & 0x0F) << 4) | (foreground & 0x0F);
        return static_cast<uint16_t>(character) | (static_cast<uint16_t>(attribute) << 8);
    }
}