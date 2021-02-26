/*=============================================================================
  
                     SCATTER LOADING DESCRIPTION FILE
  
  Copyright 2014,2017 by Qualcomm Technologies, Inc. All Rights Reserved.
  
  GENERAL DESCRIPTION
  
  The scatter loading description file is used to define the SBL1 memory map.
=============================================================================*/
/*=============================================================================
  
                            EDIT HISTORY FOR FILE
   This section contains comments describing changes made to the module.
   Notice that changes are listed in reverse chronological order.
  
  
  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  11/10/14   aab       Updated to set pmic.elf image entry point
  10/14/14   kedara  Inital revision.
=============================================================================*/

/*=============================================================================
     NOTE:  The linker script name *has* to be "sbl1.ld" as the build rules
     use that name to check for linker script presence
=============================================================================*/

#include "../../Include/Target_cust.h"

PHDRS {
  CODE_ROM PT_LOAD;
}

SECTIONS
{
  _ModuleEntryPoint = SCL_PMIC_CONFIG_BUF_BASE;
  /*  This section contains SBL1's code and ro data */
  PMIC_ROM SCL_PMIC_CONFIG_BUF_BASE: 
  {
    KEEP(*(pm_psi_info))
    *(.text .stub .text.*)
    *(RO)
    *(.gcc_except_table  .got .got.plt )
    /* RO DATA */
    *(.constdata .rodata .rodata.* .gnu.linkonce.r.* .bss)
    
    KEEP(*(RW .data .data.*))
    
    ASSERT(SIZEOF(PMIC_ROM) <= SCL_PMIC_CONFIG_BUF_SIZE, "Invalid size of PMIC_ROM Section");
  } : CODE_ROM

  TBSS :{*(.tbss) }
 
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
