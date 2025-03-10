#include <axioma/gdt.h>

GDTEntry GDT::entries[5];
GDTPointer GDT::pointer;

extern "C" void gdt_flush(uint32_t);

void GDT::Initialize() {
    pointer.limit = (sizeof(GDTEntry) * 5) - 1;
    pointer.base = (uint32_t)&entries;

    SetGate(0, 0, 0, 0, 0);
    SetGate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); 
    SetGate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    SetGate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    SetGate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    gdt_flush((uint32_t)&pointer);
}

void GDT::SetGate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    entries[num].base_low = (base & 0xFFFF);
    entries[num].base_middle = (base >> 16) & 0xFF;
    entries[num].base_high = (base >> 24) & 0xFF;

    entries[num].limit_low = (limit & 0xFFFF);
    entries[num].granularity = (limit >> 16) & 0x0F;

    entries[num].granularity |= gran & 0xF0;
    entries[num].access = access;
}