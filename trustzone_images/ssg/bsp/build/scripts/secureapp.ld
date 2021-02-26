PHDRS {
  CODE_RO PT_LOAD;
  MTDATA PT_LOAD FLAGS(0x04000004); ​/* Bits 24-26 MANIFEST SEGMENT 0x04 */
  DATA PT_LOAD;
  GOT PT_LOAD;
  DYN PT_DYNAMIC;
  REL PT_LOAD;
}

SECTIONS {
  .plt ALIGN(4096) : {
    text_section_base = .;
    * (.plt*)
  } : CODE_RO

  .text : {
    * (.text* kthread_shared)
    KEEP(*(tzapp_syscall_asm))
    * (EXCLUDE_FILE(*metadata.o) .rodata* .eh_frame*)
    text_section_limit = .;
  } : CODE_RO

  .metadata ALIGN(4096) : {
    *metadata.o (.data* .rodata*)
    * (.metadata.sentinel)
    ASSERT(SIZEOF(.metadata) != 0, "No metadata section defined.");
  } : MTDATA

  .data ALIGN(4096) : {
    data_section_base = .;
    * (EXCLUDE_FILE(*metadata.o) .data*)
    * (.bss* COMMON)
    data_section_limit = .;
  } : DATA

  .rel.text : {
    *(.rel.text)
  } : DATA
  .rel.rodata : {
    *(.rel.rodata)
  } : DATA
  .data.rel.ro.local : {
    *(.data.rel.ro.local)
  } : DATA
  .data.rel.ro : {
    *(.data.rel.ro)
  } : DATA
  .rel.data : {
    *(.rel.data)
  } : DATA
  .data.rel : {
    *(.data.rel)
  } : DATA
  .data.rel.local : {
    *(.data.rel.local)
  } : DATA

  INIT_ARRAY : {
    FILL(0x0)
    __init_array_start = .;
    *(.init_array)
    __init_array_end = .;
  } : DATA

  FINI_ARRAY : {
    FILL(0x0)
    __fini_array_start = .;
    *(.fini_array)
    __fini_array_end = .;
  } : DATA

  .got ALIGN(4096) : { *(.got.plt) *(.igot.plt) *(.got) *(.igot) } : GOT

  .dynamic ALIGN(4096) : { *(.dynamic) } : DYN : REL

  .rel.dyn : { *(.rel.dyn) } : REL
  .rel.plt : { *(.rel.plt) } : REL
  .hash    : { *(.hash) }    : REL
  .dynsym  : { *(.dynsym) }  : REL
  .dynstr  : { *(.dynstr) }  : REL

  .gnu.attributes 0 : { KEEP (*(.gnu.attributes)) }
  .note.gnu.arm.ident 0 : { KEEP (*(.note.gnu.arm.ident)) }
  /DISCARD/ : { *(.ARM.exidx*) *(.note.GNU-stack) *(.gnu_debuglink) *(.gnu.lto_*) *(.init) *(.fini) }
}
