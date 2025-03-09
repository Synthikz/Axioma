#ifndef _PROGRAM_H_
#define _PROGRAM_H_

#include "main.h"

struct ProgramHeader {
    uint32_t magic;
    uint32_t entry_point;
    uint32_t code_size;   
    uint32_t data_size; 
};

static const uint32_t PROGRAM_MAGIC = 0x4B534150; // "PASK"

struct ProgramAllocation {
    uint32_t address;
    uint32_t size;
    bool used;
};

class ProgramLoader {
private:
    static const uint32_t PROGRAM_LOAD_ADDRESS = 0x100000;
    static const uint32_t MAX_PROGRAM_SIZE = 0x100000;       // 1MB
    
    ProgramHeader* current_program;
    void* loaded_program_address;
    
    static const uint32_t MAX_ALLOCATIONS = 16;
    ProgramAllocation allocations[MAX_ALLOCATIONS];
    
    bool ValidateHeader(ProgramHeader* header);
    void* AllocateMemory(uint32_t size);
    void FreeMemory(void* address);
    
public:
    ProgramLoader();
    bool LoadProgram(const uint8_t* program_data, uint32_t size);
    bool ExecuteProgram();
    void UnloadProgram();
    void ReturnToKernel();
};

#endif
