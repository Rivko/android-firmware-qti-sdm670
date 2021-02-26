/** @file PlatformInfoApp.c
   
  Test application for the PlatformInfo driver.

  Copyright (c) 2011, 2018, Qualcomm Technologies, Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/06/18   yps     Added more platform strings
 03/07/11   mdt     Created.

=============================================================================*/



/*=========================================================================
      Include Files
==========================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/TestInterface.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/EFIPlatformInfo.h>


/*=========================================================================
      Macro Definitions
==========================================================================*/

#define PLATFORMINFO_TEST_NAME "PlatformInfoTest"


/*=========================================================================
      Data Definitions
==========================================================================*/

static const char *aPlatformTypeStrings[EFI_PLATFORMINFO_NUM_TYPES] =
{
  "UNKNOWN",
  "CDP (SURF)",
  "FFA",
  "FLUID",
  "FUSION",
  "OEM",
  "QT",
  "MTP_MDM",
  "MTP",
  "LiQUID",
  "DragonBoard",
  "QRD",
  "EVB",
  "HRD",
  "DTV",
  "RUMI",
  "VIRTIO",
  "GOBI",
  "CBH",
  "BTS",
  "XPM",
  "RCM",
  "DMA",
  "STP",
  "SBC",
  "ADP",
  "CHI",
  "SDP",
  "RRP",
  "CLS",
  "TTP",
  "HDK",
  "IOT",
  "ATP",
  "IDP",
};

static const char *aPlatformKeyStrings[EFI_PLATFORMINFO_NUM_KEYS] =
{
  "UNKNOWN",
  "DDR_FREQ",
  "GFX_FREQ",
  "CAMERA_FREQ",
  "FUSION",
  "CUST",
};


/*=========================================================================
      Functions
==========================================================================*/


/**
  Test PlatformInfo application entry point. 

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/

EFI_STATUS
EFIAPI
PlatformInfoAppMain (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                          eResult;
  EFI_PLATFORMINFO_PROTOCOL           *hPlatformInfoProtocol;
  EFI_PLATFORMINFO_PLATFORM_INFO_TYPE PlatformInfo;
  UINT32 i, nVal, nKeys = 0;

  TEST_START(PLATFORMINFO_TEST_NAME);

  eResult = gBS->LocateProtocol(&gEfiPlatformInfoProtocolGuid, NULL,
    (VOID **) &hPlatformInfoProtocol);
  if (eResult != EFI_SUCCESS)
  {
    AsciiPrint("Error: Failed to locate PlatformInfo protocol.\n");
    goto endtest;
  }

  eResult = hPlatformInfoProtocol->GetPlatformInfo(hPlatformInfoProtocol,
    &PlatformInfo);
  if (eResult != EFI_SUCCESS)
  {
    AsciiPrint("Error: GetPlatformInfo failed.\n");
    goto endtest;
  }

  if (PlatformInfo.platform >= EFI_PLATFORMINFO_NUM_TYPES)
  {
    AsciiPrint("Error: Unknown platform type (%d).\n", PlatformInfo.platform);
    eResult = EFI_PROTOCOL_ERROR;
    goto endtest;
  }

  AsciiPrint ("Platform: %a\n", aPlatformTypeStrings[PlatformInfo.platform]);
  AsciiPrint ("Subtype:  %d\n", PlatformInfo.subtype);
  AsciiPrint ("Version:  %d.%d\n", PlatformInfo.version >> 16,
    PlatformInfo.version & 0xFFFF);
  AsciiPrint ("Keys:\n");
  for (i = 0; i < EFI_PLATFORMINFO_NUM_KEYS; i++)
  {
    eResult = hPlatformInfoProtocol->GetKeyValue(hPlatformInfoProtocol,
      (EFI_PLATFORMINFO_KEY_TYPE)i, &nVal);
    if (eResult == EFI_SUCCESS)
    {
      nKeys++;
      AsciiPrint("  %a: %d\n", aPlatformKeyStrings[i], nVal);
    }
    else if (eResult != EFI_NOT_FOUND)
    {
      AsciiPrint("Error: GetKeyValue failed.\n");
      goto endtest;
    }
  }

  if (nKeys == 0)
  {
    AsciiPrint("  None.\n");
  }

  eResult = EFI_SUCCESS;

endtest:
  TestStatus(PLATFORMINFO_TEST_NAME, eResult);
  TEST_STOP(PLATFORMINFO_TEST_NAME);
  return eResult;
}

