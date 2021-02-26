/** @file FwFileIoWrapper.c
   
  Commonly used FV IO Wrapper used for legacy 
  and data-driven firmware update code.

  Copyright (c) 2012-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2015/04/15     mic         Added 64bit firmware update support
 2012/12/11     mic         Fixed KW warnings
 2012/12/04     mic         Added fix for FatWrite ASSERT issue
 2012/11/14     mic         Added safe FAT write function
 2012/11/12     mic         Added FwUpdateValidateAndCorrectFileInfoTime 
                            function
 2012/11/05     rs          Added support for deletion of DPP item
 2012/10/04     mic         Initial version

=============================================================================*/

#include <PiDxe.h>
#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/FwCommonLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Protocol/EFIDpp.h>
#include <Protocol/BlockIo.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Pi/PiDxeCis.h>
#include <Protocol/FirmwareVolume2.h>

#include "FwFileIoWrapper.h"

#define FIRST_BLOCK       0
#define DEFAULT_TRY_COUNT 2

extern EFI_GUID gQcomTokenSpaceGuid;


/**
 * Validate and correct FileInfo time
 * 
 * @param Time   - FAT file info structure
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateValidateAndCorrectFileInfoTime(
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
 * Delete a FAT file by a given Handle and file name.
 * 
 * @param PartHandle - Handle of partition
 * @param FileName   - File name
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateFatDelete(
    IN EFI_HANDLE               PartHandle,
    IN CHAR16                   *FileName

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


  if((PartHandle == NULL) ||
     (FileName == NULL))

  {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->HandleProtocol(PartHandle,
                              &gEfiSimpleFileSystemProtocolGuid,
                              (VOID **)&Fs);

  HANDLE_ERROR_LABEL(Status, gBS->HandleProtocol, FwUpdateFatDeleteExit);

  Status = Fs->OpenVolume(Fs,&FsRoot);
  HANDLE_ERROR_LABEL(Status, Fs->OpenVolume, FwUpdateFatDeleteExit);

  // Open the file as read and not write in case the file is marked with read-only attribute
  OpenMode = EFI_FILE_MODE_READ;

  Status = FsRoot->Open(FsRoot,
                        &File,
                        FileName,                          
                        OpenMode, 
                        Attributes);

  if (Status  == EFI_NOT_FOUND)
  {
    return EFI_SUCCESS;
  }

  HANDLE_ERROR_LABEL(Status, FsRoot->Open, FwUpdateFatDeleteExit);

  // First attempt to clear any read-only attribute which would prevent deleting the file   

  Status = FwUpdateFatGetFileInfo(File,&FileInfoSize,&FileInfo);
       
  if(!EFI_ERROR(Status) && (FileInfo != NULL))
  {
    if((FileInfo->Attribute) & EFI_FILE_READ_ONLY)
    {
      FileInfo->Attribute = (FileInfo->Attribute) & (0xFFFFFFFFFFFFFFFE) ;

      /*Validate and correct the FileInfo time is required before callig 
        setInfo function. Also, igore the return status if 
        FwUpdateValidateAndCorrectFileInfoTime returns error */
      FwUpdateValidateAndCorrectFileInfoTime(&FileInfo->CreateTime);
      FwUpdateValidateAndCorrectFileInfoTime(&FileInfo->ModificationTime);
      FwUpdateValidateAndCorrectFileInfoTime(&FileInfo->LastAccessTime);

      File->SetInfo (File, &FileInfoGuid, FileInfo->Size, FileInfo);
      HANDLE_ERROR_LABEL(Status, File->SetInfo, FwUpdateFatDeleteExit);
    }
    gBS->FreePool(FileInfo);
    FileInfo = NULL;
  }
  
  // Close and re-open the file with read & write to allow deleting
  File->Close(File);

  OpenMode = EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE;

  Status = FsRoot->Open(FsRoot,
                        &File,
                        FileName,                          
                        OpenMode,
                        Attributes);
  HANDLE_ERROR_LABEL(Status, FsRoot->Open, FwUpdateFatDeleteExit);

  //read out the file size and zero out the file content before deletion
  Status = FwUpdateFatGetFileInfo(File,&FileInfoSize,&FileInfo);
       
  if(!EFI_ERROR(Status) && (FileInfo != NULL))
  {
    FileSize= FileInfo->FileSize;

    Status = gBS->AllocatePool(EfiBootServicesData, FileSize, (VOID**)&Buffer); 
    if (!EFI_ERROR(Status) && (Buffer != NULL))
    {
      ZeroMem(Buffer,FileSize);

      Status = File->Write(File,&FileSize, Buffer);
      HANDLE_ERROR_LABEL(Status, File->Write, FwUpdateFatDeleteExit);

      Status = File->Flush(File);
      HANDLE_ERROR_LABEL(Status, File->Flush, FwUpdateFatDeleteExit);

      Status = gBS->FreePool(Buffer);
      HANDLE_ERROR_LABEL(Status, gBS->FreePool, FwUpdateFatDeleteExit);

      Buffer = NULL;
    }
    else
    {
      HANDLE_ERROR_LABEL(Status,gBS->AllocatePool,FwUpdateFatDeleteExit);
    }
    gBS->FreePool(FileInfo);
    FileInfo = NULL;
  }

  Status = File->Delete(File) ;
  if(!EFI_ERROR(Status))
  {
    File = NULL;
  }
  HANDLE_ERROR_LABEL(Status, File->Delete, FwUpdateFatDeleteExit);

  FsRoot->Close(FsRoot);
  if(!EFI_ERROR(Status))
  {
    FsRoot = NULL;
  }

FwUpdateFatDeleteExit:

    //Cleanup
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
  return Status;
}

/**
 * First write a file to FAT  by a given Handle, buffer, buffer size and file 
 * name, then double check if it is written correctly.
 * 
 * @param PartHandle - Handle of partition
 * @param FileName   - File name
 * @param FileBuffer - Buffer
 * @param BufferSize - Buffer size
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateFatWriteAndCheck(
    IN EFI_HANDLE  PartHandle,
    IN CHAR16     *FileName,
    IN VOID       *Buffer,
    IN OUT UINTN   BufferSize
    )
{
  EFI_STATUS Status      = EFI_SUCCESS;
  VOID      *TempBuf     = NULL;
  UINTN      TempBufSize = 0; 
  UINTN      i           = 0;
  
  if ((PartHandle == NULL) || 
      (FileName == NULL) || 
      (Buffer == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  for (i = 0; i < DEFAULT_TRY_COUNT; i++)
  {
    /* Write the given buffer to the FAT partition PartHandle points to */
    Status = FwUpdateFatWrite(PartHandle,
                              FileName,
                              Buffer,
                              BufferSize);
    HANDLE_ERROR_LABEL(Status,FwUpdateFatWrite,FwUpdateFatWriteAndCheckExit);

    /* Read out the written FAT file */
    Status = FwUpdateFatRead(PartHandle,
                             FileName,
                             &TempBuf,
                             &TempBufSize);
    HANDLE_ERROR_LABEL(Status,FwUpdateFatRead,FwUpdateFatWriteAndCheckExit);
    
    /* 1. Compare the size of the file and the size of the original buffer, it should
          match.
       2. Compare the content of the file and the content of the original buffer, 
          it should match.
       3. Set the Status to EFI_VOLUME_CORRUPTED and continue if it 
          doesn't match */

    if((BufferSize == TempBufSize) &&
       (CompareMem(Buffer, TempBuf, BufferSize) == 0))
    {
      Status = EFI_SUCCESS;
      goto FwUpdateFatWriteAndCheckExit;
    }
    else
    {
      Status = EFI_VOLUME_CORRUPTED; 
      continue;
    }

  }
  
FwUpdateFatWriteAndCheckExit:

  if(TempBuf != NULL)
  {
    gBS->FreePool(TempBuf);
    TempBuf = NULL;
  }

  return Status;
}

/**
 * Write a file to FAT  by a given Handle, buffer, buffer size and file name.
 * 
 * @param PartHandle - Handle of partition
 * @param FileName   - File name
 * @param FileBuffer - Buffer
 * @param BufferSize - Buffer size
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateFatWrite(
    IN EFI_HANDLE               PartHandle,
    IN CHAR16                   *FileName,
    IN VOID                     *Buffer,
    IN OUT UINTN                BufferSize
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

  if ((PartHandle == NULL) || 
      (FileName == NULL) || 
      (Buffer == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->HandleProtocol(PartHandle,
                              &gEfiSimpleFileSystemProtocolGuid,
                              (VOID **)&Fs);
  HANDLE_ERROR_LABEL(Status, gBS->HandleProtocol, FwUpdateFatWriteExit);

  Status = Fs->OpenVolume(Fs,&FsRoot);
  HANDLE_ERROR_LABEL(Status, Fs->OpenVolume, FwUpdateFatWriteExit);

  if (FsRoot != NULL)
  {
    OpenMode = EFI_FILE_MODE_READ|EFI_FILE_MODE_WRITE|EFI_FILE_MODE_CREATE;

    Status = FsRoot->Open(FsRoot,
                          &File,
                          FileName,                          
                          OpenMode, 
                          Attributes);

    if(EFI_ERROR(Status))
    {
      if(Status == EFI_NOT_FOUND)
      {
        //Tried to write file to nonexistent directory,
        //need to create the directory and try again.

        Status = FwUpdateCreateDirectory(FsRoot,
                                         FileName,
                                         Attributes);
        HANDLE_ERROR_LABEL(Status, FwUpdateCreateDirectory, FwUpdateFatWriteExit);

        Status = FsRoot->Open(FsRoot,
                              &File,
                              FileName,                          
                              OpenMode,
                              Attributes);
        HANDLE_ERROR_LABEL(Status, FsRoot->Open, FwUpdateFatWriteExit);
      }
      else
      {
        HANDLE_ERROR_LABEL(Status, FsRoot->Open, FwUpdateFatWriteExit);
      }
    }
  }
  else
  {
    Status = EFI_NOT_FOUND;
    HANDLE_ERROR_LABEL(Status, Fs->OpenVolume, FwUpdateFatWriteExit);
  }

  if (File != NULL)
  {
    Status = FwUpdateFatGetFileInfo(File,&FileInfoSize,&FileInfo);
         
    if((!EFI_ERROR(Status)) && (FileInfo != NULL))
    {
      //Clear out the read only attribute
      if((FileInfo->Attribute) & EFI_FILE_READ_ONLY)
      {
        FileInfo->Attribute = (FileInfo->Attribute) & (0xFFFFFFFFFFFFFFFE) ;

        /*Validate and correct the FileInfo time is required before callig 
          setInfo function. Also, igore the return status if 
          FwUpdateValidateAndCorrectFileInfoTime returns error */
        FwUpdateValidateAndCorrectFileInfoTime(&FileInfo->CreateTime);
        FwUpdateValidateAndCorrectFileInfoTime(&FileInfo->ModificationTime);
        FwUpdateValidateAndCorrectFileInfoTime(&FileInfo->LastAccessTime);

        Status = File->SetInfo (File, &FileInfoGuid, FileInfo->Size, FileInfo);
      }
    }

    Status = File->Write(File,
                         &BufferSize,
                         Buffer);
    HANDLE_ERROR_LABEL(Status, File->Write, FwUpdateFatWriteExit);

    if(FileInfo != NULL)
    {
      gBS->FreePool(FileInfo);
      FileInfo = NULL;
    }
   
    Status = FwUpdateFatGetFileInfo(File,&FileInfoSize,&FileInfo);
    HANDLE_ERROR_LABEL(Status, FwUpdateFatGetFileInfo, FwUpdateFatWriteExit);

    if(FileInfo != NULL)
    {
      //Clear out the read only attribute
      if((FileInfo->Attribute) & EFI_FILE_READ_ONLY)
      {
        FileInfo->Attribute = (FileInfo->Attribute) & (0xFFFFFFFFFFFFFFFE) ;
      }
      FileInfo->FileSize = BufferSize;
        
      /*Validate and correct the FileInfo time is required before callig 
        setInfo function. Also, igore the return status if 
        FwUpdateValidateAndCorrectFileInfoTime returns error */
      FwUpdateValidateAndCorrectFileInfoTime(&FileInfo->CreateTime);
      FwUpdateValidateAndCorrectFileInfoTime(&FileInfo->ModificationTime);
      FwUpdateValidateAndCorrectFileInfoTime(&FileInfo->LastAccessTime);
        
      Status = File->SetInfo (File, &FileInfoGuid, FileInfo->Size, FileInfo);
      HANDLE_ERROR_LABEL(Status, File->SetInfo, FwUpdateFatWriteExit);

      Status = File->Flush(File);
      HANDLE_ERROR_LABEL(Status, File->Flush, FwUpdateFatWriteExit);
    }
  }
  else
  {
    Status = EFI_NOT_FOUND;
    HANDLE_ERROR_LABEL(Status, FsRoot->Open, FwUpdateFatWriteExit);

  }

FwUpdateFatWriteExit:

  //Cleanup
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

  return Status;
}


/**
 * Read a file from FAT into a buffer by a given Handle and file name.
 * 
 * @param PartHandle - Handle of partition
 * @param FileName   - File name
 * @param FileBuffer - Buffer
 * @param BufferSize - Buffer size
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateFatRead(
    IN EFI_HANDLE               PartHandle,
    IN CHAR16                   *FileName,
    OUT VOID                    **Buffer,
    OUT UINTN                   *BufferSize
    )
{
  EFI_STATUS                        Status       = EFI_SUCCESS;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL   *Fs          = NULL;
  EFI_FILE_PROTOCOL                 *FsRoot      = NULL;
  EFI_FILE_PROTOCOL                 *File        = NULL;
  EFI_FILE_INFO                     *FileInfo    = NULL;
  UINTN                             FileInfoSize = 0;
  UINT64                            Attributes   = 0x0;

  if ((PartHandle == NULL) ||
      (FileName == NULL) || 
      (Buffer == NULL) || 
      (BufferSize == NULL))
  { 
    return EFI_INVALID_PARAMETER;
  }
  *Buffer = NULL;

  Status = gBS->HandleProtocol(PartHandle,
                              &gEfiSimpleFileSystemProtocolGuid,
                              (VOID **)&Fs);
  HANDLE_ERROR_LABEL(Status, gBS->HandleProtocol, FwUpdateFatReadExit);

  Status = Fs->OpenVolume(Fs,&FsRoot);
  HANDLE_ERROR_LABEL(Status, Fs->OpenVolume, FwUpdateFatReadExit); 

  if (FsRoot != NULL)
  {
    Status = FsRoot->Open(FsRoot,
                          &File,
                          FileName,                          
                          EFI_FILE_MODE_READ,
                          Attributes);
    HANDLE_ERROR_LABEL(Status, FsRoot->Open, FwUpdateFatReadExit); 
  }
  else
  {
    Status = EFI_NOT_FOUND;
    HANDLE_ERROR_LABEL(Status, FsRoot->Open, FwUpdateFatReadExit); 
  }

  Status = FwUpdateFatGetFileInfo(File,&FileInfoSize,&FileInfo);
  HANDLE_ERROR_LABEL(Status, FwUpdateFatGetFileInfo, FwUpdateFatReadExit);

  if ( (FileInfo != NULL) && (File != NULL))
  {
    *BufferSize = FileInfo->FileSize;
    
    Status = AllocateAndZero(EfiBootServicesData,*BufferSize,Buffer);
    HANDLE_ERROR_LABEL(Status, AllocateAndZero, FwUpdateFatReadExit);

    Status = File->Read(File,
                        BufferSize,
                        *Buffer);
    HANDLE_ERROR_LABEL(Status, File->Read, FwUpdateFatReadExit);
  }
  else
  {
    Status = EFI_NOT_FOUND;
    HANDLE_ERROR_LABEL(Status, FwUpdateFatGetFileInfo, FwUpdateFatReadExit);
  }
FwUpdateFatReadExit:

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

  /* Free up the buffer if there was a failure in Read(). */
  if (EFI_ERROR(Status))
  {
    if (*Buffer != NULL)
    {
      gBS->FreePool(*Buffer);
      *Buffer = NULL;
    }
  }

  return Status;
}


/**
 * Delete a DPP item by given Type,Name
 * 
 * @param Type       - Dpp type
 * @param DppName    - Dpp name
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateDppDelete(
    IN CHAR16                   *Type,
    IN CHAR16                   *DPPname

    )
{
  EFI_STATUS                   Status      = EFI_SUCCESS;
  EFI_DPP_PROTOCOL            *DppProtocol = NULL;

  if ((DPPname == NULL) ||  
      (Type == NULL)) 
  {
    return EFI_INVALID_PARAMETER;
  }
  // Get DPP protocol interface
  Status = gBS->LocateProtocol(&gEfiDppProtocolGuid,
                               NULL,
                               (VOID**)&DppProtocol);
  HANDLE_ERROR_LABEL(Status, gBS->LocateProtocol, FwUpdateDppDeleteExit);

  //Delete provisioning data from DPP.
  Status = DppProtocol->DeleteItemDPP(DppProtocol,
                                      Type,
                                      DPPname);
  HANDLE_ERROR_LABEL(Status, gBS->LocateProtocol, FwUpdateDppDeleteExit);

FwUpdateDppDeleteExit:

  return Status;
}

/**
 * Write a buffer to DPP by given Type,Name,buffer and buffer size
 * 
 * @param Type       - Dpp type
 * @param DppName    - Dpp name
 * @param FileBuffer - Buffer
 * @param BufferSize - Buffer size
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateDppWrite(
    IN CHAR16                   *Type,
    IN CHAR16                   *DPPname,
    IN VOID                     *Buffer,
    IN UINTN                    BufferSize

    )
{
  EFI_STATUS                   Status      = EFI_SUCCESS;
  EFI_DPP_PROTOCOL            *DppProtocol = NULL;

  if ((DPPname == NULL) || 
      (Buffer == NULL) || 
      (Type == NULL)) 
  {
    return EFI_INVALID_PARAMETER;
  }

  // Get DPP protocol interface
  Status = gBS->LocateProtocol(&gEfiDppProtocolGuid,
                               NULL,
                               (VOID**)&DppProtocol);
  HANDLE_ERROR_LABEL(Status, gBS->LocateProtocol, FwUpdateDppWriteExit);

  //Store provisioning data into DPP.
  Status = DppProtocol->SetDPP(DppProtocol,
                              Type,
                              DPPname,
                              Buffer,
                              BufferSize);
  HANDLE_ERROR_LABEL(Status, gBS->LocateProtocol, FwUpdateDppWriteExit);

FwUpdateDppWriteExit:

  return Status;
}


/**
 * Read from DPP into a buffer by a given Name.
 * 
 * @param Type       - Dpp type
 * @param DppName    - Dpp name
 * @param FileBuffer - Buffer
 * @param BufferSize - Buffer size
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateDppRead(
    IN CHAR16                   *Type,
    IN CHAR16                   *DPPName,
    OUT VOID                    **Buffer,
    IN  UINTN                   *BufferSize

    )
{
  EFI_STATUS                   Status      = EFI_SUCCESS;
  EFI_DPP_PROTOCOL            *DppProtocol = NULL;
  UINTN                        iBufferSize = 0;

  if ((Type == NULL) || 
      (DPPName == NULL) || 
      (Buffer == NULL) || 
      (BufferSize == NULL)) 
  {   
    return EFI_INVALID_PARAMETER;
  }

  *Buffer = NULL;
  // Get DPP protocol interface
  Status = gBS->LocateProtocol(&gEfiDppProtocolGuid,
                               NULL,
                               (VOID**)&DppProtocol);
  HANDLE_ERROR_LABEL(Status, gBS->LocateProtocol, FwUpdateDppReadExit);

  //Call with NULL to get the size
  Status = DppProtocol->GetDPP(DppProtocol,
                               Type,
                               DPPName,
                               NULL,
                               &iBufferSize);
  HANDLE_ERROR_LABEL(Status, DppProtocol->GetDPP, FwUpdateDppReadExit);

  Status = AllocateAndZero(EfiBootServicesData,iBufferSize,Buffer);
  HANDLE_ERROR_LABEL(Status, AllocateAndZero, FwUpdateDppReadExit);

  //Call with the buffer now to get the DPP item
  Status = DppProtocol->GetDPP(DppProtocol,
                               Type,
                               DPPName,
                               *Buffer,
                               &iBufferSize);
  HANDLE_ERROR_LABEL(Status, DppProtocol->GetDPP, FwUpdateDppReadExit);

  *BufferSize = iBufferSize;
  return EFI_SUCCESS;

FwUpdateDppReadExit:

  if (EFI_ERROR(Status))
  {
    if (*Buffer != NULL)
    {
      gBS->FreePool(*Buffer);
      *Buffer = NULL;
    }
  }
  return Status;
}

/**
 * Delete a partition by a given handle.
 * 
 * @param PartHandle - Handle of partition
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdatePartitionDelete(
    IN EFI_HANDLE               PartHandle
    )
{
  EFI_STATUS                   Status      = EFI_SUCCESS;
  EFI_BLOCK_IO_PROTOCOL       *BlockIo     = NULL;
  UINTN                        BlockSize   = 0;
  UINTN                        BlockCount  = 0;
  UINT64                       BlockNumber = 0;
  VOID                        *Buffer      = NULL;

  if (PartHandle == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->HandleProtocol(PartHandle,
                               &gEfiBlockIoProtocolGuid,
                               (VOID**)&BlockIo);
  HANDLE_ERROR_LABEL(Status, gBS->HandleProtocol, FwUpdatePartitionDeleteExit);

  BlockSize = BlockIo->Media->BlockSize;
  BlockCount = (BlockIo->Media->LastBlock)+1;

  Status = AllocateAndZero(EfiBootServicesData,BlockSize,&Buffer);
  HANDLE_ERROR_LABEL(Status, AllocateAndZero, FwUpdatePartitionDeleteExit);

  //Writes out Zeros onto the partition one block at a time
  for (BlockNumber = 0; BlockNumber < BlockCount; BlockNumber++)
  {
    Status = BlockIo->WriteBlocks(BlockIo,
                                BlockIo->Media->MediaId,
                                BlockNumber,
                                BlockSize,
                                Buffer);
    HANDLE_ERROR_LABEL(Status, BlockIo->WriteBlocks, FwUpdatePartitionDeleteExit);
  }

FwUpdatePartitionDeleteExit:

  //Cleanup
  if(Buffer)
  {
    gBS->FreePool(Buffer);
    Buffer = NULL;
  }

  return Status;
}

/**
 * Write to block by a given buffer, buffer size and handle.
 * 
 * @param PartHandle - Handle of partition
 * @param FileBuffer - Buffer
 * @param BufferSize - Buffer size
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdatePartitionWrite(
    IN EFI_HANDLE               PartHandle,
    IN VOID                     *Buffer,
    IN UINTN                    BufferSize

    )
{
  EFI_STATUS               Status         = EFI_SUCCESS;
  EFI_BLOCK_IO_PROTOCOL    *BlockIo       = NULL;
  UINTN                    BlockSize      = 0;
  VOID                     *AlignedBuffer = NULL;

  if ((PartHandle == NULL) || (Buffer == NULL))
  { 
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->HandleProtocol(PartHandle,
                               &gEfiBlockIoProtocolGuid,
                               (VOID**)&BlockIo);
  HANDLE_ERROR_LABEL(Status, gBS->HandleProtocol, FwUpdatePartitionWriteExit);

  BlockSize = BlockIo->Media->BlockSize;

  //Align length to block size.
  ALIGN_TO_BLOCK(BufferSize, BlockSize, BufferSize);


  Status = FwUpdateBlockFileAlignFileBuffer2(EfiBootServicesData, 
                                             BufferSize, 
                                             Buffer,
                                             &AlignedBuffer, 
                                             BufferSize);
  HANDLE_ERROR_LABEL(Status, FwUpdateBlockFileAlignFileBuffer2, FwUpdatePartitionWriteExit);

  Status = BlockIo->WriteBlocks(BlockIo,
                                BlockIo->Media->MediaId,
                                (UINT64) FIRST_BLOCK,
                                BufferSize,
                                AlignedBuffer);
  HANDLE_ERROR_LABEL(Status, BlockIo->WriteBlocks, FwUpdatePartitionWriteExit);

  Status = BlockIo->FlushBlocks(BlockIo);
  HANDLE_ERROR_LABEL(Status, BlockIo->FlushBlocks, FwUpdatePartitionWriteExit);

FwUpdatePartitionWriteExit:

  if(AlignedBuffer != NULL)
  {
    gBS->FreePool(AlignedBuffer);
    AlignedBuffer = NULL;
  }

  return Status;
}

/**
 * Read from Block into a buffer by a given handle.
 * 
 * @param PartHandle - Handle of partition
 * @param Buffer - Buffer
 * @param BufferSize - Buffer size
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdatePartitionRead(
    IN EFI_HANDLE               PartHandle,
    OUT VOID                    **Buffer,
    OUT UINTN                   *BufferSize

    )
{
  EFI_STATUS               Status      = EFI_SUCCESS;
  EFI_BLOCK_IO_PROTOCOL    *BlockIo    = NULL;
  UINTN                    BytesToMove = 0;
  UINTN                    BlockSize   = 0;
  
  if ((PartHandle == NULL) ||
      (Buffer == NULL) || 
      (BufferSize == NULL))
  { 
    return EFI_INVALID_PARAMETER;
  }
  *Buffer = NULL;
  Status = gBS->HandleProtocol(PartHandle,
                               &gEfiBlockIoProtocolGuid,
                              (VOID**)&BlockIo);
  
  HANDLE_ERROR_LABEL(Status, gBS->HandleProtocol, FwUpdatePartitionReadExit);

  BlockSize = BlockIo->Media->BlockSize;
  BytesToMove = BlockSize*((BlockIo->Media->LastBlock)+1);
  *BufferSize = BytesToMove;

  Status = AllocateAndZero(EfiBootServicesData,BytesToMove,Buffer); 
  HANDLE_ERROR_LABEL(Status, AllocateAndZero, FwUpdatePartitionReadExit);
  
  Status = BlockIo->ReadBlocks(BlockIo,
                               BlockIo->Media->MediaId,
                              (UINT64) FIRST_BLOCK,
                               BytesToMove,
                               *Buffer); 
  HANDLE_ERROR_LABEL(Status, BlockIo->ReadBlocks, FwUpdatePartitionReadExit);
  
  return EFI_SUCCESS;

FwUpdatePartitionReadExit:
  
  if(EFI_ERROR(Status))
  {
    if (*Buffer != NULL)
    {
      gBS->FreePool(*Buffer);     
      *Buffer = NULL;
    }  
  }
  
  return Status;
}

/**
 * Reads file from an FV
 *  into a buffer.
 * 
 * @param PartHandle - Handle to FileSystem
 * @param NameGuid   - File Guid
 * @param FileBuffer - Buffer
 * @param BufferSize - Buffer size
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS
FwUpdateFvRead(
  IN EFI_HANDLE               PartHandle,
  IN EFI_GUID                 *NameGuid,
  OUT VOID                    **FileBuffer,
  OUT UINTN                   *BufferSize
    )
{
  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv2                 = NULL;
  EFI_STATUS                    Status               = EFI_SUCCESS;
  EFI_FV_FILETYPE               FoundType;
  EFI_FV_FILE_ATTRIBUTES        FileAttributes;
  UINT32                        AuthenticationStatus; 

  if((PartHandle == NULL) || 
     (NameGuid == NULL) ||
     (FileBuffer == NULL) ||
     (BufferSize == NULL)
     )
  {
    return EFI_INVALID_PARAMETER;
  }
  *FileBuffer = NULL;

  Status = gBS->HandleProtocol(PartHandle,
                               &gEfiFirmwareVolume2ProtocolGuid,
                               (VOID**)&Fv2);
  HANDLE_ERROR_LABEL(Status,gBS->HandleProtocol,FwUpdateFvReadExit);

  Status = Fv2->ReadFile(Fv2,
                         NameGuid,
                         FileBuffer,
                         BufferSize,
                         &FoundType,
                         &FileAttributes,
                         &AuthenticationStatus);
  HANDLE_ERROR_LABEL(Status,Fv2->ReadFile,FwUpdateFvReadExit);
 
  return EFI_SUCCESS;

FwUpdateFvReadExit:

  if(EFI_ERROR(Status))
  {
    if(*FileBuffer != NULL)
    {
      gBS->FreePool(*FileBuffer);
      *FileBuffer = NULL;
    }
  }
  return Status;

}


/**
 * Function to get FV handle
 * 
 * @param FvHandle - FV handle

 * @return : EFI_STATUS
 */
EFI_STATUS
FwUpdateGetFvHandle(
    OUT EFI_HANDLE *FvHandle
    )
{
  EFI_STATUS       Status       = EFI_SUCCESS;
  VOID             *FileBuffer  = NULL;
  UINTN            VariableSize = sizeof(VOID*);
  UINTN            FileSize     = 0;
  
  if(*FvHandle == NULL)
  {
    Status = gRT->GetVariable(
                   L"PayloadBuffer",
                   &gQcomTokenSpaceGuid,
                   NULL,
                   &VariableSize,
                   &FileBuffer
                   );
    ERR_HANDLE(Status);
     
    Status = gRT->GetVariable(
                   L"PayloadBufferSize",
                   &gQcomTokenSpaceGuid,
                   NULL,
                   &VariableSize,
                   &FileSize
                   );
    ERR_HANDLE(Status);
     
    Status = gDS->ProcessFirmwareVolume(FileBuffer,FileSize,FvHandle);
       
    if(EFI_ERROR(Status))
    {
      if(Status != EFI_ALREADY_STARTED)
      {
        return Status;
      }
      else
      {
        Status = EFI_SUCCESS;
      }
    }
  }
	
  return Status;

};



