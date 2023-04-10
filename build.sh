rm -f *.o *.elf *.bin *.s
rm -rf build
mkdir build
cd build

arm-none-eabi-gcc -mcpu=arm7tdmi -Os -static -fPIC -nostartfiles -c ../myentry.c ../mypatch.c
# arm-none-eabi-gcc -mcpu=arm7tdmi myentry.c -Os -static -fPIC -nostartfiles -S -o myentry.s
arm-none-eabi-ld -T ../mylink.ld myentry.o mypatch.o -o myentry.elf
arm-none-eabi-objcopy -O binary myentry.elf myentry.bin

xxd -i -n payload_bin myentry.bin > ../payload.h

# xxd -i payload.bin > payload.h
# xxd -i -n mysave_bin output_zelda.sav > mysave.h
gcc -g ../patcher.c   -o ../patch

# arm-none-eabi-readelf payload.elf -s | grep write_flash_patched | awk '{print $2}'