#ifndef _IDT_H_
#define _IDT_H_

#include <stdint.h>
#include <axioma/libc.h>

struct IDTEntry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_high;
} __attribute__((packed));

struct IDTPointer {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

class IDT {
private:
    static IDTEntry entries[256];
    static IDTPointer pointer;

public:
    static void Initialize();
    static void SetGate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags);
};

#endif