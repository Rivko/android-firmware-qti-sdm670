/*=============================================================================
  
                     SCATTER LOADING DESCRIPTION FILE
  
  Copyright 2014 - 2015,2017 by Qualcomm Technologies Technologies, Inc. 
  All Rights Reserved.
  
  GENERAL DESCRIPTION
  
  The scatter loading description file is used to define the Jtag Programmer memory map.
=============================================================================*/
/*=============================================================================
  
                            EDIT HISTORY FOR FILE
   This section contains comments describing changes made to the module.
   Notice that changes are listed in reverse chronological order.
  
  
  when         who     what, where, why
  ----------   ---     -------------------------------------------------------
  2017-01-26   yps     Fixed LLVM384 compiling issue
  2015-03-31   whc     SPI-NOR support 
  2015-02-01   bn      Corrected the Stack address
  2014-12-09   whc     added UFS support
  2014-11-06   bn      Inital revision.
=============================================================================*/

/*=============================================================================
     NOTE:  The linker script name *has* to be "sbl1.ld" as the build rules
     use that name to check for linker script presence
=============================================================================*/

#include "../../Include/Target_cust.h"

PHDRS {
  JTAG_PROGRAMMER_CODE_ROM PT_LOAD;    
  JTAG_PROGRAMMER_CODE_STACK PT_LOAD;
  JTAG_PROGRAMMER_CODE_RAM PT_LOAD;
  JTAG_PROGRAMMER_CODE_RAM_ZI PT_LOAD;
}

SECTIONS
{
  /*  This section of 128KB contains Jtag Programmer code */
  JTAG_PROGRAMMER_CODE SCL_IMEM_BASE: 
  {
    *JtagProgrammer.o* (ModuleEntryPoint)
    *JtagProgrammer_emmc.o* (RO)
    *JtagProgrammer_ufs.o* (RO)
    *JtagProgrammer_spi.o* (RO)
    *(.text .stub .text.*)
    *(ARM_MMU)
    *(RO)
    *(.gcc_except_table  .got .got.plt )
    /* RO DATA */
    *(.constdata .rodata .rodata.* .gnu.linkonce.r.*)
    /*TODO: we are 200bytes over, optimize*/
    /*ASSERT(SIZEOF(JTAG_PROGRAMMER_CODE) <= SIZE_128KB, "Jtag Programmer code is greater than dedicated 128KB");*/
  } : JTAG_PROGRAMMER_CODE_ROM
  
  /* Stack location = IMEM Base + 128KB CODE_ROM + 32KB Stack */
  JTAG_PROGRAMMER_STACK (SCL_IMEM_BASE + SIZE_128KB) : 
  {
    . = . + SIZE_32KB;
    Image$$JTAG_PROGRAMMER_STACK$$Start = . ;
  } : JTAG_PROGRAMMER_CODE_STACK

  /* The rest of the section contains Jtag Programmer Code RAM and ZI */
  JTAG_PROGRAMMER_RW :
  {
    *(.data .data.* .data1)
    *(RW)
  } : JTAG_PROGRAMMER_CODE_RAM

  /**/
  JTAG_PROGRAMMER_ZI :
  {
    Image$$JTAG_PROGRAMMER_ZI$$Start = . ;
    *(COMMON)
    *(.bss*)
    *(CACHE_MMU_ZI)
    *JtagProgrammer*.o* (.bss)
    *(ZI)
  } : JTAG_PROGRAMMER_CODE_RAM_ZI

  JTAG_PROGRAMMER_IMEM_END (SCL_IMEM_BASE + SCL_IMEM_SIZE) :
  {  
     Image$$JTAG_PROGRAMMER_ZI$$End = . ;
  }
  
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
