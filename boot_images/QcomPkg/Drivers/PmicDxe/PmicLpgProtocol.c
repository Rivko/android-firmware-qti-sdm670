/*! @file PmicLpgProtocol.c 

 *  PMIC-LPG MODULE RELATED DECLARATION
 *  This file contains functions and variable declarations to support 
 *  the PMIC LPG (Light Pulse Generator) module.
 *
 *  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 *  Qualcomm Technologies Proprietary and Confidential.

 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/30/15   al      Added API to configure glitch removal   
12/08/14   al      Added API to set LPG DTEST  
07/01/14   al      Adding get_status 
06/09/14   al      Arch update
12/13/13   aa      PMIC Dec Addition
05/09/13   al      Api to enable/disable LpgPwm
01/30/13   al      Removing redundent calls
01/14/13   al      Removing 'dev' from api name
10/25/12   al      File renamed 
10/23/12   al      Updating copyright info and config sequence
08/21/12   al      Modified for 8974
03/05/12   sm      Added LpgSetPWMValue API
05/11/11   dy      New file.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include "pm_uefi.h"
#include "pm_lpg.h"

#include <Protocol/EFIPmicLpg.h>

#define EFI_PM_LPG_BANK_MAX 8

/*===========================================================================
                  EXTERNAL FUNCTION DECLARATIONS
===========================================================================*/

/**
  EFI_PmicLpgConfig ()

  @brief
  LpgConfig implementation of EFI_QCOM_PMIC_VREG_PROTOCOL
 */
EFI_STATUS
EFIAPI
EFI_PmicLpgConfig
(
   IN UINT32                         PmicDeviceIndex,
   IN EFI_PM_LPG_CHAN_TYPE           WhichLpg,
   IN UINT16                         PwmValue,
   IN EFI_PM_LPG_PWM_PRE_DIVIDE_TYPE PreDiv,
   IN EFI_PM_LPG_PWM_FREQ_EXPO_TYPE  PreDivExponent,
   IN EFI_PM_LPG_PWM_CLOCK_TYPE      Clock,
   IN EFI_PM_LPG_PWM_BIT_SIZE_TYPE   BitSize
   )
{
  pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

  /* set the size of the PWM */
  errFlag = pm_lpg_set_pwm_bit_size(PmicDeviceIndex, (pm_lpg_chan_type)WhichLpg, (pm_lpg_pwm_size_type)BitSize);
  if (PM_ERR_FLAG__SUCCESS != errFlag)
  {
    return EFI_DEVICE_ERROR;
  }

  /* set the clock */
  errFlag = pm_lpg_pwm_clock_sel(PmicDeviceIndex, (pm_lpg_chan_type)WhichLpg, (pm_lpg_pwm_clock_type)Clock);
  if (PM_ERR_FLAG__SUCCESS != errFlag)
  {
    return EFI_DEVICE_ERROR;
  }

  /* set the pre-divde exponent */
  errFlag = pm_lpg_pwm_set_pre_divide(PmicDeviceIndex, (pm_lpg_chan_type)WhichLpg, (pm_lpg_pwm_pre_divide_type)PreDiv,
                                      (pm_lpg_pwm_freq_expo_type)PreDivExponent);
  if (PM_ERR_FLAG__SUCCESS != errFlag)
  {
    return EFI_DEVICE_ERROR;
  }

  /* set PWM value */
  errFlag = pm_lpg_pwm_set_pwm_value(PmicDeviceIndex, (pm_lpg_chan_type)WhichLpg, (uint16)PwmValue);
  if (PM_ERR_FLAG__SUCCESS != errFlag)
  {
    return EFI_DEVICE_ERROR;
  }

  /*LPG Source Select*/
  errFlag = pm_lpg_pwm_src_select(PmicDeviceIndex, (pm_lpg_chan_type)WhichLpg, (pm_lpg_src_type)PM_LPG_PWM_SRC_PWM_REGISTER);
  if (PM_ERR_FLAG__SUCCESS != errFlag)
  {
    return EFI_DEVICE_ERROR;
  }

  /* enable PWM */
  errFlag = pm_lpg_pwm_enable(PmicDeviceIndex, (pm_lpg_chan_type)WhichLpg, TRUE);

  if (errFlag != PM_ERR_FLAG__SUCCESS)
  {
    return EFI_DEVICE_ERROR;
  }

  /* turn on PWM output */
  errFlag = pm_lpg_pwm_output_enable(PmicDeviceIndex, (pm_lpg_chan_type)WhichLpg, TRUE);
  if (PM_ERR_FLAG__SUCCESS != errFlag)
  {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
  EFI_PmicLpgSetPWMValue ()

  @brief
  SetPWMValue implementation of EFI_QCOM_PMIC_VREG_PROTOCOL
 */
EFI_STATUS
EFIAPI
EFI_PmicLpgSetPWMValue
(
   IN unsigned          PmicDeviceIndex,
   EFI_PM_LPG_CHAN_TYPE WhichLpg,
   UINT16               PwmValue
   )
{
  pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

  /* set PWM value */
  errFlag = pm_lpg_pwm_set_pwm_value(PmicDeviceIndex, (pm_lpg_chan_type)WhichLpg, (uint16)PwmValue);
  if (PM_ERR_FLAG__SUCCESS != errFlag)
  {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
  EFI_PmicLpgPwmEnable ()

  @brief
  LpgPwmEnable implementation of EFI_QCOM_PMIC_VREG_PROTOCOL
 */
EFI_STATUS
EFIAPI
EFI_PmicLpgPwmEnable
(
   IN UINT32            PmicDeviceIndex,
   EFI_PM_LPG_CHAN_TYPE WhichLpg,
   BOOLEAN              Enable
   )
{
  pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

  /* enable/disable LPG PWM */
  errFlag = pm_lpg_pwm_enable(PmicDeviceIndex, (pm_lpg_chan_type)WhichLpg, (boolean)Enable);
  if (errFlag != PM_ERR_FLAG__SUCCESS)
  {
    return EFI_DEVICE_ERROR;
  }

  /* turn on/off LPG PWM output */
  errFlag = pm_lpg_pwm_output_enable(PmicDeviceIndex, (pm_lpg_chan_type)WhichLpg, (boolean)Enable);
  if (PM_ERR_FLAG__SUCCESS != errFlag)
  {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
  EFI_PmicLpgGetStatus
  @brief
  This reads LPG status
 */
EFI_STATUS
EFIAPI
EFI_PmicLpgGetStatus
(
   IN  UINT32                   PmicDeviceIndex,
   IN  EFI_PM_LPG_CHAN_TYPE     WhichLpg,
   OUT EFI_PM_LPG_STATUS_TYPE   *LpgStatus
   )
{
  pm_err_flag_type  errFlag  = PM_ERR_FLAG__SUCCESS;
  pm_lpg_status_type lpg_status;

  if (NULL == LpgStatus)
  {
    return EFI_DEVICE_ERROR;
  }

  errFlag =  pm_lpg_get_status((uint32)PmicDeviceIndex, (pm_lpg_chan_type)WhichLpg, &lpg_status);
  if (errFlag != PM_ERR_FLAG__SUCCESS)
  {
    return EFI_DEVICE_ERROR;
  }
  else
  {
    LpgStatus->PwmOutput         = (BOOLEAN)(lpg_status.pwm_output);
    LpgStatus->PwmValue          = (UINT16)(lpg_status.pwm_value);
    LpgStatus->PreDiv            = (EFI_PM_LPG_PWM_PRE_DIVIDE_TYPE)(lpg_status.pre_div);
    LpgStatus->Exp               = (EFI_PM_LPG_PWM_FREQ_EXPO_TYPE)(lpg_status.exp);
    LpgStatus->PwmFreqClk        = (EFI_PM_LPG_PWM_CLOCK_TYPE)(lpg_status.pwm_freq_clk);
    LpgStatus->PwmBitSize        = (EFI_PM_LPG_PWM_BIT_SIZE_TYPE)(lpg_status.pwm_bit_size);
    LpgStatus->LpgSrcType        = (EFI_PM_LPG_SRC_TYPE)(lpg_status.lpg_src_type);
    LpgStatus->GlitchRemoval     = (BOOLEAN)(lpg_status.glitch_removal);
    LpgStatus->FullScale         = (BOOLEAN)(lpg_status.full_scale);
    LpgStatus->EnPhaseStagger    = (BOOLEAN)(lpg_status.en_phase_stagger);
    LpgStatus->RampDirection     = (BOOLEAN)(lpg_status.ramp_direction);
    LpgStatus->PatternRepeat     = (BOOLEAN)(lpg_status.pattern_repeat);
    LpgStatus->RampToggle        = (BOOLEAN)(lpg_status.ramp_toggle);
    LpgStatus->EnPauseHi         = (BOOLEAN)(lpg_status.en_pause_hi);
    LpgStatus->EnPauseLo         = (BOOLEAN)(lpg_status.en_pause_lo);
    LpgStatus->LutLowIndex       = (UINT32)(lpg_status.lut_low_index);
    LpgStatus->LutHighIndex      = (UINT32)(lpg_status.lut_high_index);
    LpgStatus->RampStepDuration  = (UINT32)(lpg_status.ramp_step_duration);
    LpgStatus->HiMultiplier      = (UINT32)(lpg_status.hi_multiplier);
    LpgStatus->LowMultiplier     = (UINT32)(lpg_status.low_multiplier);
  }

  return EFI_SUCCESS;
}

/**
  EFI_PmicLpgSetLpgDtest ()

  @brief
  EFI_PmicLpgSetLpgDtest implementation of EFI_QCOM_PMIC_LPG_PROTOCOL
 */
EFI_STATUS
EFIAPI
EFI_PmicLpgSetLpgDtest
(
  IN  UINT32                       PmicDeviceIndex,
  IN  EFI_PM_LPG_CHAN_TYPE         WhichLpg,
  IN  EFI_PM_LPG_CHAN_DTEST_TYPE   Dtest,
  IN  EFI_PM_LPG_CHAN_LPG_OUT_TYPE LpgOut
   )
{
  pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
            
  errFlag = pm_lpg_set_lpg_dtest(PmicDeviceIndex, (pm_lpg_chan_type)WhichLpg, (pm_lpg_chan_dtest_type)Dtest, (pm_lpg_chan_lpg_out_type)LpgOut);

  if (errFlag != PM_ERR_FLAG__SUCCESS)
  {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}


/**
  EFI_PmicLpgConfigGlitchRemoval ()

  @brief
  EFI_PmicLpgGlitchRemoval implementation of EFI_QCOM_PMIC_LPG_PROTOCOL
 */
EFI_STATUS
EFIAPI
EFI_PmicLpgConfigGlitchRemoval
(
  IN  UINT32                       PmicDeviceIndex,
  IN  EFI_PM_LPG_CHAN_TYPE         WhichLpg,
  IN  BOOLEAN                      Enable
  )
{
  pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

  errFlag = pm_lpg_config_pwm_type(PmicDeviceIndex, (pm_lpg_chan_type)WhichLpg, Enable);

  if (errFlag != PM_ERR_FLAG__SUCCESS)
  {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
  PMIC LPG UEFI Protocol implementation
 */
EFI_QCOM_PMIC_LPG_PROTOCOL PmicLpgProtocolImplementation =
{
  PMIC_LPG_REVISION,
  EFI_PmicLpgConfig,
  EFI_PmicLpgSetPWMValue,
  EFI_PmicLpgPwmEnable,
  EFI_PmicLpgGetStatus,
  EFI_PmicLpgSetLpgDtest,
  EFI_PmicLpgConfigGlitchRemoval,
}; 
