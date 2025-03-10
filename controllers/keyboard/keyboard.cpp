#include <keyboard/keyboard.h>
#include <serial/com.h>
#include <video/vga.h>

namespace Controllers {

    KeyboardController::KeyboardController() : last_key(0), key_released(true) {}

    char KeyboardController::GetKeyCharacter(const unsigned char scan_code) {
        if (scan_code & 0x80) {
            key_released = true;
            return 0;
        }

        if (scan_code == last_key && !key_released) {
            return 0;
        }

        last_key = scan_code;
        key_released = false;

        switch(scan_code) {
            case KEYS::A: return 'a';
            case KEYS::B: return 'b';
            case KEYS::C: return 'c';
            case KEYS::D: return 'd';
            case KEYS::E: return 'e';
            case KEYS::F: return 'f';
            case KEYS::G: return 'g';
            case KEYS::H: return 'h';
            case KEYS::I: return 'i';
            case KEYS::J: return 'j';
            case KEYS::K: return 'k';
            case KEYS::L: return 'l';
            case KEYS::M: return 'm';
            case KEYS::N: return 'n';
            case KEYS::O: return 'o';
            case KEYS::P: return 'p';
            case KEYS::Q: return 'q';
            case KEYS::R: return 'r';
            case KEYS::S: return 's';
            case KEYS::T: return 't';
            case KEYS::U: return 'u';
            case KEYS::V: return 'v';
            case KEYS::W: return 'w';
            case KEYS::X: return 'x';
            case KEYS::Y: return 'y';
            case KEYS::Z: return 'z';
            case KEYS::_1: return '1';
            case KEYS::_2: return '2';
            case KEYS::_3: return '3';
            case KEYS::_4: return '4';
            case KEYS::_5: return '5';
            case KEYS::_6: return '6';
            case KEYS::_7: return '7';
            case KEYS::_8: return '8';
            case KEYS::_9: return '9';
            case KEYS::_0: return '0';
            case KEYS::SPACE: return ' ';
            case KEYS::OPEN_BRACKET: return '[';
            case KEYS::CLOSE_BRACKET: return ']';
            case KEYS::COMMA: return ',';
            case KEYS::DOT: return '.';
            case KEYS::SEMICOLON: return ';';
            case KEYS::FORWARD_SLASH: return '/';
            case KEYS::SLASH: return '\\';
            case KEYS::ENTER: return '\n';
            case KEYS::BACKSPACE: return '\b';
            default: return 0;
        }
    }

    char KeyboardController::GetInput() {
        unsigned char scan_code = 0;

        uint8_t status;
        do {
            asm volatile("inb $0x64, %0" : "=a" (status));
        } while (!(status & 0x01));

        asm volatile("inb $0x60, %0" : "=a" (scan_code));

        return GetKeyCharacter(scan_code);
    }

    extern "C" void keyboard_interrupt_handler() {
        Controllers::KeyboardController keyboard;
        char key = keyboard.GetInput();
        if (key) {
            Controllers::VGAController vga;
            vga.PrintChar(key, WHITE, BLUE);
        }
        Controllers::PIC::SendEOI(1);
    }

}