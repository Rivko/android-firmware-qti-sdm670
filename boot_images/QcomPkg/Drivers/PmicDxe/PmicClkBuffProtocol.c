/*! @file PmicClkBuffProtocol.c

*  PMIC- CLK BUFF MODULE RELATED DECLARATION
*  This file contains functions and variable declarations to support 
*  the PMIC Clk Buff module.
*
*  Copyright (c) 2012 - 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
*  Qualcomm Technologies Proprietary and Confidential.	
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/09/14   al      Arch update
13/12/13   aa      PmicLib Dec Addition
02/21/13   al      Add clk div api 
10/25/12   al      File renamed
10/23/12   al	   Updating copyright info 
09/07/12   al      New file.
===========================================================================*/

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include "pm_clk.h"
#include "pm_uefi.h"

#include <Protocol/EFIPmicClkBuff.h>

/*===========================================================================
EXTERNAL FUNCTION DECLARATIONS
===========================================================================*/
/**
EFI_PmicClkBuffEnable ()

@brief
Enables Pmic Clock buffer
*/

EFI_STATUS
EFIAPI
EFI_PmicClkBuffEnable
(
   IN UINT32               PmicDeviceIndex,
   IN EFI_PM_CLK_BUFF_TYPE ClkBuffType,
   IN BOOLEAN              Enable
   )
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

    /* Enable Pmic Clock Buff*/
    errFlag = pm_clk_sw_enable(PmicDeviceIndex, (pm_clk_type)ClkBuffType, (pm_on_off_type)Enable);
    if (PM_ERR_FLAG__SUCCESS != errFlag)
    {
        return EFI_DEVICE_ERROR;
    }

    return EFI_SUCCESS;
}


/**
EFI_PmicClkBuffSetOutDrvStrength()

@brief
Sets Pmic clock buffer output drive strength
*/

EFI_STATUS
EFIAPI
EFI_PmicClkBuffSetOutDrvStrength
(
   IN UINT32                                     PmicDeviceIndex,
   IN EFI_PM_CLK_BUFF_TYPE                       ClkBuffType,
   IN EFI_PM_CLK_BUFF_OUTPUT_DRIVE_STRENGTH_TYPE OutDrvStrength
   )
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

    /* Set clock buffer output drive strength*/
    errFlag = pm_clk_drv_strength(PmicDeviceIndex, (pm_clk_type)ClkBuffType, (pm_clk_drv_strength_type)OutDrvStrength);
    if (PM_ERR_FLAG__SUCCESS != errFlag)
    {
        return EFI_DEVICE_ERROR;
    }

    return EFI_SUCCESS;
}

/**
EFI_PmicClkBuffSetOutClkDiv()

@brief
Sets Pmic clock buffer output drive strength
*/

EFI_STATUS
EFIAPI
EFI_PmicClkBuffSetOutClkDiv
(
   IN UINT32                 PmicDeviceIndex,
   IN EFI_PM_CLK_BUFF_TYPE   ClkBuffType,
   IN UINT32                 Divider
   )
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

    /* Set clock buff output divider*/
    errFlag = pm_clk_set_out_clk_div(PmicDeviceIndex, (pm_clk_type)ClkBuffType, (uint32)Divider);
    if (PM_ERR_FLAG__SUCCESS != errFlag)
    {
        return EFI_DEVICE_ERROR;
    }

    return EFI_SUCCESS;
}

/**
EFI_PmicClkBuffGetStatus()

@brief
Reads the clock status
*/

EFI_STATUS
EFIAPI
EFI_PmicClkBuffGetStatus
(
   IN UINT32                   PmicDeviceIndex,
   IN EFI_PM_CLK_BUFF_TYPE     ClkBuffType,
   OUT EFI_PM_CLK_STATUS_TYPE  *ClkStatus
   )
{
    pm_err_flag_type  errFlag  = PM_ERR_FLAG__SUCCESS;
    pm_clk_status_type clk_status;

    if (NULL == ClkStatus)
    {
        return EFI_DEVICE_ERROR;
    }

    errFlag =  pm_clk_get_status((uint32)PmicDeviceIndex, (pm_clk_type)ClkBuffType, &clk_status);
    if (errFlag != PM_ERR_FLAG__SUCCESS)
    {
        return EFI_DEVICE_ERROR;
    }
    else
    {
        ClkStatus->DrvStrength = (EFI_PM_CLK_BUFF_OUTPUT_DRIVE_STRENGTH_TYPE)(clk_status.clk_drv_strength);
        ClkStatus->SwEnable    = (BOOLEAN)(clk_status.clk_sw_enable);
        ClkStatus->PinCtrled   = (BOOLEAN)(clk_status.clk_pin_ctrled);
        ClkStatus->PullDown    = (BOOLEAN)(clk_status.clk_pull_down);
        ClkStatus->OutDiv      = (UINT32)(clk_status.clk_out_div);
    }

    return EFI_SUCCESS;
}

/**
PMIC CLK BUFF UEFI Protocol implementation
*/
EFI_QCOM_PMIC_CLKBUFF_PROTOCOL PmicClkBuffProtocolImplementation =
{
    PMIC_CLKBUFF_REVISION,
    EFI_PmicClkBuffEnable,
    EFI_PmicClkBuffSetOutDrvStrength,
    EFI_PmicClkBuffSetOutClkDiv,
    EFI_PmicClkBuffGetStatus,
}; 
