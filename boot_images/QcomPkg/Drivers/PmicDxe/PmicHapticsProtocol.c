/*! @file PmicHapticsProtocol.c 

*  PMIC- HAPTICS MODULE RELATED DECLARATION
*  This file contains functions and variable declarations to support 
*  the PMIC IBB module.
*
*  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved. 
*  Qualcomm Technologies Proprietary and Confidential.
*/
/*===========================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/22/15   al      New file.
===========================================================================*/

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

#include "pm_uefi.h"
#include "pm_haptics.h"

#include <Protocol/EFIPmicHaptics.h>


/*===========================================================================
EXTERNAL FUNCTION DECLARATIONS
===========================================================================*/

EFI_STATUS
EFIAPI
EFI_PmicHapticsEnable
(
  IN UINT32  PmicDeviceIndex,
  IN BOOLEAN Enable
  )
{
  EFI_STATUS                           Status  = EFI_SUCCESS;
  pm_err_flag_type                     errFlag = PM_ERR_FLAG__SUCCESS;

  errFlag = pm_haptics_enable ( PmicDeviceIndex, (pm_on_off_type)Enable);
  if(PM_ERR_FLAG__SUCCESS != errFlag)
  {
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}



EFI_STATUS
EFIAPI
EFI_PmicHapticsSetActuatorType
(
  IN UINT32                       PmicDeviceIndex,
  IN EFI_PM_HAPTICS_ACTUATOR_TYPE ActuatorType
  )
{
  EFI_STATUS                           Status  = EFI_SUCCESS;
  pm_err_flag_type                     errFlag = PM_ERR_FLAG__SUCCESS;

  errFlag = pm_haptics_set_actuator_type( PmicDeviceIndex, (pm_haptics_actuator_type)ActuatorType);

  if(PM_ERR_FLAG__SUCCESS != errFlag)
  {
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}



EFI_STATUS
EFIAPI
EFI_PmicHapticsSetLraResonanceType
(
  IN UINT32                      PmicDeviceIndex,
  IN EFI_PM_HAPTICS_LRA_RES_TYPE LraRes
  )
{
  EFI_STATUS                           Status  = EFI_SUCCESS;
  pm_err_flag_type                     errFlag = PM_ERR_FLAG__SUCCESS;

  errFlag = pm_haptics_set_lra_resonance_type( PmicDeviceIndex, (pm_haptics_lra_res_type)LraRes);

  if(PM_ERR_FLAG__SUCCESS != errFlag)
  {
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}



EFI_STATUS
EFIAPI
EFI_PmicHapticsSetSlewRate
(
  IN UINT32                   PmicDeviceIndex,
  IN EFI_PM_HAPTICS_SLEW_RATE SlewRate
  )
{
  EFI_STATUS                           Status  = EFI_SUCCESS;
  pm_err_flag_type                     errFlag = PM_ERR_FLAG__SUCCESS;

  errFlag = pm_haptics_set_slew_rate( PmicDeviceIndex, (pm_haptics_slew_rate)SlewRate);

  if(PM_ERR_FLAG__SUCCESS != errFlag)
  {
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}




EFI_STATUS
EFIAPI
EFI_PmicHapticsPlay
(
  IN UINT32                      PmicDeviceIndex,
  IN EFI_PM_HAPTICS_VIBRATE_TYPE Vibrate
  )
{
  EFI_STATUS                           Status  = EFI_SUCCESS;
  pm_err_flag_type                     errFlag = PM_ERR_FLAG__SUCCESS;

  errFlag = pm_haptics_play(PmicDeviceIndex, (pm_haptics_vibrate_type)Vibrate);

  if(PM_ERR_FLAG__SUCCESS != errFlag)
  {
    Status = EFI_DEVICE_ERROR;
  }
  return Status;
}




EFI_STATUS
EFIAPI
EFI_PmicHapticsSetVmaxVolt
(
  IN UINT32 PmicDeviceIndex,
  IN UINT32 VmaxCfgVolt
  )
{
  EFI_STATUS                           Status  = EFI_SUCCESS;
  pm_err_flag_type                     errFlag = PM_ERR_FLAG__SUCCESS;

  errFlag = pm_haptics_set_vmax_volt(PmicDeviceIndex, VmaxCfgVolt);

  if(PM_ERR_FLAG__SUCCESS != errFlag)
  {
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}



EFI_STATUS
EFIAPI
EFI_PmicHapticsInternalPwmFreqSel
(
  IN UINT32 PmicDeviceIndex,
  IN UINT32 FreqSelDataInKhz
  )
{
  EFI_STATUS                           Status  = EFI_SUCCESS;
  pm_err_flag_type                     errFlag = PM_ERR_FLAG__SUCCESS;

  errFlag = pm_haptics_internal_pwm_freq_sel (PmicDeviceIndex, FreqSelDataInKhz);

  if(PM_ERR_FLAG__SUCCESS != errFlag)
  {
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}



EFI_STATUS
EFIAPI
EFI_PmicHapticsExternalPwmFreqSel
(
  IN UINT32 PmicDeviceIndex,
  IN UINT32 FreqSelDataInKhz
  )
{
  EFI_STATUS                           Status  = EFI_SUCCESS;
  pm_err_flag_type                     errFlag = PM_ERR_FLAG__SUCCESS;

  errFlag = pm_haptics_external_pwm_freq_sel(PmicDeviceIndex, FreqSelDataInKhz);

  if(PM_ERR_FLAG__SUCCESS != errFlag)
  {
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}



EFI_STATUS
EFIAPI
EFI_PmicHapticsPwmCapSel
(
  IN UINT32                           PmicDeviceIndex,
  IN EFI_PM_HAPTICS_PWM_CAP_SEL_TYPE  CapSelData
  )
{
  EFI_STATUS                           Status  = EFI_SUCCESS;
  pm_err_flag_type                     errFlag = PM_ERR_FLAG__SUCCESS;

  errFlag = pm_haptics_pwm_cap_sel(PmicDeviceIndex, (pm_haptics_pwm_cap_sel_type)CapSelData);

  if(PM_ERR_FLAG__SUCCESS != errFlag)
  {
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}



EFI_STATUS
EFIAPI
EFI_PmicHapticsWfRepeat
(
  IN UINT32                                PmicDeviceIndex,
  IN EFI_PM_HAPTICS_WAVEFORM_REPEAT_TYPE   WfRepeatData,
  IN EFI_PM_HAPTICS_WAVEFORM_S_REPEAT_TYPE WfSRepeatData
  )
{
  EFI_STATUS                           Status  = EFI_SUCCESS;
  pm_err_flag_type                     errFlag = PM_ERR_FLAG__SUCCESS;

  errFlag = pm_haptics_wf_repeat(PmicDeviceIndex, (pm_haptics_waveform_repeat_type)WfRepeatData, (pm_haptics_waveform_s_repeat_type)WfSRepeatData);

  if(PM_ERR_FLAG__SUCCESS != errFlag)
  {
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}


/*===========================================================================
PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup @ingroup efi_pmicHaptics_protocol
@par Summary
Qualcomm PMIC HAPTICS Protocol interface.

@par Parameters
@inputprotoparams{pmic_haptics_proto_params.tex} 
*/

EFI_QCOM_PMIC_HAPTICS_PROTOCOL PmicHapticsProtocolImplementation =
{
    PMIC_HAPTICS_REVISION,
    EFI_PmicHapticsEnable,
    EFI_PmicHapticsSetActuatorType,
    EFI_PmicHapticsSetLraResonanceType,
    EFI_PmicHapticsSetSlewRate,
    EFI_PmicHapticsPlay,
    EFI_PmicHapticsSetVmaxVolt,
    EFI_PmicHapticsInternalPwmFreqSel,
    EFI_PmicHapticsExternalPwmFreqSel,
    EFI_PmicHapticsPwmCapSel,
    EFI_PmicHapticsWfRepeat,
};




