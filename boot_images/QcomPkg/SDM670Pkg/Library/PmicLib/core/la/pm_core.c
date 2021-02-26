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
04/12/17   va      Enable FG,schg protocol
07/07/16   cm      Remove Gold cluster enable.
06/01/16   sm      Removed EFI_QCOM_CHARGER_EX_PROTOCOL related changes
05/10/16   al      Adding PON protocol 
05/06/16   sm      Added Pmic Charger Protocol to install 
04/09/16   al      Enable Gold cluster
03/30/16   va      enable protocol 
03/09/16   va      Moved common protocol install to pmic.c
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
#include "Protocol/EFIPmicSchg.h"
#include "Protocol/EFIPmicFg.h"
#include "Protocol/EFIPmicVreg.h"
#include "Protocol/EFIPmicPwrOn.h"


/*===========================================================================

                 LOCAL CONSTANT AND MACRO DEFINITIONS

===========================================================================*/

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
  EFI_STATUS  Status = EFI_SUCCESS;
  

  /* Install platform specific PMIC protocols */
  Status = gBS->InstallMultipleProtocolInterfaces(
    &ImageHandle,
     &gQcomPmicSchgProtocolGuid,
     &PmicSchgProtocolImplementation,
     &gQcomPmicFgProtocolGuid,
     &PmicFgBasicProtocolImplementation,
     &gQcomPmicPonProtocolGuid, /*installing gQcomPmicPonProtocolGuid for open source*/
     &PmicPonProtocolImplementation,
     NULL
    );

  return (pm_err_flag_type)(Status);
}

pm_err_flag_type pm_post_pmic_usb_initialization(void)
{
  return PM_ERR_FLAG__SUCCESS;
}

