/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  PMIC Startup Services

GENERAL DESCRIPTION
  This file contains initialization functions and corresponding variable
  declarations to support interaction with the Qualcomm Pmic chips.

  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/18/17   va      Moving target specific call to target file
04/18/17   va      Added API call to set USB typeC port role in PmicInitialize based on HostModeSupported PCD
04/12/17   va      Adding PON protocol 
02/07/17   sm      Added changes to skip installing SCHG and FG protocols for CLS platform
03/30/16   va      enable protocol 
03/09/16   va      moved common protocol install to pmic.c
03/08/16   al      Moving NPA to common
07/23/15   al      Adding NPA
07/21/14   va      Adding Fuel Gauge Protocol
07/03/14   al      Adding MIPI-BIF, RGB. Removing SW workaround since it doesn't solve issue
06/06/14   al      Adding workaround for HW issue 
04/28/14   va      Expose Npa Test protocol
05/09/14   al      Adding IBB and LAB 
04/18/14   al      Added SMBCHG 
11/22/13   va      PmicLib Dec Addition
10/02/13   al      New File
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "com_dtypes.h"
#include "pm_uefi.h"
#include <Library/UefiBootServicesTableLib.h>

/**
  PMIC PROTOCOL interface
*/

#include "Protocol/EFIPmicFg.h"
#include "Protocol/EFIPmicSchg.h"
#include "Protocol/EFIPmicUsb.h"
#include "Protocol/EFIPlatformInfo.h"
#include "Protocol/EFIPmicPon.h"
#include <Library/QcomLib.h>
#include "pm_version.h"

/*===========================================================================

                 LOCAL CONSTANT AND MACRO DEFINITIONS

===========================================================================*/
#define PM_SECONDARY_PMIC_INDEX  1


/*===========================================================================

                VARIABLES DEFINITIONS

===========================================================================*/

extern EFI_QCOM_PMIC_SCHG_PROTOCOL        PmicSchgProtocolImplementation;
extern EFI_QCOM_PMIC_FG_BASIC_PROTOCOL    PmicFgBasicProtocolImplementation;
extern EFI_QCOM_PMIC_PON_PROTOCOL         PmicPonProtocolImplementation;


/*===========================================================================

                LOCAL FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

                EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================
FUNCTION pm_install_uefi_protocol                                EXTERNAL FUNCTION

DESCRIPTION
    This function installs pmic uefi protocols.

    It does the following:
    1)  It initializes the target specific pmic uefi protocol 

INPUT PARAMETERS
  Imagehandle and SystemTable pointer

RETURN VALUE
  pm_err_flag_type

DEPENDENCIES
  pm_init must have been called

SIDE EFFECTS
  None.
===========================================================================*/

pm_err_flag_type pm_post_pmic_initialization(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_STATUS                  Status          = EFI_SUCCESS;
  EFI_PLATFORMINFO_PLATFORM_TYPE PlatformType;

  Status = GetPlatformType(&PlatformType);
  if(EFI_SUCCESS != Status)
  {
    DEBUG((EFI_D_WARN, "PmicDxe:: %a Error getting platform type  \r\n", __FUNCTION__));
    return (pm_err_flag_type)EFI_DEVICE_ERROR;
  }

  /* Install platform specific PMIC protocols */
  Status = gBS->InstallMultipleProtocolInterfaces( &ImageHandle,
                                                   &gQcomPmicPonProtocolGuid, /*installing gQcomPmicPonProtocolGuid for open source*/
                                                   &PmicPonProtocolImplementation,
                                                   NULL
                                                   );

  if(pm_get_pmic_model(PM_SECONDARY_PMIC_INDEX) == PMIC_IS_PMI8998)
  {
    /* Install platform specific PMIC protocols */
    Status = gBS->InstallMultipleProtocolInterfaces( &ImageHandle,
                                                     &gQcomPmicSchgProtocolGuid,
                                                     &PmicSchgProtocolImplementation,
                                                     &gQcomPmicFgProtocolGuid,
                                                     &PmicFgBasicProtocolImplementation,
                                                     NULL
                                                     );
  }
  return (pm_err_flag_type)(Status);
}

pm_err_flag_type pm_post_pmic_usb_initialization()
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_QCOM_PMIC_USB_PROTOCOL *PmicUsbProtocol = NULL;

  /*Host mode feature is supported based on the PCD flag */
  if(pm_get_pmic_model(PM_SECONDARY_PMIC_INDEX) == PMIC_IS_PMI8998)
  {
    Status = gBS->LocateProtocol(&gQcomPmicUsbProtocolGuid, NULL, (void**)&PmicUsbProtocol);
    if (EFI_SUCCESS != Status)
    {
      PmicUsbProtocol = NULL;
      DEBUG(( EFI_D_WARN, "PmicDxe:: %a Failed to open PMIC USB protocol Status = %r \r\n", __FUNCTION__, Status));
    }

    if(PmicUsbProtocol)
    {
       Status = PmicUsbProtocol->UsbCSetPortRole(EFI_PM_USB_TYPEC_ROLE_DRP);
    }
  }
  return (pm_err_flag_type)(Status);
}


