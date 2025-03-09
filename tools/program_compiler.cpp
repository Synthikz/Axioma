#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>

struct ProgramHeader {
    uint32_t magic; 
    uint32_t entry_point; 
    uint32_t code_size;
    uint32_t data_size;
};

static const uint32_t PROGRAM_MAGIC = 0x4B534150; // "PASK"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input_bin> <output_program>" << std::endl;
        return 1;
    }
    
    std::ifstream input(argv[1], std::ios::binary);
    if (!input) {
        std::cerr << "Failed to open input file: " << argv[1] << std::endl;
        return 1;
    }
    
    std::vector<uint8_t> code_data((std::istreambuf_iterator<char>(input)),
                               std::istreambuf_iterator<char>());
    input.close();
    
    ProgramHeader header;
    header.magic = PROGRAM_MAGIC;
    header.entry_point = 0;
    header.code_size = code_data.size();
    header.data_size = 0;
    
    std::ofstream output(argv[2], std::ios::binary);
    if (!output) {
        std::cerr << "Failed to create output file: " << argv[2] << std::endl;
        return 1;
    }
    
    output.write(reinterpret_cast<const char*>(&header), sizeof(header));
    output.write(reinterpret_cast<const char*>(code_data.data()), code_data.size());
    
    output.close();
    
    std::cout << "Program created successfully: " << argv[2] << std::endl;
    std::cout << "Code size: " << header.code_size << " bytes" << std::endl;
    std::cout << "Entry point offset: " << header.entry_point << std::endl;
    
    return 0;
}