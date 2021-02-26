/** @file
  This file include all platform action which can be customized by IBV/OEM.

  Copyright (c) 2010-2017, Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  Portions copyright (c) 2004 - 2008, Intel Corporation. All rights reserved.

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
 08/17/17   aus     Added support to share SDCC register address 
 05/05/17   sj      Added the 64 bit cycle initialization at BDS stage
 10/31/16   bh      Remove splash screen rendering from BDS
 10/24/16   ai      Send SD card event when entering BDS menu
 08/26/16   vk      Remove unused function
 08/22/16   jt      Remove Sdcc config
 08/11/16   akl     Remove handling of NPA ExitBootServices. Handled in NpaDxe.
 03/30/16   ab      Moving handling of Glink ExitBootServices to GlinkDxe
 08/16/16   vk      Reduce logs in exit boot service for normal boot
 07/05/16   bh      Use partition label for file logging
 06/22/16   bh      Display total UEFI exec LA, move tools FV mounting
 06/22/16   bh      Updated retail checks
 06/08/16   sj      removed the key mapping for massstorage mode
 05/09/16   vk      Try to load EBL from test fv
 03/29/16   bh      Added logic to launch app from Guided Fv
 03/16/16   ml      Added rendering of the splash logo from within the BDS
 03/14/16   yg      Cleanup and remove dead code
 03/04/16   bh      Print UEFI execution time from start to POST
 02/22/16   vk      Load LinuxLoader from ABL
 02/22/16   vk      Add Shared_IMEM_Base variable
 01/27/16   sm      Replaced FgExit() API with FgBasicExit()
 01/11/16   ts      Removed PCIe init from BDS ExitBootServices
 10/15/15   bh      Simplify debug scripts to use info block
 10/14/15   vk      Added NULL check
 09/30/15   bh      Update firmware versioning
 07/07/11   niting  Initial revision.

=============================================================================*/

#include <PiDxe.h>

#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PerformanceLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/EfiFileLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/SerialPortShLib.h>
#include <Library/UefiSigDb.h>
#include <Library/FuseControlLib.h>
#include <Library/GenericBdsLib.h>
#include <Library/SecBootSigLib.h>

#include <Library/QcomBdsLib.h>
#include <Library/QcomLib.h>
#include <Library/QcomUtilsLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/QcomTargetLib.h>
#include <Library/HWConfigLib.h>
#include <Library/ProcAsmLib.h>

#include <Library/UefiCfgLib.h>
#include <Include/UefiInfoBlk.h>

#include <Guid/Gpt.h>
#include <Guid/EventGroup.h>
#include <Guid/GlobalVariable.h>

#include <Library/FwUpdateLib.h>
#include <Library/FwProvisionLib.h>
#include <Library/GuidedFvLib.h>
#include <Library/BootConfig.h>

#include <Protocol/BlockIo.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/LoadFile.h>
#include <Protocol/FirmwareVolumeBlock.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/PciIo.h>
#include <Protocol/DevicePath.h>

#include <Protocol/EFIEncryption.h>
#include <Protocol/EFIVariableServices.h>
#include <Protocol/EFICapsule.h>
#include <Protocol/EFIDisplayPwrCtrl.h>
#include <Protocol/EFIPmicFg.h>
#include <Protocol/EFIToolSupport.h>
#include <Protocol/EFIScm.h>
#include <Protocol/EFIClock.h>
#include <Protocol/EFIHWIO.h>
#include <Library/TimerLib.h>

#include "PlatformBds.h"

extern VOID BdsBootDeviceSelect ( VOID );
extern EFI_STATUS BdsWaitForSingleEvent (IN UINT64 Timeout);

//Partition Guid
extern EFI_GUID gEfiEmmcGppPartition1Guid;
extern EFI_GUID gEfiEmmcUserPartitionGuid;
extern EFI_GUID gEfiEmmcBootPartition1Guid;
extern EFI_GUID gEfiLogFSPartitionGuid;
extern EFI_GUID gEfiUfsLU4Guid;
extern EFI_GUID gToolsFvGuid;
extern EFI_GUID gMainFvGuid;

//Partition Type
extern EFI_GUID gEfiPartTypeSystemPartGuid;
extern EFI_GUID gEfiPlatPartitionTypeGuid;

extern EFI_GUID gEfiSdRemovableGuid;
extern EFI_GUID gEfiACPITableLoadGuid;
extern EFI_GUID gEfiAblFvNameGuid;

//Exit Boot Services guid
extern EFI_GUID gEfiEventExitBootServicesGuid;

//SCMDxe guid
extern EFI_GUID gQcomScmProtocolGuid;

//Charging Event GUID
extern EFI_GUID gEfiEventChargerEnableGuid;
extern EFI_GUID gEfiEventChargerDisableGuid;
extern EFI_GUID gEfiStartTimeHobGuid;
EFI_EVENT EventChargingStarted = NULL;
EFI_EVENT EventChargingEnd = NULL;
EFI_QCOM_DISPLAY_PWR_CTRL_PROTOCOL *gDisplayPwrCtrlProtocol = NULL;

EFI_EVENT ACPITableLoadEvent  = NULL;
EFI_EVENT EfiExitBootServicesEvent = (EFI_EVENT)NULL;
EFI_VARIABLESERVICES_PROTOCOL *VariableServicesProtocol = NULL;
EFI_CAPSULE_PROTOCOL *CapsuleProtocol = NULL;
QCOM_SCM_PROTOCOL *pSCMProtocol = NULL;
EFI_CLOCK_PROTOCOL *pClockProtocol = NULL;

STATIC UINT32 LcdDebugFlag = 0;
STATIC UINT8  EnableShellFlag = 0;
STATIC UINT8  VolatileTables = 0;
STATIC UINT8  EnableVolatileBootOptions = 0;

STATIC UINT32 BootCycleCount = 0;

EFI_STATUS
BdsStartApp (IN UINTN  Argc, IN CHAR8  **Argv);

VOID
EFIAPI
PlatformBdsLoadShellForNonProdMode (IN  BDS_COMMON_OPTION *Option);

EFI_STATUS
EFIAPI
PlatformBdsPreLoadBootOption (IN  BDS_COMMON_OPTION *Option);

EFI_STATUS
EFIAPI
WriteLogBufToPartition (VOID);

STATIC
EFI_STATUS
DisplayBootTime (CHAR8* Key, CHAR8* Event, BOOLEAN OnLCD);

STATIC
VOID
DisplayUefiTotalTime (VOID);

/* Initialize the cycle counter to track performance */
STATIC VOID
StartCyclCounter (VOID);


/* Initialize the cycle counter to track performance */
STATIC VOID
StartCyclCounter (VOID)
{
  UINTN RegVal;
  UINT64 Val;
  UINT64 Scale;
  UINT64 AppsProcClkMhz;

  /* User mode enable to read in non secure mode */
  WriteUserEnReg (1);

  /* Reset counters */
  RegVal = (0x41 << 24) |  /* IMP */
           (4 << 11)    |  /* N */
           (1 << 2);       /* Reset CCNT */
  WritePMonCtlReg (RegVal);

  ReadCycleCntReg();

  /* Scale bootcounter running at 32KHz to CPU frequency in MHz, counting every 64 cycles.
     Get the Scale to be accurate to 3 decimal places by multiplying it with 2^10.
     Then divide by 2^10 (right shift by 10) to nullify the multiplication done before and
     divide by 64 (Right shift 6) => Right shift by 16 to set Cycle Counter Start Value */
  AppsProcClkMhz = GetPerformanceCounterProperties (NULL, NULL);


  Scale = (AppsProcClkMhz << 10)/1000;
  Val =  Scale *  GetTimeInNanoSecond (GetPerformanceCounter ());
  Val = (Val >> 10);
  WriteCycleCntReg(Val);

  /* Check if write went through */
  ReadCycleCntReg();

  /* Enable Cycle counter */
  WriteCntEnSetReg (((UINT32)1 << 31));

  /* Check if we start counting */
  ReadCycleCntReg();

  /* Enable CCNT */
  RegVal = (0x41 << 24) |  /* IMP */
           (4 << 11)    |  /* N */
           (1);            /* Enable all counters */
  WritePMonCtlReg (RegVal);

  /* Disable User mode access */
  WriteUserEnReg (0);

  /* Write to TPIDRURW */
  WriteTPIDRURWReg (0x56430000);

  /* Write to TPIDRURO */
  WriteTPIDRUROReg (0);

  /* Example to Read the counter value, Should read small */
  ReadCycleCntReg();
}


/**
  Displays POST Time.

**/
VOID
EFIAPI
DisplayPOSTTime (VOID)
{
  DisplayUefiTotalTime ();
  DisplayBootTime("POST Time     ", "OS Loader", TRUE);
}

/**
 * Write protection call for the GPP and Boot partitions
 *@param  Event                 Event whose notification
 *                              function is being invoked.
 *
 *@param  Context               The pointer to the notification
 *                              function's context which is
 *                              implementation-dependent.
 */
VOID
EFIAPI
PlatformBDSExitBSCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

#ifndef PRE_SIL
  /* Dump Log Buffer to File */
  Status = WriteLogBufToPartition ();
  if (EFI_ERROR(Status))
  {
     DEBUG ((EFI_D_ERROR, "BDS: LogFs sync skipped, %r\r\n", Status));
  }
#endif

  Status = gBS->LocateProtocol (&gEfiVariableServicesProtocolGuid,
                                NULL,
                                (VOID**) &VariableServicesProtocol);
  if (Status != EFI_SUCCESS)
  {
    DEBUG ((EFI_D_INFO, "BDS: Variable services protocol %r\n", Status));
  }

  Status = gBS->LocateProtocol (&gEfiCapsuleProtocolGuid,
                                NULL,
                                (VOID**) &CapsuleProtocol);
  if (CapsuleProtocol == NULL)
  {
    DEBUG ((EFI_D_INFO, "BDS: Capsule services protocol %r\n", Status));
  }

  Status = gBS->LocateProtocol (&gQcomScmProtocolGuid,
                                NULL,
                                (VOID**) &pSCMProtocol);
  if (pSCMProtocol == NULL)
  {
    DEBUG ((EFI_D_WARN, "Could not locate pSCM protocol\n"));
  }

  /* Flush the NV Storage tables before turning ON write protection
   * Do this only if the protocol available. During initial stages the
   * the protocol might not be installed */
  if (VariableServicesProtocol != NULL)
  {
    UINT32 SyncEndTime, SyncStartTime = GetTimerCountms ();

    Status = VariableServicesProtocol->FlushVariableNV (VariableServicesProtocol);
    if (EFI_ERROR(Status))
    {
      DEBUG ((EFI_D_ERROR, "Failed to flush NV var, Status = 0x%08x\r\n", Status));
    }
    else
    {
      SyncEndTime = GetTimerCountms ();
      DEBUG ((EFI_D_WARN, "Sync Duration = %d ms\r\n", SyncEndTime - SyncStartTime));
    }
  }

  if (CapsuleProtocol != NULL)
  {
    Status = CapsuleProtocol->FlushCapsulePartition (CapsuleProtocol);
    if (Status != EFI_SUCCESS)
    {
      DEBUG ((EFI_D_ERROR, "Failed to flush capsule partition, Status = 0x%08x\r\n", Status));
    }
  }

  /* This is added after the flushing of NV vars as this callback will deregister the Listeners,
   * hence FLushing to RPMB/GPT will fail after this point until HLOS SCM driver (only RPMB is supported) is loaded.
   */
  if (pSCMProtocol != NULL)
  {
    Status = pSCMProtocol->ScmExitBootServicesHandler (pSCMProtocol);
  }

}


VOID
SetOsIndicationsSupported (VOID)
{
  EFI_STATUS Status;
  UINT64     OsIndicationSupport;
  UINTN      DataSize;
  UINT32     Attributes;

  DataSize = sizeof(OsIndicationSupport);
  Status = gRT->GetVariable (
                  L"OsIndicationsSupported",
                  &gEfiGlobalVariableGuid,
                  &Attributes,
                  &DataSize,
                  &OsIndicationSupport
                  );
  if (EFI_ERROR(Status)) {   
    DEBUG ((EFI_D_INFO, "OsIndicationsSupported variable not present \n"));  
    OsIndicationSupport = EFI_OS_INDICATIONS_BOOT_TO_FW_UI;
  }
  else {
    DEBUG ((EFI_D_INFO, "OsIndicationsSupported variable present \n"));  
    // If the variable already exists then OR the new value
    OsIndicationSupport |= EFI_OS_INDICATIONS_BOOT_TO_FW_UI;
  }
  
  Status = gRT->SetVariable (
                  L"OsIndicationsSupported",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  sizeof(OsIndicationSupport),
                  &OsIndicationSupport
                  );
}

BOOLEAN
CheckAndClearBootToFWUIField (VOID)
{
  EFI_STATUS Status;
  UINT64     OsIndication = 0;
  UINTN      DataSize;
  UINT32     Attributes; 
  BOOLEAN    RetVal = FALSE;
  //
  // If OsIndications is invalid, remove it.
  // Invalid case
  //   1. Data size != UINT64  
  //   2. OsIndication attribute inconsistence
  //
  OsIndication = 0;
  Attributes = 0;
  DataSize = sizeof(OsIndication);
  Status = gRT->GetVariable (
                  L"OsIndications",
                  &gEfiGlobalVariableGuid,
                  &Attributes,
                  &DataSize,
                  &OsIndication
                  );

  if (!EFI_ERROR(Status)) {
    if (DataSize != sizeof(OsIndication) ||
        Attributes != (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE)){

      DEBUG ((EFI_D_ERROR, "Unformalized OsIndications variable exists. Delete it\n"));
      Status = gRT->SetVariable (
                      L"OsIndications",
                      &gEfiGlobalVariableGuid,
                      Attributes,
                      0,
                      &OsIndication
                      );
      ASSERT_EFI_ERROR (Status);
      
      RetVal = FALSE;
      }
    else {    
      if(OsIndication & EFI_OS_INDICATIONS_BOOT_TO_FW_UI) {
        DEBUG ((EFI_D_INFO, "OsIndications variable has BOOT_TO_FW_UI bit set\n"));
        RetVal = TRUE;
        
        /* Clear the bit, so that we dont boot into Setup app on next boot */
        OsIndication &= ~EFI_OS_INDICATIONS_BOOT_TO_FW_UI;
        Status = gRT->SetVariable (
                        L"OsIndications",
                        &gEfiGlobalVariableGuid,
                        Attributes,
                        sizeof(OsIndication),
                        &OsIndication
                        );
        ASSERT_EFI_ERROR (Status);      
      }
      else {
        RetVal = FALSE;
        DEBUG ((EFI_D_INFO, "OsIndications variable has BOOT_TO_FW_UI bit cleared\n"));
      }
    }
  }
        
  return RetVal;        
}

/* Send SD card detection event */
VOID
SendEventDetectSdCard(VOID)
{
  EFI_STATUS Status;
  Status = EfiEventGroupSignal(&gEfiEventDetectSdCardGuid);
  if (Status != EFI_SUCCESS)
  {
    DEBUG ((EFI_D_ERROR, "Error: Cannot signal Dected Sd Card Event\n"));
  }
}

VOID
LaunchBDSMenu(VOID)
{
  EFI_STATUS Status = EFI_SUCCESS;
  
  DEBUG ((EFI_D_WARN, "Hotkey detected, entering Menu\n"));
  
  //Disable watchdog
  gBS->SetWatchdogTimer(0, 0, 0, NULL);

  // Signal read to boot event
  EfiSignalEventReadyToBoot ();

  // End Perf marker
  PERF_END (NULL, "BDS", NULL, 0);

  // Load Debug FV image here
#ifndef PRE_SIL
  Status = LoadDebugToolsFv ();
  if (Status != EFI_SUCCESS){
     DEBUG ((EFI_D_WARN, "WARNING: Debug Tools FV failed to load, Status : %r", Status));
  }
#endif

  // Send SD card detection event
  SendEventDetectSdCard();

  BdsConnectAllDrivers ();

  SerialPortDrain ();
  EnableSynchronousSerialPortIO ();

  // Launch Bds Menu from Tools FV
  LaunchMenu(NULL);

  // Drop into EBL Shell
  LaunchAppFromGuidedFv(&gToolsFvGuid, "Ebl", NULL);
  LaunchAppFromGuidedFv(&gMainFvGuid, "Ebl", NULL);
}      
//
// BDS Platform Functions
//

#define DEF_APP_STR_LEN 64

/**
 * Launch default BDS Boot App.
 * If default BDS Boot App is specified then this function should NOT return,
 * on failure to launch or if the launched app returns, launch the shell app
 * if the device is NOT in retail mode as determined by PRODMODE Build and
 * fuse blown status.
*/
STATIC
EFI_STATUS
EFIAPI
LaunchDefaultBDSApps (VOID)
{

  EFI_STATUS Status = EFI_SUCCESS;
  CHAR8 DefaultApp[DEF_APP_STR_LEN];
  UINTN Size = DEF_APP_STR_LEN;

  Status = GetConfigString ("DefaultChargerApp", DefaultApp, &Size);
  if (Status == EFI_SUCCESS)
  {
    Status = LaunchAppFromGuidedFv(&gMainFvGuid, DefaultApp, NULL);
    if (EFI_ERROR(Status))
      DEBUG((EFI_D_ERROR, "Failed to launch default charger app, status: %r\n", Status));
  }

  Size = DEF_APP_STR_LEN;

  Status = GetConfigString ("DefaultBDSBootApp", DefaultApp, &Size);
  if (Status == EFI_SUCCESS)
  {
    DisplayPOSTTime ();
    LaunchAppFromGuidedFv(&gEfiAblFvNameGuid, DefaultApp, NULL);
    //If we return from above function, considered a failure
    ConfirmShutdownOnFailure();
  }
  else
  {
    DEBUG ((EFI_D_INFO, "[QcomBds] No default boot app specified\n"));
  }

  return Status;
}

/**
 * Launch default OEM Setup App.
*/
STATIC
VOID
LaunchOEMSetupApp (VOID)
{   
  CHAR8       DefaultApp[DEF_APP_STR_LEN];
  UINTN       Size = DEF_APP_STR_LEN;  
  EFI_STATUS  Status = EFI_SUCCESS;
  
  SetOsIndicationsSupported();
  
  if(CheckAndClearBootToFWUIField())
  {
    Status = GetConfigString ("OEMSetupApp", DefaultApp, &Size);    
    if (Status == EFI_SUCCESS)
    {
      Status = LaunchAppFromGuidedFv(&gMainFvGuid, DefaultApp, NULL);
      if (EFI_ERROR(Status))
        DEBUG((EFI_D_ERROR, "Failed to launch default OEM setup app, status: %r\n", Status));
    }
    else if(!RETAIL)
    {
      /* fallback to BDS menu, since BOOT_TO_FW_UI bit is set but no OEMApp specified */
      LaunchBDSMenu();
    }
  }
}


/**
 Detect hot key push for entering
   - Mass storage mode
   - UEFI Shell 2.0/EBL
  */
STATIC
EFI_STATUS
EFIAPI
PlatformBdsDetectHotKey (VOID)
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_INPUT_KEY Key;
  UINT32   ReadKeyAttrib = 0;
  UINTN   DataSize;
  UINT8    Flag = 0;
  UINT8   IBlkFlag = 0;
  STATIC volatile UINT32 ForceMenuForJTAG = 0;
  UefiInfoBlkType* UefiInfoBlockPtr = NULL;
  UINT32  EnableOEMSetupAppInRetail = 0x0;  
  CHAR8 DefaultApp[DEF_APP_STR_LEN];
  UINTN Size = DEF_APP_STR_LEN;    

  //PrintBootOrder();

  UefiInfoBlockPtr = (UefiInfoBlkType*)GetInfoBlkPtr ();
  if (UefiInfoBlockPtr != NULL)
  {
    IBlkFlag = UefiInfoBlockPtr->BdsHotkey;
    if (IBlkFlag)
      DEBUG((EFI_D_ERROR, "Hotkey initiated by T32\n"));
  }

  DataSize = sizeof(Flag);
  Status = gRT->GetVariable (BDS_HOTKEY_STATE_VARNAME,
                             &gQcomTokenSpaceGuid,
                             NULL,
                             &DataSize,
                             &Flag);

  /* Disable hotkey menu if BDSHotKeyState = 1 */
  if ((Status == EFI_SUCCESS) && (Flag == BDSHotKeyStateDisableDetect)  && (ForceMenuForJTAG == 0))
  {
    DEBUG ((EFI_D_WARN, "HotKey Menu Disabled. Skipping HotKey Detection.\r\n"));
    return EFI_UNSUPPORTED;
  }

  ReadKeyAttrib |= READ_KEY_ATTRIB_RESET_AFTER_READ;
  ReadKeyAttrib |= READ_KEY_ATTRIB_NO_BLOCKING;

  /* Init for KW */
  Key.ScanCode = SCAN_NULL;
  Key.UnicodeChar = CHAR_NULL;


  /* Force hotkey menu if BDSHotKeyState = 2 */
  if (ForceMenuForJTAG || (Flag == BDSHotKeyStateForceMenu) || IBlkFlag)
  {
    Status = EFI_SUCCESS;
    Key.ScanCode = SCAN_HOME;
  }
  else
  {
    // Check if HotKey is found
    Status = ReadAnyKey (&Key, ReadKeyAttrib);
  }

  if (Status == EFI_SUCCESS)
  {
    DEBUG ((EFI_D_INFO, "Key Detected: ScanCode = (0x%x), UnicodeChar =(0x%x) \r\n",
            Key.ScanCode, Key.UnicodeChar));

    EnableSynchronousSerialPortIO ();

  if (Key.ScanCode == SCAN_HOME)
    {
      Status = GetConfigValue("EnableOEMSetupAppInRetail", &EnableOEMSetupAppInRetail);
      Status = GetConfigString ("OEMSetupApp", DefaultApp, &Size);    
      if ((Status == EFI_SUCCESS) && (EnableOEMSetupAppInRetail == 0x01))
      {
        DEBUG ((EFI_D_WARN, "Hotkey detected with OEMSetupApp set, entering OEMSetupApp\n"));
        Status = LaunchAppFromGuidedFv(&gMainFvGuid, DefaultApp, NULL);
        if (EFI_ERROR(Status))
          DEBUG((EFI_D_ERROR, "Failed to launch default OEM setup app, status: %r\n", Status));
      }
      else if(!RETAIL)
      {
        DEBUG ((EFI_D_WARN, "Hotkey detected, entering Menu\n"));        
        LaunchBDSMenu();
      }
    }
  }

  DEBUG ((EFI_D_INFO, "Continuing with BDS initialization\n"));
  return Status;
}

STATIC
EFI_STATUS
EFIAPI
PlatformBdsDetectBootHotKey (BDS_INIT_OPTION *InitOption)
{
  EFI_INPUT_KEY Key = {SCAN_NULL, 0};
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32   ReadKeyAttrib = 0;

  ReadKeyAttrib |= READ_KEY_ATTRIB_RESET_AFTER_READ;
  ReadKeyAttrib |= READ_KEY_ATTRIB_NO_BLOCKING;

  Status = ReadAnyKey (&Key, ReadKeyAttrib);

  if (Key.ScanCode == SCAN_DOWN)
    *InitOption = BootFromRemovableMedia;
  else
    *InitOption = OptionNone;

  return Status;
}

STATIC
EFI_STATUS
EFIAPI
DetectRetailUserAttentionHotkey ()
{
  EFI_INPUT_KEY   Key = {SCAN_NULL, 0};
  EFI_STATUS      Status = EFI_SUCCESS;
  UINT32          ReadKeyAttrib = 0;
  UINT16          HotKeyCode;
  UINTN           FailCnt = 0;

  Status = GetConfigValue("DetectRetailUserAttentionHotkeyCode", (UINT32 *)&HotKeyCode);
  if (!EFI_ERROR(Status))    
  {
    ReadKeyAttrib |= READ_KEY_ATTRIB_RESET_AFTER_READ;
    ReadKeyAttrib |= READ_KEY_ATTRIB_NO_BLOCKING;

    Status = ReadAnyKey (&Key, ReadKeyAttrib);

    if(Key.ScanCode == HotKeyCode)
    {        
      while (1)
      {
        /* Wait 250ms before reading */
        gBS->Stall (250000);

        Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
        if (Status != EFI_SUCCESS)
        {
          if (FailCnt > 2)
            break;

          ++FailCnt;
        }
      }

      AsciiPrint ("-----------------------\n\n"
                  "Do you want to delete secure boot variables\n"
                  "-----------------------\n\n");      
      AsciiPrint ("Press [Power + Vol Down] to confirm\n"
                  "Press [Power + Vol Up] to cancel\n"); 
                  
      while(1)
      {
        Status = ReadAnyKey (&Key, ReadKeyAttrib);
        if((Key.ScanCode == SCAN_DELETE) || (Key.ScanCode == SCAN_HOME))
          break;
      }      
        
      if (!EFI_ERROR(Status) && (Key.ScanCode == SCAN_DELETE))
      {        
        if(VariableServicesProtocol && (VariableServicesProtocol->DelSecBootVars != NULL))
        {
          DEBUG ((EFI_D_WARN, "Deleting secure boot variables\n"));
          VariableServicesProtocol->DelSecBootVars(VariableServicesProtocol);
        }
      }
      DEBUG ((EFI_D_WARN, "System Restart \n"));
      gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
    }
  }
  
  return Status;
}

VOID ConfirmShutdownOnFailure (VOID)
{
  EFI_INPUT_KEY  Key;
  EFI_STATUS Status;
  Print (L"Press any key to shutdown\n");
  Status = ReadAnyKey (&Key, 0);
  if (Status == EFI_SUCCESS)
  {
    Print (L"Key detected, shutting down\n");
    gRT->ResetSystem (EfiResetShutdown, EFI_SUCCESS, 0, NULL);
  }
}


/**
  This is called after the BDS exhuasts all options, either
  through enumeration or attempts to boot
**/
VOID
PlatformBdsBootHalt (VOID)
{
  //Last resort if no other bootable option exists
  if (!RETAIL && EnableShellFlag)
  {
    LoadDebugToolsFv ();
    PlatformBdsLoadShellForNonProdMode (NULL);
  }
  ConfirmShutdownOnFailure();
}

/**
  An empty function to pass error checking of CreateEventEx ().

  @param  Event                 Event whose notification function is being invoked.
  @param  Context               The pointer to the notification function's context,
                                which is implementation-dependent.

**/
VOID
EFIAPI
ACPITableLoadEmptyFuntion (
  IN EFI_EVENT                Event,
  IN VOID                     *Context
  )
{
  return;
}

VOID
InitLcdDebugFlag (VOID)
{
  EFI_STATUS                Status;
  UINTN                     VarSize;
  UINT32                    VarData;

  if (RETAIL)
  {
    LcdDebugFlag = 0;  /* Production mode - no messages */
    EnableShellFlag = 0;  /* Production mode - no shell    */
  }
  else
  {
    /* Read DispDebugInfo variable, and handle if it is not present, default no display */
    VarSize = sizeof(UINT32);
    Status = gRT->GetVariable (L"DispDebugInfo", &gQcomTokenSpaceGuid, NULL, &VarSize, &LcdDebugFlag);

    if (EFI_ERROR(Status))
      LcdDebugFlag = 0;  /* Default no info on LCD */

    VarData = 0;
    Status = GetConfigValue ("EnableShell", &VarData);

    if (EFI_ERROR(Status))
      EnableShellFlag = 0; /* Default no Shell*/
    else
      EnableShellFlag = VarData;
  }
}

STATIC
VOID
DisplayUefiTotalTime (VOID)
{
  UINT64 Time;
  UINT32 *UefiStartTime;
  EFI_HOB_GUID_TYPE *GuidHob;

  GuidHob = GetFirstGuidHob (&gEfiStartTimeHobGuid);
  UefiStartTime = GET_GUID_HOB_DATA(GuidHob);

  Time = GetTimerCountms ();

  DEBUG ((EFI_D_ERROR, "UEFI Total : %d ms\n", Time-(*UefiStartTime)));
}

/**
 Display Boot Time in milli seconds
 **/

STATIC
EFI_STATUS
DisplayBootTime (CHAR8* Key, CHAR8* Event, BOOLEAN OnLCD)
{
  UINT64 Time;

  Time = GetTimerCountms ();

  if(LcdDebugFlag && OnLCD)
  {
    AsciiPrint("%a [%5lld] %a\n", Key, Time, Event);
  }
  else
  {
    DEBUG ((EFI_D_ERROR, "%a [%5lld] %a\n", Key, Time, Event));
  }

  return EFI_SUCCESS;
}

VOID
PrintCoreSpeedInfo (VOID)
{
  EFI_STATUS Status;

  Status = gBS->LocateProtocol (&gEfiClockProtocolGuid, NULL,
                                (VOID**) &pClockProtocol);
  if (Status == EFI_SUCCESS)
  {
    UINT32 FreqHz = 0;
    UINT32 FreqMHz = 0;
    UINT32 PerfLevel;
    UINT32 VoltageMv;
    Status = pClockProtocol->GetCpuPerfLevel(pClockProtocol, 0, &PerfLevel);
    if (Status == EFI_SUCCESS)
    {
      if (pClockProtocol->GetCpuPerfLevelFrequency(pClockProtocol, 0,
                                                   PerfLevel,
                                                   &FreqHz,
                                                   &VoltageMv) == EFI_SUCCESS)
      {
        FreqMHz = FreqHz/1000000;
        DEBUG ((EFI_D_ERROR, "Core 0 Freq: %d MHz\r\n", FreqMHz));
      }
    }
  }
}

/**
  Retrieve platform info and send to the console
**/
STATIC
EFI_STATUS
DisplayPlatformInfo (VOID)
{
  EFI_STATUS         Status;
  CHAR8              DestBuffer[128];
  CHAR8*             PlatTypePtr;
  CHAR8*             ChipNamePtr;
  CHAR8*             ChipVerPtr;
  CHAR8              AsciiString[50];
  UINTN              Sz;
  BOOLEAN            DispVer = PcdGetBool (VersionDisplay);

  /* Override compile time setting */
  if (LcdDebugFlag == 1)
    DispVer = TRUE;
  else
    DispVer = FALSE;

  Status = GetPlatformStrings (DestBuffer, sizeof(DestBuffer),
                               &PlatTypePtr, &ChipNamePtr, &ChipVerPtr);

  if (Status != EFI_SUCCESS)
    return Status;

  /* Display to LCD Screen only if needed during development time */
  if (DispVer)
  {
    AsciiPrint ("Platform   : %a\n", PlatTypePtr);
    AsciiPrint ("Chip Name  : %a\n", ChipNamePtr);
    AsciiPrint ("Chip Ver   : %a\n", ChipVerPtr);
  }
  else
  {
    DEBUG ((EFI_D_ERROR, "Platform   : %a\n", PlatTypePtr));
    DEBUG ((EFI_D_ERROR, "Chip Name  : %a\n", ChipNamePtr));
    DEBUG ((EFI_D_ERROR, "Chip Ver   : %a\n", ChipVerPtr));
  }


  Sz = AsciiSPrint (AsciiString, sizeof (AsciiString),
                   "\nPlatform : %a\n", PlatTypePtr);

  /* For nul termination */
  ++Sz;

  gRT->SetVariable (L"PlatformInfo", &gQcomTokenSpaceGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS, Sz, AsciiString);

  PrintCoreSpeedInfo();

  return Status;
}

/**
  Updates platform specific variables for use after BDS

**/

VOID
EFIAPI
UpdatePlatformVars(VOID)
{
  EFI_STATUS Status;
  EFI_HWIO_PROTOCOL *HWIOProtocol = NULL;
  UINTN      DataSize = 0;
  UINT64     Addr = 0;
  CHAR8      *SdccRegBaseName = NULL;

  Status = gBS->LocateProtocol(&gEfiHwioProtocolGuid, NULL, (void**)&HWIOProtocol);
  if ((EFI_SUCCESS != Status) || (HWIOProtocol == NULL))
    DEBUG ((EFI_D_ERROR, "ERROR: Failed to locate HWIO Protocol: 0x%08x\n", Status));

  Status = GetConfigValue64("SharedIMEMBaseAddr", &Addr);
  if (Status == EFI_SUCCESS)
  {
    DataSize = sizeof(Addr);
    Status = gRT->SetVariable (L"Shared_IMEM_Base",
                               &gQcomTokenSpaceGuid,
                               EFI_VARIABLE_BOOTSERVICE_ACCESS,
                               DataSize,
                               &Addr);
  }

  if (!HWIOProtocol)
    return;

  Status = HWIOProtocol->MapRegion(HWIOProtocol, "+SLP_CNTR", (UINT8**) &Addr);
  if (EFI_SUCCESS != Status)
    DEBUG ((EFI_D_ERROR, "ERROR: Failed to map SLP_CNTR base address: 0x%08x\n", Status));

  DataSize = sizeof(Addr);
  Status = gRT->SetVariable (L"MPM2_SLP_CNTR_ADDR",
                             &gQcomTokenSpaceGuid,
                             EFI_VARIABLE_BOOTSERVICE_ACCESS,
                             DataSize,
                             &Addr);

  if(boot_from_ufs() == TRUE)
  {
  Status = HWIOProtocol->MapRegion(HWIOProtocol, "+UFS_MEM_UFS", (UINT8**) &Addr);
  }
  else
  {
    SdccRegBaseName = (CHAR8 *)PcdGetPtr(SDCCRegBase);
    if (SdccRegBaseName != NULL)
    {
      Status = HWIOProtocol->MapRegion(HWIOProtocol, SdccRegBaseName, (UINT8**) &Addr);
    }
  }							 

  if (EFI_SUCCESS != Status)
    DEBUG ((EFI_D_ERROR, "ERROR: Failed to map UFS_MEM_UFS_REGS base address: 0x%08x\n", Status));

  DataSize = sizeof(Addr);
  Status = gRT->SetVariable (L"BootDeviceBaseAddr",
                             &gQcomTokenSpaceGuid,
                             EFI_VARIABLE_BOOTSERVICE_ACCESS,
                             DataSize,
                             &Addr);

}

/**
  Updates the number of boot cycles and prints boot information stored in NV.

**/
VOID
EFIAPI
UpdateNVVars (VOID)
{
  EFI_STATUS Status;
  UINTN DataSize;
  UINT32 VarData = 0;

  // Print Boot Cycles
  DataSize = sizeof(BootCycleCount);
  Status = gRT->GetVariable (L"MTC",
                             &gQcomTokenSpaceGuid,
                             NULL,
                             &DataSize,
                             &BootCycleCount);
  if (Status != EFI_SUCCESS)
  {
    DEBUG ((EFI_D_WARN, "Boot Cycles not initialized, set to 0\n"));
    BootCycleCount = 0;
  }
  DEBUG ((EFI_D_WARN, "Boot Cycle : %d\n", BootCycleCount));


  // Print Run Cycles
  DataSize = sizeof(UINT32);
  Status = gRT->GetVariable (L"RunCycles",
                             &gQcomTokenSpaceGuid,
                             NULL,
                             &DataSize,
                             &VarData);

  if (Status == EFI_NOT_FOUND)
    VarData = 0;

  ++VarData;

  Status = gRT->SetVariable (L"RunCycles",
                             &gQcomTokenSpaceGuid,
                             (EFI_VARIABLE_NON_VOLATILE |
                             EFI_VARIABLE_BOOTSERVICE_ACCESS |
                             EFI_VARIABLE_RUNTIME_ACCESS),
                             DataSize,
                             &VarData);

  DataSize = sizeof(UINT32);
  Status = gRT->GetVariable (L"RunCycles",
                             &gQcomTokenSpaceGuid,
                             NULL,
                             &DataSize,
                             &VarData);
  if (Status != EFI_SUCCESS)
  {
    DEBUG ((EFI_D_WARN, "Run Cycles : not set\n"));
    VarData = 0;
  }
  DEBUG ((EFI_D_WARN, "Run  Cycle : %d\n", VarData));
}

/* Setup Platform related security */
VOID SetupPlatformSecurity (VOID)
{
  EFI_STATUS    Status;
  UINTN         VarSize;
  UINT8*        Buffer = NULL;
  UINT8         SetupMode, SecureBoot;
  UINT32        TypeGUIDBufferSize;

  /* Assume by default we are in setupmode and Secure Boot is not enabled */
  SetupMode = 1;
  SecureBoot = 0;

  VarSize = sizeof(SetupMode);
  Status = gRT->GetVariable (L"SetupMode", &gEfiGlobalVariableGuid,
                             NULL, &VarSize, &SetupMode);
  if (EFI_ERROR(Status))
  {
    DEBUG ((EFI_D_WARN, "SetupMode Variable does not exist. Will not enable Secure Boot\n"));
  }

  if (SetupMode == 1)
  {
    /* Disable secure boot if in setup mode */
    SecureBoot = 0;
    Status = gRT->SetVariable (L"SecureBoot", &gEfiGlobalVariableGuid,
                               EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                               VarSize, &SecureBoot);
  }
  else
  {
    /* Enable Secure Boot if not in setup mode */
    SecureBoot = 1;
    Status = gRT->SetVariable (L"SecureBoot", &gEfiGlobalVariableGuid,
                               EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                               VarSize, &SecureBoot);
  }

  /* Post signal here to enable Image Authentication */
  if (SecureBoot == 1)
    SetSecBootRegSignal ();

  /* Set the variable indicating the signature types supported. If Auth routines support
   * more than SigDB lib supports, then the GUID's from Auth routines also need to be appended
   * to this list */
  TypeGUIDBufferSize = 0;
  if ((GetSupportedSignatureTypes (0, &TypeGUIDBufferSize) == SIG_DB_ENUM_BUFFER_SIZE_INSUFFICIENT) &&
     (TypeGUIDBufferSize > 0))
  {
    Buffer = AllocatePool(TypeGUIDBufferSize);

    if (Buffer)
    {
      if (GetSupportedSignatureTypes (Buffer, &TypeGUIDBufferSize) == SIG_DB_ENUM_SUCCESS)
      {
        VarSize = TypeGUIDBufferSize;
        Status = gRT->SetVariable (L"SignatureSupport", &gEfiGlobalVariableGuid,
                                   EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                   VarSize, Buffer);
      }
      FreePool (Buffer);
    }
  }
}

/**
  Library constructor which registers for the Exit boot services callback
**/
EFI_STATUS
EFIAPI
RegisterEBSCallback
(
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  //Register to Exit Boot Service Event
  Status = gBS->CreateEventEx (EVT_NOTIFY_SIGNAL,
                               TPL_CALLBACK,
                               PlatformBDSExitBSCallback,
                               NULL,
                               &gEfiEventExitBootServicesGuid,
                               &EfiExitBootServicesEvent);
  ASSERT_EFI_ERROR (Status);
  return Status;
}


VOID
MountRequiredFatPartitions(VOID)
{
  EFI_STATUS Status;
  /* Mount EFIESP for boot option enumeration */
  Status = MountFatPartition(L"efiesp");
}

VOID
PerformVariableActions (VOID)
{
  EFI_STATUS Status;
  UINTN      DataSize = 0;

  /* Look up Variable Services protocol to be used to flush Variable tables
   * during Exit Bootservices. Cache the protocol handle to use just in case
   * if somebody else installs the protocol to spoof and hijack.
   * Its Ok for this call to fail if the protocol is not installed. */
  Status = gBS->LocateProtocol (&gEfiVariableServicesProtocolGuid,
                                NULL,
                                (VOID**) &VariableServicesProtocol);
  if (Status != EFI_SUCCESS)
  {
    DEBUG ((EFI_D_INFO, "Var Store  : Warning, not persistent\r\n"));
  }
  else
  {
    UpdateNVVars ();
  }

  /* Check if UEFI NV tables are volatile */
  DataSize = sizeof(VolatileTables);
  Status = gRT->GetVariable (L"VolatileTables",
                             &gQcomTokenSpaceGuid,
                             NULL,
                             &DataSize,
                             &VolatileTables);
  if ((Status == EFI_SUCCESS) && (VolatileTables != 0) && (RETAIL))
  {
    /* In production mode, Shutdown if tables are volatile;
       this indicates an error in reading UEFI NV tables */
    DEBUG ((EFI_D_ERROR, "ERROR: UEFI NV Variables not properly initialized. Shutting down.\r\n"));
    gRT->ResetSystem (EfiResetShutdown, EFI_SUCCESS, 0, NULL);

    /* Should not reach here */
    CpuDeadLoop();
  }
  else if (VolatileTables != 0)
  {
    DEBUG ((EFI_D_WARN, "WARNING: UEFI NV tables are enabled as volatile.\r\n"));

    /* If volatile tables are initialized for NV tables,
       check if manual flag is set to enable UEFI NV tables as volatile */
    DataSize = sizeof(EnableVolatileBootOptions);
    Status = gRT->GetVariable (L"EnableVolatileBootOptions",
                               &gQcomTokenSpaceGuid,
                               NULL,
                               &DataSize,
                               &EnableVolatileBootOptions);
  }

  UpdatePlatformVars ();
}

/**
  Platform Bds init. Include the platform firmware vendor, revision
  and so crc check.

**/
VOID
PlatformBdsInit(VOID)
{
  BDS_INIT_OPTION InitOption = OptionNone;
  PlatformBdsInitEx (&InitOption);
}

/**
  Platform Bds init with init options passed for removable boot detect
**/
VOID
EFIAPI
PlatformBdsInitEx (BDS_INIT_OPTION *InitOption)
{
  EFI_STATUS                     Status;
#ifndef PRE_SIL
  EFI_QCOM_TOOLSUPPORT_PROTOCOL *ToolSupportProtocol = NULL;
  EFI_ENCRYPTION_PROTOCOL       *Encryption = NULL;
#endif
  UINT32  HotKeyDetection = 0x0;
  UINT32  EnableOEMSetupAppInRetail = 0x0;  

  DEBUG ((DEBUG_ERROR, "-----------------------------\n"));
  DisplayBootTime("Platform Init ", "BDS", TRUE);


  /* Handle all variable storage related things */
  PerformVariableActions();

  /* Make consoles available to Apps via ST */
  BdsLibConnectAllConsoles ();

  /* Sample and display system information (if required on LCD display screen) */
  InitLcdDebugFlag ();
  DisplayPlatformInfo ();

  /* Mount LogFS partition if enabled in the uefi platform config */
  MountFatPartition(L"logfs");

  DEBUG ((DEBUG_ERROR, "-----------------------------\n"));

  if(VariableServicesProtocol == NULL)
  {
    Status = gBS->LocateProtocol (&gEfiVariableServicesProtocolGuid,
                                NULL,
                                (VOID**) &VariableServicesProtocol);
  }
  
  Status = GetConfigValue("DetectRetailUserAttentionHotkey", &HotKeyDetection);
  if (!EFI_ERROR(Status) && (HotKeyDetection == 0x01))
  { 
    DetectRetailUserAttentionHotkey();
  }
  
  if(VariableServicesProtocol && (VariableServicesProtocol->SendMileStoneCall != NULL))
  {    
    Status = VariableServicesProtocol->SendMileStoneCall(VariableServicesProtocol);
  }    
  
  Status = GetConfigValue("EnableOEMSetupAppInRetail", &EnableOEMSetupAppInRetail);
  if (!EFI_ERROR(Status) && (EnableOEMSetupAppInRetail == 0x01))
  {   
    LaunchOEMSetupApp();
  }  

  /************************************************************************************
   * WARNING:   START
   *
   * NOTE: Security Alert..!!
   *
   *   The code until the function SetupPlatformSecurity is called, is running
   *   WITHOUT UEFI security enabled yet. So, in production image any Menu/Shell
   *   or app running, before security enabled, will be a security hole.
   *
   *   So do NOT modify any code in this WARNING context.
   *
   */
  if ((!RETAIL) || (EnableOEMSetupAppInRetail == 0x01))
  {
    /* Detect hotkey for development purposes
     * If enabled in PROD image this would be a SECURITY HOLE
     * So do not enable this */
    PlatformBdsDetectHotKey ();
  }

  /* Load default debug application specified in config file */
  LaunchDefaultBDSApps ();

  /* Validate and take any action for the HW platform configuration */
  ValidateHWConfig ();

  /*  This is the place where UEFI Security is enabled, including UEFI Image Authentication.
   *  So any image executed before this call, has to be strictly from part of an already
   *  authenticated image/package and the app should not be another app launching platform
   *  like shell (unless it also make sure the app launched is from authenticated package).
   */
  SetupPlatformSecurity ();

  /*
   * WARNING:   END
   *
   *********************************************************************************/

  StartCyclCounter();

  MountRequiredFatPartitions();

  PlatformBdsDetectBootHotKey (InitOption);

  /* It is necessary to run SetupPlatformSecurity() before RunFwUpdateMain() and RunFwProvisionMain()
   * so that payload would be authenticated. It is also necessary to run RunFwUpdateMain() before
   * ACPI loading so that FwUpdate is more robust.
   */
  DEBUG ((EFI_D_INFO, "Attempting to start: Firmware update\n"));
  Status = RunFwUpdateMain(gImageHandle, gST);
  if (Status != EFI_SUCCESS)
  {
    DEBUG ((EFI_D_WARN, "Firmware update failed\n"));
  }

  // Signal ACPI Table Loading
  Status = gBS->CreateEventEx (EVT_NOTIFY_SIGNAL,
                               TPL_CALLBACK,
                               ACPITableLoadEmptyFuntion,
                               NULL,
                               &gEfiACPITableLoadGuid,
                               &ACPITableLoadEvent);
  if (Status == EFI_SUCCESS)
    gBS->SignalEvent (ACPITableLoadEvent);

#ifndef PRE_SIL
  /* ACPI tables should be loaded at this point. It is necessary to run
   * RunFwProvisionMain() after ACPI loading so that provisioning routine
   * can access RSDP pointer in EFI configuration table and save it for
   * HLOS app to consume.
   */
  DEBUG ((EFI_D_INFO, "Attempting to start: Firmware provisioning\n"));
  Status = RunFwProvisionMain (gImageHandle, gST);
  if (Status != EFI_SUCCESS)
  {
    DEBUG ((EFI_D_WARN, "Firmware provisioning failed\n"));
  }

  // Locate EFI_ENCRYPTION_PROTOCOL.
  Status = gBS->LocateProtocol (&gEfiEncryptionProtocolGuid,
                                NULL,
                                (VOID**)&Encryption);

  // call SetHdcp to set hdcp provisioning data
  if ((Status == EFI_SUCCESS) && (Encryption != NULL))
  {
    Status = Encryption->SetHdcp ();
    if (EFI_ERROR(Status))
    {
      DEBUG ((EFI_D_INFO, "Set Hdcp Data failed, Status =  (0x%x)\r\n", Status));
    }
  }

  // Locate EFI_TOOL_SUPPORT_PROTOCOL.
  Status = gBS->LocateProtocol (&gQcomToolSupportProtocolGuid,
                                NULL,
                                (VOID**)&ToolSupportProtocol);

  // Security calls
  if ((Status == EFI_SUCCESS) && (ToolSupportProtocol != NULL ))
  {
    ToolSupportProtocol->HandleMorPpi (ToolSupportProtocol);

    if ((ToolSupportProtocol->TzFuseMilestone (ToolSupportProtocol) != EFI_SUCCESS) ||
        (ToolSupportProtocol->NeedQcomPkProtection (ToolSupportProtocol) == TRUE))
    {
      gRT->ResetSystem (EfiResetShutdown, EFI_SUCCESS, 0, NULL);
      /* Should not reach here */
      CpuDeadLoop();
    }
  }

  /* ACPI tables should be loaded before SetMeasureBootStartSignal()
   * and SetMeasureBootRegSignal().
   * Start measuring and register
   */
  SetMeasureBootStartSignal ();

  /* Register measure boot values */
  SetMeasureBootRegSignal ();
#endif

  /* Fix System Table CRC after updating Firmware Revision */
  gST->Hdr.CRC32 = 0;
  gBS->CalculateCrc32 ((VOID *)gST, sizeof(EFI_SYSTEM_TABLE), &gST->Hdr.CRC32);

  Status = gBS->LocateProtocol (&gQcomScmProtocolGuid,
                                NULL,
                                (VOID**)&pSCMProtocol);
  if (EFI_ERROR(Status))
  {
     DEBUG ((EFI_D_ERROR, "Register SCM protocol failed, Status =  (0x%x)\r\n", Status));
  }

  DEBUG ((DEBUG_ERROR, "Platform Init End : %d\n-----------------------------\n",
                      GetTimerCountms ()));

  /* Dump Log Buffer to File */
  Status = WriteLogBufToPartition ();
  if (EFI_ERROR(Status))
  {
     DEBUG ((EFI_D_WARN, "Write Log Buffer to FAT partition failed, Status = (%r)\r\n", Status));
  }

  /* Drain serial port finally */
  SerialPortDrain ();
}

/**
  Connect the predefined platform default console device. Always try to find
  and enable the vga device if have.

  @param PlatformConsole          Predefined platform default console device array.

  @retval EFI_SUCCESS             Success connect at least one ConIn and ConOut
                                  device, there must have one ConOut device is
                                  active vga device.
  @return Return the status of BdsLibConnectAllDefaultConsoles ()

**/
EFI_STATUS
PlatformBdsConnectConsole (
  IN BDS_CONSOLE_CONNECT_ENTRY   *PlatformConsole
  )
{
  return EFI_SUCCESS;
}

/**
  Connect with predefined platform connect sequence,
  the OEM/IBV can customize with their own connect sequence.
**/
VOID
PlatformBdsConnectSequence (VOID)
{
  //
  // Just use the simple policy to connect all devices
  //
  BdsLibConnectAll ();
}

/**
  Load the predefined driver option, OEM/IBV can customize this
  to load their own drivers

  @param BdsDriverLists  - The header of the driver option link list.

**/
VOID
PlatformBdsGetDriverOption (
  IN OUT LIST_ENTRY              *BdsDriverLists
  )
{
}

/**
  Perform the platform diagnostic, such like test memory. OEM/IBV also
  can customize this function to support specific platform diagnostic.

  @param MemoryTestLevel  The memory test intensive level
  @param QuietBoot        Indicate if need to enable the quiet boot
  @param BaseMemoryTest   A pointer to BdsMemoryTest()

**/
VOID
PlatformBdsDiagnostics (
  IN EXTENDMEM_COVERAGE_LEVEL    MemoryTestLevel,
  IN BOOLEAN                     QuietBoot,
  IN BASEM_MEMORY_TEST           BaseMemoryTest
  )
{
}


/**
  Start an EFI image (PE32+ with EFI defined entry point).

  Argv[0] - device name and path
  Argv[1] - "" string to pass into image being started

  fs1:\Temp\Fv.Fv "arg to pass" ; load an FV from the disk and pass the
                                ; ascii string arg to pass to the image
  fv0:\FV                       ; load an FV from an FV (not common)
  LoadFile0:                    ; load an FV via a PXE boot

  @param  Argc   Number of command arguments in Argv
  @param  Argv   Array of strings that represent the parsed command line.
                 Argv[0] is the App to launch

  @return EFI_SUCCESS

**/
EFI_STATUS
BdsStartApp (IN UINTN  Argc, IN CHAR8  **Argv)
{
  EFI_STATUS                  Status;
  EFI_OPEN_FILE               *File;
  EFI_DEVICE_PATH_PROTOCOL    *DevicePath;
  EFI_HANDLE                  ImageHandle;
  UINTN                       ExitDataSize;
  CHAR16                      *ExitData;
  VOID                        *Buffer;
  UINTN                       BufferSize;
  EFI_LOADED_IMAGE_PROTOCOL   *ImageInfo;

  ImageHandle = NULL;

  if (Argc < 1)
    return EFI_INVALID_PARAMETER;

  File = EfiOpen (Argv[0], EFI_FILE_MODE_READ, 0);
  if (File == NULL)
    return EFI_INVALID_PARAMETER;

  DevicePath = File->DevicePath;
  if (DevicePath != NULL)
  {
    // check for device path form: blk, fv, fs, and loadfile
    Status = gBS->LoadImage (FALSE, gImageHandle, DevicePath, NULL, 0, &ImageHandle);
  }
  else
  {
    // Check for buffer form: A0x12345678:0x1234 syntax.
    // Means load using buffer starting at 0x12345678 of size 0x1234.

    Status = EfiReadAllocatePool (File, &Buffer, &BufferSize);
    if (EFI_ERROR (Status))
    {
      EfiClose (File);
      return Status;
    }

    if (Buffer == NULL)
      return EFI_OUT_OF_RESOURCES;

    Status = gBS->LoadImage (FALSE, gImageHandle, DevicePath, Buffer, BufferSize, &ImageHandle);

    if (Buffer != NULL)
      FreePool (Buffer);
  }

  EfiClose (File);

  if (!EFI_ERROR (Status))
  {
    if (Argc >= 2)
    {
      // Argv[1] onwards are strings that we pass directly to the EFI application
      // We don't pass Argv[0] to the EFI Application, just the args
      Status = gBS->HandleProtocol (ImageHandle, &gEfiLoadedImageProtocolGuid, (VOID **)&ImageInfo);
      ASSERT_EFI_ERROR (Status);

      if (ImageInfo == NULL)
        return EFI_NOT_FOUND;

      /* Need WideChar string as CmdLineArgs */
      ImageInfo->LoadOptionsSize = 2 * (UINT32)AsciiStrSize (Argv[1]);
      ImageInfo->LoadOptions     = AllocatePool (ImageInfo->LoadOptionsSize);
      if (ImageInfo->LoadOptions == NULL)
        return EFI_OUT_OF_RESOURCES;
      AsciiStrToUnicodeStr (Argv[1], ImageInfo->LoadOptions);
    }

    // Transfer control to the EFI image we loaded with LoadImage()
    Status = gBS->StartImage (ImageHandle, &ExitDataSize, &ExitData);
  }

  return Status;
}

/**
  Hook point after a boot attempt succeeds. We don't expect a
  boot option to return, so the UEFI 2.0 specification defines
  that you will default to an interactive mode and stop
  processing the BootOrder list in this case. This is also a
  platform implementation and can be customized by IBV/OEM.

  @param  Option                  Pointer to Boot Option that succeeded to boot.

**/
VOID
EFIAPI
PlatformBdsBootSuccess (IN  BDS_COMMON_OPTION *Option)
{
  // measure this string.
  SetMeasureReturnFromEfiAppFromBootOptionSignal ();

  DEBUG ((EFI_D_WARN, "Successfully booted %S\n", Option->Description));
}

/**
  Hook point after a boot attempt fails.

  @param  Option                  Pointer to Boot Option that failed to boot.
  @param  Status                  Status returned from failed boot.
  @param  ExitData                Exit data returned from failed boot.
  @param  ExitDataSize            Exit data size returned from failed boot.

**/
VOID
EFIAPI
PlatformBdsBootFail (
  IN  BDS_COMMON_OPTION  *Option,
  IN  EFI_STATUS         Status,
  IN  CHAR16             *ExitData,
  IN  UINTN              ExitDataSize
  )
{
  // measure this string.
  SetMeasureReturnFromEfiAppFromBootOptionSignal ();

  DEBUG ((EFI_D_ERROR, "Failed to boot %S\n Status = 0x%08x\n", Option->Description, Status));
}

/**
  This function is remained for IBV/OEM to do some platform action,
  if there no console device can be connected.

  @return EFI_SUCCESS      Direct return success now.

**/
EFI_STATUS
PlatformBdsNoConsoleAction (VOID)
{
  return EFI_SUCCESS;
}

/**
  This function locks platform flash that is not allowed to be updated during normal boot path.
  The flash layout is platform specific.
**/
VOID
EFIAPI
PlatformBdsLockNonUpdatableFlash (VOID)
{
  return;
}


/**
  Lock the ConsoleIn device in system table. All key
  presses will be ignored until the Password is typed in. The only way to
  disable the password is to type it in to a ConIn device.

  @param  Password        Password used to lock ConIn device.

  @retval EFI_SUCCESS     lock the Console In Spliter virtual handle successfully.
  @retval EFI_UNSUPPORTED Password not found

**/
EFI_STATUS
EFIAPI
LockKeyboards (IN  CHAR16    *Password)
{
  return EFI_UNSUPPORTED;
}

/**
  Hook point to boot from eMMC, if booting from BootOrder
  failed. We don't expect a boot option to return, so the UEFI
  2.0 specification defines that you will default to an
  interactive mode and stop processing the BootOrder list in
  this case. This is also a platform implementation and can be
  customized by IBV/OEM.

  @param  Option                  Pointer to Boot Option that succeeded to boot.

**/
VOID
EFIAPI
PlatformBdsLoadShellForNonProdMode (IN  BDS_COMMON_OPTION *Option)
{
  BOOLEAN ProdModeEnabled = (RETAIL);

  if (ProdModeEnabled)
  {
    ASSERT (FALSE);
    CpuDeadLoop ();
  }

  /* End Perf marker */
  PERF_END (NULL, "BDS", NULL, 0);

  SerialPortDrain ();
  EnableSynchronousSerialPortIO ();

  /* TODO: Conditionalize these to loading only from specific GUIDed FVs
   * that is are already authenticated
   * */
  // Drop to UEFI Shell
  DisplayPOSTTime ();

  //PrintBootOrder();

  DEBUG ((EFI_D_WARN, "Attempting to start: Ebl\n"));
  LaunchAppFromGuidedFv(&gToolsFvGuid, "Ebl", NULL);

  DEBUG ((EFI_D_WARN, "Attempting to start: Shell\n"));
  LaunchAppFromGuidedFv(&gToolsFvGuid, "Shell", NULL);
}

/**
  Function that executes just before loading a boot option image.

  @param  Option                  Pointer to Boot Option that succeeded to boot.

**/
EFI_STATUS
EFIAPI
PlatformBdsPreLoadBootOption (IN  BDS_COMMON_OPTION *Option)
{
  BOOLEAN ProdModeEnabled = (RETAIL);

  /* Do not allow boot option to boot if in production scenario */
  if ((VolatileTables != 0) && (ProdModeEnabled || (!EnableVolatileBootOptions)))
    return EFI_DEVICE_ERROR;

  if (Option == NULL)
    return EFI_INVALID_PARAMETER;

  if (Option->Description == NULL)
    DEBUG ((DEBUG_INFO | DEBUG_LOAD | DEBUG_ERROR, "Booting option %d:(Boot%04x) from unknown device path\n", Option->OptionNumber, Option->BootCurrent));
  else
    DEBUG ((DEBUG_INFO | DEBUG_LOAD | DEBUG_ERROR, "Booting option %d:(Boot%04x) \"%S\"\n", Option->OptionNumber, Option->BootCurrent, Option->Description));

  DisplayPOSTTime();

  return EFI_SUCCESS;
}

/**
  This Function writes UEFI Log Buffer to LOGFS Partition

  @retval EFI_SUCCESS     File was written successfully
**/
#define MAX_LEN 13
#define FILE_EXT L".txt"
EFI_STATUS
EFIAPI
WriteLogBufToPartition (VOID)
{
  EFI_STATUS Status;
  UINT32 VarData;
  CHAR16 LogBufFile[MAX_LEN] = L"UefiLog";
  UefiInfoBlkType* UefiInfoBlockPtr = NULL;
  UINTN Length;

  UefiInfoBlockPtr = (UefiInfoBlkType*)GetInfoBlkPtr ();
  if (UefiInfoBlockPtr == NULL)
  {
    DEBUG ((EFI_D_ERROR, "GetInfoBlkPtr() invalid!"));
    return EFI_NOT_FOUND;
  }

  /* Only keep Logs of past 5 Boot Cycles
     File number corresponds to current BootCycle (0-4) */
  VarData = BootCycleCount % 5;
  VarData = VarData + '0';  /* Convert int to char */
  /* Construct File name */
  StrCat (LogBufFile, (CHAR16*)&VarData);
  StrCat (LogBufFile, FILE_EXT);

  Length = UefiInfoBlockPtr->UartLogBufferLen;
  if (Length == 0 || UefiInfoBlockPtr->UartLogBufferPtr == NULL)
  {
    DEBUG ((EFI_D_ERROR, "RAM Log Buffer is empty!"));
    return EFI_NOT_FOUND;
  }

  /* Write Logs to Partition */
  Status = WriteFile (LogBufFile,
                      NULL,
                      NULL,
                      L"logfs",
                      TRUE,
                      NULL,
                      &Length,
                      0,
                      (UINT8*)UefiInfoBlockPtr->UartLogBufferPtr,
                      UefiInfoBlockPtr->UartLogBufferLen);
  if (Length != UefiInfoBlockPtr->UartLogBufferLen)
  {
    DEBUG ((EFI_D_ERROR, "Unable to write complete log to partition. Request:0x%08x Written:0x%08x\n",
      UefiInfoBlockPtr->UartLogBufferLen, Length));
  }
  return Status;
}
