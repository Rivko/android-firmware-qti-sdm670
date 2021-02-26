/** @file FwFileIO_Block.h
   
  Header for externally-available functions of the 
  FwFileIO_Block

  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2012/05/07     rs           Added support for decryption of Boot Firmware	
 2012/05/03     MiC          Initial version

=============================================================================*/

#ifndef __QCOM_FW_FILEIO_BLOCK_H__
#define __QCOM_FW_FILEIO_BLOCK_H__

#include <Uefi.h>
#include "FwHandle_Partition.h"
#include "FwHandle_File.h"

/**
 * Performs either a Block read 
 * or Block-Write operation. 
 * 
 * 
 * @param PartHandle - Handle for the partition to read/write 
 * @param FileBuffer - Buffer for input/output.
 * @param FwFile - Enum for the file being read/written.
 * @param FileSize - When Read, returns number of bytes read 
 *                  When Write, is the number of btyes to write.
 * @param Read - Boolean, whether to Read or Write.
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateBlockFileOperation(
    IN EFI_HANDLE               PartHandle,
    IN VOID                     **FileBuffer,
    IN QCOM_FW_UPDATE_FW_FILE   FwFile,
    IN OUT UINTN                *FileSize,
    IN BOOLEAN                  Read
    );



/**
 * Reads file from an unformatted partition
 *  into a buffer.
 * 
 * @param PartHandle - Handle to FileSystem
 * @param FileBuffer - Buffer to store the file
 * @param FwFile - File Identifier
 * @param FileSize - Number of bytes that were read.
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateReadFileFromBlock(
    IN EFI_HANDLE               PartHandle,
    IN VOID                     **FileBuffer,
    IN QCOM_FW_UPDATE_FW_FILE   FwFile,
    OUT UINTN                   *FileSize

    );



/**
 * Writes file to an unformatted partition
 *  from a buffer.
 * 
 * @param PartHandle - Handle to FileSystem
 * @param FileBuffer - Buffer to store the file
 * @param FwFile - File Identifier
 * @param FileSize - File size - Number of bytes to write..  
 *  
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateWriteFileToBlock(
    IN EFI_HANDLE               PartHandle,
    IN VOID                     **FileBuffer,
    IN QCOM_FW_UPDATE_FW_FILE   FwFile,
    IN UINTN                    *FileSize

    );



/**
 * Performs either a Block read 
 * or Block write operation. 
 * 
 * 
 * @param PartHandle - Handle for the partition to read/write 
 * @param Offset - Logical block address to read from or write 
 *               to.
 * @param Length - Number of bytes to read or write. It will be
 *               aligned to block size.
 * @param Buffer - Buffer to store the data. 
 * @param Read - Boolean, whether to Read or Write.
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateBlockOperation(
    IN EFI_HANDLE   PartHandle,
    IN UINT32       Offset,
    IN OUT UINT32   *Length,
    IN OUT VOID     **Buffer,
    IN BOOLEAN      Read
    );



/**
 * Query the partition info
 * 
 * @param PartHandle - Handle for the partition to read/write 
 * @param PartInfo - Buffer to store the data. 
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateGetPartitionInfo(
    IN EFI_HANDLE       PartHandle,
    OUT PPARTITION_INFO PartInfo
    );


/**
 * Reads block data from a partition
 * 
 * @param PartHandle - Handle for the partition to read 
 * @param Offset     - Logical block address to read from.
 * @param Length     - Number of bytes to read. Must be 
 *                     aligned to block size.
 * @param Buffer     - Buffer to store the data.
 *  
 * @return EFI_STATUS 
 */

EFI_STATUS
FwUpdateReadBlocks(
    IN EFI_HANDLE   PartHandle,
    IN UINT32       Offset,
    IN OUT UINT32   *Length,
    OUT VOID        **Buffer
    );



/**
 * Writes block data to a partition
 * 
 * @param PartHandle - Handle for the partition to write to
 * @param Offset     - Logical block address to write to.
 * @param Length     - Number of bytes to write. Must be 
 *                     aligned to block size.
 * @param Buffer     - Buffer with data to write.
 *  
 * @return EFI_STATUS 
 */

EFI_STATUS
FwUpdateWriteBlocks(
    IN EFI_HANDLE   PartHandle,
    IN UINT32       Offset,
    IN OUT UINT32   *Length,
    IN VOID         **Buffer
    );

#endif
