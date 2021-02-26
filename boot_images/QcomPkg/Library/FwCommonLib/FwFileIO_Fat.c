/** @file FatFile.c
   
  Functions FAT Partitions for CopyFwLib.

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights   
  Reserved. Qualcomm Technologies Proprietary and Confidential. 

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2013/02/26     mic         Fixed new KW warnings
 2012/12/11     mic         Fixed KW warnings
 2012/11/12     mic         Added FwUpdateValidateAndCorrectFileInfoTime 
                            function
 2011/10/12     jthompso    Initial version

=============================================================================*/

#include <Uefi.h>
#include <Guid/FileInfo.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Include/Library/FwCommonLib.h>
#include <Protocol/SimpleFileSystem.h>
#include "FwFileIO_Fat.h"


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
FwUpdateCreateDirectory(
    IN EFI_FILE_PROTOCOL    *FsRoot,
    IN UINT16               *FileName,
    IN UINT64               Attributes)
{
    EFI_STATUS          Status = EFI_SUCCESS;
    EFI_FILE_PROTOCOL   *File;
    UINT64              OpenMode;

    UINT16              **Paths = NULL; //Paths to be created
    UINT16              **NewPaths = NULL;
    UINT16              *CurrentPath;

    UINTN               CharCounter;
    UINTN               StringCounter;

    UINTN               StringCount = 0;
    
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
FwUpdateFatGetFileInfo(
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
    HANDLE_ERROR_LABEL (Status, gBS->AllocatePool, FwUpdateFatGetFileInfoExit);

    Status = File->GetInfo(File,
                           &gEfiFileInfoGuid,
                           FileInfoSize,
                           (VOID*)*FileInfo);
    HANDLE_ERROR_LABEL (Status, File->GetInfo, FwUpdateFatGetFileInfoExit);
  }
  else if( EFI_ERROR(Status))
  {
    HANDLE_ERROR_LABEL (Status, File->GetInfo, FwUpdateFatGetFileInfoExit);
  }
  else
  {
    /* This is to handle EFI_SUCCESS case, status shouldn't be EFI_SUCCESS if
       *FileInfoSize is 0.*/
    Status = EFI_UNSUPPORTED;
    HANDLE_ERROR_LABEL (Status, File->GetInfo, FwUpdateFatGetFileInfoExit);
  }
  
  return Status;

FwUpdateFatGetFileInfoExit:

  if(*FileInfo != NULL)
  {
    gBS->FreePool(*FileInfo);
    *FileInfo = NULL;
  }
  return Status;
}

/**
 * Performs either a read or 
 *  write on a FAT filesystem. 
 * 
 * 
 * @param PartHandle  - Handle for the FAT partition. 
 * @param FileBuffer  - Buffer for bytes read or bytes 
 *                      to write out.
 * @param FwFile      - Enum for the file being sought.
 * @param FileSize    - For read, the number of bytes read, 
 *                      For write, the number of bytes to write.
 * @param Read        - Whether this operation is read or 
 *                      write.
 * @param FileHandle  - Optional, return the handle of the file.
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateFatFileOperation(
    IN EFI_HANDLE               PartHandle,
    IN VOID                     **FileBuffer,
    IN QCOM_FW_UPDATE_FW_FILE   FwFile,
    IN OUT UINTN                *FileSize,
    IN BOOLEAN                  Read
    )
{
  EFI_STATUS                        Status        = EFI_SUCCESS;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL   *Fs           = NULL;
  EFI_FILE_PROTOCOL                 *FsRoot       = NULL;
  EFI_FILE_PROTOCOL                 *File         = NULL;
  volatile UINT64                   OpenMode;
  UINT64                            Attributes    = 0x0;
  UINT16                            *FileName     = NULL;
  EFI_GUID                          FileInfoGuid  = EFI_FILE_INFO_ID ;
  UINTN                             FileInfoSize  = 0;
  EFI_FILE_INFO                     *FileInfo     = NULL;

  EFI_STATUS (*AllocateFunction)() = NULL;
  EFI_STATUS (*FileOperation)() = NULL;

  /* locate file protocol */
  Status = gBS->HandleProtocol(PartHandle,
                              &gEfiSimpleFileSystemProtocolGuid,
                              (VOID **)&Fs);
  HANDLE_ERROR_LABEL (Status, gBS->HandleProtocol, CleanUp);

  /* Open volume*/
  Status = Fs->OpenVolume(Fs,&FsRoot);
  HANDLE_ERROR_LABEL (Status, Fs->OpenVolume, CleanUp);

  if (FsRoot == NULL)
  {
    Status = EFI_VOLUME_CORRUPTED;
    goto CleanUp;
  }
  
  /* set file open mode*/
  if(Read)
  {
    OpenMode = EFI_FILE_MODE_READ;
  }
  else
  {
    OpenMode = EFI_FILE_MODE_READ|EFI_FILE_MODE_WRITE|EFI_FILE_MODE_CREATE;
  }

  /* Get file name from enum */
  Status = FwUpdateGetFileName(FwFile,&FileName);
  HANDLE_ERROR_LABEL (Status, FwUpdateGetFileName, CleanUp);

  /*Open file*/
  Status = FsRoot->Open(FsRoot,
                        &File,
                        FileName,                          
                        OpenMode,
                        Attributes);
      
  if(EFI_ERROR(Status))
  {
    //Tried to write file to nonexistent directory,
    //  need to create the directory and try again.
    if(Status == EFI_NOT_FOUND && !Read)
    {
      Status = FwUpdateCreateDirectory(FsRoot,
                                       FileName,
                                       Attributes);
      HANDLE_ERROR_LABEL (Status, FwUpdateCreateDirectory, CleanUp);

      Status = FsRoot->Open(FsRoot,
                            &File,
                            FileName,                          
                            OpenMode,
                            Attributes);
      HANDLE_ERROR_LABEL (Status, FsRoot->Open, CleanUp);
    }
    else
    {
      HANDLE_ERROR_LABEL (Status, FsRoot->Open, CleanUp);
    }
  }

  /* Get File Info*/
  Status = FwUpdateFatGetFileInfo(File,&FileInfoSize,&FileInfo);
  HANDLE_ERROR_LABEL (Status, FwUpdateFatGetFileInfo, CleanUp);
  
  if (FileInfo == NULL)
  {
    Status = EFI_VOLUME_CORRUPTED;
    goto CleanUp;
  }

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
      
  if(Read)
  {
    AllocateFunction    = AllocateAndZero;
    FileOperation       = File->Read;  
    *FileSize = FileInfo->FileSize;
  }
  else //write
  {
    AllocateFunction    = DummyFunction;
    FileOperation       = File->Write;
  }    

  //Fill in Allocateor
  Status = AllocateFunction(EfiBootServicesData,*FileSize,FileBuffer);
  HANDLE_ERROR_LABEL (Status, AllocateFunction, CleanUp);


  //TODO: Fill in Read/Write

  Status = FileOperation(File,
                          FileSize,
                          *FileBuffer);
  HANDLE_ERROR_LABEL (Status, FileOperation, CleanUp);

  if(!Read)
  {
    if (FileInfo)
    {
      gBS->FreePool(FileInfo);
      FileInfo = NULL;
    }
    Status = FwUpdateFatGetFileInfo(File,&FileInfoSize,&FileInfo);
    HANDLE_ERROR_LABEL (Status, FwUpdateFatGetFileInfo, CleanUp);

    if( FileInfo != NULL)
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
      // set correct size
      FileInfo->FileSize = *FileSize;

      /*Validate and correct the FileInfo time is required before callig 
        setInfo function. Also, igore the return status if 
        FwUpdateValidateAndCorrectFileInfoTime returns error */
      FwUpdateValidateAndCorrectFileInfoTime(&FileInfo->CreateTime);
      FwUpdateValidateAndCorrectFileInfoTime(&FileInfo->ModificationTime);
      FwUpdateValidateAndCorrectFileInfoTime(&FileInfo->LastAccessTime);

      Status = File->SetInfo (File, &FileInfoGuid, FileInfo->Size, FileInfo);
    }
  }

CleanUp:

  if (FileInfo != NULL)
  {
    gBS->FreePool(FileInfo);
    FileInfo = NULL;
  }
  if (File != NULL)
  {
    File->Close(File);    
    File = NULL;
  }
  if (FsRoot != NULL)
  {
    FsRoot->Close(FsRoot);
    FsRoot = NULL;
  }

  return Status;

}

/**
 * Reads file from a FAT partition
 *  into a buffer.
 * 
 * @param PartHandle - Handle to FileSystem
 * @param FileBuffer - Buffer to store the file
 * @param FwFile     - File Identifier 
 * @param FileSize   - File size, number of bytes read. 
 *  
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateReadFileFromFat(
    IN EFI_HANDLE               PartHandle,
    IN VOID                     **FileBuffer,
    IN QCOM_FW_UPDATE_FW_FILE   FwFile,
    OUT UINTN                   *FileSize
    )
{
    return FwUpdateFatFileOperation(PartHandle,FileBuffer,FwFile,FileSize,TRUE);
}



/**
 * Writes file to a FAT partition
 *  from a buffer.
 * 
 * @param PartHandle - Handle to FileSystem
 * @param FileBuffer - Buffer to store the file
 * @param FwFile     - File Identifier 
 * @param FileSize   - File size, number of bytes to read. 
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateWriteFileToFat(
    IN EFI_HANDLE               PartHandle,
    IN VOID                     **FileBuffer,
    IN QCOM_FW_UPDATE_FW_FILE   FwFile,
    IN OUT UINTN                *FileSize

    )
{
    return FwUpdateFatFileOperation(PartHandle,FileBuffer,FwFile,FileSize,FALSE);
}


/** 
 *  
 * Gets the device path for the device which contains the 
 * requested file and adds a File Path entry for the requested 
 * file to the device path. 
 *  
 * This device path is passed to LoadImage(). 
 *  
 * This function should only be called for 
 * the Payload Application. 
 * 
 * @param FwFile  - Desired file
 * @param SrcPart - Desired File's partition
 * @param DevPath - Return the device path with the 
 *                  File Path entry added.
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateGetFilePathDevPath(
    IN  QCOM_FW_UPDATE_FW_FILE      FwFile,
    IN  QCOM_FW_UPDATE_FW_PARTITION SrcPart,
    OUT EFI_DEVICE_PATH_PROTOCOL    **DevPath
    )
{
  EFI_STATUS                          Status                = EFI_SUCCESS;
  EFI_HANDLE                          PartHandle            = NULL;
  EFI_DEVICE_PATH_UTILITIES_PROTOCOL* DevPathUtilities      = NULL;
  UINT16                              *FileName             = NULL;
  UINTN                               FileNameLength        = 0;
  FILEPATH_DEVICE_PATH                *FilePathDevNode      = NULL;
  UINTN                               FilePathDevNodeLength = 0;
  EFI_DEVICE_PATH_PROTOCOL            *FileSystemDevPath    = NULL;
  
  if(DevPath == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  Status = FwUpdateGetHandleForPartition(SrcPart,
                                         &PartHandle
                                         );

  HANDLE_ERROR_LABEL(Status,FwUpdateGetHandleForPartition,FwUpdateGetFilePathDevPathExit);
    
  Status = gBS->HandleProtocol(PartHandle,
                               &gEfiDevicePathProtocolGuid,
                               (VOID**)&FileSystemDevPath);
  HANDLE_ERROR_LABEL(Status,gBS->HandleProtocol,FwUpdateGetFilePathDevPathExit);
  
    
  Status = gBS->LocateProtocol(&gEfiDevicePathUtilitiesProtocolGuid,
                               NULL,
                               (VOID**)&DevPathUtilities);

  HANDLE_ERROR_LABEL(Status,gBS->LocateProtocol,FwUpdateGetFilePathDevPathExit);


  Status = FwUpdateGetFileName(FwFile,&FileName);
  HANDLE_ERROR_LABEL(Status,FwUpdateGetFileName,FwUpdateGetFilePathDevPathExit);
 

  FileNameLength = (StrLen(FileName)+1)<<1;
   
  FilePathDevNodeLength = sizeof(EFI_DEVICE_PATH) + FileNameLength;

  Status = gBS->AllocatePool(EfiBootServicesData,
                             FilePathDevNodeLength,
                            (VOID**)&FilePathDevNode);
  HANDLE_ERROR_LABEL(Status,gBS->AllocatePool,FwUpdateGetFilePathDevPathExit);
 
  if (FilePathDevNode != NULL)
  {
    FilePathDevNode->Header.Type = MEDIA_DEVICE_PATH;
    FilePathDevNode->Header.SubType = MEDIA_FILEPATH_DP;
    FilePathDevNode->Header.Length[0] = FilePathDevNodeLength &0x00FF;
    FilePathDevNode->Header.Length[1] = FilePathDevNodeLength &0xFF00;

    CopyMem(FilePathDevNode->PathName,FileName,FileNameLength);

    *DevPath = DevPathUtilities->AppendDeviceNode(FileSystemDevPath,
                                                   (EFI_DEVICE_PATH_PROTOCOL*)FilePathDevNode);
      
    if(*DevPath == NULL)
    {
      Status = EFI_OUT_OF_RESOURCES;
      DEBUG_ERR_LABEL("DevPathUtilities->AppendDeviceNode failed",FwUpdateGetFilePathDevPathExit);
    }

    return Status;
  }
  else
  {
    Status = EFI_OUT_OF_RESOURCES;
    HANDLE_ERROR_LABEL(Status,gBS->AllocatePool,FwUpdateGetFilePathDevPathExit);
  } 

FwUpdateGetFilePathDevPathExit:

  if( FilePathDevNode != NULL)
  {
    gBS->FreePool(FilePathDevNode);
    FilePathDevNode = NULL;
  }

  return Status;   
}


/**
 * Performs either a read or 
 *  write on a FAT filesystem. 
 * 
 * 
 * @param PartHandle - Handle for the FAT partition. 
 * @param FileName   - Unicode Null-terminated file name.
 * @param FileBuffer - Buffer for bytes read or bytes 
 *                     to write out.
 * @param FileSize   - For read, the number of bytes read, 
 *                     For write, the number of bytes to write.
 * @param Read       - Whether this operation is read or write. 
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateFatFileOperation2(
    IN EFI_HANDLE   PartHandle,
    IN CHAR16       *FileName,
    IN OUT VOID     **FileBuffer,
    IN OUT UINTN    *FileSize,
    IN BOOLEAN      Read
    )
{
  EFI_STATUS                        Status       = EFI_SUCCESS;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL   *Fs          = NULL;
  EFI_FILE_PROTOCOL                 *FsRoot      = NULL;
  EFI_FILE_PROTOCOL                 *File        = NULL;
  volatile UINT64                   OpenMode;
  UINT64                            Attributes   = 0x0;
  EFI_GUID                          FileInfoGuid = EFI_FILE_INFO_ID ;
  UINTN                             FileInfoSize = 0;
  EFI_FILE_INFO                     *FileInfo    = NULL;

  EFI_STATUS (*AllocateFunction)() = NULL;
  EFI_STATUS (*FileOperation)() = NULL;

  /* Locate protocoal*/
  Status = gBS->HandleProtocol(PartHandle,
                              &gEfiSimpleFileSystemProtocolGuid,
                              (VOID **)&Fs);
  HANDLE_ERROR_LABEL (Status, gBS->HandleProtocol, CleanUp);

  /* Open Volume*/
  Status = Fs->OpenVolume(Fs, &FsRoot);
  HANDLE_ERROR_LABEL (Status, Fs->OpenVolume, CleanUp);   
  if (FsRoot == NULL)
  {
    Status = EFI_VOLUME_CORRUPTED;
    goto CleanUp;
  }

  /* set file open mode*/
  if(Read)
  {
    OpenMode = EFI_FILE_MODE_READ;
  }
  else
  {
    OpenMode = EFI_FILE_MODE_READ|EFI_FILE_MODE_WRITE|EFI_FILE_MODE_CREATE;
  }

  /* Open file*/
  Status = FsRoot->Open(FsRoot,
                        &File,
                        FileName,                          
                        OpenMode,
                        Attributes);
      
  if(EFI_ERROR(Status))
  {
    //Tried to write file to nonexistent directory,
    //  need to create the directory and try again.
    if(Status == EFI_NOT_FOUND && !Read)
    {
      Status = FwUpdateCreateDirectory(FsRoot,
                                        FileName,
                                        Attributes);
      HANDLE_ERROR_LABEL (Status, FwUpdateCreateDirectory, CleanUp);   

      Status = FsRoot->Open(FsRoot,
                            &File,
                            FileName,                          
                            OpenMode,
                            Attributes);
      HANDLE_ERROR_LABEL (Status, FsRoot->Open, CleanUp);   

    }
    else
    {
      HANDLE_ERROR_LABEL (Status, FsRoot->Open, CleanUp);   
    }
  }

  Status = FwUpdateFatGetFileInfo(File,&FileInfoSize,&FileInfo);
  HANDLE_ERROR_LABEL (Status, FwUpdateFatGetFileInfo, CleanUp);   

  if (FileInfo == NULL)
  {
    Status = EFI_VOLUME_CORRUPTED;
    goto CleanUp;
  }
 
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

  if(Read)
  {
    AllocateFunction    = AllocateAndZero;
    FileOperation       = File->Read;
    *FileSize           = FileInfo->FileSize;
  }
  else //write
  {
    AllocateFunction    = DummyFunction;
    FileOperation       = File->Write;
  }    

  //Fill in Allocateor
  Status = AllocateFunction(EfiBootServicesData, *FileSize, FileBuffer);
  HANDLE_ERROR_LABEL (Status, AllocateFunction, CleanUp);

  //TODO: Fill in Read/Write

  Status = FileOperation(File,
                         FileSize,
                        *FileBuffer);
  HANDLE_ERROR_LABEL (Status, FileOperation, CleanUp);
      
  if(!Read)
  {
    if (FileInfo != NULL)
    {
      gBS->FreePool(FileInfo);
      FileInfo = NULL;
    }
    Status = FwUpdateFatGetFileInfo(File,&FileInfoSize,&FileInfo);
    HANDLE_ERROR_LABEL (Status, FwUpdateFatGetFileInfo, CleanUp);

    if ( FileInfo != NULL)
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
      /* set the file size*/
      FileInfo->FileSize = *FileSize;

      /*Validate and correct the FileInfo time is required before callig 
        setInfo function. Also, igore the return status if 
        FwUpdateValidateAndCorrectFileInfoTime returns error */
      FwUpdateValidateAndCorrectFileInfoTime(&FileInfo->CreateTime);
      FwUpdateValidateAndCorrectFileInfoTime(&FileInfo->ModificationTime);
      FwUpdateValidateAndCorrectFileInfoTime(&FileInfo->LastAccessTime);

      Status = File->SetInfo (File, &FileInfoGuid, FileInfo->Size, FileInfo);
    }
  }

CleanUp:

  if (FileInfo != NULL)
  {
    gBS->FreePool(FileInfo);
  }
  if (File != NULL)
  {
    File->Close(File);    
    File = NULL;
  }
  if (FsRoot != NULL)
  {
    FsRoot->Close(FsRoot);
    FsRoot = NULL;
  }
  return Status;
}

/**
 * Reads file from a FAT partition
 *  into a buffer.
 * 
 * @param PartHandle - Handle to FileSystem 
 * @param FileName   - Unicode Null-terminated file name. 
 * @param FileBuffer - Buffer to store the file
 * @param FileSize   - File size, number of bytes read. 
 *  
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateReadFileFromFat2(
    IN EFI_HANDLE               PartHandle,
    IN CHAR16                   *FileName,
    IN OUT VOID                 **FileBuffer,
    OUT UINTN                   *FileSize
    )
{
    return FwUpdateFatFileOperation2(PartHandle, FileName, FileBuffer, FileSize, TRUE);
}



/**
 * Writes file to a FAT partition
 *  from a buffer.
 * 
 * @param PartHandle - Handle to FileSystem 
 * @param FileName   - Unicode Null-terminated file name. 
 * @param FileBuffer - Buffer to store the file
 * @param FileSize   - File size, number of bytes to write. 
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateWriteFileToFat2(
    IN EFI_HANDLE   PartHandle,
    IN CHAR16       *FileName,
    IN VOID         **FileBuffer,
    IN UINTN        *FileSize
    )
{
    return FwUpdateFatFileOperation2(PartHandle, FileName, FileBuffer, FileSize, FALSE);
}


/**
 * Create a File in FAT
 *  from a buffer.
 * 
 * @param PartHandle - Handle to FileSystem
 * @param FileBuffer - Buffer to store the file
 * @param FwFile     - File Identifier 
 * @param FileSize   - File size, number of bytes to read. 
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
CheckandCreateFilesinFat(
        VOID
      )
{
    EFI_STATUS Status ;
    EFI_HANDLE PartHandle ;
    VOID *Buffer =NULL;
    UINTN FileSize;
    UINT8 UpdateStateDefault  = (UINT8)FW_STATE_NOT_IN_PROGRESS ;
    UINT8 ProvisionFlagDefault = (UINT8)QCOM_FW_UPDATE_FLAG_VALUE_FACTORY_PROVISIONED_NOT_SET ;
    
    Status = FwUpdateGetHandleForPartition( QCOM_FW_UPDATE_PARTITION_GPP_FAT16, &PartHandle);
  
   
    ERR_HANDLE(Status);
    Status = FwUpdateReadFileFromFat(PartHandle,&Buffer,QCOM_FW_UPDATE_FILE_UPDATE_STATE_FILE,&FileSize);
    if(Buffer!=NULL)
    {
      gBS->FreePool(Buffer);
      Buffer=NULL;
    }
    if(Status == EFI_NOT_FOUND)
    {
        FileSize = 1;
        Status = gBS->AllocatePool (EfiBootServicesData , FileSize ,&Buffer );
        gBS->SetMem(Buffer, FileSize, 0);
        CopyMem(Buffer, &UpdateStateDefault, FileSize);
        ERR_HANDLE(Status);
        Status = FwUpdateWriteFileToFat(PartHandle, &Buffer, QCOM_FW_UPDATE_FILE_UPDATE_STATE_FILE, &FileSize );
        ERR_HANDLE(Status);
        gBS->FreePool (Buffer) ;
        Buffer=NULL;
    }
    ERR_HANDLE(Status);
    
    Status = FwUpdateReadFileFromFat(PartHandle,&Buffer,QCOM_FW_UPDATE_FILE_BP_GPT_FILE,&FileSize);
    if(Buffer!=NULL)
    {
      gBS->FreePool(Buffer);
      Buffer=NULL;
    }
    if(Status == EFI_NOT_FOUND)
    {
        FileSize = 1;
        Status = gBS->AllocatePool (EfiBootServicesData , FileSize ,&Buffer );
        gBS->SetMem(Buffer, FileSize, 0); 
        ERR_HANDLE(Status);
        Status = FwUpdateWriteFileToFat(PartHandle, &Buffer, QCOM_FW_UPDATE_FILE_BP_GPT_FILE, &FileSize );
        ERR_HANDLE(Status);
        gBS->FreePool (Buffer) ;
        Buffer=NULL;
    }
    ERR_HANDLE(Status);
    
    Status = FwUpdateReadFileFromFat(PartHandle,&Buffer,QCOM_FW_UPDATE_FILE_FACTORY_PROVISIONED_FLAG_FILE,&FileSize);
    if(Buffer!=NULL)
    {
      gBS->FreePool(Buffer);
      Buffer=NULL;
    }
    if(Status == EFI_NOT_FOUND)
    {
        FileSize = 1;
        Status = gBS->AllocatePool (EfiBootServicesData , FileSize ,&Buffer );
        gBS->SetMem(Buffer, FileSize, 0); 
        CopyMem(Buffer , &ProvisionFlagDefault, FileSize );
        ERR_HANDLE(Status);
        Status = FwUpdateWriteFileToFat(PartHandle, &Buffer, QCOM_FW_UPDATE_FILE_FACTORY_PROVISIONED_FLAG_FILE, &FileSize );
        ERR_HANDLE(Status);
        gBS->FreePool (Buffer) ;
        Buffer=NULL;
    }
    ERR_HANDLE(Status);
   
    Status = FwUpdateReadFileFromFat(PartHandle,&Buffer,QCOM_FW_UPDATE_FILE_GPP_GPT_FILE,&FileSize);
    if(Buffer!=NULL)
    {
      gBS->FreePool(Buffer);
      Buffer=NULL;
    }
    if(Status == EFI_NOT_FOUND)
    {
        FileSize = 1;
        Status = gBS->AllocatePool (EfiBootServicesData , FileSize ,&Buffer );
        gBS->SetMem(Buffer, FileSize, 0); 
        ERR_HANDLE(Status);
        Status = FwUpdateWriteFileToFat(PartHandle, &Buffer, QCOM_FW_UPDATE_FILE_GPP_GPT_FILE, &FileSize );
        ERR_HANDLE(Status);
        gBS->FreePool (Buffer) ;
        Buffer=NULL;
    }
    
    return Status;

}


/**
 * Given a FAT directory path, retrieve the names of all files  
 * that immediately reside under specified path. This is not a 
 * recursive operation so subfolders are not pinged.
 * 
 * @param PartHandle - Handle to FAT partition
 * @param FwFile     - Enumeration of directory path. 
 * @param Path       - Upon return, store the unicode string of 
 *                     directory path 
 * @param DirName    - Upon return, store the bottom-level 
 *                     subfolder name of specified directory
 *                     path, e.g. if path is "\DPP\QCOM\", then
 *                     return "QCOM".
 * @param FileList   - Upon return, a pointer to a list of 
 *                     unicode Null-terminated file names.
 * @param FileCount  - Upon return, the number of files. 
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateGetFileList(
    IN  EFI_HANDLE  PartHandle,
    IN  CHAR16      *Path,
    OUT CHAR16      **Subfolder,
    OUT CHAR16      ***FileList,
    OUT UINTN       *FileCount
    )
{
    EFI_STATUS                      Status;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *Fs;
    EFI_FILE_PROTOCOL               *FsRoot;
    EFI_FILE_PROTOCOL               *File;
    UINTN                           Size;
    UINT16                          *FileName;
    //EFI_FILE_SYSTEM_INFO          *FsInfo;
    EFI_FILE_INFO                   *FsFileInfo = NULL;
    EFI_FILE_INFO                   *DirInfo = NULL;
    UINTN                           ReadSize;
    CHAR16                          **FList;
    UINTN                           FCount = 0;

    *FileCount = 0;

    Status = gBS->HandleProtocol(PartHandle,
                                 &gEfiSimpleFileSystemProtocolGuid,
                                 (VOID **)&Fs);

    ERR_HANDLE(Status);

    Status = Fs->OpenVolume(Fs, &FsRoot);

    ERR_HANDLE(Status);

    FileName = Path;

    //AsciiPrint("Pathname = %s\n", FileName);

    /*
    // Get information about the volume
    Size = 0;
    Status = FsRoot->GetInfo(FsRoot, &gEfiFileSystemInfoGuid, &Size, FsInfo);
    if (Status == EFI_BUFFER_TOO_SMALL) 
    {
        FsInfo = AllocatePool(Size);
        Status = FsRoot->GetInfo(FsRoot, &gEfiFileSystemInfoGuid, &Size, FsInfo);
    }

    AsciiPrint("FsInfo==>\n");
    AsciiPrint("FsInfo->Size = 0x%x\n", FsInfo->Size);
    AsciiPrint("FsInfo->ReadOnly = 0x%x\n", FsInfo->ReadOnly);
    AsciiPrint("FsInfo->VolumeSize = 0x%x\n", FsInfo->VolumeSize);
    AsciiPrint("FsInfo->FreeSpace = 0x%x\n", FsInfo->FreeSpace);
    AsciiPrint("FsInfo->BlockSize = 0x%x\n", FsInfo->BlockSize);
    AsciiPrint("FsInfo->VolumeLabel = %s\n", &FsInfo->VolumeLabel[0]); 
    */ 

    // Get information about the file
    Status = FsRoot->Open(FsRoot, &File, FileName, EFI_FILE_MODE_READ, 0);

    ERR_HANDLE(Status);

    Size = 0;
    Status = File->GetInfo(File, &gEfiFileInfoGuid, &Size, NULL);
    if (Status == EFI_BUFFER_TOO_SMALL) 
    {
        FsFileInfo = AllocatePool(Size);
        Status = File->GetInfo(File, &gEfiFileInfoGuid, &Size, FsFileInfo);
    }

    if (FsFileInfo ==  NULL)
    {
        return EFI_SUCCESS;
    }

    /*
    AsciiPrint("FsFileInfo==>\n");
    AsciiPrint("FsFileInfo->Size = 0x%x\n", FsFileInfo->Size);
    AsciiPrint("FsFileInfo->FileSize = 0x%x\n", FsFileInfo->FileSize);
    AsciiPrint("FsFileInfo->PhysicalSize = 0x%x\n", FsFileInfo->PhysicalSize);
    AsciiPrint("FsFileInfo->Attribute = 0x%x\n", FsFileInfo->Attribute);
    AsciiPrint("FsFileInfo->FileName = %s\n", &FsFileInfo->FileName[0]); 
    */ 

    if (!(FsFileInfo->Attribute & EFI_FILE_DIRECTORY)) 
    {
        return EFI_SUCCESS;
    }
    
    *Subfolder = AllocatePool(StrSize(&FsFileInfo->FileName[0])); 

    if (*Subfolder == NULL) 
    {
        goto FwUpdateGetFileListExit;
    }

    CopyMem(*Subfolder, &FsFileInfo->FileName[0], StrSize(&FsFileInfo->FileName[0]));

    // First get files count
    FCount = 0;
    File->SetPosition(File, 0);
    for (;;) 
    {
        // First read gets the size
        DirInfo = NULL;
        ReadSize = 0;
        Status = File->Read(File, &ReadSize, DirInfo);
        if (Status == EFI_BUFFER_TOO_SMALL) 
        {
            // Allocate the buffer for the real read
            DirInfo = AllocatePool (ReadSize);
            if (DirInfo == NULL) 
            {
                goto FwUpdateGetFileListExit;
            }
        
            // Read the data
            Status = File->Read (File, &ReadSize, DirInfo);
            if ((EFI_ERROR (Status)) || (ReadSize == 0)) 
            {
                break;
            }
        } 
        else 
        {
            break;
        }

        if (!(DirInfo->Attribute & EFI_FILE_DIRECTORY))
        {
            FCount++;
            //AsciiPrint ("%,14ld %s\n", DirInfo->FileSize, &DirInfo->FileName[0]);
        }
      
        FreePool (DirInfo);
        DirInfo = NULL;
    }

    *FileCount = FCount;
    //AsciiPrint("FileCount = %d\n", *FileCount);

    // Allocate buffer
    *FileList = AllocatePool (*FileCount * sizeof(CHAR16*));

    if (*FileList == NULL) 
    {
        goto FwUpdateGetFileListExit;
    }

    FList = *FileList;

    //AsciiPrint("FatFile: *FileList = 0x%p\n", *FileList);

    // Fill in the buffer
    FCount = 0;
    File->SetPosition(File, 0);
    for (;;) 
    {
        // First read gets the size
        DirInfo = NULL;
        ReadSize = 0;
        Status = File->Read(File, &ReadSize, DirInfo);
        if (Status == EFI_BUFFER_TOO_SMALL) 
        {
            // Allocate the buffer for the real read
            DirInfo = AllocatePool (ReadSize);
            if (DirInfo == NULL) 
            {
                goto FwUpdateGetFileListExit;
            }
        
            // Read the data
            Status = File->Read (File, &ReadSize, DirInfo);
            if ((EFI_ERROR (Status)) || (ReadSize == 0)) 
            {
                break;
            }
        } 
        else 
        {
            break;
        }

        if (!(DirInfo->Attribute & EFI_FILE_DIRECTORY))
        {
            FList[FCount] = AllocatePool(StrSize(&DirInfo->FileName[0]));
            CopyMem(FList[FCount], &DirInfo->FileName[0], StrSize(&DirInfo->FileName[0]));
            FCount++;
            //AsciiPrint ("%,14ld %s\n", DirInfo->FileSize, FList[*FileCount]);
        }
      
        FreePool (DirInfo);
        DirInfo = NULL;
    }

FwUpdateGetFileListExit:

    //Cleanup
    if (DirInfo != NULL) 
    {
        FreePool (DirInfo);
    }

    if (FsFileInfo != NULL) 
    {
        FreePool (FsFileInfo);
    }

    File->Close(File);    
    FsRoot->Close(FsRoot);

    return Status;
}
