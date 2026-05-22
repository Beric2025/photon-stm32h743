/*
 * Linker script for STM32H743IITx
 * Flash: 2MB, SRAM1: 128KB (0x20000000), SRAM2: 512KB (0x24000000)
 */

MEMORY
{
    FLASH   (rx) : ORIGIN = 0x08000000, LENGTH = 2048K
    SRAM1   (rw) : ORIGIN = 0x20000000, LENGTH = 128K
    SRAM2   (rw) : ORIGIN = 0x24000000, LENGTH = 512K
    ETH_RAM (rw) : ORIGIN = 0x30040000, LENGTH = 16K
}

_estack = ORIGIN(SRAM1) + LENGTH(SRAM1);

SECTIONS
{
    /* Reset and interrupt vectors */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > FLASH

    /* Program code */
    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        *(.glue_7)
        *(.glue_7t)
        *(.eh_frame)
        KEEP(*(.init))
        KEEP(*(.fini))
        . = ALIGN(4);
        _etext = .;
    } > FLASH

    /* Constant data */
    .rodata :
    {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } > FLASH

    /* ARM exception unwinding */
    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH

    /* Pre-initialization array */
    .preinit_array :
    {
        PROVIDE_HIDDEN(__preinit_array_start = .);
        KEEP(*(.preinit_array*))
        PROVIDE_HIDDEN(__preinit_array_end = .);
    } > FLASH

    /* Initialization array */
    .init_array :
    {
        PROVIDE_HIDDEN(__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array*))
        PROVIDE_HIDDEN(__init_array_end = .);
    } > FLASH

    /* Finalization array */
    .fini_array :
    {
        PROVIDE_HIDDEN(__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array*))
        PROVIDE_HIDDEN(__fini_array_end = .);
    } > FLASH

    /* Initialized data - stored in FLASH, copied to SRAM at startup */
    _sidata = LOADADDR(.data);

    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > SRAM1 AT > FLASH

    /* Uninitialized data (BSS) */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
        PROVIDE(end = .);
        PROVIDE(_end = .);
    } > SRAM1

    /* SRAM2 data */
    .sram2_data (NOLOAD) :
    {
        . = ALIGN(4);
        *(.sram2_data)
        *(.sram2_data*)
        . = ALIGN(4);
    } > SRAM2

    /* Ethernet Rx buffer - placed at dedicated MPU region */
    .eth_rx_buf (NOLOAD) :
    {
        . = ALIGN(4);
        *(.eth_rx_buf)
        . = ALIGN(4);
    } > ETH_RAM

    /* Remove debug information from the standard library */
    /DISCARD/ :
    {
        libc.a(*)
        libm.a(*)
        libgcc.a(*)
    }
}
