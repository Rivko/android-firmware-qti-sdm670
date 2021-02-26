/*! @file PmicOledbProtocol.c

*  PMIC - OLEDB MODULE RELATED DECLARATION
*  This file contains functions and variable declarations to support
*  the PMIC OLEDB module.
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
04/10/17    ks      Added EFI_PmicOledbDispOffPbsTrigger protocol(CR-2004206)
02/07/17    sv      Added EFI_PmicOledbPdCtr protocol
11/14/16    mr      Initial revision
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

#include "pm_uefi.h"
#include "pm_oledb.h"
#include "pm_pbs_client.h"
#include "pm_version.h"

#include <Protocol/EFIPmicOledb.h>


/*===========================================================================

                        EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
EFI_STATUS EFI_PmicOledbGetStatus (
    IN  BOOLEAN PmicDeviceIndex,
    OUT EFI_PM_OLEDB_STS_TYPE *OledbStatus
    )
{
    if(!OledbStatus)
    {
        return EFI_DEVICE_ERROR;
    }

    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_oledb_get_status(PmicDeviceIndex, (pm_oledb_sts_type *)OledbStatus);

    return (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFI_PmicOledbModuleEnable (
    UINT8 PmicDeviceIndex,
    BOOLEAN ModuleEn
    )
{
    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_oledb_module_enable(PmicDeviceIndex, ModuleEn);

    return (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFI_PmicOledbSetModuleRdy (
    UINT8 PmicDeviceIndex,
    BOOLEAN ModuleRdy
    )
{
    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_oledb_set_module_rdy(PmicDeviceIndex, ModuleRdy);

    return (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFI_PmicOledbSetExtPinCtl (
    UINT8 PmicDeviceIndex,
    BOOLEAN ExtPinCtl
    )
{
    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_oledb_set_ext_pin_ctl(PmicDeviceIndex, ExtPinCtl);

    return (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFI_PmicOledbSwireCtl (
    UINT8 PmicDeviceIndex,
    EFI_PM_OLEDB_SWIRE_CONTROL SwireControl,
    BOOLEAN Enable
    )
{
    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_oledb_swire_ctl(PmicDeviceIndex, (pm_oledb_swire_control)SwireControl, Enable);

    return (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFI_PmicOledbVoutProgramming (
    UINT8 PmicDeviceIndex,
    EFI_PM_OLEDB_VOUT_PROGRAM VoutProgram,
    UINT32 OpVoltageMv
    )
{
    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_oledb_vout_programming(PmicDeviceIndex, (pm_oledb_vout_program)VoutProgram, OpVoltageMv);

    return (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFI_PmicOledbPdCtrl(
    IN UINT8 PmicDeviceIndex,
    IN BOOLEAN PdEnable
    )
{
    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_oledb_pd_ctrl(PmicDeviceIndex, PdEnable);

    return (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFI_PmicOledbSoftstartCtl (
    UINT8 PmicDeviceIndex,
    EFI_PM_OLEDB_SOFTSTART_CTL_TYPE *SoftstartCtl
    )
{
    if(!SoftstartCtl)
    {
        return EFI_DEVICE_ERROR;
    }

    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_oledb_softstart_ctl(PmicDeviceIndex, (pm_oledb_softstart_ctl_type *)SoftstartCtl);

    return (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFI_PmicOledbPsmCtl (
    UINT8 PmicDeviceIndex,
    EFI_PM_OLEDB_PSM_CTL_TYPE *PsmCtl
    )
{
    if(!PsmCtl)
    {
        return EFI_DEVICE_ERROR;
    }

    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_oledb_psm_ctl(PmicDeviceIndex, (pm_oledb_psm_ctl_type *)PsmCtl);

    return (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFI_PmicOledbPfmCtl (
    UINT8 PmicDeviceIndex,
    EFI_PM_OLEDB_PFM_CTL_TYPE *PfmCtl
    )
{
    if(!PfmCtl)
    {
        return EFI_DEVICE_ERROR;
    }

    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_oledb_pfm_ctl(PmicDeviceIndex, (pm_oledb_pfm_ctl_type *)PfmCtl);

    return (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFI_PmicOledbDispOffPbsTrigger (
    UINT8 PmicDeviceIndex
    )
{
    boolean SwirePulseStatus = FALSE;
    uint8 TriggerArg =1;
    uint8 TriggerReturn =1;
    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

    pm_device_info_type PmicDeviceInfo;
    uint8 pmic_variant;

    errFlag  = pm_get_pmic_info(PmicDeviceIndex, &PmicDeviceInfo);
    errFlag |= pm_get_pmic_variant(PmicDeviceIndex, &pmic_variant);
    if( ( PMIC_IS_PM660L == PmicDeviceInfo.ePmicModel) && ( PmicDeviceInfo.nPmicAllLayerRevision <= 2 ))
    { 
      if( !((2 == PmicDeviceInfo.nPmicAllLayerRevision) && (PmicDeviceInfo.nPmicMetalRevision > 0) && (pmic_variant > 0)))
      { 
        errFlag |= pm_oledb_get_swire_sparebit_status(PmicDeviceIndex, &SwirePulseStatus); /* spare bit will be set by PBS RAM when 51 pulses are seen on SWIRE during Display on */
        if( PM_ERR_FLAG__SUCCESS !=errFlag )
        {
          return errFlag; 
        }
        if( TRUE == SwirePulseStatus)
        {
            errFlag = pm_pbs_oledb_sw_trigger(PmicDeviceIndex, PM_PBS_CLIENT_2, PM_PBS_CLIENT_SW_TRIGGER, &TriggerArg, &TriggerReturn);
            if(PM_ERR_FLAG__SUCCESS == errFlag)
            {
              errFlag =pm_oledb_clear_swire_sparebit_status(PmicDeviceIndex);
            }
        }
        else
        {
          DEBUG((EFI_D_WARN,"OledbDispOffPbsTrigger: Oledb sparebit is not set\n"));
        }
      }
    }
    return (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}


/**
PMIC OLEDB UEFI Protocol implementation
*/
EFI_QCOM_PMIC_OLEDB_PROTOCOL PmicOledbProtocolImplementation =
{
   PMIC_OLEDB_REVISION,
   EFI_PmicOledbGetStatus,
   EFI_PmicOledbModuleEnable,
   EFI_PmicOledbSetModuleRdy,
   EFI_PmicOledbSetExtPinCtl,
   EFI_PmicOledbSwireCtl,
   EFI_PmicOledbVoutProgramming,
   EFI_PmicOledbPdCtrl,
   EFI_PmicOledbSoftstartCtl,
   EFI_PmicOledbPsmCtl,
   EFI_PmicOledbPfmCtl,
   EFI_PmicOledbDispOffPbsTrigger,
};
