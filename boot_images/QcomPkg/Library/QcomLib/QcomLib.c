/** @file QcomLib.c

  Contains generic Qualcomm library functions.

  Copyright (c) 2010-2019, Qualcomm Technologies Incorporated.
  All rights reserved.
  Portions Copyright (c) 2009, Apple Inc. All rights reserved.<BR>

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
 03/29/19   sr      return success if file system already mounted  
 08/06/18   yps     Added more platform strings
 09/22/17   c_vkuppa Fix NULL check
 01/17/17   lg      Update smbios processor name string (remove chip ID string)
 12/07/16   lg      Fix GetUUID() to fill smbios data with a hash when product 
					serial num is more than 16 bytes
 09/13/16   jt      Change SdccCardInfo to MemCardInfo 
 09/01/16   bh      Do platform specific reset for EDL
 08/23/16   ai      Add support for connect controller on specific handles
 08/24/16   bh      Update timeout function to provide key info to caller
 08/18/16   bh      Add API to unmount FAT partition
 08/11/16   bh      Add EDL API
 08/10/16   va      Caller to print debug msg for time out if required
 07/05/16   bh      Add partition label parameter for file system APIs
 06/22/16   bh      Check before mounting debug partitions in retail case
 06/15/16   bd      Remove offlinecrashdump library dependency from QcomLib
 06/08/16   sj      removed the mass storage cookie references
 03/07/16   ml      Moved bitmap process to separate file.
 03/04/16   bh      Allow fat mounting by partition type also
 02/08/16   vk      Remove GetOsType()
 01/13/15   bh      Allow for null root device type to retrieve block io handles
 11/19/15   va      Added WaitForTimeoutNoKey function
 10/14/15   vk      Add NULL pointer checks
 06/25/15   bh      Add selection by parition label and function to mount fat parti
 05/15/15   sm      Added LoadFileFromVolume
 04/23/15   vk      Add UFS support
 03/06/15   jb      Allow partition type to be exposed as protocol per UEFI spec
 12/12/14   vk      Remove API
 12/10/14   bh      Removed strncpy banned API
 12/09/14   na      Return from RenderBGRTImage() if graphics protocol does not exist
 12/02/14   bh      Add WaitForTimeout function
 10/28/14   ao      update PlatformInfo list
 10/22/14   aus     Added support for EDL mode
 10/16/14   vk      LoadFileFromPartition signature update
 09/02/14   vk      KW fixes
 04/28/14   cpaulo  Updated copyright statement to use QTI
 04/22/14   aus     Use SetDloadCookie() API instead of modifying directly
 03/18/14   vk      Fix gcc warnings
 03/12/14   aus     Mass storage mode support
 01/13/14   vk      Sync to UEFI 2.1 relevant portions
 11/01/13   vk      Fix 64 bit compiler warnings
 04/06/13   vk      Marketing name in processor string
 04/11/13   ma      Rename KeypadDeviceGuid
 04/09/13   yg      Improve Mounted FS handle search
 04/04/13   yg      Fix to get native keypad protocol
 03/28/13   niting  Moved InCarveoutMode to OfflineCrashDumpLib
 03/07/13   sm      Added ReadFromFV API
 02/25/13   vk      Move GetOsType to UefiCfgLib function
 02/11/13   niting  Use UefiCfgLib for mass storage cookie address
 01/09/13   vk      Fix KlockWork warnings
 11/02/12   vk      Added DeleteFile
 11/09/12   jmb     Adding CBH platform
 09/17/12   yg      Redesigned ReadAnyKey API
 09/06/12   yg      Fix Paranthesis logic error
 08/10/12   yg      Added ReadAnyKey API
 07/18/12   jz      Added WriteFile()
 07/09/12   yg      Add FileSize Param & function HexStrToBin
 07/09/12   vishalo Remove MassStorage variable
 07/05/12   rsb     Added FlushVariableServices()
 06/11/12   jz      Added InCarveoutMode()
 06/11/12   vishalo Set MSM8960 v4.1 string as v3.2.1
 06/05/12   rs      Removing 'static' from RenderBGRTImage() and
                    ConversionPixelCopy() to enable other drivers to use it.
 05/11/12   plc     If platform subtype is not "UNKNOWN", concatenate that onto
                    end of platform string in GetPlatformStrings()
 05/02/12   vishalo Added License info
 04/26/12   vishalo Add GetOsType()
 04/24/12   yg      Some debug messages cleanup
 04/19/12   leungm  Added support for BGRT
 04/13/12   jz      Added GetProcessorName()
 04/2/12    kpa     Added String manipulation and File read api.
 03/21/12   shilpar Added API to read from a file to a buffer
 03/15/12   jz      Set variable to indicate mass storage mode enabled
 01/26/12   yg      Add new API to get Platform strings.
 12/15/11   yg      Add new API to get Platform type.
 11/15/11   vishalo Run image from specified partition
 08/22/11   yg      Get Partition type for filtering
 08/16/11   yg      Added BlockIo Select API
 07/19/11   pbitra  Added strncpy definition
 07/08/11   niting  Added mass storage support

=============================================================================*/

#include <Uefi.h>

#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/QcomLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/PerformanceLib.h>
#include <Library/DevicePathLib.h>
#include <Library/QcomBaseLib.h>
#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Protocol/BlockIo.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/DevicePath.h>
#include <Protocol/EFIChipInfo.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimpleTextOut.h>
#include <Protocol/SerialIo.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/EmbeddedDevice.h>
#include <Protocol/EFIUsbIo.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/EFIVariableServices.h>
#include <Guid/FileSystemInfo.h>
#include <Guid/FileInfo.h>
#include <Library/HobLib.h>
#include <Library/UefiCfgLib.h>
#include <Library/OfflineCrashDump.h>
#include <Library/DxeServicesTableLib.h>
#include <Protocol/EFICardInfo.h>
#include <Library/BootConfig.h>
#include <Library/DxeServicesLib.h>
#include "string.h"
#include "DDIPlatformInfo.h"
#include <Library/QcomTargetLib.h>
#include <Library/CookieLib.h>
#include <Protocol/Hash2.h>


/* Volume Label size 11 chars, round off to 16 */
#define VOLUME_LABEL_SIZE      16


/* List of all the filters that need device path protocol in the handle to filter */
#define FILTERS_NEEDING_DEVICEPATH   (BLK_IO_SEL_PARTITIONED_MBR | \
                                      BLK_IO_SEL_PARTITIONED_GPT | \
                                      BLK_IO_SEL_MATCH_PARTITION_TYPE_GUID | \
                                      BLK_IO_SEL_SELECT_ROOT_DEVICE_ONLY  | \
                                      BLK_IO_SEL_MATCH_ROOT_DEVICE)

/* FileInfo-size = SIZE_OF_EFI_FILE_INFO + sizeof(name of directory entry)
    Since we don't know the sizeof(name of directory entry),
    we can set FileInfo-size = SIZE_OF_EFI_FILE_INFO + 256*/
#define FILE_INFO_SIZE (SIZE_OF_EFI_FILE_INFO + 256)

STATIC HandleInfo HandleInfoList[2];
STATIC PartiSelectFilter HandleFilter;
extern EFI_GUID gEfiEmmcUserPartitionGuid;
extern EFI_GUID gEfiFileInfoGuid;
extern EFI_GUID gQcomKeypadDeviceGuid;
extern EFI_GUID gQcomMemoryCaptureGuid;


#ifdef _MSC_VER
int   _fltused = 0x9875;
#endif
/* Local function prototype headers */
#ifdef _MSC_VER
#pragma warning( disable : 4146 4244)
#endif


CONST CHAR8 *PlatformTypeStrings[DALPLATFORMINFO_NUM_TYPES] =
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

EFI_STATUS
LoadPeCoffSectionFromFv (
 IN  EFI_HANDLE   FvHandle,
 IN  EFI_GUID     *NameGuid
 );

EFI_STATUS
FindApplicationMatchingUiSection (
  IN  CHAR16      *UiString,
  OUT EFI_HANDLE  *FvHandle,
  OUT EFI_GUID    *NameGuid
  );

/* Sleep without burning CPU cycles

   @param      Timeoutms    The number of milliseconds to wait
   @return     Status       Status of the WaitForEvent function
*/
EFI_STATUS
WaitForTimeout (IN UINT32             Timeoutms, 
                IN UINT32             WaitOptions, 
                IN OUT EFI_INPUT_KEY *UserKey OPTIONAL)
{
  UINTN           Index;
  EFI_STATUS      Status = EFI_NOT_READY;
  EFI_EVENT       TimerEvent;
  EFI_EVENT       WaitList[3];
  UINT64          TimeoutVal;
  EFI_INPUT_KEY   KeyVal;
  UINTN           NumEvents = 1;

  KeyVal.UnicodeChar = 0;
  KeyVal.ScanCode = 0;

  if (Timeoutms == 0)
    return EFI_INVALID_PARAMETER;

  /* Create a Timer event  */
  Status = gBS->CreateEvent (EVT_TIMER, TPL_CALLBACK, NULL, NULL, &TimerEvent);

  if (Status != EFI_SUCCESS)
    return Status;

  /* Convert to 100ns units */
  TimeoutVal = Timeoutms * 10000;

  /* Set Timer event */
  gBS->SetTimer (TimerEvent, TimerRelative, TimeoutVal);

  /* Wait for some event or the timer */
  WaitList[0] = TimerEvent;
  WaitList[1] = gST->ConIn->WaitForKey;
  /* Add events based on WaitOptions */
  if ( (WaitOptions && TIMEOUT_WAIT_FOR_KEY) != 0)
    NumEvents++;

  Status = gBS->WaitForEvent (NumEvents, WaitList, &Index);

  if (Index == 1)
  {
    gST->ConIn->ReadKeyStroke (gST->ConIn, &KeyVal);
    DEBUG((EFI_D_WARN, "key:%x,%x\n", KeyVal.UnicodeChar, KeyVal.ScanCode));
  }

  if (UserKey != NULL)
  {
    UserKey->UnicodeChar = KeyVal.UnicodeChar;
    UserKey->ScanCode = KeyVal.ScanCode;
  }

  gBS->CloseEvent (TimerEvent);

  return Status;
}


/* Sleep without burning CPU cycles

   @param      Timeoutms    The number of milliseconds to wait
   @return     Status       Status of the WaitForEvent function
*/
EFI_STATUS
WaitForTimeoutNoKey (IN UINT32   Timeoutms)
{
  UINTN           Index;
  EFI_STATUS      Status = EFI_NOT_READY;
  EFI_EVENT       TimerEvent;
  EFI_EVENT       WaitList[2];
  UINT64          TimeoutVal;
  UINTN           NumEvents = 1;

  if (Timeoutms == 0)
    return EFI_INVALID_PARAMETER;

  /* Create a Timer event  */
  Status = gBS->CreateEvent (EVT_TIMER, TPL_CALLBACK, NULL, NULL, &TimerEvent);

  if (Status != EFI_SUCCESS)
    return Status;

  /* Convert to 100ns units */
  TimeoutVal = Timeoutms * 10000;

  /* Set Timer event */
  gBS->SetTimer (TimerEvent, TimerRelative, TimeoutVal);

  /* Wait for some event or the timer */
  WaitList[0] = TimerEvent;

  Status = gBS->WaitForEvent (NumEvents, WaitList, &Index);

  DEBUG((EFI_D_WARN, "WaitforTimeout Completed \n"));

  gBS->CloseEvent (TimerEvent);

  return Status;
}

/**
 *  Load and execute image from FV.
 *
 * UiString: Application/image to be loaded.
 */



// This function takes name of the image file in FV and returns handle to it.
EFI_STATUS
FindApplicationMatchingUiSection (
  IN  CHAR16      *UiString,
  OUT EFI_HANDLE  *FvHandle,
  OUT EFI_GUID    *NameGuid
  )
{
  EFI_STATUS                    Status;
  EFI_STATUS                    NextStatus;
  UINTN                         NoHandles;
  EFI_HANDLE                    *Buffer = NULL;
  UINTN                         Index;
  EFI_FV_FILETYPE               FileType;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;
  VOID                          *Key;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINTN                         Size;
  UINTN                         UiStringLen;
  CHAR16                        *UiSection;
  UINT32                        Authentication;


  UiStringLen = 0;
  if (UiString != NULL) {
//    DEBUG ((DEBUG_ERROR, "UiString %s\n", UiString));
    UiStringLen = StrLen (UiString);
  }

  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiFirmwareVolume2ProtocolGuid, NULL, &NoHandles, &Buffer);
  if (!EFI_ERROR (Status)) {
    for (Index = 0; Index < NoHandles; Index++) {
      Status = gBS->HandleProtocol (Buffer[Index], &gEfiFirmwareVolume2ProtocolGuid, (VOID **)&Fv);
      if (!EFI_ERROR (Status)) {
        Key = AllocatePool (Fv->KeySize);
        ASSERT (Key != NULL);

        if (Key == NULL) //For KW, or if ASSERT is disabled
          return EFI_OUT_OF_RESOURCES;

        ZeroMem (Key, Fv->KeySize);

        FileType = EFI_FV_FILETYPE_APPLICATION;

        do {
          NextStatus = Fv->GetNextFile (Fv, Key, &FileType, NameGuid, &Attributes, &Size);
          if (!EFI_ERROR (NextStatus)) {
            if (UiString == NULL) {
              //
              // If UiString is NULL match first application we find.
              //
              *FvHandle = Buffer[Index];
              FreePool (Key);
              return Status;
            }

            UiSection = NULL;
            Status = Fv->ReadSection (
                          Fv,
                          NameGuid,
                          EFI_SECTION_USER_INTERFACE,
                          0,
                          (VOID **)&UiSection,
                          &Size,
                          &Authentication
                          );
            if (!EFI_ERROR (Status)) {
              if (StrnCmp (UiString, UiSection, UiStringLen) == 0) {
                //
                // We found a UiString match.
                //
                *FvHandle = Buffer[Index];
                FreePool (Key);
                FreePool (UiSection);
                return Status;
              }
              FreePool (UiSection);
            }
          }
        } while (!EFI_ERROR (NextStatus));

        FreePool (Key);
      }
    }

    FreePool (Buffer);
   }

  return EFI_NOT_FOUND;
}


EFI_DEVICE_PATH *
FvFileDevicePath (
  IN  EFI_HANDLE   FvHandle,
  IN  EFI_GUID     *NameGuid
  )
{
  EFI_DEVICE_PATH_PROTOCOL          *DevicePath;
  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH NewNode;

  DevicePath = DevicePathFromHandle (FvHandle);

  EfiInitializeFwVolDevicepathNode (&NewNode, NameGuid);

  return AppendDevicePathNode (DevicePath, (EFI_DEVICE_PATH_PROTOCOL *)&NewNode);
}


// Function to load and execute the image
EFI_STATUS
LoadPeCoffSectionFromFv (
 IN  EFI_HANDLE   FvHandle,
 IN  EFI_GUID     *NameGuid
 )
{
  EFI_STATUS                    Status;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  EFI_HANDLE                    ImageHandle;

  DevicePath = FvFileDevicePath (FvHandle, NameGuid);
  Status = gBS->LoadImage (TRUE, gImageHandle, DevicePath, NULL, 0, &ImageHandle);
  if (EFI_ERROR (Status))
    return Status;

  return gBS->StartImage (ImageHandle, NULL, NULL);
}


EFI_STATUS
LaunchFromFv(
IN  CHAR16      *UiString
)
{
  EFI_HANDLE                FvHandle;
  EFI_GUID                  NameGuid;
  EFI_STATUS                Status;

  Status = FindApplicationMatchingUiSection (UiString, &FvHandle, &NameGuid);
  if (EFI_ERROR (Status))
    return Status;

  Status = LoadPeCoffSectionFromFv (FvHandle, &NameGuid);
  return Status;
}



/**
  Returns a pointer to interface for EfiSimpleTextInProtocol,
  for which vendor guid is same as the guid published by keypad

  @retval KeypadProtocol if the operation was successful
 */

EFI_STATUS
EFIAPI
GetNativeKeypad (OUT  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL    **KeypadProtocol)
{
  UINTN                                Index;
  UINTN                                KeypadHandleCount;
  VENDOR_DEVICE_PATH                  *DevPathInst;
  VENDOR_DEVICE_PATH                  *RootDevicePath;
  EFI_HANDLE                          *KeypadHandles;
  EFI_STATUS                           Status;

  Index = 0;
  KeypadHandleCount   = 0;
  KeypadHandles  = NULL;
  DevPathInst = NULL;

  // Return all Handles for EfiSimpleTextInProtocol
  gBS->LocateHandleBuffer(ByProtocol,
                           &gEfiSimpleTextInProtocolGuid,
                           NULL,
                           &KeypadHandleCount,
                           &KeypadHandles);


   // loop through all handles
  for (Index = 0; Index < KeypadHandleCount; Index++)
  {
     Status = gBS->HandleProtocol (KeypadHandles[Index],
                                   &gEfiDevicePathProtocolGuid,
                                   (VOID **) &DevPathInst);

    if (EFI_ERROR (Status))
      continue;

    /* Get the device path */
    RootDevicePath = DevPathInst;
    if (RootDevicePath->Header.Type != HARDWARE_DEVICE_PATH ||
        RootDevicePath->Header.SubType != HW_VENDOR_DP ||
        ((RootDevicePath->Header.Length[0] | (RootDevicePath->Header.Length[1] << 8)) != sizeof(VENDOR_DEVICE_PATH)) ||
        CompareGuid (&gQcomKeypadDeviceGuid, &DevPathInst->Guid) == FALSE)
     continue;

     Status = gBS->HandleProtocol (
             KeypadHandles[Index],
             &gEfiSimpleTextInputExProtocolGuid,
             (VOID **) KeypadProtocol);
             break;
  }

  //Free the handle buffer
  if (KeypadHandles != NULL)
    FreePool(KeypadHandles);

  return EFI_SUCCESS;
}

/* Returns 0 if the volume label matches otherwise non zero */
UINTN
CompareVolumeLabel (IN EFI_SIMPLE_FILE_SYSTEM_PROTOCOL*   Fs,
                    IN CHAR8*                             ReqVolumeName)
{
  INT32 CmpResult;
  UINT32 j;
  UINT16 VolumeLabel[VOLUME_LABEL_SIZE];
  EFI_FILE_PROTOCOL  *FsVolume = NULL;
  EFI_STATUS         Status;
  UINTN                               Size;
  EFI_FILE_SYSTEM_INFO                *FsInfo;

  // Get information about the volume
  Status = Fs->OpenVolume (Fs, &FsVolume);

  if (Status != EFI_SUCCESS)
    return 1;

  /* Get the Volume name */
  Size = 0;
  FsInfo = NULL;
  Status = FsVolume->GetInfo (FsVolume, &gEfiFileSystemInfoGuid, &Size, FsInfo);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    FsInfo = AllocatePool (Size);
    Status = FsVolume->GetInfo (FsVolume, &gEfiFileSystemInfoGuid, &Size, FsInfo);
    if(Status != EFI_SUCCESS) {
      FreePool(FsInfo);
      FsInfo = NULL;
    }
  }

  if(FsInfo == NULL)
    return 1;

  /* Convert the passed in Volume name to Wide char and upper case */
  for (j = 0; (j < VOLUME_LABEL_SIZE - 1) && ReqVolumeName[j]; ++j)
  {
    VolumeLabel[j] = ReqVolumeName[j];

    if ((VolumeLabel[j] >= 'a') && (VolumeLabel[j] <= 'z'))
      VolumeLabel[j] -= ('a' - 'A');
  }
  /* Null termination */
  VolumeLabel[j] = 0;

  /* Change any lower chars in volume name to upper (ideally this is not needed) */
  for (j = 0; (j < VOLUME_LABEL_SIZE - 1) && FsInfo->VolumeLabel[j]; ++j)
  {
    if ((FsInfo->VolumeLabel[j] >= 'a') && (FsInfo->VolumeLabel[j] <= 'z'))
      FsInfo->VolumeLabel[j] -= ('a' - 'A');
  }
  CmpResult = StrnCmp (FsInfo->VolumeLabel, VolumeLabel, VOLUME_LABEL_SIZE);

  FreePool (FsInfo);
  FsVolume->Close (FsVolume);

  return CmpResult;
}

#define TO_UPPER(a)  ((((a) >= 'a') && ((a) <= 'z')) ? ((a) - 'a' + 'A') : (a))

/**
  Case insensitive unicode str comparison.
  Returns 0 if equal, difference in characters if not
**/
INTN
EFIAPI
StriCmp(
  IN CONST CHAR16   *FirstString,
  IN CONST CHAR16   *SecondString
  )
{
  CHAR16 UpperFirstString;
  CHAR16 UpperSecondString;

  if (FirstString == NULL)
    return -1;
  if (SecondString == NULL)
    return -1;

  if (StrSize(FirstString)== 0)
  {
    ASSERT (FALSE);
    return -1;
  }

  if (StrSize(SecondString) == 0)
  {
    ASSERT (FALSE);
    return -1;
  }

  if ((StrSize(FirstString) == 0) || (StrSize(SecondString) == 0))
    return -1;

  UpperFirstString = TO_UPPER(*FirstString);
  UpperSecondString = TO_UPPER(*SecondString);

  while ((*FirstString != L'\0') && (UpperFirstString == UpperSecondString))
  {
    FirstString++;
    SecondString++;
    UpperFirstString = TO_UPPER(*FirstString);
    UpperSecondString = TO_UPPER(*SecondString);
  }

  return UpperFirstString - UpperSecondString;
}

/**
  Returns a list of BlkIo handles based on required criteria
  SelectionAttrib : Bitmask representing the conditions that need
                    to be met for the handles returned. Based on the
                    selections filter members should have valid values.
  FilterData      : Instance of Partition Select Filter structure that
                    needs extended data for certain type flags. For example
                    Partition type and/or Volume name can be specified.
  HandleInfoPtr   : Pointer to array of HandleInfo structures in which the
                    output is returned.
  MaxBlkIopCnt    : On input, max number of handle structures the buffer can hold,
                    On output, the number of handle structures returned.

  @retval EFI_SUCCESS if the operation was successful
 */
EFI_STATUS
EFIAPI
GetBlkIOHandles (
  IN  UINT32                SelectionAttrib,
  IN  PartiSelectFilter     *FilterData,
  OUT HandleInfo            *HandleInfoPtr,
  IN OUT  UINT32*           MaxBlkIopCnt
  )
{
  EFI_BLOCK_IO_PROTOCOL               *BlkIo;
  EFI_HANDLE                          *BlkIoHandles;
  UINTN                               BlkIoHandleCount;
  UINTN                               i;
  UINTN                               DevicePathDepth;
  HARDDRIVE_DEVICE_PATH               *Partition, *PartitionOut;
  EFI_STATUS                          Status;
  EFI_DEVICE_PATH_PROTOCOL            *DevPathInst;
  EFI_DEVICE_PATH_PROTOCOL            *TempDevicePath;
  VENDOR_DEVICE_PATH                  *RootDevicePath;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL     *Fs;
  UINT32                              BlkIoCnt = 0;
  EFI_PARTITION_ENTRY                 *PartEntry;

  if ((MaxBlkIopCnt == 0) || (HandleInfoPtr == 0))
    return EFI_INVALID_PARAMETER;

  /* Adjust some defaults first */
  if ((SelectionAttrib & (BLK_IO_SEL_MEDIA_TYPE_REMOVABLE | BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE)) == 0)
    SelectionAttrib |= (BLK_IO_SEL_MEDIA_TYPE_REMOVABLE | BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE);

  if (((BLK_IO_SEL_PARTITIONED_GPT | BLK_IO_SEL_PARTITIONED_MBR) & SelectionAttrib) == 0)
    SelectionAttrib |= (BLK_IO_SEL_PARTITIONED_GPT | BLK_IO_SEL_PARTITIONED_MBR);

  /* If we need Filesystem handle then search based on that its narrower search than BlkIo */
  if (SelectionAttrib & (BLK_IO_SEL_SELECT_MOUNTED_FILESYSTEM | BLK_IO_SEL_SELECT_BY_VOLUME_NAME))
    gBS->LocateHandleBuffer (ByProtocol,
                             &gEfiSimpleFileSystemProtocolGuid,
                             NULL,
                             &BlkIoHandleCount,
                             &BlkIoHandles);
  else
    gBS->LocateHandleBuffer (ByProtocol,
                             &gEfiBlockIoProtocolGuid,
                             NULL,
                             &BlkIoHandleCount,
                             &BlkIoHandles);

  /* Loop through to search for the ones we are interested in. */
  for (i = 0; i < BlkIoHandleCount; i++){

    Status = gBS->HandleProtocol (BlkIoHandles[i],
                                  &gEfiBlockIoProtocolGuid,
                                  (VOID **) &BlkIo);
    /* Fv volumes will not support Blk I/O protocol */
    if (Status == EFI_UNSUPPORTED) {
      continue;
    }

    ASSERT_EFI_ERROR (Status);

    /* Check if the media type criteria (for removable/not) satisfies */
    if (BlkIo->Media->RemovableMedia)
    {
      if ((SelectionAttrib & BLK_IO_SEL_MEDIA_TYPE_REMOVABLE) == 0)
        continue;
    }
    else
    {
      if ((SelectionAttrib & BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE) == 0)
        continue;
    }

    /* Clear the pointer, we can get it if the filter is set */
    PartitionOut = 0;

    /* Check if partition related criteria satisfies */
    if ((SelectionAttrib & FILTERS_NEEDING_DEVICEPATH) != 0)
    {
      Status = gBS->HandleProtocol (BlkIoHandles[i],
                                    &gEfiDevicePathProtocolGuid,
                                    (VOID**)&DevPathInst);

      /* If we didn't get the DevicePath Protocol then this handle
       * cannot be used */
      if (EFI_ERROR (Status))
        continue;

      DevicePathDepth = 0;

      /* Get the device path */
      TempDevicePath = DevPathInst;
      RootDevicePath = (VENDOR_DEVICE_PATH*) DevPathInst;
      Partition = (HARDDRIVE_DEVICE_PATH*)TempDevicePath;

      if ((SelectionAttrib & (BLK_IO_SEL_SELECT_ROOT_DEVICE_ONLY  |
                              BLK_IO_SEL_MATCH_ROOT_DEVICE)) != 0)
      {
        if (!FilterData)
          return EFI_INVALID_PARAMETER;

        /* If this is not the root device that we are looking for, ignore this
         * handle */
        if (RootDevicePath->Header.Type != HARDWARE_DEVICE_PATH ||
            RootDevicePath->Header.SubType != HW_VENDOR_DP ||
            (RootDevicePath->Header.Length[0] | (RootDevicePath->Header.Length[1] << 8)) !=
               sizeof(VENDOR_DEVICE_PATH) ||
            ((FilterData->RootDeviceType != NULL) &&
             (CompareGuid (FilterData->RootDeviceType, &RootDevicePath->Guid) == FALSE)))
          continue;
      }

      /* Locate the last Device Path Node */
      while (!IsDevicePathEnd (TempDevicePath)) {
        DevicePathDepth++;
        Partition = (HARDDRIVE_DEVICE_PATH*)TempDevicePath;
        TempDevicePath = NextDevicePathNode (TempDevicePath);
      }

      /* If we need the handle for root device only and if this is representing
       * a sub partition in the root device then ignore this handle */
      if (SelectionAttrib & BLK_IO_SEL_SELECT_ROOT_DEVICE_ONLY)
        if (DevicePathDepth > 1)
          continue;

      /* Check if the last node is Harddrive Device path that contains the
       * Partition information */
      if (Partition->Header.Type == MEDIA_DEVICE_PATH  &&
          Partition->Header.SubType == MEDIA_HARDDRIVE_DP  &&
          (Partition->Header.Length[0] |
           (Partition->Header.Length[1] << 8)) == sizeof(*Partition))
      {
        PartitionOut = Partition;

        if ((SelectionAttrib & BLK_IO_SEL_PARTITIONED_GPT) == 0)
          if (Partition->MBRType == PARTITIONED_TYPE_GPT)
            continue;

        if ((SelectionAttrib & BLK_IO_SEL_PARTITIONED_MBR) == 0)
          if (Partition->MBRType == PARTITIONED_TYPE_MBR)
            continue;

        /* PartitionDxe implementation should return partition type also */
        if ((SelectionAttrib & BLK_IO_SEL_MATCH_PARTITION_TYPE_GUID) != 0)
        {
          GUID *PartiType;
          VOID *Interface;

          if (!FilterData || FilterData->PartitionType == 0)
            return EFI_INVALID_PARAMETER;

          Status = gBS->HandleProtocol (BlkIoHandles[i],
                                        FilterData->PartitionType,
                                        (VOID**)&Interface);
          if (EFI_ERROR(Status))
          {
            Status = gBS->HandleProtocol (BlkIoHandles[i],
                                          &gEfiPartitionTypeGuid,
                                          (VOID**)&PartiType);
            if(Status != EFI_SUCCESS)
              continue;

            if (CompareGuid (PartiType, FilterData->PartitionType) == FALSE)
              continue;
          }
        }
      }
      /* If we wanted a particular partition and didn't get the HDD DP,
         then this handle is probably not the interested ones */
      else if ((SelectionAttrib & BLK_IO_SEL_MATCH_PARTITION_TYPE_GUID) != 0)
          continue;
    }

    /* Check if the Filesystem related criteria satisfies */
    if ((SelectionAttrib & BLK_IO_SEL_SELECT_MOUNTED_FILESYSTEM) != 0)
    {
      Status = gBS->HandleProtocol(BlkIoHandles[i], &gEfiSimpleFileSystemProtocolGuid, (VOID **)&Fs);
      if(Status != EFI_SUCCESS)
        continue;

      if ((SelectionAttrib & BLK_IO_SEL_SELECT_BY_VOLUME_NAME) != 0)
      {
        if (!FilterData || FilterData->VolumeName == 0)
          return EFI_INVALID_PARAMETER;

        if (CompareVolumeLabel (Fs, FilterData->VolumeName) != 0)
          continue;
      }
    }

    /* Check if the Partition name related criteria satisfies */
    if ((SelectionAttrib & BLK_IO_SEL_MATCH_PARTITION_LABEL) != 0)
    {
      Status = gBS->HandleProtocol(BlkIoHandles[i], &gEfiPartitionRecordGuid, (VOID **) &PartEntry);
      if (Status != EFI_SUCCESS)
        continue;

      if ((SelectionAttrib & BLK_IO_SEL_STRING_CASE_INSENSITIVE) != 0)
      {
        if (StriCmp(PartEntry->PartitionName, FilterData->PartitionLabel))
          continue;
      }
      else
      {
        if (StrCmp(PartEntry->PartitionName, FilterData->PartitionLabel))
          continue;
      }
    }

    /* We came here means, this handle satisfies all the conditions needed,
     * Add it into the list */
    HandleInfoPtr[BlkIoCnt].Handle = BlkIoHandles[i];
    HandleInfoPtr[BlkIoCnt].BlkIo = BlkIo;
    HandleInfoPtr[BlkIoCnt].PartitionInfo = PartitionOut;
    BlkIoCnt++;
    if (BlkIoCnt >= *MaxBlkIopCnt)
      break;
  }

  *MaxBlkIopCnt = BlkIoCnt;

  /* Free the handle buffer */
  if (BlkIoHandles != NULL)
    FreePool(BlkIoHandles);

  return EFI_SUCCESS;
}

CHAR16 *DebugPartitions[] = {};

BOOLEAN
IsDebugPartition(CHAR16 *Label)
{
  EFI_STATUS Status;
  UINT32  Setting = 0;
  UINTN   i;
  UINTN   NumDebugPartitions = 0;

  /* Handle LogFs partition according to uefiplat */
  if (!StriCmp(Label, L"logfs")) {
    Status = GetConfigValue("EnableLogFsSyncInRetail", &Setting); 
    if ((Status == EFI_SUCCESS) && (Setting == 1))
      return FALSE; 
    else
      return TRUE; 
  }

  NumDebugPartitions = sizeof(DebugPartitions)/sizeof(*DebugPartitions);
  for (i = 0; i < NumDebugPartitions; i++)
  {
    if (!StriCmp(Label, DebugPartitions[i]))
      return TRUE;
  }

  return FALSE;
}

/**
  Mount specific FAT partition

  @param  PartitionLabel  Name of partition label to mount
  @return Status

**/
EFI_STATUS
MountFatPartition(
  CHAR16    *PartitionLabel
  )
{
  EFI_STATUS Status;
  UINT32     MaxHandles;
  UINT32     Attrib = 0;
  VOID*      Fs;

  if (PartitionLabel == NULL)
    return EFI_INVALID_PARAMETER;

  DEBUG((EFI_D_WARN, "Mounting FAT Volume: %s\n", PartitionLabel));

  Attrib = BLK_IO_SEL_PARTITIONED_GPT;
  Attrib |= BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE;

  if (RETAIL && IsDebugPartition(PartitionLabel))
    return EFI_INVALID_PARAMETER;

  if (PartitionLabel)
    Attrib |= BLK_IO_SEL_MATCH_PARTITION_LABEL;

  Attrib |= BLK_IO_SEL_STRING_CASE_INSENSITIVE;

  MaxHandles = sizeof(HandleInfoList)/sizeof(*HandleInfoList);

  HandleFilter.RootDeviceType = 0;
  HandleFilter.PartitionLabel = 0;
  HandleFilter.PartitionType = 0;
  HandleFilter.VolumeName = 0;

  if (PartitionLabel)
    HandleFilter.PartitionLabel = PartitionLabel;

  Status = GetBlkIOHandles(Attrib, &HandleFilter, HandleInfoList, &MaxHandles);
  if (EFI_ERROR(Status) || (MaxHandles != 1))
  {
    DEBUG((EFI_D_INFO, "[MountFat] Failed to get handle\n"));
    return Status;
  }

  /*If FAT file systems is already mounted, return success at this point */
  Status = gBS->HandleProtocol(HandleInfoList[0].Handle, &gEfiSimpleFileSystemProtocolGuid, (VOID **)&Fs);
  if(Status == EFI_SUCCESS)
  {
    DEBUG((EFI_D_INFO, "[MountFatExt] Partition Handle already Exists \n"));
    return Status;
  }

  Status = gBS->ConnectController (HandleInfoList[0].Handle, NULL, NULL, TRUE);
  if (EFI_ERROR(Status))
    DEBUG((EFI_D_INFO, "[MountFat] Failed to connect controller\n"));

  return Status;
}

/**
  Unmount specific FAT partition

  @param  BlkIoHandle  Block IO handle of partition to unmount FAT
  @return Status

**/
EFI_STATUS
UnmountFatPartition(
  EFI_HANDLE *UserHandle
  )
{
  EFI_STATUS             Status;
  EFI_PARTITION_ENTRY   *PartEntry;
  EFI_HANDLE            *FatHandles = NULL;
  EFI_BLOCK_IO_PROTOCOL *BlkIo, *UserBlkIo;
  UINTN                  FatHandleCount, i;
  UINT8                  Matched;

  Matched = FALSE;
  BlkIo = NULL;
  UserBlkIo = NULL;
  FatHandleCount = 0;
    

  if (UserHandle == NULL)
    return EFI_INVALID_PARAMETER;

  Status = gBS->HandleProtocol(UserHandle, 
                               &gEfiPartitionRecordGuid, 
                               (VOID **) &PartEntry);

  if (EFI_ERROR(Status) || (PartEntry == NULL))
    return EFI_UNSUPPORTED;


  Status = gBS->HandleProtocol(UserHandle, 
                               &gEfiBlockIoProtocolGuid, 
                               (VOID **) &UserBlkIo);

  if (EFI_ERROR(Status) || (UserBlkIo == NULL))
    return EFI_UNSUPPORTED;


  Status = gBS->LocateHandleBuffer (ByProtocol,
                                    &gEfiSimpleFileSystemProtocolGuid,
                                    NULL,
                                    &FatHandleCount,
                                    &FatHandles);
  if (EFI_ERROR(Status))
    return Status;

  for (i = 0; i < FatHandleCount; i++){

    Status = gBS->HandleProtocol (FatHandles[i],
                                  &gEfiBlockIoProtocolGuid,
                                  (VOID **) &BlkIo);
    /* Fv volumes will not support Blk I/O protocol */
    if (Status == EFI_UNSUPPORTED) {
      continue;
    }

    if (BlkIo == UserBlkIo)
    {
      Matched = TRUE;
      break;
    }
  }
  
  if (Matched)
  {
    Status = gBS->DisconnectController (FatHandles[i], NULL, NULL);
    if (EFI_ERROR(Status))
      DEBUG((EFI_D_INFO, "[UnmountFat] Failed to disconnect controller\n"));
    return Status;

  }
  else
    return EFI_NOT_FOUND;

}

/**
  Start an EFI image only from a specified GPP partition

  @param  Path   Image path
  @param  RootDeviceTypeGuid eg: gEfiEmmcGppPartition1Guid
  @param  Partition Type GUID eg: gEfiPartTypeSystemPartGuid
  @param  Boolean TRUE for Non-removable / FALSE for Removable
  @param  Volume Name to Match
  @return Exit code from image

**/
EFI_STATUS
ExecImgFromVolume(
  CHAR16        *Path,
  EFI_GUID      *RootDeviceType,
  EFI_GUID      *PartitionType,
  BOOLEAN       SelectNonRemovable,
  CHAR8         *VolumeName
  )
{
  EFI_STATUS Status;
  EFI_DEVICE_PATH_PROTOCOL *DevicePath;
  EFI_HANDLE Image;
  UINT32 MaxHandles;
  UINTN ExitDataSize;
  CHAR16 *ExitData;
  UINT32 Attrib = 0;

  MaxHandles = sizeof(HandleInfoList)/sizeof(*HandleInfoList);

  if( ( (RootDeviceType == NULL) && (PartitionType == NULL) && (VolumeName == NULL))
         || (Path == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Attrib |= BLK_IO_SEL_PARTITIONED_GPT;

  if (SelectNonRemovable == TRUE)
    Attrib |= BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE;
  else if(SelectNonRemovable == FALSE)
    Attrib |= BLK_IO_SEL_MEDIA_TYPE_REMOVABLE;

  if (RootDeviceType != NULL)
    Attrib |= BLK_IO_SEL_MATCH_ROOT_DEVICE;

  if (PartitionType != NULL)
    Attrib |= BLK_IO_SEL_MATCH_PARTITION_TYPE_GUID;

  HandleFilter.PartitionType = PartitionType;
  HandleFilter.RootDeviceType = RootDeviceType;

  if(VolumeName != NULL)
    HandleFilter.VolumeName = VolumeName;
  else
    HandleFilter.VolumeName = 0;

  Status = GetBlkIOHandles(Attrib, &HandleFilter, HandleInfoList, &MaxHandles);

  if(Status == EFI_SUCCESS) {
    if(MaxHandles == 0)
      return EFI_NO_MEDIA;

    if(MaxHandles != 1) {
      //Unable to deterministically load from single partition
      DEBUG(( EFI_D_INFO, "ExecImgFromVolume(): multiple partitions found.\r\n"));
      return EFI_LOAD_ERROR;
    }
  }
  else {
    return Status;
  }

  DevicePath = FileDevicePath (HandleInfoList[0].Handle, Path);

  Status = gBS->LoadImage (FALSE, gImageHandle, DevicePath, NULL, 0, &Image);
  if(EFI_ERROR(Status)) {
   return Status;
  }

  Status = gBS->StartImage (Image, &ExitDataSize, &ExitData);
  return Status;
}


/*
 *  Return the platform type if available.
 *
 * */
EFI_STATUS
GetPlatformType (EFI_PLATFORMINFO_PLATFORM_TYPE* PlatformTypePtr)
{
  EFI_PLATFORMINFO_PROTOCOL              *hPlatformInfoProtocol;
  EFI_PLATFORMINFO_PLATFORM_INFO_TYPE    PlatformInfo;
  EFI_STATUS                             Status;

  if (PlatformTypePtr == NULL)
    return EFI_INVALID_PARAMETER;

  Status = gBS->LocateProtocol (&gEfiPlatformInfoProtocolGuid, NULL,
                                (VOID **) &hPlatformInfoProtocol);
  if (Status != EFI_SUCCESS)
    return EFI_NOT_READY;

  Status = hPlatformInfoProtocol->GetPlatformInfo(hPlatformInfoProtocol, &PlatformInfo);

  if (Status != EFI_SUCCESS)
    return Status;

  *PlatformTypePtr = PlatformInfo.platform;

  return EFI_SUCCESS;
}


/**
 * Get the platform related strings for Platform Type, Chip and Chip version
 *
 * @param  DestinationBuffer   Pointer to buffer where result can be expected
 * @param  BufferSize          Size of the buffer
 * @param  PlatformTypeStr     Optional Pointer to Platform type string
 * @param  ChipNameStr         Optional Pointer to Chip Name string
 * @param  ChipVerStr          Optional Pointer to Chip Version string
 * **/
EFI_STATUS
GetPlatformStrings (IN OUT  CHAR8*  DestinationBuffer,
                    IN      UINTN   BufferSize,
                    OUT     CHAR8** PlatformTypeStr  OPTIONAL,
                    OUT     CHAR8** ChipNameStr      OPTIONAL,
                    OUT     CHAR8** ChipVerStr       OPTIONAL)
{
  EFI_STATUS                  Status;
  EFI_CHIPINFO_PROTOCOL       *pChipInfoProtocol;
  EFI_PLATFORMINFO_PROTOCOL   *pPlatformInfoProtocol;
  EFIChipInfoVersionType      Version;
  CHAR8                       *TempPtr;
  UINTN                       Size;
  UINT16                      Low = 0, High = 0;
  EFI_PLATFORMINFO_PLATFORM_INFO_TYPE PlatformInfo;

  if (DestinationBuffer == NULL || BufferSize == 0)
    return EFI_INVALID_PARAMETER;

  /* Get Protocol handles */
  Status = gBS->LocateProtocol (&gEfiChipInfoProtocolGuid, NULL,
                               (VOID **) &pChipInfoProtocol);
  if (EFI_ERROR(Status))
    return Status;

  Status = gBS->LocateProtocol ( &gEfiPlatformInfoProtocolGuid, NULL,
                                (VOID **) &pPlatformInfoProtocol);
  if (EFI_ERROR(Status))
    return Status;

  TempPtr = DestinationBuffer;

  Status = pPlatformInfoProtocol->GetPlatformInfo(pPlatformInfoProtocol,
                                                  &PlatformInfo);
  if (EFI_ERROR(Status))
  {
    DEBUG ((EFI_D_INFO, "GetPlatformInfo failed\n"));
    return Status;
  }

  if (BufferSize < 20)
    return EFI_BUFFER_TOO_SMALL;


  AsciiStrnCpy (TempPtr, PlatformTypeStrings[PlatformInfo.platform],
                BufferSize);

  if (PlatformTypeStr)
    *PlatformTypeStr = TempPtr;

  /* This includes the nul terminator */
  Size = AsciiStrSize (TempPtr);
  BufferSize -= Size;
  TempPtr += Size;

  if (BufferSize < 12)
    return EFI_BUFFER_TOO_SMALL;

  if ((Status = pChipInfoProtocol->GetChipIdString(pChipInfoProtocol,
                                      TempPtr, BufferSize)) != EFI_SUCCESS)
    return Status;

  if (ChipNameStr)
    *ChipNameStr = TempPtr;

  Size = AsciiStrSize (TempPtr);
  BufferSize -= Size;
  TempPtr += Size;

  if (BufferSize < 12)
    return EFI_BUFFER_TOO_SMALL;

  if ((Status = pChipInfoProtocol->GetChipVersion(pChipInfoProtocol,
                                                  &Version)) != EFI_SUCCESS)
    return Status;
  
  Low = Version & 0xFFFF;
  High = (Version >> 16) & 0xFFFF;

  if (TempPtr != NULL)
    AsciiSPrint (TempPtr, BufferSize, "%d.%d", High, Low);

  if (ChipVerStr)
    *ChipVerStr = TempPtr;

  return EFI_SUCCESS;
}

/**
 * Get processor name string
 *
 * @param  ProcNameStr   Pointer to buffer where result can be expected
 * @param  StrSize       Size of the buffer
 * **/

#define MAX_PROCESSOR_STR_LEN    64
#define SKIP_CHIPID_LETTER_LEN   3

EFI_STATUS
GetProcessorName (IN OUT  CHAR8*  ProcNameStr,
                  IN      UINTN    StrSize)
{

  EFI_STATUS                  Status;
  STATIC CHAR8                ProcessorNameString[MAX_PROCESSOR_STR_LEN];
  EFI_CHIPINFO_PROTOCOL       *pChipInfoProtocol;
  UINTN                       Len;

  AsciiStrCpy (ProcessorNameString, "Qualcomm Snapdragon ");
  Len = AsciiStrLen (ProcessorNameString);

  if (ProcNameStr == NULL || StrSize == 0)
    return EFI_INVALID_PARAMETER;

  if (StrSize < AsciiStrLen ("Qualcomm Snapdragon xxxxxxx"))
    return EFI_BUFFER_TOO_SMALL;

  /* Get Protocol handles */
  Status = gBS->LocateProtocol (&gEfiChipInfoProtocolGuid, NULL,
                               (VOID **) &pChipInfoProtocol);
  if (EFI_ERROR(Status))
    return Status;

  pChipInfoProtocol->GetMarketingNameString (pChipInfoProtocol,
                                             (CHAR8*)(((UINTN)ProcessorNameString + Len)),
                                             (MAX_PROCESSOR_STR_LEN - Len));

  if (StrSize < AsciiStrLen (ProcessorNameString))
    return EFI_BUFFER_TOO_SMALL;

  AsciiStrCpy (ProcNameStr, ProcessorNameString);
  return EFI_SUCCESS;
}

/**
  Open a file from a specified GPP partition and return the file-handle

  @param  Path                File path
  @param  RootDeviceTypeGuid  eg: gEfiEmmcGppPartition1Guid
  @param  PartitionType       eg: gEfiPlatPartitionTypeGuid
  @param  PartitionLabel      eg: L"logfs"
  @param  SelectNonRemovable  TRUE for Non-removable / FALSE for Removable
  @param  VolumeName          Volume to match
  @param  FileHandle          Pointer to File-handle that is opened in this fucntion
  @return EFI_SUCCESS         If file was opened successfully

**/
STATIC
EFI_STATUS
OpenFile(
  IN  CHAR16           *Path,
  IN  EFI_GUID         *RootDeviceType,
  IN  EFI_GUID         *PartitionType,
  IN  CHAR16           *PartitionLabel,
  IN  BOOLEAN          SelectNonRemovable,
  IN  CHAR8            *VolumeName,
  IN  UINT64           OpenMode,
  OUT EFI_FILE_HANDLE  *FileHandle
  )
{
  EFI_STATUS                        Status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL   *Volume;
  EFI_FILE_HANDLE                   RootFileHandle;
  UINT32                            MaxHandles;
  UINT32                            Attrib = 0;

  MaxHandles = sizeof(HandleInfoList)/sizeof(*HandleInfoList);

  if( ( (RootDeviceType == NULL) && (PartitionType == NULL) && (PartitionLabel == NULL)) )
    return EFI_INVALID_PARAMETER;

  Attrib |= BLK_IO_SEL_PARTITIONED_GPT;

  if (SelectNonRemovable == TRUE)
    Attrib |= BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE;

  if (RootDeviceType != NULL)
    Attrib |= BLK_IO_SEL_MATCH_ROOT_DEVICE;

  if (PartitionType != NULL)
    Attrib |= BLK_IO_SEL_MATCH_PARTITION_TYPE_GUID;

  if (PartitionLabel != NULL)
  {
    Attrib |= BLK_IO_SEL_MATCH_PARTITION_LABEL;
    Attrib |= BLK_IO_SEL_STRING_CASE_INSENSITIVE;
  }

  HandleFilter.PartitionType = PartitionType;
  HandleFilter.RootDeviceType = RootDeviceType;
  HandleFilter.PartitionLabel = PartitionLabel;

  if(VolumeName != NULL)
      HandleFilter.VolumeName = VolumeName;
    else
      HandleFilter.VolumeName = 0;

  // Returns the IO Block handle
  Status = GetBlkIOHandles(Attrib,
                           &HandleFilter,
                           HandleInfoList,
                           &MaxHandles
                           );

  if(!EFI_ERROR(Status)) {
    if(MaxHandles == 0)
      return EFI_NO_MEDIA;

    if(MaxHandles != 1) {
      // Unable to deterministically load from single partition
      DEBUG(( EFI_D_INFO, "OpenFile(): multiple partitions found \r\n"));
      return EFI_LOAD_ERROR;
    }
  }

  // Find the file system interface to the device
  Status = gBS->HandleProtocol(HandleInfoList[0].Handle,
                               &gEfiSimpleFileSystemProtocolGuid,
                               (VOID *) &Volume
                               );
  if(EFI_ERROR(Status))
    return Status;

  // Open the root directory of the volume
  Status = Volume->OpenVolume(Volume,
                              &RootFileHandle
                              );
  if(EFI_ERROR(Status) || (RootFileHandle == NULL))
    return Status;

  Status = RootFileHandle->Open(RootFileHandle,
                                FileHandle,
                                Path,
                                OpenMode,
                                0
                                );
  if(EFI_ERROR(Status))
    *FileHandle = NULL;

  // Close root-dir handle
  if(RootFileHandle != NULL){
    RootFileHandle->Close(RootFileHandle);
  }

  return Status;
}

/**
  Return the size of a file

  @param  Path                File path
  @param  RootDeviceTypeGuid  eg: gEfiEmmcGppPartition1Guid
  @param  PartitionType       eg: gEfiPlatPartitionTypeGuid
  @param  PartitionLabel      eg: L"logfs"
  @param  SelectNonRemovable  TRUE for Non-removable / FALSE for Removable
  @param  Volumename          VolumeName to match
  @param  FileSize            Size of file
  @return EFI_SUCCESS         On successfully retreiving file-size

**/
EFI_STATUS
GetFileSize(
  IN  CHAR16           *Path,
  IN  EFI_GUID         *RootDeviceType,
  IN  EFI_GUID         *PartitionType,
  IN  CHAR16           *PartitionLabel,
  IN  BOOLEAN          SelectNonRemovable,
  IN  CHAR8            *VolumeName,
  OUT UINT32           *FileSize
  )
{
  EFI_STATUS      Status;
  EFI_FILE_HANDLE FileHandle;

  // Open File
  Status = OpenFile(Path,
                    RootDeviceType,
                    PartitionType,
                    PartitionLabel,
                    SelectNonRemovable,
                    VolumeName,
                    EFI_FILE_MODE_READ,
                    &FileHandle
                    );

  // Get the File info
  if( (!EFI_ERROR(Status)) && (FileHandle != NULL) )
  {
    EFI_FILE_INFO *Info    = NULL;
    UINTN         InfoSize = FILE_INFO_SIZE;

    Info = AllocatePool (InfoSize);

    if(Info == NULL){
      Status =  EFI_OUT_OF_RESOURCES;
    }
    else{
      Status = FileHandle->GetInfo(FileHandle,
                                   &gEfiFileInfoGuid,
                                   &InfoSize,
                                   Info
                                   );

      if(Status == EFI_BUFFER_TOO_SMALL){
        // If Status == EFI_BUFFER_TOO_SMALL, InfoSize is too small to read file-info
        // InfoSize is updated with the actual size that needs to be allocated for Info
        // So, re-allocate with InfoSize, and call GetInfo()
        FreePool (Info);
        Info   = AllocatePool (InfoSize);
        if (Info == NULL)
        {
          AsciiPrint ("ERROR: Memory Allocation failed !\n");
          return EFI_OUT_OF_RESOURCES;
        }

        Status = FileHandle->GetInfo(FileHandle,
                                     &gEfiFileInfoGuid,
                                     &InfoSize,
                                     Info
                                     );
      }

      if(!EFI_ERROR(Status)){
        // If GetInfo() is successful, return the size of file
        *FileSize = Info->FileSize;
      }

      FreePool (Info);
      Info = NULL;
    }

    // Close file-handle
    if(FileHandle != NULL){
      Status = FileHandle->Close(FileHandle);
    }
  }

  return Status;
}

/**
  Read a file to a buffer

  @param  Path                File path
  @param  RootDeviceTypeGuid  eg: gEfiEmmcGppPartition1Guid
  @param  PartitionType       eg: gEfiPlatPartitionTypeGuid
  @param  PartitionLabel      eg: L"logfs"
  @param  SelectNonRemovable  TRUE for Non-removable / FALSE for Removable
  @param  Volumename          VolumeName to match
  @param  BytesToRead         # of bytes to read from file
  @param  ReadOffset          Read-offset in file in bytes
  @param  FileBuffer          Data read from file is copied to FileBuffer
  @param  FileBufferSize      Size of the file
  @return EFI_SUCCESS         If file was read successfully

**/
EFI_STATUS
ReadFile(
  IN     CHAR16           *Path,
  IN     EFI_GUID         *RootDeviceType,
  IN     EFI_GUID         *PartitionType,
  IN     CHAR16           *PartitionLabel,
  IN     BOOLEAN          SelectNonRemovable,
  IN     CHAR8            *VolumeName,
  IN OUT UINTN            *BytesToRead,
  IN     UINT64           ReadOffset,
  OUT    UINT8            *FileBuffer,
  IN     UINT32           FileBufferSize
  )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  EFI_FILE_HANDLE FileHandle;


  if((FileBuffer == NULL) || (BytesToRead == NULL) || (*BytesToRead == 0) || ((*BytesToRead + ReadOffset) > FileBufferSize)){
    return EFI_INVALID_PARAMETER;
  }

  // Open File
  Status = OpenFile(Path,
                    RootDeviceType,
                    PartitionType,
                    PartitionLabel,
                    SelectNonRemovable,
                    VolumeName,
                    EFI_FILE_MODE_READ,
                    &FileHandle
                    );

  if ((!EFI_ERROR(Status)) && (FileHandle != NULL)){
    // Set read-offset
    Status = FileHandle->SetPosition(FileHandle,
                                     ReadOffset
                                     );

    if(EFI_ERROR(Status)){
      DEBUG ((DEBUG_LOAD, "ReadFile(): Unable to set offset for file-read\r\n" ));
    }
    else{
      // Read file content
      Status = FileHandle->Read(FileHandle,
                                BytesToRead,
                                FileBuffer
                                );

      if ((EFI_ERROR (Status)) || (*BytesToRead == 0)) {
        DEBUG ((DEBUG_LOAD, "ReadFile() Unable to read from file \r\n"));
      }
    }

    // Close the file handle
    if(FileHandle != NULL){
      Status = FileHandle->Close(FileHandle);
    }
  }

  return Status;
}

/**
  Write a buffer to a file

  @param  Path                File path
  @param  RootDeviceTypeGuid  eg: gEfiEmmcGppPartition1Guid
  @param  PartitionType       eg: gEfiPlatPartitionTypeGuid
  @param  PartitionLabel      eg: L"logfs"
  @param  SelectNonRemovable  TRUE for Non-removable / FALSE for Removable
  @param  Volumename          VolumeName to match
  @param  BytesToRead         # of bytes to write to file
  @param  WriteOffset         Write-offset in file in bytes
  @param  FileBuffer          Data written to a file is copied from FileBuffer
  @param  FileBufferSize      Size of the file
  @return EFI_SUCCESS         If file was written successfully 

**/ 
EFI_STATUS
WriteFile(
  IN     CHAR16           *Path,
  IN     EFI_GUID         *RootDeviceType,
  IN     EFI_GUID         *PartitionType,
  IN     CHAR16           *PartitionLabel,
  IN     BOOLEAN          SelectNonRemovable,
  IN     CHAR8            *VolumeName,
  IN     UINTN            *BytesToWrite,
  IN     UINT64           WriteOffset,
  IN     UINT8            *FileBuffer,
  IN     UINT32           FileBufferSize
  )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  EFI_FILE_HANDLE FileHandle;


  if((FileBuffer == NULL) || (BytesToWrite == NULL) || (*BytesToWrite == 0) || ((*BytesToWrite + WriteOffset) > FileBufferSize)){
    return EFI_INVALID_PARAMETER;
  }

  // Open File
  Status = OpenFile(Path,
                    RootDeviceType,
                    PartitionType,
                    PartitionLabel,
                    SelectNonRemovable,
                    VolumeName,
                    EFI_FILE_MODE_READ|EFI_FILE_MODE_WRITE|EFI_FILE_MODE_CREATE,
                    &FileHandle
                    );

  if ((!EFI_ERROR(Status)) && (FileHandle != NULL)){
    // Set write-offset
    Status = FileHandle->SetPosition(FileHandle,
                                     WriteOffset
                                     );

    if(EFI_ERROR(Status)){
      DEBUG ((DEBUG_LOAD, "WriteFile(): Unable to set offset for file-write\r\n" ));
    }
    else{
      // Write file content
      Status = FileHandle->Write(FileHandle,
                                BytesToWrite,
                                FileBuffer
                                );

      if ((EFI_ERROR (Status)) || (*BytesToWrite == 0)) {
        DEBUG ((DEBUG_LOAD, "WriteFile() Unable to write the file \r\n"));
      }
    }

    // Close the file handle
    if(FileHandle != NULL){
      Status = FileHandle->Close(FileHandle);
    }
  }

  return Status;
}

/**
  Delete a  file from specified partition  
  @param  Path            Path to Smbios file
  @param  RootDeviceType  Root Device Type GUID 
  @param  Partition Type  GUID eg: gEfiPartTypeSystemPartGuid
  @param  PartitionLabel      eg: L"logfs"
  @param  SelectNonRemovable  TRUE for Non-removable / FALSE for Removable
  @param  Volumename          VolumeName to match
  @return EFI_SUCCESS if successful
**/
EFI_STATUS
DeleteFile (
  IN  CHAR16           *Path,
  IN  EFI_GUID         *RootDeviceType,
  IN  EFI_GUID         *PartitionType,
  IN  CHAR16           *PartitionLabel,
  IN  BOOLEAN          SelectNonRemovable,
  IN  CHAR8            *VolumeName
  )
{
  EFI_STATUS Status;
  EFI_FILE_HANDLE FileHandle;

  Status = OpenFile(Path,
                    RootDeviceType,
                    PartitionType,
                    PartitionLabel,
                    SelectNonRemovable,
                    VolumeName,
                    EFI_FILE_MODE_READ|EFI_FILE_MODE_WRITE,
                    &FileHandle
                    );

  if( (!EFI_ERROR(Status)) && (FileHandle!= NULL) )
  {
    Status = FileHandle->Delete(FileHandle);
    if(EFI_ERROR(Status))
      AsciiPrint("Unable to delete file");
  }

  return Status;
}

/**
  Convert string to integer. The string should have numeric characters.
  conversion stops at first non-numeric character encountered.

  @param  Str      Null Terminated string of characters.
          StrSize Max size of buffer, to prevent against
                   strings that are not NULL terminated.

  @retval  int    converted integer value of valid numeric string.
                          0 if invalid string

**/
UINT32 AsciiStrToHex (CHAR8* Str, UINT32 StrSize)
{
  UINT32 result = 0;
  if(Str == NULL)
    return 0;

  while( (*Str!=0) && StrSize )
  {
    // Break as soon as non-hex char encountered.
    if (!((*Str >= '0' && *Str <= '9') ||
          (*Str >= 'a' && *Str <= 'f') ||
          (*Str >= 'A' && *Str <= 'F')))
      break;

    result = result << 4;
    if(*Str >= '0' && *Str <= '9')
      result = result + (*Str - '0');
    if(*Str >= 'a' && *Str <= 'f')
      result = result + (*Str - 'a') + 10;
    if(*Str >= 'A' && *Str <= 'F')
      result = result + (*Str - 'A') + 10;

    StrSize--;
    Str++;
  }

  return result;
}

/**
  Convert string to Lowercase if lowercase alphabets found.

  @param  Str      Null Terminated string of characters.
          StrSize Max size of buffer, to prevent against
                   strings that are not NULL terminated.

  @retval  void

**/
void AsciiStrToLower (CHAR8* Str, UINT32 StrSize)
{
  do
  {
    if(Str == NULL)
      break;
    while( (*Str!=0) && StrSize )
    {
      if(*Str >= 'A' && *Str <= 'Z')
      {
         *Str = *Str + ('a'-'A');
      }
      StrSize--;
      Str++;
    }
  }while(0);
}


/**
  AsciiToInt
  @brief
  Converts ASCII string to unsigned integer
**/
UINTN AsciiToInt(CHAR8* Str)
{

  return (AsciiStrDecimalToUintn(Str));

}

/**
  Loads a  file from a partition specified by Root Device type GUID
  Partition Type GUID and Block IO attributes

  @param  Path            Path to Smbios file
  @param  RootDeviceType  Root Device Type GUID
  @param  Partition Type  GUID eg: gEfiPartTypeSystemPartGuid
  @param  PartitionLabel  eg: L"logfs"
  @param  BlkIOAttrib     Attributes to be used for fetching Block IO handle
  @param  ReadFileSize    Ptr that can hold read file size

  @return EFI_SUCCESS if successful
**/

EFI_STATUS
LoadFileFromPartition(
  UINT32        BlkIOAttrib,
  CHAR16        *Path,
  EFI_GUID      *RootDeviceType,
  EFI_GUID      *PartitionType,
  CHAR16        *PartitionLabel,
  UINT8         **CfgBuffPtr,
  UINTN         *ReadFileSize
)
{
  EFI_STATUS Status = EFI_NOT_FOUND;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL   *Volume = NULL;
  EFI_FILE_HANDLE                   RootFileHandle;
  EFI_FILE_HANDLE                   FileHandle;
  VOID                              *CfgFileBuffer = NULL;
  UINTN                             ReadSize;
  UINT32                            MaxHandles;
  HandleInfo                        HandleInfoList[2];
  PartiSelectFilter                 HandleFilter;
  EFI_FILE_INFO                     *FileInfo = NULL;

  MaxHandles = sizeof(HandleInfoList)/sizeof(*HandleInfoList);

  do
  {
    if (((RootDeviceType == NULL) && (PartitionType == NULL) && (PartitionLabel == NULL)) ||
        (Path == NULL) || (CfgBuffPtr == NULL) )
    {
      Status = EFI_INVALID_PARAMETER;
      break;
    }

    HandleFilter.PartitionType = PartitionType;
    HandleFilter.RootDeviceType = RootDeviceType;
    HandleFilter.PartitionLabel = PartitionLabel;
    HandleFilter.VolumeName = 0;

    Status = GetBlkIOHandles(BlkIOAttrib, &HandleFilter, HandleInfoList, &MaxHandles);

    if(Status == EFI_SUCCESS)
    {
      if(MaxHandles == 0)
      {
        Status = EFI_NO_MEDIA;
        break;
      }
      if(MaxHandles != 1)
      {
        //Unable to deterministically load from single partition
        DEBUG(( EFI_D_WARN, "LoadFileFromPartition(Path: %s): multiple partitions found\n", Path));
        Status = EFI_LOAD_ERROR;
        break;
      }
    }

    // File the file system interface to the device
    Status = gBS->HandleProtocol (HandleInfoList[0].Handle,
                                  &gEfiSimpleFileSystemProtocolGuid,
                                  (VOID *) &Volume);
    if(Status != EFI_SUCCESS)
      break;

    // Open the root directory of the volume
    Status = Volume->OpenVolume (Volume, &RootFileHandle);

    if((Status != EFI_SUCCESS) || (RootFileHandle == NULL))
      break;

    //Open File
    Status = RootFileHandle->Open (RootFileHandle,
                                   &FileHandle,
                                   Path,
                                   (UINT64)EFI_FILE_MODE_READ,
                                   (UINT64)0);
    if((Status != EFI_SUCCESS) || (FileHandle == NULL))
      break;

    // Found the file. Now we need to read it. Before we can read the file we need to
    // figure out how big the file is.
    //
    FileInfo = NULL;
    ReadSize = 0;
    Status = FileHandle->GetInfo (FileHandle,
                                  &gEfiFileInfoGuid,
                                  &ReadSize,
                                  FileInfo);

    if (Status == EFI_BUFFER_TOO_SMALL)
    {
      FileInfo = AllocatePool (ReadSize);
      if (FileInfo == NULL)
      {
        Status = EFI_OUT_OF_RESOURCES;
        break;
      }
      Status = FileHandle->GetInfo (FileHandle,
                                    &gEfiFileInfoGuid,
                                    &ReadSize,
                                    FileInfo);
    }

    if (!EFI_ERROR (Status) && (FileInfo != NULL))
    {
      // Allocate space for the file
      CfgFileBuffer = AllocatePool ((UINTN)FileInfo->FileSize);
      if (CfgFileBuffer == NULL)
      {
        Status = EFI_OUT_OF_RESOURCES;
        break;
      }
      // Read the file into the buffer we allocated
      ReadSize = (UINTN)FileInfo->FileSize;
      Status = FileHandle->Read (FileHandle, &ReadSize, CfgFileBuffer);
    }

    if(Status != EFI_SUCCESS)
    {
      DEBUG (( EFI_D_WARN, "LoadFileFromPartition() failed to Read File \r\n"));
      break;
    }

    if (CfgFileBuffer != NULL)
    {
      *CfgBuffPtr = CfgFileBuffer;
      if (ReadFileSize)
        *ReadFileSize = (UINTN)FileInfo->FileSize;
      Status = EFI_SUCCESS;
    }
    if (FileHandle != NULL)
      FileHandle->Close(FileHandle);

  }while(0);

  return Status;
}

/**
  Loads a  file from a volume specified by name and Block IO attributes

  @param  Path            Path to Smbios file
  @param  VolumeName      Name of volume to load from
  @param  BlkIOAttrib     Attributes to be used for fetching Block IO handle
  @param  ReadFileSize    Ptr that can hold read file size

  @return EFI_SUCCESS if successful
**/

EFI_STATUS
LoadFileFromVolume(
  UINT32        BlkIOAttrib,
  CHAR16        *Path,
  CHAR8         *VolumeName,
  UINT8         **CfgBuffPtr,
  UINTN         *ReadFileSize
)
{
  EFI_STATUS Status = EFI_NOT_FOUND;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL   *Volume = NULL;
  EFI_FILE_HANDLE                   RootFileHandle;
  EFI_FILE_HANDLE                   FileHandle;
  VOID                              *CfgFileBuffer = NULL;
  UINTN                             ReadSize;
  UINT32                            MaxHandles;
  HandleInfo                        HandleInfoList[2];
  PartiSelectFilter                 HandleFilter;
  EFI_FILE_INFO                     *FileInfo = NULL;

  MaxHandles = sizeof(HandleInfoList)/sizeof(*HandleInfoList);

  do
  {
    if ( (VolumeName == NULL) || (Path == NULL) || (CfgBuffPtr == NULL) )
    {
      Status = EFI_INVALID_PARAMETER;
      break;
    }

    HandleFilter.PartitionType = 0;
    HandleFilter.RootDeviceType = 0;
    HandleFilter.VolumeName = VolumeName;

    BlkIOAttrib |= (BLK_IO_SEL_SELECT_MOUNTED_FILESYSTEM | BLK_IO_SEL_SELECT_BY_VOLUME_NAME);

    Status = GetBlkIOHandles(BlkIOAttrib, &HandleFilter, HandleInfoList, &MaxHandles);

    if(Status == EFI_SUCCESS)
    {
      if(MaxHandles == 0)
      {
        Status = EFI_NO_MEDIA;
        break;
      }
      if(MaxHandles != 1)
      {
        //Unable to deterministically load from single partition
        DEBUG(( EFI_D_WARN, "LoadFileFromVolume(Path: %s): multiple partitions found\n", Path));
        Status = EFI_LOAD_ERROR;
        break;
      }
    }

    // File the file system interface to the device
    Status = gBS->HandleProtocol (HandleInfoList[0].Handle,
                                  &gEfiSimpleFileSystemProtocolGuid,
                                  (VOID *) &Volume);
    if(Status != EFI_SUCCESS)
      break;

    // Open the root directory of the volume
    Status = Volume->OpenVolume (Volume, &RootFileHandle);

    if((Status != EFI_SUCCESS) || (RootFileHandle == NULL))
      break;

    //Open File
    Status = RootFileHandle->Open (RootFileHandle,
                                   &FileHandle,
                                   Path,
                                   (UINT64)EFI_FILE_MODE_READ,
                                   (UINT64)0);
    if((Status != EFI_SUCCESS) || (FileHandle == NULL))
      break;

    // Found the file. Now we need to read it. Before we can read the file we need to
    // figure out how big the file is.
    //
    FileInfo = NULL;
    ReadSize = 0;
    Status = FileHandle->GetInfo (FileHandle,
                                  &gEfiFileInfoGuid,
                                  &ReadSize,
                                  FileInfo);

    if (Status == EFI_BUFFER_TOO_SMALL)
    {
      FileInfo = AllocatePool (ReadSize);
      if (FileInfo == NULL)
      {
        Status = EFI_OUT_OF_RESOURCES;
        break;
      }
      Status = FileHandle->GetInfo (FileHandle,
                                    &gEfiFileInfoGuid,
                                    &ReadSize,
                                    FileInfo);
    }

    if (!EFI_ERROR (Status) && (FileInfo != NULL))
    {
      // Allocate space for the file
      CfgFileBuffer = AllocatePool ((UINTN)FileInfo->FileSize);
      if (CfgFileBuffer == NULL)
      {
        Status = EFI_OUT_OF_RESOURCES;
        break;
      }
      // Read the file into the buffer we allocated
      ReadSize = (UINTN)FileInfo->FileSize;
      Status = FileHandle->Read (FileHandle, &ReadSize, CfgFileBuffer);
    }

    if(Status != EFI_SUCCESS)
    {
      DEBUG (( EFI_D_WARN, "LoadFileFromVolume() failed to Read File \r\n"));
      break;
    }

    if (CfgFileBuffer != NULL)
    {
      *CfgBuffPtr = CfgFileBuffer;
      if (ReadFileSize)
        *ReadFileSize = (UINTN)FileInfo->FileSize;
      Status = EFI_SUCCESS;
    }
    if (FileHandle != NULL)
      FileHandle->Close(FileHandle);

  }while(0);

  return Status;
}


/**
  This function flushes the UEFI variable services to ensure that the variable operations
  are written to disk without requiring a reset.

  @retval  EFI_SUCCESS      Successfully flushed NV variable services
  @retval  EFI_UNSUPPORTED  Returned if not able to locate variable services protocol
**/
EFI_STATUS
FlushVariableServices(
  VOID
  )
{
  EFI_STATUS Status = EFI_UNSUPPORTED;
  STATIC EFI_VARIABLESERVICES_PROTOCOL *gVariableServicesProtocol = NULL;

  if (gVariableServicesProtocol == NULL)
  {
    Status = gBS->LocateProtocol(&gEfiVariableServicesProtocolGuid,
                                 NULL,
                                 (VOID**) &gVariableServicesProtocol);
    if (EFI_ERROR(Status))
    {
      return Status;
    }
    else if (gVariableServicesProtocol == NULL)
    {
      return EFI_UNSUPPORTED;
    }
  }

  Status = gVariableServicesProtocol->FlushVariableNV(gVariableServicesProtocol);

  return Status;
}

UINTN
HexStrToBin (UINT8* Buffer, UINTN BuffSize, UINT32* OutBuffer, UINTN OutSize)
{
  UINTN ValSize;
  UINT8* ValPtr = (UINT8*)OutBuffer;
  UINT8 c;
  UINT8 ByteVal;

  if ((Buffer == NULL) || (BuffSize == 0) || (OutBuffer == NULL) || (OutSize == 0))
    return 0;

  SetMem (ValPtr, OutSize, 0);

  ValSize = 0;
  while (BuffSize && OutSize && ValPtr && Buffer && *Buffer)
  {
    c = *Buffer;
    /* Skip spaces */
    if ((c == ' ') || (c == '\t'))
    {
      ++Buffer;
      --BuffSize;
      continue;
    }
    ByteVal = 0;

    if ((c >= '0') && (c <= '9'))
      ByteVal = ByteVal + (c - '0');
    else
      if ((c >= 'a') && (c <= 'f'))
        ByteVal += (c - 'a' + 10);
      else
        if ((c >= 'A') && (c <= 'F'))
          ByteVal += (c - 'A' + 10);
        else   /* is this else clause needed? */
        {
          ++Buffer;
          --BuffSize;
        }


    /* Skip spaces */
    while (BuffSize && Buffer && *Buffer)
    {
      ++Buffer;
      --BuffSize;
      c = *Buffer;
      if ((c == ' ') || (c == '\t'))
        continue;
      else
        break;
    }

    if ((BuffSize == 0) || (Buffer == NULL) || (*Buffer == 0))
    {
      *ValPtr = ByteVal;
      ++ValSize;
      break;
    }

    c = *Buffer;
    ByteVal = ByteVal << 4;

    if ((c >= '0') && (c <= '9'))
      ByteVal = ByteVal + (c - '0');
    else
      if ((c >= 'a') && (c <= 'f'))
        ByteVal += (c - 'a' + 10);
      else
        if ((c >= 'A') && (c <= 'F'))
          ByteVal += (c - 'A' + 10);
        else   /* is this else clause needed? */
        {
          ++Buffer;
          --BuffSize;
          *ValPtr = ByteVal;
        }

    *ValPtr = ByteVal;
    --BuffSize;
    --OutSize;
    ++Buffer;
    ++ValPtr;
    ++ValSize;
  }

  return ValSize;
}

/* NOTE: this is non-blocking call */
STATIC EFI_STATUS
ReadKeyInternal (OUT EFI_INPUT_KEY  *KeyPtr OPTIONAL, UINT32 Attributes OPTIONAL)
{
  EFI_KEY_DATA     TempKeyData;
  EFI_STATUS       Status;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *InputEx;

//  Enable this later
//  if (Attributes & READ_KEY_ATTRIB_USE_NATIVE_BUTTONS_ONLY)
//    Status = GetNativeKeypad (&InputEx);
//  else
    Status = gBS->HandleProtocol (gST->ConsoleInHandle,
                                  &gEfiSimpleTextInputExProtocolGuid,
                                  (VOID**)&InputEx);
  if (Status != EFI_SUCCESS)
    return EFI_DEVICE_ERROR;

  SetMem (&TempKeyData, sizeof(TempKeyData), 0);

  /* Check which Key */
  Status = InputEx->ReadKeyStrokeEx (InputEx, &TempKeyData);

  if (Attributes & READ_KEY_ATTRIB_RESET_AFTER_READ)
    InputEx->Reset (InputEx, FALSE);

  if (Status != EFI_SUCCESS)
    return EFI_DEVICE_ERROR;

  if (KeyPtr)
  {
    KeyPtr->ScanCode = TempKeyData.Key.ScanCode;
    KeyPtr->UnicodeChar = TempKeyData.Key.UnicodeChar;

    /* Check if there are other possible mapped keys */
    if ((KeyPtr->ScanCode == 0) && (KeyPtr->UnicodeChar == 0))
    {
      UINT32 ShiftState, ToggleState;

      ShiftState = TempKeyData.KeyState.KeyShiftState;
      ToggleState = TempKeyData.KeyState.KeyToggleState;

      /* HOME Button under the screen mapped as Logo Key, this is internally mapped as
       * HOME key */
      if ((ShiftState == (EFI_SHIFT_STATE_VALID | EFI_RIGHT_LOGO_PRESSED)) &&
          (ToggleState == (EFI_TOGGLE_STATE_VALID | EFI_KEY_STATE_EXPOSED)))
      {
        KeyPtr->ScanCode = SCAN_HOME;
      }
    }
  }

  return EFI_SUCCESS;
}

/* NOTE: this is a blocking call, doesn't return unless a key press is detected */
EFI_STATUS
ReadAnyKey (OUT EFI_INPUT_KEY *KeyPtr OPTIONAL, UINT32 Attributes OPTIONAL)
{
  EFI_EVENT        KeyEvent;
  UINTN            OutIndex;
  EFI_STATUS       Status;

  if ((Attributes & READ_KEY_ATTRIB_NO_BLOCKING) == 0)
  {
    /* TODO: if user requests only native buttons, then may be we have to use
     *       that protocol's event */
    while (1)
    {
      KeyEvent = gST->ConIn->WaitForKey;
      Status = gBS->WaitForEvent (1, &KeyEvent, &OutIndex);

      if (Status != EFI_SUCCESS)
        return EFI_DEVICE_ERROR;

      /* If its key we got our expected key */
      if (OutIndex == 0)
        break;
    }
  }

  return ReadKeyInternal (KeyPtr, Attributes);
}

EFI_STATUS ReadFromFV (
  IN  CONST CHAR16 *UiString,
  OUT VOID        **DataBuffer,
  OUT UINTN        *DataSize
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *Buffer = NULL;
  EFI_GUID                      NameGuid;
  EFI_STATUS                    NextStatus;
  UINTN                         NoHandles;
  UINTN                         Index;
  EFI_FV_FILETYPE               FileType;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv = NULL;
  VOID                          *Key = NULL;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINTN                         UiStringLen = 0;
  CHAR16                        *UiSection = NULL;
  UINTN                         UiSectionSize;
  UINT32                        Authentication;
  BOOLEAN                       bFound = FALSE;

  if ((UiString == NULL) ||
      (DataBuffer == NULL) ||
      (DataSize == NULL)
     )
  {
    return EFI_INVALID_PARAMETER;
  }
  UiStringLen = StrLen (UiString);

  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiFirmwareVolume2ProtocolGuid, NULL, &NoHandles, &Buffer);
  if (EFI_ERROR (Status) || (Buffer == NULL))
  {
    goto clean_up;
  }

  for (Index = 0; Index < NoHandles; Index++)
  {
    Status = gBS->HandleProtocol (Buffer[Index], &gEfiFirmwareVolume2ProtocolGuid, (VOID **)&Fv);
    if (EFI_ERROR (Status))
    {
      /* Move to the next firmware volume. */
      continue;
    }

    Status = gBS->AllocatePool(EfiBootServicesData, Fv->KeySize, (VOID**)&Key);
    if (EFI_ERROR (Status))
    {
       goto clean_up;
    }

    ZeroMem(Key, Fv->KeySize);

    FileType = EFI_FV_FILETYPE_FREEFORM;

    do
    {
      NextStatus = Fv->GetNextFile (Fv, Key, &FileType, &NameGuid, &Attributes, &UiSectionSize);
      if (!EFI_ERROR (NextStatus))
      {

        UiSection = NULL;
        UiSectionSize = 0;
        Status = Fv->ReadSection (
                      Fv,
                      &NameGuid,
                      EFI_SECTION_USER_INTERFACE,
                      0,
                      (VOID **)&UiSection,
                      &UiSectionSize,
                      &Authentication
                      );
        if (EFI_ERROR (Status))
        {
          /* Move to the next file in the firmware volume. */
          continue;
        }

        if (StrnCmp (UiString, UiSection, UiStringLen) == 0)
        {
          // We found a UiString match.
          //
          // Read first instance found of raw section content in NameGuid file
          if ((DataBuffer != NULL) && (DataSize != NULL))
          {
            *DataBuffer     = NULL;
            *DataSize       = 0;
            Status          = Fv->ReadSection (
                                Fv,
                                &NameGuid,
                                EFI_SECTION_RAW,
                                0,
                                DataBuffer,
                                DataSize,
                                &Authentication
                                );

            bFound = TRUE;
          }
          goto clean_up;
        }

        gBS->FreePool (UiSection);
        UiSection = NULL;

      }
    } while (!EFI_ERROR (NextStatus));

    gBS->FreePool (Key);
    Key = NULL;
  }

clean_up:

  if (UiSection)
  {
     gBS->FreePool (UiSection);
     UiSection = NULL;
  }

  if (Key)
  {
     gBS->FreePool (Key);
     Key = NULL;
  }

  if (Buffer)
  {
     gBS->FreePool (Buffer);
     Buffer = NULL;
  }

  if (!EFI_ERROR(Status))
  {
    if (bFound == FALSE)
    {
      Status = EFI_NOT_FOUND;
    }
    else
    {
      Status = EFI_SUCCESS;
    }
  }

  return Status;
}

EFIAPI
EFI_STATUS ConnectByProtocol (
  EFI_GUID *ProtocolGuid
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  *Handles = NULL;
  UINTN       HandleCount;
  UINTN       i;

  if (ProtocolGuid == NULL) 
  {
    return EFI_INVALID_PARAMETER;
  } 

  Status = gBS->LocateHandleBuffer (
                           ByProtocol,
                           ProtocolGuid,
                           NULL,
                           &HandleCount,
                           &Handles);
  if (EFI_ERROR(Status))
  {
    return Status; 
  }

  if (HandleCount > 0 && Handles == NULL) 
  {
    return EFI_UNSUPPORTED;
  }

  for (i = 0; i < HandleCount; i++)
  {
    gBS->ConnectController (Handles[i], NULL, NULL, TRUE);
  }

  if (Handles != NULL) {
    FreePool (Handles);
  }

  return EFI_SUCCESS;
}

EFIAPI
EFI_STATUS
GetUUID(
  OUT VOID *Buffer,
  IN UINT32 BufferSize)
{
  EFI_STATUS                   Status;
  MEM_CARD_INFO               CardInfoData;
  EFI_HANDLE                  *SdccHandles;
  EFI_BLOCK_IO_PROTOCOL       *BlkIo;
  UINTN                        NumberOfSdccHandles;
  UINTN                        j;
  EFI_MEM_CARDINFO_PROTOCOL  *CardInfo;
  EFI_HASH2_OUTPUT Hash2Output;
  EFI_HASH2_PROTOCOL *pEfiHash2Protocol = NULL;
  EFI_GUID *HashAlgorithm = &gEfiHashAlgorithmSha1Guid;
  UINTN DigestSize = 0;

  if((Buffer == NULL) || (BufferSize > 16))   // Up to first 16 bytes of MEM_CARD_INFO can be returned in current implementation
  {
    DEBUG((EFI_D_ERROR,"Invalid parameters passed to GetUUID\n"));
    Status = EFI_INVALID_PARAMETER;
    goto ErrorExit;
  }

  Status = gBS->LocateHandleBuffer (ByProtocol,
                                    &gEfiBlockIoProtocolGuid,
                                    NULL,
                                    &NumberOfSdccHandles,
                                    &SdccHandles);
  if(Status != EFI_SUCCESS)
  {
    DEBUG((EFI_D_ERROR,"Failed to get BlkIo handles\n"));
    goto ErrorExit;
  }

  // Loop through to search for the ones we are interested in,
  // Non removable media.
  for (j = 0; j < NumberOfSdccHandles; j++)
  {

    Status = gBS->HandleProtocol (SdccHandles[j],
                                  &gEfiBlockIoProtocolGuid,
                                  (VOID **) &BlkIo);
    if (EFI_ERROR (Status))
      continue;

    if (BlkIo->Media->RemovableMedia)
      continue;

    Status = gBS->HandleProtocol (SdccHandles[j],
                                  &gEfiMemCardInfoProtocolGuid,
                                  (VOID**)&CardInfo);

    if(Status != EFI_SUCCESS)
      continue;

	Status = CardInfo->GetCardInfo (CardInfo, &CardInfoData);
	if (Status != EFI_SUCCESS)
	{
	  DEBUG((EFI_D_ERROR,"Failed to get Card Info! status: %d\n", Status));
	  goto ErrorExit;
	}
	
	if (CardInfoData.serial_num_len > 16)
	{
	  // hash the value of product_serial_num
	  Status = gBS->LocateProtocol(&gEfiHash2ProtocolGuid,
                               NULL,
                               (VOID **)&pEfiHash2Protocol);
	  if (Status != EFI_SUCCESS) {
		DEBUG((EFI_D_ERROR, "LocateProtocol: EfiHash2Protocol unsuccessful! status: %d\n", Status));
		goto ErrorExit;
	  }
	  
	  Status = pEfiHash2Protocol->GetHashSize(pEfiHash2Protocol, HashAlgorithm,
                                          &DigestSize);
										  
	  if (Status != EFI_SUCCESS) {
		DEBUG((EFI_D_ERROR, "GetHashSize unsuccessful! status: %d\n", Status));
		goto ErrorExit;
	  }
	  if (DigestSize != 20) {
		DEBUG((EFI_D_ERROR, "GetHashSize: DigestSize is not an expected value!\n"));
		goto ErrorExit;
	  }
	  
	  Status = pEfiHash2Protocol->HashInit(pEfiHash2Protocol, HashAlgorithm);
	  if (Status != EFI_SUCCESS) {
		DEBUG((EFI_D_ERROR, "HashInit unsuccessful! status: %d\n", Status));
		goto ErrorExit;
	  }
	  
	  Status = pEfiHash2Protocol->HashUpdate(pEfiHash2Protocol, (UINT8 *)&CardInfoData.product_serial_num, CardInfoData.serial_num_len);
	  if (Status != EFI_SUCCESS) {
		DEBUG((EFI_D_ERROR, "HashUpdate unsuccessful! status: %d\n", Status));
		goto ErrorExit;
	  }
	  
	  Status = pEfiHash2Protocol->HashFinal(pEfiHash2Protocol, &Hash2Output);
	  if (Status != EFI_SUCCESS) {
		DEBUG((EFI_D_ERROR, "HashFinal unsuccessful! status: %d\n", Status));
		goto ErrorExit;
	  }

	  // copy the lower bytes into the buffer
	  CopyMem(Buffer, (UINT8 *)&Hash2Output.Sha1Hash[DigestSize-BufferSize], BufferSize);
      Status = EFI_SUCCESS;
      break;
	}
	else
	{
	  CopyMem(Buffer, &CardInfoData.product_serial_num[0], MIN(CardInfoData.serial_num_len, BufferSize));
      Status = EFI_SUCCESS;
      break;
    }
  }

ErrorExit:
  return Status;

}

/**
  Enable EDL mode upon reset.

  @retval None
 */
VOID EnterEDLMode ( VOID )
{
  // Platform specific reset with EDL passed 
  gRT->ResetSystem (EfiResetPlatformSpecific, EFI_SUCCESS, StrSize(L"EDL"), L"EDL");
}


#ifdef _MSC_VER
#pragma warning( default : 4146 4244)
#endif
