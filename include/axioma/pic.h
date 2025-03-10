#ifndef _PIC_H_
#define _PIC_H_

#include <stdint.h>

namespace Controllers {
    class PIC {
    public:
        static void Remap();
        static void SendEOI(uint8_t irq);
    };
}

#endif