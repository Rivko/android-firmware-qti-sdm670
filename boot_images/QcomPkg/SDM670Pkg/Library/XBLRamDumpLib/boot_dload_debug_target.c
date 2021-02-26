/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             Boot Dload Debug Target File

GENERAL DESCRIPTION
  This module defines the target specific memory debug information to be sent
  to host.
  Please make sure that dump region file name follows the 8.3 file name format!

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright 2011 - 2017 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/XBLRamDumpLib/boot_dload_debug_target.c#8 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/02/17   aus     Added support for post target init to do the encryption in mini dump 
10/12/17   aus     Updated to support minidump
09/22/17   rohik   Added support for saving the logdump partition with PC dump
04/20/17   yps     remove boot_pimem_get_status
04/19/17   kpa     update dload_debug_add_region return type
04/11/17   kpa     support 4GB DDR dump size
03/02/17   ds      add aoss_cc regs
02/07/17   yps     Added pon history dump
12/20/16   yps     Using 2GB size split dump files larger than 2GB
04/18/16   digants Added region in ramdump for part bin information
01/27/16   kpa     Support taking 6GB DDR ramdumps, update pimem get status api
07/09/15   kpa     Add IPA memory region dumps for chip version v2 onwards.
06/25/15   kpa     Allow DDR CS1 address range to be lesser than CS0 for ramdumps.
06/25/15   as      Updated dload_mem_debug_supported
05/19/15   kpa     Add pImem region dumps runtime if pimem is enabled.
02/27/15   kpa     Remove marking rpm message ram as restricted memory.
02/07/15   kpa     Update dload_mem_debug_target_init to include DDR CS1 when its non-contiguous
10/29/15   kpa     set mem_dump_supported to true
10/29/14   kpa     Support 64 bit memory map
09/23/14   ck      Moved target specific functions dload_mem_debug_is_device_retail_unlocked
                   and dload_mem_debug_supported from boot_dload_debug.
08/08/14   ck      Refactored Data ZI region attributes for GCC compiler
07/21/14   ck      Moved from Loader to RamDump
05/07/14   kedara  Fix compilation warnings.
04/29/14   ck      Adding zero_init attribute on small variables that must be in ZI region
04/02/14   ck      Added RPM MSG RAM as a restricted region
03/17/14   ck      Added call to dload_mem_debug_zero_peripheral_memory in retail unlocked scenario
01/10/14   ck      Added support for retail unlock dumps
06/13/13   dh      Add dload_add_ddr_training_data to dump ddr training data
06/06/13   dh      Call sbl1_hw_get_reset_status to read the backed up copy of
                   gcc reset status

02/11/13   dh      Move dload_mem_debug_supported to common code
01/28/13   dh      Add DDR as the last region in memory debug table
09/20/12   dh      Add Pmic power on reason and reset status to ram dump
07/17/12   dh      Remove reset status region
05/23/11   dh      call dload_mem_debug_len_init in dload_mem_debug_target_init to
                   set memory region table to be fixed length, required by sahara ram dump
04/05/11   dh      do not include customer.h
09/29/11   dh      Initial revision

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "boot_dload_debug.h"
#include "boot_ddr_info.h"
#include "boot_target.h"
#include "boot_shared_imem_cookie.h"
#include "boot_extern_pmic_interface.h"
#include "boot_extern_seccfg_interface.h"
#include "boot_sbl_qsee_interface.h"
#include "boot_msm.h"
#include "boot_extern_pimem_interface.h"
#include "boot_logger.h"
#include "sbl1_hw.h"
#include "sbl1_mc.h"
#include "LoaderUtils.h"
#include "boot_sdcc.h"
#include "boot_flash_dev_if.h"
#include "boot_fastcall_tz.h"
#include "boot_whitelist_prot.h"
#include "boot_util.h"
#include "boot_minidump.h"
#include "boot_dload.h"
#include "smem_type.h"
#include "smem.h"
#include "boot_shared_functions.h"
#include "boot_dload_debug.h"
#include "Target_cust.h"


uint32  raw_partition_addr = RAW_PARTITION_BUF_ADDR;
uint32 raw_partition_len = RAW_PARTITION_BUF_SIZE;
extern uint8 log_dump_partition_id[];
boot_flash_trans_if_type * trans_if =NULL;
extern boolean sahara_mem_debug_enabled ;
uint32 dload_flag =0x0;

extern dload_debug_type dload_debug_info[NUM_REGIONS];
uint32 start_index_default;

#ifdef FEATURE_DLOAD_MEM_DEBUG

/*===========================================================================

**  Function :  dload_add_raw_partition

** ==========================================================================
*/
/*!
* 
* @brief
*   Add the logdump partition region
*   
* @par Dependencies
*   This api Must be called after dload_add_ddr_training_data_info
*   
* @retval
*   None                                                               
* 
* @par Side Effects
*   None
* 
*/


static void dload_add_raw_partition(void)
{
	//open the log dump partition in case of sahara mem debug for SD dump we will handle in 
	//boot_dump_logdump_partition API
  	if (sahara_mem_debug_enabled)
	{	
	 boot_flash_configure_target_image(log_dump_partition_id);
	 trans_if = boot_flash_dev_open_image(GEN_IMG);

	 /* If this partition exists */
	 if(trans_if != NULL)
	 {
		raw_partition_len = boot_coldplug_get_partition_size_by_image_id(GEN_IMG);
		if(raw_partition_len)
		{
		  boot_log_message("adding raw partition");
          dload_debug_add_region(OPTIONAL_DEF_SAVE,
                          (uint32)(raw_partition_addr),
                           raw_partition_len,
                          "log partition", "LOGDUMP.BIN");

		  dload_debug_add_restricted_region((uint32)raw_partition_addr,
                                    raw_partition_len);
		}
	}
   }
}


/*===========================================================================

**  Function :  dload_add_ddr_training_data_info

** ==========================================================================
*/
/*!
* 
* @brief
*   Add the ddr training data region, ddr training data is loaded from ddr 
*   training data partition
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
static void dload_add_ddr_training_data(void)
{
  boolean add_region_status;
  
  /* Buffer to store ddr training data */
  static uint8 ddr_training_data_dump[SCL_DDR_TRAINING_DATA_BUF_SIZE];

  if(boot_set_entry_as_img_whitelist((uint64)(&ddr_training_data_dump),(uint64)(SCL_DDR_TRAINING_DATA_BUF_SIZE)))
  {
    /* Load SCL_DDR_TRAINING_DATA_BUF_SIZE of data from the training partition*/
    sbl1_load_ddr_training_data_from_partition(ddr_training_data_dump,
                                               SCL_DDR_TRAINING_DATA_BUF_SIZE);
    
    add_region_status = dload_debug_add_region(OPTIONAL_DEF_SAVE,
                            (uint32)ddr_training_data_dump,
                            SCL_DDR_TRAINING_DATA_BUF_SIZE,
                            "DDR Training Data", "DDR_DATA.BIN");
                            
    BL_VERIFY((add_region_status == TRUE),
                BL_ERR_RAM_DUMP_FAIL|BL_ERROR_GROUP_BOOT);                              
  }
}

/*===========================================================================

**  Function :  add_default_smem_entries

** ==========================================================================
*/
/*!
* 
* @brief
*   Add SMEM image version info and SW build information
*   
* @par Dependencies
*   
* @retval
*   None                                                               
* 
* @par Side Effects
*   None
* 
*/
static void add_default_smem_entries(void)
{
  uint32 *platform_info = NULL;
  uint32 platform_info_size = 0;
  uint32 count = 0x0;
  uint32 i;
  
  struct md_smem_table 
  {
    smem_mem_type   smem_id;
    char            *filename;
  };
  
  struct md_smem_table dload_def_smem_entries[ ] = {
    {SMEM_HW_SW_BUILD_ID, "MD_SMEMINFO.BIN"},
    {SMEM_IMAGE_VERSION_TABLE, "MD_SMEM_VER.BIN"},
  };

  count = (sizeof(dload_def_smem_entries)/sizeof(dload_def_smem_entries[0]));
  
  for(i=0;i<count;i++)
  {
    platform_info = smem_get_addr(dload_def_smem_entries[i].smem_id, &platform_info_size);
    if(platform_info != NULL)
    {
      dload_debug_add_region(OPTIONAL_DEF_SAVE,
                            (uint32)platform_info ,  platform_info_size,
                            "SMEM info", dload_def_smem_entries[i].filename);  
    } 
  }
}  


/*===========================================================================

**  Function :  dload_add_pmic_info

** ==========================================================================
*/
/*!
* 
* @brief
*   Add the Reset Status register region
*   
* @par Dependencies
*   This api Must be called after clk_regime_apps_init_boot 
*   
* @retval
*   None                                                               
* 
* @par Side Effects
*   None
* 
*/
static void dload_add_pmic_info(boolean is_md_enabled)
{
  // Dump variable must be declared as static and have section attribute to
  // be added to RamDump Data ZI region.  USB controller can not access locked 
  // TCM region  
  boolean add_region_status = FALSE;
  char pmic_pon_filename[DLOAD_DEBUG_STRLEN_BYTES],pmic_hist_filename[DLOAD_DEBUG_STRLEN_BYTES];
  
  static uint64 pmic_power_on __attribute__((section ("BOOT_INTERNAL_HEAP")));
  static pm_pbs_pon_reason_history_data_type pmic_pon_history __attribute__((section (".bss.BOOT_UNCACHED_ZI_ZONE")));

  if(is_md_enabled)
  {
    qsnprintf (pmic_pon_filename, DLOAD_DEBUG_STRLEN_BYTES, "MD_PMIC_PON.BIN");
    qsnprintf (pmic_hist_filename, DLOAD_DEBUG_STRLEN_BYTES, "MD_PMON_HIS.BIN");    
  }
  else
  {
    qsnprintf (pmic_pon_filename, DLOAD_DEBUG_STRLEN_BYTES, "PMIC_PON.BIN");
    qsnprintf (pmic_hist_filename, DLOAD_DEBUG_STRLEN_BYTES, "PMON_HIS.BIN");    
  }
    
  /* Get power on reason for pmic device 0 */
  boot_pm_dev_get_power_on_reason(0, &pmic_power_on);
  boot_pm_pbs_get_pon_reason_history(&pmic_pon_history);

  add_region_status = dload_debug_add_region(OPTIONAL_DEF_SAVE,
                          (uint32)(&pmic_power_on),
                          (uint32) sizeof(pmic_power_on),
                          "Pmic PON stat", pmic_pon_filename);

  BL_VERIFY((add_region_status == TRUE), BL_ERR_RAM_DUMP_FAIL|BL_ERROR_GROUP_BOOT);
  
  add_region_status = dload_debug_add_region(OPTIONAL_DEF_SAVE, 
                          (uint32)(&pmic_pon_history), 
                          (uint32) sizeof(pmic_pon_history), 
                          "PM PON HIST ", pmic_hist_filename); 
                          
  BL_VERIFY((add_region_status == TRUE), BL_ERR_RAM_DUMP_FAIL|BL_ERROR_GROUP_BOOT);
}


/*===========================================================================

**  Function :  dload_add_reset_status

** ==========================================================================
*/
/*!
* 
* @brief
*   Add the Reset Status register
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
static void dload_add_reset_status(boolean is_md_enabled)
{
  // Dump variable must be declared as static and have section attribute to
  // be added to RamDump Data ZI region.  USB controller can not access locked 
  // TCM region  
  boolean add_region_status = FALSE;  
  char rst_stat_filename[DLOAD_DEBUG_STRLEN_BYTES];

  static uint32 reset_status_register
    __attribute__((section ("BOOT_INTERNAL_HEAP")));
  
  if(is_md_enabled)
    qsnprintf (rst_stat_filename, DLOAD_DEBUG_STRLEN_BYTES, "MD_RST_STAT.BIN");
  else
    qsnprintf (rst_stat_filename, DLOAD_DEBUG_STRLEN_BYTES, "RST_STAT.BIN");

  reset_status_register = sbl1_hw_get_reset_status();
  add_region_status = dload_debug_add_region(OPTIONAL_DEF_SAVE,
                            (uint32)(&reset_status_register),
                            (uint32) sizeof(reset_status_register),
                            "Reset Status Region", rst_stat_filename);
                            
  BL_VERIFY((add_region_status == TRUE),
                   BL_ERR_RAM_DUMP_FAIL|BL_ERROR_GROUP_BOOT);                            
}


/*===========================================================================

**  Function :  dload_add_pimem_region

** ==========================================================================
*/
/*!
* 
* @brief
*   Add Pimem region dumps
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
static void dload_add_pimem_region(void)
{
  boolean add_region_status= FALSE;
  uint32 size_in_mb = 0;
  PIMEM_STATUS pimem_enable_status = -1;
  boot_tzt_result_regs_t result_regs;
  
  if (sbl1_get_current_el() == EL_1)
  {
     boot_fastcall_tz(TZ_PIMEM_GET_STATUS_CMD,
                      TZ_PIMEM_INIT_CMD_PARAM_ID,
                      0,0,0,0,
                      &result_regs);

     pimem_enable_status = (PIMEM_STATUS)result_regs.reg[0];
     size_in_mb = (uint32)result_regs.reg[1];
  }
  if( PIMEM_OK == pimem_enable_status)
  {
    /*Add pimem region */
    add_region_status = dload_debug_add_region(OPTIONAL_DEF_SAVE,
                          (uintnt)SCL_pIMEM_BASE,
                          (uintnt)(size_in_mb << CONVERT_TO_MB_SHIFT),
                          "PIMEM region", "PIMEM.BIN");
                          
    BL_VERIFY((add_region_status == TRUE),
                       BL_ERR_RAM_DUMP_FAIL|BL_ERROR_GROUP_BOOT);                          
  }
  else
  {
    boot_log_message("pImem Disabled. Hence Not added to Dumps.");
  }

}

/*===========================================================================

**  Function :  dload_add_ipa_memory_regions

** ==========================================================================
*/
/*!
* 
* @brief
*   Add IPA region dumps for chip versions v2 onwards
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
static void dload_add_ipa_memory_regions(void)
{
  boolean add_region_status = FALSE;
  struct memory_region dump_regions[] = {IPA_MEMORY_REGION_TABLE}; 
  uint32 index = 0;  

  /* RAM-DUMP table defined in .builds file */
  while ( dump_regions[index].region_base != 0x0 ) 
  {
    add_region_status =  dload_debug_add_region(OPTIONAL_DEF_SAVE, 
                                     dump_regions[index].region_base,
                                     dump_regions[index].region_size,
                                     dump_regions[index].desc,
                                     dump_regions[index].filename
                                   );
                             
   BL_VERIFY((add_region_status == TRUE),
              BL_ERR_RAM_DUMP_FAIL|BL_ERROR_GROUP_BOOT);                              
      index++;
  }
}

/*===========================================================================

**  Function :  dload_add_binning_info

** ==========================================================================
*/
/*!
* 
* @brief
*   Add the Part bin information
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
static void dload_add_binning_info(void)
{
  // Dump variable must be declared as static and have section attribute to
  // be added to RamDump Data ZI region.  USB controller can not access locked 
  // TCM region  
  boolean add_region_status = FALSE;
  static uint32 part_bin_info_register
    __attribute__((section ("BOOT_INTERNAL_HEAP")));

	part_bin_info_register = HWIO_IN(QFPROM_RAW_PTE_ROW1_MSB);

	add_region_status = dload_debug_add_region(OPTIONAL_DEF_SAVE,
                                     (uint32)(&part_bin_info_register),
                                     (uint32) sizeof(part_bin_info_register),
                                     "Part Bin info", "PART_BIN.BIN");
                                     
  BL_VERIFY((add_region_status == TRUE),
              BL_ERR_RAM_DUMP_FAIL|BL_ERROR_GROUP_BOOT);
}

/*===========================================================================

**  Function :  dload_add_aoss_cc_regs

** ==========================================================================
*/
/*!
* 
* @brief
*   Add the aoss_cc register information
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
static void dload_add_aoss_cc_regs(void)
{
  boolean add_region_status = FALSE;
  static uint32 aoss_cc_fsm_status_register __attribute__((section ("BOOT_INTERNAL_HEAP")));
  static uint32 aoss_cc_fsm_ctrl_register __attribute__((section ("BOOT_INTERNAL_HEAP")));

  aoss_cc_fsm_status_register = HWIO_IN(AOSS_CC_RESET_FSM_STATUS);
  aoss_cc_fsm_ctrl_register = HWIO_IN(AOSS_CC_RESET_FSM_CTRL);
  
  add_region_status = dload_debug_add_region(OPTIONAL_DEF_SAVE,
                                     (uint32)(&aoss_cc_fsm_status_register),
                               (uint32) sizeof(aoss_cc_fsm_status_register),
                                        "Aoss rst fsm stat", "FSM_STS.BIN");

  BL_VERIFY((add_region_status == TRUE),
            BL_ERR_RAM_DUMP_FAIL|BL_ERROR_GROUP_BOOT);                                         
  
  add_region_status = dload_debug_add_region(OPTIONAL_DEF_SAVE,
                                        (uint32)(&aoss_cc_fsm_ctrl_register),
                                  (uint32) sizeof(aoss_cc_fsm_ctrl_register),
                                        "Aoss rst fsm ctrl", "FSM_CTRL.BIN");
  BL_VERIFY((add_region_status == TRUE),
            BL_ERR_RAM_DUMP_FAIL|BL_ERROR_GROUP_BOOT);                                            
}


/*===========================================================================

 ** Function :  dload_add_minidump_regions

** ==========================================================================
*/
static void dload_add_minidump_regions(void)
{ 
  /* Add additional regions for Minidump case, pass info on oem key, so that HLOS regions
     can be zeroed */
  add_minidump_regions();
  
  start_index_default = dload_mem_debug_num_ent();
  add_default_smem_entries();
  dload_add_pmic_info(TRUE);
  dload_add_reset_status(TRUE);
  
  dload_debug_add_region(OPTIONAL_DEF_SAVE, 
                        (uint64)SHARED_IMEM_BASE, 
    					SHARED_IMEM_SIZE,
                        "Shared IMEM", "MD_SHRDIMEM.BIN"); 							
}


/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_POST_TARGET_INIT

DESCRIPTION
  This function does any post processing to be done after debug table 
  initialization.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/                                                                         
void dload_mem_debug_post_target_init(void)
{
  boot_boolean is_auth_enabled = FALSE;	 
  boot_is_auth_enabled(&is_auth_enabled);	  
	
  /* Add encryption artifacts */	
  if ((dload_flag & SBL_MINIDUMP_MODE_BIT_MASK) && (is_auth_enabled))
  {
    dload_debug_check_oem_key(start_index_default,dload_mem_debug_num_ent());	  
    dload_debug_encr_all_regions(start_index_default,dload_mem_debug_num_ent()); 	  	
  	dload_debug_add_unencr_regions();
  } 	  	  
}

/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_TARGET_INIT

DESCRIPTION
  This function initializes clocks as needed for this target, as well as sets
  up the memory regions to be used.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/                                                                          
void dload_mem_debug_target_init(void)
{
  boolean add_region_status = FALSE;
  uintnt start_addr =0;
  uint64 size =0;
  uint32 index =0;
  sbl_if_shared_ddr_device_info_type *available_ddr;
  char ddr_string_info[DLOAD_DEBUG_STRLEN_BYTES],ddr_filename[DLOAD_DEBUG_STRLEN_BYTES];
  uint32 file_cnt =0 ;
  
  struct memory_region dump_regions[] = {MEMORY_REGION_TABLE}; 
  
  /* Set memory region table to be fixed length, required by sahara*/
  dload_mem_debug_len_init();

  /* Check if device is retail unlocked. Do not dump internal memories 
     in retail unlock scenario.  Also zero peripheral memory so it does not
     appear in DDR dump. */
  if (dload_mem_debug_is_device_retail_unlocked()) 
  {
    dump_regions[0].region_base = 0x0;
    dump_regions[0].region_size = 0x0;
    dump_regions[0].desc = NULL;
    dump_regions[0].filename = NULL;

    dload_mem_debug_zero_peripheral_memory();
  }
  
  dload_flag = boot_dload_read_saved_cookie();
  
/*
 ********************************WARNING**************************************
 
 Please make sure all dump region file names follow the 8.3 file name format!
 
 *****************************************************************************
*/
  /* Check if DLOAD flag is set, if not only minidump regions are to be dumped */
  if(dload_flag & SBL_DLOAD_MODE_BIT_MASK)
  { 
    index = 0;
    
    /* RAM-DUMP table defined in .builds file */
    while ( dump_regions[index].region_base != 0x0 ) 
    {
        add_region_status = dload_debug_add_region(OPTIONAL_DEF_SAVE, 
                                 dump_regions[index].region_base,
                                 dump_regions[index].region_size,
                                 dump_regions[index].desc,
                                 dump_regions[index].filename
                               );
         if(add_region_status == FALSE)
         {
           break;         
         }
          
        index++;
    }
    BL_VERIFY((add_region_status == TRUE),
                 BL_ERR_RAM_DUMP_FAIL|BL_ERROR_GROUP_BOOT);
     
    /* Add IPA memory dumps */
    dload_add_ipa_memory_regions();  
    
    /* Add Pmic power on reason*/
    dload_add_pmic_info(FALSE);  
    
    /* Add reset status */ 
    dload_add_reset_status(FALSE);  
    
    /* Add ddr training data */
    dload_add_ddr_training_data();
    
    /* Add pImem region */
    dload_add_pimem_region();  
    
    /* Add the aoss_cc register information */
    dload_add_aoss_cc_regs();

    dload_add_raw_partition();	
     
    index = 0;
    available_ddr = boot_get_ddr_info();
    while(index < available_ddr->noofddr)
    {
       start_addr = available_ddr->ddr_info[index].cs_addr;
       size = available_ddr->ddr_info[index].ramsize << CONVERT_TO_MB_SHIFT;
    
      /* Define DDR Memory Region  EBI1 CS0/CS1 etc */
      file_cnt = 0;
      do {
        qmemset(ddr_string_info,0,DLOAD_DEBUG_STRLEN_BYTES);
        qmemset(ddr_filename,0,DLOAD_DEBUG_STRLEN_BYTES);
        qsnprintf (ddr_string_info, DLOAD_DEBUG_STRLEN_BYTES, " DDR CS%u part%u Memory", index, file_cnt);
        qsnprintf (ddr_filename, DLOAD_DEBUG_STRLEN_BYTES, "DDRCS%u_%u.BIN", index, file_cnt);
    
        if(size >= 0x80000000) {
          add_region_status = dload_debug_add_region(OPTIONAL_DEF_SAVE, start_addr + 0x80000000 * file_cnt, 0x80000000, 
                                 ddr_string_info, ddr_filename);
                                 
          BL_VERIFY((add_region_status == TRUE),
                     BL_ERR_RAM_DUMP_FAIL|BL_ERROR_GROUP_BOOT);                               
          size -= 0x80000000;
        }
        else {
          add_region_status = dload_debug_add_region(OPTIONAL_DEF_SAVE, start_addr + 0x80000000 * file_cnt, size, 
                                 ddr_string_info, ddr_filename);
                                 
          BL_VERIFY((add_region_status == TRUE),
                     BL_ERR_RAM_DUMP_FAIL|BL_ERROR_GROUP_BOOT);                               
          size -= size;
        }
        file_cnt++;
      } while(size) ;
     index++;
    }

  }

  /* Add the minidump regions, if enabled */  
  if(dload_flag & SBL_MINIDUMP_MODE_BIT_MASK)
  {	
     dload_add_minidump_regions();  
  }
}


/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_SUPPORT

DESCRIPTION
  This function returns if memory debug is supported.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean dload_mem_debug_supported()
{
  boolean mem_dump_supported = TRUE;
  bl_shared_data_type * bl_shared_data_ptr;
  int32 sbl1_entry = 0;

  bl_shared_data_ptr = sbl1_get_shared_data();

  sbl1_entry = boot_sbl_qsee_interface_get_image_entry(&(bl_shared_data_ptr->sbl_qsee_interface), SECBOOT_SBL1_SW_TYPE);

  BL_VERIFY(sbl1_entry != -1,
                BL_ERR_IMG_NOT_FOUND|BL_ERROR_GROUP_BOOT);

  mem_dump_supported = (boolean)boot_qsee_is_memory_dump_allowed(&(bl_shared_data_ptr->sbl_qsee_interface.boot_image_entry[sbl1_entry].image_verified_info));

  return mem_dump_supported;
}


/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_IS_DEVICE_RETAIL_UNLOCKED

DESCRIPTION
  This function returns if device is retail unlocked
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean dload_mem_debug_is_device_retail_unlocked(void)
{
  return FALSE;
}


#endif /*FEATURE_DLOAD_MEM_DEBUG*/
