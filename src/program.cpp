#include <axioma/program.h>
#include <stdint.h>
#include <string.h>
#include <serial/com.h> 

static void ReportError(const char* msg, uint32_t code) {
    Controllers::SerialController serial;
    serial.WriteString(msg);
    serial.WriteHEX(code);
    serial.WriteString("\n");
}

ProgramLoader::ProgramLoader()
    : current_program(nullptr), loaded_program_address(nullptr), program_stack(0)
{
    for (uint32_t i = 0; i < MAX_ALLOCATIONS; i++) {
        allocations[i].address = 0;
        allocations[i].size = 0;
        allocations[i].used = false;
    }
}

bool ProgramLoader::ValidateHeader(ProgramHeader* header) {
    if (header->magic != PROGRAM_MAGIC) {
        ReportError("Error: Invalid program header (magic mismatch). Magic: ", header->magic);
        return false;
    }
    
    if (header->entry_point >= header->code_size) {
        ReportError("Error: Entry point exceeds code size. Entry: ", header->entry_point);
        return false;
    }
    
    if (header->code_size + header->data_size > MAX_PROGRAM_SIZE - PROGRAM_STACK_SIZE) {
        ReportError("Error: Program code and data exceed allowed size. Size: ", header->code_size + header->data_size);
        return false;
    }
    
    return true;
}

void* ProgramLoader::AllocateMemory(uint32_t size) {
    if (size > MAX_PROGRAM_SIZE) {
        return nullptr;
    }
    
    for (uint32_t i = 0; i < MAX_ALLOCATIONS; i++) {
        if (!allocations[i].used) {
            allocations[i].address = PROGRAM_LOAD_ADDRESS + i * MAX_PROGRAM_SIZE;
            allocations[i].size = size;
            allocations[i].used = true;
            return reinterpret_cast<void*>(allocations[i].address);
        }
    }
    return nullptr;
}

void ProgramLoader::FreeMemory(void* address) {
    for (uint32_t i = 0; i < MAX_ALLOCATIONS; i++) {
        if (allocations[i].address == reinterpret_cast<uint32_t>(address)) {
            allocations[i].used = false;
            allocations[i].address = 0;
            allocations[i].size = 0;
            break;
        }
    }
}

bool ProgramLoader::SetupProgramEnvironment() {
    if (loaded_program_address) {
        uint32_t base_address = reinterpret_cast<uint32_t>(loaded_program_address);
        uint32_t total_program_size = sizeof(ProgramHeader) + 
                                     current_program->code_size + 
                                     current_program->data_size;

        uint32_t stack_base = (base_address + total_program_size + 15) & ~15;
        
        if (stack_base + PROGRAM_STACK_SIZE > base_address + MAX_PROGRAM_SIZE) {
            ReportError("Error: Not enough memory for program stack. Base address: ", base_address);
            return false;
        }
        
        memset(reinterpret_cast<void*>(stack_base), 0, PROGRAM_STACK_SIZE);
        program_stack = stack_base + PROGRAM_STACK_SIZE;
        return true;
    }
    return false;
}

bool ProgramLoader::LoadProgram(const uint8_t* program_data, uint32_t size) {
    if (!program_data || size < sizeof(ProgramHeader)) {
        ReportError("Invalid program data or size", 1);
        return false;
    }

    UnloadProgram();

    void* load_address = AllocateMemory(size + PROGRAM_STACK_SIZE);
    if (!load_address) {
        ReportError("Memory allocation failed", 2);
        return false;
    }
    memcpy(load_address, program_data, size);
    current_program = (ProgramHeader*)load_address;
    loaded_program_address = load_address;

    if (!SetupProgramEnvironment()) {
        UnloadProgram();
        return false;
    }

    return true;
}

bool ProgramLoader::ExecuteProgram() {
    if (loaded_program_address == nullptr || current_program == nullptr) {
        ReportError("Error: Program not loaded properly. Code: ", 2);
        return false;
    }

    typedef void (*EntryPoint)();
    EntryPoint entry = (EntryPoint)((uint8_t*)loaded_program_address + sizeof(ProgramHeader) + current_program->entry_point);

    uint32_t kernel_stack;
    asm volatile("mov %%esp, %0" : "=r"(kernel_stack));

    asm volatile(
        "mov %0, %%esp \n"
        "call *%1     \n"
        "mov %2, %%esp \n"
        : 
        : "r"(program_stack), "r"(entry), "r"(kernel_stack)
        : "memory"
    );

    ReturnToKernel();
    return true;
}

void ProgramLoader::UnloadProgram() {
    if (loaded_program_address) {
        FreeMemory(loaded_program_address);
    }
    current_program = nullptr;
    loaded_program_address = nullptr;
    program_stack = 0;
}

void ProgramLoader::ReturnToKernel() {
    UnloadProgram();
}