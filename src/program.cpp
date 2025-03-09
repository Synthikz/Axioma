#include <program.h>
#include <stdint.h>
#include <string.h>

ProgramLoader::ProgramLoader()
    : current_program(nullptr), loaded_program_address(nullptr)
{
    for (uint32_t i = 0; i < MAX_ALLOCATIONS; i++) {
        allocations[i].address = 0;
        allocations[i].size = 0;
        allocations[i].used = false;
    }
}

bool ProgramLoader::ValidateHeader(ProgramHeader* header) {
    return (header->magic == PROGRAM_MAGIC);
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
    
    // Allocate some extra memory for stack (e.g., 16KB)
    void* load_address = AllocateMemory(expected_size + 16384);
    if (!load_address) {
        return false;
    }
    
    // Copy the program into memory
    memcpy(load_address, program_data, size);
    current_program = (ProgramHeader*)load_address;
    loaded_program_address = load_address;
    return true;
}

bool ProgramLoader::ExecuteProgram() {
    if (!current_program) {
        return false;
    }

    uint32_t entry_address = reinterpret_cast<uint32_t>(loaded_program_address) + 
                             sizeof(ProgramHeader) + current_program->entry_point;
    
    typedef void (*EntryPointFunc)();
    EntryPointFunc entry = (EntryPointFunc)entry_address;

    entry();
    
    UnloadProgram();
    
    return true;
}

void ProgramLoader::UnloadProgram() {
    if (loaded_program_address) {
        FreeMemory(loaded_program_address);
    }
    current_program = nullptr;
    loaded_program_address = nullptr;
}

void ProgramLoader::ReturnToKernel() {
    UnloadProgram();
}