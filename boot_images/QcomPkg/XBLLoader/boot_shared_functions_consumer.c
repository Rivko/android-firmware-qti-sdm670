/*=============================================================================

                       Boot Shared Functions Consumer

GENERAL DESCRIPTION
  This file contains definitions of functions for boot shared functions

Copyright 2014 - 2018 by QUALCOMM Technologies Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/28/17   yps     Corrected the case of boot_DalPlatformInfo_PlatformInfo 
10/30/17   ks      Added LCDB api's for Ramdump Display
10/12/17   aus     Added functions to support secure minidump
09/28/17   rohik   Added functions for support of raw partitions dump collection  
08/28/17   aus     Added coldplug APIs
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
10/03/16   kpa     Added Clock_InitUSB api
06/23/16   kpa     Added api's to support Display in ramdump image
06/09/16   kpa     Added DALSYS_Malloc and DALSYS_Free functions
06/13/16   jch     added pm_schg_usb_get_typec_status
04/06/16   kpa     rename boot_fastcall_scm_2arg to boot_fastcall_scm
03/09/16   kpa     remove cache api's from shared functions, add fastcall
01/12/16   ka      Renamed qusb_pbl_dload_check() to qusb_ldr_utils_fedl_check()
09/16/15   kpa     Added dcache_inval_region
08/24/15   ck      Added boot_dload_transition_pbl_forced_dload
08/06/15   as      Use shared functions
08/01/15   kpa     Added dcache_flush_region
07/28/15   rp      Added Clock_SetBIMCSpeed
07/11/15   rp	   Changed boot_enable_led function to add an additional parameter
06/25/15   as      Added sbl1_get_shared_data and boot_sbl_qsee_interface_get_image_entry
04/23/15   kpa     Added pmic and hotplug apis
10/01/14   ck      Removed Hotplug functions
09/30/14   ck      Added boot_err_fatal
09/30/14   ck      Removing efs functions as EFS driver properly split
09/23/14   ck      Added boot_extern_crypto_interface functions
09/23/14   ck      Added boot_extern_efs_interface functions
09/23/14   ck      Added boot_extern_seccfg_interface functions
08/05/14   ck      Updated boot_clobber_check_global_whitelist_range prototype
07/14/14   ck      Intial creation
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_shared_functions.h"


/* Following prototypes were copied from boot_error_handler.c */
void boot_error_handler
(
  const char* filename_ptr,     /* File this function was called from -
                                   defined by __FILE__ */
  uint32      line,             /* Line number this function was called
                                   from - defined by  __LINE__ */
  uint32      err_code          /* Enum that describes error type */
);
static void boot_install_error_callback
(
  bl_error_callback_func_type,
  void *,
  bl_error_callback_node_type *node_ptr
);


/* Assign boot shared functions table to memory carved out in scatter load */
static boot_shared_functions_type * shared_functions_table =
  (boot_shared_functions_type *) SCL_SBL1_SHARED_FUNCTIONS_TABLE_BASE;


/* Boot error handler uses a global bl_err_if structure.  Create a new one
   here and assign with functions from the producer. */
boot_error_if_type bl_err_if = 
{
  boot_error_handler,
  boot_install_error_callback
};


boolean boot_shared_functions_init(void)
{
  boolean init_valid = FALSE;


  /* Verify that the shared functions table is the proper version and it was
     filled out by a producer. */
  if ((shared_functions_table->version == SHARED_FUNCTIONS_VERSION) &&
      (shared_functions_table->magic_cookie_1 == SHARED_FUNCTIONS_MAGIC_COOKIE_1) &&
      (shared_functions_table->magic_cookie_2 == SHARED_FUNCTIONS_MAGIC_COOKIE_2))
  {
    init_valid = TRUE;
  }
  
  return init_valid;
}


boolean boot_check_display_allowed()
{
	return shared_functions_table->boot_check_display_vbatt();
}


/* The following functions are the "shim" layer between the original function
   call and execution via function pointer. For each entry in the boot shared
   functions type structure there must be a shim. */

boot_flash_dev_if_type * boot_flash_dev_if_get_singleton(void)
{
  return shared_functions_table->boot_flash_dev_if_get_singleton();
}


void boot_error_handler(const char * filename_ptr,
                        uint32 line,
                        uint32 error)
{
  shared_functions_table->boot_error_handler(filename_ptr,
                                             line,
                                             error);
}


void boot_install_error_callback(bl_error_callback_func_type cb_func,
                                 void * data_ptr,
                                 bl_error_callback_node_type * node_ptr)
{
  shared_functions_table->boot_install_error_callback(cb_func,
                                                      data_ptr,
                                                      node_ptr);
}


boot_boolean boot_enable_led(uint32 led_color, boot_boolean led_on)
{
  return shared_functions_table->boot_enable_led(led_color, led_on);
}


void boot_hw_reset(boot_hw_reset_type reset_type)
{
  shared_functions_table->boot_hw_reset(reset_type);
}


boot_boolean boot_toggle_led(void)
{
  return shared_functions_table->boot_toggle_led();
}


void boot_toggle_led_init(void)
{
  shared_functions_table->boot_toggle_led_init();
}


void boot_log_message(char * message)
{
  shared_functions_table->boot_log_message(message);
}


boolean boot_set_entry_as_img_whitelist(uint64 start_addr, uint64 size)
{
  return shared_functions_table->boot_set_entry_as_img_whitelist(start_addr, size);
}

boolean boot_whitelist_address_range_check(uint64 start_addr,uint64 size,buffer_type buf_typ)
{
  return shared_functions_table->boot_whitelist_address_range_check(start_addr,size,buf_typ);
}

void boot_invalidate_img_whitelist(void)
{
  return shared_functions_table->boot_invalidate_img_whitelist();
}

void boot_statistics_flash_read_start(void)
{
  return shared_functions_table->boot_statistics_flash_read_start();
}

void boot_statistics_flash_read_stop(uint32 bytes_read)
{
  return shared_functions_table->boot_statistics_flash_read_stop(bytes_read);
}

void boot_dload_transition_pbl_forced_dload(void)
{
  shared_functions_table->boot_dload_transition_pbl_forced_dload();
}


void boot_dload_set_cookie(void)
{
  shared_functions_table->boot_dload_set_cookie();
}

uint32 boot_dload_read_saved_cookie(void)
{
  return shared_functions_table->boot_dload_read_saved_cookie();
}

boot_boolean boot_pbl_is_auth_enabled(void)
{
  return shared_functions_table->boot_pbl_is_auth_enabled();
}


secboot_hw_etype boot_is_auth_enabled(boot_boolean * is_auth_enabled)
{
  return shared_functions_table->boot_is_auth_enabled(is_auth_enabled);
}


void boot_auth_load_header( mi_boot_image_header_type* image_header )
{
  return shared_functions_table->boot_auth_load_header(image_header);
}


void boot_update_auth_image_info( uint8* hash_buffer_base_addr )
{
  return shared_functions_table->boot_update_auth_image_info(hash_buffer_base_addr);
}


bl_error_boot_type boot_auth_image( struct bl_shared_data_type *bl_shared_data, uint32 image_type )
{
  return shared_functions_table->boot_auth_image(bl_shared_data, image_type);
}


bl_shared_data_type * sbl1_get_shared_data(void)
{
  return shared_functions_table->sbl1_get_shared_data();
}


void sbl_error_handler(void)
{
  shared_functions_table->sbl_error_handler();
}


sbl_if_shared_ddr_device_info_type * boot_get_ddr_info(void)
{
  return shared_functions_table->boot_get_ddr_info();
}


uint32 sbl1_hw_get_reset_status(void)
{
  return shared_functions_table->sbl1_hw_get_reset_status();
}


void sbl1_load_ddr_training_data_from_partition(uint8 * ddr_training_data_buf,
                                                uint32 ddr_training_data_size)
{
  shared_functions_table->
    sbl1_load_ddr_training_data_from_partition(ddr_training_data_buf,
                                               ddr_training_data_size);
}


pm_err_flag_type boot_pm_dev_get_power_on_reason(unsigned pmic_device_index,
                                                 uint64 * pwr_on_reason)
{
  return shared_functions_table->
    boot_pm_dev_get_power_on_reason(pmic_device_index,
                                    pwr_on_reason);
}

pm_err_flag_type boot_pm_pbs_get_pon_reason_history(pm_pbs_pon_reason_history_data_type* pon_reason_history) 
{ 
  return shared_functions_table-> 
    boot_pm_pbs_get_pon_reason_history(pon_reason_history); 
} 

pm_err_flag_type boot_pm_pon_wdog_cfg (uint8 pmic_device_index, uint32 s1_timer, uint32 s2_timer, pm_pon_reset_cfg_type reset_cfg_type)
{
  return shared_functions_table-> 
    boot_pm_pon_wdog_cfg(pmic_device_index,s1_timer,s2_timer,reset_cfg_type); 	
}
  
pm_err_flag_type boot_pm_pon_wdog_enable (uint8 pmic_device_index, pm_on_off_type enable)
{
  return shared_functions_table-> 
    boot_pm_pon_wdog_enable(pmic_device_index,enable); 		
}

boot_boolean
boot_qsee_is_memory_dump_allowed(secboot_verified_info_type * secboot_info)
{
  return shared_functions_table->boot_qsee_is_memory_dump_allowed(secboot_info);
}

void boot_qsee_zero_peripheral_memory(void)
{
  shared_functions_table->boot_qsee_zero_peripheral_memory();
}


CeMLErrorType boot_CeMLDeInit(void)
{
  return shared_functions_table->boot_CeMLDeInit();
}


CeMLErrorType boot_CeMLHashDeInit(CeMLCntxHandle ** _h)
{
  return shared_functions_table->boot_CeMLHashDeInit(_h);
}


CeMLErrorType boot_CeMLHashFinal(CeMLCntxHandle * _h,
                                 CEMLIovecListType * ioVecOut)
{
  return shared_functions_table->boot_CeMLHashFinal(_h,
                                                    ioVecOut);
} 


CeMLErrorType boot_CeMLHashInit(CeMLCntxHandle ** _h,
                                CeMLHashAlgoType pAlgo)
{
  return shared_functions_table->boot_CeMLHashInit(_h,
                                                   pAlgo);
}


CeMLErrorType boot_CeMLHashSetParam(CeMLCntxHandle * _h,
                                    CeMLHashParamType nParamID, 
                                    const void * pParam, 
                                    uint32 cParam,
                                    CeMLHashAlgoType pAlgo)
{
  return shared_functions_table->boot_CeMLHashSetParam(_h,
                                                       nParamID,
                                                       pParam,
                                                       cParam,
                                                       pAlgo);
}


CeMLErrorType boot_CeMLHashUpdate(CeMLCntxHandle * _h,
                                  CEMLIovecListType ioVecIn)
{
  return shared_functions_table->boot_CeMLHashUpdate(_h,
                                                     ioVecIn);
}


CeMLErrorType boot_CeMLInit(void)
{
  return shared_functions_table->boot_CeMLInit();
}


void boot_err_fatal(void)
{
  shared_functions_table->boot_err_fatal();
}

uint64 boot_coldplug_get_partition_size_by_image_id(image_type image_id)
{
  return shared_functions_table->boot_coldplug_get_partition_size_by_image_id(image_id);
}

boolean dev_sdcc_write_bytes(void * ram_addr,
                             uint64 dst_offset,
                             uint32 bytes_to_write,
                             image_type image_id)
{
  return shared_functions_table->dev_sdcc_write_bytes(ram_addr,
                                                      dst_offset,
                                                      bytes_to_write,
                                                      image_id);
}

boolean dev_sdcc_read_bytes(void * ram_addr,
                             uint64 dst_offset,
                             uint32 bytes_to_read,
                             uint32 parti_id)
{
  return shared_functions_table->dev_sdcc_read_bytes(ram_addr,
                                                      dst_offset,
                                                      bytes_to_read,
                                                      parti_id);
}

int16 boot_get_coldplug_dev_handle(image_type image_id)
{
  return shared_functions_table->boot_get_coldplug_dev_handle(image_id);
}

void boot_set_pwrkey_reset_type(boot_hw_reset_type reset_type)
{
  shared_functions_table->boot_set_pwrkey_reset_type(reset_type);
}

void boot_set_pshold_reset_type(boot_hw_reset_type reset_type)
{
  shared_functions_table->boot_set_pshold_reset_type(reset_type);
}

  /*API's used by USB driver */

pm_err_flag_type pm_schg_usb_irq_status(uint32 device_index, 
                                                  pm_schg_usb_irq_type irq, 
                                                  pm_irq_status_type type, 
                                                  boolean *status )
{
  return shared_functions_table->pm_schg_usb_irq_status(device_index,
                                                                 irq,
                                                                 type,
                                                                 status);
}

pm_err_flag_type pm_schg_usb_get_typec_status(uint32 pmic_device, 
                                              pm_schg_usb_typec_status_type *typec_status)
{
  return shared_functions_table->pm_schg_usb_get_typec_status(pmic_device, typec_status);
}

boolean qusb_ldr_utils_fedl_check(void)
{
  return shared_functions_table->qusb_ldr_utils_fedl_check();
}

boolean Clock_SetBIMCSpeed(uint32 nFreqKHz )
{
  return shared_functions_table->Clock_SetBIMCSpeed(nFreqKHz);
}

int32 boot_sbl_qsee_interface_get_image_entry(boot_sbl_qsee_interface * sbl_qsee_interface,
                                              secboot_sw_type image_id)
{
  return shared_functions_table->boot_sbl_qsee_interface_get_image_entry(sbl_qsee_interface,
                                                                         image_id);
}


int boot_scm(uintnt smc_id, uintnt param_id, uintnt r0, uintnt r1, uintnt r2,
             uintnt r3, uintnt session_id, boot_tzt_result_regs_t* resp_buff_ptr)
{
  return shared_functions_table->boot_scm(smc_id, param_id, r0, r1, r2, r3, session_id, resp_buff_ptr);
}


uint32 sbl1_get_current_el( void )
{
  return shared_functions_table->sbl1_get_current_el();
}

DALResult
DAL_DeviceAttachEx(const char *pszArg,
                   DALDEVICEID DevId,
                   DALInterfaceVersion ClientVersion,
                   DalDeviceHandle **phDalDevice)
{
  return shared_functions_table->DAL_DeviceAttachEx(pszArg,
                    DevId,
                   ClientVersion,
                   phDalDevice);
}
				   
DALResult DALSYS_Malloc(uint32 dwSize, void **ppMem)
{
  return shared_functions_table->DALSYS_Malloc(dwSize, ppMem);
}

DALResult DALSYS_Free(void *pmem)
{
  return shared_functions_table->DALSYS_Free(pmem);
}

DALResult
DAL_DeviceAttach(DALDEVICEID DeviceId,DalDeviceHandle **phDevice)
{
  return shared_functions_table->DAL_DeviceAttach(DeviceId,phDevice);
}

DALResult
DAL_DeviceDetach(DalDeviceHandle *hDevice)
{
  return shared_functions_table->DAL_DeviceDetach(hDevice);
}

DALResult
DALSYS_GetDALPropertyHandle(DALDEVICEID DeviceId,DALSYSPropertyHandle hDALProps)
{
  return shared_functions_table->DALSYS_GetDALPropertyHandle(DeviceId,hDALProps);
}

DALResult
DALSYS_GetDALPropertyHandleStr(const char *pszDevName, DALSYSPropertyHandle hDALProps)
{
  return shared_functions_table->DALSYS_GetDALPropertyHandleStr(pszDevName,hDALProps);
}

DALResult
DALSYS_GetPropertyValue(DALSYSPropertyHandle hDALProps, const char *pszName,
                  uint32 dwId,
                   DALSYSPropertyVar *pDALPropVar)
{
  return shared_functions_table->DALSYS_GetPropertyValue(hDALProps,pszName,dwId,pDALPropVar);
}

void DALSYS_LogEvent(DALDEVICEID DeviceId, uint32 dwLogEventType, const char * pszFmt,
                ...)
{
  return shared_functions_table->DALSYS_LogEvent(DeviceId,dwLogEventType,pszFmt);                 
}

void
DALSYS_BusyWait(uint32 pause_time_us)
{
  return shared_functions_table->DALSYS_BusyWait(pause_time_us);
}

VOID * DALSYS_memset(VOID * buffer, UINT8 value, UINTN length)
{
  return shared_functions_table->DALSYS_memset(buffer,value,length);
}

DALResult
DALSYS_EventCreate(uint32 dwAttribs, DALSYSEventHandle *phEvent,
               DALSYSEventObj *pEventObj)
{
   return shared_functions_table->DALSYS_EventCreate(dwAttribs,phEvent,pEventObj);
}

DALResult
DALSYS_DestroyObject(DALSYSObjHandle hObj)
{
  return shared_functions_table->DALSYS_DestroyObject(hObj);
}

DALResult
DALSYS_EventCtrlEx(DALSYSEventHandle hEvent, uint32 dwCtrl, uint32 dwParam,
                   void *pPayload, uint32 dwPayloadSize)
{
  return shared_functions_table->DALSYS_EventCtrlEx(hEvent,  dwCtrl,  dwParam,
                    pPayload,  dwPayloadSize);
}
 
DALResult
DALSYS_SyncCreate(uint32 dwAttribs,
                  DALSYSSyncHandle *phSync,
                  DALSYSSyncObj *pSyncObj)
{
  return shared_functions_table->DALSYS_SyncCreate(dwAttribs,phSync,pSyncObj);               
}

DALResult DALSYS_MemRegionAlloc (uint32 dwAttribs, DALSYSMemAddr VirtualAddr,
               DALSYSMemAddr PhysicalAddr, uint32 dwLen, DALSYSMemHandle *phMem,DALSYSMemObj *pMemObj)
{
  return shared_functions_table->DALSYS_MemRegionAlloc(dwAttribs,VirtualAddr,PhysicalAddr,dwLen,phMem,pMemObj);
}

DALResult
DALSYS_MemInfo(DALSYSMemHandle hMem, DALSYSMemInfo *pMemInfo)
{
  return shared_functions_table->DALSYS_MemInfo(hMem,pMemInfo);
}    

DALResult DALSYS_EventMultipleWait (DALSYSEventHandle* phEvent, int nEvents,
                         uint32 dwTimeoutUs,uint32 *pdwEventIdx)
{
  return shared_functions_table->DALSYS_EventMultipleWait(phEvent,nEvents,dwTimeoutUs,pdwEventIdx);                          
}

void busywait (uint32 pause_time_us)
{
  return shared_functions_table->busywait(pause_time_us);
}

DALResult busywait_init(void)
{
  return shared_functions_table->busywait_init();
}

boolean boot_log_get_init_info(uint8 **log_base_addr_ptr, uintnt *size_ptr)
{
  return shared_functions_table->boot_log_get_init_info(log_base_addr_ptr,
                                                         size_ptr);
}

boolean Clock_InitUSB(void)
{
  return shared_functions_table->Clock_InitUSB();
}

boolean Clock_Usb30EnableSWCollapse(boolean enable)
{
  return shared_functions_table->Clock_Usb30EnableSWCollapse(enable);
}

boolean Clock_Usb30GetSWCollapse(void)
{
  return shared_functions_table->Clock_Usb30GetSWCollapse();
}

uint32 Clock_SetSDCClockFrequencyExt(uint32 nFreqKHz, ClockSDCType eClock)
{
  return shared_functions_table->Clock_SetSDCClockFrequencyExt(nFreqKHz, eClock);
}

boolean Clock_SetSDCClockFrequency(ClockBootPerfLevelType ePerfLevel, ClockSDCType eClock)
{
  return shared_functions_table->Clock_SetSDCClockFrequency(ePerfLevel, eClock);
}

boolean Clock_DisableUSB(void)
{
  return shared_functions_table->Clock_DisableUSB();
}

boolean Clock_SetQUPV3Frequency(ClockQUPV3Type eClock, uint32 nFreqKHz)
{
  return shared_functions_table->Clock_SetQUPV3Frequency(eClock, nFreqKHz);
}

void boot_restore_ramdump_sp(void)
{
  return shared_functions_table->boot_restore_ramdump_sp();
}

void boot_DALSYS_InitMod(DALSYSConfig *pCfg)
{
  return shared_functions_table->boot_DALSYS_InitMod(pCfg);
}
void boot_DALSYS_DeInitMod(void)
{
  return shared_functions_table->boot_DALSYS_DeInitMod();
}
byte* sbl1_get_external_heap_addr(void)
{
  return shared_functions_table->sbl1_get_external_heap_addr();
}
uint32 sbl1_get_external_heap_size(void)
{
  return shared_functions_table->sbl1_get_external_heap_size();
}
void boot_DALSYS_HeapInit(void *pHeapBase, uint32 dwHeapSz, uint32 dwInitType) 
{
  return shared_functions_table->boot_DALSYS_HeapInit(pHeapBase,dwHeapSz,dwInitType);
}
void boot_DALSYS_HeapDeInit(void *pHeapBase) 
{
  return shared_functions_table->boot_DALSYS_HeapDeInit(pHeapBase);
}

void DALSYS_InitMod(DALSYSConfig * pCfg)
{
  return shared_functions_table->boot_DALSYS_InitMod(pCfg);
}

DALResult DALSYS_CacheCommand(uint32 CacheCmd, DALSYSMemAddr VirtualAddr, uint32 dwLen)
{
  return shared_functions_table->DALSYS_CacheCommand(CacheCmd, VirtualAddr, dwLen);
}

UINTN DALSYS_memscpy(VOID * pDest, UINTN iDestSz, VOID * pSrc, UINTN iSrcSize)
{
  return shared_functions_table->DALSYS_memscpy(pDest, iDestSz, pSrc, iSrcSize);
}

pm_err_flag_type pm_ldo_sw_enable (uint8 pmic_chip, uint8  ldo_peripheral_index, pm_on_off_type  on_off)
{
  return shared_functions_table->pm_ldo_sw_enable(pmic_chip, ldo_peripheral_index, on_off);
}

pm_err_flag_type pm_ldo_volt_level(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_volt_level_type volt_level)
{
  return shared_functions_table->pm_ldo_volt_level(pmic_chip, ldo_peripheral_index, volt_level);
}

pm_err_flag_type pm_ibb_lcd_amoled_sel(uint32 device_index, boolean en_amoled_mode)
{
  return shared_functions_table->pm_ibb_lcd_amoled_sel(device_index, en_amoled_mode);
}

pm_err_flag_type pm_ibb_set_soft_strt_chgr_resistor(uint32 device_index, uint32 resistor_kohms)
{
  return shared_functions_table->pm_ibb_set_soft_strt_chgr_resistor(device_index, resistor_kohms);
}

pm_err_flag_type pm_ibb_ibb_module_rdy(uint32 device_index, boolean ibb_rdy)
{
  return shared_functions_table->pm_ibb_ibb_module_rdy(device_index, ibb_rdy);
}

pm_err_flag_type pm_ibb_config_ibb_ctrl(uint32 device_index, boolean en_module, boolean swire_rdy)
{
  return shared_functions_table->pm_ibb_config_ibb_ctrl(device_index, en_module, swire_rdy);
}

pm_err_flag_type pm_ibb_get_ibb_status(uint32 device_index, pm_ibb_status_type* ibb_status)
{
  return shared_functions_table->pm_ibb_get_ibb_status(device_index, ibb_status);
}

pm_err_flag_type pm_lab_config_current_sense(uint32 device_index, pm_lab_isense_type isense_tap, pm_lab_isense_type isense_gain)
{
  return shared_functions_table->pm_lab_config_current_sense(device_index, isense_tap, isense_gain);
}

pm_err_flag_type pm_lab_config_pulse_skip_ctrl(uint32 device_index, uint32 ps_threshold, boolean select_ps_table, boolean select_digital_ps, boolean enable_ps)
{
  return shared_functions_table->pm_lab_config_pulse_skip_ctrl(device_index, ps_threshold, select_ps_table, select_digital_ps, enable_ps);
}

pm_err_flag_type pm_lab_lcd_amoled_sel(uint32 device_index, boolean en_amoled_mode)
{
  return shared_functions_table->pm_lab_lcd_amoled_sel(device_index, en_amoled_mode);
}

pm_err_flag_type pm_lab_lab_module_rdy(uint32 device_index, boolean lab_rdy)
{
  return shared_functions_table->pm_lab_lab_module_rdy(device_index, lab_rdy);
}

pm_err_flag_type pm_lab_ibb_rdy_en(uint32 device_index, boolean ibb_rdy_en)
{
  return shared_functions_table->pm_lab_ibb_rdy_en(device_index, ibb_rdy_en);
}

pm_err_flag_type pm_lab_get_lab_status(uint32 device_index, pm_lab_status_type* lab_status)
{
  return shared_functions_table->pm_lab_get_lab_status(device_index, lab_status);
}

pm_err_flag_type pm_wled_sel_ovp_thr(uint32 pmic_device_index, uint32 resource_index, pm_wled_ovp_threshold_type ovp_threshold_type)
{
  return shared_functions_table->pm_wled_sel_ovp_thr(pmic_device_index, resource_index, ovp_threshold_type);
}

pm_err_flag_type pm_wled_sel_bst_ilimit(uint32 pmic_device_index, uint32 resource_index, pm_wled_bst_ilimit_type bst_ilimit_type)
{
  return shared_functions_table->pm_wled_sel_bst_ilimit(pmic_device_index, resource_index, bst_ilimit_type);
}

pm_err_flag_type pm_wled_sel_slew_rate(uint32 pmic_device_index, pm_wled_slew_rate_type slew_rate_type)
{
  return shared_functions_table->pm_wled_sel_slew_rate(pmic_device_index, slew_rate_type);
}

pm_err_flag_type pm_wled_set_fsw_ctrl(uint32 pmic_device_index, uint32 resource_index, uint32 fsw_khz)
{
  return shared_functions_table->pm_wled_set_fsw_ctrl(pmic_device_index, resource_index, fsw_khz);
}

pm_err_flag_type pm_wled_set_led_duty_cycle(uint32 pmic_device_index, pm_wled_led_type led_type, uint16 data)
{
  return shared_functions_table->pm_wled_set_led_duty_cycle(pmic_device_index, led_type, data);
}

pm_err_flag_type pm_wled_enable_current_sink(uint32 pmic_device_index, pm_wled_led_type led_type)
{
  return shared_functions_table->pm_wled_enable_current_sink(pmic_device_index, led_type);
}

pm_err_flag_type pm_wled_enable_modulator(uint32 pmic_device_index, pm_wled_led_type led_type, boolean enable)
{
  return shared_functions_table->pm_wled_enable_modulator(pmic_device_index, led_type, enable);
}

pm_err_flag_type pm_wled_enable_module(uint32 pmic_device_index, uint32 resource_index, boolean enable)
{
  return shared_functions_table->pm_wled_enable_module(pmic_device_index, resource_index, enable);
}

pm_err_flag_type pm_wled_en_sync(uint32 pmic_device_index, pm_wled_led_type led_type, boolean enable)
{
  return shared_functions_table->pm_wled_en_sync(pmic_device_index, led_type, enable);
}

pm_err_flag_type pm_wled_en_amoled(uint32 pmic_device_index, pm_wled_display_type display_type)
{
  return shared_functions_table->pm_wled_en_amoled(pmic_device_index, display_type);
}

pm_err_flag_type pm_wled_set_amoled_vout(uint32 pmic_device_index, pm_wled_amoled_ctrl_type  vout_ctrl, uint32 avdd_milli_volt)
{
  return shared_functions_table->pm_wled_set_amoled_vout(pmic_device_index, vout_ctrl, avdd_milli_volt);
}

pm_err_flag_type pm_lcdb_module_enable(uint8 pmic_index, boolean module_en)
{
  return shared_functions_table->pm_lcdb_module_enable(pmic_index, module_en);
}

pm_err_flag_type pm_lcdb_set_module_rdy(uint8 pmic_index, boolean module_rdy)
{
  return shared_functions_table->pm_lcdb_set_module_rdy(pmic_index, module_rdy);
}

pm_err_flag_type pm_fg_batt_info_get_vbatt(uint32 pmic_device, uint32 *pVBat)
{
  return shared_functions_table->pm_fg_batt_info_get_vbatt(pmic_device, pVBat);
}

DALResult boot_DalPlatformInfo_PlatformInfo(void *pPlatforminfo)
{
  return shared_functions_table->boot_DalPlatformInfo_PlatformInfo(pPlatforminfo);
}

DALResult boot_DalPlatformInfo_ChipId(void * pChipSetId)

{
  return shared_functions_table->boot_DalPlatformInfo_ChipId(pChipSetId);
}

DALResult boot_DalPlatformInfo_ChipFamily(void * pChipsetfamily)

{
  return shared_functions_table->boot_DalPlatformInfo_ChipFamily(pChipsetfamily);
}


struct coldplug_iter* coldplug_iter_open(enum coldplug_device_type dev_type, int is_embedded_device, int only_fat_partitions)
{
	return shared_functions_table->coldplug_iter_open(dev_type, is_embedded_device, only_fat_partitions);
}

struct coldplug_iter_record* coldplug_iter_next(struct coldplug_iter *citer)
{
	return shared_functions_table->coldplug_iter_next(citer);
}

int coldplug_iter_close(struct coldplug_iter *citer)
{
	return shared_functions_table->coldplug_iter_close(citer);
}

int coldplug_write(struct coldplug_handle *chdl,uint64 lba, uint64 lba_count, uint8 *buf)
{
	return shared_functions_table->coldplug_write(chdl,lba,lba_count,buf);
}

int coldplug_get_size(struct coldplug_handle *chdl, uint32 *lba_size,uint64 *lba_count)
{
	return shared_functions_table->coldplug_get_size(chdl,lba_size,lba_count);
}

int coldplug_read(struct coldplug_handle *chdl,uint64 lba, uint64 lba_count, uint8 *buf)
{
	return shared_functions_table->coldplug_read(chdl,lba,lba_count,buf);
}
  
int coldplug_close_partition(struct coldplug_handle *chdl)
{
	return shared_functions_table->coldplug_close_partition(chdl);
}

int coldplug_open_device(enum coldplug_device_type dev_type,int is_embedded_device)
{
	return shared_functions_table->coldplug_open_device(dev_type,is_embedded_device);
}

struct coldplug_handle* coldplug_open_partition(const char *parti_name)
{
	return shared_functions_table->coldplug_open_partition(parti_name);	
}

int coldplug_get_start_lba(struct coldplug_handle *chdl, uint64 *start_lba)
{
	return shared_functions_table->coldplug_get_start_lba(chdl,start_lba);		
}

void smem_init( void )
{
  return shared_functions_table->smem_init();
}

void *smem_get_addr(smem_mem_type smem_type, uint32 *buf_size)
{
  return shared_functions_table->smem_get_addr(smem_type,buf_size);	
}

