# 1 "/local/mnt/workspace/CRMBuilds/ADSP.VT.5.0-00526-SDM710-1_20201217_021900/b/adsp_proc/build/bsp/sdc_img/build/sdc.scl"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 323 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "/local/mnt/workspace/CRMBuilds/ADSP.VT.5.0-00526-SDM710-1_20201217_021900/b/adsp_proc/build/bsp/sdc_img/build/sdc.scl" 2
# 17 "/local/mnt/workspace/CRMBuilds/ADSP.VT.5.0-00526-SDM710-1_20201217_021900/b/adsp_proc/build/bsp/sdc_img/build/sdc.scl"
ENTRY (__main)
PHDRS {
  SDC_CODE PT_LOAD;
  SDC_DATA PT_LOAD;
}


SECTIONS {
  CODE_RAM 0x0 : {
    KEEP(*\startup.o(SDC_ENTRY))
    *(.text*)
    *(RexSyscalls)
    *(.rodata.*)
  } : SDC_CODE
  ASSERT(SIZEOF(CODE_RAM) <= 0x10000, "SDC_CODE oversized");

  CODE_RAM_END (0x0 + 0x10000) : {
    Image$$CODE_RAM_END$$Base = .;

  }

  IMAGE_INFO_HEADER 0xE0000 : {
    Image$$IMAGE_INFO_HEADER$$Base = .;

  } : SDC_DATA

  DATA_RAM (ADDR(IMAGE_INFO_HEADER)+0x40) : {
    Image$$DATA_RAM$$Base = .;
    * (.data*)
    * (.bss*)
  }

  VECTOR_TABLE ALIGN(0x400) : {
    *(VECTOR_TABLE_SECTION)
  }

  STACK_OVERFLOW (((0xE0000 + 0x8000) - 0xA00) - 0x20) : {
    . += 0x20;
  }
  STACK ((0xE0000 + 0x8000) - 0xA00) : {
    Image$$STACK$$Base = .;
    . += 0xA00;
    Image$$STACK$$ZI$$Limit = .;
  }




  DATA_RAM_END (0xE0000 + 0x8000) : {
    Image$$DATA_RAM_END$$Base = .;

  }

  ASSERT(SIZEOF(IMAGE_INFO_HEADER) + SIZEOF(DATA_RAM) + SIZEOF(STACK_OVERFLOW) + SIZEOF(STACK) < 0x8000, "SDC_DATA oversized");

  /DISCARD/ : {
    *(.ARM.exidx*)
  }
}
