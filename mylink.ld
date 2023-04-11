
SECTIONS
{
    . = 0x0;
    .data :
    {
        *(MY_GLOBLE_VAR)
    }
    .text :
    {
        *(*)
    }
} 
