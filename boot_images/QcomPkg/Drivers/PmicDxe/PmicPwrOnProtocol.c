/*! @file PmicPowerOnProtocol.c 

 *  PMIC-POWERON MODULE RELATED DECLARATION
 *  This file contains functions and variable declarations to support 
 *  the PMIC POWERON module.
 *
 * Copyright (c) 2012-2017 Qualcomm Technologies, Inc.  All Rights Reserved. 
 * Qualcomm Technologies Proprietary and Confidential.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/29/16   cs      Adding PON protocol API to enable/disable edge trigger
05/10/16   al      Adding PON protocol and removing deprecated ones
01/22/16   al      Added API to write to spare registers
09/16/15   al      Updating Xvdd reading API
05/05/15   al      Adding API to read PON real time irq status
04/21/15   sv      Removed unwanted variable check from PonTrigger API.
01/14/15   al      Adding API to enable/disable PON trigger
06/09/14   al      Arch update
05/19/14   sm      Added API to get PBL_PON Status
                   Deprecated GetBatteryRemovedStatus API
04/29/14   al      Deprecating unsupported APIsÅ 
02/20/14   al      Adding watchdog APIs 
13/12/13   aa      PmicLib Dec Addition
01/29/13   al      Cleaning compiler warnings  
01/24/13   al      Adding API to get pmic on/off/reset reason 
11/01/12   al      Battery removal status 
10/25/12   al      File renamed 
02/27/12   al      Added device index and resource index
04/11/12   dy      Add GetWatchDogStatus API
04/04/12   al      Added API EFI_PmicPwrUpHardReset 
03/20/12   sm      New file.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/**
  EFI interfaces
 */
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include "DALSys.h"

/**
  PMIC Lib interfaces
 */
#include "pm_uefi.h"
#include "pm_pon.h"
#include "Protocol/EFIPmicPwrOn.h"

/*===========================================================================
                  EXTERNAL FUNCTION DECLARATIONS
===========================================================================*/

EFI_STATUS 
EFIAPI 
EFI_PmicPwronWdogCfg
(
  IN UINT32                      PmicDeviceIndex, 
  IN UINT32                      S1Timer,
  IN UINT32                      S2Timer,
  IN EFI_PM_PWRON_RESET_CFG_TYPE ResetCfgType 
)
{
  pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;
  
  errFlag = pm_pon_wdog_cfg(PmicDeviceIndex, S1Timer, S2Timer,(pm_pon_reset_cfg_type)ResetCfgType);

  if(PM_ERR_FLAG__SUCCESS != errFlag)
  {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

EFI_STATUS 
EFIAPI 
EFI_PmicPwronWdogEnable
(
  IN UINT32                      PmicDeviceIndex, 
  IN BOOLEAN                     Enable
)
{
  pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;
  pm_on_off_type OnOff = (Enable)? PM_ON : PM_OFF ;

  errFlag = pm_pon_wdog_enable(PmicDeviceIndex, OnOff);

  if(PM_ERR_FLAG__SUCCESS != errFlag)
  {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}


EFI_STATUS 
EFIAPI 
EFI_PmicPwronWdogPet
(
  IN UINT32 PmicDeviceIndex
)
{
  pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

  errFlag = pm_pon_wdog_pet(PmicDeviceIndex);

  if(PM_ERR_FLAG__SUCCESS != errFlag)
  {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}


/**
  EFI_PmicPwronGetPonPblStatus ()

  @brief
  GetPonPblStatus implementation of EFI_QCOM_PMIC_POWERON_PROTOCOL
 */
EFI_STATUS 
EFIAPI 
EFI_PmicPwronGetPonPblStatus
(
  IN  UINT32 PmicDeviceIndex, 
  IN  EFI_PM_PWRON_PON_PBL_STATUS_TYPE PblStatusType,
  OUT BOOLEAN *Status
)
{
  pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

  if(!Status)
  {
    return EFI_INVALID_PARAMETER;
  }
   
  switch(PblStatusType)
  {
  case EFI_PM_PWRON_PON_PBL_STATUS_DVDD_RB_OCCURRED:
    errFlag = pm_pon_pbl_get_status(PmicDeviceIndex, PM_PON_PBL_DVDD_RB_OCCURRED, Status);
    break;
  case EFI_PM_PWRON_PON_PBL_STATUS_XVDD_RB_OCCURRED:
    errFlag = pm_pon_pbl_get_status(PmicDeviceIndex, PM_PON_PBL_XVDD_RB_OCCURRED, Status);
    break;
  default:
    errFlag = PM_ERR_FLAG_INVALID_PARAMETER;
    break;
  }

  if(PM_ERR_FLAG__SUCCESS != errFlag)
  {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
  EFI_PmicPwronSetPonTrigger ()

  @brief
  EFI_PmicPwronSetPonTrigger implementation of EFI_PM_PWRON_SET_PON_TRIGGER 
 */
EFI_STATUS 
EFIAPI 
EFI_PmicPwronSetPonTrigger
(
  IN UINT32 PmicDeviceIndex, 
  IN EFI_PM_PON_TRIGGER_TYPE PonTrigger,
  OUT BOOLEAN Enable
)
{
  pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;
  EFI_STATUS Status = EFI_SUCCESS;

  errFlag = pm_pon_trigger_enable(PmicDeviceIndex, (pm_pon_trigger_type)PonTrigger, Enable);

  Status = (PM_ERR_FLAG__SUCCESS == errFlag)? EFI_SUCCESS : EFI_DEVICE_ERROR;

  return Status;
}


/**
  EFI_PmicPwronGetPonRtStatus ()

  @brief
  EFI_PmicPwronGetPonRtStatus returns the real time interrupt status of PON module
 */
EFI_STATUS 
EFIAPI 
EFI_PmicPwronGetPonRtStatus
(
  IN UINT32              PmicDeviceIndex, 
  IN EFI_PM_PON_IRQ_TYPE PonIrqType,
  OUT BOOLEAN            *RtStatus
)
{
  EFI_STATUS Status = EFI_SUCCESS;

  pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;
 
  if(NULL == RtStatus)
  {
    return EFI_DEVICE_ERROR;
  }

  errFlag = pm_pon_irq_status(PmicDeviceIndex, (pm_pon_irq_type)PonIrqType, PM_IRQ_STATUS_RT, RtStatus);
  
  Status = (PM_ERR_FLAG__SUCCESS == errFlag)? EFI_SUCCESS : EFI_DEVICE_ERROR;

  return Status;
}

EFI_STATUS 
EFIAPI 
EFI_PmicPonGetSpareReg
(
  IN UINT32                    PmicDeviceIndex, 
  IN EFI_PM_PON_SPARE_REG_TYPE SpareReg,
  OUT UINT8                    *DataPtr
)
{
  EFI_STATUS Status = EFI_SUCCESS;

  pm_err_flag_type  err_flag = PM_ERR_FLAG__SUCCESS;
 
  if(NULL == DataPtr)
  {
    return EFI_INVALID_PARAMETER;
  }
 
  err_flag = pm_pon_get_spare_reg(PmicDeviceIndex,(pm_pon_spare_reg_type)SpareReg ,DataPtr);

  Status = (PM_ERR_FLAG__SUCCESS == err_flag)? EFI_SUCCESS : EFI_DEVICE_ERROR;

  return Status;
}


EFI_STATUS 
EFIAPI 
EFI_PmicPonSetSpareReg
(
  IN UINT32                    PmicDeviceIndex, 
  IN EFI_PM_PON_SPARE_REG_TYPE SpareReg,  
  IN UINT8                     SetValue,
  IN UINT8                     Mask
)
{
  EFI_STATUS Status = EFI_SUCCESS;

  pm_err_flag_type  err_flag = PM_ERR_FLAG__SUCCESS;
 
  err_flag = pm_pon_set_spare_reg(PmicDeviceIndex,(pm_pon_spare_reg_type)SpareReg, SetValue, Mask);

  Status = (PM_ERR_FLAG__SUCCESS == err_flag)? EFI_SUCCESS : EFI_DEVICE_ERROR;

  return Status;
}


EFI_STATUS 
EFIAPI 
EFI_PmicPonGetPonReason
(
   IN  UINT32                  PmicDeviceIndex, 
   OUT EFI_PM_PON_REASON_TYPE* PonReason
)
{
  EFI_STATUS Status = EFI_SUCCESS;

  pm_err_flag_type  err_flag = PM_ERR_FLAG__SUCCESS;

  if(NULL == PonReason)
  {
    Status = EFI_INVALID_PARAMETER;
  }
  else
  {
    err_flag = pm_pon_get_pon_reason(PmicDeviceIndex, (pm_pon_reason_type*) PonReason);
    
    Status = (PM_ERR_FLAG__SUCCESS == err_flag)? EFI_SUCCESS : EFI_DEVICE_ERROR;
  }

  return Status;
}


EFI_STATUS 
EFIAPI 
EFI_PmicPonWarmResetStatus
(
   IN  UINT32      PmicDeviceIndex, 
   OUT BOOLEAN*    WarmReset
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  pm_err_flag_type  err_flag = PM_ERR_FLAG__SUCCESS;
  pm_pon_on_state_reason_type Reason;

  if(NULL == WarmReset)
  {
    Status = EFI_DEVICE_ERROR;
  }
  else
  {
    err_flag =  pm_pon_get_on_reason(PmicDeviceIndex, &Reason);

    if(err_flag == PM_ERR_FLAG__SUCCESS)
    {
      *WarmReset = (Reason.warm_seq == TRUE) ? TRUE : FALSE;
    }

    Status = (PM_ERR_FLAG__SUCCESS == err_flag)? EFI_SUCCESS : EFI_DEVICE_ERROR;
  }

  return Status;
}


EFI_STATUS 
EFIAPI 
EFI_PmicPonGetAllPonReasons
(
   IN  UINT32      PmicDeviceIndex, 
   OUT UINT64*     PonReasons
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  pm_err_flag_type  err_flag = PM_ERR_FLAG__SUCCESS;

  if(NULL == PonReasons)
  {
    Status = EFI_DEVICE_ERROR;
  }
  else
  {
    err_flag =  pm_pon_get_all_pon_reasons(PmicDeviceIndex, PonReasons);

    Status = (PM_ERR_FLAG__SUCCESS == err_flag)? EFI_SUCCESS : EFI_DEVICE_ERROR;
  }

  return Status;
}

/**
EFI_PmicPwronEnableEdgePonTrigger ()

@brief
EFI_PmicPwronEnableEdgePonTrigger implementation of EFI_PM_PWRON_ENABLE_EDGE_PON_TRIGGER
*/
EFI_STATUS
EFIAPI
EFI_PmicPwronEnableEdgePonTrigger
(
IN UINT32 PmicDeviceIndex,
IN EFI_PM_PON_OPTION_BIT_TYPE OptionBitType,
IN BOOLEAN Enable
)
{
  pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;
  EFI_STATUS Status = EFI_SUCCESS;

  errFlag = pm_pon_set_option_bit(PmicDeviceIndex, (pm_pon_option_bit_type)OptionBitType, Enable);

  Status = (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;

  return Status;
}
/**
  PMIC PWRON UEFI Protocol implementation
 */
EFI_QCOM_PMIC_PWRON_PROTOCOL PmicPwronProtocolImplementation = 
{
  PMIC_PWRON_REVISION,
  EFI_PmicPwronGetPonPblStatus,
  EFI_PmicPwronSetPonTrigger,
  EFI_PmicPwronGetPonRtStatus,
  EFI_PmicPonGetSpareReg,
  EFI_PmicPonSetSpareReg,
  EFI_PmicPonGetPonReason,
  EFI_PmicPonWarmResetStatus,
  EFI_PmicPonGetAllPonReasons,
  EFI_PmicPwronEnableEdgePonTrigger,
};

EFI_QCOM_PMIC_PON_PROTOCOL PmicPonProtocolImplementation = 
{
  PMIC_PON_REVISION,
  EFI_PmicPonGetSpareReg,
  EFI_PmicPonSetSpareReg,
  EFI_PmicPonGetPonReason,
  EFI_PmicPonWarmResetStatus,
};
