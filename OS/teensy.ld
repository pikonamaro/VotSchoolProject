ENTRY(boot_main)

SECTIONS
{
  . = ALIGN(4);

  .boot :
  {
    KEEP(*(.boot))
    *(.text*)
    *(.funcs*)
    *(.rodata*)
    *(.data*)
    *(.bss*)
    *(COMMON)
  }
  
  
  
}
