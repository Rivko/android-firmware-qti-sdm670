/** @file FwFvFile.c
   
  Copy functions for Firmware Volumes for CopyFwLib.

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

#include <Library/FwCommonLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include "FwFileIO_Fv.h"



/**
 * Reads file from an FV
 *  into a buffer.
 * 
 * @param PartHandle - Handle to FileSystem
 * @param FileBuffer - Buffer to store the file
 * @param FwFile     - File Identifier
 * @param FileSize   - File size.  
 *  
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateReadFileFromFV(
    IN EFI_HANDLE               PartHandle,
    IN VOID                     **FileBuffer,
    IN QCOM_FW_UPDATE_FW_FILE   FwFile,
    IN OUT UINTN                *FileSize

    )
{
    EFI_FIRMWARE_VOLUME2_PROTOCOL       *Fv2;
    EFI_STATUS                          Status;
    EFI_GUID                            *NameGuid;    
    EFI_FV_FILETYPE                     FoundType;
    EFI_FV_FILE_ATTRIBUTES              FileAttributes;
    UINT32                              AuthenticationStatus;
    QCOM_FW_UPDATE_FFS_RAW_FILE         *RawFile = NULL;
    
    
    
    Status = gBS->HandleProtocol(PartHandle,
                                 &gEfiFirmwareVolume2ProtocolGuid,
                                 (VOID**)&Fv2);

    ERR_HANDLE(Status);

    Status = FwUpdateGetFileGuid(FwFile,
                                 (const EFI_GUID **)&NameGuid);
    ERR_HANDLE(Status);

    //First Find the file Size;
    Status = Fv2->ReadFile(Fv2,
                           NameGuid,
                           (VOID*)&RawFile,
                           FileSize,
                           &FoundType,
                           &FileAttributes,
                           &AuthenticationStatus);


    ERR_HANDLE(Status);

    *FileBuffer = RawFile;

	return EFI_SUCCESS;
}


