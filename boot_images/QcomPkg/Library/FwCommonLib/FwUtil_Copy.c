/** @file FwUtil_Copy.c
   
  Copy functions for Firmware updates.

  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential. 

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2015/04/16     mic	        Use updated DppProtocol 
 2012/11/12     mic         Added FwUpdateValidateAndCorrectFileInfoTime 
                            function
 2012/07/03     mic         Added WriteBuffertoDPPAndCheck/WriteBuffertoFatAndCheck function 
                            for OPMKey support
 2012/06/15     mic         Added safe file deletion
 2012/05/07     rs          Added support for decryption of Boot Firmware
 2011/10/12     jthompso    Initial version

=============================================================================*/
#include <Uefi.h>
#include <Guid/FileInfo.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/FwCommonLib.h>
#include <Protocol/SimpleFileSystem.h>
#include "FwUtil_Copy.h"
#include <Protocol/EFIDpp.h>

/**
 * Helper to compare fw files, ensure two files are inditical
 *  
 * 
 * @param FwFile - Enum for the desired file
 * @param SrcPart - Enum for the source partition
 * @param DestPart - Enum for the destination partition 
 * @param OutputFileName - Enum for the output name of the 
 *                       file
 * 
 * @return : files are identical. else not equal
 */
BOOLEAN
CompareFwFile(
    IN QCOM_FW_UPDATE_FW_FILE FwFileSrc,
    IN QCOM_FW_UPDATE_FW_PARTITION SrcPart,
    IN QCOM_FW_UPDATE_FW_PARTITION DestPart,
    IN QCOM_FW_UPDATE_FW_FILE FwFileDest
    )
{
	VOID								*FileBufferSrc, *FileBufferDes;
	EFI_STATUS						   Status;
	EFI_HANDLE						   PartHandle;	  
	UINTN							   FileSizeSrc,FileSizeDes;
	BOOLEAN							   ret;	
	
	//Placeholders for Partition-specific
	// read/write functions.
	EFI_STATUS (*ReadFileSrc) () = NULL;
	EFI_STATUS (*ReadFileDes)() = NULL;
	
	
	//Read file Src file into buffer
	Status = FwUpdateGetHandleForPartition(SrcPart,&PartHandle);
		   
	if(EFI_ERROR(Status))
	{
		return FALSE;
	}
	
	Status = FwUpdateGetReadFunction(SrcPart,(VOID**)&ReadFileSrc);
	
	if(EFI_ERROR(Status))
	{
		return FALSE;
	}
	
	Status = (ReadFileSrc)(PartHandle,&FileBufferSrc,FwFileSrc,&FileSizeSrc);
	
	if(EFI_ERROR(Status))
	{
		return FALSE;
	}
	   
	   
	//Read dest file into buffer.
	
	Status = FwUpdateGetHandleForPartition(DestPart,&PartHandle);
	
	if(EFI_ERROR(Status))
	{
		return FALSE;
	}
	
	Status = FwUpdateGetReadFunction(DestPart,(VOID**)&ReadFileDes);
	
	if(EFI_ERROR(Status))
	{
		return FALSE;
	}

	Status = (ReadFileDes)(PartHandle,&FileBufferDes,FwFileSrc,&FileSizeDes);

	if(EFI_ERROR(Status))
	{
		return FALSE;
	}
		   
	ret = CompareMem (FileBufferDes,FileBufferSrc,FileSizeSrc)==0 ? TRUE:FALSE; 
		
	gBS->FreePool(FileBufferSrc);
	gBS->FreePool(FileBufferDes);
	return ret;  

}


/**
 * Helper to CopyFwFile, ensures input 
 * combination is valid. 
 *  
 * This function blocks invalid copy 
 *  scenarios (i.e., FAT to FV, a
 *  case we do not use) from happening.
 * 
 * 
 * @param FwFile - Enum for the desired file
 * @param SrcPart - Enum for the source partition
 * @param DestPart - Enum for the destination partition 
 * @param OutputFileName - Enum for the output name of the 
 *                       file
 * 
 * @return BOOLEAN 
 */
BOOLEAN
CopyFwFileValidInputs(
    IN QCOM_FW_UPDATE_FW_FILE FwFile,
    IN QCOM_FW_UPDATE_FW_PARTITION SrcPart,
    IN QCOM_FW_UPDATE_FW_PARTITION DestPart,
    IN QCOM_FW_UPDATE_FW_FILE FwFileDest
    )
{
    return TRUE;
}



/**
 * This function copies a firmware file (DPP, EFS, SBL, etc), 
 * from the given source to the given destination. 
 * 
 * 
 * @param FwFile - Enum for the desired file
 * @param SrcPart - Enum for the source partition
 * @param DestPart - Enum for the destination partition 
 * @param OutputFileName - Enum for the output name of the 
 *                       file
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS
CopyFwFile(
    IN QCOM_FW_UPDATE_FW_FILE FwFile,
    IN QCOM_FW_UPDATE_FW_PARTITION SrcPart,
    IN QCOM_FW_UPDATE_FW_PARTITION DestPart,
    IN QCOM_FW_UPDATE_FW_FILE FwFileDest
    )
{

    VOID                                *FileBuffer;
    EFI_STATUS                          Status;
    EFI_HANDLE                          PartHandle;    
    UINTN                               FileSize;

    //Placeholders for Partition-specific
    // read/write functions.
    EFI_STATUS (*ReadFile) () = NULL;
    EFI_STATUS (*WriteFile)() = NULL;


    if(!CopyFwFileValidInputs(FwFile,
                             SrcPart,
                             DestPart,
                             FwFileDest)
      )
    {
        return EFI_INVALID_PARAMETER;
    }


    //Read file Src file into buffer
    Status = FwUpdateGetHandleForPartition(SrcPart,&PartHandle);
        
    if(EFI_ERROR(Status))
    {
        return Status;
    }

    Status = FwUpdateGetReadFunction(SrcPart,(VOID**)&ReadFile);

    if(EFI_ERROR(Status))
    {
        return Status;
    }
 
    Status = (ReadFile)(PartHandle,&FileBuffer,FwFile,&FileSize);

    if(EFI_ERROR(Status))
    {
        return Status;
    }
    
    
    //Write file back to Dest.

    Status = FwUpdateGetHandleForPartition(DestPart,&PartHandle);

    if(EFI_ERROR(Status))
    {
        return Status;
    }

    Status = FwUpdateGetWriteFunction(DestPart,(VOID**)&WriteFile);

    if(EFI_ERROR(Status))
    {
        return Status;
    }

    Status = (WriteFile)(PartHandle, &FileBuffer,FwFileDest,&FileSize);
    
    gBS->FreePool(FileBuffer);

    return Status;   


}
/**
 * Helper to write a buffer in DPP and  ensure the write is successful
 *  
 * @param Srcbuffer - the source buffer
 * @param bufSzie - buffer size
 * @param FwFileDest - the dest file name
*  @param[in]  Type  - An unicode string specifying provision
								      data type, e.g. L"QCOM" or L"OEM".
 * @param trycnt - times to try.
 
 * @return : EFI_STATUS
 */

EFI_STATUS
WriteBuffertoDPPAndCheck(
    IN VOID*  Srcbuffer,
    IN INTN   bufSize,
    IN UINT16 *FwFileDest,
    IN CHAR16 *Type,    
    IN INTN   trycnt
    )
{
  EFI_DPP_PROTOCOL *DppProtocol = NULL;
  UINTN             pBufSize = 0; 
  VOID*             bufTemp = NULL;
  EFI_STATUS        Status = EFI_SUCCESS;
  UINTN             i = 0;  
  
  /*Write EncryptedData to DPP*/
  Status = gBS->LocateProtocol(&gEfiDppProtocolGuid,
                               NULL,
                               (VOID**)&DppProtocol);
  if(EFI_ERROR(Status))
  {
    return Status;
  }

  Status = gBS->AllocatePool(EfiBootServicesData,bufSize,(VOID **)&bufTemp);
  if(EFI_ERROR(Status))
  {
    return Status;
  }

  for( i=0;i< trycnt; i++ )
  {
    pBufSize = bufSize;
    Status = DppProtocol->SetDPP(DppProtocol,L"QCOM",FwFileDest,Srcbuffer,pBufSize);  
    if (!EFI_ERROR(Status))
    {
      pBufSize = bufSize;
      Status = DppProtocol->GetDPP(DppProtocol,L"QCOM",FwFileDest,bufTemp,&pBufSize);  
      if (!EFI_ERROR(Status))
      {
        if( CompareMem (bufTemp,Srcbuffer,bufSize)==0)
        {
          break;
        }
        else
        {
          Status = EFI_VOLUME_CORRUPTED; 
        }
      }
    }	
  }
  if (bufTemp != NULL)
  {
    gBS->FreePool(bufTemp);
    bufTemp = NULL;
  }
  return Status;   
}

/**
 * Helper to back up a buffer in FAT system and  ensure the back up is successful
 *  
 * @param Srcbuffer - the source buffer
 * @param bufSize - buffer size
 * @param FwFileDest - the dest file name
 * @param trycnt - times to try.
 
 * @return : EFI_STATUS
 */

EFI_STATUS
WriteBuffertoFatAndCheck(
    IN VOID* Srcbuffer,
    IN INTN bufSize,
    IN QCOM_FW_UPDATE_FW_FILE FwFileDest,
    IN INTN trycnt
    )
{

  EFI_STATUS                          Status =EFI_SUCCESS;
  UINTN                               i = 0;  
  EFI_HANDLE                          PartHandle = NULL;
  VOID                                *TempBuf = NULL;
  INTN                                pBufSize = 0;
  EFI_STATUS (*ReadFile) () = NULL;
  EFI_STATUS (*WriteFile)() = NULL;

  Status = FwUpdateGetHandleForPartition( QCOM_FW_UPDATE_PARTITION_GPP_FAT16, &PartHandle);        
  if(EFI_ERROR(Status))
  {
    return Status;
  }
  Status = FwUpdateGetWriteFunction(QCOM_FW_UPDATE_PARTITION_GPP_FAT16,(VOID**)&WriteFile);
  if(EFI_ERROR(Status))
  {
    return Status;
  }
  Status = FwUpdateGetReadFunction(QCOM_FW_UPDATE_PARTITION_GPP_FAT16,(VOID**)&ReadFile);
  if(EFI_ERROR(Status))
  {
    return Status;
  }
             
  for( i=0;i< trycnt; i++ )
  {
    pBufSize = bufSize;
 	  Status = (WriteFile)(PartHandle, &Srcbuffer,FwFileDest,&pBufSize);
    if (!EFI_ERROR(Status))
    {
      pBufSize = bufSize;
      Status = (ReadFile)(PartHandle,&TempBuf,FwFileDest,&pBufSize);
      if (!EFI_ERROR(Status))
      {
        if( CompareMem (TempBuf,Srcbuffer,bufSize)==0)
        {
          break;
        }
        else
        {
          Status = EFI_VOLUME_CORRUPTED; 
        }
      }
    }	
  }
  if (TempBuf != NULL)
  {
    gBS->FreePool(TempBuf);
    TempBuf = NULL;
  }
  return Status;   
}


/**
 * Helper to compare fw files and ensure two files are inditical
 *  
 * 
 * @param FwFile - Enum for the desired file
 * @param SrcPart - Enum for the source partition
 * @param DestPart - Enum for the destination partition 
 * @param OutputFileName - Enum for the output name of the 
 *                       file
 * @param trycnt - times to try.
 
 * @return : EFI_STATUS
 */

EFI_STATUS
CopyFwFileAndCheck(
    IN QCOM_FW_UPDATE_FW_FILE FwFile,
    IN QCOM_FW_UPDATE_FW_PARTITION SrcPart,
    IN QCOM_FW_UPDATE_FW_PARTITION DestPart,
    IN QCOM_FW_UPDATE_FW_FILE FwFileDest,
    IN INTN trycnt
    )
{

    EFI_STATUS                          Status =EFI_SUCCESS;
    UINTN                               i;

	for( i=0;i< trycnt; i++ ){

		Status =CopyFwFile(FwFile,SrcPart,DestPart,FwFileDest);
		if (!EFI_ERROR(Status))
		{
			if(CompareFwFile(FwFile,SrcPart,DestPart,FwFileDest))
			{
				break;
			}
			else
			{
				Status = EFI_VOLUME_CORRUPTED;
			}
		}
	}
    return Status;   

}

EFI_STATUS
DeleteFwFile(
    IN QCOM_FW_UPDATE_FW_FILE FwFile,
    IN QCOM_FW_UPDATE_FW_PARTITION Partition
    )
{
    EFI_STATUS Status;
    EFI_HANDLE PartHandle ;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *Fs;
    EFI_FILE_PROTOCOL        *FsRoot;
    EFI_FILE_PROTOCOL        *File;
    UINT64                    Attributes = 0x0;
    UINT16                   *FileName = NULL;
    volatile UINT64           OpenMode;
    EFI_GUID                  FileInfoGuid = EFI_FILE_INFO_ID;
    UINTN                     FileInfoSize = 0;
    UINTN                     FileSize = 0;
    EFI_FILE_INFO            *FileInfo = NULL;
    VOID                     *Buffer= NULL;

    Status = FwUpdateGetHandleForPartition(Partition,&PartHandle);

    Status = gBS->HandleProtocol(PartHandle,
                                 &gEfiSimpleFileSystemProtocolGuid,
                                 (VOID **)&Fs);

    if(EFI_ERROR(Status)) {
        return Status;
    }

    Status = Fs->OpenVolume(Fs,&FsRoot);

    if(EFI_ERROR(Status)) {
        return Status;
    }

    Status = FwUpdateGetFileName(FwFile,&FileName);
    
    if(EFI_ERROR(Status)) {
        return Status;
    }

    // Open the file as read and not write in case the file is marked with read-only attribute
    OpenMode = EFI_FILE_MODE_READ;

    Status = FsRoot->Open(FsRoot,
                          &File,
                          FileName,                          
                          OpenMode,
                          Attributes);

    if(EFI_ERROR(Status)) {
        return Status;
    }

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

        Status = File->SetInfo (File, &FileInfoGuid, FileInfo->Size, FileInfo);
      }

      Status = gBS->FreePool(FileInfo);
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
      
    if(EFI_ERROR(Status))
    {
        return Status;
    }

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
      Status = File->Flush(File);
      Status = gBS->FreePool(Buffer);
        Buffer = NULL;
      }
      Status = gBS->FreePool(FileInfo);
      FileInfo = NULL;
    }


    Status = File->Delete(File) ;

    FsRoot->Close(FsRoot);

    return Status ;
}


/**
 * This function detects the presence of a firmware file at the 
 * given destination.
 * 
 * 
 * @param FwFile - Enum for the desired file
 * @param Partition - Enum for the destination partition 
 * @param FoundFile - Indicate whether specified file is found 
 *                  or not
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS
FindFwFile(
    IN  QCOM_FW_UPDATE_FW_FILE FwFile,
    IN  QCOM_FW_UPDATE_FW_PARTITION Partition,
    OUT BOOLEAN *FoundFile
    )
{
    EFI_STATUS Status;
    EFI_HANDLE PartHandle ;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *Fs;
    EFI_FILE_PROTOCOL        *FsRoot;
    EFI_FILE_PROTOCOL        *File;
    UINT64                   Attributes = 0x0;
    UINT16                   *FileName;
    volatile UINT64          OpenMode;

    Status = FwUpdateGetHandleForPartition(Partition,&PartHandle);

    if(EFI_ERROR(Status)) {
        return Status;
    }

    Status = gBS->HandleProtocol(PartHandle,
                                 &gEfiSimpleFileSystemProtocolGuid,
                                 (VOID **)&Fs);

    if(EFI_ERROR(Status)) {
        return Status;
    }

    Status = Fs->OpenVolume(Fs,&FsRoot);

    if(EFI_ERROR(Status)) {
        return Status;
    }

    Status = FwUpdateGetFileName(FwFile,&FileName);
    
    if(EFI_ERROR(Status)) {
        return Status;
    }

    OpenMode = EFI_FILE_MODE_READ;

    Status = FsRoot->Open(FsRoot,
                          &File,
                          FileName,                          
                          OpenMode,
                          Attributes);

    if(EFI_NOT_FOUND == Status) {
        *FoundFile  = FALSE;
    }
    else {
        *FoundFile = TRUE;
    }

    return EFI_SUCCESS ;

}


/**
 * This function copies a firmware file (DPP, EFS, SBL, etc), 
 * from the given source to the given destination. 
 *  
 *  
 * @param SrcPart - Enum for the source partition 
 * @param SrcFileName - Unicode Null-terminated source file name
 * @param DestPart - Enum for the destination partition 
 * @param DestFileName - Unicode Null-terminated destination 
 *                     file name
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
CopyFwFile2(
    IN QCOM_FW_UPDATE_FW_PARTITION  SrcPart,
    IN CHAR16                       *SrcFileName,
    IN QCOM_FW_UPDATE_FW_PARTITION  DestPart,
    IN CHAR16                       *DestFileName
    )
{
    VOID                                *FileBuffer;
    EFI_STATUS                          Status;
    EFI_HANDLE                          PartHandle;    
    UINTN                               FileSize;

    //Placeholders for Partition-specific
    // read/write functions.
    EFI_STATUS (*ReadFile) () = NULL;
    EFI_STATUS (*WriteFile)() = NULL;

    //Read Src file into buffer
    Status = FwUpdateGetHandleForPartition(SrcPart, &PartHandle);
        
    if(EFI_ERROR(Status))
    {
        return Status;
    }

    Status = FwUpdateGetReadFunction2(SrcPart, (VOID**)&ReadFile);

    if(EFI_ERROR(Status))
    {
        return Status;
    }
 
    Status = (ReadFile)(PartHandle, SrcFileName, &FileBuffer, &FileSize);

    if(EFI_ERROR(Status))
    {
        return Status;
    }
    
    
    //Write file back to Dest.
    Status = FwUpdateGetHandleForPartition(DestPart, &PartHandle);

    if(EFI_ERROR(Status))
    {
        return Status;
    }

    Status = FwUpdateGetWriteFunction2(DestPart,(VOID**)&WriteFile);

    if(EFI_ERROR(Status))
    {
        return Status;
    }

    Status = (WriteFile)(PartHandle, DestFileName, &FileBuffer ,&FileSize);
    
    gBS->FreePool(FileBuffer);

    return Status;   


}


/**
 * This function delete a firmware file (DPP, EFS, SBL, etc)
 *  
 *  
 * @param FileName - Unicode null-terminated file path
 * @param Partition - Enum of the partition
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS
DeleteFwFile2(
    IN CHAR16                       *FileName,
    IN QCOM_FW_UPDATE_FW_PARTITION  Partition
    )
{
    EFI_STATUS Status;
    EFI_HANDLE PartHandle ;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *Fs;
    EFI_FILE_PROTOCOL        *FsRoot;
    EFI_FILE_PROTOCOL        *File;
    UINT64                   Attributes = 0x0;
    volatile UINT64          OpenMode;
    EFI_GUID                 FileInfoGuid = EFI_FILE_INFO_ID;
    UINTN                    FileInfoSize = 0;
    UINTN                    FileSize = 0;
    EFI_FILE_INFO            *FileInfo = NULL;
    VOID                     *Buffer= NULL;

    Status = FwUpdateGetHandleForPartition(Partition, &PartHandle);

    Status = gBS->HandleProtocol(PartHandle,
                                 &gEfiSimpleFileSystemProtocolGuid,
                                 (VOID **)&Fs);

    if(EFI_ERROR(Status)) {
        return Status;
    }

    Status = Fs->OpenVolume(Fs,&FsRoot);

    if(EFI_ERROR(Status)) {
        return Status;
    }

    // Open the file as read and not write in case the file is marked with read-only attribute
    OpenMode = EFI_FILE_MODE_READ;

    Status = FsRoot->Open(FsRoot,
                          &File,
                          FileName,                          
                          OpenMode,
                          Attributes);

    if(EFI_ERROR(Status)) {
        return Status;
    }

    //Clear out the read only attribute to avoid access denied failure during delete
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

        Status = File->SetInfo (File, &FileInfoGuid, FileInfo->Size, FileInfo);
      }

      Status = gBS->FreePool(FileInfo);
      FileInfo = NULL;
    }
    
    // Close and re-open the file with write access to delete it
    File->Close(File);

    OpenMode = EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE;
        
    Status = FsRoot->Open(FsRoot,
                          &File,
                          FileName,                          
                          OpenMode,
                          Attributes);
    
    if(EFI_ERROR(Status))
    {
        return Status;
    }

    //read out the file size and zero out the file content
    Status = FwUpdateFatGetFileInfo(File,&FileInfoSize,&FileInfo);
         
    if(!EFI_ERROR(Status) && (FileInfo != NULL))
    {
      FileSize= FileInfo->FileSize;
      Status = gBS->AllocatePool(EfiBootServicesData, FileSize, (VOID**)&Buffer); 
      if (!EFI_ERROR(Status) && (Buffer != NULL))
      {
      ZeroMem(Buffer,FileSize);
      Status = File->Write(File,&FileSize, Buffer);
      Status = File->Flush(File);
      Status = gBS->FreePool(Buffer);
        Buffer = NULL;
      }
      Status = gBS->FreePool(FileInfo);
      FileInfo = NULL;
    }
    

    Status = File->Delete(File) ;

    FsRoot->Close(FsRoot);

    return Status ;
}


/**
 * This function detects the presence of a firmware file at the 
 * given destination.
 * 
 * 
 * @param FileName - Unicode null-terminated file path
 * @param Partition - Enum for the destination partition 
 * @param FoundFile - Indicate whether specified file is found 
 *                  or not
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS
FindFwFile2(
    IN  CHAR16                      *FileName,
    IN  QCOM_FW_UPDATE_FW_PARTITION Partition,
    OUT BOOLEAN                     *FoundFile
    )
{
    EFI_STATUS Status;
    EFI_HANDLE PartHandle ;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *Fs;
    EFI_FILE_PROTOCOL        *FsRoot;
    EFI_FILE_PROTOCOL        *File;
    UINT64                   Attributes = 0x0;
    volatile UINT64          OpenMode;

    Status = FwUpdateGetHandleForPartition(Partition, &PartHandle);

    if(EFI_ERROR(Status)) {
        return Status;
    }

    Status = gBS->HandleProtocol(PartHandle,
                                 &gEfiSimpleFileSystemProtocolGuid,
                                 (VOID **)&Fs);

    if(EFI_ERROR(Status)) {
        return Status;
    }

    Status = Fs->OpenVolume(Fs,&FsRoot);

    if(EFI_ERROR(Status)) {
        return Status;
    }

    OpenMode = EFI_FILE_MODE_READ;

    Status = FsRoot->Open(FsRoot,
                          &File,
                          FileName,                          
                          OpenMode,
                          Attributes);

    if(EFI_NOT_FOUND == Status) {
        *FoundFile  = FALSE;
    }
    else {
        *FoundFile = TRUE;
    }

    return EFI_SUCCESS ;

}

/**
 * Helper to compare decrypted FW files, checks if two files are
 * identical 
 *  
 * 
 * @param FwFile              - Enum for the desired file
 * @param DecryptedBuffer     - Source Buffer to copy from 
 * @param DecryptedBufferSize - Size of the source buffer 
 * @param DestPart            - Enum for the destination 
 *  						    partition
 * @param OutputFileName      - Enum for the output name of the 
 *                              file
 * 
 * @return 
 * TRUE                       - files are identical 
 * FALSE                      - files are not identical    
 */
BOOLEAN
CompareDecryptFwFile(
    IN QCOM_FW_UPDATE_FW_FILE FwFileSrc,
    IN VOID *DecryptedBuffer,
	IN UINTN DecryptedBufferSize,
    IN QCOM_FW_UPDATE_FW_PARTITION DestPart,
    IN QCOM_FW_UPDATE_FW_FILE FwFileDest
    )
{
	VOID							  *FileBufferDes = NULL;
	EFI_STATUS						   Status;
	EFI_HANDLE						   PartHandle;	  
	UINTN							   FileSizeDes;
	BOOLEAN							   ret;	
	
	//Placeholders for Partition-specific read functions.
	EFI_STATUS (*ReadFileDes)() = NULL;
	
	//Read dest file into buffer.
	Status = FwUpdateGetHandleForPartition(DestPart,&PartHandle);
	
	if(EFI_ERROR(Status))
	{
		return FALSE;
	}
	
	Status = FwUpdateGetReadFunction(DestPart,(VOID**)&ReadFileDes);
	
	if(EFI_ERROR(Status))
	{
		return FALSE;
	}

	Status = (ReadFileDes)(PartHandle,&FileBufferDes,FwFileSrc,&FileSizeDes);

	if(EFI_ERROR(Status))
	{
		return FALSE;
	}
		   
	if(CompareMem(FileBufferDes,DecryptedBuffer,DecryptedBufferSize)==0) 
	{
		ret = TRUE;
	}
	else
	{
		ret = FALSE;
	}
		
	gBS->FreePool(FileBufferDes);
	return ret;  

}
