/*! @file PmicVregProtocol.c 

 *  PMIC-VREG MODULE RELATED DECLARATION
 *  This file contains functions and variable declarations to support 
 *  the PMIC GPIO module.
 *
 *  Copyright (c) 2012 - 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
 *  Qualcomm Technologies Proprietary and Confidential.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/03/15   al      Updated rails API
08/29/14   al      KW fixes
08/11/14   al      Adding API to read VREG_OK
06/09/14   al      Arch update 
04/24/14   al      Adding get_status 
13/12/13   aa      PmicLib Dec Addition
10/23/12   al      Adding phase control 
01/29/13   al      Cleaning compiler warnings  
10/25/12   al      File renamed 
10/23/12   al      Updating copyright info 
9/14/12    al      Added PmicVregSetPwrMode 
7/27/12    al      Moved to QcomPkg/Drivers/PmicDxe
09/15/11   sm      Added STELEVEL and GETLEVEL functions.
05/11/11   dy      New file.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/EFIPmicVreg.h>
#include "pm_ldo.h"
#include "pm_smps.h"
#include "pm_vs.h"
#include "pm_uefi.h"

#define PM_VREG_RAIL_TYPE_SMPS      0x1
#define PM_VREG_RAIL_TYPE_LDO       0x2
#define PM_VREG_RAIL_TYPE_VS        0x3
#define PM_VREG_RAIL_TYPE_INVALID   0x4

/*===========================================================================
                  INTERNAL FUNCTION DECLARATIONS
===========================================================================*/
/*This function will return the type of rail and resource index of that particular rail*/
uint32 pm_vreg_get_rail_type(EFI_PM_VREG_ID_TYPE vreg_id, uint8 *resource_index)
{
  uint32 rail_type = PM_VREG_RAIL_TYPE_INVALID;

  if (resource_index == NULL)
  {
    rail_type = PM_VREG_RAIL_TYPE_INVALID;
  }
  else if(vreg_id < EFI_PM_LDO_INVALID) /*LDO*/
  {
    rail_type = PM_VREG_RAIL_TYPE_LDO;
    *resource_index = (uint8)(vreg_id - EFI_PM_LDO_1); 
  }
  else if(vreg_id < EFI_PM_SMPS_INVALID) /*SMPS*/
  {
    rail_type = PM_VREG_RAIL_TYPE_SMPS;
    *resource_index = (uint8)(vreg_id - EFI_PM_SMPS_1); 
  }
  else if(vreg_id < EFI_PM_VREG_INVALID) /*VS*/
  {
    rail_type = PM_VREG_RAIL_TYPE_VS;
    *resource_index = (uint8)(vreg_id - EFI_PM_VS_LVS_1); 
  }

  return rail_type;
}

/*===========================================================================
                  EXTERNAL FUNCTION DECLARATIONS
===========================================================================*/
/**
  EFI_PmicVregControl ()

  @brief
  VregControl implementation of EFI_QCOM_PMIC_VREG_PROTOCOL
 */
EFI_STATUS
EFIAPI
EFI_PmicVregControl
(
  IN UINT32                PmicDeviceIndex,  
  IN EFI_PM_VREG_ID_TYPE   VregId,
  IN BOOLEAN               Enable
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint8 resource_index = 0;

  switch (pm_vreg_get_rail_type(VregId, &resource_index))
  {
  case PM_VREG_RAIL_TYPE_SMPS:
    err_flag = pm_smps_sw_enable(PmicDeviceIndex, resource_index,(pm_on_off_type)Enable);
    break;
  case PM_VREG_RAIL_TYPE_LDO:
    err_flag = pm_ldo_sw_enable(PmicDeviceIndex, resource_index,(pm_on_off_type) Enable);
    break;
  case PM_VREG_RAIL_TYPE_VS:
    err_flag = pm_vs_sw_enable(PmicDeviceIndex, resource_index,(pm_on_off_type)Enable);
    break;
  default:
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }

  Status = (err_flag == PM_ERR_FLAG__SUCCESS)? EFI_SUCCESS : EFI_DEVICE_ERROR;

  return Status;
}

/**
  EFI_PmicVregSetLevel ()

  @brief
  VregSetLevel implementation of EFI_QCOM_PMIC_VREG_PROTOCOL
 */

EFI_STATUS
EFIAPI
EFI_PmicVregSetLevelUv
(
  IN UINT32                 PmicDeviceIndex, 
  IN EFI_PM_VREG_ID_TYPE    VregId,
  IN UINT32                 VoltageInUv
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint8 resource_index = 0;

  switch (pm_vreg_get_rail_type(VregId, &resource_index))
  {
  case PM_VREG_RAIL_TYPE_SMPS:
    err_flag = pm_smps_volt_level(PmicDeviceIndex, resource_index, VoltageInUv);
    break;
  case PM_VREG_RAIL_TYPE_LDO:
    err_flag = pm_ldo_volt_level(PmicDeviceIndex, resource_index, VoltageInUv);
    break;
  case PM_VREG_RAIL_TYPE_VS:
  default:
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }

  Status = (err_flag == PM_ERR_FLAG__SUCCESS)? EFI_SUCCESS : EFI_DEVICE_ERROR;

  return Status;
}


EFI_STATUS
EFIAPI
EFI_PmicVregSetLevel
(
  IN UINT32                 PmicDeviceIndex, 
  IN EFI_PM_VREG_ID_TYPE    VregId,
  IN UINT32                 VoltageInMv
)
{
  return EFI_PmicVregSetLevelUv(PmicDeviceIndex, VregId, (VoltageInMv*1000) );
}



/**
  EFI_PmicVregGetLevel ()

  @brief
  VregGetLevel implementation of EFI_QCOM_PMIC_VREG_PROTOCOL
 */
EFI_STATUS
EFIAPI
EFI_PmicVregGetLevel
(
  IN UINT32               PmicDeviceIndex, 
  IN EFI_PM_VREG_ID_TYPE  VregId,
  OUT UINT32              *Voltage
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint8 resource_index = 0;
  uint32 micro_volt = 0;

  if (NULL == Voltage)
  {
    return EFI_DEVICE_ERROR;
  }

  switch (pm_vreg_get_rail_type(VregId, &resource_index))
  {
  case PM_VREG_RAIL_TYPE_SMPS:
    err_flag = pm_smps_volt_level_status(PmicDeviceIndex, resource_index, &micro_volt);
    break;
  case PM_VREG_RAIL_TYPE_LDO:
    err_flag = pm_ldo_volt_level_status(PmicDeviceIndex, resource_index, &micro_volt);
    break;
  case PM_VREG_RAIL_TYPE_VS:
  default:
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    break;
  }

  *Voltage = micro_volt;

  Status = (err_flag == PM_ERR_FLAG__SUCCESS)? EFI_SUCCESS : EFI_DEVICE_ERROR;

  return Status;
}

/**
  EFI_PmicVregSetPwrMode ()

  @brief
  VregSetPwrMode implementation of EFI_QCOM_PMIC_VREG_PROTOCOL
 */
EFI_STATUS
EFIAPI
EFI_PmicVregSetPwrMode
(
  IN UINT32                  PmicDeviceIndex, 
  IN EFI_PM_VREG_ID_TYPE     VregId,
  IN EFI_PM_PWR_SW_MODE_TYPE SwMode
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint8 resource_index = 0;
  pm_sw_mode_type mode = (pm_sw_mode_type)SwMode;

  switch (pm_vreg_get_rail_type(VregId, &resource_index))
  {
  case PM_VREG_RAIL_TYPE_SMPS:
    err_flag = pm_smps_sw_mode(PmicDeviceIndex, resource_index, mode);
    break;
  case PM_VREG_RAIL_TYPE_LDO:
    err_flag = pm_ldo_sw_mode(PmicDeviceIndex, resource_index, mode);
    break;
  case PM_VREG_RAIL_TYPE_VS:
    err_flag = pm_vs_sw_mode(PmicDeviceIndex, resource_index, mode);
    break;
  default:
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }

  Status = (err_flag == PM_ERR_FLAG__SUCCESS)? EFI_SUCCESS : EFI_DEVICE_ERROR;

  return Status;
}

/**
  EFI_PmicVregMultiphaseCtrl ()

  @brief
  VregMultiphaseCtrl implementation of EFI_QCOM_PMIC_VREG_PROTOCOL
 */
EFI_STATUS
EFIAPI
EFI_PmicVregMultiphaseCtrl
(
  IN UINT32                  PmicDeviceIndex, 
  IN EFI_PM_VREG_ID_TYPE     VregId,
  IN UINT32                  NumberOfPhase
)
{
  (void)PmicDeviceIndex;
  (void)VregId;
  (void)NumberOfPhase;

  return EFI_UNSUPPORTED;

}


/**
  EFI_PmicVregGetStatus ()
  @brief
  EFI_PmicVregGetStatus implementation of EFI_QCOM_PMIC_VREG_PROTOCOL 
 */
EFI_STATUS
EFIAPI
EFI_PmicVregGetStatus
( 
  IN  UINT32                   PmicDeviceIndex, 
  IN  EFI_PM_VREG_ID_TYPE      VregId,
  OUT EFI_PM_VREG_STATUS_TYPE  *VregStatus
)
{
  
  EFI_STATUS Status = EFI_SUCCESS;
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

  pm_on_off_type  PullDown = PM_INVALID;
  pm_on_off_type  PinCtrled  = PM_INVALID;
  pm_on_off_type  SwEnable  = PM_INVALID;
  pm_sw_mode_type SwMode = PM_SW_MODE_INVALID;
  boolean VregOk = FALSE;
  uint8 resource_index = 0;

  if (NULL == VregStatus)
  {
    return EFI_INVALID_PARAMETER;
  }

  switch (pm_vreg_get_rail_type(VregId, &resource_index))
  {
  case PM_VREG_RAIL_TYPE_SMPS:
    err_flag = pm_smps_pull_down_status (PmicDeviceIndex,  resource_index, &PullDown);
    err_flag |= pm_smps_sw_mode_status (PmicDeviceIndex, resource_index, &SwMode);
    err_flag |= pm_smps_pin_ctrled_status(PmicDeviceIndex, resource_index, &PinCtrled);
    err_flag |= pm_smps_sw_enable_status (PmicDeviceIndex, resource_index, &SwEnable);
    err_flag |= pm_smps_vreg_ready_status(PmicDeviceIndex, resource_index, &VregOk);
    break;
  case PM_VREG_RAIL_TYPE_LDO:
    err_flag = pm_ldo_pull_down_status (PmicDeviceIndex,  resource_index, &PullDown); 
    err_flag |= pm_ldo_sw_mode_status (PmicDeviceIndex, resource_index, &SwMode); 
    err_flag |= pm_ldo_pin_ctrled_status(PmicDeviceIndex, resource_index, &PinCtrled); 
    err_flag |= pm_ldo_sw_enable_status (PmicDeviceIndex, resource_index, &SwEnable);
    err_flag |= pm_ldo_vreg_ready_status(PmicDeviceIndex, resource_index, &VregOk);
    break;
  case PM_VREG_RAIL_TYPE_VS:
    err_flag = pm_vs_pull_down_status (PmicDeviceIndex,  resource_index, &PullDown);
    err_flag |= pm_vs_sw_mode_status   (PmicDeviceIndex, resource_index, &SwMode);
    err_flag |= pm_vs_pin_ctrled_status(PmicDeviceIndex, resource_index, &PinCtrled); 
    err_flag |= pm_vs_sw_enable_status (PmicDeviceIndex, resource_index, &SwEnable); 
    err_flag |= pm_vs_vreg_ready_status(PmicDeviceIndex, resource_index, &VregOk);
    break;
  default:
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
 
  if (PM_ERR_FLAG__SUCCESS == err_flag && Status == EFI_SUCCESS)
  {
    VregStatus->PullDown  = (EFI_PM_ON_OFF_TYPE)PullDown;
    VregStatus->SwMode    = (EFI_PM_PWR_SW_MODE_TYPE) SwMode;
    VregStatus->PinCtrled = (EFI_PM_ON_OFF_TYPE)PinCtrled;
    VregStatus->SwEnable  = (EFI_PM_ON_OFF_TYPE)SwEnable;
    VregStatus->VregOk    = (BOOLEAN)VregOk;
  }
  else if (PM_ERR_FLAG__SUCCESS != err_flag)
  {
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}

/**
  PMIC VREG UEFI Protocol implementation
 */
EFI_QCOM_PMIC_VREG_PROTOCOL PmicVregProtocolImplementation = 
{
    PMIC_VREG_REVISION,
    EFI_PmicVregControl,
    EFI_PmicVregSetLevel,
    EFI_PmicVregGetLevel,
    EFI_PmicVregSetPwrMode,
    EFI_PmicVregMultiphaseCtrl,
    EFI_PmicVregGetStatus,
    EFI_PmicVregSetLevelUv,
};
