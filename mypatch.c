#include "globlevar.h"

void __attribute__((naked)) jump_to_target(u32 target_addr)
{
    asm volatile (
    "ldr pc, %0\n\t" // 将origin_entry的地址加载到r0寄存器中
    : 
    : "m" (target_addr) // 输入操作数是origin_entry的内存地址
    : 
    );
}
void jump_back_to_game(u32 target_addr_Add1)
{
    jump_to_target(target_addr_Add1 - 1);
}