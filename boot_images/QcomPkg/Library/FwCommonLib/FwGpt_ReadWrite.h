/** @file FwGpt_ReadWrite.h
   
  Headerfile for FwGpt_ReadWrite.c

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

#ifndef __QCOM_FW_GPT_READWRITE_H__
#define __QCOM_FW_GPT_READWRITE_H__


#include <Uefi.h>
#include "FwHandle_Partition.h"
#include "FwHandle_File.h"


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
    );



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
    );

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
    );



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
    );




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
    );



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
    );







#endif

