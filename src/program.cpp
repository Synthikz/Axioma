#include "program.h"
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
    
    void* load_address = AllocateMemory(expected_size);
    if (!load_address) {
        return false;
    }
    
    memcpy(load_address, program_data, size);
    current_program = (ProgramHeader*)load_address;
    loaded_program_address = load_address;
    return true;
}

bool ProgramLoader::ExecuteProgram() {
    if (!current_program) {
        return false;
    }
    
    // Calculate the actual entry point address
    uint32_t entry_address = reinterpret_cast<uint32_t>(loaded_program_address) + sizeof(ProgramHeader);
    
    // Use a more robust way to call the program
    typedef void (*EntryPointFunc)();
    EntryPointFunc entry = (EntryPointFunc)entry_address;
    
    // Set up a proper execution environment
    // Save our stack pointer and other registers
    uint32_t esp_backup;
    asm volatile ("mov %%esp, %0" : "=r"(esp_backup));
    
    // Call the program
    entry();
    
    // Restore our stack pointer
    asm volatile ("mov %0, %%esp" : : "r"(esp_backup));
    
    // Unload the program
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