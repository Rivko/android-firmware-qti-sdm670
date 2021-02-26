/*=============================================================================

                              SBL1 Main Control

GENERAL DESCRIPTION
  This file contains the main control for SBL1 execution. Since this implements
  the main control functionality, it may be dependent on all modules of the
  system.

Copyright 2011-2018 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when         who     what, where, why
----------   ---     ----------------------------------------------------------
10/23/17     aus     Check return from Clock_debugInit
07/19/17     yps     Map device programmer DMA region as noncacheable
07/05/17     rp      Added boot_ddr_test call after pimem initialization (once XBL_SEC unlocks DDR regions)
06/27/17     yps     remove mmu_flush_cache_and_disable and mmu_enable_instruction_cache
06/23/17     ds      coldplug changes
06/15/17     yps     Added flush cache regions and disable cache before mmu disable.After MMU initialized enable cache&mmu.
06/06/17     ds		 update mmu settings for regions mapped by PBL
06/01/17     daison   Added generic function sbl1_save_data_to_ddr_partition and modified 
                      sbl1_save_ddr_training_data_to_partition
05/15/17     c_vkuppa Remove initialization of roll back protection image table
05/02/17     yps     Added sbl1_get_external_heap_addr and  sbl1_get_external_heap_size for ramdump
04/18/17     kpa     remove logging for handling efs cookies
04/01/17     kpa     Added xblconfig_ramlib_init
03/03/17     ds      remove clobber prot
01/31/17     digants whitelist related changes
01/30/17     ds      mmu disable before doing any mmu change
12/14/16     vpo     Added initialization for chip info driver
11/07/16     kpa     move pimem init to sbl1_post_ddr_init
10/21/16     kpa     Change DDR size for PreSil to be 2GB
09/30/16     kpa     Update DDR size for PreSil boot setup
07/28/16     kpa     Remove PRE_SIL flag and dependencies.
06/15/16     kpa     updates to mark boot log init info
05/03/16     rp      Remove training check from sbl1_post_ddr_init
03/17/16     bd      Add few more regions to clobber list
01/07/16     plc     Flush the region we ZI, in case PBL marks us as WB Cache
10/27/15     elt     Added support for boot log independent time marker
10/22/15     plc     Replaced tlb invalidate with generic EL-agnostic call
10/13/15     plc     Sync to 8996 tip
10/12/15     plc     Enable pre-silicon population of PlatformInfo RUMI/Virtio
10/07/15     plc     Enable cache and add runtime support to detect RUMI/Virtio
08/26/15     plc     Bring 8998 up to 8996 tip
08/07/15     plc     Stub out mmu, pbl-dependent shared data for ramload virtio
06/25/15     as      Added sbl1_get_shared_data
06/24/15     ck      Moved boot log initialization
03/27/15     kpa     Add ocimem XBL buffers to clobber protect, code cleanup.
02/03/15     wek     Make MMU Attributes static, reverts change on 02/03/15.
02/19/15     kpa     Updates to pass ddr self refresh api info to Qsee.
02/03/15     wek     Change the MMU attributes for DeviceProgrammer.
02/02/15     wek     Updates to zero initialize OCIMEM ZI buffer for DevProg.
01/31/15     kpa     Updates to zero initialize OCIMEM ZI buffer.
01/29/15     kpa     Update secdat_buffer_location.
11/24/14     wek     Added device programmer lite call.
10/29/14     kpa     Support 64 bit memory map
10/15/14     kpa     Initialize bl_shared_data.build_type
09/30/14     ck      Switched from extern_efs_interface to extern_efs_cookie_interface
09/09/14     kedara  Fix SBL1_DDR_LOG_BUF_SECTION address
08/25/14     kedara  Update cache mmu configuration.
08/19/14     ck      Refactored boot log logic because it moved
08/08/14     ck      Renamed and resized DLOAD_ZI
07/21/14     ck      Added call to boot_shared_functions_register
07/18/14     kedara  Added feature_fix_8996_mmu, temporarily disable mmu config.
06/12/14     kedara  Updates to support 64 bit compilation using llvm.
06/03/14     ck      Added flash statistic entry when deinitializing boot logger
05/28/14     ck      Added PBL timestamp milestones to sbl1_boot_logger_init
04/11/14     ck      Added SMEM and OCIMEM to boot clobber memory protection
04/08/14     ck      Set mmu_pagetable_size in bl_shared_data after moving PT's to DDR
04/08/14     ck      Added call to boot_l2_page_table_init in setup of initial memory map
04/08/14     ck      Added logic to copy page table size from PBL into bl_shared_data
04/02/14     ck      Updated use of boot_sbl_qsee_interface as it changed per Bear BAS 2.0
04/01/14     ck      Updated sbl1_retrieve_shared_info_from_pbl as Bear PBL now uses common secboot
03/25/14     jz      Ported placeholder for sbl_save_regs
03/21/14     ck      Added support to load SEC partition
03/18/14     ck      Updated boot_hw_reset calls as they now take a reset type parameter
03/14/14     ck      Ported fix to move boot_busywait_init before boot_ logger initialization
03/06/14     ck      Added sbl1_populate_initial_mem_map to sbl1_main_ctl to get execution
                     in RPM Code RAM early
01/08/14     ck      Changed SEGMENT #2 references to SYSTEM_DEBUG
01/03/14     ck      Added hole in boot_clobber memory region for DDR training partition.
01/03/14     ck      Added DDR and Segment #2 region to boot_clobber memory region.
12/16/13     ck      Removed sbl1_qfprom_test as TZ runs after SBL in Bear family.
12/13/13     ck      Removed boot_rollback_update_img_version call to update SBL version.
12/09/13     ck      Removed sbl1_update_sbl1_rollback_fuse and sbl1_update_all_rollback_fuses
                     as all fuse blowing is done after SBL in QSEE for Bear family
12/09/13     ck      Removed sbl1_update_sbl1_rollback_fuse as all fuse blowing is done after SBL in QSEE
11/21/13     ck      Removed sbl1_signal_tz_sbl_done
10/22/13     ck      Removed call to DISABLE_REMAPPER because it is not needed in 8916
10/17/13     ck      Removed sbl1_rpm_sync() because in Bear family RPM is
                     not executed until after SBL1 has run.
10/17/13     ck      Removed sbl1_check_l2_parity_error() as this was used in
                     Krait processors.  8916 is A53.
10/17/13     ck      Removed sbl1_update_shared_imem_base() as the alternative
                     IMEM location was a 8974v1 thing.  Not needed in 8916.
10/17/13     ck      Removed sbl1_pagetable_dep_enable() as the stack and
                     pagetable area has changed.  The entire MMU initialization
                     needs to be overhauled and will be before 8916 release.
08/06/13     aus     Added support for new secboot_verified_info structure
07/10/13     aus     Added placeholder for sbl_error_handler
06/26/13     aus     Remove sbl1_dead_battery_charge
06/13/13     dh      Update sbl1_load_ddr_training_data_from_partition to take buffer
                     address as argument
04/04/13     dh      Move boot_DALSYS_InitMod to early sbl1_main_ctl
04/03/13     dh      Use boot_dload_is_dload_mode_set instead of uefi dump cookie
                     to determin if device is in dload mode
04/11/13     jz      Disable remapper
04/03/13     dh      Add boot_init_stack_chk_canary in the beginning of sbl1_main_ctl
03/28/13     dhaval  set hash algorithm to be used during elf hashing in SBL
03/26/13     dh      Rename sbl1_relocate_page_table to sbl1_relocate_page_table_to_ddr
03/21/13     dh      Call sbl1_move_boot_log_to_ddr inside sbl1_relocate_page_table
                     to ensure we don't log in ddr before training is done
03/14/13     jz      Use mmu_set_dacr to configure DACR for client access
03/05/13     dh      Use mmu_l1_pagetable_base in sbl_shared_data as a common
                     pointer to page table base
02/28/13     jz      Moved page table settings out to boot_mmu_table.h
02/27/13     dh      If DDR training is not required OR we are in dload mode 
                     relocate page table to ddr and map all the regions we need
02/15/13     jz      Fixed bootup issue after TZ execution due to missing page table
02/14/13     kedara  Added sbl1_update_all_rollback_fuses.
02/13/13     dh      Add boot_secboot_ftbl_init
02/07/13     dh      Refactor sbl1 page table api
01/28/13     dh      Map sbl1 code section as read only 
12/06/12     dh      Change SHARED_IMEM_TPM_HASH_REGION_BASE to SHARED_IMEM_TPM_HASH_REGION_OFFSET
12/05/12     dh      if BOOT_ENTER_PBL_DLOAD_ON_SBL_ERROR is defined do not enter dload mode
12/04/12     dh      Move shared imem to OCIMEM for 8974v2
12/03/12     dh      Remove sbl1_rollback_version_update, add sbl1_update_sbl1_rollback_fuse
11/27/12     dh      Add sbl1_rpm_sync
11/19/12     dh      Store sbl1's hash info passed by pbl to shared imem
11/28/12     dh      Add roll back version protection logic for sbl1
11/16/12     dh      Check for L2 cache parity error in beginning of sbl1, if error
                     exists set dload cookie and reset after pmic device is configured
                     Move boot_shared_imem_init in beginning of sbl1_main_ctl
10/24/12     dh      Add boot_internal_heap and boot_external_heap, call
                     boot_DALSYS_HeapInit to initialize DAL heaps
10/09/12     dh      Add sbl1_debug_mode_enter to enable cookie based debugging
09/21/12     dh      Add boot_clock_debug_init as the first thing in sbl1_main_ctl
09/13/12     dh      Set boot logger's reference time to 0 
                     since mpm sleep clock resets after sys.u
08/30/12     dh      Relocate boot log to DDR once DDR is up
08/20/12     dh      Add sbl1_update_shared_imem_base to move shared imem base
                     to rpm msg ram for 8974v1
08/16/12     dh      Move sbl1_retrieve_shared_info_from_pbl in front of sbl1_hw_init
07/19/12     dh      Add sbl1_qfprom_test to call qfprom test framework
07/16/12     dh      Call sbl1_hw_init in early sbl1_main_ctl
06/07/12     dh      Zero initialize DLOAD buffer region
05/30/12     dh      Update boot logger to use rpm pbl start time as reference time
05/29/12     dh      Add sbl1_tlmm_init
05/24/12     dh      combine sbl1_ddr_data_init and sbl1_relocate_page_table into
                     sbl1_post_ddr_data_init
05/08/12     dh      Add sbl1_dead_battery_charge
05/03/12     dh      Remove hw_smem_init, it's integrated in 
                     DalPlatformInfo_CfgFromMem now
04/18/12     dh      passing appsbl entry address in tz milestone call as tz will
                     jump to appsbl.
03/15/12     dh      Added sbl1_populate_initial_mem_map and sbl1_relocate_page_table
12/01/11     dh      Added sbl1_rollback_version_update and sbl1_signal_tz_sbl_done.
08/26/11     dh      Initial version
===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include <stdio.h>
#include BOOT_PBL_H
#include "boot_sbl_if.h"
#include "boot_logger.h"
#include "boot_target.h"
#include "boot_dload.h"
#include "boot_util.h"
#include "boot_cache_mmu.h"
#include "boot_config.h"
#include "boot_ram_init.h"
#include "boot_shared_imem_cookie.h"
#include "boot_ddr_info.h"
#include "boot_config_data.h"
#include "boot_authenticator.h"
#include "boot_extern_dal_interface.h"
#include "boot_extern_platforminfo_interface.h"
#include "boot_extern_clk_interface.h"
#include "boot_extern_tlmm_interface.h"
#include "boot_extern_ddr_interface.h"
//#include "boot_extern_ddi_interface.h"
#include "boot_extern_deviceprogrammer_lite_interface.h"
#include "boot_extern_coldplug_interface.h"
#include "boot_extern_efs_cookie_interface.h"
#include "boot_extern_busywait_interface.h"
#include "boot_extern_seccfg_interface.h"
#include "sbl1_target.h"
#include "boot_page_table_armv8.h"
#include "boot_mmu_table.h"
#include "sbl1_hw.h"
#include "boot_flash_dev_if.h"
#include "boot_sdcc.h"
#include "sbl1_mc.h"
#include "boothw_target.h"
#include "boot_loader.h"
#include "boot_authenticator.h"
#include "boot_hash.h"
#include "boot_msm.h"
#include "boot_sbl_qsee_interface.h"
#include "tzbsp_fuseprov_chipset.h"
#include "boot_elf_header.h"
#include "boot_shared_functions_producer.h"
#include "boot_statistics.h"
#include "boot_extern_debug_interface.h"
#include "boot_arch_determination.h"
#include "boot_extern_sec_img_interface.h"
#include "ChipInfo.h"
#include "boot_whitelist_prot.h"
#include "XBLConfigRamlib.h"
#include "boot_flash_target.h"

#define TZBSP_SHARED_IMEM_TZ_AREA_BASE          (SHARED_IMEM_BASE + 0x734)
#define TZBSP_IMEM_ADDR(offset) \
        (TZBSP_SHARED_IMEM_TZ_AREA_BASE + (offset))
#define TZBSP_SHARED_EXECUTION_PASS_STATE       TZBSP_IMEM_ADDR(0x78)

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

extern uintnt  Image$$SBL1_LOG_BUF_SECTION$$Base[];
extern uintnt  Image$$SBL1_TIME_MARKER_BUF_SECTION$$Base[];
extern uintnt  Image$$SBL1_LOG_META_INFO_SECTION$$Base;
extern uintnt  Image$$SBL1_DDR_LOG_META_INFO_SECTION$$Base;
extern uintnt  Image$$SBL1_DDR_LOG_BUF_SECTION$$Base[]; 
extern uintnt  Image$$SBL1_DDR_TIME_MARKER_BUF_SECTION$$Base[];
#ifdef FEATURE_DEVICEPROGRAMMER_IMAGE
extern uintnt  Image$$SBL1_DEVPROG_UNCACHED_ZI$$Base[];
extern uintnt  Image$$SBL1_DEVPROG_UNCACHED_ZI$$ZI$$Length[];
#endif

extern uint64 mmu_l1_ddr_pagetable[];

/* Shared data between SBL*/
bl_shared_data_type bl_shared_data;

static boot_sbl_if_shared_info_type sbl1_shared_data;

/* SBL1 DDR ZI Initialization structure */
static boot_ram_init_data_type sbl1_ram_init_data_ddr =
{
  NULL,   //load_rw_base;
  NULL,  //image_rw_base;
  0,        //image_rw_len;
  (uint8*)&Image$$SBL1_DDR_ZI$$Base,  //image_zi_base;
  &Image$$SBL1_DDR_ZI$$ZI$$Length     //image_zi_len;
};


/* SBL1 DDR Uncached ZI Initialization structure */
static boot_ram_init_data_type sbl1_ram_init_data_uncached_zi =
{
  NULL,  //load_rw_base;
  NULL,  //image_rw_base;
  0,     //image_rw_len;
  (uint8*)&Image$$SBL1_DDR_UNCACHED_ZI$$Base,  //image_zi_base;
  &Image$$SBL1_DDR_UNCACHED_ZI$$ZI$$Length     //image_zi_len;
};


/* SBL1 OCIMEM ZI Initialization structure */
static boot_ram_init_data_type sbl1_ram_init_data_ocimem =
{
  NULL,   //load_rw_base;
  NULL,  //image_rw_base;
  0,        //image_rw_len;
  (uint8*)&Image$$SBL1_OCIMEM_DATA$$Base ,  //image_zi_base;
  &Image$$SBL1_OCIMEM_DATA$$ZI$$Length     //image_zi_len;
};

/* SBL1 OCIMEM ZI Initialization structure */
static boot_ram_init_data_type sbl1_ram_init_data_ocimem_devprog =
{
  NULL,   //load_rw_base;
  NULL,  //image_rw_base;
  0,        //image_rw_len;
  (uint8*)&Image$$SBL1_DATA_ZI$$Base,  //image_zi_base;
  &Image$$SBL1_DATA_ZI$$ZI$$Length     //image_zi_len;
};

/* Boot Logger specific variables */
static uint32 sbl_start_time = 0;
static boot_log_init_data boot_log_data =
{
  (void *)&Image$$SBL1_LOG_BUF_SECTION$$Base,
  SCL_SBL1_BOOT_LOG_BUF_SIZE,
  (void *)&Image$$SBL1_TIME_MARKER_BUF_SECTION$$Base,
  SCL_SBL1_BOOT_TIME_MARKER_BUF_SIZE,
  (void *)&Image$$SBL1_LOG_META_INFO_SECTION$$Base,
  SCL_SBL1_BOOT_LOG_META_INFO_SIZE,
  NULL
};

/* DDR parameter partition ID */
extern uint8 ddr_params_partition_id[];

/* SEC partition ID */
extern uint8 sec_partition_id[];

/* Internal heap memory, inside the SBL1 image*/
static byte boot_internal_heap[BOOT_INTERNAL_HEAP_SIZE] SECTION(".bss.BOOT_INTERNAL_HEAP");

/* External heap memory, inside DDR */
static byte boot_external_heap[BOOT_EXTERNAL_HEAP_SIZE] SECTION(".bss.BOOT_EXTERNAL_HEAP");

/* Global variable indicating if L2 error has happened */
uint32 L2_parity_error_in_sbl = FALSE;

/*DLOAD flag for SBL1 to enter PBL error handler*/
#ifdef BOOT_ENTER_PBL_DLOAD_ON_SBL_ERROR
  static boot_boolean edload_flag = TRUE;
#else
  static boot_boolean edload_flag = FALSE;
#endif

/* Flag incidates if page table has been moved to DDR or not */
boot_boolean page_table_is_in_ddr = FALSE;

/* Secboot shared data structure */
secboot_verified_info_type sbl_verified_info;

/*=============================================================================
                              FUNCTION DEFINITIONS
=============================================================================*/

static void sbl1_debug_mode_enter()
{
  volatile uint32 *boot_debug_cookie_ptr = (uint32 *)SBL1_DEBUG_COOKIE_ADDR;
	
  /* Loop here to wait for jtag attach if cookie value matches*/
  while(*boot_debug_cookie_ptr == SBL1_DEBUG_COOKIE_VAL);
}


/*===========================================================================
**  Function :  sbl1_retrieve_shared_info_from_pbl
** ==========================================================================
*/
/*!
* 
* @brief
*   This function retreieves shared data from the PBL
* 
* @param[in] pbl_shared Pointer to shared data
*        
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static void sbl1_retrieve_shared_info_from_pbl(boot_pbl_shared_data_type *pbl_shared)
{
  static boot_pbl_shared_data_type sbl1_pbl_shared_data;
  
  BL_VERIFY( (pbl_shared != NULL), BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );
    
  /* Copy the PBL shared data structure to SBL1 address space. */
  qmemcpy(&sbl1_pbl_shared_data,
          pbl_shared,
          sizeof(sbl1_pbl_shared_data));

  sbl1_shared_data.pbl_shared_data = &sbl1_pbl_shared_data;

  bl_shared_data.sbl_shared_data = &sbl1_shared_data;
  
  /* Set sdcc device info to NULL so it will be populated in sdcc init*/
  bl_shared_data.sbl_shared_data->sdcc_boot_device_info = NULL;


  /* Set the L1 pagetable base address and total pagetable size. */  
  bl_shared_data.sbl_shared_data->mmu_l1_pagetable_base = 
                             (uint32 *)pbl_shared->pbl_page_tbl_base;

  bl_shared_data.sbl_shared_data->mmu_pagetable_size =
    pbl_shared->pbl_page_tbl_size;
  

  /* Intialize the PBL accessor routines, pbl accessor will make its own copy */
  boot_pbl_interface_init( sbl1_shared_data.pbl_shared_data );
  
  /* Copy PBL verified info into SBL verified info */
  qmemcpy(&sbl_verified_info,
          &pbl_shared->secboot_shared_data->pbl_verified_info, 
          sizeof(sbl_verified_info));

#if (defined(SHARED_IMEM_TPM_HASH_REGION_SIZE) && (defined(SHARED_IMEM_TPM_HASH_REGION_OFFSET)))
  
  /* Clear the tpm hash imem */
  qmemset((void *)((uintnt)SHARED_IMEM_TPM_HASH_REGION_OFFSET + (uintnt)boot_shared_imem_cookie_ptr), 
          0x0,
          SHARED_IMEM_TPM_HASH_REGION_SIZE);
  
  /* 
    Initialize Tpm hash pointer to base of memory location
    allocated to store all hashes.
  */
  bl_shared_data.sbl_shared_data->next_avail_tpm_hash_blk_ptr = 
         (uint32*)((uintnt)SHARED_IMEM_TPM_HASH_REGION_OFFSET + (uintnt)boot_shared_imem_cookie_ptr);
  
  /* 
    First uint32 refers to number of hashes. Initialize it to 0. The value
    will be incremented every time a hash (sha 256) is stored 
  */
  *(bl_shared_data.sbl_shared_data->next_avail_tpm_hash_blk_ptr) = 0;
  
  /* Make ptr point to start of Hashes */  
  bl_shared_data.sbl_shared_data->next_avail_tpm_hash_blk_ptr++;
  
#endif

  /* Set build type based on compile flags */
  bl_shared_data.build_type = SBL_BUILD_SBL;
  bl_shared_data.build_type = PcdGet32 (PcdBuildType);

} /* sbl1_retrieve_shared_info_from_pbl */


/*===========================================================================
**  Function :  sbl1_boot_logger_init
** ==========================================================================
*/
/*!
* 
* @brief
*   This function initialize the boot logger, must be called as the first 
*   function in main control function. 
*
* @param[in] boot_log_data boot logger's initialization data
* @param[in] pbl_shared    pbl shared data
*        
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static void sbl1_boot_logger_init
(
  boot_log_init_data *boot_log_data,
  boot_pbl_shared_data_type *pbl_shared
)
{
  /*initialize boot logger*/
  boot_log_init(boot_log_data);

  /*initialize boot logger*/
  boot_log_set_init_info_marker();

  /* Write PBL timestamp milestones into beginning of log */
  boot_pbl_log_milestones(pbl_shared);

  /*Set the reference time to 0 as the start of boot*/
  boot_log_set_ref_time(0);

  /* Add SBL start entry using stored time from beginning of sbl1_main_ctl */
  boot_log_message_raw("SBL1, Start",
                       sbl_start_time,
                       LOG_MSG_TYPE_BOOT,
                       NULL);

}/* sbl1_boot_logger_init */


/*===========================================================================
**  Function :  sbl1_boot_logger_deinit
** ==========================================================================
*/
/*!
* 
* @brief
*   This function stops the boot logger and dumps the log information. Should
*   be called right before exiting the bootloader 
*
*        
* @par Dependencies
*   Called after sbl1_boot_logger_init()
*   
*/
void sbl1_boot_logger_deinit()
{
  boot_log_message("SBL1, End");

  boot_log_delta_time(sbl_start_time,
                      "SBL1, Delta");

  /* Call boot statistics to add flash entry to boot log. */
  boot_statistics_log_flash();

  /* Add DDR frequency to boot log. */
  boot_log_ddr_frequency();

  boot_log_dump(&boot_log_data);
  
  /*Check if the boot debug scripts instructs to loop
    forever. It would instruct to do so, to enable collecting
    bootup times at end of boot process.
  
    etb_buf_addr is reused as a memory location for infinite loop
    magic number. etb_buf_addr is set by HLOS which does not 
    run in this use case. Hence it can be safely used. 
    boot_shared_imem_cookie_ptr should be set via boot_ram_init.c 
    in sbl2 by now. 
  */
  if(boot_shared_imem_cookie_ptr != NULL)
  {
    boot_log_forever_loop_check(boot_shared_imem_cookie_ptr->etb_buf_addr);
  }
}/* sbl1_boot_logger_deinit */


/*===========================================================================
**  Function :  sbl1_hw_platform_pre_ddr
** ==========================================================================
*/
/*!
* 
* @brief
*     	Parse cdt table to get start of sw-platform id CDB.
*       Call platform-id API and pass pointer to start of sw-platform id CDB.
*       Call DalPlatformInfo_CDTConfigPreDDR to share platform info to rpm
* 
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
*/
void sbl1_hw_platform_pre_ddr(bl_shared_data_type* bl_shared_data)
{
  uint8* platform_id_cdb_ptr = NULL;
  uint32 platform_id_len = 0;
  DalDeviceHandle *phPlatform;
  DALResult eResult;
  char platform_info_buffer[BOOT_LOG_TEMP_BUFFER_SIZE];
  struct cdt_info *cdt_info_ptr = (struct cdt_info *)
                          bl_shared_data->sbl_shared_data->config_data_table_info;

  /*get a pointer to platform id data from configuration data table*/
  platform_id_cdb_ptr = 
              boot_get_config_data_block(cdt_info_ptr->cdt_ptr,
                                         CONFIG_DATA_BLOCK_INDEX_V1_PLATFORM_ID,
                                         &platform_id_len);  
										 
  if(platform_id_cdb_ptr != NULL)
  {
    /* Write platform info out to boot logger. */
    qmemset(platform_info_buffer,
            0,
            BOOT_LOG_TEMP_BUFFER_SIZE);

    snprintf(platform_info_buffer,
             BOOT_LOG_TEMP_BUFFER_SIZE,
             "CDT Version:%d,Platform ID:%d,Major ID:%d,Minor ID:%d,Subtype:%d",
             platform_id_cdb_ptr[0],
             platform_id_cdb_ptr[1],
             platform_id_cdb_ptr[2],
             platform_id_cdb_ptr[3],
             platform_id_cdb_ptr[4]);

    boot_log_message(platform_info_buffer);


    eResult = boot_DAL_DeviceAttachEx(NULL,
                                      DALDEVICEID_PLATFORMINFO,
                                      DALPLATFORMINFO_INTERFACE_VERSION,
                                      &phPlatform);
                                 
    if (eResult == DAL_SUCCESS) 
    {
      /*call the following API to store the platform id*/
      boot_DalPlatformInfo_CDTConfigPreDDR(phPlatform, platform_id_cdb_ptr);
      boot_DAL_DeviceDetach(phPlatform);
    }
  }
  
}/* sbl1_hw_platform_pre_ddr() */


/*===========================================================================
**  Function :  sbl1_hw_platform_smem
** ==========================================================================
*/
/*!
* 
* @brief
*     	Parse cdt table to get start of sw-platform id CDB.
*       Call platform-id API and pass pointer to start of sw-platform id CDB.
*       Call hw_init_smem to store platform id to SMEM
* 
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
*/
void sbl1_hw_platform_smem(bl_shared_data_type* bl_shared_data)
{
  uint8* platform_id_cdb_ptr = NULL;
  uint32 platform_id_len = 0;
  DalDeviceHandle *phPlatform;
  DALResult eResult;
  struct cdt_info *cdt_info_ptr = (struct cdt_info *)
                          bl_shared_data->sbl_shared_data->config_data_table_info;
  /*get a pointer to platform id data from configuration data table*/
  platform_id_cdb_ptr = 
              boot_get_config_data_block(cdt_info_ptr->cdt_ptr,
                                         CONFIG_DATA_BLOCK_INDEX_V1_PLATFORM_ID,
                                         &platform_id_len);  
   
  if(platform_id_cdb_ptr != NULL)
  {
    eResult = boot_DAL_DeviceAttachEx(NULL,
                                      DALDEVICEID_PLATFORMINFO,
                                      DALPLATFORMINFO_INTERFACE_VERSION,
                                      &phPlatform);
                                 
    if (eResult == DAL_SUCCESS) 
    {
      /*call the following API to store the platform id to DAL and SMEM*/
      boot_DalPlatformInfo_CDTConfigPostDDR(phPlatform, platform_id_cdb_ptr);
      boot_DAL_DeviceDetach(phPlatform);
    }
  }
  
}/* sbl1_hw_platform_smem() */

/*===========================================================================
**  Function :  sbl1_update_ddr_info
** ==========================================================================
*/
/*!
* 
* @brief
*   This funcion will get ddr information from ddr driver and put it in 
*   sbl_shared_data. 
* 
* @param[in] bl_shared_data Pointer to shared data
*        
* @par Dependencies
*   Must be called after sbl1_ddr_init
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static void sbl1_update_ddr_info(bl_shared_data_type *bl_shared_data)
{
  static sbl_if_shared_ddr_device_info_type ddr_info;
  DalPlatformInfoPlatformType platform;
  uint8 i;

  // __FIX__SDM845:  substitute non DAL platforminfo api
  platform = boot_DalPlatformInfo_Platform();
  
  ddr_size_info ddr_available = boot_ddr_get_size();
  ddr_size_partition ddr_partition_info = boot_ddr_get_partition();

  if(platform == DALPLATFORMINFO_TYPE_RUMI || platform == DALPLATFORMINFO_TYPE_VIRTIO)
  {
    ddr_available.ddr_cs0[0]      = 0x800;
    ddr_available.ddr_cs0_addr[0] = 0x80000000;
    ddr_available.ddr_cs1[0]      = 0x0; 
    ddr_available.ddr_cs1_addr[0] = 0x0;

    for (i = 1; i < DDR_MAX_NUM_CH; i++)
    {
      ddr_available.ddr_cs0[i]      = 0x0;
      ddr_available.ddr_cs0_addr[i] = 0x0;
      
      ddr_available.ddr_cs1[i]      = 0x0; 
      ddr_available.ddr_cs1_addr[i] = 0x0;
    }

    ddr_partition_info.ddr_cs0[0].num_partitions=0x1;
    ddr_partition_info.ddr_cs0[0].ddr_addr = 0x80000000;
    ddr_partition_info.ddr_cs1[0].num_partitions=0x01;
    ddr_partition_info.ddr_cs1[0].ddr_addr = 0x0;

    for (i = 1; i < DDR_MAX_NUM_CH; i++)
    {
      ddr_partition_info.ddr_cs0[i].num_partitions=0x0;
      ddr_partition_info.ddr_cs0[i].ddr_addr = 0x00000000;
      ddr_partition_info.ddr_cs1[i].num_partitions=0x0;
      ddr_partition_info.ddr_cs1[i].ddr_addr = 0x000000000;	
    }
  }

  boot_share_extended_ddr_info(&ddr_info, &ddr_available, &ddr_partition_info);
  bl_shared_data->sbl_shared_data->ddr_shared_info = &ddr_info;
  boot_set_ddr_info(bl_shared_data);
}


/*===========================================================================
**  Function :  sbl1_update_ddr_page_table
** ==========================================================================
*/
/*!
* 
* @brief
*   This funcion will map all avaliable DDR memory and addtional necessary mappings
*   to the page table in DDR
* 
* @param[in] bl_shared_data Pointer to shared data
*  
* @param[in] mmu_l1_page_table_ptr Pointer to the base of L1 page table that we want
*                                  to update
*
* @par Dependencies
*   Must be called after sbl1_ddr_zi_init
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static void sbl1_update_ddr_page_table
(
  bl_shared_data_type *bl_shared_data, 
  uintnt *mmu_l1_page_table_ptr
)
{
  uint32 ddr_index = 0;
  boot_boolean result = FALSE;
  sbl_if_shared_ddr_device_info_type *ddr_shared_info = NULL;
  sbl_if_shared_ddr_info_type *ddr_info = NULL;

  /* Memory block to descript DDR memory */
  struct mem_block *sbl_ddr_mem_block = &sbl1_ddr_mem_block;
  
  /* Populate the available DDR size info from DDR driver */
  sbl1_update_ddr_info(bl_shared_data);  
  ddr_shared_info = boot_get_ddr_info();

  /*Add all DDR region into the new page table as cacheble*/
  for(; ddr_index < ddr_shared_info->noofddr; ddr_index++)
  {
    ddr_info = &ddr_shared_info->ddr_info[ddr_index];
    
    /*Ensure base address is aligned to 1GB boundary */
    if(ddr_info->cs_addr & (SIZE_1GB_MASK))
    {
      /*Align to upper 1GB boundary */
      sbl_ddr_mem_block->p_base = ((ddr_info->cs_addr & (~SIZE_1GB_MASK)) + SIZE_1GB);
      sbl_ddr_mem_block->size_in_kbytes = (ddr_info->ramsize <<10 ) 
                                         - ((ddr_info->cs_addr & SIZE_1GB_MASK)>> 10 );
    }
    else
    {
      sbl_ddr_mem_block->p_base = ddr_info->cs_addr;
      /* ddr_info->ramsize is in MB */
      sbl_ddr_mem_block->size_in_kbytes = ddr_info->ramsize << 10;      
    }
    sbl_ddr_mem_block->v_base = sbl_ddr_mem_block->p_base;
    result = boot_mmu_page_table_map_single_mem_block
             ((uint64*)mmu_l1_page_table_ptr, sbl_ddr_mem_block);
    
    BL_VERIFY(result,
            BL_ERR_MMU_PGTBL_MAPPING_FAIL|BL_ERROR_GROUP_BOOT);                                         
    
    if(ddr_info->cs_addr & (SIZE_1GB_MASK))
    {
      /*Map lower unaligned address */
      sbl_ddr_mem_block->p_base = ddr_info->cs_addr;
      sbl_ddr_mem_block->v_base = ddr_info->cs_addr;

      /* ddr_info->ramsize is in MB */
      sbl_ddr_mem_block->size_in_kbytes = ((ddr_info->cs_addr & SIZE_1GB_MASK)>> 10 );
    
      result = boot_mmu_page_table_map_single_mem_block
               ((uint64*)mmu_l1_page_table_ptr, sbl_ddr_mem_block);
    
      BL_VERIFY(result,
            BL_ERR_MMU_PGTBL_MAPPING_FAIL|BL_ERROR_GROUP_BOOT);                                         
    } 
  } 

  /* Add the addtional memory map to new page table */
  result = boot_mmu_page_table_map_mem_block_list
           ((uint64*)mmu_l1_page_table_ptr, sbl1_addtional_memory_map);
    
  BL_VERIFY(result,
            BL_ERR_MMU_PGTBL_MAPPING_FAIL|BL_ERROR_GROUP_BOOT); 

  /* Invalidate the TLB after map is updated */
  mmu_invalidate_tlb();
}

/*===========================================================================
**  Function :  sbl1_move_boot_log_to_ddr
** ==========================================================================
*/
/*!
* 
* @brief
*   This funcion will Move existing boot log in sbl1 image region to DDR
*
* @par Dependencies
*   Must be called after sbl1_ddr_init
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static void sbl1_move_boot_log_to_ddr()
{
  /* Relocate boot logger buffer to DDR and continue to save log in DDR */
  boot_log_relocate((void *)&Image$$SBL1_DDR_LOG_META_INFO_SECTION$$Base,
                    (void *)&Image$$SBL1_DDR_LOG_BUF_SECTION$$Base,
                    SCL_SBL1_DDR_BOOT_LOG_BUF_SIZE,  
                    (void *)&Image$$SBL1_DDR_TIME_MARKER_BUF_SECTION$$Base,
                    SCL_SBL1_DDR_BOOT_TIME_MARKER_BUF_SIZE,
                    (void *)&Image$$SBL1_LOG_META_INFO_SECTION$$Base,
                    (void *)&Image$$SBL1_LOG_BUF_SECTION$$Base,
                     SCL_SBL1_BOOT_LOG_BUF_SIZE,
                    (void *)&Image$$SBL1_TIME_MARKER_BUF_SECTION$$Base,
                    SCL_SBL1_BOOT_TIME_MARKER_BUF_SIZE);
  
  boot_log_data.log_buffer_start = (void *)&Image$$SBL1_DDR_LOG_BUF_SECTION$$Base;
  boot_log_data.log_buffer_size = SCL_SBL1_DDR_BOOT_LOG_BUF_SIZE;
  boot_log_data.time_marker_buffer_start = (void *)&Image$$SBL1_DDR_TIME_MARKER_BUF_SECTION$$Base;
  boot_log_data.time_marker_buffer_size = SCL_SBL1_DDR_BOOT_TIME_MARKER_BUF_SIZE;
  boot_log_data.meta_info_start = (void *)&Image$$SBL1_DDR_LOG_META_INFO_SECTION$$Base;
  
  /* Set Boot Logger Location and size information in shared_imem for use 
     in next stage of boot process */
  boot_shared_imem_cookie_ptr->boot_log_addr = (UINT64)boot_log_data.log_buffer_start;
  boot_shared_imem_cookie_ptr->boot_log_size = boot_log_data.log_buffer_size;
}


/*===========================================================================
**  Function :  sbl1_ddr_zi_init
** ==========================================================================
*/
/*!
* 
* @brief
*   This funcion will zero initialize SBL1's ZI and Page Table in DDR
*   sbl_shared_data. 
* 
* @param[in] bl_shared_data Pointer to shared data
*        
* @par Dependencies
*   Must be called after sbl1_ddr_init
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static void sbl1_ddr_zi_init()
{
  /* Zero out the SBL1 DDR ZI */
  boot_ram_init(&sbl1_ram_init_data_ddr);

  /* Zero out the SBL UNCACHED DDR ZI region */
  boot_ram_init(&sbl1_ram_init_data_uncached_zi);
}


/*===========================================================================
**  Function :  sbl1_relocate_page_table_to_ddr
** ==========================================================================
*/
/*!
* 
* @brief
*   This funcion will relocate page table from imem to DDR and map all avaliable
*   DDR memory to page table
* 
* @param[in] bl_shared_data Pointer to shared data
*  
* @par Dependencies
*   Must be called after sbl1_ddr_init
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_relocate_page_table_to_ddr(bl_shared_data_type *bl_shared_data)
{
  if (page_table_is_in_ddr == FALSE)
  {
    boot_boolean result = FALSE;
    
    /* Firt Zero out the SBL1 DDR Page table region */
    qmemset((void *)SCL_SBL1_DDR_PAGE_TABLE_BASE, 0x0, SCL_SBL1_DDR_PAGE_TABLE_SIZE);
    
    /* Copy existing L1 and L2 page table from IMEM to DDR */
    result = boot_mmu_copy_page_table();
    BL_VERIFY(result , BL_ERR_MMU_PGTBL_CPY_FAIL|BL_ERROR_GROUP_BOOT);
    
    /*Add all necessary mappings to the DDR page table*/
    sbl1_update_ddr_page_table(bl_shared_data, (uintnt *)mmu_l1_ddr_pagetable);
    
    /* Relocate the page table base register, new page table will take effect after this call */
    mmu_relocate_page_table_base(mmu_l1_ddr_pagetable);
    
    /* Update the global page table base pointer */
    bl_shared_data->sbl_shared_data->mmu_l1_pagetable_base = (uint32 *)mmu_l1_ddr_pagetable;
    page_table_is_in_ddr = TRUE;
    
    /* Add the DDR heap to DAL */
    boot_DALSYS_HeapInit(boot_external_heap, BOOT_EXTERNAL_HEAP_SIZE, TRUE);

    /* Move boot log to DDR */    
    sbl1_move_boot_log_to_ddr();

  }
  
}


/*===========================================================================
**  Function :  sbl1_post_ddr_init
** ==========================================================================
*/
/*!
* 
* @brief
*   This funcion will initialize the data section in DDR and relocate boot log to DDR
*   After execution of this function SBL1 will enter Sahara in stead of PBL
*   emergency dload mode in case of error
*
* @par Dependencies
*   Must be called after sbl1_ddr_init
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_post_ddr_init(bl_shared_data_type *bl_shared_data)
{
  struct mem_block pimem_mem_block;
  
  boot_ddr_test(boot_clock_get_ddr_speed());

  /* Configure Pimem memory region */
  pimem_mem_block.p_base = SCL_pIMEM_BASE;
  pimem_mem_block.v_base = SCL_pIMEM_BASE;
  pimem_mem_block.size_in_kbytes = SCL_pIMEM_SIZE >> 10;
  pimem_mem_block.memory_mapping = MMU_L2_SECTION_MAPPING;
  pimem_mem_block.access = MMU_PAGETABLE_MEM_READ_WRITE;
  pimem_mem_block.executable = MMU_PAGETABLE_NON_EXECUTABLE_REGION;
  /* update cache attribute to WB */
  pimem_mem_block.cachetype = MMU_PAGETABLE_MEM_WRITE_BACK_CACHE;
  
  boot_log_message("pImem Init Start");
  boot_log_start_timer();

  /* Initialize Pimem by calling into XBL SEC */
  boot_fastcall_tz_no_rsp (TZ_PIMEM_INIT_CMD,
                           TZ_PIMEM_INIT_CMD_PARAM_ID,
                           0,0,0,0);
  boot_log_stop_timer("pImem Init End, Delta");

  /* Initialize DDR ZI region */
  sbl1_ddr_zi_init();

  sbl1_relocate_page_table_to_ddr(bl_shared_data);  
 
  if (edload_flag != TRUE)
  {
    /* Update the dload entry to sbl1 sahara dload entry function */
    sbl_dload_entry = sbl1_dload_entry;
  }	
}

/*===========================================================================
**  Function :  sbl_error_handler
** ==========================================================================
*/
/*!
* 
* @brief
*   This saves registers and fills up the information
*   in the boot crash information for simulator tool
*
* @param[in] None
*        
* @par Dependencies
*  none
*   
*/
void sbl_error_handler(void)
{

}

/*===========================================================================
**  Function :  sbl_save_regs
** ==========================================================================
*/
/*!
* 
* @brief
*   Save current registers at the time of crash
*
* @par Dependencies
*  None        
*   
*/
void sbl_save_regs(void)
{
}

/*===========================================================================
**  Function :  sbl1_post_ddr_training_init
** ==========================================================================
*/
/*!
* 
* @brief
*   This funcion will initialize the data section in DDR, populate ddr size information
*   then relocate page table from IMEM to DDR and map all memory regions required by SBL1 
*   in page table.
*
* @par Dependencies
*   Must be called after sbl1_ddr_init
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_post_ddr_training_init(bl_shared_data_type *bl_shared_data)
{
  if (boot_ddr_params_is_training_required() == TRUE)
  {
    /* Reinitialize DDR ZI region because DDR training may corrupt DDR*/
    sbl1_ddr_zi_init();
  
    /* Relocate page table to DDR */
    sbl1_relocate_page_table_to_ddr(bl_shared_data);
  }
}


/*===========================================================================
**  Function :  sbl1_load_ddr_training_data_from_partition
** ==========================================================================
*/
/*!
* 
* @brief
*   This funcion will read the ddr training data from ddr parition
*
* @par Dependencies
*   Must be called before sbl1_ddr_init
*   
* @retval
*   pointer to ddr training data
* 
* @par Side Effects
*   None
* 
*/
void sbl1_load_ddr_training_data_from_partition
(
  uint8* ddr_training_data_buf,
  uint32 ddr_training_data_size
)
{
  boot_boolean success = FALSE;
  boot_flash_trans_if_type *trans_if = NULL;
  
  /* Verify ddr training data size is valid */
  BL_VERIFY((ddr_training_data_size != 0) && 
            (ddr_training_data_size <= SCL_DDR_TRAINING_DATA_BUF_SIZE),
            BL_ERR_DDR_TRAINING_DATA_SIZE_INVALID|BL_ERROR_GROUP_DDR);
            
  boot_flash_configure_target_image(ddr_params_partition_id);

  trans_if = boot_flash_dev_open_image(GEN_IMG);

  BL_VERIFY( trans_if != NULL, BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );
   
  /* Read the ddr training data out, store it in the buffer */
  success = boot_flash_trans_read(trans_if,
                                  ddr_training_data_buf,
                                  0,
                                  ddr_training_data_size,
                                  IMAGE_BODY_TYPE);
                                  
  BL_VERIFY(success, BL_ERR_FLASH_READ_FAIL|BL_ERROR_GROUP_BOOT);
  
  /* close parition */
  boot_flash_dev_close_image(&trans_if);
}

/*===========================================================================
**  Function :  sbl1_read_from_ddr_partition
** ==========================================================================
*/
/*!
* 
* @brief
*   This funcion will read the ddi data from ddr parition
*
* @par Dependencies
*   
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_read_from_ddr_partition
(
  uint32 data_offset,
  uint8* out_buf,
  uint32 out_buf_size
)
{
  boot_boolean success = FALSE;
  boot_flash_trans_if_type *trans_if = NULL;
  whitelst_tbl_entry_type data_white[] = DDI_DATA_WHITELIST;
  
  BL_VERIFY((out_buf_size != 0),BL_ERR_DDI_SIZE_INVALID|BL_ERROR_GROUP_DDR);
            
  boot_flash_configure_target_image(ddr_params_partition_id);

  trans_if = boot_flash_dev_open_image(GEN_IMG);

  BL_VERIFY( trans_if != NULL, BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );
  
  if(!(boot_set_active_img_whitelist_table(data_white)))
  {
    BL_ERR_FATAL(BL_ERR_IMG_SECURITY_FAIL);
  }
  
  /* Read the data to the destination address */
  success = boot_flash_trans_read(trans_if,
                                  (void *) (uintnt)out_buf,
                                  data_offset,
                                  out_buf_size,
                                  IMAGE_BODY_TYPE);

  BL_VERIFY(success, BL_ERR_FLASH_READ_FAIL|BL_ERROR_GROUP_BOOT);
  
  boot_invalidate_img_whitelist();
  
  /* close parition */
  boot_flash_dev_close_image(&trans_if);
}

/*===========================================================================
**  Function :  sbl1_load_sec_partition
** ==========================================================================
*/
/*!
* 
* @brief
*   This funcion will read the sec partition and load it to a defined memory 
*   region
*
* @par Dependencies
*   Must be called after TZ is loaded
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_load_sec_partition ( bl_shared_data_type *bl_shared_data )
{
  boot_boolean success = FALSE;
  boot_flash_trans_if_type *trans_if = NULL;
  uint32 secdat_buffer_location = 0;
  uint32 bytes_read = 0;
  whitelst_tbl_entry_type sec_dat_whitelist[] = SEC_DAT_WHITELIST;

  if(boot_dload_is_dload_mode_set() == FALSE)
  {
    /*Load sec image only if not in dload mode */
    
    /* Reset the flash statistics byte counter so the number of bytes read
       from flash can be read later. */
    boot_statistics_reset_flash_byte_counter();
   
    boot_log_message("Image Load, Start");
    boot_log_start_timer();
   
    secdat_buffer_location = TZBSP_SECDAT_BUFFER_BASE;
        
    boot_flash_configure_target_image(sec_partition_id);
   
    trans_if = boot_flash_dev_open_image(GEN_IMG);
   
    /* Partition is present, then load it to memory */
    if (trans_if != NULL) 
    {
       /* Verify destination address and size */
       BL_VERIFY(TZBSP_SECDAT_BUFFER_SIZE &&
                 secdat_buffer_location,
               BL_ERR_INVALID_SECDAT_BUFFER|BL_ERROR_GROUP_BOOT);    
      
      if(!(boot_set_active_img_whitelist_table(sec_dat_whitelist)))
      {
        BL_ERR_FATAL(BL_ERR_IMG_SECURITY_FAIL);
      }  
      
       /* Read the SEC data to the destination address */
       success = boot_flash_trans_read(trans_if,
                                      (void *) (uintnt)secdat_buffer_location,
                                       0,
                                       TZBSP_SECDAT_BUFFER_SIZE,
                                       IMAGE_BODY_TYPE);
      
     BL_VERIFY(success, BL_ERR_FLASH_READ_FAIL|BL_ERROR_GROUP_BOOT);
   
       /* Close parition */
       boot_flash_dev_close_image(&trans_if);
    }
   
   
    /* Retreive number of bytes read from flash via boot statistics. */
    bytes_read = boot_statistics_get_flash_byte_counter();
   
    /* Format bytes read value into string. */
    snprintf(bootlog_buffer,
             BOOT_LOG_TEMP_BUFFER_SIZE,
             "(%d Bytes)",
             bytes_read);
   
    boot_log_stop_timer_optional_data("SEC Image Loaded, Delta",
                                    bootlog_buffer);
  }									
}


/*===========================================================================
**  Function :  sbl1_save_ddr_training_data_to_partition
** ==========================================================================
*/
/*!
* 
* @brief
*   This funcion will save ddr training data to ddr partition
* 
* @param[in] ddr_data_ptr Pointer to ddr data
*
* @param[in] ddr_data_size size of ddr data to be written
*     
* @par Dependencies
*   Must be called after ddr_post_init
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_save_data_to_ddr_partition
(
  uint8* ddr_partition_data_ptr,
  uint32 ddr_partition_data_size,
  uint32 ddr_partition_data_offset
)
{
  boot_flash_trans_if_type *trans_if = NULL;
  boot_boolean success = FALSE;
  
  /* Write DDR training data to storage device */
  boot_flash_configure_target_image(ddr_params_partition_id);

  trans_if = boot_flash_dev_open_image(GEN_IMG);

  BL_VERIFY( trans_if != NULL, BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );

  success = dev_sdcc_write_bytes(ddr_partition_data_ptr,
                                 ddr_partition_data_offset,
                                 ddr_partition_data_size,
                                 GEN_IMG);

  BL_VERIFY(success, BL_ERR_SDCC_WRITE_FAIL|BL_ERROR_GROUP_BOOT);

  /* close parition*/
  boot_flash_dev_close_image(&trans_if);
}

/*===========================================================================
**  Function :  sbl1_save_ddr_training_data_to_partition
** ==========================================================================
*/
/*!
* 
* @brief
*   This funcion will save ddr training data to ddr partition
* 
* @param[in] ddr_training_data_ptr Pointer to ddr training data
*
* @param[in] ddr_training_data_size size of ddr training data
*     
* @par Dependencies
*   Must be called after ddr_post_init
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_save_ddr_training_data_to_partition
(
  uint8* ddr_training_data_ptr,
  uint32 ddr_training_data_size,
  uint32 ddr_training_data_offset
)
{

  sbl1_save_data_to_ddr_partition(ddr_training_data_ptr, ddr_training_data_size, ddr_training_data_offset);
  
}


/*===========================================================================
**  Function :  sbl1_efs_handle_cookies
** ==========================================================================
*/
/*!
* 
* @brief
*   Calls efs cookie handling api to perform efs backup/restore action
* 
* @param[in] bl_shared_data Pointer to shared data
*   
* @par Dependencies
*   None
*   
* @retval
*   None                                                               
* 
* @par Side Effects
*   None
* 
*/
void sbl1_efs_handle_cookies(bl_shared_data_type* bl_shared_data)
{
  boot_efs_handle_cookies();
}/* sbl1_efs_handle_cookies */

/*===========================================================================
**  Function :  sbl1_pagetable_add_MMU_entry
** ==========================================================================
*/
/*!
* 
* @brief
*   Configure the page table to contain an entry for itself 
* 
* @param[in] bl_shared_data Pointer to shared data
*   
* @par Dependencies
*   None
*   
* @retval
*   None                                                               
* 
* @par Side Effects
*   None
* 
*/
static void sbl1_pagetable_add_MMU_entry(bl_shared_data_type* bl_shared_data)
{
  boot_boolean result;  
  uintnt pt_base = (uintnt)SCL_SBL1_PAGE_TABLE_BASE;
  uintnt pt_size = (uintnt)SCL_SBL1_PAGE_TABLE_SIZE;
  
  struct mem_block *sbl_mmu_page_table_mem_block = &sbl1_mmu_page_table_mem_block;

  /* No free L2 and L3 page table entries are available in pagetable memory region
    shared by bootrom. Hence allocate a seperate memory region to hold future new L2/L3
    page tables, till DDR is initialized
    */
  sbl_mmu_page_table_mem_block->p_base = pt_base;
  sbl_mmu_page_table_mem_block->v_base = pt_base;
  
  sbl_mmu_page_table_mem_block->size_in_kbytes = 
    (pt_size) >> 10;

  /* Now populate the sbl page table memory map block */
  result =
    boot_mmu_page_table_map_single_mem_block
    ((uint64*)mmu_get_page_table_base(), sbl_mmu_page_table_mem_block);     
    
  BL_VERIFY(result , BL_ERR_MMU_PGTBL_MAPPING_FAIL|BL_ERROR_GROUP_BOOT);
}

/*===========================================================================
**  Function :  sbl1_populate_initial_mem_map
** ==========================================================================
*/
/*!
* 
* @brief
*   populate the existing pbl page table with SBL memory mapping
* 
* @param[in] bl_shared_data Pointer to shared data
*   
* @par Dependencies
*   None
*   
* @retval
*   None                                                               
* 
* @par Side Effects
*   None
* 
*/
void sbl1_populate_initial_mem_map(bl_shared_data_type* bl_shared_data)
{
  boot_boolean result;
  uintnt baseaddr = 0x0;
  xbl_internal_mem_region_type sbl_imem_ranges[]=SBL_IMEM_RANGES_TABLE;
  /* OCIMEM TZ regions 256KB.*/
  /* marked as non-executable device shared readwrite.
    This covers marking xpu protected
    XBL SEC and QSEE sections as device shared */
  
  struct mem_block qsee_ocimem_region = 
  {
    SCL_IMEM_BASE,
    SCL_IMEM_BASE,
    SCL_TZ_OCIMEM_BUFFER >> 10,
    MMU_L3_SECTION_MAPPING,
    MMU_PAGETABLE_MEM_READ_WRITE, 
    MMU_PAGETABLE_DEVICE_SHARED,
    MMU_PAGETABLE_NON_EXECUTABLE_REGION
  }; 

#ifdef FEATURE_DEVICEPROGRAMMER_IMAGE
  struct mem_block deviceprog_uncached_zi_region = 
  {
    (uint32)Image$$SBL1_DEVPROG_UNCACHED_ZI$$Base,
    (uint32)Image$$SBL1_DEVPROG_UNCACHED_ZI$$Base,
    ((uint32)Image$$SBL1_DEVPROG_UNCACHED_ZI$$ZI$$Length) >> 10,
    MMU_L3_SECTION_MAPPING,
    MMU_PAGETABLE_MEM_READ_WRITE, 
    MMU_PAGETABLE_MEM_NON_CACHEABLE,
    MMU_PAGETABLE_NON_EXECUTABLE_REGION
  }; 	
#endif
  
  BL_VERIFY(bl_shared_data->
              sbl_shared_data->
              pbl_shared_data->
              pbl_page_tbl_base != NULL,
              BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT);
  
  /*
    zero out L1 page table entries for unused address space
    spanning across 64GB. each L1 page table entry (mapping 1GB) is 8 bytes
    bootrom maps first GB, ie only first 8 bytes are used. ensure remaining
    (63 gb * 8bytes) are zero
  */
  
  baseaddr = ((uintnt)mmu_get_page_table_base() + 0x8);
  qmemset((void *)baseaddr, 0x0,(8 * 63));
  
  /* invalidate TLB before disabling MMU*/
  mmu_invalidate_tlb();
  /* Flush internal memory regions and disable cache*/
  boot_flush_regions_disable_dcache(sbl_imem_ranges,sizeof(sbl_imem_ranges)/sizeof(xbl_internal_mem_region_type));
  /* Disable MMU before making any changes to MMU and the attributes*/
  boot_disable_mmu();

  
  /* First populate the initial memory map list. Bootrom is assumed
    to have configured the mmu 
  */

  sbl1_pagetable_add_MMU_entry(bl_shared_data);
  
  //check why devprg is breaking with updating pbl mappings
 // result = boot_mmu_page_table_map_mem_block_list((uint64*)mmu_get_page_table_base(), update_regions_mapped_by_pbl);
  
 // BL_VERIFY(result , BL_ERR_MMU_PGTBL_MAPPING_FAIL|BL_ERROR_GROUP_BOOT);
  
  if(boot_dload_is_dload_mode_set())
  {
   /* OCIMEM TZ regions 256KB.*/
  result = boot_mmu_page_table_map_single_mem_block
          ((uint64*)mmu_get_page_table_base(), &qsee_ocimem_region);
 
  BL_VERIFY(result , BL_ERR_MMU_PGTBL_MAPPING_FAIL|BL_ERROR_GROUP_BOOT); 
  }
  else
  {
  	/* unmap TZ SDI region mapped by PBL or XBL_SEC to avoid speculative accesses */
  	result = boot_mmu_find_entry_and_unmap((uintnt*)mmu_get_page_table_base(),SCL_TZ_SDI_MAPPED_REGION,SCL_TZ_SDI_MAPPED_REGION_SIZE);
    //check why devprg is breaking when unmapping PBL region
   // result |= boot_mmu_find_entry_and_unmap((uintnt*)mmu_get_page_table_base(),SCL_PBL_REGION_DEFAULT_MAPPED,SCL_PBL_REGION_DEFAULT_MAPPED_SIZE);
    
    result |= boot_mmu_find_entry_and_unmap((uintnt*)mmu_get_page_table_base(),
              SCL_UNUSED_ADDRESS1_BASE,SCL_UNUSED_ADDRESS1_SIZE);
    
    BL_VERIFY(result , BL_ERR_MMU_PGTBL_MAPPING_FAIL|BL_ERROR_GROUP_BOOT);
  }
#ifdef FEATURE_DEVICEPROGRAMMER_IMAGE
    /* OCIMEM TZ regions 256KB.*/
  result = boot_mmu_page_table_map_single_mem_block
          ((uint64*)mmu_get_page_table_base(), &deviceprog_uncached_zi_region);
 
  BL_VERIFY(result , BL_ERR_MMU_PGTBL_MAPPING_FAIL|BL_ERROR_GROUP_BOOT); 
#endif
  result =
      boot_mmu_page_table_map_mem_block_list
       ((uint64*)mmu_get_page_table_base(), sbl_initial_memory_map);
  
    BL_VERIFY(result , BL_ERR_MMU_PGTBL_MAPPING_FAIL|BL_ERROR_GROUP_BOOT);  
 
  mmu_reconfigure();
  boot_enable_mmu();
  mmu_invalidate_tlb();
  boot_enable_dcache();
}

/*===========================================================================
**  Function :  sbl1_tlmm_init
** ==========================================================================
*/
/*!
* 
* @brief
*   SBL1 wrapper to Initialize Tlmm and gpio for low power config
*
* @param[in] shared_data Pointer to shared data
*        
* @par Dependencies
*   must be called after boot_smem_init
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_tlmm_init(bl_shared_data_type *bl_shared_data)
{
  /* Initialize Tlmm and gpio for low power config ,must be done after smem init*/
  if (FALSE == boot_gpio_init())
  {
   BL_VERIFY(FALSE, BL_ERR_INIT_GPIO_FOR_TLMM_CONFIG_FAIL|BL_ERROR_GROUP_BOOT);
  }  
}


/*===========================================================================
**  Function :  sbl1_init_qsee_interface
** ==========================================================================
*/
/*!
* 
* @brief
*   Zero out the SBL - QSEE interface and populate with SBL sec info.
*
* @param[in]
*   bl_shared_data           Pointer to shared data
*   sbl1_auth_verified_info  Pointer to auth data provided by PBL
*        
* @par Dependencies
*   None
*   
*/
void sbl1_init_sbl_qsee_interface(bl_shared_data_type *bl_shared_data,
                                  secboot_verified_info_type *sbl1_auth_verified_info)
{
  // Zero out the interface
  qmemset(&(bl_shared_data->sbl_qsee_interface),
          0x0,
          sizeof(boot_sbl_qsee_interface));

  // Assign the magic numbers and version
  bl_shared_data->sbl_qsee_interface.magic_1 = SBL_QSEE_MAGIC_NUMBER_1;
  bl_shared_data->sbl_qsee_interface.magic_2 = SBL_QSEE_MAGIC_NUMBER_2;
  bl_shared_data->sbl_qsee_interface.version = SBL_QSEE_INTERFACE_VERSION;  // version 4
  bl_shared_data->sbl_qsee_interface.reset_required = (uint32) NO_RESET;

  /* Api to put DDR in self refresh */
  bl_shared_data->sbl_qsee_interface.ddr_enter_self_refresh = (uint64)boot_ddr_enter_self_refresh;

  /* Api to bring DDR out of self refresh */
  bl_shared_data->sbl_qsee_interface.ddr_exit_self_refresh = (uint64)boot_ddr_exit_self_refresh;
           
  /* Update QSEE interface with SBL auth information provided by PBL */
  qmemcpy(&(bl_shared_data->sbl_qsee_interface.boot_image_entry[0].image_verified_info),
          sbl1_auth_verified_info,
          sizeof(secboot_verified_info_type));
  /*
    sbl_qsee_interface.appsbl_entry_index is populated at the very end after
    loading appsbl image.
  */

  /* Update SBL entry with information not passed by PBL */
  bl_shared_data->sbl_qsee_interface.boot_image_entry[0].image_id = SECBOOT_SBL_SW_TYPE;
  bl_shared_data->sbl_qsee_interface.boot_image_entry[0].entry_point = SCL_SBL1_VECTOR_BASE;
  bl_shared_data->sbl_qsee_interface.number_images++;
}


/*===========================================================================
**  Function :  sbl1_appsbl_arch_determination
** ==========================================================================
*/
/*!
* 
* @brief
*   Determines APPSBL's architecture and updates the SBL - QSEE interface.
*   This is needed because Loader and Core are merged into a single ELF.
*   I.E. 64bit Loader with 32bit APPSBL in a 64bit ELF misrepresents APPSBL.
*
* @param[in]
*   bl_shared_data           Pointer to shared data
*        
* @par Dependencies
*   None
*   
*/
void sbl1_appsbl_arch_determination(bl_shared_data_type *bl_shared_data)
{
  int32 appsbl_entry_index = -1;
  uintnt entry_point = 0;
  boot_arch_mode_type arch_type = BOOT_ARCH_MODE_UNKNOWN;
  boot_images_entry * boot_images =
    bl_shared_data->sbl_qsee_interface.boot_image_entry;


  /* Obtain APPSBL entry in SBL - QSEE interface  Because APPSBL can be
     a merged or standalone image a special api must be used to locate
     it. */
  appsbl_entry_index =
    boot_sbl_qsee_interface_get_image_entry(&(bl_shared_data->sbl_qsee_interface), SECBOOT_APPSBL_SW_TYPE);

  BL_VERIFY(appsbl_entry_index != -1,
            BL_ERR_IMG_NOT_FOUND|BL_ERROR_GROUP_BOOT);


  /* Obtain APPSBL entry point. */
  entry_point =
    boot_images[appsbl_entry_index].entry_point;

  BL_VERIFY(entry_point != 0,
            BL_ERR_INVALID_ENTRY_POINT|BL_ERROR_GROUP_BOOT);


  /* Determine the architecture by the first instruction of the image which
     is almost always a branch. */
  arch_type = boot_arch_determination_by_branch((uint32 *)entry_point);

  BL_VERIFY(arch_type != BOOT_ARCH_MODE_UNKNOWN,
            BL_ERR_IMAGE_ARCH_UNKNOWN|BL_ERROR_GROUP_BOOT);


  /* Update the SBL - QSEE interface to the proper e_ident. */
  if (arch_type == BOOT_ARCH_MODE_AARCH64)
  {
    boot_images[appsbl_entry_index].e_ident = ELF_CLASS_64;
  }
  else
  {
    boot_images[appsbl_entry_index].e_ident = ELF_CLASS_32;
  }
}


/*===========================================================================

**  Function :  sbl1_get_shared_data

** ==========================================================================
*/
/*!
*
* @brief
*   This function returns the pointer to sbl1 shared data
*
* @par Dependencies
*   None
*
* @retval
*   bl_shared_data_type *
*
* @par Side Effects
*   None
*
*/
bl_shared_data_type * sbl1_get_shared_data( void )
{
   return &bl_shared_data;
}  /* sbl1_get_shared_data */


/*===========================================================================

**  Function :  sbl1_get_external_heap_addr

** ==========================================================================
*/
/*!
*
* @brief
*   This function returns external heap address
*
* @par Dependencies
*   None
*
* @retval
*   byte *
*
* @par Side Effects
*   None
*
*/

byte* sbl1_get_external_heap_addr(void)
{
  return boot_external_heap;
}

/*===========================================================================

**  Function :  sbl1_get_external_heap_size

** ==========================================================================
*/
/*!
*
* @brief
*   This function returns external heap size
*
* @par Dependencies
*   None
*
* @retval
*   uint32
*
* @par Side Effects
*   None
*
*/
uint32 sbl1_get_external_heap_size(void)
{
  return BOOT_EXTERNAL_HEAP_SIZE;
}
/*===========================================================================
**  Function :  sbl1_main_ctl
** ==========================================================================
*/
/*!
* 
* @brief
*   The  Main Controller performs the following functions:
*       - Initializes ram
*       - And so on...
* 
* @param[in] pbl_shared Pointer to shared data
*  
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   This function never returns.
* 
*/
void sbl1_main_ctl(boot_pbl_shared_data_type *pbl_shared)
{
  DALResult bsy_wait_init;
  
#ifdef FEATURE_DEVICEPROGRAMMER_IMAGE  
  xblconfig_status_type status;   
#endif   

 /* Calculate the SBL start time for use during boot logger initialization. */
  sbl_start_time = CALCULATE_TIMESTAMP(HWIO_IN(TIMETICK_CLK));

  /*Configure Domain access control register */
  mmu_set_dacr(DACR_ALL_DOMAIN_CLIENTS);

  /* Initialize busywait module
     Note: This is required to be done before logger init due to uart driver
     dependency on busywait */
  BL_VERIFY((bsy_wait_init=boot_busywait_init()) == DAL_SUCCESS, (uint16)bsy_wait_init|BL_ERROR_GROUP_BUSYWAIT);
  
  /* Enable qdss workaround*/
  BL_VERIFY(boot_clock_debug_init() == TRUE, FALSE|BL_ERROR_GROUP_CLK );
  
  /* Enter debug mode if debug cookie is set */
  sbl1_debug_mode_enter();
  
  /* Initialize the stack protection canary */
  boot_init_stack_chk_canary();
  
  /* Initialize boot shared imem */
  boot_shared_imem_init(&bl_shared_data);

  /* Zero out the SBL1 OCIMEM ZI */
  boot_ram_init(&sbl1_ram_init_data_ocimem);

  if (PcdGet32 (PcdBuildType) != SBL_BUILD_SBL)
  {
    /* ZI DeviceProgrammer OCIMEM because PBL doesn't do it for us. */
    boot_ram_init(&sbl1_ram_init_data_ocimem_devprog);
  }
  
  /* Explicitly flush the ZI region, in case PBL placed us in WB cache. */
  dcache_flush_region((void *)SCL_SBL1_OCIMEM_DATA_BASE, SCL_SBL1_OCIMEM_DATA_SIZE);  

  /* Initialize the ChipInfo driver */
  ChipInfo_Init();

  /* Retrieve info passed from PBL*/
  sbl1_retrieve_shared_info_from_pbl(pbl_shared);

  /* Initialize the QSEE interface */
  sbl1_init_sbl_qsee_interface(&bl_shared_data,
                               &sbl_verified_info);

  /* Initialize the shared functions structure.
     This provides other images with function pointers inside of Loader. */
  boot_shared_functions_register();

  /* Initialize SBL memory map.
     Initializing early because drivers could be located in RPM Code RAM. */
  sbl1_populate_initial_mem_map(&bl_shared_data);

  /* Initialize boot logger and start the log timer.
     This must be done after sbl1_retrieve_shared_info_from_pbl
     and boot_secboot_ftbl_init. */
  sbl1_boot_logger_init(&boot_log_data,
                        pbl_shared);
  boot_log_set_meta_info(boot_log_data.meta_info_start);
  
  /* set hash algorithm */
  BL_VERIFY(boot_set_hash_algo(SBL_HASH_SHA256) == BL_ERR_NONE,
            BL_ERR_UNSUPPORTED_HASH_ALGO|BL_ERROR_GROUP_BOOT);
  
  /* Initialize dal heap using internal memory */
  boot_DALSYS_HeapInit(boot_internal_heap,
                       BOOT_INTERNAL_HEAP_SIZE,
                       FALSE);
  
  /*Initialize DAL, needs to be called before modules that uses DAL */  
  boot_DALSYS_InitMod(NULL); 
  
  /* Call sbl1_hw_init to config pmic device so we can use PS_HOLD to reset */
  sbl1_hw_init();
  

#ifdef FEATURE_DEVICEPROGRAMMER_IMAGE
  /*---------------------------------------------------------------------
    Intialize XBLconfig ramlib for usecases where xblconfig segments are
    part of image and loaded along with it to ram, as in deviceprogrammer
    usecase
   ----------------------------------------------------------------------*/
   /*Initialize XBL config ramlib */
  status = xblconfig_ramlib_init( 
             bl_shared_data.sbl_shared_data->pbl_shared_data->elf_meta_info.elf_hdr,
             bl_shared_data.sbl_shared_data->pbl_shared_data->elf_meta_info.prog_hdr,
             (void *)bl_shared_data.sbl_shared_data->pbl_shared_data->elf_meta_info.hash_seg_hdr
           );
  BL_VERIFY(status == XBLCONFIG_SUCCESS, 
            BL_ERR_IMG_NOT_FOUND|BL_ERROR_GROUP_BOOT); 


            
  /*----------------------------------------------------------------------
   When compiling for Deviceprogrammer-DDR the function below calls the 
   pre-proc and post-proc function arrays for each image-entry in the 
   boot_config_table, leading up to the eventual call of 
   boot_hand_control_to_deviceprogrammer_DDR_main().  This ensures that 
   even if images are added or removed from boot_config_table[], or the
   ordering of those functions in pre-procs/post-procs arrays changes, 
   execution leading to boot_hand_control_to_deviceprogrammer_DDR_main()
   for Deviceprogrammer_DDR image will be identical to that of SBL image
   execution, except for the fact that elf-image loading support is not
   compiled in.
  ----------------------------------------------------------------------*/
  if(bl_shared_data.build_type == SBL_BUILD_DEVICEPROGRAMMER_DDR )
  {
    /* Save reset register logs */
    boot_save_reset_register_log(&bl_shared_data);
  
    /* Initialize the flash device */
    boot_flash_init(&bl_shared_data);
  
    /* Initialize XBL config Lib */
    sbl1_xblconfig_init(&bl_shared_data);    
    
    boot_handoff_to_devprogDDR_or_DDRDebugImage(&bl_shared_data);
  } 
  else if(bl_shared_data.build_type == SBL_BUILD_DEVICEPROGRAMMER_LITE )
  {
/*----------------------------------------------------------------------
   In SBL builds the function below is stubbed out (does nothing).
   When building Deviceprogrammer the function below never returns.
   The function below becomes active by defining the feature
   FEATURE_BOOT_EXTERN_DEVICEPROGRAMMER_LITE_COMPLETED.
  ---------------------------------------------------------------------*/
    boot_log_message("Entering DeviceProg lite");  
    boot_hand_control_to_deviceprogrammer_lite_main (pbl_shared);
  }
#else                                             

  /* Save reset register logs */
  boot_save_reset_register_log(&bl_shared_data);
  
  /* Initialize the flash device */
  boot_flash_init(&bl_shared_data);
  
  /* Initialize XBL config Lib */
  sbl1_xblconfig_init(&bl_shared_data);
  /*-----------------------------------------------------------------------
    Process the target-dependent SBL1 procedures
  -----------------------------------------------------------------------*/
  boot_config_process_bl(&bl_shared_data);
#endif

} /* sbl1_main_ctl() */
