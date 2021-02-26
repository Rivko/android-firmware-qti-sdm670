/*! @file PmicVersionProtocol.c 

 *  PMIC-VERSION MODULE RELATED DECLARATION
 *  This file contains functions and variable declarations to support 
 *  the PMIC GPIO module.
 *
 *  Copyright (c) 2012 - 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 *  Qualcomm Technologies Proprietary and Confidential.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
13/12/13   aa      PmicLib Dec Addition
04/11/13   al      Created
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include "pm_uefi.h"

#include <Protocol/EFIPmicVersion.h>

/*===========================================================================
                  EXTERNAL FUNCTION DECLARATIONS
===========================================================================*/

/**
  EFI_PmicGetPmicInfo ()

  @brief
  GetPmicInfo implementation of EFI_QCOM_PMIC_VERSION_PROTOCOL
 */
EFI_STATUS
EFIAPI
EFI_PmicGetPmicInfo
(
  IN   UINT32                     PmicDeviceIndex, 
  OUT  EFI_PM_DEVICE_INFO_TYPE    *DeviceInfo
)
{
  if (DeviceInfo==NULL)
  {
     return EFI_DEVICE_ERROR;
  }

  switch (PmicDeviceIndex)
  {
    case 0:
      DeviceInfo->PmicModel = EFI_PMIC_IS_PM8998;
      DeviceInfo->PmicAllLayerRevision = 2;
      DeviceInfo->PmicMetalRevision = 0;
      break;
    case 1:
      DeviceInfo->PmicModel = EFI_PMIC_IS_PMI8998;
      DeviceInfo->PmicAllLayerRevision = 2;
      DeviceInfo->PmicMetalRevision = 0;
      break;
    case 2:
      DeviceInfo->PmicModel = EFI_PMIC_IS_PM8005;
      DeviceInfo->PmicAllLayerRevision = 2;
      DeviceInfo->PmicMetalRevision = 0;
      break;
    
  }

  return EFI_SUCCESS;

}


/**
  PMIC VERSION UEFI Protocol implementation
 */
EFI_QCOM_PMIC_VERSION_PROTOCOL PmicVersionProtocolImplementation = 
{
    PMIC_VERSION_REVISION,
    EFI_PmicGetPmicInfo
};
