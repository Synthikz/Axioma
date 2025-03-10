#include <axioma/idt.h>
#include <video/vga.h>
#include <serial/com.h>

extern "C" void idt_flush(uint32_t);
extern "C" void isr128();
extern "C" void irq1_handler();

IDTEntry IDT::entries[256];
IDTPointer IDT::pointer;

extern "C" void double_fault_handler(uint32_t int_no) {
    Controllers::VGAController vga;
    Controllers::SerialController serial;
    vga.PrintString("\n\n[ERROR] Double Fault! System Halted.\n", WHITE, RED);
    serial.WriteString("Double Fault Detected!\n");
    serial.WriteHEX(int_no);

    while (1) { __asm__("hlt"); } // Detener la CPU
}

extern "C" void default_interrupt_handler(uint32_t int_no) {
    Controllers::VGAController vga;
    Controllers::SerialController serial;
    vga.PrintString("\n\n[ERROR] Unhandled interrupt: ", WHITE, RED);
    serial.WriteHEX(int_no);
    while(1) {
        __asm__("hlt");
    }
}

void IDT::Initialize() {
    pointer.limit = sizeof(IDTEntry) * 256 - 1;
    pointer.base = (uint32_t)&entries;

    memset(&entries, 0, sizeof(IDTEntry) * 256);
    SetGate(33, (uint32_t)irq1_handler, 0x08, 0x8E);
    SetGate(0x80, (uint32_t)isr128, 0x08, 0x8E);
    idt_flush((uint32_t)&pointer);
}

void IDT::SetGate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags) {
    entries[num].offset_low = base & 0xFFFF;
    entries[num].offset_high = (base >> 16) & 0xFFFF;
    entries[num].selector = selector;
    entries[num].zero = 0;
    entries[num].type_attr = flags;
}
