#include "CommonUtil.h"
#include "FatUtil.h"
/** @file FatUtil.c
   
  Qualcomm Dpp tool application

  Copyright (c) 2010-2013, Qualcomm Technologies, Inc. All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.
  
**/
/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/05/13   vk      New line at EOF for warning cleanup
 07/16/13   mic     Initial version
=============================================================================*/

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

EFI_STATUS
CreateFatDirectory(
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

EFI_STATUS
FatWrite(
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
  HANDLE_ERROR_LABEL(Status, gBS->HandleProtocol, FatWriteExit);

  Status = Fs->OpenVolume(Fs,&FsRoot);
  HANDLE_ERROR_LABEL(Status, Fs->OpenVolume, FatWriteExit);

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

        Status = CreateFatDirectory(FsRoot,
                                         FileName,
                                         Attributes);
        HANDLE_ERROR_LABEL(Status, CreateFatDirectory, FatWriteExit);

        Status = FsRoot->Open(FsRoot,
                              &File,
                              FileName,                          
                              OpenMode,
                              Attributes);
        HANDLE_ERROR_LABEL(Status, FsRoot->Open, FatWriteExit);
      }
      else
      {
        HANDLE_ERROR_LABEL(Status, FsRoot->Open, FatWriteExit);
      }
    }
  }
  else
  {
    Status = EFI_NOT_FOUND;
    HANDLE_ERROR_LABEL(Status, Fs->OpenVolume, FatWriteExit);
  }

  if (File != NULL)
  {
    Status = GetFileInfo(File,&FileInfoSize,&FileInfo);
         
    if((!EFI_ERROR(Status)) && (FileInfo != NULL))
    {
      //Clear out the read only attribute
      if((FileInfo->Attribute) & EFI_FILE_READ_ONLY)
      {
        FileInfo->Attribute = (FileInfo->Attribute) & (0xFFFFFFFFFFFFFFFE) ;

        /*Validate and correct the FileInfo time is required before callig 
          setInfo function. Also, igore the return status if 
          ValidateAndCorrectFileInfoTime returns error */
        ValidateAndCorrectFileInfoTime(&FileInfo->CreateTime);
        ValidateAndCorrectFileInfoTime(&FileInfo->ModificationTime);
        ValidateAndCorrectFileInfoTime(&FileInfo->LastAccessTime);

        Status = File->SetInfo (File, &FileInfoGuid, FileInfo->Size, FileInfo);
      }
    }

    Status = File->Write(File,
                         &BufferSize,
                         Buffer);
    HANDLE_ERROR_LABEL(Status, File->Write, FatWriteExit);

    if(FileInfo != NULL)
    {
      gBS->FreePool(FileInfo);
      FileInfo = NULL;
    }
   
    Status = GetFileInfo(File,&FileInfoSize,&FileInfo);
    HANDLE_ERROR_LABEL(Status, GetFileInfo, FatWriteExit);

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
        ValidateAndCorrectFileInfoTime returns error */
      ValidateAndCorrectFileInfoTime(&FileInfo->CreateTime);
      ValidateAndCorrectFileInfoTime(&FileInfo->ModificationTime);
      ValidateAndCorrectFileInfoTime(&FileInfo->LastAccessTime);
        
      Status = File->SetInfo (File, &FileInfoGuid, FileInfo->Size, FileInfo);
      HANDLE_ERROR_LABEL(Status, File->SetInfo, FatWriteExit);

      Status = File->Flush(File);
      HANDLE_ERROR_LABEL(Status, File->Flush, FatWriteExit);
    }
  }
  else
  {
    Status = EFI_NOT_FOUND;
    HANDLE_ERROR_LABEL(Status, FsRoot->Open, FatWriteExit);

  }

FatWriteExit:

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
FatRead(
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
  HANDLE_ERROR_LABEL(Status, gBS->HandleProtocol, FatReadExit);

  Status = Fs->OpenVolume(Fs,&FsRoot);
  HANDLE_ERROR_LABEL(Status, Fs->OpenVolume, FatReadExit); 

  if (FsRoot != NULL)
  {
    Status = FsRoot->Open(FsRoot,
                          &File,
                          FileName,                          
                          EFI_FILE_MODE_READ,
                          Attributes);
    HANDLE_ERROR_LABEL(Status, FsRoot->Open, FatReadExit); 
  }
  else
  {
    Status = EFI_NOT_FOUND;
    HANDLE_ERROR_LABEL(Status, FsRoot->Open, FatReadExit); 
  }

  Status = GetFileInfo(File,&FileInfoSize,&FileInfo);
  HANDLE_ERROR_LABEL(Status, FwUpdateFatGetFileInfo, FatReadExit);

  if ( (FileInfo != NULL) && (File != NULL))
  {
    *BufferSize = FileInfo->FileSize;
    
    Status = AllocateAndZero(EfiBootServicesData,*BufferSize,Buffer);
    HANDLE_ERROR_LABEL(Status, AllocateAndZero, FatReadExit);

    Status = File->Read(File,
                        BufferSize,
                        *Buffer);
    HANDLE_ERROR_LABEL(Status, File->Read, FatReadExit);
  }
  else
  {
    Status = EFI_NOT_FOUND;
    HANDLE_ERROR_LABEL(Status, GetFileInfo, FatReadExit);
  }
FatReadExit:

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

