/** @file ReadWrite.c
   
  Externally-accessible Read and Write functions for 
  FwCommonLib. 
   
  Allows for File-enum and Partition-enum abstraction so 
  that external callers do not have to manage getting handles. 

  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2012/05/03     MiC        Initial version

=============================================================================*/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/FwCommonLib.h>
#include "FwGpt_ReadWrite.h"

/**
 * This function either reads/writes a 
 * Fw File from/to a buffer, given only 
 * the file's File enum and partition enum. 
 * 
 * 
 * @param FwFile 
 * @param SrcPart 
 * @param FileBuffer 
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwFileOperation(
    IN  QCOM_FW_UPDATE_FW_FILE      FwFile,
    IN  QCOM_FW_UPDATE_FW_PARTITION SrcPart,
    IN OUT VOID                     **FileBuffer,    
    IN  UINTN                       *FileSize,
    BOOLEAN                         Read
    )
{
    EFI_STATUS (*FileOperation)() = NULL;
    EFI_HANDLE PartHandle;
    EFI_STATUS Status;

    Status = FwUpdateGetHandleForPartition(SrcPart,
                                           &PartHandle);

    ERR_HANDLE(Status);

    if(Read)
    {
        Status = FwUpdateGetReadFunction(SrcPart,
                                        (VOID**) &FileOperation);        
    }
    else
    {
        Status = FwUpdateGetWriteFunction(SrcPart,
                                        (VOID**) &FileOperation);        

    }
    
    ERR_HANDLE(Status);

    Status = FileOperation(PartHandle,FileBuffer,FwFile,FileSize);    

    return Status;
}



/**
 * This function reads a firmware file (DPP, EFS, SBL, etc), 
 * from the given partition into a buffer.
 * 
 * 
 * @param FwFile - Enum for the desired file
 * @param SrcPart - Enum for the source partition
 * @param FileBuffer - Buffer in which to read FwFile.
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS
ReadFwFile(
    IN  QCOM_FW_UPDATE_FW_FILE      FwFile,
    IN  QCOM_FW_UPDATE_FW_PARTITION SrcPart,
    IN  VOID                        **FileBuffer,
    IN  UINTN                       *FileSize
    )
{
    return FwFileOperation(FwFile,SrcPart,FileBuffer,FileSize,TRUE);
}


/**
 * This function writes a firmware file (DPP, EFS, SBL, etc), 
 * from the given buffer into a partition. 
 * 
 * 
 * @param FwFile - Enum for the desired file
 * @param SrcPart - Enum for the source partition
 * @param FileBuffer - Buffer from which to write FwFile.
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS
WriteFwFile(
    IN  QCOM_FW_UPDATE_FW_FILE      FwFile,
    IN  QCOM_FW_UPDATE_FW_PARTITION SrcPart,
    OUT  VOID                       **FileBuffer,
    IN  UINTN                       *FileSize
    )
{
    return FwFileOperation(FwFile,SrcPart,FileBuffer,FileSize,FALSE);
}


/**
 * This function either reads/writes a 
 * Fw File from/to a buffer, given only 
 * the file's File enum and partition enum. 
 * 
 * 
 * @param FileName - Unicode Null-terminated file path
 * @param SrcPart 
 * @param FileBuffer 
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwFileOperation2(
    IN  CHAR16                      *FileName,
    IN  QCOM_FW_UPDATE_FW_PARTITION SrcPart,
    IN  OUT VOID                    **FileBuffer,    
    IN  UINTN                       *FileSize,
    BOOLEAN                         Read
    )
{
    EFI_STATUS (*FileOperation)() = NULL;
    EFI_HANDLE PartHandle;
    EFI_STATUS Status;

    Status = FwUpdateGetHandleForPartition(SrcPart,
                                           &PartHandle);

    ERR_HANDLE(Status);

    if(Read)
    {
        Status = FwUpdateGetReadFunction2(SrcPart,
                                  (VOID**) &FileOperation);        
    }
    else
    {
        Status = FwUpdateGetWriteFunction2(SrcPart,
                                  (VOID**) &FileOperation);        

    }
    
    ERR_HANDLE(Status);

    Status = FileOperation(PartHandle, FileName, FileBuffer, FileSize);    

    return Status;
}



/**
 * This function reads a firmware file (DPP, EFS, SBL, etc), 
 * from the given partition into a buffer.
 * 
 * 
 * @param FileName - Unicode Null-terminated file path
 * @param SrcPart - Enum for the source partition
 * @param FileBuffer - Buffer in which to read FwFile.
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS
ReadFwFile2(
    IN  CHAR16                      *FileName,
    IN  QCOM_FW_UPDATE_FW_PARTITION SrcPart,
    IN  VOID                        **FileBuffer,
    IN  UINTN                       *FileSize
    )
{
    return FwFileOperation2(FileName, SrcPart, FileBuffer, FileSize, TRUE);
}


/**
 * This function writes a firmware file (DPP, EFS, SBL, etc), 
 * from the given buffer into a partition. 
 * 
 * 
 * @param FileName - Unicode Null-terminated file path
 * @param SrcPart - Enum for the source partition
 * @param FileBuffer - Buffer from which to write FwFile.
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS
WriteFwFile2(
    IN  CHAR16                      *FileName,
    IN  QCOM_FW_UPDATE_FW_PARTITION SrcPart,
    OUT  VOID                       **FileBuffer,
    IN  UINTN                       *FileSize
    )
{
    return FwFileOperation2(FileName, SrcPart, FileBuffer, FileSize, FALSE);
}



