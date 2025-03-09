#include <addresses.h>

volatile uint16_t* const Addresses::video_memory_color = reinterpret_cast<uint16_t*>(0xB8000);
volatile uint16_t* const Addresses::video_memory_monochrome = reinterpret_cast<uint16_t*>(0xB0000);
uint16_t Addresses::keyboard_port = 0x60;
