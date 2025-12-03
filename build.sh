#!/bin/bash

set -e

EXTRA_DIR=src/kernel/extra
SRC_DIR=src/kernel
BUILD_DIR=build

mkdir -p $BUILD_DIR
mkdir -p $BUILD_DIR/iso/boot/grub
nasm -f elf32 src/kernel/gdt_flush.asm -o build/gdt_flush.o
# Assemble and include boot.s (contains multiboot header)
gcc -m32 -ffreestanding -c boot.s -o build/boot.o

echo "Target: $SRC_DIR, $EXTRA_DIR..."

OBJ_FILES=""

echo "Compiling sources from $SRC_DIR..."
for f in $SRC_DIR/*.c; do
    fname=$(basename "$f")
    oname="${fname%.c}.o"
    echo " -> $fname"
    gcc -m32 -ffreestanding -nostdlib -fno-pic -fno-stack-protector -I src/kernel -c "$f" -o "$BUILD_DIR/$oname"
    OBJ_FILES="$OBJ_FILES $BUILD_DIR/$oname"
done

# compile any extra sources if directory exists and add their objects
if [ -d "$EXTRA_DIR" ]; then
    echo "Compiling extra sources from $EXTRA_DIR..."
    for f in $EXTRA_DIR/*.c; do
        [ -e "$f" ] || continue
        fname=$(basename "$f")
        oname="${fname%.c}.o"
        echo " -> extra: $fname"
        gcc -m32 -ffreestanding -nostdlib -fno-pic -fno-stack-protector -I src/kernel -c "$f" -o "$BUILD_DIR/$oname"
        OBJ_FILES="$OBJ_FILES $BUILD_DIR/$oname"
    done
fi

# Include any non-C objects (boot.o) in object list when linking
OBJ_FILES="$OBJ_FILES build/boot.o"

echo "Linking kernel..."
ld -m elf_i386 -T linker.ld -o $BUILD_DIR/kernel.elf $OBJ_FILES build/gdt_flush.o

echo "Checking multiboot header..."
hexdump -C $BUILD_DIR/kernel.elf | head -5

echo "Checking sections..."
readelf -S $BUILD_DIR/kernel.elf | head -15

# Создание grub.cfg
cat > $BUILD_DIR/iso/boot/grub/grub.cfg << EOF
set timeout=0
set default=0

menuentry "SurteoKernel" {
    multiboot /boot/kernel.elf
    boot
}
EOF

cp $BUILD_DIR/kernel.elf $BUILD_DIR/iso/boot/

echo "Creating ISO..."
grub-mkrescue -o SurteoOS.iso $BUILD_DIR/iso/ 2>/dev/null

if [ $? -eq 0 ]; then
    echo "Booting QEMU..."
    qemu-system-i386 -cdrom SurteoOS.iso -m 128M -serial stdio
else
    echo "Failed to create ISO"
    exit 1
fi
