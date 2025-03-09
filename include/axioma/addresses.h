#ifndef _ADDRESSES_H_
#define _ADDRESSES_H_

#include <cstdint>

class Addresses
{
private:
    Addresses() {}

public:
    static volatile uint16_t* const video_memory_color;
    static volatile uint16_t* const video_memory_monochrome;
    static uint16_t keyboard_port;
};

#endif