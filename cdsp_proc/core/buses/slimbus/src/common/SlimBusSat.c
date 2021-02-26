/*
===========================================================================

FILE:         SlimBusSat.c

DESCRIPTION:  This file implements the satellite functionality of the 
              SLIMbus driver.

===========================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/buses/slimbus/src/common/SlimBusSat.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
07/07/16   MJS     Interrupt enablement changes for HLOS compatibility.
03/24/16   MJS     Retention support.
02/23/16   MJS     Remove unused timer callback handles.
02/05/16   MJS     Add support for multiple endpoints per port.
08/25/15   MJS     Cleanup, remove unnecessary RemoveClientNode() calls.
08/14/15   MJS     Use hardware enumeration flag when calling StopIdle/ResumeIdle.
07/24/15   MJS     Increment available transactions for all devices after
                   satellite receives master capability.
07/17/15   MJS     Don't enable/disable ints during enter/exit low power mode.
06/08/15   NSD     Replace SlimBus_EventWait() calls with SlimBus_EventTimedWait().
05/22/15   MJS     Call StopIdle for satellite mode initialization.
11/18/14   MJS     Perform dummy read after POR for hardware workaround.
06/17/14   MJS     Support logging of master power up/down notifications.
05/22/14   MJS     Add workaround to phase shift frame sync after timeout.
03/04/14   MJS     Fix Klocwork issue.
01/31/14   MJS     Cleanup bInIsr flag.
08/16/13   MJS     Disable interrupts before turning off satellite clock.
07/24/13   MJS     Increment available transactions for all devices after
                   satellite receives master capability.
04/12/13   dj      Updated for SLIMBUS_CLOCK_ON & SLIMBUS_CLOCK_OFF to return code
03/21/13   MJS     Support for standalone satellite hardware configuration.
03/21/13   MJS     Cleanup low power and pause clock logic.
03/13/13   dj      Replace memcpy with SlimBus_Memcpy
02/05/13   dj      Fix Code analysis warnings
12/17/12   MJS     Satellite support for hardware power collapse.
11/20/12   MJS     Cleanup master ack event after default timeout occurs.
11/13/12   MJS     Support for non-standalone satellite clock management.
10/26/12   MJS     Add interrupt disable function.
10/25/12   MJS     Support for role reversal.
09/21/12   MJS     Add interrupt enable function.
06/26/12   MJS     Add final port disconnection bitmask for power management.
06/13/12   MJS     Support for standlone satellite low-power management.
05/16/12   MJS     Updated logging macros.
12/18/11   MJS     Remove references to DAL.
05/01/12   MJS     Fix Klocwork warning.
03/06/12   MJS     Remove Fusion-specific initialization.
10/21/11   MJS     Detect channels being activated from reconfiguration
                   sequence and simulate master ack for the reconfig request.
10/14/11   MJS     Support for standalone inactive framer mode.
09/29/11   MJS     Fix for reporting unsupported messages.
09/12/11   MJS     Improved logging for error conditions.
08/23/11   MJS     Implement alternate SW workaround for CR-0000156195,
                   only disconnect BAM after port is disconnected.
05/25/11   MJS     Separate hardware from software initialization.
05/24/11   MJS     Enable error condition interupts in NGD.
05/05/11   MJS     Check, clear and return client error status after
                   receiving master acknowledgement.
04/14/11   MJS     Add logging facility.
11/01/10   MJS     Initial revision.

===========================================================================
             Copyright (c) 2010, 2011, 2012, 2013, 2014, 2015 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

#include "SlimBusTarget.h"
#include SLIMBUS_OS_H
#include "SlimBus.h"
#include "SlimBusConsts.h"
#include "SlimBusLists.h"
#include "SlimBusSat.h"
#include SB_TRACE_HEADER(SlimBusSat)


SB_TEST_DECLARE();

/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/

/**
 * @brief Checks if the driver binary supports satellite-specific logic 
 *
 * This function checks whether the compiled driver binary supports 
 * satellite-specific logic
 * 
 * @return TRUE if satellite-specfic logic is supported, FALSE otherwise 
 */
SBBOOL SlimBus_IsSatSupported( void )
{
  return TRUE;
}


/**
 * @brief Check for and enter standalone low power mode
 *
 * This function checks whether it is OK to transition to a standalone
 * low power mode and if so transitions to low power mode 
 * 
 * @param pDevCtxt  Pointer to the device context
 * 
 * @return SB_SUCCESS if there were no errors, error code on 
 *         error
 */
static SBResult SlimBus_SatCheckEnterStandaloneLowPowerMode
(
  SlimBusDevCtxt *pDevCtxt
)
{
  SBResult result = SB_SUCCESS;

  if ( pDevCtxt->bClockIsOn )
  {
    if ( SlimBus_ReadyForLowPowerMode(pDevCtxt) &&
         ( !pDevCtxt->bIsHwOwner ||
           !pDevCtxt->bActiveFramer ) )
    {
      if (pDevCtxt->bIsHwOwner)
      {
        SlimBusDeviceType *pMgrDevice = NULL;

        SB_LOG_INFO_0("Resetting hardware and turning off slimbus core clock");

        /* Reset Slimbus hardware. */
        HAL_sb_CompReset(pDevCtxt->hSB);

        SlimBus_DeinitGpios(pDevCtxt);

        pMgrDevice = pDevCtxt->pDevices[SB_MSG_LA_MANAGER&SB_LA_IDX_MASK];
        if (NULL != pMgrDevice)
        {
          /* Mark available transactions as 0 until we receive master announcement */
          pMgrDevice->uAvailTrans = 0;
        }
      }
      else
      {
        SB_LOG_INFO_0("Turning off satellite slimbus clock");
        
        SlimBus_DeviceDisableInterrupts(pDevCtxt);
      }
      
      result = SLIMBUS_CLOCK_OFF(pDevCtxt);

      if ( !pDevCtxt->bClockIsOn )
      {
        SlimBus_TimerStart(pDevCtxt,
                           pDevCtxt->hEnumTimerHandle,
                           pDevCtxt->hEnumTimerEvent,
                           0);

        SlimBus_TimerStart(pDevCtxt,
                           pDevCtxt->hSFTimerHandle,
                           pDevCtxt->hSFTimerEvent,
                           0);
      }
    }
  }
  
  return result;
}

/**
 * @brief Leave standalone satellite low-power mode
 *
 * This function leaves standalone satellite low-power mode 
 * 
 * @param pDevCtxt  Pointer to the device context
 * 
 * @return SB_SUCCESS if there were no errors, error code on 
 *         error
 */
static SBResult SlimBus_SatLeaveStandaloneLowPowerMode
(
  SlimBusDevCtxt *pDevCtxt
)
{
  SBResult result = SB_SUCCESS;

  if ( !pDevCtxt->bClockIsOn )
  {
    SB_LOG_INFO_0("Turning on satellite/standalone slimbus ref clock");

    result = SLIMBUS_CLOCK_ON(pDevCtxt);
  }
  
  if ( pDevCtxt->bClockIsOn )
  {
    /* Perform dummy HW read after POR for QCTDD01713765 */
    (void) HAL_sb_CompGetHwVersion(pDevCtxt->hSB);
    
    /* Check for power collapse and restore non-retained registers if necessary */
    SlimBus_CheckAndRestoreNonRetentionRegisters(pDevCtxt);

    SlimBus_DeviceEnableInterrupts(pDevCtxt);
  }
  
  return result;
}

/**
 * @brief Process a satellite-specific received message
 *
 * Process a satellite-specific received message 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] length  Length of the message
 * @param[in] data  First word of the received message
 * @param[in] pDataBuf  Pointer to the second word in the 
 *       received message buffer
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
SBResult SlimBus_ProcessSatRxMsg
(
  SlimBusDevCtxt *pDevCtxt,
  uint32 length,
  uint32 data,
  uint32 *pDataBuf
)
{
  uint32 srcLA;
  SlimBusMsgMtType eMT;
  SlimBusMsgMcType eMC;
  SlimBusMsgMcUsrType eUsrMC;
  SBResult result = SB_SUCCESS;
  
  eMT = (SlimBusMsgMtType) SB_MSG_GET(data,GENERIC_0,MT);
  eMC = (SlimBusMsgMcType) SB_MSG_GET(data,GENERIC_0,MC);
  eUsrMC = (SlimBusMsgMcUsrType) eMC;
  srcLA = SB_MSG_GET(data,GENERIC_0,LA);

  if ( SB_MSG_MT_SRC_REFERRED_USER == eMT &&
       SB_MSG_LA_MANAGER == srcLA )
  {
    if ( ( SB_MSG_MC_USR_LA_REPLY == eUsrMC ||
           SB_MSG_MC_USR_GENERIC_ACK == eUsrMC ) &&
         !SB_TEST_REPLIES_IGNORED() )
    {
      uint8 TID = SB_MSG_GET(data,USR_GENERIC_ACK_0,TID);
      SlimBusDeviceType *pMgrDevice;
      SlimBusEventNodeType *pEvent = NULL;

      pMgrDevice = pDevCtxt->pDevices[SB_MSG_LA_MANAGER&SB_LA_IDX_MASK];
      if (NULL != pMgrDevice)
      {
        pEvent = pMgrDevice->TIDMap[TID&SB_TID_IDX_MASK];
      }
      
      if (NULL != pEvent &&
          SB_BF_GET(pEvent->uRxMarker,RXMARKER,LA) == SB_MSG_LA_MANAGER &&
          SB_BF_GET(pEvent->uRxMarker,RXMARKER,TID) == TID)
      {
        if (SB_MSG_MC_USR_LA_REPLY == eUsrMC )
        {
          uint8 *puBuf = (uint8*) pDataBuf;
          SlimBusLogicalAddrType LA = puBuf[SLIMBUS_ENUM_ADDR_BUF_SIZE];
          SlimBusDeviceType *pDevice;
          if ( pDataBuf[0] != 0 || 
               (pDataBuf[1] & 0xffff) != 0 )
          {
            SBBOOL bNewDevice = FALSE;

            pDevice = pDevCtxt->pDevices[LA&SB_LA_IDX_MASK];

            if ( NULL == pDevice )
            {
              if (pDevCtxt->bNoMalloc)
              {
                /* If we're in ISR then we can't alloc new struct */
                return SLIMBUS_ERR_RETRY;
              }
              pDevice = SlimBus_NewDeviceStruct();
              if (NULL == pDevice)
              {
                 return SB_ERROR;
              }
              pDevCtxt->pDevices[LA&SB_LA_IDX_MASK] = pDevice;
              SlimBus_Memcpy(pDevice->aEA, puBuf, SLIMBUS_ENUM_ADDR_BUF_SIZE);
              bNewDevice = TRUE;
            }
            if ( bNewDevice ||
                 memcmp(pDevice->aEA, puBuf, SLIMBUS_ENUM_ADDR_BUF_SIZE) == 0 )
            {
              pDevice->LA = LA;
              pDevice->eState = SB_ENUM_ACTIVE;
              if (NULL != pEvent->pRxBuf)
              {
                *(uint32*)pEvent->pRxBuf = LA | SB_ACK_SUCCESS_FLAG;
              }
            }
            else
            {
              if (NULL != pEvent->pRxBuf)
              {
                *(uint32*)pEvent->pRxBuf = SB_ACK_FAILURE_FLAG;
              }
            }
          }
          else
          {
            if (NULL != pEvent->pRxBuf)
            {
              *(uint32*)pEvent->pRxBuf = SB_ACK_FAILURE_FLAG;
            }
          }
        }
        else if ( SB_MSG_MC_USR_GENERIC_ACK == eUsrMC )
        {
          uint8 R = SB_MSG_GET(*pDataBuf,USR_GENERIC_ACK_1,RESP);
          if (NULL != pEvent->pRxBuf)
          {
            if (R != 0)
            {
              *(uint32*)pEvent->pRxBuf = SB_ACK_SUCCESS_FLAG;
            }
            else
            {
              *(uint32*)pEvent->pRxBuf = SB_ACK_FAILURE_FLAG;
            }
          }
        }

        pMgrDevice->TIDMap[TID&SB_TID_IDX_MASK] = NULL;
        SlimBus_EventTrigger(pEvent->hEvent);
        PushHeadEvent(pEvent->pFreeRoot, pEvent);

        /* Signal anyone waiting on an available transaction */
        pMgrDevice->uAvailTrans++;
        pEvent = PopHeadEvent(&pMgrDevice->TransEventQueue);
        if (NULL != pEvent)
        {
          SlimBus_EventTrigger(pEvent->hEvent);
        }
      }
    }
    else if ( SB_MSG_MC_USR_QC_MASTER_CAPABILITY == eUsrMC &&
              length >= SB_MSG_MC_USR_QC_MASTER_CAPABILITY_MIN_LEN )
    {
      uint32 uMagic = SB_MSG_GET(data,USR_QC_MASTER_CAPABILITY_0,MAGIC_LSB) |
        (SB_MSG_GET(pDataBuf[0],USR_QC_MASTER_CAPABILITY_1,MAGIC_MSB)<<
         (32-SB_MSG_MC_USR_QC_MASTER_CAPABILITY_0__MAGIC_LSB_SHFT));
      uint32 uProtoVer = SB_MSG_GET(pDataBuf[0],USR_QC_MASTER_CAPABILITY_1,PROTO_VER);
      uint32 uProto = SB_MSG_GET(pDataBuf[0],USR_QC_MASTER_CAPABILITY_1,SAT_PROTO);
      uint32 uNumTrans = 0, uIdx;
      SlimBusMsgFifoType *pTxMsgFifo = &pDevCtxt->TxMsgFifo;
      uint32 *pTxBuf;
      
      uProto &= SB_QC_SUPPORTED_PROTOCOLS;

      if ( uMagic == SB_QC_MASTER_PROTOCOL_MAGIC &&
           uProtoVer >= 1 )
      {
        SlimBusDeviceType *pDevice;
        if (uProto & SB_QC_MASTER_PROTOCOL_V1)
        {
          uNumTrans = 1;
        }
        else
        {
          SlimBus_ErrorFatal(pDevCtxt, "SLIMbus Satellite: unable to negotiate protocol!");
        }

        /* Setup message queues if they are enabled */
        SlimBus_SetupMsgQueues(pDevCtxt);

        pTxBuf = pTxMsgFifo->getBufFcn(NULL, NULL, pTxMsgFifo, 
                                       SB_MSG_MC_USR_REPORT_QC_SATELLITE_LEN);
        if (NULL == pTxBuf )
        {
          return SLIMBUS_ERR_RETRY;
        }

        pTxBuf[0] = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_USR_REPORT_QC_SATELLITE_LEN,
                                          SB_MSG_MT_SRC_REFERRED_USER,
                                          SB_MSG_MC_USR_REPORT_QC_SATELLITE,
                                          SB_MSG_DT_UNICAST,
                                          SB_MSG_LA_MANAGER) |
          SB_MSG_SET(SB_QC_MASTER_PROTOCOL_MAGIC_LSB,USR_REPORT_QC_SATELLITE_0,MAGIC_LSB);
        pTxBuf[1] = 
          SB_MSG_SET(SB_QC_MASTER_PROTOCOL_MAGIC_MSB,USR_REPORT_QC_SATELLITE_1,MAGIC_MSB) |
          SB_MSG_SET(SB_QC_REPORT_SATELLITE_VER,USR_REPORT_QC_SATELLITE_1,PROTO_VER) |
          SB_MSG_SET(uProto,USR_REPORT_QC_SATELLITE_1,SAT_PROTO);

        pTxMsgFifo->sendBufFcn(pDevCtxt, pTxMsgFifo);

        pDevice = SB_GET_MGR_DEV_PTR(pDevCtxt);
        pDevice->uMaxTrans = uNumTrans;
        pDevice->eState = SB_ENUM_ACTIVE;
        
        for (uIdx = 0; uIdx < pDevCtxt->numDeviceSlots; uIdx++)
        {
          SlimBusDeviceType *pDevice = pDevCtxt->pDevices[uIdx];
          
          if (NULL != pDevice &&
              SB_ENUM_ACTIVE == pDevice->eState)
          {
            while (pDevice->uAvailTrans < uNumTrans)
            {
              SlimBusEventNodeType *pEvent;

              pDevice->uAvailTrans++;
              /* Signal anyone waiting on an available transaction */
              pEvent = PopHeadEvent(&pDevice->TransEventQueue);
              if (NULL != pEvent)
              {
                SlimBus_EventTrigger(pEvent->hEvent);
              }
              else
              {
                pDevice->uAvailTrans = uNumTrans;
              }
            }
          }
        }

        SB_LOG_INFO_1("received master capability (protocol: 0x%lx)", uProto);

        if (pDevCtxt->bHwEnumLock)
        {
          SlimBus_ResumeIdle(pDevCtxt);
          pDevCtxt->bHwEnumLock = FALSE;
        }
      }
    }
    else if ( SB_MSG_MC_USR_NOTIFY_MGR_AWAKE == eUsrMC &&
              length >= SB_MSG_MC_USR_NOTIFY_MGR_AWAKE_LEN )
    {
      uint32 TID = SB_MSG_GET(data,USR_NOTIFY_MGR_AWAKE_0,TID);
      uint32 uCode = SB_MSG_GET(pDataBuf[0],USR_NOTIFY_MGR_AWAKE_1,CODE);
      
      SB_LOG_INFO_2("Got manager powered up notification (TID: %d) (code: %d)",
                    TID, uCode);
    }
    else if ( SB_MSG_MC_USR_NOTIFY_MGR_SLEEP == eUsrMC &&
              length >= SB_MSG_MC_USR_NOTIFY_MGR_SLEEP_LEN )
    {
      uint32 TID = SB_MSG_GET(data,USR_NOTIFY_MGR_SLEEP,TID);

      SB_LOG_INFO_1("Got manager powered down notification (TID: %d)", TID);
    }
    else
    {
      result = SLIMBUS_ERR_UNHANDLED_MSG;
    }
  }
  else
  {
    result = SLIMBUS_ERR_UNHANDLED_MSG;
  }
  
  return result;
}

/**
 * @brief Wait for acknowledgement response from the master 
 *        driver
 *
 * This function waits for an acknowledgement response from the 
 * master driver 
 * 
 * @param[in] pClientCtxt  Pointer to the client context
 * @param[in] pBuf  Pointer to buffer to use for Tx event 
 *       marker
 * @param[in] TID  Transaction ID
 * @param[in] puMarker  Pointer to location to store ack marker 
 *       data
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
_IRQL_requires_(DISPATCH_LEVEL)
SBResult SlimBus_WaitForMasterAck
(
  SlimBusClientCtxt *pClientCtxt,
  SlimBusEventNodeType *pEventNode,
  uint8 TID,
  uint32 *puMarker
)
{
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SBResult result = SB_SUCCESS;
  
  if (pDevCtxt->eDrvType == SB_SATELLITE_DRIVER)
  {
    SlimBusDeviceType *pDevice = SB_GET_MGR_DEV_PTR(pDevCtxt);
    uint32 uMsgHeader = 0;
    uint32 uMarker = 0;

    if (NULL == pEventNode)
    {
      return SB_ERROR;
    }
    
    if (NULL != pEventNode->pTxBuf)
    {
      uMsgHeader = *pEventNode->pTxBuf;
    }
    
    pEventNode->uRxMarker = 
      SB_BF_SET(SB_MSG_LA_MANAGER,RXMARKER,LA) |
      SB_BF_SET(TID,RXMARKER,TID);
    pEventNode->pRxBuf = (uint8*)&uMarker;
    pEventNode->pRxBufSize = NULL;
    pDevice->TIDMap[TID&SB_TID_IDX_MASK] = pEventNode;
    pDevice->uAvailTrans--;

    /* Create event handle again to grab current thread ID */
    SlimBus_EventReset(pClientCtxt->hClientEvent);

    SlimBus_UnlockDevice(pDevCtxt);

    result = SlimBus_EventTimedWait(pClientCtxt,
                                    pEventNode->hEvent,
                                    SB_DEFAULT_RX_TIMEOUT);

    SlimBus_LockDevice(pDevCtxt);

    if ( SB_SUCCESS == result )
    {
      if ( uMarker == 0 )
      {
        result = SLIMBUS_ERR_MSG_RESP_TIMEOUT;
        SB_LOG_ERR_2("master ack timeout (client: 0x%lx) (msg header: 0x%lx)", 
                     (uint32)pClientCtxt, uMsgHeader);
      }
      else 
      {
        if ( ! (uMarker & SB_ACK_SUCCESS_FLAG) )
        {
          result = SB_ERROR;
        }
      }
    }
    else
    {
      SlimBus_CleanupMsgEvent(pDevice, TID, pEventNode, pEventNode->pFreeRoot);
    }
    
    if (SLIMBUS_ERR_NONE != pClientCtxt->eLastMsgErr)
    {
      result = pClientCtxt->eLastMsgErr;
      pClientCtxt->eLastMsgErr = SLIMBUS_ERR_NONE;
    }
    if (NULL != puMarker)
    {
      *puMarker = uMarker;
    }
  }
  
  return result;
}

/**
 * @brief Query the master driver for a logical address
 *
 * This function queries the remote master driver for a Slimbus 
 * native device logical address 
 * 
 * @param[in] pClientCtxt  Pointer to the client context
 * @param[in] pEA  Pointer to the enumeration address to query
 * @param[in] uEASize  Size of the enumeration address
 * @param[out] pLA  Pointer to location to store the logical 
 *       address that was looked up
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
SBResult SlimBus_QueryRemoteLogicalAddr
(
  SlimBusClientCtxt *pClientCtxt,
  const uint8 *pEA,
  uint32 uEASize,
  SlimBusLogicalAddrType * pLA
)
{
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SBResult result = SB_ERROR;

  UNREFERENCED_PARAMETER(uEASize);
  
  do
  {
    uint32 *pBuf;
    uint32 uMarker = 0;
    uint32 TID;
    SlimBusEventNodeType *pEvent;
    
    if (NULL == PeekHeadEvent(&pClientCtxt->FreeEventStack))
    {
      break;
    }
    pBuf = pDevCtxt->TxMsgFifo.getBufFcn(pClientCtxt, SB_GET_MGR_DEV_PTR(pDevCtxt),
                                         &pDevCtxt->TxMsgFifo,
                                         SB_MSG_MC_USR_LA_QUERY_LEN);
    if (NULL == pBuf)
    {
      break;
    }
    TID = SlimBus_GetNextMasterTID(pDevCtxt);
    pBuf[0] = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_USR_LA_QUERY_LEN,
                                    SB_MSG_MT_DEST_REFERRED_USER,
                                    SB_MSG_MC_USR_LA_QUERY, SB_MSG_DT_UNICAST,
                                    SB_MSG_LA_MANAGER) |
      SB_MSG_SET(TID,USR_LA_QUERY_0,TID);
    SlimBus_Memcpy(&pBuf[1], pEA, SLIMBUS_ENUM_ADDR_BUF_SIZE);

    pEvent = SlimBus_EnqueueMsgEvent(&pDevCtxt->TxMsgFifo, pClientCtxt, pBuf,
                                  pClientCtxt->hClientEvent);
    pDevCtxt->TxMsgFifo.sendBufFcn(pDevCtxt, &pDevCtxt->TxMsgFifo);
    if (NULL == pEvent)
    {
      /* Added for klocwork, this should never happen due to PeekHeadEvent() 
         being checked earlier in this function */
      break;
    }
    result = SlimBus_WaitForMasterAck(pClientCtxt, pEvent, (uint8) TID, &uMarker);

    if (SB_SUCCESS == result)
    {
      *pLA = uMarker & 0xff;
    }
  } while (FALSE);
  
  return result;
}

/**
 * @brief Signal the remote master driver to process the 
 *        reconfiguration sequence for the Next* messages queued
 *        by this client
 *
 * This function signals the remote master driver to process the 
 * reconfiguration sequence for the Next* message that have been 
 * queued by this client 
 * 
 * @param[in] pClientCtxt  Pointer to the client context
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
static SBResult SlimBus_TriggerRemoteReconfigSeq(SlimBusClientCtxt *pClientCtxt)
{
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SBResult result = SB_SUCCESS;

  SlimBusMsgFifoType *pTxMsgFifo = &pDevCtxt->TxMsgFifo;
  uint32 TID;
  uint32 *pTxBuf;

  do
  {
    if (NULL == PeekHeadEvent(&pClientCtxt->FreeEventStack))
    {
      result = SB_ERROR;
      break;
    }
    pTxBuf = pTxMsgFifo->getBufFcn(pClientCtxt, SB_GET_MGR_DEV_PTR(pDevCtxt),
                                  pTxMsgFifo, SB_MSG_MC_USR_DO_RECONFIGURE_NOW_LEN);
    if (NULL == pTxBuf)
    {
      result = SB_ERROR;
      break;
    }
    TID = SlimBus_GetNextMasterTID(pDevCtxt);
    pTxBuf[0] = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_USR_DO_RECONFIGURE_NOW_LEN,
                                      SB_MSG_MT_DEST_REFERRED_USER,
                                      SB_MSG_MC_USR_DO_RECONFIGURE_NOW,
                                      SB_MSG_DT_UNICAST,
                                      SB_MSG_LA_MANAGER) |
      SB_MSG_SET(TID,USR_DO_RECONFIGURE_NOW_0,TID);
    pTxBuf[1] = SB_MSG_SET(pClientCtxt->BasicClient.uClientNum,
                            USR_DO_RECONFIGURE_NOW_1,CLIENT_NUM);
  
    pClientCtxt->uSatReconfigTID = TID;
    result = SlimBus_SendMasterAckedMsg(pClientCtxt, pTxMsgFifo, pTxBuf, (uint8) TID);
  } while (FALSE);
  
  return result;
}

/**
 * @brief This function performs the driver-specific 
 *        functionality to connect a port to a data channel
 *
 * This function performs the driver-specific functionality to 
 * connect a port to a data channel 
 * 
 * @param[in] pClientCtxt  Pointer to the client context
 * @param[in] pDevice  Pointer to the native SLIMbus device 
 *               structure corresponding to the device
 *               containing the port to connect
 * @param[in] eFlow  Flow direction for the port 
 * @param[in] uPortNum  Port number to connect 
 * @param[in] uChanNum  Channel number to connect 
 * 
 * @return SB_SUCCESS on success, error code otherwise
 */
static SBResult SlimBus_SatConnectPortToChannel
(
  SlimBusClientCtxt *pClientCtxt,
  SlimBusDeviceType *pDevice,
  SlimBusPortFlowType eFlow,
  uint32 uPortNum,
  uint32 uEndPointNum,
  uint32 uChanNum
)
{
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  uint32 *pBuf;
  uint8 MC;
  uint32 TID;
  
  /* If we are satellite driver, we have to proxy the port connect message
     through the master driver */

  if (NULL == PeekHeadEvent(&pClientCtxt->FreeEventStack))
  {
    return SB_ERROR;
  }

  pBuf = pDevCtxt->TxMsgFifo.getBufFcn(pClientCtxt, SB_GET_MGR_DEV_PTR(pDevCtxt),
                                       &pDevCtxt->TxMsgFifo, 
                                       SB_MSG_MC_USR_REQ_CONNECT_SOURCE_LEN);
  if ( NULL == pBuf )
  {
    return SB_ERROR;
  }

  if (eFlow == SLIMBUS_PORT_SOURCE_FLOW)
  {
    MC = SB_MSG_MC_USR_REQ_CONNECT_SOURCE;
  }
  else
  {
    MC = SB_MSG_MC_USR_REQ_CONNECT_SINK;
  }
  TID = SlimBus_GetNextMasterTID(pDevCtxt);
  pBuf[0] = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_USR_REQ_CONNECT_SOURCE_LEN,
                                  SB_MSG_MT_DEST_REFERRED_USER,
                                  MC, SB_MSG_DT_UNICAST,
                                  SB_MSG_LA_MANAGER) |
    SB_MSG_SET(pDevice->LA,USR_REQ_CONNECT_SOURCE_0,DEST_LA);
  pBuf[1] =
    SB_MSG_SET(uPortNum,USR_REQ_CONNECT_SOURCE_1,PORT_NUM) |
    SB_MSG_SET(uEndPointNum,USR_REQ_CONNECT_SOURCE_1,EP_NUM) |
    SB_MSG_SET(uChanNum,USR_REQ_CONNECT_SOURCE_1,CHAN_NUM) |
    SB_MSG_SET(TID,USR_REQ_CONNECT_SOURCE_1,TID);

  return SlimBus_SendMasterAckedMsg(pClientCtxt, &pDevCtxt->TxMsgFifo, pBuf, (uint8) TID);
}

/**
 * @brief This function performs the driver-specific 
 *        functionality to disconnect a port from a data channel
 *
 * This function performs the driver-specific functionality to 
 * disconnect a port from a data channel 
 * 
 * @param[in] pClientCtxt  Pointer to the client context
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] pDevice  Pointer to the native SLIMbus device 
 *               structure corresponding to the device
 *               containing the port to disconnect
 * @param[in] uPortNum  Port number to disconnect 
 * 
 * @return SB_SUCCESS on success, error code otherwise
 */
static SBResult SlimBus_SatDisconnectPortFromChannel
(
  SlimBusClientCtxt *pClientCtxt,
  SlimBusDevCtxt *pDevCtxt,
  SlimBusDeviceType *pDevice,
  uint32 uPortNum
)
{
  uint32 *pBuf;
  uint32 TID;

  /* If we are satellite driver, we have to proxy the port disconnect message
     through the master driver */

  if (NULL != pClientCtxt)
  {
    if (NULL == PeekHeadEvent(&pClientCtxt->FreeEventStack))
    {
      return SB_ERROR;
    }
  }
  
  pBuf = pDevCtxt->TxMsgFifo.getBufFcn(pClientCtxt, SB_GET_MGR_DEV_PTR(pDevCtxt),
                                       &pDevCtxt->TxMsgFifo, 
                                       SB_MSG_MC_USR_REQ_DISCONNECT_PORT_LEN);
  if ( NULL == pBuf )
  {
    return SB_ERROR;
  }
  
  if (NULL != pClientCtxt)
  {  
    TID = SlimBus_GetNextMasterTID(pDevCtxt);
  }
  else
  {
    TID = SB_TID_NOACK;
  }

  pBuf[0] = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_USR_REQ_DISCONNECT_PORT_LEN,
                                  SB_MSG_MT_DEST_REFERRED_USER,
                                  SB_MSG_MC_USR_REQ_DISCONNECT_PORT,
                                  SB_MSG_DT_UNICAST,
                                  SB_MSG_LA_MANAGER) |
    SB_MSG_SET(pDevice->LA,USR_REQ_DISCONNECT_PORT_0,DEST_LA);
  pBuf[1] =
    SB_MSG_SET(uPortNum,USR_REQ_DISCONNECT_PORT_1,PORT_NUM) |
    SB_MSG_SET(TID,USR_REQ_DISCONNECT_PORT_1,TID);

  if (NULL != pClientCtxt)
  {
    return SlimBus_SendMasterAckedMsg(pClientCtxt, &pDevCtxt->TxMsgFifo, pBuf, (uint8) TID);
  }
  else
  {
    pDevCtxt->TxMsgFifo.sendBufFcn(pDevCtxt, &pDevCtxt->TxMsgFifo);
    return SB_SUCCESS;
  }
}

/**
 * Nop function for the satellite Slimbus driver 
 *  
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return SB_SUCCESS
 */
static SBResult SlimBus_SatNop(SlimBusDevCtxt *pDevCtxt)
{
  UNREFERENCED_PARAMETER(pDevCtxt);
  
  return SB_SUCCESS;
}

/**
 * @brief Perform satellite-specific device driver 
 *        initialization
 *
 * This function performs satellite-specific device driver 
 * initialization 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return SB_SUCCESS on sucess, error code on error 
 */
SBResult SlimBus_DeviceInitSat(SlimBusDevCtxt *pDevCtxt)
{
  SlimBusDeviceType *pDevice;

  pDevCtxt->eMsgDev = HAL_SB_DEVICE_NON_PORTED_GENERIC_1;

  if (SB_SUCCESS != SlimBus_InitSatParams(pDevCtxt))
  {
    return SB_ERROR;
  }

  pDevCtxt->connectPortFcn = SlimBus_SatConnectPortToChannel;
  pDevCtxt->disconnectPortFcn = SlimBus_SatDisconnectPortFromChannel;
  pDevCtxt->doReconfigSeqFcn = SlimBus_TriggerRemoteReconfigSeq;
  if ( pDevCtxt->bIsHwOwner ||
       SlimBus_UseSatelliteClockControl(pDevCtxt) )
  {
    pDevCtxt->checkEnterLowPowerFcn = SlimBus_SatCheckEnterStandaloneLowPowerMode;
    pDevCtxt->leaveLowPowerFcn = SlimBus_SatLeaveStandaloneLowPowerMode;
  }
  else
  {
    pDevCtxt->checkEnterLowPowerFcn = SlimBus_SatNop;
    pDevCtxt->leaveLowPowerFcn = SlimBus_SatNop;
  }

  if (NULL == pDevCtxt->pDevices[SB_MSG_LA_MANAGER&SB_LA_IDX_MASK])
  {
    /* Create device structure for the manager */
    pDevice = SlimBus_NewDeviceStruct();
    if (NULL == pDevice)
    {
      return SB_ERROR;
    }
    else
    {
      pDevCtxt->pDevices[SB_MSG_LA_MANAGER&SB_LA_IDX_MASK] = pDevice;
      pDevice->LA = SB_MSG_LA_MANAGER;
      /* Manager doesn't have an EA, so just fill with a pattern */
      memset(pDevice->aEA, 0x5A, SLIMBUS_ENUM_ADDR_BUF_SIZE);
      pDevice->eState = SB_ENUM_ACTIVE;
  
      /* Mark available transactions as 0 until we receive master announcement */
      pDevice->uAvailTrans = 0;
    }
  }
  
  return SB_SUCCESS;
}

