
SECTIONS
{
    . = 0x0;
    .data :
    {
        *(MY_GLOBLE_VAR)
        *(MY_ENTRY)
    }
    .text :
    {
        *(*)
    }
} 
