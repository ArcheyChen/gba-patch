#include "globlevar.h"

extern void jump_to_target(u32 target_addr);
void jump_back_to_game(u32 target_addr_Add1);
void my_entry();

volatile static const GlobleVar globleVar __attribute__((section("MY_GLOBLE_VAR")))  =
{0x1234,(vu32)my_entry};

void __attribute__((nake))my_entry(){
    vu8* sram = (vu8*)0x0E000000;
    vu8* flash_save = (vu8*)globleVar.flash_save_addr;
    for(int i=0;i<64 * 1024;i++){
        sram[i] = flash_save[i];
    }

    jump_back_to_game(globleVar.origin_entry+1);

}

