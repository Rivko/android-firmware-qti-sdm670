/*! @file PmicLcdbProtocol.c

*  PMIC - LCDB MODULE RELATED DECLARATION
*  This file contains functions and variable declarations to support
*  the PMIC LCDB module.
*
*  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
*  Qualcomm Technologies Proprietary and Confidential.
*/

/*===========================================================================

                        EDIT HISTORY

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when        who     what, where, why
--------    ----    ---------------------------------------------------------
11/14/16    mr      Initial revision
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

#include "pm_uefi.h"
#include "pm_lcdb.h"

#include <Protocol/EFIPmicLcdb.h>



/*===========================================================================

                        EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
EFI_STATUS EFI_PmicLcdbGetStatus (
    IN  BOOLEAN PmicDeviceIndex,
    OUT EFI_PM_LCDB_STS_TYPE *LcdbStatus
    )
{
    if(!LcdbStatus)
    {
        return EFI_DEVICE_ERROR;
    }

    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_lcdb_get_status(PmicDeviceIndex, (pm_lcdb_sts_type *)LcdbStatus);

    return (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFI_PmicLcdbModuleEnable (
    IN BOOLEAN PmicDeviceIndex,
    IN BOOLEAN ModuleEn
    )
{
    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;
    STATIC BOOLEAN ModuleRdy = FALSE;

    if(ModuleEn && !ModuleRdy)
    {
        ModuleRdy = TRUE;
        errFlag = pm_lcdb_set_module_rdy(PmicDeviceIndex, ModuleRdy);
    }
    
    errFlag |= pm_lcdb_module_enable(PmicDeviceIndex, ModuleEn);
    
    return (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFI_PmicLcdbSetModuleRdy (
    IN BOOLEAN PmicDeviceIndex,
    IN BOOLEAN ModuleRdy
    )
{
    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_lcdb_set_module_rdy(PmicDeviceIndex, ModuleRdy);

    return (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFI_PmicLcdbSetConfigSel (
    IN BOOLEAN PmicDeviceIndex,
    IN EFI_PM_LCDB_CONFIG_SEL ConfigSel
    )
{
    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_lcdb_set_config_sel(PmicDeviceIndex, (pm_lcdb_config_sel)ConfigSel);

    return (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFI_PmicLcdbSetBoostOpVoltage (
    IN BOOLEAN PmicDeviceIndex,
    IN UINT32 OpVoltageMv
    )
{
    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_lcdb_set_boost_output_voltage(PmicDeviceIndex, OpVoltageMv);

    return (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFI_PmicLcdbModuleHwEnable (
    IN BOOLEAN PmicDeviceIndex,
    IN BOOLEAN HwEnRdy
    )
{
    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_lcdb_module_hw_enable(PmicDeviceIndex, HwEnRdy);

    return (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFI_PmicLcdbCurrSenseCfg (
    IN BOOLEAN PmicDeviceIndex,
    IN EFI_PM_LCDB_CURR_SENSE_TYPE *CurrSense
    )
{
    if(!CurrSense)
    {
        return EFI_DEVICE_ERROR;
    }

    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_lcdb_curr_sense_cfg(PmicDeviceIndex, (pm_lcdb_curr_sense_type *)CurrSense);

    return (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFI_PmicLcdbPsCtl (
    IN BOOLEAN PmicDeviceIndex,
    IN EFI_PM_LCDB_PS_CTL_TYPE *PsCtl
    )
{
    if(!PsCtl)
    {
        return EFI_DEVICE_ERROR;
    }

    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_lcdb_ps_ctl(PmicDeviceIndex, (pm_lcdb_ps_ctl_type *)PsCtl);

    return (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFI_PmicLcdbPfmCtl (
    IN BOOLEAN PmicDeviceIndex,
    IN EFI_PM_LCDB_PFM_CTL_TYPE *PfmCtl
    )
{
    if(!PfmCtl)
    {
        return EFI_DEVICE_ERROR;
    }

    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_lcdb_pfm_ctl(PmicDeviceIndex, (pm_lcdb_pfm_ctl_type *)PfmCtl);

    return (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFI_PmicLcdbSetLdoOpVoltage (
    IN BOOLEAN PmicDeviceIndex,
    IN UINT32 OpVoltageMv
    )
{
    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_lcdb_set_ldo_output_voltage(PmicDeviceIndex, OpVoltageMv);

    return (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFI_PmicLcdbSetNcpOpVoltage (
    IN BOOLEAN PmicDeviceIndex,
    IN BOOLEAN NcpVoutEn,
    IN INT32 OpVoltageMv /* negative value */
    )
{
    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_lcdb_set_ncp_output_voltage(PmicDeviceIndex, NcpVoutEn, OpVoltageMv);

    return (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFI_PmicLcdbNcpIlimCtl (
    IN BOOLEAN PmicDeviceIndex,
    IN EFI_PM_LCDB_NCP_ILIM_CTL_TYPE *NcpIlimCtl
    )
{
    if(!NcpIlimCtl)
    {
        return EFI_DEVICE_ERROR;
    }

    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_lcdb_ncp_ilim_ctl(PmicDeviceIndex, (pm_lcdb_ncp_ilim_ctl_type *)NcpIlimCtl);

    return (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFI_PmicLcdbNcpSoftstartCtl (
    IN BOOLEAN PmicDeviceIndex,
    IN EFI_PM_LCDB_NCP_SOFTSTART_CTL NcpSoftstartTime
    )
{
    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_lcdb_ncp_soft_start_ctl(PmicDeviceIndex, (pm_lcdb_ncp_softstart_ctl)NcpSoftstartTime);

    return (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}


/**
PMIC LCDB UEFI Protocol implementation
*/
EFI_QCOM_PMIC_LCDB_PROTOCOL PmicLcdbProtocolImplementation =
{
   PMIC_LCDB_REVISION,
   EFI_PmicLcdbGetStatus,
   EFI_PmicLcdbModuleEnable,
   // EFI_PmicLcdbSetModuleRdy,
   EFI_PmicLcdbSetConfigSel,
   EFI_PmicLcdbSetBoostOpVoltage,
   EFI_PmicLcdbModuleHwEnable,
   EFI_PmicLcdbCurrSenseCfg,
   EFI_PmicLcdbPsCtl,
   EFI_PmicLcdbPfmCtl,
   EFI_PmicLcdbSetLdoOpVoltage,
   EFI_PmicLcdbSetNcpOpVoltage,
   EFI_PmicLcdbNcpIlimCtl,
   EFI_PmicLcdbNcpSoftstartCtl,
};
