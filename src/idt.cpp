#include <axioma/idt.h>
#include <video/vga.h>
#include <serial/com.h>

IDTEntry IDT::entries[256];
IDTPointer IDT::pointer;

extern "C" void isr128();
extern "C" void load_idt();


extern "C" void default_interrupt_handler(uint32_t int_no) {
    Controllers::VGAController vga;
    Controllers::SerialController serial;
    vga.PrintString("\n\n[ERROR] Unhandled interrupt: ", WHITE, RED);
    serial.WriteHEX(int_no);
}

void IDT::Initialize() {
    pointer.limit = sizeof(IDTEntry) * 256 - 1;
    pointer.base = (uint32_t)&entries;

    memset(&entries, 0, sizeof(IDTEntry) * 256);

    for (uint16_t i = 0; i < 256; i++) {
        SetGate(i, (uint32_t)default_interrupt_handler, 0x08, 0x8E);
    }

    SetGate(0x80, (uint32_t)isr128, 0x08, 0x8E);

    asm volatile("lidt %0" : : "m" (pointer));
}

void IDT::SetGate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags) {
    entries[num].offset_low = base & 0xFFFF;
    entries[num].offset_high = (base >> 16) & 0xFFFF;
    entries[num].selector = selector;
    entries[num].zero = 0;
    entries[num].type_attr = flags | 0x60;
}
