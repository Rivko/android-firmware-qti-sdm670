/** @file UfsBlockIO2.c
   
   This file implements the utility function for Block IO 2 in UFS driver
  
   Copyright (c) 2014 - 2017 Qualcomm Technologies, Inc. 
   All Rights Reserved. 
   Qualcomm Technologies Proprietary and Confidential  
   
**/

/*=============================================================================
                              EDIT HISTORY

when         who     what, where, why
----------   ----    ---------------------------------------------------------- 
2017-02-01   jt      Don't continue to poll if return status is success
2016-09-23   jt      Add storage write protect  
2015-11-16   rh      Adapt the BlockIO2 file from SDCC driver
2015-04-17   rm      Only check transfer complete in BlkIoTakeOverTransfer 
2015-03-10   rm      Use same TPL for BLOCKIO and BLOCKIO2
2015-03-06   rm      Add fix for Blockio2 concurrent issue 
2015-01-09   rm      Add fix for Blockio2 random crash 
2014-05-09   rm      Initial version.

=============================================================================*/

#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/ArmLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UncachedMemoryAllocationLib.h>

#include <Protocol/BlockIo.h>
#include <Protocol/BlockIo2.h>
#include <Protocol/EFIRpmb.h>
#include <Protocol/EFICardInfo.h>
#include <Protocol/EFIEraseBlock.h>
#include <Protocol/EFIStorageWriteProtect.h>

#include "UFS.h"
#include "UfsBlockIO2.h"
#include <api/storage/ufs_api.h>

#define UFS_TPL TPL_CALLBACK

static EFI_LOCK          BlkIo2Lock = EFI_INITIALIZE_LOCK_VARIABLE (UFS_TPL);
extern BLKIO2_REQ_LIST   gUfsReqList;

extern EFI_EVENT         gUfsBlkIo2TimerEvent;
extern DRIVER_INFO       gUfsTransferInfo;
//extern BLKIO2_XFER_INFO  gBlkIo2_Xfer_Info;
/* used to initialize other lock */
static EFI_LOCK BlkIo2InitLock = EFI_INITIALIZE_LOCK_VARIABLE (UFS_TPL); 

static VOID BlkIo2MovToNextReq (BLKIO2_REQ_LIST *pReqList); 
static BOOLEAN ListIsNotEmpty (BLKIO2_REQ_LIST *pReqList);

/******************************************************************************
*    Reset DEVICE STATUS struct, cancel the timer and signal the 
*    pending event
******************************************************************************/
static VOID BlkIo2CleanUp()
{
   /*Cancel timer if set*/
   if (gUfsTransferInfo.TimerSet == TRUE)
   {
      gUfsTransferInfo.TimerSet = FALSE;
      gBS->SetTimer (gUfsBlkIo2TimerEvent, TimerCancel, 100000);
   }
   
   if (gUfsTransferInfo.EventPending == TRUE)
   {
      gUfsTransferInfo.EventPending = FALSE;
      gBS->SignalEvent ((gUfsTransferInfo.Token)->Event);
   }
} 

/******************************************************************************
*   Handle error in Block IO 2
*
* Arguments:
*    pReqList           [IN] : pointer to Block Io 2 request list
*    rc                 [IN] : status
******************************************************************************/
// Was static
VOID BlkIo2ErrorHandler(
   IN BLKIO2_REQ_LIST          *pReqList,
   IN INT32                     rc
   )
{
   EFI_STATUS status = EFI_SUCCESS;
   
   if (rc != UFS_EOK)
   {
      if (rc == -UFS_EINVAL) {
         status = EFI_INVALID_PARAMETER;
      } 
      else {
         status = EFI_DEVICE_ERROR;
      }
   } 
   else {
      status = EFI_SUCCESS;
   }
   
   if (gUfsTransferInfo.EventPending == TRUE)
   {
      gUfsTransferInfo.EventPending = FALSE;
      gBS->SignalEvent((gUfsTransferInfo.Token)->Event);
   }
   
   if (ListIsNotEmpty(pReqList))
   {
      pReqList->CurReq->Token->TransactionStatus = status;
      gBS->SignalEvent(pReqList->CurReq->Token->Event);
      BlkIo2MovToNextReq(pReqList);
      status = EFI_DEVICE_ERROR;
   }
   
   BlkIo2CleanUp();
} 

/******************************************************************************
*   Block IO have priority to handle IO request while Block IO 2 is in progress. 
*   In this function, status related to Block IO will be returned at the end of the 
*   function while status related to Block IO 2 will be saved in the buffer and 
*   signal to the caller in the future
*
* Arguments:
*    This               [IN] : Indicates a pointer to the calling context.
*    MediaId            [IN] : Id of the media, changes every time the media is replaced.
*    Lba                [IN] : The starting Logical Block Address to read from
*    BufferSize         [IN] : Size of Buffer, must be a multiple of device block size.
*    Buffer             [IN] : A pointer to the destination buffer for the data.
*
* Returns:
*    EFI_SUCCESS or error status
******************************************************************************/
EFI_STATUS BlkIoTakeOverTransfer (
                        IN EFI_BLOCK_IO_PROTOCOL   *This,
                        IN UINT32                   MediaId,
                        IN EFI_LBA                  Lba,
                        IN UINTN                    BufferSize,
                        OUT VOID                   *Buffer)
{
   EFI_STATUS         status = EFI_SUCCESS;
   INT32              rc = UFS_EOK;
   UINT32             count = 0;
   BLKIO2_REQ_LIST   *pReqList = &gUfsReqList;
   struct ufs_handle *hUFS;
   UFS_DEV           *UfsDevice;
       
   UfsDevice = UFS_DEV_FROM_BLOCKIO (This);
   hUFS = (struct ufs_handle *) UfsDevice->DeviceHandle;       

   /* poll the transfer complete or any error status bits */
   while(count < UFS_XFR_POLL_MAX) 
   {  
      rc = ufs_poll_xfer(hUFS);

      if (rc == UFS_EOK) {
         break; 
      }

      if (rc == -UFS_EBUSY) {
         continue;
      }

      gBS->Stall(100);
      count += 100;
   }

   if (rc != UFS_EOK)
   { 
      /* If error, the current Block IO 2 request failed, save 
         the status and token then set EventPending bit */
      pReqList->CurReq->Token->TransactionStatus = EFI_DEVICE_ERROR;
      gUfsTransferInfo.EventPending = TRUE;
      gUfsTransferInfo.Token = pReqList->CurReq->Token;
   
      BlkIo2MovToNextReq(pReqList);
      return EFI_DEVICE_ERROR;
   }
   
   /* Do Block IO transfer */  
   if (gUfsTransferInfo.TransferType == BLOCKIO_READ)
      status = UFSReadBlocks (This, MediaId, Lba, BufferSize, Buffer);
   else 
      status = UFSWriteBlocks (This, MediaId, Lba, BufferSize, Buffer);

   /* If Block IO error */    
   if (status != EFI_SUCCESS)
   {
      /* If previous Block IO 2 request completed, save EFI_SUCCESS to it */
      if (pReqList->CurReq->BufferSize == 0)
         pReqList->CurReq->Token->TransactionStatus = EFI_SUCCESS;
      /* If not completed, it cannot continue since Block IO error,
         save EFI_DEVICE_ERROR to it */
      else
         pReqList->CurReq->Token->TransactionStatus = EFI_DEVICE_ERROR;

      gUfsTransferInfo.EventPending = TRUE;
      gUfsTransferInfo.Token = pReqList->CurReq->Token;
   
      BlkIo2MovToNextReq(pReqList);
      return status;
   }
 
   /* Check if previous Block IO 2 completed */ 
   if (pReqList->CurReq->BufferSize == 0)
   {
      gUfsTransferInfo.EventPending = TRUE;
      pReqList->CurReq->Token->TransactionStatus = EFI_SUCCESS;
      gUfsTransferInfo.Token = pReqList->CurReq->Token;
      BlkIo2MovToNextReq(pReqList);
   }
   pReqList->CurReq->IsStart = FALSE;
   return status;
} 

/******************************************************************************
* Block IO 2 start to make transfer
*
* Arguments:
*    pReqList           [IN] : pointer to Block Io 2 request list
*
* Returns:
*    error code
******************************************************************************/
// Was static
INT32 BlkIo2StartTransfer (BLKIO2_REQ_LIST *pReqList)
{
   INT32         rc = UFS_EOK;
   UFS_DEV      *UfsDevice;
   UINT32        transfer_size = 0, block_count = 0, block_size = 0;
   EFI_LBA       Lba;
   UINTN         BufferSize;
   UINT8        *temp_buffer;
   struct ufs_handle *hUFS;
   
   /* Grab from request list */        
   BufferSize = pReqList->CurReq->BufferSize; 
   if (BufferSize == 0) {
     return -UFS_EINVAL;
   }

   Lba = pReqList->CurReq->Lba; 
   temp_buffer = (UINT8*)pReqList->CurReq->Buffer;
         
   UfsDevice = UFS_DEV_FROM_BLOCKIO2 (pReqList->CurReq->This);
   hUFS = UfsDevice->DeviceHandle;

   block_size = UfsDevice->BlkIo.Media->BlockSize;
   block_count = (BufferSize/block_size);
   transfer_size = (block_count > UFS_MAX_BLOCK_TRANSFERS) ? 
                     UFS_MAX_BLOCK_TRANSFERS : block_count;
      
   if (pReqList->CurReq->Type == BLOCKIO2_READ) {
      rc = ufs_read_ex (hUFS, temp_buffer, Lba, transfer_size);
   }
   else if (pReqList->CurReq->Type == BLOCKIO2_WRITE) {
      rc = ufs_write_ex (hUFS, temp_buffer, Lba, transfer_size);
   }
   else {
      DEBUG ((EFI_D_ERROR, "Blkio2: Bad transfer type value!\n"));
      ASSERT(FALSE);
   }
                             
   Lba = Lba + transfer_size;
   block_count = block_count - transfer_size;
   BufferSize = block_count * block_size;
   temp_buffer = temp_buffer + (transfer_size * block_size);
   
   // Restore to request entry 
   pReqList->CurReq->BufferSize = BufferSize;
   pReqList->CurReq->Lba = Lba;
   pReqList->CurReq->Buffer = (VOID*)temp_buffer; 
   pReqList->CurReq->IsStart = TRUE;
  
   return rc;
} 

/******************************************************************************
*  Handle one request in Block IO 2 way. That is, start the transfer, set timer 
*  and signal the pending event. Error will be handled inside the function.
*
* Arguments:
*    pReqList           [IN] : pointer to Block Io 2 request list
******************************************************************************/
static VOID BlkIo2HandleRequest (BLKIO2_REQ_LIST *pReqList)
{
   INT32 rc = UFS_EOK;
   
   rc = BlkIo2StartTransfer(pReqList);
   if (rc == -UFS_EBUSY) {
      // Pending transfer, continue
      ;
   }
   else if (rc != UFS_EOK)
   {
      BlkIo2ErrorHandler(pReqList, rc);
      return;
   }

   // Set timer if not set
   if (gUfsTransferInfo.TimerSet == FALSE)
   {
      gUfsTransferInfo.TimerSet = TRUE;
      gBS->SetTimer (gUfsBlkIo2TimerEvent, TimerPeriodic, BLKIO2_POLL_PERIOD);
   }
   if (gUfsTransferInfo.EventPending == TRUE)
   {
      gUfsTransferInfo.EventPending = FALSE;
      gBS->SignalEvent((gUfsTransferInfo.Token)->Event);
   }
} 

/******************************************************************************
*  Handle requests in request queue depending on device status
*
* Arguments:
*    Event              [IN] : UEFI event method, not used
*    Context            [IN] : Can be any forms of input parameter, not used
******************************************************************************/
VOID BlkIo2TimerEventNotify (
                     IN  EFI_EVENT  Event,
                     IN  VOID      *Context)
{  
   INT32 rc = -UFS_ENOENT;  
   EFI_BLOCK_IO2_TOKEN    *temp_token;
   BLKIO2_REQ_LIST        *pReqList = &gUfsReqList;
   
   EfiAcquireLock (&BlkIo2Lock);

   if (ListIsNotEmpty(pReqList) && pReqList->CurReq->IsStart) { 
      // only check for transfer done or busy 
      UFS_DEV *UfsDevice = UFS_DEV_FROM_BLOCKIO2 (pReqList->CurReq->This);
      rc = ufs_poll_xfer (UfsDevice->DeviceHandle);      
   }

   if (rc == -UFS_EIO) {
      BlkIo2ErrorHandler(pReqList, rc);
      EfiReleaseLock (&BlkIo2Lock);
      return;
   }
   
   if (rc == -UFS_ENOENT) {
      // No pending transfer
      if (ListIsNotEmpty(pReqList)) {
         if (!pReqList->CurReq->IsStart) {
            BlkIo2HandleRequest (pReqList);
         } 
         else {
            DEBUG ((EFI_D_ERROR, "Blkio2: Warning transfer started but DEVICE_IDLE\n "));
            ASSERT(FALSE);                 
         }        
      }
      else {
         BlkIo2CleanUp();
      }  
   }
   else if (rc == -UFS_EBUSY) {
      if (gUfsTransferInfo.EventPending == TRUE) {
         gUfsTransferInfo.EventPending = FALSE;
         gBS->SignalEvent((gUfsTransferInfo.Token)->Event);
      }
   }
   else if (rc == UFS_EOK) {
      if(pReqList->CurReq->BufferSize > 0) {  
         BlkIo2HandleRequest(pReqList);
         EfiReleaseLock (&BlkIo2Lock);
         return;
      }
      
      temp_token = pReqList->CurReq->Token;
      ASSERT(temp_token);                 
      BlkIo2MovToNextReq(pReqList);
      if (!ListIsNotEmpty(pReqList))
         BlkIo2CleanUp();       
      else 
         BlkIo2HandleRequest(pReqList);
      
      temp_token->TransactionStatus = EFI_SUCCESS;
      gBS->SignalEvent(temp_token->Event);   
   }
   else {
      DEBUG ((EFI_D_ERROR, "UNKNOWN_STATE!\n"));
   }
   
   EfiReleaseLock (&BlkIo2Lock);
   return;
}

/******************************************************************************
*  This function is to check whether list is empty
*
* Arguments:
*    pReqList           [IN] : pointer to Block Io 2 structure
******************************************************************************/
static BOOLEAN ListIsNotEmpty (BLKIO2_REQ_LIST *pReqList)
{
   return (pReqList->Full == TRUE || pReqList->LastReq != pReqList->CurReq);
} 

/******************************************************************************
*  This function is to move the pointer to next request in the queue;
*  to be called after dealing with current request
*
* Arguments:
*    pReqList           [IN] : pointer to Block Io 2 structure
******************************************************************************/
static VOID BlkIo2MovToNextReq(BLKIO2_REQ_LIST *pReqList)
{
   EfiAcquireLock (&pReqList->ReqListLock);
   
   SetMem(pReqList->CurReq, sizeof(REQ_ENTRY), 0);
   
   if(ListIsNotEmpty(pReqList))
   {
      if(pReqList->Full == TRUE)
        pReqList->Full = FALSE;
        
      pReqList->CurReq ++;
      
      if(pReqList->CurReq == pReqList->Tail) 
         pReqList->CurReq = pReqList->Head;
   }      
   EfiReleaseLock (&pReqList->ReqListLock);
}

/******************************************************************************
*  This function is to move the pointer to next request in the queue;
*  to be called after dealing with current request
*
* Arguments:
*    pReqList           [IN] : pointer to Block Io 2 structure
*    ListSize           [IN] : The size of request queue
*
* Returns:
*    return EFI_SUCCESS or EFI_OUT_OF_RESOURCES
******************************************************************************/
EFI_STATUS BlkIo2ReqListInit (BLKIO2_REQ_LIST *pReqList, UINTN ListSize)
{
   EFI_STATUS Status = EFI_SUCCESS;
   pReqList->ReqListLock = BlkIo2InitLock;
   /* Allocate buffer for requests list.*/
   pReqList->Head = (REQ_ENTRY *)AllocateZeroPool(ListSize * sizeof(REQ_ENTRY));
   if (pReqList->Head == NULL)
   {
      Status = EFI_OUT_OF_RESOURCES;
      DEBUG ((EFI_D_WARN, "Command Queue initialization failed\n"));
   }
   else
   {
      pReqList->Tail = pReqList->Head + ListSize;
      pReqList->CurReq = pReqList->Head;
      pReqList->LastReq = pReqList->Head;
      pReqList->Full = FALSE; 
   }    
   
   return Status;
}
