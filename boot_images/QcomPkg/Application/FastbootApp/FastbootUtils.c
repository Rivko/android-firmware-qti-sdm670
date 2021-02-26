/** @file FastbootUtils.c

  Copyright (c) 2014-2015, Qualcomm Technologies, Inc. All rights reserved.<BR>
  Portions Copyright (c) 2013-2014, ARM Ltd. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

/*
 * Copyright (c) 2009, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

 /*=============================================================================
                              EDIT HISTORY


 when         who     what, where, why
 ----------   ---     -----------------------------------------------------------
 04/23/2015   vk      Add UFS support, minor cleanup
 09/12/2014   bs      Made Fastboot standalone app 
 08/15/2014   ml      Clean up
 08/15/2014   ml      Initial version

=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PrintLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include <Guid/EventGroup.h>

#include <Protocol/BlockIo.h>
#include <Protocol/DiskIo.h>
#include <Protocol/SimpleTextOut.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/EFIUsbDevice.h>

#include <Library/QcomLib.h>

#include "FastbootUtils.h"

#define MAX_DOWNLOAD_SIZE       1024*1024*100
#define MAX_DOWNLOAD_SIZE_STR   "100000000"
#define MAX_BUFFER_SIZE         MAX_DOWNLOAD_SIZE
#define ENDPOINT_IN             0x01
#define ENDPOINT_OUT            0x81
#define MAX_FASTBOOT_STR_LEN    64
#define MAX_NUM_PARTITIONS      128
#define ERASE_BUFF_SIZE         256*1024
#define ERASE_BUFF_BLOCKS       256*2
#define USB_BUFFER_SIZE         1024*1024
#define VERSION_STR_LEN         96

#define FASTBOOT_STRING_MAX_LENGTH  256
#define FASTBOOT_COMMAND_MAX_LENGTH 64


typedef struct _FASTBOOT_VAR
{
  struct _FASTBOOT_VAR *next;
  CONST CHAR8 *name;
  CONST CHAR8 *value;
} FASTBOOT_VAR;

STATIC FASTBOOT_VAR *Varlist;
STATIC EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *mTextOut;
BOOLEAN         Finished = FALSE;

typedef enum
{
  ExpectCmdState,
  ExpectDataState,
  FastbootStateMax
} ANDROID_FASTBOOT_STATE;

STATIC ANDROID_FASTBOOT_STATE mState = ExpectCmdState;

/* When in ExpectDataState, the number of bytes of data to expect: */
STATIC UINT32 mNumDataBytes;
/* .. and the number of bytes so far received this data phase */
STATIC UINT32 mBytesReceivedSoFar;
/*  and the buffer to save data into */
STATIC UINT8 *mDataBuffer = NULL;

STATIC HandleInfo HandleInfoList[MAX_NUM_PARTITIONS];

extern VOID                         *gRxBuffer;
extern VOID                         *gTxBuffer;
extern EFI_USB_DEVICE_PROTOCOL      *UsbDeviceProtocol;

//Fastboot GUID
extern EFI_GUID gUSBFastbootGuid;

//eMMC Physical Partition GUIDs
extern EFI_GUID gEfiEmmcUserPartitionGuid;

//UFS LUN GUIDs
extern EFI_GUID gEfiUfsLU0Guid;
extern EFI_GUID gEfiUfsLU1Guid;
extern EFI_GUID gEfiUfsLU2Guid;
extern EFI_GUID gEfiUfsLU3Guid;
extern EFI_GUID gEfiUfsLU4Guid;
extern EFI_GUID gEfiUfsLU5Guid;

/* Data structure to store the command list */
typedef struct _FASTBOOT_CMD
{
  struct _FASTBOOT_CMD *next;
  CONST CHAR8 *prefix;
  UINT32 prefix_len;
  VOID (*handle)(CONST CHAR8 *arg, VOID *data, UINT32 sz);
} FASTBOOT_CMD;

STATIC FASTBOOT_CMD *cmdlist;

EFI_STATUS
FastbootInit()
{
   // OEM overwrite function
   return EFI_SUCCESS;
}

/* Each Entry is dynamically Allocated,
 *  So we must deallocate on exit
 */
EFI_STATUS
FastbootUnInit()
{
  
  FASTBOOT_VAR *Var;
  FASTBOOT_VAR *VarPrev = NULL;

  for (Var = Varlist; Var->next; Var = Var->next) {
    if(VarPrev){
      FreePool(VarPrev);
    }
    VarPrev = Var;
  }
  if(Var){
    FreePool(Var);
  }
  return EFI_SUCCESS;
}

/* Implementation to find the Variable in the List */
CONST CHAR8 *
FastbootGetVar (
  IN  CONST CHAR8   *Name
  )
{
  FASTBOOT_VAR *Var;
  CHAR8         *ptr = (CHAR8 *)Name;
  for (Var = Varlist; Var; Var = Var->next) {
    if (!AsciiStrCmp(Var->name, ptr)) {
      return Var->value;
    }
  }
    return NULL; 
}

/* Publish a variable readable by the built-in getvar command
 * These Variables must not be temporary, shallow copies are used.
 */
EFI_STATUS
FastbootPublishVar (
  IN  CONST CHAR8   *Name,
  IN  CONST CHAR8   *Value
  )
{
  FASTBOOT_VAR *Var;
  Var = AllocatePool(sizeof(*Var));
  if (Var) {
    Var->next = Varlist;
    Varlist = Var;
    Var->name  = Name;
    Var->value = Value;
  }
  return EFI_SUCCESS;
}


/* Returns the Remaining amount of bytes expected
 * This lets us bypass ZLT issues
 */
UINTN GetXfrSize(VOID)
{
  UINTN BytesLeft = mNumDataBytes - mBytesReceivedSoFar;
  if (mState == ExpectDataState)
  {
    if (BytesLeft > USB_BUFFER_SIZE)
      return USB_BUFFER_SIZE;
    else
      return BytesLeft;
  }
  else
  {
    return USB_BUFFER_SIZE;
  }
}


EFI_STATUS FastbootEnvSetup(VOID *base, UINT32 size);

STATIC VOID AcceptCmd (IN  UINTN Size,IN  CONST CHAR8 *Data);

VOID FastbootAck(CONST CHAR8 *code, CONST CHAR8 *Reason)
{
  if (Reason == 0)
    Reason = "";

  AsciiSPrint(gTxBuffer, MAX_FASTBOOT_STR_LEN, "%a%a", code, Reason);
  UsbDeviceProtocol->Send(ENDPOINT_OUT, AsciiStrLen(gTxBuffer) , gTxBuffer);
  DEBUG((EFI_D_ERROR, "Sending %d:%a\n" ,AsciiStrLen(gTxBuffer), gTxBuffer));
}

VOID FastbootFail(CONST CHAR8 *Reason)
{
  FastbootAck("FAIL", Reason);
}

VOID FastbootOkay(CONST CHAR8 *info)
{
  FastbootAck("OKAY", info);
}

STATIC
EFI_STATUS
EnumeratePartitions (UINT32 *HandleCount)
{
  EFI_STATUS               Status;	
  UINT32                   MaxHandles;
  PartiSelectFilter        HandleFilter;
  UINT32                   Attribs = 0;

  if (HandleCount == NULL)
    return EFI_INVALID_PARAMETER;

  Attribs |= BLK_IO_SEL_MATCH_ROOT_DEVICE;

  MaxHandles = sizeof(HandleInfoList) / sizeof(*HandleInfoList);
  HandleFilter.PartitionType = 0;
  HandleFilter.VolumeName = 0;
  HandleFilter.RootDeviceType = &gEfiEmmcUserPartitionGuid;

  Status = GetBlkIOHandles(Attribs, &HandleFilter, HandleInfoList, &MaxHandles);
  if (EFI_ERROR (Status) || MaxHandles == 0)
  {
    MaxHandles = sizeof(HandleInfoList) / sizeof(*HandleInfoList);
    HandleFilter.PartitionType = 0;
    HandleFilter.VolumeName = 0;
    HandleFilter.RootDeviceType = &gEfiUfsLU1Guid;
	
    Status = GetBlkIOHandles(Attribs, &HandleFilter, HandleInfoList, &MaxHandles);
    if (EFI_ERROR (Status))
      return EFI_NOT_FOUND;

    *HandleCount = *HandleCount + MaxHandles;

    MaxHandles = (sizeof(HandleInfoList) / sizeof(*HandleInfoList)) - *HandleCount;
    HandleFilter.PartitionType = 0;
    HandleFilter.VolumeName = 0;
    HandleFilter.RootDeviceType = &gEfiUfsLU4Guid;
	
    Status = GetBlkIOHandles(Attribs, &HandleFilter, &HandleInfoList[*HandleCount], &MaxHandles);
    if (EFI_ERROR (Status))
      return EFI_NOT_FOUND;

    *HandleCount = *HandleCount + MaxHandles;
  }
  else
  {
    *HandleCount = MaxHandles;
  }

  return Status;
}


EFI_STATUS
FastbootFlashPartition (
  IN CHAR8  *PartitionName,
  IN UINTN   Size,
  IN VOID   *Image
  )
{
  EFI_STATUS               Status;
  EFI_BLOCK_IO_PROTOCOL   *BlockIo = NULL;
  EFI_DISK_IO_PROTOCOL    *DiskIo;
  UINT32                   MediaId;
  UINTN                    PartitionSize;
  CHAR8                    PartitionNameAscii[60];
  BOOLEAN                  PartitionFound;
  UINT32                   HandleCount = 0;
  UINT16                   i;
  EFI_PARTITION_ENTRY     *PartEntry;
  EFI_HANDLE              *Handle = NULL;

  Status = EnumeratePartitions (&HandleCount);
  if (Status != EFI_SUCCESS)
    return Status;	  
  
  PartitionFound = FALSE;
  for (i = 0; i < HandleCount; i++)
  {
    Status = gBS->HandleProtocol(HandleInfoList[i].Handle, &gEfiPartitionRecordGuid, (VOID **)&PartEntry);
    if (EFI_ERROR (Status)) {
         continue;
    }
    UnicodeStrToAsciiStr(PartEntry->PartitionName, PartitionNameAscii);

    if (AsciiStrCmp (PartitionName, PartitionNameAscii) == 0) {
      PartitionFound = TRUE;
      BlockIo = HandleInfoList[i].BlkIo;
      Handle = HandleInfoList[i].Handle;
      break;
    }
  }
  if (!PartitionFound)
  {
    AsciiPrint("Partition not found\n");
    return EFI_NOT_FOUND;
  }

  // Check image will fit on device
  PartitionSize = (BlockIo->Media->LastBlock + 1) * BlockIo->Media->BlockSize;
  if (PartitionSize < Size)
  {
    DEBUG ((EFI_D_ERROR, "Partition not big enough.\n"));
    DEBUG ((EFI_D_ERROR, "Partition Size:\t%d\nImage Size:\t%d\n", PartitionSize, Size));

    return EFI_VOLUME_FULL;
  }

  MediaId = BlockIo->Media->MediaId;

  Status = gBS->OpenProtocol (
    Handle,
    &gEfiDiskIoProtocolGuid,
    (VOID **) &DiskIo,
    gImageHandle,
    NULL,
    EFI_OPEN_PROTOCOL_GET_PROTOCOL
  );

  ASSERT_EFI_ERROR (Status);
  if (Image == NULL) {
     DEBUG((EFI_D_ERROR, "No image to flash\n"));
     return EFI_NO_MEDIA;
  }
  Status = DiskIo->WriteDisk (DiskIo, MediaId, 0, Size, Image);
  if (EFI_ERROR (Status))
    return Status;

  BlockIo->FlushBlocks(BlockIo);
  return Status;
}

EFI_STATUS
FastbootErasePartition (IN CHAR8 *PartitionName)
{
  EFI_STATUS               Status;
  EFI_BLOCK_IO_PROTOCOL   *BlockIo = NULL;
  UINT32                   MediaId;
  UINT64                   PartitionSize;
  CHAR8                    PartitionNameAscii[60];
  BOOLEAN                  PartitionFound;
  UINT32                   HandleCount = 0;
  UINT64                   i;
  UINT8                   *Zeros;
  EFI_PARTITION_ENTRY     *PartEntry;

  Zeros = AllocateZeroPool (ERASE_BUFF_SIZE);
  if (Zeros == NULL)
  {
    DEBUG ((EFI_D_ERROR, "Allocation failed \n"));
    return EFI_OUT_OF_RESOURCES;
  }

  Status = EnumeratePartitions (&HandleCount);
  if (Status != EFI_SUCCESS)
    return Status;	  
  
  PartitionFound = FALSE;
  for (i = 0; i < HandleCount; i++)
  {
      Status = gBS->HandleProtocol(HandleInfoList[i].Handle, &gEfiPartitionRecordGuid, (VOID **)&PartEntry);
      if (EFI_ERROR (Status))
        continue;

      UnicodeStrToAsciiStr(PartEntry->PartitionName, PartitionNameAscii);

      if (AsciiStrCmp (PartitionName, PartitionNameAscii) == 0)
      {
        PartitionFound = TRUE;
        BlockIo = HandleInfoList[i].BlkIo;
        break;
      }
  }

  if (!PartitionFound)
  {
    if (Zeros)
      FreePool (Zeros);
    return EFI_NOT_FOUND;
  }

  MediaId = BlockIo->Media->MediaId;
  //PartitionSize = (BlockIo->Media->LastBlock + 1) * BlockIo->Media->BlockSize;
  PartitionSize = (BlockIo->Media->LastBlock + 1);

  /* Write 256 K no matter what unless its smaller.*/
  i = 0;
  //AsciiPrint("eraseing size: %ld\n",PartitionSize);
  while (PartitionSize > 0)
  {
    if (PartitionSize > ERASE_BUFF_BLOCKS)
    {
      BlockIo->WriteBlocks(BlockIo,  MediaId, i, ERASE_BUFF_SIZE, Zeros);
      i += ERASE_BUFF_BLOCKS;
      PartitionSize = PartitionSize - ERASE_BUFF_BLOCKS;
      // AsciiPrint("erase 256Kb\n");
    }
    else{
      BlockIo->WriteBlocks(BlockIo,  MediaId, i, PartitionSize * BlockIo->Media->BlockSize, Zeros);
      PartitionSize = 0;
    }
  }

  if (Zeros)
    FreePool (Zeros);

  return Status;
}

STATIC VOID HandleDownload (IN CHAR8 *NumBytesString)
{
  CHAR8       Response[12] = "DATA";
  CHAR16      OutputString[FASTBOOT_STRING_MAX_LENGTH];

  // Argument is 8-character ASCII string hex representation of number of bytes
  // that will be sent in the data phase.
  // Response is "DATA" + that same 8-character string.

  // Parse out number of data bytes to expect
  mNumDataBytes = AsciiStrHexToUint64 (NumBytesString);
  if (mNumDataBytes == 0) {
    mTextOut->OutputString (mTextOut, L"ERROR: Fail to get the number of bytes to download.\r\n");
    FastbootFail("Failed to get the number of bytes to download");
    return;
  }

  UnicodeSPrint (OutputString, sizeof (OutputString), L"Downloading %d bytes\r\n", mNumDataBytes);
  mTextOut->OutputString (mTextOut, OutputString);
  AsciiStrnCpy (Response + 4, NumBytesString, 8);
  CopyMem(gTxBuffer, Response, 12);
  mState = ExpectDataState;
  mBytesReceivedSoFar = 0;
  UsbDeviceProtocol->Send(ENDPOINT_OUT, 12 , gTxBuffer);
  DEBUG((EFI_D_ERROR, "HandleDownload: Send 12 %a\n", gTxBuffer));
}

STATIC VOID HandleFlash ( IN CHAR8 *PartitionName )
{
  EFI_STATUS  Status;
  if (mDataBuffer == NULL)
  {
    // Doesn't look like we were sent any data
    FastbootFail("No data to flash");
    return;
  }

  Status = FastbootFlashPartition (
             PartitionName,
             mNumDataBytes,
             mDataBuffer
  );

  if (Status == EFI_NOT_FOUND)
  {
    FastbootFail("No such partition.");
    mTextOut->OutputString (mTextOut, L"No such partition.\r\n");
  }
  else if (EFI_ERROR (Status))
  {
    FastbootFail("Error flashing partition.");
    mTextOut->OutputString (mTextOut, L"Error flashing partition.\r\n");
    DEBUG ((EFI_D_ERROR, "Couldn't flash image:  %r\n", Status));
  }
  else
  {
    mTextOut->OutputString (mTextOut, L"Done.\r\n");
    FastbootOkay("");
  }
}
STATIC VOID HandleErase ( IN CHAR8 *PartitionName )
{
  EFI_STATUS  Status;
  CHAR16      OutputString[FASTBOOT_STRING_MAX_LENGTH];

  // Build output string
  UnicodeSPrint (OutputString, sizeof (OutputString), L"Erasing partition %a\r\n", PartitionName);
  mTextOut->OutputString (mTextOut, OutputString);

  Status = FastbootErasePartition (PartitionName);
  if (EFI_ERROR (Status)) {
    FastbootFail("Check device console.");
    DEBUG ((EFI_D_ERROR, "Couldn't erase image:  %r\n", Status));
  } else {
    FastbootOkay("");
  }
}

/* Boot is currently not supported */
STATIC VOID HandleBoot ( VOID )
{
 /* EFI_STATUS Status;

  mTextOut->OutputString (mTextOut, L"Booting downloaded image\r\n");

  if (mDataBuffer == NULL) {
    // Doesn't look like we were sent any data
    FastbootFail("No image in memory");
    return;
  }

  // We don't really have any choice but to report success, because once we
  // boot we lose control of the system.
  FastbootOkay("");

  Status = BootAndroidBootImg (mNumDataBytes, mDataBuffer);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Failed to boot downloaded image: %r\n", Status));
  }
  */
    FastbootFail("Boot Not Supported");
}

STATIC VOID AcceptData (IN  UINTN  Size, IN  VOID  *Data)
{
  UINT32 RemainingBytes = mNumDataBytes - mBytesReceivedSoFar;
  CHAR16 OutputString[FASTBOOT_STRING_MAX_LENGTH];
  STATIC UINTN Count = 0;

  /* Protocol doesn't say anything about sending extra data so just ignore it. */
  if (Size > RemainingBytes)
  {
    Size = RemainingBytes;
  }

  CopyMem (&mDataBuffer[mBytesReceivedSoFar], Data, Size);
  mBytesReceivedSoFar += Size;

  /* Show download progress. Don't do it for every packet  as outputting text
   * might be time consuming - do it on the last packet and on every 32nd packet
   */
  if ((Count++ % 32) == 0 || Size == RemainingBytes)
  {
    UnicodeSPrint (
      OutputString,
      sizeof (OutputString),
      L"%8d / %8d bytes downloaded (%d%%)\n",
      mBytesReceivedSoFar,
      mNumDataBytes,
      (mBytesReceivedSoFar * 100) / mNumDataBytes // percentage
      );
    mTextOut->OutputString (mTextOut, OutputString);
  }

  /* Either queue the max transfer size 1 MB
     or only queue the remaining amount of data
     left to avoid zlt issues
   */
  if (mBytesReceivedSoFar == mNumDataBytes)
  {
    /* Download Finished */
    mTextOut->OutputString (mTextOut, L"\r\n");
    FastbootOkay("");
    mState = ExpectCmdState;
  }else
  {
    UsbDeviceProtocol->Send(ENDPOINT_IN, GetXfrSize(), gRxBuffer);
    DEBUG((EFI_D_ERROR, "AcceptData: Send %d: %a\n", GetXfrSize(), gTxBuffer));
  }
}

/*
  This is the NotifyFunction passed to CreateEvent in the FastbootAppEntryPoint
  It will be called by the UEFI event framework when the transport protocol
  implementation signals that data has been received from the Fastboot host.
  The parameters are ignored.
*/
VOID DataReady (IN UINTN    Size, IN VOID    *Data)
{
  DEBUG((EFI_D_ERROR, "DataReady %d\n", Size));
  if (mState == ExpectCmdState) 
    AcceptCmd (Size, (CHAR8 *) Data);
  else if (mState == ExpectDataState)
    AcceptData (Size, Data);
  else
    ASSERT (FALSE);
}

STATIC VOID FatalErrorNotify (IN EFI_EVENT  Event, IN VOID      *Context)
{
  mTextOut->OutputString (mTextOut, L"Fatal error sending command response. Exiting.\r\n");
  Finished = TRUE;
}

/* This function must be called to deallocate the USB buffers, as well
 * as the main Fastboot Buffer. Also Frees Variable data Structure
 */
EFI_STATUS
FastbootAppUnInit (VOID)
{
  if (mDataBuffer)
    FreePool(mDataBuffer);
  FastbootUnInit();
  UsbDeviceProtocol->Stop();
  return EFI_SUCCESS;
}

EFI_STATUS
FastbootAppInit (VOID)
{
  EFI_STATUS                      Status;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL  *TextIn;
  EFI_EVENT                       mFatalSendErrorEvent;
  CHAR8                           *FastBootBuffer;

  mDataBuffer = NULL;
  
  /* Initialize the Fastboot Platform Protocol */
  Status = FastbootInit();
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Fastboot: Couldn't initialise Fastboot Protocol: %r\n", Status));
    return Status;
  }

  /* Locate the Fastboot USB Transport Protocol UsbDevice */
  Status = gBS->LocateProtocol (&gEfiSimpleTextOutProtocolGuid, NULL, (VOID **) &mTextOut);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Fastboot: Couldn't open UsbDevice Protocol: %r\n", Status));
    return Status;
  }

  /* Locate the text output Protocol, used by mText */
  Status = gBS->LocateProtocol (&gEfiSimpleTextInProtocolGuid, NULL, (VOID **) &TextIn);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Fastboot: Couldn't open Text Input Protocol: %r\n", Status));
    return Status;
  }

  /* Disable watchdog */
  Status = gBS->SetWatchdogTimer (0, 0x10000, 0, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Fastboot: Couldn't disable watchdog timer: %r\n", Status));
  }

  // Create event to pass to FASTBOOT_PROTOCOL.Send, signalling a
  // fatal error
  Status = gBS->CreateEvent (
                 EVT_NOTIFY_SIGNAL,
                 TPL_CALLBACK,
                 FatalErrorNotify,
                 NULL,
                 &mFatalSendErrorEvent
                 );
  ASSERT_EFI_ERROR (Status);

  mTextOut->OutputString (mTextOut,
      L"Android Fastboot mode - version " ANDROID_FASTBOOT_VERSION ". Press any key to quit.\r\n");

  /* Allocate buffer used to store images passed by the download command */
  FastBootBuffer = AllocatePool(MAX_BUFFER_SIZE);
  if (!FastBootBuffer) 
  {
    AsciiPrint("Not enough memory to Allocate Fastboot Buffer");
    ASSERT(FALSE);
  }

  /* Another init called, artifact of porting from LK */
  FastbootEnvSetup( (void*) FastBootBuffer, MAX_BUFFER_SIZE);
  return EFI_SUCCESS;
}

/* See header for documentation */
VOID FastbootRegister(CONST CHAR8 *prefix,
           VOID (*handle)(CONST CHAR8 *arg, VOID *data, UINT32 sz))
{
  FASTBOOT_CMD *cmd;
  cmd = AllocatePool(sizeof(*cmd));
  if (cmd)
  {
    cmd->prefix = prefix;
    cmd->prefix_len = AsciiStrLen(prefix);
    cmd->handle = handle;
    cmd->next = cmdlist;
    cmdlist = cmd;
  }
}

STATIC VOID CmdReboot(CONST CHAR8 *arg, VOID *data, UINT32 sz)
{
    AsciiPrint("rebooting");
    FastbootOkay("");
    gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);

    // Shouldn't get here
    DEBUG ((EFI_D_ERROR, "Fastboot: gRT->Resetystem didn't work\n"));
    FastbootFail("Failed to reboot");
}

STATIC VOID CmdContinue(CONST CHAR8 *arg, VOID *data, UINT32 sz)
{
    FastbootOkay("");
    AsciiPrint("Received 'continue' command. Exiting Fastboot mode\n");
    Finished = TRUE;
}

STATIC VOID CmdGetVar(CONST CHAR8 *arg, VOID *data, UINT32 sz)
{
    CONST CHAR8 *ptr = FastbootGetVar(arg);
    FastbootOkay(ptr);
}


STATIC VOID CmdBoot(CONST CHAR8 *arg, VOID *data, UINT32 sz)
{
    HandleBoot ();
}

STATIC VOID CmdFlash(CONST CHAR8 *arg, VOID *data, UINT32 sz)
{
  CHAR8 *Command = (CHAR8 *) arg;
  HandleFlash (Command);
}

STATIC VOID CmdErase(CONST CHAR8 *arg, VOID *data, UINT32 sz)
{
    CHAR8 *Command = (CHAR8 *) arg;
    HandleErase(Command);
}

STATIC VOID CmdPowerDown(CONST CHAR8 *arg, VOID *data, UINT32 sz)
{
    FastbootOkay("");
    gRT->ResetSystem (EfiResetShutdown, EFI_SUCCESS, 0, NULL);

    /* Shouldn't get here */
    DEBUG ((EFI_D_ERROR, "Fastboot: gRT->ResetSystem didn't work\n"));
}

STATIC VOID CmdDownload(CONST CHAR8 *arg, VOID *data, UINT32 sz)
{
    HandleDownload((CHAR8 *)arg);
}

STATIC VOID AcceptCmd (IN UINTN  Size,IN  CONST CHAR8 *Data)
{
  CHAR8    *CmdBuffer;
  FASTBOOT_CMD *cmd;
  
  CmdBuffer = AllocateZeroPool (1024);
  if (CmdBuffer == NULL)
  {
    AsciiPrint("\nAllocation Failed\n");
    FastbootFail("Allocation Failed");
    return;
  }

  AsciiStrnCpy(CmdBuffer, Data, Size);
  
  CmdBuffer[Size] = 0;
  DEBUG((EFI_D_ERROR, "Cmd: %a\n", CmdBuffer));
  if (!Data)
  {
      FastbootFail("no given command");
      if (CmdBuffer)
        FreePool (CmdBuffer);
      return;
  }

  for (cmd = cmdlist; cmd; cmd = cmd->next)
  {
      if (AsciiStrnCmp(CmdBuffer, cmd->prefix, cmd->prefix_len))
          continue;
      cmd->handle((CONST CHAR8*) CmdBuffer + cmd->prefix_len, (VOID *) mDataBuffer, mBytesReceivedSoFar);
      {
        if (CmdBuffer)
          FreePool (CmdBuffer);
        return;
      }
  }
  AsciiPrint("\nFastboot Send Fail\n");
  FastbootFail("unknown command");

  if (CmdBuffer)
    FreePool (CmdBuffer);
}

/* Registers all Stock commands, Publishes all stock variables
 * and partitiion sizes. base and size are the respective parameters
 * to the Fastboot Buffer used to store the downloaded image for flashing
 */
EFI_STATUS FastbootEnvSetup(VOID *base, UINT32 size)
{
  mDataBuffer = base;
  mNumDataBytes = size;

  /* Find all Software Partitions in the User Partition */
  EFI_STATUS               Status;
  EFI_BLOCK_IO_PROTOCOL   *BlockIo = NULL;
  UINTN                    PartitionSize ;
  UINT32                   HandleCount = 0;
  UINT16                   i;
  EFI_PARTITION_ENTRY     *PartEntry;
  CHAR8                    StrProduct[64];
  CHAR8                    FullProduct[64];
  CHAR8                   *PlatformTypePtr, *ChipNamePtr, *ChipVerPtr;
  CHAR8                    VersionStr[VERSION_STR_LEN];
  UINTN                    StrSize = VERSION_STR_LEN;
  CHAR8                    PartitionSizeStr[MAX_FASTBOOT_STR_LEN];
  UINT32                   NumPartitions = 0;

  STATIC CHAR8             AsciiPartValues[MAX_NUM_PARTITIONS][MAX_FASTBOOT_STR_LEN];
  STATIC CHAR8             AsciiPartNames[MAX_NUM_PARTITIONS][MAX_FASTBOOT_STR_LEN];
  STATIC CHAR8             AsciiPartNamesTemp[MAX_FASTBOOT_STR_LEN];

  Status = EnumeratePartitions (&HandleCount);
  if (Status != EFI_SUCCESS)
    return Status;	  

  for (i = 0; i < HandleCount; i++)
  {
    Status = gBS->HandleProtocol(HandleInfoList[i].Handle, &gEfiPartitionRecordGuid, (VOID **)&PartEntry);
    if (EFI_ERROR (Status)) 
      continue;

    UnicodeStrToAsciiStr(PartEntry->PartitionName, AsciiPartNamesTemp);
    AsciiSPrint(AsciiPartNames[NumPartitions], sizeof(AsciiPartNames[NumPartitions]),"partition-size:%a", AsciiPartNamesTemp);
    BlockIo = HandleInfoList[i].BlkIo;
    PartitionSize =  (BlockIo->Media->LastBlock + 1) * BlockIo->Media->BlockSize ;
    AsciiSPrint(PartitionSizeStr, sizeof(PartitionSizeStr), "%d", PartitionSize);
    AsciiStrCpy(AsciiPartValues[NumPartitions], PartitionSizeStr);

    FastbootPublishVar(AsciiPartNames[NumPartitions],AsciiPartValues[NumPartitions]);
    NumPartitions++;
  }
  

  //Get FW version and publish
  Status = gRT->GetVariable (L"FwVerStr", &gQcomTokenSpaceGuid,
    NULL, &StrSize, VersionStr);

  if (EFI_ERROR (Status))
  {
    //DEBUG((EFI_D_ERROR, "\nQualcomm UEFI FW Version : Invalid or Unset\n"));
    FastbootPublishVar("version", "0.5");
  } 
  else
  {
    //DEBUG((EFI_D_ERROR,"\nQualcomm UEFI FW Version : %a\n", VersionStr));
    FastbootPublishVar("version", VersionStr);
  }

  //Build Product name and publish 
  Status = GetPlatformStrings(StrProduct, sizeof(StrProduct),
                      &PlatformTypePtr, &ChipNamePtr, &ChipVerPtr);
  
  AsciiStrCpy(FullProduct, PlatformTypePtr);
  AsciiStrCat(FullProduct, ":");
  AsciiStrCat(FullProduct, ChipNamePtr);
  AsciiStrCat(FullProduct, ":");
  AsciiStrCat(FullProduct, ChipVerPtr);
  FastbootPublishVar("product", FullProduct);
  
  /* These are the rest of the variables */
  FastbootPublishVar("kernel", "uefi");
  FastbootPublishVar("max-download-size", MAX_DOWNLOAD_SIZE_STR);

  /* Thses are the stock commands */
  FastbootRegister("reboot", CmdReboot);
  FastbootRegister("continue", CmdContinue);
  FastbootRegister("getvar:", CmdGetVar);
  FastbootRegister("boot:", CmdBoot);
  FastbootRegister("flash:", CmdFlash);
  FastbootRegister("erase:", CmdErase);
  FastbootRegister("powerdown:", CmdPowerDown);
  FastbootRegister("download:", CmdDownload);

  return EFI_SUCCESS;
}
