/*
===========================================================================

FILE:         SlimBusBamLib.c

DESCRIPTION:  This file implements the SLIMbus core driver BAM functionality
              via the BAM library.

===========================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1/buses/slimbus/src/dal/SlimBusBamLib.c#3 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
08/13/18   DPK     Removed checking of dummy descriptors (as part of QCTDD03351991) for Source port.
06/01/17   RC      Added the Logic to submit and clear the dummy descriptor
                   when port has 32bit datashift bit set for the QCTDD03351991.
03/29/17   RC      Added the API to clean the bam handles after BAM reset.
11/16/16   MJS     Klocwork fixes.
07/07/16   MJS     Interrupt enablement changes for HLOS compatibility.
05/04/16   MJS     Add BAM deinit support.
02/17/16   MJS     Fix BAM descriptor coverage gap in Rx message queue.
02/05/16   MJS     Add support for multiple endpoints per port.
11/03/15   MJS     Add mutex locking to avoid race condition when computing
                   the active/free IOVec slots.
10/14/15   MJS     Code portability changes for message FIFO index type.
10/14/15   MJS     Add support for batched messages.
10/14/15   MJS     Fix active slot accounting in BamGetIOVecActiveSlots().
10/08/15   MJS     Add NULL pointer check in GetOnePipeDesc().
06/08/15   NSD     Replace SlimBus_EventWait() calls with SlimBus_EventTimedWait().
05/05/15   NSD     Add API to get the number of used BAM IO Vector slots 
03/16/15   MJS     Fix compiler warnings.
01/07/15   mq      Prevent NULL pointer dereference
07/09/14   MJS     Don't use uLog logging without device mutex.
06/06/14   MJS     Add API to get the number of free IO vector slots.
04/14/14   MJS     Fix SlimBus_GetBamIOVecEx logic for polled mode.
02/28/14   MJS     Streamline ISR and don't lock device mutex in ISR.
02/14/14   MJS     Fix klocwork issues.
11/05/13   MJS     Fix for Slimbus and BAM IOVEC flags not defined equivalently.
10/17/13   MJS     Fix false port disconnection when DMA interrupt is
                   received while submitting BAM descriptors.
08/19/13   MJS     Support for combined port DMA interrupt.
08/19/13   MJS     Deprecate GetBamEvent API.
05/14/13   MJS     Update for BamPipeHandler logging macro.
04/24/13   MJS     Allow re-registering the same BAM event.
04/10/13   MJS     Fix uOffset incrementing.
03/08/13   MJS     Check for NULL context pointer during BAM callbacks.
02/21/13   MJS     Add port synchronization lock.
01/25/13   MJS     Fix Klocwork issue.
12/09/12   MJS     Set DESC_DONE flag for retrieval of polled pipe descs.
11/29/12   MJS     Support for BAM deferred processing to avoid deadlock.
10/26/12   MJS     Fix for BAM message queue support.
10/03/12   MJS     Add SlimBus_GetBamIOVecEx.
09/12/12   MJS     Workaround for BAM deadlock issue.
08/23/12   MJS     Ensure BAM is initialized during master pipe config.
05/16/12   MJS     Updated logging macros.
04/16/12   MJS     Updated logging and malloc APIs.
02/13/12   MJS     Remove port event mask.
12/07/11   MJS     Initial revision.

===========================================================================
             Copyright (c) 2011-2018 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

#include "SlimBusTarget.h"
#include SLIMBUS_OS_H
#include "SlimBusMaster.h"
#include "SlimBusBam.h"
#include "SlimBusLists.h"
#include "bam.h"

/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/

#define SB_BAM_PORT_DESC_SIZE 256

/* BAM transfer context structure */
typedef struct
{
  bam_iovec_type IoVec;
  SlimBusBamCtxt *pCtxt;
  void *pUser;
  SBBOOL bActive;
} SlimBusBamXfer;

/* BAM master port context structure */
typedef struct
{
  /* Make sure pPort is first to match union pointer */
  SlimBusMasterPortType *pPort;
  void *pUser;
  DALSYSEventHandle hEvent;
} SlimBusBamPortCtxt;

/* BAM context structure */
struct SlimBusBamCtxt
{
  bam_handle hBAM;
  uint32 PipeNum;
  SBBOOL bPoll;
  uint32 uXferBufCnt;
  uint32 uXferHead;
  uint32 uXferTail;
  SlimBusBamXfer *pXferBuf;
  SlimBusDevCtxt *pDevCtxt;
  union
  {
    SlimBusMsgFifoType *pMsgFifo;
    SlimBusBamPortCtxt PortCtxt;
    void *pVoid;
  } Obj;
};

/**
 * @brief Overall BAM callback
 *
 * This function implements the top-level BAM callback
 * 
 * @param[in] bam_result  BAM result
 */
static void SlimBusBamCb(bam_result_type bam_result)
{
  UNREFERENCED_PARAMETER(bam_result);
}

/**
 * @brief Tx BAM message queue callback handler
 *
 * This function implements the Tx BAM message queue handler for 
 * BAM events 
 * 
 * @param[in] bam_result  Pointer to the BAM result
 * @param[in] pCtxt  Pointer to the BAM context
 */
static void SlimBusBamTxHandler(bam_result_type *bam_result, SlimBusBamCtxt *pCtxt)
{
  SlimBusDevCtxt *pDevCtxt;
  SlimBusBamXfer *pXfer;

  if ( NULL == pCtxt ||
       NULL == pCtxt->Obj.pMsgFifo ||
       NULL == pCtxt->pDevCtxt )
  {
    return;
  }
  
  pDevCtxt = pCtxt->pDevCtxt;
  
  SlimBus_BamAcquireLock(pDevCtxt);

  if ( ( bam_result->event == BAM_EVENT_DESC_DONE ||
         bam_result->event == BAM_EVENT_EOT ) &&
       NULL != bam_result->data.xfer.xfer_cb_data )
  {
    pXfer = (SlimBusBamXfer*) bam_result->data.xfer.xfer_cb_data;
    
    pXfer->bActive = FALSE;
    pXfer->IoVec = bam_result->data.xfer.iovec;
  
    while (pCtxt->uXferTail != pCtxt->uXferHead)
    {
      uint32 uXferTail = pCtxt->uXferTail;
      pXfer = &pCtxt->pXferBuf[uXferTail];

      if (TRUE == pXfer->bActive)
      {
        break;
      }

      pCtxt->uXferTail++;
      if (pCtxt->uXferTail >= pCtxt->uXferBufCnt)
      {
        pCtxt->uXferTail -= pCtxt->uXferBufCnt;
      }

      if ( 0 == pXfer->IoVec.buf_pa &&
           0 == pXfer->IoVec.buf_size )
      {
        /* If the buffer pointer and size are zero, then we must have gotten failure */
        continue;
      }

      if ( pXfer->IoVec.flags & BAM_IOVEC_FLAG_EOT )
      {
        SlimBusEventNodeType *pEventNode = NULL;
        
        while( NULL != ( pEventNode = NextEventNode(&pDevCtxt->TxEventQueue, pEventNode) ) )
        {
          if ( (size_t)pEventNode->pTxPhysBuf == (size_t)pXfer->IoVec.buf_pa )
          {
            pEventNode->pTxBuf = NULL;
            pEventNode->pTxPhysBuf = NULL;
            break;
          }
        }
      }
      else
      {
        SlimBusMsgIdxType uDescIdx;
  
        uDescIdx = (uint32*)(pXfer->IoVec.buf_pa) - pCtxt->Obj.pMsgFifo->pPhysBuf;
  
        SlimBus_NormalizeIndexMinMax(&uDescIdx, pCtxt->Obj.pMsgFifo->uAckIdx,
                                     pCtxt->Obj.pMsgFifo->uTailIdx, pCtxt->Obj.pMsgFifo->uLen);

        uDescIdx += (pXfer->IoVec.buf_size+sizeof(uint32)-1)/sizeof(uint32);
        if ( pCtxt->Obj.pMsgFifo->uAckIdx < uDescIdx )
        {
          pCtxt->Obj.pMsgFifo->uAckIdx = uDescIdx;
        }
        if ( pCtxt->Obj.pMsgFifo->uAckIdx > pCtxt->Obj.pMsgFifo->uTailIdx )
        {
          pCtxt->Obj.pMsgFifo->uAckIdx = pCtxt->Obj.pMsgFifo->uTailIdx;
        }
      }
    }
  }
  else
  {
    SB_LOG_ERR_0("Got invalid BAM Tx message callback!");
  }
  
  SlimBus_BamReleaseLock(pDevCtxt);  

  pDevCtxt->checkEnterLowPowerFcn(pDevCtxt);
}

/**
 * @brief Rx BAM message queue callback
 *
 * This function implements the Rx BAM message queue callback 
 * for BAM events 
 * 
 * @param[in] bam_result  Pointer to the BAM result
 */
static void SlimBusBamRxCb(bam_result_type bam_result)
{
  SlimBusBamCtxt *pCtxt = (SlimBusBamCtxt*) bam_result.cb_data;
  SlimBusDevCtxt *pDevCtxt;
  SlimBusBamXfer *pXfer;

  if ( NULL == pCtxt ||
       NULL == pCtxt->Obj.pMsgFifo ||
       NULL == pCtxt->pDevCtxt )
  {
    return;
  }

  pDevCtxt = pCtxt->pDevCtxt;

  SlimBus_BamAcquireLock(pDevCtxt);

  if ( ( bam_result.event == BAM_EVENT_DESC_DONE ||
         bam_result.event == BAM_EVENT_EOT ) &&
       NULL != bam_result.data.xfer.xfer_cb_data )
  {
    SlimBusBasicClientCtxt *pMsgClient = NULL;
    uint32 uToken;
    
    pXfer = (SlimBusBamXfer*) bam_result.data.xfer.xfer_cb_data;

    pXfer->bActive = FALSE;
    pXfer->IoVec = bam_result.data.xfer.iovec;
  
    SlimBus_InterruptAcquireLock(pDevCtxt, &uToken);
    if ( !pDevCtxt->bMsgWorkLoopSignalled )
    {
      pDevCtxt->bMsgWorkLoopSignalled = TRUE;
      if ( NULL != (pMsgClient = PeekHeadClient(&pDevCtxt->MsgWaitList)) )
      {
        /* If a client is waiting to receive a message, signal the client thread 
           to process the received message */
        SlimBus_EventTrigger(pMsgClient->pClientCtxt->hClientEvent);
      }
      else
      {
        /* Use the worker thread if there is no client waiting to receive a message */
        SlimBus_EventTrigger(pDevCtxt->hMsgWorkLoopEvent);  
      }
    }
    SlimBus_InterruptReleaseLock(pDevCtxt, &uToken);
  }
  else
  {
    SB_LOG_ERR_0("Got invalid BAM Rx message callback!");
  }

  SlimBus_BamReleaseLock(pDevCtxt);  
}

/**
 * @brief Slimbus BAM pipe callback handler
 *
 * This function handles the processing of received BAM pipe 
 * events 
 * 
 * @param[in] bam_result BAM result
 * @param[in] pCtxt  Pointer to the BAM context
 * @param[out] peEvent  Pointer to the location to store the 
 *       extracted BAM events
 */
static void SlimBusBamPipeHandler
(
  bam_result_type *bam_result,
  SlimBusBamCtxt *pCtxt,
  SlimBusBamEventType *peEvent
  )
{
  SlimBusBamXfer *pXfer;
  SlimBusBamEventType eEvent = 0;

  if ( NULL == pCtxt )
  {
    return;
  }

  if (NULL != peEvent)
  {
    *peEvent = 0;
  }
  
  if ( bam_result->event == BAM_EVENT_DESC_DONE ||
       bam_result->event == BAM_EVENT_EOT )
  {
    pXfer = (SlimBusBamXfer*) bam_result->data.xfer.xfer_cb_data;

    if (NULL != pXfer)
    {
      pXfer->bActive = FALSE;
      pXfer->IoVec = bam_result->data.xfer.iovec;
    }
    else
    {
      bam_result->event = BAM_EVENT_INVALID;
    }
  }
  else if ( bam_result->event == BAM_EVENT_ERROR )
  {
    SB_LOG_DECLARE_PDEVCTXT(pCtxt->pDevCtxt);
    SB_LOG_ERR_0("Got port BAM error callback!");
  }

  eEvent = (SlimBusBamEventType)(bam_result->event);
  
  if (NULL != peEvent)
  {
    *peEvent = eEvent;
  }
}

/**
 * @brief Slimbus BAM pipe callback 
 *
 * This function handles the Slimbus BAM pipe callbacks
 * 
 * @param[in] bam_result  BAM result
 */
static void SlimBusBamPipeCb(bam_result_type bam_result)
{
  SlimBusBamCtxt *pCtxt = (SlimBusBamCtxt*) bam_result.cb_data;
  SlimBusDevCtxt *pDevCtxt;
  SlimBusBamEventType eEvent = 0;
  DALSYSEventHandle hEvent;
  void *pUser;

  if ( NULL == pCtxt ||
       NULL == pCtxt->pDevCtxt )
  {
    return;
  }

  pDevCtxt = pCtxt->pDevCtxt;

  SlimBus_BamAcquireLock(pDevCtxt);

  SlimBusBamPipeHandler(&bam_result, pCtxt, &eEvent);

  hEvent = pCtxt->Obj.PortCtxt.hEvent;
  pUser = pCtxt->Obj.PortCtxt.pUser;
  
  SlimBus_BamReleaseLock(pDevCtxt);

  if (0 != eEvent &&
      NULL != hEvent)
  {
    SlimBus_EventTriggerEx(hEvent, pUser);
  }
}

/**
 * @brief Signal the client for a BAM event
 *
 * This function signals the client for a BAM event
 * 
 * @param[in] pPort  Pointer to the master port structure 
 */
void SlimBus_SignalBamEvent(SlimBusMasterPortType *pPort)
{
  SlimBusBamCtxt *pCtxt = pPort->pBamCtxt;

  if ( NULL != pCtxt )
  {
    DALSYSEventHandle hEvent = pCtxt->Obj.PortCtxt.hEvent;
    void *pUser = pCtxt->Obj.PortCtxt.pUser;

    if (NULL != hEvent)
    {
      SlimBus_EventTriggerEx(hEvent, pUser);
    }
  }
}

/**
 * @brief Get one processed descriptor for a BAM port
 *
 * This function retrieves on processed descriptor for a BAM
 * port
 * 
 * @param[in] pPort  Pointer to the master port structure
 */
SBBOOL SlimBus_GetOnePipeDesc(SlimBusMasterPortType *pPort)
{
  SlimBusBamCtxt *pCtxt = pPort->pBamCtxt;
  
  if ((NULL != pCtxt) && (NULL != pCtxt->hBAM))
  {
    if ( pCtxt->bPoll )
    {
      bam_result_type result = {0};
      SlimBusDevCtxt *pDevCtxt = pCtxt->pDevCtxt;

      bam_pipe_poll(pCtxt->hBAM, &result);
      if ( BAM_EVENT_INVALID != result.event &&
           (uint32)result.data.xfer.xfer_cb_data == SLIMBUS_DATASHIFT_INVALIDATE )
      {
        pPort->bDataShiftRemoval = FALSE;
        SB_LOG_INFO_1("Processed dummy IO vector (port %d)", pPort->portNum);
        bam_pipe_poll(pCtxt->hBAM, &result);
      }
      if (BAM_EVENT_INVALID != result.event)
      {
        SlimBus_BamAcquireLock(pDevCtxt);
        SlimBusBamPipeHandler(&result, pCtxt, NULL);
        SlimBus_BamReleaseLock(pDevCtxt);  
        
        return TRUE;
      }
    }
  }

  return FALSE;
}

/**
 * @brief Perform deferred BAM processing that cannot be done 
 *        during BAM callbacks in order to avoid deadlock
 *
 * This function performs deferred BAM processing that cannot be 
 * done during BAM callbacks in order to avoid deadlock. 
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device context
 */
void SlimBus_BamDoDeferredWork(SlimBusDevCtxt *pDevCtxt)
{
  SlimBusBamCtxt *pCtxt;

  /* Rx completion processing */

  pCtxt = pDevCtxt->RxMsgFifo.pBamCtxt;

  if (NULL != pCtxt)
  {
    SlimBus_BamAcquireLock(pDevCtxt);
  
    while (pCtxt->uXferTail != pCtxt->uXferHead)
    {
      uint32 uXferTail = pCtxt->uXferTail;
      SlimBusBamXfer *pXfer = &pCtxt->pXferBuf[uXferTail];
      uint32 uDescSize, uLenMask;
      SlimBusMsgIdxType uDescIdx;
  
      if (TRUE == pXfer->bActive)
      {
        break;
      }
  
      pCtxt->uXferTail++;
      if (pCtxt->uXferTail >= pCtxt->uXferBufCnt)
      {
        pCtxt->uXferTail -= pCtxt->uXferBufCnt;
      }
  
      if ( 0 == pXfer->IoVec.buf_pa &&
           0 == pXfer->IoVec.buf_size )
      {
        /* If the buffer pointer and size are zero, then we must have gotten failure */
        continue;
      }
  
      uDescIdx = (uint32*)(pXfer->IoVec.buf_pa) - pCtxt->Obj.pMsgFifo->pPhysBuf;
      uDescSize = pXfer->IoVec.buf_size;
  
      uLenMask = pCtxt->Obj.pMsgFifo->uLen-1;
  
      SlimBus_NormalizeIndexMinMax(&uDescIdx, pCtxt->Obj.pMsgFifo->uHeadIdx,
                                   pCtxt->Obj.pMsgFifo->uAckIdx, pCtxt->Obj.pMsgFifo->uLen);
  
      /* ZI the remainder of the previous descriptor if the size received 
         is smaller than the size requested */
      while (pCtxt->Obj.pMsgFifo->uHeadIdx < uDescIdx)
      {
        pCtxt->Obj.pMsgFifo->pBuf[pCtxt->Obj.pMsgFifo->uHeadIdx&uLenMask] = 0;
        pCtxt->Obj.pMsgFifo->uHeadIdx++;
      }
      uDescIdx += (uDescSize+sizeof(uint32)-1)/sizeof(uint32);
      if ( uDescIdx > pCtxt->Obj.pMsgFifo->uHeadIdx )
      {
        pCtxt->Obj.pMsgFifo->uHeadIdx = uDescIdx;
      }
      if ( pCtxt->Obj.pMsgFifo->uHeadIdx > pCtxt->Obj.pMsgFifo->uAckIdx )
      {
        pCtxt->Obj.pMsgFifo->uHeadIdx = pCtxt->Obj.pMsgFifo->uAckIdx;
      }
    }
  
    SlimBus_BamReleaseLock(pDevCtxt);  
  }
}


/**
 * @brief Initialize a BAM context
 *
 * Initialize a BAM context structure
 * 
 * @param[in,out] ppCtxt  Pointer to a location to store the 
 *       BAM context pointer
 * @param[in] pDevCtxt  Pointer to the Slimbus device context
 * @param[in] pObj  Pointer to the context-specific data
 * @param[in] uPipeNum  BAM pipe number
 * @param[in] uMaxXfers  Maximum number of simultaneous BAM 
 *       transfers
 * 
 * @return SB_SUCCESS on success, error code on error
 */
static SBResult SlimBus_InitBamCtxt(SlimBusBamCtxt **ppCtxt, SlimBusDevCtxt *pDevCtxt, void *pObj, uint32 uPipeNum, uint32 uMaxXfers)
{
  if (NULL == ppCtxt)
  {
    return SB_ERROR;
  }
  
  if (NULL == *ppCtxt)
  {
    if (SB_SUCCESS != SlimBus_Malloc(sizeof(SlimBusBamCtxt), (void**)ppCtxt, NULL))
    {
      *ppCtxt = NULL;
      return SB_ERROR;
    }
    memset(*ppCtxt, 0, sizeof(SlimBusBamCtxt));
  }
  if ( NULL == (*ppCtxt)->pXferBuf ||
       uMaxXfers != (*ppCtxt)->uXferBufCnt )
  {
    if (NULL != (*ppCtxt)->pXferBuf)
    {
      SlimBus_Free((*ppCtxt)->pXferBuf);
    }
    if (SB_SUCCESS != SlimBus_Malloc(sizeof(SlimBusBamXfer)*uMaxXfers, 
                                     (void**)&(*ppCtxt)->pXferBuf, NULL))
    {
      SlimBus_Free(*ppCtxt);
      *ppCtxt = NULL;
      return SB_ERROR;
    }

    memset((*ppCtxt)->pXferBuf, 0, sizeof(SlimBusBamXfer)*uMaxXfers);
  }
  
  (*ppCtxt)->pDevCtxt = pDevCtxt;
  (*ppCtxt)->Obj.pVoid = pObj;
  (*ppCtxt)->PipeNum = uPipeNum;
  (*ppCtxt)->uXferBufCnt = uMaxXfers;
  (*ppCtxt)->uXferHead = (*ppCtxt)->uXferTail = 0;
  
  return SB_SUCCESS;
}

/**
 * @brief Free a BAM context
 *
 * This function frees a BAM context structure
 * 
 * @param[in,out] ppCtxt  Pointer to a pointer to the BAM 
 *       context
 * 
 * @return SB_SUCCESS on success, error code on error
 */
static SBResult SlimBus_FreeBamCtxt(SlimBusBamCtxt **ppCtxt)
{
  SlimBusBamCtxt *pCtxt;

  if (NULL == ppCtxt)
  {
    return SB_ERROR;
  }
  
  pCtxt = *ppCtxt;
  *ppCtxt = NULL;

  if (NULL != pCtxt)
  {
    if (NULL != pCtxt->pXferBuf)
    {
      SlimBus_Free(pCtxt->pXferBuf);
    }
    SlimBus_Free(pCtxt);
  }
  
  return SB_SUCCESS;
}

/**
 * @brief Initialize the BAM handle
 *
 * This function initializes the handle to the BAM driver
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SBResult SlimBus_BamInit(SlimBusDevCtxt *pDevCtxt)
{
  SBResult result = SB_SUCCESS;
  bam_callback_type bam_cb = { 0 };
  static bam_config_type bam_config = { 0 };

  if (NULL == pDevCtxt->Plat.hBamSync)
  {
    if (DAL_SUCCESS != 
        DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE_INTERRUPT, &pDevCtxt->Plat.hBamSync, NULL))
    {
      return SB_ERROR;
    }
  }
  
  bam_config.bam_pa = pDevCtxt->pBSP->hBamDev;
  bam_config.bam_va = pDevCtxt->hSB - (pDevCtxt->pBSP->uHwioBase - (uint32)pDevCtxt->pBSP->hBamDev);
  bam_config.bam_irq = pDevCtxt->pBSP->uBamIntId;
  bam_config.bam_irq_mask = BAM_IRQ_HRESP_ERR_EN|BAM_IRQ_ERR_EN;
  bam_config.sum_thresh = 32*1024;
  bam_config.options = 0;
  bam_cb.func = SlimBusBamCb;
  bam_cb.data = pDevCtxt;
  
  if (NULL == pDevCtxt->Plat.hBAM)
  {
    pDevCtxt->Plat.hBAM = bam_init(&bam_config, &bam_cb);
  }
  if (NULL == pDevCtxt->Plat.hBAM)
  {
    return SB_ERROR;
  }
  
  if (NULL == pDevCtxt->hBamDescMem)
  {
    
    result = SlimBus_PhysMemAlloc(pDevCtxt->numPorts*SB_BAM_PORT_DESC_SIZE,
                                  &pDevCtxt->hBamDescMem);
    
    if (SB_SUCCESS != result)
    {
      pDevCtxt->hBamDescMem = NULL;
      return result;
    }
  }
  return SB_SUCCESS;
}

/**
 * @brief De-initialize the BAM handle
 *
 * This function de-initializes the handle to the BAM driver
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SBResult SlimBus_BamDeinit(SlimBusDevCtxt *pDevCtxt)
{
  bam_status_type bam_result = BAM_SUCCESS;

  if (NULL != pDevCtxt->Plat.hBAM)
  {
    bam_result = bam_deinit(pDevCtxt->Plat.hBAM, TRUE);
    if (BAM_SUCCESS != bam_result)
    {
      SB_LOG_ERR_0("Error during bam_deinit");
    }
    pDevCtxt->Plat.hBAM = NULL;
  }

  if ( BAM_SUCCESS == bam_result )
  {
    return SB_SUCCESS;
  }
  else
  {
    return SB_ERROR;
  }
}

/**
 * @brief Allocate space the next available message in a 
 *        BAM-based Tx message FIFO
 *
 * This function allocates space for the next available message 
 * in a BAM-based Tx message FIFO.  Any processed BAM 
 * descriptors will be read out of the descriptor FIFO. 
 * 
 * @param[in] pClientCtxt  Pointer to the client context
 * @param[in] pDevice  Pointer to the Slimbus native device
 * @param[in] pMsgFifo  Pointer to the message FIFO
 * @param[in] uNumBytes  Number of bytes in the message to 
 *       allocate
 * 
 * @return Pointer to the space allocated for the message 
 */
static uint32 *SlimBus_GetTxBamMsgBuffer
(
  SlimBusClientCtxt *pClientCtxt,
  SlimBusDeviceType *pDevice,
  SlimBusMsgFifoType *pMsgFifo,
  uint32 uNumBytes
)
{
  SlimBusBamCtxt *pCtxt = pMsgFifo->pBamCtxt;

  if (!pMsgFifo->bBusy &&
      NULL != pCtxt)
  {  
    uint32 uTailIdx = pMsgFifo->uAckIdx;
    if (pMsgFifo->uHeadIdx + SB_MAX_MSG_LENGTH_WORDS >= uTailIdx + pMsgFifo->uLen)
    {
      while(TRUE)
      {
        bam_result_type result = {0};
        bam_pipe_poll(pCtxt->hBAM, &result);
        if (BAM_EVENT_INVALID == result.event)
        {
          break;
        }
        SlimBusBamTxHandler(&result, pCtxt);
      }
    }
  }

  return SlimBus_GetTxMsgBuffer(pClientCtxt, pDevice, pMsgFifo, uNumBytes);
}

/**
 * @brief Setup and configure message queues
 *
 * This function sets up and configures message queues, if 
 * message queue functionality is enabled. 
 * 
 * @param[in] pDevCtxt  Pointer to the SLIMbus device context
 * 
 * @return SB_SUCCESS if setup was successful, error code 
 *         otherwise
 */
SBResult SlimBus_SetupMsgQueues(SlimBusDevCtxt *pDevCtxt)
{
  SBResult result = SB_ERROR;
  SlimBusMsgIdxType uVirtAddr = 0, uPhysAddr = 0;
  uint32 uOffset = 0;

  result = SlimBus_BamInit(pDevCtxt);
  if (SLIMBUS_ERR_RETRY == result)
  {
    return result;
  }

  SlimBus_AllocMsgBuffers(pDevCtxt, &uOffset);
  
  if (pDevCtxt->bUseTxMsgQueue || pDevCtxt->bUseRxMsgQueue)
  {
    do
    {
      if ( NULL == pDevCtxt->hPhysMem || 
           NULL == pDevCtxt->Plat.hBAM )
      {
        result = SB_ERROR;
        break;
      }

      if (SB_SUCCESS != SlimBus_PhysMemInfo(pDevCtxt->hPhysMem, &uVirtAddr, &uPhysAddr))
      {
        result = SB_ERROR;
        break;
      }

      if (NULL == pDevCtxt->TxMsgFifo.pBamCtxt && pDevCtxt->bUseTxMsgQueue)
      {
        bam_pipe_config_type pipe_cfg = {0};
        SlimBusBamCtxt *pCtxt;
        uint32 uPipeNum = HAL_sb_DeviceMsgQGetPipeOffset(pDevCtxt->hSB,
                                                         pDevCtxt->eMsgDev,
                                                         HAL_SB_DEVICE_MSG_QUEUE_TX);
        pipe_cfg.options = BAM_O_DESC_DONE|BAM_O_ERROR;
        pipe_cfg.dir  = BAM_DIR_CONSUMER;
        pipe_cfg.mode = BAM_MODE_SYSTEM;
        pipe_cfg.desc_base_va = uVirtAddr + uOffset;
        pipe_cfg.desc_base_pa = uPhysAddr + uOffset;
        pipe_cfg.desc_size = SB_TX_DESC_FIFO_SIZE;
        pipe_cfg.evt_thresh = 1; /* this value ignored except for BAM-to-BAM */

        if (SB_SUCCESS != SlimBus_InitBamCtxt(&pDevCtxt->TxMsgFifo.pBamCtxt,
                                              pDevCtxt, &pDevCtxt->TxMsgFifo, uPipeNum,
                                              pipe_cfg.desc_size/sizeof(bam_iovec_type)))
        {
          result = SB_ERROR;
          break;
        }

        pCtxt = pDevCtxt->TxMsgFifo.pBamCtxt;

        pCtxt->hBAM = bam_pipe_init(pDevCtxt->Plat.hBAM, uPipeNum, &pipe_cfg, NULL);
        if (NULL == pCtxt->hBAM)
        {
          SlimBus_FreeBamCtxt(&pDevCtxt->TxMsgFifo.pBamCtxt);
          result = SB_ERROR;
          break;
        }
        
        pCtxt->bPoll = TRUE;
        if (BAM_SUCCESS != bam_pipe_setirqmode(pCtxt->hBAM, FALSE, (BAM_O_DESC_DONE|BAM_O_ERROR)))
        {
          bam_pipe_deinit(pCtxt->hBAM);
          SlimBus_FreeBamCtxt(&pDevCtxt->TxMsgFifo.pBamCtxt);
          result = SB_ERROR;
          break;
        }
      }
      uOffset += SB_TX_DESC_FIFO_SIZE;

      if (NULL == pDevCtxt->RxMsgFifo.pBamCtxt && pDevCtxt->bUseRxMsgQueue)
      {
        bam_pipe_config_type pipe_cfg = {0};
        bam_callback_type bam_cb = {0};
        SlimBusBamCtxt *pCtxt;
        uint32 uPipeNum = HAL_sb_DeviceMsgQGetPipeOffset(pDevCtxt->hSB,
                                                         pDevCtxt->eMsgDev,
                                                         HAL_SB_DEVICE_MSG_QUEUE_RX);
        pipe_cfg.options = BAM_O_DESC_DONE|BAM_O_ERROR;
        pipe_cfg.dir  = BAM_DIR_PRODUCER;
        pipe_cfg.mode = BAM_MODE_SYSTEM;
        pipe_cfg.desc_base_va = uVirtAddr + uOffset;
        pipe_cfg.desc_base_pa = uPhysAddr + uOffset;
        pipe_cfg.desc_size = SB_RX_DESC_FIFO_SIZE;
        pipe_cfg.evt_thresh = 1; /* evt_thresh is ignored except for BAM-to-BAM */
        
        if (SB_SUCCESS != SlimBus_InitBamCtxt(&pDevCtxt->RxMsgFifo.pBamCtxt,
                                              pDevCtxt, &pDevCtxt->RxMsgFifo, uPipeNum,
                                              pipe_cfg.desc_size/sizeof(bam_iovec_type)))
        {
          result = SB_ERROR;
          break;
        }

        pCtxt = pDevCtxt->RxMsgFifo.pBamCtxt;
        bam_cb.func = SlimBusBamRxCb;
        bam_cb.data = pCtxt;

        pCtxt->hBAM = bam_pipe_init(pDevCtxt->Plat.hBAM, uPipeNum, &pipe_cfg, &bam_cb);
        if (NULL == pCtxt->hBAM)
        {
          SlimBus_FreeBamCtxt(&pDevCtxt->RxMsgFifo.pBamCtxt);
          result = SB_ERROR;
          break;
        }
        
        pDevCtxt->RxMsgFifo.uAckIdx = pDevCtxt->RxMsgFifo.uHeadIdx;
        if (SlimBus_SubmitRxDescriptors(&pDevCtxt->RxMsgFifo) == 0)
        {
          result = SB_ERROR;
          break;
        }
      }
      uOffset += SB_RX_DESC_FIFO_SIZE;
    
      pDevCtxt->pDummyShiftMem = uPhysAddr + uOffset;
      uOffset += sizeof(uint32);
      
      if (pDevCtxt->bUseTxMsgQueue)
      {
        HAL_sb_DeviceMsgQEnable(pDevCtxt->hSB, pDevCtxt->eMsgDev,
                                HAL_SB_DEVICE_MSG_QUEUE_TX);
      
        pDevCtxt->TxMsgFifo.getBufFcn = SlimBus_GetTxBamMsgBuffer;
        pDevCtxt->TxMsgFifo.sendBufFcn = SlimBus_SendTxBamBuffer;
      }
    
      if (pDevCtxt->bUseRxMsgQueue)
      {
        pDevCtxt->RxMsgFifo.eFlags = SB_MSG_FIFO_USE_RX_OVERHANG;
      
        HAL_sb_DeviceMsgQEnable(pDevCtxt->hSB, pDevCtxt->eMsgDev,
                                HAL_SB_DEVICE_MSG_QUEUE_RX);
        /* Set timeout value to minimum */
        HAL_sb_DeviceRxMsgQSetTimeOutVal(pDevCtxt->hSB, pDevCtxt->eMsgDev, 1);
      }

      result = SB_SUCCESS;
    } while (FALSE);

    if (SB_SUCCESS != result)
    {
      SB_LOG_ERR_0("Disabling message queues due to BAM error");

      /* Set flag to FALSE if message queue setup failed */
      pDevCtxt->bUseTxMsgQueue = FALSE;
      pDevCtxt->bUseRxMsgQueue = FALSE;
    }
  }
  
  return result;
}

/**
 * @brief Transfer a single BAM IO vector
 *
 * This function transfers a single BAM IO vector
 * 
 * @param[in] pCtxt  Pointer to the BAM context
 * @param[in] uAddr  Physical start address for the transfer
 * @param[in] uSize  Size of the transfer in bytes
 * @param[in] pUser  User-defined pointer associated with the 
 *               transfer
 * @param[in] uFlags  Flags for the transfer
 * 
 * @return SB_SUCCESS on sucess, error code on error
 */
SBResult SlimBus_BamTransferIOVec
(
  SlimBusBamCtxt *pCtxt,
  SlimBusMsgIdxType uAddr,
  uint32 uSize,
  void *pUser,
  uint32 uFlags
)
{
  SlimBusBamXfer *pXfer;
  uint32 uXferIdx;
  SlimBusDevCtxt *pDevCtxt;

  if ( NULL == pCtxt ||
       NULL == pCtxt->pDevCtxt ||
       NULL == pCtxt->pXferBuf )
  {
    return SB_ERROR;
  }
  
  pDevCtxt = pCtxt->pDevCtxt;
  
  SlimBus_BamAcquireLock(pDevCtxt);
  
  if ( pCtxt->uXferHead + 1 == pCtxt->uXferTail ||
       pCtxt->uXferHead + 1 == pCtxt->uXferTail + pCtxt->uXferBufCnt )
  {
    /* Always return error code if there is no room for a new descriptor */
    SB_LOG_ERR_2("No room for new BAM descriptor xfer (head: 0x%lx) (tail: 0x%lx)",
                 pCtxt->uXferHead, pCtxt->uXferTail);
    SlimBus_BamReleaseLock(pDevCtxt);  
    return SLIMBUS_ERR_QUEUE_FULL;
  }

  if (pCtxt->bPoll)
  {
    /* If we are polling the pipe, we need to make sure the INT flag 
       is always enabled so that we can retrieve the descriptor later */
    uFlags |= SLIMBUS_BAM_IOVEC_FLAG_INT;
  }

  uXferIdx = pCtxt->uXferHead;
  pXfer = &pCtxt->pXferBuf[uXferIdx];
  pXfer->pCtxt = pCtxt;
  memset(&pXfer->IoVec, 0, sizeof(pXfer->IoVec));
  pXfer->IoVec.buf_pa = uAddr;
  pXfer->IoVec.buf_size = uSize;
  if (SLIMBUS_BAM_IOVEC_FLAG_INT & uFlags)
  {
    pXfer->IoVec.flags |= BAM_IOVEC_FLAG_INT;
  }
  if (SLIMBUS_BAM_IOVEC_FLAG_EOT & uFlags)
  {
    pXfer->IoVec.flags |= BAM_IOVEC_FLAG_EOT;
  }
  pXfer->pUser = pUser;
  pXfer->bActive = FALSE;

  /* Won't get notification that the transfer is compelete if no flags are set */
  if (uFlags != 0)
  {
    pXfer->bActive = TRUE;
    if (++pCtxt->uXferHead >= pCtxt->uXferBufCnt)
    {
      pCtxt->uXferHead -= pCtxt->uXferBufCnt;
    }
  }

  SlimBus_BamReleaseLock(pDevCtxt);  

  /* TODO: need to validate BAM enums equal SB enums */
  bam_status_type result = bam_pipe_transfer(pCtxt->hBAM, uAddr, uSize, pXfer->IoVec.flags, pXfer);
  if (BAM_SUCCESS == result)
  {
    return SB_SUCCESS;
  }
  else
  {
    SlimBus_BamAcquireLock(pDevCtxt);
    pXfer->bActive = FALSE;
    pXfer->IoVec.buf_pa = 0;
    pXfer->IoVec.buf_size = 0;
    SlimBus_BamReleaseLock(pDevCtxt);  
    
    return SB_ERROR;
  }
}

/**
 * @brief Get number of free IO vector slots
 *
 * This function retrieves the number of free BAM IO vector
 * slots
 * 
 * @param[in] pBamCtxt  Pointer to the BAM context
 * 
 * @return The number of free BAM IO vector slots
 */
uint32 SlimBus_BamGetIOVecFreeSlots
(
  SlimBusBamCtxt *pCtxt
)
{
  uint32 uXferIdx;
  uint32 uFreeCnt = 0;
  
  SlimBus_BamAcquireLock(pCtxt->pDevCtxt);

  uXferIdx = pCtxt->uXferHead;
  
  if ( uXferIdx == pCtxt->uXferTail )
  {
    uFreeCnt = pCtxt->uXferBufCnt-1;
  }
  else
  {
    while ( uXferIdx+1 != pCtxt->uXferTail && 
            uXferIdx+1 != pCtxt->uXferTail + pCtxt->uXferBufCnt )
    {
      uFreeCnt++;
      if (++uXferIdx >= pCtxt->uXferBufCnt)
      {
        uXferIdx -= pCtxt->uXferBufCnt;
      }
    }
  }
  
  SlimBus_BamReleaseLock(pCtxt->pDevCtxt);  

  return uFreeCnt;
}

/**
 * @brief Get number of active IO vector slots
 *
 * This function retrieves the number of active BAM IO vector
 * slots
 * 
 * @param[in] pBamCtxt  Pointer to the BAM context
 * 
 * @return The number of used BAM IO vector slots
 */
uint32 SlimBus_BamGetIOVecActiveSlots
(
  SlimBusBamCtxt *pCtxt
)
{
  uint32 uXferIdx;
  uint32 uActiveCnt = 0;
 
  SlimBus_BamAcquireLock(pCtxt->pDevCtxt);

  uXferIdx = pCtxt->uXferTail;
  
  while ( uXferIdx != pCtxt->uXferHead )
  {
    if ( pCtxt->pXferBuf[uXferIdx].bActive )
    {
      uActiveCnt++;
    }
    if (++uXferIdx >= pCtxt->uXferBufCnt)
    {
      uXferIdx -= pCtxt->uXferBufCnt;
    }
  }

  SlimBus_BamReleaseLock(pCtxt->pDevCtxt);  

  return uActiveCnt;
}

/**
 * @brief Drain the BAM descriptor FIFO of IO vectors
 *
 * This function drains the BAM desriptor FIFO of IO vectors
 * 
 * @param[in] pMsgFifo  Pointer to the message FIFO to drain of 
 *                 IO vectors
 * 
 * @return SB_SUCCESS on sucess, error code on error
 */
SBResult SlimBus_EmptyBamIOVec(SlimBusMsgFifoType *pMsgFifo)
{
  SlimBusBamCtxt *pCtxt = pMsgFifo->pBamCtxt;
  
  /* Only the Tx FIFO should use polling mode */  
  if ( NULL != pCtxt && pCtxt->bPoll )
  {
    while(TRUE)
    {
      bam_result_type result = {0};
      bam_pipe_poll(pCtxt->hBAM, &result);
      if (BAM_EVENT_INVALID == result.event)
      {
        break;
      }
      SlimBusBamTxHandler(&result, pCtxt);
    }
  }

  return SB_SUCCESS;
}

/**
  @brief Configure a BAM data pipe for a SLIMbus master port
  
  This function configures the BAM pipe for a SLIMbus master port to 
  source or sink data.  Half-duplex ports are treated logically 
  as a single port with both a source and sink pipe.  All other 
  ports are unidirectional and have either a source or sink 
  pipe. 

  @param[in] h  Client handle to the SLIMbus driver
  @param[in] hPort  Client handle to the master port 
  @param[in] eFlow  The pipe flow to configure, either source or 
        sink
  @param[in] pConfig  Pointer to the configuration to set for the BAM pipe

  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_ConfigMasterPipe(uint32 ClientId, DalDeviceHandle * h, SlimBusResourceHandle  hPort, SlimBusPortFlowType  eFlow, const SlimBusPipeConfigType * pConfig) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  uint32 idx;
  SlimBusMasterPortType *pPort = NULL;
  SBResult result = SB_SUCCESS;

  if (!pClientCtxt->bOpen ||
      NULL == pConfig ||
      SLIMBUS_PORT_NUM_ENUMS <= eFlow )
  {
    return SB_ERROR;
  }

  SlimBus_LockClient(pClientCtxt);
  SlimBus_LockDevice(pDevCtxt);

  if (eFlow == SLIMBUS_PORT_SOURCE_FLOW)
  {
    SB_LOG_VERB_2("Configure master pipe (client: 0x%lx) "
                  "(hport: 0x%lx) (flow: Source)", (uint32)pClientCtxt, hPort);
  }
  else
  {
    SB_LOG_VERB_2("Configure master pipe (client: 0x%lx) "
                  "(hport: 0x%lx) (flow: Sink)", (uint32)pClientCtxt, hPort);
  }

  do
  {
    idx = SB_GET_HANDLE_IDX(hPort);
    if (idx >= pDevCtxt->numPorts || 
        idx >= sizeof(pDevCtxt->Ports)/sizeof(pDevCtxt->Ports[0]) ||
        hPort != pDevCtxt->Ports[idx].handle)
    {
      result = SB_ERROR;
      break;
    }
    pPort = &pDevCtxt->Ports[idx];
    /* For half-duplex ports, determine which physical port we are trying
       to configure the pipe for */
    if ( pPort->eReqs & SLIMBUS_PORT_REQ_O_HALF_DUPLEX &&
         idx+1 < sizeof(pDevCtxt->Ports)/sizeof(pDevCtxt->Ports[0]) &&
         pPort->eFlow != eFlow )
    {
      pPort = &pDevCtxt->Ports[idx+1];
    }
    
    result = SlimBus_BamInit(pDevCtxt);
    if (SB_SUCCESS != result)
    {
      SB_LOG_ERR_2("Error initializing BAM handle during config pipe "
                   "(client: 0x%lx) (h: 0x%lx)", (uint32)pClientCtxt, hPort);
      break;
    }

    /* Wait for the port to disconnect if it is in the middle of disconnecting */
    if (pPort->eDisconnectState == SB_PORT_DISCONNECTING)
    {
      SB_LOG_INFO_3("Waiting for port disconnection during config pipe "
                    "(client: 0x%lx) (resource: 0x%lx) (port: %ld)",
                    (uint32)pClientCtxt, pPort->handle, pPort->portNum );

      pPort->bSignalDisconnect = TRUE;
      SlimBus_EventReset(pClientCtxt->hClientEvent);
      SlimBus_UnlockDevice(pDevCtxt);

      result = SlimBus_EventTimedWait(pClientCtxt,
                                      pClientCtxt->hClientEvent,
                                      SB_DEFAULT_CLIENT_TIMEOUT);
      SlimBus_LockDevice(pDevCtxt);
      if (SB_SUCCESS != result)
      {
        SB_LOG_WARN_3("Timeout waiting for port disconnection event"
                      " (client: 0x%lx) (resource: 0x%lx) (port: %ld)",
                      (uint32)pClientCtxt, pPort->handle, pPort->portNum );
      }
    }

    if ( pPort->bBamConnected )
    {
      SB_LOG_ERR_2("Change of BAM configuration for a connected port not allowed "
                   "(client: 0x%lx) (h: 0x%lx)", (uint32)pClientCtxt, hPort);
      result = SB_ERROR;
      break;
    }
    else
    {
      SlimBusMsgIdxType uVirtAddr = 0, uPhysAddr = 0;
      bam_pipe_config_type pipe_cfg = {0};
      bam_callback_type pipe_cb = {0};
      SlimBusBamCtxt *pCtxt;
      uint32 uPipeNum = 
        HAL_sb_PgdPortGetPipeNum(pDevCtxt->hSB, 0, pPort->portNum);
      
      if ( NULL == pDevCtxt->hBamDescMem )
      {
        result = SB_ERROR;
        break;
      }
      if (SB_SUCCESS != SlimBus_PhysMemInfo(pDevCtxt->hBamDescMem, &uVirtAddr, &uPhysAddr))
      {
        result = SB_ERROR;
        break;
      }

      if ( NULL == pConfig->DescFifo.pBase &&
           pConfig->DescFifo.uMinSize > SB_BAM_PORT_DESC_SIZE )
      {
        result = SB_ERROR;
        break;
      }
      
      SlimBus_PortAcquireLock(pDevCtxt, pPort);
      
      do
      {
        uint32 uDefaultOptions = BAM_O_DESC_DONE;
        
        pPort->eFlow = eFlow;
        pPort->eOptions = 0;

        if (SLIMBUS_BAM_O_DESC_DONE & pConfig->eOptions)
        {
          pipe_cfg.options |= BAM_O_DESC_DONE;
        }
        if (SLIMBUS_BAM_O_EOT & pConfig->eOptions)
        {
          pipe_cfg.options |= BAM_O_EOT;
        }
        if (0 != pipe_cfg.options)
        {
          pipe_cfg.options |= BAM_O_DESC_DONE;
        }
        if ( HAL_sb_IsPortDmaIntSupported(pDevCtxt->hSB) )
        {
          /* For combined DMA interrupt, want BAM interrupt disabled but able
             to retrieve processed descriptors via polling */
          pipe_cfg.options = 0;
        }
        if (eFlow == SLIMBUS_PORT_SOURCE_FLOW)
        {
          pipe_cfg.dir = BAM_DIR_CONSUMER;
        }
        else
        {
          pipe_cfg.dir = BAM_DIR_PRODUCER;
        }
        pipe_cfg.mode = BAM_MODE_SYSTEM;

        if (NULL != pConfig->DescFifo.pBase)
        {
          pipe_cfg.desc_base_va = (uint32)pConfig->DescFifo.pBase;
          pipe_cfg.desc_base_pa = pConfig->DescFifo.uPhysBase;
          pipe_cfg.desc_size = pConfig->DescFifo.uSize;
        }
        else
        {
          pipe_cfg.desc_base_va = uVirtAddr + (pPort->portNum-pDevCtxt->uBasePortNum)*SB_BAM_PORT_DESC_SIZE;
          pipe_cfg.desc_base_pa = uPhysAddr + (pPort->portNum-pDevCtxt->uBasePortNum)*SB_BAM_PORT_DESC_SIZE;
          pipe_cfg.desc_size = SB_BAM_PORT_DESC_SIZE;
        }
        pipe_cfg.evt_thresh = 1; /* evt_thresh is ignored except for BAM-to-BAM */

        if (SB_SUCCESS != SlimBus_InitBamCtxt(&pPort->pBamCtxt,
                                              pDevCtxt, pPort, uPipeNum,
                                              pipe_cfg.desc_size/sizeof(bam_iovec_type)))
        {
          result = SB_ERROR;
          break;
        }

        pCtxt = pPort->pBamCtxt;
        pipe_cb.func = SlimBusBamPipeCb;
        pipe_cb.data = pCtxt;

        pCtxt->hBAM = bam_pipe_init(pDevCtxt->Plat.hBAM, uPipeNum, &pipe_cfg, &pipe_cb);

        if (NULL == pCtxt->hBAM)
        {
          SlimBus_FreeBamCtxt(&pPort->pBamCtxt);
          SB_LOG_ERR_3("Failure to initialize BAM pipe "
                       "(client: 0x%lx) (hport: 0x%lx) (hpipe: 0x%lx)",
                       (uint32)pClientCtxt, hPort, (uint32)pPort->pBamCtxt);
          result = SB_ERROR;
          break;
        }
        if (0 == pipe_cfg.options)
        {
          pCtxt->bPoll = TRUE;

          if (BAM_SUCCESS != bam_pipe_setirqmode(pCtxt->hBAM, FALSE, uDefaultOptions))
          {
            bam_pipe_deinit(pCtxt->hBAM);
            SlimBus_FreeBamCtxt(&pPort->pBamCtxt);
            SB_LOG_ERR_3("Failure to set BAM pipe IRQ mode "
                         "(client: 0x%lx) (hport: 0x%lx) (hpipe: 0x%lx)",
                         (uint32)pClientCtxt, hPort, (uint32)pPort->pBamCtxt);
            result = SB_ERROR;
            break;
          }
        }

        pPort->bBamConnected = TRUE;
        HAL_sb_PgdPortEnable(pDevCtxt->hSB, 0, pPort->portNum);

        /* If 32bit datashift bit set to the port for the 
           HW bug(QCTDD03351991) we need to clear the 
           32-bit shift by submitting the dummy descriptor 
           of 4 bytes(32bit) buffer*/
        if ( eFlow == SLIMBUS_PORT_SINK_FLOW &&
             pPort->bDataShiftRemoval )
        {
          if (BAM_SUCCESS != bam_pipe_transfer(pPort->pBamCtxt->hBAM, pDevCtxt->pDummyShiftMem,
                                               sizeof(uint32), BAM_IOVEC_FLAG_INT,
                                               (void*)SLIMBUS_DATASHIFT_INVALIDATE))  
          {
            SB_LOG_ERR_1("Error returned from BAM pipe transfer to remove "
                         "the invalid data shift (port: %d)", pPort->portNum);
            result = SB_ERROR;
            break;
          }
          SB_LOG_INFO_1("Sumbitted dummy IO vector for hardware data shift "
                        "(port: %d)", pPort->portNum);
        }

      } while(FALSE);

      SlimBus_PortReleaseLock(pDevCtxt, pPort);
    }
  } while (FALSE);

  SlimBus_UnlockDevice(pDevCtxt);
  SlimBus_UnlockClient(pClientCtxt);

  return result;
}


/**
  @brief Register for asynchronous BAM event(s)

  This function registers for asynchronous BAM event(s).  If the 
  registered event is a callback event, a 
  SlimBusBamEventNotifyType structure will be passed back as the
  user data in the callback.  If the event is not a callback 
  event, the SlimBusBamEventNotifyType structure can be 
  retrieved by the DalSlimBus_GetBamEvent() function after the 
  event object has been signaled. 

  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hPort  Master port handle corresponding to the BAM 
        pipe
  @param[in] eTransferDir  Direction of data flow for the BAM 
        pipe to register for.  For bi-directional ports, there
        is one BAM pipe for each of the transmit and receive
        directions.
  @param[in] pReg  Pointer to location of BAM registration 
        structure
  
  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_RegisterBamEvent(uint32 ClientId, DalDeviceHandle * h, SlimBusResourceHandle  hPort, SlimBusBamTransferType  eTransferDir, SlimBusBamRegisterEventType * pReg) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  uint32 idx;
  SlimBusMasterPortType *pPort = NULL;
  SlimBusBamCtxt *pCtxt;
  SBResult result = SB_SUCCESS;

  if (!pClientCtxt->bOpen ||
      NULL == pReg ||
      SLIMBUS_BAM_NUM_ENUMS <= eTransferDir )
  {
    return SB_ERROR;
  }

  SlimBus_LockDevice(pDevCtxt);

  do
  {
    idx = SB_GET_HANDLE_IDX(hPort);
    if (idx >= pDevCtxt->numPorts || 
        idx >= sizeof(pDevCtxt->Ports)/sizeof(pDevCtxt->Ports[0]) ||
        hPort != pDevCtxt->Ports[idx].handle)
    {
      result = SB_ERROR;
      break;
    }
    pPort = &pDevCtxt->Ports[idx];
    if ( pPort->eReqs & SLIMBUS_PORT_REQ_O_HALF_DUPLEX &&
         idx+1 < sizeof(pDevCtxt->Ports)/sizeof(pDevCtxt->Ports[0]) )
    {
      if ( eTransferDir == SLIMBUS_BAM_RECEIVE )
      {
        pPort = &pDevCtxt->Ports[idx+1];
      }
      else if ( eTransferDir == SLIMBUS_BAM_DEFAULT )
      {
        /*error*/
        result = SB_ERROR;
        break;
      }
    }

    if (NULL == pPort->pBamCtxt)
    {
      /*error*/
      result = SB_ERROR;
      break;
    }

    pCtxt = pPort->pBamCtxt;
    
    if ( NULL != pCtxt->Obj.PortCtxt.hEvent &&
         NULL != pReg->hEvent &&
         pCtxt->Obj.PortCtxt.hEvent != pReg->hEvent )
    {
      /* Not able to change the event handle after port already started */
      result = SB_ERROR;
      break;
    }
    
    pCtxt->Obj.PortCtxt.hEvent = pReg->hEvent;
    pCtxt->Obj.PortCtxt.pUser = pReg->pUser;
  } while (FALSE);

  SlimBus_UnlockDevice(pDevCtxt);

  return result;
}


/**
  @brief Submit a new data transfer to the BAM descriptor FIFO 

  This function submits a new data transfer to the BAM 
  descriptor FIFO. 

  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hPort  Master port handle corresponding to the BAM 
        pipe
  @param[in] eTransferDir  Direction of data flow for the BAM 
        pipe to submit the transfer for.  For bi-directional
        ports, there is one BAM pipe for each of the transmit
        and receive directions.
  @param[in] pIOVec  Pointer to the IO-vector for the data 
        transfer
  @param[in] pUser   User pointer that will be passed back 
        during an asynchronous BAM callback event for the
        descriptor
   
  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_SubmitBamTransfer(uint32 ClientId, DalDeviceHandle * h, SlimBusResourceHandle  hPort, SlimBusBamTransferType  eTransferDir, SlimBusBamIOVecType * pIOVec, void * pUser) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  uint32 idx;
  SlimBusMasterPortType *pPort = NULL;
  SlimBusEventHandle hEvent = NULL;
  void *pUserData = NULL;
  SlimBusBamCtxt *pCtxt;
  SlimBusBamXfer *pXfer;
  uint32 uXferIdx;
  SBResult result = SB_SUCCESS;
  bam_status_type bam_result = {0};
  SlimBusChannelType *pChannel = NULL;
  const char *pszErrStr = NULL;

  if (!pClientCtxt->bOpen ||
      NULL == pIOVec ||
      SLIMBUS_BAM_NUM_ENUMS <= eTransferDir )
  {
    return SB_ERROR;
  }

  idx = SB_GET_HANDLE_IDX(hPort);
    
  if (idx >= pDevCtxt->numPorts ||
      idx >= sizeof(pDevCtxt->Ports)/sizeof(pDevCtxt->Ports[0]))
  {
    return SB_ERROR;
  }
  pPort = &pDevCtxt->Ports[idx];
  if ( pPort->eReqs & SLIMBUS_PORT_REQ_O_HALF_DUPLEX &&
       idx+1 < sizeof(pDevCtxt->Ports)/sizeof(pDevCtxt->Ports[0]) )
  {
    if ( eTransferDir == SLIMBUS_BAM_RECEIVE )
    {
      pPort = &pDevCtxt->Ports[idx+1];
    }
    else if ( eTransferDir == SLIMBUS_BAM_DEFAULT )
    {
      return SB_ERROR;
    }
  }

  SlimBus_PortAcquireLock(pDevCtxt, pPort);

  do
  {
    if (hPort != pPort->handle)
    {
      result = SB_ERROR;
      break;
    }

    pChannel = pPort->aEndPoints[0].pChannel;
    if (NULL == pPort->pBamCtxt)
    {
      pszErrStr = "Attempt to submit descriptors when BAM handle is NULL";
      result = SB_ERROR;
      break;
    }

    if ( !pPort->bBamConnected )
    {
      pszErrStr = "Attempt to submit descriptors when BAM is not connected";
      result = SB_ERROR;
      break;
    }

    if ( NULL != pChannel &&
         SLIMBUS_CHANNEL_STATUS_REMOVED != pChannel->eCurStatus &&
         SLIMBUS_CHANNEL_STATUS_REMOVED == pChannel->eNewStatus )
    {
      pszErrStr = "Attempt to submit descriptors when port channel is being removed";
      result = SB_ERROR;
      break;
    }

    pCtxt = pPort->pBamCtxt;

    SlimBus_BamAcquireLock(pDevCtxt);

    if ( pCtxt->uXferHead + 1 == pCtxt->uXferTail ||
         pCtxt->uXferHead + 1 == pCtxt->uXferTail + pCtxt->uXferBufCnt )
    {
      pszErrStr = "No room for new port BAM descriptor xfer";
      result = SB_ERROR;
      SlimBus_BamReleaseLock(pDevCtxt);  
      break;
    }

    uXferIdx = pCtxt->uXferHead;
    pXfer = &pCtxt->pXferBuf[uXferIdx];
    pXfer->pCtxt = pCtxt;
    memset(&pXfer->IoVec, 0, sizeof(pXfer->IoVec));
    pXfer->IoVec.buf_pa = pIOVec->uAddr;
    pXfer->IoVec.buf_size = pIOVec->uSize;
    pXfer->IoVec.flags = 0;
    if ( pIOVec->uFlags & SLIMBUS_BAM_IOVEC_FLAG_INT )
    {
      pXfer->IoVec.flags |= BAM_IOVEC_FLAG_INT;
    }
    if ( pIOVec->uFlags & SLIMBUS_BAM_IOVEC_FLAG_EOT )
    {
      pXfer->IoVec.flags |= BAM_IOVEC_FLAG_EOT;
    }
    pXfer->pUser = pUser;
    pXfer->bActive = FALSE;

    if (0 == pXfer->IoVec.flags)
    {
      pXfer = NULL;
    }

    if (NULL != pXfer)
    {
      pXfer->bActive = TRUE;
      if (++pCtxt->uXferHead >= pCtxt->uXferBufCnt)
      {
        pCtxt->uXferHead -= pCtxt->uXferBufCnt;
      }
    }

    SlimBus_BamReleaseLock(pDevCtxt);  

    bam_result = bam_pipe_transfer(pCtxt->hBAM, pIOVec->uAddr, pIOVec->uSize,
                                   ((pXfer!=NULL)?pXfer->IoVec.flags:0), pXfer);
    if (BAM_SUCCESS != bam_result)
    {
      SlimBus_BamAcquireLock(pDevCtxt);

      if (NULL != pXfer)
      {
        pXfer->bActive = FALSE;
        pXfer->IoVec.buf_pa = 0;
        pXfer->IoVec.buf_size = 0;
      }

      SlimBus_BamReleaseLock(pDevCtxt);  

      pszErrStr = "Error returned from BAM pipe transfer";
      result = SB_ERROR;
      break;
    }

    /* Increment our internal counters for how many descriptors and bytes
       have been submitted (for debug) */
    pPort->uDescSubmit++;
    pPort->uBytesSubmit += pIOVec->uSize;

  } while(FALSE);

  SlimBus_PortReleaseLock(pDevCtxt, pPort);

  if (NULL != pszErrStr)
  {
    SlimBus_LockDevice(pDevCtxt);
    SB_LOG_ERR_4("%s (client: 0x%lx) (portres: 0x%lx) (chanres: 0x%lx)", pszErrStr,
                 (uint32)pClientCtxt, pPort->handle, ((NULL!=pChannel)?pChannel->handle:0));
    SlimBus_UnlockDevice(pDevCtxt);
  }
  
  /* If the interrupt is disabled, then it means an underflow/overflow 
     condition has previously occurred and we should re-enable the interrupt
     once the condition is resolved */    
  if (SB_SUCCESS == result)
  {
    uint32 uIntMask = (1<<pPort->portNum);
    uint32 uEnMask = HAL_sb_PgdPortGetIntEnEe(pDevCtxt->hSB, 0, pDevCtxt->pBSP->uMyEE);
    HAL_sb_PgdPortEventType eEvents = HAL_sb_PgdPortGetEvents(pDevCtxt->hSB, 0, pPort->portNum);
    uint32 uToken;
    
    if ( !(uIntMask & uEnMask) ||
          (eEvents & HAL_SB_PGD_PORT_UNDERFLOW_EVENT) ||
          (eEvents & HAL_SB_PGD_PORT_OVERFLOW_EVENT) )
    {
      SlimBus_InterruptAcquireLock(pDevCtxt, &uToken);
      
      MSG_3(MSG_SSID_QDSP6, MSG_LEGACY_HIGH,
            "[WARN] Detected port overflow/underflow during submit transfer "
            "(client: 0x%lx) (port: %ld) (bIntDis: %d)",
            pClientCtxt, pPort->portNum, (uIntMask & uEnMask));

      uEnMask = HAL_sb_PgdPortGetIntEnEe(pDevCtxt->hSB, 0, pDevCtxt->pBSP->uMyEE);
      eEvents = HAL_sb_PgdPortGetEvents(pDevCtxt->hSB, 0, pPort->portNum);

      if ( ( (eEvents & HAL_SB_PGD_PORT_UNDERFLOW_EVENT) ||
             (eEvents & HAL_SB_PGD_PORT_OVERFLOW_EVENT) ) &&
           !(eEvents & HAL_SB_PGD_PORT_DMA_EVENT) &&
           !(eEvents & HAL_SB_PGD_PORT_DISCONNECT_EVENT) )
      {
        /* Clear the interrupt to make sure that port will
           restart transfers to/from the port FIFO */
        HAL_sb_PgdPortClearIntEe(pDevCtxt->hSB, 0, pDevCtxt->pBSP->uMyEE, uIntMask);
      }
      
      if ( !(uIntMask & uEnMask) )
      {
        /* Re-enable the port interrupt */
        uEnMask |= uIntMask;
        HAL_sb_PgdPortSetIntEnEe(pDevCtxt->hSB, 0, pDevCtxt->pBSP->uMyEE, uEnMask);
      }

      SlimBus_InterruptReleaseLock(pDevCtxt, &uToken);
    }
  }
  
  if (NULL != hEvent)
  {
    /* Signal event here to avoid potential deadlock */
    SlimBus_EventTriggerEx(hEvent, pUserData);
  }

  return result;
}


/**
  @brief Get a processed BAM IO-vector with user data

  This function fetches the next processed BAM IO-vector.  The 
  IO-vector structure will be zeroed if there are no more 
  processed IO-vectors.  The user data pointer is optional and 
  if supplied will be filled with the user data pointer passed 
  when the descriptor was submitted. 

  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hPort  Master port handle corresponding to the BAM 
        pipe
  @param[in] eTransferDir  Direction of data flow for the BAM
        pipe to get the IO-vector for.  For bi-directional
        ports, there is one BAM pipe for each of the transmit
        and receive directions.
  @param[out] pIOVec  Pointer to the location to store a 
        processed IO-vector.  The structure will be zeroed if
        there are no more processed IO-vectors.
  @param[out] ppUser  Pointer to location to store the user data 
        pointer associated with a processed IO-vector.  The
        pointer will be NULL if there are no more processed
        IO-vectors.
   
  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult
SlimBus_GetBamIOVecEx(uint32 ClientId, DalDeviceHandle * h, SlimBusResourceHandle  hPort, SlimBusBamTransferType  eTransferDir, SlimBusBamIOVecType * pIOVec, void **ppUser)
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  uint32 idx;
  SlimBusMasterPortType *pPort = NULL;
  SlimBusBamCtxt *pCtxt;
  SlimBusBamXfer *pXfer;
  uint32 uXferIdx;
  SBResult result = SB_SUCCESS;

  if (!pClientCtxt->bOpen ||
      NULL == pIOVec ||
      SLIMBUS_BAM_NUM_ENUMS <= eTransferDir )
  {
    return SB_ERROR;
  }

  idx = SB_GET_HANDLE_IDX(hPort);
  if (idx >= pDevCtxt->numPorts ||
      idx >= sizeof(pDevCtxt->Ports)/sizeof(pDevCtxt->Ports[0]))
  {
    return SB_ERROR;
  }
  pPort = &pDevCtxt->Ports[idx];
  if ( pPort->eReqs & SLIMBUS_PORT_REQ_O_HALF_DUPLEX &&
       idx+1 < sizeof(pDevCtxt->Ports)/sizeof(pDevCtxt->Ports[0]) )
  {
    if ( eTransferDir == SLIMBUS_BAM_RECEIVE )
    {
      pPort = &pDevCtxt->Ports[idx+1];
    }
    else if ( eTransferDir == SLIMBUS_BAM_DEFAULT )
    {
      return SB_ERROR;
    }
  }

  SlimBus_PortAcquireLock(pDevCtxt, pPort);

  do
  {
    if (hPort != pPort->handle)
    {
      result = SB_ERROR;
      break;
    }

    pCtxt = pPort->pBamCtxt;
    if (NULL == pCtxt)
    {
      result = SB_ERROR;
      break;
    }

    if ( pCtxt->bPoll &&
         pCtxt->pXferBuf[pCtxt->uXferTail].bActive )
    {
      bam_result_type result = {0};

      bam_pipe_poll(pCtxt->hBAM, &result);

      if (BAM_EVENT_INVALID != result.event)
      {
        SlimBus_BamAcquireLock(pDevCtxt);
        SlimBusBamPipeHandler(&result, pCtxt, NULL);
        SlimBus_BamReleaseLock(pDevCtxt);  
      }
    }
    
    SlimBus_BamAcquireLock(pDevCtxt);
    
    do
    {
      if (pCtxt->uXferTail == pCtxt->uXferHead)
      {
        result = SB_ERROR;
        break;
      }
      
      uXferIdx = pCtxt->uXferTail;
      pXfer = &pCtxt->pXferBuf[uXferIdx];
      
      if ( pXfer->bActive )
      {
        result = SB_ERROR;
        break;
      }
      
      if (++pCtxt->uXferTail >= pCtxt->uXferBufCnt)
      {
        pCtxt->uXferTail -= pCtxt->uXferBufCnt;
      }

      pIOVec->uAddr = pXfer->IoVec.buf_pa;
      pIOVec->uSize = pXfer->IoVec.buf_size;
      pIOVec->uFlags = 0;
      if (BAM_IOVEC_FLAG_INT & pXfer->IoVec.flags)
      {
        pIOVec->uFlags |= SLIMBUS_BAM_IOVEC_FLAG_INT;
      }
      if (BAM_IOVEC_FLAG_EOT & pXfer->IoVec.flags)
      {
        pIOVec->uFlags |= SLIMBUS_BAM_IOVEC_FLAG_EOT;
      }
      if (NULL != ppUser)
      {
        *ppUser = pXfer->pUser;
      }
    } while (FALSE);

    SlimBus_BamReleaseLock(pDevCtxt);  
  } while (FALSE);

  SlimBus_PortReleaseLock(pDevCtxt, pPort);

  return result;
}


/**
  @brief Get a processed BAM IO-vector 

  This function fetches the next processed BAM IO-vector.  The 
  IO-vector structure will be zeroed if there are no more 
  processed IO-vectors. 

  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hPort  Master port handle corresponding to the BAM 
        pipe
  @param[in] eTransferDir  Direction of data flow for the BAM
        pipe to get the IO-vector for.  For bi-directional
        ports, there is one BAM pipe for each of the transmit
        and receive directions.
  @param[out] pIOVec  Pointer to the location to store a 
        processed IO-vector.  The structure will be zeroed if
        there are no more processed IO-vectors.
   
  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_GetBamIOVec(uint32 ClientId, DalDeviceHandle * h, SlimBusResourceHandle  hPort, SlimBusBamTransferType  eTransferDir, SlimBusBamIOVecType * pIOVec) 
{
  return SlimBus_GetBamIOVecEx(ClientId, h, hPort, eTransferDir, pIOVec, NULL);
}


/**
  @brief Read the event queue for a BAM pipe

  This function reads the event queue for a BAM pipe.  Events 
  will be added to the queue when they have been registered 
  using a non-callback trigger. 

  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hPort  Master port handle corresponding to the BAM 
        pipe
  @param[in] eTransferDir  Direction of data flow for the BAM
        pipe to read the event queue.  For bi-directional ports,
        there is one BAM pipe for each of the transmit and
        receive directions.
  @param[out] pEvent  Pointer to the location to store the event
        read from the queue.
   
  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_GetBamEvent(uint32 ClientId, SlimBusHandle h, SlimBusResourceHandle  hPort, SlimBusBamTransferType  eTransferDir, SlimBusBamEventType *peEvent) 
{
  /* Deprecated */

  UNREFERENCED_PARAMETER(h);
  UNREFERENCED_PARAMETER(hPort);
  UNREFERENCED_PARAMETER(eTransferDir);
  UNREFERENCED_PARAMETER(peEvent);

  return SB_ERROR;
}

/**
 * @brief Reset the BAM hardware
 *
 * This function resets the BAM hardware
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device 
 *       structure
 * 
 * @return SB_SUCCESS on sucess, error code on error 
 */
SBResult SlimBus_ResetBAM(SlimBusDevCtxt *pDevCtxt)
{
  bam_reset(pDevCtxt->Plat.hBAM);

  return SB_SUCCESS;
}

/**
 * @brief Detach from the BAM driver
 *
 * This function detaches from the BAM driver
 * 
 * @param[in,out] ppBamCtxt  Pointer to the pointer to the BAM 
 *       context
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SBResult SlimBus_DetachBAM(SlimBusBamCtxt **ppCtxt)
{
  bam_status_type result = BAM_SUCCESS;
  SlimBusDevCtxt *pDevCtxt;
  bam_handle hBAM;

  if (NULL == ppCtxt ||
      NULL == (*ppCtxt) ||
      NULL == (*ppCtxt)->pDevCtxt )
  {
    return SB_ERROR;
  }
  
  pDevCtxt = (*ppCtxt)->pDevCtxt;
    
  SlimBus_BamAcquireLock(pDevCtxt);

  hBAM = (*ppCtxt)->hBAM;
  SlimBus_FreeBamCtxt(ppCtxt);

  SlimBus_BamReleaseLock(pDevCtxt);  

  if (NULL != hBAM)
  {
    result = bam_pipe_deinit(hBAM);
  }

  if (BAM_SUCCESS == result)
  {
    return SB_SUCCESS;
  }
  else
  {
    return SB_ERROR;
  }
}


/**
 * @brief Disconnect the BAM port
 *
 * This function disconnects the Slimbus BAM port
 * 
 * @param[in] pPort  Pointer to the master port structure
 */
void SlimBus_DisconnectBAM(SlimBusMasterPortType *pPort)
{
  SlimBusBamCtxt *pCtxt;
  bam_handle hBAM;
  
  pCtxt = pPort->pBamCtxt;

  if (NULL != pCtxt &&
      NULL != pCtxt->pDevCtxt)
  {
    SlimBusDevCtxt *pDevCtxt = pCtxt->pDevCtxt;

    SlimBus_BamAcquireLock(pDevCtxt);

    hBAM = pCtxt->hBAM;
    pCtxt->hBAM = NULL;
    pCtxt->uXferHead = pCtxt->uXferTail = 0;

    SlimBus_BamReleaseLock(pDevCtxt);  

    if (NULL != hBAM)
    {
      bam_pipe_deinit(hBAM);
    }
  }
  
  pPort->bBamConnected = FALSE;
}

/**
 * @brief Unregister a BAM port event
 *
 * This function unregisters a BAM port event
 * 
 * @param[in] pPort  Pointer to the master port structure
 */
void SlimBus_UnregisterBamPortEvent(SlimBusMasterPortType *pPort)
{
  SlimBusBamCtxt *pCtxt;

  pCtxt = pPort->pBamCtxt;

  if (NULL != pCtxt)
  {
    SlimBusDevCtxt *pDevCtxt = pCtxt->pDevCtxt;

    SlimBus_BamAcquireLock(pDevCtxt);

    pCtxt->Obj.PortCtxt.hEvent = NULL;
    pCtxt->Obj.PortCtxt.pUser = NULL;

    SlimBus_BamReleaseLock(pDevCtxt);  
  }
}

/**
 * @brief Clear port Bamhandle 
 *
 * This function update the BAM handle to NULL 
 * 
 * @param[in] pPort  Pointer to the master port structure
 */
void SlimBus_ClearBamhandle(SlimBusMasterPortType *pPort)
{
  SlimBusBamCtxt *pCtxt;

  pCtxt = pPort->pBamCtxt;

  if (NULL != pCtxt)
  {
    pCtxt->hBAM = NULL;
  }
}

