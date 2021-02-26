/*=============================================================================
  
                     SCATTER LOADING DESCRIPTION FILE
  
  Copyright 2014 - 2017 by Qualcomm Technologies, Inc. All Rights Reserved.
  
  GENERAL DESCRIPTION
  
  The scatter loading description file is used to define the SBL1 memory map.
=============================================================================*/
/*=============================================================================
  
                            EDIT HISTORY FOR FILE
   This section contains comments describing changes made to the module.
   Notice that changes are listed in reverse chronological order.
  
  
  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  07/25/17   yps     Added support to retrieve external heap base and size
  07/19/17   yps     Seperate usb DMA region
  03/03/17   kpa     Added support to retrieve internal heap base and size  
  03/01/17   kpa     Added check to ensure vector table size is within limits
  01/26/17   yps     fixed CLANG38 upgrade issue
  06/02/16	 rp		 Remove SCL_DDR_TRAINING_DATA_BUF_SIZE from assert for SBL1_DATA_ZI
  03/26/16   kpa     move smc api code to sdi region for access in debug flow
  03/25/16   kpa     revert updates to reuse bootrom stack address space  
  03/22/16   kpa     relocate boot ZI from ocimem to L2 TCM, Relocate stack to 
                     reclaim bootrom buffer, remove SBL1_STACK,
                     Added DDR driver segment
  12/17/15   jt      Change sdcc_mmc_util from .bss to COMMON
  10/27/15   elt     Added regions for boot logger time markers
  10/07/15   plc     Add RUMI UFS symbols to correct region
  08/07/15   plc     Update for MSM8998
  08/03/15   rp	     Moved page table and shared function table to OCIMEM
  07/24/15   ck      Moved HALbimc.o to TCM region
  07/21/15   kpa     Resize XBL L2 TCM ZI buffer.
  07/15/15   bn      Moved buffers used by SDCC's clients to OCIMEM
  02/26/15   ck      Added missing busywait and rodata allocations
  02/20/15   kpa     Code cleanup. 
  02/12/15   kpa     Update Debug/SDI region.  
  02/12/15   kpa     Add assert to check RPM coderam buffer size.  
  02/12/15   kpa     Add XBL code region to rpm code ram buffer.  
  01/31/15   kpa     Updates to zero initialize OCIMEM ZI buffer.  
  01/21/15   kpa     Move pre-ddr init page tables to code ram, add ocimem buffer
  11/20/14   ck      Removed references to USB as it has moved to XBLRamDump
  10/22/14   ck      Added .bss to page table regions to ensure they are ZI
  10/01/14   kpa     Include Target_cust.h instead of boot_target_scl.h
  08/18/14   ck      Cleaned up SBL regions and added region protections
  08/08/14   ck      Renamed and resized SBL1_DLOAD_ZI as contents have moved to XBLRamDump.
  06/26/14   kedara  Update sections.
  05/15/14   kedara  Inital revision.
=============================================================================*/

#include "../../Include/Target_cust.h"

PHDRS {
  CODE_ROM PT_LOAD;
  CODE_STACK PT_LOAD;
  CODE_RAM PT_LOAD;
  CODE_RAM_ZI PT_LOAD;
  BOOT_DDR PT_LOAD;
  BOOT_SYSTEM_DEBUG PT_LOAD;
  BOOT_SYSTEM_DEBUG_DATA PT_LOAD;
  BOOT_DDR_DSF_RAM_BUFFER PT_LOAD;
  BOOT_OCIMEM_DATA PT_LOAD;
  BOOT_OCIMEM_DEVICEPROG_DATA PT_LOAD;
}

SECTIONS
{
  /*====================== Sections in OCIMEM ===============================*/

  /* Area of OCIMEM used for XBL code/data */
  SBL1_OCIMEM_DATA  SCL_SBL1_OCIMEM_DATA_BASE :
  {
    Image$$SBL1_OCIMEM_DATA$$Base = . ;
    
    *(.bss.BOOT_OCIMEM_PAGE_TABLE_ZONE)
    *ufs_bsp_*.o* (COMMON);
    *ufs_memory.o* (COMMON);
    *coldplug_pcache*.o* (.bss);
    *boot_sdcc_hotplug.o* (.bss);
    
    *sdcc_mini_api.o* (COMMON);
    *sdcc_hc.o* (.bss);
    *sdcc_mmc_util.o* (COMMON);
    
    ASSERT(SIZEOF(SBL1_OCIMEM_DATA) <= SCL_SBL1_OCIMEM_DATA_SIZE,   "SBL1_OCIMEM_DATA is too large for DeviceProgrammerLite [ERROR]");
  } : BOOT_OCIMEM_DATA
  Image$$SBL1_OCIMEM_DATA$$ZI$$Length = SIZEOF(SBL1_OCIMEM_DATA);

  DEVPROG_UNCACHED_ZI SCL_DEVICEPROG_ZI_BASE:
  {
    Image$$SBL1_DEVPROG_UNCACHED_ZI$$Base = . ;
    *(.bss.BOOT_UNCACHED_ZI_ZONE)
  }: BOOT_OCIMEM_DEVICEPROG_DATA
  Image$$SBL1_DEVPROG_UNCACHED_ZI$$ZI$$Length= SIZEOF(DEVPROG_UNCACHED_ZI);
  
  SBL1_OCIMEM_DEVICEPROG_DATA : 
  {
    Image$$SBL1_DEVICEPROG_FH_ZI$$Base = . ;
    *boot_sdcc_hotplug.o* (.bss);
    qusb_*.o* (COMMON)
    qusb_*.o* (.bss .bss.*)
    devprg_*.o* (.bss .bss.*)
    devprg_*.o* (COMMON)
    devprgb_buffers_li*.o* (.bss .bss.*)
    devprgb_buffers_li*.o* (COMMON)
  
    ASSERT(SIZEOF(DEVPROG_UNCACHED_ZI) + SIZEOF(SBL1_OCIMEM_DEVICEPROG_DATA) <= SCL_DEVICEPROG_ZI_SIZE, "Invalid size of SBL1_OCIMEM_DEVICEPROG_DATA Section for DeviceProgrammerLite [ERROR]");
  }: BOOT_OCIMEM_DEVICEPROG_DATA
  Image$$SBL1_DEVICEPROG_FH_ZI$$ZI$$Length = SIZEOF(SBL1_OCIMEM_DEVICEPROG_DATA);

  /* This section contains the system debug image code that came from SDI.
  */
  SYSTEM_DEBUG_CO_RO SCL_SYSTEM_DEBUG_CO_RO_BASE:
  {
    *(BOOT_UTIL_ASM)
    *boot_extern_debug_interface*.o* (.text*); 
    *ddr_sdi_wrapper*.o* (.text*); 
    *ddrss_init_sdi*.o* (.text*); 
    *ClockSDI*.o* (.text*);
    *icb_sdi*.o* (.text*);
    *busywait.o* (.text*);
    PlatformInfoLoader.o* (.text*)
    *TLMM*.o* (.text*)
    DalVAdc.o* (.text*)
    PmicArb*.o* (.text*)

    *boot_extern_debug_interface*.o* (.rodata*); 
    *ddr_sdi_wrapper*.o* (.rodata*); 
    *ddrss_init_sdi*.o* (.rodata*); 
    *ClockSDI*.o* (.rodata*);
    *icb_sdi*.o* (.rodata*);
    *busywait.o* (.rodata*);
    *(XBL_SDI_ASM)
    ASSERT(SIZEOF(SYSTEM_DEBUG_CO_RO) <= SCL_SYSTEM_DEBUG_CO_RO_SIZE, "Invalid size for SYSTEM_DEBUG_CO_RO  for DeviceProgrammerLite [ERROR]");
  } : BOOT_SYSTEM_DEBUG
  
  /* section contains system debug related data and placed above system debug 
     related CO+RO in OCIMEM.
   */
  SYSTEM_DEBUG_DATA SCL_SYSTEM_DEBUG_DATA_BASE : ALIGN(8)
  {
    *boot_extern_debug_interface*.o* (.data*);
    *boot_extern_debug_interface*.o* (.bss*);
    *ddr_sdi_wrapper*.o* (.data*);
    *ddr_sdi_wrapper*.o* (.bss*);
    *ddrss_init_sdi*.o* (.data*);
    *ddrss_init_sdi*.o* (.bss*);
    *ClockSDI*.o* (.data*);
    *ClockSDI*.o* (.bss*);
    *icb_sdi*.o* (.data*);
    *icb_sdi*.o* (.bss*);
    
    ASSERT(SIZEOF(SYSTEM_DEBUG_DATA) <= SCL_SYSTEM_DEBUG_DATA_SIZE, "Invalid size for SYSTEM_DEBUG_DATA  for DeviceProgrammerLite [ERROR]");	
  } : BOOT_SYSTEM_DEBUG_DATA
  
  
  /* This must be placed at the end or adjacent execution regions overlap */
  SYSTEM_DEBUG_QSEE_INFO SCL_SYSTEM_DEBUG_QSEE_INFO_BASE :ALIGN(8)
  { 
    Image$$SYSTEM_DEBUG_QSEE_INFO$$Base = . ;
    
  } : BOOT_SYSTEM_DEBUG_DATA
  Image$$SYSTEM_DEBUG_QSEE_INFO$$ZI$$Length = SIZEOF(SYSTEM_DEBUG_QSEE_INFO);
  
  /*=================== Sections Containing DDR/DSF Driver code =============*/

  SBL1_DDR_DSF_ROM SCL_SBL1_DDR_DSF_ROM_BASE :
  {
    *(DDRSS_MEM_RDWR)  
    *ddrss*.o* (EXCLUDE_FILE( *ddrss_init_sdi*.o* ) .text*)
    *ddr_ss*.o* (.text*); 
    *shrm*.o* (.text*);     
    mc*.o* (.text*);   
    *ddr_phy*.o* (.text*);  
    *dtts_load_ram*.o* (.text*);
    *mccc*.o* (.text*);
    *llcc*.o* (.text*)
    *target_config*.o* (.text*);
    *ddr_training_params*.o* (.text*);
    *frwk*.o* (.text*);
    
    *ddrss*.o* (EXCLUDE_FILE( *ddrss_init_sdi*.o* ) .rodata*)
    *ddr_ss*.o* (.rodata*);
    *shrm*.o* (.rodata*);
    mc*.o* (.rodata*);  
    *ddr_phy*.o* (.rodata*);      
    *dtts_load_ram*.o* (.rodata*);
    *mccc*.o* (.rodata*);
    *llcc*.o* (.rodata*);
    *target_config*.o* (.rodata*);
    *ddr_training_params*.o* (.rodata*);
    *frwk*.o* (.rodata*);
    
    *ddrss*.o* (EXCLUDE_FILE( *ddrss_init_sdi*.o* ) .constdata*)
    *ddr_ss*.o* (.constdata*);
    *shrm*.o* (.constdata*);
    mc*.o* (.constdata*);    
    *ddr_phy*.o* (.constdata*);  
    *dtts_load_ram*.o* (.constdata*);   
    *mccc*.o* (.constdata*);
    *llcc*.o* (.constdata*);
    *target_config*.o* (.constdata*);
    *ddr_training_params*.o* (.constdata*);
    *frwk*.o* (.constdata*);

    *ddrss*.o* (EXCLUDE_FILE( *ddrss_init_sdi*.o* ) .stub*)
    *ddr_ss*.o* (.stub*);
    *shrm*.o* (.stub*);
    mc*.o* (.stub*);    
    *ddr_phy*.o* (.stub*);  
    *dtts_load_ram*.o* (.stub*);     
    *mccc*.o* (.stub*);
    *llcc*.o* (.stub*);
    *target_config*.o*	(.stub*);
    *ddr_training_params*.o* (.stub*);
    *frwk*.o* (.stub*);
    
    ASSERT(SIZEOF(SBL1_DDR_DSF_ROM) <= SCL_SBL1_DDR_DSF_ROM_SIZE,   "DeviceProgrammerDDR: SBL1_DDR_DSF_ROM Region Overflow [Error]");
  
  } : BOOT_DDR_DSF_RAM_BUFFER
  
  SBL1_DDR_DSF_DATA SCL_SBL1_DDR_DSF_DATA_BASE :
  {
    *ddrss*.o* (EXCLUDE_FILE( *ddrss_init_sdi*.o* ) .data*)
    *ddr_ss*.o* (.data*);
    *shrm*.o* (.data*);
    mc*.o* (.data*);  
    *ddr_phy*.o* (.data*);   
    *dtts_load_ram*.o* (.data*);
    *mccc*.o* (.data*);
    *llcc*.o* (.data*);
    *target_config*.o* (.data*);
    *ddr_training_params*.o* (.data*);
    *frwk*.o* (.data*);
    
    *ddrss*.o* (EXCLUDE_FILE( *ddrss_init_sdi*.o* ) .bss*)
    *ddr_ss*.o* (.bss*);
    *shrm*.o* (.bss*);
    mc*.o* (.bss*);  
    *ddr_phy*.o* (.bss*);     
    *dtts_load_ram*.o* (.bss*);    
    *mccc*.o* (.bss*);
    *llcc*.o* (.bss*);
    *target_config*.o*	(.bss*);
    *ddr_training_params*.o* (.bss*);
    *frwk*.o* (.bss*);
    
    ASSERT(SIZEOF(SBL1_DDR_DSF_DATA) <= SCL_SBL1_DDR_DSF_DATA_SIZE,   "DeviceProgrammerDDR: SBL1_DDR_DSF_DATA Region Overflow [Error]");
    ASSERT((SCL_SBL1_DDR_DSF_ROM_SIZE + SCL_SBL1_DDR_DSF_DATA_SIZE) 
            <= SCL_SBL1_DDR_DSF_SEGMENT_SIZE,   "DeviceProgrammerDDR: DDR/DSF driver region Overflow [Error]");
 
  } : BOOT_DDR_DSF_RAM_BUFFER  
    
  
  /*====================== Sections in L2 TCM ===============================*/
  /*  This section contains SBL1's code and ro data */
  SBL1_ROM SCL_SBL1_CODE_BASE: 
  {
    Image$$SBL1_VECTOR_TABLE$$Start = . ;
    *(SBL1_VECTOR_TABLE)
    Image$$SBL1_VECTOR_TABLE$$End = . ;
    *(SBL1_EXCEPTION_HANDLERS)
    *sbl1_Aarch64.o* (SBL1_ENTRY)
    *(RO)
    *(ARM_MMU)
    *(.gcc_except_table  .got .got.plt )
    /* RO DATA */
    *(.gnu.linkonce.r.*)

    *(.rodata* )
    *(.constdata* )
    *(.text*)
    *(.stub )
     ASSERT((Image$$SBL1_VECTOR_TABLE$$End - Image$$SBL1_VECTOR_TABLE$$Start) <= SCL_SBL1_VECTOR_TABLE_SIZE, "DeviceProgrammerLite: SBL1_VECTOR_TABLE Region Overflow");
     ASSERT(SIZEOF(SBL1_ROM) <= SCL_SBL1_CODE_SIZE, "Invalid size of SBL1_ROM Section for DeviceProgrammerLite [ERROR]");
  } : CODE_ROM


  SBL1_STACK SCL_SBL1_STACK_BASE: 
  {
    . = . + SCL_SBL1_STACK_SIZE;
    ASSERT(SIZEOF(SBL1_STACK) == SCL_SBL1_STACK_SIZE, "Invalid size of SBL1_STACK Section");  
  } : CODE_STACK


  SBL1_DATA_RW SCL_SBL1_DATA_BASE :
  {
    Image$$SBL1_DATA_RW$$Base = . ;

    *(ERR_DATA_PTR ERR_DATA  .tdata)

    *(.data* )
 
    ASSERT(SIZEOF(SBL1_DATA_RW) <= SCL_SBL1_DATA_SIZE, "Invalid size of SBL1_DATA_RW Section for DeviceProgrammerLite [ERROR]");
  } : CODE_RAM
  Image$$SBL1_DATA_RW$$Length = SIZEOF(SBL1_DATA_RW);

  TBSS :{*(.tbss) }

  /*====================== Sections in DDR =========================*/

  /* This section contains all ZI that can be allocated in DDR. */  
  SBL1_DDR_ZI SCL_SBL1_DDR_ZI_BASE :
  {
    Image$$SBL1_DDR_ZI$$Base = . ;

    /* Placeholder to reserve space for the boot logger's meta info data. */
    Image$$SBL1_DDR_LOG_META_INFO_SECTION$$Base = .;
    . = . + SCL_SBL1_BOOT_LOG_META_INFO_SIZE;

    /* Placeholder to reserve buffer space for the boot logger's log messages */
    Image$$SBL1_DDR_LOG_BUF_SECTION$$Base = .;  
    . = . + SCL_SBL1_DDR_BOOT_LOG_BUF_SIZE;

    /* Placeholder to reserve buffer space for the boot logger's time markers */
    Image$$SBL1_DDR_TIME_MARKER_BUF_SECTION$$Base = .;
    . = . + SCL_SBL1_DDR_BOOT_TIME_MARKER_BUF_SIZE;    
    
    * (.bss.BOOT_DDR_ZI_DATA_ZONE)
    *coldplug*.o (.bss*)
    *fs_*.o (.bss*)
    *hfat*.o (.bss*)
    *Ce*.o (.bss*)
    Image$$BOOT_EXTERNAL_HEAP$$Base = . ;
    *(.bss.BOOT_EXTERNAL_HEAP)
    BOOT_EXTERNAL_HEAP_LENGTH_PLUS_START_ADDR = . ;
    *(.bss.BOOT_DDR_ZI_ZONE)
    ASSERT(SIZEOF(SBL1_DDR_ZI) <= SCL_SBL1_DDR_ZI_SIZE, "SBL1_DDR_ZI is too large for DeviceProgrammerLite [ERROR]");
  } : BOOT_DDR
  Image$$SBL1_DDR_ZI$$ZI$$Length = SIZEOF(SBL1_DDR_ZI);
  Image$$BOOT_EXTERNAL_HEAP$$Length = BOOT_EXTERNAL_HEAP_LENGTH_PLUS_START_ADDR - Image$$BOOT_EXTERNAL_HEAP$$Base;
  SBL1_DEVICEPROG_USB_ZI : ALIGN(8)
  {
    Image$$SBL1_DEVICEPROGRAMMER_USB_ZI$$Base = .;
    *devprgb_buffers_dd*.obj (.bss .bss.*)
    *devprgb_buffers_dd*.obj (COMMON)
  }
  Image$$SBL1_DEVICEPROGRAMMER_USB_ZI$$ZI$$Length = SIZEOF(SBL1_DEVICEPROG_USB_ZI);

  /* This section contains L1 and L2 page table that's allocated in DDR.
   SCL_SBL1_DDR_PAGE_TABLE_BASE must be 16k aligned */
  SBL1_DDR_PAGE_TABLE  SCL_SBL1_DDR_PAGE_TABLE_BASE :
 {
    *(.bss.BOOT_DDR_PAGE_TABLE_ZONE)
    ASSERT(SIZEOF(SBL1_DDR_PAGE_TABLE) <= SCL_SBL1_DDR_PAGE_TABLE_SIZE,   "SBL1_DDR_PAGE_TABLE is too large for DeviceProgrammerLite [ERROR]");
  } : BOOT_DDR


  /* This section contains the uncached ZI region */
  SBL1_DDR_UNCACHED_ZI  SCL_SBL1_DDR_UNCACHED_ZI_BASE :
  {
    Image$$SBL1_DDR_UNCACHED_ZI$$Base = . ;
    *(BOOT_DDR_UNCACHED_ZI)  
    *(.bss.BOOT_DDR_UNCACHED_ZI_ZONE)

    ASSERT(SIZEOF(SBL1_DDR_UNCACHED_ZI) <= SCL_SBL1_DDR_UNCACHED_ZI_SIZE, "SBL1_DDR_UNCACHED_ZI is too large for DeviceProgrammerLite [ERROR]");

    /* Ensure that all DDR regions fit in the allocated memory space */
    ASSERT((SCL_SBL1_DDR_ZI_SIZE + SCL_SBL1_DDR_PAGE_TABLE_SIZE + SCL_SBL1_DDR_UNCACHED_ZI_SIZE) <= SCL_SBL1_DDR_DATA_SIZE, "SBL1 DDR Region Overflow for DeviceProgrammerLite [ERROR]");
  } : BOOT_DDR
  Image$$SBL1_DDR_UNCACHED_ZI$$ZI$$Length = SIZEOF(SBL1_DDR_UNCACHED_ZI);
 

  /*====================== BSS Section in L2 TCM =========================*/
 
   /* Area of L2 TCM used to hold L2 and L3 page tables because PBL does not supply
     enough memory to expand on what it created. */
  SBL1_PAGE_TABLE  SCL_SBL1_PAGE_TABLE_BASE :
  {
    *(.bss.BOOT_PAGE_TABLE_ZONE)
    ASSERT(SIZEOF(SBL1_PAGE_TABLE) <= SCL_SBL1_PAGE_TABLE_SIZE,   "DeviceProgrammerLite: SBL1_PAGE_TABLE Region Overflow [ERROR]");
  } : CODE_RAM_ZI  
  
  
  SBL1_SHARED_FUNCTIONS_TABLE SCL_SBL1_SHARED_FUNCTIONS_TABLE_BASE :
  {
    Image$$SBL1_SHARED_FUNCTIONS_TABLE$$Base = . ;
    . = . + SCL_SBL1_SHARED_FUNCTIONS_TABLE_SIZE;

  } : CODE_RAM_ZI
  Image$$SBL1_SHARED_FUNCTIONS_TABLE$$Length = SIZEOF(SBL1_SHARED_FUNCTIONS_TABLE);
 
 
  /* This section contains non DDR ZI. */
  /* Must be placed at EOF as it contains the *(.bss.*) catch all */
  SBL1_DATA_ZI SCL_SBL1_DATA_ZI_BASE: 
  {
    Image$$SBL1_DATA_ZI$$Base = . ;

    /* Placeholder to reserve space for the boot logger's meta info data. */
     Image$$SBL1_LOG_META_INFO_SECTION$$Base = .;
    . = . + SCL_SBL1_BOOT_LOG_META_INFO_SIZE;
    
    /* Placeholder to reserve buffer space for the boot logger's log messages */
    Image$$SBL1_LOG_BUF_SECTION$$Base = .;
    . = . +  SCL_SBL1_BOOT_LOG_BUF_SIZE;

    /* Placeholder to reserve buffer space for the boot logger's time markers */
    Image$$SBL1_TIME_MARKER_BUF_SECTION$$Base = .;
    . = . +  SCL_SBL1_BOOT_TIME_MARKER_BUF_SIZE;
        
    *fs_hotplug*.o(.bss)
    *fs_blockdev_ufs*.o(.bss)
    *fs_blockdev*.o(.bss)
    *ufs_memory.o(.bss)

    Image$$BOOT_INTERNAL_HEAP$$Base = . ;
    *(.bss.BOOT_INTERNAL_HEAP)
    BOOT_BOOT_INTERNAL_HEAP_PLUS_START_ADDR = . ;
    
    *(.bss.BOOT_INTERNAL_MEMZI)
    *(CACHE_MMU_ZI)
    /* excludes .bss for DDR_ZI and DLOAD_ZI */    
    *( .bss* )
    *(.dynbss)
    *(COMMON)
    *(__libc_freeres_ptrs)
    ASSERT(SIZEOF(SBL1_DATA_ZI) <= SCL_SBL1_DATA_ZI_SIZE, "Invalid size of SBL1_DATA_ZI Section for DeviceProgrammerLite [ERROR]");

    /* Ensure SBL1 regions fit inside of allocated region */
    ASSERT((SCL_SBL1_CODE_SIZE + SCL_SBL1_STACK_SIZE+ SCL_SBL1_DATA_SIZE + SCL_SBL1_DATA_ZI_SIZE
             + SCL_SBL1_PAGE_TABLE_SIZE + SCL_SBL1_SHARED_FUNCTIONS_TABLE_SIZE)
            <= SCL_SBL1_IMAGE_SIZE, "SBL1 Image Size Overflow for DeviceProgrammerLite [ERROR]");
  } : CODE_RAM_ZI
  Image$$SBL1_DATA_ZI$$ZI$$Length = SIZEOF(SBL1_DATA_ZI) ;
  Image$$BOOT_INTERNAL_HEAP$$Length = BOOT_BOOT_INTERNAL_HEAP_PLUS_START_ADDR - Image$$BOOT_INTERNAL_HEAP$$Base ;
  /*======================================================================*/


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
