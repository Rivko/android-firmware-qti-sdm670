/** @file FwBlockFile.c
   
  Copy functions for Unformatted Partitions (Block-IO copies) 
  for CopyFwLib.

  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who            what, where, why
 --------       ---            --------------------------------------------------
 2012/10/08     mic             moved common macro definition to FwCommonLib.h
 2012/05/07     rs              Added support for decryption of Boot Firmware
 2011/10/12     jthompso        Initial version

=============================================================================*/
#include <Uefi.h>
#include <Guid/FileInfo.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/FwCommonLib.h>
#include <Protocol/BlockIo.h>
#include <Protocol/SimpleFileSystem.h>
#include "FwFileIO_Block.h"

#define FIRST_BLOCK 0

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

    )
{ 
    EFI_STATUS Status;
    EFI_BLOCK_IO_PROTOCOL   *BlockIo;
    UINTN                   BytesToMove;
    UINTN                   BlockSize;
    EFI_STATUS (*BlockOperation)() = NULL;
    EFI_STATUS (*AllocateFunction)() = NULL;

    Status = gBS->HandleProtocol(PartHandle,
                                 &gEfiBlockIoProtocolGuid,
                                 (VOID**)&BlockIo);

    ERR_HANDLE(Status);

    BlockSize = BlockIo->Media->BlockSize;

    //Set Read and Write operations.
    if(Read)
    {
        BlockOperation = BlockIo->ReadBlocks;
        AllocateFunction = AllocateAndZero;
        BytesToMove = BlockSize*((BlockIo->Media->LastBlock)+1);        
        *FileSize = BytesToMove;
    }
    else
    {
        BlockOperation = BlockIo->WriteBlocks;
        AllocateFunction = FwUpdateBlockFileAlignFileBuffer;
        
        //Align Bytes to Move to block size.
        ALIGN_TO_BLOCK(*FileSize,BlockIo->Media->BlockSize,BytesToMove);
    }    

    Status = AllocateFunction(EfiBootServicesData,*FileSize,FileBuffer,BytesToMove);

    ERR_HANDLE(Status);

    Status = BlockOperation(BlockIo,
                            BlockIo->Media->MediaId,
                            (UINT64) FIRST_BLOCK,
                            BytesToMove,
                            *FileBuffer);


    ERR_HANDLE(Status);

    Status = BlockIo->FlushBlocks(BlockIo);

    ERR_HANDLE(Status);

    return Status;
}


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

    )
{    
    return FwUpdateBlockFileOperation(PartHandle,FileBuffer,FwFile,FileSize,TRUE);

}


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

    )
{
    return FwUpdateBlockFileOperation(PartHandle,FileBuffer,FwFile,FileSize,FALSE);
}


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
    )
{ 
    EFI_STATUS              Status;
    EFI_BLOCK_IO_PROTOCOL   *BlockIo;
    UINTN                   BlockSize;
    EFI_STATUS (*BlockOperation)() = NULL;
    EFI_STATUS (*AllocateFunction)() = NULL;

    Status = gBS->HandleProtocol(PartHandle,
                                 &gEfiBlockIoProtocolGuid,
                                 (VOID**)&BlockIo);

    ERR_HANDLE(Status);

    BlockSize = BlockIo->Media->BlockSize;
 
    //Align length to block size.
    ALIGN_TO_BLOCK(*Length, BlockSize, *Length);

    //Set Read and Write operations.
    if(Read)
    {
        BlockOperation = BlockIo->ReadBlocks;
        AllocateFunction = AllocateAndZero;
    }
    else
    {
        BlockOperation = BlockIo->WriteBlocks;
        AllocateFunction = FwUpdateBlockFileAlignFileBuffer;
    }    

    Status = AllocateFunction(EfiBootServicesData, *Length, Buffer, *Length);

    ERR_HANDLE(Status);

    Status = BlockOperation(BlockIo,
                            BlockIo->Media->MediaId,
                            (EFI_LBA)Offset,
                            *Length,
                            *Buffer);


    ERR_HANDLE(Status);

    Status = BlockIo->FlushBlocks(BlockIo);

    ERR_HANDLE(Status);

    return Status;
}


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
    )
{ 
    EFI_STATUS              Status;
    EFI_BLOCK_IO_PROTOCOL   *BlockIo;
    
    Status = gBS->HandleProtocol(PartHandle,
                                 &gEfiBlockIoProtocolGuid,
                                 (VOID**)&BlockIo);

    ERR_HANDLE(Status);

    PartInfo->BlockSize = BlockIo->Media->BlockSize;
    PartInfo->PartSize = BlockIo->Media->BlockSize * ((BlockIo->Media->LastBlock) + 1);  

    return Status;
}


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
    )
{
    return FwUpdateBlockOperation(PartHandle, Offset, Length, Buffer, TRUE);
}


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
    )
{
    return FwUpdateBlockOperation(PartHandle, Offset, Length, Buffer, FALSE);
}
