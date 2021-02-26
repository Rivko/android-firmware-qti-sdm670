/** @file DppUtil.c
   
  Qualcomm Dpp tool application

  Copyright (c) 2010-2013, Qualcomm Technologies, Inc. All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/16/13   mic     Initial version
=============================================================================*/
#include "DppUtil.h"
#include "CommonUtil.h"

#define OUTPUT_DIR_NAME   L"ProvDump"

static DPP_PARTITION_TYPE               gDppPartType                 = DPP_PARTITION_TYPE_UNKNOWN;
static UINT32                           gDppBlockSize                = 0;
static UINT64                           gDppSize                     = 0;
static EFI_BLOCK_IO_PROTOCOL           *gDppBlockIo                  = NULL;

static EFI_GUID gEfiDppPartitionGuid = 
  {0x9992FD7D, 0xEC66, 0x4CBC, {0xA3, 0x37, 0x0D, 0xA1, 0xD4, 0xC9, 0x3F, 0x8F}};

static EFI_GUID gEfiDppBlockPartitionGuid = 
  {0x9992FD7D, 0xEC66, 0x4CBC, {0xA3, 0x37, 0x0D, 0xA1, 0xD4, 0xC9, 0x3F, 0x8F}};

static EFI_GUID gEfiDppFatPartitionGuid = 
  {0xebd0a0a2, 0xb9e5, 0x4433, {0x87, 0xc0, 0x68, 0xb6, 0xb7, 0x26, 0x99, 0xc7}};

EFI_STATUS
GetDppBlockInfo(VOID)
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
BlockDppProtocolInit(
  IN EFI_HANDLE         DppHandle
  )
{
    EFI_STATUS  Status = EFI_SUCCESS;
  
    if(DppHandle == NULL)
    {
      Status = EFI_UNSUPPORTED;
      goto BlockDppProtocolInit;
    }
  
    // Get Block IO protocol interface to DPP handle
    Status = gBS->HandleProtocol(DppHandle,
                                 &gEfiBlockIoProtocolGuid,
                                 (VOID**)&gDppBlockIo);
    HANDLE_ERROR_LABEL(Status,gBS->HandleProtocol,BlockDppProtocolInit);

    
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
    
BlockDppProtocolInit:

    return Status;
}

/**
  Initialize global variables which will be used to access DPP
**/
EFI_STATUS
DppGvariableInit(VOID)
{
  EFI_STATUS     Status       = EFI_SUCCESS;
  EFI_HANDLE     PartHandle   = NULL;
  
  /* Search if Block-based DPP exist? if not, check if Fat-based DPP exist */
  Status = GetPartitionHandle(&PartHandle,
                              &gEfiDppBlockPartitionGuid,
                              DPP_NAME,
                              &gEfiBlockIoProtocolGuid);
  if(!EFI_ERROR(Status) && (PartHandle != NULL))
  {
    Status = BlockDppProtocolInit(PartHandle);
    HANDLE_ERROR_LABEL(Status, BlockDppProtocolInit, GetDppTypeExit);
    gDppPartType = DPP_PARTITION_TYPE_BLOCK;

  }
  else if (Status == EFI_NOT_FOUND)
  {
    Status = GetPartitionHandle(&PartHandle,
                                &gEfiDppFatPartitionGuid,
                                DPP_NAME,
                                &gEfiSimpleFileSystemProtocolGuid);
    if(!EFI_ERROR(Status) && (PartHandle != NULL))
    {
      gDppPartType = DPP_PARTITION_TYPE_FAT;
    }
    else
    {
      Status = EFI_UNSUPPORTED;
      AsciiPrint("Warning:  DPP not found \n");
      goto GetDppTypeExit;
    }
  }
  else
  {
    Status = EFI_UNSUPPORTED;
    AsciiPrint("Warning: Block DPP unsupported platform type \n");
    goto GetDppTypeExit;
  }

  /* initialized required global variable */
  if (gDppPartType == DPP_PARTITION_TYPE_BLOCK)
  {
    Status = GetDppBlockInfo();
    HANDLE_ERROR_LABEL(Status, GetDppInfo, GetDppTypeExit);
    AsciiPrint( "Block DPP detected\n");
  }
  else if (gDppPartType == DPP_PARTITION_TYPE_FAT)
  {
    AsciiPrint( "Fat DPP detected\n");
  }
  else
  { /* shouldn't happen */
    AsciiPrint( "unknown DPP detected\n");
  }

GetDppTypeExit:

return Status;
}

EFI_STATUS
DppItemRead (
  IN  CHAR16       *Type,
  IN  CHAR16       *Name,
  OUT UINT8       **Buff,
  OUT UINT32       *Size
)
{
  EFI_STATUS        Status      = EFI_SUCCESS;
  EFI_DPP_PROTOCOL *DppProtocol = NULL;
  UINT32            BuffSize    = 0;


  if( (Buff == NULL) || (Type == NULL) ||
      (Name == NULL) || (Size == NULL))
  {
    Status = EFI_INVALID_PARAMETER;
    goto DppItemReadExit;
  }
  /* Get DPP protocol interface */
  Status = gBS->LocateProtocol(&gEfiDppProtocolGuid,
                               NULL,
                               (VOID**)&DppProtocol);
  HANDLE_ERROR_LABEL(Status, gBS->LocateProtocol, DppItemReadExit);

  /* use Dpp protocol to get required buffer size first */
  Status = DppProtocol->GetDPP( DppProtocol, Type, Name, NULL, &BuffSize);
  HANDLE_ERROR_LABEL(Status, DppProtocol->GetDPP, DppItemReadExit);

  /* allocate required buffer */
  *Buff = (UINT8 *) AllocatePool(BuffSize);
  if(*Buff == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto DppItemReadExit;
  }

  *Size = BuffSize;

  /* read dpp item */
  Status = DppProtocol->GetDPP( DppProtocol, Type, Name, *Buff, &BuffSize);
  HANDLE_ERROR_LABEL(Status, DppProtocol->GetDPP, DppItemReadExit);

DppItemReadExit:

  return Status;
}

EFI_STATUS
DppItemWrite (
  IN  CHAR16       *Type,
  IN  CHAR16       *Name,
  IN UINT8         *Buff,
  IN UINT32         Size
  )
{
  EFI_STATUS        Status      = EFI_SUCCESS;
  EFI_DPP_PROTOCOL *DppProtocol = NULL;
  
  if( (Buff == NULL) ||
      (Type == NULL) ||
      (Name == NULL))
  {
    Status = EFI_INVALID_PARAMETER;
    goto DppItemWriteExit;
  }

  /* Get DPP protocol interface */
  Status = gBS->LocateProtocol(&gEfiDppProtocolGuid,
                               NULL,
                               (VOID**)&DppProtocol);
  HANDLE_ERROR_LABEL(Status, gBS->LocateProtocol, DppItemWriteExit);

  /* write dpp item*/
  Status = DppProtocol->SetDPP( DppProtocol, Type, Name, Buff, Size);
  HANDLE_ERROR_LABEL(Status, DppProtocol->SetDPP, DppItemWriteExit);

DppItemWriteExit:

  return Status;
}

EFI_STATUS
DppItemDelete (
  IN  CHAR16       *Type,
  IN  CHAR16       *Name
  )
{

  EFI_STATUS        Status      = EFI_SUCCESS;
  EFI_DPP_PROTOCOL *DppProtocol = NULL;
  
  if( (Type == NULL) ||
      (Name == NULL))
  {
    Status = EFI_INVALID_PARAMETER;
    goto DppItemDeleteExit;
  }

  /* Get DPP protocol interface */
  Status = gBS->LocateProtocol(&gEfiDppProtocolGuid,
                               NULL,
                               (VOID**)&DppProtocol);
  HANDLE_ERROR_LABEL(Status, gBS->LocateProtocol, DppItemDeleteExit);

  /* Dpp delete*/
  Status = DppProtocol->DeleteItemDPP(DppProtocol, Type, Name);
  HANDLE_ERROR_LABEL(Status, DppProtocol->DeleteItemDPP, DppItemDeleteExit);

DppItemDeleteExit:

  return Status;

}

EFI_STATUS
DppErase (VOID)
{
  EFI_STATUS        Status      = EFI_SUCCESS;
  EFI_DPP_PROTOCOL *DppProtocol = NULL;
  
  /* Get DPP protocol interface */
  Status = gBS->LocateProtocol(&gEfiDppProtocolGuid,
                               NULL,
                               (VOID**)&DppProtocol);
  HANDLE_ERROR_LABEL(Status, gBS->LocateProtocol, DppEraseExit);

  /* erase Dpp*/
  Status = DppProtocol->EraseDPP(DppProtocol);
  HANDLE_ERROR_LABEL(Status, DppProtocol->EraseDPP, DppEraseExit);

DppEraseExit:

  return Status;
}

EFI_STATUS
ListBlockDppItems(VOID)
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
    goto ListBlockDppItemsExit;
  }

  Status = ReadDPPBlocks(DPP_HEADER_LBA, BufferSize, Buffer);

  if(EFI_ERROR(Status))
  {
    AsciiPrint("ERROR: ProvDump - Failed to read DPP block data at LBA %d, size %d! "
               "Error Code: 0x%08X\r\n",
               DPP_HEADER_LBA,
               BufferSize,
               Status);
    goto ListBlockDppItemsExit;
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
    goto ListBlockDppItemsExit;
  }

  pDppDataEntry = (PDPP_DATA_ENTRY)((UINT8*)Buffer + pDppDirHeader->DataEntriesOffset);

  NumOfEntriesUsed = pDppDirHeader->DataEntriesUsed;
  
  if(NumOfEntriesUsed  > DPP_MAX_DATA_ENTRIES)
  {
    AsciiPrint("ERROR: ProvDump - DPP is not in a valid state!\r\n");
    Status = EFI_ABORTED;
    goto ListBlockDppItemsExit;
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
    
ListBlockDppItemsExit:

  if(Buffer != NULL)
  {
    FreePool(Buffer);
  }

  return Status;
}


EFI_STATUS
ListAllDppItems(VOID)
{
  EFI_STATUS Status = EFI_SUCCESS;

   switch (gDppPartType)
   {
      case DPP_PARTITION_TYPE_FAT:
       AsciiPrint("  List FAT not supported yet\n\n");
        break;

      case DPP_PARTITION_TYPE_BLOCK: 
        Status = ListBlockDppItems();
        break;
      default:
       Status = EFI_UNSUPPORTED;
       AsciiPrint("  Not supported yet\n\n");

         break;
   }   

  return Status;
}
