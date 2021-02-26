/*! \file pm_sbl_boot_oem.c
*  \n
*  \brief This file contains PMIC device initialization function where initial PMIC
*  \n SBL settings are configured through the PDM auto-generated code.
*  \n
*  \n &copy; Copyright 2015-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/12/17   sv      Initialize FgSram in pm_sbl_chg_pre_init API.
12/04/15   aab     Creation
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_sbl_boot_target.h"
#include "pm_pon.h"
#include "pm_app_pon.h"
#include "pm_sbl_boot.h"
#include "pm_device.h"
#include "pm_comm.h"
#include "pm_utils.h"
#include "pm_schg_usb.h"
#include "DDIChipInfo.h"
#include <stdio.h>
#include "boot_logger.h"
#include "CoreVerify.h"
#include "pm_ldo.h"
#include "DALDeviceId.h"
#include "DDIPlatformInfo.h"
#include "DDIChipInfo.h"
#include "pm_app_vreg.h"
#include "pm_target_information.h"
#include "pm_clk.h"
#include "pm_uefi.h"
#include "pm_fg_sram.h"
#include "pm_log_utils.h"
#include "pm_schg_misc.h"
#include "boothw_target.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
#define PM660L_LCD_VARIENT 0x2
#define PM_INDEX 0
#define PON_PON_REASON1_USB_CHG 0x10
/*===========================================================================

                        FUNCTION DEFINITIONS 

===========================================================================*/

pm_err_flag_type
pm_device_pre_init(void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  pm_sbl_specific_data_type *config_param_ptr = NULL;

  config_param_ptr = (pm_sbl_specific_data_type*)pm_target_information_get_specific_info(PM_PROP_SBL_SPECIFIC_DATA);
  CORE_VERIFY_PTR(config_param_ptr);

  if(config_param_ptr->rfclk2_config_flag)  // RF_CLK for WLAN
  {
    err_flag |= pm_comm_write_byte(0, 0x5447, 0x0, 0);	// Do NOT follow pin control
    err_flag |= pm_comm_write_byte(0, 0x5448, 0x80, 0);	// Enable pull down for RF_CLK1 peripheral
  }

  return err_flag;
}

pm_err_flag_type
pm_device_post_init(void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

  //These configurations is only used for development phones and should be commented out for production phones
  err_flag |= pm_app_pon_pshold_cfg(PM_APP_PON_CFG_WARM_RESET);
  err_flag |= pm_app_pon_reset_cfg( PM_APP_PON_RESET_SOURCE_KPDPWR, PM_APP_PON_CFG_WARM_RESET, 10256, 2000); //PON KPDPWR PON Reset configuration
  err_flag |= pm_app_pon_reset_cfg( PM_APP_PON_RESET_SOURCE_RESIN_AND_KPDPWR, PM_APP_PON_CFG_DVDD_HARD_RESET, 10256, 2000); //PON RESIN_AND_KPDPWR PON Reset configuration

  err_flag |= pm_app_pon_reset_init();
  
  /*check for valid PON keypress */
  err_flag |= pm_app_pwrkey_long_press_check(PM_PON_PWRKEY_DBNC_CHK_AT_LOADER);
  
  return err_flag;
}

pm_err_flag_type
pm_driver_pre_init (void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

  return err_flag;
}

pm_err_flag_type
pm_driver_post_init(void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  pm_device_info_type PmicDeviceInfo;
  uint8 pmic_variant;

  DalPlatformInfoPlatformType platform_type = DALPLATFORMINFO_TYPE_UNKNOWN;
  platform_type = DalPlatformInfo_Platform();

  pm_sbl_specific_data_type *config_param_ptr = NULL;

  config_param_ptr = (pm_sbl_specific_data_type *)pm_target_information_get_specific_info(PM_PROP_SBL_SPECIFIC_DATA);
  CORE_VERIFY_PTR(config_param_ptr);

  if(config_param_ptr->rfclk2_config_flag)
  {
    err_flag |= pm_clk_sw_enable(0, PM_CLK_RF_1, PM_OFF); // Turn off WLAN RF_CLK
  }
  
  pm_get_pmic_variant(PM_DEVICE_1, &pmic_variant);
  err_flag = pm_get_pmic_info(PM_DEVICE_1, &PmicDeviceInfo);
  if( ( PMIC_IS_PM660L == PmicDeviceInfo.ePmicModel) && (PM660L_LCD_VARIENT == pmic_variant) && (1 == PmicDeviceInfo.nPmicAllLayerRevision) && (0 == PmicDeviceInfo.nPmicMetalRevision) )
  {
     pm_log_message("PM660L 1.0 Have LCDB HW issue.. ");
     return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  err_flag |= pm_app_vreg_clear_ocp_status();

  return err_flag;
}

pm_err_flag_type
pm_sbl_chg_pre_init (void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  PmicFgSram_Init(PM_INDEX);
  /*  Start battery id conversion in continuous mode.
      Next image (UEFI) would need to stop this mode */
  err_flag = pm_sbl_set_batt_id_forced_mode(TRUE);
  
  return err_flag;
}

pm_err_flag_type
pm_sbl_chg_post_init (void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

  return err_flag;
}


pm_err_flag_type
pm_app_auto_power_on_check(uint32 device_index)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   pm_pon_reason_type pon_reason;
   pm_pon_on_state_reason_type on_reason;
   pm_schg_misc_sysok_reason_type sysok_reason;
   boolean usbin_status = FALSE;

   //If the SYS_OK reason is wi-power(DC-IN), then skip this WA
   err_flag = pm_schg_misc_get_sysok_reason(device_index, &sysok_reason);
   if((PM_ERR_FLAG__SUCCESS == err_flag) && (sysok_reason  == PM_SCHG_MISC_SYSOK_REASON__DCIN))
   {
	   return PM_ERR_FLAG__SUCCESS;
   }

   err_flag = pm_pon_get_pon_reason(device_index, &pon_reason);
   err_flag |=  pm_pon_get_on_reason(device_index, &on_reason);
   if (err_flag != PM_ERR_FLAG_SUCCESS)
   {
       pm_log_message("ERROR, pm_pon_get_pon_reason()");
       return err_flag;
   }

   if(((*((uint8*)&pon_reason))==PON_PON_REASON1_USB_CHG) && (on_reason.warm_seq == FALSE))
   {
       err_flag |= pm_schg_usb_irq_status(device_index, PM_SCHG_USB_IRQ_USBIN_UV, PM_IRQ_STATUS_RT, &usbin_status);
       if (err_flag != PM_ERR_FLAG_SUCCESS)
       {
           pm_log_message("ERROR, pm_schg_usb_irq_status()");
           return err_flag;
       }
       if(usbin_status == TRUE)
       {
           pm_log_message("ERROR, Device is shutting down due to false USBIN power on!");
           boot_hw_powerdown();
       }
   }

   return err_flag;
}

