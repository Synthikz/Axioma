#include <idt.h>

IDTEntry IDT::entries[256];
IDTPointer IDT::pointer;

extern "C" void isr128();

void IDT::Initialize() {
    pointer.limit = sizeof(IDTEntry) * 256 - 1;
    pointer.base = (uint32_t)&entries;

    memset(&entries, 0, sizeof(IDTEntry) * 256);

    SetGate(0x80, (uint32_t)isr128, 0x08, 0x8E);

    asm volatile("lidt %0" : : "m" (pointer));
}

void IDT::SetGate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags) {
    entries[num].offset_low = base & 0xFFFF;
    entries[num].offset_high = (base >> 16) & 0xFFFF;
    entries[num].selector = selector;
    entries[num].zero = 0;
    entries[num].type_attr = flags;
}