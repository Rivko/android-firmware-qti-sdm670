/*! @file PmicLabProtocol.c 

*  PMIC- LAB MODULE RELATED DECLARATION
*  This file contains functions and variable declarations to support 
*  the PMIC LAB module.
*
*  Copyright (c) 2014-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
*  Qualcomm Technologies Proprietary and Confidential.
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------    ---     ---------------------------------------------------------
10/03/17   ks      Added support for new IBB Type/Subtype
03/05/15   al      Adding API for setting ISENSE and PS_CTL
07/03/14   al      Updating to read VREG_OK
06/09/14   al      Arch update 
05/29/14   al      Adding API to configure Vdisp 
05/14/14   al      New file.
===========================================================================*/

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

#include "pm_uefi.h"
#include "pm_lab.h"

#include <Protocol/EFIPmicLab.h>


/*===========================================================================

                        EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/**
EFI_PmicLabLcdAmoledSel ()

@brief
Selects AMOLED or LCD
*/
EFI_STATUS 
EFIAPI 
EFI_PmicLabLcdAmoledSel        
(
   IN UINT32 PmicDeviceIndex, 
   IN BOOLEAN AmoledModeEn
)
{
    EFI_STATUS                           Status  = EFI_SUCCESS;
    pm_err_flag_type                     errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_lab_lcd_amoled_sel((uint32)PmicDeviceIndex, (boolean)AmoledModeEn);

    if(PM_ERR_FLAG__SUCCESS != errFlag)
    {
        Status = EFI_DEVICE_ERROR;
    }

    return Status;
}



/**
EFI_PmicLabModuleRdy ()

@brief
Make module ready to be enabled
*/
EFI_STATUS 
EFIAPI 
EFI_PmicLabModuleRdy           
(
   IN UINT32 PmicDeviceIndex, 
   IN BOOLEAN LabRdy
)
{
    EFI_STATUS                           Status  = EFI_SUCCESS;
    pm_err_flag_type                     errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_lab_lab_module_rdy((uint32)PmicDeviceIndex, (boolean)LabRdy);

    if(PM_ERR_FLAG__SUCCESS != errFlag)
    {
        Status = EFI_DEVICE_ERROR;
    }

    return Status;
}


/**
EFI_PmicLabModuleEn ()

@brief
enables module
*/
EFI_STATUS 
EFIAPI 
EFI_PmicLabModuleEn            
(
   IN UINT32 PmicDeviceIndex, 
   IN BOOLEAN ModuleEn
)
{
    EFI_STATUS                           Status  = EFI_SUCCESS;
    pm_err_flag_type                     errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_lab_en_lab_module((uint32)PmicDeviceIndex, (boolean)ModuleEn);

    if(PM_ERR_FLAG__SUCCESS != errFlag)
    {
        Status = EFI_DEVICE_ERROR;
    }

    return Status;
}


/**
EFI_PmicLabIbbRdyEn ()

@brief
Allows module to be reday by IBB
*/
EFI_STATUS 
EFIAPI 
EFI_PmicLabIbbRdyEn            
(
   IN UINT32 PmicDeviceIndex, 
   IN BOOLEAN IbbRdyEn
)
{
    EFI_STATUS                           Status  = EFI_SUCCESS;
    pm_err_flag_type                     errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_lab_ibb_rdy_en((uint32)PmicDeviceIndex, (boolean)IbbRdyEn);

    if(PM_ERR_FLAG__SUCCESS != errFlag)
    {
        Status = EFI_DEVICE_ERROR;
    }

    return Status;
}


/**
EFI_PmicLabConfigPrechargeCtrl ()

@brief
Configures precharge control
*/
EFI_STATUS 
EFIAPI 
EFI_PmicLabConfigPrechargeCtrl 
(
   IN UINT32 PmicDeviceIndex, 
   IN BOOLEAN FastPrecharge, 
   IN UINT32 MaxTimeUsec
   )
{
    EFI_STATUS                           Status  = EFI_SUCCESS;
    pm_err_flag_type                     errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_lab_config_precharge_ctrl((uint32)PmicDeviceIndex, (boolean)FastPrecharge, (uint32)MaxTimeUsec);

    if(PM_ERR_FLAG__SUCCESS != errFlag)
    {
        Status = EFI_DEVICE_ERROR;
    }

    return Status;
}


/**
EFI_PmicLabGetStatus ()

@brief
Gets LAB status
*/
EFI_STATUS 
EFIAPI 
EFI_PmicLabGetStatus           
(
   IN UINT32 PmicDeviceIndex, 
   OUT EFI_PM_LAB_STATUS_TYPE* LabStatus
)
{
    EFI_STATUS                           Status  = EFI_SUCCESS;
    pm_err_flag_type                     errFlag = PM_ERR_FLAG__SUCCESS;

    if(NULL == LabStatus)
    {
        return EFI_DEVICE_ERROR;
    }

    errFlag = pm_lab_get_lab_status(PmicDeviceIndex, (pm_lab_status_type *)LabStatus);

    if(PM_ERR_FLAG__SUCCESS != errFlag)
    {
        Status = EFI_DEVICE_ERROR;
    }

    return Status;
}


/**
EFI_PmicLabConfigOutputVolt ()

@brief
Configures output volt, Vdisp
*/
EFI_STATUS
EFIAPI
EFI_PmicLabConfigOutputVolt
(
    IN UINT32   PmicDeviceIndex,
    IN BOOLEAN  OverrideOutputVolt,
    IN UINT32   OutputMillivolt
    )
{   
    EFI_STATUS                           Status  = EFI_SUCCESS;
    pm_err_flag_type                     errFlag = PM_ERR_FLAG__SUCCESS;
    
    errFlag = pm_lab_config_output_volt((uint32)PmicDeviceIndex, (boolean)OverrideOutputVolt, (uint32)OutputMillivolt);

    if(PM_ERR_FLAG__SUCCESS != errFlag)
    {
        Status = EFI_DEVICE_ERROR;
    }


    return Status;
}


EFI_STATUS
EFIAPI
EFI_PmicLabConfigCurrentSense
(
    IN UINT32   PmicDeviceIndex,
    IN EFI_PM_LAB_ISENSE_TYPE  IsenseTap,
    IN EFI_PM_LAB_ISENSE_TYPE  IsenseGain
    )
{   
    EFI_STATUS                           Status  = EFI_SUCCESS;
    pm_err_flag_type                     errFlag = PM_ERR_FLAG__SUCCESS;
    
    errFlag = pm_lab_config_current_sense(PmicDeviceIndex, (pm_lab_isense_type)IsenseTap, (pm_lab_isense_type)IsenseGain);

    if(PM_ERR_FLAG__SUCCESS != errFlag)
    {
        Status = EFI_DEVICE_ERROR;
    }


    return Status;
}



EFI_STATUS
EFIAPI
EFI_PmicLabConfigPulseSkipCtrl
(
    IN UINT32   PmicDeviceIndex,
    IN UINT32   PsThreshold, 
    IN BOOLEAN  SelectPsTable, 
    IN BOOLEAN  SelectDigitalPs, 
    IN BOOLEAN  EnablePs
    )
{   
    EFI_STATUS                           Status  = EFI_SUCCESS;
    pm_err_flag_type                     errFlag = PM_ERR_FLAG__SUCCESS;
    
    errFlag = pm_lab_config_pulse_skip_ctrl(PmicDeviceIndex, PsThreshold, SelectPsTable, SelectDigitalPs, EnablePs);

    if(PM_ERR_FLAG__SUCCESS != errFlag)
    {
        Status = EFI_DEVICE_ERROR;
    }


    return Status;
}

  
/* For LAB 3.0/+ Revision */
EFI_STATUS EFI_PmicLabSwireCtrl (
    IN UINT32  PmicDeviceIndex,
    IN EFI_PM_LAB_SWIRE_PGM_CTL  SwireCtl,
    IN BOOLEAN  Enable
    )
{
    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_lab_swire_ctrl(PmicDeviceIndex, (pm_lab_swire_pgm_ctl)SwireCtl, Enable);

    return (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFI_PmicLabSetDefaultVoltage (
    IN UINT32  PmicDeviceIndex,
    IN UINT32  OpVoltageMv
    )
{
    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_lab_set_default_voltage(PmicDeviceIndex, OpVoltageMv);

    return (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFI_PmicLabCurrentLimitCfg (
    IN UINT32  PmicDeviceIndex,
    IN BOOLEAN EnCurrentLimit,
    IN UINT32  CurrentMax
    )
{
    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_lab_current_limit_cfg(PmicDeviceIndex, EnCurrentLimit, CurrentMax);

    return (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFI_PmicLabSwHighPsrrCtrl (
    IN UINT32  PmicDeviceIndex,
    IN EFI_PM_LAB_SW_HIGH_PSRR_CTL  SwHighPsrrCtl,
    IN BOOLEAN  Enable
    )
{
    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_lab_sw_high_psrr_ctrl(PmicDeviceIndex, (pm_lab_sw_high_psrr_ctl)SwHighPsrrCtl, Enable);

    return (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFI_PmicLabPdCtrl (
    IN UINT32  PmicDeviceIndex,
    IN BOOLEAN  PdEnable
    )
{
    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_lab_pd_ctrl(PmicDeviceIndex, PdEnable);

    return (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}
  
EFI_STATUS EFI_PmicLabVphEnvelopCtrl (
    IN UINT32  PmicDeviceIndex,
    IN EFI_PM_LAB_VPH_VREF_HIGH_PSRR_SEL VrefHighPsrrSel,
    IN EFI_PM_LAB_VPH_HW_HIGH_PSRR_SRC_SEL HwHighPsrrSrcSel
    )
{
    pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;

    errFlag = pm_lab_vph_envelop_ctrl(PmicDeviceIndex,
                                      (pm_lab_vph_vref_high_psrr_sel)VrefHighPsrrSel,
                                      (pm_lab_vph_hw_high_psrr_src_sel)VrefHighPsrrSel);

    return (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}


/*===========================================================================

                        PMIC LAB PROTOCOL IMPLEMENTATION

===========================================================================*/
EFI_QCOM_PMIC_LAB_PROTOCOL PmicLabProtocolImplementation = 
{
   PMIC_LAB_REVISION,
   EFI_PmicLabLcdAmoledSel,        
   EFI_PmicLabModuleRdy,           
   EFI_PmicLabModuleEn,            
   EFI_PmicLabIbbRdyEn,            
   EFI_PmicLabConfigPrechargeCtrl, 
   EFI_PmicLabGetStatus,
   EFI_PmicLabConfigOutputVolt,
   EFI_PmicLabConfigCurrentSense, 
   EFI_PmicLabConfigPulseSkipCtrl,   
    /* LAB 3.0/+ Revision */
    EFI_PmicLabSwireCtrl,
    EFI_PmicLabSetDefaultVoltage,
    EFI_PmicLabCurrentLimitCfg,
    EFI_PmicLabSwHighPsrrCtrl,
    EFI_PmicLabPdCtrl,
    EFI_PmicLabVphEnvelopCtrl,
};
