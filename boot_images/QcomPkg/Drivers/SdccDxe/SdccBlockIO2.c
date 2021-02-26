/** @file sdcc_blockio2.c
   
   This file implements the utility function for Block IO 2 in SDCC driver
  
   Copyright (c) 2014 - 2015 Qualcomm Technologies, Inc. 
   All Rights Reserved. 
   Qualcomm Technologies Proprietary and Confidential  
   
**/

/*=============================================================================
                              EDIT HISTORY

when         who     what, where, why
----------   ----    ----------------------------------------------------------
2015-04-17   rm      Only check transfer complete in BlkIoTakeOverTransfer 
2015-03-10   rm      Use same TPL for BLOCKIO and BLOCKIO2
2015-03-06   rm      Add fix for Blockio2 concurrent issue 
2015-01-09   rm      Add fix for Blockio2 random crash 
2014-05-09   rm      Initial version.

=============================================================================*/

#include "SdccBlockIO2.h"

static EFI_LOCK    BlkIo2Lock = EFI_INITIALIZE_LOCK_VARIABLE (SDCC_TPL);
BLKIO2_REQ_LIST   EmmcReqList;

extern UINT32            EmmcDriveNo; 
extern EFI_EVENT         BlkIo2InternalEvent;
DRIVER_INFO              MMCHS_TransferInfo;
extern BLKIO2_XFER_INFO  BlkIo2_Xfer_Info;
/* used to initialize other lock */
static EFI_LOCK BlkIo2InitLock = EFI_INITIALIZE_LOCK_VARIABLE (SDCC_TPL); 

static VOID BlkIo2MovToNextReq(BLKIO2_REQ_LIST *pReqList); 
static BOOLEAN ListIsNotEmpty(BLKIO2_REQ_LIST *pReqList);

/******************************************************************************
* Name: BlkIo2CleanUp
*
* Description:
*    Reset DEVICE STATUS struct, cancel the timer and signal the 
*    pending event
* 
******************************************************************************/
static 
VOID 
BlkIo2CleanUp(
   )
{
    /*Cancel timer if set*/
   if (TRUE == MMCHS_TransferInfo.TimerSet)
   {
      MMCHS_TransferInfo.TimerSet = FALSE;
      gBS->SetTimer (BlkIo2InternalEvent, TimerCancel, 100000);
   }
   
   
   if (TRUE == MMCHS_TransferInfo.EventPending)
   {
      MMCHS_TransferInfo.EventPending = FALSE;
      gBS->SignalEvent((MMCHS_TransferInfo.Token)->Event);
   }

} /* BlkIo2CleanUp */

/******************************************************************************
* Name: BlkIo2ErrorHandler
*
* Description:
*    Handle error in Block IO 2
*
* Arguments:
*    pReqList           [IN] : pointer to Block Io 2 request list
*    rc                 [IN] : Sdcc status
*
* Returns:
*    None
******************************************************************************/
static
VOID
BlkIo2ErrorHandler(
   IN BLKIO2_REQ_LIST          *pReqList,
   IN SDCC_STATUS               rc
   )
{
   EFI_STATUS status = EFI_SUCCESS;
   
   if (SDCC_NO_ERROR != rc)
   {
      /* There's an error */
      MMCHSDecodeErrorCode (rc);

      if ((SDCC_ERR_CARD_REMOVED == rc) || (SDCC_ERR_CARD_UNDETECTED == rc) ||
          (SDCC_ERR_CMD_TIMEOUT == rc))
      {       
         status = EFI_NO_MEDIA;
      }
      else if (SDCC_ERR_INVALID_PARAM == rc)
      {
         status = EFI_INVALID_PARAMETER;
      }
      else
      {
         status = EFI_DEVICE_ERROR;
      }
   }
   else
   {
      status = EFI_SUCCESS;
   }
   
   if (TRUE == MMCHS_TransferInfo.EventPending)
   {
      MMCHS_TransferInfo.EventPending = FALSE;
      gBS->SignalEvent((MMCHS_TransferInfo.Token)->Event);
   }
   
   if (ListIsNotEmpty(pReqList))
   {
      pReqList->CurReq->Token->TransactionStatus = status;
      gBS->SignalEvent(pReqList->CurReq->Token->Event);
      BlkIo2MovToNextReq(pReqList);
      status = EFI_DEVICE_ERROR;
   }
   
   BlkIo2CleanUp();
} /* BlkIo2ErrorHandler */

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
  IN UINT32                   MediaId,
  IN EFI_LBA                  Lba,
  IN UINTN                    BufferSize,
  OUT VOID                   *Buffer
   )
{
   EFI_STATUS           status = EFI_SUCCESS;
   SDCC_STATUS          rc = SDCC_NO_ERROR;
   UINT32               count = 0;
   DEVICE_STATUS        ds;
   
   BLKIO2_REQ_LIST   *pReqList = &EmmcReqList;
       
   /* poll the transfer complete or any error status bits */
   while(count < SDHCI_STATUS_POLL_MAX)
   {  
      ds = sdcc_sdhci_check_transfer_status(EmmcDriveNo, &rc);
      if (ds == TRANSFER_DONE || ds == DEVICE_ERROR)
      {
         break;
      }
      sdcc_udelay(100);
      count += 100;
   }

   if (rc != SDCC_NO_ERROR)
   { 
      /* If error, the current Block IO 2 request failed, save 
         the status and token then set EventPending bit */
      pReqList->CurReq->Token->TransactionStatus = EFI_DEVICE_ERROR;
      MMCHS_TransferInfo.EventPending = TRUE;
      MMCHS_TransferInfo.Token = pReqList->CurReq->Token;
   
      BlkIo2MovToNextReq(pReqList);
      return EFI_DEVICE_ERROR;
   }
   
   if (ds != DEVICE_IDLE)
      sdcc_complete_transfer();

   /*Do Block IO transfer*/  
   if (MMCHS_TransferInfo.TransferType == BLOCKIO_READ)
      status = MMCHSReadBlocks (This, MediaId, Lba, BufferSize, Buffer);
   else 
      status = MMCHSWriteBlocks (This, MediaId, Lba, BufferSize, Buffer);

   /* If Block IO error */    
   if (status != EFI_SUCCESS)
   {
      /* If previous Block IO 2 request completed, save EFI_SUCCESS
         to it */
      if (pReqList->CurReq->BufferSize == 0)
         pReqList->CurReq->Token->TransactionStatus = EFI_SUCCESS;
      /* If not completed, it cannot continue since Block IO error,
         save EFI_DEVICE_ERROR to it */
      else
         pReqList->CurReq->Token->TransactionStatus = EFI_DEVICE_ERROR;

      MMCHS_TransferInfo.EventPending = TRUE;
      MMCHS_TransferInfo.Token = pReqList->CurReq->Token;
   
      BlkIo2MovToNextReq(pReqList);
      return status;
   }
 
   /* Check if previous Block IO 2 completed */ 
   if (pReqList->CurReq->BufferSize == 0)
   {
      MMCHS_TransferInfo.EventPending = TRUE;
      pReqList->CurReq->Token->TransactionStatus = EFI_SUCCESS;
      MMCHS_TransferInfo.Token = pReqList->CurReq->Token;
      BlkIo2MovToNextReq(pReqList);
   }
   pReqList->CurReq->IsStart = FALSE;
   return status;
} /* BlkIoTakeOverTransfer */

/******************************************************************************
* Name: BlkIo2StartTransfer
*
* Description:
*    Block IO 2 start to make transfer
*
* Arguments:
*    pReqList           [IN] : pointer to Block Io 2 request list
*
* Returns:
*    SDCC_NO_ERROR or error code
******************************************************************************/
static
SDCC_STATUS
BlkIo2StartTransfer(
   BLKIO2_REQ_LIST       *pReqList
   )
{
   SDCC_STATUS  rc = SDCC_NO_ERROR;
   MMCHS_DEV    *MmchsDevice;
   UINT32 transfer_size = 0, block_count = 0, block_size = 0;
   EFI_LBA  Lba;
   UINTN    BufferSize;
   UINT8    *temp_buffer;
   OPERATION_TYPE      Type;
   sdcc_device  *sdcc_handle;

   
   /*grab from request list*/        
   BufferSize = pReqList->CurReq->BufferSize; 
   if (BufferSize == 0)
   {
     return SDCC_ERR_INVALID_PARAM;
   }
    
   Lba = pReqList->CurReq->Lba; 
   temp_buffer = (UINT8*)pReqList->CurReq->Buffer;
   Type = pReqList->CurReq->Type;
         
   MmchsDevice = MMCHS_DEV_FROM_BLOCKIO2 (pReqList->CurReq->This);
   sdcc_handle = MmchsDevice->DeviceHandle;
   block_size = MmchsDevice->BlkIo.Media->BlockSize;
   block_count = (BufferSize/block_size);
   transfer_size = (block_count > SDCC_MAX_BLOCK_TRANSFER_SIZE) ? 
               SDCC_MAX_BLOCK_TRANSFER_SIZE : block_count;
      
   rc = sdcc_handle_request (sdcc_handle, 
                             Lba, 
                             temp_buffer, 
                             transfer_size,
                             Type);
                             
   Lba = Lba + transfer_size;
   block_count = block_count - transfer_size;
   BufferSize = block_count * block_size;
   temp_buffer = temp_buffer + (transfer_size * block_size);
   
   /*Restore to request entry*/
   pReqList->CurReq->BufferSize = BufferSize;
   pReqList->CurReq->Lba = Lba;
   pReqList->CurReq->Buffer = (VOID*)temp_buffer; 
   pReqList->CurReq->IsStart = TRUE;
  
   return rc;
} /* BlkIo2StartTransfer */

/******************************************************************************
* Name: BlkIo2HandleRequest
*
* Description:
*    Handle one request in Block IO 2 way. That is, start the
*    transfer, set timer and signal the pending event. Error will
*    be handled inside the function.
*
* Arguments:
*    pReqList           [IN] : pointer to Block Io 2 request list
*
* Returns:
*    None
******************************************************************************/
static
VOID 
BlkIo2HandleRequest(BLKIO2_REQ_LIST *pReqList)
{
   SDCC_STATUS               rc = SDCC_NO_ERROR;
   
   rc = BlkIo2StartTransfer(pReqList);
         
   if(rc != SDCC_NO_ERROR)
   {
      BlkIo2ErrorHandler(pReqList, rc);
      return;
   }
      
   /*Set timer if not set*/
   if (FALSE == MMCHS_TransferInfo.TimerSet)
   {
      MMCHS_TransferInfo.TimerSet = TRUE;
      gBS->SetTimer (BlkIo2InternalEvent, TimerPeriodic, BLK_IO2_POLLING_PERIOD);
   }
      
   if (TRUE == MMCHS_TransferInfo.EventPending)
   {
      MMCHS_TransferInfo.EventPending = FALSE;
      gBS->SignalEvent((MMCHS_TransferInfo.Token)->Event);
   }
   return;
} /* BlkIo2HandleRequest */

/******************************************************************************
* Name: BlkIo2HandleRequest
*
* Description:
*    Handle requests in request queue depending on device status
*
* Arguments:
*    Event              [IN] : UEFI event method, not used
*    Context            [IN] : Can be any forms of input parameter, not used
* Returns:
*    None
******************************************************************************/
VOID
BlkIo2TimerEventNotify(
  IN  EFI_EVENT                Event,
  IN  VOID                     *Context
  )
{	
   DEVICE_STATUS ds;  
   SDCC_STATUS rc = SDCC_NO_ERROR;
   EFI_BLOCK_IO2_TOKEN *temp_token;
   
   EfiAcquireLock (&BlkIo2Lock);
   
   /*For now use emmc request only*/
   BLKIO2_REQ_LIST *pReqList = &EmmcReqList;
   
   if (ListIsNotEmpty(pReqList) && pReqList->CurReq->IsStart)
   { 
      /* only check for transfer done or busy */
      ds = sdcc_sdhci_check_transfer_status(EmmcDriveNo, &rc);
   }
   else
   {
      ds = sdcc_get_device_status(EmmcDriveNo, &rc);
      if (ds == TRANSFER_DONE)
      {
         DEBUG ((EFI_D_ERROR, "Blkio2: TRANSFER_DONE when queue is empty or transfer not started!\n"));
         ASSERT(FALSE);
      }    
   }
   
   if (ds == DEVICE_ERROR)
   {
      BlkIo2ErrorHandler(pReqList, rc);
      EfiReleaseLock (&BlkIo2Lock);
      return;
   }
   
   if (ds == DEVICE_IDLE) 
   {
      if (ListIsNotEmpty(pReqList))
      {
         if (!pReqList->CurReq->IsStart)
         {
            BlkIo2HandleRequest(pReqList);
         }
         else
         {
            DEBUG ((EFI_D_ERROR, "Blkio2: Warning transfer started but DEVICE_IDLE\n "));
            ASSERT(FALSE);                 
         }		   
      }
      else   
      {
	    BlkIo2CleanUp();
      }	
   }
   else if (ds == DEVICE_BUSY)
   {
      if (TRUE == MMCHS_TransferInfo.EventPending)
      {
         MMCHS_TransferInfo.EventPending = FALSE;
         gBS->SignalEvent((MMCHS_TransferInfo.Token)->Event);
      }
   }
   else if (ds == TRANSFER_DONE)
   {
      sdcc_complete_transfer();
      
      /*Request not finished*/
      if(pReqList->CurReq->BufferSize > 0)
      {  
         BlkIo2HandleRequest(pReqList);
         EfiReleaseLock (&BlkIo2Lock);
         return;
      }
      
      temp_token = pReqList->CurReq->Token;
      BlkIo2MovToNextReq(pReqList);
      if (!ListIsNotEmpty(pReqList))
         BlkIo2CleanUp();       
      else 
         BlkIo2HandleRequest(pReqList);
      
      temp_token->TransactionStatus = EFI_SUCCESS;
      gBS->SignalEvent(temp_token->Event);   
   }
   else
   {
      DEBUG ((EFI_D_ERROR, "UNKNOWN_STATE!\n"));
   }
   
   EfiReleaseLock (&BlkIo2Lock);
   return;
}

/******************************************************************************
* Name: ListIsNotEmpty
*
* Description:
*    This function is to check whether list is empty
*
* Arguments:
*    pReqList           [IN] : pointer to Block Io 2 structure
*
* Returns:
*    Returns true or false
*
******************************************************************************/
static BOOLEAN ListIsNotEmpty(BLKIO2_REQ_LIST *pReqList)
{
   return (pReqList->Full == TRUE || pReqList->LastReq != pReqList->CurReq);
} /* ListIsNotEmpty */

/******************************************************************************
* Name: BlkIo2MovToNextReq
*
* Description:
*    This function is to move the pointer to next request in the queue
*    This function has to be called after dealing with current request
*
* Arguments:
*    pReqList           [IN] : pointer to Block Io 2 structure
*
* Returns:
*    None
*
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
   return;
} /* MovToNextReq */

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
EFI_STATUS BlkIo2ReqListInit(BLKIO2_REQ_LIST *pReqList, UINTN ListSize)
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
