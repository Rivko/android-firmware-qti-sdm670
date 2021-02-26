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
08/18/17   mr      Added support to read Pmic Extended info (with Variant revision)
13/12/13   aa      PmicLib Dec Addition
04/11/13   al      Created
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include "pm_uefi.h"
#include "pm_version.h"

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
  pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
  EFI_PM_DEVICE_INFO_TYPE PmicDeviceInfo;

  errFlag = pm_get_pmic_info((uint8)PmicDeviceIndex, (pm_device_info_type*)&PmicDeviceInfo);

  if (errFlag || (!DeviceInfo))
  {
     return EFI_DEVICE_ERROR;
  }

  *DeviceInfo = PmicDeviceInfo;
  return EFI_SUCCESS;

}

/**
  EFI_PmicGetPmicInfoExt ()

  @brief
  GetPmicInfo (Extended PMIC Info with Variant revision) implementation of EFI_QCOM_PMIC_VERSION_PROTOCOL
 */
EFI_STATUS
EFIAPI
EFI_PmicGetPmicInfoExt
(
  IN   UINT32                      PmicDeviceIndex,
  OUT  EFI_PM_DEVICE_INFO_EXT_TYPE *DeviceInfo
)
{
  if(!DeviceInfo)
  {
    return EFI_DEVICE_ERROR;
  }

  pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
  EFI_PM_DEVICE_INFO_TYPE PmicDeviceInfo;
  UINT8 PmicVariantRevision = 0;

  errFlag  = pm_get_pmic_info((uint8)PmicDeviceIndex, (pm_device_info_type*)&PmicDeviceInfo);
  errFlag |= pm_get_pmic_variant((uint8)PmicDeviceIndex, &PmicVariantRevision);
  if(errFlag != PM_ERR_FLAG__SUCCESS)
  {
    return EFI_DEVICE_ERROR;
  }

  DeviceInfo->PmicModel = PmicDeviceInfo.PmicModel;
  DeviceInfo->PmicAllLayerRevision = PmicDeviceInfo.PmicAllLayerRevision;
  DeviceInfo->PmicMetalRevision = PmicDeviceInfo.PmicMetalRevision;
  DeviceInfo->PmicVariantRevision = (UINT32)PmicVariantRevision;

  return EFI_SUCCESS;
}


/**
  PMIC VERSION UEFI Protocol implementation
 */
EFI_QCOM_PMIC_VERSION_PROTOCOL PmicVersionProtocolImplementation = 
{
    PMIC_VERSION_REVISION,
    EFI_PmicGetPmicInfo,
    EFI_PmicGetPmicInfoExt
};
