/** @file FvFile.h
   
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
#ifndef __QCOM_FW_FILEIO_FV_H__
#define __QCOM_FW_FILEIO_FV_H__


#include <Pi/PiFirmwareFile.h>
#include <Pi/PiFirmwareVolume.h>
#include <Protocol/FirmwareVolume2.h>
#include <Library/FwCommonLib.h>


typedef struct{

    EFI_COMMON_SECTION_HEADER Header;
    UINT8                      Data[];

}QCOM_FW_UPDATE_FFS_RAW_FILE;

/**
 * Reads file from an FV
 *  into a buffer.
 * 
 * @param PartHandle - Handle to FileSystem
 * @param FileBuffer - Buffer to store the file
 * @param FwFile     - File Identifier
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateReadFileFromFV(
    IN EFI_HANDLE               PartHandle,
    IN VOID                     **FileBuffer,
    IN QCOM_FW_UPDATE_FW_FILE   FwFile,
    IN OUT UINTN                *FileSize
    );

#endif
