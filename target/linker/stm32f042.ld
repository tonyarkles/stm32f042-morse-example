
ENTRY(Reset_Handler)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 32K
  RAM (xrw) : ORIGIN = 0x20000000, LENGTH = 6K
}

_estack = 0x20001800;

SECTIONS
{
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector))
    . = ALIGN(4);
  } >FLASH

  .text :
  {
    . = ALIGN(4);
    *(.text*)
    *(.rodata*)
    . = ALIGN(4);
    _sidata = .;
    
  } >FLASH

  .data :
  {
    /* Start of the data section, used by the startup script */
    . = ALIGN(4);
    _sdata = .;
    _data = .;

    *(.data*)

    . = ALIGN(4);
    /* End of the data section */
    _edata = .;
  } >RAM

  .bss :
  {
    . = ALIGN(4);
    __bss_start__ = .; /* for newlib */
    _sbss = .; /* for startup script */
    _bss = .;
    *(.bss*)
    . = ALIGN(4);
    __bss_end__ = .; /* for newlib */
    _ebss = .; /* for startup script */
  }

  PROVIDE ( end = _ebss );
  PROVIDE ( __end__ = _ebss );
}
