#include "globlevar.h"

volatile static const GlobleVar globleVar __attribute__((section("data")))  ={1,2,3,4};

void __attribute__((naked)) my_entry();

extern void __attribute__((naked)) jump_to_target(u32 target_addr);

void __attribute__((naked)) my_entry(){
    vu8* sram = (vu8*)0x0E000000;
    vu8* flash_save = (vu8*)globleVar.flash_save_addr;
    for(int i=0;i<64 * 1024;i++){
        sram[i] = flash_save[i];
    }
    // while(1);
    jump_to_target(globleVar.origin_entry);

}
