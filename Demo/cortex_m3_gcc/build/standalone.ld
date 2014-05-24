/* Linker script for Cortex-M3 processor on the Ferrari board
 *
 * Modified by: Zimin Wang
 * 	        simon.zmwang@gmail.com
 *
 */

OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")

MEMORY
{
  rom (rx) : ORIGIN = 0x0, LENGTH = 64K
  ram (rwx) : ORIGIN = 0x20000000, LENGTH = 176K
}

SECTIONS
{
  .text :
  {
    KEEP(*(.isr_vectors))
    *(.text*)
    *(.rodata*)
  } >rom

  /* .ARM.exidx is sorted, so has to go in its own output section */
  .ARM.exidx :
  {
    _exidx_start = .;
    *(.ARM.exidx*)
    _exidx_end = .;
  } >rom
  _etext = .;

  .data : AT (ADDR(.text) + SIZEOF(.text) + SIZEOF(.ARM.exidx) )
  {
    _data = .;
    *(.data*)
    _edata = .;
  } >ram

  .bss :
  {
    _bss = .;
    *(.bss*)
    _ebss = .;
    . = ALIGN (8);
    _end = .;
  } >ram

}
