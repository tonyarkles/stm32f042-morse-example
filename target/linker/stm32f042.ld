
ENTRY(Reset_Handler)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 32K
  RAM (xrw) : ORIGIN = 0x20000000, LENGTH = 6K
}

_estack = 0x20001800;
__stack = _estack;

SECTIONS
{
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector))
    . = ALIGN(4);
  } >FLASH

  .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH
   PROVIDE_HIDDEN (__exidx_start = .);
  .ARM.exidx   : { *(.ARM.exidx* .gnu.linkonce.armexidx.*) } >FLASH
   PROVIDE_HIDDEN (__exidx_end = .);

  .text :
  {
    . = ALIGN(4);
    KEEP (*(SORT_NONE(.init)))

    /* copied from default linker script from gcc */
    *(.text.unlikely .text.*_unlikely .text.unlikely.*)
    *(.text.exit .text.exit.*)
    *(.text.startup .text.startup.*)
    *(.text.hot .text.hot.*)
    *(.text .stub .text.* .gnu.linkonce.t.*)
    /* .gnu.warning sections are handled specially by elf32.em.  */
    *(.gnu.warning)
    *(.glue_7t) *(.glue_7) *(.vfp11_veneer) *(.v4_bx)

    . = ALIGN(4);

    /* Bringing in other sections too */
    KEEP (*(SORT_NONE(.fini)))
    *(.rodata .rodata.* .gnu.linkonce.r.*)

    . = ALIGN(4);

    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);

    . = ALIGN(4);

    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array ))
    PROVIDE_HIDDEN (__init_array_end = .);

    . = ALIGN(4);

    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array ))
    PROVIDE_HIDDEN (__fini_array_end = .);

    . = ALIGN(4);

    

    __etext = .;
    _sidata = __etext;
  } >FLASH

  .data : AT (_sidata)
  {
     . = ALIGN(4);
     __data_start__ = . ;
     *(.data .data.* .gnu.linkonce.d.*)
     *(.data1)
     . = ALIGN(4);
     __data_end__ = .; 
  } >RAM

  .bss            :
  {
    __bss_start = .;
    __bss_start__ = .;
    *(.dynbss)
    *(.bss .bss.* .gnu.linkonce.b.*)
    . = ALIGN(4);
    __bss_end = . ; __bss_end__ = . ;
  } >RAM
}
