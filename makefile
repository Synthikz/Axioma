# Compiler and assembler
CC = g++
CXX = g++
ASM = nasm

# Compiler flags
CC_FLAGS = -m32 -c -nostdlib -std=c++11 -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -fno-stack-protector
ASM_FLAGS = -felf32

# Source files
CPP_FILES := $(wildcard src/*.cpp)
ASM_FILES := asm/boot.asm asm/syscall.asm

# Object files (in build/out)
OBJ_FILES = $(patsubst src/%.cpp,build/out/%.o,$(CPP_FILES)) \
             $(patsubst asm/%.asm,build/out/%.o,$(ASM_FILES))

# Output binary
KERNEL_BIN := build/Kernel.bin

# Program compiler utility
PROGRAM_COMPILER := build/program_compiler

# Tools directory
TOOLS_DIR := tools

# All target
all: $(KERNEL_BIN)

# Clean target
clean:
	rm -f $(KERNEL_BIN)
	rm -rf build/out iso
	rm -f $(PROGRAM_COMPILER)

# Create build directories
build:
	mkdir -p $@

build/out:
	mkdir -p $@

# Rule for C++ files
build/out/%.o: src/%.cpp | build/out
	$(CC) $(CC_FLAGS) -Iinclude -o $@ $<

# Rule for ASM files
build/out/%.o: asm/%.asm | build/out
	$(ASM) $(ASM_FLAGS) -o $@ $<

# Link the kernel
$(KERNEL_BIN): $(OBJ_FILES) build/out/sample_program.o | build
	ld -m elf_i386 -nostdlib -T linker/linker.ld -o $@ $^
	chmod +x config/iso.sh
	./config/iso.sh

# Build the program compiler utility
$(PROGRAM_COMPILER): $(TOOLS_DIR)/program_compiler.cpp | build
	$(CXX) -o $@ $<

# Compile the sample program
build/sample.bin: $(TOOLS_DIR)/sample_program.cpp | build
	$(CC) -m32 -c -nostdlib -ffreestanding -O2 -fno-pic -fno-pie -o build/sample.o $<
	ld -m elf_i386 -nostdlib -T linker/sample_program.ld --oformat binary -o build/sample.bin build/sample.o

# Create a program for the kernel
build/sample.pask: build/sample.bin $(PROGRAM_COMPILER) | build
	$(PROGRAM_COMPILER) build/sample.bin $@

# Target to build sample program
sample-program: build/sample.pask

# Include sample program in kernel binary
build/out/sample_program.o: asm/sample.asm | build/out
	$(ASM) $(ASM_FLAGS) -o $@ $<

# Add the sample program to object files
OBJ_FILES += build/out/sample_program.o

# Run ISO in QEMU
run-qemu: $(KERNEL_BIN)
	@echo "Starting QEMU with ISO..."
	qemu-system-i386 -cdrom build/Kernel.iso

# Run kernel binary directly in QEMU
run-qemu-kernel: $(KERNEL_BIN)
	@echo "Starting QEMU with kernel binary..."
	qemu-system-i386 -kernel $(KERNEL_BIN)

# Tools target to build program compiler and sample program
tools: $(PROGRAM_COMPILER) sample-program

.PHONY: all clean run-qemu run-qemu-kernel tools sample-program
