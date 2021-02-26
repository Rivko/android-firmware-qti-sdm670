/* @file Pmic.c

  Implements the Pmic protocol
  
  Copyright (c) 2011-2016 Qualcomm Technologies, Inc.  All Rights Reserved. 
  Qualcomm Technologies Proprietary and Confidential.
*/


/*=============================================================================
                              EDIT HISTORY


 when         who     what, where, why
 --------     ---     -----------------------------------------------------------
 08/22/16     pb      Enable NPA test protocol   
 08/19/16     pb      Enabled Test protocol (added QcomTargetLib.h for PRODMODE)
 03/24/16     al      Add keypress support
 03/13/16     ck      Enable PMIC NPA client support
 03/09/16     va      Moved common protocol install to pmic.c
 03/08/16     al      Checking for NpaDxe before initializing PAM   
 07/23/15     al      Refactoring code
 11/12/14     al      Adding core specific DAL initialization
 10/07/13     al      Moving platform config to target
 05/06/13     al      Storing pmic info and FabId
 04/01/13     sm      Added check for BattChgApp UEFI Variable
 03/07/13     dy      Add shutdown path for Liquid if pon trigger is charger
 11/06/12     dy      Add SMB350 Workaround
 10/25/12     al      Moved protocol installation to targets
=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/QcomLib.h>
#include <Library/DebugLib.h>
#include "DALDeviceId.h"
#include "DALFramework.h"


#include "pm_uefi.h"

/**
  PMIC PROTOCOL interface
 */
#include <Library/UefiBootServicesTableLib.h>
#include <Library/QcomTargetLib.h>
#include "Protocol/EFIPmicPwrOn.h"
#include "Protocol/EFIPmicVersion.h"
#include "Protocol/EFIPmicUsb.h"

/*===========================================================================

                VARIABLES DEFINITIONS

===========================================================================*/
extern EFI_QCOM_PMIC_PWRON_PROTOCOL     PmicPwronProtocolImplementation;
extern EFI_QCOM_PMIC_VERSION_PROTOCOL   PmicVersionProtocolImplementation;
extern EFI_QCOM_PMIC_USB_PROTOCOL       PmicUsbProtocolImplementation;
/*===========================================================================

                 LOCAL CONSTANT AND MACRO DEFINITIONS

===========================================================================*/


#define ARRAY_LENGTH(x)  (sizeof(x) / sizeof(x[0]))

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
extern DALREG_DriverInfo DALSystem_DriverInfo;

static StringDevice gaDALModDriverList[] =
{
   {"/pmic/target", 0, 0, &DALSystem_DriverInfo, 0, NULL},
   {"/pmic/common", 0, 0, &DALSystem_DriverInfo, 0, NULL},
};


static DALSYSConfig gPmicDALSYSModConfig =
{
   // string device info
   {ARRAY_LENGTH(gaDALModDriverList), gaDALModDriverList},
   //numeric device info
   {0,NULL}
};




/**
  PmicInitialize ()

  @brief
  Init implementation of EFI_QCOM_PMIC_CLIENT_DEVICE_PROTOCOL
 */
EFI_STATUS
PmicInitialize(
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable)
{
  EFI_STATUS Status = EFI_SUCCESS;

  /* initialize DAL */
  DALSYS_InitMod(&gPmicDALSYSModConfig);

  /*install common protocols*/
  Status |= pm_install_protocol(ImageHandle,SystemTable);

  return Status;
}


pm_err_flag_type pm_install_protocol(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_STATUS  Status = EFI_SUCCESS;

  Status  = gBS->InstallMultipleProtocolInterfaces(
    &ImageHandle,
    &gQcomPmicPwrOnProtocolGuid,
    &PmicPwronProtocolImplementation,
    &gQcomPmicVersionProtocolGuid,
    &PmicVersionProtocolImplementation,
    &gQcomPmicUsbProtocolGuid,
    &PmicUsbProtocolImplementation,
    NULL
    );

  return (pm_err_flag_type)(Status);
}



