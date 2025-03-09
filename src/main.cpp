#include "main.h"

static const int MAX_CMD_LENGTH = 64;
char command_buffer[MAX_CMD_LENGTH];
int cmd_position = 0;

extern "C" uint8_t sample_program_data[];
extern "C" uint32_t sample_program_size;

extern "C" void isr128();

void process_command(Kernel& kernel, ProgramLoader& loader) {
    command_buffer[cmd_position] = '\0';
    
    if (strcmp(command_buffer, "run") == 0) {
        kernel.PrintString("\nRunning sample program...\n", LIGHT_GREEN, BLUE);
        if (loader.LoadProgram(sample_program_data, sample_program_size)) {
            if (!loader.ExecuteProgram()) {
                kernel.PrintString("Program execution failed!\n", LIGHT_RED, BLUE);
            }
        } else {
            kernel.PrintString("Failed to load program!\n", LIGHT_RED, BLUE);
        }
    }
    else if (strcmp(command_buffer, "clear") == 0) {
        kernel.ClearVideoBuffer(WHITE, BLUE);
        kernel.PrintString("=== Simple C++ Kernel ===\n", YELLOW, BLUE);
        kernel.PrintString("Type 'help' for available commands\n", CYAN, BLUE);
    }
    else if (strcmp(command_buffer, "help") == 0) {
        kernel.PrintString("\nAvailable commands:\n", CYAN, BLUE);
        kernel.PrintString("  run    - Execute the sample program\n", WHITE, BLUE);
        kernel.PrintString("  clear  - Clear the screen\n", WHITE, BLUE);
        kernel.PrintString("  help   - Display this help message\n", WHITE, BLUE);
    }
    else if (cmd_position > 0) {
        kernel.PrintString("\nUnknown command: ", LIGHT_RED, BLUE);
        kernel.PrintString(command_buffer, LIGHT_RED, BLUE);
        kernel.PrintString("\nType 'help' for available commands\n", LIGHT_RED, BLUE);
    }
    
    cmd_position = 0;
    kernel.PrintString("\n> ", LIGHT_GREEN, BLUE);
}

extern "C" void axio_main() {
    IDT::Initialize();
    
    IDT::SetGate(0x80, (uint32_t)isr128, 0x08, 0x8E);

    asm volatile("sti");
    
    Kernel kernel;
    Keyboard keyboard;
    ProgramLoader program_loader;
    
    kernel.PrintString("=== Simple C++ Kernel with Syscalls ===\n", YELLOW, BLUE);
    kernel.PrintString("Type 'help' for available commands\n", CYAN, BLUE);
    kernel.PrintString("\n> ", LIGHT_GREEN, BLUE);
    
    while(true) {
        char key = keyboard.GetInput();
        if(key) {
            if (key == '\n') {
                process_command(kernel, program_loader);
            }
            else if (key == '\b') {
                if (cmd_position > 0) {
                    cmd_position--;
                    kernel.PrintCharacter('\b', WHITE, BLUE);
                }
            }
            else if (cmd_position < MAX_CMD_LENGTH - 1) {
                command_buffer[cmd_position++] = key;
                kernel.PrintCharacter(key, WHITE, BLUE);
            }
        }
        for(volatile int i = 0; i < 10000; i++) {}
    }
}