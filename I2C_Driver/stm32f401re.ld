/*
 * Linker script for STM32F401RE
 * 
 * Memory layout:
 * FLASH: 512KB at 0x08000000
 * SRAM:  96KB at 0x20000000
 */

ENTRY(Reset_Handler)

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 512K
    SRAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 96K
}

SECTIONS
{
    .text :
    {
        KEEP(*(.isr_vector))
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
    } > FLASH

    .data :
    {
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > SRAM AT> FLASH

    _sidata = LOADADDR(.data);

    .bss :
    {
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > SRAM

    . = ALIGN(4);
    _end = .;
}
