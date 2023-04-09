#include "globlevar.h"

static GlobleVar globleVar={1,2,3,4};

void __attribute__((naked)) my_entry();

void __attribute__((naked)) jump_to_target(void)
{
    asm volatile (
    "ldr pc, %0\n\t" // 将origin_entry的地址加载到r0寄存器中
    : 
    : "m" (globleVar.origin_entry) // 输入操作数是origin_entry的内存地址
    : "r0" // 破坏了r0寄存器
    );
}

void __attribute__((naked)) my_entry(){
    vu8* sram = (vu8*)0x0E000000;
    vu8* flash_save = (vu8*)globleVar.flash_save_addr;
    for(int i=0;i<64 * 1024;i++){
        sram[i] = flash_save[i];
    }
    // while(1);
    jump_to_target();

}
