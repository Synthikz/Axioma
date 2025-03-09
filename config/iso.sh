#!/bin/bash
set -e

grub-file --is-x86-multiboot build/Kernel.bin || { 
    echo "Not a valid multiboot image!"
    exit 1
}

mkdir -p iso/boot/grub

cp build/Kernel.bin iso/boot/
cp config/grub.cfg iso/boot/grub/

grub-mkrescue -o build/Kernel.iso iso

echo "ISO created at build/Kernel.iso"