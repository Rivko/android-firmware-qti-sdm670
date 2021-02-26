/** @file PmicLibNull.c
  
  Stub functions for PmicLib

  Copyright (c) 2016, Qualcomm Technologies, Inc. All rights reserved.
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/25/17   sv      Added pm_rgb_get_pmic_index API
 07/19/16   kpa     Added pm_schg_usb_get_typec_status
 06/20/16   kpa     Added pmic api stubs
 05/14/15   plc     Updated for latest API's needed for basetarget compilation
 10/03/14   al      Updated for compilation
 05/30/14   kedara  Initial revision

=============================================================================*/


/*==========================================================================

                               INCLUDE FILES

===========================================================================*/

#include "pm.h"
#include "com_dtypes.h"
#include "pm_uefi.h"

#include "pm_gpio.h"
#include "pm_schg_usb.h"
#include "pm_haptics.h"

/**
  PMIC PROTOCOL interface
 */
#include <Library/UefiBootServicesTableLib.h>
#include "Protocol/EFIPmicClkBuff.h"
#include "Protocol/EFIPmicVib.h"
#include "Protocol/EFIPmicWled.h"
#include "Protocol/EFIPmicGpio.h"
#include "Protocol/EFIPmicMpp.h"
#include "Protocol/EFIPmicVreg.h"
#include "Protocol/EFIPmicLpg.h"
#include "Protocol/EFIPmicRTC.h"
#include "Protocol/EFIPmicPwrOn.h"
#include "Protocol/EFIPmicMipiBif.h"
#include "Protocol/EFIPmicRgbLed.h"
#include "Protocol/EFIPmicVersion.h"
#include "Protocol/EFIPmicSmbchg.h"
//#include "Protocol/EFIPmicNpaTest.h"
#include "Protocol/EFIPmicIbb.h"
#include "Protocol/EFIPmicLab.h"
#include "Protocol/EFIPmicFg.h"

#define PM_MAX_NUM_PMICS       7 //current SPMI spec supports 16 slave id

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/*===========================================================================

**  Function :  pm_device_init

** ==========================================================================
*/
/*! 
 * @brief This function executes the pmic device initialization
 * @return Error flag.
 *
 */ 
pm_err_flag_type pm_device_init ( void )
{
  return PM_ERR_FLAG__SUCCESS;
}

/*===========================================================================

**  Function :  pm_pon_get_all_pon_reasons

** ==========================================================================
*/
/*!
 * @brief  This function returns the phone power-on reason. 
 *
 * INPUT PARAMETERS
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 *@param pwr_on_reason
 *    - pointer to 64-bit unsigned integer that stores the all PON reasons
 *   including PON power on, Warm Reset Reason and POFF_REASON,
 *   SOFT_RESET_REASON.
 *    PON_REASON
 *    PON_WARM_RESET_REASON
 *    PON_POFF_REASON
 *    PON_SOFT_RESET_REASON
 *
 * @return pm_err_flag_type.
 *         PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
 *                                       version of the PMIC.
 *         PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
 *                                       with the PMIC.
 *         PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 *@par Dependencies
 *      and pm_init() must have been called.
===========================================================================*/
pm_err_flag_type pm_pon_get_all_pon_reasons(uint32 pmic_device_index, uint64* pon_reasons)
{
  return PM_ERR_FLAG__SUCCESS;
}


/*===========================================================================

**  Function :  pm_driver_init

** ==========================================================================
*/
/*! 
 * @brief This function executes the pmic device initialization
 * @return Error flag.
 *
 */ 
pm_err_flag_type pm_driver_init(void)
{
  return PM_ERR_FLAG__SUCCESS;
}


/*===========================================================================

**  Function :  pm_sbl_chg_init

** ==========================================================================
*/
/*! 
 * @brief This function executes the pmic device initialization
 * @return Error flag.
 *
 */ 
pm_err_flag_type pm_sbl_chg_init(void)
{
  return PM_ERR_FLAG__SUCCESS;
}


/*===========================================================================

**  Function :  pm_smem_init

** ==========================================================================
*/
/*! 
 * @brief This function executes the pmic smem initialization
 * @return Error flag.
 *
 */ 
pm_err_flag_type pm_smem_init(void)
{
  return PM_ERR_FLAG__SUCCESS;
}


/*===========================================================================

**  Function :  pm_rgb_led_config

** ==========================================================================
*/
/*! 
 * @brief This function executes the pmic rgb led functions to turn on blue LED
 * @return Error flag.
 *
 */ 
pm_err_flag_type pm_rgb_led_config
(
   uint32                     device_index,
   pm_rgb_which_type          rgb_peripheral,
   uint32                     rgb_mask,
   pm_rgb_voltage_source_type source_type,
   uint32                     dim_level,
   boolean                    enable_rgb )
{
  return PM_ERR_FLAG__SUCCESS;
}


/*===========================================================================

**  Function :  pm_pon_ps_hold_cfg

** ==========================================================================
*/
/**
 * @brief Configure MSM PS_HOLD behavior.
 *  
 * @details Configure PMIC to act on MSM PS_HOLD state
 * 
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 * @param ps_hold_cfg 
 *          PM_PON_RESET_CFG_WARM_RESET,
 *          PM_PON_RESET_CFG_NORMAL_SHUTDOWN,
 *          PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_SHUTDOWN, Shutdown + dVdd_rb remove main battery
 *          PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_SHUTDOWN, Shutdown + xVdd_rb remove main and coin cell battery
 *          PM_PON_RESET_CFG_HARD_RESET,     Shutdown + Auto pwr up
 *   
 *                   
 * @return pm_err_flag_type
 */
pm_err_flag_type pm_pon_ps_hold_cfg
(
  uint32 pmic_device_index, 
  pm_pon_reset_cfg_type ps_hold_cfg
)
{
  return PM_ERR_FLAG__SUCCESS;
}

pm_err_flag_type pm_vib_set_volt
(
  uint32              pmic_device_index,
  pm_vib_which_type vib,
  uint16                volt
)
{
  return PM_ERR_FLAG__SUCCESS;
}

pm_err_flag_type pm_vib_enable
(
  uint32            pmic_device_index,
  pm_vib_which_type vib,
  pm_vib_mode_type  mode,
  boolean           enable
)
{
  return PM_ERR_FLAG__SUCCESS;
}

pm_err_flag_type pm_get_pmic_info(uint8 pmic_index, pm_device_info_type* pmic_device_info_ptr)

{
  if((pmic_device_info_ptr == NULL) || (pmic_index >= PM_MAX_NUM_PMICS))
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  pmic_device_info_ptr->ePmicModel = PMIC_IS_UNKNOWN;
  pmic_device_info_ptr->nPmicAllLayerRevision = 0;
  pmic_device_info_ptr->nPmicMetalRevision = 0;
  return PM_ERR_FLAG__SUCCESS;
}

pm_err_flag_type 
pm_app_pon_pshold_cfg(pm_app_pon_reset_cfg_type  ps_hold_cfg)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  return err_flag;
}

pm_err_flag_type
pm_device_programmer_init (void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  return err_flag;
}

pm_err_flag_type pm_init( void )
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  return err_flag;
}

/*===========================================================================
FUNCTION pm_install_uefi_protocol                                EXTERNAL FUNCTION

DESCRIPTION
    This function installs pmic uefi protocols.

    It does the following:
    1)  It initializes the target specific pmic uefi protocol 

INPUT PARAMETERS
  Imagehandle and SystemTable pointer

RETURN VALUE
  pm_err_flag_type

DEPENDENCIES
  pm_init must have been called

SIDE EFFECTS
  None.
===========================================================================*/
pm_err_flag_type pm_install_uefi_protocol(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_STATUS Status = EFI_SUCCESS;
  return Status;
}

pm_err_flag_type
pm_pon_reset_source_cfg(uint32 pmic_device_index, pm_pon_reset_source_type reset_source, uint32 s1_timer, uint32 s2_timer, pm_pon_reset_cfg_type reset_cfg_type)
{
  pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
  return err_flag;
}

pm_err_flag_type
pm_pon_reset_source_ctl(uint32 pmic_device_index,
                        pm_pon_reset_source_type reset_source, pm_on_off_type on_off)
{
  pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
  return err_flag;
}


pm_err_flag_type pm_system_init(void)
{
  return PM_ERR_FLAG__SUCCESS;
}

pm_err_flag_type
pm_pon_warm_reset_status(uint32 pmic_device_index, boolean *status)
{
  return PM_ERR_FLAG__SUCCESS;
}

pm_err_flag_type
pm_pon_warm_reset_status_clear(uint32 pmic_device_index)
{
  return PM_ERR_FLAG__SUCCESS;
}

pm_err_flag_type pm_schg_usb_irq_status(uint32 device_index, pm_schg_usb_irq_type irq, pm_irq_status_type type, boolean *status)
{
  return PM_ERR_FLAG__SUCCESS;
}

pm_err_flag_type pm_gpio_set_ext_pin_config(uint8 pmic_chip, pm_gpio_perph_index gpio, pm_gpio_ext_pin_config_type  ext_pin_config)
{
   return PM_ERR_FLAG__SUCCESS;
}

pm_err_flag_type pm_schg_usb_get_typec_status(uint32 pmic_device, pm_schg_usb_typec_status_type *typec_status)
{
  return PM_ERR_FLAG__SUCCESS;
}

pm_err_flag_type
pm_app_pon_reset_cfg( pm_app_pon_reset_source_type  app_pon_reset_source,
                      pm_app_pon_reset_cfg_type     app_pon_reset_cfg,
                      uint32 s1_timer,
                      uint32 s2_timer )
{
  return PM_ERR_FLAG__SUCCESS;
}

pm_err_flag_type pm_fg_batt_info_get_vbatt(uint32 pmic_device, uint32 *pVBat)
{
  return PM_ERR_FLAG__SUCCESS;
}

pm_err_flag_type pm_smps_volt_level(uint8 pmic_chip, uint8 smps_peripheral_index, pm_volt_level_type volt_level)
{
  return PM_ERR_FLAG__SUCCESS;
}

pm_err_flag_type
pm_pbs_get_pon_reason_history(pm_pbs_pon_reason_history_data_type *pon_reason_history_ptr )
{
  return PM_ERR_FLAG__SUCCESS;
}


pm_err_flag_type pm_schg_usb_get_apsd_result_status(uint32 pmic_device, pm_schg_usb_apsd_result_status_type *apsd_result)
{
 return PM_ERR_FLAG__SUCCESS;
}

pm_err_flag_type
pm_aop_pre_init(void)
{
 return PM_ERR_FLAG__SUCCESS;  
}

pm_err_flag_type pm_haptics_set_vmax_volt(uint32 pmic_chip, uint32 vmax_cfg_volt)
{
    return PM_ERR_FLAG__SUCCESS;
}

pm_err_flag_type pm_haptics_enable(uint32 pmic_chip, pm_on_off_type enable)
{
    return PM_ERR_FLAG__SUCCESS;
}

pm_err_flag_type pm_haptics_play(uint32 pmic_chip, pm_haptics_vibrate_type vibrate)
{
    return PM_ERR_FLAG__SUCCESS;
}

pm_err_flag_type pm_rgb_get_pmic_index(pm_rgb_Uasge_type Rgb_led_type, uint32 *rgb_chg_pmic_index)
{
  return PM_ERR_FLAG__SUCCESS;
}
