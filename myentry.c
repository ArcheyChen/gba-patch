typedef unsigned short u16;
typedef volatile unsigned int vu32;
typedef unsigned int u32;
typedef volatile unsigned short vu16;
typedef volatile unsigned char vu8;
typedef volatile signed short vs16;
void my_entry();
static vu32 flash_save_addr = 0xff;
static vu32 idle_irq_handler = 0xff;
static vu32 keypad_irq_handler = 0xff;
// vu32 flash_save_sector = 0xff;

static vu32 origin_entry = 0xfbff90;

void __attribute__((naked)) jump_to_target(void)
{
    asm volatile (
    "ldr pc, %0\n\t" // 将origin_entry的地址加载到r0寄存器中
    : 
    : "m" (origin_entry) // 输入操作数是origin_entry的内存地址
    : "r0" // 破坏了r0寄存器
    );
}

void __attribute__((naked)) my_entry(){
    vu8* sram = (vu8*)0x0E000000;
    vu8* flash_save = (vu8*)flash_save_addr;
    for(int i=0;i<64 * 1024;i++){
        sram[i] = flash_save[i];
    }
    // while(1);
    jump_to_target();

}
