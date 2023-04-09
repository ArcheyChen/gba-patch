rm -f *.o *.elf *.bin *.s

arm-none-eabi-gcc -mcpu=arm7tdmi myentry.c -Os -static -fPIC -nostartfiles -c -o myentry.o
arm-none-eabi-gcc -mcpu=arm7tdmi myentry.c -Os -static -fPIC -nostartfiles -S -o myentry.s
arm-none-eabi-ld -T mylink.ld myentry.o -o myentry.elf
arm-none-eabi-objcopy -O binary myentry.elf myentry.bin

xxd -i -n payload_bin myentry.bin > payload.h

# xxd -i payload.bin > payload.h
# xxd -i -n mysave_bin output_zelda.sav > mysave.h
gcc -g patcher.c   -o patch

# arm-none-eabi-readelf payload.elf -s | grep write_flash_patched | awk '{print $2}'