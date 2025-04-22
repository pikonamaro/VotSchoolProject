#!/bin/bash
set -e

NAME="$1"
JUNK_DIR="junk"

mkdir -p "$JUNK_DIR"

INCLUDE_DIR="./libs"
echo "Script executed from: ${PWD}"

BASEDIR=$(dirname $0)
echo "Script location: ${BASEDIR}"
arm-none-eabi-g++ -mcpu=cortex-m7 -mthumb -O2 -nostdlib -ffreestanding \
    -fno-exceptions -fno-rtti -fno-builtin -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-use-cxa-atexit \
    -fPIC -fPIE -fpie -mpic-data-is-text-relative \
    -fdata-sections -ffunction-sections -fno-merge-constants \
    -I"$INCLUDE_DIR" \
    -c "$NAME.cpp" -o "$JUNK_DIR/$NAME.o"

arm-none-eabi-ld -T teensy.ld \
    -o "$JUNK_DIR/$NAME.elf" \
    "$JUNK_DIR/$NAME.o" 

arm-none-eabi-objcopy -O binary "$JUNK_DIR/$NAME.elf" "$NAME.bin"

echo "Build complete: $NAME.bin"
