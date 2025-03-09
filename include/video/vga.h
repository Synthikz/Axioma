#ifndef VGA_CONTROLLER_H
#define VGA_CONTROLLER_H

#include <stdint.h>
#include <axioma/color_table.h>

namespace Controllers {

class VGAController {
private:
    volatile uint16_t* video_buffer;
    unsigned int current_pos;
    unsigned int current_line;

    void outb(uint16_t port, uint8_t value);

public:
    VGAController();
    uint16_t VideoEntry(char character, uint8_t foreground, uint8_t background);
    void ClearVideoBuffer(uint8_t foreground, uint8_t background);
    void UpdateCursor();
    void ScrollUp();
    void PrintString(const char* str, uint8_t foreground, uint8_t background);
    void PrintChar(char character, uint8_t foreground, uint8_t background);
};

}

#endif
