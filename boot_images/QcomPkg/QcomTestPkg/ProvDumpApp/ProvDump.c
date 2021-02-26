/** @file ProvDump.c
   
  Qualcomm Provision Data Dump application

  Copyright (c) 2010-2013, Qualcomm Technologies, Inc. All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.
  
  Portions Copyright (c) 2009 - 2012, Intel Corporation. All rights reserved.
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
 06/05/13   mic     Fixed Klocwork warnings
 05/17/13   jd      Fixed uninitialized local variables
 05/14/13   jd      Fixed Klocwork warnings
 03/21/13   jd      Fixed Linux build issue
 03/06/13   jd      Added option to dump provision item list and content
 10/22/12   jmw     New File

=============================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/QcomLib.h>
#include <Guid/GlobalVariable.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/EFIDpp.h>
#include <Protocol/BlockIo.h>
#include <Protocol/SimpleFileSystem.h>
#include <Guid/FileInfo.h>

#include "../../Drivers/DppDxe/Dpp.h"

#define MAC_ADDR_SIZE 6
#define WLAN_NUM_MAC_ADDR 4

#define OUTPUT_DIR_NAME   L"ProvDump"

typedef struct _MACADDR
{
  UINT8 bytes[ MAC_ADDR_SIZE ];
} MACADDR;

typedef struct 
{
  UINT8 version;
  UINT8 length;
  UINT8 numMacAddr;
  MACADDR macAddress[WLAN_NUM_MAC_ADDR];
} data_wlan_mac_addr;

typedef struct 
{
  UINT8 version;
  UINT8 length;
  MACADDR macAddress;
} data_bt_mac_addr;

#define OUTPUT_WIDTH  16

typedef enum Operation_tag
{
  OP_UNKNOWN = 0,
  OP_LIST_PROVISION_DATA,
  OP_DUMP_QCOM_PROVISION_DATA,
  OP_SAVE_PROVISION_DATA,
  OP_DUMP_GENERIC_PROVISION_DATA
} Operation;

#define ALIGN_TO_BLOCK(Size, BlockSize, AlignedSize) \
  { \
    AlignedSize = Size + (BlockSize - 1); \
    AlignedSize = AlignedSize & ~(BlockSize -1); \
  }

static UINT32 gDppBlockSize = 0;
static UINT64 gDppSize = 0;
static EFI_BLOCK_IO_PROTOCOL  *gDppBlockIo = NULL;

static EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *gEfiSimpleFileSystemProtocol = NULL;
static EFI_FILE_PROTOCOL               *gFsRoot = NULL;

static EFI_HANDLE gImageHandle = NULL;
static EFI_LOADED_IMAGE_PROTOCOL *gLoadedImage  = NULL;

static EFI_GUID gEfiDppPartitionGuid = 
  {0x9992FD7D, 0xEC66, 0x4CBC, {0xA3, 0x37, 0x0D, 0xA1, 0xD4, 0xC9, 0x3F, 0x8F}};

EFI_STATUS
GetFileInfo(
  IN EFI_FILE_PROTOCOL  *File,
  IN OUT UINTN          *FileInfoSize,
  IN OUT EFI_FILE_INFO **FileInfo
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
    
  if ((File == NULL) || (FileInfoSize == NULL) || (FileInfo == NULL))
  {
    Status = EFI_INVALID_PARAMETER;
    goto GetFileInfoExit;
  }

  *FileInfoSize = 0;
  *FileInfo = NULL;

  /* Get the required FileInfo buffer size by giving *FileInfoSize = 0,
     Status is expected to be EFI_BUFFER_TOO_SMALL*/
  Status = File->GetInfo(File,
                         &gEfiFileInfoGuid,
                         FileInfoSize,
                         (VOID*)*FileInfo);

  if(Status == EFI_BUFFER_TOO_SMALL)
  {    
    Status = gBS->AllocatePool(EfiBootServicesData,
                               *FileInfoSize,
                               (VOID**)FileInfo);
    if (EFI_ERROR(Status)) 
    {
      goto GetFileInfoExit;
    }
    
    Status = File->GetInfo(File,
                           &gEfiFileInfoGuid,
                           FileInfoSize,
                           (VOID*)*FileInfo);
    if (EFI_ERROR(Status)) 
    {
      goto GetFileInfoExit;
    }
  }
  else if( EFI_ERROR(Status))
  {
    goto GetFileInfoExit;
  }
  else
  {
    /* This is to handle EFI_SUCCESS case, status shouldn't be EFI_SUCCESS if
       *FileInfoSize is 0.*/
    Status = EFI_UNSUPPORTED;
    goto GetFileInfoExit;
  }
  
  return Status;

GetFileInfoExit:

  if(FileInfo != NULL && *FileInfo != NULL)
  {
    gBS->FreePool(*FileInfo);
    *FileInfo = NULL;
  }
  return Status;
}

EFI_STATUS
UpdateFileInfoTime(
  IN OUT EFI_TIME *Time,
  IN     EFI_TIME *CurTime
  )
{

  if (Time == NULL || CurTime == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  if (Time->Year <= 1980 ||
       Time->Month < 1 ||
       Time->Month > 12 ||
       Time->Day < 1 ||
       Time->Day > 31 ||
       Time->Hour > 23 ||
       Time->Minute > 59 ||
       Time->Second > 59 ||
       Time->Nanosecond > 999999999
       ) 
  {
    Time->Year = 2012;
    Time->Month = 1;
    Time->Day = 1;
    Time->Hour = 1;
    Time->Minute = 1;
    Time->Second = 1;
    Time->Nanosecond = 1;
  
  }

  return EFI_SUCCESS;
}

EFI_STATUS
UpdateFileInfo(
  IN EFI_FILE_PROTOCOL *File
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINTN FileInfoSize = 0;
  EFI_FILE_INFO *FileInfo = NULL;
  EFI_GUID FileInfoGuid = EFI_FILE_INFO_ID ;
  EFI_TIME CurTime = {0};

  Status = GetFileInfo(File, &FileInfoSize, &FileInfo);
  if(EFI_ERROR(Status) || FileInfo == NULL)
  {
    AsciiPrint("ERROR: ProvDump - Failed to get file info! Error Code: 0x%08X\r\n", Status);
    goto UpdateFileInfoExit;
  }
  
  //Clear out the read only attribute
  if((FileInfo->Attribute) & EFI_FILE_READ_ONLY)
  {
    FileInfo->Attribute = (FileInfo->Attribute) & (0xFFFFFFFFFFFFFFFE) ;
  }

  Status = gRT->GetTime(&CurTime, NULL);
  if (EFI_ERROR(Status)) 
  {
    //AsciiPrint("ERROR: ProvDump - Failed to get current time! Error Code: 0x%08X\r\n", Status);
    //goto UpdateFileInfoExit;
  }

  UpdateFileInfoTime(&FileInfo->CreateTime, &CurTime);
  UpdateFileInfoTime(&FileInfo->ModificationTime, &CurTime);
  UpdateFileInfoTime(&FileInfo->LastAccessTime, &CurTime);

  Status = File->SetInfo (File, &FileInfoGuid, FileInfo->Size, FileInfo);

UpdateFileInfoExit:

  return Status;
}

/**
  Initialize global variables which will be used to access DPP
**/
EFI_STATUS
GetDppInfo()
{
  EFI_STATUS  Status = EFI_SUCCESS;
  EFI_HANDLE  *HandleBuffer = NULL;
  UINTN       NumOfHandles = 0;
  EFI_HANDLE  DppHandle = NULL;
  EFI_GUID    *PartType = NULL;
  UINTN       HandleIndex = 0;
    
  // Locate DPP partition handler
  Status = gBS->LocateHandleBuffer(ByProtocol,
                                   &gEfiBlockIoProtocolGuid,
                                   NULL,
                                   &NumOfHandles,
                                   &HandleBuffer);

  if(EFI_ERROR(Status))
  {
    AsciiPrint("ERROR: ProvDump - Failed to locate Block IO protocol handlers! "
               "Error Code: 0x%08X\r\n",
               Status);
    goto GetDppInfoExit;
  }

  for(HandleIndex = 0; HandleIndex < NumOfHandles; HandleIndex++)
  {
    // Query the handle
    Status = gBS->HandleProtocol(HandleBuffer[HandleIndex],
                                 &gEfiPartitionTypeGuid,
                                 (VOID**)&PartType);
    
    if(EFI_ERROR(Status))
    {
      continue;
    }

    if(CompareGuid(PartType, &gEfiDppPartitionGuid))
    {
      DppHandle = HandleBuffer[HandleIndex];
      break;
    }                    
  }

  if(DppHandle == NULL)
  {
    Status = EFI_UNSUPPORTED;
    goto GetDppInfoExit;
  }

  // Get Block IO protocol interface to DPP handle
  Status = gBS->HandleProtocol(DppHandle,
                               &gEfiBlockIoProtocolGuid,
                               (VOID**)&gDppBlockIo);

  if(EFI_ERROR(Status))
  {
    AsciiPrint("ERROR: ProvDump - Failed to locate Block IO protocol for DPP! "
               "Error Code: 0x%08X\r\n",
               Status);
    goto GetDppInfoExit;
  }
  
  gDppBlockSize = gDppBlockIo->Media->BlockSize;
  if(gDppBlockSize == 0)
  {
    gDppBlockSize = 512;
  }

  gDppSize = gDppBlockIo->Media->BlockSize * ((gDppBlockIo->Media->LastBlock) + 1);
  if(gDppSize == 0)
  {
    gDppSize = 16 * 1024 * 1024;
  }

GetDppInfoExit:

  if(HandleBuffer != NULL)
  {
    FreePool(HandleBuffer);
  }

  return Status;
}

EFI_STATUS
ReadDPPBlocks(
  IN UINT32       Offset,
  IN UINT32       Length,
  OUT VOID        *Buffer
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  // Check input parameters
  if((Buffer == NULL) ||
     (Length == 0))
  {
    Status = EFI_INVALID_PARAMETER;
    goto ReadDPPBlocksExit;
  }

  if((Length & (gDppBlockSize - 1)) != 0)
  {
    Status = EFI_INVALID_PARAMETER;
    goto ReadDPPBlocksExit;
  }

  // Read block data
  Status = gDppBlockIo->ReadBlocks(gDppBlockIo,
                                   gDppBlockIo->Media->MediaId,
                                   (EFI_LBA)Offset,
                                   Length,
                                   Buffer);

  if(EFI_ERROR(Status))
  {
    AsciiPrint("ERROR: ProvDump - Failed to read block data from DPP! "
               "Error Code: 0x%08X\r\n",
               Status);
    goto ReadDPPBlocksExit;
  }

  Status = gDppBlockIo->FlushBlocks(gDppBlockIo);

  if(EFI_ERROR(Status))
  {
    AsciiPrint("ERROR: ProvDump - Failed to flush blocks! "
               "Error Code: 0x%08X\r\n",
               Status);
    goto ReadDPPBlocksExit;
  }

ReadDPPBlocksExit:

  return Status;
}

EFI_STATUS
DisplayProvItems()
{
  EFI_STATUS              Status = EFI_SUCCESS;
  VOID                    *Buffer = NULL;
  UINTN                   BufferSize = 0;
  UINT32                  Index = 0;
  UINT32                  NumOfEntriesUsed = 0;
  PDPP_HEADER             pDppHeader = NULL;
  PDPP_DIRECTORY_HEADER   pDppDirHeader = NULL;
  PDPP_DATA_ENTRY         pDppDataEntry = NULL;
    
  // Read DPP header, directory header, and directory entries
  BufferSize = sizeof(DPP_HEADER) + 
               sizeof(DPP_DIRECTORY_HEADER) +
               sizeof(DPP_DATA_ENTRY) * DPP_MAX_DATA_ENTRIES;

  ALIGN_TO_BLOCK(BufferSize, gDppBlockSize, BufferSize);

  Buffer = AllocateZeroPool(BufferSize);
  
  if (Buffer == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto DisplayProvItemsExit;
  }

  Status = ReadDPPBlocks(DPP_HEADER_LBA, BufferSize, Buffer);

  if(EFI_ERROR(Status))
  {
    AsciiPrint("ERROR: ProvDump - Failed to read DPP block data at LBA %d, size %d! "
               "Error Code: 0x%08X\r\n",
               DPP_HEADER_LBA,
               BufferSize,
               Status);
    goto DisplayProvItemsExit;
  }
    
  pDppHeader = (PDPP_HEADER)Buffer;
  pDppDirHeader = (PDPP_DIRECTORY_HEADER)((PDPP_HEADER)Buffer + 1);

  // Check validity of DPP
  if((pDppHeader->Signature != DPP_SIGNATURE) ||
     (pDppHeader->Revision != DPP_REVISION) ||
     (pDppHeader->Size != sizeof(DPP_HEADER)) ||
     (pDppDirHeader->Size != sizeof(DPP_DIRECTORY_HEADER)) ||
     (pDppDirHeader->DataEntriesAllocated != DPP_MAX_DATA_ENTRIES))
  {
    AsciiPrint("ERROR: ProvDump - DPP is not in a valid state!\r\n");
    Status = EFI_ABORTED;
    goto DisplayProvItemsExit;
  }

  pDppDataEntry = (PDPP_DATA_ENTRY)((UINT8*)Buffer + pDppDirHeader->DataEntriesOffset);

  NumOfEntriesUsed = pDppDirHeader->DataEntriesUsed;
  
  if(NumOfEntriesUsed  > DPP_MAX_DATA_ENTRIES)
  {
    AsciiPrint("ERROR: ProvDump - DPP is not in a valid state!\r\n");
    Status = EFI_ABORTED;
    goto DisplayProvItemsExit;
  }

  AsciiPrint("\n");
  AsciiPrint("%d provision item(s) found.\n", NumOfEntriesUsed);
  AsciiPrint("--------------------------------------------\n");

  for (Index = 0; Index < NumOfEntriesUsed; Index++) 
  {
    if (pDppDataEntry) 
    {
      AsciiPrint("Item No : %d\n", Index+1);
      AsciiPrint("   Type : %s\n", pDppDataEntry->Type);
      AsciiPrint("   Name : %s\n", pDppDataEntry->Name);
      AsciiPrint("  Valid : %s\n", pDppDataEntry->Valid == 0 ? L"N" : L"Y");
      AsciiPrint("--------------------------------------------\n");
    }
    pDppDataEntry++;
  }

  AsciiPrint("\n");
    
DisplayProvItemsExit:

  if(Buffer != NULL)
  {
    FreePool(Buffer);
  }

  return Status;
}

EFI_STATUS
EFIAPI
PrintDataInHexAndChar (
  IN UINT8 *pData,
  IN UINT32 Size
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINTN  i = 0;
  UINTN  j = 0;
  UINT32 NumCols = OUTPUT_WIDTH;
  UINT32 NumRows = 0;
  UINT32 LastRowColumn = 0;
  UINT8 cData = 0;
  
  NumRows = Size / NumCols;
  LastRowColumn = Size % NumCols;
  NumRows = LastRowColumn == 0 ? NumRows : (NumRows + 1);

  AsciiPrint("-------------------------------------------------------------------\n");
  for (i = 0; i < NumRows; i++)
  {
    if (i == NumRows - 1) 
    {
      NumCols = LastRowColumn;
    }
    
    for (j = 0; j < NumCols; j++) 
    {
      AsciiPrint("%02X ", pData[i*OUTPUT_WIDTH+j]);
    }

    if (i == NumRows - 1) 
    {
      for (j = 0; j < OUTPUT_WIDTH - NumCols; j++) 
      {
        AsciiPrint("   ");
      }
    }

    AsciiPrint("   ");

    for (j = 0; j < NumCols; j++) 
    {
      cData = pData[i*OUTPUT_WIDTH+j];
      if (cData <= 0x20) 
      {
        AsciiPrint("%c", '.');
      }
      else
      {
        AsciiPrint("%c", cData);
      }
    }

    AsciiPrint("\n");
  }

  return Status;
}

/**
  return the next parameter from a command line string;

  This function moves the next parameter from Walker into TempParameter and moves
  Walker up past that parameter for recursive calling.  When the final parameter
  is moved *Walker will be set to NULL;

  Temp Parameter must be large enough to hold the parameter before calling this
  function.

  @param[in, out] Walker        pointer to string of command line.  Adjusted to
                                reminaing command line on return
  @param[in, out] TempParameter pointer to string of command line item extracted.

**/
VOID
EFIAPI
GetNextParameter(
  CHAR16 **Walker,
  CHAR16 **TempParameter
  )
{
  CHAR16 *NextDelim = NULL;
  CHAR16 *TempLoc = NULL;

  if (Walker == NULL ||
      *Walker == NULL ||
      TempParameter == NULL ||
      *TempParameter == NULL)
  {
    return;
  }
  
  //
  // make sure we dont have any leading spaces
  //
  while ((*Walker)[0] == L' ') {
      (*Walker)++;
  }

  //
  // make sure we still have some params now...
  //
  if (StrLen(*Walker) == 0) {
    ASSERT((*Walker)[0] == CHAR_NULL);
    *Walker = NULL;
    return;
  }

  //
  // we have a quoted parameter
  // could be the last parameter, but SHOULD have a trailing quote
  //
  if ((*Walker)[0] == L'\"') {
    NextDelim = NULL;
    for (TempLoc = *Walker + 1 ; TempLoc != NULL && *TempLoc != CHAR_NULL ; TempLoc++) {
      if (*TempLoc == L'^' && *(TempLoc+1) == L'\"') {
        TempLoc++;
      } else if (*TempLoc == L'\"') {
        NextDelim = TempLoc;
        break;
      }
    }

    if (NextDelim - ((*Walker)+1) == 0) {
      //
      // found ""
      //
      StrCpy(*TempParameter, L"");
      *Walker = NextDelim + 1;
    } else if (NextDelim != NULL) {
      StrnCpy(*TempParameter, (*Walker)+1, NextDelim - ((*Walker)+1));
      *Walker = NextDelim + 1;
    } else {
      //
      // last one... someone forgot the training quote!
      //
      StrCpy(*TempParameter, *Walker);
      *Walker = NULL;
    }
    for (TempLoc = *TempParameter ; TempLoc != NULL && *TempLoc != CHAR_NULL ; TempLoc++) {
      if (*TempLoc == L'^' && *(TempLoc+1) == L'\"') {
        CopyMem(TempLoc, TempLoc+1, StrSize(TempLoc) - sizeof(TempLoc[0]));
      }
    }
  } else {
    //
    // we have a regular parameter (no quote) OR
    // we have the final parameter (no trailing space)
    //
    NextDelim = StrStr((*Walker), L" ");
    if (NextDelim != NULL) {
      StrnCpy(*TempParameter, *Walker, NextDelim - (*Walker));
      (*TempParameter)[NextDelim - (*Walker)] = CHAR_NULL;
      *Walker = NextDelim+1;
    } else {
      //
      // last one.
      //
      StrCpy(*TempParameter, *Walker);
      *Walker = NULL;
    }
    for (NextDelim = *TempParameter ; NextDelim != NULL && *NextDelim != CHAR_NULL ; NextDelim++) {
      if (*NextDelim == L'^' && *(NextDelim+1) == L'^') {
        CopyMem(NextDelim, NextDelim+1, StrSize(NextDelim) - sizeof(NextDelim[0]));
      }
    }
    while ((*TempParameter)[StrLen(*TempParameter)-1] == L' ') {
      (*TempParameter)[StrLen(*TempParameter)-1] = CHAR_NULL;
    }
    while ((*TempParameter)[0] == L' ') {
      CopyMem(*TempParameter,
             (*TempParameter)+1,
             StrSize(*TempParameter) - sizeof((*TempParameter)[0]));
    }
  }
  return;
}

/**
  Function to populate Argc and Argv.

  This function parses the CommandLine and divides it into standard C style Argc/Argv
  parameters for inclusion in EFI_SHELL_PARAMETERS_PROTOCOL.  this supports space
  delimited and quote surrounded parameter definition.

  @param[in] CommandLine         String of command line to parse
  @param[in, out] Argv           pointer to array of strings; one for each parameter
  @param[in, out] Argc           pointer to number of strings in Argv array

  @return EFI_SUCCESS           the operation was sucessful
  @return EFI_OUT_OF_RESOURCES  a memory allocation failed.
**/
EFI_STATUS
EFIAPI
ParseCommandLineToArgs(
  IN CONST CHAR16 *CommandLine,
  IN OUT CHAR16 ***Argv,
  IN OUT UINTN *Argc
  )
{
  UINTN       Count = 0;
  CHAR16      *TempParameter = NULL;
  CHAR16      *Walker = NULL;
  CHAR16      *NewParam = NULL;
  UINTN       Size = 0;

  if (Argc == NULL || Argv == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  if (CommandLine == NULL || StrLen(CommandLine)==0) {
    (*Argc) = 0;
    (*Argv) = NULL;
    return (EFI_SUCCESS);
  }

  Size = StrSize(CommandLine);
  TempParameter = AllocateZeroPool(Size);
  if (TempParameter == NULL) {
    return (EFI_OUT_OF_RESOURCES);
  }

  for ( Count = 0
      , Walker = (CHAR16*)CommandLine
      ; Walker != NULL && *Walker != CHAR_NULL
      ; GetNextParameter(&Walker, &TempParameter)
      , Count++
     );

/*  Count = 0;
  Walker = (CHAR16*)CommandLine;
  while(Walker != NULL) {
    GetNextParameter(&Walker, &TempParameter);
    Count++;
  }
*/
  //
  // lets allocate the pointer array
  //
  (*Argv) = AllocateZeroPool((Count)*sizeof(CHAR16*));
  if (*Argv == NULL) {
    return (EFI_OUT_OF_RESOURCES);
  }

  *Argc = 0;
  Walker = (CHAR16*)CommandLine;
  while(Walker != NULL && *Walker != CHAR_NULL) {
    SetMem16(TempParameter, Size, CHAR_NULL);
    GetNextParameter(&Walker, &TempParameter);
    NewParam = AllocateZeroPool(StrSize(TempParameter));
    if (NewParam == NULL)
    {
      return EFI_OUT_OF_RESOURCES;
    }    
    StrCpy(NewParam, TempParameter);
    ((CHAR16**)(*Argv))[(*Argc)] = NewParam;
    (*Argc)++;
  }
  ASSERT(Count >= (*Argc));
  return (EFI_SUCCESS);
}

EFI_STATUS
DumpProvisionData (
  EFI_DPP_PROTOCOL *DppProtocol,
  IN  CHAR16       *Type,
  IN  CHAR16       *Name
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINTN i = 0;
  UINT8 *Buff = NULL;
  UINT32 BuffSize = 0;

  Status = DppProtocol->GetDPP( DppProtocol, Type, Name, NULL, &BuffSize);
  if (EFI_ERROR(Status)) 
  {
    AsciiPrint("ERROR: ProvDump - Failed to get provision data from DPP! "
               "Error Code: 0x%08X\r\n",
               Status);
    goto DumpProvisionDataExit;
  }

  Buff = (UINT8 *) AllocatePool(BuffSize);

  if(Buff == NULL)
  {
    Status = RETURN_OUT_OF_RESOURCES;
    goto DumpProvisionDataExit;
  }

  Status = DppProtocol->GetDPP( DppProtocol, Type, Name, Buff, &BuffSize);

  if((EFI_SUCCESS != Status) || (Buff == NULL))
  {
    AsciiPrint("ERROR: ProvDump - Failed to get provision data from DPP! "
               "Error Code: 0x%08X\r\n",
               Status);
    goto DumpProvisionDataExit;
  }

  AsciiPrint("\r\n%a\r\n", Name);
  for (i = 1; i <= BuffSize; i++) {
    AsciiPrint(" %02X", Buff[i-1]);
    if ((i % 16) == 0) AsciiPrint("\n");
  }
  AsciiPrint("\r\n");

  FreePool(Buff);

DumpProvisionDataExit:

  return Status;
}

EFI_STATUS
GetProvisionData (
  EFI_DPP_PROTOCOL *DppProtocol,
  IN  CHAR16       *Type,
  IN  CHAR16       *Name,
  OUT UINT8        **Buff,
  OUT UINT32       BufferLen
)
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT32 BuffSize = 0;


  Status = DppProtocol->GetDPP( DppProtocol, Type, Name, NULL, &BuffSize);
  if (EFI_ERROR(Status)) 
  {
    AsciiPrint("ERROR: ProvDump - Failed to get provision data from DPP! "
               "Error Code: 0x%08X\r\n",
               Status);
    goto GetProvisionDataExit;
  }

  if (BufferLen != BuffSize) 
  {
    AsciiPrint("ERROR: ProvDump - Length different than expected!\n");
    Status = EFI_DEVICE_ERROR;
    goto GetProvisionDataExit;
  }

  *Buff = (UINT8 *) AllocatePool(BuffSize);

  if(*Buff == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto GetProvisionDataExit;
  }

  Status = DppProtocol->GetDPP( DppProtocol, Type, Name, *Buff, &BuffSize);

GetProvisionDataExit:

  return Status;
}

EFI_STATUS
GetProvisionData2 (
  EFI_DPP_PROTOCOL *DppProtocol,
  IN  CHAR16       *Type,
  IN  CHAR16       *Name,
  OUT UINT8        **Buff,
  OUT UINT32       *Size
)
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT32 BuffSize = 0;

  Status = DppProtocol->GetDPP( DppProtocol, Type, Name, NULL, &BuffSize);
  if (EFI_ERROR(Status)) 
  {
    AsciiPrint("ERROR: ProvDump - Failed to get provision data from DPP! "
               "Error Code: 0x%08X\r\n",
               Status);
    goto GetProvisionData2Exit;
  }

  *Buff = (UINT8 *) AllocatePool(BuffSize);
  if(*Buff == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto GetProvisionData2Exit;
  }

  *Size = BuffSize;

  Status = DppProtocol->GetDPP( DppProtocol, Type, Name, *Buff, &BuffSize);

GetProvisionData2Exit:

  return Status;
}

EFI_STATUS 
EFIAPI
PrintUsage ()
{
  AsciiPrint("\n");
  AsciiPrint("Provision Data Dump Utility, version 0.1.\n");
  AsciiPrint("Copyright (c) 2013 Qualcomm Technologies Inc.\n");
  AsciiPrint("\n");
  AsciiPrint("Usage: ProvDump.efi <type> <name>\n");
  AsciiPrint("\n");
  AsciiPrint("  type : QCOM or OEM\n");
  AsciiPrint("  name : Provision data ID, for example: WLAN.PROVISION\n");
  AsciiPrint("\n");
  AsciiPrint("Run \"ProvDump.efi -list\" to display list of provision data\n");
  AsciiPrint("Run \"ProvDump.efi -save\" to save provision data in files\n");
  AsciiPrint("Run \"ProvDump.efi -qcom\" to dump formated QCOM-defined provision data\n");
  AsciiPrint("\n");

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
ListProvisionData ()
{
  EFI_STATUS  Status = EFI_SUCCESS;
  
  Status = GetDppInfo();
  if (Status != EFI_SUCCESS) 
  {
    AsciiPrint("ERROR: ProvDump - Failed to get DPP info! Error Code: 0x%08X\r\n", Status);
    goto ListProvisionDataExit;
  }

  Status = DisplayProvItems();
  
ListProvisionDataExit:

  return Status;
}

EFI_STATUS
SaveProvItems()
{
  EFI_STATUS              Status = EFI_SUCCESS;
  VOID                    *Buffer = NULL;
  UINTN                   BufferSize = 0;
  UINT32                  Index = 0;
  UINT32                  NumOfEntriesUsed = 0;
  PDPP_HEADER             pDppHeader = NULL;
  PDPP_DIRECTORY_HEADER   pDppDirHeader = NULL;
  PDPP_DATA_ENTRY         pDppDataEntry = NULL;
  UINT8                   *pData = NULL;
  UINT32                  Size = 0;
  CHAR16                  *FilePath = NULL;
  EFI_FILE_PROTOCOL       *File = NULL;
  EFI_DPP_PROTOCOL        *pDppProtocol = NULL;
  
  /* Locate DPP Protocol */
  Status = gBS->LocateProtocol( &gEfiDppProtocolGuid, NULL, (VOID**) &pDppProtocol );
  if (Status != EFI_SUCCESS) 
  {
    AsciiPrint("ERROR: ProvDump - Failed to locate DPP Protocol! Error Code: 0x%08X\r\n", Status);
    goto SaveProvItemsExit;
  }
  
  // Read DPP header, directory header, and directory entries
  BufferSize = sizeof(DPP_HEADER) + 
               sizeof(DPP_DIRECTORY_HEADER) +
               sizeof(DPP_DATA_ENTRY) * DPP_MAX_DATA_ENTRIES;

  ALIGN_TO_BLOCK(BufferSize, gDppBlockSize, BufferSize);

  Buffer = AllocateZeroPool(BufferSize);
  
  if (Buffer == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto SaveProvItemsExit;
  }

  Status = ReadDPPBlocks(DPP_HEADER_LBA, BufferSize, Buffer);

  if(EFI_ERROR(Status))
  {
    AsciiPrint("ERROR: ProvDump - Failed to read DPP block data at LBA %d, size %d! "
               "Error Code: 0x%08X\r\n",
               DPP_HEADER_LBA,
               BufferSize,
               Status);
    goto SaveProvItemsExit;
  }
    
  pDppHeader = (PDPP_HEADER)Buffer;
  pDppDirHeader = (PDPP_DIRECTORY_HEADER)((PDPP_HEADER)Buffer + 1);

  // Check validity of DPP
  if((pDppHeader->Signature != DPP_SIGNATURE) ||
     (pDppHeader->Revision != DPP_REVISION) ||
     (pDppHeader->Size != sizeof(DPP_HEADER)) ||
     (pDppDirHeader->Size != sizeof(DPP_DIRECTORY_HEADER)) ||
     (pDppDirHeader->DataEntriesAllocated != DPP_MAX_DATA_ENTRIES))
  {
    AsciiPrint("ERROR: ProvDump - DPP is not in a valid state!\r\n");
    Status = EFI_ABORTED;
    goto SaveProvItemsExit;
  }

  pDppDataEntry = (PDPP_DATA_ENTRY)((UINT8*)Buffer + pDppDirHeader->DataEntriesOffset);

  NumOfEntriesUsed = pDppDirHeader->DataEntriesUsed;
  
  if(NumOfEntriesUsed  > DPP_MAX_DATA_ENTRIES)
  {
    AsciiPrint("ERROR: ProvDump - DPP is not in a valid state!\r\n");
    Status = EFI_ABORTED;
    goto SaveProvItemsExit;
  }

  Status = gBS->OpenProtocol((gLoadedImage->DeviceHandle),
                             &gEfiSimpleFileSystemProtocolGuid,
                             (VOID**)&gEfiSimpleFileSystemProtocol,
                             gImageHandle,
                             NULL,
                             EFI_OPEN_PROTOCOL_GET_PROTOCOL);
  if (EFI_ERROR (Status)) 
  {
    AsciiPrint("ERROR: ProvDump - Failed to open simple file system protocol! "
               "Error Code: 0x%08X\r\n",
               Status);
    goto SaveProvItemsExit;
  }

  Status = gEfiSimpleFileSystemProtocol->OpenVolume(gEfiSimpleFileSystemProtocol, &gFsRoot);
  if (EFI_ERROR (Status)) 
  {
    AsciiPrint("ERROR: ProvDump - Failed to open the volume! Error Code: 0x%08X\r\n", Status);
    goto SaveProvItemsExit;
  }

  Status = gFsRoot->Open(gFsRoot,
                        &File,
                        OUTPUT_DIR_NAME,
                        EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE,
                        EFI_FILE_DIRECTORY);
  if (EFI_ERROR (Status)) 
  {
    AsciiPrint("ERROR: ProvDump - Failed to open file! Error Code: 0x%08X\r\n", Status);
    goto SaveProvItemsExit;
  }
  
  UpdateFileInfo(File);

  AsciiPrint("\n");
  AsciiPrint("%d provision item(s) found. Valid provision data will be saved in folder %s\n",
             NumOfEntriesUsed,
             OUTPUT_DIR_NAME);
  
  for (Index = 0; Index < NumOfEntriesUsed; Index++) 
  {
    if (pDppDataEntry && pDppDataEntry->Valid) 
    {
      Status = GetProvisionData2(pDppProtocol, 
                                 pDppDataEntry->Type, 
                                 pDppDataEntry->Name, 
                                 &pData, 
                                 &Size);
      if (EFI_ERROR(Status)) 
      {
        AsciiPrint("ERROR: ProvDump - Failed to get provision data for %s:%s from DPP! "
                   "Error Code: 0x%08X\r\n", 
                   pDppDataEntry->Type,
                   pDppDataEntry->Name,
                   Status);
        goto SaveProvItemsExit;
      }

      // Assemble the full file path
      FilePath = AllocateZeroPool(StrSize(pDppDataEntry->Type) +
                                  StrSize(pDppDataEntry->Name) + 
                                  StrSize(OUTPUT_DIR_NAME) +
                                  2 * StrSize(L"\\"));
      if(FilePath == NULL)
      {
        Status = EFI_OUT_OF_RESOURCES;
        goto SaveProvItemsExit;
      }

      StrCat(FilePath, L"\\");
      StrCat(FilePath, OUTPUT_DIR_NAME);
      StrCat(FilePath, L"\\");
      StrCat(FilePath, pDppDataEntry->Type);
      StrCat(FilePath, L"__");
      StrCat(FilePath, pDppDataEntry->Name);
            
      AsciiPrint("%s\n", FilePath);

      // 
      Status = gFsRoot->Open(gFsRoot,
                             &File,
                             FilePath,
                             EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE,
                             0);
      if (EFI_ERROR (Status)) 
      {
        AsciiPrint("Failed to Open file %r\n", Status);
        goto SaveProvItemsExit;
      }

      Status = File->Write(File, &Size, pData);
      if (EFI_ERROR (Status)) 
      {
        AsciiPrint("Failed to write file %r\n", Status);
        goto SaveProvItemsExit;
      }

      Status = File->Flush(File);

      UpdateFileInfo(File);

      Status = File->Close(File);

      if (pData != NULL) 
      {
        gBS->FreePool(pData);
        pData = NULL;
      }
      
      if(FilePath != NULL)
      {
        gBS->FreePool(FilePath);
        FilePath = NULL;
      }

      pDppDataEntry++;
    }
  }
   
  AsciiPrint("\n");
   
SaveProvItemsExit:

  if(Buffer != NULL)
  {
    FreePool(Buffer);
    Buffer = NULL;
  }

  if (pData != NULL) 
  {
    FreePool(pData);
    pData = NULL;
  }
      
  if(FilePath != NULL)
  {
    FreePool(FilePath);
    FilePath = NULL;
  }

  return Status;
}

EFI_STATUS
EFIAPI
SaveProvisionDataToFiles ()
{
  EFI_STATUS  Status = EFI_SUCCESS;

  Status = GetDppInfo();
  if (Status != EFI_SUCCESS) 
  {
    AsciiPrint("ERROR: ProvDump - Failed to get DPP info! Error Code: 0x%08X\r\n", Status);
    goto SaveProvisionDataToFilesExit;
  }

  Status = SaveProvItems();
  
SaveProvisionDataToFilesExit:

  if (gFsRoot) 
  {
    gFsRoot->Close(gFsRoot);
  }

  return Status;
}

EFI_STATUS
EFIAPI
DumpQcomProvisionData ()
{
  EFI_STATUS  Status = EFI_SUCCESS;
  EFI_DPP_PROTOCOL *pDppProtocol = NULL;
  UINTN  i = 0;
  data_wlan_mac_addr *pWlanMacAddr = NULL;
  data_bt_mac_addr *pBtMacAddr = NULL;
  
  /* Locate DPP Protocol */
  Status = gBS->LocateProtocol( &gEfiDppProtocolGuid, NULL, (VOID**) &pDppProtocol );
  if (Status != EFI_SUCCESS) 
  {
    AsciiPrint("ERROR: ProvDump - Failed to locate DPP Protocol! Error Code: 0x%08X\r\n", Status);
    goto DumpQcomProvisionDataExit;
  }

  Status = GetProvisionData (pDppProtocol, 
                             L"QCOM", 
                             L"WLAN.PROVISION", 
                             (UINT8**)&pWlanMacAddr, 
                             sizeof(data_wlan_mac_addr));
  if (Status != EFI_SUCCESS) 
  {
    AsciiPrint("ERROR: ProvDump - Failed to get WLAN.PROVISION data from DPP! "
               "Error Code: 0x%08X\r\n",
               Status);
    return Status;
  }

  AsciiPrint("WLAN.PROVISION\r\n");

  if(pWlanMacAddr != NULL)
  {
    if (pWlanMacAddr->version == 1)
      {
        for (i = 0; i < pWlanMacAddr->numMacAddr; i++)
        {
          AsciiPrint("Physical Address. . . . . . . . . : %02X-%02X-%02X-%02X-%02X-%02X\r\n", 
            pWlanMacAddr->macAddress[i].bytes[0],
            pWlanMacAddr->macAddress[i].bytes[1],
            pWlanMacAddr->macAddress[i].bytes[2],
            pWlanMacAddr->macAddress[i].bytes[3],
            pWlanMacAddr->macAddress[i].bytes[4],
            pWlanMacAddr->macAddress[i].bytes[5]
          );
        }
      }
      else
      {
        AsciiPrint("Unknown data struct version\r\n");
      }
      FreePool (pWlanMacAddr);
  }
  else
  {
      Status = EFI_OUT_OF_RESOURCES;
      AsciiPrint("ERROR: ProvDump - Failed to get WLAN.PROVISION data from DPP! "
                 "Error Code: 0x%08X\r\n",
                 Status);
      return Status;
  }

  Status = GetProvisionData (pDppProtocol, 
                             L"QCOM", 
                             L"BT.PROVISION", 
                             (UINT8**)&pBtMacAddr, 
                             sizeof(data_bt_mac_addr) );
  if (Status != EFI_SUCCESS) 
  {
    AsciiPrint("ERROR: ProvDump - Failed to get BT.PROVISION data from DPP! "
               "Error Code: 0x%08X\r\n",
               Status);
    return Status;
  }

  AsciiPrint("BT.PROVISION\r\n");

  if (pBtMacAddr != NULL)
  {
      if (pBtMacAddr->version == 1)
      {
          AsciiPrint("Physical Address. . . . . . . . . : %02X-%02X-%02X-%02X-%02X-%02X\r\n", 
          pBtMacAddr->macAddress.bytes[0],
          pBtMacAddr->macAddress.bytes[1],
          pBtMacAddr->macAddress.bytes[2],
          pBtMacAddr->macAddress.bytes[3],
          pBtMacAddr->macAddress.bytes[4],
          pBtMacAddr->macAddress.bytes[5]
        );
      }
      else
      {
          AsciiPrint("Unknown data struct version\r\n");
      }
      FreePool (pBtMacAddr);
  }
  else
  {
      Status = EFI_OUT_OF_RESOURCES;
      AsciiPrint("ERROR: ProvDump - Failed to get BT.PROVISION data from DPP! "
                 "Error Code: 0x%08X\r\n",
                 Status);
      return Status;
  }
  //AsciiPrint("ProvDump Done!\r\n");
DumpQcomProvisionDataExit:

  return Status;
}

EFI_STATUS
EFIAPI
DumpGenericProvisionData (
  IN CHAR16 *Type,
  IN CHAR16 *Name
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  EFI_DPP_PROTOCOL *pDppProtocol = NULL;
  UINT8 *pData = NULL;
  UINT32 Size = 0;
  
  /* Locate DPP Protocol */
  Status = gBS->LocateProtocol( &gEfiDppProtocolGuid, NULL, (VOID**) &pDppProtocol );
  if (Status != EFI_SUCCESS) 
  {
    AsciiPrint("ERROR: ProvDump - Failed to locate DPP Protocol! Error Code: 0x%08X\r\n", Status);
    goto DumpGenericProvisionDataExit;
  }

  Status = GetProvisionData2(pDppProtocol, 
                             Type, 
                             Name, 
                             &pData, 
                             &Size);
  if (Status != EFI_SUCCESS) 
  {
    AsciiPrint("ERROR: ProvDump - Failed to get provision data for %s:%s from DPP! "
               "Error Code: 0x%08X\r\n",
               Type,
               Name,
               Status);
    goto DumpGenericProvisionDataExit;
  }

  AsciiPrint("\n");
  AsciiPrint("%s:%s, size=%d\n", Type, Name, Size);
  
  PrintDataInHexAndChar(pData, Size);

  AsciiPrint("\n");
  
  FreePool (pData);

DumpGenericProvisionDataExit:

  return Status;
}

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
ProvDumpMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  CHAR16 *FullCmdLine = NULL;
  CHAR16 **Argv = NULL;
  UINTN Argc = 0;
  Operation eOP = OP_UNKNOWN;
  
  // Save a copy of ImageHandle
  gImageHandle = ImageHandle;

  // Get image info to parse command line
  Status = gBS->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, (VOID *)&gLoadedImage);
  if (EFI_ERROR(Status)) 
  {
    AsciiPrint("ERROR: ProvDump - Failed to handle LoadedImage protocol! "
               "Error Code: 0x%08X\r\n",
               Status);
    goto ProvDumpMainExit;
  }

  if ( gLoadedImage->LoadOptions == NULL || gLoadedImage->LoadOptionsSize == 0 ) 
  {
    Status = EFI_INVALID_PARAMETER;
    goto ProvDumpMainExit;
  }

  FullCmdLine = gLoadedImage->LoadOptions;

  Status = ParseCommandLineToArgs(FullCmdLine, &Argv, &Argc);
  if (EFI_ERROR(Status) || Argv == NULL) 
  {
    AsciiPrint("ERROR: ProvDump - Failed to parse command line to args! "
               "Error Code: 0x%08X\r\n",
               Status);
    goto ProvDumpMainExit;
  }

  if (Argc == 1 || Argc > 3) 
  {
    PrintUsage();
    goto ProvDumpMainExit;
  }
  
  if (Argc == 2)
  {
    if (StrCmp(Argv[1], L"-list") == 0)
    {
      eOP = OP_LIST_PROVISION_DATA;
    }
    else if (StrCmp(Argv[1], L"-qcom") == 0) 
    {
      eOP = OP_DUMP_QCOM_PROVISION_DATA;
    }
    else if (StrCmp(Argv[1], L"-save") == 0) 
    {
      eOP = OP_SAVE_PROVISION_DATA;
    }
    else
    {
      PrintUsage();
      goto ProvDumpMainExit;
    }
  }

  if (Argc == 3) 
  {
    eOP = OP_DUMP_GENERIC_PROVISION_DATA;
  }

  switch (eOP) 
  {
  case OP_LIST_PROVISION_DATA:
    Status = ListProvisionData();
    break;
  case OP_DUMP_QCOM_PROVISION_DATA:
    Status = DumpQcomProvisionData();
    break;
  case OP_DUMP_GENERIC_PROVISION_DATA:
    Status = DumpGenericProvisionData(Argv[1], Argv[2]);
    break;
  case OP_SAVE_PROVISION_DATA:
    Status = SaveProvisionDataToFiles();
    break;
  default:
    AsciiPrint("ERROR: ProvDump - I don't know what to do!\r\n");
    break;
  }
  
ProvDumpMainExit:

  return Status;
}

