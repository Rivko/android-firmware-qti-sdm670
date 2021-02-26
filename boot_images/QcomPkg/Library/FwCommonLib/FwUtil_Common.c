/** @file FwCommonHelpers.c
   
  Functions common to all FwCommonLib.

  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2011/10/12     jthompso    Initial version

=============================================================================*/

#include <Uefi.h>
#include <Guid/FileInfo.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/FwCommonLib.h>
#include <Library/QcomLib.h>
#include <Library/DebugLib.h>
#include <Protocol/SimpleFileSystem.h>
#include "FwUtil_Common.h"


/**
 * Used to fill a 
 * function pointer in 
 * the case where there is no 
 * applicable function.
 * 
 * 
 * @return EFI_STATUS - EFI_SUCCESS
 */
EFI_STATUS
DummyFunction(void)
{
    return EFI_SUCCESS;
}

/**
 * Wrapper to gBS->Allocatepool. 
 *  
 * Clears buffer returned by 
 * gBS->Allocatepool. 
 * 
 * 
 * @param PoolType - PoolType for AllocatPool 
 * @param Size -     Size of buffer
 * @param Buffer -   Buffer 
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
AllocateAndZero(
    IN EFI_MEMORY_TYPE  PoolType,
    IN UINTN            Size,
    OUT VOID            **Buffer
    )
{
    EFI_STATUS Status;
    Status = gBS->AllocatePool(PoolType,Size,Buffer);

    if(EFI_ERROR(Status))
    {
        return Status;
    }
    ZeroMem(*Buffer,Size );
    return EFI_SUCCESS;
};

/**
 * print guid
 *  
 * @param guid - guid 
 * 
 * @return void
 */
VOID
FwCommonPrintGuid(
  IN EFI_GUID  guid,
  IN UINT32 DebugLevel
  )
{
  UINTN i = 0;
  DEBUG((DebugLevel,"{0x%08x,0x%04x,0x%04x,{",guid.Data1,guid.Data2,guid.Data3));


  for (i = 0; i < 8; i++)
  {
    DEBUG((DebugLevel,"0x%02x",guid.Data4[i]));
    if(i!=7)
    {
      DEBUG((DebugLevel,","));
    }
  }
  DEBUG((DebugLevel,"}}"));


}

EFI_STATUS
FwUpdateBlockFileAlignFileBuffer2(
    IN EFI_MEMORY_TYPE  PoolType,
    IN UINTN            FileSize,
    IN VOID             *SrcBuffer,
    OUT VOID            **OutBuffer,
    IN UINTN            FileSizeAligned
    )
{
  EFI_STATUS Status            = EFI_SUCCESS;
  VOID*      FileBufferAligned = NULL;

  if((SrcBuffer == NULL) ||
     (OutBuffer == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }
  Status = AllocateAndZero(PoolType,
                           FileSizeAligned,
                           &FileBufferAligned);
  ERR_HANDLE(Status);

  if(SrcBuffer != NULL)
  {
    CopyMem(FileBufferAligned,SrcBuffer,FileSize);

  }
  *OutBuffer = FileBufferAligned;

  return Status;
    
}

EFI_STATUS
FwUpdateBlockFileAlignFileBuffer(
    IN EFI_MEMORY_TYPE  PoolType,
    IN UINTN            FileSize,
    OUT VOID            **FileBuffer,
    IN UINTN            FileSizeAligned
    )
{
  EFI_STATUS Status            = EFI_SUCCESS;
  VOID*      FileBufferAligned = NULL;

  if(FileBuffer == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }
  Status = AllocateAndZero(PoolType,
                           FileSizeAligned,
                           &FileBufferAligned);
  ERR_HANDLE(Status);

  if(*FileBuffer != NULL)
  {
    CopyMem(FileBufferAligned,*FileBuffer,FileSize);
    Status = gBS->FreePool(*FileBuffer); 
    ERR_HANDLE(Status);
  }
  *FileBuffer = FileBufferAligned;

  return Status;
    
}



