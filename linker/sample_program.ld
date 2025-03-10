ENTRY(_start)
SECTIONS
{
    . = 0x100000;
    
    .text ALIGN(4K) : {
        *(.text*)
    }
    
    .rodata ALIGN(4K) : {
        *(.rodata*)
    }
    
    .data ALIGN(4K) : {
        *(.data*)
    }
    
    /DISCARD/ : {
        *(.eh_frame*)
        *(.comment*)
        *(.note*)
        *(.bss*)
    }
}
