/** @file AppCmds.c

  Module to implement app commands as helper functions to CmdApp

  Copyright 2012 - 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.


**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/08/17   kpa     Added support for MountFv tool
 09/04/17   gk      UEFI USB compliance test changes
 07/28/17   pm      Added support to set UsbHLOSPortRole value
 06/22/16   ck      Remove the option to use secondary port for the debugger
 03/22/16   bh      Adding BootOrder creation for 64-bit BDS testing
 11/18/15   va      UEFI Factoy ship mode support
 10/08/15   bh      Change default delay to 30s for desktop
 08/13/15   al      Adding Charging related
 06/25/15   bh      Change guid name
 05/28/15   rli     Add toggle for UEFI File Logging
 10/02/13   ck      Add SetBootUsbFirstDelayValue
 08/29/13   ml      Added support for SSUsbfn
 06/20/13   yg      Small improvement to range config
 05/13/13   niting  Add options to support memory dump
 05/12/13   niting  Change HotKey Menu option
 05/09/13   yg      Add low power tests
 05/02/13   sb      Add ToggleUseUsbFnPrimPort
 04/03/13   sm      Editted DisableChgApp API definition
 04/03/13   yg      Added LowPowerMode command
 03/20/13   nk      Added XPU Error Fatal
 02/20/13   niting  Added flag to toggle volatile boot options
 02/15/13   niting  Added ability to keep display ON during charging
 02/15/13   dy      Add ToggleDisplay
 02/14/13   mliss   Add ToggleUseSynopsys
 02/07/13   dy      Add ConfigRsense option
 12/10/12   yg      Handle -ve use cases better
 11/13/12   dy      Add Disable QC Charger App option
 09/17/12   yg      Add Disable Hotkey and some cleanup
 08/22/12   yg      Add Boot USBFirst
 08/22/12   vk      Add ToggleEnableShell
 07/27/12   yg      Convert Disable to toggle function
 07/24/12   yg      Add DisableWriteProtect function
 07/18/12   yg      Disable reset, cleanup of messages
 07/06/12   vishalo Initial Revision
=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/QcomLib.h>
#include <Library/OfflineCrashDump.h>
#include "AppCmds.h"
#include <Protocol/EFIDisplayPwrCtrl.h>
#include <Protocol/EFIClock.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/DevicePathLib.h>
#include <Library/MountFv.h>

extern EFI_GUID gQcomTokenSpaceGuid;
extern EFI_GUID gStickyGuid;

#define INVERTED_MEANING         TRUE
#define NON_INVERTED_MEANING     FALSE

EFI_STATUS
QueryToggleFlag (CONST CHAR8* FeatureString, UINT8* Flagp, BOOLEAN Inverted)
{
  EFI_STATUS    Status;
  EFI_INPUT_KEY    Key;
  UINT8         FeatureFlag;

  if (Flagp == NULL)
    return EFI_INVALID_PARAMETER;

  FeatureFlag = *Flagp;
  if (Inverted)
  {
    if (*Flagp == 0)
      FeatureFlag = 1;
    else
      FeatureFlag = 0;
  }

  if (FeatureFlag == 0)
    AsciiPrint("Feature \"%a\" : Disabled\r\n", FeatureString);
  else
    AsciiPrint("Feature \"%a\" : Enabled\r\n", FeatureString);

  AsciiPrint("\r\nPress [UP] key to Enable, [DOWN] key to Disable, [Other] key NO change\r\n");

  Status = ReadAnyKey (&Key, 0);
  if (EFI_ERROR(Status))
    return EFI_DEVICE_ERROR;

  if (Key.ScanCode == SCAN_UP)
  {
    AsciiPrint("\r\nEnabling \"%a\"\r\n\r\n", FeatureString);
    *Flagp = 1;
  }
  else if (Key.ScanCode == SCAN_DOWN)
  {
    AsciiPrint("\r\nDisabling \"%a\"\r\n\r\n", FeatureString);
    *Flagp = 0;
  }
  else
  {
    AsciiPrint("\r\n\nNot confirmed, exiting.\r\n");
    return EFI_NO_MAPPING;
  }

  if (Inverted)
  {
    if (*Flagp == 0)
      *Flagp = 1;
    else
      *Flagp = 0;
  }
  return EFI_SUCCESS;
}

EFI_STATUS
QuerySetFlag (CONST CHAR8* FeatureString, CONST CHAR8* ValueMapStr, INT8* Flagp, UINT8 MinVal, UINT8 MaxVal)
{
  EFI_STATUS    Status;
  EFI_INPUT_KEY    Key;
  INT8   PrevVal;

  if (Flagp == NULL)
    return EFI_INVALID_PARAMETER;

  PrevVal = *Flagp;

  AsciiPrint("\r\nFeature \"%a\" = %d\r\n\n", FeatureString, *Flagp);

  AsciiPrint("Value Map :\r\n%a\r\n", ValueMapStr);

  AsciiPrint("\r\nPress [UP] key to increment, [DOWN] key to decrement, [Other] key Done\r\n\n");

  AsciiPrint("\rNew Value: %d ", *Flagp);

  while (1)
  {
    Status = ReadAnyKey (&Key, 0);
    if (EFI_ERROR(Status))
      return EFI_DEVICE_ERROR;

    if (Key.ScanCode == SCAN_UP)
    {
      if (*Flagp < MaxVal)
      {
        *Flagp += 1;
        AsciiPrint("\rNew Value: %d ", *Flagp);

      }
    }
    else if (Key.ScanCode == SCAN_DOWN)
    {
      if (*Flagp > MinVal)
      {
        *Flagp -= 1;
        AsciiPrint("\rNew Value: %d ", *Flagp);
      }
    }
    else
    {
      AsciiPrint("\r\n");
      if (*Flagp == PrevVal)
      {
        AsciiPrint("No change, exiting\r\n");
        return EFI_NO_MAPPING;
      }
      break;
    }
  }

  return EFI_SUCCESS;
}


EFI_STATUS
DispInfo(CHAR8* Arg)
{
  EFI_STATUS    Status;
  UINT8         Flag = 0;
  UINTN         VarSize = 0;

  VarSize = sizeof(Flag);

  Status = gRT->GetVariable (L"DispDebugInfo", &gQcomTokenSpaceGuid,
                             NULL, &VarSize, &Flag);

  if (Status == EFI_NOT_FOUND)
    Flag = 0;

  if (QueryToggleFlag ("Display Info on Screen", &Flag, NON_INVERTED_MEANING) != EFI_SUCCESS)
    return EFI_SUCCESS;

  VarSize = sizeof(Flag);

  Status = gRT->SetVariable (L"DispDebugInfo", &gQcomTokenSpaceGuid,
                             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                             VarSize, &Flag);

  if(Status == EFI_SUCCESS)
    AsciiPrint("Reboot device gracefully for setting to take effect.\r\n");
  else
    AsciiPrint("ERROR: Unable to store setting\r\n");

  return Status;
}

EFI_STATUS
ToggleWriteProtection (CHAR8* Arg)
{
  EFI_STATUS    Status;
  UINT8         Flag = 0;
  UINTN         VarSize;

  VarSize = sizeof(Flag);
  Status = gRT->GetVariable (L"DisableWriteProtect", &gQcomTokenSpaceGuid,
                             NULL, &VarSize, &Flag);

  if(Status != EFI_SUCCESS)
    Flag = 0;

  if (QueryToggleFlag ("eMMC Write Protection", &Flag, INVERTED_MEANING) != EFI_SUCCESS)
    return EFI_SUCCESS;

  VarSize = sizeof(Flag);
  Status = gRT->SetVariable (L"DisableWriteProtect", &gQcomTokenSpaceGuid,
                             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                             VarSize, &Flag);

  if(Status == EFI_SUCCESS)
    AsciiPrint("Reboot/PowerCycle device gracefully for setting to take effect.\r\n");
  else
    AsciiPrint("ERROR: Unable to change/store setting\r\n");

  return Status;
}

EFI_STATUS
ToggleEnableShell (CHAR8* Arg)
{
  EFI_STATUS    Status;
  UINT8         Flag = 0;
  UINTN         VarSize;

  VarSize = sizeof(Flag);
  Status = gRT->GetVariable (L"EnableShell", &gQcomTokenSpaceGuid,
                             NULL, &VarSize, &Flag);

  if(Status != EFI_SUCCESS)
    Flag = 0;

  if (QueryToggleFlag ("Run Shell on Boot fail", &Flag, NON_INVERTED_MEANING) != EFI_SUCCESS)
    return EFI_SUCCESS;

  VarSize = sizeof(Flag);
  Status = gRT->SetVariable (L"EnableShell", &gQcomTokenSpaceGuid,
                             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                             VarSize, &Flag);

  if(Status == EFI_SUCCESS)
    AsciiPrint("Reboot/PowerCycle device gracefully for setting to take effect.\r\n");
  else
    AsciiPrint("ERROR: Unable to change/store setting\r\n");

  return Status;
}

EFI_GUID gOwnerGuid = {0x77fa9abd, 0x0359, 0x4d32, {0xbd, 0x60, 0x28, 0xf4, 0xe7, 0x8f, 0x78, 0x4b}};

EFI_STATUS
BootUSBFirst(CHAR8* Arg)
{
  EFI_STATUS    Status;
  UINT8         Flag = 0;
  UINTN         VarSize;

  VarSize = sizeof(Flag);
  Status = gRT->GetVariable (L"AttemptUSBFirst", &gOwnerGuid,
                             NULL, &VarSize, &Flag);

  if(Status != EFI_SUCCESS)
    Flag = 0;

  if (QueryToggleFlag ("Attempt USB First", &Flag, NON_INVERTED_MEANING) != EFI_SUCCESS)
    return EFI_SUCCESS;

  VarSize = sizeof(Flag);
  Status = gRT->SetVariable (L"AttemptUSBFirst", &gOwnerGuid,
                             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                             VarSize, &Flag);

  if(Status == EFI_SUCCESS)
    AsciiPrint("Reboot/PowerCycle device gracefully for setting to take effect.\r\n");
  else
    AsciiPrint("ERROR: Unable to change/store setting\r\n");

  return Status;
}


/*
 * Handler to toggle the BDSHotKeyState variable.
 */

#define  BDS_HOTKEY_STATE_MAP_STRING     " 0 = Detect HotKey"      "\r\n" \
                                         " 1 = Disable HotKey"     "\r\n" \
                                         " 2 = Force HotKey Menu"

EFI_STATUS
ToggleBDSHotKeyState (CHAR8* Arg)
{
  EFI_STATUS    Status;
  INT8          Flag = 0;
  UINTN         VarSize;
  UINT8         MaxVal = MaxBDSHotKeyState - 1;

  VarSize = sizeof(Flag);
  Status = gRT->GetVariable (BDS_HOTKEY_STATE_VARNAME, &gQcomTokenSpaceGuid,
                             NULL, &VarSize, &Flag);

  if(Status != EFI_SUCCESS)
    Flag = 0;

  if (QuerySetFlag ("BDS HotKey State", BDS_HOTKEY_STATE_MAP_STRING, &Flag, 0, MaxVal) != EFI_SUCCESS)
    return EFI_SUCCESS;

  VarSize = sizeof(Flag);
  Status = gRT->SetVariable (BDS_HOTKEY_STATE_VARNAME, &gQcomTokenSpaceGuid,
                             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                             VarSize, &Flag);

  if (Status == EFI_SUCCESS)
    AsciiPrint ("Reboot/PowerCycle device gracefully for setting to take effect.\r\n");
  else
    AsciiPrint ("ERROR: Unable to change/store setting\r\n");

  return Status;
}

#define  BDS_CLOCKLOG_STATE_MAP_STRING   " 0 = Disable Clock logging"   "\r\n" \
                                         " 1 = Uart Clock logging"     "\r\n" \
                                         " 2 = File Clock logging"

EFI_STATUS
ToggeClockLoggingState (CHAR8* Arg)
{
  // check the variable's current value if it exists, and create it if not.
  EFI_STATUS    Status;
  INT8          Flag = 0;
  UINTN         VarSize;
  UINT8         MaxVal = MaxBDSClockLogState - 1;

  VarSize = sizeof(Flag);
  Status = gRT->GetVariable (BDS_CLOCKLOG_STATE_VARNAME, &gQcomTokenSpaceGuid,
             NULL, &VarSize, &Flag);

  if(Status != EFI_SUCCESS)
     Flag = 0;

  if (QuerySetFlag ("BDS Clock Log State", BDS_CLOCKLOG_STATE_MAP_STRING, &Flag, 0, MaxVal) != EFI_SUCCESS)
    return EFI_SUCCESS;

  VarSize = sizeof(Flag);
  Status = gRT->SetVariable (BDS_CLOCKLOG_STATE_VARNAME, &gQcomTokenSpaceGuid,
                             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                             VarSize, &Flag);

  if (Status == EFI_SUCCESS)
    AsciiPrint ("Clock log State setting success.\r\n");
  else
    AsciiPrint ("ERROR: Unable to change/store setting\r\n");

  return Status;
}

/*
 *  Dump Clock status to file or UART
 */
EFI_STATUS
DumpClockLoggingState (CHAR8* Arg)
{
  EFI_STATUS    eStatus;
  INT8          Flag = 0;
  UINTN         VarSize;
  EFI_TEST_CLOCK_PROTOCOL *pTestClockProtocol;

  VarSize = sizeof(Flag);
  eStatus = gRT->GetVariable (BDS_CLOCKLOG_STATE_VARNAME, &gQcomTokenSpaceGuid,
             NULL, &VarSize, &Flag);

  if(eStatus != EFI_SUCCESS)
  {
    AsciiPrint (" BDS Menu: Read Clock log State setting fail.\r\n");
    return eStatus;
  }

  if (Flag == BDSClockLogStateDisable)
  {
    AsciiPrint ("BDS Menu: Clock log State is disable.\r\n");
    return eStatus;
  }

  eStatus = gBS->LocateProtocol(&gEfiTestClockProtocolGuid,NULL,
                        (VOID **)&pTestClockProtocol );

  if ((eStatus != EFI_SUCCESS) ||(pTestClockProtocol == NULL))
  {
    AsciiPrint ("BDS Menu: Test Clock Protocol not available \r\n");
    return eStatus;
  }

  eStatus = pTestClockProtocol->DumpClocksState(pTestClockProtocol, 0);

  if (eStatus == EFI_SUCCESS)
    AsciiPrint ("BDS Menu: Clock log State dumping success.\r\n");
  else
    AsciiPrint ("BDS Menu: Unable to dump Clock log State\r\n");

  return eStatus;
}

/*
 * Launch ChargerTest App
 */

EFI_STATUS
ChargerTest (CHAR8* Arg)
{
  EFI_STATUS    Status = EFI_SUCCESS;

  /* Launch Charger Test App here with default argument as to test Ex Protocol APIs */
  //Status = LaunchAppFromGuidedFv(&gToolsFvGuid, "ChargerTest", NULL);
  if(Status == EFI_SUCCESS)
    AsciiPrint("Charger Test App Launch Successful \r\n");
  else
    AsciiPrint("ERROR: Launching Charger Test App %r \r\n", Status);

  return Status;
}


/*
 * Handler to toggle the UsbHostPrimaryPort variable to modify
 * the configuration of the USB cores.
 */

EFI_STATUS
ToggleEnableUsbHostPrimPort (CHAR8* Arg)
{
  EFI_STATUS    Status;
  UINT8          Flag = 0;
  UINTN         VarSize;

  VarSize = sizeof(Flag);
  Status = gRT->GetVariable (L"UsbHostPrimaryPort", &gQcomTokenSpaceGuid,
                             NULL, &VarSize, &Flag);

  if(Status != EFI_SUCCESS) {
    Flag = 0;
  }

  if (QueryToggleFlag ("USB host mode on primary port", &Flag, NON_INVERTED_MEANING) != EFI_SUCCESS)
    return EFI_SUCCESS;

  VarSize = sizeof(Flag);
  Status = gRT->SetVariable (L"UsbHostPrimaryPort", &gQcomTokenSpaceGuid,
                             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                             VarSize, &Flag);

  if(Status == EFI_SUCCESS)
    AsciiPrint("Reboot/PowerCycle device gracefully for setting to take effect.\r\n");
  else
    AsciiPrint("ERROR: Unable to change/store setting\r\n");

  return Status;
}

/*
*
 * Handler to toggle the SetHsUartBaudRate variable to enable
 * high speed 3Mbps UART.
 */

EFI_STATUS
ToggleEnableHsUartBaudRate (CHAR8* Arg)
{
  EFI_STATUS    Status;
  UINT8          Flag = 0;
  UINTN         VarSize;

  VarSize = sizeof(Flag);
  Status = gRT->GetVariable (L"SetHsUartBaudRate", &gQcomTokenSpaceGuid,
                             NULL, &VarSize, &Flag);

  if(Status != EFI_SUCCESS) {
    Flag = 0;
  }

  if (QueryToggleFlag ("Set 3Mbps UART Baud Rate", &Flag, NON_INVERTED_MEANING) != EFI_SUCCESS)
    return EFI_SUCCESS;

  VarSize = sizeof(Flag);
  Status = gRT->SetVariable (L"SetHsUartBaudRate", &gQcomTokenSpaceGuid,
                             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                             VarSize, &Flag);

  if(Status == EFI_SUCCESS)
    AsciiPrint("Reboot/PowerCycle device gracefully for setting to take effect.\r\n");
  else
    AsciiPrint("ERROR: Unable to change/store setting\r\n");

  return Status;
}

/*
 * Handler for ToggleSSUsbfnForceSS Command
 */
EFI_STATUS
ToggleSSUsbfnForceSS (CHAR8* Arg)
{
  EFI_STATUS    Status;
  UINT8         Flag = 0;
  UINTN         VarSize;

  VarSize = sizeof(Flag);
  Status = gRT->GetVariable (L"UsbfnHijackRequests", &gQcomTokenSpaceGuid,
                             NULL, &VarSize, &Flag);

  if(Status != EFI_SUCCESS) {
    Flag = 0;
  }

  if (QueryToggleFlag ("Force Usbfn SuperSpeed Enumeration", &Flag, NON_INVERTED_MEANING) != EFI_SUCCESS)
    return EFI_SUCCESS;

  VarSize = sizeof(Flag);
  Status = gRT->SetVariable (L"UsbfnHijackRequests", &gQcomTokenSpaceGuid,
                             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                             VarSize, &Flag);

  if(Status == EFI_SUCCESS)
    AsciiPrint("Reboot/PowerCycle device gracefully for setting to take effect.\r\n");
  else
    AsciiPrint("ERROR: Unable to change/store setting\r\n");

  return Status;
}


/*
 * Handler for ToggleForceUsbDeviceModeCompliance Command
 */
 
EFI_STATUS
ToggleForceUsbDeviceModeCompliance (CHAR8* Arg)
{
  EFI_STATUS    Status;
  UINT8         Flag = 0;
  UINTN         VarSize;

  VarSize = sizeof(Flag);
  Status = gRT->GetVariable (L"UsbfnForceComplianceMode", &gQcomTokenSpaceGuid,
                             NULL, &VarSize, &Flag);

  if(Status != EFI_SUCCESS) {
    Flag = 0;
  }

  if (QueryToggleFlag ("Force Usbfn Compliance Mode", &Flag, NON_INVERTED_MEANING) != EFI_SUCCESS)
    return EFI_SUCCESS;

  VarSize = sizeof(Flag);
  Status = gRT->SetVariable (L"UsbfnForceComplianceMode", &gQcomTokenSpaceGuid,
                             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                             VarSize, &Flag);

  if(Status == EFI_SUCCESS)
    AsciiPrint("Reboot/PowerCycle device gracefully for setting to take effect.\r\n");
  else
    AsciiPrint("ERROR: Unable to change/store setting\r\n");

  return Status;
}


/*
 * Handler to set the delay time used when BootUsbFirst option
 * is enabled.
 */
EFI_STATUS
SetBootUsbFirstDelayValue(CHAR8 *Arg)
{
  EFI_STATUS    Status          = EFI_SUCCESS;
  INT8          Flag            = 0;
  UINTN         VarSize         = sizeof(Flag);
  CONST UINT8   DefaultDelayVal = 30;
  CONST UINT8   MinVal          = 0;
  CONST UINT8   MaxVal          = 50;
  CONST CHAR8   *FeatureString  = "Set BootUsbFirstDelay Value";
  CONST CHAR8   *ValueMapStr    = "X second delay";

  // Query BootUsbFirstDelayValue variable
  Status = gRT->GetVariable(L"BootUsbFirstDelayValue",
                            &gQcomTokenSpaceGuid,
                            NULL,
                            &VarSize,
                            &Flag);

  // if the variable does not exit, use the default value
  if (Status != EFI_SUCCESS)
  {
    Flag = DefaultDelayVal;
  }

  // allow the user to set the new delay value
  Status = QuerySetFlag(FeatureString,
                        ValueMapStr,
                        &Flag,
                        MinVal,
                        MaxVal);

  if (Status != EFI_SUCCESS)
  {
    return EFI_SUCCESS;
  }

  // store the change for the delay value
  VarSize = sizeof(Flag);
  Status = gRT->SetVariable(L"BootUsbFirstDelayValue",
                            &gQcomTokenSpaceGuid,
                            (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE),
                            VarSize,
                            &Flag);

  if (Status == EFI_SUCCESS)
  {
    AsciiPrint("Reboot/PowerCycle device gracefully for setting to take effect.\r\n");
  }
  else
  {
    AsciiPrint("ERROR: Unable to change/store setting\r\n");
  }

  return Status;
}

/*
 * Handler to set USB HLOS PORT ROLE VALUE
 */
#define  USB_HLOS_PORT_ROLE_STRING     " 1 = Host Mode"      "\r\n" \
                                       " 2 = Peripheral mode"     "\r\n" \
                                       " 3 = Auto (mode based on type of cable)"

EFI_STATUS
SetUsbHLOSPortRole (CHAR8* Arg)
{
  EFI_STATUS    Status;
  INT8          Flag = 0;
  UINTN         VarSize;
  CONST UINT8   DefaultVal = 3;
  CONST UINT8   MinVal     = 1;
  CONST UINT8   MaxVal     = 3;

  VarSize = sizeof(Flag);
  Status = gRT->GetVariable (L"UsbHLOSPortRole", &gQcomTokenSpaceGuid,
                             NULL, &VarSize, &Flag);
	
  // if the variable does not exit, use the default value
  if (Status != EFI_SUCCESS)
  {
    Flag = DefaultVal;
  }

  if (QuerySetFlag ("Set USB HLOS Port Role", USB_HLOS_PORT_ROLE_STRING, &Flag, MinVal, MaxVal) != EFI_SUCCESS)
    return EFI_SUCCESS;

  VarSize = sizeof(Flag);
  Status = gRT->SetVariable (L"UsbHLOSPortRole", &gQcomTokenSpaceGuid,
                             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                             VarSize, &Flag);

  if (Status == EFI_SUCCESS)
    AsciiPrint ("Reboot/PowerCycle device gracefully for setting to take effect.\r\n");
  else
    AsciiPrint ("ERROR: Unable to change/store setting\r\n");

  return Status;
}

/*
 * Handler to Toggle Display During Charging
 */
EFI_STATUS
ToggleDisplayDuringCharging (CHAR8* Arg)
{
  EFI_STATUS    Status;
  UINT8         Flag = 0;
  UINTN         VarSize;

  VarSize = sizeof(Flag);
  Status = gRT->GetVariable (L"DispCharging", &gQcomTokenSpaceGuid,
                             NULL, &VarSize, &Flag);

  if(Status != EFI_SUCCESS)
    Flag = 0;

  if (QueryToggleFlag ("Toggle Display During Charging", &Flag, INVERTED_MEANING) != EFI_SUCCESS)
    return EFI_SUCCESS;

  VarSize = sizeof(Flag);
  Status = gRT->SetVariable (L"DispCharging", &gQcomTokenSpaceGuid,
                             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                             VarSize, &Flag);

  if(Status == EFI_SUCCESS)
    AsciiPrint("Reboot/PowerCycle device gracefully for setting to take effect.\r\n");
  else
    AsciiPrint("ERROR: Unable to change/store setting\r\n");

  return Status;
}

/*
 * Handler to Toggle Enable Volatile Boot Options
 */
EFI_STATUS
ToggleEnableVolatileBootOptions (CHAR8* Arg)
{
  EFI_STATUS    Status;
  UINT8         Flag = 0;
  UINTN         VarSize;

  VarSize = sizeof(Flag);
  Status = gRT->GetVariable (L"EnableVolatileBootOptions", &gQcomTokenSpaceGuid,
                             NULL, &VarSize, &Flag);

  if(Status != EFI_SUCCESS)
    Flag = 0;

  if (QueryToggleFlag ("Toggle Enable Volatile Boot Options", &Flag, FALSE) != EFI_SUCCESS)
    return EFI_SUCCESS;

  VarSize = sizeof(Flag);
  Status = gRT->SetVariable (L"EnableVolatileBootOptions", &gQcomTokenSpaceGuid,
                             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                             VarSize, &Flag);

  if(Status == EFI_SUCCESS)
    AsciiPrint("Reboot/PowerCycle device gracefully for setting to take effect.\r\n");
  else
    AsciiPrint("ERROR: Unable to change/store setting\r\n");

  return Status;
}

/*
 * Handler to Toggle EnableLegacySBLDump
 */
EFI_STATUS
ToggleEnableLegacySBLDump (CHAR8* Arg)
{
  EFI_STATUS    Status;
  UINT8         Flag = 0;
  UINTN         VarSize;

  VarSize = sizeof(Flag);
  Status = gRT->GetVariable (ENABLE_LEGACY_SBL_DUMP_VARNAME, &gQcomTokenSpaceGuid,
                             NULL, &VarSize, &Flag);

  if(Status != EFI_SUCCESS)
    Flag = 0;

  if (QueryToggleFlag ("Toggle EnableLegacySBLDump", &Flag, FALSE) != EFI_SUCCESS)
    return EFI_SUCCESS;

  VarSize = sizeof(Flag);
  Status = gRT->SetVariable (ENABLE_LEGACY_SBL_DUMP_VARNAME, &gQcomTokenSpaceGuid,
                             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                             VarSize, &Flag);

  if(Status == EFI_SUCCESS)
    AsciiPrint("Reboot/PowerCycle device gracefully for setting to take effect.\r\n");
  else
    AsciiPrint("ERROR: Unable to change/store setting\r\n");

  return Status;
}

/*
 * Handler to Toggle OfflineMemoryDumpUseCapability
 */
EFI_STATUS
ToggleOfflineMemoryDumpUseCapability (CHAR8* Arg)
{
  EFI_STATUS    Status;
  UINT8         Flag = 0;
  UINTN         VarSize;
  UINT8			FlagNew = 0;

  VarSize = sizeof(Flag);
  Status = gRT->GetVariable (OFFLINE_MEMORY_DUMP_USE_CAPABILITY_VARNAME, &gOSAVendorGuid,
                             NULL, &VarSize, &Flag);

  if(Status != EFI_SUCCESS)
    Flag = 0;

  FlagNew = Flag&0x1;

  if (QueryToggleFlag ("Toggle OfflineMemoryDumpUseCapability", &FlagNew, FALSE) != EFI_SUCCESS)
    return EFI_SUCCESS;

  Flag = (Flag&0xFE)|FlagNew;

  VarSize = sizeof(Flag);
  Status = gRT->SetVariable (OFFLINE_MEMORY_DUMP_USE_CAPABILITY_VARNAME, &gOSAVendorGuid,
                             OFFLINE_MEMORY_DUMP_USE_CAPABILITY_ATTR,
                             VarSize, &Flag);

  if(Status == EFI_SUCCESS)
    AsciiPrint("Reboot/PowerCycle device gracefully for setting to take effect.\r\n");
  else
    AsciiPrint("ERROR: Unable to change/store setting\r\n");

  return Status;
}

EFI_STATUS
XpuErrFatal(CHAR8* Arg)
{
  EFI_STATUS    Status;
  UINT8         Flag = 0;
  UINTN         VarSize;

  VarSize = sizeof(Flag);
  Status = gRT->GetVariable (L"XpuErrFatalEnable", &gQcomTokenSpaceGuid,
                             NULL, &VarSize, &Flag);

  if(Status != EFI_SUCCESS)
    Flag = 0;

  if (QueryToggleFlag ("XPU Error Fatal", &Flag, NON_INVERTED_MEANING) != EFI_SUCCESS)
    return EFI_SUCCESS;

  Status = gRT->SetVariable (L"XpuErrFatalEnable", &gQcomTokenSpaceGuid,
                             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                             VarSize, &Flag);

  if(Status == EFI_SUCCESS)
    AsciiPrint("Reboot/PowerCycle device gracefully for setting to take effect.\r\n");
  else
    AsciiPrint("ERROR: Unable to change/store setting\r\n");

  return Status;
}

VOID
TurnOnDisplay (VOID)
{
  EFI_STATUS  Status;
  EFI_DISPLAY_TYPE eDisplayType = EFI_DISPLAY_TYPE_PRIMARY;
  EFI_DISPLAY_POWER_CTRL_STATE ePowerState = EFI_DISPLAY_POWER_STATE_ON;
  EFI_QCOM_DISPLAY_PWR_CTRL_PROTOCOL *gDisplayPwrCtrlProtocol = NULL;

  Status = gBS->LocateProtocol(&gQcomDisplayPwrCtrlProtocolGuid,
                               NULL,
                               (VOID**) &gDisplayPwrCtrlProtocol);

  if ((Status != EFI_SUCCESS) || (gDisplayPwrCtrlProtocol == NULL))
  {
    DEBUG ((EFI_D_WARN, "Display Power Control protocol not available\r\n"));
    return;
  }

  Status = gDisplayPwrCtrlProtocol->DisplayPanelPowerControl(eDisplayType, ePowerState);

  if (Status == EFI_SUCCESS)
    AsciiPrint("Successfully turned ON display\r\n");
  else
    AsciiPrint("Display turn ON Failed\r\n");
}

VOID
TurnOffDisplay (VOID)
{
  EFI_STATUS Status;
  EFI_DISPLAY_TYPE eDisplayType = EFI_DISPLAY_TYPE_PRIMARY;
  EFI_DISPLAY_POWER_CTRL_STATE ePowerState = EFI_DISPLAY_POWER_STATE_OFF;
  EFI_QCOM_DISPLAY_PWR_CTRL_PROTOCOL *gDisplayPwrCtrlProtocol = NULL;

  Status = gBS->LocateProtocol(&gQcomDisplayPwrCtrlProtocolGuid,
                               NULL,
                               (VOID**) &gDisplayPwrCtrlProtocol);

  if ((Status != EFI_SUCCESS) || (gDisplayPwrCtrlProtocol == NULL))
  {
    AsciiPrint("Display Power Control protocol not available\r\n");
    return;
  }

  Status = gDisplayPwrCtrlProtocol->DisplayPanelPowerControl(eDisplayType, ePowerState);

  if (Status == EFI_SUCCESS)
    AsciiPrint("Successfully turned OFF display\r\n");
  else
    AsciiPrint("Display turne OFF Failed\r\n");
}

VOID
LowClock (VOID)
{
  EFI_STATUS    Status = EFI_SUCCESS;
  EFI_CLOCK_PROTOCOL  *mClockProtocol;

  Status = gBS->LocateProtocol (&gEfiClockProtocolGuid, NULL, (VOID **)&mClockProtocol);
  if ((Status != EFI_SUCCESS) || (mClockProtocol == NULL))
  {
    AsciiPrint("Clock protocol not available\r\n");
    return;
  }
  Status = mClockProtocol->EnterLowPowerMode(mClockProtocol);

  if (Status == EFI_SUCCESS)
    AsciiPrint("Switched to low clock.\r\n");
  else
    AsciiPrint("Switch to low clock FAILED.\r\n");
}

VOID
HighClock (VOID)
{
  EFI_STATUS    Status = EFI_SUCCESS;
  EFI_CLOCK_PROTOCOL  *mClockProtocol;

  Status = gBS->LocateProtocol (&gEfiClockProtocolGuid, NULL, (VOID **)&mClockProtocol);
  if ((Status != EFI_SUCCESS) || (mClockProtocol == NULL))
  {
    AsciiPrint("Clock protocol not available\r\n");
    return;
  }

  Status = mClockProtocol->ExitLowPowerMode(mClockProtocol);

  if (Status == EFI_SUCCESS)
    AsciiPrint("Switched to normal clock.\r\n");
  else
    AsciiPrint("Switch to normal clock FAILED.\r\n");
}

/*
 *  Low Power Mode
 */
EFI_STATUS
LowPowerMode (CHAR8* Arg)
{
  EFI_STATUS    Status = EFI_SUCCESS;
  UINTN Disp, Clk, Opr;

  ASSERT_EFI_ERROR(Status);

  /* LowPowerMode xxxx
   * Where x can be one of the following (combine these logically)
   * D : Display
   * C : Clock
   * H : Restore all to normal power mode
   * R : Same as H
   * L : Low power mode (default if not specified, if no additional option should result into max savings)
   *
   * Example : LD => turn off display
   *           HD => Turn ON display
   *           LCK => Display off + Low clock (currently this is same as no LCK specified)
   *
   * */
#define  OPERATION_NONE        0
#define  OPERATION_LOW_PWR     1
#define  OPERATION_RESTORE     2
#define  OPERATION_LOWEST_PWR  3
#define  OPERATION_NORMAL_PWR  4

  Disp = 0;
  Clk = 0;
  Opr = OPERATION_NONE;

  if (Arg == NULL)
  {
    Opr = OPERATION_LOWEST_PWR;
  }
  else
  {
    CHAR8* ch = Arg;
    UINTN DevCnt = 0;

    while (ch && *ch)
    {
      switch (*ch)
      {
        case 'c': case 'C':
          Clk = 1;
          DevCnt++;
          break;

        case 'd': case 'D':
          Disp = 1;
          DevCnt++;
          break;

        case 'r': case 'R':
        case 'h': case 'H':
          Opr = OPERATION_RESTORE;
          break;

        case 'l': case 'L':
          Opr = OPERATION_LOW_PWR;
          break;

        default:
          ch = NULL;
      }
      if (ch)
        ++ch;
    }

    if (DevCnt == 0)
    {
      if (Opr == OPERATION_RESTORE)
        Opr = OPERATION_NORMAL_PWR;
      else if (Opr == OPERATION_LOW_PWR)
        Opr = OPERATION_LOWEST_PWR;
    }
  }

  switch (Opr)
  {
    case OPERATION_LOWEST_PWR:
      LowClock ();
      TurnOffDisplay ();
      break;

    case OPERATION_NORMAL_PWR:
      TurnOnDisplay ();
      HighClock ();
      break;

    case OPERATION_LOW_PWR:
      if (Clk)
        LowClock ();
      if (Disp)
        TurnOffDisplay ();
      break;

    case OPERATION_RESTORE:
      if (Disp)
        TurnOnDisplay ();
      if (Clk)
        HighClock ();
      break;
  }

  return Status;
}

EFI_STATUS
PrintChargerAppDbgMsg (CHAR8* Arg)
{
  EFI_STATUS    Status;
  UINT8         Flag = 0;
  UINTN         VarSize;

  VarSize = sizeof(Flag);
  Status = gRT->GetVariable (L"PrintChargerAppDbgMsg", &gQcomTokenSpaceGuid,
                             NULL, &VarSize, &Flag);

  if(Status != EFI_SUCCESS)
    Flag = 0;

  if (QueryToggleFlag ("Print Charger App Debug messages", &Flag, NON_INVERTED_MEANING) != EFI_SUCCESS)
    return EFI_SUCCESS;

  VarSize = sizeof(Flag);
  Status = gRT->SetVariable (L"PrintChargerAppDbgMsg", &gQcomTokenSpaceGuid,
                             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                             VarSize, &Flag);

  if(Status == EFI_SUCCESS)
    AsciiPrint("Successfully stored setting\r\n");
  else
    AsciiPrint("ERROR: Unable to change/store setting\r\n");

  return Status;

}


EFI_STATUS
EnShipMode (CHAR8* Arg)
{
  // check the variable's current value if it exists, and create it if not.
  EFI_STATUS Status;
  UINT8 Flag = 0;
  UINTN VarSize = 0;

  VarSize = sizeof(Flag);

  Status = gRT->GetVariable (L"ShipMode", &gQcomTokenSpaceGuid,
             NULL, &VarSize, &Flag);

  if (Status == EFI_NOT_FOUND)
    Flag = 0;

  if (QueryToggleFlag ("Toggle PMIC Ship Mode", &Flag, NON_INVERTED_MEANING) != EFI_SUCCESS)
  return EFI_SUCCESS;

  VarSize = sizeof(Flag);

  Status = gRT->SetVariable (L"ShipMode", &gQcomTokenSpaceGuid,
               EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
             VarSize, &Flag);

  if(Status == EFI_SUCCESS){
    AsciiPrint("PMIC Ship Mode "); //.\r\n");
    if(Flag == 0){
      AsciiPrint("disabled. \r\n");
    } else {
      AsciiPrint("enabled. \r\n");
    }
  }
  else{
  AsciiPrint("ERROR: Unable to store setting\r\n");
  }
  return Status;
}



EFI_STATUS
FileLoggingToggle (CHAR8* Arg)
{
  // check the variable's current value if it exists, and create it if not.
  EFI_STATUS Status;
  UINT8 Flag = 0;
  UINTN VarSize = 0;

  VarSize = sizeof(Flag);

  Status = gRT->GetVariable (L"EnableFileLogging", &gQcomTokenSpaceGuid,
             NULL, &VarSize, &Flag);

  if (Status == EFI_NOT_FOUND)
    Flag = 0;

  if (QueryToggleFlag ("Toggle UEFI File Logging", &Flag, NON_INVERTED_MEANING) != EFI_SUCCESS)
  return EFI_SUCCESS;

  VarSize = sizeof(Flag);

  Status = gRT->SetVariable (L"EnableFileLogging", &gQcomTokenSpaceGuid,
               EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
             VarSize, &Flag);

  if(Status == EFI_SUCCESS){
    AsciiPrint("File Logging has been "); //.\r\n");
    if(Flag == 0){
      AsciiPrint("disabled. \r\n");
    } else {
      AsciiPrint("enabled. \r\n");
    }
  }
  else{
  AsciiPrint("ERROR: Unable to store setting\r\n");
  }
  return Status;
}



BOOLEAN
CheckValid(EFI_DEVICE_PATH_PROTOCOL *DevPath)
{
  EFI_DEVICE_PATH_PROTOCOL  *TempDevicePath;
  EFI_BLOCK_IO_PROTOCOL     *BlockIo;
  EFI_HANDLE                Handle;
  EFI_STATUS Status;

  TempDevicePath = DevPath;
  Status = gBS->LocateDevicePath (&gEfiBlockIoProtocolGuid, &TempDevicePath, &Handle);

  if (!EFI_ERROR (Status)) {
    Status = gBS->HandleProtocol (Handle, &gEfiBlockIoProtocolGuid, (VOID **)&BlockIo);
    if (!EFI_ERROR (Status)) {
      return TRUE;
    }
  } else {
    Status = gBS->LocateDevicePath (&gEfiSimpleFileSystemProtocolGuid, &TempDevicePath, &Handle);
    if (!EFI_ERROR (Status)) {
      return TRUE;
    }
  }

  return FALSE;
}




VOID
Add64BitBootOrder(CHAR8* Arg)
{
  EFI_STATUS                 Status;
  UINTN                      HandleCount;
  EFI_HANDLE                 *HandleBuffer;
  UINTN                      Index;
  EFI_DEVICE_PATH_PROTOCOL   *DevicePath;
  EFI_BLOCK_IO_PROTOCOL      *BlkIo;
  UINTN AddFlag = 0;

  DEBUG((EFI_D_ERROR, "Adding all non-removable options to BootOrder\n"));


  Status = gBS->LocateHandleBuffer (
                      ByProtocol,
                      &gEfiSimpleFileSystemProtocolGuid,
                      NULL,
                      &HandleCount,
                      &HandleBuffer);

  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "Could not locate by FS protocol\n"));
    return;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                      HandleBuffer[Index],
                      &gEfiBlockIoProtocolGuid,
                      (VOID **) &BlkIo
                      );

    if (BlkIo == NULL)
      continue;

    if (BlkIo->Media->RemovableMedia)
      continue;

    Status = gBS->HandleProtocol (
                      HandleBuffer[Index],
                      &gEfiDevicePathProtocolGuid,
                      (VOID **) &DevicePath
                      );

    if (DevicePath == NULL) {
      DEBUG((EFI_D_ERROR, "Could not get device path\n"));
      continue;
    }
    if (CheckValid(DevicePath))
    {
      AddFlag = ADD_FLAG_ADD_FIRST | ADD_FLAG_NO_BOOT_NEXT;

      Status = AddDevPathToBootOrder(NULL,
                                     DevicePath,
                                     L"Testing BootOrder",
                                     NULL,
                                     0,
                                     AddFlag);
      if (EFI_ERROR(Status))
      {
        DEBUG((EFI_D_ERROR, "Error adding option to BootOrder\n"));
        continue;
      }

      DEBUG((EFI_D_ERROR, "Option found, adding to BootOrder\n"));
    }
  }
}

/*
 * Handler to command for mounting a FV
 */
EFI_STATUS
MountFvCommand (CHAR8* Arg)
{
  EFI_STATUS    Status = EFI_SUCCESS;

  if (Arg == NULL)
  {
    AsciiPrint(" Error: MountFv requires Fv partition name eg: \"MountFv toolsfv\".\r\n");
  }
  else
  {
    Status = MountFvPartition(Arg);
  }
  return Status;
}
