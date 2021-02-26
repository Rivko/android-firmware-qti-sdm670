/** @file Dpp.c

  Implementation for Qualcomm UEFI DPP protocol

  Copyright (c) 2012-2015 Qualcomm Technologies, Inc. All rights 
  reserved. 

**/

/*=============================================================================
                              EDIT HISTORY


when       who     what, where, why
--------   ---     ----------------------------------------------------------- 
04/16/15   mic	   Updated to support 64 bit arch 
09/05/13   vk      Warning cleanup
07/16/13   mic     Added FAT-based DPP protocol support
11/05/12   rs      Added support for deletion of DPP items 
03/25/12   jd      Initial revision
=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/QcomLib.h>

#include <Guid/GlobalVariable.h>
#include <Guid/FileInfo.h>
#include <Guid/FileSystemInfo.h>

#include <Pi/PiDxeCis.h>

#include <Protocol/BlockIo.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/FirmwareVolume2.h>

/**
  DPP PROTOCOL interfaces
 */
#include <Protocol/EFIDpp.h>

#include "Dpp.h"

#define ERR_HANDLE(Status)\
  do { \
    if (EFI_ERROR (Status)) { \
      return Status; \
    } \
  } while (0)

#define HANDLE_ERROR_LABEL(Status, Function, Label) \
  do {  \
    if(EFI_ERROR(Status))   \
    {   \
      DEBUG((EFI_D_ERROR,": %a returned %r\n", #Function, Status)); \
      goto Label; \
    }   \
  }while (0)

#define HANDLE_ERR_CONTINUE(Status, Function) \
  do {  \
    if(EFI_ERROR(Status))   \
    {   \
      DEBUG((EFI_D_ERROR,": %a returned %r...Continue\n", #Function, Status)); \
    }   \
  }while (0)

#define DEBUG_ERR_LABEL(MESSAGE, Label) \
  do {  \
       DEBUG((EFI_D_ERROR, MESSAGE)); \
       goto Label; \
  }while (0)


#define ALIGN_TO_BLOCK(Size, BlockSize, AlignedSize) \
  { \
    AlignedSize = Size + (BlockSize - 1); \
    AlignedSize = AlignedSize & ~(BlockSize -1); \
  }

#define DPP_NAME L"DPP"
#define TYPE_MEDIA_DEVICE_PATH 4

static DPP_PART_STATUS         gDppPartStatus = {0};
static BOOLEAN                 gCachedDppPartStatusValid = FALSE;
static DPP_PARTITION_TYPE      gDppPartType = DPP_PARTITION_TYPE_UNKNOWN;
static UINT32                  gDppBlockSize = 0;
static UINT64                  gDppSize = 0;
static EFI_BLOCK_IO_PROTOCOL  *gDppBlockIo = NULL;
static EFI_HANDLE             gDPPFatHandle = NULL;

static EFI_GUID gEfiDppBlockPartitionGuid = 
  {0x9992FD7D, 0xEC66, 0x4CBC, {0xA3, 0x37, 0x0D, 0xA1, 0xD4, 0xC9, 0x3F, 0x8F}};

static EFI_GUID gEfiDppFatPartitionGuid = 
  {0xebd0a0a2, 0xb9e5, 0x4433, {0x87, 0xc0, 0x68, 0xb6, 0xb7, 0x26, 0x99, 0xc7}};

/**
 * Validate and correct FileInfo time
 * 
 * @param Time   - FAT file info structure
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
ValidateAndCorrectFileInfoTime(
  IN OUT EFI_TIME *Time
  )
{

  if (Time == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  /*if any bad variable found in Time structure, set the whole
    time structure to default value 
  */
  if (Time->Year < 1980 ||
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


/**
 * Reads the FileInfo from a given file.  Caller needs to
 * free the allocated FileInfo.
 * 
 * @param File         - Handle to File
 * @param FileInfoSize - Output to file info size
 * @param FileInfo     - Returns modified pointer to FileInfo
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS
GetFatFileInfo(
    IN EFI_FILE_PROTOCOL  *File,
    IN OUT UINTN          *FileInfoSize,
    IN OUT EFI_FILE_INFO **FileInfo
    )
{
  EFI_STATUS Status = EFI_SUCCESS;
    
  if ((File == NULL) || (FileInfoSize == NULL) || (FileInfo == NULL))
  {
    return EFI_INVALID_PARAMETER;
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
    HANDLE_ERROR_LABEL (Status, gBS->AllocatePool, GetFatFileInfoExit);

    Status = File->GetInfo(File,
                           &gEfiFileInfoGuid,
                           FileInfoSize,
                           (VOID*)*FileInfo);
    HANDLE_ERROR_LABEL (Status, File->GetInfo, GetFatFileInfoExit);
  }
  else if( EFI_ERROR(Status))
  {
    HANDLE_ERROR_LABEL (Status, File->GetInfo, GetFatFileInfoExit);
  }
  else
  {
    /* This is to handle EFI_SUCCESS case, status shouldn't be EFI_SUCCESS if
       *FileInfoSize is 0.*/
    Status = EFI_UNSUPPORTED;
    HANDLE_ERROR_LABEL (Status, File->GetInfo, GetFatFileInfoExit);
  }
  
  return Status;

GetFatFileInfoExit:

  if(*FileInfo != NULL)
  {
    gBS->FreePool(*FileInfo);
    *FileInfo = NULL;
  }
  return Status;
}

/**
 * Creates a directory in the given File system root.
 *  
 * Called by FwUpdateFatFileOperation when writing a file 
 *  fails because the directory is nonexistent. 
 * 
 * @param FsRoot      - File System Root where directory will be
 *                      created.
 * @param FileName    - Full path of file that 
 *                      FwUpdateFatFileOperation was attempting
 *                      to create.
 * @param Attributes  - Attributes passed in from 
 *                      FwUpdateFatFileOperation
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
CreateFatDirectory(
    IN EFI_FILE_PROTOCOL    *FsRoot,
    IN UINT16               *FileName,
    IN UINT64               Attributes)
{
  EFI_STATUS            Status        = EFI_SUCCESS;
  EFI_FILE_PROTOCOL    *File          = NULL;
  UINT64                OpenMode      = 0;
  UINT16              **Paths         = NULL; 
  UINT16              **NewPaths      = NULL;
  UINT16               *CurrentPath   = NULL;
  UINTN                 CharCounter   = 0;
  UINTN                 StringCounter = 0;
  UINTN                 StringCount   = 0;
    
    //Find how many directories we need to create.
    for(CharCounter = 0; CharCounter < StrLen(FileName); CharCounter++)
    {
        if((FileName[CharCounter] == L'\\') && (CharCounter > 0) ) 
        {
            //Make new string for this directory
            Status = gBS->AllocatePool(EfiBootServicesData,
                                       CharCounter<<1,
                                       (VOID**)&CurrentPath);
            ERR_HANDLE(Status);

            CopyMem(CurrentPath,FileName,CharCounter<<1);
            CurrentPath[CharCounter] = L'\0';

            //Add this directory string to the list of directories

            Status = gBS->AllocatePool(EfiBootServicesData,
                                       StringCount+1,
                                       (VOID**)&NewPaths);
            ERR_HANDLE(Status);

            if(StringCount > 0)
            {
                //Copy current paths to the expanded paths list
                CopyMem(NewPaths,Paths,(StringCount*sizeof(UINT16*)) );

                //Cleanup old Paths List
                Status = gBS->FreePool((VOID*)Paths);
                ERR_HANDLE(Status);    
            }
            Paths = NewPaths;
            Paths[StringCount] = CurrentPath;
            StringCount++;
        }
    }
    OpenMode = EFI_FILE_MODE_READ|EFI_FILE_MODE_WRITE|EFI_FILE_MODE_CREATE;
    Attributes |= EFI_FILE_DIRECTORY;
    //Create Each directory, but do not create the file.
    for(StringCounter = 0; StringCounter < StringCount; StringCounter++)
    {    
        Status = FsRoot->Open(FsRoot,
                          &File,
                          Paths[StringCounter],                          
                          OpenMode,
                          Attributes);
        ERR_HANDLE(Status);
    }
    return Status;
}

/**
 * Function to get a matching partition handle
 * after comparing the Emmc type, Partition Name and 
 * Partition type GUID
 *  
 * @param PartHandle    - partition handle
 * @Param PartTypeGuid  - Partition type GUID
 * @Param PartitionName - Partition name
 * @Param Protocol      - Protocol
 * @return : EFI_STATUS
 */
EFI_STATUS
GetPartitionHandle(
    IN OUT EFI_HANDLE              *PartHandle,
    IN EFI_GUID                    *PartTypeGuid,
    IN CHAR16                      *PartitionName,
    IN EFI_GUID                    *Protocol
    )
{
  EFI_STATUS                 Status             = EFI_SUCCESS;
  EFI_HANDLE                *HandleBuffer       = NULL;
  UINTN                      NumHandles         = 0;
  UINTN                      HandleCounter      = 0;
  EFI_GUID                  *TempPartTypeGuid   = NULL;
  CHAR16                    *TempPartitionName  = NULL;
  EFI_PARTITION_ENTRY       *TempPartEntry      = NULL;
  EFI_DEVICE_PATH_PROTOCOL  *DevPathInst        = NULL;

  if ((PartHandle == NULL) ||
      (PartTypeGuid == NULL) ||
      (PartitionName == NULL) ||
      (Protocol == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }
     
  Status = gBS->LocateHandleBuffer(ByProtocol,
                                  Protocol,
                                   NULL,
                                   &NumHandles,
                                   &HandleBuffer);
  HANDLE_ERROR_LABEL(Status,gBS->LocateHandleBuffer,GetPartitionHandle);
         
  for(HandleCounter = 0; HandleCounter < NumHandles; HandleCounter++)
  {   
    Status = gBS->HandleProtocol(HandleBuffer[HandleCounter],
                                &gEfiPartitionTypeGuid,
                                (VOID**)&TempPartTypeGuid);    
    if(EFI_ERROR(Status))
    {
      continue;
    }

    Status = gBS->HandleProtocol(HandleBuffer[HandleCounter],
                                 &gEfiDevicePathProtocolGuid,
                                (VOID**)&DevPathInst); 
    if(EFI_ERROR(Status))
    {
      continue;
    }

    Status = gBS->HandleProtocol(HandleBuffer[HandleCounter],
                               &gEfiPartitionRecordGuid,
                               (VOID **) &TempPartEntry);
    if(EFI_ERROR(Status))
    {
      continue;
    }

    TempPartitionName = TempPartEntry->PartitionName;
    DevPathInst = (EFI_DEVICE_PATH_PROTOCOL*)(((UINT8*)DevPathInst)+DevPathInst->Length[0]);

    if(DevPathInst->Type == TYPE_MEDIA_DEVICE_PATH)
    {    
      if((CompareGuid(TempPartTypeGuid,PartTypeGuid)) 
         && (CompareMem(TempPartitionName, PartitionName, StrSize(TempPartitionName)) == 0)) 
      {
        *PartHandle = HandleBuffer[HandleCounter];
        Status = EFI_SUCCESS;
        goto GetPartitionHandle;
      }
    }                   
  }
  
  Status = EFI_NOT_FOUND;

GetPartitionHandle:
  if(HandleBuffer != NULL)
  {
    gBS->FreePool(HandleBuffer);
    HandleBuffer = NULL;
  }
  
  return Status;
};

/**

  ReadDPPBlocks()

  Reads block data from DPP.

  @param[in]  Offset        Logical block address to read data from.
  @param[in]  Length        Number of bytes to read. Must be 
                            aligned to intrinsic block size.
  @param[out] Buffer        A pointer to buffer that receives the data.
                            Size of buffer must be aligned to intrinsic
                            block size.

  @return
  EFI_SUCCESS               Successfully read DPP block data.
  EFI_INVALID_PARAMETER     Invalid parameters.
  EFI_XXXX                  Other errors. (Refer to UEFI spec for details)

**/

EFI_STATUS
ReadDPPBlocks(
  IN UINTN         Offset,
  IN UINTN         Length,
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
    //DEBUG((EFI_D_WARN, "Failed to read block data from DPP.\n"));
    goto ReadDPPBlocksExit;
  }

  Status = gDppBlockIo->FlushBlocks(gDppBlockIo);

  if(EFI_ERROR(Status))
  {
    //DEBUG((EFI_D_WARN, "Failed to flush blocks.\n"));
    goto ReadDPPBlocksExit;
  }

ReadDPPBlocksExit:

  return Status;
}


/**

  WriteDPPBlocks()

  Write block data to DPP.

  @param[in]  Offset        Logical block address to write data to.
  @param[in]  Length        Number of bytes to write. Must be 
                            aligned to intrinsic block size.
  @param[out] Buffer        A pointer to buffer that contains data to be
                            written. Size of buffer must be
                            aligned to intrinsic block size.

  @return
  EFI_SUCCESS               Successfully write DPP block data.
  EFI_INVALID_PARAMETER     Invalid parameters.
  EFI_XXXX                  Other errors. (Refer to UEFI spec for details)

**/

EFI_STATUS
WriteDPPBlocks(
  IN UINTN         Offset,
  IN UINTN         Length,
  IN VOID         *Buffer
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  // Check input parameters
  if((Buffer == NULL) ||
     (Length == 0))
  {
    Status = EFI_INVALID_PARAMETER;
    goto WriteDPPBlocksExit;
  }

  if((Length & (gDppBlockSize - 1)) != 0)
  {
    Status = EFI_INVALID_PARAMETER;
    goto WriteDPPBlocksExit;
  }

  // Write block data
  Status = gDppBlockIo->WriteBlocks(gDppBlockIo,
                                    gDppBlockIo->Media->MediaId,
                                    (EFI_LBA)Offset,
                                    Length,
                                    Buffer);

  if(EFI_ERROR(Status))
  {
    //DEBUG((EFI_D_WARN, "Failed to write block data to DPP.\n"));
    goto WriteDPPBlocksExit;
  }

  Status = gDppBlockIo->FlushBlocks(gDppBlockIo);

  if(EFI_ERROR(Status))
  {
    //DEBUG((EFI_D_WARN, "Failed to flush blocks.\n"));
    goto WriteDPPBlocksExit;
  }

WriteDPPBlocksExit:

  return Status;
}


/**

  GetDPPPartStatus()

  Check DPP partition status.

  @param[out] DppPartStatus A pointer to buffer that receives DPP status.

  @return
  EFI_SUCCESS               Successfully retrieved DPP block data.
  EFI_INVALID_PARAMETER     Invalid parameters.
  EFI_OUT_OF_RESOURCES      No enough system resource 
  EFI_XXXX                  Other errors. (Refer to UEFI spec for details)

  Note:

  1. Always call this function to get DPP partition status,
     instead of referencing global variables directly.

  2. No need to check return status. If this function call fails, it will
     return default status (i.e. uninitialized)

**/

EFI_STATUS
GetDPPPartStatus(
  OUT PDPP_PART_STATUS DppPartStatus
  )
{
  EFI_STATUS            Status = EFI_SUCCESS;
  VOID                  *Buffer = NULL;
  UINTN                 BufferSize = 0;
  PDPP_HEADER           pDppHeader = NULL;
  PDPP_DIRECTORY_HEADER pDppDirHeader = NULL;

  if(DppPartStatus == NULL)
  {
    Status = EFI_INVALID_PARAMETER;
    goto GetDPPStatusExit;
  }
    
  if(gCachedDppPartStatusValid)
  {
    // return cached copy
    CopyMem((VOID*)DppPartStatus, (VOID*)&gDppPartStatus, sizeof(DPP_PART_STATUS));
  }
  else
  {
    // Initialize the status
    ZeroMem((VOID*)DppPartStatus, sizeof(DPP_PART_STATUS));
    
    // Read DPP header + Directory header
    BufferSize = sizeof(DPP_HEADER) + sizeof(DPP_DIRECTORY_HEADER);
    
    ALIGN_TO_BLOCK(BufferSize, gDppBlockSize, BufferSize);
  
    Buffer = AllocateZeroPool(BufferSize);
  
    if(Buffer == NULL)
    {
      Status = EFI_OUT_OF_RESOURCES;
      goto GetDPPStatusExit;
    }
  
    Status = ReadDPPBlocks(DPP_HEADER_LBA, BufferSize, Buffer);

    if(EFI_ERROR(Status))
    {
      //DEBUG((EFI_D_WARN, "Failed to read DPP block data at LBA %d, size %d.\n", 0, BufferSize));
      goto GetDPPStatusExit;
    }

    pDppHeader = (PDPP_HEADER)Buffer;
    pDppDirHeader = (PDPP_DIRECTORY_HEADER)((PDPP_HEADER)Buffer + 1);

    // Check validity of DPP
    if((pDppHeader->Signature == DPP_SIGNATURE) &&
       (pDppHeader->Revision == DPP_REVISION) &&
       (pDppHeader->Size == sizeof(DPP_HEADER)) &&
       (pDppDirHeader->Size == sizeof(DPP_DIRECTORY_HEADER)) &&
       (pDppDirHeader->DataEntriesAllocated == DPP_MAX_DATA_ENTRIES))
    {
      DppPartStatus->Initialized = TRUE;
    }
    else
    {
      DppPartStatus->Initialized = FALSE;
    }

    // Cache the status
    CopyMem((VOID*)&gDppPartStatus, (VOID*)DppPartStatus, sizeof(DPP_PART_STATUS));
    gCachedDppPartStatusValid = TRUE;
  }

GetDPPStatusExit:

  if(Buffer != NULL)
  {
    FreePool(Buffer);
  }

  return Status;
}
/**

  InitializeDPP()
  
  Initialize DPP partition to enable its usage.
  
  @return
  EFI_SUCCESS               Successfully initialized DPP.
  EFI_OUT_OF_RESOURCES      No enough system resource 
  EFI_XXXX                  Other errors. (Refer to UEFI spec for details)
   
**/

EFI_STATUS
InitializeDPP(
  VOID
  )
{
  EFI_STATUS              Status = EFI_SUCCESS;
  VOID                    *Buffer = NULL;
  UINTN                   BufferSize = 0;
  PDPP_HEADER             pDppHeader = NULL;
  PDPP_DIRECTORY_HEADER   pDppDirHeader = NULL;

  // Allocate buffer for DPP header + directory header + directory entries
  BufferSize = sizeof(DPP_HEADER) + 
               sizeof(DPP_DIRECTORY_HEADER) +
               sizeof(DPP_DATA_ENTRY) * DPP_MAX_DATA_ENTRIES;

  ALIGN_TO_BLOCK(BufferSize, gDppBlockSize, BufferSize);

  Buffer = AllocateZeroPool(BufferSize);
    
  if(Buffer == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto InitializeDPPExit;
  }

  // Populate DPP header
  pDppHeader = (PDPP_HEADER)Buffer;
  pDppHeader->Signature = DPP_SIGNATURE;
  pDppHeader->Size = sizeof(DPP_HEADER);
  pDppHeader->Revision = DPP_REVISION;
  pDppHeader->DirectoryOffset = sizeof(DPP_HEADER);
  pDppHeader->BlockSize = gDppBlockSize;
  pDppHeader->PartSize = gDppSize;

  // Populate DPP Directory Header
  pDppDirHeader = (PDPP_DIRECTORY_HEADER)((PDPP_HEADER)Buffer + 1);
  pDppDirHeader->Size = sizeof(DPP_DIRECTORY_HEADER);
  pDppDirHeader->DataEntriesOffset = sizeof(DPP_HEADER) + sizeof(DPP_DIRECTORY_HEADER);
  pDppDirHeader->DataEntriesAllocated = DPP_MAX_DATA_ENTRIES;
  pDppDirHeader->DataEntriesUsed = 0;
  pDppDirHeader->DataSpaceOffset = BufferSize;
  pDppDirHeader->DataSpaceAllocated = pDppHeader->PartSize - pDppDirHeader->DataSpaceOffset;

  /*The DataSpaceUsed includes invalid entries*/
  pDppDirHeader->DataSpaceUsed = 0;

  // Write back to DPP
  Status = WriteDPPBlocks(DPP_HEADER_LBA, BufferSize, Buffer);

  if(EFI_ERROR(Status))
  {
    //DEBUG((EFI_D_WARN, "Failed to write DPP block data at LBA %d, size %d.\n", 0, BufferSize));
    goto InitializeDPPExit;
  }

  // Invalid cached DPP partition status
  gCachedDppPartStatusValid = FALSE;

InitializeDPPExit:

  if(Buffer != NULL)
  {
    FreePool(Buffer);
  }

  return Status;
}

/**

  BlockSetDPP()

  Stores device provision data into block-based DPP.

   
  @param[in]  Type          An unicode string specifying provision data type,
                            e.g. "QCOM" or "OEM".
  @param[in]  Name          An unicode string specifying provision data name,
                            e.g. "WLAN.PROVISION"
  @param[in]  Data          A pointer to the buffer that contains provision data
                            to be written.
  @param[in]  DataSize      The size of provision data, in bytes.

  @return
  EFI_SUCCESS               Provision data is written to DPP successfully.
  EFI_INVALID_PARAMETER     Invalid parameter.
  EFI_OUT_OF_RESOURCES      No enough system resource 
  EFI_XXXX                  Other errors. (Refer to UEFI spec for details)

**/
EFI_STATUS
BlockSetDPP(
  IN CHAR16             *Type,
  IN CHAR16             *Name,
  IN VOID               *Data,
  IN UINTN               DataSize
  )
{
  EFI_STATUS              Status = EFI_SUCCESS;
  VOID                    *Buffer = NULL;
  UINTN                   BufferSize = 0;
  INT32                   Index = 0;
  UINT32                  NewEntryIndex = 0;
  UINT32                  NumOfEntriesUsed = 0;
  VOID                    *AlignedDataBuffer = NULL;
  UINT32                  AlignedDataSize = 0;
  UINT32                  BlockOffset = 0;
  BOOLEAN                 UpdateDataOnly = FALSE;
  BOOLEAN                 Replacement = FALSE;
  BOOLEAN                 FoundSlot = FALSE;
  PDPP_HEADER             pDppHeader = NULL;
  PDPP_DIRECTORY_HEADER   pDppDirHeader = NULL;
  PDPP_DATA_ENTRY         pDppDataEntry = NULL;
  UINT32                  ValidityCheckCount = 0;

  // Check input parameters
  if ((Type == NULL) ||
      (Name == NULL) ||
      (Data == NULL) ||
      (DataSize == 0))
  {
    Status = EFI_INVALID_PARAMETER;
    goto BlockSetDPPExit;
  }
  
  if ((StrLen(Type) > DPP_DATA_TYPE_MAX_LENGTH) ||
      (StrLen(Name) > DPP_DATA_NAME_MAX_LENGTH))
  {
    Status = EFI_INVALID_PARAMETER;
    goto BlockSetDPPExit;
  }

  // Read DPP header + directory header + directory entries
  BufferSize = sizeof(DPP_HEADER) + 
               sizeof(DPP_DIRECTORY_HEADER) +
               sizeof(DPP_DATA_ENTRY) * DPP_MAX_DATA_ENTRIES;
         
  ALIGN_TO_BLOCK(BufferSize, gDppBlockSize, BufferSize);

  Buffer = AllocateZeroPool(BufferSize);
  
  if (Buffer == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto BlockSetDPPExit;
  }

SetDPPCheckValidity:

  Status = ReadDPPBlocks(DPP_HEADER_LBA, BufferSize, Buffer);

  if(EFI_ERROR(Status))
  {
    //DEBUG((EFI_D_WARN, "Failed to read DPP block data at LBA %d, size %d.\n", 0, BufferSize));
    goto BlockSetDPPExit;
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
    //DEBUG((EFI_D_WARN, "DPP is not in a valid state.\n"));
    Status = InitializeDPP();
    if(EFI_ERROR(Status))
    {
      //DEBUG((EFI_D_WARN, "Failed to initialize DPP.\n"));
      goto BlockSetDPPExit;
    }
    else
    {
      // DPP initialized. Reload header info from DPP
      ValidityCheckCount++;
      if(ValidityCheckCount >= 5)
      {
        //DEBUG((EFI_D_WARN, "Failed to check DPP Validity.\n"));
        Status = EFI_ABORTED;
        goto BlockSetDPPExit;
      }
      else
      {
        goto SetDPPCheckValidity;
      }
    }
  }

  pDppDataEntry = (PDPP_DATA_ENTRY)((UINT8*)Buffer + pDppDirHeader->DataEntriesOffset);

  NumOfEntriesUsed = pDppDirHeader->DataEntriesUsed;

  // Check if still have available directory entries
  if(NumOfEntriesUsed  > DPP_MAX_DATA_ENTRIES)
  {
    //DEBUG((EFI_D_WARN, "DPP is corrupted.\n"));
    Status = EFI_ABORTED;
    goto BlockSetDPPExit;
  }

  // Assume we need to append the new provision data at the end
  NewEntryIndex = NumOfEntriesUsed;

  // Look up if the same provision data already exist, and if it can be replaced.
  if(NumOfEntriesUsed != 0)
  {
    // Searching existing entries backward so that if multiple instances of same provision data
    // exist, it will be replaced at the first available slot of forward searching order.
    for(Index = NumOfEntriesUsed - 1; Index >= 0; Index--)
    {
      if(CompareMem(pDppDataEntry[Index].Type, Type, StrSize(Type)) == 0 &&
         CompareMem(pDppDataEntry[Index].Name, Name, StrSize(Name)) == 0)
      {
        // Found an existing entry that matches the new data, invalid this entry
        pDppDataEntry[Index].Valid = DPP_DATA_INVALID;
    
        if(DataSize <= pDppDataEntry[Index].AllocatedSize)
        {
          // Will replace the provision data at current location
          NewEntryIndex = Index;
          Replacement = TRUE;
        }
      }
    }
  }


  if(!Replacement)
  {
    //Check if there is a free hole of this size in the dpp entries
    if(NumOfEntriesUsed != 0)
    {
      // Searching existing entries
      for(Index = 0; Index < NumOfEntriesUsed; Index++)
      {
        if(pDppDataEntry[Index].Valid == DPP_DATA_INVALID)
        {
          if(DataSize <= pDppDataEntry[Index].AllocatedSize)
          {
            // Will replace the provision data at current location
            NewEntryIndex = Index;
            Replacement = TRUE;
            FoundSlot = TRUE;
            break;
          }
        }
      }
    }
    //If no slot was found, we need to add a new entry.
    if(!FoundSlot)
    {  
      // Check if there is free space to add a new entry and store provision data
      if((NewEntryIndex  >= DPP_MAX_DATA_ENTRIES) || 
         ((DataSize > (pDppDirHeader->DataSpaceAllocated - pDppDirHeader->DataSpaceUsed))))
      {
        //DEBUG((EFI_D_WARN, "Insufficient resources for new provision data.\n"));
        Status = EFI_OUT_OF_RESOURCES;
        goto BlockSetDPPExit;
      }
    }
  }
  
  // Insert the new provision data
  ALIGN_TO_BLOCK(DataSize, gDppBlockSize, AlignedDataSize);
  
  if(AlignedDataSize != DataSize)
  {
    // Need to align the buffer
    AlignedDataBuffer = AllocateZeroPool(AlignedDataSize);
    if(AlignedDataBuffer == NULL)
    {
      Status = EFI_OUT_OF_RESOURCES;
      goto BlockSetDPPExit;
    }
    else
    {
      CopyMem(AlignedDataBuffer, Data, DataSize);
    }
  }
  else
  {
    AlignedDataBuffer = Data;
  }

  if(!Replacement)
  {
    CopyMem(pDppDataEntry[NewEntryIndex].Name, Name, StrSize(Name));
    CopyMem(pDppDataEntry[NewEntryIndex].Type, Type, StrSize(Type));
    pDppDataEntry[NewEntryIndex].AllocatedSize = AlignedDataSize;
    pDppDataEntry[NewEntryIndex].Offset = pDppDirHeader->DataSpaceUsed;
    pDppDataEntry[NewEntryIndex].UsedSize = (UINT32)DataSize;
    pDppDataEntry[NewEntryIndex].Valid = DPP_DATA_VALID;

    // Update directory
    pDppDirHeader->DataEntriesUsed++;
    pDppDirHeader->DataSpaceUsed += AlignedDataSize;
  }
  else
  {    
    if((pDppDataEntry[NewEntryIndex].UsedSize == DataSize) &&
       (pDppDataEntry[NewEntryIndex].Valid == DPP_DATA_VALID))
    {
      UpdateDataOnly = TRUE;
    }
    /*This is a replacement case which uses a found slot. 
      We need to update the name and type of this entry as
      it was cleared when we deleted the previous DPP item.
      */
    else
    {
      CopyMem(pDppDataEntry[NewEntryIndex].Name, Name, StrSize(Name));
      CopyMem(pDppDataEntry[NewEntryIndex].Type, Type, StrSize(Type));
      pDppDataEntry[NewEntryIndex].UsedSize = (UINT32)DataSize;
      pDppDataEntry[NewEntryIndex].Valid = DPP_DATA_VALID;
    }
  }

  // Write DPP data
  BlockOffset = (pDppDirHeader->DataSpaceOffset + pDppDataEntry[NewEntryIndex].Offset)
                /pDppHeader->BlockSize;

  Status = WriteDPPBlocks(BlockOffset, AlignedDataSize, AlignedDataBuffer);

  if(EFI_ERROR(Status))
  {
    //DEBUG((EFI_D_WARN, "Failed to write DPP block data at LBA %d, size %d.\n", BlockOffset, AlignedDataSize));
    goto BlockSetDPPExit;
  }

  // Update directory
  // If it is a replacement operation with exact the same data size, skip updating directory
  if(!UpdateDataOnly)
  {
    Status = WriteDPPBlocks(DPP_HEADER_LBA, BufferSize, Buffer);

    if(EFI_ERROR(Status))
    {
      //DEBUG((EFI_D_WARN, "Failed to write DPP block data at LBA %d, size %d.\n", 0, BufferSize));
      goto BlockSetDPPExit;
    }
  }

BlockSetDPPExit:

  if(Buffer != NULL)
  {
    FreePool(Buffer);
  }

  if(AlignedDataBuffer != NULL)
  {
    if(AlignedDataBuffer != Data)
    {
      FreePool(AlignedDataBuffer);
    }
  }

  return Status;
}


/**

  BlockGetDPP()
  
  This function retrieves the device provision data from Device
  Provisioning Partition of GPP.
     
  @param[in]  Type          An unicode string of provision data type,
                            e.g. L"QCOM" or L"OEM".
  @param[in]  Name          An unicode string of provision data name,
                            e.g. L"WLAN.PROVISION".
  @param[out] Data          A pointer to caller allocated buffer that will
                            store retrieved provision data. If NULL is
                            passed in, this function will return the 
                            provision data size.
  @param[out] DataSize      Upon successful return, the size of provision
                            data.
  
  @return
  EFI_SUCCESS               Provision data is retrieved from DPP successfully.
  EFI_INVALID_PARAMETER     Invalid parameter. 
  EFI_OUT_OF_RESOURCES      No enough system resource.
  EFI_NOT_FOUND             Specified provision data not found.
  EFI_XXXX                  Other errors. (Refer to UEFI spec for details)
 
**/
EFI_STATUS
BlockGetDPP(
  IN CHAR16             *Type,
  IN CHAR16             *Name,
  IN OUT VOID           *Data,
  IN OUT UINTN          *DataSize
  )
{
  EFI_STATUS              Status = EFI_SUCCESS;
  VOID                    *Buffer = NULL;
  UINTN                   BufferSize = 0;
  VOID                    *AlignedDataBuffer = NULL;
  UINTN                   AlignedDataSize = 0;
  UINT32                  Index = 0;
  UINT32                  MatchedEntryIndex = 0;
  UINT32                  NumOfEntriesUsed = 0;
  UINT32                  BlockOffset = 0;
  BOOLEAN                 Found = FALSE;
  PDPP_HEADER             pDppHeader = NULL;
  PDPP_DIRECTORY_HEADER   pDppDirHeader = NULL;
  PDPP_DATA_ENTRY         pDppDataEntry = NULL;
    
  if((Type == NULL) ||
     (Name == NULL) ||
     (DataSize == NULL))
  {
    Status = EFI_INVALID_PARAMETER;
    goto BlockGetDPPExit;
  }
    
  if((StrLen(Type) > DPP_DATA_TYPE_MAX_LENGTH) ||
     (StrLen(Name) > DPP_DATA_NAME_MAX_LENGTH))
  {
    Status = EFI_INVALID_PARAMETER;
    goto BlockGetDPPExit;
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
    goto BlockGetDPPExit;
  }

  Status = ReadDPPBlocks(DPP_HEADER_LBA, BufferSize, Buffer);

  if(EFI_ERROR(Status))
  {
    //DEBUG((EFI_D_WARN, "Failed to read DPP block data at LBA %d, size %d.\n", 0, BufferSize));
    goto BlockGetDPPExit;
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
    //DEBUG((EFI_D_WARN, "DPP is not in a valid state.\n"));
    Status = EFI_ABORTED;
    goto BlockGetDPPExit;
  }

  pDppDataEntry = (PDPP_DATA_ENTRY)((UINT8*)Buffer + pDppDirHeader->DataEntriesOffset);

  NumOfEntriesUsed = pDppDirHeader->DataEntriesUsed;
  
  if(NumOfEntriesUsed  > DPP_MAX_DATA_ENTRIES)
  {
    //DEBUG((EFI_D_WARN, "DPP is corrupted.\n"));
    Status = EFI_ABORTED;
    goto BlockGetDPPExit;
  }

  // Look up the provision data
  for(Index = 0; Index < NumOfEntriesUsed; Index++)
  {
    if(CompareMem(pDppDataEntry[Index].Type, Type, StrSize(Type)) == 0 &&
       CompareMem(pDppDataEntry[Index].Name, Name, StrSize(Name)) == 0 &&
       pDppDataEntry[Index].Valid == DPP_DATA_VALID)
    {
      // Found it!
      Found = TRUE;
      MatchedEntryIndex = Index;
      break;
    }
  }

  if(!Found)
  {
    //AsciiPrint("GetDPP: Desired provision data not found.\n");
    Status = EFI_NOT_FOUND;
    goto BlockGetDPPExit;
  }

  // Get the provision data size
  *DataSize = pDppDataEntry[MatchedEntryIndex].UsedSize;

  if(Data == NULL)
  {  
    Status = EFI_SUCCESS;
    goto BlockGetDPPExit;
  }
  else
  {
    ALIGN_TO_BLOCK(*DataSize, gDppBlockSize, AlignedDataSize);
  
    AlignedDataBuffer = AllocateZeroPool(AlignedDataSize);

    if(AlignedDataBuffer == NULL)
    {
      Status = EFI_OUT_OF_RESOURCES;
      goto BlockGetDPPExit;
    }
  
    BlockOffset = (pDppDirHeader->DataSpaceOffset + pDppDataEntry[MatchedEntryIndex].Offset)/pDppHeader->BlockSize;
  
    Status = ReadDPPBlocks(BlockOffset, AlignedDataSize, AlignedDataBuffer);
  
    if(EFI_ERROR(Status))
    {
      //DEBUG((EFI_D_WARN, "Failed to read DPP block data at LBA %d, size %d.\n", BlockOffset, AlignedDataSize));
      goto BlockGetDPPExit;
    }

    CopyMem(Data, AlignedDataBuffer, *DataSize);
  }
    
BlockGetDPPExit:

  if(Buffer != NULL)
  {
    FreePool(Buffer);
  }

  if(AlignedDataBuffer != NULL)
  {
    FreePool(AlignedDataBuffer);
  }

  return Status; 
}

/**

  BlockDeleteItemDPP()
  
  This function deletes the device provision data from Device
  Provisioning Partition of GPP.
   
  @param[in]  Type          An unicode string of provision data type,
                            e.g. L"QCOM" or L"OEM".
  @param[in]  Name          An unicode string of provision data name,
                            e.g. L"WLAN.PROVISION".
  @return
  EFI_SUCCESS               Provision data is deleted from DPP 
                            successfully. 
  EFI_INVALID_PARAMETER     Invalid parameter. 
  EFI_NOT_FOUND             Specified provision data not found. 
  EFI_XXXX                  Other errors. (Refer to UEFI spec 
                            for details) 
**/
EFI_STATUS
BlockDeleteItemDPP(
  IN CHAR16             *Type,
  IN CHAR16             *Name
  )
{
  EFI_STATUS              Status = EFI_SUCCESS;
  VOID                    *Buffer = NULL;
  UINTN                   BufferSize = 0;
  INT32                   Index = 0;
  BOOLEAN                 FoundDPPItem = FALSE;
  UINT32                  NumOfEntriesUsed = 0;
  VOID                    *AlignedDataBuffer = NULL;
  UINTN                   DataSize = 0;
  UINT32                  AlignedDataSize = 0;
  UINT32                  BlockOffset = 0;
  PDPP_HEADER             pDppHeader = NULL;
  PDPP_DIRECTORY_HEADER   pDppDirHeader = NULL;
  PDPP_DATA_ENTRY         pDppDataEntry = NULL;

  // Check input parameters
  if ((Type == NULL) || (Name == NULL))
  {
    Status = EFI_INVALID_PARAMETER;
    goto BlockDeleteDPPExit;
  }
  
  if ((StrLen(Type) > DPP_DATA_TYPE_MAX_LENGTH) ||
      (StrLen(Name) > DPP_DATA_NAME_MAX_LENGTH))
  {
    Status = EFI_INVALID_PARAMETER;
    goto BlockDeleteDPPExit;
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
    goto BlockDeleteDPPExit;
  }

  Status = ReadDPPBlocks(DPP_HEADER_LBA, BufferSize, Buffer);

  if(EFI_ERROR(Status))
  {
    //DEBUG((EFI_D_WARN, "Failed to read DPP block data at LBA %d, size %d.\n", 0, BufferSize));
    goto BlockDeleteDPPExit;
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
    //DEBUG((EFI_D_WARN, "DPP is not in a valid state.\n"));
    Status = EFI_ABORTED;
    goto BlockDeleteDPPExit;
  }

  pDppDataEntry = (PDPP_DATA_ENTRY)((UINT8*)Buffer + pDppDirHeader->DataEntriesOffset);

  NumOfEntriesUsed = pDppDirHeader->DataEntriesUsed;
  
  if(NumOfEntriesUsed  > DPP_MAX_DATA_ENTRIES)
  {
    //DEBUG((EFI_D_WARN, "DPP is corrupted.\n"));
    Status = EFI_ABORTED;
    goto BlockDeleteDPPExit;
  }

  // Look up the provision data to delete
  for(Index = 0; Index < NumOfEntriesUsed; Index++)
  {
    if(CompareMem(pDppDataEntry[Index].Type, Type, StrSize(Type)) == 0 &&
       CompareMem(pDppDataEntry[Index].Name, Name, StrSize(Name)) == 0 &&
       pDppDataEntry[Index].Valid == DPP_DATA_VALID)
    {
      // Found the entry to delete!
      //Clear out the type, name and set it as invalid.
      FoundDPPItem = TRUE;
      pDppDataEntry[Index].Valid = DPP_DATA_INVALID;

      ZeroMem((VOID*)pDppDataEntry[Index].Name, sizeof(pDppDataEntry[Index].Name));
      ZeroMem((VOID*)pDppDataEntry[Index].Type, sizeof(pDppDataEntry[Index].Type));

      // Get the provision data size
      //Can we assume this size will never be corrupt?
      DataSize = pDppDataEntry[Index].UsedSize;

      if(pDppDataEntry[Index].UsedSize > pDppDataEntry[Index].AllocatedSize)
      {
        //This is a indication of corruption. Error out to avoid erasing other data
        Status = EFI_ABORTED;
        goto BlockDeleteDPPExit;
      }

      //Reset usedsize to 0, keeping allocatedsize intact.
      pDppDataEntry[Index].UsedSize = 0;

      ALIGN_TO_BLOCK(DataSize, gDppBlockSize, AlignedDataSize);

      AlignedDataBuffer = AllocateZeroPool(AlignedDataSize);

      if(AlignedDataBuffer == NULL)
      {
        Status = EFI_OUT_OF_RESOURCES;
        goto BlockDeleteDPPExit;
      }

      /*First update the directory
      If we lose power, atleast the entry is marked as invalid 
      although the DPP Data will be stale*/ 
      Status = WriteDPPBlocks(DPP_HEADER_LBA, BufferSize, Buffer);
      if(EFI_ERROR(Status))
      {
        goto BlockDeleteDPPExit;
      }

      BlockOffset = (pDppDirHeader->DataSpaceOffset + pDppDataEntry[Index].Offset)
                    / pDppHeader->BlockSize;
      //Zero out the DPP Data
      Status = WriteDPPBlocks(BlockOffset, AlignedDataSize, AlignedDataBuffer);
      if(EFI_ERROR(Status))
      {
        goto BlockDeleteDPPExit;
      }

      break;
    }
  }

  if(!FoundDPPItem)
  {
    Status = EFI_NOT_FOUND;
    goto BlockDeleteDPPExit;
  }

BlockDeleteDPPExit:
  if(Buffer != NULL)
  {
    FreePool(Buffer);
    Buffer = NULL;
  }

  if(AlignedDataBuffer != NULL)
  {
    FreePool(AlignedDataBuffer);
    AlignedDataBuffer = NULL;
  }
  return Status;

}

/**

  FatSetDPP()

  Stores device provision data into block-based DPP.

   
  @param[in]  Type          An unicode string specifying provision data type,
                            e.g. "QCOM" or "OEM".
  @param[in]  Name          An unicode string specifying provision data name,
                            e.g. "WLAN.PROVISION"
  @param[in]  Data          A pointer to the buffer that contains provision data
                            to be written.
  @param[in]  DataSize      The size of provision data, in bytes.

  @return
  EFI_SUCCESS               Provision data is written to DPP successfully.
  EFI_INVALID_PARAMETER     Invalid parameter.
  EFI_OUT_OF_RESOURCES      No enough system resource 
  EFI_XXXX                  Other errors. (Refer to UEFI spec for details)

**/
EFI_STATUS
FatSetDPP(
  IN CHAR16             *Type,
  IN CHAR16             *Name,
  IN VOID               *Data,
  IN UINTN               DataSize
  )
{

  EFI_STATUS                        Status       = EFI_SUCCESS;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL   *Fs          = NULL;
  EFI_FILE_PROTOCOL                 *FsRoot      = NULL;
  EFI_FILE_PROTOCOL                 *File        = NULL;
  EFI_FILE_INFO                     *FileInfo    = NULL;
  UINTN                             FileInfoSize = 0;
  volatile UINT64                   OpenMode;
  UINT64                            Attributes   = 0x0;
  EFI_GUID                          FileInfoGuid = EFI_FILE_INFO_ID ;
  CHAR16                           *pFilePath    = NULL; 
  UINTN                             nTypeLen     = 0;
  UINTN                             nNameLen     = 0;

  if ((Type == NULL) || 
      (Name == NULL) || 
      (Data == NULL))
  {
    Status = EFI_INVALID_PARAMETER;
    goto FatSetDPPExit;
  }
  
  nTypeLen = StrLen(Type);
  nNameLen = StrLen(Name);

  if ((nTypeLen > DPP_DATA_TYPE_MAX_LENGTH) ||
      (nNameLen > DPP_DATA_NAME_MAX_LENGTH))
  {
    Status = EFI_INVALID_PARAMETER;
    goto FatSetDPPExit;
  }
  /* allocate buffer for CHAR16 file path */
  pFilePath = AllocateZeroPool(2 * (nTypeLen + nNameLen +StrSize (L"\\\\")));
  if (pFilePath != NULL)
  {
    StrCat (pFilePath, L"\\");
    StrCat (pFilePath, Type);
    StrCat (pFilePath, L"\\");
    StrCat (pFilePath, Name);
  }
  else
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto FatSetDPPExit;
  }

  Status = gBS->HandleProtocol(gDPPFatHandle,
                              &gEfiSimpleFileSystemProtocolGuid,
                              (VOID **)&Fs);
  HANDLE_ERROR_LABEL(Status, gBS->HandleProtocol, FatSetDPPExit);

  Status = Fs->OpenVolume(Fs,&FsRoot);
  HANDLE_ERROR_LABEL(Status, Fs->OpenVolume, FatSetDPPExit);

  if (FsRoot != NULL)
  {
    /* concat dir name and file name */
    OpenMode = EFI_FILE_MODE_READ|EFI_FILE_MODE_WRITE|EFI_FILE_MODE_CREATE;

    Status = FsRoot->Open(FsRoot,
                          &File,
                          pFilePath,                          
                          OpenMode, 
                          Attributes);

    if(EFI_ERROR(Status))
    {
      if(Status == EFI_NOT_FOUND)
      {
        //Tried to write file to nonexistent directory,
        //need to create the directory and try again.
        Status = CreateFatDirectory(FsRoot,
                                    pFilePath,
                                    Attributes);
        HANDLE_ERROR_LABEL(Status, CreateFatDirectory, FatSetDPPExit);

        Status = FsRoot->Open(FsRoot,
                              &File,
                              pFilePath,                          
                              OpenMode,
                              Attributes);
        HANDLE_ERROR_LABEL(Status, FsRoot->Open, FatSetDPPExit);
      }
      else
      {
        HANDLE_ERROR_LABEL(Status, FsRoot->Open, FatSetDPPExit);
      }
    }
  }
  else
  {
    Status = EFI_NOT_FOUND;
    HANDLE_ERROR_LABEL(Status, Fs->OpenVolume, FatSetDPPExit);
  }

  if (File != NULL)
  {
    Status = GetFatFileInfo(File,&FileInfoSize,&FileInfo);
         
    if((!EFI_ERROR(Status)) && (FileInfo != NULL))
    {
      //Clear out the read only attribute
      if((FileInfo->Attribute) & EFI_FILE_READ_ONLY)
      {
        FileInfo->Attribute = (FileInfo->Attribute) & (0xFFFFFFFFFFFFFFFE) ;

        /* Validate and correct the FileInfo time is required before callig 
                   setInfo function. Also, igore the return status if 
                   FwUpdateValidateAndCorrectFileInfoTime returns error */
        ValidateAndCorrectFileInfoTime(&FileInfo->CreateTime);
        ValidateAndCorrectFileInfoTime(&FileInfo->ModificationTime);
        ValidateAndCorrectFileInfoTime(&FileInfo->LastAccessTime);

        Status = File->SetInfo (File, &FileInfoGuid, FileInfo->Size, FileInfo);
      }
    }

    Status = File->Write(File,
                         &DataSize,
                         Data);
    HANDLE_ERROR_LABEL(Status, File->Write, FatSetDPPExit);

    if(FileInfo != NULL)
    {
      gBS->FreePool(FileInfo);
      FileInfo = NULL;
    }
   
    Status = GetFatFileInfo(File,&FileInfoSize,&FileInfo);
    HANDLE_ERROR_LABEL(Status, GetFatFileInfo, FatSetDPPExit);

    if(FileInfo != NULL)
    {
      //Clear out the read only attribute
      if((FileInfo->Attribute) & EFI_FILE_READ_ONLY)
      {
        FileInfo->Attribute = (FileInfo->Attribute) & (0xFFFFFFFFFFFFFFFE) ;
      }
      FileInfo->FileSize = DataSize;
        
      /* Validate and correct the FileInfo time is required before callig 
               setInfo function. Also, igore the return status if 
              FwUpdateValidateAndCorrectFileInfoTime returns error */
      ValidateAndCorrectFileInfoTime(&FileInfo->CreateTime);
      ValidateAndCorrectFileInfoTime(&FileInfo->ModificationTime);
      ValidateAndCorrectFileInfoTime(&FileInfo->LastAccessTime);
        
      Status = File->SetInfo (File, &FileInfoGuid, FileInfo->Size, FileInfo);
      HANDLE_ERROR_LABEL(Status, File->SetInfo, FatSetDPPExit);

      Status = File->Flush(File);
      HANDLE_ERROR_LABEL(Status, File->Flush, FatSetDPPExit);
    }
  }
  else
  {
    Status = EFI_NOT_FOUND;
    HANDLE_ERROR_LABEL(Status, FsRoot->Open, FatSetDPPExit);

  }

FatSetDPPExit:

  if(FileInfo)
  {
    gBS->FreePool(FileInfo);
    FileInfo = NULL;
  }

  if(File)
  {
    File->Close(File);
    File = NULL; 
  }
    
  if (FsRoot)
  {
    FsRoot->Close(FsRoot);
    FsRoot = NULL;
  }
  
  if (pFilePath)
  {
    FreePool(pFilePath);
    pFilePath = NULL;
  }

  return Status;
}

/**

  FatGetDPP()
  
  This function retrieves the device provision data from Device
  Provisioning Partition of GPP.
     
  @param[in]  Type          An unicode string of provision data type,
                            e.g. L"QCOM" or L"OEM".
  @param[in]  Name          An unicode string of provision data name,
                            e.g. L"WLAN.PROVISION".
  @param[out] Data          A pointer to caller allocated buffer that will
                            store retrieved provision data. If NULL is
                            passed in, this function will return the 
                            provision data size.
  @param[out] DataSize      Upon successful return, the size of provision
                            data.
  
  @return
  EFI_SUCCESS               Provision data is retrieved from DPP successfully.
  EFI_INVALID_PARAMETER     Invalid parameter. 
  EFI_OUT_OF_RESOURCES      No enough system resource.
  EFI_NOT_FOUND             Specified provision data not found.
  EFI_XXXX                  Other errors. (Refer to UEFI spec for details)
 
**/
EFI_STATUS
FatGetDPP(
  IN CHAR16             *Type,
  IN CHAR16             *Name,
  IN OUT VOID           *Data,
  IN OUT UINTN          *DataSize
  )
{

  EFI_STATUS                        Status       = EFI_SUCCESS;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL   *Fs          = NULL;
  EFI_FILE_PROTOCOL                 *FsRoot      = NULL;
  EFI_FILE_PROTOCOL                 *File        = NULL;
  EFI_FILE_INFO                     *FileInfo    = NULL;
  UINTN                             FileInfoSize = 0;
  UINT64                            Attributes   = 0x0;
  CHAR16                           *pFilePath    = NULL; 
  UINTN                             nTypeLen     = 0;
  UINTN                             nNameLen     = 0;


  if ((Name == NULL) || 
      (Type == NULL) || 
      (DataSize == NULL))
  { 
    Status = EFI_INVALID_PARAMETER;
    goto FatGetDPPExit;
   }

  nTypeLen = StrLen(Type);
  nNameLen = StrLen(Name);

  if ((nTypeLen > DPP_DATA_TYPE_MAX_LENGTH) ||
      (nNameLen > DPP_DATA_NAME_MAX_LENGTH))
  {
    Status = EFI_INVALID_PARAMETER;
    goto FatGetDPPExit;
  }

  pFilePath = AllocateZeroPool ( 2*(nTypeLen + nNameLen + 2 * StrSize (L"\\")));
  if (pFilePath != NULL)
  {
    StrCat (pFilePath, L"\\");
    StrCat (pFilePath, Type);
    StrCat (pFilePath, L"\\");
    StrCat (pFilePath, Name);
  }
  else
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto FatGetDPPExit;
  }
  
  Status = gBS->HandleProtocol(gDPPFatHandle,
                              &gEfiSimpleFileSystemProtocolGuid,
                              (VOID **)&Fs);
  HANDLE_ERROR_LABEL(Status, gBS->HandleProtocol, FatGetDPPExit);

  Status = Fs->OpenVolume(Fs,&FsRoot);
  HANDLE_ERROR_LABEL(Status, Fs->OpenVolume, FatGetDPPExit); 

  if (FsRoot != NULL)
  {
    Status = FsRoot->Open(FsRoot,
                          &File,
                          pFilePath,                          
                          EFI_FILE_MODE_READ,
                          Attributes);
    HANDLE_ERROR_LABEL(Status, FsRoot->Open, FatGetDPPExit); 
  }
  else
  {
    Status = EFI_NOT_FOUND;
    HANDLE_ERROR_LABEL(Status, FsRoot->Open, FatGetDPPExit); 
  }

  Status = GetFatFileInfo(File,&FileInfoSize,&FileInfo);
  HANDLE_ERROR_LABEL(Status, GetFatFileInfo, FatGetDPPExit);

  if ( (FileInfo != NULL) && (File != NULL))
  {  
    if (Data == NULL) /* return the required buffer size so that caller can 
                         allocate the buffer and call again*/
    {
        *DataSize = FileInfo->FileSize;
        Status = EFI_SUCCESS;
        goto FatGetDPPExit;
    }
    if (*DataSize < FileInfo->FileSize)
    {
      Status = EFI_BUFFER_TOO_SMALL; 
      goto FatGetDPPExit;
    }
    else
    {
      ZeroMem(Data, *DataSize);

      Status = File->Read(File,
                          DataSize,
                          Data);
      HANDLE_ERROR_LABEL(Status, File->Read, FatGetDPPExit);
    }
  }
  else
  {
    Status = EFI_NOT_FOUND;
    HANDLE_ERROR_LABEL(Status, FwUpdateFatGetFileInfo, FatGetDPPExit);
  }
FatGetDPPExit:

  if(FileInfo)
  {
    gBS->FreePool(FileInfo);
    FileInfo = NULL;
  }

  if (File)
  {
    File->Close(File);
    File = NULL; 
  }
    
  if (FsRoot)
  {
    FsRoot->Close(FsRoot);
    FsRoot = NULL;
  }

  if (pFilePath)
  {
    FreePool(pFilePath);
    pFilePath = NULL;
  }

  return Status;

}

/**

  FatDeleteItemDPP()
  
  This function deletes the device provision data from Device
  Provisioning Partition of GPP.
   
  @param[in]  Type          An unicode string of provision data type,
                            e.g. L"QCOM" or L"OEM".
  @param[in]  Name          An unicode string of provision data name,
                            e.g. L"WLAN.PROVISION".
  @return
  EFI_SUCCESS               Provision data is deleted from DPP 
                            successfully. 
  EFI_INVALID_PARAMETER     Invalid parameter. 
  EFI_NOT_FOUND             Specified provision data not found. 
  EFI_XXXX                  Other errors. (Refer to UEFI spec 
                            for details) 
**/
EFI_STATUS
FatDeleteItemDPP(
  IN CHAR16             *Type,
  IN CHAR16             *Name
  )
{

  EFI_STATUS                         Status       = EFI_SUCCESS;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL   *Fs           = NULL;
  EFI_FILE_PROTOCOL                 *FsRoot       = NULL;
  EFI_FILE_PROTOCOL                 *File         = NULL;
  EFI_FILE_INFO                     *FileInfo     = NULL;
  volatile UINT64                    OpenMode;
  UINT64                             Attributes   = 0x0;
  EFI_GUID                           FileInfoGuid = EFI_FILE_INFO_ID ;
  UINTN                              FileInfoSize = 0;
  UINTN                              FileSize     = 0;
  VOID                              *Buffer       = NULL;
  CHAR16                            *pFilePath    = NULL; 
  UINTN                              nTypeLen     = 0;
  UINTN                              nNameLen     = 0;


  if((Type == NULL) ||
     (Name == NULL))

  {
    Status = EFI_INVALID_PARAMETER;
    goto FatDeleteItemDPPExit;
  }
  nTypeLen = StrLen(Type);
  nNameLen = StrLen(Name);

  if ((nTypeLen > DPP_DATA_TYPE_MAX_LENGTH) ||
      (nNameLen > DPP_DATA_NAME_MAX_LENGTH))
  {
    Status = EFI_INVALID_PARAMETER;
    goto FatDeleteItemDPPExit;
  }

  pFilePath = AllocateZeroPool ( 2 * (nTypeLen + nNameLen +StrSize (L"\\\\")));
  if (pFilePath != NULL)
  {
    StrCat (pFilePath, L"\\");
    StrCat (pFilePath, Type);
    StrCat (pFilePath, L"\\");
    StrCat (pFilePath, Name);
  }
  else
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto FatDeleteItemDPPExit;
  }
  
  Status = gBS->HandleProtocol(gDPPFatHandle,
                              &gEfiSimpleFileSystemProtocolGuid,
                              (VOID **)&Fs);

  HANDLE_ERROR_LABEL(Status, gBS->HandleProtocol, FatDeleteItemDPPExit);

  Status = Fs->OpenVolume(Fs,&FsRoot);
  HANDLE_ERROR_LABEL(Status, Fs->OpenVolume, FatDeleteItemDPPExit);

  // Open the file as read and not write in case the file is marked with read-only attribute
  OpenMode = EFI_FILE_MODE_READ;

  Status = FsRoot->Open(FsRoot,
                        &File,
                        pFilePath,                          
                        OpenMode, 
                        Attributes);

  if (Status  == EFI_NOT_FOUND)
  {
    return EFI_SUCCESS;
  }

  HANDLE_ERROR_LABEL(Status, FsRoot->Open, FatDeleteItemDPPExit);

  // First attempt to clear any read-only attribute which would prevent deleting the file   

  Status = GetFatFileInfo(File,&FileInfoSize,&FileInfo);
       
  if(!EFI_ERROR(Status) && (FileInfo != NULL))
  {
    if((FileInfo->Attribute) & EFI_FILE_READ_ONLY)
    {
      FileInfo->Attribute = (FileInfo->Attribute) & (0xFFFFFFFFFFFFFFFE) ;

      /* Validate and correct the FileInfo time is required before callig 
               setInfo function. Also, igore the return status if 
              FwUpdateValidateAndCorrectFileInfoTime returns error */
      ValidateAndCorrectFileInfoTime(&FileInfo->CreateTime);
      ValidateAndCorrectFileInfoTime(&FileInfo->ModificationTime);
      ValidateAndCorrectFileInfoTime(&FileInfo->LastAccessTime);

      File->SetInfo (File, &FileInfoGuid, FileInfo->Size, FileInfo);
      HANDLE_ERROR_LABEL(Status, File->SetInfo, FatDeleteItemDPPExit);
    }
    gBS->FreePool(FileInfo);
    FileInfo = NULL;
  }
  
  // Close and re-open the file with read & write to allow deleting
  File->Close(File);

  OpenMode = EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE;

  Status = FsRoot->Open(FsRoot,
                        &File,
                        pFilePath,                          
                        OpenMode,
                        Attributes);
  HANDLE_ERROR_LABEL(Status, FsRoot->Open, FatDeleteItemDPPExit);

  //read out the file size and zero out the file content before deletion
  Status = GetFatFileInfo(File,&FileInfoSize,&FileInfo);
       
  if(!EFI_ERROR(Status) && (FileInfo != NULL))
  {
    FileSize= FileInfo->FileSize;

    Status = gBS->AllocatePool(EfiBootServicesData, FileSize, (VOID**)&Buffer); 
    if (!EFI_ERROR(Status) && (Buffer != NULL))
    {
      ZeroMem(Buffer,FileSize);

      Status = File->Write(File,&FileSize, Buffer);
      HANDLE_ERROR_LABEL(Status, File->Write, FatDeleteItemDPPExit);

      Status = File->Flush(File);
      HANDLE_ERROR_LABEL(Status, File->Flush, FatDeleteItemDPPExit);

      Status = gBS->FreePool(Buffer);
      HANDLE_ERROR_LABEL(Status, gBS->FreePool, FatDeleteItemDPPExit);

      Buffer = NULL;
    }
    else
    {
      HANDLE_ERROR_LABEL(Status,gBS->AllocatePool,FatDeleteItemDPPExit);
    }
    gBS->FreePool(FileInfo);
    FileInfo = NULL;
  }

  Status = File->Delete(File) ;
  if(!EFI_ERROR(Status))
  {
    File = NULL;
  }
  HANDLE_ERROR_LABEL(Status, File->Delete, FatDeleteItemDPPExit);

  FsRoot->Close(FsRoot);
  if(!EFI_ERROR(Status))
  {
    FsRoot = NULL;
  }

FatDeleteItemDPPExit:

  if(Buffer)
  {
    gBS->FreePool(Buffer);
    Buffer = NULL;
  }

  if(FileInfo)
  {
    gBS->FreePool(FileInfo);
    FileInfo = NULL;
  }

  if (File)
  {
    File->Close(File);
    File = NULL; 
  }
    
  if (FsRoot)
  {
    FsRoot->Close(FsRoot);
    FsRoot = NULL;
  }
  
  if (pFilePath)
  {
    FreePool(pFilePath);
    pFilePath = NULL;
  }
  return Status;

}

/**

  EraseDPP()

  Erase entire DPP partition.
   
  @param[in]  This          A pointer to the calling context. 
   
  @return
  EFI_SUCCESS               Successfully write DPP block data.
  EFI_OUT_OF_RESOURCES      No enough system resource 
  EFI_XXXX                  Other errors. (Refer to UEFI spec for details)

  Note:

  1. Given the DPP size, erasing entire DPP is a time-consuming operation.

**/

EFI_STATUS
EraseDPP(
  IN  EFI_DPP_PROTOCOL  *This
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  VOID        *Buffer = NULL;
  UINTN       BufferSize = 0;

  if (gDppPartType == DPP_PARTITION_TYPE_BLOCK)
  {
    BufferSize = gDppSize;

    ALIGN_TO_BLOCK(BufferSize, gDppBlockSize, BufferSize);

    Buffer = AllocateZeroPool(BufferSize);

    if(Buffer == NULL)
    {
      Status = EFI_OUT_OF_RESOURCES;
      goto EraseDPPExit;
    }

    // Write back to DPP
    Status = WriteDPPBlocks(DPP_HEADER_LBA, BufferSize, Buffer);

    if(EFI_ERROR(Status))
    {
      //DEBUG((EFI_D_WARN, "Failed to write DPP block data at LBA %d, size %d.\n", 0, BufferSize));
      goto EraseDPPExit;
    }

    // Invalid cached DPP partition status
    gCachedDppPartStatusValid = FALSE;
  }
  else
  {
    Status = EFI_UNSUPPORTED;
    DEBUG_ERR_LABEL("Erase DPP unsupported on existing DPP",EraseDPPExit);
  }

EraseDPPExit:

  if(Buffer != NULL)
  {
    FreePool(Buffer);
    Buffer = NULL;
  }

  return Status;
}

/**

  SetDPP()

  Stores device provision data into DPP.

  @param[in]  This          A pointer to the calling context. 
   
  @param[in]  Type          An unicode string specifying provision data type,
                            e.g. "QCOM" or "OEM".
  @param[in]  Name          An unicode string specifying provision data name,
                            e.g. "WLAN.PROVISION"
  @param[in]  Data          A pointer to the buffer that contains provision data
                            to be written.
  @param[in]  DataSize      The size of provision data, in bytes.

  @return
  EFI_SUCCESS               Provision data is written to DPP successfully.
  EFI_INVALID_PARAMETER     Invalid parameter.
  EFI_OUT_OF_RESOURCES      No enough system resource 
  EFI_XXXX                  Other errors. (Refer to UEFI spec for details)

**/
EFI_STATUS
SetDPP(
  IN  EFI_DPP_PROTOCOL  *This,
  IN CHAR16             *Type,
  IN CHAR16             *Name,
  IN VOID               *Data,
  IN UINTN               DataSize
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  if ((Type == NULL) ||
      (Name == NULL) ||
      (Data == NULL) ||
      (DataSize == 0))
  {
    Status = EFI_INVALID_PARAMETER;
    goto SetDPPExit;
  }
  
  if ((StrLen(Type) > DPP_DATA_TYPE_MAX_LENGTH) ||
      (StrLen(Name) > DPP_DATA_NAME_MAX_LENGTH))
  {
    Status = EFI_INVALID_PARAMETER;
    goto SetDPPExit;
  }

  if (gDppPartType == DPP_PARTITION_TYPE_BLOCK)
  {
    Status = BlockSetDPP(Type,Name,Data,DataSize);
    HANDLE_ERROR_LABEL(Status,BlockSetDPP,SetDPPExit);
  }
  else if (gDppPartType == DPP_PARTITION_TYPE_FAT)
  {
    Status = FatSetDPP(Type,Name,Data,DataSize);
    HANDLE_ERROR_LABEL(Status,FatSetDPP,SetDPPExit);
  }
  else
  {
    Status = EFI_UNSUPPORTED;
    DEBUG_ERR_LABEL("Unsupported DPP type",SetDPPExit);
  }

SetDPPExit:

  return Status;
}

/**

  GetDPP()
  
  This function retrieves the device provision data from Device
  Provisioning Partition of GPP.
  
  @param[in]  This          A pointer to the calling context.
   
  @param[in]  Type          An unicode string of provision data type,
                            e.g. L"QCOM" or L"OEM".
  @param[in]  Name          An unicode string of provision data name,
                            e.g. L"WLAN.PROVISION".
  @param[out] Data          A pointer to caller allocated buffer that will
                            store retrieved provision data. If NULL is
                            passed in, this function will return the 
                            provision data size.
  @param[out] DataSize      Upon successful return, the size of provision
                            data.
  
  @return
  EFI_SUCCESS               Provision data is retrieved from DPP successfully.
  EFI_INVALID_PARAMETER     Invalid parameter. 
  EFI_OUT_OF_RESOURCES      No enough system resource.
  EFI_NOT_FOUND             Specified provision data not found.
  EFI_XXXX                  Other errors. (Refer to UEFI spec for details)
 
**/
EFI_STATUS
GetDPP(
  IN  EFI_DPP_PROTOCOL  *This,
  IN CHAR16             *Type,
  IN CHAR16             *Name,
  IN OUT VOID           *Data,
  IN OUT UINTN          *DataSize
  )
{
  EFI_STATUS              Status = EFI_SUCCESS;
    
  if((Type == NULL) ||
     (Name == NULL) ||
     (DataSize == NULL))
  {
    Status = EFI_INVALID_PARAMETER;
    goto GetDPPExit;
  }
    
  if((StrLen(Type) > DPP_DATA_TYPE_MAX_LENGTH) ||
     (StrLen(Name) > DPP_DATA_NAME_MAX_LENGTH))
  {
    Status = EFI_INVALID_PARAMETER;
    goto GetDPPExit;
  }

  if (gDppPartType == DPP_PARTITION_TYPE_BLOCK)
  {
    Status = BlockGetDPP(Type,Name,Data,DataSize);
    HANDLE_ERROR_LABEL(Status,BlockGetDPP,GetDPPExit);
  }
  else if (gDppPartType == DPP_PARTITION_TYPE_FAT)
  {
    Status = FatGetDPP(Type,Name,Data,DataSize);
    HANDLE_ERROR_LABEL(Status,FatGetDPP,GetDPPExit);
  }
  else
  {
    Status = EFI_UNSUPPORTED;
    DEBUG_ERR_LABEL("Unsupported DPP type",GetDPPExit);
  }

GetDPPExit:

  return Status;
}

/**

  DeleteItemDPP()
  
  This function deletes the device provision data from Device
  Provisioning Partition of GPP.
  
  @param[in]  This          A pointer to the calling context.
   
  @param[in]  Type          An unicode string of provision data type,
                            e.g. L"QCOM" or L"OEM".
  @param[in]  Name          An unicode string of provision data name,
                            e.g. L"WLAN.PROVISION".
  @return
  EFI_SUCCESS               Provision data is deleted from DPP 
                            successfully. 
  EFI_INVALID_PARAMETER     Invalid parameter. 
  EFI_NOT_FOUND             Specified provision data not found. 
  EFI_XXXX                  Other errors. (Refer to UEFI spec 
                            for details) 
 
**/
EFI_STATUS
DeleteItemDPP(
  IN  EFI_DPP_PROTOCOL  *This,
  IN CHAR16             *Type,
  IN CHAR16             *Name
  )
{
  EFI_STATUS              Status = EFI_SUCCESS;

  // Check input parameters
  if ((Type == NULL) || (Name == NULL))
  {
    Status = EFI_INVALID_PARAMETER;
    goto DeleteDPPExit;
  }
  
  if ((StrLen(Type) > DPP_DATA_TYPE_MAX_LENGTH) ||
      (StrLen(Name) > DPP_DATA_NAME_MAX_LENGTH))
  {
    Status = EFI_INVALID_PARAMETER;
    goto DeleteDPPExit;
  }


  if (gDppPartType == DPP_PARTITION_TYPE_BLOCK)
  {
    Status = BlockDeleteItemDPP(Type,Name);
    HANDLE_ERROR_LABEL(Status,BlockSetDPP,DeleteDPPExit);
  }
  else if (gDppPartType == DPP_PARTITION_TYPE_FAT)
  {
    Status = FatDeleteItemDPP(Type,Name);
    HANDLE_ERROR_LABEL(Status,FatSetDPP,DeleteDPPExit);
  }
  else
  {
    Status = EFI_UNSUPPORTED;
    DEBUG_ERR_LABEL("Unsupported DPP type",DeleteDPPExit);
  }
  
DeleteDPPExit:

  return Status;
}

/**
  UEFI DPP Protocol implementation
 */
static EFI_DPP_PROTOCOL gEfiDppProtocol = 
{
  EFI_DPP_PROTOCOL_REVISION,
  SetDPP,
  GetDPP,
  EraseDPP,
  DeleteItemDPP
};

/** 
  
  BlockDppProtocolInit()
  
  Initializes Block-based DPP protocol
 
  @param[in]  ImageHandle   The firmware allocated handle for the EFI image.  
  @param[in]  SystemTable   A pointer to the EFI System Table.
    
  @return 
  EFI_SUCCESS               DPP protocol initialized successfully.
  EFI_UNSUPPORTED           DPP protocol not supported.
  EFI_XXXXXX                Failure occured. (Refer to UEFI spec for details) 
   
**/

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
  
  FatDppProtocolInit()
  
  Initializes Fat-based DPP protocol
 
  @param[in]  ImageHandle   The firmware allocated handle for the EFI image.  
  @param[in]  SystemTable   A pointer to the EFI System Table.
    
  @return 
  EFI_SUCCESS               DPP protocol initialized successfully.
  EFI_UNSUPPORTED           DPP protocol not supported.
  EFI_XXXXXX                Failure occured. (Refer to UEFI spec for details) 
   
**/

EFI_STATUS
FatDppProtocolInit(
  IN EFI_HANDLE DppHandle
  )
{
    EFI_STATUS  Status = EFI_SUCCESS;
  
    if(DppHandle == NULL)
    {
      Status = EFI_UNSUPPORTED;
      goto FatDppProtocolInit;
    }
    
    gDPPFatHandle = DppHandle;

FatDppProtocolInit:

    return Status;
}

/** 
  
  InitializeDppProtocol()
  
  Initializes DPP protocol
 
  @param[in]  ImageHandle   The firmware allocated handle for the EFI image.  
  @param[in]  SystemTable   A pointer to the EFI System Table.
    
  @return 
  EFI_SUCCESS               DPP protocol initialized successfully.
  EFI_UNSUPPORTED           DPP protocol not supported.
  EFI_XXXXXX                Failure occured. (Refer to UEFI spec for details) 
   
**/

EFI_STATUS
InitializeDppProtocol(
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  EFI_HANDLE  PartHandle = NULL;
    
  Status = GetPartitionHandle(&PartHandle,
                              &gEfiDppBlockPartitionGuid,
                              DPP_NAME,
                              &gEfiBlockIoProtocolGuid);
  if(!EFI_ERROR(Status) && (PartHandle != NULL))
  {
    Status = BlockDppProtocolInit(PartHandle);
    HANDLE_ERROR_LABEL(Status, BlockDppProtocolInit, InitializeDppProtocolExit);
    gDppPartType = DPP_PARTITION_TYPE_BLOCK;

  }else if (Status == EFI_NOT_FOUND)
  {
    Status = GetPartitionHandle(&PartHandle,
                                &gEfiDppFatPartitionGuid,
                                DPP_NAME,
                                &gEfiSimpleFileSystemProtocolGuid);
    if(!EFI_ERROR(Status) && (PartHandle != NULL))
    {
      Status = FatDppProtocolInit(PartHandle);
      HANDLE_ERROR_LABEL(Status, FatDppProtocolInit, InitializeDppProtocolExit);
      gDppPartType = DPP_PARTITION_TYPE_FAT;
    }
    else
    {
      Status = EFI_UNSUPPORTED;
      DEBUG_ERR_LABEL("\nWarning: DPP not found, please check if DPP is formated \n",
                      InitializeDppProtocolExit);
    }
  }
  else
  {
     Status = EFI_UNSUPPORTED;
     DEBUG_ERR_LABEL("Warning: Block DPP unsupported platform type \n",InitializeDppProtocolExit);
  }

  Status = gBS->InstallMultipleProtocolInterfaces(&ImageHandle,
                                                  &gEfiDppProtocolGuid,
                                                  (void *)&gEfiDppProtocol,
                                                  NULL);
  HANDLE_ERROR_LABEL(Status, gBS->InstallMultipleProtocolInterfaces, InitializeDppProtocolExit);
  if (gDppPartType == DPP_PARTITION_TYPE_BLOCK)
  {
    DEBUG((EFI_D_WARN, "Block"));
  }
  else if (gDppPartType == DPP_PARTITION_TYPE_FAT)
  {
    DEBUG((EFI_D_WARN, "FAT"));
  }
  else
  { /* shouldn't happen */
    DEBUG((EFI_D_WARN, "Unknown"));
  }
  DEBUG((EFI_D_WARN, " DPP protocol installed!\n"));
  
InitializeDppProtocolExit:


  return Status;
}
