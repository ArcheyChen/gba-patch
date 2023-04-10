#include "globlevar.h"

volatile static const GlobleVar globleVar __attribute__((section("MY_GLOBLE_VAR")))  ={1,2,3,4};

extern void __attribute__((naked)) jump_to_target(u32 target_addr);

void jump_back_to_game(u32 target_addr_Add1);

void __attribute__((nake,section("MY_ENTRY")))my_entry(){
    vu8* sram = (vu8*)0x0E000000;
    vu8* flash_save = (vu8*)globleVar.flash_save_addr;
    for(int i=0;i<64 * 1024;i++){
        sram[i] = flash_save[i];
    }

    jump_back_to_game(globleVar.origin_entry+1);
    // while(1);
    // jump_to_target(func_ptr);
    // jump_to_target(globleVar.origin_entry);

}

