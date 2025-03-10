#ifndef _GDT_H_
#define _GDT_H_

#include <stdint.h>

struct GDTEntry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed));

struct GDTPointer {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

class GDT {
private:
    static GDTEntry entries[5];
    static GDTPointer pointer;

    static void SetGate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

public:
    static void Initialize();
};

#endif