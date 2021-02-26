/** @file PMICTest.c

Tests for PMIC

  Copyright (c) 2010-2011, 2019 Qualcomm Technologies, Inc.  All Rights Reserved.
  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED. 
  
**/

/*=============================================================================
EDIT HISTORY


when       who     what, where, why
--------   ---     ----------------------------------------------------------- 
11/26/13   al      Adding test interface for test automation
07/25/13   al      Checking for PMIC information and GPIO status
06/27/13   al      Checking for protocols presence only 
09/12/12   al      Added Clk Buff
09/12/12   al      Added RTC example calls
08/21/12   al      LPG example calls
07/31/12   al      Added example calls
03/03/11   dy      New File

=============================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/TestInterface.h>

#include <Protocol/EFIPmicVersion.h>
#include <Protocol/EFIPmicGpio.h>

#include "com_dtypes.h"

static EFI_STATUS test_version(uint32);
static EFI_STATUS test_gpio(void);

/**
The user Entry Point for Application. The user code starts with this function
as the real entry point for the application.

@param[in] ImageHandle    The firmware allocated handle for the EFI image.  
@param[in] SystemTable    A pointer to the EFI System Table.

@retval EFI_SUCCESS       The entry point is executed successfully.
@retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
PmicTestMain(
   IN EFI_HANDLE        ImageHandle,
   IN EFI_SYSTEM_TABLE  *SystemTable
   )
{
   EFI_STATUS  Status = EFI_SUCCESS;
   uint32 pmic = 0;
   uint32 pmic_presence[2] = { FALSE, FALSE };

   TEST_START("PmicTest");
   for (pmic = 0; pmic < (sizeof(pmic_presence) / sizeof(uint32)); pmic++)
   {
      Status = test_version(pmic);
      if (!Status)
      {
	 pmic_presence[pmic] = TRUE;
      }
   }

   /*check GPIO and MPP for only primary pmic*/
   /*If no primary pmic then device error*/
   if (!pmic_presence[0])
   {
      TestStatus("PmicTest",EFI_DEVICE_ERROR);
      TEST_STOP("PmicTest");
      return EFI_DEVICE_ERROR;
   }
   else
   {
      Status = test_gpio();
   }
   TestStatus("PmicTest",Status);
   TEST_STOP("PmicTest");
   return Status;
}


static EFI_STATUS
test_gpio()
{
   EFI_STATUS  Status = EFI_SUCCESS;
   static EFI_QCOM_PMIC_GPIO_PROTOCOL           *PmicGpioProtocol = NULL;
   EFI_PM_GPIO_STATUS_TYPE                       GpioStatus;


   Status = gBS->LocateProtocol(
      &gQcomPmicGpioProtocolGuid,
      NULL,
      (VOID **)&PmicGpioProtocol
      );

   if (Status != EFI_SUCCESS)
   {
      DEBUG((EFI_D_WARN, "GPIO Protocol not present\n"));
      return Status;
   }

   Status = PmicGpioProtocol->GetStatus(0, (EFI_PM_GPIO_WHICH_TYPE)0, &GpioStatus);
   if (!Status)
   {
      DEBUG((EFI_D_WARN, "***GPIO Pass\n"));
   }
   return Status;
}


static EFI_STATUS
test_version(uint32 pmic)
{
  EFI_STATUS  Status = EFI_SUCCESS;
  static EFI_QCOM_PMIC_VERSION_PROTOCOL           *PmicVersionProtocol = NULL;
  EFI_PM_DEVICE_INFO_TYPE    PmicDeviceInfo;

  Status = gBS->LocateProtocol(
    &gQcomPmicVersionProtocolGuid,
    NULL,
    (VOID **)&PmicVersionProtocol
    );

  if (Status != EFI_SUCCESS)
  {
    DEBUG((EFI_D_WARN, "PMIC version Protocol not present\n"));
    return Status;
  }

  Status = PmicVersionProtocol->GetPmicInfo(pmic, &PmicDeviceInfo);
  if (!Status)
  {
    DEBUG((EFI_D_ERROR, "***PMIC %d: DETECTED\n\r", pmic));
    switch (PmicDeviceInfo.PmicModel)
    {
    case 0:
      DEBUG((EFI_D_ERROR, "----->PmicModel: EFI_PMIC_IS_UNKNOWN \n\r"));  break;
    case 1:
      DEBUG((EFI_D_ERROR, "----->PmicModel: EFI_PMIC_IS_PM8941 \n\r"));   break;
    case 2:
      DEBUG((EFI_D_ERROR, "----->PmicModel: EFI_PMIC_IS_PM8841 \n\r"));   break;
    case 3:
      DEBUG((EFI_D_ERROR, "----->PmicModel: EFI_PMIC_IS_PM8019 \n\r"));   break;
    case 4:
      DEBUG((EFI_D_ERROR, "----->PmicModel: EFI_PMIC_IS_PM8026 \n\r"));   break;
    case 5:
      DEBUG((EFI_D_ERROR, "----->PmicModel: EFI_PMIC_IS_PM8110 \n\r"));   break;
    case 6:
      DEBUG((EFI_D_ERROR, "----->PmicModel: EFI_PMIC_IS_PMA8084 \n\r"));  break;
    case 7:
      DEBUG((EFI_D_ERROR, "----->PmicModel: EFI_PMIC_IS_SMB8962 \n\r"));  break;
    case 8:
      DEBUG((EFI_D_ERROR, "----->PmicModel: EFI_PMIC_IS_PMD9635 \n\r"));  break;
    case EFI_PMIC_IS_PM8994:
      DEBUG((EFI_D_ERROR, "----->PmicModel: EFI_PMIC_IS_PM8994 \n\r"));  break;
    case EFI_PMIC_IS_PMI8994:
      DEBUG((EFI_D_ERROR, "----->PmicModel: EFI_PMIC_IS_PMI8994 \n\r"));  break;
    case EFI_PMIC_IS_PM8916:
      DEBUG((EFI_D_ERROR, "----->PmicModel: EFI_PMIC_IS_PM8916  \n\r"));  break;
    case EFI_PMIC_IS_PM8004:
      DEBUG((EFI_D_ERROR, "----->PmicModel: EFI_PMIC_IS_PM8004 \n\r"));  break;
    case EFI_PMIC_IS_PM8909:
      DEBUG((EFI_D_ERROR, "----->PmicModel: EFI_PMIC_IS_PM8909  \n\r"));  break;
    case EFI_PMIC_IS_PM2433:
      DEBUG((EFI_D_ERROR, "----->PmicModel: EFI_PMIC_IS_PM2433  \n\r"));  break;
    case EFI_PMIC_IS_PMK8001:
      DEBUG((EFI_D_ERROR, "----->PmicModel: EFI_PMIC_IS_PMK8001 \n\r"));  break;
    case EFI_PMIC_IS_PMI8996:
      DEBUG((EFI_D_ERROR, "----->PmicModel: EFI_PMIC_IS_PMI8996 \n\r"));  break;
    case EFI_PMIC_IS_PM8998:
      DEBUG((EFI_D_ERROR, "----->PmicModel: EFI_PMIC_IS_PM8998  \n\r"));  break;
    case EFI_PMIC_IS_PMI8998:
      DEBUG((EFI_D_ERROR, "----->PmicModel: EFI_PMIC_IS_PMI8998  \n\r"));  break;
    case EFI_PMIC_IS_PM8953:
      DEBUG((EFI_D_ERROR, "----->PmicModel: EFI_PMIC_IS_PM8953  \n\r"));  break;
    case EFI_PMIC_IS_PM8005:
      DEBUG((EFI_D_ERROR, "----->PmicModel: EFI_PMIC_IS_PM8005     \n\r"));  break;
    case EFI_PMIC_IS_PM8937:
      DEBUG((EFI_D_ERROR, "----->PmicModel: EFI_PMIC_IS_PM8937     \n\r"));  break;
    default:
      DEBUG((EFI_D_ERROR, "----->PmicModel: EFI_PMIC_IS_INVALID \n\r")); break;
    }

    DEBUG((EFI_D_ERROR, "----->PmicAllLayerRevision: %d\n\r", PmicDeviceInfo.PmicAllLayerRevision));
    DEBUG((EFI_D_ERROR, "----->PmicMetalRevision: %d\n\r", PmicDeviceInfo.PmicMetalRevision));
  }
  else
  {
    DEBUG((EFI_D_ERROR, "***PMIC %d: NOT DETECTED \n\r", pmic));
  }

  return EFI_SUCCESS;
}



