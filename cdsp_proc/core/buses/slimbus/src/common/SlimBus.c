/*
===========================================================================

FILE:         SlimBus.c

DESCRIPTION:  This file implements the SLIMbus core driver.

===========================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/buses/slimbus/src/common/SlimBus.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
06/27/17   MJS     Workaround for issue exiting idle mode.
06/19/17   MJS     Fix issue with endpoint array memory allocation.
06/01/17   RC      Added logic to clear the datashift bit for the
                   HWBUG QCTDD03351991 after core reset.
05/25/17   MJS     Add support to disable retention.
03/29/17   RC      Added the logic to clean the bam handles after BAM reset.
03/03/17   MJS     Enhanced error reporting for port alloc failure.
01/31/17   MJS     Ensure message worker flag is reset during hardware init.
11/29/16   MJS     Porting HLOS compatibility changes.
11/16/16   MJS     Attempt recovery if hardware fails to leave idle mode.
11/16/16   MJS     Klocwork fixes.
09/27/16   MJS     Add query PGD port index API.
07/07/16   MJS     Interrupt enablement changes for HLOS compatibility.
05/25/16   MJS     Deinit the BAM during reinit after power collapse.
05/12/16   MJS     Disable enumeration timer when not the hardware owner.
05/10/16   MJS     Add clock gear callback and max endpoint APIs.
05/04/16   MJS     Deinit the BAM during restore from retention.
04/14/16   MJS     Fix for retention support.
04/11/16   MJS     Cleanup.
03/24/16   MJS     Retention support.
03/11/16   MJS     Return an error code for device open timeout.
02/23/16   MJS     Remove unused timer callback handles.
02/17/16   MJS     Fix BAM descriptor coverage gap in Rx message queue.
02/05/16   MJS     Add support for multiple endpoints per port.
11/11/15   MJS     Fix validation for ports that require manual disconnect.
10/14/15   MJS     Add support for batched messages.
10/09/15   MJS     Don't disable the port interrupt for REMOVE_CHANNEL event.
10/08/15   MJS     Disable port disconnection interrupt in the ISR context.
10/08/15   MJS     Prevent BAM handle from being accessed while disconnecting.
09/01/15   MJS     Check explicitly for port underflow/overflow during flush.
08/14/15   MJS     Set wait for hardware enum flag when calling StopIdle.
08/05/15   MJS     Save event handle during SendClientMessage.
07/17/15   MJS     Check status of EventTimedWait during WaitForMsgEvent.
07/17/15   MJS     Mark all device available transactions as zero until after
                   enumeration.
07/17/15   MJS     Offset counter numbers by base port.
07/16/15   NSD     Upper watermark value subtracted from the FIFO size.
06/24/15   SG      ClientCtxt timeout event destroyed in SlimBus_DeviceDetach.
06/17/15   MJS     Don't enable interrupts during DeviceInitHardware.
06/10/15   SG      Freed master port list in SlimBus_DeviceDetach to resolve 
                   heap corruption issue.
06/08/15   NSD     Replace SlimBus_EventWait() calls with SlimBus_EventTimedWait()
                   and clean up the SlimBusTimerCallback function.
06/04/15   NSD     Change the wordings of log messages, integrate changes
                   to enhance timeline by checking for external clock
                   toggle only once during initialization.
05/27/15   NSD     Remove the condition to check if the Rx port reset workaround
                   is available.
05/22/15   MJS     Call StopIdle for satellite mode initialization.
05/06/15   NSD     Remove sending of secondary port disconnect to the client.
04/28/15   NSD     Add support for no data line activity interrupt on 
                   interface device.
04/22/15   NSD     Add support bypass software workaround when superframe
                   sync is not achieved when the hardware support is available.
04/01/15   SG      Reseted PGD port wait event signal.
02/05/15   MJS     Fix compiler warnings.
09/09/14   mq      Multi-dataline support.
09/08/14   MJS     Use alternate implementation for low power mode with
                   inactive framer.
09/04/14   MJS     Reset hardware after timeout if frame sync not acquired.
07/09/14   MJS     Fix validation done for channel define request processing.
06/17/14   MJS     Add support for minimum clock gear.
06/17/14   MJS     Send master power up/down notification to satellites.
06/12/14   MJS     Don't enable port interrupt when manually disconnecting
                   port after reactivation, to avoid missing disconnect event.
06/06/14   MJS     Make sure BAM EOT flag is set when Tx message queue is 
                   near capacity.
05/28/14   MJS     Fix to avoid sending a secondary port disconnect twice.
05/22/14   MJS     Add workaround to phase shift frame sync after timeout.
05/07/14   MJS     Allow BAM trust registers to not be programmed.
04/30/14   MJS     Fix for crash after client timeout waiting for enumeration.
04/22/14   MJS     Ensure framer and interface devices are enumerated before
                   unblocking client open calls.
04/21/14   MJS     Attempt to reset hardware when there is a timeout sending
                   mesasges.
04/09/14   MJS     Block client open calls until hardware is enumerated.
03/06/14   MJS     Fix race condition where event can be signalled before
                   it is returned to free queue.
02/28/14   MJS     Streamline ISR and don't lock device mutex in ISR.
02/25/14   MJS     Use same port and counter indexes for combined port DMA.
02/14/14   MJS     Fix klockwork issues.
02/12/14   jb      Marked inline functions as extern for LLVM
01/31/14   MJS     Use interrupt-based clock toggle detection not polling.
12/04/13   MJS     Support for purging any active channels during ADSP SSR.
11/22/13   DDK     Removing error fatal from the driver.
11/07/13   MJS     Support for MDM restart when master is inactive framer.
10/29/13   MJS     Store the clock gear being scheduled.
08/19/13   MJS     Support for combined port DMA interrupt.
07/31/13   MJS     Support for shadow counter, upper watermark and hardware
                   hardware optional capabilities.
07/25/13   MJS     Check for external GPIO toggle during hardware init.
06/17/13   MJS     Update offset for new HWIO.
06/17/13   MJS     Fix Klocwork warning.
06/14/13   dj      Fix to prevent accessing deleted shared channels.
05/14/13   MJS     SSR port and channel deallocation enhancements.
05/12/13   MJS     Fix for registering for SSR for satellite driver.
05/12/13   MJS     Ensure port interrupt is enabled after port is configured.
04/29/13   MJS     Ensure clock active until after enumeration.
04/22/13   MJS     Pushed/pulled protocol support.
04/22/13   MJS     Fix reserved message slot usage from master driver.
04/16/13   MJS     Support for retrying satellite ack responses.
04/12/13   dj      Updates for clock on and clock off returning error code
04/10/13   dj      Add fix for race condition surpriseremove dev with active request 
04/10/13   MJS     Add hack for programming BAM secure registers.
04/04/13   dj      Add check for tail index normalization
03/22/13   dj      Added error logging when SlimBus_EmptyBamIOVec returns error
03/21/13   MJS     Support for inactive framer.
03/21/13   MJS     Support for standalone satellite hardware configuration.
03/21/13   MJS     Allow shared channel to be allocated by multiple clients.
03/21/13   MJS     Cleanup low power and pause clock logic.
03/13/13   dj      Fix device open causing mismatched stop idle calls 
03/13/13   dj      Fix memcpy to use OS appropriate ones. 
03/11/13   MJS     Cancel reconfiguration sequence when nacked.
03/04/13   MJS     Lock device mutex during VE validation logging.
02/21/13   MJS     Fix check for port deallocation during device close.
02/21/13   MJS     Add port synchronization lock.
02/05/13   dj      Fix Code analysis warnings
01/09/13   MJS     Support more than 32 channels using dynamic allocation.
12/17/12   MJS     Satellite support for hardware power collapse.
12/09/12   MJS     Mask out overflow/underflow events during channel removal.
11/29/12   MJS     Support BAM deferred processing within client context.
11/29/12   MJS     Reset the message queue pipes during hardware
                   initialization.
11/21/12   MJS     Make sure port interrupt stays enabled during secondary
                   port disconnect.
11/13/12   MJS     Ensure client array is ZI during driver initialization.
11/08/12   MJS     Make sure device stays active with allocated ports.
11/06/12   MJS     Fix for SSR hardware init issue after role reversal.
11/01/12   dj      Add support for ddi
10/25/12   MJS     Support for role reversal.
10/23/12   MJS     Check for device open counter overflow.
10/01/12   MJS     Fix client open reference counting.
09/12/12   MJS     Disconnect ports that have been removed from shared 
                   channels when new ports are connected to the channels.
09/07/12   MJS     Add timeout while waiting for available transactions.
09/05/12   MJS     Refactor message queue "while" loops.
08/29/12   MJS     Fix issue where BAM EOT could not be set when expected
                   during Tx message transmission.
08/18/12   MJS     Wait to receive ACK for secondary disconnect message.
08/17/12   MJS     Signal BAM descriptor flush during port disconnection.
07/23/12   MJS     Fix issue with deallocation of half-duplex ports.
07/09/12   MJS     Wait for BAM descriptors to flush before removing channels.
06/26/12   MJS     Add final port disconnection bitmask for power management.
06/13/12   MJS     Support for standlone satellite low-power management.
06/07/12   MJS     Attempt to cleanup channels after master ack failures.
05/23/12   MJS     Strip out idle timer.
05/16/12   MJS     Update logging macros.
05/04/12   MJS     Relaxed validation for MSM EA lookup protection.
05/02/12   MJS     Update to reflect that watermark level was changed to 
                   be number of words instead of bytes.
05/01/12   MJS     Fix Klocwork warnings.
04/25/12   MJS     Slight restructure for QNX binary compatibility.
04/19/12   MJS     Enhanced logic for detecting initial channel activation.
04/23/12   MJS     Fix multi-channel port mask initialization.
04/17/12   MJS     Enhanced mempool tagging.
04/14/12   MJS     Add back GetPortEvent to common code.
04/11/12   MJS     Allow recovery from bad offsets within GetMsgBuffer.
04/11/12   MJS     Add common device dealloc routine.
04/10/12   MJS     Fix bug with incrementing uAvailTrans for Tx events.
04/09/12   MJS     Fix SAL warnings.
04/04/12   MJS     Reset BAM pipe after delay timer in workloop.
04/04/12   MJS     Fixes for level 4 warnings.
03/28/12   MJS     Compatibility with WPP logging.
03/07/12   MSJ     Fix compiler warnings.
02/27/12   MJS     Fix bug with incrementing uAvailTrans for Tx events.
02/27/12   MJS     Support event wait timeout/failure.
02/16/12   MJS     Don't block port deallocation when disconnection is
                   pending, fix late port disconnection issue.
02/07/12   MJS     Add reconfiguration sequence timeout mechanism.
01/25/12   MJS     Improve BAM driver abstraction.
01/23/12   MJS     Support for entering pause clock only after idle timeout.
01/17/12   MJS     Check for entering pause clock during timer callback.
01/06/12   MJS     Bug fix for InterruptAcquireLock dereference.
12/11/11   MJS     Remove references to DAL.
11/29/11   MJS     Add support for deallocating ports before the ports have
                   finished disconnecting.
11/15/11   MJS     Fix klocwork issues.
10/21/11   MJS     Detect channels being activated from reconfiguration
                   sequence and simulate master ack for the reconfig request.
10/19/11   MJS     Refinement of port disconnection logic.
10/12/11   AV      Added Slimbus SSR feature.
10/12/11   MJS     Don't disconnect port when corresponding channel is
                   pending removal.
10/04/11   MJS     Fix AllocSharedDataChannel resource allocation.
09/29/11   MJS     Make sure port is disconnected when ConfigMasterPort() or
                   ConfigMasterPipe() is called. 
09/27/11   MJS     Add client shared channel support.
09/26/11   MJS     Working pause clock implementation, report unsupported
                   Rx messages.
09/14/11   MJS     Fix double port disconnection issue when deallocating
                   a port while channel is being removed concurrently.
09/12/11   MJS     Improved logging for error conditions.
08/23/11   MJS     Implement alternate SW workaround for CR-0000156195,
                   only disconnect BAM after port is disconnected.
08/23/11   MJS     Fix for ExchangeValueElement() when writing zeros.
08/17/11   MJS     Remove synchronization from ReadProgressCounter API.
07/21/11   MJS     Make sure port interrupt stays enabled when waiting for
                   manual port disconnection.
07/19/11   MJS     Fix for spurious disconnection event, wait for manual
                   port disconnection to complete.
07/01/11   MJS     Clear port event handle on port deallocation.
06/17/11   AV      Added contiguous phys memory flag for memory alloc.
06/17/11   MJS     Check return code from DeviceInitTarget().
06/09/11   MJS     Support for shadow register initialization.
06/06/11   MJS     Cache port options, timeout for reenabling port int.
05/24/11   MJS     Add pause clock support and ref clock management.
05/24/11   MJS     Cleanup, add master-specific interrupt handling function,
                   remove hard-coded max client limit.
05/16/11   MJS     Add additional debug print statements.
05/10/11   MJS     Add port disconnection interrupt workaround.
05/10/11   MJS     Add support for slimbus master component data loopback.
05/07/11   MJS     Allow satellite clients to use LAs without doing EA lookups.
04/14/11   MJS     Add logging facility.
04/08/11   MJS     Implement workarounds for Waverider v1 and v2.
06/21/10   MJS     Initial revision.

===========================================================================
             Copyright (c) 2010-2017 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

#include "SlimBusTarget.h"
#include SLIMBUS_OS_H
#include "SlimBus.h"
#include "SlimBusConsts.h"
#include "SlimBusLists.h"
#include "SlimBusMaster.h"
#include "SlimBusSat.h"
#include "SlimBusBam.h"
#include SB_TRACE_HEADER(SlimBus)

SB_TEST_DEFINE();

#define COMMON_WORKAROUNDS (SB_WRKRND_O_PC_FIFO_SAMPLE_OFFSET|SB_WRKRND_O_RX_PORT_RESET|SB_WRKRND_O_IE_CLEAR|SB_WRKRND_O_SETUP_TRUST|SB_WRKRND_O_DATASHIFT_INVALIDATE)

/* Global variable to allow interactive override of log level */
int32 g_iSlimBusLogLevelOverride = SB_LOG_LEVEL_UNDEFINED;

/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/
static SBResult
SlimBus_ClientInit(SlimBusClientCtxt *pCtxt);

_IRQL_requires_(DISPATCH_LEVEL) static void
SlimBus_DeallocPort(SlimBusClientCtxt *pCtxt, SlimBusMasterPortType *pPort);

/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/

/**
 * @brief Get pointer to the first channel data structure, 
 *        either from the resource group or from the singleton
 *        handle
 *
 * This function gets a pointer to the first channel data 
 * structure, based on the input passed in.  If the hRes 
 * parameter is a resource group, the first structure in the 
 * group will be returned, if the hRes parameter is a singleton 
 * channel handle, the channel data structure will be returned. 
 * If a resource group handle was passed in, a pointer to the 
 * resource group data structure will also be returned. 
 * 
 * @param[in] pClientCtxt  Pointer to the client context
 * @param[in] hRes  Resource handle, either channel or group
 * @param[out] ppGroup  Pointer to pointer to group structure
 * 
 * @return Pointer to the first channel structure
 */
SLIMBUS_INLINE SlimBusChannelType *GetChannelHandle(SlimBusClientCtxt *pClientCtxt, SlimBusResourceHandle hRes, SlimBusGroupType **ppGroup )
{
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  uint32 idx;
  SlimBusHandleType eType;
  SlimBusGroupType *pGroup = NULL;
  SlimBusChannelType *pChannel = NULL;
  
  eType = SB_GET_HANDLE_TYPE(hRes);
  idx = SB_GET_HANDLE_IDX(hRes);
  
  if ( eType == SB_HANDLE_TYPE_CHANNEL )
  {
    if ( idx < pDevCtxt->numChannelSlots &&
         idx < sizeof(pDevCtxt->pChannels)/sizeof(pDevCtxt->pChannels[0]) &&
         NULL != pDevCtxt->pChannels[idx] &&
         pDevCtxt->pChannels[idx]->handle == hRes )
    {
      pChannel = pDevCtxt->pChannels[idx];
    }
  }
  else if ( eType == SB_HANDLE_TYPE_SHARED_CHANNEL )
  {
    if ( idx < pDevCtxt->numSharedChannelSlots &&
         NULL != pClientCtxt->paSharedChannels &&
         NULL != pClientCtxt->paSharedChannels[idx] &&
         pClientCtxt->paSharedChannels[idx]->handle == hRes )
    {
      pChannel = pClientCtxt->paSharedChannels[idx];
    }
  }
  else if ( eType == SB_HANDLE_TYPE_GROUP )
  {
    if ( NULL != ppGroup &&
         idx < pDevCtxt->numGroupSlots &&
         idx < sizeof(pDevCtxt->Groups)/sizeof(pDevCtxt->Groups[0]) &&
         pDevCtxt->Groups[idx].handle == hRes )
    {
      pGroup = &pDevCtxt->Groups[idx];
      pChannel = NextChannelNode(&pGroup->ResourceList, NULL);
      if ( NULL == pChannel ||
           ( SB_HANDLE_TYPE_CHANNEL != SB_GET_HANDLE_TYPE(pChannel->handle) &&
             SB_HANDLE_TYPE_SHARED_CHANNEL != SB_GET_HANDLE_TYPE(pChannel->handle) ) )
      {
        pChannel = NULL;
        pGroup = NULL;
      }
    }
  }
  
  if ( NULL != ppGroup )
  {
    *ppGroup = pGroup;
  }
  return pChannel;
}

/**
 * @brief Get the next pseudo-random number in the sequence
 *
   This function gets the next "pseudo-random" number in the 
   sequence, currently just an incremental value. 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return The next pseudo-random number in the sequence
 */
SLIMBUS_INLINE uint8 GetPrn(SlimBusDevCtxt *pDevCtxt)
{
  uint8 prn = pDevCtxt->prnSeed;

  pDevCtxt->prnSeed = (pDevCtxt->prnSeed + 1) % 256;
  
  return prn;
}

/**
 * @brief Enable interrupt for a port
 *
 * This function enables the interrupt for a port, and as 
 * necessary, clears the interrupt disabled flag for the port 
 * and disables the reconfig_done interrupt enable. 
 * 
 * @param pDevCtxt  Pointer to the device context
 * @param uIntMask  Bitmask of ports to enable
 */
void EnablePortInterrupt(SlimBusDevCtxt *pDevCtxt, uint32 uIntMask)
{
  uint32 uMask, uToken;
  SlimBus_InterruptAcquireLock(pDevCtxt, &uToken);
  uMask = HAL_sb_PgdPortGetIntEnEe(pDevCtxt->hSB, 0, pDevCtxt->pBSP->uMyEE);
  uMask |= uIntMask;
  HAL_sb_PgdPortSetIntEnEe(pDevCtxt->hSB, 0, pDevCtxt->pBSP->uMyEE, uMask);
  SlimBus_InterruptReleaseLock(pDevCtxt, &uToken);
}

/**
 * @brief Disable interrupt for a port
 *
 * This function disables the interrupt for a port, setting the 
 * interrupt disabled flag for the port, and enabling the 
 * reconfig_done interrupt if needed.
 * 
 * @param pDevCtxt  Pointer to the device context
 * @param uIntMask  Bitmask of ports to disable
 */
void DisablePortInterrupt(SlimBusDevCtxt *pDevCtxt, uint32 uIntMask)
{
  uint32 uMask, uToken;
  SlimBus_InterruptAcquireLock(pDevCtxt, &uToken);
  uMask = HAL_sb_PgdPortGetIntEnEe(pDevCtxt->hSB, 0, pDevCtxt->pBSP->uMyEE);
  uMask &= ~uIntMask;
  HAL_sb_PgdPortSetIntEnEe(pDevCtxt->hSB, 0, pDevCtxt->pBSP->uMyEE, uMask);
  SlimBus_InterruptReleaseLock(pDevCtxt, &uToken);
}
/**
 * @brief Wait until the destination device is able to receive a 
 *        new non-posted transaction
 *
 * This function waits until the destination device is able to 
 * receive a new non-posted message transaction.  If the device 
 * is already ready to receive a new transaction, the available 
 * transaction count will be decreased and the function will 
 * return immediately. 
 * 
 * @param[in] pClientCtxt  Pointer to the client context
 * @param[in] pDevice  Pointer to the native SLIMbus device 
 *                 structure
 * 
 * @return SB_SUCCESS on success, an error code on error
 */
_IRQL_requires_(DISPATCH_LEVEL)
static SBResult
SlimBus_WaitForAvailDeviceTrans(SlimBusClientCtxt *pClientCtxt, SlimBusDeviceType *pDevice)
{
  SBResult result = SB_SUCCESS;
  SlimBusEventNodeType *pEventNode = NULL;

  if (NULL == pClientCtxt || NULL == pDevice )
  {
    return SB_SUCCESS;
  }
  
  if (pDevice->uAvailTrans == 0)
  {
    SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);

    pEventNode = PopHeadEvent(&pClientCtxt->FreeEventStack);
    if (NULL == pEventNode)
    {
      SB_LOG_ERR_1("No free events while waiting for available transaction (client 0x%lx)", (uint32)pClientCtxt);
      return SB_ERROR;
    }

    pEventNode->hEvent = pClientCtxt->hClientEvent;
    pEventNode->peMsgErr = &pClientCtxt->eLastMsgErr;

    /* Create event handle again to grab current thread ID */
    SlimBus_EventReset(pClientCtxt->hClientEvent);
    
    while (pDevice->uAvailTrans == 0)
    {
      PushTailEvent(&pDevice->TransEventQueue, pEventNode);

      SlimBus_UnlockDevice(pDevCtxt);

      result = SlimBus_EventTimedWait(pClientCtxt,
                                      pEventNode->hEvent,
                                      SB_DEFAULT_RX_TIMEOUT);

      SlimBus_LockDevice(pDevCtxt);

      if (SB_SUCCESS != result)
      {
        SB_LOG_ERR_2("Timeout while waiting for available transactions (client 0x%lx) (LA: 0x%x)",
                     (uint32)pClientCtxt, pDevice->LA);
        break;
      }
    }

    PushHeadEvent(pEventNode->pFreeRoot, pEventNode);
  }
  
  return result;  
}

/**
 * @brief Wait until the bus is active and not in a pause state
 *
 * This function waits until the SLIMbus is active and not in
 * a pause state.
 * 
 * @param[in] pClientCtxt  Pointer to the client context
 * @param[in] pDevice  Pointer to the native SLIMbus device 
 *                 structure
 * 
 * @return SB_SUCCESS on success, an error code on error
 */
_IRQL_requires_(DISPATCH_LEVEL)
static SBResult
SlimBus_WaitForBusActive(SlimBusClientCtxt *pClientCtxt, SlimBusDeviceType *pDevice)
{
  SBResult result = SB_SUCCESS;
  SlimBusEventNodeType *pEventNode = NULL;
  SlimBusDevCtxt *pDevCtxt;
  
  if (NULL == pClientCtxt || NULL == pDevice )
  {
    return SB_SUCCESS;
  }
  
  pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);

  if (NULL != pDevCtxt->pSched &&
      SB_PAUSE_NOT_PAUSED != pDevCtxt->pSched->ePause )
  {
    pEventNode = PopHeadEvent(&pClientCtxt->FreeEventStack);
    if (NULL == pEventNode)
    {
      SB_LOG_ERR_1("No free events while waiting for bus active (client 0x%lx)", (uint32)pClientCtxt);
      return SB_ERROR;
    }

    /* Create event handle again to grab current thread ID */
    SlimBus_EventReset(pClientCtxt->hClientEvent);

    pEventNode->hEvent = pClientCtxt->hClientEvent;
    PushTailEvent(&pDevCtxt->PauseEventQueue, pEventNode);
    SB_LOG_INFO_1("Waiting for resume from pause clock to send message (LA: 0x%x)",
                  pDevice->LA);

    SlimBus_UnlockDevice(pDevCtxt);

    result = SlimBus_EventTimedWait(pClientCtxt,
                                    pEventNode->hEvent,
                                    SB_DEFAULT_CLIENT_TIMEOUT);

    SlimBus_LockDevice(pDevCtxt);

    /* On success, Rx queue processing logic will remove the event from queue */
    if (SB_SUCCESS != result)
    {
      SB_LOG_ERR_2("Timeout while waiting for bus active (client 0x%lx) (LA: 0x%x)",
                   (uint32)pClientCtxt, pDevice->LA);
    }
    
    SB_LOG_INFO_1("Resuming message send after resume from pause clock (LA: 0x%x)",
                  pDevice->LA);
    PushHeadEvent(pEventNode->pFreeRoot, pEventNode);
  }
  
  return result;  
}

/**
 * @brief Wait until the message FIFO is not busy
 *
 * This function waits until the message FIFO is not busy. 
 * 
 * @param[in] pClientCtxt  Pointer to the client context
 * @param[in] pMsgFifo  Pointer to the message FIFO
 * 
 * @return SB_SUCCESS on success, an error code on error
 */
_IRQL_requires_(DISPATCH_LEVEL)
static SBResult
SlimBus_WaitForMsgFifoNotBusy(SlimBusClientCtxt *pClientCtxt, SlimBusMsgFifoType *pMsgFifo)
{
  SBResult result = SB_SUCCESS;
  SlimBusEventNodeType *pEventNode = NULL;
  SlimBusDevCtxt *pDevCtxt;
  
  if (NULL == pClientCtxt || NULL == pMsgFifo || SB_ACK_IDX_UNUSED == pMsgFifo->uAckIdx )
  {
    return SB_ERROR;
  }
  
  pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);

  pEventNode = PopHeadEvent(&pClientCtxt->FreeEventStack);
  if (NULL == pEventNode)
  {
    SB_LOG_ERR_1("No free events while waiting for Fifo not busy (client 0x%lx)", (uint32)pClientCtxt);
    return SB_ERROR;
  }

  /* Create event handle again to grab current thread ID */
  SlimBus_EventReset(pClientCtxt->hClientEvent);

  pEventNode->hEvent = pClientCtxt->hClientEvent;
  PushTailEvent(&pMsgFifo->WaitList, pEventNode);

  SlimBus_UnlockDevice(pDevCtxt);

  result = SlimBus_EventTimedWait(pClientCtxt,
                                  pEventNode->hEvent,
                                  SB_DEFAULT_CLIENT_TIMEOUT);

  SlimBus_LockDevice(pDevCtxt);

  if (SB_SUCCESS != result)
  {
    SB_LOG_ERR_1("Timeout while waiting for Fifo not busy (client 0x%lx)", (uint32)pClientCtxt);
  }

  PushHeadEvent(pEventNode->pFreeRoot, pEventNode);
  
  return result;  
}

/**
 * @brief Submit descriptors to the Rx message queue FIFO
 *
 * This function submits descriptors to the Rx message queue 
 * FIFO, until a descriptor has been submitted for all the bytes
 * in the message FIFO, or the descriptor FIFO is full.
 * 
 * @param[in] pMsgFifo  Pointer to the message FIFO
 * 
 * @return  Number of descriptors submitted to the FIFO
 */
uint32 SlimBus_SubmitRxDescriptors( SlimBusMsgFifoType *pMsgFifo )
{
  SBResult result = SB_SUCCESS;
  const uint32 uDefaultLen = SB_RX_MSG_DESC_SIZE;
  SlimBusMsgIdxType uDescIdx;
  /* uLen will be power of 2 so subtract 1 to get mask */
  uint32 uLenMask = pMsgFifo->uLen-1;
  uint32 uDescCnt = 0;

  /* Take the opportunity to normalize the indexes */
  if (pMsgFifo->uTailIdx >= pMsgFifo->uLen)
  {
    SlimBusMsgIdxType uBase = pMsgFifo->uTailIdx&~uLenMask;
    if (uBase > pMsgFifo->uHeadIdx)
    {
      /* This should never be true */
      uBase = pMsgFifo->uHeadIdx&~uLenMask;
    }
    if (uBase > pMsgFifo->uAckIdx)
    {
      /* This should never be true */
      uBase = pMsgFifo->uAckIdx&~uLenMask;
    }
    pMsgFifo->uTailIdx -= uBase;
    pMsgFifo->uHeadIdx -= uBase;
    pMsgFifo->uAckIdx -= uBase;
  }

  uDescIdx = pMsgFifo->uAckIdx;
  /* Prevent submitting IO vectors to BAM that would be rejected
     (can only submit up to one less descriptor that the max queue size) */
  while (uDescIdx + uDefaultLen*2 < pMsgFifo->uTailIdx + pMsgFifo->uLen )
  {
    SlimBusMsgIdxType uNewDescIdx = uDescIdx + uDefaultLen;
    if ((uNewDescIdx&~uLenMask) > (uDescIdx&~uLenMask))
    {
      uNewDescIdx = uNewDescIdx&~uLenMask;
    }
    if (uNewDescIdx >= pMsgFifo->uTailIdx + pMsgFifo->uLen)
    {
      /* subtract one to make sure head doesn't overtake tail */
      uNewDescIdx = pMsgFifo->uTailIdx + pMsgFifo->uLen - 1;
    }
    if (uDefaultLen > 1)
    {
      SlimBus_MemoryBarrier(&pMsgFifo->pBuf[uDescIdx&uLenMask]);
    }
    result = SlimBus_BamTransferIOVec(pMsgFifo->pBamCtxt,
                                      (size_t)&pMsgFifo->pPhysBuf[uDescIdx&uLenMask],
                                      (uint32)(uNewDescIdx-uDescIdx)*sizeof(uint32), NULL, 
                                      SLIMBUS_BAM_IOVEC_FLAG_INT);
    if (SB_SUCCESS != result)
    {
      SB_LOG_DECLARE_PDEVCTXT(SB_GET_PDEVCTXT(pMsgFifo->hDevCtxt));
      SB_LOG_ERR_4("Error transferring Rx iovec (uNewDescIdx: 0x%lx) (uDescIdx: 0x%lx) (uTailIdx: 0x%lx) (uHeadIdx: 0x%lx)",
                   (uint32)uNewDescIdx, (uint32)uDescIdx, 
                   (uint32)pMsgFifo->uTailIdx, (uint32)pMsgFifo->uHeadIdx);
      break;
    }

    /* Update ack index now that we know the descriptor was accepted */
    pMsgFifo->uAckIdx = uDescIdx = uNewDescIdx;
    uDescCnt++;
  }
  
  return uDescCnt;
}

/**
 * @brief Allocate space for the next available message in a 
 *        message FIFO
 *
 * This function allocates space for the next available message 
 * in a message FIFO 
 * 
 * @param[in] pClientCtxt  Pointer to the client context
 * @param[in] pDevice  Pointer to the Slimbus native device
 * @param[in] pMsgFifo  Pointer to the message FIFO
 * @param[in] uNumBytes  Number of bytes in the message to 
 *       allocate
 * 
 * @return Pointer to the space allocated for the message 
 */
uint32 *SlimBus_GetMsgBuffer
(
  SlimBusClientCtxt *pClientCtxt,
  SlimBusDeviceType *pDevice,
  SlimBusMsgFifoType *pMsgFifo,
  uint32 uNumBytes
)
{
  uint32 uWordSize = (uNumBytes+(sizeof(uint32)-1))/sizeof(uint32);
  uint32 uLenMask;
  SlimBusMsgIdxType uTailIdx, uOldHeadIdx;
  uint32 *pBuf;
  SB_LOG_DECLARE_PDEVCTXT(SB_GET_PDEVCTXT((NULL==pMsgFifo)?(NULL):(pMsgFifo->hDevCtxt)));
  
  UNREFERENCED_PARAMETER(pDevice);

  if (NULL == pMsgFifo || NULL == pMsgFifo->pBuf || SB_MAX_MSG_LENGTH_BYTES < uNumBytes)
  {
    if (NULL != pMsgFifo)
    {
      SB_LOG_ERR_3("Invalid params to getmsgbuffer (pMsgFifo: 0x%lx) (pBuf: 0x%lx) "
                   "(uNumBytes: 0x%lx)", (uint32)pMsgFifo, (uint32)pMsgFifo->pBuf, uNumBytes);
    }
    return NULL;
  }

  uLenMask = pMsgFifo->uLen-1;
  uTailIdx = pMsgFifo->uAckIdx;
  if (SB_ACK_IDX_UNUSED == pMsgFifo->uAckIdx)
  {
    uTailIdx = pMsgFifo->uTailIdx;
  }

  /* Normalize the indexes: if all indexes are greater than or equal to the
     FIFO length, then subtract the FIFO length from the indexes */
  if (uTailIdx & ~uLenMask)
  {
    SlimBusMsgIdxType uSubtract;
    uint32 uToken;
    SlimBus_InterruptAcquireLock(SB_GET_PDEVCTXT(pMsgFifo->hDevCtxt), &uToken);
    uSubtract = uTailIdx & ~uLenMask;
    if (uSubtract > pMsgFifo->uHeadIdx)
    {
      /* This should never be true */
      uSubtract = pMsgFifo->uHeadIdx&~uLenMask;
    }
    if (uSubtract > pMsgFifo->uAckIdx)
    {
      /* This should never be true */
      uSubtract = pMsgFifo->uAckIdx&~uLenMask;
    }
    if (uSubtract > pMsgFifo->uTailIdx)
    {
      /* This should never be true */
      uSubtract = pMsgFifo->uTailIdx&~uLenMask;
    }

    pMsgFifo->uHeadIdx -= uSubtract;
    pMsgFifo->uTailIdx -= uSubtract;
    uTailIdx -= uSubtract;
    if (SB_ACK_IDX_UNUSED != pMsgFifo->uAckIdx)
    {
      pMsgFifo->uAckIdx -= uSubtract;
    }
    SlimBus_InterruptReleaseLock(SB_GET_PDEVCTXT(pMsgFifo->hDevCtxt), &uToken);
  }
  
  while (pMsgFifo->uHeadIdx + uWordSize >= uTailIdx + pMsgFifo->uLen)
  {
    SBResult result = SB_ERROR;

    if (SB_ACK_IDX_UNUSED != pMsgFifo->uAckIdx)
    {
      result = SlimBus_WaitForMsgFifoNotBusy(pClientCtxt, pMsgFifo);

      uTailIdx = pMsgFifo->uAckIdx;
    }

    if (SB_SUCCESS != result)
    {
      /* error: out of buffer space */
      SB_LOG_ERR_3("No buffer space in queue (uHeadIdx: 0x%llx) (uTailIdx: 0x%llx) (uLen: 0x%lx)",
                   pMsgFifo->uHeadIdx, uTailIdx, pMsgFifo->uLen);
      return NULL;
    }
  }
  
  uOldHeadIdx = pMsgFifo->uHeadIdx;
  pBuf = &pMsgFifo->pBuf[pMsgFifo->uHeadIdx&uLenMask];
  pMsgFifo->uHeadIdx += uWordSize;

  if ( (uOldHeadIdx&~uLenMask) < (pMsgFifo->uHeadIdx&~uLenMask) )
  {
    pMsgFifo->uOverhang = pMsgFifo->uHeadIdx&uLenMask;
  }

  return pBuf;
}

/**
 * @brief Allocate space the next available message in a Tx 
 *        message FIFO
 *
 * This function allocates space for the next available message 
 * in a Tx message FIFO.  If the destination device is not ready 
 * to receive another non-posted transaction, this function will
 * block until another transaction ID is available. 
 * 
 * @param[in] pClientCtxt  Pointer to the client context
 * @param[in] pDevice  Pointer to the Slimbus native device
 * @param[in] pMsgFifo  Pointer to the message FIFO
 * @param[in] uNumBytes  Number of bytes in the message to 
 *       allocate
 * 
 * @return Pointer to the space allocated for the message 
 */
uint32 *SlimBus_GetTxMsgBuffer
(
  SlimBusClientCtxt *pClientCtxt,
  SlimBusDeviceType *pDevice,
  SlimBusMsgFifoType *pMsgFifo,
  uint32 uNumBytes
)
{
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pMsgFifo->hDevCtxt);
  SBResult result = SB_SUCCESS;
  
  /* Indicate that this client is waiting to send a Tx message */
  pDevCtxt->uNumTxMsgWaiters++;
  
  do
  {
    /* TODO: only block for read ops */
    /* For both read and write operations need to verify there is an available
       transaction, can't do a write op while the last read op is ongoing */
    result = SlimBus_WaitForAvailDeviceTrans(pClientCtxt, pDevice);
    if (SB_SUCCESS != result)
    {
      break;
    }
  
    result = pDevCtxt->leaveLowPowerFcn(pDevCtxt);
    if (SB_SUCCESS != result)
    {
      break;
    }
    
    /* Don't send any Tx messages until the bus is in an active state, to
       avoid sending messages across a clock pause boundary or when a device
       has lost message sync after clock pause */    
    result = SlimBus_WaitForBusActive(pClientCtxt, pDevice);
    if (SB_SUCCESS != result)
    {
      break;
    }
  } while (FALSE);

  /* Indicate that this client is proceeding to send a Tx message and
     is no longer waiting */
  pDevCtxt->uNumTxMsgWaiters--;
  
  if (SB_SUCCESS == result)
  {
    return SlimBus_GetMsgBuffer(pClientCtxt, pDevice, pMsgFifo, uNumBytes);
  }
  else
  {
    return NULL;
  }
}


/**
 * @brief Send the next (oldest) message in a Tx message FIFO 
 *        using CSR access
 *
 * This function sends the next (oldest) message in a Tx message
 * FIFO, using CSR access.  If the Tx CSR messaging interface is
 * busy sending a hardware generated message, the function will 
 * wait until the messaging interface is ready. 
 * 
 * @param[in] pDevCtxt  Pointer to the SLIMbus device
 * @param[in] pMsgFifo  Pointer to the message FIFO structure
 */
static void SlimBus_SendTxBuffer
(
  SlimBusDevCtxt *pDevCtxt,
  SlimBusMsgFifoType *pMsgFifo
)
{
  uint32 *pBuf, uLen;
  
  if ( !pMsgFifo->bBusy )
  {
    if ( NULL != (pBuf = SlimBus_PeekBuffer(pMsgFifo, &uLen)) )
    {
      pMsgFifo->bBusy = TRUE;
  
      while ( HAL_sb_DeviceIsTxMsgBufferBusy(pDevCtxt->hSB, pDevCtxt->eMsgDev) )
      {
        SLIMBUS_BUSYWAIT(1);
      }
      HAL_sb_DeviceTxMsgWrite(pDevCtxt->hSB, pDevCtxt->eMsgDev, pBuf, uLen);
    }
  }
}


/**
 * @brief Send the next (oldest) message in a Tx message FIFO 
 *        using the BAM
 *
 * This function sends the next (oldest) message in a Tx message 
 * FIFO, using the BAM. If the interface is busy (there is a BAM
 * descriptor in flight with the EOT flag set), no new message 
 * will be sent, instead messages would get sent later when the 
 * ISR detects that the descriptor with EOT has been sent has 
 * been processed. 
 * 
 * @param[in] pDevCtxt  Pointer to the SLIMbus device
 * @param[in] pMsgFifo  Pointer to the message FIFO structure
 */
void SlimBus_SendTxBamBuffer
(
  SlimBusDevCtxt *pDevCtxt,
  SlimBusMsgFifoType *pMsgFifo
)
{
  SBBOOL bNotify = FALSE;
  SBResult result = SB_SUCCESS;
  
  if ( !pMsgFifo->bBusy )
  {
    uint32 uLenMask = pMsgFifo->uLen-1;
    SlimBusMsgIdxType uHeadIdx = pMsgFifo->uHeadIdx;
    SlimBusEventNodeType *pEvent = NULL;
    
    if (pMsgFifo->uTailIdx + pMsgFifo->uLen < pMsgFifo->uHeadIdx)
    {
      /* Some bad corruption must have occured */
      SB_LOG_ERR_2("Corruption of MsgFifo indicies detected during send "
                   "(uTailIdx: 0x%lx) (uHeadIdx: 0x%lx)",
                   (uint32)pMsgFifo->uTailIdx, (uint32)pMsgFifo->uHeadIdx);
      pMsgFifo->uTailIdx = pMsgFifo->uHeadIdx;
    }
    if (pMsgFifo->uAckIdx < pMsgFifo->uTailIdx)
    {
      if (SB_SUCCESS != SlimBus_EmptyBamIOVec(pMsgFifo))
      {
        SB_LOG_ERR_0("SlimBus_EmptyBamIOVec returned error!");
      }
    }
    if ( ( SB_MASTER_DRIVER == pDevCtxt->eDrvType && !pDevCtxt->pSched->bDataSlotsScheduled ) ||
         ( 0 == pDevCtxt->uOpenCnt ) )
    {
      bNotify = TRUE;
    }
    if ( NULL != (pEvent = PeekHeadEvent(&pDevCtxt->TxEventQueue)) )
    {
      SlimBusMsgIdxType uDescIdx = pEvent->pTxBuf - pMsgFifo->pBuf;
      uint32 length = SB_MSG_GET(*pEvent->pTxBuf,GENERIC_0,LEN);
      uint32 uWordLen = (length+sizeof(uint32)-1)/sizeof(uint32);
      /* Normalize uDescIdx, it should be >= the tail */
      SlimBus_NormalizeIndexMinMax(&uDescIdx, pMsgFifo->uTailIdx,
                                   pMsgFifo->uHeadIdx, pMsgFifo->uLen);
      if (uDescIdx >= pMsgFifo->uTailIdx)
      {
        uHeadIdx = uDescIdx + uWordLen;
        bNotify = TRUE;
      }
    }
    if ( ( pMsgFifo->uHeadIdx > pMsgFifo->uAckIdx + pMsgFifo->uLen/2 ) ||
         ( SlimBus_BamGetIOVecFreeSlots(pMsgFifo->pBamCtxt) <= 2 ) )
    {
      bNotify = TRUE;
    }
    while ( pMsgFifo->uTailIdx < uHeadIdx )
    {
      SlimBusMsgIdxType uNewTailIdx = uHeadIdx;
      uint32 uFlags = 0;
      if ( (pMsgFifo->uTailIdx&~uLenMask) < (uNewTailIdx&~uLenMask) )
      {
        uNewTailIdx = (pMsgFifo->uTailIdx&~uLenMask) + pMsgFifo->uLen + pMsgFifo->uOverhang;
        /* don't set uOverhang to 0 in case we need to unwind the descriptor */
      }
      if ( bNotify &&
           uNewTailIdx == uHeadIdx )
      {
        uFlags = SLIMBUS_BAM_IOVEC_FLAG_EOT;
      }
      
      SlimBus_MemoryBarrier(&pMsgFifo->pBuf[pMsgFifo->uTailIdx&uLenMask]);
      result = SlimBus_BamTransferIOVec(pMsgFifo->pBamCtxt,
                                        (size_t)&pMsgFifo->pPhysBuf[pMsgFifo->uTailIdx&uLenMask],
                                        (uint32)(uNewTailIdx-pMsgFifo->uTailIdx)*sizeof(uint32),
                                        NULL, uFlags);
      if (SB_SUCCESS != result)
      {
        /* If TransferOne() fails, try again later when there may be more 
           descriptor FIFO space */
        SB_LOG_WARN_2("Transfer Tx IO vector returned error (uNewTailIdx: 0x%lx) (uTailIdx: 0x%lx)",
                      (uint32)uNewTailIdx, (uint32)pMsgFifo->uTailIdx);
        return;
      }
      
      /* Update the tail index now that we know the descriptor was accepted */      
      pMsgFifo->uTailIdx = uNewTailIdx;

      if ( SLIMBUS_BAM_IOVEC_FLAG_EOT & uFlags )
      {
        pMsgFifo->bBusy = TRUE;
      }
    }
  }
}


/**
 * @brief Stub for "sending" the next message in a message FIFO
 *
 * This function is a stub to handle "sending" the next message 
 * in a non-Tx message FIFO, no action is performed for 
 * "sending". 
 * 
 * @param[in] pDevCtxt  Pointer to the SLIMbus device
 * @param[in] pMsgFifo  Pointer to the message FIFO structure 
 */
void SlimBus_DefaultSendBuffer
(
  SlimBusDevCtxt *pDevCtxt,
  SlimBusMsgFifoType *pMsgFifo
)
{
  UNREFERENCED_PARAMETER(pDevCtxt);
  UNREFERENCED_PARAMETER(pMsgFifo);

  /* do nothing */
}

/**
 * @brief Peek the oldest message from a message FIFO
 *
 * This function peeks the oldest message (from the tail) of a 
 * message FIFO. 
 * 
 * @param[in] pMsgFifo  Pointer to the message FIFO
 * @param[out] puNumWords  The number of words in the message 
 * 
 * @return  Pointer to the peeked message 
 */
uint32* SlimBus_PeekBuffer
(
  SlimBusMsgFifoType *pMsgFifo,
  uint32 *puNumWords
)
{
  uint32 uNumWords = 0, length = 0;
  uint32 uLenMask;
  uint32 *pBuf = NULL;

  /* uLen is power of 2 so subtract 1 to get the mask */
  uLenMask = pMsgFifo->uLen-1;
  if ( pMsgFifo->uTailIdx + pMsgFifo->uLen < pMsgFifo->uHeadIdx )
  {
    /* Some bad corruption must have occured */
    SB_LOG_DECLARE_PDEVCTXT(SB_GET_PDEVCTXT(pMsgFifo->hDevCtxt));
    SB_LOG_ERR_2("Corruption of MsgFifo indicies detected during peek "
                 "(uTailIdx: 0x%lx) (uHeadIdx: 0x%lx)",
                 (uint32)pMsgFifo->uTailIdx, (uint32)pMsgFifo->uHeadIdx);
    pMsgFifo->uTailIdx = pMsgFifo->uHeadIdx;
  }
  while ( pMsgFifo->uTailIdx < pMsgFifo->uHeadIdx )
  {
    length = SB_MSG_GET(pMsgFifo->pBuf[pMsgFifo->uTailIdx&uLenMask],GENERIC_0,LEN);
    if (length > SB_MAX_MSG_LENGTH_BYTES)
    {
      /* TODO: error condition */
      break;
    }
    if ( length > 0 )
    {
      uNumWords = (length+(sizeof(uint32)-1))/sizeof(uint32);
      if (pMsgFifo->uTailIdx + uNumWords <= pMsgFifo->uHeadIdx)
      {
        if ( (pMsgFifo->eFlags & SB_MSG_FIFO_USE_RX_OVERHANG) &&
             ((pMsgFifo->uTailIdx+uNumWords)&~uLenMask) > (pMsgFifo->uTailIdx&~uLenMask) )
        {
          uint32 uOverhang = (pMsgFifo->uTailIdx+uNumWords)&uLenMask;
          SlimBus_Memcpy(pMsgFifo->pBuf+pMsgFifo->uLen, pMsgFifo->pBuf,
                         uOverhang*sizeof(uint32));
        }
        pBuf = &pMsgFifo->pBuf[pMsgFifo->uTailIdx&uLenMask];
      }
      else
      {
        uNumWords = 0;
      }
      break;
    }
    pMsgFifo->uTailIdx++;
  }
  
  if (NULL != puNumWords)
  {
    *puNumWords = uNumWords;
  }
  
  return pBuf;
}

/**
 * @brief Enqueues a message event for a Tx message needing
 *        completion notification
 *
 * This function enqueues a message event for a Tx message that 
 * needs completion notification. The calling function should 
 * verify that there is an available event structure before 
 * calling this function. 
 * 
 * @param pMsgFifo[in]  Pointer to the message FIFO
 * @param pClientCtxt[in]  Pointer to the client context
 * @param pBuf[in]  Pointer to the message buffer needing 
 *            acknowledgement
 * @param hEvent[in]  Event handle that will receive 
 *              notification
 * 
 * @return  Pointer to the event structure enqueued
 */
SlimBusEventNodeType *SlimBus_EnqueueMsgEvent
(
  SlimBusMsgFifoType *pMsgFifo,
  SlimBusClientCtxt *pClientCtxt,
  uint32 *pBuf,
  SlimBusEventHandle hEvent
)
{
  SlimBusEventNodeType *pEvent = NULL;
  if (pMsgFifo->eFlags & SB_MSG_FIFO_ENABLE_NOTIFY)
  {
    pEvent = PopHeadEvent(&pClientCtxt->FreeEventStack);
    if (NULL != pEvent)
    {
      pEvent->hEvent = hEvent;
      pEvent->pTxBuf = pBuf;
      pEvent->pTxPhysBuf = NULL;
      pEvent->peMsgErr = &pClientCtxt->eLastMsgErr;
      PushTailEvent(&SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt)->TxEventQueue, pEvent);
    }
    else
    {
      SB_LOG_DECLARE_PDEVCTXT(SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt));
      SB_LOG_ERR_1("No free events during EnqueueMsgEvent (client 0x%lx)", (uint32)pClientCtxt);
    }
  }
  return pEvent;
}

/**
 * @brief Is sending of a message in a FIFO complete
 *
 * This function determines if the sending of a message in a 
 * FIFO has completed. 
 * 
 * @param[in] pMsgFifo  Pointer to the message FIFO structure 
 * @param[in] pBuf  Pointer to message to check for completion 
 */
SLIMBUS_INLINE SBBOOL SlimBus_IsMsgSendComplete
(
  SlimBusMsgFifoType *pMsgFifo,
  uint32 *pBuf
)
{
  SlimBusMsgIdxType uIdx = pBuf - pMsgFifo->pBuf;
  if ( NULL == pBuf)
  {
    return TRUE;
  }
  if ( pBuf < pMsgFifo->pBuf || 
       pBuf >= pMsgFifo->pBuf + pMsgFifo->uLen)
  {
    return FALSE;
  }
  
  /* Normalize the index, the index must always be >= the AckIdx */
  while (uIdx < pMsgFifo->uAckIdx)
  {
    uIdx += pMsgFifo->uLen;
  }
  return ( uIdx < pMsgFifo->uTailIdx );
}

/**
 * @brief Allocate a new SLIMbus native device structure
 *
 * This function allocates a new SLIMbus native device structure
 * 
 * @return Pointer to the allocated device strucuture 
 */ 
SlimBusDeviceType* SlimBus_NewDeviceStruct( void )
{
  SlimBusDeviceType *pDevice = NULL;
  
  SlimBus_Malloc(sizeof(SlimBusDeviceType), (void**)&pDevice, SB_MEMTAG_1);
  if (NULL != pDevice)
  {
    memset(pDevice, 0, sizeof(*pDevice));
    RootNodeInit(&pDevice->TransEventQueue);
    /* assume just 1 max transactions per device */
    pDevice->uMaxTrans = 1;
    pDevice->uAvailTrans = 1;
    pDevice->uDataLineMask = 0x01;
    pDevice->uNextTIDIdx = SB_MAX_TIDS-1;
  }
  
  return pDevice;
}

/**
 * @brief Get SLIMbus native device structure pointer
 *
 * This function retrieves the SLIMbus native device structure 
 * pointer corresponding to the logical address passed as 
 * parameter.
 * 
 * @param pDevCtxt  Pointer to the device context
 * @param LA  Logical address
 * 
 * @return Pointer to SLIMbus native device structure
 */
SlimBusDeviceType* SlimBus_GetDevicePtr
(
  SlimBusDevCtxt *pDevCtxt,
  SlimBusLogicalAddrType LA
)
{
  SlimBusDeviceType *pDevice = pDevCtxt->pDevices[LA&SB_LA_IDX_MASK];

  if ( NULL == pDevice && 
       SB_SATELLITE_DRIVER == pDevCtxt->eDrvType )
  {
    /* If we are the satellite driver, and we haven't looked up the 
       logical address, we need to assume logical address was looked up
       on the master side and that the device is active, and in that case
       just initialize device structure with defaults */
    pDevice = SlimBus_NewDeviceStruct();
    if ( NULL != pDevice )
    {
       pDevCtxt->pDevices[LA&SB_LA_IDX_MASK] = pDevice;
       pDevice->LA = LA;
       pDevice->eState = SB_ENUM_ACTIVE;

       SB_LOG_INFO_1("Auto created device structure (LA: 0x%x)", LA);
    }
  }
  else if ( NULL == pDevice ||
            pDevice->LA != LA ||
            SB_ENUM_ACTIVE != pDevice->eState )
  {
    pDevice = NULL;
  }
  
  return pDevice;
}

/**
 * @brief Perform cleanup for a message event
 *
 * This function performs cleanup operations for a message event
 * 
 * @param[in] pDevice  Pointer to a SLIMbus native device 
 *       structure
 * @param[in] TID  Transaction ID
 * @param[in] pEventNode  Pointer to the event node
 * @param[in] pRootNode  Pointer to the list to return the event 
 *       node
 */
void SlimBus_CleanupMsgEvent
(
  SlimBusDeviceType *pDevice,
  uint32 TID,
  SlimBusEventNodeType *pEventNode,
  SlimBusLinkNodeType *pRootNode
)
{
  pEventNode->pTxBuf = NULL;
  pEventNode->pTxPhysBuf = NULL;
  PushTailEvent(pRootNode, pEventNode);

  if (NULL != pDevice)
  {
    if (pEventNode == pDevice->TIDMap[TID&SB_TID_IDX_MASK])
    {
      pDevice->TIDMap[TID&SB_TID_IDX_MASK] = NULL;
  
      /* Signal anyone waiting on an available transaction */
      pDevice->uAvailTrans++;
      pEventNode = PopHeadEvent(&pDevice->TransEventQueue);
      if (NULL != pEventNode)
      {
        SlimBus_EventTrigger(pEventNode->hEvent);
      }
    }
  }
}

/**
 * @brief Check if the hardware is enumerated
 *
 * This function checks whether the hardware has enumerated
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return TRUE if the hardware has enumerated, FALSE otherwise
 */
SBBOOL SlimBus_HardwareIsEnumerated(SlimBusDevCtxt *pDevCtxt)
{
  if ( pDevCtxt->bIsHwOwner )
  {
    return ( HAL_sb_IntfIsMessageSyncAcquired(pDevCtxt->hSB) &&
             HAL_sb_DeviceIsEnumerated(pDevCtxt->hSB, pDevCtxt->eMsgDev) &&
             HAL_sb_DeviceIsEnumerated(pDevCtxt->hSB, HAL_SB_DEVICE_INTERFACE) &&
             HAL_sb_DeviceIsEnumerated(pDevCtxt->hSB, HAL_SB_DEVICE_FRAMER) &&
             HAL_sb_DeviceIsEnumerated(pDevCtxt->hSB, HAL_SB_DEVICE_PORTED_GENERIC) );
  }
  else
  {
    return HAL_sb_DeviceIsEnumerated(pDevCtxt->hSB, pDevCtxt->eMsgDev);
  }
}

/**
 * @brief Check if the common driver is ready to enter low-power 
 *        mode
 *
 * This function checks if the common driver is ready to enter 
 * low-power mode 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return TRUE if ready to enter low-power mode, FALSE 
 *         otherwise
 */
SBBOOL SlimBus_ReadyForLowPowerMode(SlimBusDevCtxt *pDevCtxt)
{
  return ( 0 == pDevCtxt->uOpenCnt &&
           pDevCtxt->TxMsgFifo.uAckIdx == pDevCtxt->TxMsgFifo.uHeadIdx &&
           pDevCtxt->RxMsgFifo.uTailIdx == pDevCtxt->RxMsgFifo.uHeadIdx &&
           NULL == PeekHeadEvent(&pDevCtxt->TxEventQueue) &&
           NULL == PeekHeadEvent(&pDevCtxt->RxEventQueue) &&
           0 == pDevCtxt->uNumTxMsgWaiters &&
           0 == pDevCtxt->uPortDisconnectMask &&
           0 == pDevCtxt->uPortFinalDisconnectMask &&
           0 == pDevCtxt->uPortBamResetMask &&
           0 == pDevCtxt->uPortBamResetDeferMask &&
           !pDevCtxt->bPendingSSR &&
           SlimBus_HardwareIsEnumerated(pDevCtxt) );
}

/**
 * @brief Perform work related to processing notification of a 
 *        received message
 *
 * This function performs work related to procsesing 
 * notification of a received message 
 * 
 * @param[in] pDevCtxt  Pointer to the SlimBus device
 * @param[in] pSignalList  Pointer to the signal list
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SBResult SlimBus_MsgWorkerCallback(SlimBusDevCtxt *pDevCtxt, SlimBusLinkNodeType *pSignalList)
{
  SlimBusIsrCtxt IsrCtxt;
  uint32 *pBuf = NULL;
  uint32 uToken;

  SlimBus_InterruptAcquireLock(pDevCtxt, &uToken);
  pDevCtxt->bMsgWorkLoopSignalled = FALSE;
  IsrCtxt = pDevCtxt->IsrCtxt;
  memset(&pDevCtxt->IsrCtxt, 0, sizeof(pDevCtxt->IsrCtxt));
  SlimBus_InterruptReleaseLock(pDevCtxt, &uToken);

  if ( 0 != IsrCtxt.uMsgIntStatus ||
       0 != IsrCtxt.uPgdPortIntMaskedStatus ||
       0 != IsrCtxt.uPortDisMask ||
       0 != IsrCtxt.uFrmIntStatus ||
       0 != IsrCtxt.uIntfIntStatus ||
       0 != IsrCtxt.uPgdIntStatus )
  {
    SlimBusCommonDpc(pDevCtxt, &IsrCtxt, pSignalList);
  }
  
  SlimBus_BamDoDeferredWork(pDevCtxt);

  while (NULL != (pBuf = SlimBus_PopBuffer(&pDevCtxt->RxMsgFifo, NULL)))
  {
    SlimBus_ProcessRxMsg(pDevCtxt, pBuf, pSignalList);
  }

  if (pDevCtxt->bUseRxMsgQueue)
  {
    /* re-submit new Rx descriptors */
    SlimBus_SubmitRxDescriptors(&pDevCtxt->RxMsgFifo);
  }

  (void) pDevCtxt->checkEnterLowPowerFcn(pDevCtxt);

  return SB_SUCCESS;
}

/**
 * @brief Wait for message transaction completion event
 *
 * This function performs functionality related to waiting for 
 * the completion event for a message transaction. 
 * 
 * @param[in] pClientCtxt  Pointer to the client context
 * @param[in] pEventNode  Pointer to the event node
 * 
 * @return SB_SUCCESS on success, error code on error
 */
_IRQL_requires_(DISPATCH_LEVEL)
SBResult SlimBus_WaitForMsgEvent(SlimBusClientCtxt *pClientCtxt, SlimBusEventNodeType *pEventNode)
{
  SBResult result = SB_SUCCESS;
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SlimBusLinkNodeType SignalList;  /* List of events that will need signalling */
  SlimBusEventNodeType *pSignalEvent = NULL;
  SBBOOL bRetry = FALSE;

  if (NULL == pEventNode)
  {
    return SB_ERROR;
  }

  /* Initialize the signalling list structure */
  RootNodeInit(&SignalList);

  pEventNode->bSyncWait = TRUE;

  while ( pEventNode->bSyncWait && SB_SUCCESS == result )
  {
    SlimBus_BamAcquireLock(pDevCtxt);
    if (bRetry)
    {
      PushHeadClient(&pDevCtxt->MsgWaitList, &pClientCtxt->BasicClient);
    }
    else
    {
      PushTailClient(&pDevCtxt->MsgWaitList, &pClientCtxt->BasicClient);
    }
    SlimBus_BamReleaseLock(pDevCtxt);  

    SlimBus_UnlockDevice(pDevCtxt);

    /* Wait for the completion event */
    result = SlimBus_EventTimedWait(pClientCtxt,
                                    pEventNode->hEvent,
                                    SB_DEFAULT_CLIENT_TIMEOUT);

    SlimBus_LockDevice(pDevCtxt);

    SlimBus_BamAcquireLock(pDevCtxt);
    RemoveClientNode(&pClientCtxt->BasicClient);
    SlimBus_BamReleaseLock(pDevCtxt);  

    SlimBus_MsgWorkerCallback(pDevCtxt, &SignalList);
    
    while (NULL != (pSignalEvent = PopHeadEvent(&SignalList)))
    {
      SBBOOL bSyncWait = pSignalEvent->bSyncWait;

      pSignalEvent->bSyncWait = FALSE;
      if ( pEventNode != pSignalEvent )
      {
        if (bSyncWait)
        {
          SlimBus_EventTrigger(pSignalEvent->hEvent);
        }
        else
        {
          SlimBusEventHandle hEvent = pSignalEvent->hEvent;
          
          PushHeadEvent(pSignalEvent->pFreeRoot, pSignalEvent);
          SlimBus_UnlockDevice(pDevCtxt);
          SlimBus_EventTrigger(hEvent);
          SlimBus_LockDevice(pDevCtxt);
        }
      }
    }
    
    bRetry = TRUE;
  }
  
  pEventNode->bSyncWait = FALSE;
  if ( SB_SUCCESS == result )
  {
    /* On success, just return the event node to the free stack */
    PushHeadEvent(pEventNode->pFreeRoot, pEventNode);
  }
  else
  {
    if (NULL != pEventNode->peMsgErr)
    {
      *(pEventNode->peMsgErr) = SLIMBUS_ERR_MSG_RESP_TIMEOUT;
    }
  }
  
  return result;
}

/**
 * @brief Process a received message
 *
 * Process a received message 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] length  Length of the message
 * @param[in] pDataBuf  Pointer to the first word in the 
 *       received message buffer
 * @param[in] pSignalList  Pointer to the list of events that 
 *       need to be signalled
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
SBResult SlimBus_ProcessRxMsg
(
  SlimBusDevCtxt *pDevCtxt,
  uint32 *pDataBuf,
  SlimBusLinkNodeType *pSignalList
)
{
  SBResult result = SB_SUCCESS;
  SlimBusMsgMcType eMC;
  SlimBusMsgMtType eMT;
  uint32 length;
  
  uint32 data = *(pDataBuf++);

  length = SB_MSG_GET(data,GENERIC_0,LEN);
  eMT = (SlimBusMsgMtType) SB_MSG_GET(data,GENERIC_0,MT);
  eMC = (SlimBusMsgMcType) SB_MSG_GET(data,GENERIC_0,MC);
  if ( SB_MASTER_DRIVER == pDevCtxt->eDrvType )
  {
    result = SlimBus_ProcessMasterRxMsg(pDevCtxt, length, data, pDataBuf);
  }
  else if ( SB_SATELLITE_DRIVER == pDevCtxt->eDrvType )
  {
    result = SlimBus_ProcessSatRxMsg(pDevCtxt, length, data, pDataBuf);
  }

  if ( SLIMBUS_ERR_UNHANDLED_MSG == result )
  {
    result = SB_SUCCESS;
    
    if (SB_MSG_MT_CORE == eMT)
    {
      if ( (SB_MSG_MC_REPLY_VALUE == eMC ||
            SB_MSG_MC_REPLY_INFORMATION == eMC) )
      {
        if (!SB_TEST_REPLIES_IGNORED())
        {
          uint8 TID, LA;
          SlimBusDeviceType *pDevice = NULL;
          SlimBusEventNodeType *pEventNode = NULL;
    
          LA = (uint8) SB_MSG_GET(data,GENERIC_0,LA);
          TID = SB_MSG_GET(data,REPLY_VALUE_0,TID);
    
          pDevice = pDevCtxt->pDevices[LA&SB_LA_IDX_MASK];
          if (NULL != pDevice)
          {
            pEventNode = pDevice->TIDMap[TID&SB_TID_IDX_MASK];
          }
    
          if (NULL != pEventNode &&
              NULL != pDevice &&
              pDevice->LA == LA &&
              SB_BF_GET(pEventNode->uRxMarker,RXMARKER,TID) == TID)
          {
            uint32 uCount = length - SB_MSG_MC_REPLY_VALUE__VS_OFFSET;
    
            if (uCount > SB_BF_GET(pEventNode->uRxMarker,RXMARKER,BUF_SIZE))
            {
              uCount = SB_BF_GET(pEventNode->uRxMarker,RXMARKER,BUF_SIZE);
            }
            if (uCount > 0 && NULL != pEventNode->pRxBuf)
            {
              SlimBus_Memcpy(pEventNode->pRxBuf, pDataBuf, uCount);
            }
            if (NULL != pEventNode->pRxBufSize)
            {
              *pEventNode->pRxBufSize = uCount;
            }
    
            SlimBus_CleanupMsgEvent(pDevice, TID, pEventNode, pSignalList);
          }
        }
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
  }

  if ( SLIMBUS_ERR_UNHANDLED_MSG == result )
  {
    SB_LOG_WARN_3("Received unhandled message (msg: 0x%lx 0x%lx 0x%lx)",
                  data, ((length>4)?(pDataBuf[0]):0), ((length>8)?(pDataBuf[1]):0));
  }
  
  return result;
}

/**
 * @brief Process messages received in an ISR context
 *
 * This function processes Rx messages received in an ISR 
 * context.  If a message is not able to be processed in ISR 
 * context, then SLIMBUS_ERR_RETRY will be returned. 
 * 
 * @param pDevCtxt[in]  Pointer to the device context
 * @param pSignalList[in]  Pointer to the root node to enqueue 
 *                   events that need to be signalled
 * 
 * @return  SB_SUCCESS on success, SLIMBUS_ERR_RETRY if the 
 *          messages could not be processed in ISR context, or
 *          an error code otherwise
 */
SBResult SlimBus_HandleIsrRxMsgs
(
  SlimBusDevCtxt *pDevCtxt,
  SlimBusLinkNodeType *pSignalList
)
{
  SBResult result = SB_SUCCESS;
  
  /* If the message we just received is the only one in the Rx buffer,
     then try to process the message inside the ISR to save a context switch */
  if ( !pDevCtxt->bMsgWorkLoopSignalled )
  {
    uint32 *pDataBuf;
    
    while ( NULL != (pDataBuf = SlimBus_PeekBuffer(&pDevCtxt->RxMsgFifo, NULL)) )
    {
      result = SlimBus_ProcessRxMsg(pDevCtxt, pDataBuf, pSignalList);
      /* If the message could not be processed within the ISR, then signal
         the workloop to process the message(s) */
      if (result == SLIMBUS_ERR_RETRY)
      {
        /* signal the Rx messaging workloop */
        pDevCtxt->bMsgWorkLoopSignalled = TRUE;
        SlimBus_EventTrigger(pDevCtxt->hMsgWorkLoopEvent);
        break;
      }
      else
      {
        SlimBus_PopBuffer(&pDevCtxt->RxMsgFifo, NULL);
      }
    }
  }

  return result;
}

/**
 * @brief Process a list of events that need signalling
 *
 * This function processes a list of events that need to be 
 * signalled. 
 * 
 * @param pDevCtxt[in]  Pointer to the device context
 * @param pSignalList[in]  Pointer to the list of events that
 *                   need to be signalled
 */
void SlimBus_ProcessSignalList
(
  SlimBusDevCtxt *pDevCtxt,
  SlimBusLinkNodeType *pSignalList
)
{
  if (NULL != PeekHeadEvent(pSignalList))
  {
    SlimBusEventNodeType *pEventNode = NULL;

    while (NULL != (pEventNode = PopHeadEvent(pSignalList)))
    {
      SBBOOL bSyncWait = pEventNode->bSyncWait;
      SlimBusEventHandle hEvent = pEventNode->hEvent;
      
      pEventNode->bSyncWait = FALSE;
      if (!bSyncWait)
      {
        SlimBus_LockDevice(pDevCtxt);
        PushHeadEvent(pEventNode->pFreeRoot, pEventNode);
        SlimBus_UnlockDevice(pDevCtxt);
      }
      SlimBus_EventTrigger(hEvent);
    }
  }
}

/**
 * @brief Allocate buffer space for Tx and Rx message queues
 *
 * This function allocates buffer space for the Tx and Rx 
 * message queues.  If BAM message queues are enabled, the 
 * buffer space is allocated from physical uncached memory. 
 * 
 * @param pDevCtxt[in]  Pointer to the device context
 * @param puOffset[out]  Location to store the offset into the 
 *                physical buffer of space unused by the message
 *                queues
 * 
 * @return SB_SUCCESS on success, error code otherwise
 */
SBResult SlimBus_AllocMsgBuffers(SlimBusDevCtxt *pDevCtxt, uint32 *puOffset)
{
  SBResult result = SB_SUCCESS;
  uint32 uOffset = 0;
  size_t uVirtAddr = 0, uPhysAddr = 0;
  
  if (pDevCtxt->bUseTxMsgQueue || pDevCtxt->bUseRxMsgQueue)
  {
    if (NULL == pDevCtxt->hPhysMem)
    {
      result = SlimBus_PhysMemAlloc(pDevCtxt->TxMsgFifo.uLen*sizeof(uint32) + 
                                    SB_MAX_MSG_LENGTH_BYTES +
                                    pDevCtxt->RxMsgFifo.uLen*sizeof(uint32) + 
                                    SB_MAX_MSG_LENGTH_BYTES +
                                    SB_TX_DESC_FIFO_SIZE + SB_RX_DESC_FIFO_SIZE,
                                    &pDevCtxt->hPhysMem);
      if (SB_SUCCESS != result)
      {
        pDevCtxt->hPhysMem = NULL;
      }
    }

    if (NULL != pDevCtxt->hPhysMem)
    {
      result = SlimBus_PhysMemInfo(pDevCtxt->hPhysMem, &uVirtAddr, &uPhysAddr);
      if (SB_SUCCESS != result)
      {
        SlimBus_DestroyObject(pDevCtxt->hPhysMem);
        pDevCtxt->hPhysMem = NULL;
      }
    }
    
    if (NULL == pDevCtxt->hPhysMem)
    {
      SB_LOG_ERR_0("Disabling message queues due to physical malloc failure");
      pDevCtxt->bUseTxMsgQueue = FALSE;
      pDevCtxt->bUseRxMsgQueue = FALSE;
    }
  }
  
  if (NULL == pDevCtxt->TxMsgFifo.pBuf)
  {
    if (NULL != pDevCtxt->hPhysMem)
    {
      pDevCtxt->TxMsgFifo.pBuf = (uint32*)(uVirtAddr + uOffset);
      pDevCtxt->TxMsgFifo.pPhysBuf = (uint32*)(uPhysAddr + uOffset);
    }
    else
    {
      uint32 uSize = pDevCtxt->TxMsgFifo.uLen*sizeof(uint32)+SB_MAX_MSG_LENGTH_BYTES;
      SlimBus_Malloc(uSize, (void**)(&pDevCtxt->TxMsgFifo.pBuf), SB_MEMTAG_2);
      if (NULL != pDevCtxt->TxMsgFifo.pBuf)
      {
        memset(pDevCtxt->TxMsgFifo.pBuf, 0, uSize);
      }
    }
  }
  uOffset += pDevCtxt->TxMsgFifo.uLen*sizeof(uint32) + SB_MAX_MSG_LENGTH_BYTES;
  
  if (NULL == pDevCtxt->RxMsgFifo.pBuf)
  {
    if (NULL != pDevCtxt->hPhysMem)
    {
      pDevCtxt->RxMsgFifo.pBuf = (uint32*)(uVirtAddr + uOffset);
      pDevCtxt->RxMsgFifo.pPhysBuf = (uint32*)(uPhysAddr + uOffset);
    }
    else
    {
      uint32 uSize = pDevCtxt->RxMsgFifo.uLen*sizeof(uint32)+SB_MAX_MSG_LENGTH_BYTES;
      SlimBus_Malloc(uSize, (void**)(&pDevCtxt->RxMsgFifo.pBuf), SB_MEMTAG_3);
      if (NULL != pDevCtxt->RxMsgFifo.pBuf)
      {
        memset(pDevCtxt->RxMsgFifo.pBuf, 0, uSize);
      }
    }
  }
  uOffset += pDevCtxt->RxMsgFifo.uLen*sizeof(uint32) + SB_MAX_MSG_LENGTH_BYTES;
  
  if (NULL != puOffset)
  {
    *puOffset = uOffset;
  }
  
  return SB_SUCCESS;
}


/**
 * @brief SLIMbus driver common interrupt service routine
 *
 * SLIMbus driver common interrupt service routine
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] pIsrCtxt  Pointer to the interrupt context 
 * 
 * @return Pointer to NULL 
 */
SBResult SlimBusCommonIsr(SlimBusDevCtxt *pDevCtxt, struct SlimBusIsrCtxt *pIsrCtxt)
{
  uint32 uIntStatus = 0;
  uint32 uMaskedIntStatus = 0;

  uIntStatus = HAL_sb_DeviceIntGetStatus(pDevCtxt->hSB, pDevCtxt->eMsgDev);
  if (uIntStatus & HAL_SB_MGR_INT_RX_MSG_RCVD)
  {
    uint32 data, length;
    uint32 *pDataBuf = NULL;

    HAL_sb_DeviceRxMsgRead(pDevCtxt->hSB, pDevCtxt->eMsgDev,
                           &data, sizeof(data)/sizeof(uint32));
    length = SB_MSG_GET(data,GENERIC_0,LEN);

    pDataBuf = pDevCtxt->RxMsgFifo.getBufFcn(NULL, NULL,
                                             &pDevCtxt->RxMsgFifo, length);

    if (NULL != pDataBuf)
    {
      HAL_sb_DeviceRxMsgRead(pDevCtxt->hSB, pDevCtxt->eMsgDev,
                             pDataBuf, (length+3)/sizeof(uint32));
    }
  }
  if (0 != uIntStatus)
  {
    pIsrCtxt->uMsgIntStatus |= uIntStatus;
    HAL_sb_DeviceIntClear(pDevCtxt->hSB, pDevCtxt->eMsgDev, uIntStatus);
  }

  uIntStatus = HAL_sb_PgdPortGetIntStatusEe(pDevCtxt->hSB, 0, pDevCtxt->pBSP->uMyEE);
  if (0 != uIntStatus)
  {
    uint32 uPgdPortEnableMask = HAL_sb_PgdPortGetIntEnEe(pDevCtxt->hSB, 0, pDevCtxt->pBSP->uMyEE);
    uint32 uOrigPgdPortEnableMask = uPgdPortEnableMask;
    uint32 uIntClearMask = 0;
    uint32 uIntMask = 0;
    uint32 i = 0;

    uMaskedIntStatus = (uIntStatus & uPgdPortEnableMask);

    for (i = 0; (i < pDevCtxt->numPorts) && (uIntMask < uIntStatus); i++)
    {
      SlimBusMasterPortType *pPort = &pDevCtxt->Ports[i];
      uint32 uPortNum = pPort->portNum;

      uIntMask = (1<<uPortNum);

      if (uIntMask & uIntStatus)
      {
        HAL_sb_PgdPortEventType eEvents = HAL_sb_PgdPortGetEvents(pDevCtxt->hSB, 0, uPortNum);
        
        if (uIntMask & uMaskedIntStatus)
        {
          if (eEvents & HAL_SB_PGD_PORT_UNDERFLOW_EVENT)
          {
            pIsrCtxt->uPortUnderFlowMask |= uIntMask;
          }
          if (eEvents & HAL_SB_PGD_PORT_OVERFLOW_EVENT)
          {
            pIsrCtxt->uPortOverFlowMask |= uIntMask;
          }
        }
        else
        {
          /* Only process these events if the port interrupt is enabled */
          eEvents &= ~(HAL_SB_PGD_PORT_UNDERFLOW_EVENT|HAL_SB_PGD_PORT_OVERFLOW_EVENT);
        }

        if ( (eEvents & HAL_SB_PGD_PORT_DISCONNECT_EVENT) )
        {
          pIsrCtxt->uPortDisMask |= uIntMask;
        }
        
        if ( (eEvents & HAL_SB_PGD_PORT_DMA_EVENT) )
        {
          /* Clear the interrupt for the port */
          uIntClearMask |= uIntMask;
        }
        
        if ( (eEvents & HAL_SB_PGD_PORT_UNDERFLOW_EVENT) ||
             (eEvents & HAL_SB_PGD_PORT_OVERFLOW_EVENT) ||
             (eEvents & HAL_SB_PGD_PORT_DISCONNECT_EVENT) )
        {
          /* Keep the interrupt disabled until it can be processed */
          uPgdPortEnableMask &= ~uIntMask;
        }
        
        if ( (eEvents & HAL_SB_PGD_PORT_DMA_EVENT) )
        {
          pIsrCtxt->uPortDmaMask |= uIntMask;
        }

        if ( !(eEvents & HAL_SB_PGD_PORT_DISCONNECT_EVENT) &&
             !(eEvents & HAL_SB_PGD_PORT_UNDERFLOW_EVENT) &&
             !(eEvents & HAL_SB_PGD_PORT_OVERFLOW_EVENT) )
        {
          /* We've processed the interrupt, so don't need to pass on to DPC */
          uIntStatus &= ~uIntMask;
          uMaskedIntStatus &= ~uIntMask;
        }
      }
    }
    
    /* Record which ports are interrupting */
    pIsrCtxt->uPgdPortIntRawStatus |= uIntStatus;

    /* Record which ports we may have to disable until we can service the DPC */
    pIsrCtxt->uPgdPortIntMaskedStatus |= uMaskedIntStatus;

    if (uIntClearMask != 0)
    {
      HAL_sb_PgdPortClearIntEe(pDevCtxt->hSB, 0, pDevCtxt->pBSP->uMyEE, uIntClearMask);
    }
    if (uOrigPgdPortEnableMask != uPgdPortEnableMask)
    {
      HAL_sb_PgdPortSetIntEnEe(pDevCtxt->hSB, 0, pDevCtxt->pBSP->uMyEE, uPgdPortEnableMask);
    }
  }

  if ( !pIsrCtxt->uPortDmaMask || uMaskedIntStatus || pIsrCtxt->uMsgIntStatus )
  {
    if (pDevCtxt->bIsHwOwner)
    {
      uIntStatus = HAL_sb_DeviceIntGetStatus(pDevCtxt->hSB, HAL_SB_DEVICE_FRAMER);
      if (0 != uIntStatus)
      {
        pIsrCtxt->uFrmIntStatus |= uIntStatus;
        HAL_sb_DeviceIntClear(pDevCtxt->hSB, HAL_SB_DEVICE_FRAMER, uIntStatus);
      }

      uIntStatus = HAL_sb_DeviceIntGetStatus(pDevCtxt->hSB, HAL_SB_DEVICE_INTERFACE);
      if (0 != uIntStatus)
      {
        pIsrCtxt->uIntfIntStatus |= uIntStatus;
        HAL_sb_DeviceIntClear(pDevCtxt->hSB, HAL_SB_DEVICE_INTERFACE, uIntStatus);
      }

      uIntStatus = HAL_sb_DeviceIntGetStatus(pDevCtxt->hSB, HAL_SB_DEVICE_PORTED_GENERIC);
      if (0 != uIntStatus)
      {
        pIsrCtxt->uPgdIntStatus |= uIntStatus;
        HAL_sb_DeviceIntClear(pDevCtxt->hSB, HAL_SB_DEVICE_PORTED_GENERIC, uIntStatus);
      }
    }
  }

  return SB_SUCCESS;
}

/**
 * @brief Recycle a master port so that it can be reused
 *
 * This function recycles a SlimBus master port data structure 
 * so that it is ready to be reused 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] pPort  Pointer to the master port data structure
 */
static void SlimBus_RecyclePort
(
  SlimBusDevCtxt *pDevCtxt,
  SlimBusMasterPortType *pPort
)
{
  uint32 uPortMask = (1<<pPort->portNum);

  pPort->eState = SB_HANDLE_AVAILABLE;
  pPort->eReqs = SLIMBUS_PORT_REQ_O_DEFAULT;
  pPort->eEnabledEvents = SLIMBUS_EVENT_O_NONE;
  pPort->eDisconnectState = SB_PORT_NOT_DISCONNECTING;

  if (!pDevCtxt->bPendingSSR)
  {
    HAL_sb_PgdPortDisable(pDevCtxt->hSB, 0, pPort->portNum);
  
    DisablePortInterrupt(pDevCtxt, (1<<pPort->portNum));

    if ( !(pDevCtxt->eWrkrnd & SB_WRKRND_O_NO_PORT_MULTI_CHAN) )
    {
      /* clear multi-channel bitmap */
      /* Note: can't have half-duplex port in multi-channel group */
      uint32 uMask = HAL_sb_PgdPortGetMultiChanl(pDevCtxt->hSB, 0, pPort->portNum);
      /* Remove this port from any other ports in the multi-channel group */
      uMask &= ~uPortMask;
      HAL_sb_PgdPortSetMultiChanl(pDevCtxt->hSB, 0, uMask);
      /* Set this port as being in its own group */
      HAL_sb_PgdPortSetMultiChanl(pDevCtxt->hSB, 0, uPortMask);
    }
  }
  
  /* Call resume idle for each port deallocated */        
  SlimBus_ResumeIdle(pDevCtxt);
}

/**
 * @brief Handle port disconnection processing
 *
 * This function handles port disconnection processing
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] pPort  Pointer to the master port data structure 
 */
static void SlimBus_HandlePortDisconnect
(
  SlimBusDevCtxt *pDevCtxt,
  SlimBusMasterPortType *pPort
)
{
  /* Signal any client that is waiting for port to disconnect */
  if (pPort->bSignalDisconnect)
  {
    if (NULL != pPort->pClientCtxt)
    {
      SlimBus_EventTrigger(pPort->pClientCtxt->hClientEvent);
    }
    pPort->bSignalDisconnect = FALSE;
  }
  pPort->uBytesSubmit = 0;
  pPort->uDescSubmit = 0;
  pPort->eDisconnectState = SB_PORT_NOT_DISCONNECTING;

  /* If the port has already been deallocated, then complete the
     deallocation cleanup and make port available again */
  if (SB_HANDLE_DEALLOCATED == pPort->eState)
  {
    SlimBus_RecyclePort(pDevCtxt, pPort);            
  }
}

/**
 * @brief Process the disconnection of a port
 *
 * This function processes the disconnection of a port
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] pPort  Pointer to the master port data structure
 * @param[out] peEvents  Pointer to the location to store 
 *                  detected port events
 */
void SlimBus_ProcessPortDisconnection
(
  SlimBusDevCtxt *pDevCtxt,
  SlimBusMasterPortType *pPort,
  SlimBusPortEventType *peEvents
)
{
  uint32 uIntMask = (1<<pPort->portNum);
  uint32 endPointNum;
  SBBOOL bNeedBamDisconnect = FALSE;
  
  SB_LOG_VERB_3("Got port disconnection (client: 0x%lx) (resource: 0x%lx) (port: %ld)",
                (uint32)pPort->pClientCtxt, pPort->handle, pPort->portNum);

  SlimBus_PortAcquireLock(pDevCtxt, pPort);

  if ( (NULL != pPort->pBamCtxt) &&
       pPort->bBamConnected )
  {
    bNeedBamDisconnect = TRUE;
    pPort->eDisconnectState = SB_PORT_WAITING_TO_DISCONNECT;

    if ( !(pDevCtxt->uPortBamResetMask & uIntMask) )
    {
      /* Indicate that the BAM needs to be reset */
      pDevCtxt->uPortBamResetDeferMask |= uIntMask;
    }
  }

  for (endPointNum = 0; endPointNum < pPort->numEndPoints; endPointNum++)
  {
    RemoveEndPointNode(&pPort->aEndPoints[endPointNum]);
    pPort->aEndPoints[endPointNum].pChannel = NULL;
  }

  /* We can clear the interrupt status now that the port is disconnected */
  HAL_sb_PgdPortClearIntEe(pDevCtxt->hSB, 0, pDevCtxt->pBSP->uMyEE, uIntMask);

  pDevCtxt->uPortDisconnectMask &= ~uIntMask;
  pDevCtxt->uPortFinalDisconnectMask &= ~uIntMask;
  pDevCtxt->uPortConnectMask &= ~uIntMask;

  /* Signal any client that is waiting for port descriptors to flush 
     (note that flush can't happen anymore once port is disconnected,
      so we need to signal any waiting thread here otherwise it would
      never get signalled) */
  if (pPort->uSignalDescEmptyCnt > 0)
  {
    if (NULL != pPort->pClientCtxt)
    {
      SlimBus_EventTrigger(pPort->pClientCtxt->hClientEvent);
    }
    pPort->uSignalDescEmptyCnt = 0;
  }

  if (NULL != peEvents)
  {
    *peEvents |= SLIMBUS_EVENT_O_PORT_DISCONNECT;
  }

  /* Unless the BAM needs to be reset, process the port disconnection */
  if (!bNeedBamDisconnect)
  {
    SlimBus_HandlePortDisconnect(pDevCtxt, pPort);
  }

  SlimBus_PortReleaseLock(pDevCtxt, pPort);
}

/**
 * @brief This function will reset MsgFifo
 * 
 * @param[in] pMsgFifo pointer to message fifo.
 * 
 * @return None 
 */
void SlimBus_ResetMsgFifo(SlimBusMsgFifoType *pMsgFifo)
{
  /* Flush queue */
  pMsgFifo->uTailIdx = pMsgFifo->uHeadIdx;
  pMsgFifo->uOverhang = 0;
  pMsgFifo->bBusy = FALSE;
  if(pMsgFifo->uAckIdx != SB_ACK_IDX_UNUSED)
  {
    pMsgFifo->uAckIdx = pMsgFifo->uHeadIdx;
  }

  /* Detach pMsgFifo BAM */
  if ( NULL != pMsgFifo->pBamCtxt )
  {
    SlimBus_DetachBAM(&pMsgFifo->pBamCtxt);
  }
}

/**
 * @brief SLIMbus driver common deferred procedure call
 *
 * SLIMbus driver common deferred procedure call
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] pIsrCtxt  Pointer to the interrupt context 
 * @param[in] pSignalList  Pointer to the signal list
 * 
 * @return SB_SUCCESS on success, an error code on error 
 */
SBResult SlimBusCommonDpc(SlimBusDevCtxt *pDevCtxt, struct SlimBusIsrCtxt *pIsrCtxt, SlimBusLinkNodeType *pSignalList)
{
  uint32 uIntStatus = 0, uPgdIntStatus = 0;
  SlimBusEventNodeType *pEventNode = NULL;
  
  if (pDevCtxt->bIsHwOwner)
  {
    if (pIsrCtxt->uFrmIntStatus & HAL_SB_FRM_INT_DEV_ERR)
    {
      SB_LOG_ERR_0("Framer device error");
    }
    if (pIsrCtxt->uFrmIntStatus & HAL_SB_FRM_INT_IE_VE_CHANGE)
    {
      SB_LOG_WARN_1("Framer device IE change (IE: 0x%lx)",
                    HAL_sb_DeviceGetIeStatus(pDevCtxt->hSB, HAL_SB_DEVICE_FRAMER));
    }

    if (pIsrCtxt->uIntfIntStatus & HAL_SB_INTF_INT_DEV_ERR)
    {
      uint32 uMask, uToken;

      HAL_sb_DeviceIntClear(pDevCtxt->hSB, HAL_SB_DEVICE_INTERFACE,
                            HAL_SB_INTF_INT_DEV_ERR);

      /* Disable interrupt for throttling to prevent it recurring until next pause clock */
      SlimBus_InterruptAcquireLock(pDevCtxt, &uToken);
      uMask = HAL_sb_DeviceIntGetMask(pDevCtxt->hSB, HAL_SB_DEVICE_INTERFACE);
      uMask &= ~HAL_SB_INTF_INT_DEV_ERR;
      HAL_sb_DeviceIntSetMask(pDevCtxt->hSB, HAL_SB_DEVICE_INTERFACE, uMask);
      SlimBus_InterruptReleaseLock(pDevCtxt, &uToken);

      SB_LOG_ERR_0("Intf device error");
    }
    if (pIsrCtxt->uIntfIntStatus & HAL_SB_INTF_INT_IE_VE_CHANGE)
    {
      uint32 uMask, uToken;

      HAL_sb_DeviceIntClear(pDevCtxt->hSB, HAL_SB_DEVICE_INTERFACE,
                            HAL_SB_INTF_INT_IE_VE_CHANGE);

      /* Disable interrupt for throttling to prevent it recurring until next pause clock */
      SlimBus_InterruptAcquireLock(pDevCtxt, &uToken);
      uMask = HAL_sb_DeviceIntGetMask(pDevCtxt->hSB, HAL_SB_DEVICE_INTERFACE);
      uMask &= ~HAL_SB_INTF_INT_IE_VE_CHANGE;
      HAL_sb_DeviceIntSetMask(pDevCtxt->hSB, HAL_SB_DEVICE_INTERFACE, uMask);
      SlimBus_InterruptReleaseLock(pDevCtxt, &uToken);

      SB_LOG_WARN_1("Intf device IE change (IE: 0x%lx)",
                    HAL_sb_DeviceGetIeStatus(pDevCtxt->hSB, HAL_SB_DEVICE_INTERFACE));
    }

    if (pIsrCtxt->uIntfIntStatus & HAL_SB_INTF_INT_NO_DATA_LINE_ACTIVITY)
    {
      SB_LOG_ERR_0("Intf device no data line activity");
    
      HAL_sb_DeviceIntClear(pDevCtxt->hSB, HAL_SB_DEVICE_INTERFACE,
                            HAL_SB_INTF_INT_NO_DATA_LINE_ACTIVITY);

      /* Restart the enumeration timer in case of no data line activity */
      if ( pDevCtxt->bActiveFramer )
      {
        SlimBus_TimerStart(pDevCtxt,
                           pDevCtxt->hEnumTimerHandle,
                           pDevCtxt->hEnumTimerEvent,
                           SB_ENUMERATION_TIMEOUT);
      }
    }

    if (pIsrCtxt->uPgdIntStatus & HAL_SB_PGD_INT_DEV_ERR)
    {
      SB_LOG_ERR_0("PGD device error");
    }
    if (pIsrCtxt->uPgdIntStatus & HAL_SB_PGD_INT_IE_VE_CHANGE)
    {
      SB_LOG_WARN_1("PGD device IE change (IE: 0x%lx)",
                    HAL_sb_DeviceGetIeStatus(pDevCtxt->hSB, HAL_SB_DEVICE_PORTED_GENERIC));
    }
  }
  
  uIntStatus = pIsrCtxt->uMsgIntStatus;
  if ( uIntStatus != 0 )
  {
    if ( uIntStatus & HAL_SB_MGR_INT_TX_MSG_BUF_CONTENTION )
    {
      SB_LOG_WARN_0("contention for Tx interface");

      /* Resend the last message if there was contention */
      pDevCtxt->TxMsgFifo.sendBufFcn(pDevCtxt, &pDevCtxt->TxMsgFifo);
    }
    if ( ( uIntStatus & (uint32)HAL_SB_MGR_INT_TX_MSG_SENT ) ||
         ( uIntStatus & HAL_SB_MGR_INT_TX_MSG_NACKED_TWICE ) )
    {
      SlimBusErrorType eErr = SLIMBUS_ERR_NONE;
      uint8 NackMC = 0;
      pEventNode = NULL;
  
      if (uIntStatus & HAL_SB_MGR_INT_TX_MSG_BUF_CONTENTION)
      {
        /* Shouldn't get contention plus message sent, error if that is the case */
        SB_LOG_ERR_0("Slimbus Tx contention plus message sent!");
      }
      if ( uIntStatus & HAL_SB_MGR_INT_TX_MSG_NACKED_TWICE ||
           SB_TEST_NACK_FORCED() )
      {
        eErr = SLIMBUS_ERR_MSG_NACKED;
        /* TODO: consider queue for nacked mc */
        NackMC = (uint8) HAL_sb_DeviceTxMsgGetNackedMc(pDevCtxt->hSB, pDevCtxt->eMsgDev);
        SB_LOG_ERR_1("Tx message was nacked (MC: 0x%x)", NackMC);
      }
      
      if (pDevCtxt->bUseTxMsgQueue)
      {
        SlimBus_EmptyBamIOVec(&pDevCtxt->TxMsgFifo);
      }
      else
      {
        SlimBus_PopBuffer(&pDevCtxt->TxMsgFifo, NULL);
      }
      
      /* NOTE: can optimize, only one message with notification could have been sent */
      while( NULL != ( pEventNode = PeekHeadEvent(&pDevCtxt->TxEventQueue) ) )
      {
        SBBOOL bMsgErr = FALSE;
        
        if ( !SlimBus_IsMsgSendComplete(&pDevCtxt->TxMsgFifo, pEventNode->pTxBuf) )
        {
          break;
        }

        if ( SLIMBUS_ERR_NONE != eErr )
        {
          if (NULL != pEventNode->peMsgErr)
          {
            *(pEventNode->peMsgErr) = eErr;
          }

          bMsgErr = TRUE;
        }
        
        /* If the Tx message is expecting a response, and if there was 
           no transmission error, then push the event on the Rx wait queue */
        if ( 0 != pEventNode->uRxMarker &&
             !bMsgErr )
        {
          PushTailEvent(&pDevCtxt->RxEventQueue, pEventNode);
        }
        else
        {
          uint8 TID, LA;
          SlimBusDeviceType *pDevice;

          LA = SB_BF_GET(pEventNode->uRxMarker,RXMARKER,LA);
          TID = SB_BF_GET(pEventNode->uRxMarker,RXMARKER,TID);
          pDevice = pDevCtxt->pDevices[LA&SB_LA_IDX_MASK];
          if ( NULL != pDevice &&
               LA != pDevice->LA )
          {
            /* If pDevice does not correspond to the LA, set to NULL */
            pDevice = NULL;
          }
          
          SlimBus_CleanupMsgEvent(pDevice, TID, pEventNode, pSignalList);
        }
      }
      
      pDevCtxt->TxMsgFifo.uAckIdx = pDevCtxt->TxMsgFifo.uTailIdx;
      pDevCtxt->TxMsgFifo.bBusy = FALSE;
      
      while ( NULL != ( pEventNode = PopHeadEvent(&pDevCtxt->TxMsgFifo.WaitList) ) )
      {
        SlimBus_EventTrigger(pEventNode->hEvent);
      }

      /* Send any remaining messages now that direct-access interface is idle */
      pDevCtxt->TxMsgFifo.sendBufFcn(pDevCtxt, &pDevCtxt->TxMsgFifo);
    }
    if (uIntStatus & HAL_SB_MGR_INT_RX_MSG_RCVD)
    {
      SlimBus_HandleIsrRxMsgs(pDevCtxt, pSignalList);
    }
    if (uIntStatus & HAL_SB_MGR_INT_RECONFIG_DONE)
    {
      /* Remove any ports from the connect mask that are now actually active */
      if (0 != pDevCtxt->uPortConnectMask)
      {
        uint32 uIdx = 0;
        
        for (uIdx = 0; uIdx < pDevCtxt->numPorts; uIdx++)
        {
          uint32 portNum = pDevCtxt->Ports[uIdx].portNum;
          if ( (pDevCtxt->uPortConnectMask & (1<<portNum)) &&
               HAL_sb_PgdPortGetChannelNumber(pDevCtxt->hSB, 0, portNum) != 0 )
          {
            pDevCtxt->uPortConnectMask &= ~(1<<portNum);
          }
        }
      }

      if (pDevCtxt->bIsHwOwner)
      {
        SBBOOL bActiveFramer = HAL_sb_FrmIsActiveFrm(pDevCtxt->hSB);

        if (SB_MASTER_DRIVER == pDevCtxt->eDrvType)
        {
          pDevCtxt->uFrmClkGear = pDevCtxt->uNewClkGear;
        }
        else
        {
          pDevCtxt->uFrmClkGear = HAL_sb_FrmGetCurClkGear(pDevCtxt->hSB);
        }
        
        if (bActiveFramer != pDevCtxt->bActiveFramer)
        {
          SB_LOG_INFO_1("Framer active state changed (bActive: %ld)", bActiveFramer);
          pDevCtxt->bActiveFramer = bActiveFramer;
        }
       
        if (SB_SATELLITE_DRIVER == pDevCtxt->eDrvType)
        {
          /* Master driver will do voltage control from master worker thread */
          SLIMBUS_VOLTAGE_CTRL(pDevCtxt, pDevCtxt->uFrmClkGear);
        }
      }
      
      /* In case we are a satellite driver and it takes a long time for the
         master to ACK our reconfiguration request, check the port statuses
         of any ports pending to be active due to the reconfiguration sequence,
         and if we find the ports already active before we have received the
         master ACK, simulate the master ACK instead so that the client will
         be able to start servicing any ports that have become active */
      if ( pDevCtxt->uLocalReconfigPendCnt > 0  &&
           SB_SATELLITE_DRIVER == pDevCtxt->eDrvType )
      {
        uint32 uIdx, endPointNum;
        for (uIdx = 0; uIdx < pDevCtxt->numPorts; uIdx++)
        {
          SlimBusMasterPortType *pPort = &pDevCtxt->Ports[uIdx];
          if ( pPort->bBamConnected )
          {
            for (endPointNum = 0; endPointNum < pPort->numEndPoints; endPointNum++)
            {
              if ( NULL != pPort->aEndPoints[endPointNum].pChannel &&
                   NULL != pPort->aEndPoints[endPointNum].pChannel->pClientCtxt &&
                   pPort->aEndPoints[endPointNum].pChannel->pClientCtxt->bReConfigPending &&
                   SLIMBUS_CHANNEL_STATUS_ACTIVE == pPort->aEndPoints[endPointNum].pChannel->eNewStatus &&
                   SLIMBUS_CHANNEL_STATUS_ACTIVE != pPort->aEndPoints[endPointNum].pChannel->eCurStatus &&
                   HAL_sb_PgdPortIsChannelActive(pDevCtxt->hSB, 0, pPort->portNum) )
              {
                SlimBusDeviceType *pMgrDevice;
                SlimBusEventNodeType *pEvent = NULL;
                uint32 TID = pPort->aEndPoints[endPointNum].pChannel->pClientCtxt->uSatReconfigTID;
                
                pMgrDevice = pDevCtxt->pDevices[SB_MSG_LA_MANAGER&SB_LA_IDX_MASK];
                if (NULL != pMgrDevice)
                {
                  pEvent = pMgrDevice->TIDMap[TID&SB_TID_IDX_MASK];
                  if (NULL != pEvent)
                  {
                    /* Need to simulate success from USR_GENERIC_ACK receipt */
                    if (NULL != pEvent->pRxBuf)
                    {
                      *(uint32*)pEvent->pRxBuf = SB_ACK_SUCCESS_FLAG;
                    }
                    
                    SB_LOG_INFO_3("Simulating master ACK for reconfiguration sequence "
                                  "(channel already active) (client: 0x%lx) (channel: %ld) "
                                  "(port: %ld)",
                                  (uint32)pPort->aEndPoints[endPointNum].pChannel->pClientCtxt,
                                  pPort->aEndPoints[endPointNum].pChannel->channelNum,
                                  pPort->portNum );
  
                    SlimBus_CleanupMsgEvent(pMgrDevice, TID, pEvent, pSignalList);
                    
                    /* There could only have been one reconfiguration sequence
                       activated when RECONFIG_DONE is triggered, so once we 
                       trigger the client that was responsible for the reconfig
                       we are done */
                    break;
                  }
                }
              }
            }
          }
        }
      }
      
      /* If uPortIntDisMask is set and RECONFIG_DONE interrupt is enabled
         to catch disconnections from ports with interrupts disabled,
         the disconnection event will be captured below in the 
         PgdPortGetIntStatusEe() check */
    }
    if (uIntStatus & HAL_SB_MGR_INT_INVALID_TX_MSG)
    {
      SB_LOG_ERR_0("Invalid Tx message was sent");
    }
    if (uIntStatus & HAL_SB_MGR_INT_DEV_ERR)
    {
      SB_LOG_ERR_0("Msg device error");
    }
    if (uIntStatus & HAL_SB_MGR_INT_IE_VE_CHANGE)
    {
      SB_LOG_WARN_1("Msg device IE change (IE: 0x%lx)",
                    HAL_sb_DeviceGetIeStatus(pDevCtxt->hSB, pDevCtxt->eMsgDev));
    }
  }
  
  uPgdIntStatus = pIsrCtxt->uPgdPortIntRawStatus;
  uPgdIntStatus |= pDevCtxt->uPortDisconnectMask;
  if ( 0 != uPgdIntStatus )
  {
    uint32 i;
    for (i = 0; i < pDevCtxt->numPorts; i++)
    {
      SlimBusMasterPortType *pPort = &pDevCtxt->Ports[i];
      uint32 uPortNum = pPort->portNum;
      uint32 uIntMask = (1<<uPortNum);
      if ( uPgdIntStatus & uIntMask )
      {
        SlimBusPortEventType eEvents = SLIMBUS_EVENT_O_NONE;
        SBBOOL bIsIntEnabled = (pIsrCtxt->uPgdPortIntMaskedStatus & uIntMask);
        
        /* Don't record port overflow/underflow events if the port interrupt 
           is disabled or we are waiting for disconnection interrupt 
           or port disconnect is expected */
        if ( bIsIntEnabled &&
             !pPort->bSignalDisconnect &&
             (pPort->eDisconnectState == SB_PORT_NOT_DISCONNECTING ) )
        {
          if (uIntMask & pIsrCtxt->uPortOverFlowMask)
          {
            eEvents |= SLIMBUS_EVENT_O_FIFO_OVERRUN;
          }
          if (uIntMask & pIsrCtxt->uPortUnderFlowMask)
          {
            eEvents |= SLIMBUS_EVENT_O_FIFO_UNDERRUN;
          }
        }
        
        if ( (uIntMask & pIsrCtxt->uPortDisMask) ||
             ( (uIntMask & pDevCtxt->uPortDisconnectMask) &&
               !(uIntMask & pDevCtxt->uPortConnectMask) &&
               (HAL_sb_PgdPortGetChannelNumber(pDevCtxt->hSB, 0, uPortNum) == 0) ) )
        {
          SlimBus_ProcessPortDisconnection(pDevCtxt, pPort, &eEvents);
        }
        else if ( eEvents & (SLIMBUS_EVENT_O_FIFO_OVERRUN|SLIMBUS_EVENT_O_FIFO_UNDERRUN) )
        {
          uint32 endPointNum;
          for (endPointNum = 0; endPointNum < pPort->numEndPoints; endPointNum++)
          {
            if ( ( NULL == pPort->aEndPoints[endPointNum].pChannel ||
                   SLIMBUS_CHANNEL_STATUS_REMOVED != pPort->aEndPoints[endPointNum].pChannel->eNewStatus ) &&
                 SB_HANDLE_ALLOCATED == pPort->eState )
            {
              SB_LOG_WARN_3("Detected port overflow/underflow "
                            "(client: 0x%lx) (resource: 0x%lx) (port: %ld)",
                            (uint32)pPort->pClientCtxt, pPort->handle, pPort->portNum);
              break;
            }
          }

          /* Signal any client that is waiting for port descriptors to flush */
          if (pPort->uSignalDescEmptyCnt > 0)
          {
            if (NULL != pPort->pClientCtxt)
            {
              SlimBus_EventTrigger(pPort->pClientCtxt->hClientEvent);
            }
            pPort->uSignalDescEmptyCnt = 0;
          }
        }

        if ( pPort->eDisconnectState == SB_PORT_DISCONNECTING )
        {
          /* Clear and enable the interrupt if we are waiting for disconnection */
          HAL_sb_PgdPortClearIntEe(pDevCtxt->hSB, 0, pDevCtxt->pBSP->uMyEE, uIntMask);
          EnablePortInterrupt(pDevCtxt, uIntMask);
        }
        
        if ( pPort->eReqs & SLIMBUS_PORT_REQ_O_HALF_DUPLEX )
        {
          /* Note: For half-duplex port, only even port will be used for events */    
          pPort = &pDevCtxt->Ports[uPortNum&(~0x1)];
        }

        /* Mask out overflow/underflow events when performing channel removal */
        if (pDevCtxt->uPortDisconnectMask & uIntMask)
        {
          eEvents &= ~(SLIMBUS_EVENT_O_FIFO_OVERRUN|SLIMBUS_EVENT_O_FIFO_UNDERRUN);
        }
        /* Mask the new events by the events enabled by the client */
        eEvents &= pPort->eEnabledEvents;
        /* If there are no events left after masking, then clear out our
           internal interrupt status bit */
        if (SLIMBUS_EVENT_O_NONE == eEvents)
        {
          uPgdIntStatus &= ~uIntMask;
        }

        /* Add the new events to the active event bitmask */
        pPort->eActiveEvents |= eEvents;
      }
    }

    /* Enable the BAM reset timer if there are new ports needed to be reset 
       and timer is not already running */
    if ( (0 == pDevCtxt->uPortBamResetMask) &&
         (0 != pDevCtxt->uPortBamResetDeferMask) )
    {
      pDevCtxt->uPortBamResetMask = pDevCtxt->uPortBamResetDeferMask;
      pDevCtxt->uPortBamResetDeferMask = 0;

      SlimBus_TimerStart(pDevCtxt,
                         pDevCtxt->hBamResetTimerHandle,
                         pDevCtxt->hBamResetTimerEvent,
                         SB_BAM_RESET_TIMEOUT);
    }
  }

  if (SB_MASTER_DRIVER == pDevCtxt->eDrvType)
  {
    SlimBus_HandleMasterInterrupts(pDevCtxt, pIsrCtxt);
  }
  
  /* Signal clients waiting for hardware to enumerate */
  if ( NULL != PeekHeadClient(&pDevCtxt->EnumWaitList) &&
       !HAL_sb_FrmIsInIdleMode(pDevCtxt->hSB) &&
       SlimBus_HardwareIsEnumerated(pDevCtxt) &&
       ( NULL == pDevCtxt->pSched ||
         !pDevCtxt->pSched->bMasterWorkerBusy ) )
  {
    SlimBusBasicClientCtxt *pClientCtxt;
    
    /* Cancel the enumeration timer */
    SlimBus_TimerStart(pDevCtxt, pDevCtxt->hEnumTimerHandle,
                       pDevCtxt->hEnumTimerEvent, 0);
    SlimBus_TimerStart(pDevCtxt, pDevCtxt->hSFTimerHandle,
                       pDevCtxt->hSFTimerEvent, 0);
            
    while (NULL != (pClientCtxt = PopHeadClient(&pDevCtxt->EnumWaitList)) )
    {
      if (NULL != pClientCtxt->pClientCtxt)
      {
        SlimBus_EventTrigger(pClientCtxt->pClientCtxt->hClientEvent);
      }
    }
  }
  
  /* signal events here to avoid deadlock */
  if ( uPgdIntStatus != 0 )
  {
    uint32 i;
    for (i = 0; i < pDevCtxt->numPorts; i++)
    {
      SlimBusMasterPortType *pPort = &pDevCtxt->Ports[i];
      uint32 uPortNum = pPort->portNum;
      uint32 uIntMask = (1<<uPortNum);

      if ( uPgdIntStatus & uIntMask )
      {
        if (NULL != pPort->hEvent)
        {
          SlimBus_EventTriggerEx(pPort->hEvent, pPort->pEventUserData);
        }
      }
    }
  }
  
  return SB_SUCCESS;
}


/**
 * @brief Handles a BAM reset timer event
 *
 * This function handles a BAM reset timer event
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 */
void SlimBusBamResetTimerCallback(SlimBusDevCtxt *pDevCtxt)
{
  SlimBus_LockDevice(pDevCtxt);

  /* Reset the BAM pipe for any ports that need it reset */
  if (0 != pDevCtxt->uPortBamResetMask)
  {
    uint32 uIdx;
    for ( uIdx = 0; uIdx < pDevCtxt->numPorts; uIdx++ )
    {
      SlimBusMasterPortType *pPort = &pDevCtxt->Ports[uIdx];
      uint32 uIntMask = (1<<pPort->portNum);
      
      if ( ( uIntMask & pDevCtxt->uPortBamResetMask ) &&
           pPort->bBamConnected )
      {
        SB_LOG_VERB_3("disconnecting BAM pipe (client: 0x%lx) (resource: 0x%lx) (port: %ld)",
                      (uint32)pPort->pClientCtxt, pPort->handle, pPort->portNum);

        SlimBus_PortAcquireLock(pDevCtxt, pPort);
        SlimBus_DisconnectBAM(pPort);
        SlimBus_HandlePortDisconnect(pDevCtxt, pPort);
        SlimBus_PortReleaseLock(pDevCtxt, pPort);
      }
    }
  }
  
  pDevCtxt->uPortBamResetMask = pDevCtxt->uPortBamResetDeferMask;
  pDevCtxt->uPortBamResetDeferMask = 0;

  /* If we still have ports that need the BAM reset, then set the timer again */
  if (0 != pDevCtxt->uPortBamResetMask)
  {
    SlimBus_TimerStart(pDevCtxt, pDevCtxt->hBamResetTimerHandle,
                       pDevCtxt->hBamResetTimerEvent,
                       SB_BAM_RESET_TIMEOUT);
  }

  (void) pDevCtxt->checkEnterLowPowerFcn(pDevCtxt);

  SlimBus_UnlockDevice(pDevCtxt);
}

/**
 * @brief Reset the Slimbus hardware core following an 
 *        enumeration timeout
 *
 * This function resets the Slimbus hardare core following an 
 * enumeration timeout 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 */
void SlimBusEnumFailHardwareReset(SlimBusDevCtxt *pDevCtxt)
{
  SlimBusEventNodeType *pEvent, *pNextEvent;
  uint32 i = 0;

  if (!pDevCtxt->bIsHwOwner)
  {
    return;
  }
  
  if ( HAL_sb_FrmIsInIdleMode(pDevCtxt->hSB) )
  {
    SB_LOG_ERR_0("Framer failed to leave idle mode after timeout");
  }
  else
  {
    SB_LOG_ERR_3("Hardware failed to enumerate after timeout (FS: %d) (SFS: %d) (MS: %d)",
                 HAL_sb_IntfIsFrameSyncAcquired(pDevCtxt->hSB),
                 HAL_sb_IntfIsSuperframeSyncAcquired(pDevCtxt->hSB),
                 HAL_sb_IntfIsMessageSyncAcquired(pDevCtxt->hSB));
  }

  /* Reset the hardware */
  if (TRUE != HAL_sb_CompReset(pDevCtxt->hSB))
  {
    SB_LOG_ERR_0("Hardware reset failed after enumeration timeout");
  }
  else
  {
    SB_LOG_INFO_0("Hardware reset successful after enumeration timeout");
  }

  /* Make sure all the Tx events are removed from the device event queue */
  pEvent = PeekHeadEvent(&pDevCtxt->TxEventQueue);
  while (NULL != pEvent)
  {
     pNextEvent = NextEventNode(&pDevCtxt->TxEventQueue, pEvent);
     if (NULL != pEvent->peMsgErr)
     {
       *(pEvent->peMsgErr) = SLIMBUS_ERR_MSG_NACKED;
     }
     RemoveEventNode(pEvent);
     if(NULL != pEvent->hEvent)
     {
       SlimBus_EventTrigger(pEvent->hEvent);
     }
     pEvent = pNextEvent;
  }

  /* Re-initialize hardware */
  if(SB_SUCCESS != SlimBus_DeviceInitHardware(pDevCtxt))
  {
    SB_LOG_ERR_0("Hardware initialization failed after enumeration timeout");
  }

  /* As Bam reset is happening in SlimBus_DeviceInitHardware, mark the bamhandles NULL*/
  for(i = 0;i < pDevCtxt->numPorts ;i++)
  {
    SlimBusMasterPortType *pPort = &pDevCtxt->Ports[i];
    
    if(pPort != NULL)
    {
      if(pPort->eState == SB_HANDLE_ALLOCATED)
      {
        SlimBus_ClearBamhandle(pPort);
      }
    }
  }
}

/**
 * @brief Handles a hardware enumeration timer event
 *
 * This function handles a hardware enumeration timer event
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 */
void SlimBusEnumTimerCallback(SlimBusDevCtxt *pDevCtxt)
{
  SlimBus_LockDevice(pDevCtxt);

  if ( pDevCtxt->bClockIsOn &&
       (  HAL_sb_FrmIsInIdleMode(pDevCtxt->hSB) ||
         !SlimBus_HardwareIsEnumerated(pDevCtxt) ||
          SB_PAUSE_RESUMING_FROM_PAUSE == pDevCtxt->pSched->ePause ) )
  {
    SlimBusEnumFailHardwareReset(pDevCtxt);
  }
  else
  {
    /* Signal clients waiting for hardware to enumerate */
    SlimBusBasicClientCtxt *pClientCtxt;
    
    while (NULL != (pClientCtxt = PopHeadClient(&pDevCtxt->EnumWaitList)) )
    {
      if (NULL != pClientCtxt->pClientCtxt)
      {
        SlimBus_EventTrigger(pClientCtxt->pClientCtxt->hClientEvent);
      }
    }

    if ( NULL != pDevCtxt->pSched &&
         pDevCtxt->pSched->bSendPowerNotify )
    {
      /* signal the reconfiguration workloop */
      SlimBus_EventTrigger(pDevCtxt->hMasterWorkLoopEvent);
    }
  }

  SlimBus_UnlockDevice(pDevCtxt);
}


/**
 * @brief Handles a hardware superframe sync timer event
 *
 * This function handles a hardware superframe sync timer event
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 */
void SlimBusSFTimerCallback(SlimBusDevCtxt *pDevCtxt)
{
  SlimBus_LockDevice(pDevCtxt);

  if ( pDevCtxt->bClockIsOn )
  {
    if ( !HAL_sb_IntfIsFrameSyncAcquired(pDevCtxt->hSB) )
    {
      if (++pDevCtxt->uFrameSyncTimerCnt > SB_ENUMERATION_TIMEOUT/1000)
      {
        pDevCtxt->uFrameSyncTimerCnt = 0;
        SlimBusEnumFailHardwareReset(pDevCtxt);
      }
    }
    else if ( !HAL_sb_IntfIsSuperframeSyncAcquired(pDevCtxt->hSB) )
    {
      uint32 uTimerCnt = 5;
      uint32 uClkGear = 8;
      
      pDevCtxt->uFrameSyncTimerCnt = 0;
      
      for (uClkGear = 8; uClkGear > pDevCtxt->uFrmClkGear; uClkGear--)
      {
        uTimerCnt *= 2;
      }
      if (SB_MASTER_DRIVER != pDevCtxt->eDrvType)
      {
        uTimerCnt = 20;
      }

      if (++pDevCtxt->uSFTimerCnt >= uTimerCnt)
      {
        if (!HAL_sb_IsFrameSyncShiftSupported(pDevCtxt->hSB))
        {
          pDevCtxt->uSFTimerCnt = 0;

          HAL_sb_DeviceDisable(pDevCtxt->hSB, HAL_SB_DEVICE_INTERFACE);
          SlimBus_DeinitGpios(pDevCtxt);
          SLIMBUS_BUSYWAIT(uTimerCnt*4);
          SlimBus_InitGpios(pDevCtxt);
          HAL_sb_DeviceEnable(pDevCtxt->hSB, HAL_SB_DEVICE_INTERFACE);
          SB_LOG_INFO_0("Shifting superframe sync acquisition offset (FS: 1)");
        }
        else
        {
          SB_LOG_INFO_0("Received a timer event while waiting for superframe sync");
        }
      }
    }
    else
    {
      pDevCtxt->uFrameSyncTimerCnt = 0;
    }
    
    if ( !HAL_sb_IntfIsMessageSyncAcquired(pDevCtxt->hSB) )
    {
      SlimBus_TimerStart(pDevCtxt,
                         pDevCtxt->hSFTimerHandle,
                         pDevCtxt->hSFTimerEvent,
                         SB_SUPERFRAME_BASE_TIMEOUT);
    }
    else if ( !SlimBus_HardwareIsEnumerated(pDevCtxt) )
    {
      SlimBus_TimerStart(pDevCtxt,
                         pDevCtxt->hEnumTimerHandle,
                         pDevCtxt->hEnumTimerEvent,
                         SB_ENUMERATION_TIMEOUT);
    }
    else if ( NULL == pDevCtxt->pSched ||
              !pDevCtxt->pSched->bMasterWorkerBusy )
    {
      SlimBusBasicClientCtxt *pClientCtxt;

      while (NULL != (pClientCtxt = PopHeadClient(&pDevCtxt->EnumWaitList)) )
      {
        if (NULL != pClientCtxt->pClientCtxt)
        {
          SlimBus_EventTrigger(pClientCtxt->pClientCtxt->hClientEvent);
        }
      }
      
      if ( NULL != pDevCtxt->pSched &&
           pDevCtxt->pSched->bSendPowerNotify )
      {
        /* signal the reconfiguration workloop */
        SlimBus_EventTrigger(pDevCtxt->hMasterWorkLoopEvent);
      }
    }
  }

  SlimBus_UnlockDevice(pDevCtxt);
}


/**
 * @brief Initialize the client context 
 *  
 * This function initializes the client context data structure
 * 
 * @param[in] pCtxt  Pointer to the client context
 * 
 * @return SB_SUCCESS on success, an error code on error 
 */
static SBResult
SlimBus_ClientInit(SlimBusClientCtxt *pCtxt)
{
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pCtxt->hDevCtxt);
  SBResult nErr = SB_SUCCESS;
  int i;
  
  for (i = 0; i < (int)pDevCtxt->numClientSlots; i++)
  {
    if (NULL == pDevCtxt->paClientCtxt[i])
    {
      break;
    }
  }
  if (i >= (int)pDevCtxt->numClientSlots)
  {
    uint32 uNewClientSlots = pDevCtxt->numClientSlots * 2;
    if (SB_SUCCESS != SlimBus_Realloc(uNewClientSlots*sizeof(pDevCtxt->paClientCtxt[0]),
                                      pDevCtxt->numClientSlots*sizeof(pDevCtxt->paClientCtxt[0]),
                                      (void**)&pDevCtxt->paClientCtxt, SB_MEMTAG_4))
    {
      return SB_ERROR;
    }
    pDevCtxt->numClientSlots = uNewClientSlots;
  }
  pCtxt->BasicClient.uClientNum = i;
  pDevCtxt->paClientCtxt[i] = pCtxt;
  
  nErr = SlimBus_CreateSyncEvent(&pCtxt->hClientEvent, &pCtxt->ClientEventObj);
  if (SB_SUCCESS != nErr)
  {
    /* TODO: cleanup */
    return nErr;
  }

  if (NULL == pCtxt->hSync)
  {
    nErr = SlimBus_CreateClientLock(pCtxt);
    if (SB_SUCCESS != nErr)
    {
      return nErr;
    }
  }

  nErr = SlimBus_CreateTimeoutEvent(&pCtxt->hTimeoutEvent, NULL);
  if (SB_SUCCESS != nErr)
  {
    /* TODO: cleanup */
    return nErr;
  }
  
  RootNodeInit(&pCtxt->FreeEventStack);
  for (i = 0; i < sizeof(pCtxt->EventNodes)/sizeof(pCtxt->EventNodes[0]); i++)
  {
    pCtxt->EventNodes[i].pFreeRoot = &pCtxt->FreeEventStack;
    PushHeadEvent(&pCtxt->FreeEventStack, &pCtxt->EventNodes[i]);
  }
  
  memset(&pCtxt->BasicClient.ConfigMsgFifo, 0, sizeof(pCtxt->BasicClient.ConfigMsgFifo));
  if ( SB_MASTER_DRIVER == SB_GET_PDEVCTXT(pCtxt->hDevCtxt)->eDrvType )
  {
    if (SB_SUCCESS != SlimBus_Malloc(SB_CFG_MSG_BUF_LENGTH*sizeof(uint32)+SB_MAX_MSG_LENGTH_BYTES,
                                      (void**)&(pCtxt->BasicClient.ConfigMsgFifo.pBuf), SB_MEMTAG_5))
    {
      /* TODO: cleanup */
      return SB_ERROR;
    }
    pCtxt->BasicClient.ConfigMsgFifo.uLen = SB_CFG_MSG_BUF_LENGTH;
    pCtxt->BasicClient.ConfigMsgFifo.uAckIdx = SB_ACK_IDX_UNUSED;
  }
  pCtxt->BasicClient.ConfigMsgFifo.getBufFcn = SlimBus_GetMsgBuffer;
  pCtxt->BasicClient.ConfigMsgFifo.sendBufFcn = SlimBus_DefaultSendBuffer;
  pCtxt->BasicClient.ConfigMsgFifo.hDevCtxt = pCtxt->hDevCtxt;
  RootNodeInit(&pCtxt->BasicClient.ConfigMsgFifo.WaitList);
  pCtxt->BasicClient.pClientCtxt = pCtxt;
  
  memset(&pCtxt->BatchMsgFifo, 0, sizeof(pCtxt->BatchMsgFifo));
  pCtxt->BatchMsgFifo.getBufFcn = SlimBus_GetMsgBuffer;
  pCtxt->BatchMsgFifo.sendBufFcn = SlimBus_DefaultSendBuffer;
  pCtxt->BatchMsgFifo.hDevCtxt = pCtxt->hDevCtxt;
    
  pCtxt->eLastMsgErr = SLIMBUS_ERR_NONE;
  
  return nErr;
}

/**
 * @brief Initialize the Tx messaging FIFO
 *
 * This function initializes the Tx messaging FIFO
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 */
void SlimBus_InitTxFifo(SlimBusDevCtxt *pDevCtxt)
{
  pDevCtxt->TxMsgFifo.uLen = SB_TX_MSG_BUF_LENGTH; /* Note: doesn't include tail pad */
  pDevCtxt->TxMsgFifo.bBusy = FALSE;
  pDevCtxt->TxMsgFifo.eFlags = SB_MSG_FIFO_ENABLE_NOTIFY;
  pDevCtxt->TxMsgFifo.getBufFcn = SlimBus_GetTxMsgBuffer;
  pDevCtxt->TxMsgFifo.sendBufFcn = SlimBus_SendTxBuffer;
  pDevCtxt->TxMsgFifo.hDevCtxt = SlimBus_GetDevHandle(pDevCtxt);
  RootNodeInit(&pDevCtxt->TxMsgFifo.WaitList);
}

/**
 * @brief Initialize the Rx messaging FIFO
 *
 * This function initializes the Rx messaging FIFO
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 */
void SlimBus_InitRxFifo(SlimBusDevCtxt *pDevCtxt)
{
  pDevCtxt->RxMsgFifo.uLen = SB_RX_MSG_BUF_LENGTH; /* Note: doesn't include tail pad */
  pDevCtxt->RxMsgFifo.eFlags = SB_MSG_FIFO_FLAGS_NONE;
  pDevCtxt->RxMsgFifo.uAckIdx = SB_ACK_IDX_UNUSED;
  pDevCtxt->RxMsgFifo.bBusy = FALSE; /* Note: bBusy not used for Rx */
  pDevCtxt->RxMsgFifo.getBufFcn = SlimBus_GetMsgBuffer;
  pDevCtxt->RxMsgFifo.sendBufFcn = SlimBus_DefaultSendBuffer;
  pDevCtxt->RxMsgFifo.hDevCtxt = SlimBus_GetDevHandle(pDevCtxt);
  RootNodeInit(&pDevCtxt->RxMsgFifo.WaitList);
}

/**
 * @brief Check if retention has been lost and restore 
 *        non-retained registers if necessary
 *
 * This function checks if retention has been lost and restores
 *        non-retained registers if necessary
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return None
 */
void SlimBus_CheckAndRestoreNonRetentionRegisters(SlimBusDevCtxt *pDevCtxt)
{
  /* Check if we are waking from retention */
  if ( HAL_sb_CompIsRetentionEnabled(pDevCtxt->hSB) &&
       0 == HAL_sb_DeviceIntGetMask(pDevCtxt->hSB, pDevCtxt->eMsgDev) )
  {
    SB_LOG_INFO_0("Restoring non-retention registers");

    /* Reset the message queues if they were already configured */
    if (NULL != pDevCtxt->RxMsgFifo.pBamCtxt)
    {
      SlimBus_ResetMsgFifo(&pDevCtxt->RxMsgFifo);
    }
    if (NULL != pDevCtxt->TxMsgFifo.pBamCtxt)
    {
      SlimBus_ResetMsgFifo(&pDevCtxt->TxMsgFifo);
    }

    if (pDevCtxt->bIsHwOwner)
    {
      SlimBus_BamDeinit(pDevCtxt);
      SlimBus_BamInit(pDevCtxt);
      SlimBus_ResetBAM(pDevCtxt);
    }

    SlimBus_InitRxFifo(pDevCtxt);
    SlimBus_InitTxFifo(pDevCtxt);

    SlimBus_SetupMsgQueues(pDevCtxt);

    SlimBus_DeviceEnableInterrupts(pDevCtxt);

    if(pDevCtxt->eWrkrnd & SB_WRKRND_O_DATASHIFT_INVALIDATE)
    {
      uint32 i;
      /*32bit  Datashift at the port level due to HWBUG QCTDD03351991 will be get cleared
        after the core reset, so we need to clear the corresponding flag */
      for ( i = 0; i < pDevCtxt->numPorts; i++ )
      {
        if (pDevCtxt->Ports[i].bDataShiftRemoval)
        {
          pDevCtxt->Ports[i].bDataShiftRemoval = FALSE;
          SB_LOG_INFO_1("Cleared datashift workaround bit (port %d)",
                        pDevCtxt->Ports[i].portNum);
        }
      }
    }
  }
}

/**
 * @brief Initialize the device
 *
 * This function initializes the device
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
SBResult SlimBus_DeviceInitInternal(SlimBusDevCtxt *pDevCtxt)
{
  SBResult result = SB_SUCCESS;
  uint32 i;
  uint32 uPortBitField = 0;

  /* Note: hSync object already entered in DeviceAttach() function */
  do
  {
    result = SlimBus_InitDriverType(pDevCtxt);
    if (SB_SUCCESS != result)
    {
      break;
    }

    pDevCtxt->bPendingSSR = FALSE;

    /* Perform target-specific initialization */
    result = SlimBus_DeviceInitTarget(pDevCtxt);

    /* Initialize logging facility */
    SB_LOG_INIT(pDevCtxt);

    if (SB_SUCCESS != result)
    {
      SB_LOG_ERR_0("Error performing target-specific initialization");
      break;
    }

    /* Validate the BSP data */
    if ( NULL == pDevCtxt->pBSP ||
         SLIMBUS_BSP_VER_LATEST != pDevCtxt->pBSP->uStructVer )
    {
      result = SB_ERROR;
      break;
    }

    /* Use the common workarounds as a starting point */
    pDevCtxt->eWrkrnd = COMMON_WORKAROUNDS;

    pDevCtxt->numClientSlots = SB_DEFAULT_CLIENTS_PER_DEVICE;
    if (SB_SUCCESS != SlimBus_Malloc((SB_DEFAULT_CLIENTS_PER_DEVICE*
                                       sizeof(pDevCtxt->paClientCtxt[0])),
                                      (void**)&pDevCtxt->paClientCtxt, SB_MEMTAG_6))
    {
      result = SB_ERROR;
      break;
    }

    memset(pDevCtxt->paClientCtxt, 0,
           pDevCtxt->numClientSlots*sizeof(pDevCtxt->paClientCtxt[0]));
    
    pDevCtxt->uDefClkGear = SB_MAX_CLK_GEAR;
    pDevCtxt->uMinClkGear = 0;

    result = SlimBus_InitParams(pDevCtxt);
    if (SB_SUCCESS != result)
    {
      break;
    }

    /* Grab some cheap entropy */  
    pDevCtxt->prnSeed = (uint8)(((size_t)pDevCtxt>>2) ^ ((size_t)&result>>2));

    pDevCtxt->numDeviceSlots = SB_MAX_DEVICES;
    pDevCtxt->numGroupSlots = SB_MAX_GROUPS;

    if (pDevCtxt->uBasePortNum > SB_MAX_MASTER_PORTS)
    {
      pDevCtxt->uBasePortNum = SB_MAX_MASTER_PORTS;
    }
    if (pDevCtxt->numPorts + pDevCtxt->uBasePortNum > SB_MAX_MASTER_PORTS)
    {
      pDevCtxt->numPorts = SB_MAX_MASTER_PORTS - pDevCtxt->uBasePortNum;
    }
    if (pDevCtxt->uBaseChanNum + pDevCtxt->numChannelSlots > pDevCtxt->uBaseSharedChanNum)
    {
      pDevCtxt->numChannelSlots = pDevCtxt->uBaseSharedChanNum - pDevCtxt->uBaseChanNum;
    }
    if (pDevCtxt->uBaseSharedChanNum + pDevCtxt->numSharedChannelSlots > SB_MAX_DATA_CHANNEL_PARAMS)
    {
      pDevCtxt->numSharedChannelSlots = SB_MAX_DATA_CHANNEL_PARAMS - pDevCtxt->uBaseSharedChanNum;
    }
    if (pDevCtxt->numCounterSlots > SB_MAX_COUNTERS)
    {
      pDevCtxt->numCounterSlots = SB_MAX_COUNTERS;
    }
    
    /* Initialize the port, channel, counter and group data structures */
    for ( i = 0; i < pDevCtxt->numPorts; i++ )
    {
      pDevCtxt->Ports[i].handle = SB_HANDLE_INVALID;
      pDevCtxt->Ports[i].eState = SB_HANDLE_AVAILABLE;
      pDevCtxt->Ports[i].portNum = pDevCtxt->uBasePortNum + i;
      uPortBitField |= (1 << pDevCtxt->Ports[i].portNum);
      pDevCtxt->Ports[i].numEndPoints = 0;
    }
    for ( i = 0; i < pDevCtxt->numCounterSlots; i++ )
    {
      pDevCtxt->Counters[i].counterNum = pDevCtxt->uBasePortNum + i;
      pDevCtxt->Counters[i].handle = SB_HANDLE_INVALID;
      pDevCtxt->Counters[i].eState = SB_HANDLE_AVAILABLE;
    }
    for ( i = 0; i < pDevCtxt->numGroupSlots; i++ )
    {
      pDevCtxt->Groups[i].handle = SB_HANDLE_INVALID;
      RootNodeInit(&pDevCtxt->Groups[i].ResourceList);
    }

    /* Initialize the Tx message FIFO data structure */
    SlimBus_InitTxFifo(pDevCtxt);

    /* Initialize the Rx message FIFO data structure */
    SlimBus_InitRxFifo(pDevCtxt);

    /* Allocate buffer space for message buffers */
    SlimBus_AllocMsgBuffers(pDevCtxt, NULL);

    /* Initialize the Tx and Rx event queues */  
    RootNodeInit(&pDevCtxt->TxEventQueue);
    RootNodeInit(&pDevCtxt->RxEventQueue);

    RootNodeInit(&pDevCtxt->PauseEventQueue);

    RootNodeInit(&pDevCtxt->MsgWaitList);
    RootNodeInit(&pDevCtxt->EnumWaitList);

    result = SlimBus_InitServices(pDevCtxt);
    if (SB_SUCCESS != result)
    {
      break;
    }
    
    pDevCtxt->uRootFreqDiv8 = SB_DEFAULT_ROOT_FREQ_HZ_DIV8;
    pDevCtxt->uSuperFreq = pDevCtxt->uRootFreqDiv8 / SB_CELLS_PER_SUPERFRAME_DIV8;
    /* Manager LA is not valid, denotes we need to lookup the PGD LA */
    pDevCtxt->pgdLA = SB_MSG_LA_MANAGER;
    
    /* Do master or satellite specific initialization */
    if ( SB_MASTER_DRIVER == pDevCtxt->eDrvType )
    {
      if ( !SlimBus_IsMasterSupported() )
      {
        result = SB_ERROR;
        break;
      }
      result = SlimBus_DeviceInitMaster(pDevCtxt);
      if (SB_SUCCESS != result)
      {
        break;
      }
    }
    else
    {
      if ( !SlimBus_IsSatSupported() )
      {
        result = SB_ERROR;
        break;
      }
      result = SlimBus_DeviceInitSat(pDevCtxt);
      if (SB_SUCCESS != result)
      {
        break;
      }
    }

    pDevCtxt->uNewClkGear = pDevCtxt->uFrmClkGear = pDevCtxt->uDefClkGear;
    
    if(!pDevCtxt->bSSRcbRegistered)
    {
      result = SlimBus_RegisterSSRcb(pDevCtxt);
      if (SB_SUCCESS != result)
      {
        return result;
      }
      pDevCtxt->bSSRcbRegistered = TRUE;
    }

    SlimBus_MarkDeviceStatic(pDevCtxt);
    
    SB_LOG_INFO_3("*** Driver Initialization (master: %d) (protocols: 0x%x) "
                  "(workarounds: 0x%x)", (pDevCtxt->eDrvType == SB_MASTER_DRIVER),
                  SB_QC_SUPPORTED_PROTOCOLS, pDevCtxt->eWrkrnd);
  } while (FALSE);

  return result;
}


/**
 * @brief Enable interrupts
 *
 * This function enables interrupts
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return SB_SUCCESS on sucess, error code on error 
 */
SBResult SlimBus_DeviceEnableInterrupts(SlimBusDevCtxt *pDevCtxt)
{
  if (pDevCtxt->bManangeInterruptEnable)
  {
    uint32 uToken;
  
    SlimBus_InterruptAcquireLock(pDevCtxt, &uToken);
  
    HAL_sb_DeviceIntSetMask(pDevCtxt->hSB, pDevCtxt->eMsgDev, (uint32)
                            (HAL_SB_NGD_INT_DEV_ERR|
                             HAL_SB_NGD_INT_IE_VE_CHANGE|
                             HAL_SB_NGD_INT_TX_MSG_BUF_CONTENTION|
                             HAL_SB_NGD_INT_TX_MSG_NACKED_TWICE|
                             HAL_SB_NGD_INT_TX_MSG_SENT|
                             HAL_SB_NGD_INT_RX_MSG_RCVD|
                             HAL_SB_NGD_INT_INVALID_TX_MSG|
                             HAL_SB_MGR_INT_RECONFIG_DONE) );
    
    if (pDevCtxt->bIsHwOwner)
    {
      HAL_sb_DeviceIntSetMask(pDevCtxt->hSB, HAL_SB_DEVICE_FRAMER, (uint32)
                              (HAL_SB_FRM_INT_DEV_ERR|
                               HAL_SB_FRM_INT_IDLE_MODE_EXITED|
                               HAL_SB_FRM_INT_IE_VE_CHANGE) );
  
      HAL_sb_DeviceIntSetMask(pDevCtxt->hSB, HAL_SB_DEVICE_INTERFACE, (uint32)
                              (HAL_SB_INTF_INT_DEV_ERR|
                               HAL_SB_INTF_INT_IE_VE_CHANGE|
                               HAL_SB_INTF_INT_NO_DATA_LINE_ACTIVITY) );
  
      HAL_sb_DeviceIntSetMask(pDevCtxt->hSB, HAL_SB_DEVICE_PORTED_GENERIC, (uint32)
                              (HAL_SB_INTF_INT_DEV_ERR|
                               HAL_SB_INTF_INT_IE_VE_CHANGE) );
    }
    
    SlimBus_InterruptReleaseLock(pDevCtxt, &uToken);
  }

  return SB_SUCCESS;
}

/**
 * @brief Disable interrupts
 *
 * This function disables interrupts
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return SB_SUCCESS on sucess, error code on error 
 */
SBResult SlimBus_DeviceDisableInterrupts(SlimBusDevCtxt *pDevCtxt)
{
  if (pDevCtxt->bManangeInterruptEnable)
  {
    uint32 uToken;
  
    SlimBus_InterruptAcquireLock(pDevCtxt, &uToken);
  
    /* Need to leave some bit set in the interrupt mask to detect retention */
    HAL_sb_DeviceIntSetMask(pDevCtxt->hSB, pDevCtxt->eMsgDev,
                            (uint32)HAL_SB_NGD_INT_TX_MSG_BUF_CONTENTION);
  
    if (pDevCtxt->bIsHwOwner)
    {
      HAL_sb_DeviceIntSetMask(pDevCtxt->hSB, HAL_SB_DEVICE_FRAMER, 0);
  
      HAL_sb_DeviceIntSetMask(pDevCtxt->hSB, HAL_SB_DEVICE_INTERFACE, 0);
  
      HAL_sb_DeviceIntSetMask(pDevCtxt->hSB, HAL_SB_DEVICE_PORTED_GENERIC, 0);
    }
    
    SlimBus_InterruptReleaseLock(pDevCtxt, &uToken);
  }

  return SB_SUCCESS;
}

/**
 * @brief Initialize the device
 *
 * This function initializes the device
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
SBResult
SlimBus_DeviceInitHardware(SlimBusDevCtxt *pDevCtxt)
{
  SBResult result = SB_SUCCESS;
  uint32 uNumHwPorts, uNumHwCounters;
  uint32 i = 0;

  if ( pDevCtxt->bHwInit || !pDevCtxt->bIsHwOwner )
  {
    /* If software thinks hardware is initialized and it is, then
       don't initialize again.  If we own the hardware and software
       doesn't think it is initialized, then always do the initialization
       (to cover resetting the hardware during software restart). */
    if ( HAL_sb_DeviceIsEnabled(pDevCtxt->hSB, pDevCtxt->eMsgDev) )
    {
      return SB_SUCCESS;
    }
  }
  
  /* Reset the messages queues if they were already configured */
  if (NULL != pDevCtxt->RxMsgFifo.pBamCtxt)
  {
    SlimBus_ResetMsgFifo(&pDevCtxt->RxMsgFifo);
    SlimBus_InitRxFifo(pDevCtxt);
  }
  if (NULL != pDevCtxt->TxMsgFifo.pBamCtxt)
  {
    SlimBus_ResetMsgFifo(&pDevCtxt->TxMsgFifo);
    SlimBus_InitTxFifo(pDevCtxt);
  }
  
  if ( pDevCtxt->bIsHwOwner )
  {
    uint32 uSubFrmCode = 0;
    
    if (NULL != pDevCtxt->pSched)
    {
      uSubFrmCode = pDevCtxt->pSched->uSubFrmCode;
    }
    
    if ( !pDevCtxt->bHwInit || 
         !pDevCtxt->bActiveFramer )
    {
      if ( SlimBus_CheckGpioClockToggle(pDevCtxt) )
      {
        if ( pDevCtxt->bActiveFramer )
        {
          SB_LOG_WARN_0("Switching to external framer mode due to external clock toggle");
          pDevCtxt->bActiveFramer = FALSE;
        
          /* If hardware has never been initialized and we expect to be the active
             framer yet detect an external framer, assume that the master driver
             has been restarted, and therefore need to flush any active channels */
          if ( SB_MASTER_DRIVER == pDevCtxt->eDrvType &&
               NULL != pDevCtxt->pSched )
          {
            pDevCtxt->pSched->bPurge = TRUE;
          }
        }
      }
      else
      {
        if ( !pDevCtxt->bActiveFramer )
        {
          if ( SB_MASTER_DRIVER == pDevCtxt->eDrvType &&
               NULL != pDevCtxt->pSched )
          {
            SB_LOG_WARN_0("Switching to active framer mode due to lack of external clock toggle");
            pDevCtxt->bActiveFramer = TRUE;
            pDevCtxt->pSched->bPurge = TRUE;
          }
          else
          {
            SB_LOG_WARN_0("Bus appears to be paused but driver is expecting external framer");
          }
        }
      }
    }
    
    result = SlimBus_InitGpios(pDevCtxt);
    if (SB_SUCCESS != result)
    {
      SB_LOG_ERR_0("failed init GPIOs");
      return result;
    }

    /* Make sure we can't send messages to PGD until it has enumerated */
    /* Use MANAGER LA to denote that we haven't yet looked up the PGD */
    if ( SB_MSG_LA_MANAGER != pDevCtxt->pgdLA )
    {
      SlimBusDeviceType *pDevice = pDevCtxt->pDevices[(pDevCtxt->pgdLA)&SB_LA_IDX_MASK];
      if (NULL != pDevice)
      {
        pDevice->uAvailTrans = 0;
      }
    }

    if (TRUE != HAL_sb_CompReset(pDevCtxt->hSB))
    {
      SB_LOG_ERR_0("Hardware reset failed");
    }

    SlimBus_BamDeinit(pDevCtxt);
    SlimBus_BamInit(pDevCtxt);
    SlimBus_ResetBAM(pDevCtxt);

    SlimBus_SetupMsgQueues(pDevCtxt);

    if (pDevCtxt->eWrkrnd & SB_WRKRND_O_SETUP_TRUST)
    {
      HAL_sb_CompSetMgrRscGrpEe(pDevCtxt->hSB, pDevCtxt->auEeAssign[0]);
      HAL_sb_CompSetNgdEe(pDevCtxt->hSB, HAL_SB_NON_PORTED_GENERIC_1,
                          pDevCtxt->auEeAssign[1]);
      HAL_sb_CompSetNgdEe(pDevCtxt->hSB, HAL_SB_NON_PORTED_GENERIC_2,
                          pDevCtxt->auEeAssign[2]);
    }

    HAL_sb_FrmInternalWakeupEnable(pDevCtxt->hSB);
    HAL_sb_FrmSetRefClkClkGear(pDevCtxt->hSB, SB_MAX_CLK_GEAR);
    HAL_sb_FrmSetBootRootFreq(pDevCtxt->hSB, 1);
    HAL_sb_FrmSetBootClkGear(pDevCtxt->hSB, pDevCtxt->uFrmClkGear);
    HAL_sb_FrmSetBootSubFrmMode(pDevCtxt->hSB, uSubFrmCode);

    HAL_sb_CompEnablePhaseShift(pDevCtxt->hSB);
    HAL_sb_CompInitRetention(pDevCtxt->hSB);
    
    if (pDevCtxt->bActiveFramer)
    {
      HAL_sb_FrmBootSetActiveFrm(pDevCtxt->hSB);
    }
    else
    {
      HAL_sb_FrmBootSetUnActiveFrm(pDevCtxt->hSB);
    }
    
    HAL_sb_DeviceEnable(pDevCtxt->hSB, HAL_SB_DEVICE_FRAMER);

    if (!pDevCtxt->bActiveFramer)
    {
      /* Note: according to SWI, when using external framer, the framer device
               must be initialized first and then there must be delay before
               configuring other devices */
      SLIMBUS_BUSYWAIT(100);
    }

    HAL_sb_DeviceEnable(pDevCtxt->hSB, HAL_SB_DEVICE_INTERFACE);

    HAL_sb_DeviceEnable(pDevCtxt->hSB, HAL_SB_DEVICE_PORTED_GENERIC);
  }
  
  pDevCtxt->bMsgWorkLoopSignalled = FALSE;
  
  SlimBus_DeviceEnableInterrupts(pDevCtxt);

  HAL_sb_DeviceEnable(pDevCtxt->hSB, pDevCtxt->eMsgDev);

  /* Do master or satellite specific initialization */
  if ( SB_MASTER_DRIVER == pDevCtxt->eDrvType &&
       pDevCtxt->pSched != NULL )
  {
    pDevCtxt->pSched->ePause = SB_PAUSE_NOT_PAUSED;
  }
  else
  {
    uint32 uIdx;

    for (uIdx = 0; uIdx < pDevCtxt->numDeviceSlots; uIdx++)
    {
      SlimBusDeviceType *pDevice = pDevCtxt->pDevices[uIdx];
      
      if (NULL != pDevice &&
          SB_ENUM_ACTIVE == pDevice->eState)
      {
        /* Mark available transactions as 0 until we receive master announcement */
        pDevice->uAvailTrans = 0;
      }
    }
  }

  if (!pDevCtxt->bHwEnumLock)
  {
    if (SB_SUCCESS == SlimBus_StopIdle(pDevCtxt))
    {
      pDevCtxt->bHwEnumLock = TRUE;
    }
  }

  uNumHwPorts = HAL_sb_PgdGetNumPorts(pDevCtxt->hSB, 0);
  if (uNumHwPorts < pDevCtxt->uBasePortNum)
  {
    pDevCtxt->numPorts = 0;
  }
  else if (uNumHwPorts < pDevCtxt->uBasePortNum + pDevCtxt->numPorts)
  {
    pDevCtxt->numPorts = uNumHwPorts - pDevCtxt->uBasePortNum;
  }
  
  uNumHwCounters = HAL_sb_PgdGetNumPcVfrBlks(pDevCtxt->hSB, 0);
  if (uNumHwCounters < pDevCtxt->uBasePortNum)
  {
    pDevCtxt->numCounterSlots = 0;
  }
  else if (uNumHwCounters < pDevCtxt->uBasePortNum + pDevCtxt->numCounterSlots)
  {
    pDevCtxt->numCounterSlots = uNumHwCounters - pDevCtxt->uBasePortNum;
  }
  
  if (pDevCtxt->eWrkrnd & SB_WRKRND_O_SETUP_TRUST)
  {
    uint32 uPortBitField = 0;

    for ( i = 0; i < pDevCtxt->numPorts; i++ )
    {
      uPortBitField |= (1 << pDevCtxt->Ports[i].portNum);
    }

    /* Initialize the port ownership for the local EE */
    HAL_sb_PgdPortSetOwnerEe(pDevCtxt->hSB, 0, pDevCtxt->pBSP->uMyEE, uPortBitField);
  }
  
  if ( pDevCtxt->bIsHwOwner )
  {
    HAL_sb_CompEnable(pDevCtxt->hSB);
  }

  pDevCtxt->bHwInit = TRUE;

  SB_LOG_INFO_0("Hardware initialization completed");

  /* Start the timer for hardware enumeration timeout */
  if (pDevCtxt->bIsHwOwner)
  {
    if (pDevCtxt->bActiveFramer)
    {
      SlimBus_TimerStart(pDevCtxt,
                         pDevCtxt->hEnumTimerHandle,
                         pDevCtxt->hEnumTimerEvent,
                         SB_ENUMERATION_TIMEOUT);
    }
    else 
    {
      pDevCtxt->uSFTimerCnt = 0;
      pDevCtxt->uFrameSyncTimerCnt = 0;
      SlimBus_TimerStart(pDevCtxt,
                         pDevCtxt->hSFTimerHandle,
                         pDevCtxt->hSFTimerEvent,
                         SB_SUPERFRAME_BASE_TIMEOUT);
    }
  }
  
  return result;
}

/**
 * @brief Open the device for the client
 *
 * This function opens the device for the client and initializes
 * the client data structures 
 * 
 * @param[in] h  Client device handle
 * @param[in] mode  Mode used to open the device
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
SBResult
SlimBus_DeviceOpen(uint32 ClientId, SlimBusHandle h, uint32 mode)
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SBResult result = SB_SUCCESS;
  SBBOOL bWaitForEnum = FALSE;
  
  UNREFERENCED_PARAMETER(mode);

  SlimBus_LockClient(pClientCtxt);
  SlimBus_LockDevice(pDevCtxt);

  if (!pClientCtxt->bOpen && !pDevCtxt->bPendingSSR)
  {
    SB_LOG_INFO_2("Slimbus device is being opened (client: 0x%lx) (#open %ld)",
                  (uint32)pClientCtxt, pDevCtxt->uOpenCnt+1);

    if (NULL == pClientCtxt->BasicClient.pClientCtxt)
    {
      result = SlimBus_ClientInit(pClientCtxt);
    }
    
    if ( 0 == pDevCtxt->uOpenCnt + 1 )
    {
      /* Check for rollover and return an error if that would happen */
      result = SB_ERROR;
    }
    else if (SB_SUCCESS == result)
    {
      pClientCtxt->bOpen = TRUE;
      pDevCtxt->bEverOpened = TRUE;

      if (0 == pDevCtxt->uOpenCnt++)
      {
        result = pDevCtxt->leaveLowPowerFcn(pDevCtxt);

        if (SB_SUCCESS == result)
        {
          result = SlimBus_DeviceInitHardware(pDevCtxt);
        }
      }

      if (SB_SUCCESS == result)
      {
        /* Call stop idle for each device open */        
        SlimBus_StopIdle(pDevCtxt);

        if ( HAL_sb_FrmIsInIdleMode(pDevCtxt->hSB) ||
             !SlimBus_HardwareIsEnumerated(pDevCtxt) )
        {
          PushTailClient(&pDevCtxt->EnumWaitList, &pClientCtxt->BasicClient);
          SlimBus_EventReset(pClientCtxt->hClientEvent);
          bWaitForEnum = TRUE;
        }
      }
      else
      {
        pClientCtxt->bOpen = FALSE;
        pDevCtxt->uOpenCnt--;
      }
    }
  }

  if (!pClientCtxt->bOpen)
  {
    SB_LOG_ERR_2("Error during device open (client: 0x%lx) (#open %ld)",
                 (uint32)pClientCtxt, pDevCtxt->uOpenCnt);
  }
  
  SlimBus_UnlockDevice(pDevCtxt);

  if ( bWaitForEnum )
  {
    if (SB_SUCCESS != (result = SlimBus_EventTimedWait(pClientCtxt,
                                                       pClientCtxt->hClientEvent,
                                                       SB_DEFAULT_CLIENT_TIMEOUT)))
    {
      SlimBus_LockDevice(pDevCtxt);
      RemoveClientNode(&pClientCtxt->BasicClient);
      SB_LOG_WARN_0("Timeout waiting for hardware enumeration during device open");
      pDevCtxt->bDeviceOpenTimeout = TRUE;
      pClientCtxt->bOpen = FALSE;
      pDevCtxt->uOpenCnt--;
      /* Call resume idle for each device open */        
      SlimBus_ResumeIdle(pDevCtxt);
      SlimBus_UnlockDevice(pDevCtxt);
    }
  }

  SlimBus_UnlockClient(pClientCtxt);
  
  return result;
}

/**
 * @brief Close the device for the client
 *
 * This function closes the device for the client 
 * 
 * @param[in] h  Client device handle
 * 
 * @return SB_SUCCESS 
 */
SBResult
SlimBus_DeviceClose(uint32 ClientId, SlimBusHandle h)
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);

  SlimBus_LockDevice(pDevCtxt);

  if ( pClientCtxt->bOpen ||
       pClientCtxt->bOpenb4SSR )
  {
    uint32 uIdx;
    
    /* Make sure all the ports associated with this client are deallocated */
    for (uIdx = 0; uIdx < pDevCtxt->numPorts; uIdx++)
    {
      SlimBusMasterPortType *pPort = &pDevCtxt->Ports[uIdx];

      if ( pClientCtxt == pPort->pClientCtxt &&
           SB_HANDLE_ALLOCATED == pPort->eState )
      {
        SB_LOG_INFO_2("Deallocating port during device close (client: 0x%lx) (hport: %lx)",
                      (uint32)pClientCtxt, pPort->handle);
        SlimBus_DeallocPort(pClientCtxt, pPort);
      }
    }

    pClientCtxt->bOpen = FALSE;
    pClientCtxt->bOpenb4SSR = FALSE;
    pDevCtxt->uOpenCnt--;

    SB_LOG_INFO_2("Slimbus device closed (client: 0x%lx) (#open %ld)",
                  (uint32)pClientCtxt, pDevCtxt->uOpenCnt);

    (void) pDevCtxt->checkEnterLowPowerFcn(pDevCtxt);

    /* Call resume idle for each device close */        
    SlimBus_ResumeIdle(pDevCtxt);
  }

  SlimBus_UnlockDevice(pDevCtxt);

  return SB_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are extended in SlimBus Interface.
------------------------------------------------------------------------------*/


/**
 * @brief Internal function used to get the logical address 
 *        corresponding to a SLIMBus native device's enumeration
 *        address
 *
 * This function retrieves the logical address corresonding to a 
 * SLIMbus native device's enumeration address.  This function 
 * is used internally by the Slimbus driver and doesn't do mutex 
 * locking. 
 * 
 * @param[in] pClientCtxt  Pointer to the client context
 * @param[in] pEA  Pointer to the Slimbus native device 
 *       enumeration address
 * @param[in] uEASize  Size of the enumeration address
 * @param[out] pLA  Pointer to location to store the device's 
 *       logical address
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
static SBResult 
SlimBus_GetDeviceLogicalAddrInternal( SlimBusClientCtxt *pClientCtxt, const uint8 *pEA, uint32 uEASize, SlimBusLogicalAddrType * pLA) 
{
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SBResult result = SB_ERROR;
  uint32 i;

  /* Search the local device enumeration data structure for the enumeration address */
  for (i = 0; i < pDevCtxt->numDeviceSlots; i++)
  {
    SlimBusDeviceType *pDevice = pDevCtxt->pDevices[i];
    if ( (NULL != pDevice &&
          pDevice->eState == SB_ENUM_ACTIVE) &&
         (memcmp(pEA, pDevice->aEA, SLIMBUS_ENUM_ADDR_BUF_SIZE) == 0) )
    {
      *pLA = pDevice->LA;
      result = SB_SUCCESS;
      break;
    }
  }
  /* If the enumeration address was not found in the local data structure,
     and this driver instance is a satellite driver, query the master driver
     for the logical address */
  if ( i >= pDevCtxt->numDeviceSlots &&
       SB_SATELLITE_DRIVER == pDevCtxt->eDrvType )
  {
    result = SlimBus_QueryRemoteLogicalAddr(pClientCtxt, pEA, uEASize, pLA);
  }
  
  return result;
}

/**
 * @brief Get the logical address of the master component's 
 *        ported generic device
 *
 * This function is used internally by the driver to get the 
 * logical address of the master component's ported generic 
 * device 
 * 
 * @param[in] pClientCtxt  Pointer to the client context
 * @param[out] pLA  Location to store the logical address of the 
 *       PGD
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
static SBResult
SlimBus_GetMasterPgdLA( SlimBusClientCtxt *pClientCtxt, SlimBusLogicalAddrType *pLA )
{
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SBResult result = SB_SUCCESS;
  
  /* We cache the PGD LA, if we don't have a cached value we need to do the lookup */
  /* Use MANAGER LA to denote that we haven't yet looked up the PGD */
  if ( SB_MSG_LA_MANAGER == pDevCtxt->pgdLA )
  {
    uint32 uRev = 0;
    uint8 uaPgdEA[SLIMBUS_ENUM_ADDR_BUF_SIZE];

    SlimBus_Memcpy(uaPgdEA, pDevCtxt->pBSP->uaMasterEA, SLIMBUS_ENUM_ADDR_BUF_SIZE);
    /* PGD has Device Index = 5 */
    uaPgdEA[1] = 5;
    
    /* Do the EA->LA lookup */
    result = SlimBus_GetDeviceLogicalAddrInternal(pClientCtxt, uaPgdEA,
                                                  SLIMBUS_ENUM_ADDR_BUF_SIZE,
                                                  &pDevCtxt->pgdLA);

    if (SB_SUCCESS == result)
    {
      SB_LOG_INFO_2("detected SB core (version: 0x%lx) (workarounds: 0x%x)",
                    uRev, pDevCtxt->eWrkrnd);
    }
  }
  
  *pLA = pDevCtxt->pgdLA;
  
  return result;
}

/**
 * @brief Retrieve the logical address corresponding to a 
 *        Slimbus native device enumeration address
 *
 * This function retrieves the logical address corresponding to 
 * a Slimbus native device enumeration address.  This function 
 * is used by external clients and performs mutex locking. 
 * 
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] pEA  Pointer to buffer containing device 
        enumeration address to query
  @param[in] uEASize Size of device enumeration address buffer 
  @param[out] pLA  Pointer to location to store of the logical 
        address of the device
   
  @return  SB_SUCCESS on success, an error code on error
 */
SBResult 
SlimBus_GetDeviceLogicalAddr(uint32 ClientId, SlimBusHandle h, const uint8 *pEA, uint32 uEASize, SlimBusLogicalAddrType * pLA) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SBResult result = SB_ERROR;

  SlimBus_LockClient(pClientCtxt);
  SlimBus_LockDevice(pDevCtxt);

  if (  !pClientCtxt->bOpen ||
        SLIMBUS_ENUM_ADDR_BUF_SIZE != uEASize ||
        NULL == pEA ||
        NULL == pLA)
  {
    SlimBus_UnlockDevice(pDevCtxt);
    SlimBus_UnlockClient(pClientCtxt);
    return SB_ERROR;
  }
  
  SB_LOG_INFO_3("Got LA query (client: 0x%lx) (EA: 0x%04x%08lx)", 
                (uint32)pClientCtxt, *(uint16*)&pEA[4], *(uint32*)&pEA[0]);
    
  if ( (pEA[5] == pDevCtxt->pBSP->uaMasterEA[5]) &&
       (pEA[4] == pDevCtxt->pBSP->uaMasterEA[4]) &&
       (pEA[3] == pDevCtxt->pBSP->uaMasterEA[3]) &&
       ((pEA[2]&0xf0) == (pDevCtxt->pBSP->uaMasterEA[2]&0xf0)) &&
       (0x5/*PGD*/ == pDevCtxt->pBSP->uaMasterEA[1]) )
  {
    /* Disallow enumeration address lookup for master component */
    result = SB_ERROR;
  }
  else
  {
    result = SlimBus_GetDeviceLogicalAddrInternal(pClientCtxt, pEA, uEASize, pLA);
  }
  
  if (SB_SUCCESS != result)
  {
    SB_LOG_WARN_3("logical address lookup failed (client: 0x%lx) "
                  "(EA: 0x%04x%08lx)", (uint32)pClientCtxt, *(uint16*)&pEA[4], *(uint32*)&pEA[0]);
  }

  SlimBus_UnlockDevice(pDevCtxt);
  SlimBus_UnlockClient(pClientCtxt);

  return result;
}


/**
  @brief Request a particular SLIMbus root frequency
   
  This function requests a particular SLIMbus root frequency. 
  The frequency request will only be honored if there are no 
  data channels active on the bus.

  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] uFreqKhz  Requested root frequency in kilohertz
   
  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_RequestRootFreq(uint32 ClientId, SlimBusHandle h, uint32  uFreqKhz) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);

  if ( !pClientCtxt->bOpen )
  {
    return SB_ERROR;
  }
  
  // TODO: Implementation

  /* This is stub implementation */
  if ( uFreqKhz == SB_DEFAULT_ROOT_FREQ_HZ )
  {
    return SB_SUCCESS;
  }
  else
  {
    return SB_ERROR;
  }
}


/**
  @brief Get the last SLIMbus driver error.
   
  This function retrieves the last SLIMbus driver errror that 
  occurred.

  @param[in] h  Client handle to the SLIMbus driver 
  @param[out] peError  Pointer to location to store the last 
        SLIMbus driver error
   
  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_GetLastError(uint32 ClientId, SlimBusHandle h, SlimBusErrorType * peError) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);

  SlimBus_LockDevice(pDevCtxt);

  if ( !pClientCtxt->bOpen ||
       NULL == peError )
  {
    SlimBus_UnlockDevice(pDevCtxt);
    return SB_ERROR;
  }

  *peError = pClientCtxt->eLastMsgErr;
  pClientCtxt->eLastMsgErr = SLIMBUS_ERR_NONE;
  
  SlimBus_UnlockDevice(pDevCtxt);

  return SB_SUCCESS;
}


/**
  @brief Get the SLIMbus hardware capabilities.
 
  This function gets the SLIMbus hardware capabilities and 
  optional functionalities. 
  
  @param[in] _h  Client handle to the SLIMbus driver
   
  @param[out] peHwCap  Pointer to location to store the 
        enumeration of hardware capabilities
  
  @return SB_SUCCESS on success, error code on error
 */
SBResult
SlimBus_GetHardwareCapability(uint32 ClientId, SlimBusHandle h, SlimBusHwCapType *peHwCap)
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SlimBusHwCapType eHwCap = 0;
  
  if (NULL != peHwCap)
  {
    if ( HAL_sb_IsPortDmaIntSupported(pDevCtxt->hSB) )
    {
      eHwCap |= SLIMBUS_HW_CAP_COMBINED_PORT_DMA_INT;
    }
    if ( HAL_sb_IsShadowProgressCounterSupported(pDevCtxt->hSB) )
    {
      eHwCap |= SLIMBUS_HW_CAP_TIMESTAMP_SHADOW_COUNTER;
    }
    if ( HAL_sb_IsPortUpperWatermarkSupported(pDevCtxt->hSB) )
    {
      eHwCap |= SLIMBUS_HW_CAP_PORT_UPPER_WATERMARK;
    }
    if ( HAL_sb_IsPortMultiEndpointSupported(pDevCtxt->hSB) )
    {
      eHwCap |= SLIMBUS_HW_CAP_PORT_MULTI_ENDPOINT;
    }
    
    *peHwCap = eHwCap;
  }
  
  return SB_SUCCESS;
}


/**
  @brief Get the maximum number of endpoints that the hardware 
         is able to support
   
  This function retreives the maximum number of endpoints per 
  port that the hardware is able to support.  This function 
  doesn't take into account whether ports supporting the maximum
  number of endpoints have already been allocated or are 
  available for allocation.

  @param[in] _h  Client handle to the SLIMbus driver 
   
  @param[out] puMaxEP  Pointer to location to store the max
        number of endpoints
  
  @return  SB_SUCCESS on success, an error code on error
  */
SBResult
SlimBus_GetMaxEndPointsSupported(uint32 ClientId, SlimBusHandle h, uint32 *puMaxEP)
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  uint32 i;

  if (NULL == puMaxEP)
  {
    return SB_ERROR;
  }
  
  *puMaxEP = 1;
  
  for (i = pDevCtxt->uBasePortNum; i < pDevCtxt->uBasePortNum + pDevCtxt->numPorts; i++)
  {
    if (pDevCtxt->pBSP->uaNumEndPoints[i] > *puMaxEP)
    {
      *puMaxEP = pDevCtxt->pBSP->uaNumEndPoints[i];
    }
  }

  return SB_SUCCESS;
}


/**
 * @brief Clear most client context data from a master port 
 *        structure
 *
 * This function clears most client context data from a master 
 * port structure 
 * 
 * @param pPort  Pointer to the master port data structure
 */
static void SlimBus_ClearPortClientCtxt( SlimBusMasterPortType *pPort )
{
  if ( NULL != pPort->pBamCtxt )
  {
    SlimBus_UnregisterBamPortEvent(pPort);
  }

  pPort->handle = SB_HANDLE_INVALID;
  pPort->hEvent = NULL;
  pPort->eActiveEvents = SLIMBUS_EVENT_O_NONE;
  pPort->eEnabledEvents = SLIMBUS_EVENT_O_NONE;
  pPort->pClientCtxt = NULL;
}

/**
 * @brief Perform steps to deallocate a port handle
 *
 * This function performs cleanup to deallocate a single port 
 * handle 
 * 
 * @param pCtxt  Pointer to the client context
 * @param pPort  Pointer to the master port data structure
 */
_IRQL_requires_(DISPATCH_LEVEL)
static void SlimBus_DeallocPort
(
  SlimBusClientCtxt *pCtxt,
  SlimBusMasterPortType *pPort
)
{
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pCtxt->hDevCtxt);
  uint32 uPortMask = (1<<pPort->portNum);
  SBResult result = SB_SUCCESS;
  
  if (pDevCtxt->bPendingSSR)
  {
    SlimBus_PortAcquireLock(pDevCtxt, pPort);

    SlimBus_ClearPortClientCtxt(pPort);
    SlimBus_RecyclePort(pDevCtxt, pPort);

    SlimBus_PortReleaseLock(pDevCtxt, pPort);
    
    return;
  }
  
  if ( (pDevCtxt->uPortDisconnectMask & uPortMask) ||
       (SB_PORT_NOT_DISCONNECTING != pPort->eDisconnectState) )
  {
    /* If channel is being removed and port disconnected, then continue even
       if removal has not completed */

    SlimBus_PortAcquireLock(pDevCtxt, pPort);

    SlimBus_ClearPortClientCtxt(pPort);
    pPort->eState = SB_HANDLE_DEALLOCATED;

    SlimBus_PortReleaseLock(pDevCtxt, pPort);

    return;
  }

  /* Disconnect the port if still active when we try to deallocate it */
  while ( (HAL_sb_PgdPortGetChannelNumber(pDevCtxt->hSB, 0, pPort->portNum) > 0) ||
          (NULL != pPort->aEndPoints[0].pChannel) ||
          (pPort->eDisconnectState != SB_PORT_NOT_DISCONNECTING) )
  {
    SlimBusLogicalAddrType LA;
    SlimBusDeviceType *pDevice;
    
    if (SB_SUCCESS != SlimBus_GetMasterPgdLA(pCtxt, &LA))
    {
      break;
    }

    pDevice = pDevCtxt->pDevices[LA&SB_LA_IDX_MASK];

    SB_LOG_INFO_3("Waiting for port disconnection during deallocation "
                  "(client: 0x%lx) (resource: 0x%lx) (port: %ld)", (uint32)pCtxt, 
                  pPort->handle, pPort->portNum );

    /* Make sure the port interrupt stays enabled even if overflow/underflow occurs */
    EnablePortInterrupt(pDevCtxt, (1<<pPort->portNum));

    if (pPort->eDisconnectState == SB_PORT_NOT_DISCONNECTING)
    {
      pPort->eDisconnectState = SB_PORT_DISCONNECTING;
      pDevCtxt->uPortFinalDisconnectMask |= (1<<pPort->portNum);
      pDevCtxt->disconnectPortFcn(pCtxt, pDevCtxt, pDevice, pPort->portNum);
    }

    /* If port is disconnected, then we're done */
    if ( (HAL_sb_PgdPortGetChannelNumber(pDevCtxt->hSB, 0, pPort->portNum) == 0) &&
         (NULL == pPort->aEndPoints[0].pChannel) &&
         (pPort->eDisconnectState == SB_PORT_NOT_DISCONNECTING) )
    {
      break;
    }
      
    /* Note: pPort->pChannel gets set to NULL during the ISR handler */

    pPort->bSignalDisconnect = TRUE;
    SlimBus_EventReset(pCtxt->hClientEvent);
    SlimBus_UnlockDevice(pDevCtxt);
  
    result = SlimBus_EventTimedWait(pCtxt,
                                    pCtxt->hClientEvent,
                                    SB_DEFAULT_CLIENT_TIMEOUT);
  
    SlimBus_LockDevice(pDevCtxt);
  
    if (SB_SUCCESS != result)
    {
      pPort->bSignalDisconnect = FALSE;
      break;
    }
  }
  
  SlimBus_PortAcquireLock(pDevCtxt, pPort);

  if ( NULL != pPort->pBamCtxt &&
       pPort->bBamConnected )
  {
    /* This condition should never be true, we should disconnect the BAM
       in the ISR */
    SB_LOG_VERB_3("disconnecting BAM pipe during dealloc (client: 0x%lx) (resource: 0x%lx) "
                  "(port: %ld)", (uint32)pPort->pClientCtxt, pPort->handle, pPort->portNum);
    SlimBus_DisconnectBAM(pPort);
  }

  SlimBus_ClearPortClientCtxt(pPort);
  SlimBus_RecyclePort(pDevCtxt, pPort);

  SlimBus_PortReleaseLock(pDevCtxt, pPort);
}

/**
  @brief Deallocate ports from the SLIMbus master
   
  This function deallocates ports from the SLIMbus master. 
   
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] phPortBuf  Location of the handles to the ports to 
        deallocate
  @param[in] uPortBufSize  Size of the buffer of port handles 
        in bytes, used to derive the number of port handles to
        deallocate

  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_DeallocMasterPorts(uint32 ClientId, SlimBusHandle h, SlimBusResourceHandle * phPortBuf, uint32  uPortBufSize) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);

  uint32 numPorts, portIdx;

  SB_CHECK_IRQL();

  SlimBus_LockClient(pClientCtxt);
  SlimBus_LockDevice(pDevCtxt);

  if ( !pClientCtxt->bOpen ||
       uPortBufSize % sizeof(SlimBusResourceHandle) != 0 ||
       phPortBuf == NULL)
  {
    SlimBus_UnlockDevice(pDevCtxt);
    SlimBus_UnlockClient(pClientCtxt);
    return SB_ERROR;
  }
  numPorts = uPortBufSize / sizeof(SlimBusResourceHandle);

  for (portIdx = 0; portIdx < numPorts; portIdx++)
  {
    SlimBusResourceHandle handle = phPortBuf[portIdx];
    uint32 i = SB_GET_HANDLE_IDX(handle);
    
    if ( i < pDevCtxt->numPorts && 
         i < sizeof(pDevCtxt->Ports)/sizeof(pDevCtxt->Ports[0]) &&
         pDevCtxt->Ports[i].handle == phPortBuf[portIdx] )
    {
      SlimBusPortReqType eReqs = pDevCtxt->Ports[i].eReqs;
      
      SlimBus_DeallocPort(pClientCtxt, &pDevCtxt->Ports[i]);      

      if ( (eReqs & SLIMBUS_PORT_REQ_O_HALF_DUPLEX) && 
           (i+1 < sizeof(pDevCtxt->Ports)/sizeof(pDevCtxt->Ports[0])) )
      {
        SlimBus_PortAcquireLock(pDevCtxt, &pDevCtxt->Ports[i+1]);

        SlimBus_ClearPortClientCtxt(&pDevCtxt->Ports[i+1]);
        SlimBus_RecyclePort(pDevCtxt, &pDevCtxt->Ports[i+1]);

        SlimBus_PortReleaseLock(pDevCtxt, &pDevCtxt->Ports[i+1]);
      }
    }
    
    phPortBuf[portIdx] = SB_HANDLE_INVALID;
  }

  SlimBus_UnlockDevice(pDevCtxt);
  SlimBus_UnlockClient(pClientCtxt);

  return SB_SUCCESS;
}


/**
  @brief Allocate port from the SLIMbus master 
   
  This function allocates one or more ports from the SLIMbus 
  master. 
   
  Ports allocated by this function correspond to logical ports 
  as defined by the SLIMbus spec, e.g. a half-duplex port is 
  considered a single logical port even though the port may 
  consume additional hardware resources within the SLIMbus 
  master. 

  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] eReqs  Bitmap of requirements for the type of
        port(s) to allocate
  @param[in] uMinNumEndPoints  Minimum required number of 
        endpoints supported by the port
  @param[in] uMinFifoBytes  Minimum required size of port FIFO 
        in bytes
  @param[out] phPortBuf  Location to store the handles to the 
        allocated ports
  @param[in] uPortBufSize  Size of the buffer of port handles in
        bytes, used to derive the number of port handles to
        allocate

  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_AllocMasterPortsInternal
(
  uint32 ClientId, 
  SlimBusHandle h,
  SlimBusPortReqType eReqs,
  uint32 uMinNumEndPoints,
  uint32 uMinFifoBytes,
  SlimBusResourceHandle *phPortBuf,
  uint32 uPortBufSize
) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SBResult result = SB_SUCCESS;
  
  uint32 multiChanMap = 0;
  uint32 numPorts, portIdx;
  uint32 i, j;
  
  UNREFERENCED_PARAMETER(uMinFifoBytes);

  SlimBus_LockDevice(pDevCtxt);
  
  if ( !pClientCtxt->bOpen ||
       (uPortBufSize % sizeof(SlimBusResourceHandle) != 0) || 
       (phPortBuf == NULL) ||
       (uMinNumEndPoints == 0) ||
       ( (eReqs & SLIMBUS_PORT_REQ_O_MULTI_CHAN_GROUP) &&
         (eReqs & SLIMBUS_PORT_REQ_O_HALF_DUPLEX) ) )
  {
    SlimBus_UnlockDevice(pDevCtxt);
    return SB_ERROR;
  }
  numPorts = uPortBufSize / sizeof(SlimBusResourceHandle);

  memset(phPortBuf, 0, uPortBufSize);
  
  /* Get the PGD LA before we allocate any ports, to make sure we know
     the chip revision and what workarounds are required */  
  if (pDevCtxt->eWrkrnd & SB_WRKRND_O_NO_PORT_MULTI_CHAN)
  {
    SlimBusLogicalAddrType LA;
    if (SB_SUCCESS != SlimBus_GetMasterPgdLA(pClientCtxt, &LA))
    {
      SlimBus_UnlockDevice(pDevCtxt);
      return SB_ERROR;
    }
  }
  
  /* Search for ports available for allocation and do the allocation */  
  for (portIdx = 0, i = 0; portIdx < numPorts && i < pDevCtxt->numPorts; i++)
  {
    if (pDevCtxt->Ports[i].eState == SB_HANDLE_AVAILABLE)
    {
      SlimBusMasterPortType *pPort = NULL;
      SlimBusResourceHandle handle = 
        SB_SET_HANDLE_TYPE(SB_HANDLE_TYPE_MASTER_PORT) |
        SB_SET_HANDLE_PRN(GetPrn(pDevCtxt)) |
        SB_SET_HANDLE_IDX(i);

      if (HAL_sb_PgdPortGetFifoSize(pDevCtxt->hSB, 0, pDevCtxt->Ports[i].portNum) < uMinFifoBytes)
      {
        result = SLIMBUS_ERR_CONSTRAINTS_NOT_SATISFIED;
        continue;
      }
      if (uMinNumEndPoints > 1 && 
          pDevCtxt->pBSP->uaNumEndPoints[i + pDevCtxt->uBasePortNum] < uMinNumEndPoints)
      {
        result = SLIMBUS_ERR_NOT_ENOUGH_ENDPOINTS;
        continue;
      }
      if (uMinNumEndPoints > pDevCtxt->Ports[i].numEndPoints)
      {
        if (SB_SUCCESS != SlimBus_Realloc(sizeof(SlimBusEndPointType)*uMinNumEndPoints,
                                          sizeof(SlimBusEndPointType)*pDevCtxt->Ports[i].numEndPoints,
                                          (void**)&pDevCtxt->Ports[i].aEndPoints,
                                          SB_MEMTAG_25))
        {
          /* memory alloc failure is serious, just abort if that happens */
          result = SB_ERROR;
          break;
        }
        pDevCtxt->Ports[i].numEndPoints = uMinNumEndPoints;
        for ( j = 0; j < pDevCtxt->Ports[i].numEndPoints; j++ )
        {
          pDevCtxt->Ports[i].aEndPoints[j].endPointNum = j;
          pDevCtxt->Ports[i].aEndPoints[j].pPort = &pDevCtxt->Ports[i];
        }
      }
      
      /* Special logic for half-duplex ports, allocate second physical port */
      if ( eReqs & SLIMBUS_PORT_REQ_O_HALF_DUPLEX )
      {
        /* Validate that half-duplex port starts on even port number, and
           can use two consecutive physical ports */
        if ( ( i % 2 == 1 ) ||
             ( i + 1 >= pDevCtxt->numPorts ) ||
             ( pDevCtxt->Ports[i+1].eState != SB_HANDLE_AVAILABLE ) )
        {
          continue;
        }
        
         /* Call stop idle for each port allocated */        
        SlimBus_StopIdle(pDevCtxt);
        
        pPort = &pDevCtxt->Ports[i+1];

        SlimBus_PortCreateLock(pDevCtxt, pPort);

        SlimBus_PortAcquireLock(pDevCtxt, pPort);

        pPort->handle = handle;
        pPort->eState = SB_HANDLE_ALLOCATED;
        pPort->eReqs = eReqs;
        pPort->pClientCtxt = pClientCtxt;
        pPort->eFlow = SLIMBUS_PORT_SINK_FLOW;
        pPort->uBytesSubmit = 0;
        pPort->uDescSubmit = 0;

        if ( NULL != pPort->pBamCtxt &&
             pPort->bBamConnected )
        {
          /* This condition should never be true, we should disconnect the BAM
             in the ISR */
          SB_LOG_VERB_3("disconnecting BAM pipe during alloc (client: 0x%lx) (resource: 0x%lx) "
                        "(port: %ld)", (uint32)pClientCtxt, pPort->handle, pPort->portNum);
          SlimBus_DisconnectBAM(pPort);
        }
        
        SlimBus_PortReleaseLock(pDevCtxt, pPort);
      }
      
      /* Call stop idle for each port allocated */        
      SlimBus_StopIdle(pDevCtxt);
      
      pPort = &pDevCtxt->Ports[i];
      
      SlimBus_PortCreateLock(pDevCtxt, pPort);

      SlimBus_PortAcquireLock(pDevCtxt, pPort);
      
      pPort->handle = handle;
      pPort->eState = SB_HANDLE_ALLOCATED;
      pPort->eReqs = eReqs;
      pPort->pClientCtxt = pClientCtxt;
      pPort->eFlow = SLIMBUS_PORT_SOURCE_FLOW;
      pPort->uBytesSubmit = 0;
      pPort->uDescSubmit = 0;
      
      if ( NULL != pPort->pBamCtxt &&
           pPort->bBamConnected )
      {
        /* This condition should never be true, we should disconnect the BAM
           in the ISR */
        SB_LOG_VERB_3("disconnecting BAM pipe during alloc (client: 0x%lx) (resource: 0x%lx) "
                      "(port: %ld)", (uint32)pClientCtxt, pPort->handle, pPort->portNum);
        SlimBus_DisconnectBAM(pPort);
      }

      phPortBuf[portIdx] = handle;

      /* Set bit in multi-channel bitmap for this port */
      if ( eReqs & SLIMBUS_PORT_REQ_O_MULTI_CHAN_GROUP )
      {
        multiChanMap |= (1 << pPort->portNum);
      }
      else
      {
        if ( !(pDevCtxt->eWrkrnd & SB_WRKRND_O_NO_PORT_MULTI_CHAN) )
        {
          /* Note: multi-channel bits are set even for single channel */
          HAL_sb_PgdPortSetMultiChanl(pDevCtxt->hSB, 0, (1 << pPort->portNum));
        }
      }
      
      SlimBus_PortReleaseLock(pDevCtxt, pPort);

      portIdx++;
    }
  }
  /* If we encountered an error before allocating all the ports, de-allocate
     the ports that did get allocated */
  if (portIdx < numPorts)
  {
    SB_LOG_ERR_4("Not enough qualifying master ports available during alloc "
                 "request (err: 0x%x) (numPorts: %d) (minEP: %d) (minFIFO: %d)",
                 result, numPorts, uMinNumEndPoints, uMinFifoBytes); 

    if (SB_SUCCESS == result)
    {
      result = SLIMBUS_ERR_OUT_OF_RESOURCES;
    }

    SlimBus_UnlockDevice(pDevCtxt);
    SlimBus_DeallocMasterPorts(ClientId, h, phPortBuf, uPortBufSize);
    return result;
  }
  
  /* Set the multi-channel bitmap for a multi-channel group */
  if ( eReqs & SLIMBUS_PORT_REQ_O_MULTI_CHAN_GROUP )
  {
    if ( !(pDevCtxt->eWrkrnd & SB_WRKRND_O_NO_PORT_MULTI_CHAN) )
    {
      /* set multi-channel bitmap */
      HAL_sb_PgdPortSetMultiChanl(pDevCtxt->hSB, 0, multiChanMap);
    }
  }
  
  SlimBus_UnlockDevice(pDevCtxt);

  return SB_SUCCESS;
}

/**
  @brief Allocate port from the SLIMbus master 
   
  This function allocates one or more ports from the SLIMbus 
  master. 
   
  Ports allocated by this function correspond to logical ports 
  as defined by the SLIMbus spec, e.g. a half-duplex port is 
  considered a single logical port even though the port may 
  consume additional hardware resources within the SLIMbus 
  master. 

  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] eReqs  Bitmap of requirements for the type of
        port(s) to allocate
  @param[in] uMinFifoBytes  Minimum required size of port FIFO 
        in bytes
  @param[out] phPortBuf  Location to store the handles to the 
        allocated ports
  @param[in] uPortBufSize  Size of the buffer of port handles in
        bytes, used to derive the number of port handles to
        allocate

  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_AllocMasterPorts(uint32 ClientId, SlimBusHandle h, SlimBusPortReqType  eReqs, uint32  uMinFifoBytes, SlimBusResourceHandle * phPortBuf, uint32  uPortBufSize) 
{
  return SlimBus_AllocMasterPortsInternal(ClientId, h, eReqs, 1, uMinFifoBytes, phPortBuf, uPortBufSize);
}


/**
  @brief Allocate port with multiple endpoints from the SLIMbus 
         master
   
  This function allocates one port that may contain multiple 
  endpoints from the SLIMbus master. 
   
  The port allocated by this function corresponds to a logical 
  port as defined by the SLIMbus spec, and may contains one or 
  more endpoints. 

  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] uMinNumEndPoints  Minimum required number of 
        endpoints supported by the port
  @param[in] uMinFifoBytes  Minimum required size of port FIFO 
        in bytes
  @param[out] phPort  Location to store the handle to the 
        allocated port

  @return  SB_SUCCESS on success, an error code on error
  */
SBResult SlimBus_AllocMasterPortWithEndPoints(uint32 ClientId, SlimBusHandle h, uint32 uMinNumEndPoints, uint32  uMinFifoBytes, SlimBusResourceHandle *phPort)
{
  return SlimBus_AllocMasterPortsInternal(ClientId, h, 0, uMinNumEndPoints, uMinFifoBytes, phPort, sizeof(*phPort));
}

/**
  @brief Get handle to a port on a slave device
   
  This function retrieves a handle to a port on a slave device
  that can be used for setting up a data channel.
   
  @param[in] h  Client handle to the SLIMbus driver
  @param[in] LA  Logical address of slave device containing the port
  @param[in] uPortNum  Port number on the slave device
  @param[out] phPort  Location to store the port handle

  @return  SB_SUCCESS on success, an error code on error
   
  */
SBResult 
SlimBus_GetSlavePortHandle(uint32 ClientId, SlimBusHandle h, SlimBusLogicalAddrType  LA, uint32  uPortNum, SlimBusResourceHandle * phPort) 
{
  UNREFERENCED_PARAMETER(h);

  if ( NULL == phPort )
  {
    return SB_ERROR;
  }

  *phPort = 
    SB_SET_HANDLE_TYPE(SB_HANDLE_TYPE_SLAVE_PORT) |
    SB_SET_HANDLE_LA(LA) |
    SB_SET_HANDLE_IDX(uPortNum);

  return SB_SUCCESS;
}


/**
  @brief Configure the parameters of a SLIMbus master port
   
  This function configures the parameters of a SLIMbus master 
  port.  SLIMbus slave ports cannot be configured by this 
  function. 
   
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hPort  Client handle to the master port 
  @param[in] pConfig  Pointer to the configuration to set for 
        the master port

  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_ConfigMasterPort(uint32 ClientId, SlimBusHandle h, SlimBusResourceHandle  hPort, const SlimBusPortConfigType * pConfig) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  uint32 idx, portNum, uFifoSize, uUpperWaterMark;
  SlimBusMasterPortType *pPort = NULL, *pOtherPort = NULL;
  SBResult result = SB_SUCCESS;

  SB_CHECK_IRQL();

  SlimBus_LockClient(pClientCtxt);
  SlimBus_LockDevice(pDevCtxt);

  if (!pClientCtxt->bOpen ||
      NULL == pConfig)
  {
    SlimBus_UnlockDevice(pDevCtxt);
    SlimBus_UnlockClient(pClientCtxt);
    return SB_ERROR;
  }

  do
  {
    HAL_sb_PgdPortDataAlignType eAlign = HAL_SB_PGD_PORT_DATA_ALIGN_LSB;
    
    idx = SB_GET_HANDLE_IDX(hPort);
    if (idx >= pDevCtxt->numPorts || 
        idx >= sizeof(pDevCtxt->Ports)/sizeof(pDevCtxt->Ports[0]) ||
        hPort != pDevCtxt->Ports[idx].handle)
    {
      result = SB_ERROR;
      break;
    }
    pPort = &pDevCtxt->Ports[idx];
    if ( (pPort->eReqs & SLIMBUS_PORT_REQ_O_HALF_DUPLEX) && 
         (idx+1 < sizeof(pDevCtxt->Ports)/sizeof(pDevCtxt->Ports[0])) )
    {
      pOtherPort = &pDevCtxt->Ports[idx+1];
    }

    SB_LOG_VERB_7("Configure master port (client: 0x%lx) (hport: 0x%lx) "
                  "(watermark: %ld) (upperwatermark: %ld) (fifosize: %ld) "
                  "(packed: %ld) (alignmsb: %ld)", (uint32)pClientCtxt, hPort, pConfig->uWaterMark,
                  ((pConfig->bUpperWaterMarkValid)?pConfig->uUpperWaterMark:0),
                  HAL_sb_PgdPortGetFifoSize(pDevCtxt->hSB, 0, pPort->portNum),
                  (pConfig->eOptions&SLIMBUS_PORT_O_PACKED),
                  (pConfig->eOptions&SLIMBUS_PORT_O_ALIGN_MSB));

    /* Wait for the port to disconnect if it is in the middle of disconnecting */
    if (pPort->eDisconnectState != SB_PORT_NOT_DISCONNECTING)
    {
      SB_LOG_INFO_3("Waiting for port disconnection during config port "
                    "(client: 0x%lx) (resource: 0x%lx) (port: %ld)", (uint32)pClientCtxt, 
                    pPort->handle, pPort->portNum );

      pPort->bSignalDisconnect = TRUE;
      SlimBus_EventReset(pClientCtxt->hClientEvent);
      SlimBus_UnlockDevice(pDevCtxt);

      result = SlimBus_EventTimedWait(pClientCtxt,
                                      pClientCtxt->hClientEvent,
                                      SB_DEFAULT_CLIENT_TIMEOUT);

      SlimBus_LockDevice(pDevCtxt);

      if (SB_SUCCESS != result)
      {
        pPort->bSignalDisconnect = FALSE;
        break;
      }
    }

    /* Configure both the primary and secondary ports for half-duplex ports */
    for ( ; NULL != pPort; pPort = pOtherPort, pOtherPort = NULL )
    {
      portNum = pPort->portNum;

      /* Cache the port configuration options */
      pPort->eOptions = pConfig->eOptions;
      
      if ( pConfig->uWaterMark > HAL_sb_PgdPortGetFifoSize(pDevCtxt->hSB, 0, portNum) )
      {
        result = SB_ERROR;
        break;
      }
      if ( TRUE == pConfig->bUpperWaterMarkValid &&
           pConfig->uUpperWaterMark > HAL_sb_PgdPortGetFifoSize(pDevCtxt->hSB, 0, portNum) )
      {
        result = SB_ERROR;
        break;
      }
      
      uFifoSize = HAL_sb_PgdPortGetFifoSize(pDevCtxt->hSB, 0, portNum);
      uUpperWaterMark = uFifoSize - pConfig->uUpperWaterMark;
      
      HAL_sb_PgdPortSetWatermark(pDevCtxt->hSB, 0, portNum, pConfig->uWaterMark/sizeof(uint32));
      if ( HAL_sb_IsPortUpperWatermarkSupported(pDevCtxt->hSB) )
      {
        if ( TRUE == pConfig->bUpperWaterMarkValid )
        {
          HAL_sb_PgdPortSetUpperWatermark(pDevCtxt->hSB, 0, portNum, uUpperWaterMark/sizeof(uint32));
          HAL_sb_PgdPortUpperWatermarkEnable(pDevCtxt->hSB, 0, portNum);
        }
        else
        {
          HAL_sb_PgdPortUpperWatermarkDisable(pDevCtxt->hSB, 0, portNum);
        }
      }
      
      if ( pConfig->eOptions & SLIMBUS_PORT_O_ALIGN_MSB )
      {
        eAlign = HAL_SB_PGD_PORT_DATA_ALIGN_MSB;
      }
      HAL_sb_PgdPortSetAlignment(pDevCtxt->hSB, 0, portNum, eAlign);
      
      if ( pConfig->eOptions & SLIMBUS_PORT_O_PACKED )
      {
        HAL_sb_PgdPortPackEnable(pDevCtxt->hSB, 0, portNum);
      }
      else
      {
        HAL_sb_PgdPortPackDisable(pDevCtxt->hSB, 0, portNum);
      }
      
      if ( HAL_sb_IsPortDmaIntSupported(pDevCtxt->hSB) )
      {
        HAL_sb_PgdPortDmaIrqEnable(pDevCtxt->hSB, 0, portNum);
      }
      
      HAL_sb_PgdPortSetBlkSize(pDevCtxt->hSB, 0, portNum, pConfig->uBlockSize);
      HAL_sb_PgdPortSetTransSize(pDevCtxt->hSB, 0, portNum, pConfig->uTransSize);
      
      EnablePortInterrupt(pDevCtxt, (1<<pPort->portNum));

      HAL_sb_PgdPortEnable(pDevCtxt->hSB, 0, portNum);
    }
  } while (FALSE);
  
  SlimBus_UnlockDevice(pDevCtxt);
  SlimBus_UnlockClient(pClientCtxt);

  return result;
}


/**
  @brief Sets the messaging bandwidth required by the client
  
  Sets the total messaging bandwidth required by the client for 
  control and status operations.  Set the required bandwidth to 
  zero to relinquish reserved bandwidth. The bandwidth request 
  will take effect at the next reconfiguration boundary. 
  
  @param[in] h  Client handle to the SLIMbus driver
  @param[in] uBandwidth_bps  The bandwidth to reserve in bits per second

  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_NextReserveMsgBandwidth(uint32 ClientId, SlimBusHandle h, uint32  uBandwidth_bps) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SlimBusMsgFifoType *pMsgFifo;
  uint32 *pBuf;
  SBResult result = SB_SUCCESS;
  uint32 uSlots;

  SlimBus_LockClient(pClientCtxt);
  SlimBus_LockDevice(pDevCtxt);

  if ( !pClientCtxt->bOpen )
  {
    SlimBus_UnlockDevice(pDevCtxt);
    SlimBus_UnlockClient(pClientCtxt);
    return SB_ERROR;
  }

  if ( SB_MASTER_DRIVER == pDevCtxt->eDrvType )
  {
    pMsgFifo = &pClientCtxt->BasicClient.ConfigMsgFifo;    
  }
  else
  {
    pMsgFifo = &pDevCtxt->TxMsgFifo;
  }

  do
  {
    /* Can't do any reconfiguration functions if we already have a reconfiguration
       pending */
    if (pClientCtxt->bReConfigPending)
    {
      result = SB_ERROR;
      break;
    }
    
    if ( uBandwidth_bps/8 >= pDevCtxt->uRootFreqDiv8 )
    {
      uSlots = SB_SLOTS_PER_SUPERFRAME;
    }
    else
    {
      uSlots = 
        ( uBandwidth_bps*(SB_CELLS_PER_SUPERFRAME_DIV8/SB_CELLS_PER_SLOT/2) + 
          (pDevCtxt->uRootFreqDiv8/2-1)/*round up*/ ) /
        (pDevCtxt->uRootFreqDiv8/2);
    }
    
    /* Check if the new request is different than the previous request */
    if ( uSlots != pClientCtxt->uNewReservedMsgSlots )
    {
      uint32 TID;
      pBuf = pMsgFifo->getBufFcn(pClientCtxt, SB_GET_MGR_DEV_PTR(pDevCtxt),
                                 pMsgFifo, SB_MSG_MC_USR_REQ_MSG_BANDWIDTH_LEN);
      if ( NULL == pBuf )
      {
        result = SB_ERROR;
        break;
      }
     
      SB_LOG_VERB_2("Got message bandwitdth request (client: 0x%lx) (bps: %ld)",
                    (uint32)pClientCtxt, uBandwidth_bps);
           
      TID = SlimBus_GetNextMasterTID(pDevCtxt);
      pBuf[0] = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_USR_REQ_MSG_BANDWIDTH_LEN,
                                      SB_MSG_MT_DEST_REFERRED_USER,
                                      SB_MSG_MC_USR_REQ_MSG_BANDWIDTH,
                                      SB_MSG_DT_UNICAST,
                                      SB_MSG_LA_MANAGER) |
        SB_MSG_SET(pClientCtxt->BasicClient.uClientNum,
                   USR_REQ_MSG_BANDWIDTH_0,CLIENT_NUM) |
        SB_MSG_SET(uSlots,USR_REQ_MSG_BANDWIDTH_0,MSG_SLOTS_LSB);
      pBuf[1] =
        SB_MSG_SET(uSlots>>(32-SB_MSG_MC_USR_REQ_MSG_BANDWIDTH_0__MSG_SLOTS_LSB_SHFT),
                   USR_REQ_MSG_BANDWIDTH_1,MSG_SLOTS_MSB) | 
        SB_MSG_SET(TID,USR_REQ_MSG_BANDWIDTH_1,TID);


      result = SlimBus_SendMasterAckedMsg(pClientCtxt, pMsgFifo, pBuf, (uint8)TID);
      if (SB_SUCCESS == result ||
          SLIMBUS_ERR_MSG_RESP_TIMEOUT == result)
      {
        /* When there is a timeout, assume master will eventually process
           the request, and therefore update the internal state accordingly */
        pClientCtxt->uPendingReconfigReqs++;
        pClientCtxt->uNewReservedMsgSlots = uSlots;
      }
    }
  } while (FALSE);

  SlimBus_UnlockDevice(pDevCtxt);
  SlimBus_UnlockClient(pClientCtxt);

  return result;
}


/**
  @brief Set the size of the batched message buffer in bytes 
  
  Internal function to set the size of the batched message 
  buffer in bytes
 
  @param[in] pClientCtxt  Pointer to the client context
  @param[in] uMaxBufferSize  The size of the batched message 
        buffer in bytes
  
  @return SB_SUCCESS on success, an error code on error 
  */
static SBResult
SlimBus_SetBatchBufferSizeInternal(SlimBusClientCtxt *pClientCtxt, uint32 uMaxBufferSize)
{
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SBResult result = SB_SUCCESS;

  if (!pDevCtxt->bUseTxMsgQueue || !pClientCtxt->bOpen)
  {
    return SB_ERROR;
  }

  SlimBus_LockClient(pClientCtxt);

  if (uMaxBufferSize == pClientCtxt->BatchMsgFifo.uLen*4)
  {
    SlimBus_UnlockClient(pClientCtxt);
    return SB_SUCCESS;
  }

  if (NULL != pClientCtxt->hBatchMem)
  {
    SlimBus_DestroyObject(pClientCtxt->hBatchMem);
    pClientCtxt->hBatchMem = NULL;
    pClientCtxt->BatchMsgFifo.pBuf = NULL;
    pClientCtxt->BatchMsgFifo.pPhysBuf = NULL;
    pClientCtxt->BatchMsgFifo.uLen = 0;
  }

  if (uMaxBufferSize < SB_DEFAULT_BATCH_BUFFER_SIZE)
  {
    uMaxBufferSize = SB_DEFAULT_BATCH_BUFFER_SIZE;
  }

  result = SlimBus_PhysMemAlloc(uMaxBufferSize, &pClientCtxt->hBatchMem);
  if (SB_SUCCESS != result)
  {
    pClientCtxt->hBatchMem = NULL;
  }
  else
  {
    size_t uVirtAddr = 0, uPhysAddr = 0;

    result = SlimBus_PhysMemInfo(pClientCtxt->hBatchMem, &uVirtAddr, &uPhysAddr);
    if (SB_SUCCESS != result)
    {
      SlimBus_DestroyObject(pClientCtxt->hBatchMem);
      pClientCtxt->hBatchMem = NULL;
    }
    else
    {
      pClientCtxt->BatchMsgFifo.pBuf = (uint32*) uVirtAddr;
      pClientCtxt->BatchMsgFifo.pPhysBuf = (uint32*) uPhysAddr;
      pClientCtxt->BatchMsgFifo.uLen = uMaxBufferSize/4;
      pClientCtxt->BatchMsgFifo.uAckIdx = SB_ACK_IDX_UNUSED;
      pClientCtxt->BatchMsgFifo.uTailIdx = 0;
      pClientCtxt->BatchMsgFifo.uHeadIdx = 0;
      pClientCtxt->BatchMsgFifo.getBufFcn = SlimBus_GetMsgBuffer;
      pClientCtxt->BatchMsgFifo.sendBufFcn = SlimBus_DefaultSendBuffer;
    }
  }

  SlimBus_UnlockClient(pClientCtxt);

  return result;
}

/**
  @brief Set the size of the batched message buffer in bytes 
  
  Function to set the size of the batched message buffer in bytes
 
  @param[in] h  Client handle to the SLIMbus driver
  @param[in] uMaxBufferSize  The size of the batched message 
        buffer in bytes
  
  @return SB_SUCCESS on success, an error code on error 
  */
SBResult SlimBus_SetBatchBufferSize(uint32 ClientId, SlimBusHandle h, uint32 uMaxBufferSize)
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  
  SB_CHECK_IRQL();

  return SlimBus_SetBatchBufferSizeInternal(pClientCtxt, uMaxBufferSize);
}

/**
 * @brief Get the size of an IE/VE slice in bytes from the 
 *        message slice code
 *
 * This function decodes the size in bytes of an IE/VE slice 
 * from the message slice code 
 * 
 * @param[in] uCode  The message slice code
 * 
 * @return Size of the slice in bytes 
 */
uint8 GetSliceSizeFromCode(uint32 uCode)
{
  static uint8 ucCodeToSize[8] = { 1, 2, 3, 4, 6, 8, 12, 16 };
  if (uCode >= 8)
  {
    return 0;
  }
  else
  {
    return ucCodeToSize[uCode];
  }
}


/**
 * @brief Get the slice code of an IE/VE slice from the slice 
 *        size in bytes
 *
 * This function encodes the slice code of an IE/VE slice from 
 * the message slice size in bytse 
 * 
 * @param[in] uReqCode  The slice size in bytes
 * 
 * @return Encoded slice code 
 */
uint8 GetSliceCodeFromSize(uint32 uReqSize)
{
  static uint8 ucSizeToCode[16] = 
    {
      0, 1, 2, 3, 3, 4, 4, 5, 
      5, 5, 5, 6, 6, 6, 6, 7
    };
  
  if (uReqSize == 0)
  {
    uReqSize = 1;
  }
  if (uReqSize > 16)
  {
    uReqSize = 16;
  }
  return ucSizeToCode[uReqSize-1];
}


/**
 * @brief Send a client-initiated message and wait for 
 *        transmission completion or the read message response.
 *
 * This function sends a client-initiated message and waits for 
 * indication of transmission completion or for the read message
 * response. 
 * 
 * @param[in] pClientCtxt  Pointer to the client context
 * @param[in] pBuf  Pointer to the message buffer to send
 * @param[in] pDevice  Pointer to the destination SLIMbus device
 *       structure
 * @param[out] pucReadData  Pointer to the buffer to store data 
 *       read from an IE/VE slice, or NULL if there is not data
 *       to read
 * @param[in] uReadDataLen  Size of the read data buffer
 * @param[out] puActualReadDataLen  Pointer to the location to 
 *       store the actual number of bytes read from an IE/VE
 *       slice
 * @param[in] hEvent  Client event handle, for asynchronous 
 *       operation
 * 
 * @return SB_SUCCESS on success, error code on error
 */
static SBResult
SlimBus_SendClientMessage(SlimBusClientCtxt *pClientCtxt, uint32 *pBuf, SlimBusDeviceType *pDevice, uint8 *pucReadData, uint32 uReadDataLen, uint32 *puActualReadDataLen, SlimBusEventHandle hEvent)
{
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  uint32 TID = SB_MSG_GET(pBuf[0],REPLY_VALUE_0,TID);
  SlimBusEventNodeType *pEventNode = NULL;
  SBResult result = SB_SUCCESS;

  /* Wait for completion of the transaction (if operation is not non-blocking or batched) */
  if (SLIMBUS_EVENT_HANDLE_BATCHED_XFER == hEvent)
  {
    pClientCtxt->BatchMsgFifo.sendBufFcn(pDevCtxt, &pClientCtxt->BatchMsgFifo);
  }
  else if (SLIMBUS_EVENT_HANDLE_NON_BLOCKING == hEvent)
  {
    pDevCtxt->TxMsgFifo.sendBufFcn(pDevCtxt, &pDevCtxt->TxMsgFifo);
  }
  else
  {  
    if (NULL == hEvent)
    {
      /* Use internal client event object if client does not provide its
         own asynchronous event */
      hEvent = pClientCtxt->hClientEvent;

      /* Create event handle again to grab current thread ID */
      SlimBus_EventReset(pClientCtxt->hClientEvent);
    }

    pEventNode = SlimBus_EnqueueMsgEvent(&pDevCtxt->TxMsgFifo, pClientCtxt, pBuf, hEvent);
    pDevCtxt->TxMsgFifo.sendBufFcn(pDevCtxt, &pDevCtxt->TxMsgFifo);
    if (NULL == pEventNode)
    {
      /* This should never be NULL due to previous check of client's free stack */
      return SB_ERROR;
    }

    /* Set Tx and Rx markers for event notification */
    if (NULL != pucReadData)
    {
      pEventNode->uRxMarker = 
        SB_BF_SET(pDevice->LA,RXMARKER,LA)|
        SB_BF_SET(TID,RXMARKER,TID)|
        SB_BF_SET(uReadDataLen,RXMARKER,BUF_SIZE);
      pEventNode->pRxBuf = pucReadData;
      pEventNode->pRxBufSize = puActualReadDataLen;
      pDevice->TIDMap[TID&SB_TID_IDX_MASK] = pEventNode;
    }
    else
    {
      pEventNode->uRxMarker = 0;
      pEventNode->pRxBuf = NULL;
      pEventNode->pRxBufSize = NULL;
    }

    if (pClientCtxt->hClientEvent == hEvent)
    {
      result = SlimBus_WaitForMsgEvent(pClientCtxt, pEventNode);
      /* On success, Tx/Rx queue processing logic will pop the event from 
         the Tx/Rx event queue and push event node back on free stack */

      if (SB_SUCCESS != result)
      {
        /* On wait failure, we need to do event cleanup here */
        SlimBus_CleanupMsgEvent(pDevice, TID, pEventNode, pEventNode->pFreeRoot);
        SB_LOG_ERR_1("Event wait timeout during SendClientMessage (uAvail: %ld)",
                     pDevice->uAvailTrans);
      }
      else
      {
        /* if there was no error waiting for the event notification, 
           check if there was an error during the transaction (recorded in 
           the client error code) and return the error */
        result = pClientCtxt->eLastMsgErr;
        pClientCtxt->eLastMsgErr = SLIMBUS_ERR_NONE;
      }

      if (SB_SUCCESS != result)
      {
        SB_LOG_ERR_1("Client message transaction failure (client: 0x%lx)", (uint32)pClientCtxt);
      }
    }
  }
  
  return result;
}

/**
 * @brief Function containing aggregated logic for performing 
 *        any IE/VE read/write operation
 *
 * This function contains aggregated logic for performing any 
 * IE/VE read or write operation 
 * 
 * @param[in] pClientCtxt  Pointer to the client context
 * @param[in] MC  Message code to use to generate the message
 * @param[in] LA  Destination logical address to send the 
 *       message to
 * @param[in] uByteAddr  Byte address of the start of the IE/VE 
 *       slice
 * @param[in] pucWriteData  Pointer to the buffer containing 
 *       data to write to the IE/VE slice, or NULL if there is
 *       no data to write
 * @param[in] uWriteDataLen  Size of the write data buffer
 * @param[out] pucReadData  Pointer to the buffer to store data 
 *       read from the IE/VE slice, or NULL if there is not data
 *       to read
 * @param[in] uReadDataLen  Size of the read data buffer
 * @param[out] puActualReadDataLen  Pointer to the location to 
 *       store the actual number of bytes read from the IE/VE
 *       slice
 * @param[in] hEvent  Client event handle, for asynchronous 
 *       operation
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
static SBResult
SlimBus_DoReadWriteOp( SlimBusClientCtxt *pClientCtxt, uint32 MC, SlimBusLogicalAddrType  LA, uint32  uByteAddr,  const uint8*  pucWriteData,  uint32  uWriteDataLen, uint8*  pucReadData,  uint32  uReadDataLen,  uint32*  puActualReadDataLen, SlimBusEventHandle  hEvent )
{
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SBResult result = SB_SUCCESS;
  uint8 ucCode, ucCurSize, TID = 0;
  uint32 EC, uMsgLen = 0;
  uint32 *pBuf;
  SlimBusDeviceType *pDevice = NULL;
  SlimBusMsgFifoType *pTxMsgFifo = NULL;
  
  /* Slice length is always equal to "read data" length */  
  const uint32 uSliceLen = uReadDataLen;
  
  /* Get the slice code and validate that the slice length is valid */
  ucCode = GetSliceCodeFromSize(uSliceLen);
  ucCurSize = GetSliceSizeFromCode(ucCode);
  if (ucCurSize != uSliceLen)
  {
    SlimBus_LockDevice(pDevCtxt);
    SB_LOG_ERR_2("DoReadWriteOp got bad slice size (ucCurSize: 0x%lx) (uSliceLen: %ld)",
                 ucCurSize, uSliceLen);
    SlimBus_UnlockDevice(pDevCtxt);
    return SB_ERROR;
  }
  /* Non-blocking operation is not allowed when there is read data */
  if ( NULL != pucReadData &&
       (SLIMBUS_EVENT_HANDLE_NON_BLOCKING == hEvent ||
        SLIMBUS_EVENT_HANDLE_BATCHED_XFER == hEvent) )
  {
    SlimBus_LockDevice(pDevCtxt);
    SB_LOG_ERR_0("Non blocking DoReadWriteOp with read data pointer");
    SlimBus_UnlockDevice(pDevCtxt);
    return SB_ERROR;
  }

  if ( (hEvent == SLIMBUS_EVENT_HANDLE_BATCHED_XFER) &&
       (NULL == pClientCtxt->hBatchMem) )
  {
    if (SB_SUCCESS != SlimBus_SetBatchBufferSizeInternal(pClientCtxt, SB_DEFAULT_BATCH_BUFFER_SIZE))
    {
      SlimBus_LockDevice(pDevCtxt);
      SB_LOG_ERR_0("Error attempting to initialize buffer for batch transfers");
      SlimBus_UnlockDevice(pDevCtxt);
      return SB_ERROR;
    }
  }

  SlimBus_LockClient(pClientCtxt);
  if ( !pClientCtxt->bOpen )
  {
    SlimBus_UnlockClient(pClientCtxt);
    return SB_ERROR;
  }

  if (SLIMBUS_EVENT_HANDLE_BATCHED_XFER == hEvent)
  {
    pTxMsgFifo = &pClientCtxt->BatchMsgFifo;
  }
  else
  {
    SlimBus_LockDevice(pDevCtxt);

    pTxMsgFifo = &pDevCtxt->TxMsgFifo;

    SB_LOG_INFO_4("Got IE/VE transaction (LA: 0x%x) (addr: 0x%lx) (#write: %ld) (#read: %ld)",
                  LA, uByteAddr, ((NULL!=pucWriteData)?uReadDataLen:0),
                  ((NULL!=pucReadData)?uReadDataLen:0));
  }

  do
  {
    if (SLIMBUS_EVENT_HANDLE_BATCHED_XFER != hEvent)
    {
      /* Get the device structure pointer and validate that device has enumerated */
      if (NULL == (pDevice = SlimBus_GetDevicePtr(pDevCtxt, LA)))
      {
        SB_LOG_ERR_1("Unable to get device pointer (LA: 0x%x)", LA);
        result = SB_ERROR;
        break;
      }

      /* Get an event notification structure (not used for non-blocking operation) */
      if (SLIMBUS_EVENT_HANDLE_NON_BLOCKING != hEvent)
      {
        if (NULL == PeekHeadEvent(&pClientCtxt->FreeEventStack))
        {
          SB_LOG_ERR_1("No free events during DoReadWriteOp (client 0x%lx)", (uint32)pClientCtxt);
          result = SB_ERROR;
          break;
        }
      }
    }
    
    /* Length of fixed-length portions of the IE/VE messages */
    uMsgLen = SB_MSG_MC_CHANGE_VALUE__CM_OFFSET;
    if (NULL != pucReadData)
    {
      /* Add length of TID field */
      uMsgLen++;
    }
    if (NULL != pucWriteData)
    {
      /* Add length of data to write */
      uMsgLen += uWriteDataLen;
    }

    /* Get message buffer */
    pBuf = pTxMsgFifo->getBufFcn(pClientCtxt, pDevice, pTxMsgFifo, uMsgLen);
    if (NULL == pBuf)
    {
      result = SB_ERROR;
      break;
    }

    /* assemble the Element Code for the message */
    EC = 
      SB_BF_SET(ucCode,EC,SS) |
      SB_BF_SET(SB_EC_BYTE_ACCESS,EC,BYTE_MODE) |
      SB_BF_SET(uByteAddr,EC,ADDR);
    *pBuf = SB_MSG_ASM_FIRST_WORD(uMsgLen, SB_MSG_MT_CORE,
                                  MC, SB_MSG_DT_UNICAST, LA);
    {
      uint8 *pucBuf = ((uint8*)pBuf)+SB_MSG_MC_REQUEST_VALUE__TID_OFFSET;
      
      /* Get the next available Transaction ID if there is data to read */
      if (NULL != pucReadData)
      {
        do {
          pDevice->uNextTIDIdx = (pDevice->uNextTIDIdx + 1) % SB_MAX_TIDS;
        } while (pDevice->TIDMap[pDevice->uNextTIDIdx] != NULL);
        TID = (uint8) (pDevice->uBaseTID + pDevice->uNextTIDIdx);
        pDevice->uAvailTrans--;
        
        *(pucBuf++) = TID;
      }
      
      *(pucBuf++) = (EC&0xff);
      *(pucBuf++) = (EC>>8)&0xff;
      
      if (NULL != pucWriteData && uWriteDataLen > 0)
      {
        SlimBus_Memcpy(pucBuf, pucWriteData, uWriteDataLen);
      }
    }

    /* Send the message and wait for completion notification */
    result = SlimBus_SendClientMessage(pClientCtxt, pBuf, pDevice, pucReadData,
                                       uReadDataLen, puActualReadDataLen, hEvent);
    
  } while (FALSE);

  if (SLIMBUS_EVENT_HANDLE_BATCHED_XFER != hEvent)
  {
    SlimBus_UnlockDevice(pDevCtxt);
  }
  SlimBus_UnlockClient(pClientCtxt);
  
  return result;  
}

/**
  @brief Read value elements from a slave device.
  
  This function reads value element(s) from a slave device using
  byte-based access.  This function will perform an asynchronous
  read if the hEvent parameter passed is not NULL, otherwise a 
  synchronous read will be performed. 
   
  For an asynchronous transaction, the hEvent will be signalled 
  when the transaction is complete, and the error status can be 
  retrieved by SlimBus_GetLastError(). 
  
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] LA  Logical address of the device to read values 
        elements from
  @param[in] uByteAddr  The byte address of the first byte to 
        read.
  @param[out] pucReadData  Pointer to the buffer to store the 
        data read from the value elements
  @param[in] uReadDataLen  Number of value element bytes to read 
        and the size of the read data buffer; allowed values for
        uReadDataLen are 1, 2, 3, 4, 6, 8, 12 and 16
  @param[in] puActualReadDataLen  Optional pointer to location 
        to store the number of value element bytes actually read
  @param[in] hEvent  Event handle
  
  @return  SB_SUCCESS on success, an error code on error
   
  */
SBResult 
SlimBus_ReadValueElement(uint32 ClientId, SlimBusHandle h, SlimBusLogicalAddrType  LA, uint32  uByteAddr,  uint8*  pucReadData,  uint32  uReadDataLen,  uint32*  puActualReadDataLen, SlimBusEventHandle  hEvent) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);

  SB_CHECK_IRQL();

  if ( !pClientCtxt->bOpen ||
       ( NULL == pucReadData ) ||
       ( 0 == uReadDataLen ) ||
       ( SB_VE_MAX_SLICE_SIZE_BYTES < uReadDataLen ) ||
       ( uByteAddr + uReadDataLen > SB_VE_MAP_RESERVED_ADDR ) )
  {
    SlimBus_LockDevice(pDevCtxt);
    SB_LOG_ERR_4("Read VE validation error (bOpen: %ld) (uByteAddr: 0x%lx) "
                 "(uReadDataLen: %ld) (pucReadData: 0x%lx)", pClientCtxt->bOpen,
                 uByteAddr, uReadDataLen, (uint32)pucReadData);
    SlimBus_UnlockDevice(pDevCtxt);
    return SB_ERROR;
  }

  return SlimBus_DoReadWriteOp(pClientCtxt, SB_MSG_MC_REQUEST_VALUE, 
                               LA, uByteAddr, NULL, 0,
                               pucReadData, uReadDataLen, puActualReadDataLen,
                               hEvent);
}

/**
  @brief Write value elements to a slave device.
  
  This function writes value element(s) to a slave device using 
  byte-based access.  This function will perform a synchronous 
  write if the hEvent parameter is equal to NULL, a non-blocking
  write if the hEvent parameter is equal to 
  SLIMBUS_EVENT_HANDLE_NON_BLOCKING, or otherwise an 
  asynchronous write will be performed.
   
  For an asynchronous transaction, the hEvent will be signalled 
  when the transaction is complete, and the error status can be 
  retrieved by SlimBus_GetLastError(). 
  
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] LA  Logical address of the device to write values 
        elements to
  @param[in] uByteAddr  The byte address of the first byte to 
        write.
  @param[in] uByteSize  The number of value element bytes to 
        write; allowed values for uByteSize are 1, 2, 3, 4, 6,
        8, 12 and 16
  @param[in] pucWriteData  Optional pointer to the buffer 
        containing the data to write to the value elements; if
        NULL is passed for pucWriteData then 0 must be passed
        for uWriteDataLen
  @param[in] uWriteDataLen  Size of the write data buffer; if 
        the write buffer length is less than the number of bytes
        to write, the destination device will pad the end of the
        write data with zeros; if the write data buffer length
        is larger than the number of bytes to write, the extra
        bytes at the end will be ignored
  @param[in] hEvent  Event handle
  
  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_WriteValueElement(uint32 ClientId, SlimBusHandle h, SlimBusLogicalAddrType  LA, uint32  uByteAddr, uint32  uByteSize, const uint8*  pucWriteData,  uint32  uWriteDataLen, SlimBusEventHandle  hEvent) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  
  SB_CHECK_IRQL();

  if ( !pClientCtxt->bOpen ||
       ( 0 == uByteSize ) ||
       ( SB_VE_MAX_SLICE_SIZE_BYTES < uByteSize ) ||
       ( SB_VE_MAX_SLICE_SIZE_BYTES < uWriteDataLen ) ||
       ( NULL == pucWriteData && uWriteDataLen > 0 ) ||
       ( uByteAddr + uByteSize > SB_VE_MAP_RESERVED_ADDR ) )
  {
    SlimBus_LockDevice(pDevCtxt);
    SB_LOG_ERR_5("Write VE validation error (bOpen: %ld) (uByteAddr: 0x%lx) (uByteSize: %ld) "
                 "(uWriteDataLen: %ld) (pucWriteData: 0x%lx)", pClientCtxt->bOpen,
                 uByteAddr, uByteSize, uWriteDataLen, (uint32)pucWriteData);
    SlimBus_UnlockDevice(pDevCtxt);
    return SB_ERROR;
  }
  
  if ( uWriteDataLen > uByteSize )
  {
    uWriteDataLen = uByteSize;
  }
  while ( uWriteDataLen > 0 && pucWriteData[uWriteDataLen-1] == 0x00 )
  {
    uWriteDataLen--;
  }

  return SlimBus_DoReadWriteOp(pClientCtxt, SB_MSG_MC_CHANGE_VALUE, 
                               LA, uByteAddr, pucWriteData, uWriteDataLen,
                               NULL, uByteSize, NULL, hEvent);
}


/**
  @brief Perform atomic exchange of value elements on a slave 
         device.
  
  This function performs an atomic exchange of value element(s) 
  on a slave device  This function will perform an asynchronous 
  exchange if the hEvent parameter passed is not NULL, otherwise 
  a synchronous exchange will be performed. 
   
  For an asynchronous transaction, the hEvent will be signalled 
  when the transaction is complete, and the error status can be 
  retrieved by SlimBus_GetLastError(). 
  
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] LA  Logical address of the device to exchange 
        values elements on
  @param[in] uByteAddr  The byte address of the first byte to 
        read.
  @param[in] pucWriteData  Optional pointer to the buffer 
        containing the data to write to the value elements; if
        NULL is passed for pucWriteData then 0 must be passed
        for uWriteDataLen
  @param[in] uWriteDataLen  Size of the write data buffer; if 
        the write buffer length is less than the number of bytes
        to read, the destination device will pad the end of
        the write data with zeros; if the write data buffer
        length is larger than the number of bytes to read, the
        extra bytes at the end will be ignored
  @param[out] pucReadData  Pointer to the buffer to store the 
        data read from the value elements
  @param[in] uReadDataLen  Number of value element bytes to 
        write and read, and the size of the read data buffer;
        allowed values for uReadDataLen are 1, 2, 3, 4, 6, 8, 12
        and 16
  @param[in] puActualReadDataLen  Optional pointer to location 
        to store the number of value element bytes actually read
  @param[in] hEvent  Event handle
  
  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_ExchangeValueElement(uint32 ClientId, SlimBusHandle h, SlimBusLogicalAddrType  LA, uint32  uByteAddr,  const uint8*  pucWriteData,  uint32  uWriteDataLen,  uint8*  pucReadData,  uint32  uReadDataLen,  uint32*  puActualReadDataLen, SlimBusEventHandle  hEvent) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);

  SB_CHECK_IRQL();

  if ( !pClientCtxt->bOpen ||
       ( NULL == pucReadData ) ||
       ( 0 == uReadDataLen ) ||
       ( SB_VE_MAX_SLICE_SIZE_BYTES < uReadDataLen ) ||
       ( SB_VE_MAX_SLICE_SIZE_BYTES < uWriteDataLen ) ||
       ( NULL == pucWriteData && uWriteDataLen > 0 ) ||
       ( uByteAddr + uReadDataLen > SB_VE_MAP_RESERVED_ADDR ) )
  {
    return SB_ERROR;
  }

  if ( uWriteDataLen > uReadDataLen )
  {
    uWriteDataLen = uReadDataLen;
  }
  while ( uWriteDataLen > 0 && pucWriteData[uWriteDataLen-1] == 0x00 )
  {
    uWriteDataLen--;
  }

  return SlimBus_DoReadWriteOp(pClientCtxt, SB_MSG_MC_REQUEST_CHANGE_VALUE,
                               LA, uByteAddr, pucWriteData, uWriteDataLen, 
                               pucReadData, uReadDataLen, puActualReadDataLen,
                               hEvent);
}


/**
  @brief Read information elements from a slave device.
  
  This function reads information element(s) from a slave device 
  using byte-based access.  This function will perform an 
  asynchronous read if the hEvent parameter passed is not NULL, 
  otherwise a synchronous read will be performed. 
   
  For an asynchronous transaction, the hEvent will be signalled 
  when the transaction is complete, and the error status can be 
  retrieved by SlimBus_GetLastError().
  
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] LA  Logical address of the device to read 
        information elements from
  @param[in] uByteAddr  The byte address of the first byte to 
        read.
  @param[out] pucReadData  Pointer to the buffer to store the 
        data read from the information elements
  @param[in] uReadDataLen  Number of information element bytes 
        to read and the size of the read data buffer; allowed
        values for uReadDataLen are 1, 2, 3, 4, 6, 8, 12 and 16
  @param[in] puActualReadDataLen  Optional pointer to location 
        to store number of information element bytes actually
        read
  @param[in] hEvent  Event handle
  
  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_ReadInfoElement(uint32 ClientId, SlimBusHandle h, SlimBusLogicalAddrType  LA, uint32  uByteAddr,  uint8*  pucReadData,  uint32  uReadDataLen,  uint32*  puActualReadDataLen, SlimBusEventHandle  hEvent) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);

  SB_CHECK_IRQL();

  if ( !pClientCtxt->bOpen ||
       ( NULL == pucReadData ) ||
       ( 0 == uReadDataLen ) ||
       ( SB_VE_MAX_SLICE_SIZE_BYTES < uReadDataLen ) ||
       ( uByteAddr + uReadDataLen > SB_VE_MAP_RESERVED_ADDR ) )
  {
    return SB_ERROR;
  }

  return SlimBus_DoReadWriteOp(pClientCtxt, SB_MSG_MC_REQUEST_INFORMATION,
                               LA, uByteAddr, NULL, 0,
                               pucReadData, uReadDataLen, puActualReadDataLen,
                               hEvent);
}


/**
  @brief Clear information elements on a slave device.
  
  This function clears information element(s) on a slave device 
  using byte-based access. This function will perform a synchronous 
  clear if the hEvent parameter is equal to NULL, a non-blocking
  clear if the hEvent parameter is equal to 
  SLIMBUS_EVENT_HANDLE_NON_BLOCKING, or otherwise an 
  asynchronous clear will be performed. 
   
  For an asynchronous transaction, the hEvent will be signalled 
  when the transaction is complete, and the error status can be 
  retrieved by SlimBus_GetLastError(). 
  
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] LA  Logical address of the device to clear 
        information elements on
  @param[in] uByteAddr  The byte address of the first byte to 
        clear
  @param[in] uByteSize  Number of information element bytes to 
        clear; allowed values for uByteSize are 1, 2, 3, 4, 6,
        8, 12 and 16
  @param[in] pucClearMask  Optional pointer to the buffer 
        containing the mask to use for clearing information
        elements, where every bit set to '1' in the mask
        indicates that the corresponding information element bit
        should be cleared; if NULL is passed for pucClearMask
        then 0 must be passed for uClearMaskLen
  @param[in] uClearMaskLen  Size of the clear mask buffer; if 
        the clear mask length is less than the number of bytes
        to clear, the destination device will pad the end of the
        clear mask with ones; if the clear mask length is larger
        than the number of bytes to clear, the extra bytes at
        the end will be ignored
  @param[in] hEvent  Event handle
  
  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_ClearInfoElement(uint32 ClientId, SlimBusHandle h, SlimBusLogicalAddrType  LA, uint32  uByteAddr, uint32  uByteSize, const uint8*  pucClearMask,  uint32  uClearMaskLen, SlimBusEventHandle  hEvent) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  const uint8 *pucMyClearMask = pucClearMask;
  uint8 ucTempClearMask[SB_VE_MAX_SLICE_SIZE_BYTES];

  SB_CHECK_IRQL();

  if ( !pClientCtxt->bOpen ||
       ( 0 == uByteSize ) ||
       ( SB_VE_MAX_SLICE_SIZE_BYTES < uByteSize ) ||
       ( SB_VE_MAX_SLICE_SIZE_BYTES < uClearMaskLen ) ||
       ( NULL == pucClearMask && uClearMaskLen > 0 ) ||
       ( uByteAddr + uByteSize > SB_VE_MAP_RESERVED_ADDR ) )
  {
    return SB_ERROR;
  }

  if ( uClearMaskLen > uByteSize )
  {
    uClearMaskLen = uByteSize;
  }
  
  if (SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt)->eWrkrnd & SB_WRKRND_O_IE_CLEAR)
  {
    if (uClearMaskLen < uByteSize)
    {
      memset(ucTempClearMask, 0xff, uByteSize);
      SlimBus_Memcpy(ucTempClearMask, pucClearMask, uClearMaskLen);
      pucMyClearMask = ucTempClearMask;
      uClearMaskLen = uByteSize;
    }
  }
  else
  {
    while ( uClearMaskLen > 0 && pucClearMask[uClearMaskLen-1] == 0xff )
    {
      uClearMaskLen--;
    }
  }

  return SlimBus_DoReadWriteOp(pClientCtxt, SB_MSG_MC_CLEAR_INFORMATION,
                               LA, uByteAddr, pucMyClearMask, uClearMaskLen, 
                               NULL, uByteSize, NULL, hEvent);
}


/**
  @brief Perform atomic read and clear of information elements 
         on a slave device.
  
  This function performs an atomic read and clear of information
  element(s) on a slave device using byte-based access.  This 
  function will perform an asynchronous operation if the hEvent 
  parameter passed is not NULL, otherwise a synchronous 
  operation will be performed. 
   
  For an asynchronous transaction, the hEvent will be signalled 
  when the transaction is complete, and the error status can be 
  retrieved by SlimBus_GetLastError(). 
  
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] LA  Logical address of the device to read and clear
        information elements on
  @param[in] uByteAddr  The byte address of the first byte to 
        read and clear
  @param[in] pucClearMask  Optional pointer to the buffer 
        containing the mask to use for clearing information
        elements, where every bit set to '1' in the mask
        indicates that the corresponding information element bit
        should be cleared; if NULL is passed for pucClearMask
        then 0 must be passed for uClearMaskLen
  @param[in] uClearMaskLen  Size of the clear mask buffer; if 
        the clear mask length is less than the number of bytes
        to clear, the destination device will pad the end of the
        clear mask with ones; if the clear mask length is larger
        than the number of bytes to clear, the extra bytes at
        the end will be ignored
  @param[out] pucReadData  Pointer to the buffer to store the 
        data read from the information elements
  @param[in] uReadDataLen  Number of information element bytes 
        to read and clear, and the size of the read data buffer;
        allowed values for uReadDataLen are 1, 2, 3, 4, 6, 8, 12
        and 16
  @param[out] puActualReadDataLen  Optional pointer to location 
        to store number of information element bytes actually
        read
  @param[in] hEvent  Event handle
  
  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_ReadAndClearInfoElement(uint32 ClientId, SlimBusHandle h, SlimBusLogicalAddrType  LA, uint32  uByteAddr,  const uint8*  pucClearMask,  uint32  uClearMaskLen, uint8*  pucReadData,  uint32  uReadDataLen,  uint32*  puActualReadDataLen, SlimBusEventHandle  hEvent) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  const uint8 *pucMyClearMask = pucClearMask;
  uint8 ucTempClearMask[SB_VE_MAX_SLICE_SIZE_BYTES];

  SB_CHECK_IRQL();

  if ( !pClientCtxt->bOpen ||
       ( NULL == pucReadData ) ||
       ( 0 == uReadDataLen ) ||
       ( SB_VE_MAX_SLICE_SIZE_BYTES < uReadDataLen ) ||
       ( SB_VE_MAX_SLICE_SIZE_BYTES < uClearMaskLen ) ||
       ( NULL == pucClearMask && uClearMaskLen > 0 ) ||
       ( uByteAddr + uReadDataLen > SB_VE_MAP_RESERVED_ADDR ) )
  {
    return SB_ERROR;
  }

  if ( uClearMaskLen > uReadDataLen )
  {
    uClearMaskLen = uReadDataLen;
  }
  
  if (SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt)->eWrkrnd & SB_WRKRND_O_IE_CLEAR)
  {
    if (uClearMaskLen < uReadDataLen)
    {
      memset(ucTempClearMask, 0xff, uReadDataLen);
      SlimBus_Memcpy(ucTempClearMask, pucClearMask, uClearMaskLen);
      pucMyClearMask = ucTempClearMask;
      uClearMaskLen = uReadDataLen;
    }
  }
  else
  {
    while ( uClearMaskLen > 0 && pucClearMask[uClearMaskLen-1] == 0xff )
    {
      uClearMaskLen--;
    }
  }

  return SlimBus_DoReadWriteOp(pClientCtxt, SB_MSG_MC_REQUEST_CLEAR_INFORMATION,
                               LA, uByteAddr, pucMyClearMask, uClearMaskLen, 
                               pucReadData, uReadDataLen, puActualReadDataLen,
                               hEvent);
}



/**
  @brief Send a destination-referred user-defined message to a 
         slave device.
 
  This function sends a destination-referred user-defined 
  message to a slave device.  This function will perform an 
  asynchronous operation if the hEvent parameter is not NULL, 
  otherwise a synchronous operation is performed and the 
  function will return when the message transmission is 
  complete. 
  
  For an asynchronous transaction, the hEvent will be signaled 
  when the transaction is complete, and the error status can be 
  retrieved by SlimBus_GetLastError(). 
  
  @param[in] _h  Client handle to the SLIMbus driver
  
  @param[in] LA  Logical address of the device to send the
        user-defined message to
  
  @param[in] MC  Message code of the user-defined message
  
  @param[in] pucPayload  Pointer to the payload data to send
  
  @param[in] uPayloadLen  Size of the payload data in bytes
  
  @param[in] hEvent  Event handle
  
  @return SB_SUCCESS on success, an error code on error 
   
  @see SlimBus_GetLastError()
 */
SBResult
SlimBus_SendUserDefinedMessage(uint32 ClientId, SlimBusHandle h, SlimBusLogicalAddrType LA, uint32 MC, const uint8* pucPayload, uint32 uPayloadLen, SlimBusEventHandle hEvent)
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = (SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt));
  SBResult result = SB_SUCCESS;
  uint32 uMsgLen = 0;
  uint32 *pBuf;
  SlimBusDeviceType *pDevice = NULL;
  SlimBusMsgFifoType *pTxMsgFifo = NULL;


  SB_CHECK_IRQL();

  if ( !pClientCtxt->bOpen ||
       ( SB_MAX_MSG_PAYLOAD_SIZE_BYTES < uPayloadLen ) ||
       ( NULL == pucPayload && uPayloadLen > 0 ) )
  {
    SlimBus_LockDevice(pDevCtxt);
    SB_LOG_ERR_3("Send user-defined msg VE validation error (bOpen: %ld) "
                 "(uPayloadLen: %ld) (pucPayload: 0x%lx)", pClientCtxt->bOpen,
                 uPayloadLen, (uint32)pucPayload);
    SlimBus_UnlockDevice(pDevCtxt);
    return SB_ERROR;
  }

  if ( (hEvent == SLIMBUS_EVENT_HANDLE_BATCHED_XFER) &&
       (NULL == pClientCtxt->hBatchMem) )
  {
    if (SB_SUCCESS != SlimBus_SetBatchBufferSizeInternal(pClientCtxt, SB_DEFAULT_BATCH_BUFFER_SIZE))
    {
      SlimBus_LockDevice(pDevCtxt);
      SB_LOG_ERR_0("Error attempting to initialize buffer for batch transfers");
      SlimBus_UnlockDevice(pDevCtxt);
      return SB_ERROR;
    }
  }

  SlimBus_LockClient(pClientCtxt);

  if (SLIMBUS_EVENT_HANDLE_BATCHED_XFER == hEvent)
  {
    pTxMsgFifo = &pClientCtxt->BatchMsgFifo;
  }
  else
  {
    SlimBus_LockDevice(pDevCtxt);

    pTxMsgFifo = &pDevCtxt->TxMsgFifo;

    SB_LOG_INFO_3("Got user-defined message request (LA: 0x%x) (MC: %ld) (size: %ld)",
                  LA, MC, uPayloadLen);
  }

  do
  {
    if (SLIMBUS_EVENT_HANDLE_BATCHED_XFER != hEvent)
    {
      /* Get the device structure pointer and validate that device has enumerated */
      if (NULL == (pDevice = SlimBus_GetDevicePtr(pDevCtxt, LA)))
      {
        SB_LOG_ERR_1("Unable to get device pointer (LA: 0x%x)", LA);
        result = SB_ERROR;
        break;
      }
    
      if ( SB_MSG_LA_MANAGER == LA ||
           NULL != pDevice->pSat )
      {
        SB_LOG_ERR_1("Got request to send user-defined message to restricted device (LA: 0x%x)", LA);
        result = SB_ERROR;
        break;
      }

      /* Get an event notification structure (not used for non-blocking operation) */
      if (SLIMBUS_EVENT_HANDLE_NON_BLOCKING != hEvent)
      {
        if (NULL == PeekHeadEvent(&pClientCtxt->FreeEventStack))
        {
          SB_LOG_ERR_1("No free events during SendUserDefinedMessage (client 0x%lx)", (uint32)pClientCtxt);
          result = SB_ERROR;
          break;
        }
      }
    }

    /* fixed-length and payload portions of the message */
    uMsgLen = SB_MSG_MC_REQUEST_VALUE__TID_OFFSET + uPayloadLen;

    /* Get message buffer */
    pBuf = pTxMsgFifo->getBufFcn(pClientCtxt, pDevice, pTxMsgFifo, uMsgLen);
    if (NULL == pBuf)
    {
      result = SB_ERROR;
      break;
    }

    /* assemble the message */
    *pBuf = SB_MSG_ASM_FIRST_WORD(uMsgLen, SB_MSG_MT_DEST_REFERRED_USER,
                                  MC, SB_MSG_DT_UNICAST, LA);
    {
      uint8 *pucBuf = ((uint8*)pBuf)+SB_MSG_MC_REQUEST_VALUE__TID_OFFSET;

      if (NULL != pucPayload && uPayloadLen > 0)
      {
        //  pDevCtxt->TxMsgFifo.getBufFcn makes sure that there is enough
        //  space in the destination buffer.
        //     
        SlimBus_Memscpy(pucBuf, uPayloadLen, pucPayload, uPayloadLen);
      }
    }

    /* Send the message and wait for completion notification */
    result = SlimBus_SendClientMessage(pClientCtxt, pBuf, pDevice, NULL, 0, NULL, hEvent);
  } while (FALSE);

  if (SLIMBUS_EVENT_HANDLE_BATCHED_XFER != hEvent)
  {
    SlimBus_UnlockDevice(pDevCtxt);
  }
  SlimBus_UnlockClient(pClientCtxt);

  return result;  
}


/**
  @brief Send out all queued batch messages. 
  
  This function sends out all batched messages that have
  been queued; all messages that have been submitted using
  the SLIMBUS_EVENT_HANDLE_BATCHED_XFER flag will be sent
  out.  This function is synchronous and will return
  execution to the client after all messages have been
  transmitted successfully.
 
  @param[in] h  Client handle to the SLIMbus driver
  
  @return SB_SUCCESS on success, an error code on error 
  */
SBResult
SlimBus_SendBatchedMessages(uint32 ClientId, SlimBusHandle h)
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = (SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt));
  SBResult result = SB_SUCCESS;
  SlimBusEventNodeType *pEventNode = NULL;
  
  SB_CHECK_IRQL();
  
  SlimBus_LockClient(pClientCtxt);
  SlimBus_LockDevice(pDevCtxt);

  if ( !pClientCtxt->bOpen ||
       !pClientCtxt->hBatchMem ||
       0 == pClientCtxt->BatchMsgFifo.uHeadIdx )
  {
    SB_LOG_ERR_0("Validation error during SendBatchedMessages");
    SlimBus_UnlockDevice(pDevCtxt);
    SlimBus_UnlockClient(pClientCtxt);
    return SB_ERROR;
  }
    
  do
  {
    uint32 uData = pClientCtxt->BatchMsgFifo.pBuf[0];
    SlimBusLogicalAddrType LA = (SlimBusLogicalAddrType) SB_MSG_GET(uData,GENERIC_0,LA);
    SlimBusDeviceType *pDevice = NULL;
    
    SB_LOG_INFO_2("Got request to send batched messages (client 0x%lx) (#bytes: %ld)",
                  (uint32)pClientCtxt, (uint32)pClientCtxt->BatchMsgFifo.uHeadIdx*4);
    
    /* Get the device structure pointer and validate that device has enumerated */
    if (NULL == (pDevice = SlimBus_GetDevicePtr(pDevCtxt, LA)))
    {
      SB_LOG_ERR_1("Unable to get device pointer (LA: 0x%x)", LA);
      result = SB_ERROR;
      break;
    }
        
    if (NULL == PeekHeadEvent(&pClientCtxt->FreeEventStack))
    {
      SB_LOG_ERR_1("No free events during SendBatchMessages (client 0x%lx)", (uint32)pClientCtxt);
      result = SB_ERROR;
      break;
    }
    
    /* Wait until we have enumerated and there are available transactions */
    result = SlimBus_WaitForAvailDeviceTrans(pClientCtxt, pDevice);
    if (SB_SUCCESS != result)
    {
      break;
    }

    /* Indicate that this client is waiting to send a Tx message */
    pDevCtxt->uNumTxMsgWaiters++;

    result = pDevCtxt->leaveLowPowerFcn(pDevCtxt);
    if (SB_SUCCESS != result)
    {
      pDevCtxt->uNumTxMsgWaiters--;
      break;
    }

    /* Don't send any Tx messages until the bus is in an active state, to
       avoid sending messages across a clock pause boundary or when a device
       has lost message sync after clock pause */    
    result = SlimBus_WaitForBusActive(pClientCtxt, pDevice);
    if (SB_SUCCESS != result)
    {
      pDevCtxt->uNumTxMsgWaiters--;
      break;
    }

    /* Indicate that this client is proceeding to send a Tx message and
       is no longer waiting */
    pDevCtxt->uNumTxMsgWaiters--;
    
    /* Wait until the Tx FIFO is no longer busy */
    while ( pDevCtxt->TxMsgFifo.bBusy )
    {
      result = SlimBus_WaitForMsgFifoNotBusy(pClientCtxt, &pDevCtxt->TxMsgFifo);
      if (SB_SUCCESS != result)
      {
        SB_LOG_ERR_1("Timeout waiting for Tx Fifo to go idle during SendBatchedMessages (client 0x%lx)", (uint32)pClientCtxt);
        break;
      }
    }
    
    /* Create event handle again to grab current thread ID */
    SlimBus_EventReset(pClientCtxt->hClientEvent);

    pEventNode = SlimBus_EnqueueMsgEvent(&pDevCtxt->TxMsgFifo, pClientCtxt, 
                                         pClientCtxt->BatchMsgFifo.pBuf,
                                         pClientCtxt->hClientEvent);
    if (NULL == pEventNode)
    {
      /* This should never be NULL due to previous check of client's free stack */
      result = SB_ERROR;
      break;
    }
    /* Set Tx marker for event notification */
    pEventNode->pTxPhysBuf = pClientCtxt->BatchMsgFifo.pPhysBuf;
    pEventNode->uRxMarker = 0;
    pEventNode->pRxBuf = NULL;
    pEventNode->pRxBufSize = NULL;

    pDevCtxt->TxMsgFifo.bBusy = TRUE;
    result = SlimBus_BamTransferIOVec(pDevCtxt->TxMsgFifo.pBamCtxt,
                                      (SlimBusMsgIdxType)pClientCtxt->BatchMsgFifo.pPhysBuf,
                                      (uint32)pClientCtxt->BatchMsgFifo.uHeadIdx*4,
                                      NULL,
                                      SLIMBUS_BAM_IOVEC_FLAG_EOT);
    if ( SB_SUCCESS != result )
    {
      break;
    }
    
    result = SlimBus_WaitForMsgEvent(pClientCtxt, pEventNode);

    if (SB_SUCCESS != result)
    {
      /* On wait failure, we need to do event cleanup here */
      SlimBus_CleanupMsgEvent(NULL, 0, pEventNode, pEventNode->pFreeRoot);
      SB_LOG_ERR_0("Event wait timeout during SendBatchedMessage");
    }
    else
    {
      /* if there was no error waiting for the event notification, 
         check if there was an error during the transaction (recorded in 
         the client error code) and return the error */
      result = pClientCtxt->eLastMsgErr;
      pClientCtxt->eLastMsgErr = SLIMBUS_ERR_NONE;
    }

    if (SB_SUCCESS != result)
    {
      SB_LOG_ERR_1("Batch message transaction failure (client: 0x%lx)", (uint32)pClientCtxt);
    }
        
    pClientCtxt->BatchMsgFifo.uHeadIdx = 0;
  } while(FALSE);
  
  SlimBus_UnlockDevice(pDevCtxt);
  SlimBus_UnlockClient(pClientCtxt);

  return result;
}


/**
  @brief Allocate SLIMbus data channel
   
  This function allocates a SLIMbus data channel.

  @param[in] h  Client handle to the SLIMbus driver 
  @param[out] phChannel  Location to store the allocated data 
        channel handle

  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_AllocDataChannel(uint32 ClientId, SlimBusHandle h, SlimBusResourceHandle * phChannel) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SBResult result = SLIMBUS_ERR_OUT_OF_RESOURCES;
  int i;
  
  SlimBus_LockDevice(pDevCtxt);

  if (!pClientCtxt->bOpen ||
      NULL == phChannel)
  {
    SlimBus_UnlockDevice(pDevCtxt);
    return SB_ERROR;
  }

  for (i = 0; i < (int)pDevCtxt->numChannelSlots; i++)
  {
    SlimBusChannelType *pChannel = pDevCtxt->pChannels[i];
    if (NULL == pChannel)
    {
      SlimBus_Malloc(sizeof(*pChannel), (void**)&pChannel, SB_MEMTAG_21);
      if (NULL != pChannel)
      {
        memset(pChannel, 0, sizeof(*pChannel));
        pDevCtxt->pChannels[i] = pChannel;
        pChannel->handle = SB_HANDLE_INVALID;
        pChannel->eState = SB_HANDLE_AVAILABLE;
        pChannel->channelNum = pDevCtxt->uBaseChanNum + i;
        RootNodeInit(&pChannel->EndPointList);
      }
    }
    if (NULL != pChannel &&
        pChannel->eState == SB_HANDLE_AVAILABLE)
    {
      SlimBusResourceHandle handle =
        SB_SET_HANDLE_TYPE(SB_HANDLE_TYPE_CHANNEL) |
        SB_SET_HANDLE_PRN(GetPrn(pDevCtxt)) |
        SB_SET_HANDLE_IDX(i);
      pChannel->eState = SB_HANDLE_ALLOCATED;
      pChannel->pClientCtxt = pClientCtxt;
      pChannel->handle = handle;
      pChannel->eCurStatus = SLIMBUS_CHANNEL_STATUS_REMOVED;
      pChannel->eNewStatus = SLIMBUS_CHANNEL_STATUS_REMOVED;
      *phChannel = handle;

      result = SB_SUCCESS;
      break;
    }
  }

  if (SB_SUCCESS != result)
  {
    SB_LOG_ERR_0("Failed to allocated data channel");
  }

  SlimBus_UnlockDevice(pDevCtxt);

  return result;
}

/**
  @brief Allocate shared SLIMbus data channel
   
  This function allocates a SLIMbus data channel that can be 
  shared and utilized from more than one execution environment. 

  @param[in] h  Client handle to the SLIMbus driver 
   
  @param[in] uChannelNum  Shared channel number
   
  @param[out] phChannel  Location to store the allocated data 
        channel handle

  @return  SB_SUCCESS on success, an error code on error
   
  @see SlimBus_DeallocDataChannel(), SlimBus_AllocDataChannel() 
  */
SBResult 
SlimBus_AllocSharedDataChannel(uint32 ClientId, SlimBusHandle h, uint32 uChannelNum, SlimBusResourceHandle * phChannel)
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SBResult result = SB_ERROR;
  uint32 uPortIdx;
  SlimBusChannelType *pChannel = NULL;
  
  SlimBus_LockDevice(pDevCtxt);

  if (!pClientCtxt->bOpen ||
      NULL == phChannel)
  {
    SlimBus_UnlockDevice(pDevCtxt);
    return SB_ERROR;
  }

  do
  {
    if (uChannelNum < pDevCtxt->uBaseSharedChanNum ||
        uChannelNum >= pDevCtxt->uBaseSharedChanNum + pDevCtxt->numSharedChannelSlots)
    {
      SB_LOG_ERR_1("Shared channel allocation request out of range (chan# %ld)", uChannelNum );
      break;
    }
    uPortIdx = uChannelNum - pDevCtxt->uBaseSharedChanNum;
    
    if (NULL == pClientCtxt->paSharedChannels)
    {
      if (SB_SUCCESS != SlimBus_Malloc(pDevCtxt->numSharedChannelSlots*
                                       (sizeof(pClientCtxt->paSharedChannels[0])),
                                       (void**)&pClientCtxt->paSharedChannels, SB_MEMTAG_23))
      {
        break;
      }

      memset(pClientCtxt->paSharedChannels, 0,
             pDevCtxt->numSharedChannelSlots*sizeof(pClientCtxt->paSharedChannels[0]));

    }
    if (NULL != pClientCtxt->paSharedChannels)
    {
      pChannel = pClientCtxt->paSharedChannels[uPortIdx];
      
      if (NULL == pChannel)
      {
        SlimBus_Malloc(sizeof(*pChannel), (void**)&pChannel, SB_MEMTAG_22);
        if (NULL != pChannel)
        {
          memset(pChannel, 0, sizeof(*pChannel));
          pClientCtxt->paSharedChannels[uPortIdx] = pChannel;
          pChannel->handle = SB_HANDLE_INVALID;
          pChannel->eState = SB_HANDLE_AVAILABLE;
          pChannel->channelNum = uChannelNum;
          RootNodeInit(&pChannel->EndPointList);
        }
      }
      if (NULL != pChannel &&
          pChannel->eState == SB_HANDLE_AVAILABLE)
      {
        SlimBusResourceHandle handle =
          SB_SET_HANDLE_TYPE(SB_HANDLE_TYPE_SHARED_CHANNEL) |
          SB_SET_HANDLE_PRN(GetPrn(pDevCtxt)) |
          SB_SET_HANDLE_IDX(uPortIdx);
        pChannel->eState = SB_HANDLE_ALLOCATED;
        pChannel->pClientCtxt = pClientCtxt;
        pChannel->handle = handle;
        pChannel->eCurStatus = SLIMBUS_CHANNEL_STATUS_REMOVED;
        pChannel->eNewStatus = SLIMBUS_CHANNEL_STATUS_REMOVED;
        *phChannel = handle;
  
        result = SB_SUCCESS;
        break;
      }
    }

    result = SLIMBUS_ERR_OUT_OF_RESOURCES;
    SB_LOG_ERR_1("Shared channel alloc failed, already allocated (chan# %ld)", uChannelNum );
  } while(FALSE);

  SlimBus_UnlockDevice(pDevCtxt);

  return result;
}

/**
 * @brief Calculate the Presence Rate (PR) for a data channel
 *
 * This function calculates the Presence Rate (PR) for a data 
 * channel 
 * 
 * @param[in] pChannelReq  Pointer to the channel request data 
 *       structure
 * 
 * @return The calculated presence rate 
 */
static uint32 
SlimBus_CalcPresenceRate(const SlimBusChannelReqType *pChannelReq)
{
  uint32 uRate = 0, u4kRate = 0, u11kRate = 0;
  uint32 uExp = 0;
  uint32 uPR = 0;
  SBBOOL bExact = TRUE;
  SBBOOL bDone = FALSE;
  SlimBusRateFamilyType eRateFam;

  /* Presence rate is always 0 for async-simplex protocol */
  if ( pChannelReq->eDesiredProtocol >= SLIMBUS_PROTO_ASYNC_SIMPLEX )
  {
    return 0;
  }
  
  if ( pChannelReq->eBaseSampleRate == SLIMBUS_RATE_FAM_1_HZ )
  {
    uRate = pChannelReq->uRateMultiplier / 4000;
    u4kRate = uRate;
    if ( uRate * 4000 == pChannelReq->uRateMultiplier )
    {
      eRateFam = SLIMBUS_RATE_FAM_4_KHZ;
    }
    else
    {
      uRate = pChannelReq->uRateMultiplier / 11025;
      u11kRate = uRate;
      if ( uRate * 11025 == pChannelReq->uRateMultiplier )
      {
        eRateFam = SLIMBUS_RATE_FAM_11p025_KHZ;
        
      }
      else
      {
        eRateFam = SLIMBUS_RATE_FAM_1_HZ;
        uRate = pChannelReq->uRateMultiplier;
      }
    }
  }
  else
  {
    eRateFam = pChannelReq->eBaseSampleRate;
    uRate = pChannelReq->uRateMultiplier;
  }
  
  if ( eRateFam == SLIMBUS_RATE_FAM_1_HZ )
  {
    /* do best-effort PR calculation using either 11k or 4k/12k families */
    bExact = FALSE;
    if ( (u4kRate+1) * 4000 < (u11kRate+1) * 11025 )
    {
      uRate = u4kRate + 1;  /* round up */
      eRateFam = SLIMBUS_RATE_FAM_4_KHZ;
    }
    else
    {
      uRate = u11kRate + 1;  /* round up */
      eRateFam = SLIMBUS_RATE_FAM_11p025_KHZ;
    }
  }
  
  /* Convert uRate to coefficient-exponent representation */
  while (!bDone)
  {
    while ( (uRate & 0x1) != 0x1 )
    {
      uRate >>= 1;
      uExp++;
    }
    if ( uRate > 3 )
    {
      /* Round-up if the coefficient would not be exactly equal to 1 or 3 */
      uRate++;
      bExact = FALSE;
    }
    else
    {
      /* The coefficient is now either 1 or 3 and we are done */
      bDone = TRUE;
    }
  }
  
  if ( eRateFam == SLIMBUS_RATE_FAM_4_KHZ )
  {
    if ( uRate == 1 )
    {
      uPR = 0x10;
    }
    else
    {
      uPR = 0x00;
      uExp++;
    }
  }
  else
  {
    uPR = 0x08;
    uExp++;
  }
  if ( uExp <= 7 )
  {
    uPR |= uExp;
    if ( bExact )
    {
      uPR |= 0x80;  /* add Frequency Locked bit */
    }
  }
  else
  {
    uPR = 0;
  }
  
  return uPR;
}

/**
  @brief Set or change the requested parameters of a data 
         channel
   
  This function sets or changes the requested parameters of a 
  data channel.  The new parameters take effect at the next 
  reconfiguration boundary. 
   
  The event handle passed as a parameter will be signalled if 
  the requested data channel parameters are rejected due to lack 
  of bandwidth.  After requesting data channel parameters, the 
  data channel status returned by SlimBus_GetDataChannelStatus() 
  will be updated once the data channel is accepted. 

  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hChannel  Data channel handle or resource group 
        handle
  @param[in] pChannelReq  Pointer to the requested data channel 
        parameters
  
  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_NextDefineDataChannel(uint32 ClientId, SlimBusHandle h, SlimBusResourceHandle  hChannel, const SlimBusChannelReqType * pChannelReq) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SlimBusMsgFifoType *pMsgFifo;
  uint32 MC, uCnt = 1;
  SlimBusGroupType *pGroup = NULL;
  SlimBusChannelType *pChannel = NULL;
  SBResult result = SB_SUCCESS;
  uint32 *pBuf;
  
  SlimBus_LockClient(pClientCtxt);
  SlimBus_LockDevice(pDevCtxt);

  if ( !pClientCtxt->bOpen ||
       NULL == pChannelReq ||
       SLIMBUS_PROTO_NUM_ENUMS <= pChannelReq->eDesiredProtocol ||
       SLIMBUS_RATE_FAM_NUM_ENUMS <= pChannelReq->eBaseSampleRate ||
       SLIMBUS_DATA_FORMAT_NUM_ENUMS <= pChannelReq->eDataFormat ||
       SLIMBUS_SIDEBAND_FORMAT_NUM_ENUMS <= pChannelReq->eSidebandFormat ||
       0 != (pChannelReq->uSampleSize_bits & 0x3) ||
       SB_MAX_DATA_SEGMENT_LEN < (pChannelReq->uSampleSize_bits/SB_CELLS_PER_SLOT) ||
       0 == pChannelReq->uRateMultiplier )
  {
    SlimBus_UnlockDevice(pDevCtxt);
    SlimBus_UnlockClient(pClientCtxt);
    return SB_ERROR;
  }

  /* TODO: master does not support LPCM data format */

  if ( SB_MASTER_DRIVER == pDevCtxt->eDrvType )
  {
    pMsgFifo = &pClientCtxt->BasicClient.ConfigMsgFifo;    
  }
  else
  {
    pMsgFifo = &pDevCtxt->TxMsgFifo;
  }

  do
  {
    uint32 uReqRate = 0;  /* number of samples per root superframe */

    /* coefficient and exponent representation for uRate, where:
       uRate == uRateCoeff * (2^uRateExp), and
       uRateCoeff in {1,3}
          see: Channel Rate Multiplier in SLIMbus spec Table 20 */
    uint32 uRateExp = 0, uRateCoeff = 0;
    SlimBusRateCoeffType eRateCoeff;
    uint32 uPR = 0;  /* presence rate */
    uint32 uPushPullNum = 0, uPushPullDiv = 0;
    /* bExact is true if the number samples per root superframe is integer and
       meets the restrictions on the value of Channel Rate Multiplier from
       SLIMbus spec Table 20 */
    SBBOOL bExact = TRUE;
    SlimBusProtocolType eProto;
    uint32 TID;
    SlimBusPortFlowType eFlow = SLIMBUS_PORT_NUM_ENUMS;
    SlimBusPortReqType eReqs = SLIMBUS_PORT_REQ_O_DEFAULT;
    
    pChannel = GetChannelHandle(pClientCtxt, hChannel, &pGroup);
    if ( NULL == pChannel )
    {
      result = SB_ERROR;
      break;
    }
    if ( NULL == pGroup &&
         NULL != pChannel->pGroup )
    {
      SB_LOG_WARN_2("Request to define individual channel within a multi-channel "
                    "group coverted to a group request (channel: %ld) (resource: %ld)",
                    pChannel->channelNum, hChannel);
      pGroup = pChannel->pGroup;
      pChannel = NextChannelNode(&pGroup->ResourceList, NULL);
      if ( NULL == pChannel )
      {
        result = SB_ERROR;
        break;
      }
      hChannel = pChannel->handle;
    }
    if ( NULL != pGroup )
    {
      uCnt = GetNodeCount(&pGroup->ResourceList);
    }

    SB_LOG_VERB_8("Got define channel request (client: 0x%lx) (resource: 0x%lx) (channel: %ld) "
                  "(#channels: %ld) (protocol: %d) (baserate: %d) (ratemult: %ld) (bitwidth: %ld)",
                  (uint32)pClientCtxt, hChannel, pChannel->channelNum, uCnt, 
                  pChannelReq->eDesiredProtocol, pChannelReq->eBaseSampleRate,
                  pChannelReq->uRateMultiplier, pChannelReq->uSampleSize_bits);

    if ( pClientCtxt->bReConfigPending )
    {
      SB_LOG_ERR_1("Reconfiguration pending during call to define channel "
                   "(client: 0x%lx)", (uint32)pClientCtxt );
      result = SB_ERROR;
      break;
    }

    {
      SlimBusChannelType *pTempChannel = pChannel;
      SBBOOL bFirst = TRUE;

      /* Validate that all the master ports are configured the same */
      do
      {
        SlimBusEndPointType *pEndPoint = NULL;
        while (NULL != (pEndPoint = NextEndPointNode(&pTempChannel->EndPointList, pEndPoint)))
        {
          if ( 0 == HAL_sb_PgdPortGetChannelNumber(pDevCtxt->hSB, 0, pEndPoint->pPort->portNum) )
          {
            if (bFirst)
            {
              eFlow = pEndPoint->pPort->eFlow;
              eReqs = pEndPoint->pPort->eReqs;
              bFirst = FALSE;
              break;
            }
            else if (eFlow != pEndPoint->pPort->eFlow ||
                     eReqs != pEndPoint->pPort->eReqs)
            {
              SB_LOG_ERR_1("Master port flow validation error during call to define channel "
                           "(client: 0x%lx)", (uint32)pClientCtxt );
              result = SB_ERROR;
              pGroup = NULL;
              break;
            }
          }
        }
        /* Don't allow a new definition if channel is being removed */
        if ( pTempChannel->eCurStatus != pTempChannel->eNewStatus &&
             SLIMBUS_CHANNEL_STATUS_REMOVED == pTempChannel->eNewStatus )
        {
          SB_LOG_ERR_1("Channel removal pending during call to define channel "
                       "(client: 0x%lx)", (uint32)pClientCtxt );
          result = SB_ERROR;
          break;
        }
      } while ((NULL != pGroup) && (NULL != (pTempChannel = NextChannelNode(&pGroup->ResourceList, pTempChannel))));
      if (SB_SUCCESS != result)
      {
        break;
      }
    }
    
    /* Calculate presence rate */
    uPR = SlimBus_CalcPresenceRate(pChannelReq);
    
    /* TODO: reconsider if this needs to be done when not push/pull */
    if ( pChannelReq->eBaseSampleRate == SLIMBUS_RATE_FAM_4_KHZ )
    {
      uReqRate = 4000 * pChannelReq->uRateMultiplier;
    }
    else if ( pChannelReq->eBaseSampleRate == SLIMBUS_RATE_FAM_11p025_KHZ )
    {
      uReqRate = 11025 * pChannelReq->uRateMultiplier;
    }
    else
    {
      uReqRate = pChannelReq->uRateMultiplier;
    }
    /* Max allowed sample freq: 768 segments/superframe @28.8Mhz = 3.6 MHz */
    if (uReqRate > 3600000)
    {
      result = SB_ERROR;
      break;
    }

    /* Compute uRate (the number of samples per root superframe) */
    if ( pChannelReq->eBaseSampleRate == SLIMBUS_RATE_FAM_4_KHZ &&
         pDevCtxt->uSuperFreq == 4000 )
    {
      /* Optimized calculation for root frequency == 24576000 Hz */
      uRateCoeff = pChannelReq->uRateMultiplier;
    }
    else if ( pChannelReq->eBaseSampleRate == SLIMBUS_RATE_FAM_11p025_KHZ &&
              pDevCtxt->uSuperFreq == 3675 )
    {
      /* Optimized calculation for root freqeuncy == 22579200 Hz */
      uRateCoeff = 3 * pChannelReq->uRateMultiplier;
    }
    else
    {
      uint32 uTemp = 0;
      
      uTemp = uReqRate * SB_CELLS_PER_SUPERFRAME_DIV8;
      uRateCoeff = uTemp / pDevCtxt->uRootFreqDiv8;
      if ( uRateCoeff * pDevCtxt->uRootFreqDiv8 != uTemp )
      {
        /* Round-up if the number of samples per root superframe is not integer */  
        uRateCoeff++;
        bExact = FALSE;
      }
    }

    /* Convert uRateCoeff to coefficient-exponent representation */
    uRateExp = 0;
    {
      SBBOOL bDone = FALSE;
      while (!bDone)
      {
        while ( (uRateCoeff & 0x1) != 0x1 )
        {
          uRateCoeff >>= 1;
          uRateExp++;
        }
        if ( uRateCoeff > 3 )
        {
          /* Round-up the coefficient if it would not be exactly 1 or 3 */
          uRateCoeff++;
          bExact = FALSE;
        }
        else
        {
          /* The coefficient is now either 1 or 3 and we are done */
          bDone = TRUE;
        }
      }
    }

    eProto = pChannelReq->eDesiredProtocol;
    if ( eProto == SLIMBUS_PROTO_HARD_ISO &&
         !bExact )
    {
      /* Its an error if client requets to use only isochronous protocol but
         we can't fit the sample rate evenly into the root frequency */
      result = SB_ERROR;
      break;
    }
    else if ( eProto == SLIMBUS_PROTO_AUTO_ISO )
    {
      if ( bExact )
      {
        eProto = SLIMBUS_PROTO_HARD_ISO;
      }
      else
      {
        /* Convert auto-isochronous protocol to pushed or pulled if the sample
           rate doesn't fit evenly into the root frequency */
        if ( 0 != (eReqs & SLIMBUS_PORT_REQ_O_HALF_DUPLEX ) )
        {
          SB_LOG_ERR_1("Unable to convert half duplex to push-pull in call to define channel "
                       "(client: 0x%lx)", (uint32)pClientCtxt );
          result = SB_ERROR;
          break;
        }
        if ( SLIMBUS_PORT_SOURCE_FLOW == eFlow )
        {
          eProto = SLIMBUS_PROTO_PUSHED;
        }
        else if ( SLIMBUS_PORT_SINK_FLOW == eFlow )
        {
          eProto = SLIMBUS_PROTO_PULLED;
        }
        else
        {
          SB_LOG_ERR_1("Auto iso mode specified for channel but unable"
                       "to determine flow direction (client: 0x%lx)", (uint32)pClientCtxt );
          result = SB_ERROR;
          break;
        }
      }
    }
    
    /* Convert coefficient integer value to enumeration value, coefficient 
       integer value is either 1 or 3 */
    if ( uRateCoeff == 1 )
    {
      if ( uRateExp > 9 )
      {
        result = SB_ERROR;
        break;
      }
      eRateCoeff = SB_RATE_COEFF_1;
    }
    else
    {
      if ( uRateExp > 8 )
      {
        result = SB_ERROR;
        break;
      }
      eRateCoeff = SB_RATE_COEFF_3;
    }

    /* compute pushed/pulled hardware presence rate */
    if ( SLIMBUS_PROTO_PULLED == eProto ||
         SLIMBUS_PROTO_PUSHED == eProto )
    {
      uint32 uActualRate = (uRateCoeff << uRateExp) * pDevCtxt->uRootFreqDiv8 / 
        (SB_CELLS_PER_SUPERFRAME_DIV8);
      uint32 uPrimeIdx = 0;
      /* First 8 primes excluding 2 */
      const uint32 uaPrimes[] = { 3, 5, 7, 11, 13, 17, 19, 23 };

      uPushPullNum = uReqRate;
      uPushPullDiv = uActualRate;

      SB_LOG_VERB_2("Converted rate for push/pull protocol: %ld => %ld", uReqRate, uActualRate);
      
      /* Partially reduce the fraction, removing only the factors of 2 */
      while ( ((uPushPullNum & 0x1) == 0) &&
              ((uPushPullDiv & 0x1) == 0) )
      {
        uPushPullNum >>= 1;
        uPushPullDiv >>= 1;
      }

      /* Reducing the fraction by factors of 2 was not sufficient, do further reduction */
      for ( uPrimeIdx = 0;
            uPushPullDiv > 0x7fff && uPrimeIdx < sizeof(uaPrimes)/sizeof(uaPrimes[0]);
            uPrimeIdx++ )
      {
        while ( (uPushPullDiv > 0x7fff) &&
                ((uPushPullNum % uaPrimes[uPrimeIdx]) == 0) &&
                ((uPushPullDiv % uaPrimes[uPrimeIdx]) == 0) )
        {
          uPushPullNum /= uaPrimes[uPrimeIdx];
          uPushPullDiv /= uaPrimes[uPrimeIdx];
        }
      }
      
      /* If we couldn't sufficiently reduce the fraction */      
      if ( uPushPullDiv > 0x7fff )
      {
        result = SB_ERROR;
        break;
      }
    }
    
    pBuf = pMsgFifo->getBufFcn(pClientCtxt, SB_GET_MGR_DEV_PTR(pDevCtxt),
                              pMsgFifo, SB_MSG_MC_USR_CHANNEL_DEF__CN_OFFSET+uCnt);
    if ( NULL == pBuf )
    {
      result = SB_ERROR;
      break;
    }
    
    if ( pChannelReq->eOptions & SLIMBUS_CHAN_O_AUTO_ACTIVATE )
    {
      MC = SB_MSG_MC_USR_CHANNEL_DEF_ACTIVATE;
    }
    else
    {
      MC = SB_MSG_MC_USR_CHANNEL_DEF;
    }

    TID = SlimBus_GetNextMasterTID(pDevCtxt);
    pBuf[0] = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_USR_CHANNEL_DEF__CN_OFFSET+uCnt,
                                    SB_MSG_MT_DEST_REFERRED_USER,
                                    MC, SB_MSG_DT_UNICAST,
                                    SB_MSG_LA_MANAGER) |
      SB_MSG_SET(pClientCtxt->BasicClient.uClientNum,USR_CHANNEL_DEF_0,CLIENT_NUM) |
      SB_MSG_SET(pChannelReq->eDataFormat,USR_CHANNEL_DEF_0,DF);
    pBuf[1] =
      SB_MSG_SET(pChannelReq->uSampleSize_bits/SB_CELLS_PER_SLOT,USR_CHANNEL_DEF_1,DL) |
      SB_MSG_SET(eRateCoeff,USR_CHANNEL_DEF_1,COEFF) |
      SB_MSG_SET(pChannelReq->eSidebandFormat,USR_CHANNEL_DEF_1,SF) |
      SB_MSG_SET(eProto,USR_CHANNEL_DEF_1,PROTO) |
      SB_MSG_SET(uRateExp,USR_CHANNEL_DEF_1,EXP) |
      SB_MSG_SET(uPR,USR_CHANNEL_DEF_1,PR) |
      SB_MSG_SET(TID,USR_CHANNEL_DEF_1,TID);
    {
      uint8 *pucBuf = (uint8*)pBuf + SB_MSG_MC_USR_CHANNEL_DEF__CN_OFFSET;
      do
      {
        SlimBusEndPointType *pEndPoint = NULL;
        
        *(pucBuf++) = (uint8)pChannel->channelNum;
        
        while (NULL != (pEndPoint = NextEndPointNode(&pChannel->EndPointList, pEndPoint)))
        {
          if (0 == HAL_sb_PgdPortGetChannelNumber(pDevCtxt->hSB, 0, pEndPoint->pPort->portNum))
          {
            /* Program the push/pull fraction counter if protocol is push or pull */
            if ( ( SLIMBUS_PROTO_PULLED == eProto &&
                   SLIMBUS_PORT_SINK_FLOW == pEndPoint->pPort->eFlow ) ||
                 ( SLIMBUS_PROTO_PUSHED == eProto &&
                   SLIMBUS_PORT_SOURCE_FLOW == pEndPoint->pPort->eFlow ) )
            {
              HAL_sb_PgdPortSetNumSamplesForPushPull(pDevCtxt->hSB, 0,
                                                     pEndPoint->pPort->portNum,
                                                     uPushPullNum);
              HAL_sb_PgdPortSetRptPeriodForPushPull(pDevCtxt->hSB, 0,
                                                    pEndPoint->pPort->portNum,
                                                    uPushPullDiv);
            }
            else
            {
              HAL_sb_PgdPortSetNumSamplesForPushPull(pDevCtxt->hSB, 0,
                                                     pEndPoint->pPort->portNum,
                                                     0);
              HAL_sb_PgdPortSetRptPeriodForPushPull(pDevCtxt->hSB, 0,
                                                    pEndPoint->pPort->portNum,
                                                    0);
            }
          }
        }

        if ( pChannelReq->eOptions & SLIMBUS_CHAN_O_AUTO_ACTIVATE || 
             pChannel->eCurStatus == SLIMBUS_CHANNEL_STATUS_ACTIVE )
        {
          pChannel->eNewStatus = SLIMBUS_CHANNEL_STATUS_ACTIVE;
        }
        else
        {
          pChannel->eNewStatus = SLIMBUS_CHANNEL_STATUS_INACTIVE;
        }

        /* save for DEBUGGING */
        pChannel->ChannelReq = *pChannelReq;

      } while ( (NULL != pGroup) &&
                (NULL != (pChannel = NextChannelNode(&pGroup->ResourceList, pChannel))) );
    }

    result = SlimBus_SendMasterAckedMsg(pClientCtxt, pMsgFifo, pBuf, (uint8)TID);
    if (SB_SUCCESS == result ||
        SLIMBUS_ERR_MSG_RESP_TIMEOUT == result)
    {
      /* When there is a timeout, assume master will eventually process
         the request, and therefore update the internal state accordingly */
      pClientCtxt->uPendingReconfigReqs++;
    }
    if (SB_SUCCESS != result)
    {
      SB_LOG_ERR_1("Master ack failure in call to define channel (client: 0x%lx)",
                   (uint32)pClientCtxt );
    }
  } while (FALSE);

  if (SB_SUCCESS != result)
  {
    SB_LOG_ERR_1("Error during define data channel request (client: 0x%lx)", (uint32)pClientCtxt);
  }

  SlimBus_UnlockDevice(pDevCtxt);
  SlimBus_UnlockClient(pClientCtxt);

  return result;
}


/**
  @brief Connect a port to a data channel
   
  This function connects the specified port to the data channel. 
  The connection will take effect immediately. 
   
  For master ports, if the BAM pipe has already been configured, 
  the port FIFO will begin to fill as soon as the port is 
  connected. 

  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hChannel  Data channel handle 
  @param[in] eFlow  The type of port connection to make, source 
        or sink
  @param[in] hPort  Port handle
  
  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_ConnectPortToChannel(uint32 ClientId, SlimBusHandle h, SlimBusResourceHandle  hChannel, SlimBusPortFlowType  eFlow, SlimBusResourceHandle  hPort) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  uint32 portNum, chanNum;
  SlimBusLogicalAddrType LA;
  SlimBusGroupType *pGroup = NULL;
  SlimBusChannelType *pChannel = NULL;
  SlimBusMasterPortType *pPort = NULL;
  uint32 endPointNum = 0;
  SlimBusDeviceType *pDevice = NULL;
  SBResult result = SB_SUCCESS;

  SB_CHECK_IRQL();

  SlimBus_LockClient(pClientCtxt);
  SlimBus_LockDevice(pDevCtxt);
  
  if ( !pClientCtxt->bOpen )
  {
    SlimBus_UnlockDevice(pDevCtxt);
    SlimBus_UnlockClient(pClientCtxt);
    return SB_ERROR;
  }

  pChannel = GetChannelHandle(pClientCtxt, hChannel, &pGroup);

  do
  {
    if ( NULL != pDevCtxt->pSched &&
         pDevCtxt->pSched->bPurge )
    {
      SB_LOG_ERR_0("Attempt to connect port during channel purge");
      result = SB_ERROR;
      break;
    }

    if (NULL == pChannel)
    {
      result = SB_ERROR;
      break;
    }
    
    chanNum = pChannel->channelNum;

    /* If the port to connect is a master port, make sure we locally own
       the port, that the port is not already connected to a channel, and that
       the channel is not already connected to another local master port */
    if ( SB_GET_HANDLE_TYPE(hPort) == SB_HANDLE_TYPE_MASTER_PORT )
    {
      uint32 idx = SB_GET_HANDLE_IDX(hPort);
      if (idx >= pDevCtxt->numPorts || 
          idx >= sizeof(pDevCtxt->Ports)/sizeof(pDevCtxt->Ports[0]) ||
          hPort != pDevCtxt->Ports[idx].handle)
      {
        result = SB_ERROR;
        break;
      }
      pPort = &pDevCtxt->Ports[idx];
      portNum = pPort->portNum;
      result = SlimBus_GetMasterPgdLA(pClientCtxt, &LA);
      if (SB_SUCCESS != result)
      {
        break;
      }
      if ( NULL != pGroup &&
           0 == endPointNum &&
           GetNodeCount(&pGroup->ResourceList) > pPort->numEndPoints )
      {
        SB_LOG_ERR_4("Not enough endpoints to support connect master port request "
                     "(client: 0x%lx) (#channels: %ld) (port: %ld) (#endpoints %ld)",
                     (uint32)pClientCtxt, GetNodeCount(&pGroup->ResourceList),
                     portNum, pPort->numEndPoints);
        result = SLIMBUS_ERR_NOT_ENOUGH_ENDPOINTS;
        break;
      }
      
      if (eFlow == SLIMBUS_PORT_SOURCE_FLOW)
      {
        SB_LOG_VERB_5("Got connect master port request (client: 0x%lx) (channel: %ld) "
                      "(port: %ld) (endpoint: %ld) (flow: Source) (LA: 0x%x)",
                      (uint32)pClientCtxt, chanNum, portNum, endPointNum, LA);
      }
      else if (eFlow == SLIMBUS_PORT_SINK_FLOW)
      {
        SB_LOG_VERB_5("Got connect master port request (client: 0x%lx) (channel: %ld) "
                      "(port: %ld) (endpoint: %ld) (flow: Sink) (LA: 0x%x)",
                      (uint32)pClientCtxt, chanNum, portNum, endPointNum, LA);
      }
      /* Wait for the port to disconnect if it is in the middle of disconnecting */
      if (pPort->eDisconnectState != SB_PORT_NOT_DISCONNECTING)
      {
        SB_LOG_INFO_3("Waiting for port disconnection during connect port "
                      "(client: 0x%lx) (resource: 0x%lx) (port: %ld)", (uint32)pClientCtxt, 
                      pPort->handle, pPort->portNum );

        pPort->bSignalDisconnect = TRUE;
        SlimBus_EventReset(pClientCtxt->hClientEvent);
        SlimBus_UnlockDevice(pDevCtxt);

        result = SlimBus_EventTimedWait(pClientCtxt,
                                        pClientCtxt->hClientEvent,
                                        SB_DEFAULT_CLIENT_TIMEOUT);

        SlimBus_LockDevice(pDevCtxt);

        if (SB_SUCCESS != result)
        {
          pPort->bSignalDisconnect = FALSE;
          break;
        }
      }
    }
    else if ( SB_GET_HANDLE_TYPE(hPort) == SB_HANDLE_TYPE_SLAVE_PORT )
    {
      LA = SB_GET_HANDLE_LA(hPort);
      portNum = SB_GET_HANDLE_IDX(hPort);

      if (eFlow == SLIMBUS_PORT_SOURCE_FLOW)
      {
        SB_LOG_VERB_5("Got connect slave port request (client: 0x%lx) (channel: 0x%lx) "
                      "(LA: 0x%x) (port: 0x%lx) (endpoint: %ld) (flow: Source)", 
                      (uint32)pClientCtxt, chanNum, LA, portNum, endPointNum);
      }
      else if (eFlow == SLIMBUS_PORT_SINK_FLOW)
      {
        SB_LOG_VERB_5("Got connect slave port request (client: 0x%lx) (channel: 0x%lx) "
                      "(LA: 0x%x) (port: 0x%lx) (endpoint: %ld) (flow: Sink)",
                      (uint32)pClientCtxt, chanNum, LA, portNum, endPointNum);
      }
    }
    else
    {
      result = SB_ERROR;
      break;
    }

    if (eFlow >= SLIMBUS_PORT_NUM_ENUMS)
    {
      result = SB_ERROR;
      break;
    }

    if (NULL == (pDevice = SlimBus_GetDevicePtr(pDevCtxt, LA)))
    {
      result = SB_ERROR;
      break;
    }

    if ( eFlow == SLIMBUS_PORT_SOURCE_FLOW )
    {
      uint32 uIdx;

      for (uIdx = 0; uIdx < pDevCtxt->numPorts; uIdx++)
      {
        SlimBusMasterPortType *pMasterPort = &pDevCtxt->Ports[uIdx];

        if ( (pMasterPort->eFlow == SLIMBUS_PORT_SOURCE_FLOW) &&
             (pDevCtxt->uPortDisconnectMask & (1<<pMasterPort->portNum)) &&
             (SB_PORT_DISCONNECTING != pMasterPort->eDisconnectState) &&
             (HAL_sb_PgdPortGetChannelNumber(pDevCtxt->hSB, 0, pMasterPort->portNum) == pChannel->channelNum) )
        {
          SB_LOG_WARN_2("Manually disconnecting previous source port due to "
                        "channel reactivating (port: %ld) (channel: %ld)", 
                        pMasterPort->portNum, pChannel->channelNum);

          pMasterPort->eDisconnectState = SB_PORT_DISCONNECTING;
          pDevCtxt->uPortDisconnectMask &= ~(1<<pMasterPort->portNum);
          pDevCtxt->disconnectPortFcn(NULL, pDevCtxt, pDevice, pMasterPort->portNum);
        }
      }
    }

    result = pDevCtxt->connectPortFcn(pClientCtxt, pDevice, eFlow, portNum, endPointNum, chanNum);
    if (SB_SUCCESS != result)
    {
      break;
    }
  
    if (NULL != pPort && NULL != pChannel)
    {
      pPort->aEndPoints[endPointNum].pChannel = pChannel;
      PushTailEndPoint(&pChannel->EndPointList, &pPort->aEndPoints[endPointNum]);
      pDevCtxt->uPortConnectMask |= (1<<pPort->portNum);
    }
    
    endPointNum++;
  } while ( (NULL != pGroup) &&
            (NULL != (pChannel = NextChannelNode(&pGroup->ResourceList, pChannel))) );
  
  if (SB_SUCCESS != result)
  {
    SB_LOG_ERR_1("Error during connect channel request (client: 0x%lx)", (uint32)pClientCtxt);
  }

  SlimBus_UnlockDevice(pDevCtxt);
  SlimBus_UnlockClient(pClientCtxt);
  
  return result;
}


/**
  @brief Disconnect a port from a data channel
   
  This function disconnects the specified port from the data 
  channel. The disconnection will take effect immediately. 
   
  For master ports, data flow will stop and the port and BAM 
  pipe will be reset when the port is disconnected.  It is not 
  necessary to explicitly disconnect ports when removing a data 
  channel as described under SlimBus_NextDataChannelControl(). 

  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hPort  Port handle
  
  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_DisconnectPortFromChannel(uint32 ClientId, SlimBusHandle h, SlimBusResourceHandle  hPort) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  uint32 idx, portNum;
  SlimBusLogicalAddrType LA;
  SlimBusMasterPortType *pPort = NULL;
  SlimBusDeviceType *pDevice = NULL;
  uint32 endPointNum;
  SBResult result = SB_SUCCESS;

  SB_CHECK_IRQL();

  SlimBus_LockClient(pClientCtxt);
  SlimBus_LockDevice(pDevCtxt);

  if ( !pClientCtxt->bOpen )
  {
    SlimBus_UnlockDevice(pDevCtxt);
    SlimBus_UnlockClient(pClientCtxt);
    return SB_ERROR;
  }

  do
  {
    /* If we are disconnecting a master port, make sure that port is locally
       owned and that the port is currently connected */
    if ( SB_GET_HANDLE_TYPE(hPort) == SB_HANDLE_TYPE_MASTER_PORT )
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
      for (endPointNum = 0; endPointNum < pPort->numEndPoints; endPointNum++)
      {
        if ( NULL != pPort->aEndPoints[endPointNum].pChannel )
        {
          break;
        }
      }
      if ( endPointNum == pPort->numEndPoints &&
           HAL_sb_PgdPortGetChannelNumber(pDevCtxt->hSB, 0, pPort->portNum) == 0 &&
           pPort->eDisconnectState == SB_PORT_NOT_DISCONNECTING )
      {
        result = SB_ERROR;
        break;
      }
      portNum = pPort->portNum;
      result = SlimBus_GetMasterPgdLA(pClientCtxt, &LA);
      if (SB_SUCCESS != result)
      {
        break;
      }
      SB_LOG_VERB_3("Got disconnect master port request (client: 0x%lx) "
                    "(resource: 0x%lx) (port: %ld)", (uint32)pClientCtxt, hPort, portNum);
    }
    else if ( SB_GET_HANDLE_TYPE(hPort) == SB_HANDLE_TYPE_SLAVE_PORT )
    {
      LA = SB_GET_HANDLE_LA(hPort);
      portNum = SB_GET_HANDLE_IDX(hPort);

      SB_LOG_VERB_4("Got disconnect slave port request (client: 0x%lx) "
                    "(resource: 0x%lx) (LA: 0x%x) (port %ld)", (uint32)pClientCtxt, 
                    hPort, LA, portNum);
    }
    else
    {
      result = SB_ERROR;
      break;
    }

    if (NULL == (pDevice = SlimBus_GetDevicePtr(pDevCtxt, LA)))
    {
      result = SB_ERROR;
      break;
    }

    if ( NULL == pPort )
    {
      result = pDevCtxt->disconnectPortFcn(pClientCtxt, pDevCtxt, pDevice, portNum);
    }
    else if ( pPort->eDisconnectState == SB_PORT_NOT_DISCONNECTING )
    {
      if ( pDevCtxt->uPortDisconnectMask & (1<<pPort->portNum) )
    {
        /* If channel is being removed and port disconnected, just wait for
           the disconnection to complete */
      }
      else
      {
        pPort->eDisconnectState = SB_PORT_DISCONNECTING;
        pDevCtxt->uPortFinalDisconnectMask |= (1<<pPort->portNum);
        result = pDevCtxt->disconnectPortFcn(pClientCtxt, pDevCtxt, pDevice, portNum);
      }
    }
    if (SB_SUCCESS != result)
    {
      break;
    }

    if (NULL != pPort)
    {
      /* If port is not yet disconnected, then wait for it to disconnect */
      if ( HAL_sb_PgdPortGetChannelNumber(pDevCtxt->hSB, 0, pPort->portNum) > 0 ||
           NULL != pPort->aEndPoints[0].pChannel ||
           pPort->eDisconnectState != SB_PORT_NOT_DISCONNECTING )
      {
        if (pPort->eDisconnectState == SB_PORT_DISCONNECTING)
        {
          /* Make sure the port interrupt stays enabled even if overflow/underflow occurs */
          EnablePortInterrupt(pDevCtxt, (1<<pPort->portNum));
        }

        pPort->bSignalDisconnect = TRUE;
        SlimBus_EventReset(pClientCtxt->hClientEvent);
        SlimBus_UnlockDevice(pDevCtxt);

        result = SlimBus_EventTimedWait(pClientCtxt,
                                        pClientCtxt->hClientEvent,
                                        SB_DEFAULT_CLIENT_TIMEOUT);

        SlimBus_LockDevice(pDevCtxt);

        if (SB_SUCCESS != result)
        {
          pPort->bSignalDisconnect = FALSE;
        }
      }

      /* Note: pPort->pChannel gets set to NULL during the ISR handler */
    }
  } while (FALSE);

  if (SB_SUCCESS != result)
  {
    SB_LOG_ERR_1("Error during disconnect port from channel request (client: 0x%lx)", (uint32)pClientCtxt);
  }

  SlimBus_UnlockDevice(pDevCtxt);
  SlimBus_UnlockClient(pClientCtxt);

  return result;
}


/**
  @brief Perform internal data channel control operation
   
  This function performs an internal data channel control 
  operation, to either activate, deactivate or remove the data 
  channel. The control operation will take effect at the next 
  reconfiguration boundary. 
   
  For the remove operation, all ports connected to the data 
  channel will be disconnected and reset at the next 
  reconfiguration boundary, and the BAM pipes corresponding to 
  master ports connected to the data channel will be reset at 
  the next reconfiguration boundary.  For the remove operation 
  it is not necessary to explicitly disconnect ports connected 
  to the channel using SlimBus_DisconnectPortFromChannel().

  @param[in] pClientCtxt  Pointer to the client context
  @param[in] pChannel  Pointer to the data channel structure 
  @param[in] pGroup  Optional pointer to the group structure 
  @param[in] eCtrl  The type of channel control operation 
  
  @return  SB_SUCCESS on success, an error code on error
  */
static SBResult 
SlimBus_NextDataChannelControlInternal(SlimBusClientCtxt *pClientCtxt, SlimBusChannelType *pChannel, SlimBusGroupType *pGroup, SlimBusChannelCtrlType  eCtrl) 
{
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SlimBusMsgFifoType *pMsgFifo;
  uint32 uCnt = 1;
  SBResult result = SB_SUCCESS;
  uint32 *pBuf;
  SlimBusResourceHandle hRes = pChannel->handle;

  if ( SB_MASTER_DRIVER == pDevCtxt->eDrvType )
  {
    pMsgFifo = &pClientCtxt->BasicClient.ConfigMsgFifo;    
  }
  else
  {
    pMsgFifo = &pDevCtxt->TxMsgFifo;
  }

  do
  {
    const SlimBusEndPointType *pEndPoint = PeekHeadEndPoint(&pChannel->EndPointList);
    uint32 TID;
    
    if (NULL != pGroup)
    {
      uCnt = GetNodeCount(&pGroup->ResourceList);
      hRes = pGroup->handle;
    }

    SB_LOG_VERB_7("Got data channel control request (client: 0x%lx) "
                  "(resource: 0x%lx) (channel: %ld) (#channels: %ld) (op: %d) "
                  "(#descs: %ld) (#bytes: %ld)", (uint32)pClientCtxt, hRes,
                  pChannel->channelNum, uCnt, eCtrl,
                  ((NULL==pEndPoint)?((uint32)-1):(pEndPoint->pPort->uDescSubmit)),
                  ((NULL==pEndPoint)?((uint32)-1):(pEndPoint->pPort->uBytesSubmit)) );

    if ( pClientCtxt->bReConfigPending )
    {
      result = SB_ERROR;
      break;
    }
    
    /* Validate the channel state change transitions */
    {
      SlimBusChannelType *pTempChannel = pChannel;
      SBBOOL bStateChanged = FALSE;
      do
      {
        if ( eCtrl == SLIMBUS_CHAN_CTRL_REMOVE )
        {  
          if ( SLIMBUS_CHANNEL_STATUS_REMOVED != pTempChannel->eNewStatus )
          {
            bStateChanged = TRUE;
          }
        }
        else if ( SLIMBUS_CHANNEL_STATUS_REMOVED == pTempChannel->eNewStatus )
        {
          SB_LOG_ERR_1("Can't activate/de-activate a channel that is already removed (res: %ld)",
                       pTempChannel->channelNum);
          /* Can't activate or deactivate a channel that is removed */
          result = SB_ERROR;
          break;
        }
        else if ( eCtrl == SLIMBUS_CHAN_CTRL_ACTIVATE &&
                  SLIMBUS_CHANNEL_STATUS_ACTIVE != pTempChannel->eNewStatus )
        {
          bStateChanged = TRUE;
        }
        else if ( eCtrl == SLIMBUS_CHAN_CTRL_DEACTIVATE &&
                  SLIMBUS_CHANNEL_STATUS_INACTIVE != pTempChannel->eNewStatus )
        {  
          bStateChanged = TRUE;
        }
      } while ((NULL != pGroup) && (NULL != (pTempChannel = NextChannelNode(&pGroup->ResourceList, pTempChannel))));
      if ( SB_SUCCESS != result ||
           !bStateChanged )
      {
        break;
      }
    }

    pBuf = pMsgFifo->getBufFcn(pClientCtxt, SB_GET_MGR_DEV_PTR(pDevCtxt),
                              pMsgFifo, SB_MSG_MC_USR_CHANNEL_CONTROL__CN_OFFSET+uCnt);
    if ( NULL == pBuf )
    {
      result = SB_ERROR;
      break;
    }
    
    TID = SlimBus_GetNextMasterTID(pDevCtxt);
    pBuf[0] = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_USR_CHANNEL_CONTROL__CN_OFFSET+uCnt,
                                    SB_MSG_MT_DEST_REFERRED_USER,
                                    SB_MSG_MC_USR_CHANNEL_CONTROL,
                                    SB_MSG_DT_UNICAST,
                                    SB_MSG_LA_MANAGER) |
      SB_MSG_SET(pClientCtxt->BasicClient.uClientNum,USR_CHANNEL_CONTROL_0,CLIENT_NUM) |
      SB_MSG_SET(eCtrl,USR_CHANNEL_CONTROL_0,CTRL);
    {
      SlimBusChannelType *pTempChannel = pChannel;
      uint8 *pucBuf = (uint8*)pBuf + SB_MSG_MC_USR_CHANNEL_CONTROL__CN_OFFSET;
      pBuf[1] = SB_MSG_SET(TID,USR_CHANNEL_CONTROL_1,TID);
      do 
      {
        if (eCtrl == SLIMBUS_CHAN_CTRL_REMOVE)
        {
          SlimBusEndPointType *pEndPoint = NULL;

          pTempChannel->eNewStatus = SLIMBUS_CHANNEL_STATUS_REMOVED;

          while (NULL != (pEndPoint = NextEndPointNode(&pTempChannel->EndPointList, pEndPoint)))
          {
            if (pEndPoint->pPort->eDisconnectState != SB_PORT_DISCONNECTING)
            {
              pDevCtxt->uPortDisconnectMask |= (1<<pEndPoint->pPort->portNum);
            }
          }
        }
        else if (eCtrl == SLIMBUS_CHAN_CTRL_DEACTIVATE)
        {
          pTempChannel->eNewStatus = SLIMBUS_CHANNEL_STATUS_INACTIVE;
        }
        else
        {
          pTempChannel->eNewStatus = SLIMBUS_CHANNEL_STATUS_ACTIVE;
        }
        *(pucBuf++) = (uint8)pTempChannel->channelNum;
      } while ( (NULL != pGroup)  &&
                (NULL != (pTempChannel = NextChannelNode(&pGroup->ResourceList, pTempChannel))) );
    }

    result = SlimBus_SendMasterAckedMsg(pClientCtxt, pMsgFifo, pBuf, (uint8)TID);
    if (SB_SUCCESS == result ||
        SLIMBUS_ERR_MSG_RESP_TIMEOUT == result)
    {
      /* When there is a timeout, assume master will eventually process
         the request, and therefore update the internal state accordingly */
      pClientCtxt->uPendingReconfigReqs++;
    }
    else
    {
      SlimBusChannelType *pTempChannel = pChannel;

      SB_LOG_ERR_2("Master nacked data channel control request, rolling back "
                   "configuration (client: 0x%lx) (eCtrl: %d)", (uint32)pClientCtxt, eCtrl);

      /* Roll back updated state if there was a failure */
      do 
      {
        SlimBusEndPointType *pEndPoint = NULL;
        while (NULL != (pEndPoint = NextEndPointNode(&pTempChannel->EndPointList, pEndPoint)))
        {
          if (pEndPoint->pPort->eDisconnectState != SB_PORT_DISCONNECTING)
          {
            pDevCtxt->uPortDisconnectMask &= ~(1<<pEndPoint->pPort->portNum);
          }
        }

        /* If channel removal is NACKed, it probably means that channel
           has already been removed, so we should try to keep our state
           consistent and mark the channel as removed; otherwise we can
           roll back the state */
        if (SLIMBUS_CHANNEL_STATUS_REMOVED == pTempChannel->eNewStatus)
        {
          pTempChannel->eCurStatus = pTempChannel->eNewStatus;
        }
        else
        {
          pTempChannel->eNewStatus = pTempChannel->eCurStatus;
        }
      } while ( (NULL != pGroup) &&
                ( NULL != (pTempChannel = NextChannelNode(&pGroup->ResourceList, pTempChannel))) );
    }
  } while (FALSE);

  if (SB_SUCCESS != result)
  {
    SB_LOG_ERR_2("Error during data channel control request "
                 "(client: 0x%lx) (eCtrl: %d)", (uint32)pClientCtxt, eCtrl);
  }
  
  return result;
}


/**
  @brief Perform data channel control operation
   
  This function performs a data channel control operation, to 
  either activate, deactivate or remove the data channel.  The 
  control operation will take effect at the next reconfiguration 
  boundary. 
   
  For the remove operation, all ports connected to the data 
  channel will be disconnected and reset at the next 
  reconfiguration boundary, and the BAM pipes corresponding to 
  master ports connected to the data channel will be reset at 
  the next reconfiguration boundary.  For the remove operation 
  it is not necessary to explicitly disconnect ports connected 
  to the channel using SlimBus_DisconnectPortFromChannel().

  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hChannel  Data channel handle or group handle 
  @param[in] eCtrl  The type of channel control operation 
  
  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_NextDataChannelControl(uint32 ClientId, SlimBusHandle h, SlimBusResourceHandle  hChannel, SlimBusChannelCtrlType  eCtrl) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SlimBusGroupType *pGroup = NULL;
  SlimBusChannelType *pChannel = NULL;
  SBResult result = SB_SUCCESS;

  SB_CHECK_IRQL();

  SlimBus_LockClient(pClientCtxt);
  SlimBus_LockDevice(pDevCtxt);

  if ( !pClientCtxt->bOpen ||
       eCtrl >= SLIMBUS_CHAN_NUM_ENUMS )
  {
    SlimBus_UnlockDevice(pDevCtxt);
    SlimBus_UnlockClient(pClientCtxt);
    return SB_ERROR;
  }
  
  do
  {
    pChannel = GetChannelHandle(pClientCtxt, hChannel, &pGroup);
    if ( NULL == pChannel )
    {
      SB_LOG_ERR_1("Failed to perform channel control due to "
                   "invalid handle (res: 0x%lx)", hChannel );
      result = SB_ERROR;
      break;
    }

    if ( NULL == pGroup &&
         NULL != pChannel->pGroup )
    {
      SB_LOG_WARN_2("Request to control individual channel within multi-channel "
                    "group coverted to group control (channel: %ld) (resource: 0x%lx)",
                    pChannel->channelNum, pChannel->pGroup->handle);
      pGroup = pChannel->pGroup;
      pChannel = NextChannelNode(&pGroup->ResourceList, NULL);
      if ( NULL == pChannel )
      {
        result = SB_ERROR;
        break;
      }
      hChannel = pChannel->handle;
    }

    result = SlimBus_NextDataChannelControlInternal(pClientCtxt, pChannel, 
                                                    pGroup, eCtrl);
  } while(FALSE);
  
  SlimBus_UnlockDevice(pDevCtxt);
  SlimBus_UnlockClient(pClientCtxt);

  return result;
}

/**
 * @brief Update the channel status after a reconfiguration 
 *        sequence
 *
 * This function updates a channel status after a 
 * reconfiguration sequence 
 * 
 * @param[in] pClientCtxt  Pointer to the client context
 * @param[in] pChannel  Pointer to the channel structure
 * @param[in] ReconfigResult  The result of the reconfiguration 
 *       sequence
 */
static void SlimBus_UpdateChannelAfterReconfig
(
  SlimBusClientCtxt *pClientCtxt,
  SlimBusChannelType *pChannel,
  SBResult ReconfigResult
)
{
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  
  if (NULL != pChannel &&
      pChannel->eState != SB_HANDLE_AVAILABLE &&
      pChannel->pClientCtxt == pClientCtxt)
  {
    if (SLIMBUS_ERR_MSG_RESP_TIMEOUT == ReconfigResult)
    {
      if ( SLIMBUS_CHANNEL_STATUS_ACTIVE == pChannel->eCurStatus ||
           SLIMBUS_CHANNEL_STATUS_ACTIVE == pChannel->eNewStatus )
      {
        /* If we get a timeout assume the worst case, that the channel is 
           still active and needs to be cleaned up later */
        pChannel->eCurStatus = pChannel->eNewStatus = SLIMBUS_CHANNEL_STATUS_ACTIVE;
      }
      else
      {
        pChannel->eCurStatus = pChannel->eNewStatus;
      }
    }
    else if (SB_SUCCESS == ReconfigResult)
    {
      /* Check for any ports that think they are disconnecting from a channel
         that we have just activated, if that is the case then that port will 
         not actually disconnect, so force the disconnection manually */
      if ( SLIMBUS_CHANNEL_STATUS_REMOVED == pChannel->eCurStatus &&
           SLIMBUS_CHANNEL_STATUS_REMOVED != pChannel->eNewStatus &&
           0 != pDevCtxt->uPortDisconnectMask )
      {
        uint32 uIdx;
        for (uIdx = 0; uIdx < pDevCtxt->numPorts; uIdx++)
        {
          SlimBusMasterPortType *pPort = &pDevCtxt->Ports[uIdx];
          if ( (pDevCtxt->uPortDisconnectMask & (1<<pPort->portNum)) &&
               (SB_PORT_DISCONNECTING != pPort->eDisconnectState) &&
               (HAL_sb_PgdPortGetChannelNumber(pDevCtxt->hSB, 0, pPort->portNum) == pChannel->channelNum) )
          {
            SlimBusDeviceType *pDevice = pDevCtxt->pDevices[(pDevCtxt->pgdLA)&SB_LA_IDX_MASK];
            
            SB_LOG_WARN_2("Manually disconnecting port after channel "
                          "has reactivated (port: %ld) (channel: %ld)", 
                          pPort->portNum, pChannel->channelNum);
            
            pPort->eDisconnectState = SB_PORT_DISCONNECTING;
            pDevCtxt->uPortDisconnectMask &= ~(1<<pPort->portNum);
            pDevCtxt->disconnectPortFcn(NULL, pDevCtxt, pDevice, pPort->portNum);
          }
        }
      }
      
      pChannel->eCurStatus = pChannel->eNewStatus;
    }
    else
    {
      pChannel->eNewStatus = pChannel->eCurStatus;
    }
    
    if ( SB_HANDLE_DEALLOCATED == pChannel->eState &&
         SLIMBUS_CHANNEL_STATUS_REMOVED == pChannel->eCurStatus )
    {
      SB_LOG_INFO_2("Finishing channel dealloc now that channel removal is complete "
                    "(client: 0x%lx) (chan: %ld)", (uint32)pClientCtxt, pChannel->channelNum );

      pChannel->eState = SB_HANDLE_AVAILABLE;
      pChannel->pClientCtxt = NULL;
    }
  }
}


/**
  @brief Perform internal reconfigure now operation
   
  This function performs an internal reconfigure now operation. 
  All operations queued by Next*() API functions will take 
  effect. 
  
  @param[in] pClientCtxt  Pointer to the client context
  
  @return  SB_SUCCESS if the reconfiguration sequence was 
           accepted, an error code on error or if the
           reconfiguration sequence was rejected
  */
static SBResult 
SlimBus_DoReconfigureNowInternal(SlimBusClientCtxt *pClientCtxt) 
{
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SBResult result = SB_SUCCESS;
  uint32 uIdx;

  do
  {
    /* Each client can only have one outstanding reconfiguration request
       at a time, error if client tries to do more than one */
    if (pClientCtxt->bReConfigPending)
    {
      /* error */
      result = SB_ERROR;
      break;
    }
    pClientCtxt->bReConfigPending = TRUE;
    pDevCtxt->uLocalReconfigPendCnt++;
    
    for (uIdx = 0; uIdx < pDevCtxt->numPorts; uIdx++)
    {
      SlimBusMasterPortType *pPort = &pDevCtxt->Ports[uIdx];
      uint32 uMask = (1<<pPort->portNum);
      uint32 numDescriptors = 1;
      if (pPort->pBamCtxt != NULL)
      {
        numDescriptors = SlimBus_BamGetIOVecActiveSlots(pPort->pBamCtxt);
      }
      if ( (uMask & pDevCtxt->uPortDisconnectMask) &&
           (pPort->pClientCtxt == pClientCtxt) )
      {
        if (( !(HAL_sb_PgdPortGetEvents(pDevCtxt->hSB, 0, pPort->portNum) & (HAL_SB_PGD_PORT_UNDERFLOW_EVENT|HAL_SB_PGD_PORT_OVERFLOW_EVENT)) )
            && (numDescriptors != 0))
        {
          pPort->uSignalDescEmptyCnt = 1;
          SlimBus_EventReset(pClientCtxt->hClientEvent);
        
          SB_LOG_INFO_1("Waiting for port descriptors to flush (port: %d)", pPort->portNum);

          SlimBus_UnlockDevice(pDevCtxt);
          result = SlimBus_EventTimedWait(pClientCtxt,
                                        pClientCtxt->hClientEvent,
                                        SB_DEFAULT_RX_TIMEOUT/SB_TIMEOUT_FACTOR);
          SlimBus_LockDevice(pDevCtxt);

          if (result == SB_SUCCESS)
          {
            SB_LOG_INFO_1("Port descriptors are flushed (port: %d)", pPort->portNum);
          }
          else
          {
            SB_LOG_WARN_1("Timeout while waiting for the port desc to be empty "
                        "(portres: 0x%lx)", pPort->handle);
            pPort->uSignalDescEmptyCnt = 0;
          }
        }
        else
        {
          SB_LOG_INFO_1("Port descriptors are flushed (port: %d)", pPort->portNum);
        }
      }
    }
    
    result = pDevCtxt->doReconfigSeqFcn(pClientCtxt);
    /* Note: for timeout, assume it will probably succeed but master 
             didn't ack the request in time */

    pClientCtxt->uPendingReconfigReqs = 0;
    
    /* Update the current channel statuses and reserved message bandwidth
       following the reconfigure now */
    for (uIdx = 0; uIdx < pDevCtxt->numChannelSlots; uIdx++)
    {
      SlimBus_UpdateChannelAfterReconfig(pClientCtxt, 
                                         pDevCtxt->pChannels[uIdx],
                                         result);
    }
    if (NULL != pClientCtxt->paSharedChannels)
    {
      for (uIdx = 0; uIdx < pDevCtxt->numSharedChannelSlots; uIdx++)
      {
        SlimBus_UpdateChannelAfterReconfig(pClientCtxt, 
                                           pClientCtxt->paSharedChannels[uIdx],
                                           result);
      }
    }
    if (SB_SUCCESS == result ||
        SLIMBUS_ERR_MSG_RESP_TIMEOUT == result)
    {
      pClientCtxt->uCurReservedMsgSlots = 
        pClientCtxt->uNewReservedMsgSlots;
    }
    else
    {
      pClientCtxt->uNewReservedMsgSlots = 
        pClientCtxt->uCurReservedMsgSlots;
    }
  
    pDevCtxt->uLocalReconfigPendCnt--;
    pClientCtxt->bReConfigPending = FALSE;
  } while (FALSE);
  
  if (SB_SUCCESS != result)
  {
    SB_LOG_ERR_1("Error during reconfigure now request (client: 0x%lx)", (uint32)pClientCtxt);
  }
  
  return result;
}

/**
  @brief Perform reconfigure now operation
   
  This function performs a reconfigure now operation.  All 
  operations queued by Next*() API functions will take effect. 
  
  @param[in] h  Client handle to the SLIMbus driver 
  
  @return  SB_SUCCESS if the reconfiguration sequence was 
           accepted, an error code on error or if the
           reconfiguration sequence was rejected
  */
SBResult 
SlimBus_DoReconfigureNow(uint32 ClientId, SlimBusHandle h) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SBResult result = SB_SUCCESS;

  SB_CHECK_IRQL();

  SlimBus_LockClient(pClientCtxt);
  SlimBus_LockDevice(pDevCtxt);

  if ( !pClientCtxt->bOpen )
  {
    SlimBus_UnlockDevice(pDevCtxt);
    SlimBus_UnlockClient(pClientCtxt);
    return SB_ERROR;
  }

  SB_LOG_VERB_1("Got reconfigure now request (client: 0x%lx)", (uint32)pClientCtxt);

  result = SlimBus_DoReconfigureNowInternal(pClientCtxt);
  
  SlimBus_UnlockDevice(pDevCtxt);
  SlimBus_UnlockClient(pClientCtxt);
  
  return result;
}

/**
  @brief Gets the status of a SLIMbus data channel
   
  This function retrives the status of a SLIMbus data 
  channel.
  
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hChannel  Data channel handle 
  @param[in] peStatus  Location to store the data channel status
  
  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_GetDataChannelStatus(uint32 ClientId, SlimBusHandle h, SlimBusResourceHandle  hChannel, SlimBusDataChannelStatusType * peStatus) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SBResult result = SB_SUCCESS;
  SlimBusChannelType *pChannel = NULL;

  SlimBus_LockDevice(pDevCtxt);

  if ( !pClientCtxt->bOpen )
  {
    SlimBus_UnlockDevice(pDevCtxt);
    return SB_ERROR;
  }

  pChannel = GetChannelHandle(pClientCtxt, hChannel, NULL);

  if (NULL != pChannel &&
      NULL != peStatus)
  {
    *peStatus = pChannel->eCurStatus;
  }
  else
  {
    result = SB_ERROR;
  }

  SlimBus_UnlockDevice(pDevCtxt);
  
  return result;
}


/**
  @brief Deallocate SLIMbus data channel internally

  This function performs internal deallocation for  a SLIMbus 
  data channel. A data channel that has not yet been removed 
  will be removed as described by 
  SlimBus_NextDataChannelControl(). 

  @param[in] pClientCtxt  Pointer to the client context
  @param[in] pDevCtxt  Pointer to the device context
  @param[in] pChannel  Pointer to the data channel structure
  
  @return  SB_SUCCESS on success, an error code on error
  */
static SBResult 
SlimBus_DeallocDataChannelInternal(uint32 ClientId, SlimBusClientCtxt *pClientCtxt, SlimBusDevCtxt *pDevCtxt, SlimBusChannelType *pChannel) 
{
  SBResult result = SB_SUCCESS;

  UNREFERENCED_PARAMETER(pDevCtxt);

  do
  {
    if ( pDevCtxt->bPendingSSR )
    {
      pChannel->eCurStatus = SLIMBUS_CHANNEL_STATUS_REMOVED;
      pChannel->eNewStatus = SLIMBUS_CHANNEL_STATUS_REMOVED;
    }
    
    /* Attempt to remove the channel if possible and it is not yet removed */
    if ( pChannel->eCurStatus != SLIMBUS_CHANNEL_STATUS_REMOVED )
    {
      uint32 uPendingReqs = pClientCtxt->uPendingReconfigReqs;
      SlimBusGroupType *pGroup = NULL;
      
      SB_LOG_WARN_2("Channel still active during dealloc, attemping removal "
                    "(res: 0x%lx) (chan: %ld)", pChannel->handle, pChannel->channelNum );

      if (NULL != pChannel->pGroup)
      {
        pGroup = pChannel->pGroup;
        pChannel = NextChannelNode(&pGroup->ResourceList, NULL);
        if (NULL == pChannel)
        {
          result = SB_ERROR;
          break;
        }
      }
      SlimBus_NextDataChannelControlInternal(pClientCtxt, pChannel, pGroup,
                                             SLIMBUS_CHAN_CTRL_REMOVE);

      if (0 == uPendingReqs)
      {
        SB_LOG_VERB_1("Generating reconfigure now request for channel dealloc "
                      "(client: 0x%lx)", (uint32)pClientCtxt);
        SlimBus_DoReconfigureNowInternal(pClientCtxt);
      }
      else
      {
        SB_LOG_WARN_2("Skipping reconfig now during channel dealloc due to "
                      "pending requests (res: 0x%lx) (chan: %ld)", pChannel->handle,
                      pChannel->channelNum );
      }
    }

    pChannel->handle = SB_HANDLE_INVALID;
    
    if ( pChannel->eCurStatus == SLIMBUS_CHANNEL_STATUS_REMOVED &&
         pChannel->eCurStatus == pChannel->eNewStatus )
    {
      pChannel->eState = SB_HANDLE_AVAILABLE;
      pChannel->pClientCtxt = NULL;
    }
    else
    {
      SB_LOG_WARN_2("Unable to complete channel removal during dealloc "
                    "(res: 0x%lx) (chan: %ld)", pChannel->handle,
                    pChannel->channelNum );

      pChannel->eState = SB_HANDLE_DEALLOCATED;
    }

    RemoveChannelNode(pChannel);
    pChannel->pGroup = NULL;

    result = SB_SUCCESS;
  } while (FALSE);

  if (SB_SUCCESS != result)
  {
    SB_LOG_ERR_2("Failed to deallocate data channel (res: 0x%lx) (chan: %ld)",
                 ((NULL!=pChannel)?pChannel->handle:NULL), 
                 ((NULL!=pChannel)?pChannel->channelNum:-1) );
  }

  return result;
}

/**
  @brief Deallocate SLIMbus data channel

  This function deallocates a SLIMbus data channel.  A data 
  channel that has not yet been removed will be removed as 
  described by SlimBus_NextDataChannelControl().

  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hChannel  Data channel handle 
  
  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_DeallocDataChannel(uint32 ClientId, SlimBusHandle h, SlimBusResourceHandle  hChannel) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SBResult result = SB_SUCCESS;
  SlimBusChannelType *pChannel;

  SB_CHECK_IRQL();

  SlimBus_LockClient(pClientCtxt);
  SlimBus_LockDevice(pDevCtxt);
  
  if ( !pClientCtxt->bOpen )
  {
    SlimBus_UnlockDevice(pDevCtxt);
    SlimBus_UnlockClient(pClientCtxt);
    return SB_ERROR;
  }

  pChannel = GetChannelHandle(pClientCtxt, hChannel, NULL);    

  if (NULL == pChannel)
  {
    result = SB_ERROR;
  }
  else
  {
    result = SlimBus_DeallocDataChannelInternal(ClientId, pClientCtxt, pDevCtxt, pChannel);
  }

  SlimBus_UnlockDevice(pDevCtxt);
  SlimBus_UnlockClient(pClientCtxt);

  return result;
}


/**
  @brief Get the FIFO status of a SLIMbus master port 
   
  This function retrieves the FIFO status of a SLIMbus master 
  port.
   
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hPort  Master port handle
  @param[in] eTransferDir  Direction of data flow for FIFO to 
        get the status for.  For bi-directional ports, there is
        one FIFO for each of the transmit and receive
        directions.
  @param[out] puFifoWordCnt  Pointer to the location to store 
        the FIFO word count
  @param[out] pbWatermarkIsHit  Pointer to the location to store
        a boolean value indicating whether the watermark has
        been hit or not

  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_GetPortFifoStatus(uint32 ClientId, SlimBusHandle h, SlimBusResourceHandle  hPort, SlimBusBamTransferType  eTransferDir, uint32 * puFifoWordCnt, SBBOOL * pbWatermarkIsHit) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  uint32 idx;
  SlimBusMasterPortType *pPort = NULL;
  SBResult result = SB_SUCCESS;

  SlimBus_LockDevice(pDevCtxt);

  if (!pClientCtxt->bOpen ||
      SLIMBUS_BAM_NUM_ENUMS <= eTransferDir )
  {
    SlimBus_UnlockDevice(pDevCtxt);
    return SB_ERROR;
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

    if (NULL != puFifoWordCnt)
    {
      *puFifoWordCnt =
        HAL_sb_PgdPortGetCurrentFifoWordCount(pDevCtxt->hSB, 0, pPort->portNum);
    }
    if (NULL != pbWatermarkIsHit)
    {
      *pbWatermarkIsHit = 
        HAL_sb_PgdPortIsWatermarkHit(pDevCtxt->hSB, 0, pPort->portNum);
    }
  } while (FALSE);

  SlimBus_UnlockDevice(pDevCtxt);

  return result;
}


/**
  @brief Register for asynchronous SLIMbus clock gear change 
         events
   
  This function registers for asynchronous SLIMbus clock gear 
  change events.  When the event object is signaled, the current 
  scheduling parameters will be passed in a 
  SlimBusSchedParamsType data structure in the callback pointer.
  Calls to this function will supersede any previous calls to 
  this function by a particular client. 
   
  @param[in] h  Client handle to the SLIMbus driver 
   
  @param[in] hEvent  Event handle to receive asynchronous port 
        event notifications
   
  @return  SB_SUCCESS on success, an error code on error
  */
SBResult
SlimBus_RegisterClockGearEvent
(
  uint32 ClientId,
  SlimBusHandle h,
  SlimBusEventHandle hEvent
)
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SBResult result = SB_SUCCESS;

  SlimBus_LockClient(pClientCtxt);

  if (!pClientCtxt->bOpen)
  {
    SlimBus_UnlockClient(pClientCtxt);
    return SB_ERROR;
  }

  pClientCtxt->hClockGearEvent = hEvent;

  SlimBus_UnlockClient(pClientCtxt);

  return result;
}

/**
  @brief Register for specific asynchronous SLIMbus master port 
         events
   
  This function registers for specific asynchronous SLIMbus 
  mater port events.  When the event object is signalled, the 
  condition that triggered the event will be available for 
  retrieval by the SlimBus_GetPortEvent() function.  Calls to 
  this function for a particular port will supercede any 
  previous calls to this function or to 
  SlimBus_RegisterPortEvent() for the port. 
   
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hPort  Master port handle
  @param[in] hEvent  Event handle to receive asynchronous port 
        event notifications
  @param[in] pUser  User pointer that will be returned in event 
        notification structure
  @param[in] eType  Type of port events to register for 

  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_RegisterPortEventEx
(
  uint32 ClientId,
  SlimBusHandle h,
  SlimBusResourceHandle  hPort,
  SlimBusEventHandle  hEvent,
  void * pUser,
  SlimBusPortEventType eType
) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  uint32 idx;
  SlimBusMasterPortType *pPort = NULL;
  SBResult result = SB_SUCCESS;

  SlimBus_LockDevice(pDevCtxt);

  if (!pClientCtxt->bOpen ||
      (SlimBusPortEventType)0 == eType )
  {
    SlimBus_UnlockDevice(pDevCtxt);
    return SB_ERROR;
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

    /* Note: For half-duplex port, only even port will be used for events */    

    pPort = &pDevCtxt->Ports[idx];
    pPort->hEvent = hEvent;
    pPort->pEventUserData = pUser;
    pPort->eEnabledEvents = eType;
  } while (FALSE);

  SlimBus_UnlockDevice(pDevCtxt);

  return result;
}


/**
  @brief Read information on recent port events
   
  This function reads information on recent asynchronous port 
  events.
   
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hPort  Master port handle; this parameter can be 
        set to NULL to get the pending events for any port owned
        by the client
  @param[out] pNotify  Pointer to event notification structure 

  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_GetPortEvent
(
  uint32 ClientId,
  SlimBusHandle h,
  SlimBusResourceHandle  hPort,
  SlimBusEventNotifyType * pNotify
) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  uint32 idx;
  SlimBusMasterPortType *pPort = NULL;
  SBResult result = SB_SUCCESS;

  SlimBus_LockDevice(pDevCtxt);

  if ( !pClientCtxt->bOpen ||
       NULL == pNotify )
  {
    SlimBus_UnlockDevice(pDevCtxt);
    return SB_ERROR;
  }

  /* Zero-out the notify structure */
  memset(pNotify, 0, sizeof(*pNotify));
  
  /* If hPort is not NULL, we get events just for the specified port; if hPort
     is NULL, we get events for one of the ports owned by the client */
  if (0 != hPort)
  {
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
  
      /* Note: For half-duplex port, only even port will be used for events */    
  
      pPort = &pDevCtxt->Ports[idx];
      pNotify->eEvent = pPort->eActiveEvents;
      pNotify->hPort = hPort;
      pNotify->pUser = pPort->pEventUserData;
      
      pPort->eActiveEvents = SLIMBUS_EVENT_O_NONE;
    } while (FALSE);
  }
  else
  {
    result = SB_SUCCESS;
    pNotify->eEvent = SLIMBUS_EVENT_O_NONE;
    pNotify->hPort = 0;
    pNotify->pUser = NULL;
    
    for ( idx = 0; idx < pDevCtxt->numPorts; idx++ )
    {
      pPort = &pDevCtxt->Ports[idx];
      if ( pPort->pClientCtxt == pClientCtxt &&
           pPort->eActiveEvents != SLIMBUS_EVENT_O_NONE )
      {
        pNotify->eEvent = pPort->eActiveEvents;
        pNotify->hPort = pPort->handle;
        pNotify->pUser = pPort->pEventUserData;

        pPort->eActiveEvents = SLIMBUS_EVENT_O_NONE;
        break;
      }
    }
  }

  SlimBus_UnlockDevice(pDevCtxt);

  return result;
}


/**
  @brief Get the index of a SLIMbus master port 
   
  This function retrieves the index of a SLIMbus master port. 
   
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hPort  Master port handle
  @param[out] puPgdIndex  Pointer to the location to store the
        PGD index
  @param[out] puPortIndex  Pointer to the location to store the 
        PGD index

  @return  SB_SUCCESS on success, an error code on error
  */
SBResult
SlimBus_GetPortIndex(SlimBusHandle h, SlimBusResourceHandle  hPort, uint32 * puPgdIndex, uint32 * puPortIndex)
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SBResult result = SB_SUCCESS;
  uint32 idx;

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
    
    if (NULL != puPgdIndex)
    {
      *puPgdIndex = 0;
    }
    
    if (NULL != puPortIndex)
    {
      *puPortIndex = pDevCtxt->Ports[idx].portNum;
    }
  } while (FALSE);
  
  return result;
}


/**
  @brief Allocate a port progress counter.
   
  This function allocates a progress counter for a SLIMbus 
  master port.  Progress counters are typically used for audio 
  sync applications. 
   
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hPort  Master port handle that the progress counter 
        should be associated with
  @param[in] eTransferDir  Direction of data flow for the BAM 
        pipe to allocate the progress counter for.  For
        bi-directional ports, there is one BAM pipe for each of
        the transmit and receive directions.
  @param[out] phCounter  Pointer to location to store allocated
        progress counter handle

  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_AllocProgressCounter(uint32 ClientId, SlimBusHandle h, SlimBusResourceHandle  hPort, SlimBusBamTransferType  eTransferDir, SlimBusResourceHandle * phCounter) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SBResult result = SLIMBUS_ERR_OUT_OF_RESOURCES;
  SlimBusMasterPortType *pPort = NULL;
  SlimBusProgressCounterType *pCounter = NULL;  
  uint32 idx;
  uint32 i;

  if ( !pClientCtxt->bOpen ||
       NULL == phCounter )
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
    if (NULL != pPort->pCounter)
    {
      result = SB_ERROR;
      break;
    }
    
    if ( HAL_sb_IsPortDmaIntSupported(pDevCtxt->hSB) )
    {
      i = idx;
      if ( i < pDevCtxt->numCounterSlots &&
           i < sizeof(pDevCtxt->Counters)/sizeof(pDevCtxt->Counters[0]) )
      {
        pCounter = &pDevCtxt->Counters[i];
        if ( pCounter->eState == SB_HANDLE_AVAILABLE &&
             pCounter->counterNum == pPort->portNum )
        {
          result = SB_SUCCESS;
        }
        else
        {
          SB_LOG_ERR_3("Counter slot %d not available for port %d and counter %d", i, pPort->portNum, pCounter->counterNum);
        }
      }
      else
      {
        SB_LOG_ERR_2("Counter slot %d out of range, total %d", i, pDevCtxt->numCounterSlots);
      }
    }
    else
    {
      for (i = 0; i < (int)pDevCtxt->numCounterSlots && i < sizeof(pDevCtxt->Counters)/sizeof(pDevCtxt->Counters[0]); i++)
      {
        pCounter = &pDevCtxt->Counters[i];
        if (pCounter->eState == SB_HANDLE_AVAILABLE)
        {
          result = SB_SUCCESS;
          break;
        }
      }
    }
    
    if ( SB_SUCCESS == result && NULL != pCounter )
    {
      SlimBusResourceHandle handle =
        SB_SET_HANDLE_TYPE(SB_HANDLE_TYPE_PROGRESS_COUNTER) |
        SB_SET_HANDLE_PRN(GetPrn(pDevCtxt)) |
        SB_SET_HANDLE_IDX(i);
      pCounter->eState = SB_HANDLE_ALLOCATED;
      pCounter->pClientCtxt = pClientCtxt;
      pCounter->pPort = pPort;
      pCounter->handle = handle;
      pPort->pCounter = pCounter;
      *phCounter = handle;
  
      HAL_sb_PgdPcSetPortNum(pDevCtxt->hSB, 0, 
                             pCounter->counterNum, pPort->portNum);
    }
    else
    {
      SB_LOG_ERR_0("Failed to allocate progress counter");
    }
  } while (FALSE);

  SlimBus_UnlockDevice(pDevCtxt);

  return result;
}


/**
  @brief Deallocate a port progress counter.
   
  This function deallocates a progress counter for a SLIMbus 
  master port.  Progress counters are typically used for audio 
  sync applications. 
   
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hCounter  Progress Counter handle

  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_DeallocProgressCounter(uint32 ClientId, SlimBusHandle h, SlimBusResourceHandle  hCounter) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SBResult result = SB_ERROR;

  if ( !pClientCtxt->bOpen )
  {
    return SB_ERROR;
  }

  SlimBus_LockDevice(pDevCtxt);

  do
  {
    uint32 idx = SB_GET_HANDLE_IDX(hCounter);

    if (idx < pDevCtxt->numCounterSlots && 
        idx < sizeof(pDevCtxt->Counters)/sizeof(pDevCtxt->Counters[0]) &&
        pDevCtxt->Counters[idx].handle == hCounter)
    {
      SlimBusProgressCounterType *pCounter = &pDevCtxt->Counters[idx];
      
      pCounter->handle = SB_HANDLE_INVALID;
      pCounter->eState = SB_HANDLE_AVAILABLE;
      pCounter->pClientCtxt = NULL;
      if (NULL != pCounter->pPort)
      {
        pCounter->pPort->pCounter = NULL;
        pCounter->pPort = NULL;
      }
      result = SB_SUCCESS;
    }
  } while (FALSE);

  SlimBus_UnlockDevice(pDevCtxt);

  return result;
}


/**
  @brief Initialize the value of a port progress counter.
   
  This function initializes the value of a SLIMbus master port 
  progress counter.
   
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hCounter  Progress Counter handle
  @param[in] uCount  Value to initialize the progress counter 
   
  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_InitProgressCounter(uint32 ClientId, SlimBusHandle h, SlimBusResourceHandle  hCounter, uint32  uCount) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SBResult result = SB_ERROR;

  if ( !pClientCtxt->bOpen )
  {
    return SB_ERROR;
  }

  SlimBus_LockDevice(pDevCtxt);

  do
  {
    uint32 idx = SB_GET_HANDLE_IDX(hCounter);

    if (idx < pDevCtxt->numCounterSlots && 
        idx < sizeof(pDevCtxt->Counters)/sizeof(pDevCtxt->Counters[0]) &&
        pDevCtxt->Counters[idx].handle == hCounter)
    {
      SlimBusProgressCounterType *pCounter = &pDevCtxt->Counters[idx];

      HAL_sb_PgdPcSetAndLoadInitVal(pDevCtxt->hSB, 0, 
                                    pCounter->counterNum, uCount);
      
      result = SB_SUCCESS;
    }
  } while (FALSE);

  SlimBus_UnlockDevice(pDevCtxt);

  return result;
}


/**
  @brief Read the value of a port progress counter. 
   
  This function reads the value of a SLIMbus master port 
  progress counter.  Either the free running counter or the VFR 
  counter can be read. 
   
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hCounter  Progress Counter handle 
  @param[in] eType  The type of counter to read, either the free 
        running counter or the VFR counter
  @param[out] puNumDMA  Pointer to store the number of 32-bit 
        words DMA-ed to or from the associated port
  @param[out] puFifoSamples  Number of samples currently in the 
        port FIFO plus and samples in the phy/FL stages,
        taking into account segment length and bit packing
   
  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_ReadProgressCounter(uint32 ClientId, SlimBusHandle h, SlimBusResourceHandle  hCounter, SlimBusCounterType  eType, uint32 * puNumDMA, uint32 * puFifoSamples) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SBResult result = SB_ERROR;

  if ( !pClientCtxt->bOpen ||
       NULL == puNumDMA ||
       NULL == puFifoSamples )
  {
    return SB_ERROR;
  }

  // Note: no synchronization here so that the API can be called with low latency

  do
  {
    uint32 idx = SB_GET_HANDLE_IDX(hCounter);

    if (idx < pDevCtxt->numCounterSlots && 
        idx < sizeof(pDevCtxt->Counters)/sizeof(pDevCtxt->Counters[0]) &&
        pDevCtxt->Counters[idx].handle == hCounter)
    {
      SlimBusProgressCounterType *pCounter = &pDevCtxt->Counters[idx];
      SlimBusMasterPortType *pPort = pCounter->pPort;

      if ( SLIMBUS_COUNTER_FREE_RUN == eType )
      {
        HAL_sb_PgdPcGetVal(pDevCtxt->hSB, 0, pCounter->counterNum, 
                           puNumDMA, puFifoSamples);
      }
      else if ( SLIMBUS_COUNTER_VFR == eType )
      {
        HAL_sb_PgdPcGetValOnVfrTs(pDevCtxt->hSB, 0, pCounter->counterNum, 
                                  puNumDMA, puFifoSamples);
      }
      else
      {
        if ( !HAL_sb_IsShadowProgressCounterSupported(pDevCtxt->hSB) )
        {
          result = SB_ERROR;
          break;
        }
        HAL_sb_PgdPcGetShadowVal(pDevCtxt->hSB, 0, pCounter->counterNum, 
                                 puNumDMA, puFifoSamples);
      }

      if (pDevCtxt->eWrkrnd & SB_WRKRND_O_PC_FIFO_SAMPLE_OFFSET)
      {
        if (NULL != pPort &&
            SLIMBUS_PORT_SINK_FLOW == pPort->eFlow &&
            ( pPort->eOptions & SLIMBUS_PORT_O_PACKED ) )
        {
          (*puFifoSamples)--;
        }
      }
      
      result = SB_SUCCESS;
    }
  } while (FALSE);

  // Note: no synchronization here so that the API can be called with low latency

  return result;
}


/**
  @brief Read the value of a port shadow progress counter and 
         timestamp.
 
  This function reads the value of a SLIMbus master port shadow 
  progress counter and timestamp.
  
  @param[in] _h  Client handle to the SLIMbus driver
   
  @param[in] hCounter  Progress Counter handle
   
  @param[out] puNumDMA  Pointer to store the number of 32-bit 
        words DMA-ed to or from the associated port at the last
        DMA interrupt
   
  @param[out] puFifoSamples  Number of samples that were in the 
        port FIFO plus samples in the phy/FL stages, taking
        into account segment length and bit packing, at the
        last DMA interrupt
   
  @param[out] puTimestamp  Pointer to store the timestamp value
        for the last DMA interrupt
   
  @param[out] pbSampleMissed  Pointer to store indication of 
        whether and DMA timestamp values have been missed
  
  @return SB_SUCCESS if the timestamp sample and counter values
          are valid, error code otherwise
   
  @see SlimBus_ReadProgressCounter() 
 */
SBResult
SlimBus_ReadProgressCounterTimestamp(uint32 ClientId, SlimBusHandle h, SlimBusResourceHandle hCounter, uint32 *puNumDMA, uint32 *puFifoSamples, uint64 *puTimestamp, SBBOOL *pbSampleMissed)
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SBResult result = SB_ERROR;
  SBBOOL bSampleMissed = FALSE, bSampleValid = TRUE;

  if ( !pClientCtxt->bOpen ||
       NULL == puTimestamp ||
       !HAL_sb_IsShadowProgressCounterSupported(pDevCtxt->hSB) )
  {
    return SB_ERROR;
  }

  // Note: no synchronization here so that the API can be called with low latency

  do
  {
    uint32 idx = SB_GET_HANDLE_IDX(hCounter);

    if (idx < pDevCtxt->numCounterSlots && 
        idx < sizeof(pDevCtxt->Counters)/sizeof(pDevCtxt->Counters[0]) &&
        pDevCtxt->Counters[idx].handle == hCounter)
    {
      SlimBusProgressCounterType *pCounter = &pDevCtxt->Counters[idx];
      SlimBusMasterPortType *pPort = pCounter->pPort;

      HAL_sb_PgdPcGetTimestamp(pDevCtxt->hSB, 0, pCounter->counterNum, 
                               puTimestamp, &bSampleValid, &bSampleMissed);
      if ( NULL != puNumDMA && NULL != puFifoSamples )
      {
        HAL_sb_PgdPcGetShadowVal(pDevCtxt->hSB, 0, pCounter->counterNum, 
                                 puNumDMA, puFifoSamples);

        if (pDevCtxt->eWrkrnd & SB_WRKRND_O_PC_FIFO_SAMPLE_OFFSET)
        {
          if (NULL != pPort &&
              SLIMBUS_PORT_SINK_FLOW == pPort->eFlow &&
              ( pPort->eOptions & SLIMBUS_PORT_O_PACKED ) )
          {
            (*puFifoSamples)--;
          }
        }
      }
      
      if ( bSampleValid )
      {
        result = SB_SUCCESS;
      }
      if ( NULL != pbSampleMissed )
      {
        *pbSampleMissed = bSampleMissed;
      }
    }
  } while (FALSE);

  // Note: no synchronization here so that the API can be called with low latency

  return result;
}


/**
  @brief Read the status of whether a progress counter VFR 
         interrupt pulse has occurred.
   
  This function reads the status of whether a progress counter 
  VFR interrupt pulse has occurred.  The status is cleared by 
  the SlimBus_ClearProgressCounterVFRStatus() function.
   
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hCounter  Progress Counter handle 
  @param[out] pbSet  Pointer to store boolean indicating whether
        VFR interrupt pulse has occurred
   
  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_ReadProgressCounterVFRStatus(uint32 ClientId, SlimBusHandle h, SlimBusResourceHandle  hCounter, SBBOOL * pbSet) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SBResult result = SB_ERROR;

  if ( !pClientCtxt->bOpen ||
       NULL == pbSet )
  {
    return SB_ERROR;
  }

  SlimBus_LockDevice(pDevCtxt);

  do
  {
    uint32 idx = SB_GET_HANDLE_IDX(hCounter);

    if (idx < pDevCtxt->numCounterSlots && 
        idx < sizeof(pDevCtxt->Counters)/sizeof(pDevCtxt->Counters[0]) &&
        pDevCtxt->Counters[idx].handle == hCounter)
    {
      SlimBusProgressCounterType *pCounter = &pDevCtxt->Counters[idx];

      *pbSet = HAL_sb_PgdPcIsVfrIntSet(pDevCtxt->hSB, 0, pCounter->counterNum);

      result = SB_SUCCESS;
    }
  } while (FALSE);

  SlimBus_UnlockDevice(pDevCtxt);

  return result;
}


/**
  @brief Clear the progress counter VFR interrupt pulse status. 
   
  This function clears the progress counter VFR interrupt pulse 
  status.
   
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hCounter  Progress Counter handle 
   
  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_ClearProgressCounterVFRStatus(uint32 ClientId, SlimBusHandle h, SlimBusResourceHandle  hCounter) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SBResult result = SB_ERROR;

  if ( !pClientCtxt->bOpen )
  {
    return SB_ERROR;
  }

  SlimBus_LockDevice(pDevCtxt);

  do
  {
    uint32 idx = SB_GET_HANDLE_IDX(hCounter);

    if (idx < pDevCtxt->numCounterSlots && 
        idx < sizeof(pDevCtxt->Counters)/sizeof(pDevCtxt->Counters[0]) &&
        pDevCtxt->Counters[idx].handle == hCounter)
    {
      SlimBusProgressCounterType *pCounter = &pDevCtxt->Counters[idx];

      HAL_sb_PgdPcClearVfrInt(pDevCtxt->hSB, 0, pCounter->counterNum);

      result = SB_SUCCESS;
    }
  } while (FALSE);

  SlimBus_UnlockDevice(pDevCtxt);

  return result;
}

/**
  @brief Allocate a resource group.
   
  This function allocates a resource group used to manipulate
  multiple resources at the same time. 
   
  Resource groups are currently supported by the following 
  functions: SlimBus_NextDefineDataChannel(), 
  SlimBus_NextDataChannelControl() 
   
  @param[in] h  Client handle to the SLIMbus driver 
  @param[out] phGroup  Pointer to location to store allocated 
        resource group handle
   
  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_AllocResourceGroup(uint32 ClientId, SlimBusHandle h, SlimBusResourceHandle * phGroup) 
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SBResult result = SLIMBUS_ERR_OUT_OF_RESOURCES;
  int i;

  SlimBus_LockDevice(pDevCtxt);

  if (!pClientCtxt->bOpen ||
      NULL == phGroup)
  {
    SlimBus_UnlockDevice(pDevCtxt);
    return SB_ERROR;
  }

  for (i = 0; i < (int)pDevCtxt->numGroupSlots && i < SB_MAX_GROUPS; i++) 
  {
    if (pDevCtxt->Groups[i].handle == SB_HANDLE_INVALID)
    {
      SlimBusResourceHandle handle =
        SB_SET_HANDLE_TYPE(SB_HANDLE_TYPE_GROUP) |
        SB_SET_HANDLE_PRN(GetPrn(pDevCtxt)) |
        SB_SET_HANDLE_IDX(i);
      pDevCtxt->Groups[i].handle = handle;
      *phGroup = handle;

      result = SB_SUCCESS;
      break;
    }
  }

  SlimBus_UnlockDevice(pDevCtxt);

  return result;
}


/**
  @brief Deallocate a resource group.
   
  This function deallocates a resource group used to manipulate
  multiple resources at the same time. 
   
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hGroup  resource group handle
   
  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_DeallocResourceGroup(uint32 ClientId, SlimBusHandle h, SlimBusResourceHandle  hGroup) 
{
   SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
   SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
   SlimBusGroupType *pGroup = NULL;
   SlimBusChannelType *pChannel = NULL;
   SBResult result = SB_SUCCESS;
   int idx;

   SlimBus_LockDevice(pDevCtxt);

   if ( !pClientCtxt->bOpen )
   {
     SlimBus_UnlockDevice(pDevCtxt);
     return SB_ERROR;
   }

   do
   {
     idx = SB_GET_HANDLE_IDX(hGroup);
     if (idx < (int)pDevCtxt->numGroupSlots &&
         idx < sizeof(pDevCtxt->Groups)/sizeof(pDevCtxt->Groups[0]) &&
         pDevCtxt->Groups[idx].handle == hGroup)
     {
       pGroup = &pDevCtxt->Groups[idx];
     }
     else
     {
       result = SB_ERROR;
       break;
     }
     while (NULL != (pChannel = PopHeadChannel(&pGroup->ResourceList)))
     {
       pChannel->pGroup = NULL;
     }
     pGroup->handle = SB_HANDLE_INVALID;
   } while (FALSE);
   
   SlimBus_UnlockDevice(pDevCtxt);
   
   return result;
}


/**
  @brief Add a resource to a resource group.
   
  This function adds a resource to a resource group.  All 
  resources within a group must be of the same type.  A resource 
  is limited to belonging to a single group at a time.
   
  Currently only data channel resources can
  be added to a resource group. 
   
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hGroup  resource group handle 
  @param[in] hResource  resource handle to add to the group 
   
  @return  SB_SUCCESS on success, an error code on error
  */
SBResult 
SlimBus_AddResourceToGroup(uint32 ClientId, SlimBusHandle h, SlimBusResourceHandle  hGroup, SlimBusResourceHandle  hResource) 
{
   SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
   SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
   SlimBusGroupType *pGroup = NULL;
   SlimBusChannelType *pChannel;
   SBResult result = SB_SUCCESS;
   uint32 uIdx;

   SlimBus_LockDevice(pDevCtxt);

   if ( !pClientCtxt->bOpen )
   {
     SlimBus_UnlockDevice(pDevCtxt);
     return SB_ERROR;
   }

   do
   {
     uIdx = SB_GET_HANDLE_IDX(hGroup);
     if (uIdx < pDevCtxt->numGroupSlots &&
         uIdx < sizeof(pDevCtxt->Groups)/sizeof(pDevCtxt->Groups[0]) &&
         pDevCtxt->Groups[uIdx].handle == hGroup)
     {
       pGroup = &pDevCtxt->Groups[uIdx];
     }
     else
     {
       result = SB_ERROR;
       break;
     }
     
     pChannel = GetChannelHandle(pClientCtxt, hResource, NULL);

     if (NULL != pChannel)
     {
       if (NULL != pChannel->GroupNode.pNext)
       {
         result = SB_ERROR;
         break;
       }
       pChannel->pGroup = pGroup;
       PushTailChannel(&pGroup->ResourceList, pChannel);
     }
     else
     {
       result = SB_ERROR;
       break;
     }
   } while (FALSE);

   SlimBus_UnlockDevice(pDevCtxt);

   return result;
}

/**
 * @brief Check the external framer state
 *
 * This function checks the framer state and performs recovery
 * if the external framer is expected to be active but the clock
 * is not detected. 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 */
void SlimBus_CheckFramerStateInternal(SlimBusDevCtxt *pDevCtxt)
{
  if ( SB_MASTER_DRIVER == pDevCtxt->eDrvType &&
       pDevCtxt->bClockIsOn &&
       !pDevCtxt->bActiveFramer &&
       !pDevCtxt->bPendingSSR )
  {
    if ( !SlimBus_CheckGpioClockToggle(pDevCtxt) )
    {
      SB_LOG_WARN_0("Detected loss of external framer, triggering SSR recovery");
      
      SlimBus_SSReset(pDevCtxt);
      SlimBus_SSRestart(pDevCtxt);
    }
  }
}


/**
 * @brief Detach from the device context
 *
 * This function detaches the client from the device context
 * 
 * @param[in] h Client handle to the device context
 * 
 * @return Reference count for the device context
 */
uint32 SlimBus_DeviceDetach(uint32 ClientId, SlimBusHandle h)
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  uint32 dwref = 0;
  SlimBusEventNodeType *pEvent, *pNextEvent;
  uint32 uIdx;
  
  SlimBus_LockClient(pClientCtxt);
  SlimBus_LockDevice(pDevCtxt);
  
  SB_LOG_INFO_2("Client detaching from device (client: 0x%lx) (device: 0x%lx)",
                (uint32)pClientCtxt, (uint32)pDevCtxt);

  if ( !pDevCtxt->bPendingSSR )
  {
    /* If the client still has any pending reconfig requests, send them out now */
    if (pClientCtxt->uPendingReconfigReqs > 0)
    {
      SB_LOG_VERB_1("Generating reconfigure now request (client: 0x%lx)",
                    (uint32)pClientCtxt);
      SlimBus_DoReconfigureNowInternal(pClientCtxt);
    }
  }

  /* Make sure all the channels associated with this client are deallocated */
  for (uIdx = 0; uIdx < pDevCtxt->numChannelSlots; uIdx++)
  {
    SlimBusChannelType *pChannel = pDevCtxt->pChannels[uIdx];
    SlimBusEndPointType *pEndPoint = NULL;

    if ( NULL != pChannel &&
         pClientCtxt == pChannel->pClientCtxt )
    {
      if ( SB_HANDLE_ALLOCATED == pChannel->eState )
      {
        SlimBus_DeallocDataChannelInternal(ClientId, pClientCtxt, pDevCtxt, pChannel);
      }

      pEndPoint = PeekHeadEndPoint(&pChannel->EndPointList);
      while (NULL != pEndPoint)
      {
        SlimBusEndPointType *pNextEndPoint =
          NextEndPointNode(&pChannel->EndPointList, pEndPoint);

        /* If channel is removed but port is still connected, it means that
           the reconfiguration sequence hasn't completed yet.  If this is
           the case it is not a problem, just set the pointers to NULL. */
        pEndPoint->pChannel = NULL;
        RemoveEndPointNode(pEndPoint);
        pEndPoint = pNextEndPoint;
      }
      pChannel->pClientCtxt = NULL;
    }
  }

  /* Make sure all the shared channels associated with this client are deallocated */
  if (NULL != pClientCtxt->paSharedChannels)
  {
    for (uIdx = 0; uIdx < pDevCtxt->numSharedChannelSlots; uIdx++)
    {
      SlimBusChannelType *pChannel = pClientCtxt->paSharedChannels[uIdx];
      SlimBusEndPointType *pEndPoint = NULL;

      if ( NULL != pChannel )
      {
        if ( SB_HANDLE_ALLOCATED == pChannel->eState )
        {
          SlimBus_DeallocDataChannelInternal(ClientId, pClientCtxt, pDevCtxt, pChannel);
        }
        pEndPoint = PeekHeadEndPoint(&pChannel->EndPointList);
        while (NULL != pEndPoint)
        {
          SlimBusEndPointType *pNextEndPoint =
            NextEndPointNode(&pChannel->EndPointList, pEndPoint);

          /* If channel is removed but port is still connected, it means that
             the reconfiguration sequence hasn't completed yet.  If this is
             the case it is not a problem, just set the pointers to NULL. */
          pEndPoint->pChannel = NULL;
          RemoveEndPointNode(pEndPoint);
          pEndPoint = pNextEndPoint;
        }
        pChannel->pClientCtxt = NULL;
        SlimBus_Free(pChannel);
        pClientCtxt->paSharedChannels[uIdx] = NULL;
      }
    }
    SlimBus_Free(pClientCtxt->paSharedChannels);
    pClientCtxt->paSharedChannels = NULL;
  }
  
  /* Make sure all the ports associated with this client are deallocated */
  for (uIdx = 0; uIdx < pDevCtxt->numPorts; uIdx++)
  {
    SlimBusMasterPortType *pPort = &pDevCtxt->Ports[uIdx];

    if ( pClientCtxt == pPort->pClientCtxt &&
         SB_HANDLE_ALLOCATED == pPort->eState )
    {
      SlimBus_DeallocPort(pClientCtxt, pPort);
    }
  }
    
  /* Make sure all the Tx events are removed from the device event queue */
  pEvent = PeekHeadEvent(&pDevCtxt->TxEventQueue);
  while (NULL != pEvent)
  {
    pNextEvent = NextEventNode(&pDevCtxt->TxEventQueue, pEvent);
    
    if (pEvent->pFreeRoot == &pClientCtxt->FreeEventStack)
    {
      RemoveEventNode(pEvent);
    }
    pEvent = pNextEvent;
  }
      
  /* Make sure all the Rx events are removed from the device event queue */
  pEvent = PeekHeadEvent(&pDevCtxt->RxEventQueue);
  while (NULL != pEvent)
  {
    uint8 TID, LA;
    SlimBusDeviceType *pDevice;
    pNextEvent = NextEventNode(&pDevCtxt->RxEventQueue, pEvent);

    if (pEvent->pFreeRoot == &pClientCtxt->FreeEventStack)
    {
      LA = SB_BF_GET(pEvent->uRxMarker,RXMARKER,LA);
      TID = SB_BF_GET(pEvent->uRxMarker,RXMARKER,TID);
      pDevice = pDevCtxt->pDevices[LA&SB_LA_IDX_MASK];
      if ( NULL != pDevice &&
           LA != pDevice->LA )
      {
        pDevice = NULL;
      }
      pEvent->bSyncWait = FALSE;
      SlimBus_CleanupMsgEvent(pDevice, TID, pEvent, pEvent->pFreeRoot);
    }
    pEvent = pNextEvent;
  }

  /* TODO: do we need to go through and dealloc any resources the 
           client has allocated? */
  if (NULL != pClientCtxt->hClientEvent)
  { 
    SlimBus_DestroyEvent(pClientCtxt->hClientEvent);
    pClientCtxt->hClientEvent = NULL;
  }
  
  if (NULL != pClientCtxt->hTimeoutEvent)
  {
    SlimBus_DestroyEvent(pClientCtxt->hTimeoutEvent);
    pClientCtxt->hTimeoutEvent = NULL;
  }
 
  if (NULL != pClientCtxt->BasicClient.ConfigMsgFifo.pBuf)
  {
    SlimBus_Free(pClientCtxt->BasicClient.ConfigMsgFifo.pBuf);
    pClientCtxt->BasicClient.ConfigMsgFifo.pBuf = NULL;
  }
  if (pClientCtxt->BasicClient.uClientNum < pDevCtxt->numClientSlots)
  {
    pDevCtxt->paClientCtxt[pClientCtxt->BasicClient.uClientNum] = NULL;
  }
  
  if (NULL != pClientCtxt->hBatchMem)
  {
    SlimBus_DestroyObject(pClientCtxt->hBatchMem);
    pClientCtxt->hBatchMem = NULL;
  }
  
  SlimBus_UnlockDevice(pDevCtxt);
  SlimBus_UnlockClient(pClientCtxt);

  return dwref;
}

