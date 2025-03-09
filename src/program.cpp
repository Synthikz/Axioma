#include <axioma/program.h>
#include <stdint.h>
#include <string.h>

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
        return false;
    }
    
    if (header->entry_point >= header->code_size) {
        return false;
    }
    
    if (header->code_size + header->data_size > MAX_PROGRAM_SIZE - PROGRAM_STACK_SIZE) {
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

        program_stack = (base_address + total_program_size + 15) & ~15;
        
        if (program_stack + PROGRAM_STACK_SIZE > 
            base_address + MAX_PROGRAM_SIZE) {
            return false;
        }
        
        memset(reinterpret_cast<void*>(program_stack), 0, PROGRAM_STACK_SIZE);
        return true;
    }
    return false;
}

bool ProgramLoader::LoadProgram(const uint8_t* program_data, uint32_t size) {
    if (size < sizeof(ProgramHeader)) {
        return false;
    }
    
    ProgramHeader* header = (ProgramHeader*)program_data;
    if (!ValidateHeader(header)) {
        return false;
    }
    
    uint32_t expected_size = sizeof(ProgramHeader) + header->code_size + header->data_size;
    if (size < expected_size) {
        return false;
    }
    
    void* load_address = AllocateMemory(expected_size + PROGRAM_STACK_SIZE);
    if (!load_address) {
        return false;
    }
    
    memcpy(load_address, program_data, size);
    current_program = (ProgramHeader*)load_address;
    loaded_program_address = load_address;
    
    if (!SetupProgramEnvironment()) {
        FreeMemory(load_address);
        current_program = nullptr;
        loaded_program_address = nullptr;
        return false;
    }
    
    return true;
}

bool ProgramLoader::ExecuteProgram() {
    if (!current_program || !loaded_program_address) {
        return false;
    }

    uint32_t entry_address = reinterpret_cast<uint32_t>(loaded_program_address) + 
                             sizeof(ProgramHeader) + current_program->entry_point;
    
    uint32_t stack_top = program_stack + PROGRAM_STACK_SIZE;
    
    asm volatile(
        "movl %%esp, %%ebx\n" 
        "movl %1, %%esp\n" 
        "call *%0\n"
        "movl %%ebx, %%esp\n"
        :
        : "r"(entry_address), "r"(stack_top)
        : "ebx", "memory"
    );
    
    UnloadProgram();
    
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