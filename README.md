# Axioma Simple C++ Kernel

Axioma is a minimalist kernel written in C++ for learning OS development concepts.

## Features

- Text-mode console with colored output
- Keyboard input handling
- Basic memory management
- Multiboot compliant

## Building

### Prerequisites

- g++ (with 32-bit support)
- nasm (Netwide Assembler)
- grub-mkrescue
- xorr


### Directory Tree

```
📦Axioma
 ┣ 📂asm
 ┃ ┗ 📜boot.asm
 ┣ 📂config
 ┃ ┣ 📜grub.cfg
 ┃ ┗ 📜iso.sh
 ┣ 📂include
 ┃ ┣ 📜addresses.h
 ┃ ┣ 📜color_table.h
 ┃ ┣ 📜kernel.h
 ┃ ┣ 📜keyboard.h
 ┃ ┣ 📜libc.h
 ┃ ┣ 📜main.h
 ┃ ┗ 📜program.h
 ┣ 📂linker
 ┃ ┗ 📜linker.ld
 ┣ 📂src
 ┃ ┣ 📜addresses.cpp
 ┃ ┣ 📜kernel.cpp
 ┃ ┣ 📜keyboard.cpp
 ┃ ┣ 📜libc.cpp
 ┃ ┣ 📜main.cpp
 ┃ ┗ 📜program.cpp
 ┣ 📂tools
 ┃ ┣ 📜program_compiler.cpp
 ┃ ┗ 📜sample_program.cpp
 ┣ 📜.gitignore
 ┣ 📜README.md
 ┗ 📜makefile

```