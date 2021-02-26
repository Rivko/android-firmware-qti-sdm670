PHDRS {
  CODE_RO PT_LOAD;
  DATA PT_LOAD;
  GOT PT_LOAD;
  DYN PT_DYNAMIC;
  REL PT_LOAD;
}

SECTIONS {
  .text : {
    KEEP(*\teetest_entry.o (TZTEE64_TESTAPPENTRYCODE))
    KEEP(*\teetest_smc.o   (TEE64_SMC))
    * (.text .text.*)
    * (.rodata)
    * (EXCLUDE_FILE(*\metadata.o) .rodata.*)
    *metadata.o (.rodata.str1.1)
  } : CODE_RO

  .sentinel : {
    * (.rodata.sentinel)
  } : CODE_RO

  .metadata_check : {
    *metadata.o (.rodata.*)
    ASSERT(SIZEOF(.metadata_check) == 0, "New metadata not accounted for here");
  } : CODE_RO

  RW_DATA : {
    *(RW_DATA)
  }
  
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
  
  .data ALIGN(4096): {
    * (.data TZAPP64ENTRYSTACK .bss COMMON)
  } : DATA
 
  TEETEST64_L1_TT_MEM ALIGN(4096): {
    *(TEETEST64_L1_TT_MEM)
  }
  
  TEETEST64_L2_TT_MEM ALIGN(4096): {
    *(TEETEST64_L2_TT_MEM)
  }
  
  TEETEST64_L3_TT_MEM ALIGN(4096): {
    *(TEETEST64_L3_TT_MEM)
  }
  
  TEETEST64__MAPPER_MEM ALIGN(4096): {
    *(TEETEST64__MAPPER_MEM)
  }
  
  
  TZ_TEE_TEST64_APP_HEAP ALIGN(4096): {
    Image$$TZ_TEE_TEST64_APP_HEAP$$ZI$$Base = .;
    . = . + 0x18000;
    Image$$TZ_TEE_TEST64_APP_HEAP$$ZI$$Limit = .;
  } : DATA
  
  TEETEST_EL1_STACK ALIGN(4096): {
    Image$$TEETEST_EL1_STACK$$ZI$$Limit = .;
    . = . + 0x8000;
    Image$$TEETEST_EL1_STACK$$ZI$$Base = .;
  } : DATA

  .got : ALIGN(4096) { *(.got.plt) *(.igot.plt) *(.got) *(.igot) } : GOT

  .dynamic : ALIGN(4096) { *(.dynamic) }: DYN : REL

  .rela.dyn : { *(.rela.dyn) } : REL
  .rel.plt : { *(.rel.plt) } : REL
  .hash : { *(.hash) } : REL
  .dynsym : { *(.dynsym) } : REL
  .dynstr : { *(.dynstr) } : REL

  .gnu.attributes 0 : { KEEP (*(.gnu.attributes)) }
  .note.gnu.arm.ident 0 : { KEEP (*(.note.gnu.arm.ident)) }
  /DISCARD/ : { *(.ARM.exidx*) *(.note.GNU-stack) *(.gnu_debuglink) *(.gnu.lto_*) *(.init) *(.fini) }
}
