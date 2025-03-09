ENTRY(_start)
SECTIONS
{
  . = 0x100000;
  
  .text : {
    *(.text*)
  }
  
  .data : {
    *(.data*)
  }
  
  /DISCARD/ : {
    *(.bss*)
    *(.note*)
    *(.comment*)
  }
}
