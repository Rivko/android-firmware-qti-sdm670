/** @file FwFileIoWrapper.h
   
  I/O Wrapper functions for Firmware Volumes, Partitions, FAT 
  and DPP 

  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2012/11/14     mic         Added safe FAT write function
 2012/10/04     mic         Initial version

=============================================================================*/
#ifndef __QCOM_FW_FILEIO_WRAPPER_H__
#define __QCOM_FW_FILEIO_WRAPPER_H__

#include <Pi/PiFirmwareFile.h>
#include <Pi/PiFirmwareVolume.h>
#include <Protocol/FirmwareVolume2.h>
#include <Library/FwCommonLib.h>

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
  );

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

    );

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
    );

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
    IN VOID                     *FileBuffer,
    IN UINTN                    BufferSize
    );


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
    OUT VOID                    **FileBuffer,
    OUT UINTN                   *BufferSize
    );

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

    );

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
    IN VOID                     *FileBuffer,
    IN UINTN                    BufferSize

    );


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
    IN CHAR16                   *DPPname,
    OUT VOID                    **FileBuffer,
    OUT UINTN                   *BufferSize

    );

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
    );

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
    IN VOID                     *FileBuffer,
    IN UINTN                    BufferSize

    );

/**
 * Read from Block into a buffer by a given handle.
 * 
 * @param PartHandle - Handle of partition
 * @param FileBuffer - Buffer
 * @param BufferSize - Buffer size
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdatePartitionRead(
    IN EFI_HANDLE               PartHandle,
    OUT VOID                    **FileBuffer,
    OUT UINTN                   *BufferSize

    );

/**
 * Read a file from FV into a buffer by a given guid and handle.
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

    );
/**
 * Function to get FV handle
 * 
 * @param FvHandle - FV handle

 * @return : EFI_STATUS
 */
EFI_STATUS
FwUpdateGetFvHandle(
    OUT EFI_HANDLE *FvHandle
    );


#endif
