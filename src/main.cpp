#include <main.h>

static const int MAX_CMD_LENGTH = 64;
char command_buffer[MAX_CMD_LENGTH];
int cmd_position = 0;

extern "C" uint8_t sample_program_data[];
extern "C" uint32_t sample_program_size;

extern "C" void isr128();

void process_command(Controllers::VGAController& vga, ProgramLoader& loader) {    
    command_buffer[cmd_position] = '\0';

    if (strcmp(command_buffer, "run") == 0) {
        vga.PrintString("\nRunning sample program...\n", LIGHT_GREEN, BLUE);
        if (loader.LoadProgram(sample_program_data, sample_program_size)) {
            if (!loader.ExecuteProgram()) {
                vga.PrintString("Program execution failed!\n", LIGHT_RED, BLUE);
            }
        } else {
            vga.PrintString("Failed to load program!\n", LIGHT_RED, BLUE);
        }
    }
    else if (strcmp(command_buffer, "test") == 0) {
        vga.PrintString("\nTesting interrupts...\n", LIGHT_GREEN, BLUE);
        uint32_t result;
        asm volatile(
            "mov $0, %%eax \n"
            "mov $0, %%ebx \n"
            "mov $0, %%ecx \n"
            "mov $0, %%edx \n"
            "int $0x80 \n"
            : "=a" (result)
            :
            : "memory"
        );
        
        vga.PrintString("Syscall test completed\n", LIGHT_GREEN, BLUE);
    }
    else if (strcmp(command_buffer, "clear") == 0) {
        vga.ClearVideoBuffer(WHITE, BLUE);
        vga.PrintString("=== Axioma Simple C++ Kernel ===\n", YELLOW, BLUE);
        vga.PrintString("Type 'help' for available commands\n", CYAN, BLUE);
    }
    else if (strcmp(command_buffer, "help") == 0) {
        vga.PrintString("\nAvailable commands:\n", CYAN, BLUE);
        vga.PrintString("  run    - Execute the sample program\n", WHITE, BLUE);
        vga.PrintString("  clear  - Clear the screen\n", WHITE, BLUE);
        vga.PrintString("  help   - Display this help message\n", WHITE, BLUE);
        vga.PrintString("  test   - Test syscall interrupts\n", WHITE, BLUE);
    }
    else if (cmd_position > 0) {
        vga.PrintString("\nUnknown command: ", LIGHT_RED, BLUE);
        vga.PrintString(command_buffer, LIGHT_RED, BLUE);
        vga.PrintString("\nType 'help' for available commands\n", LIGHT_RED, BLUE);
    }
    
    cmd_position = 0;
    vga.PrintString("\n> ", LIGHT_GREEN, BLUE);
}

extern "C" void axio_main() {
    Controllers::KeyboardController keyboard;
    Controllers::VGAController vga;
    ProgramLoader program_loader;
    GDT::Initialize();
    IDT::Initialize();
    Controllers::PIC::Remap();

    vga.ClearVideoBuffer(WHITE, BLUE);
    vga.PrintString("=== Axioma Simple C++ Kernel ===\n", YELLOW, BLUE);
    vga.PrintString("Type 'help' for available commands\n", CYAN, BLUE);
    vga.PrintString("\n> ", LIGHT_GREEN, BLUE);
    
    asm volatile("sti");
    
    while (true) {
        char key = keyboard.GetInput();
        if (key) {
            if (key == '\n') {
                process_command(vga, program_loader);
            }
            else if (key == '\b') {
                if (cmd_position > 0) {
                    cmd_position--;
                    vga.PrintChar('\b', WHITE, BLUE);
                }
            }
            else if (cmd_position < MAX_CMD_LENGTH - 1) {
                command_buffer[cmd_position++] = key;
                vga.PrintChar(key, WHITE, BLUE);
            }
        }
        
        // Small delay to reduce CPU usage
        for (volatile int i = 0; i < 5000; i++) {}
    }
}
