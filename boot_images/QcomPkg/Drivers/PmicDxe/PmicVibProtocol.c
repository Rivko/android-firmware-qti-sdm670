/*! @file PmicVibProtocol.c 

 *  PMIC-VIBRATOR MODULE RELATED DECLARATION
 *  This file contains functions and variable declarations to support 
 *  the PMIC Vibrator module.
 *
 * Copyright (c) 2012 - 2014 Qualcomm Technologies, Inc.  All Rights Reserved. 
 * Qualcomm Technologies Proprietary and Confidential.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/01/14   al      Removing unsupported 
06/09/14   al      Arch update
13/12/13   aa      PmicLib Dec Addition
03/15/13   al	   Updating api names
10/25/12   al      File renamed 
02/27/12   al      Commented out to complie for 8960 and to be implemented for 8974
02/13/12   sm      New file.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include "pm_vib.h"
#include "pm_uefi.h"

#include <Protocol/EFIPmicVib.h>

/*===========================================================================
                  EXTERNAL FUNCTION DECLARATIONS
===========================================================================*/
/**
  EFI_PmVibSetVolt()

  @brief
  SetVolt implementation of EFI_QCOM_PMIC_VIB_PROTOCOL
 */
EFI_STATUS
EFIAPI
EFI_PmVibSetVolt
(
  IN UINT32 PmicDeviceIndex, 
  IN EFI_PM_VIB_WHICH_TYPE Vib,
  IN UINT16 ValuemV
)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
    
    errFlag = pm_vib_set_volt( (unsigned)PmicDeviceIndex, (pm_vib_which_type)Vib,(uint16)ValuemV );
    
    if (errFlag != PM_ERR_FLAG__SUCCESS)
    {
        return EFI_DEVICE_ERROR;
    }
    
    return EFI_SUCCESS;
}

/**
  EFI_PmVibEnVib()

  @brief
  SetMode implementation of EFI_QCOM_PMIC_VIB_PROTOCOL
 */
EFI_STATUS
EFIAPI
EFI_PmVibEnVib
(
  IN UINT32 PmicDeviceIndex, 
  IN EFI_PM_VIB_WHICH_TYPE Vib,
  IN EFI_PM_VIB_MODE_TYPE Mode,
  IN BOOLEAN Enable
)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
    
    errFlag = pm_vib_enable((unsigned)PmicDeviceIndex,(pm_vib_which_type)Vib, (pm_vib_mode_type)Mode, (boolean)Enable);
    
    if (errFlag != PM_ERR_FLAG__SUCCESS)
    {
        return EFI_DEVICE_ERROR;
    }
    
    return EFI_SUCCESS;
}


/**
  PMIC VIB UEFI Protocol implementation
 */
EFI_QCOM_PMIC_VIB_PROTOCOL PmicVibProtocolImplementation = 
{
  PMIC_VIB_REVISION,
  EFI_PmVibSetVolt,
  EFI_PmVibEnVib,
};

