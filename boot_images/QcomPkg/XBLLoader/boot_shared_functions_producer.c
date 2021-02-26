/*=============================================================================

                       Boot Shared Functions Producer

GENERAL DESCRIPTION
  This file contains definitions of functions for boot shared functions.

Copyright 2014 - 2017 by QUALCOMM Technologies Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/30/17   ks      Added LCDB api's for Ramdump Display
10/12/17   aus     Added functions to support secure minidump
09/28/17   rohik   Added functions for support of raw partitions dump collection  
08/28/17   aus     Added coldplug APIs
08/10/17   yps     Replaced boot_DALSYS_HeapDeInit with boot_DALSYS_HeapForceDeInit
08/04/17   yps     Share platform-id APIs from XBL-loader->XBL-ramdump over shared-function-table interface
08/01/17   czq     Export WLED/LAB/IBB/LDO APIs for display.
07/06/17   vg      Added DAL APIs in shared lib
07/05/17   jch     Added pm_ldo_sw_enable
06/23/17   ds      coldplug changes
05/09/17   sj      Added Api to qurey the battery status in XBL ramdump
05/02/17   yps     Added Apis for ramdump reenter 
04/17/17   yps     remove boot_qsee_is_retail_unlocked
04/12/17   kpa     Added Clock_api to disable usb enumeration
04/11/17   kpa     Added Clock_api to enable usb enumeration
03/03/17   ds	   remove clobber prot
03/02/17   ds	   add whitelist functions
12/14/16   jt      Add SDCC clock functions 
06/23/16   kpa     Added api's to support Display in ramdump image
06/20/16   kpa     Added DALSYS_Malloc and DALSYS_Free functions
06/13/16   jch     added pm_schg_usb_get_typec_status
04/06/16   kpa     rename boot_fastcall_scm_2arg to boot_fastcall_scm
03/09/16   kpa     remove cache api's from shared functions, add fastcall
02/26/16   aab     Updated to match latest pmic charger driver
01/12/16   kameya  Rename qusb_pbl_dload_check to qusb_ldr_utils_fedl_check
09/16/15   kpa     Added dcache_inval_region
08/24/15   ck      Added boot_dload_transition_pbl_forced_dload
08/06/15   as      Use shared functions
08/01/15   kpa     Added dcache_flush_region
07/28/15   rp      Added Clock_SetBIMCSpeed
06/25/15   as      Added sbl1_get_shared_data and boot_sbl_qsee_interface_get_image_entry
04/23/15   kpa     Added pmic and hotplug apis
10/01/14   ck      Removed Hotplug functions
09/30/14   ck      Added boot_err_fatal
09/30/14   ck      Removing efs functions as EFS driver properly split
09/23/14   ck      Added boot extern crypto, efs, and seccfg interface functions
07/15/14   ck      Intial creation
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_shared_functions_producer.h"


void
boot_shared_functions_register(void)
{
  boot_shared_functions_type * function_table = 
    (boot_shared_functions_type *)SCL_SBL1_SHARED_FUNCTIONS_TABLE_BASE;


  /* Assign each function pointer here.  When adding new functions simply
     add to this list. */

  function_table->boot_CeMLDeInit = &boot_CeMLDeInit;
  function_table->boot_CeMLHashDeInit = &boot_CeMLHashDeInit;
  function_table->boot_CeMLHashFinal = &boot_CeMLHashFinal;
  function_table->boot_CeMLHashInit = &boot_CeMLHashInit;
  function_table->boot_CeMLHashSetParam = &boot_CeMLHashSetParam;
  function_table->boot_CeMLHashUpdate = &boot_CeMLHashUpdate;
  function_table->boot_CeMLInit = &boot_CeMLInit;
  function_table->boot_set_entry_as_img_whitelist = &boot_set_entry_as_img_whitelist;
  function_table->boot_whitelist_address_range_check = &boot_whitelist_address_range_check;
  function_table->boot_invalidate_img_whitelist = &boot_invalidate_img_whitelist;
  function_table->boot_statistics_flash_read_start = &boot_statistics_flash_read_start;
  function_table->boot_statistics_flash_read_stop = &boot_statistics_flash_read_stop;
  function_table->boot_dload_transition_pbl_forced_dload = &boot_dload_transition_pbl_forced_dload;
  function_table->boot_dload_set_cookie = &boot_dload_set_cookie;
  function_table->boot_dload_read_saved_cookie = &boot_dload_read_saved_cookie;  
  function_table->boot_enable_led = &boot_enable_led;
  function_table->boot_err_fatal = &boot_err_fatal;
  function_table->boot_error_handler = bl_err_if.error_handler;
  function_table->boot_flash_dev_if_get_singleton = &boot_flash_dev_if_get_singleton;
  function_table->boot_get_ddr_info = &boot_get_ddr_info;
  function_table->boot_hw_reset = &boot_hw_reset;
  function_table->boot_install_error_callback = bl_err_if.install_callback;
  function_table->boot_log_message = &boot_log_message;
  function_table->boot_pbl_is_auth_enabled = &boot_pbl_is_auth_enabled;
  function_table->boot_is_auth_enabled = &boot_is_auth_enabled;
  function_table->boot_auth_load_header = &boot_auth_load_header;
  function_table->boot_update_auth_image_info = &boot_update_auth_image_info;
  function_table->boot_auth_image = &boot_auth_image;
  function_table->boot_pm_dev_get_power_on_reason = &boot_pm_dev_get_power_on_reason;
  function_table->boot_pm_pbs_get_pon_reason_history = &boot_pm_pbs_get_pon_reason_history;
  function_table->boot_pm_pon_wdog_cfg = &boot_pm_pon_wdog_cfg;
  function_table->boot_pm_pon_wdog_enable = &boot_pm_pon_wdog_enable;  
  function_table->boot_qsee_is_memory_dump_allowed = &boot_qsee_is_memory_dump_allowed;
  function_table->boot_qsee_zero_peripheral_memory = &boot_qsee_zero_peripheral_memory;
  function_table->boot_toggle_led = &boot_toggle_led;
  function_table->boot_toggle_led_init = &boot_toggle_led_init;
  function_table->boot_sbl_qsee_interface_get_image_entry = &boot_sbl_qsee_interface_get_image_entry;

  function_table->sbl1_get_shared_data = &sbl1_get_shared_data;
  function_table->sbl_error_handler = &sbl_error_handler;
  function_table->sbl1_hw_get_reset_status = &sbl1_hw_get_reset_status;
  function_table->sbl1_load_ddr_training_data_from_partition = &sbl1_load_ddr_training_data_from_partition;

  function_table->boot_coldplug_get_partition_size_by_image_id = &boot_coldplug_get_partition_size_by_image_id;
  function_table->dev_sdcc_write_bytes = &dev_sdcc_write_bytes;
  function_table->dev_sdcc_read_bytes = &dev_sdcc_read_bytes;
  function_table->boot_get_coldplug_dev_handle = &boot_get_coldplug_dev_handle;   
  
  function_table->boot_set_pwrkey_reset_type = &boot_set_pwrkey_reset_type;
  function_table->boot_set_pshold_reset_type = &boot_set_pshold_reset_type;

  /*API's used by USB driver */
  function_table->pm_schg_usb_irq_status = &pm_schg_usb_irq_status;
  function_table->pm_schg_usb_get_typec_status = &pm_schg_usb_get_typec_status;
  function_table->qusb_ldr_utils_fedl_check = &qusb_ldr_utils_fedl_check;
  
  function_table->Clock_SetBIMCSpeed = &Clock_SetBIMCSpeed;

  function_table->boot_scm = &boot_scm;
  function_table->sbl1_get_current_el = &sbl1_get_current_el;
  function_table->DALSYS_Malloc = &DALSYS_Malloc;
  function_table->DALSYS_Free = &DALSYS_Free;
  
  function_table->DAL_DeviceAttach=&DAL_DeviceAttach;
  function_table->DAL_DeviceAttachEx=&DAL_DeviceAttachEx;
  function_table->DAL_DeviceDetach=&DAL_DeviceDetach;
  
  function_table->DALSYS_GetDALPropertyHandle=&DALSYS_GetDALPropertyHandle;
  function_table->DALSYS_GetDALPropertyHandleStr=&DALSYS_GetDALPropertyHandleStr;
  function_table->DALSYS_GetPropertyValue=&DALSYS_GetPropertyValue;
  
  function_table->DALSYS_memset=&DALSYS_memset;
  function_table->DALSYS_LogEvent=&DALSYS_LogEvent;
  function_table->DALSYS_BusyWait=&DALSYS_BusyWait;
  
  function_table->DALSYS_EventCreate=&DALSYS_EventCreate;
  function_table->DALSYS_DestroyObject=&DALSYS_DestroyObject;
  function_table->DALSYS_EventCtrlEx=&DALSYS_EventCtrlEx;
  function_table->DALSYS_SyncCreate=&DALSYS_SyncCreate;
  function_table->DALSYS_MemRegionAlloc=&DALSYS_MemRegionAlloc;
  function_table->DALSYS_MemInfo=&DALSYS_MemInfo;
  function_table->DALSYS_EventMultipleWait=&DALSYS_EventMultipleWait;
  
  function_table->busywait = &busywait;
  function_table->busywait_init = &busywait_init;
  function_table->boot_log_get_init_info = &boot_log_get_init_info;  
  function_table->Clock_InitUSB = &Clock_InitUSB;
  function_table->Clock_SetSDCClockFrequencyExt = &Clock_SetSDCClockFrequencyExt;  
  function_table->Clock_SetSDCClockFrequency = &Clock_SetSDCClockFrequency;  

  function_table->Clock_Usb30EnableSWCollapse = &Clock_Usb30EnableSWCollapse;
  function_table->Clock_Usb30GetSWCollapse = &Clock_Usb30GetSWCollapse;
  function_table->Clock_DisableUSB = &Clock_DisableUSB;
  function_table->Clock_SetQUPV3Frequency = &Clock_SetQUPV3Frequency;
  
  function_table->boot_restore_ramdump_sp=&boot_restore_ramdump_sp;
  function_table->boot_DALSYS_InitMod = &boot_DALSYS_InitMod;
  function_table->boot_DALSYS_DeInitMod = &boot_DALSYS_DeInitMod;
  function_table->sbl1_get_external_heap_addr=&sbl1_get_external_heap_addr;
  function_table->sbl1_get_external_heap_size=&sbl1_get_external_heap_size;
  function_table->boot_DALSYS_HeapInit=&boot_DALSYS_HeapInit;
  function_table->boot_DALSYS_HeapDeInit=&boot_DALSYS_HeapForceDeInit;
  function_table->DALSYS_CacheCommand=&DALSYS_CacheCommand;
  function_table->DALSYS_memscpy=&DALSYS_memscpy;
  
  function_table->boot_check_display_vbatt = &boot_check_display_vbatt;
  function_table->pm_ldo_sw_enable = &boot_pm_ldo_sw_enable;
  function_table->pm_ldo_volt_level = &boot_pm_ldo_volt_level;

  function_table->pm_ibb_lcd_amoled_sel = &boot_pm_ibb_lcd_amoled_sel;
  function_table->pm_ibb_set_soft_strt_chgr_resistor = &boot_pm_ibb_set_soft_strt_chgr_resistor;
  function_table->pm_ibb_ibb_module_rdy = &boot_pm_ibb_ibb_module_rdy;
  function_table->pm_ibb_config_ibb_ctrl = &boot_pm_ibb_config_ibb_ctrl;
  function_table->pm_ibb_get_ibb_status = &boot_pm_ibb_get_ibb_status;

  function_table->pm_lab_config_current_sense = &boot_pm_lab_config_current_sense;
  function_table->pm_lab_config_pulse_skip_ctrl = &boot_pm_lab_config_pulse_skip_ctrl;
  function_table->pm_lab_lcd_amoled_sel = &boot_pm_lab_lcd_amoled_sel;
  function_table->pm_lab_lab_module_rdy = &boot_pm_lab_lab_module_rdy;
  function_table->pm_lab_ibb_rdy_en = &boot_pm_lab_ibb_rdy_en;
  function_table->pm_lab_get_lab_status = &boot_pm_lab_get_lab_status;

  function_table->pm_wled_sel_ovp_thr = &boot_pm_wled_sel_ovp_thr;
  function_table->pm_wled_sel_bst_ilimit = &boot_pm_wled_sel_bst_ilimit;
  function_table->pm_wled_sel_slew_rate = &boot_pm_wled_sel_slew_rate;
  function_table->pm_wled_set_fsw_ctrl = &boot_pm_wled_set_fsw_ctrl;
  function_table->pm_wled_set_led_duty_cycle = &boot_pm_wled_set_led_duty_cycle;

  function_table->pm_wled_enable_current_sink = &boot_pm_wled_enable_current_sink;
  function_table->pm_wled_enable_modulator = &boot_pm_wled_enable_modulator;
  function_table->pm_wled_enable_module = &boot_pm_wled_enable_module;
  function_table->pm_wled_en_sync = &boot_pm_wled_en_sync;
  
  
  function_table->pm_wled_en_amoled = &boot_pm_wled_en_amoled;
  function_table->pm_wled_set_amoled_vout = &boot_pm_wled_set_amoled_vout;
  
  function_table->pm_lcdb_module_enable = &boot_pm_lcdb_module_enable;
  function_table->pm_lcdb_set_module_rdy = &boot_pm_lcdb_set_module_rdy;
  
  function_table->pm_fg_batt_info_get_vbatt = &boot_pm_fg_batt_info_get_vbatt;
  
  function_table->boot_DalPlatformInfo_PlatformInfo = &boot_DalPlatformInfo_PlatformInfo;
  function_table->boot_DalPlatformInfo_ChipId = &boot_DalPlatformInfo_ChipId;
  function_table->boot_DalPlatformInfo_ChipFamily = &boot_DalPlatformInfo_ChipFamily;
  
  function_table->coldplug_iter_open = &coldplug_iter_open;
  function_table->coldplug_iter_next = &coldplug_iter_next;
  function_table->coldplug_iter_close = &coldplug_iter_close;
  function_table->coldplug_write = &coldplug_write;
  function_table->coldplug_get_size = &coldplug_get_size;
  function_table->coldplug_read = &coldplug_read;
  function_table->coldplug_close_partition = &coldplug_close_partition;
  function_table->coldplug_open_device = &coldplug_open_device;
  function_table->coldplug_open_partition = &coldplug_open_partition;  
  function_table->coldplug_get_start_lba = &coldplug_get_start_lba;
  function_table->smem_init = &smem_init;
  function_table->smem_get_addr = &smem_get_addr;
  
  /* Fill in version number and magic cookie values so consumer knows table has
     been populated. */
  function_table->version = SHARED_FUNCTIONS_VERSION;
  function_table->magic_cookie_1 = SHARED_FUNCTIONS_MAGIC_COOKIE_1;
  function_table->magic_cookie_2 = SHARED_FUNCTIONS_MAGIC_COOKIE_2;
}

