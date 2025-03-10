#include <axioma/pic.h>
#include <stdint.h>

static inline void outb(uint16_t port, uint8_t value) {
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

void Controllers::PIC::Remap() {
    const uint8_t ICW1 = 0x11;
    const uint8_t ICW4 = 0x01;

    const uint16_t PIC1_COMMAND = 0x20;
    const uint16_t PIC1_DATA    = 0x21;
    const uint16_t PIC2_COMMAND = 0xA0;
    const uint16_t PIC2_DATA    = 0xA1;

    outb(PIC1_COMMAND, ICW1);
    outb(PIC2_COMMAND, ICW1);
    outb(PIC1_DATA, 0x20);
    outb(PIC2_DATA, 0x28);
    outb(PIC1_DATA, 0x04); 
    outb(PIC2_DATA, 0x02); 
    outb(PIC1_DATA, ICW4);
    outb(PIC2_DATA, ICW4);
    outb(PIC1_DATA, 0xFD);
    outb(PIC2_DATA, 0xFF);
}

void Controllers::PIC::SendEOI(uint8_t irq) {
    const uint16_t PIC1_COMMAND = 0x20;
    const uint16_t PIC2_COMMAND = 0xA0;
    if (irq >= 8) {
        outb(PIC2_COMMAND, 0x20);
    }
    outb(PIC1_COMMAND, 0x20);
}