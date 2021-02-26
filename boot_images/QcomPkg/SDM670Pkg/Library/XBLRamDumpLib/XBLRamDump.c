/** @file XBLRamDump.c
  Top-Level Logic for XBLRamDump.c
  
  Copyright (c) 2014-2018, Qualcomm Technologies, Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/05/18   aus     Added support to handle display not supported scenario
 09/06/17   yps     Remove efs_boot_initialize 
 08/10/17   yps     Initialize ramdump data zi region
 05/02/17   yps     Added Dal init and init display only once
 05/09/17   sj      Added the logic to enable display if battery threshold match
 04/27/17   kpa     Update dalsys free api argument.
 05/19/16   kpa     Added support for Display
 09/10/15   kpa     Use dcache_flush_region instead of mmu_flush_cache.
 08/24/15   ck      Added logic to enter PBL EDL if ramdump is not allowed
 09/30/14   ck      efs_boot_initialize must be called before any storage functions
 08/08/14   ck      Added boot_shared_functions_consumer.h
 07/28/14   ck      Initial revision

=============================================================================*/

#include "boot_target.h"
#include "boothw_target.h"
#include "boot_dload_debug.h"
#include "boot_raw_partition_ramdump.h"
#include "boot_sd_ramdump.h"
#include "boot_sahara.h"
#include "boot_extern_efs_interface.h"
#include "boot_shared_functions_consumer.h"
#include "BootDisplay.h"
#include "pm_fg_batt_info.h"
#include "boot_util.h"

extern uintnt Image$$RAMDUMP_DATA_ZI$$Base[];
extern uintnt Image$$RAMDUMP_DATA_ZI$$Length[];
/*===========================================================================
**  Function :  XBLRamDumpDisplayInfo
** ==========================================================================
*/
/*!
* 
* @brief
*   This function displays messages onto screen
*
* @param[in] 
*   None
*
* @par Dependencies
*   None 
* 
* @retval
*   None
* 
*/ 

boolean display_initialization_needed = TRUE;

static void XBLRamDumpDisplayInfo(void)
{
  uint8 *boot_log_base_ptr = NULL;
  uintnt boot_log_init_info_size = 0 ;
  boolean status = FALSE;
  DALResult result;

  uint8* scaled_buffer = NULL;  
  /*
    for re-enter sahara-download mode case, skip re-initializing display .
  */
  if (display_initialization_needed == TRUE) 
  {
    if (boot_display_init())
	{
       /* Example use of display scaling api*/
       
       /*Allocate buffer for image scaling */
       result = DALSYS_Malloc(GLYPH_BUFF_3X_SIZE_BYTES, (void *)&scaled_buffer);  
       BL_VERIFY(result == DAL_SUCCESS, BL_ERROR_GROUP_DAL|result);
       
       boot_display_set_font_color(COLOR_BLUE);  
       boot_display_scaled_message((uint8 *)"\n\n\rQUALCOMM ", 
                                 SCALE_3X, scaled_buffer, GLYPH_BUFF_3X_SIZE_BYTES);
       
       boot_display_set_scale_factor(SCALE_3X);
       boot_display_set_font_color(COLOR_RED);  
       boot_display_message((uint8 *)"CrashDump Mode\n\r");
       
       boot_display_set_font_color(COLOR_YELLOW);  
       boot_display_message((uint8 *)"__________________________\n\n\r");
                                      
       boot_display_set_font_color(COLOR_WHITE);  
       status = boot_log_get_init_info(&boot_log_base_ptr, &boot_log_init_info_size);
       if (status == TRUE)
       {
         boot_display_buffer(boot_log_base_ptr, boot_log_init_info_size, 
                             SCALE_2X, scaled_buffer, GLYPH_BUFF_2X_SIZE_BYTES);
       }
       
       /* Free Allocated buffers */
       result = DALSYS_Free((void *)scaled_buffer);  
       BL_VERIFY(result == DAL_SUCCESS, BL_ERROR_GROUP_DAL|result);
    }	
	display_initialization_needed = FALSE;
  }
}

/*===========================================================================
**  Function :  XBLRamDumpMain
** ==========================================================================
*/
/*!
* 
* @brief
*   This function is entry point for XBL Ramdump image
*
* @param[in] 
*   None
*
* @par Dependencies
*   None 
* 
* @retval
*   None
* 
*/ 
VOID XBLRamDumpMain( VOID )
{
  struct boot_sahara_interface* sbl_sahara_interface = NULL;
  uint32 dload_flag =0x0;

  dload_flag = boot_dload_read_saved_cookie() & (SBL_MINIDUMP_MODE_BIT_MASK | SBL_DLOAD_MODE_BIT_MASK);
  if(dload_flag == SBL_MINIDUMP_MODE_BIT_MASK)
  {
    /* Enable PMIC WDG timer only for Minidump case, since full-dumps to eMMC/SDCard/QPST will not complete within PMIC WDG expiry */
    boot_pm_pon_wdog_cfg(PMIC_DEVICE_INDEX, DLOAD_PMIC_WDG_S1_VALUE, DLOAD_PMIC_WDG_S1_VALUE, PM_PON_RESET_CFG_HARD_RESET);
    boot_pm_pon_wdog_enable(PMIC_DEVICE_INDEX, PM_ON);
  }
  
  qmemset(Image$$RAMDUMP_DATA_ZI$$Base,0,(uint32)Image$$RAMDUMP_DATA_ZI$$Length);
  dcache_flush_region((void *)Image$$RAMDUMP_DATA_ZI$$Base,(uintnt)Image$$RAMDUMP_DATA_ZI$$Length);
  /* Inform the shared functions library where the shared functions table is
     and verify that it was initialized by the producer. */
  boot_shared_functions_init();
  
  boot_DALSYS_InitMod(NULL);
  
  /* configure power key to do a hardreset */
  boot_set_pwrkey_reset_type(BOOT_HARD_RESET_TYPE);
  
  /*configure pshold to hardreset */
  boot_set_pshold_reset_type(BOOT_HARD_RESET_TYPE);

  if (boot_check_display_allowed())
  {
    XBLRamDumpDisplayInfo();
  }
  
  /* If download mode is not available then no need to continue and spin.
     Enter PBL EDL in this scenario. */
  if(!dload_mem_debug_supported())
  {
    boot_dload_transition_pbl_forced_dload();
  }


 /*-----------------------------------------------------------------------
   * Ram dump to eMMC raw partition, this function will reset device 
   * after successful dump collection if cookie is set 
   *----------------------------------------------------------------------*/
  boot_ram_dump_to_raw_parition();
  
  
#ifdef FEATURE_BOOT_RAMDUMPS_TO_SD_CARD   
  /*----------------------------------------------------------------------
   * Take the Ramdumps to SD card if  cookie file is
   *   present in SD card 
   *---------------------------------------------------------------------*/
  boot_ram_dumps_to_sd_card();
#endif /*FEATURE_BOOT_RAMDUMPS_TO_SD_CARD*/  

  /* Disable PMIC for QPST case, since QPST attach is manual and may result in WDOG expiry */
  boot_pm_pon_wdog_enable(PMIC_DEVICE_INDEX, PM_OFF);  
  
  /* Enter Sahara */

  /* Get Sahara interface */
  sbl_sahara_interface = sbl_sahara_get_interface();
  BL_VERIFY(sbl_sahara_interface != NULL, BL_ERROR_GROUP_BOOT|BL_ERR_NULL_PTR_PASSED);
  
  /* Set Sahara mode to memory debug */
  sbl_sahara_interface->sahara_mode = SAHARA_MODE_MEMORY_DEBUG;
  
  /* Flush the cache before calling into sahara so that all data is flushed to memory */  
  dcache_flush_region((void *)SCL_SBL1_DDR_ZI_BASE, SCL_SBL1_DDR_ZI_SIZE);
  dcache_flush_region((void *)SCL_SBL1_OCIMEM_DATA_BASE, SCL_SBL1_OCIMEM_DATA_SIZE);
  
  /* Enter Sahara */
  boot_sahara_entry(sbl_sahara_interface);
  
  boot_display_deinit();
}

