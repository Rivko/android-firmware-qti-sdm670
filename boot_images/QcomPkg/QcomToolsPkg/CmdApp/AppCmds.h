#ifndef __APPCMDS_H__
#define __APPCMDS_H__

/*=============================================================================

                     App commands interface Header File

  GENERAL DESCRIPTION
    Implementation of various App Commands for CmdApp

  Copyright 2012 - 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  09/08/17   kpa     Added support for MountFv tool
  09/04/17   gk      UEFI USB compliance test changes
  07/28/17   pm      Added support to set UsbHLOSPortRole value
  06/22/16   ck      Remove the option to use secondary port for the debugger
  03/22/16   bh      Adding BootOrder creation for 64-bit BDS testing
  11/18/15   va      UEFI Factoy ship mode support
  08/13/15   al      Adding Charging related
  05/28/15   rli     Add toggle for UEFI File Logging
  10/02/13   ck      Add SetBootUsbFirstDelayValue
  08/29/13   ml      Added support for SSUsbfn
  05/13/13   niting  Add options to support memory dump
  05/12/13   niting  Change HotKey Menu option
  05/02/13   sb      Add ToggleUseUsbFnPrimPort
  04/03/13   sm      Editted description for DisableChgApp
  04/03/13   yg      Added Low power command
  03/20/13   nk      Added XPU Error Fatal
  02/20/13   niting  Added flag to toggle volatile boot options
  02/15/13   niting  Added ability to keep display ON during charging
  02/15/13   dy      Add ToggleDisplay
  02/14/13   mliss   Add ToggleUseSynopsys
  02/07/13   dy      Add Toggle Battery Rsense
  11/13/12   dy      Add Disable Chg App
  09/17/12   yg      Add Disable Hotkey
  08/22/12   yg      Add Boot USBFirst
  08/22/12   vk      Add Toggle Enable Shell
  07/27/12   yg      Convert Disable to toggle function
  07/24/12   yg      Add disable write protection function
  07/17/12   vo      Initial creation

============================================================================*/

/*
 * Handler for DispInfo Command
 */
EFI_STATUS
DispInfo(CHAR8* Arg);

/*
 * Handler for ToggleWriteProtect Command
 */
EFI_STATUS
ToggleWriteProtection (CHAR8* Arg);

/*
 * Handler for ToggleEnableShell Command
 */
EFI_STATUS
ToggleEnableShell(CHAR8* Arg);

/*
 * Handler for Boot USBFirst Command
 */
EFI_STATUS
BootUSBFirst(CHAR8* Arg);

/*
 * Handler to toggle the BDSHotKeyState variable.
 */
EFI_STATUS
ToggleBDSHotKeyState (CHAR8* Arg);

/*
 *  Toggle Clock  status File Logging setting
 */
EFI_STATUS
ToggeClockLoggingState (CHAR8* Arg);

/*
 *  Dump Clock status to file or UART
 */

EFI_STATUS
DumpClockLoggingState (CHAR8* Arg);

/*
 * Handler to Toggle charger power-on trigger
 */
EFI_STATUS
DisableChgApp (CHAR8* Arg);

/*
 * Handler to Toggle Battery Rsense
 */
EFI_STATUS
ConfigRsense (CHAR8* Arg);

/*
 * Handler for ToggleEnableUsbHostPrimPort Command
 */
EFI_STATUS
ToggleEnableUsbHostPrimPort (CHAR8* Arg);

/*
 * Handler for ToggleEnableHsUartBaudRate Command
 */
EFI_STATUS
ToggleEnableHsUartBaudRate (CHAR8* Arg);

/*
 * Handler for ToggleSSUsbfnForceSS Command
 */
EFI_STATUS
ToggleSSUsbfnForceSS (CHAR8* Arg);

/*
 * Handler for ToggleForceUsbDeviceModeCompliance Command
 */
EFI_STATUS
ToggleForceUsbDeviceModeCompliance (CHAR8* Arg);

/*
 * Handler for SetBootUsbFirstDelayValue Command
 */
EFI_STATUS
SetBootUsbFirstDelayValue(CHAR8 *Arg);

/*
 * Handler for SetUsbHLOSPortRole Command
 */
EFI_STATUS
SetUsbHLOSPortRole (CHAR8* Arg);

/*
 * Handler to Toggle Display During Charging
 */
EFI_STATUS
ToggleDisplayDuringCharging (CHAR8* Arg);

/*
 * Handler to Toggle Enable Volatile Boot Options
 */
EFI_STATUS
ToggleEnableVolatileBootOptions (CHAR8* Arg);

/*
 * Handler to Toggle EnableLegacySBLDump
 */
EFI_STATUS
ToggleEnableLegacySBLDump (CHAR8* Arg);

/*
 * Handler to Toggle OfflineMemoryDumpUseCapability
 */
EFI_STATUS
ToggleOfflineMemoryDumpUseCapability (CHAR8* Arg);

/*
 *  Enable/Disable XPU Error Fatals
 */
EFI_STATUS
XpuErrFatal (CHAR8* Arg);

/*
 *  Low Power Mode
 */
EFI_STATUS
LowPowerMode (CHAR8* Arg);

/*
 * Handler to Toggle printing debug msgs from Charger App
 */
EFI_STATUS
PrintChargerAppDbgMsg (CHAR8* Arg);


/*
 *  Toggle UEFI File Logging
 */
EFI_STATUS
FileLoggingToggle (CHAR8* Arg);

/*
 *  Toggle Qcom charger app
 */
EFI_STATUS
RunQcomChgrApp (CHAR8* Arg);

/*
 * Handler to Toggle Ship Mode 
 */
EFI_STATUS
EnShipMode (CHAR8* Arg);

VOID
Add64BitBootOrder(CHAR8* Arg);

/*
 * Handler to command to mount a FV
 */
EFI_STATUS
MountFvCommand (CHAR8* Arg);

#endif  /* __APPCMDS_H__ */

