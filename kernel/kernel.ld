OUTPUT_ARCH( "riscv" )
ENTRY( _entry )

SECTIONS
{
  /*
   * ensure that entry.S / _entry is at 0x80000000,
   * where qemu's -kernel jumps.
   */
  . = 0x80000000;

  .text : {
    *(.text .text.*)
    . = ALIGN(0x1000);
    _trampoline = .;
    *(trampsec)
    . = ALIGN(0x1000);
    ASSERT(. - _trampoline == 0x1000, "error: trampoline larger than one page");
    PROVIDE(etext = .);
  }

  .rodata : {
    . = ALIGN(16);
    *(.srodata .srodata.*) /* do not need to distinguish this from .rodata */
    . = ALIGN(16);
    *(.rodata .rodata.*)
  }

  .data : {
    . = ALIGN(16);
    *(.sdata .sdata.*) /* do not need to distinguish this from .data */
    . = ALIGN(16);
    *(.data .data.*)
  }

  .bss : {
    . = ALIGN(16);
    *(.sbss .sbss.*) /* do not need to distinguish this from .bss */
    . = ALIGN(16);
    *(.bss .bss.*)
  }

  .vdso : {            /* Start an "output section" named .vdso */
    . = ALIGN(0x1000); /* Align the current address (.) on a page boundary */
    _vdso_start = .;   /* Create a symbol _vdso_start and assign it the current address */
    *(.vdso);          /* Put all "input sections" named .vdso in the current "output section" */
    . = ALIGN(0x1000); /* Align the current address on a page boundary */
    ASSERT(. - _vdso_start == 0x1000, "error: vdso section exceeds one page");
  }

  PROVIDE(end = .);
}
