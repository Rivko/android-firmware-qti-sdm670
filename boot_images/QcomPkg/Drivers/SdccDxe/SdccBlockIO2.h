/** @file sdcc_blockio2.h
   
  This file provides SDCC Block IO 2 feature internal definitions. 

  Copyright (c) 2014 - 2015 Qualcomm Technologies, Inc. 
  All Rights Reserved. 
  Qualcomm Technologies Proprietary and Confidential

**/

/*=============================================================================
                              EDIT HISTORY

when         who     what, where, why
----------   ---     -----------------------------------------------------------
2015-01-09   rm      Add fix for Blockio2 random crash
2014-05-09   rm      Initial version

=============================================================================*/

#ifndef _BLOCK_IO2_H_
#define _BLOCK_IO2_H_

#include <Library/UefiLib.h>
#include "SdccDxe.h"

/* Block IO 2 polling period, 100000 is 10ms */
#define BLK_IO2_POLLING_PERIOD  100000    
/* Max wait time for status polling. In unit of usec */
#define SDHCI_STATUS_POLL_MAX      5000000

typedef enum {
  BLOCKIO_READ,
  BLOCKIO_WRITE,
  BLOCKIO2_READ,
  BLOCKIO2_WRITE
} BLK_IO_PROTOCOL_TYPE;

typedef struct{
  BLK_IO_PROTOCOL_TYPE       TransferType;        
  BOOLEAN                    BlkIO2Initialized;   
  BOOLEAN                    SDHCiEnabled;
  BOOLEAN                    TimerSet;          
  BOOLEAN                    EventPending;    
  EFI_BLOCK_IO2_TOKEN       *Token;
} DRIVER_INFO;

typedef struct {
  EFI_BLOCK_IO2_PROTOCOL    *This;
  UINT32                     MediaId;
  EFI_LBA                    Lba;
  EFI_BLOCK_IO2_TOKEN       *Token;
  UINTN                      BufferSize;
  VOID                      *Buffer;
  OPERATION_TYPE             Type;
  BOOLEAN                    IsStart;
} REQ_ENTRY;

typedef struct {
  EFI_LOCK                   ReqListLock;
  REQ_ENTRY                 *Head;        // head pointer of the space
  REQ_ENTRY                 *Tail;        // tail pointer of the space
  REQ_ENTRY                 *CurReq;      // point to current request
  REQ_ENTRY                 *LastReq;     // point to vacant memory next to last request
  BOOLEAN                    Full;
} BLKIO2_REQ_LIST;

/******************************************************************************
* Name: BlkIoTakeOverTransfer
*
* Description:
*    Block IO have priority to handle IO request while Block IO 2 is 
*    in progress. In this function, status related to Block IO will be
*    returned at the end of the function while status related to Block
*    IO 2 will be saved in the buffer and signal to the caller in the 
*    future
*
* Arguments:
*    This               [IN] : pointer to Block Io 2 request list
*    This               [IN] : Indicates a pointer to the calling context.
*    MediaId            [IN] : Id of the media, changes every time the media is replaced.
*    Lba                [IN] : The starting Logical Block Address to read from
*    BufferSize         [IN] : Size of Buffer, must be a multiple of device block size.
*    Buffer             [IN] : A pointer to the destination buffer for the data.
*                              The caller is responsible for either having implicit or
*                              explicit ownership of the buffer.
*
* Returns:
*    EFI_SUCCESS or error status
*
******************************************************************************/
EFI_STATUS 
BlkIoTakeOverTransfer (
  IN EFI_BLOCK_IO_PROTOCOL   *This,
  IN UINT32                  MediaId,
  IN EFI_LBA                 Lba,
  IN UINTN                   BufferSize,
  OUT VOID                   *Buffer
   );
   
/******************************************************************************
* Name: BlkIo2TimerEventNotify
*
* Description:
*    Handle requests in request queue depending on device status
*
* Arguments:
*    Event              [IN] : UEFI event method, not used
*    Context            [IN] : Can be any forms of input parameter, not used
*
* Returns:
*    Returns true or false
*
******************************************************************************/	   
VOID
BlkIo2TimerEventNotify(
  IN  EFI_EVENT                Event,
  IN  VOID                     *Context
  );						  
  
/******************************************************************************
* Name: BlkIo2ReqListInit
*
* Description:
*    This function is to move the pointer to next request in the queue
*    This function has to be called after dealing with current request
*
* Arguments:
*    pReqList           [IN] : pointer to Block Io 2 structure
*    ListSize           [IN] : The size of request queue
*
* Returns:
*    return EFI_SUCCESS or EFI_OUT_OF_RESOURCES
*
******************************************************************************/
EFI_STATUS BlkIo2ReqListInit(BLKIO2_REQ_LIST *pReqList, UINTN ListSize);  

#endif /* ifdef _BLOCK_IO2_H_ */
