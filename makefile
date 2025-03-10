# Compiler and assembler
CC = g++
CXX = g++
ASM = nasm

# Compiler flags
CC_FLAGS = -g -m32 -c -nostdlib -std=c++11 -ffreestanding -O0 -Wall -Wextra -fno-exceptions -fno-rtti -fno-stack-protector -Icontrollers
ASM_FLAGS = -felf32 -g

# Source files
SRC_CPP_FILES := $(wildcard src/*.cpp)
CTRL_CPP_FILES := $(wildcard controllers/*/*.cpp)
ASM_FILES := asm/boot.asm asm/syscall.asm asm/gdt_flush.asm asm/idt_flush.asm

# Object files (in build/out)
OBJ_FILES := $(patsubst src/%.cpp,build/out/%.o,$(SRC_CPP_FILES)) \
             $(patsubst controllers/%.cpp,build/out/controllers/%.o,$(CTRL_CPP_FILES)) \
             $(patsubst asm/%.asm,build/out/%.o,$(ASM_FILES)) \
             build/out/sample_program.o

# Output binary
KERNEL_BIN := build/Kernel.bin

# Program compiler utility
PROGRAM_COMPILER := build/program_compiler

# Tools directory
TOOLS_DIR := tools

# All target
all: sample-program $(KERNEL_BIN)

# Clean target
clean:
	rm -f $(KERNEL_BIN)
	rm -rf build iso
	rm -f $(PROGRAM_COMPILER)

# Create build directories
build:
	mkdir -p build

build/out:
	mkdir -p build/out
	mkdir -p build/out/controllers
	mkdir -p build/out/controllers/keyboard
	mkdir -p build/out/controllers/video

# Rule for C++ files in src
build/out/%.o: src/%.cpp | build/out
	$(CC) $(CC_FLAGS) -Iinclude -o $@ $<

# Rule for C++ files in controllers
build/out/controllers/%.o: controllers/%.cpp | build/out
	mkdir -p $(dir $@)
	$(CC) $(CC_FLAGS) -Iinclude -o $@ $<

# Rule for ASM files in asm/
build/out/%.o: asm/%.asm | build/out
	$(ASM) $(ASM_FLAGS) -o $@ $<

build/out/sample_program.o: asm/sample.asm | build/out
	$(ASM) $(ASM_FLAGS) -o $@ $<

$(KERNEL_BIN): $(OBJ_FILES) | build
	ld -m elf_i386 -nostdlib -T linker/linker.ld -g -o $@ $^
	chmod +x config/iso.sh
	./config/iso.sh

$(PROGRAM_COMPILER): $(TOOLS_DIR)/program_compiler.cpp | build
	$(CXX) -o $@ $<

build/sample.bin: $(TOOLS_DIR)/sample_program.cpp | build
	$(CC) -m32 -c -nostdlib -ffreestanding -O2 -fno-pie -fno-stack-protector -o build/sample.o $<
	ld -m elf_i386 -nostdlib -T linker/sample_program.ld --oformat binary -o build/sample.bin build/sample.o

build/sample.pask: build/sample.bin $(PROGRAM_COMPILER) | build
	$(PROGRAM_COMPILER) build/sample.bin $@

sample-program: build/sample.pask

# Run ISO in QEMU
run-qemu: $(KERNEL_BIN)
	@echo "Starting QEMU with ISO..."
	qemu-system-i386 -cdrom build/Kernel.iso -gdb tcp::1234

# Run kernel binary directly in QEMU
run-qemu-kernel: $(KERNEL_BIN)
	@echo "Starting QEMU with kernel binary..."
	qemu-system-i386 -kernel $(KERNEL_BIN) -gdb tcp::1234

# Run kernel binary as a disk image in QEMU (sin salida gráfica)
qemu-nox: $(KERNEL_BIN)
	@echo "Starting QEMU with kernel binary as disk image..."
	qemu-system-i386 -nographic -kernel $(KERNEL_BIN) -serial mon:stdio -S -gdb tcp::28275 -D qemu.log

# Target to build program compiler and sample program
tools: $(PROGRAM_COMPILER) sample-program

.PHONY: all clean run-qemu run-qemu-kernel tools sample-program qemu-nox
