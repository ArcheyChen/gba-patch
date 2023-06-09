#ifndef __PATCH_GLOBLE_VARS_HEADER_H
#define __PATCH_GLOBLE_VARS_HEADER_H
typedef unsigned short u16;
typedef volatile unsigned int vu32;
typedef unsigned int u32;
typedef volatile unsigned short vu16;
typedef volatile unsigned char vu8;
typedef volatile signed short vs16;

typedef struct GLOBLE_VAR_STRUCT
{
    vu32 origin_entry;
    vu32 patch_entry;
    vu32 flash_save_addr;
}GlobleVar;
#endif