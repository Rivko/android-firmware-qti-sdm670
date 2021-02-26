/*! @file PmicPowerOnProtocol.c 

 *  PMIC-POWERON MODULE RELATED DECLARATION
 *  This file contains functions and variable declarations to support 
 *  the PMIC POWERON module.
 *
 * Copyright (c) 2012-2016 Qualcomm Technologies, Inc.  All Rights Reserved. 
 * Qualcomm Technologies Proprietary and Confidential.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
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
  return EFI_SUCCESS;
}


EFI_STATUS 
EFIAPI 
EFI_PmicPwronWdogPet
(
  IN UINT32 PmicDeviceIndex
)
{
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
  return EFI_SUCCESS;
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
  return EFI_SUCCESS;
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
  return EFI_SUCCESS;
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
  return EFI_SUCCESS;
}


EFI_STATUS 
EFIAPI 
EFI_PmicPonGetPonReason
(
   IN  UINT32                  PmicDeviceIndex, 
   OUT EFI_PM_PON_REASON_TYPE* PonReason
)
{
  return EFI_SUCCESS;
}


EFI_STATUS 
EFIAPI 
EFI_PmicPonWarmResetStatus
(
   IN  UINT32      PmicDeviceIndex, 
   OUT BOOLEAN*    WarmReset
  )
{
  return EFI_SUCCESS;
}


EFI_STATUS 
EFIAPI 
EFI_PmicPonGetAllPonReasons
(
   IN  UINT32      PmicDeviceIndex, 
   OUT UINT64*     PonReasons
  )
{
  return EFI_SUCCESS;
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
};

EFI_QCOM_PMIC_PON_PROTOCOL PmicPonProtocolImplementation = 
{
  PMIC_PON_REVISION,
  EFI_PmicPonGetSpareReg,
  EFI_PmicPonSetSpareReg,
  EFI_PmicPonGetPonReason,
  EFI_PmicPonWarmResetStatus,
};
