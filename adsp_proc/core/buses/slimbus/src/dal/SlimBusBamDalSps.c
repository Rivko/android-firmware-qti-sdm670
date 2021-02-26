/*
===========================================================================

FILE:         SlimBusBamDalSps.c

DESCRIPTION:  This file implements the SLIMbus core driver BAM functionality
              via DAL SPS.

===========================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1/buses/slimbus/src/dal/SlimBusBamDalSps.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
06/08/15   NSD     Replace SlimBus_EventWait() calls with SlimBus_EventTimedWait().
05/16/12   MJS     Updated logging macros.
01/16/12   MJS     Change ResetBAM() parameters.
12/07/11   MJS     Initial revision.

===========================================================================
             Copyright (c) 2011, 2012, 2015 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

#include "SlimBusBam.h"


/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/

SBResult SlimBus_BamInit(SlimBusDevCtxt *pDevCtxt)
{
  if (NULL == pDevCtxt->Plat.hBAM)
  {
    if (DAL_SUCCESS != DAL_SpsDeviceAttach(DALDEVICEID_SPS, &pDevCtxt->Plat.hBAM))
    {
      return SB_ERROR;
    }
  }
  
  return SB_SUCCESS;
}

/**
 * @brief Attach to the BAM driver
 *
 * This function attaches to the BAM driver
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * @param[out] ppBamCtxt  Pointer to the location to store the 
 *       BAM handle
 * 
 * @return SB_SUCCESS on sucess, error code on error
 */
static SBResult SlimBus_ConnectBAM(SlimBusDevCtxt *pDevCtxt, SlimBusBamCtxt **ppBamCtxt)
{
  return DAL_SpsDeviceAttach(DALDEVICEID_SPS, ppBamCtxt);
}

/**
 * @brief Setup and configure message queues
 *
 * This function sets up and configures message queues, if 
 * message queue functionality is enabled. 
 * 
 * @param[in] pDevCtxt  Pointer to the SLIMbus DAL device
 * 
 * @return SB_SUCCESS if setup was successful, error code 
 *         otherwise
 */
SBResult SlimBus_SetupMsgQueues(SlimBusDevCtxt *pDevCtxt)
{
  DAL_sps_ConnectType Connect;
  DAL_sps_RegisterEventType Reg;
  SBResult result = SB_ERROR;
  uint32 uVirtAddr = 0, uPhysAddr = 0;
  uint32 uOffset = 0;

  SlimBus_BamInit(pDevCtxt);
  
  SlimBus_AllocMsgBuffers(pDevCtxt, &uOffset);
  
  if (pDevCtxt->bUseTxMsgQueue || pDevCtxt->bUseRxMsgQueue)
  {
    do
    {
      if ( NULL == pDevCtxt->hPhysMem )
      {
        break;
      }

      SlimBus_PhysMemInfo(pDevCtxt->hPhysMem, &uVirtAddr, &uPhysAddr);

      if (NULL == pDevCtxt->hBamEvent)
      {
        result = SlimBus_CreateCallbackEvent(&pDevCtxt->hBamEvent, NULL, SlimBusMsgQueueCb, pDevCtxt);
        if (SB_SUCCESS != result)
        {  
          break;
        }
      }
    
      if (NULL == pDevCtxt->TxMsgFifo.pBamCtxt && pDevCtxt->bUseTxMsgQueue)
      {
        result = SlimBus_ConnectBAM(pDevCtxt, &pDevCtxt->TxMsgFifo.pBamCtxt);
        if (SB_SUCCESS != result)
        {  
          break;
        }

        result = DalSps_GetConfig(pDevCtxt->TxMsgFifo.pBamCtxt, &Connect);
        if (SB_SUCCESS != result)
        {
          break;
        }
        Connect.eSource = SPS_DEV_HANDLE_MEM;
        Connect.uSrcPipeIndex = 0;
        Connect.eDestination = pDevCtxt->pBSP->hBamDev;
        Connect.uDestPipeIndex = 
          HAL_sb_DeviceMsgQGetPipeOffset(pDevCtxt->hSB, pDevCtxt->eMsgDev,
                                         HAL_SB_DEVICE_MSG_QUEUE_TX);
        Connect.eMode = SPS_MODE_DEST;
        Connect.eOptions = (DAL_sps_OptionType)(SPS_O_ERROR|SPS_O_NO_Q|SPS_O_ACK_TRANSFERS|SPS_O_AUTO_ENABLE);
        Connect.eConfig = SPS_CONFIG_DEFAULT;
        Connect.Desc.pBase = (void*)(uVirtAddr + uOffset);
        Connect.Desc.uPhysBase = (uPhysAddr + uOffset);
        Connect.Desc.uSize = SB_TX_DESC_FIFO_SIZE;
        uOffset += SB_TX_DESC_FIFO_SIZE;

        result = DalSps_Connect(pDevCtxt->TxMsgFifo.pBamCtxt, &Connect);
        if (SB_SUCCESS != result)
        {
          break;
        }

        Reg.eMode = SPS_TRIGGER_CALLBACK;
        Reg.eOptions = SPS_O_ERROR;
        Reg.hEvent = pDevCtxt->hBamEvent;
        Reg.pUser = (void*)FALSE;
        result = DalSps_RegisterEvent(pDevCtxt->TxMsgFifo.pBamCtxt, &Reg);
        if (SB_SUCCESS != result)
        {
          break;
        }

      }

      if (NULL == pDevCtxt->RxMsgFifo.pBamCtxt && pDevCtxt->bUseRxMsgQueue)
      {
        result = SlimBus_ConnectBAM(pDevCtxt, &pDevCtxt->RxMsgFifo.pBamCtxt);
        if (SB_SUCCESS != result)
        {  
          break;
        }

        result = DalSps_GetConfig(pDevCtxt->RxMsgFifo.pBamCtxt, &Connect);
        if (SB_SUCCESS != result)
        {
          break;
        }
        Connect.eSource = pDevCtxt->pBSP->hBamDev;
        Connect.uSrcPipeIndex = 
          HAL_sb_DeviceMsgQGetPipeOffset(pDevCtxt->hSB, pDevCtxt->eMsgDev,
                                         HAL_SB_DEVICE_MSG_QUEUE_RX);
        Connect.eDestination = SPS_DEV_HANDLE_MEM;
        Connect.uDestPipeIndex = 0;
        Connect.eMode = SPS_MODE_SRC;
        Connect.eOptions = (DAL_sps_OptionType)(SPS_O_DESC_DONE|SPS_O_ERROR|SPS_O_AUTO_ENABLE);
        Connect.eConfig = SPS_CONFIG_DEFAULT;
        Connect.Desc.pBase = (void*)(uVirtAddr + uOffset);
        Connect.Desc.uPhysBase = (uPhysAddr + uOffset);
        Connect.Desc.uSize = SB_RX_DESC_FIFO_SIZE;
      
        result = DalSps_Connect(pDevCtxt->RxMsgFifo.pBamCtxt, &Connect);
        if (SB_SUCCESS != result)
        {
          break;
        }
        
        /* Note: for now have to use synchronous event instead of callback
                 to avoid deadlock issue between BAM and Slimbus mutexes */
        Reg.eMode = SPS_TRIGGER_WAIT;
        Reg.eOptions = (DAL_sps_OptionType) (SPS_O_DESC_DONE|SPS_O_ERROR);
        Reg.hEvent = pDevCtxt->hMsgWorkLoopEvent;
        Reg.pUser = (void*)TRUE;
        result = DalSps_RegisterEvent(pDevCtxt->RxMsgFifo.pBamCtxt, &Reg);
        if (SB_SUCCESS != result)
        {
          break;
        }

        pDevCtxt->RxMsgFifo.uAckIdx = pDevCtxt->RxMsgFifo.uHeadIdx;
        if (SlimBus_SubmitRxDescriptors(&pDevCtxt->RxMsgFifo) == 0)
        {
          result = SB_ERROR;
          break;
        }
      }
    
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
  
  return SB_SUCCESS;
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
  DAL_sps_IOVecType IOVec;
  SBResult result;
  
  if (!pMsgFifo->bBusy)
  {  
    uint32 uTailIdx = pMsgFifo->uAckIdx;
    if (pMsgFifo->uHeadIdx + SB_MAX_MSG_LENGTH_WORDS >= uTailIdx + pMsgFifo->uLen)
    {
      result = DalSps_GetIOVec(pMsgFifo->pBamCtxt, &IOVec);
      while ( SB_SUCCESS == result && 0 != IOVec.uAddr )
      {
        pMsgFifo->uAckIdx += IOVec.uSize/sizeof(uint32);
        result = DalSps_GetIOVec(pMsgFifo->pBamCtxt, &IOVec);
      }
    }
  }

  return SlimBus_GetTxMsgBuffer(pClientCtxt, pDevice, pMsgFifo, uNumBytes);
}

/**
 * @brief Transfer a single BAM IO vector
 *
 * This function transfers a single BAM IO vector
 * 
 * @param pBamCtxt  Pointer to the BAM context
 * @param uAddr  Physical start address for the transfer
 * @param uSize  Size of the transfer in bytes
 * @param pUser  User-defined pointer associated with the 
 *               transfer
 * @param uFlags  Flags for the transfer
 * 
 * @return SB_SUCCESS on sucess, error code on error
 */
SBResult SlimBus_BamTransferIOVec(SlimBusBamCtxt *pBamCtxt,  uint32  uAddr,  uint32  uSize,  void * pUser,  uint32  uFlags)
{
  return DalSps_TransferOne(pBamCtxt, uAddr, uSize, pUser, uFlags);
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
  DAL_sps_IOVecType IOVec;
  SBResult result;

  /* Empty the BAM IOVec if we're using queues */
  result = DalSps_GetIOVec(pMsgFifo->pBamCtxt, &IOVec);
  while ( SB_SUCCESS == result && 0 != IOVec.uAddr )
  {
    result = DalSps_GetIOVec(pMsgFifo->pBamCtxt, &IOVec);
  }
  
  return result;
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
SlimBus_ConfigMasterPipe( DalDeviceHandle * h, SlimBusResourceHandle  hPort, SlimBusPortFlowType  eFlow, const SlimBusPipeConfigType * pConfig) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  uint32 idx;
  SlimBusMasterPortType *pPort = NULL;
  DAL_sps_ConnectType Connect;
  SBResult result = SB_SUCCESS;

  if (!pClientCtxt->bOpen ||
      NULL == pConfig ||
      SLIMBUS_PORT_NUM_ENUMS <= eFlow )
  {
    return SB_ERROR;
  }

  SlimBus_LockClient(pClientCtxt);
  SlimBus_LockDevice(pDevCtxt);

  SB_LOG_VERB_3("Configure master pipe (client: 0x%08x) "
                "(hport: 0x%08x) (flow: %d)", pClientCtxt, hPort, eFlow);

  do
  {
    idx = SB_GET_HANDLE_IDX(hPort);
    if (idx >= pDevCtxt->numPorts || hPort != pDevCtxt->Ports[idx].handle)
    {
      result = SB_ERROR;
      break;
    }
    pPort = &pDevCtxt->Ports[idx];
    /* For half-duplex ports, determine which physical port we are trying
       to configure the pipe for */
    if ( pPort->eReqs & SLIMBUS_PORT_REQ_O_HALF_DUPLEX &&
         pPort->eFlow != eFlow )
    {
      pPort = &pDevCtxt->Ports[idx+1];
    }
    
    /* Wait for the port to disconnect if it is in the middle of disconnecting */
    if (pPort->eDisconnectState == SB_PORT_DISCONNECTING)
    {
      SB_LOG_INFO_3("Waiting for port disconnection during config pipe "
                    "(client: 0x%x) (resource: 0x%x) (port: %d)", pClientCtxt, 
                    pPort->handle, pPort->portNum );

      pPort->bSignalDisconnect = TRUE;
      SlimBus_EventReset(pClientCtxt->hClientEvent);
      SlimBus_UnlockDevice(pDevCtxt);

      SlimBus_EventTimedWait(pClientCtxt,
                             pClientCtxt->hClientEvent,
                             SB_DEFAULT_CLIENT_TIMEOUT);

      SlimBus_LockDevice(pDevCtxt);
    }

    /* Attach the the BAM device only if we are not already attached */
    if (NULL == pPort->pBamCtxt)
    {
      result = SlimBus_ConnectBAM(pDevCtxt, &pPort->pBamCtxt);
      if (SB_SUCCESS != result)
      {
        SB_LOG_ERR_2("Error attaching to port BAM device "
                     "(client: 0x%08x) (h: 0x%08x)", pClientCtxt, hPort);
        pPort->pBamCtxt = NULL;
        break;
      }
    }
    
    /* If the BAM pipe is already connected, just change the pipe options */    
    if ( pPort->bBamConnected )
    {
      /* Can't change the flow direction if we're already connected */
      if ( eFlow != pPort->eFlow )
      {
        /*error*/
        SB_LOG_ERR_3("Attempt to change the direction "
                     "of flow of a connected port (client: 0x%08x) "
                     "(h: 0x%08x) (flow: %d)", pClientCtxt, hPort, eFlow);
        result = SB_ERROR;
        break;
      }
      
      result = DalSps_GetConfig(pPort->pBamCtxt, &Connect);
      if (SB_SUCCESS != result)
      {
        SB_LOG_ERR_3("Failure to get SPS pipe config "
                     "(client: 0x%08x) (hport: 0x%08x) (hpipe: 0x%08x)",
                     pClientCtxt, hPort, pPort->pBamCtxt);
        break;
      }
      
      Connect.eOptions = pConfig->eOptions;
      
      result = DalSps_SetConfig(pPort->pBamCtxt, &Connect);
      if (SB_SUCCESS != result)
      {
        SB_LOG_ERR_3("Failure to set SPS pipe config "
                     "(client: 0x%08x) (hport: 0x%08x) (hpipe: 0x%08x)",
                     pClientCtxt, hPort, pPort->pBamCtxt);
      }
    }
    else
    {
      result = DalSps_GetConfig(pPort->-pBamCtxt, &Connect);
      if (SB_SUCCESS != result)
      {
        SB_LOG_ERR_3(1, pDevCtxt, 3, "Failure to get SPS pipe config2 "
                     "(client: 0x%08x) (hport: 0x%08x) (hpipe: 0x%08x)",
                     pClientCtxt, hPort, pPort->pBamCtxt);
        break;
      }

      pPort->eFlow = eFlow;
      
      if (eFlow == SLIMBUS_PORT_SOURCE_FLOW)
      {
        Connect.eDestination = pDevCtxt->pBSP->hBamDev;
        Connect.uDestPipeIndex = 
          HAL_sb_PgdPortGetPipeNum(pDevCtxt->hSB, 0, pPort->portNum);
        Connect.eSource = pConfig->eBamDev;
        Connect.uSrcPipeIndex = pConfig->uPipeIndex;
        Connect.eMode = SPS_MODE_DEST;
      }
      else
      {
        Connect.eSource = pDevCtxt->pBSP->hBamDev;
        Connect.uSrcPipeIndex = 
          HAL_sb_PgdPortGetPipeNum(pDevCtxt->hSB, 0, pPort->portNum);
        Connect.eDestination = pConfig->eBamDev;
        Connect.uDestPipeIndex = pConfig->uPipeIndex;
        Connect.eMode = SPS_MODE_SRC;
      }
      Connect.eConfig = SPS_CONFIG_DEFAULT;
      Connect.eOptions = pConfig->eOptions;
      Connect.eOptions |= SPS_O_AUTO_ENABLE;
      Connect.eOptions &= ~(SPS_O_IRQ_MTI);  /* TODO: any other options to filter? */
      if (NULL != pConfig->DescFifo.pBase &&
          0 != pConfig->DescFifo.uPhysBase)
      {
        Connect.Desc = pConfig->DescFifo;
      }
      if (NULL != pConfig->Data.pBase &&
          0 != pConfig->Data.uPhysBase)
      {
        Connect.Data = pConfig->Data;
      }
      
      result = DalSps_Connect(pPort->pBamCtxt, &Connect);
      if (SB_SUCCESS != result)
      {
        SB_LOG_ERR_3("Failure to connect SPS pipe "
                     "(client: 0x%08x) (hport: 0x%08x) (hpipe: 0x%08x)",
                     pClientCtxt, hPort, pPort->pBamCtxt);
        break;
      }
      
      pPort->bBamConnected = TRUE;
      HAL_sb_PgdPortEnable(pDevCtxt->hSB, 0, pPort->portNum);
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
  DAL_sps_EventNotifyType structure will be passed back as the 
  user data in the callback.  If the event is not a callback 
  event, the DAL_sps_EventNotifyType structure can be retrieved 
  by the DalSlimBus_GetBamEvent() function after the event 
  object has been signaled. 

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
SlimBus_RegisterBamEvent( DalDeviceHandle * h, SlimBusResourceHandle  hPort, SlimBusBamTransferType  eTransferDir, DAL_sps_RegisterEventType * pReg) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  uint32 idx;
  SlimBusMasterPortType *pPort = NULL;
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
    if (idx >= pDevCtxt->numPorts || hPort != pDevCtxt->Ports[idx].handle)
    {
      result = SB_ERROR;
      break;
    }
    pPort = &pDevCtxt->Ports[idx];
    if ( pPort->eReqs & SLIMBUS_PORT_REQ_O_HALF_DUPLEX )
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

    result = DalSps_RegisterEvent(pPort->pBamCtxt, pReg);
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
SlimBus_SubmitBamTransfer( DalDeviceHandle * h, SlimBusResourceHandle  hPort, SlimBusBamTransferType  eTransferDir, DAL_sps_IOVecType * pIOVec, void * pUser) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  uint32 idx;
  SlimBusMasterPortType *pPort = NULL;
  SlimBusPortEventType eEvents = SLIMBUS_EVENT_O_NONE;
  SlimBusEventHandle hEvent = NULL;
  void *pUserData;
  SBResult result = SB_SUCCESS;

  if (!pClientCtxt->bOpen ||
      NULL == pIOVec ||
      SLIMBUS_BAM_NUM_ENUMS <= eTransferDir )
  {
    return SB_ERROR;
  }

  SlimBus_LockDevice(pDevCtxt);

  do
  {
    idx = SB_GET_HANDLE_IDX(hPort);
    if (idx >= pDevCtxt->numPorts || hPort != pDevCtxt->Ports[idx].handle)
    {
      result = SB_ERROR;
      break;
    }
    pPort = &pDevCtxt->Ports[idx];
    if ( pPort->eReqs & SLIMBUS_PORT_REQ_O_HALF_DUPLEX )
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
      SB_LOG_ERR_1("Attempt to submit descriptors when BAM handle is NULL (client: 0x%x)",
                   pClientCtxt);
      result = SB_ERROR;
      break;
    }
    
    /* Increment our internal counters for how many descriptors and bytes
       have been submitted (for debug) */
    pPort->uDescSubmit++;
    pPort->uBytesSubmit += pIOVec->uSize;
    
    result = DalSps_TransferOne(pPort->pBamCtxt, pIOVec->uAddr, pIOVec->uSize, 
                                pUser, pIOVec->uFlags);
    
    /* If an event has been registered but the interrupt is disabled,
       then it means an underflow/overflow condition has previously occurred
       and we should re-enable the interrupt once the condition is resolved */    
    if (NULL != pPort->hEvent)
    {
      uint32 uIntMask = (1<<pPort->portNum);
      if ( HAL_sb_PgdPortGetIntStatusEe(pDevCtxt->hSB, 0, pDevCtxt->pBSP->uMyEE) & uIntMask)
      {
        uint32 uFifoCnt = 0;
        uint32 uWaitCnt = 0;
        
        if (pPort->eFlow == SLIMBUS_PORT_SINK_FLOW)
        {
          uFifoCnt = HAL_sb_PgdPortGetFifoSize(pDevCtxt->hSB, 0, pPort->portNum);
        }

        /* Clear the interrupt to make sure that port will
           restart transfers to/from the port FIFO */
        HAL_sb_PgdPortClearIntEe(pDevCtxt->hSB, 0, pDevCtxt->pBSP->uMyEE, uIntMask);

        /* If the channel is active, wait for the overflow or underflow
           condition to be resolved before re-enabling the interrupt */
        while ( (HAL_sb_PgdPortGetCurrentFifoWordCount(pDevCtxt->hSB, 0, pPort->portNum) == uFifoCnt) &&
                HAL_sb_PgdPortIsChannelActive(pDevCtxt->hSB, 0, pPort->portNum) &&
                uWaitCnt++ < SB_MAX_INT_REENABLE_WAIT )
        {
          SLIMBUS_BUSYWAIT(1);
        }
        /* Clear the interrupt to make sure that port will
           restart transfers to/from the port FIFO */
        HAL_sb_PgdPortClearIntEe(pDevCtxt->hSB, 0, pDevCtxt->pBSP->uMyEE, uIntMask);

        /* Since we are clearing the port interrupt above, check to make sure
           we didn't miss a port disconnection, use the channel number
           as a proxy for the port being connected since it should always
           be non-zero if the port is connected */
        if ( HAL_sb_PgdPortGetChannelNumber(pDevCtxt->hSB, 0, pPort->portNum) == 0 )
        {
          SlimBus_ProcessPortDisconnection(pDevCtxt, pPort, &eEvents);
        }
        else
        {
          /* Re-enable the port interrupt now that any underflow/overflow
             condition is resolved */
          EnablePortInterrupt(pDevCtxt, uIntMask);

          SB_LOG_WARN_3("enabled port interrupt after overflow/underflow resolved "
                        "(client: 0x%x) (resource: 0x%x) (port: %d)",
                        pClientCtxt, pPort->handle, pPort->portNum);
        }
      }
      
      /* Update the port events if there was a disconnection event */  
      if (SLIMBUS_EVENT_O_NONE != eEvents &&
          eEvents & pPort->eEnabledEvents)
      {
        pPort->eActiveEvents |= eEvents;
        hEvent = pPort->hEvent;
        pUserData = pPort->pEventUserData;
      }

    }
    
  } while (FALSE);
  
  SlimBus_UnlockDevice(pDevCtxt);
  
  if (NULL != hEvent)
  {
    /* Signal event here to avoid potential deadlock */
    SlimBus_EventTriggerEx(hEvent, (uint32)pUserData);
  }

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
SlimBus_GetBamIOVec( DalDeviceHandle * h, SlimBusResourceHandle  hPort, SlimBusBamTransferType  eTransferDir, DAL_sps_IOVecType * pIOVec) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  uint32 idx;
  SlimBusMasterPortType *pPort = NULL;
  SBResult result = SB_SUCCESS;

  if (!pClientCtxt->bOpen ||
      NULL == pIOVec ||
      SLIMBUS_BAM_NUM_ENUMS <= eTransferDir )
  {
    return SB_ERROR;
  }

  SlimBus_LockDevice(pDevCtxt);

  do
  {
    idx = SB_GET_HANDLE_IDX(hPort);
    if (idx >= pDevCtxt->numPorts || hPort != pDevCtxt->Ports[idx].handle)
    {
      result = SB_ERROR;
      break;
    }
    pPort = &pDevCtxt->Ports[idx];
    if ( pPort->eReqs & SLIMBUS_PORT_REQ_O_HALF_DUPLEX )
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

    result = DalSps_GetIOVec(pPort->pBamCtxt, pIOVec);
  } while (FALSE);

  SlimBus_UnlockDevice(pDevCtxt);

  return result;
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
SlimBus_GetBamEvent( DalDeviceHandle * h, SlimBusResourceHandle  hPort, SlimBusBamTransferType  eTransferDir, DAL_sps_EventNotifyType * pEvent) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  uint32 idx;
  SlimBusMasterPortType *pPort = NULL;
  SBResult result = SB_SUCCESS;

  if (!pClientCtxt->bOpen ||
      NULL == pEvent ||
      SLIMBUS_BAM_NUM_ENUMS <= eTransferDir )
  {
    return SB_ERROR;
  }

  SlimBus_LockDevice(pDevCtxt);

  do
  {
    idx = SB_GET_HANDLE_IDX(hPort);
    if (idx >= pDevCtxt->numPorts || hPort != pDevCtxt->Ports[idx].handle)
    {
      result = SB_ERROR;
      break;
    }
    pPort = &pDevCtxt->Ports[idx];
    if ( pPort->eReqs & SLIMBUS_PORT_REQ_O_HALF_DUPLEX )
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

    result = DalSps_GetEvent(pPort->pBamCtxt, pEvent);
  } while (FALSE);

  SlimBus_UnlockDevice(pDevCtxt);

  return result;
}

/**
 * @brief Reset the BAM hardware
 *
 * This function resets the BAM hardware
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device context
 * 
 * @return SB_SUCCESS on sucess, error code on error 
 */
SBResult SlimBus_ResetBAM(SlimBusDevCtxt *pDevCtxt)
{
  DalSps_DeviceReset(pDevCtxt->Plat.hBAM, (DAL_sps_DevHandleType) pDevCtxt->hBamDev);

  return SB_SUCCESS;
}

/**
 * @brief Detach from the BAM driver
 *
 * This function detaches from the BAM driver
 * 
 * @param[in] pBamCtxt  Pointer to the BAM context
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SBResult SlimBus_DetachBAM(SlimBusBamCtxt *pBamCtxt)
{
  DalSps_Disconnect(pBamCtxt);
  DAL_DeviceDetach(pBamCtxt);
  
  return SB_SUCCESS;
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
  DalSps_Disconnect(pPort->pBamCtxt);
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
  DAL_sps_RegisterEventType Reg;

  /* Unregister all events for the BAM pipe */
  Reg.eMode = SPS_TRIGGER_CALLBACK;
  Reg.eOptions = 
    SPS_O_DESC_DONE|SPS_O_INACTIVE|SPS_O_WAKEUP|
    SPS_O_OUT_OF_DESC|SPS_O_ERROR|SPS_O_EOT;
  Reg.hEvent = NULL;
  Reg.pUser = NULL;
  DalSps_RegisterEvent(pPort->pBamCtxt, &Reg);
}

