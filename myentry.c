typedef unsigned short u16;
typedef volatile unsigned int vu32;
typedef unsigned int u32;
typedef volatile unsigned short vu16;
typedef volatile unsigned char vu8;
typedef volatile signed short vs16;
void my_entry();
volatile char* flash_save_addr = (volatile char*)0x123;
vu32 idle_irq_handler = 0xff;
vu32 keypad_irq_handler = 0xff;
// vu32 flash_save_sector = 0xff;

static vu32 origin_entry = 0xfbff90;


volatile char flush_mode=0;
void __attribute__((naked)) jump_to_target(void)
{
    // asm("LDR R0, =origin_entry"); // 把target的地址加载到R0中
    // asm("LDR R1, [R0]"); // 跳转到R0 指向的地址
    // asm("LDR PC, [R0]"); // 跳转到R0 指向的地址
    asm volatile (
    "ldr pc, %0\n\t" // 将origin_entry的地址加载到r0寄存器中
    : // 没有输出操作数
    : "m" (origin_entry) // 输入操作数是origin_entry的内存地址
    : "r0" // 破坏了r0寄存器
    );
}

void my_entry(){
    // u32 handler = idle_irq_handler;
    // if(flush_mode){
    //     handler = keypad_irq_handler;
    // }
    // *(vu32*)(0x04000000 - 4) = handler;

    vu8* sram = (vu8*)0x0E000000;
    for(int i=0;i<64 * 1024;i++){
        sram[i] = flash_save_addr[i];
    }
    // while(1);
    jump_to_target();

}
