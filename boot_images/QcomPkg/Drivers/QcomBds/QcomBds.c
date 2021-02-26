/** @file
*  Copyright (c) 2014-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
*  Portions Copyright (c) 2011-2016, ARM Limited. All rights  Reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/18/17   bd      Send SD Card detect event in case of Boot from removable
 08/22/17   ps      Force USB print log chnages
 04/11/17   sj      Fix the USBEnumeration wait time
 02/02/17   bh      Optimize removable media booting delay
 01/25/17   vk      Free buffer
 08/10/16   va      Caller to print debug msg for time out if required
 06/22/16   bh      Updated retail check
 04/26/16   va      Mounting LogFS FAT partition for debug use 
 12/01/15   vk      Remove app name checking for ABL
 10/29/15   vk      ABL fv loading
 10/22/15   bh      BlkIo NULL check on enumerating optoins
 10/06/15   bh      Fix Boot USB First variable retrieval, change default delay to 30s
 09/30/15   bh      Update logs
 09/29/15   bh      Clean serial logs
 08/23/15   bh      Remove unecessary enumeration case, general cleanup
 06/25/15   bh      BootUSBFirst enable
 04/23/15   bh      Fix KW errors
 03/05/15   bh      Set ValidOption back to FALSE if a handle is not found
 01/04/15   bh      Fix BootOrder issue
 12/11/14   bh      Fix KW errors
 12/09/14   bh      Add support for USB boot through signaling for USB host mode toggle
 11/21/14   bh      Enumerate all media after BootOrder fail (MSFT hack)
 11/12/14   bh      Fix FreeListEntries function
 11/03/14   bh      Added POST Time output, authentication checks
 10/20/14   bh      Fix PcdFirmwareVendor to be widechar when adding to ST
 10/24/14   bh      Initial branch from ArmPkg with edits

=============================================================================*/

#include "QcomBdsInternal.h"
#include <Library/PlatformBdsLib.h>
#include <Library/GenericBdsLib.h>
#include <Library/QcomBdsLib.h>
#include <Library/PcdLib.h>
#include <Library/PerformanceLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/BlockIo.h>
#include <Protocol/Bds.h>
#include <Library/QcomLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/UefiCfgLib.h>
#include <Protocol/EFIUsbConfig.h>
#include <Protocol/PciIo.h>
#include <Protocol/UsbIo.h>
#include <Library/QcomTargetLib.h>
#include <Library/FuseControlLib.h>
#include <Library/HobLib.h>
#include <Protocol/EFIScm.h>
#include <Include/scm_sip_interface.h>
#include <Library/BdsTargetLib.h>

#define BOOT_REMOVABLE_FIRST      TRUE
#define BOOT_NON_REMOVABLE_FIRST  FALSE
#define ENUMERATE_AND_BOOT        TRUE
#define ONLY_ENUMERATE            FALSE
#define CONTINUE_ON_FAIL          FALSE
#define HALT_ON_FAIL              TRUE
#define BDS_ENUM_POLL_PERIOD_MS   200

EFI_HANDLE  mImageHandle;
UINT8       ConnectControllersDone;

typedef enum {
  RemovableMediaOnly = 0,
  NonRemovableMediaOnly,
  AllMediaTypes,
  UsbMediaOnly
} ENUMERATION_FILTER;

extern EFI_GUID gOSAVendorGuid;
extern EFI_GUID gQcomFwVersionStringGuid;

extern VOID DisplayPOSTTime( VOID );
EFI_STATUS EnumerateAndBoot (ENUMERATION_FILTER, BOOLEAN, BOOLEAN);
VOID FreeListEntries(LIST_ENTRY * );
STATIC VOID AttemptBootFromRemovable(ENUMERATION_FILTER, BOOLEAN);
STATIC BOOLEAN                    EnumerateAndBootPrintFlag = TRUE;
STATIC BOOLEAN                    EnumerationBootUSBFailFlag = TRUE;
/* Check if full device path for a USB Class or WWID device path node
   
   @param    DevicePath     Device path that is checked for USB dependency
   @return   TRUE       If the DevicePathType and DevicePathSubType matches
             FALSE      Otherwise
*/
BOOLEAN
BdsHasUsbDevicePath (
  IN  EFI_DEVICE_PATH*  DevicePath
  )
{
  EFI_DEVICE_PATH *TempDevicePath;
  if (DevicePath == NULL)
    return FALSE;

  TempDevicePath = DevicePath;
  while (!IsDevicePathEnd(TempDevicePath))
  {
    if ((DevicePathType (TempDevicePath) == MESSAGING_DEVICE_PATH) &&
          ((DevicePathSubType (TempDevicePath) == MSG_USB_CLASS_DP) ||
           (DevicePathSubType (TempDevicePath) == MSG_USB_WWID_DP) ||
           (DevicePathSubType (TempDevicePath) == MSG_USB_DP)))
    {
      return TRUE;
    }
    TempDevicePath = NextDevicePathNode(TempDevicePath);
  }

  return FALSE;
}

/**
  Removes a Boot Option from the BootOptionsList, 
  and deletes from BootOrder if the Option is invalid

  @param     BootOptionsList     Linked list of boot options
**/
VOID
RemoveOptionFromBootOrder (LIST_ENTRY * BootOptionsList, LIST_ENTRY * Entry)
{
  DEBUG((EFI_D_WARN, "Removing from BootOrder\n"));
  BDS_LOAD_OPTION_ENTRY *BootOptionEntry;

  BootOptionEntry = LOAD_OPTION_ENTRY_FROM_LINK(Entry);

  if (!IsListEmpty(&BootOptionEntry->Link)) {
    RemoveEntryList(&BootOptionEntry->Link);
  }
  //Following line for persistent boot variables only 
  BootOptionDelete(BootOptionEntry->BdsLoadOption);
}

UINT32 GetBootUsbFirstDelayValue(VOID)
{
  EFI_STATUS  Status = EFI_SUCCESS;
  INT8        DelayValue;
  CONST UINT8 DefaultDelayVal = 30;
  UINTN       VarSize = sizeof(DelayValue);

  // Query BootUsbFirstDelayValue variable
  Status = gRT->GetVariable(L"BootUsbFirstDelayValue",
                            &gQcomTokenSpaceGuid,
                            NULL,
                            &VarSize,
                            &DelayValue);
  if (EFI_ERROR(Status))
    DelayValue = DefaultDelayVal;

  return (UINT32) DelayValue;
}
  

/* Puts the actual device path into the BootOption instead of stock USB class device path 
   
   @param     BootOption     The BootOption that has a Usb Class or WWID generic device path
   @return    EFI_SUCCESS    A valid USB device path is found and put into BootOption
              EFI_NOT_FOUND  No valid USB device is connected
 * */
EFI_STATUS
GetValidUsbDevicePath(BDS_LOAD_OPTION * BootOption){
  EFI_STATUS        Status;
  UINTN             HandleCount;
  EFI_HANDLE       *HandleBuffer;
  UINTN             Index;
  EFI_DEVICE_PATH  *DevicePath;
  UINT8             DevicePathSet;
  EFI_HANDLE        Handle;
  UINTN             Progress = 0;
  UINT32            DelayValue = 0;

  DevicePath = NULL;
  DevicePathSet = FALSE;
  //If device path is already valid, use BootOption as is
  Handle = BdsLibGetBootableHandle(BootOption->FilePathList);
  if (Handle != NULL)
    return EFI_SUCCESS;

  DelayValue = GetBootUsbFirstDelayValue() * 1000;
  while ((Progress < DelayValue) && (!DevicePathSet))
  {
  UINT64 Start =0,End =0;
  Start = GetTimerCountms ();
  DEBUG((EFI_D_INFO, "GetValidUsbDevicePath start %d:\n", Start));   
    
    Status = gBS->LocateHandleBuffer (ByProtocol, 
                                      &gEfiUsbIoProtocolGuid, 
                                      NULL, 
                                      &HandleCount, 
                                      &HandleBuffer);
    if (EFI_ERROR (Status)) 
      return Status;

    for (Index = 0; Index < HandleCount; Index++) {
      DevicePath = DevicePathFromHandle(HandleBuffer[Index]);
      if (DevicePath == NULL) {
        DEBUG((EFI_D_WARN, "Could not get device path \n"));
        continue;
      }
      DEBUG((EFI_D_INFO, "Handle DevPath: %s\n", DevicePathToStr(DevicePath)));
      Handle = BdsLibGetBootableHandle(DevicePath);
      if (Handle == NULL){
        DEBUG((EFI_D_ERROR, "Could not find bootable handle\n"));
        continue;
      }
      DEBUG((EFI_D_ERROR, "USB path found\n"));
      BootOption->FilePathList = DevicePath;
      BootOption->FilePathListLength = GetUnalignedDevicePathSize (DevicePath);
      DevicePathSet = TRUE;
      break;
    }

    DEBUG((EFI_D_INFO, "BootOption DevPath: %s\n", DevicePathToStr(BootOption->FilePathList)));
    if (DevicePathSet)
      return EFI_SUCCESS;

    End = GetTimerCountms ();
  DEBUG((EFI_D_INFO, "GetValidUsbDevicePath end %d:\n", End)); 
          
  
    DEBUG((EFI_D_WARN, "."));
    WaitForTimeout(BDS_ENUM_POLL_PERIOD_MS, 0, NULL);
  if(End>=Start)
  { 
    Progress += BDS_ENUM_POLL_PERIOD_MS +(End-Start) ;
  }
    else
  {
    Progress += BDS_ENUM_POLL_PERIOD_MS;
  }   
  }
  return EFI_NOT_FOUND;
}

/*
  This function goes through the BootOrderList and
  tries to boot each option. 

  @param    BootOrderList    The populated list that contains boot options
  @return   EFI_NOT_FOUND    When the BootOrderList does not have any valid options
            EFI_ABORTED      All of the options in BootOrderList returned
*/
EFI_STATUS
BootFromBootOptionList (LIST_ENTRY * BootOrderList)
{
  BDS_LOAD_OPTION*                  BootOption;
  LIST_ENTRY*                       Entry;
  BOOLEAN                           ValidOption;
  UINTN                             BootOrderIndex;
  EFI_STATUS                        Status;

  Status = EFI_NOT_FOUND;
  ValidOption = FALSE;
  BootOrderIndex = 0;
 
  for (Entry = GetFirstNode(BootOrderList);
       !IsNull (BootOrderList, Entry);
       Entry = GetNextNode(BootOrderList, Entry) )
  {
    BootOption = LOAD_OPTION_FROM_LINK(Entry);
   
    if (!BdsLibIsValidEFIBootOptDevicePath(BootOption->FilePathList, TRUE)){
      DEBUG ((EFI_D_ERROR, "Invalid device path\n"));
      RemoveOptionFromBootOrder(BootOrderList, Entry);
      continue;
    }
    DEBUG ((EFI_D_WARN, "Booting option %d:(Boot%04X) \"%s\"\n", BootOrderIndex, BootOption->LoadOptionIndex, BootOption->Description));
    ValidOption = TRUE;
    EfiSignalEventReadyToBoot();
    DisplayPOSTTime();
    if (BdsHasUsbDevicePath(BootOption->FilePathList)){
      Status = GetValidUsbDevicePath(BootOption);
      if (EFI_ERROR(Status)) {
        DEBUG((EFI_D_WARN, "USB device not found\n"));
        continue;
      }
    }
    Status = BootOptionStart(BootOption);
    DEBUG((EFI_D_INFO, "Boot option returned: %x\n", Status));
    if (Status == EFI_SECURITY_VIOLATION)
      DEBUG((EFI_D_ERROR, "Authentication failed\n"));
    if (Status == EFI_NOT_FOUND)
      ValidOption = FALSE;

    BootOrderIndex++;
  }

  if (!ValidOption)
  {
    DEBUG((EFI_D_WARN, "No valid options found in list\n"));
    return EFI_NOT_FOUND;
  }
  return EFI_ABORTED;
}

/*
  This function enumerates boot options based on parameters
  and adds valid ones to BootOptionsList 

  @param   BootOptionsList    Head of boot option linked list to populate
  @param   Filter             Filters type of Block IO handles to consider
  @param   AddRemovableFirst  Specifies the order in which we attempt to boot options

  @return  EFI_SUCCESS        Enumeration was successful, at least one option was found
  @return  EFI_NOT_FOUND      No bootable options were found during enumeration
*/
EFI_STATUS
EnumerateOptionsAndAdd (
   LIST_ENTRY* BootOptionsList, 
   ENUMERATION_FILTER Filter, 
   BOOLEAN AddRemovableFirst
   )
{
  EFI_STATUS                        Status;
  UINTN                             HandleCount;
  EFI_HANDLE                        *HandleBuffer = NULL;
  UINTN                             Index;
  EFI_DEVICE_PATH_PROTOCOL          *DevicePath;
  EFI_BLOCK_IO_PROTOCOL             *BlkIo;
  CHAR16                            BootDescription[BOOT_DEVICE_DESCRIPTION_MAX];
  UINT32                            Attributes;
  BDS_LOAD_OPTION_ENTRY             *BdsLoadOptionEntry;
  ARM_BDS_LOADER_TYPE               BootType;
  BOOLEAN                           LoadedOption;
  if (!ConnectControllersDone)
    BdsConnectAllDrivers(); 
 
  if (Filter == AllMediaTypes)
    DEBUG((EFI_D_WARN, "[QcomBds] Enumerating all boot options\n"));
  else if (Filter == RemovableMediaOnly)
  {
    if(EnumerateAndBootPrintFlag)
    {
      EnumerateAndBootPrintFlag = FALSE;
      DEBUG((EFI_D_ERROR, "[QcomBds] Enumerating removable boot options\n"));
    }
  }
  else if (Filter == NonRemovableMediaOnly)
    DEBUG((EFI_D_WARN, "[QcomBds] Enumerating non-removable boot options\n"));
  else if (Filter == UsbMediaOnly)
    DEBUG((EFI_D_WARN, "[QcomBds] Enumerating USB media boot options\n"));
  
  Attributes = 0;
  BootType = BDS_LOADER_EFI_APPLICATION;
  Status = EFI_NOT_FOUND;
  LoadedOption = FALSE;

  // List all the Simple File System Protocols
  Status = gBS->LocateHandleBuffer (ByProtocol, 
                                    &gEfiSimpleFileSystemProtocolGuid, 
                                    NULL, 
                                    &HandleCount, 
                                    &HandleBuffer);
  if (EFI_ERROR (Status)) {
    if (HandleBuffer != NULL)
      FreePool (HandleBuffer);

    return Status;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                      HandleBuffer[Index],
                      &gEfiBlockIoProtocolGuid,
                      (VOID **) &BlkIo
                      );

    if (BlkIo == NULL)
    {
      //TODO: This would need to change to enable booting from FV
      continue;
    }
    
    DevicePath = DevicePathFromHandle(HandleBuffer[Index]);
    if (DevicePath == NULL) {
      DEBUG((EFI_D_WARN, "[QcomBds] Could not get device path\n"));
      continue;
    }

    if ((Filter == UsbMediaOnly) && (!BdsHasUsbDevicePath(DevicePath)))
    {
      //only want to enumerate USB media
      continue;
    }
      
    else if ((Filter == RemovableMediaOnly) && (!BlkIo->Media->RemovableMedia))
    {
      //only want to enumerate removable media 
      continue;
    }
    else if ((Filter == NonRemovableMediaOnly) && BlkIo->Media->RemovableMedia) 
    {
      //only want to enumerate non-removable media
      continue;
    }

    if (BdsLibIsValidEFIBootOptDevicePath(DevicePath, TRUE))
    {
      if (BdsHasUsbDevicePath(DevicePath))
        UnicodeSPrint(BootDescription, BOOT_DEVICE_DESCRIPTION_MAX, L"USB Media");
      
      else if(BlkIo->Media->RemovableMedia)
        UnicodeSPrint(BootDescription, BOOT_DEVICE_DESCRIPTION_MAX, L"Removable Media");
      
      else
        UnicodeSPrint(BootDescription, BOOT_DEVICE_DESCRIPTION_MAX, L"Non-removable Media");

      BdsLoadOptionEntry = (BDS_LOAD_OPTION_ENTRY*) AllocatePool(sizeof(BDS_LOAD_OPTION_ENTRY));
      if (BdsLoadOptionEntry == NULL) {
        DEBUG((EFI_D_ERROR, "Could not allocate resource for boot option\n"));
        continue;
      }
      Status = BootOptionCreate(Attributes, BootDescription, DevicePath, BootType, NULL, &BdsLoadOptionEntry->BdsLoadOption);
      if (!EFI_ERROR(Status))
      {
        DEBUG((EFI_D_WARN, "[QcomBds] Adding new boot/driver option Boot%04X, Description: %s\n", BdsLoadOptionEntry->BdsLoadOption->LoadOptionIndex ,BdsLoadOptionEntry->BdsLoadOption->Description));
        LoadedOption = TRUE;
        if (BlkIo->Media->RemovableMedia == AddRemovableFirst)
        {
          InsertHeadList(BootOptionsList, &BdsLoadOptionEntry->Link);
        }
        else {
          InsertTailList(BootOptionsList, &BdsLoadOptionEntry->Link);
        }
      }
    }
  }

  if (HandleBuffer != NULL)
    FreePool (HandleBuffer);    

  if (LoadedOption){
    return EFI_SUCCESS;
  }
  else {
    return EFI_NOT_FOUND;
  }
}

/* 
  Updates the Firmware Vendor in 
  the System Table and fixes CRC
*/
STATIC
EFI_STATUS
UpdateFWVendorInST(VOID)
{
  UINTN       Size;
  EFI_STATUS  Status;
  //
  // Declare the Firmware Vendor
  //
  if (FixedPcdGetPtr(PcdFirmwareVendor) != NULL) {
    Size = 0x100;
    gST->FirmwareVendor = AllocateRuntimePool (Size);
    ASSERT (gST->FirmwareVendor != NULL);
    if (gST->FirmwareVendor == NULL) {
      DEBUG((EFI_D_ERROR, "Could not update Firmware Vendor in System Table\n"));
      return EFI_OUT_OF_RESOURCES;
    }
    UnicodeSPrint (gST->FirmwareVendor, Size, L"%S", PcdGetPtr(PcdFirmwareVendor));
  }

  //
  // Fixup Table CRC after we updated Firmware Vendor
  //
  gST->Hdr.CRC32 = 0;
  Status = gBS->CalculateCrc32 ((VOID*)gST, gST->Hdr.HeaderSize, &gST->Hdr.CRC32);
  return Status;
}


/*Enumerates based on Filter and attempts to boot */
STATIC
VOID AttemptBootFromRemovable(
  ENUMERATION_FILTER Filter, 
  BOOLEAN HaltOnEnumFail
  )
{
  EFI_STATUS Status;
  UINT32     DelayValue = 0;
  UINTN      Progress = 0;

  DEBUG((EFI_D_ERROR, "[QcomBds] Removable boot path\n"));

  SendEventDetectSdCard();
  BdsConnectAllDrivers();

  DelayValue = GetBootUsbFirstDelayValue() * 1000;
  while (Progress < DelayValue)
  {
    //last try for USB boot
    if(Progress + BDS_ENUM_POLL_PERIOD_MS >= DelayValue)
    {
      EnumerationBootUSBFailFlag = FALSE;
    }
    UINT64 Start =0,End =0;
  Start = GetTimerCountms ();
  DEBUG((EFI_D_INFO, "EnumerateAndBoot USB USB start %d:\n", Start));     
    
    Status = EnumerateAndBoot(Filter, BOOT_REMOVABLE_FIRST, HaltOnEnumFail);
    if (Status == EFI_ABORTED)
    {
      //Application found and launched, but did not go to HLOS
      //In this case, we don't want to loop here and keep trying this option
      break;
    }
    End = GetTimerCountms ();
    DEBUG((EFI_D_INFO, " EnumerateAndBoot USB end %d:\n", End));
    WaitForTimeout(BDS_ENUM_POLL_PERIOD_MS, 0, NULL);
  Progress += BDS_ENUM_POLL_PERIOD_MS + (End-Start);
   }
}

/* Checks the BootNext variable and tries to boot said option */
STATIC
VOID  AttemptBootNext(VOID)
{
  UINT16    *BootNext;
  UINTN      BootNextSize;
  CHAR16     BootVariableName[9];
  EFI_STATUS Status;
  
  // If BootNext environment variable is defined then we just load it !
  BootNextSize = sizeof(UINT16);
  Status = GetGlobalEnvironmentVariable (L"BootNext", NULL, &BootNextSize, (VOID**)&BootNext);
  if (!EFI_ERROR(Status) && (BootNext != NULL)) {
    ASSERT(BootNextSize == sizeof(UINT16));

    //Delete BootNext variable before launch, per spec
    gRT->SetVariable (L"BootNext", 
                      &gEfiGlobalVariableGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                      0, 
                      NULL);

    // Generate the requested Boot Entry variable name
    UnicodeSPrint (BootVariableName, 9 * sizeof(CHAR16), L"Boot%04X", *BootNext);

    // Set BootCurrent variable
    gRT->SetVariable (L"BootCurrent", &gEfiGlobalVariableGuid,
              EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
              BootNextSize, BootNext);

    FreePool (BootNext);

    // Start the requested Boot Entry
    Status = BdsLaunchBootOption (BootVariableName);
    if (Status != EFI_NOT_FOUND) {
      // BootNext has not been succeeded launched
      if (EFI_ERROR(Status)) {
        DEBUG((EFI_D_WARN, "Fail to start BootNext.\n"));
      }

      // Delete the BootNext environment variable
      gRT->SetVariable (L"BootNext", &gEfiGlobalVariableGuid,
          EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
          0, NULL);
    }

    // Clear BootCurrent variable
    gRT->SetVariable (L"BootCurrent", &gEfiGlobalVariableGuid,
        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
        0, NULL);
  }
}

/**
  Frees the memory used by the list after it is no longer needed

  @param BootList   The list that needs to be cleared. 

**/
VOID FreeListEntries(LIST_ENTRY * BootList){
  LIST_ENTRY *Entry;
  LIST_ENTRY *TempEntry;
  
  if (BootList == NULL)
    return;
  
  Entry = GetFirstNode(BootList);
  while (!IsListEmpty(BootList) && !IsNull (BootList, Entry)) 
  {
    TempEntry = Entry;
    Entry = RemoveEntryList(Entry);
    FreePool(TempEntry);
  }  
}

/**
  This function enumerates all the boot options and tries
  to boot each option. If it fails to find options or all
  found options fail to launch or are otherwise invalid,
  the system halts and launches shell if PRODMODE is set,
  otherwise shuts down
*/
EFI_STATUS EnumerateAndBoot (
   ENUMERATION_FILTER Filter, 
   BOOLEAN AddRemovableFirst,
   BOOLEAN HaltOnEnumFail
   )
{
  LIST_ENTRY BootOptionsList;
  EFI_STATUS Status;
  
  InitializeListHead(&BootOptionsList);
  /* Enumerate the possible boot options that can be added to bootorder, but don't store them */
  Status = EnumerateOptionsAndAdd(&BootOptionsList, Filter, AddRemovableFirst);
  if (EFI_ERROR(Status))
  {
      if ( EnumerationBootUSBFailFlag )
      {
        DEBUG((EFI_D_ERROR, "."));
      }
      else
      {
        DEBUG((EFI_D_ERROR, "[QcomBds] Enumeration failed\n"));
      }

    FreeListEntries(&BootOptionsList);
    if (HaltOnEnumFail)
      PlatformBdsBootHalt();
  }
  else {

    Status = BootFromBootOptionList(&BootOptionsList);
    if (EFI_ERROR(Status))
    {
      DEBUG((EFI_D_WARN, "[QcomBds] All options failed\n"));
      FreeListEntries(&BootOptionsList);
      if (HaltOnEnumFail)
        PlatformBdsBootHalt();
    }
  } 
  
  return Status;
}

EFI_STATUS
GetAblLoadAddr (UINTN * Addr)
{
  EFI_STATUS Status;
  QCOM_SCM_PROTOCOL *pQcomScmProtocol = NULL;
  UINT64 Parameters[SCM_MAX_NUM_PARAMETERS] = {0};
  UINT64 Results[SCM_MAX_NUM_RESULTS]       = {0};
  tz_get_ablfv_entry_point_rsp_t *SysCallRsp = (tz_get_ablfv_entry_point_rsp_t*) Results;
  

  if (Addr == NULL)
    return EFI_INVALID_PARAMETER;

  Status = OverrideAblLoadAddr(Addr);
  if (Status == EFI_SUCCESS)
    return Status;

  Status = gBS->LocateProtocol (&gQcomScmProtocolGuid,
                                NULL,
                                (VOID **) &pQcomScmProtocol);

  if (EFI_ERROR(Status) || (pQcomScmProtocol == NULL))
  {
    DEBUG((EFI_D_WARN, "Locate SCM protocol failed, Status: %r\n", Status));
    return Status;
  }

  Status = pQcomScmProtocol->ScmSipSysCall (pQcomScmProtocol,
                                            TZ_GET_IMAGE_ENTRY_POINT_ID,
                                            TZ_GET_IMAGE_ENTRY_POINT_PARAM_ID,
                                            Parameters,
                                            Results);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_WARN, "ScmSipSysCall() failed, Status: %r\n", Status));
    return Status;
  }
  
  *Addr = SysCallRsp->e_entry;

  DEBUG((EFI_D_INFO, "ABL load addr: 0x%x\n", *Addr));

  return EFI_SUCCESS;
}

#define APP_STR_LEN 128
EFI_STATUS
ProcessFvLoading (VOID)
{
  EFI_STATUS Status;
  UINTN      FvLoadAddr = 0;
  CHAR8      AppToLoad[APP_STR_LEN];
  UINTN      Size = sizeof (AppToLoad);

  Status = GetConfigString("DefaultBDSBootApp", AppToLoad, &Size);
  if (Status == EFI_SUCCESS)
  {
    Status = GetAblLoadAddr(&FvLoadAddr);
    if ((Status == EFI_SUCCESS) && (FvLoadAddr != 0))
    {
      Status = ProcessFvImageFile ((VOID*)((UINTN)FvLoadAddr));
      if (Status != EFI_SUCCESS)
      {
        DEBUG((EFI_D_ERROR, "[QcomBds] Failed to load FV !\n"));
        return EFI_NOT_FOUND;
      }
    }
  }

 return EFI_NOT_FOUND; 
}

/**
  This function goes through the following boot rules to choose an option
  to load. 
 
  RULES FOR BOOTING IN BDS
  Volume Down hotkey pressed:
   - Enumerate and boot from removable media
  BootOrder variable is not set:
    - Enumerate all boot options
    - Boot removable media first, then non-removable
  BootOrder variable is set 
    Has valid options:
     - Try booting those options
    NO valid options: 
     - Enumerate all boot options
     - Boot removable media first, then non-removable
    Applications fail authentication or otherwise return
     - Enumerate all boot options and boot if possible
  
  Enumeration fails or finds no valid options
   - Drop into Shell in Non-prod mode, then ask user for input to shut down

  BootOrder variable is not set, only deleted when invalid

  @param  This             The EFI_BDS_ARCH_PROTOCOL instance.

  @return None.

**/
VOID
EFIAPI
BdsEntry (
  IN EFI_BDS_ARCH_PROTOCOL  *This
  )
{
  EFI_STATUS          Status;
  LIST_ENTRY          BootOrderList;
  BDS_INIT_OPTION     InitOption;
  UINT8               USBFirst = 0;
  UINTN               VarSize = sizeof(USBFirst);
  
  ConnectControllersDone = FALSE;

  ProcessFvLoading ();  

  PERF_END   (NULL, "DXE", NULL, 0);

  Status = UpdateFWVendorInST();
  ASSERT_EFI_ERROR(Status);

  PlatformBdsInitEx(&InitOption);

  Status = gRT->GetVariable(L"AttemptUSBFirst", &gOSAVendorGuid,
                            NULL, &VarSize, &USBFirst);
  if (EFI_ERROR(Status))
    USBFirst = 0;
  
  if ((InitOption == BootFromRemovableMedia) || USBFirst)
    AttemptBootFromRemovable(RemovableMediaOnly, CONTINUE_ON_FAIL);

  AttemptBootNext();

  //Blindly copy the BootOrder variable's associated BootOption's into the List structure
  Status = BuildBootOptionListFromBootOrder(&BootOrderList);
  if (!EFI_ERROR(Status))
  {
    //BootOrder has been parsed into list, try booting these options
    DEBUG((EFI_D_INFO, "[QcomBds] Attempting BootOrder entries\n"));
    Status = BootFromBootOptionList(&BootOrderList);
    if (EFI_ERROR(Status)) 
    { 
      //None of the boot order variables were valid, enumerate all other options
      DEBUG((EFI_D_WARN, "[QcomBds] BootOrder failed"));
    }
  }
  else
  {
    //BootOrder variable does not exist, or GetVariable otherwise failed
    DEBUG((EFI_D_WARN, "[QcomBds] BootOrder not found\n"));
  }
  
  //Enumerate Non-removable options
  EnumerateAndBoot(NonRemovableMediaOnly, BOOT_NON_REMOVABLE_FIRST, CONTINUE_ON_FAIL);

  //If all other options are exhausted, fall back on removable boot
  AttemptBootFromRemovable(RemovableMediaOnly, CONTINUE_ON_FAIL);

  //Shell or shutdown if all else fails
  PlatformBdsBootHalt();
    
  //Should halt before getting here
  ASSERT(FALSE);
}

EFI_BDS_ARCH_PROTOCOL  gBdsProtocol = {
  BdsEntry,
};

EFI_STATUS SetFwVersionStrVar (VOID)
{
  EFI_STATUS Status;
  CHAR8 *FwVerStr;
  EFI_HOB_GUID_TYPE *GuidHob; 

  //Get FW version string from HOB
  GuidHob = GetFirstGuidHob(&gQcomFwVersionStringGuid);
  if (GuidHob == NULL)
  {
    DEBUG((EFI_D_INFO, "FW version string HOB not found\n"));
    return EFI_NOT_FOUND;
  }

  FwVerStr = GET_GUID_HOB_DATA(GuidHob);
  if (FwVerStr == NULL)
  {
    DEBUG((EFI_D_INFO, "FW Version string not found in HOB\n"));
    return EFI_NOT_FOUND;
  }

  Status = gRT->SetVariable(L"FwVerStr", &gQcomTokenSpaceGuid,
                            EFI_VARIABLE_BOOTSERVICE_ACCESS, AsciiStrSize(FwVerStr), FwVerStr); 

  return Status;
}

/* Initializes BDS driver */
EFI_STATUS
EFIAPI
BdsInitialize (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
  )
{
  EFI_STATUS Status;

  mImageHandle = ImageHandle;
  
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gEfiBdsArchProtocolGuid, &gBdsProtocol,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
  
  Status = SetFwVersionStrVar();
  if (EFI_ERROR(Status))
    DEBUG((EFI_D_WARN, "Could not set firmware version string, status: %r\n", Status));

  return EFI_SUCCESS;
}
