#ifndef SERIAL_CONTROLLER_H
#define SERIAL_CONTROLLER_H

#include <stdint.h>

namespace Controllers {

class SerialController {
private:
    uint16_t port;

    void outb(uint16_t port, uint8_t value);
    uint8_t inb(uint16_t port);

public:
    SerialController(uint16_t port = 0x3F8);

    void Initialize();
    void WriteChar(char c);
    void WriteString(const char* str);
    uint8_t ReadChar();
    bool IsTransmitEmpty();
    void WriteHEX(uint8_t value);
};

}

#endif 
