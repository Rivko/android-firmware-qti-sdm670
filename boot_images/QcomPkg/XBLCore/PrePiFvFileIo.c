/** @file PrePiFvFileIo.c

  Source for loading files from an FV image in pre PI environment

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 06/29/16   bh       Initial revision
=============================================================================*/

#include <Library/PrePiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <PrePiFvFileIo.h>

#define MAX_FV0_FILENAME_LEN 64

EFI_STATUS
LoadFileFromFV0 (IN   CHAR8*    Name, 
                 OUT  UINT8**   FileBuffer,
                 OUT  UINTN*    FileSize)
{
  EFI_STATUS                 Status;
  CHAR8                      mFileSize[4] = {0};
  CHAR16                    *FNamePtr;
  CHAR16                     RequestedName[MAX_FV0_FILENAME_LEN];
  EFI_COMMON_SECTION_HEADER *SectionHeader;
  EFI_PEI_FV_HANDLE          VolumeHandle;
  EFI_PEI_FILE_HANDLE        FileHandle;

  SectionHeader = NULL;

  if (!Name || !FileBuffer || !FileSize)
    return EFI_INVALID_PARAMETER;

  AsciiStrToUnicodeStr (Name, RequestedName);
  if (Name == NULL)
    return EFI_INVALID_PARAMETER;

  Status = FfsAnyFvFindFirstFile (EFI_FV_FILETYPE_FREEFORM, &VolumeHandle, &FileHandle);

  while (Status == EFI_SUCCESS)
  {
    FNamePtr = NULL;
    if (FileHandle != NULL) {
      Status = FfsFindSectionData (EFI_SECTION_USER_INTERFACE, FileHandle, (VOID**)&FNamePtr);

      if (!EFI_ERROR (Status) && FNamePtr && (StrnCmp (RequestedName, FNamePtr, MAX_FV0_FILENAME_LEN) == 0)){
        //
        // Find RAW section in FileHandle
        //
        Status = FfsFindSectionData (EFI_SECTION_RAW, FileHandle, (VOID **)FileBuffer);
        if (EFI_ERROR (Status)) {
          return Status;
        }

        /* Section header is in word just before FileBuffer - use this to get the file size */
        SectionHeader = (EFI_COMMON_SECTION_HEADER*)((UINTN)(*FileBuffer) - (4*sizeof(CHAR8)));
        mFileSize[0] = SectionHeader->Size[0];
        mFileSize[1] = SectionHeader->Size[1];
        mFileSize[2] = SectionHeader->Size[2];
        CopyMem(FileSize, (VOID*)mFileSize, sizeof(UINTN));

        /* Subtract out size of EFI_COMMON_SECTION_HEADER */
        *FileSize -= sizeof(EFI_COMMON_SECTION_HEADER);
        return Status;
      }
    }
    Status = FfsFindNextFile (EFI_FV_FILETYPE_FREEFORM, VolumeHandle, &FileHandle);
  }

  return Status;
}

