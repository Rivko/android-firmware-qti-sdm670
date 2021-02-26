/**
 * @file sbl1_config.c
 * @brief
 * Source file for 8996 SBL1 bootloader configuration
 *
 * DEFINED VARIABLES:
 * sbl1_config_table   - Boot configuration entries for SBL1
 * load_qsee_pre_procs - SBL1 procedures run prior to QSEE loading
 *
 * Copyright (C) 2011-2017, 2018 by Qualcomm Technologies, Inc. All Rights Reserved.
 *
 */

/*==========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
        
when       who     what, where, why
--------   ---     ---------------------------------------------------------
10/12/17   aus     Enable MD SMEM global toc initialization
08/25/17   sunqi   Updated apt_sti_init call
08/04/17   naa     Added call to boot_ddi_entry in load_apdp_image_pre_procs
06/16/17   c_vkuppa Add sbl1_hw_play_vibr in load_qsee_image_post_procs
06/01/17   rp      Added boot_ddr_share_data_to_aop in load_apdp_image_post_procs
05/25/17   dais    Added support for filling ddr details in shared memory
05/19/17   sunqi   Correct image type ID for Loader STI
05/15/17   c_vkuppa Remove rollback protection from sbl1_config_table
05/11/17   kpa     remove unused api sbl1_restore_hash_segment_buffer
04/24/17   ds      xblconfig_init log message
04/20/17   kpa     remove pmic image loading
04/08/17   kpa     remove shrm image loading
03/08/17   kpa     enable xbl config init/deinit
03/07/17   ds	   added aop pre init call in aop preprocs
03/03/17   ds	   remove clobber prot
02/14/17   kpa     added xbl config init
01/27/17   digants assign the img whitelist pointer for each image
01/25/17   kpa     reorder AOP loading sequence
12/05/16   kpa     Load Pmic elf in dload mode
11/07/16   kpa     AOP SHRM Image loading sequence updates
09/12/16   kpa     remove rpm image loading logic
07/19/16   kpa     Load SHRM, AOP images
06/30/16   digant  Load APDP before QSEE devcfg
05/09/16   digant  STI image mmu mapping port and enablement  
10/18/11   dh      Initial Creation 

============================================================================
                     Copyright 2014-2015 Qualcomm Technologies Incorporated.
                            All Rights Reserved.
                    Qualcomm Confidential and Proprietary
===========================================================================*/

/*==========================================================================
                             INCLUDE FILES
===========================================================================*/
#include "boot_mc.h"
#include "boot_config.h"
#include "boot_flash_target.h"
#include "boot_flash_dev_if.h"
#include "boot_authenticator.h"
#include "boot_config_data.h"
#include "boot_dload.h"
#include "boot_loader.h"
#include "boot_ram_init.h"
#include "boot_logger.h"
#include "boot_page_table_armv8.h"
#include "boot_cache_mmu.h"
#include "boot_smem.h"
#include "boot_ram_partition.h"
#include "boot_ddr_info.h"
#include "sbl1_hw.h"
#include "sbl1_mc.h"
#include "boot_dload.h"
#include "boothw_target.h"
#include "boot_extern_clk_interface.h"
#include "boot_extern_ddr_interface.h"
#include "boot_shared_imem_cookie.h"
#include "boot_extern_pmic_interface.h"
#include "boot_extern_deviceprogrammer_interface.h"
#include "boot_extern_apt_interface.h"
#include "boot_extern_vsense_interface.h"
#include "boot_elf_header.h"
#include "sys_debug.h"
#include "boot_hash_buffer.h"
#include "boot_util.h"
#include "LoaderAptLib.h"
#include "boot_dload_dump.h"
#include "boot_fastcall_tz.h"
#include "boot_extern_platforminfo_interface.h"
#include "XBLConfig.h"
#include "boot_extern_ddi_interface.h"
#include "boot_extern_dal_interface.h"
#include "boot_extern_power_interface.h"
#include "boot_extern_subsys_interface.h"
#include "ChipInfoDefs.h"
#include "ChipInfo.h"


/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
#define BOOT_XBL_CORE_COOKIE1 (((uint64)'b' << 0) | \
                               ((uint64)'o' << 8) | \
                               ((uint64)'o' << 16) | \
                               ((uint64)'t' << 24) | \
                               ((uint64)' ' << 32) | \
                               ((uint64)'t' << 40) | \
                               ((uint64)'o' << 48) | \
                               ((uint64)' ' << 56) )

#define BOOT_XBL_CORE_COOKIE2 (((uint64)'x' << 0) | \
                               ((uint64)'b' << 8) | \
                               ((uint64)'l' << 16) | \
                               ((uint64)' ' << 24) | \
                               ((uint64)'c' << 32) | \
                               ((uint64)'o' << 40) | \
                               ((uint64)'r' << 48) | \
                               ((uint64)'e' << 56) )



typedef struct
{
  uint64 cookie1;
  uint64 cookie2;
} xbl_core_cookie;

extern uint8 qsee_partition_id[];
extern uint8 qhee_partition_id[];
extern uint8 appsbl_partition_id[];
extern uint8 sbl1_partition_id[];
extern uint8 qsee_devcfg_image_partition_id[];
extern uint8 apdp_partition_id[];
extern uint8 sti_partition_id[];
extern uint8 abl_partition_id[];
extern uint8 shrm_partition_id[];
extern uint8 aop_partition_id[];
extern uint8 adsp_partition_id[];

typedef boot_boolean (*boot_load_cancel_func_type)
(
  bl_shared_data_type *bl_shared_data
);

boot_procedure_func_type sbl1_exec_funcs[];
boot_procedure_func_type sbl1_jump_funcs[];
boot_load_cancel_func_type  sbl1_load_cancel_funcs[];
boot_procedure_func_type sbl1_pre_post_funcs[] ;

DALResult result = DAL_ERROR;
uint64 wl_base,wl_size,num,sectype,wl_seg_elf_entry_point;
whitelst_tbl_entry_type* wl_table;
char* buffer;
uint8 target_img_str_array[10];
uint8 target_img_partition_id_array[20];
boot_procedure_func_type* pre_procs_entries;
boot_procedure_func_type* post_procs_entries;
uint16 i,j,index_increment=0,proc_counter;
boot_configuration_table_entry* new_entry;


/* TODO: _FIX_ for 8996. TZ cannot access shared data in L2 TCM since it disables mmu.
  If mmu disabling in TZ is postponed beyond shared data copy, a branch 'bl' instrn
  seemed to have issues. Hence temporarily 
*/
struct boot_sbl_qsee_interface sbl_qsee_interface_ddr SECTION(".bss.BOOT_DDR_ZI_DATA_ZONE");
uint32 appsbl_index = 0;

/* SYSTEM_DEBUG_QSEE_INFO is memory specified in the scatter file for SBL
   and system debug to share information about where QSEE was loaded and
   if it's a 32 bit or 64 bit image. */
extern uintnt Image$$SYSTEM_DEBUG_QSEE_INFO$$Base;

#ifdef FEATURE_DEVICEPROGRAMMER_IMAGE
extern uintnt  Image$$SBL1_DEVICEPROGRAMMER_USB_ZI$$Base[];
extern uintnt  Image$$SBL1_DEVICEPROGRAMMER_USB_ZI$$ZI$$Length[];
#endif

/*==========================================================================
                     PRE/POST PROCEDURE DEFINITIONS 
===========================================================================*/

/* Wrapper function to hand control to deviceprogranmmer image */ 
static void sbl1_hand_control_to_devprog_ddr(bl_shared_data_type *bl_shared_data)
{
#ifdef FEATURE_DEVICEPROGRAMMER_IMAGE

  boot_boolean result = FALSE;
  struct mem_block devprg_mem_block;

  /* Update attributes for non-cacheable regions */
  devprg_mem_block.p_base = SCL_SBL1_OCIMEM_DATA_BASE;
  devprg_mem_block.v_base = SCL_SBL1_OCIMEM_DATA_BASE;
  devprg_mem_block.size_in_kbytes = SCL_SBL1_OCIMEM_DATA_SIZE >> 10;
  devprg_mem_block.memory_mapping = MMU_L3_SECTION_MAPPING;
  devprg_mem_block.access = MMU_PAGETABLE_MEM_READ_WRITE;
  devprg_mem_block.cachetype = MMU_PAGETABLE_MEM_NON_CACHEABLE;
  devprg_mem_block.executable = MMU_PAGETABLE_NON_EXECUTABLE_REGION;

  result =
    boot_mmu_page_table_map_single_mem_block((uint64*)mmu_get_page_table_base(),
                                             &devprg_mem_block);
  BL_VERIFY(result, BL_ERR_MMU_PGTBL_MAPPING_FAIL|BL_ERROR_GROUP_BOOT);

  devprg_mem_block.p_base = SCL_SBL1_PAGE_TABLE_BASE;
  devprg_mem_block.v_base = SCL_SBL1_PAGE_TABLE_BASE;
  devprg_mem_block.size_in_kbytes = SCL_SBL1_PAGE_TABLE_SIZE >> 10;

  result =
    boot_mmu_page_table_map_single_mem_block((uint64*)mmu_get_page_table_base(),
                                             &devprg_mem_block);
  BL_VERIFY(result, BL_ERR_MMU_PGTBL_MAPPING_FAIL|BL_ERROR_GROUP_BOOT);

  devprg_mem_block.p_base = (uint32)Image$$SBL1_DEVICEPROGRAMMER_USB_ZI$$Base;
  devprg_mem_block.v_base = (uint32)Image$$SBL1_DEVICEPROGRAMMER_USB_ZI$$Base;
  devprg_mem_block.size_in_kbytes = ((uint32)Image$$SBL1_DEVICEPROGRAMMER_USB_ZI$$ZI$$Length >> 10);
  devprg_mem_block.cachetype = MMU_PAGETABLE_MEM_WRITE_BACK_CACHE;
  result =
    boot_mmu_page_table_map_single_mem_block((uint64*)mmu_get_page_table_base(),
                                             &devprg_mem_block);
  BL_VERIFY(result, BL_ERR_MMU_PGTBL_MAPPING_FAIL|BL_ERROR_GROUP_BOOT);
  mmu_flush_cache();
  mmu_invalidate_tlb();

  boot_hand_control_to_deviceprogrammer_ddr_main(bl_shared_data);
#endif /* FEATURE_DEVICEPROGRAMMER_IMAGE */
}


/* Initialize XBL config Lib.
  Note:  
  Dependencies: XBL config needs to be initialized after boot flash and heap
  initializations.
*/
void sbl1_xblconfig_init(bl_shared_data_type *bl_shared_data )
{
  xblconfig_status_type status;
  uint32 handle, size;
  
  boot_log_message("xblconfig_init, Start");
  boot_log_start_timer();
  status = xblconfig_init();
  BL_VERIFY(status == XBLCONFIG_SUCCESS, 
            BL_ERR_IMG_NOT_FOUND|BL_ERROR_GROUP_BOOT); 
  boot_log_stop_timer("xblconfig_init, Delta");
  
  status = xblconfig_open((const char *)"/sbl1_config.bin", &handle, &size); 
  result = boot_DALSYS_Malloc(sizeof(char)*size, (void **)  (&buffer));
  BL_VERIFY(result == DAL_SUCCESS, BL_ERR_RESOURCE_NOT_AVAILABLE|BL_ERROR_GROUP_BOOT);
  status = xblconfig_read(handle, 0,size, buffer);
  status = xblconfig_close(handle); 
  result = boot_DALSYS_Malloc(sizeof(boot_configuration_table_entry), (void **)  (&new_entry));
  BL_VERIFY(result == DAL_SUCCESS, BL_ERR_RESOURCE_NOT_AVAILABLE|BL_ERROR_GROUP_BOOT);
  
}

boot_configuration_table_entry* sbl1_parse_config_entry()
{
  
  	if (new_entry != NULL)
  	{
      result = boot_DALSYS_Free(new_entry);
    }
  
  result = boot_DALSYS_Malloc(sizeof(boot_configuration_table_entry), (void **)  (&new_entry));
  BL_VERIFY(result == DAL_SUCCESS, BL_ERR_RESOURCE_NOT_AVAILABLE|BL_ERROR_GROUP_BOOT );
  
  /*Set host_img_id */
  BL_VERIFY(new_entry!= NULL, BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT);
  new_entry->host_img_id = 21; 
  
  /*Set host_img_type */
  new_entry->host_img_type = 0;
  
  /*Set target_img_id */
  new_entry->target_img_id = 27; 
    
  /*Set target_img_type */
  new_entry->target_img_type = 1; 
  
  
  /*Load target_img_sec_type */
  qmemcpy(&sectype,&buffer[index_increment], 2);
  new_entry->target_img_sec_type =(secboot_sw_type)sectype;
  index_increment=index_increment+4;
  
  /* Load target_img_partition_id */
  qmemcpy(&target_img_partition_id_array,&buffer[index_increment], 16);
  new_entry->target_img_partition_id = target_img_partition_id_array;
  index_increment=index_increment+16;

  /* Load target_img_str */
  qmemcpy(&target_img_str_array,&buffer[index_increment], 8);
  new_entry->target_img_str =target_img_str_array;
  index_increment=index_increment+8;

  /*Load boolean entry : load */ 
  new_entry->load =buffer[index_increment++];

  /*Load boolean entry : auth */
  new_entry->auth =buffer[index_increment++];
  
  /*Load boolean entry : exec */
  new_entry->exec =buffer[index_increment++];
  
  /*Load boolean entry : jump */
  new_entry->jump =buffer[index_increment++];
  
  /*Load boolean entry : boot_ssa_enabled */
  new_entry->boot_ssa_enabled = buffer[index_increment++];

  /*Load boolean entry : enable_xpu */ 
  new_entry->enable_xpu = buffer[index_increment++];

  /*Load Function ID for function : exec_func */
  new_entry->exec_func = (boot_procedure_func_type)sbl1_exec_funcs[(uint32)buffer[index_increment++]];

  /*Load Function ID for function : jump_func */
  new_entry->jump_func = (boot_procedure_func_type)sbl1_jump_funcs[(uint32)buffer[index_increment++]];

  /*Load Function ID for function : load_cancel */   
  new_entry->boot_load_cancel =sbl1_load_cancel_funcs[(uint32)buffer[index_increment++]];

  /*Load pre procs */
  if (pre_procs_entries != NULL)
  {
    result = boot_DALSYS_Free(pre_procs_entries);
  } 
  
  /*Load the number of processes in pre_procs */
  proc_counter=buffer[index_increment++];
  /*Allocate memory equal to number of processes */
  result = boot_DALSYS_Malloc(sizeof(boot_procedure_func_type)*proc_counter, (void **)  (&pre_procs_entries));
  BL_VERIFY(result == DAL_SUCCESS, BL_ERR_RESOURCE_NOT_AVAILABLE|BL_ERROR_GROUP_BOOT );
  /*Loop through the processes list*/  
  for(j=0;j<proc_counter;j++)
  { 
  	/* Parse each entry */
    pre_procs_entries[j] = (boot_procedure_func_type)sbl1_pre_post_funcs[(uint32)buffer[index_increment++]];   
  }
  /*Assign the array to the table*/
  new_entry->pre_procs =pre_procs_entries;
  

  /*Load post procs */
 	if (post_procs_entries != NULL)
  {
    result = boot_DALSYS_Free(post_procs_entries);
  }
  /*Load the number of processes in post_procs */
  proc_counter=buffer[index_increment++];
  /*Allocate memory equal to number of processes */
  result = boot_DALSYS_Malloc(sizeof(boot_procedure_func_type)*proc_counter, (void **)  (&post_procs_entries));
  BL_VERIFY(result == DAL_SUCCESS, BL_ERR_RESOURCE_NOT_AVAILABLE|BL_ERROR_GROUP_BOOT );
  /*Loop through the processes list*/  
  for(j=0;j<proc_counter;j++)
  { 
  	/* Parse each entry */
  	post_procs_entries[j] = (boot_procedure_func_type)sbl1_pre_post_funcs[(uint32)buffer[index_increment++]];   
  }
  /*Assign the array to the table*/
  new_entry->post_procs =post_procs_entries;

  /*Assign the sbl_qsee_interface_index*/
  new_entry->sbl_qsee_interface_index =0;

  /*Assign the xpu_proc_id*/  
  new_entry->xpu_proc_id = 0;

  /*Load the number of whitelist regions */
  num=buffer[index_increment++];

  if (wl_table != NULL)
  {
    result = boot_DALSYS_Free(wl_table);
  }
  /*Allocate memory based on number of addresses in whitelist regions*/
  result = boot_DALSYS_Malloc(sizeof(whitelst_tbl_entry_type)*num, (void **)  (&wl_table));
  BL_VERIFY(result == DAL_SUCCESS, BL_ERR_RESOURCE_NOT_AVAILABLE|BL_ERROR_GROUP_BOOT );
  /*Loop through list of whitelist regions*/
  for (i = 0,j=0; i < num-1; i++)
  {
  	/*Load the whitelist base*/
    qmemcpy(&wl_base, &buffer[index_increment], 4);      
    (wl_table+i)->start_addr=wl_base;
    index_increment+=4;

    /*Load the whitelist size*/
    qmemcpy(&wl_size, &buffer[index_increment], 4);
    (wl_table+i)->size=wl_size;
    index_increment+=4;
  }
  /*Add NULL entry*/
  (wl_table+i)->start_addr=0;
  (wl_table+i)->size=0;
  /*Assign the whitelist table*/
  new_entry->whitelist_ptr =wl_table;

  /*Load the seg_elf_entry point*/ 
  qmemcpy(&wl_seg_elf_entry_point,&buffer[index_increment], 4);
  new_entry->seg_elf_entry_point =wl_seg_elf_entry_point;
  index_increment=index_increment+8;

  return new_entry;

}


/*==========================================================================
        List of SBL1 procedures to execute prior to dload entry
===========================================================================*/
boot_procedure_func_type sbl1_pre_dload_procs[] =
{
  /*-----------------------------------------------------------------------
   * Initialization functions for dload. This has to be the first function 
   * called before Dload entry
   *----------------------------------------------------------------------*/
  boot_init_for_dload,
  
  /*-----------------------------------------------------------------------
   * Setup clocks for ram dump
   *----------------------------------------------------------------------*/
  sbl1_hw_dload_init,
  
  /*-----------------------------------------------------------------------
   * NULL pointer indicates the end of the list
   *-----------------------------------------------------------------------*/
  NULL
};

/*==========================================================================
                     CONFIG FUNCTION DEFINITIONS 
===========================================================================*/

/* Executing QSEE from SBL1 */ 
static void qsee_jump_func(bl_shared_data_type *bl_shared_data )
{
  boot_boolean result = FALSE;
  struct mem_block qsee_mem_block;
  sys_debug_qsee_info_type * qsee_info =
    (sys_debug_qsee_info_type *)&Image$$SYSTEM_DEBUG_QSEE_INFO$$Base;
   xblconfig_status_type status;


  qsee_info->entry_point = (uint64)NULL;
  qsee_info->elf_class = (uint32)NULL;

  /* Flush the cache before entering QSEE */
  dcache_flush_region((void *)SCL_QSEE_CODE_BASE, SCL_QSEE_TOTAL_SIZE);
  dcache_flush_region((void *)SCL_pIMEM_BASE, SCL_pIMEM_MINIMUM_SIZE);
  dcache_flush_region((void *)SCL_SBL1_IMAGE_BASE, SCL_SBL1_IMAGE_SIZE);  
  dcache_flush_region((void *)SCL_SBL1_DDR_DSF_ROM_BASE, SCL_SBL1_DDR_DSF_SEGMENT_SIZE);  
  dcache_flush_region((void *)SCL_SBL1_OCIMEM_DATA_BASE, SCL_SBL1_OCIMEM_DATA_SIZE);  
  dcache_flush_region((void *)SCL_SBL1_DDR_BASE, SCL_SBL1_DDR_SIZE);  

  boot_pm_vib_off();
  boot_aop_mini_dump_init();
  
  /* De-Initialize the boot logger */
  sbl1_boot_logger_deinit();
  
  /*Deinitialize XBL Config Lib*/
  status = xblconfig_deinit();
  BL_VERIFY(status == XBLCONFIG_SUCCESS, 
            BL_ERR_IMG_NOT_FOUND|BL_ERROR_GROUP_BOOT); 
  
  /* Search image entries in QSEE interface for QSEE and get entry point.
     The entry point and elf type of QSEE must be stored so if an abnormal
     reset occurs the system debug image knows how to get to QSEE.  A small
     section has been carved out in the system debug image memory for this
     data. 
  */
  qsee_info->entry_point =
    boot_sbl_qsee_interface_get_entry_point(&(bl_shared_data->sbl_qsee_interface),
                                            SECBOOT_QSEE_SW_TYPE);
  qsee_info->elf_class =
    boot_sbl_qsee_interface_get_eident(&(bl_shared_data->sbl_qsee_interface),
                                       SECBOOT_QSEE_SW_TYPE);

  /* Verify entry point is not null */
  BL_VERIFY(qsee_info->entry_point != (uint64)NULL,
            BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT);
  
  /* Verify QSEE ELF class is valid */
  BL_VERIFY((qsee_info->elf_class == ELF_CLASS_64) ||
            (qsee_info->elf_class == ELF_CLASS_32),
            BL_ERR_ELF_CLASS_INVALID|BL_ERROR_GROUP_BOOT);


  /* Mark QSEE page table as executable */
  qsee_mem_block.p_base = SCL_QSEE_CODE_BASE;
  qsee_mem_block.v_base = SCL_QSEE_CODE_BASE;
  qsee_mem_block.size_in_kbytes = SCL_QSEE_TOTAL_SIZE >> 10;
  qsee_mem_block.memory_mapping = MMU_L3_SECTION_MAPPING;
  qsee_mem_block.access = MMU_PAGETABLE_MEM_READ_WRITE;
  qsee_mem_block.cachetype = MMU_PAGETABLE_DEVICE_SHARED;
  qsee_mem_block.executable = MMU_PAGETABLE_EXECUTABLE_REGION;

  result =
    boot_mmu_page_table_map_single_mem_block((uint64 *)(uintnt)bl_shared_data->sbl_shared_data->mmu_l1_pagetable_base,
                                             &qsee_mem_block);
  BL_VERIFY(result, BL_ERR_MMU_PGTBL_MAPPING_FAIL|BL_ERROR_GROUP_BOOT);

  qsee_mem_block.p_base = SCL_pIMEM_BASE;
  qsee_mem_block.v_base = SCL_pIMEM_BASE;
  qsee_mem_block.size_in_kbytes = SCL_pIMEM_SIZE >> 10;
  qsee_mem_block.memory_mapping = MMU_L2_SECTION_MAPPING;
  qsee_mem_block.access = MMU_PAGETABLE_MEM_READ_WRITE;
  qsee_mem_block.cachetype = MMU_PAGETABLE_MEM_NON_CACHEABLE;
  qsee_mem_block.executable = MMU_PAGETABLE_EXECUTABLE_REGION;

  result =
    boot_mmu_page_table_map_single_mem_block((uint64 *)(uintnt)bl_shared_data->sbl_shared_data->mmu_l1_pagetable_base,
                                             &qsee_mem_block);
  BL_VERIFY(result, BL_ERR_MMU_PGTBL_MAPPING_FAIL|BL_ERROR_GROUP_BOOT);

  mmu_invalidate_tlb();

  /* Store the index of appsbl image entry in sbl qsee interface for TZ to fetch
    the appsbl entry point. Needed as image ID can no longer be used to identify
    appsbl image.
  */

  /*This is based on the assumption that APPSBL image is the last entry*/
  (bl_shared_data->sbl_qsee_interface).appsbl_entry_index = (bl_shared_data->sbl_qsee_interface).number_images - 1;
                      //sbl1_config_table[appsbl_index].sbl_qsee_interface_index ;
                      
  qmemcpy(&sbl_qsee_interface_ddr, &(bl_shared_data->sbl_qsee_interface),
            sizeof(boot_sbl_qsee_interface));
   
  dcache_flush_region((void *)&sbl_qsee_interface_ddr, sizeof(boot_sbl_qsee_interface));            
   
  /* Deinit hardware.  This must be done AFTER sbl1_boot_logger_deinit() 
    This is also used to turn off VMEM memory clocks. So ensure the api code/buffers
    are not in memory if it is being used.
  */  
  sbl1_hw_deinit();
  
 /* clearing the continuous boot failure count which is stored in IMEM */
  if (boot_shared_imem_cookie_ptr != NULL)
  {
	boot_shared_imem_cookie_ptr->boot_fail_count = 0;
  }
  
  /* "Exit" bootloader and enter QSEE context.
    */
    
  if (sbl1_get_current_el() == EL_1)
  {
     boot_fastcall_tz_no_rsp(TZ_SBL_END_MILESTONE,
                             TZ_SBL_END_MILESTONE_PARAM_ID,
                             (uint32)(&sbl_qsee_interface_ddr),
                             (uint32)(sizeof(sbl_qsee_interface_ddr)),
                             0,0);
  }
  else
  {  
     ((void (*)())(uintnt)(qsee_info->entry_point))
      (&(sbl_qsee_interface_ddr)); 
  }

  /*Must not reach here*/
  while(1);
}


/* Executing STI from SBL1 */ 
static void sti_jump_func(bl_shared_data_type *bl_shared_data )
{
  boot_boolean result = FALSE;
  struct mem_block sti_mem_block;

  sys_debug_qsee_info_type * sti_info =
    (sys_debug_qsee_info_type *)&Image$$SYSTEM_DEBUG_QSEE_INFO$$Base;

  /*  STI is apt's test image to verify loader. If STI image is loaded
    it is the final image that loader jumps to instead of QSEE image.
    The image is like of extention of xbl loader so the deinit api's
    need not be called.
  */  

  sti_info->entry_point = (uint64)NULL;
  sti_info->elf_class = (uint32)NULL;

  /* Flush the cache before entering STI */
  dcache_flush_region((void *)SCL_SBL1_IMAGE_BASE, SCL_SBL1_IMAGE_SIZE);  
  dcache_flush_region((void *)SCL_SBL1_DDR_DSF_ROM_BASE, SCL_SBL1_DDR_DSF_SEGMENT_SIZE);  
  dcache_flush_region((void *)SCL_SBL1_OCIMEM_DATA_BASE, SCL_SBL1_OCIMEM_DATA_SIZE);  
  dcache_flush_region((void *)SCL_SBL1_DDR_BASE, SCL_SBL1_DDR_SIZE);    
  
  /* De-Initialize the boot logger */
  sbl1_boot_logger_deinit();

  /* Search image entries in QSEE interface for STI and get entry point.
  */
  sti_info->entry_point =
    boot_sbl_qsee_interface_get_entry_point(&(bl_shared_data->sbl_qsee_interface),
                                            SECBOOT_STI_SW_TYPE);
  sti_info->elf_class =
    boot_sbl_qsee_interface_get_eident(&(bl_shared_data->sbl_qsee_interface),
                                       SECBOOT_STI_SW_TYPE);

  /* Verify entry point is not null */
  BL_VERIFY(sti_info->entry_point != (uint64)NULL,
            BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT);
  
  /* Verify STI ELF class is valid */
  BL_VERIFY((sti_info->elf_class == ELF_CLASS_64) ||
            (sti_info->elf_class == ELF_CLASS_32),
            BL_ERR_ELF_CLASS_INVALID|BL_ERROR_GROUP_BOOT);

  /* Flush the cache before entering STI */
  dcache_flush_region((void *)SCL_XBL_CORE_CODE_BASE, SCL_XBL_CORE_TOTAL_SIZE);
  dcache_flush_region((void *)sti_info, sizeof(sys_debug_qsee_info_type));

   /* Mark STI page table as executable */
	sti_mem_block.p_base = SCL_LOADERTEST_CODE_BASE;
	sti_mem_block.v_base = SCL_LOADERTEST_CODE_BASE;
	sti_mem_block.size_in_kbytes = SCL_LOADERTEST_CODE_SIZE >> 10;
	sti_mem_block.memory_mapping = MMU_L2_SECTION_MAPPING;
	sti_mem_block.access = MMU_PAGETABLE_MEM_READ_WRITE;
	sti_mem_block.cachetype = MMU_PAGETABLE_DEVICE_SHARED;
	sti_mem_block.executable = MMU_PAGETABLE_EXECUTABLE_REGION;

  result =
    boot_mmu_page_table_map_single_mem_block((uint64 *)(uintnt)bl_shared_data->sbl_shared_data->mmu_l1_pagetable_base,
                                             &sti_mem_block);
  BL_VERIFY(result, BL_ERR_MMU_PGTBL_MAPPING_FAIL|BL_ERROR_GROUP_BOOT);

  mmu_invalidate_tlb();

  boot_log_message("STI Image Entry");

  /* "Exit" bootloader and enter STI context.
     Note: This function should not return.
  */
  apt_sti_init(sti_info->entry_point, bl_shared_data);
  
/*Must not reach here*/
 while(1);
}


/* Conditionally cancel APPSBL loading in SBL1 */
static boot_boolean appsbl_load_cancel(bl_shared_data_type *bl_shared_data)
{
  boot_boolean cancel = FALSE;
  
    /* Store the index for future use */
    //appsbl_index = bl_shared_data->sbl_qsee_interface.number_images;

    /*Update APPSBL entry to enable loading XBL core segments*/
    new_entry->target_img_type = CONFIG_IMG_ELF_SEG;
    new_entry->target_img_sec_type = SECBOOT_SBL_SW_TYPE;
    boot_elf_ziinit_structures(NULL);
    boot_set_elf_and_prog_header(
        bl_shared_data->sbl_shared_data->pbl_shared_data->elf_meta_info.elf_hdr,
        bl_shared_data->sbl_shared_data->pbl_shared_data->elf_meta_info.prog_hdr
      );
    boot_set_hash_segment_buffer(
        (void *)bl_shared_data->sbl_shared_data->pbl_shared_data->elf_meta_info.hash_seg_hdr
    );

    /* Configure the target image using custom partition ID information */ 
    boot_flash_configure_target_image(sbl1_partition_id);
    
  return cancel;
}

/* Conditionally cancel ABL loading in XBL */
static boot_boolean abl_load_cancel(bl_shared_data_type *bl_shared_data)
{  
  boot_flash_trans_if_type *trans_if;    
  boot_boolean cancel = TRUE;
  mi_boot_image_header_type abl_header;
  
  do
  {
    /* Load only if ABL partition exists */
    boot_flash_configure_target_image(abl_partition_id);
    trans_if = boot_flash_dev_open_image(GEN_IMG);
    if(trans_if == NULL )
    {
       break;
    }
    boot_flash_dev_close_image( &trans_if );

	/* Validate the ELF header and load the image only if the header is valid */
	if (!boot_elf_verify_image_header(abl_partition_id, &abl_header))
	{
		/* Partition is not flashed */
		break;
	}      
    
    /* All conditions are satisfied, load ABL image */
    cancel = FALSE;
  } while(0);

  return cancel;
}

/* Conditionally cancel AOP Image loading in SBL1 (ie XBL Loader) */
static boot_boolean aop_load_cancel(bl_shared_data_type *bl_shared_data)
{
  DalPlatformInfoPlatformType platform;
  boot_boolean status = FALSE;

  platform = boot_DalPlatformInfo_Platform();

  if((boot_dload_is_dload_mode_set() == TRUE) ||
     (platform == DALPLATFORMINFO_TYPE_VIRTIO)
    )
  {
    status = TRUE;
  }
    /* Cancel loading if XBL loader is in dload mode */
  return status;
}

/* Conditionally cancel APDP loading in SBL1 */
static boot_boolean apdp_load_cancel(bl_shared_data_type *bl_shared_data)
{  
  boot_flash_trans_if_type *trans_if;    
  boot_boolean cancel = TRUE;
  mi_boot_image_header_type apdp_header;
  
  do
  {
    /* Load only if APDP partition exists */
    boot_flash_configure_target_image(apdp_partition_id);
    trans_if = boot_flash_dev_open_image(GEN_IMG);
    if(trans_if == NULL )
    {
       break;
    }
    boot_flash_dev_close_image( &trans_if );

    /* Validate the ELF header and load the APDP image only if the 
      header is valid 
    */
    if (!boot_elf_verify_image_header(apdp_partition_id, &apdp_header))
    {
      /* Partition is not flashed */
      break;
    }
    
    /* All conditions are satisfied, load APDP image */
    cancel = FALSE;
  } while(0);

  return cancel;
}

/* Conditionally cancel Qsee enumeration/ Dev config image loading in SBL1 */
static boot_boolean qsee_devcfg_image_load_cancel(bl_shared_data_type *bl_shared_data)
{
  boot_flash_trans_if_type *trans_if; 
  boot_boolean cancel = TRUE;

  do
  {
    /* Load only if Qsee enumeration/ dev config image partition exists */
    boot_flash_configure_target_image(qsee_devcfg_image_partition_id);
    trans_if = boot_flash_dev_open_image(GEN_IMG);

    if(trans_if != NULL )
    {
      boot_flash_dev_close_image( &trans_if );
      cancel = FALSE;
      break;
    }
  }while(0);
  
  return cancel;
}


static boot_boolean adsp_load_cancel(bl_shared_data_type *bl_shared_data)
{
  boot_flash_trans_if_type *trans_if; 
  boot_boolean cancel = TRUE;
  mi_boot_image_header_type adsp_header;

  do
  {
    //If booting on warlock, skip this image
    if(ChipInfo_GetChipId()==CHIPINFO_FAMILY_SDM670)
    {
      break;
    }

    //Check for partition ID
    boot_flash_configure_target_image(new_entry->target_img_partition_id);
    trans_if = boot_flash_dev_open_image(GEN_IMG);

    if(trans_if == NULL )
    {
       break;
    }
    boot_flash_dev_close_image( &trans_if );

    /* Validate the ELF header and load the ADSP image only if the 
      header is valid 
    */
    if (!boot_elf_verify_image_header(new_entry->target_img_partition_id, &adsp_header))
    {
      /* Partition is not flashed */
      break;
    }
  }while(0);
  
  return cancel;
}

/* Conditionally cancel STI loading in SBL1 */
static boot_boolean sti_load_cancel(bl_shared_data_type *bl_shared_data)
{
  boot_flash_trans_if_type *trans_if; 
  mi_boot_image_header_type sti_header;  
  boot_boolean cancel = TRUE;
  boot_boolean is_auth_enabled = TRUE;
  secboot_hw_etype status = E_SECBOOT_HW_FAILURE;
   
 /*
   Note:
   STI (APT's loader test image) to be loaded only if partition is present,
   image is flashed, authentication not enabled and debug disable fuses
   not blown   
 */
  
  do
  {
    /* Load only if STI partition exists */
    boot_flash_configure_target_image(sti_partition_id);
    trans_if = boot_flash_dev_open_image(GEN_IMG);
    if(trans_if == NULL )
    {
       break;
    }
    boot_flash_dev_close_image( &trans_if );

    /* Validate the ELF header and load the STI image only if the 
      header is valid 
    */
    if (!boot_elf_verify_image_header(sti_partition_id, &sti_header))
    {
      /* Partition is not flashed */
      break;
    }
    
    /* Do not load STI image if authentication is enabled or
      debug disable fuses are blown.
    */
    if(DEBUG_DISABLED_APPS)
    {
      break;
    }

    status = boot_is_auth_enabled(&is_auth_enabled);
    BL_VERIFY((status == E_SECBOOT_HW_SUCCESS), (uint16)status|BL_ERROR_GROUP_SECURITY); 

    if(is_auth_enabled == TRUE)
    {
      break;
    }
    
    /* All conditions are satisfied, load STI image */
    cancel = FALSE;
  
  }while(0);
  
  return cancel;
}

boot_procedure_func_type sbl1_exec_funcs[] =                                
{
    NULL, //0  
};

boot_procedure_func_type sbl1_jump_funcs[] =
{                               
   NULL,			//0

   sti_jump_func, 	//1
  
   qsee_jump_func 	//2
  
};
 
boot_load_cancel_func_type  sbl1_load_cancel_funcs[] =
{
      NULL, 							//0

      apdp_load_cancel, 				//1

      aop_load_cancel,  				//2

      qsee_devcfg_image_load_cancel,  	//3
                                    
      sti_load_cancel,  				//4
  
      abl_load_cancel,   				//5

      appsbl_load_cancel,  				//6

      adsp_load_cancel          //7

};

boot_procedure_func_type sbl1_pre_post_funcs[] =
{
	NULL,									//0

  /*-----------------------------------------------------------------------
   Initialize the default CDT before reading CDT from flash
  -----------------------------------------------------------------------*/
  	boot_config_data_table_default_init, 	//1
    
  /*-----------------------------------------------------------------------
   API called twice, this first time passes default compiled CDT contents
   to DDR driver, in case CDT only contains Platform CDB.  Second call
   will update DDR driver with Flashed CDT DDR CDB contents, if DDR CDB 
   was found in flashed CDT
  -----------------------------------------------------------------------*/
  	sbl1_ddr_set_default_params,        	//2
  
  /* Copy the configure data table from flash */
  	boot_config_data_table_init,        	//3
  
  /* Store platform id */
  	sbl1_hw_platform_pre_ddr,           	//4

  /* Initialize PMIC and railway driver */
  	sbl1_hw_pre_ddr_init,               	//5

  /* Configure ddr parameters based on eeprom CDT table data. */
  	sbl1_ddr_set_params,                	//6
  
  /* Initialize DDR */
  	(boot_procedure_func_type)sbl1_ddr_init,//7
  
  /* Train DDR if applicable */
  	sbl1_do_ddr_training,               	//8
  
 /*----------------------------------------------------------------------
   Run deviceprogrammer if compiling the deviceprogrammer_ddr image.
   In XBL builds the function below is stubbed out (does nothing)
  ----------------------------------------------------------------------*/
  	sbl1_hand_control_to_devprog_ddr,   	//9
 
 #ifndef FEATURE_DEVICEPROGRAMMER_IMAGE
  /* Run DDI testing if tests exist */
  	boot_ddi_entry,                     	//10
  
  /* Initialize SBL1 DDR ZI region, relocate boot log to DDR */   
  	sbl1_post_ddr_init,                 	//11
  
  	sbl1_hw_init_secondary,             	//12
#ifndef FEATURE_DEVICEPROGRAMMER_IMAGE
/* Backup QSEE and QHEE region for ramdumps */  
  	boot_dload_dump_security_regions,   	//13

  /* Check to see if DLOAD mode needs to be entered */
  	boot_dload_check,                   	//14

  /* Set the memory barrier pointer to shared memory */
  	boot_cache_set_memory_barrier,      	//15

  /*----------------------------------------------------------------------
   Put SMEM in debug state such that smem_alloc() calls will return NULL. 
   The state is changed back to normal once smem_boot_init() is called.
   This call has to be made after setting the memory barrier.
  ------------------------------  ----------------------------------------*/
  	boot_smem_debug_init,             		//16
    
  /* Initialize shared memory after dload to preserve logs */
  	boot_smem_init,                   		//17
  
  /* Initialize SMEM minidump table */
  	boot_smem_alloc_for_minidump,     		//18

   /* Stub out for rumi build. pmic api  pm_get_power_on_status gets 
     called from below api to get power on reason */ 
  /*----------------------------------------------------------------------
   Store Power on Status in SMEM. 
   Needs to be done after PMIC and SMEM initialization
  ----------------------------------------------------------------------*/
  	boot_smem_store_pon_status,     		//19

  /*----------------------------------------------------------------------
   Store the platform id to smem
  ----------------------------------------------------------------------*/
  	sbl1_hw_platform_smem,          		//20
  
  	(boot_procedure_func_type) boot_ddr_share_data_to_aop,  	//21

  	(boot_procedure_func_type) boot_clock_init_rpm,         	//22

  	boot_vsense_copy_to_smem,     			//23

  /*----------------------------------------------------------------------
   Get shared data out of the flash device module
  ----------------------------------------------------------------------*/
  	boot_share_flash_data,      			//24
  
  /*----------------------------------------------------------------------
   populate the ram partition table
  ----------------------------------------------------------------------*/
  	boot_populate_ram_partition_table,    	//25

  /*populate ddr details shared memory*/
  	boot_populate_ddr_details_shared_table, //26


  /*----------------------------------------------------------------------
   Initialize GPIO for low power configuration
  ----------------------------------------------------------------------*/
  	sbl1_tlmm_init,                     	//27
  
  /*-----------------------------------------------------------------------
   Calls efs cookie handling api to perform efs backup/restore
  -----------------------------------------------------------------------*/  
  	sbl1_efs_handle_cookies,            	//28
    
  /*-----------------------------------------------------------------------
   APT Security Test
   ----------------------------------------------------------------------*/
  	(boot_procedure_func_type)boot_apt_test,//29
	
  #endif
  /* do pmic pre-aop init */
  	sbl1_pm_aop_pre_init_wrapper,         	//30

#ifndef FEATURE_DEVICEPROGRAMMER_IMAGE
  /* Load SEC partition if it exists.  This must be done after QSEE is
     loaded as the partition is loaded into a QSEE buffer. */
  	sbl1_load_sec_partition,            	//31

  /* Play power on vibration */
  	sbl1_hw_play_vibr,                  	//32
#endif
  /*-----------------------------------------------------------------------
   * Determine APPSBL architecture and update SBL - QSEE interface
   *----------------------------------------------------------------------*/
  	sbl1_appsbl_arch_determination,     	//33

    #endif

    sbl1_release_adsp_proc,               //34
};
  

