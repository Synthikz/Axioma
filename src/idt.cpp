#include "idt.h"

IDTEntry IDT::entries[256];
IDTPointer IDT::pointer;

void IDT::Initialize() {
    pointer.limit = sizeof(IDTEntry) * 256 - 1;
    pointer.base = (uint32_t)&entries;

    for (uint16_t i = 0; i < 256; i++) {
        SetGate(i, 0, 0, 0);
    }

    asm volatile("lidt %0" : : "m" (pointer));
}

void IDT::SetGate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags) {
    entries[num].offset_low = base & 0xFFFF;
    entries[num].offset_high = (base >> 16) & 0xFFFF;
    entries[num].selector = selector;
    entries[num].zero = 0;
    entries[num].type_attr = flags;
}