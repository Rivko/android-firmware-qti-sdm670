/*=============================================================================
  
                     SCATTER LOADING DESCRIPTION FILE
  
  Copyright 2014, 2017 by Qualcomm Technologies, Inc. All Rights Reserved.
  
  GENERAL DESCRIPTION
  
  The scatter loading description file is used to define the SBL1 memory map.
=============================================================================*/
/*=============================================================================
  
                            EDIT HISTORY FOR FILE
   This section contains comments describing changes made to the module.
   Notice that changes are listed in reverse chronological order.
  
  
  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  08/10/17   yps     Add RAMDUMP_DATA_ZI base and length
  01/26/17   yps     Upgrade compiler to CLANG38
  09/16/14   kpa     Relocate RAMDUMP_DLOAD_ZI section.
  11/20/14   ck      Refactored for QUSB driver
  10/01/14   kpa     Include Target_cust.h instead of boot_target_scl.h
  08/08/14   ck      Added RAMDUMP_DLOAD_ZI
  07/29/14   ck      Inital version.
=============================================================================*/

#include "../../Include/Target_cust.h"

PHDRS {
  RAMDUMP_CODE_ROM PT_LOAD;
  RAMDUMP_CODE_RAM PT_LOAD;
  RAMDUMP_DLOAD_BUFFERS_RAM PT_LOAD;
}

SECTIONS
{
  /* This section contains XBLRamDump code and RO data */
  RAMDUMP_ROM SCL_RAMDUMP_CODE_BASE: 
  {
    *(.text .stub .text.* .rela.text .relaRAMDUMP_ROM*)
    *(BOOT_UTIL_ASM)
    *(RO)
    *(ARM_MMU)
    *(.gcc_except_table  .got .got.plt )
    /* RO DATA */
    *(.constdata .rodata .rodata.* .gnu.linkonce.r.*)
    
    ASSERT(SIZEOF(RAMDUMP_ROM) <= SCL_RAMDUMP_CODE_SIZE, "Invalid size of RAMDUMP_ROM Section");
  } : RAMDUMP_CODE_ROM

  /* This section contains all ZI for dload and qfprom test framework
     This section is uncached, must precede RAMDUMP_DATA_RW to first match
     .bss sections listed
  */
  RAMDUMP_DLOAD_ZI  SCL_RAMDUMP_DLOAD_BUFFERS_BASE :
  {
    *coldplug*.o* (.bss)
    *fs_*.o* (.bss)
    *hfat_*.o* (.bss)
    *qusb_*.o* (.bss)
    *sahara*.o* (.bss)
    *dload*.o* (.bss)
    *(BOOT_UNCACHED_ZI)  
    *(.bss.BOOT_UNCACHED_ZI_ZONE)
    ASSERT(SIZEOF(RAMDUMP_DLOAD_ZI) <= SCL_RAMDUMP_DLOAD_BUFFERS_SIZE,
           "RAMDUMP_DLOAD_ZI is too large");
  } : RAMDUMP_DLOAD_BUFFERS_RAM  

  RAMDUMP_DATA_RW SCL_RAMDUMP_DATA_BASE :
  {
    *(.data .data.* .data1 .rela.data .relaRAMDUMP_DATA_RW*)
    *(ERR_DATA_PTR ERR_DATA  .tdata)
    *(RW)

  } : RAMDUMP_CODE_RAM


  /* This section contains non DDR ZI. */
  RAMDUMP_DATA_ZI ALIGN(8): 
  {
    Image$$RAMDUMP_DATA_ZI$$Base = . ;
    *(BOOT_INTERNAL_HEAP)    
    *(.bss*)
    *(.dynbss)
    *(COMMON)
    *(__libc_freeres_ptrs)

    ASSERT(SIZEOF(RAMDUMP_DATA_RW) + SIZEOF(RAMDUMP_DATA_ZI) <= SCL_RAMDUMP_DATA_SIZE, "Invalid size of RAMDUMP_DATA Section");
    ASSERT((SCL_RAMDUMP_CODE_SIZE + SCL_RAMDUMP_DATA_SIZE) <= SCL_RAMDUMP_TOTAL_SIZE, "Invalid size of RamDump Image");
  } : RAMDUMP_CODE_RAM
  Image$$RAMDUMP_DATA_ZI$$Length = SIZEOF(RAMDUMP_DATA_ZI);

  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }
  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }
  /* DWARF 3 */
  .debug_pubtypes 0 : { *(.debug_pubtypes) }
  .debug_ranges   0 : { *(.debug_ranges) }
  .gnu.attributes 0 : { KEEP (*(.gnu.attributes)) }
  .note.gnu.arm.ident 0 : { KEEP (*(.note.gnu.arm.ident)) }
  /DISCARD/ : { *(.ARM.exidx*) *(.note.GNU-stack) *(.gnu_debuglink) *(.gnu.lto_*) *(.init) *(.fini) }
}
