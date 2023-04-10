#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "payload.h"
#include "globlevar.h"

FILE *romfile;
FILE *outfile;
uint32_t romsize;
uint8_t rom[0x02000000];
char signature[] = "<3 from Maniac";

enum payload_offsets {
    ORIGINAL_ENTRYPOINT_ADDR,
    FLUSH_MODE,
    SAVE_SIZE,
    PATCHED_ENTRYPOINT,
    WRITE_SRAM_PATCHED,
    WRITE_EEPROM_PATCHED,
    WRITE_FLASH_PATCHED,
};

// ldr r3, [pc, # 0]; bx r3
static unsigned char thumb_branch_thunk[] = { 0x00, 0x4b, 0x18, 0x47 };
static unsigned char arm_branch_thunk[] = { 0x00, 0x30, 0x9f, 0xe5, 0x13, 0xff, 0x2f, 0xe1 };

static unsigned char write_sram_signature[] = { 0x30, 0xB5, 0x05, 0x1C, 0x0C, 0x1C, 0x13, 0x1C, 0x0B, 0x4A, 0x10, 0x88, 0x0B, 0x49, 0x08, 0x40};
static unsigned char write_sram_ram_signature[] = { 0x04, 0xC0, 0x90, 0xE4, 0x01, 0xC0, 0xC1, 0xE4, 0x2C, 0xC4, 0xA0, 0xE1, 0x01, 0xC0, 0xC1, 0xE4 };
static unsigned char write_eeprom_signature[] = { 0x70, 0xB5, 0x00, 0x04, 0x0A, 0x1C, 0x40, 0x0B, 0xE0, 0x21, 0x09, 0x05, 0x41, 0x18, 0x07, 0x31, 0x00, 0x23, 0x10, 0x78};
static unsigned char write_flash_signature[] = { 0x70, 0xB5, 0x00, 0x03, 0x0A, 0x1C, 0xE0, 0x21, 0x09, 0x05, 0x41, 0x18, 0x01, 0x23, 0x1B, 0x03};
static unsigned char write_flash2_signature[] = { 0x7C, 0xB5, 0x90, 0xB0, 0x00, 0x03, 0x0A, 0x1C, 0xE0, 0x21, 0x09, 0x05, 0x09, 0x18, 0x01, 0x23};
// sig present without SRAM patch
static unsigned char write_flash3_signature[] = { 0xF0, 0xB5, 0x90, 0xB0, 0x0F, 0x1C, 0x00, 0x04, 0x04, 0x0C, 0x03, 0x48, 0x00, 0x68, 0x40, 0x89 };

static uint8_t *memfind(uint8_t *haystack, size_t haystack_size, uint8_t *needle, size_t needle_size, int stride)
{
    for (size_t i = 0; i < haystack_size - needle_size; i += stride)
    {
        if (!memcmp(haystack + i, needle, needle_size))
        {
            return haystack + i;
        }
    }
    return NULL;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        puts("Wrong number of args");
		scanf("%*s");
        return 1;
    }
	
	memset(rom, 0x00ff, sizeof rom);

    printf("here");
    // Open ROM file
    if (!(romfile = fopen(argv[1], "rb")))
    {
        puts("Could not open input file");
        puts(strerror(errno));
		scanf("%*s");
        return 1;
    }

    // Load ROM into memory
    fseek(romfile, 0, SEEK_END);
    romsize = ftell(romfile);

    if (romsize > sizeof rom)
    {
        puts("ROM too large - not a GBA ROM?");
		scanf("%*s");
        return 1;
    }

    if (romsize & 0x3ffff)
    {
		puts("ROM has been trimmed and is misaligned. Padding to 256KB alignment");
		romsize &= ~0x3ffff;
		romsize += 0x40000;
    }

    fseek(romfile, 0, SEEK_SET);
    fread(rom, 1, romsize, romfile);
    printf("here");

    // Check if ROM already patched.
    if (memfind(rom, romsize, signature, sizeof signature, 4))
    {
        puts("ROM already patched!");
		scanf("%*s");
        return 1;
    }

	int payload_base;
    unsigned int payload_bin_len_padded = payload_bin_len;
    if(payload_bin_len_padded & 0x3){
        payload_bin_len_padded &= ~0x3;
        payload_bin_len_padded += 0x4;
    }
	// reserve the last 128KB of ROM because some flash chips seem to have different sector topology there
    for (payload_base = romsize - 0x40000 - payload_bin_len_padded; payload_base >= 0; payload_base -= 0x20000)
    {
        int is_all_zeroes = 1;
        int is_all_ones = 1;
        for (int i = 0; i < 0x20000 + payload_bin_len_padded; ++i)
        {
            if (rom[payload_base+i] != 0)
            {
                is_all_zeroes = 0;
            }
            if (rom[payload_base+i] != 0xFF)
            {
                is_all_ones = 0;
            }
        }
        if (is_all_zeroes || is_all_ones)
        {
           break;
		}
    }
	if (payload_base < 0)
	{
		puts("ROM too small to install payload.");
		if (romsize + 0x60000 > 0x2000000)
		{
			puts("ROM alraedy max size. Cannot expand. Cannot install payload");
            scanf("%*s");
			return 1;
		}
		else
		{
			puts("Expanding ROM");
			romsize += 0x60000;
			payload_base = romsize - 0x40000 - payload_bin_len_padded;
		}
	}
	
	printf("Installing payload at offset %x, save file stored at %x\n", payload_base, payload_base + payload_bin_len_padded);
	memcpy(rom + payload_base, payload_bin, payload_bin_len);
    
    
    

	// Patch the ROM entrypoint to init sram and the dummy IRQ handler, and tell the new entrypoint where the old one was.
	if (rom[3] != 0xea)
	{
		puts("Unexpected entrypoint instruction");
		scanf("%*s");
		return 1;
	}
	unsigned long original_entrypoint_offset = rom[0];
	original_entrypoint_offset |= rom[1] << 8;
	original_entrypoint_offset |= rom[2] << 16;
	unsigned long original_entrypoint_address = 0x08000000 + 8 + (original_entrypoint_offset << 2);
	printf("Original offset was %lx, original entrypoint was %lx\n", original_entrypoint_offset, original_entrypoint_address);
	// little endian assumed, deal with it

    unsigned long new_entry_offset = 0x10;
    unsigned long origin_entry_offset = 0xc;
	unsigned long new_entrypoint_address = 0x08000000 + payload_base + new_entry_offset;
    GlobleVar *globleVar = (GlobleVar*)(rom+payload_base);
    printf("entry new_entry_offset %x payload base  %x\n",new_entry_offset, payload_base);

    // globleVar->origin_entry = original_entrypoint_address;
    *(uint32_t*)(rom + payload_base + origin_entry_offset) = original_entrypoint_address;
	0[(uint32_t*) rom] = 0xea000000 | (new_entrypoint_address - 0x08000008) >> 2;

    printf("b instr is:%x\n",0[(uint32_t*) rom]);
    printf("my_entry in file offset %x\n",payload_base + new_entry_offset);
    printf("my_entry in GBA offset %x\n",new_entrypoint_address);
    // printf("origin entry offset in file:%x\n",payload_base + origin_entry_offset);


	// Flush all changes to new file
    char *prefix = "auto_";
    size_t original_filename_length = strlen(argv[1]);
    size_t prefix_length = strlen(prefix);
    char new_filename[FILENAME_MAX];
    strncpy(new_filename, prefix, FILENAME_MAX);
    strncat(new_filename, argv[1], FILENAME_MAX);
    
    if (!(outfile = fopen(new_filename, "wb")))
    {
        puts("Could not open output file");
        puts(strerror(errno));
		scanf("%*s");
        return 1;
    }
    
    fwrite(rom, 1, romsize, outfile);
    fflush(outfile);

    printf("Patched successfully. Changes written to %s\n", new_filename);
    // scanf("%*s");
	return 0;
	
}
