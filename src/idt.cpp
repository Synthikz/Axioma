#include <idt.h>

IDTEntry IDT::entries[256];
IDTPointer IDT::pointer;

extern "C" void isr128();
extern "C" void load_idt();

void IDT::Initialize() {
    pointer.limit = sizeof(IDTEntry) * 256 - 1;
    pointer.base = (uint32_t)&entries;

    // Limpiar la IDT
    memset(&entries, 0, sizeof(IDTEntry) * 256);

    // Configurar la puerta de interrupción del syscall
    // 0x08 es el selector (segmento de código del kernel)
    // 0x8E = 0b10001110: Present, Ring 0, Interrupt Gate
    SetGate(0x80, (uint32_t)isr128, 0x08, 0x8E);

    // Asegúrate de cargar la IDT correctamente
    asm volatile("lidt %0" : : "m" (pointer));
}

void IDT::SetGate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags) {
    entries[num].offset_low = base & 0xFFFF;
    entries[num].offset_high = (base >> 16) & 0xFFFF;
    entries[num].selector = selector;
    entries[num].zero = 0;
    entries[num].type_attr = flags;
}
