/*
===========================================================================

FILE:         SlimBusMaster.c

DESCRIPTION:  This file implements the SLIMbus core driver master functionality.

===========================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/buses/slimbus/src/common/SlimBusMaster.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
02/01/18   MJS     Use separate hSchedEvent and hReconfigEvent.
10/06/17   MJS     Updated port connection tracking for multi-endpoint. 
07/07/17   MJS     Always reset core when leaving pause clock with retention
                   disabled.
06/27/17   MJS     Workaround for issue exiting idle mode.
06/13/17   MJS     Fix issue where Slimbus framer is enabled but not booted
                   following CX collapse.
06/01/17   RC      Logic added to set the dirty bit to sync port to apply
                   SW workaround for the HWBUG QCTDD03351991.
05/25/17   MJS     Add support to disable retention.
11/29/16   MJS     Porting HLOS compatibility changes.
11/16/16   MJS     Attempt recovery if hardware fails to leave idle mode.
11/16/16   MJS     Support for high-bandwidth DSD128 concurrency scheduling.
10/03/16   MJS     Support for full secondary line bandwidth scheduling.
08/11/16   MJS     Updated clock gear callback to support before/after.
07/07/16   MJS     Interrupt enablement changes for HLOS compatibility.
05/10/16   MJS     Add clock gear callback support.
04/14/16   MJS     Fix for retention support.
03/24/16   MJS     Retention support.
02/23/16   MJS     Convert master wait events to timed waits.
02/19/16   MJS     Fix for satellite source port tracking.
02/07/16   MJS     Modify algorithm to schedule constrained channels first.
02/05/16   MJS     Add support for multiple endpoints per port.
12/07/15   MJS     Don't send out reconfiguration sequences that would be empty.
12/07/15   MJS     Multi-data line channel scheduling optimization.
12/01/15   SG      Fixed memory leak issue on successive SINK connect 
                   statellite message request.
11/24/15   MJS     Subtract current channel bandwidth when assigning data lines.
08/25/15   MJS     Cleanup, remove unnecessary RemoveClientNode() calls.
08/21/15   NSD     Remove port disconnection enhancement feature, fix memory leaks.
08/14/15   NSD     Add NULL pointer check for pDestDevice.
08/14/15   MJS     Use hardware enumeration flag when calling StopIdle/ResumeIdle.
07/27/15   NSD     Add a NULL pointer check for pBasicClient in ProcessReconfigSeq
                   and fix code alignment.
06/22/15   NSD     Add support for dynamic data line assignment to all channels.
06/08/15   NSD     Replace SlimBus_EventWait() calls with SlimBus_EventTimedWait().
05/27/15   NSD     Update the data line assignment algorithm for channels and 
                   correct a spelling mistake in a framer log message.
05/13/15   NSD     Copy logic to check if data slots are scheduled before we check
                   if any data slots are used and change the clock gear value to
                   default value.
04/28/15   NSD     Add support for no data line activity interrupt on 
                   interface device.
04/22/15   NSD     Add support to enable frame sync shift feature.
03/23/15   MJS     Fixes for second data line support.
03/18/15   MJS     Fix for iterating over list while assigning data lines.
03/18/15   MJS     Add delay when retrying NACKed satellite messages.
01/07/15   mq      Cleared ISR status during SSR
11/18/14   MJS     Perform dummy read after POR for hardware workaround.
09/09/14   mq      Multi-dataline support.
09/08/14   MJS     Use alternate implementation for low power mode with
                   inactive framer.
09/03/14   MJS     Fix for race condition while checking external framer
                   satellite.
08/14/14   MJS     Fix for infinite loop when inconsistent channel params.
08/06/14   MJS     Fix for master power up/down notification logic.
06/17/14   MJS     Add support for minimum clock gear.
06/17/14   MJS     Send master power up/down notification to satellites.
05/29/14   MJS     Add workaround to limit clock drift due to gear changes.
05/22/14   MJS     Add workaround to phase shift frame sync after timeout.
05/07/14   MJS     Don't send master capability message until NGD enumerated.
05/06/14   MJS     Send master capability message after assign logical address
                   messages.
04/30/14   MJS     Fix for crash after client timeout waiting for enumeration.
04/09/14   MJS     Block client open calls until hardware is enumerated.
04/09/14   MJS     Ensure clock stays on while master workloop is active.
04/09/14   MJS     Fix logging for handover to second external framer.
03/26/14   MJS     Purge satellite channels for non-responsive satellite.
03/19/14   MJS     Enhanced support for MDM restart.
03/12/14   MJS     Support framer handover to second external framer.
01/10/14   MJS     Support for handling unexpected satellite messages.
12/04/13   MJS     Support for purging any active channels during ADSP SSR.
11/07/13   MJS     Support for MDM restart when master is inactive framer.
10/29/13   MJS     Store the clock gear being scheduled.
07/26/13   MJS     Don't purge satellite channels when powering hardware
                   back up after framer handover.
07/25/13   MJS     Perform logical addresses assignment from worker thread.
06/26/13   MJS     Use current subframe mode when scheduling is skipped.
05/06/13   MJS     Honor messaging bandwidth requests without data channels.
04/30/13   MJS     Support for framer handover.
04/23/13   MJS     Extra validation for channel ownership bitmasks.
04/22/13   MJS     Pushed/pulled protocol support.
04/22/13   MJS     Fix for channel status tracking when reconfiguration fails.
04/16/13   MJS     Support for retrying satellite ack responses.
04/12/13   dj      Updated for SLIMBUS_CLOCK_ON & SLIMBUS_CLOCK_OFF to return code
03/21/13   MJS     Support for inactive framer.
03/21/13   MJS     Support for standalone satellite hardware configuration.
03/21/13   MJS     Track channel definitions on per-client basis.
03/21/13   MJS     Cleanup low power and pause clock logic.
03/11/13   MJS     Don't perform satellite channel purge unless necessary.
03/11/13   MJS     Ensure clock stays active until ISR/DPC is exited.
03/04/13   MJS     Ensure linked list bounds aren't exceeded during remove 
                   channel param.
03/04/13   MJS     Refactor HAL component reset workaround.
02/14/13   MJS     Vote for voltage based on new clock gear prior to increase.
01/09/12   MJS     Support more than 32 channels using dynamic allocation.
12/17/12   MJS     Refactor for satellite support for hardware power collapse.
12/09/12   MJS     Perform pause clock from worker thread to allow recovery
                   from message nack during reconfiguration.
11/29/12   MJS     Move ResetMsgFifo to common source file.
11/14/12   MJS     Refactor open/close reference counting.
11/08/12   MJS     Reset the Slimbus hardware twice during hardware init.
10/26/12   MJS     Add interrupt disable function.
10/25/12   MJS     Support for voltage control and role reversal.
09/21/12   MJS     Add interrupt enable function.
09/12/12   MJS     Add port disconnect and BAM reset to power management.
08/29/12   MJS     Extend pause clock recovery window when master reports
                   loss of bus synchronization.
07/24/12   MJS     Fix race condition for two back-to-back satellite
                   reconfiguration sequences.
07/24/12   MJS     Updated thread priorities for QNX.
06/13/12   MJS     Support for standlone satellite low-power management.
05/31/12   MJS     Allow duplicate channel definition for non-shared channels
                   when parameters don't change, for error condition recovery.
05/25/12   MJS     Strip out pause clock recovery and idle timer.
05/21/12   MJS     Fix Klocwork issues.
05/16/12   MJS     Update logging macros.
04/17/12   MJS     Enhanced mempool tagging.
04/09/12   MJS     Fix SAL warnings.
04/04/12   MJS     Fixes for level 4 warnings.
03/28/12   MJS     Compatibility with WPP logging.
03/15/12   MJS     Prevent entering pause clock when Rx event is pending.
03/07/12   MJS     Fix compiler warnings.
02/27/12   MJS     Support event wait timeout/failure.
02/22/12   MJS     Don't require idle mode exited interrupt when exiting
                   clock pause immediately after reconfiguration done.
02/10/12   MJS     Fix clock gear used for calculating define channel message.
02/07/12   MJS     Unwind reconfigure sequence on any error.
02/07/12   MJS     Add reconfiguration sequence timeout mechanism.
02/02/12   MJS     Fix klocwork issues.
01/25/12   MJS     Improve BAM driver abstraction.
01/23/12   MJS     Support for entering pause clock only after idle timeout.
01/23/12   MJS     Streamline pause clock entry logic.
01/16/12   MJS     Setup message queues at the same time as hardware init.
01/06/12   MJS     Bug fix for InterruptAcquireLock dereference.
01/06/12   MJS     Fix klocwork issues.
12/18/11   MJS     Fix ordering of BAM reset during SSR.
12/18/11   MJS     Throttle interface device interrupts to prevent 
                   interrupt storm.
12/11/11   MJS     Remove references to DAL.
11/29/11   MJS     Fix race condition where power manager could put driver 
                   into idle state during framer wake up.
11/15/11   MJS     Fix klocwork issues.
10/25/11   MJS     Master support for shared channels, implement
                   reconfiguration client queue.
10/12/11   AV      Added Slimbus SSR feature. 
10/07/11   MJS     Support for purging data channels.
09/26/11   MJS     Strip out GPIO interrupt logic.
09/26/11   MJS     Working pause clock implementation, report unsupported
                   Rx messages.
09/12/11   MJS     Improved logging for error conditions.
08/23/11   MJS     Implement alternate SW workaround for CR-0000156195,
                   only disconnect BAM after port is disconnected.
08/10/11   MJS     Fix capitalization for DDIGPIOInt.h.
05/24/11   MJS     Add pause clock support, separate hardware from software
                   initialization.
05/24/11   MJS     Add master-specific interrupt handling function, 
                   remove hard-coded max client limit, add GPIO config.
05/10/11   MJS     Remove workarounds initially needed for Virtio.
04/08/11   MJS     Implement workarounds for Waverider v1 and v2.
11/01/10   MJS     Initial revision.

===========================================================================
             Copyright (c) 2010-2018 QUALCOMM Technologies Incorporated.
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
#include "SlimBusChanSched.h"
#include "SlimBusBam.h"
#include SB_TRACE_HEADER(SlimBusMaster)

/*------------------------------------------------------------------------------
 Function declarations
 -----------------------------------------------------------------------------*/ 
static SBResult SlimBus_MasterDisconnectPortFromChannel
(
  SlimBusClientCtxt *pClientCtxt,
  SlimBusDevCtxt *pDevCtxt,
  SlimBusDeviceType *pDevice,
  uint32 uPortNum
);

/*------------------------------------------------------------------------------
Helper functions
------------------------------------------------------------------------------*/

/**
 * @brief Remove a port connection tracker object
 *  
 * This function removes a port connection tracker object, when 
 * a port is being disconnected or when a channel is being 
 * removed. 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] pNode     Pointer to the port connection tracker
 */
static void RemovePortConnectionTracker
(
  SlimBusDevCtxt *pDevCtxt,
  SlimBusPortTrackerType *pNode
)
{
  if (NULL != pNode &&
      NULL != pNode->pDevice &&
      NULL != pNode->pChanParam)
  {
    /*SB_LOG_VERB_5("RemovePortConnectionTracker parameters (LA: 0x%x) (channel: %ld) (port: %ld) (EP: %ld) (flow: %ld)",
                  pNode->pDevice->LA, pNode->pChanParam->channelNum, pNode->portNum, pNode->endPointNum, pNode->eFlow);*/

    if (SLIMBUS_PORT_SOURCE_FLOW == pNode->eFlow)
    {
      pNode->pChanParam->pSrcNode = NULL;
    }
    else
    {
      SlimBusPortTrackerType **ppTemp = &pNode->pChanParam->pSinkList;
      while (NULL != *ppTemp)
      {
        if (*ppTemp == pNode)
        {
          *ppTemp = pNode->pNextSinkNode;
          break;
        }
        ppTemp = &(*ppTemp)->pNextSinkNode;
      }
    }
    pNode->pChanParam = NULL;
  }
}

/**
 * @brief Remove port connection tracker objects
 *  
 * This function removes port connection tracker objects, when a
 * port is being disconnected.  When a port is disconnected, all 
 * endpoints of the port get disconnected, so this function 
 * iterates through all the endpoints of the port. 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] pDevice   Pointer to the Slimbus device structure
 * @param[in] portNum   The port number that is being 
 *       disconnected
 */
static void RemovePortConnectionTracker2
(
  SlimBusDevCtxt *pDevCtxt,
  SlimBusDeviceType *pDevice,
  uint32 portNum
)
{
  if (NULL != pDevice->pPortArr && 
      portNum < SB_MAX_PORTS &&
      NULL != pDevice->pPortArr[portNum].pEndPointArr)
  {
    uint32 EP = 0;

    for (EP = 0; EP < SB_MAX_ENDPOINTS; EP++)
    {
      RemovePortConnectionTracker(pDevCtxt, &pDevice->pPortArr[portNum].pEndPointArr[EP]);
    }
  }
}


/**
 * @brief Add a port connection tracker object
 *  
 * This function adds a port connection tracker object, to track 
 * the connection between a source/sink port and the channel 
 * that it is connected to. 
 * 
 * @param[in] pDevCtxt Pointer to the device context
 * @param[in] pDevice  Pointer to the Slimbus device structure
 * @param[in] pParam   Pointer to the channel parameter 
 *       structure
 * @param[in] portNum  The port number that is being connected
 * @param[in] endPointNum  The endpoint number that is 
 *       being connected
 * @param[in] eFlow    The flow direction of the port being 
 *       connected
 * 
 * @return  SB_SUCCESS on success, error code on error 
 */
static SBResult AddPortConnectionTracker
(
  SlimBusDevCtxt *pDevCtxt,
  SlimBusDeviceType *pDevice,
  SlimBusChannelParamType *pParam,
  uint32 portNum,
  uint32 endPointNum,
  SlimBusPortFlowType eFlow
)
{
  /* Alloc port tracker array if it has not already been allocated */
  if (NULL == pDevice->pPortArr)
  {
    SlimBus_Malloc(sizeof(*pDevice->pPortArr)*SB_MAX_PORTS, (void **)&pDevice->pPortArr, SB_MEMTAG_26);
    memset(pDevice->pPortArr, 0, sizeof(*pDevice->pPortArr)*SB_MAX_PORTS);
  }
  /* Alloc endpoint tracker array if it has not already been allocated */
  if (NULL != pDevice->pPortArr &&
      portNum < SB_MAX_PORTS &&
      NULL == pDevice->pPortArr[portNum].pEndPointArr)
  {
    SlimBus_Malloc(sizeof(*pDevice->pPortArr[portNum].pEndPointArr)*SB_MAX_ENDPOINTS, (void **)&pDevice->pPortArr[portNum].pEndPointArr, SB_MEMTAG_27);
    memset(pDevice->pPortArr[portNum].pEndPointArr, 0, sizeof(*pDevice->pPortArr[portNum].pEndPointArr)*SB_MAX_ENDPOINTS);
  }

  if (NULL != pDevice->pPortArr && 
      portNum < SB_MAX_PORTS &&
      NULL != pDevice->pPortArr[portNum].pEndPointArr &&
      endPointNum < SB_MAX_ENDPOINTS &&
      NULL != pParam)
  {
    SlimBusPortTrackerType *pNode = &pDevice->pPortArr[portNum].pEndPointArr[endPointNum];

    /*SB_LOG_VERB_5("AddPortConnectionTracker parameters (LA: 0x%x) (channel: %ld) (port: %ld) (EP: %ld) (flow: %ld)",
                  pDevice->LA, pParam->channelNum, portNum, endPointNum, eFlow);*/

    /* Initialization: make sure port number and device pointer setup properly */
    pNode->portNum = portNum;
    pNode->endPointNum = endPointNum;
    pNode->pDevice = pDevice;

    /* Validation: make sure there is not already a source connected to this channel */
    if (SLIMBUS_PORT_SOURCE_FLOW == eFlow)
    {
      if ( pParam->pSrcNode != NULL &&
           pParam->pSrcNode->pDevice != NULL &&
          ( pParam->pSrcNode->pDevice != pDevice ||
            pParam->pSrcNode->portNum != portNum ||
            pParam->pSrcNode->endPointNum != endPointNum) )
      {
        SB_LOG_ERR_6("AddPortConnectionTracker: A source is already connected (oldLA: 0x%x) "
                     "(oldPort: %ld) (oldEP: %ld) (newLA: 0x%x) (newPort: %ld) (newEP: %ld)",
                     pParam->pSrcNode->pDevice->LA, pParam->pSrcNode->portNum, 
                     pParam->pSrcNode->endPointNum, pDevice->LA, portNum, endPointNum);
        return SB_ERROR;
      }
    }

    /* Cleanup: make sure any old association is removed */
    RemovePortConnectionTracker(pDevCtxt, pNode);

    /* Flow direction-specific association data */
    if (SLIMBUS_PORT_SOURCE_FLOW == eFlow)
    {
      pParam->pSrcNode = pNode;
    }
    else
    {
      pNode->pNextSinkNode = pParam->pSinkList;
      pParam->pSinkList = pNode;
    }

    /* Flow direction-independent association data */
    pNode->pChanParam = pParam;
    pNode->eFlow = eFlow;
  }

  return SB_SUCCESS;
}


/**
 * @brief Wait on an event object with timeout
 *
 * This function performs a timed wait on an event object
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] hEvent  Event handle
 * @param[in] uTimeoutUsec  Timeout in microseconds
 * 
 * @return SB_SUCCESS on success, error code on error
 */
static SBResult SlimBus_MasterEventTimedWait
(
  SlimBusDevCtxt *pDevCtxt,
  SlimBusEventHandle hEvent,
  uint32 uTimeoutUsec
)
{
  SBResult result = SB_ERROR;
  
  if (NULL != pDevCtxt && NULL != pDevCtxt->pSched)
  {
    uint32 uIdx = 1;
    DALSYSEventHandle hEvents[] = { hEvent, pDevCtxt->pSched->hTimeoutEvent };
    
    DALSYS_EventCtrl(pDevCtxt->pSched->hTimeoutEvent, DALSYS_EVENT_CTRL_RESET);

    result = DALSYS_EventMultipleWait(hEvents,
                                      sizeof(hEvents)/sizeof(hEvents[0]),
                                      uTimeoutUsec,
                                      &uIdx);
    if (SB_SUCCESS == result &&
        uIdx > 0)
    {
      result = SLIMBUS_ERR_MSG_RESP_TIMEOUT;
    }
  }
  
  return result;
}

/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/

/**
 * @brief Checks if the driver binary supports master-specific logic 
 *
 * This function checks whether the compiled driver binary supports 
 * master-specific logic
 * 
 * @return TRUE if master-specfic logic is supported, FALSE otherwise 
 */
SBBOOL SlimBus_IsMasterSupported( void )
{
  return TRUE;
}

/**
 * @brief Calculates the slots used by the channel
 *
 * This function calculates the slots used by the channel
 * 
 * @param[in] pParam  Pointer to a channel parameter data structure
 * 
 * @return The number of slots used
 */
SLIMBUS_INLINE uint32 SlimBus_GetSlotsUsed(SlimBusChannelParamType *pParam)
{
  uint32 uSlots = pParam->ucSegLen << pParam->ucRootExp;
  
  if (pParam->eCoeff == SB_RATE_COEFF_3)
  {
    uSlots *= 3;
  }
  
  return uSlots;
}

/**
 * @brief Allocate a satellite device structure
 *
 * This function allocates a satellite device structure
 * 
 * @param[in] pDevice  Pointer to the device context
 */
static void SlimBus_AllocSatStruct( SlimBusDeviceType *pDevice )
{
  SlimBus_Malloc(sizeof(SlimBusSatelliteType),
                (void**)&(pDevice->pSat), SB_MEMTAG_9);
  memset(pDevice->pSat, 0, sizeof(*pDevice->pSat));
  pDevice->pSat->numClientSlots = SB_DEFAULT_CLIENTS_PER_DEVICE;
  SlimBus_Malloc(SB_DEFAULT_CLIENTS_PER_DEVICE*sizeof(pDevice->pSat->paClientCtxt[0]),
                (void**)&(pDevice->pSat->paClientCtxt), SB_MEMTAG_10);
  memset(pDevice->pSat->paClientCtxt, 0,
         SB_DEFAULT_CLIENTS_PER_DEVICE*sizeof(pDevice->pSat->paClientCtxt[0]));
}

/**
 * @brief Return a channel parameter structure
 *
 * This function returns channel parameter structure for the
 * given channel number.  If necessary, a structure is allocated.
 * 
 * @param[in] pDevCtxt    Pointer to the device context
 * @param[in] pChanParam  Pointer to the chanParam buffer
 * 
 * @return The channel parameter structure
 */
static SlimBusChannelParamType* SlimBus_GetChanParam(SlimBusDevCtxt *pDevCtxt, uint32 uChanNum)
{
  SlimBusChannelParamType* pChanParam = pDevCtxt->pSched->apChanParams[uChanNum];
  
  if ( NULL == pChanParam )
  {
    SlimBus_Malloc(sizeof(SlimBusChannelParamType), (void**)(&pChanParam), SB_MEMTAG_14);
    memset(pChanParam, 0, sizeof(*pChanParam));
    pDevCtxt->pSched->apChanParams[uChanNum] = pChanParam;
    pChanParam->channelNum = uChanNum;
    pChanParam->uNumDataLines = 1;
    pChanParam->eStatus = SB_CHANNEL_INACTIVE;
  }
  
  return pChanParam;
}

/**
 * @brief Add a channel to a channel parameter list
 *
 * This function adds a channel to a channel parameter list, 
 * sorting the list and updating the number of used slots. 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] pParam  Pointer to the data channel parameter 
 *       structure
 */
static void AddChannelParam(SlimBusDevCtxt *pDevCtxt, SlimBusChannelParamType *pParam)
{
  SlimBusLinkNodeType *pRoot;
  SlimBusChannelParamType *pCurParam = NULL;
  uint32 uMaxInterval;

  /* Choose the list to add to based on the channel rate coefficient */
  if (pParam->eCoeff == SB_RATE_COEFF_1)
  {
    uMaxInterval = SB_MAX_INTERVAL_COEFF_1;
    pRoot = &pDevCtxt->pSched->ChanParamList1;
  }
  else
  {
    uMaxInterval = SB_MAX_INTERVAL_COEFF_3;
    pRoot = &pDevCtxt->pSched->ChanParamList3;
  }

  if ( pParam->eStatus == SB_CHANNEL_NEW )
  {
    /* Set max drift to 3/8 the # of clocks per segment interval */
    pParam->iMaxClkGearDrift = (uMaxInterval*SB_CELLS_PER_SLOT*3/8) >> pParam->ucRootExp;
  }
  
  /* Sort nodes in the list: 
       Primary key: ucRootExp (low to high)
       Secondary key: uGrpTotalSlots (high to low) */
  while ( NULL != (pCurParam = NextParamNode(pRoot, pCurParam)) &&
          ( pParam->ucRootExp > pCurParam->ucRootExp ||
            ( pParam->ucRootExp == pCurParam->ucRootExp &&
              pParam->uGrpTotalSlots < pCurParam->uGrpTotalSlots ) ) )
  {
    /* do nothing, continue to next list element */
  }
  /* If we've iterated past the end of the list, insert at the end of the list,
     otherwise insert just prior to the last iterated node */
  if ( NULL == pCurParam )
  {
    PushTailParam(pRoot, pParam);
  }
  else
  {
    PushTailParam(&pCurParam->LinkNode, pParam);
  }
}

/**
 * @brief Updates which data lines the channel can support
 *
 * This function updates which data lines the channel can support.
 * 
 * @param[in] pDevCtxt    Pointer to the device context
 * @param[in] uChanNum    The channel number
 * @param[in] pDevice     Pointer to the a device struct
 * 
 */
static SBResult SlimBus_UpdateDataLineMask(SlimBusDevCtxt *pDevCtxt, uint32 uChanNum, SlimBusDeviceType *pDevice)
{
  SlimBusChannelParamType *pChanParam = SlimBus_GetChanParam( pDevCtxt, uChanNum );
  uint32 uDL;
  
  if (0 == pChanParam->uDataLineMask)
  {
    /* Start-up condition: channel will initially have zero mask */
    pChanParam->uDataLineMask = pDevice->uDataLineMask;
  }
  
  if ((pChanParam->uDataLineMask & pDevice->uDataLineMask) == 0)
  {
    SB_LOG_ERR_4( "No valid data lines would exist for chan %ld on device %x (chan mask %lx, device mask %lx)",
                  pChanParam->channelNum, pDevice->LA, pChanParam->uDataLineMask, pDevice->uDataLineMask );
    return SB_ERROR;
  }
  // Remove unsupported data lines from the channel's mask
  pChanParam->uDataLineMask &= pDevice->uDataLineMask;
  
  pChanParam->uNumDataLines = 0;
  for (uDL = 0; uDL < SB_MAX_DATA_LINES; uDL++)
  {
    if (pChanParam->uDataLineMask & (1<<uDL))
    {
      pChanParam->uNumDataLines++;
    }
  }
  
  return SB_SUCCESS;
}

/**
 * @brief Assigns the channel to a data line
 *
 * This function assigns the channel to a data line
 * based on which data lines it supports.
 * 
 * @param[in] pDevCtxt    Pointer to the device context
 * @param[in] pChanParam  Pointer to the channel
 * 
 */
static void SlimBus_AssignDataLine(SlimBusDevCtxt *pDevCtxt, SlimBusChannelParamType *pChanParam, uint32 *pGrpDataLine)
{
  int32 dlIdx = SB_MAX_DATA_LINES-1;
  uint32 uSlotsUsed;
  uint32 uTmpMask;
  uint32 uTmpUsedSlots = 0xFFFFFF;
  
  if (NULL == pDevCtxt || NULL == pChanParam)
  {
    return;
  }
  
  uSlotsUsed = SlimBus_GetSlotsUsed(pChanParam);
  uTmpMask = pChanParam->uDataLineMask;

  if ((pChanParam->uGrpFlag & SB_GROUP_FLAG_START) != 0)
  {
    /* Try to assign a data line starting from the end, to leave maximum
       empty slots in the primary data line for messaging */
    for (; dlIdx >= 0; dlIdx--)
    {
      if ((uTmpMask & (1<<dlIdx)) != 0) 
      {
        if ( (pDevCtxt->pSched->aDataLineSchedules[dlIdx].uNewSlotsUsed < uTmpUsedSlots) &&
             ( ( HAL_sb_IsSecondaryDataLineSlot0Supported(pDevCtxt->hSB) &&
                 (pDevCtxt->pSched->aDataLineSchedules[dlIdx].uNewSlotsUsed + pChanParam->uGrpTotalSlots <= SB_SLOTS_PER_SUPERFRAME) ) ||
               ( !HAL_sb_IsSecondaryDataLineSlot0Supported(pDevCtxt->hSB) &&
                 (pDevCtxt->pSched->aDataLineSchedules[dlIdx].uNewSlotsUsed + pChanParam->uGrpTotalSlots <  SB_SLOTS_PER_SUPERFRAME) ) ||
               (dlIdx == 0) ) )
        {
          pChanParam->uNewDataLine = (uint32)dlIdx;
          uTmpUsedSlots = pDevCtxt->pSched->aDataLineSchedules[dlIdx].uNewSlotsUsed;
        }
      }
    }
    *pGrpDataLine = pChanParam->uNewDataLine;
  }
  else
  {
    pChanParam->uNewDataLine = *pGrpDataLine;
  }

  /* Update used slots for this channel */
  pDevCtxt->pSched->aDataLineSchedules[pChanParam->uNewDataLine].uNewSlotsUsed += uSlotsUsed;
  
  if (SB_CHANNEL_NEW == pChanParam->eStatus)
  {
    SB_LOG_INFO_2("Channel %d was assigned data line %d",
                  pChanParam->channelNum, pChanParam->uNewDataLine); 
  }
  else if (pChanParam->uNewDataLine != pChanParam->uCurDataLine)
  {
    SB_LOG_INFO_3("Channel %d was assigned data line %d (was %d)",
                  pChanParam->channelNum, pChanParam->uNewDataLine,
                  pChanParam->uCurDataLine); 
  }
}

/**
 * @brief Does a reconfiguration sequence need to be sent 
 *
 * This function determines whether a new reconfiguration 
 * sequence needs to be sent 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[out] ppClientCtxt  Pointer to a pointer to the basic 
 *       client context that is triggering the reconfiguration
 * 
 * @return TRUE if reconfiguration is needed, FALSE otherwise
 */
static SBBOOL
SlimBus_IsReconfigNeeded
(
  SlimBusDevCtxt *pDevCtxt,
  SlimBusBasicClientCtxt **ppClientCtxt
)
{
  SlimBusBasicClientCtxt *pNullClient = NULL;

  if (NULL == ppClientCtxt)
  {
    ppClientCtxt = &pNullClient;
  }
  else
  {
    *ppClientCtxt = NULL;
  }
  
  return ( pDevCtxt->pSched->bPurge ||
           SB_FRM_HANDOVER_IDLE != pDevCtxt->pSched->eFrmHandState ||
           (NULL != (*ppClientCtxt = PeekHeadClient(&pDevCtxt->ReconfigList))) );
}

/**
 * @brief Signal that next reconfiguration sequence 
 *        can be sent
 *
 * This function signals either a waiting thread or the 
 * master workloop to process the next pending 
 * reconfiguration sequence.
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 */
void SlimBus_SignalNextReconfig(SlimBusDevCtxt *pDevCtxt)
{
  if (pDevCtxt->pSched->bPurge && !pDevCtxt->pSched->bDataSlotsScheduled)
  {
    pDevCtxt->pSched->bPurge = FALSE;
  }
  
  if ( SB_PAUSE_NOT_PAUSED == pDevCtxt->pSched->ePause &&
       !pDevCtxt->pSched->bReconfigBusy &&
       SlimBus_IsReconfigNeeded(pDevCtxt, NULL) )
  {
    /* signal the reconfiguration workloop */
    SlimBus_EventTrigger(pDevCtxt->hMasterWorkLoopEvent);
  }
}

/**
 * @brief Mark internal state as having recovered from pause 
 *        clock
 *
 * This function marks the internal state as having recovered 
 * from pause clock. 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SBResult SlimBus_RecoverFromPauseClock(SlimBusDevCtxt *pDevCtxt)
{
  SlimBusEventNodeType *pEventNode;
  uint32 uToken;

  /* Re-enable interface device interrupts here in case they were disabled
     for throttling */
  SlimBus_InterruptAcquireLock(pDevCtxt, &uToken);
  HAL_sb_DeviceIntSetMask(pDevCtxt->hSB, HAL_SB_DEVICE_INTERFACE,
                          HAL_SB_INTF_INT_DEV_ERR|
                          HAL_SB_INTF_INT_IE_VE_CHANGE|
                          HAL_SB_INTF_INT_NO_DATA_LINE_ACTIVITY );  
  SlimBus_InterruptReleaseLock(pDevCtxt, &uToken);

  pDevCtxt->pSched->ePause = SB_PAUSE_NOT_PAUSED;

  while (NULL != (pEventNode = PopHeadEvent(&pDevCtxt->PauseEventQueue)))
  {
    SlimBus_EventTrigger(pEventNode->hEvent);
  }
  SlimBus_SignalNextReconfig(pDevCtxt);
  
  return SB_SUCCESS;
}

/**
 * @brief Process a master-specific received message
 *
 * Process a master-specific received message 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] length  Length of the message
 * @param[in] data  First word of the received message
 * @param[in] pDataBuf  Pointer to the second word in the 
 *       received message buffer
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
SBResult SlimBus_ProcessMasterRxMsg
(
  SlimBusDevCtxt *pDevCtxt,
  uint32 length,
  uint32 data,
  uint32 *pDataBuf
)
{
  SlimBusMsgMtType eMT;
  SlimBusMsgMcType eMC;
  SlimBusMsgMcUsrType eUsrMC;
  SBResult result = SB_SUCCESS;
  
  eMT = (SlimBusMsgMtType) SB_MSG_GET(data,GENERIC_0,MT);
  eMC = (SlimBusMsgMcType) SB_MSG_GET(data,GENERIC_0,MC);
  eUsrMC = (SlimBusMsgMcUsrType) eMC;

  if (eMT == SB_MSG_MT_CORE)
  {
    if (eMC == SB_MSG_MC_REPORT_PRESENT)
    {
      uint8 aEA[SLIMBUS_ENUM_ADDR_BUF_SIZE];
      uint32 uIdx;
      int32 avail = -1;
      SlimBusDeviceType *pDevice = NULL;
  
      if (length != SB_MSG_MC_REPORT_PRESENT_LEN)
      {
        /* TODO: handle error */
        return SB_ERROR;
      }
  
      aEA[5] = ((data>>16)&0xff);
      aEA[4] = ((data>>24)&0xff);
  
      data = *(pDataBuf++);
  
      aEA[3] = ((data>>0)&0xff);
      aEA[2] = ((data>>8)&0xff);
      aEA[1] = ((data>>16)&0xff);
      aEA[0] = ((data>>24)&0xff);
  
      for ( uIdx = 0; uIdx < pDevCtxt->numDeviceSlots; uIdx++ )
      {
        if ( NULL == pDevCtxt->pDevices[uIdx] ||
             pDevCtxt->pDevices[uIdx]->eState == SB_ENUM_INACTIVE )
        {
          if ( avail < 0 )
          {
            avail = uIdx;
          }
        }
        else if (NULL != pDevCtxt->pDevices[uIdx] &&
                 memcmp(pDevCtxt->pDevices[uIdx]->aEA, aEA, sizeof(aEA)) == 0)
        {
          pDevice = pDevCtxt->pDevices[uIdx];
          break;
        }
      }
      if ( NULL == pDevice && avail < 0 )
      {
        /* TODO: handle error, no room to enumerate device */
        return SB_ERROR;
      }

      if ( NULL == pDevice )
      {
        uIdx = avail;
        pDevice = pDevCtxt->pDevices[uIdx];
        if (NULL == pDevice)
        {
          if (pDevCtxt->bNoMalloc)
          {
            /* If we're in ISR then we can't alloc new device struct */
            return SLIMBUS_ERR_RETRY;
          }
          pDevice = SlimBus_NewDeviceStruct();
          pDevCtxt->pDevices[uIdx] = pDevice;
        }
        SlimBus_Memcpy(pDevice->aEA, aEA, sizeof(aEA));
        pDevice->LA = (uint8) (uIdx + SB_LA_DEFAULT_BASE);
      }
      data = *(pDataBuf++);
      pDevice->DC = ((data>>0)&0xff);
      pDevice->DV = ((data>>8)&0xff);

      pDevice->eState = SB_ENUM_PRESENT;
      
      SB_LOG_INFO_4("Assigning device logical address (EA: 0x%04lx%08lx) (DC: 0x%02x) (LA: 0x%x)", 
                    (((uint32)aEA[5]<<8)|(uint32)aEA[4]),
                    (((uint32)aEA[3]<<24)|((uint32)aEA[2]<<16)|((uint32)aEA[1]<<8)|(uint32)aEA[0]), 
                    pDevice->DC, pDevice->LA );
      
      /* signal the master workloop */
      pDevCtxt->pSched->bEnumPending = TRUE;
      SlimBus_EventTrigger(pDevCtxt->hMasterWorkLoopEvent);
    }
    else if ( SB_MSG_MC_REPORT_INFORMATION == eMC )
    {
      uint8 LA = (uint8) SB_MSG_GET(data,GENERIC_0,LA);
      SlimBusDeviceType *pDevice = pDevCtxt->pDevices[LA&SB_LA_IDX_MASK];
      uint32 *pBuf = NULL;
      uint8 DC = 0x55;
      uint32 value = *pDataBuf;
      uint16 EC = ((value & 0xff) << 8) | (data>>24);
      uint16 addr = (EC>>4);
      value >>= 8;
      
      if ( NULL != pDevice &&
           pDevice->LA == LA )
      {
        DC = pDevice->DC;
      }
      SB_LOG_WARN_5("Received REPORT_INFORMATION message "
                    "(LA: 0x%x) (DC: 0x%x) (addr: 0x%x) (SS: 0x%x) (value: 0x%06lx)",
                    LA, DC, addr, (EC&0xf), value);
      
      if ( NULL != pDevice &&
           pDevice->LA == LA &&
           ( addr == 0x000 ||
             addr == 0x400 ) )
      {
        pBuf = pDevCtxt->TxMsgFifo.getBufFcn(NULL, NULL, &pDevCtxt->TxMsgFifo,
                                             length);
        
        if (NULL != pBuf)
        {
          memset(pBuf, 0x00, length);
          
          pBuf[0] = SB_MSG_ASM_FIRST_WORD(length,
                                          SB_MSG_MT_CORE,
                                          SB_MSG_MC_CLEAR_INFORMATION,
                                          SB_MSG_DT_UNICAST, LA) |
            ((EC&0xff)<<24);
          pBuf[1] = (value<<8) | ((EC>>8)&0xff);
          
          pDevCtxt->TxMsgFifo.sendBufFcn(pDevCtxt, &pDevCtxt->TxMsgFifo);
        }
      }
    }
    else
    {
      result = SLIMBUS_ERR_UNHANDLED_MSG;
    }
  }
  else if ( SB_MSG_MT_DEST_REFERRED_USER == eMT )
  {
    uint8 LA = (uint8) SB_MSG_GET(data,GENERIC_0,LA);
    SlimBusDeviceType *pDevice = pDevCtxt->pDevices[LA&SB_LA_IDX_MASK];
    
    /* If we received a satellite message from device not registered as a satellite */
    if ( NULL != pDevice &&
         pDevice->LA == LA &&
         NULL == pDevice->pSat )
    {
      /* If the device is a QC device and one of the NGDs */
      if ( pDevice->aEA[5] == 0x2 &&
           pDevice->aEA[4] == 0x17 &&
           ( pDevice->aEA[1] == 0x3 ||
             pDevice->aEA[1] == 0x4 ) )
      {
        pDevice->eState = SB_ENUM_ACTIVE; /* This device must already be active */
        SlimBus_AllocSatStruct(pDevice);

        SB_LOG_INFO_1("Received unexpected satellite protocol message (LA: 0x%x)", LA);
        
        if ( NULL != pDevice->pSat )
        {
          uint32 *pBuf;
          
          /* Send the device a master capability message if we think it is a NGD */
          pBuf = pDevCtxt->TxMsgFifo.getBufFcn(NULL, NULL, &pDevCtxt->TxMsgFifo, 
                                               SB_MSG_MC_USR_QC_MASTER_CAPABILITY_LEN);
          if (NULL != pBuf )
          {
            /* Assume V1 protocol unless satellite reports greater capability */
            pDevice->pSat->uSatProtoVer = 1;
            pDevice->pSat->bExpectAnnouce = TRUE;

            pBuf[0] = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_USR_QC_MASTER_CAPABILITY_LEN,
                                            SB_MSG_MT_SRC_REFERRED_USER,
                                            SB_MSG_MC_USR_QC_MASTER_CAPABILITY,
                                            SB_MSG_DT_UNICAST,
                                            pDevice->LA) |
              SB_MSG_SET(SB_QC_MASTER_PROTOCOL_MAGIC_LSB,USR_QC_MASTER_CAPABILITY_0,MAGIC_LSB);
            pBuf[1] = 
              SB_MSG_SET(SB_QC_MASTER_PROTOCOL_MAGIC_MSB,USR_QC_MASTER_CAPABILITY_1,MAGIC_MSB) |
              SB_MSG_SET(SB_QC_MASTER_CAPABILITY_VER,USR_QC_MASTER_CAPABILITY_1,PROTO_VER) |
              SB_MSG_SET(SB_QC_SUPPORTED_PROTOCOLS,USR_QC_MASTER_CAPABILITY_1,SAT_PROTO);

            pDevCtxt->TxMsgFifo.sendBufFcn(pDevCtxt, &pDevCtxt->TxMsgFifo);
          }
        }
      }
    }
    if ( NULL != pDevice &&
         pDevice->LA == LA &&
         NULL != pDevice->pSat &&
         SB_QC_MASTER_PROTOCOL_V1 == pDevice->pSat->uSatProtoVer )
    {
      if ( SB_MSG_MC_USR_CHANNEL_CONTROL == eUsrMC ||
           SB_MSG_MC_USR_CHANNEL_DEF == eUsrMC || 
           SB_MSG_MC_USR_CHANNEL_DEF_ACTIVATE == eUsrMC ||
           SB_MSG_MC_USR_REQ_MSG_BANDWIDTH == eUsrMC )
      {
        uint8 uClientNum;
        uint8 TID = 0;
        SlimBusBasicClientCtxt *pBasicClient;
        uint32 *pBuf, *pConfigBuf;
        SBBOOL bResp = FALSE;

        uClientNum = SB_MSG_GET(data,USR_CHANNEL_DEF_0,CLIENT_NUM);

        if (SB_MSG_MC_USR_CHANNEL_DEF == eUsrMC ||
            SB_MSG_MC_USR_CHANNEL_DEF_ACTIVATE == eUsrMC)
        {
          TID = SB_MSG_GET(*pDataBuf,USR_CHANNEL_DEF_1,TID);

          SB_LOG_VERB_8("Got satellite define channel request (client: 0x%02x%02x) "
            "(channel: %d) (#channels: %ld) (protocol: %ld) (eCoeff: %ld) "
            "(exp: %ld) (bitwidth: %ld)", LA, uClientNum,
            *((uint8*)pDataBuf + SB_MSG_MC_USR_CHANNEL_DEF__CN_OFFSET - sizeof(uint32)),
            length-SB_MSG_MC_USR_CHANNEL_DEF__CN_OFFSET,
            SB_MSG_GET(*pDataBuf,USR_CHANNEL_DEF_1,PROTO),
            SB_MSG_GET(*pDataBuf,USR_CHANNEL_DEF_1,COEFF),
            SB_MSG_GET(*pDataBuf,USR_CHANNEL_DEF_1,EXP),
            SB_CELLS_PER_SLOT*SB_MSG_GET(*pDataBuf,USR_CHANNEL_DEF_1,DL) );
        }
        if (SB_MSG_MC_USR_CHANNEL_CONTROL == eUsrMC)
        {
          TID = SB_MSG_GET(*pDataBuf,USR_CHANNEL_CONTROL_1,TID);

          SB_LOG_VERB_5("Got satellite data channel control request (client: 0x%02x%02x) "
            "(channel: %d) (#channels: %ld) (op: %ld)", LA, uClientNum,
            *((uint8*)pDataBuf + SB_MSG_MC_USR_CHANNEL_CONTROL__CN_OFFSET - sizeof(uint32)),
            length-SB_MSG_MC_USR_CHANNEL_CONTROL__CN_OFFSET,
            SB_MSG_GET(data,USR_CHANNEL_CONTROL_0,CTRL) );
        }
        if (SB_MSG_MC_USR_REQ_MSG_BANDWIDTH == eUsrMC)
        {
          TID = SB_MSG_GET(*pDataBuf,USR_REQ_MSG_BANDWIDTH_1,TID);

          SB_LOG_VERB_2("Got satellite message bandwitdth request (client: 0x%02x%02x)",
                        LA, uClientNum);
        }
        if (uClientNum >= pDevice->pSat->numClientSlots)
        {
          uint32 uNewClientSlots = pDevice->pSat->numClientSlots;
          SlimBusBasicClientCtxt **paNewClientCtxt = NULL;
          
          if (pDevCtxt->bNoMalloc)
          {
            /* If we're in ISR then we can't alloc new struct */
            return SLIMBUS_ERR_RETRY;
          }
          while (uClientNum >= uNewClientSlots)
          {
            uNewClientSlots *= 2;
          }
          SlimBus_Malloc(uNewClientSlots*sizeof(paNewClientCtxt[0]),
                        (void**)&paNewClientCtxt, SB_MEMTAG_11);
          memset(paNewClientCtxt, 0, uNewClientSlots*sizeof(paNewClientCtxt[0]));
          SlimBus_Memcpy(paNewClientCtxt, pDevice->pSat->paClientCtxt,
                         pDevice->pSat->numClientSlots*sizeof(paNewClientCtxt[0]));
          SlimBus_Free(pDevice->pSat->paClientCtxt);
          pDevice->pSat->paClientCtxt = paNewClientCtxt;
          pDevice->pSat->numClientSlots = uNewClientSlots;
        }

        if ( NULL == pDevice->pSat->paClientCtxt[uClientNum] )
        {
          if (pDevCtxt->bNoMalloc)
          {
            /* If we're in ISR then we can't alloc new struct */
            return SLIMBUS_ERR_RETRY;
          }
          SlimBus_Malloc(sizeof(SlimBusBasicClientCtxt),
                        (void**)&(pDevice->pSat->paClientCtxt[uClientNum]), SB_MEMTAG_12);
          pBasicClient = pDevice->pSat->paClientCtxt[uClientNum];
          memset(pBasicClient, 0, sizeof(SlimBusBasicClientCtxt));
          SlimBus_Malloc(SB_CFG_MSG_BUF_LENGTH*sizeof(uint32)+SB_MAX_MSG_LENGTH_BYTES,
                        (void**)&pBasicClient->ConfigMsgFifo.pBuf, SB_MEMTAG_13);
          pBasicClient->ConfigMsgFifo.uLen = SB_CFG_MSG_BUF_LENGTH;
          pBasicClient->ConfigMsgFifo.uAckIdx = SB_ACK_IDX_UNUSED;
          pBasicClient->ConfigMsgFifo.getBufFcn = SlimBus_GetMsgBuffer;
          pBasicClient->ConfigMsgFifo.sendBufFcn = SlimBus_DefaultSendBuffer;
          pBasicClient->ConfigMsgFifo.hDevCtxt = SlimBus_GetDevHandle(pDevCtxt);
          pBasicClient->pDevice = pDevice;
          pBasicClient->uClientNum = uClientNum;
        }
        else
        {
          pBasicClient = pDevice->pSat->paClientCtxt[uClientNum];
        }

        pBuf = pDevCtxt->TxMsgFifo.getBufFcn(NULL, NULL, &pDevCtxt->TxMsgFifo,
                                             SB_MSG_MC_USR_GENERIC_ACK_LEN);
        if (NULL == pBuf)
        {
          return SLIMBUS_ERR_RETRY;
        }

        pConfigBuf = pBasicClient->ConfigMsgFifo.getBufFcn(NULL, NULL,
                                                           &pBasicClient->ConfigMsgFifo,
                                                           length);
        if (NULL != pConfigBuf)
        {
          SlimBus_Memcpy(pConfigBuf, (void*)(pDataBuf-1), length);
          pBasicClient->ConfigMsgFifo.sendBufFcn(pDevCtxt, &pBasicClient->ConfigMsgFifo);
          bResp = TRUE;
        }

        *(pBuf++) = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_USR_GENERIC_ACK_LEN,
                                          SB_MSG_MT_SRC_REFERRED_USER,
                                          SB_MSG_MC_USR_GENERIC_ACK,
                                          SB_MSG_DT_UNICAST,
                                          LA) |
          SB_MSG_SET(TID,USR_GENERIC_ACK_0,TID);
        *pBuf = SB_MSG_SET(bResp,USR_GENERIC_ACK_1,RESP);
        pDevCtxt->TxMsgFifo.sendBufFcn(pDevCtxt, &pDevCtxt->TxMsgFifo);
      }
      else if ( SB_MSG_MC_USR_DO_RECONFIGURE_NOW == eUsrMC )
      {
        uint8 TID, uClientNum;

        TID = SB_MSG_GET(data,USR_DO_RECONFIGURE_NOW_0,TID);
        uClientNum = SB_MSG_GET(*pDataBuf,USR_DO_RECONFIGURE_NOW_1,CLIENT_NUM);

        SB_LOG_VERB_2("Got satellite reconfigure now request (client: 0x%02x%02x)",
                      LA, uClientNum);

        if ( uClientNum < pDevice->pSat->numClientSlots &&
             NULL != pDevice->pSat->paClientCtxt[uClientNum] )
        {
          RemoveClientNode(pDevice->pSat->paClientCtxt[uClientNum]);
          PushTailClient(&pDevCtxt->ReconfigList, pDevice->pSat->paClientCtxt[uClientNum]);
          pDevice->pSat->paClientCtxt[uClientNum]->uConfigTID = TID;
          SlimBus_SignalNextReconfig(pDevCtxt);
        }
        else
        {
          uint32 *pBuf;
          pBuf = pDevCtxt->TxMsgFifo.getBufFcn(NULL, NULL, &pDevCtxt->TxMsgFifo, 
                                               SB_MSG_MC_USR_GENERIC_ACK_LEN);
          if (NULL == pBuf)
          {
            return SLIMBUS_ERR_RETRY;
          }

          SB_LOG_ERR_2("Error during satellite reconfigure now request (client: 0x%02x%02x)",
                       LA, uClientNum);

          *(pBuf++) = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_USR_GENERIC_ACK_LEN,
                                            SB_MSG_MT_SRC_REFERRED_USER,
                                            SB_MSG_MC_USR_GENERIC_ACK,
                                            SB_MSG_DT_UNICAST,
                                            LA) |
            SB_MSG_SET(TID,USR_GENERIC_ACK_0,TID);
          *pBuf = SB_MSG_SET(FALSE,USR_GENERIC_ACK_1,RESP);
          pDevCtxt->TxMsgFifo.sendBufFcn(pDevCtxt, &pDevCtxt->TxMsgFifo);
        }
      }
      else if ( SB_MSG_MC_USR_LA_QUERY == eUsrMC )
      {
        uint8 TID = SB_MSG_GET(data,USR_LA_QUERY_0,TID);
        uint8* pEA = (uint8*)pDataBuf;
        SlimBusDeviceType *pQueryDevice = NULL;
        uint32 *pBuf;
        uint32 i;

        for ( i = 0; i < pDevCtxt->numDeviceSlots; i++ )
        {
          if ( NULL != pDevCtxt->pDevices[i] &&
               SB_ENUM_ACTIVE == pDevCtxt->pDevices[i]->eState &&
               memcmp(pEA, pDevCtxt->pDevices[i]->aEA, SLIMBUS_ENUM_ADDR_BUF_SIZE) == 0)
          {
            pQueryDevice = pDevCtxt->pDevices[i];
            break;
          }
        }

        pBuf = pDevCtxt->TxMsgFifo.getBufFcn(NULL, NULL,
                                             &pDevCtxt->TxMsgFifo, 
                                             SB_MSG_MC_USR_LA_REPLY_LEN);
        if (NULL == pBuf)
        {
          return SLIMBUS_ERR_RETRY;
        }
        
        *(pBuf++) = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_USR_LA_REPLY_LEN,
                                          SB_MSG_MT_SRC_REFERRED_USER,
                                          SB_MSG_MC_USR_LA_REPLY,
                                          SB_MSG_DT_UNICAST,
                                          LA) |
          SB_MSG_SET(TID,USR_LA_REPLY_0,TID);
        if ( NULL != pQueryDevice )
        {
          SlimBus_Memcpy(pBuf, pQueryDevice->aEA, SLIMBUS_ENUM_ADDR_BUF_SIZE);
          ((uint8*)pBuf)[SLIMBUS_ENUM_ADDR_BUF_SIZE] = pQueryDevice->LA;
        }
        else
        {
          memset(pBuf, 0, SLIMBUS_ENUM_ADDR_BUF_SIZE+1);
        }

        SB_LOG_VERB_4("Got satellite LA query (satLA: 0x%x) (EA: 0x%04x%08lx) (LA: 0x%x)",
                      LA, *((uint16*)&pDataBuf[1]), *(pDataBuf), 
                      ((pQueryDevice!=NULL)?pQueryDevice->LA:0) );

        pDevCtxt->TxMsgFifo.sendBufFcn(pDevCtxt, &pDevCtxt->TxMsgFifo);
      }
      else if ( SB_MSG_MC_USR_REQ_CONNECT_SOURCE == eUsrMC ||
                SB_MSG_MC_USR_REQ_CONNECT_SINK == eUsrMC )
      {
        SBResult rtn;
        uint8 destLA = SB_MSG_GET(data,USR_REQ_CONNECT_SOURCE_0,DEST_LA);
        uint8 portNum, chanNum, endPointNum, TID;
        uint32 *pBuf;
        SlimBusDeviceType *pDestDevice;
        SlimBusChannelParamType *pChanParam;

        data = *(pDataBuf++);
        
        portNum = SB_MSG_GET(data,USR_REQ_CONNECT_SOURCE_1,PORT_NUM);
        chanNum = SB_MSG_GET(data,USR_REQ_CONNECT_SOURCE_1,CHAN_NUM);
        endPointNum = SB_MSG_GET(data,USR_REQ_CONNECT_SOURCE_1,EP_NUM);
        TID = (uint8) SB_MSG_GET(data,USR_REQ_CONNECT_SOURCE_1,TID);
        
        pDestDevice = SlimBus_GetDevicePtr(pDevCtxt, destLA);
        
        if (NULL != pDestDevice)
        {
          if ((rtn = SlimBus_UpdateDataLineMask( pDevCtxt, chanNum, pDestDevice )) != SB_SUCCESS)
          {
            return rtn;
          }
        }
        else
        {
          SB_LOG_ERR_0("Cannot find destination device");
          return SB_ERROR;
        }

        pChanParam = SlimBus_GetChanParam( pDevCtxt, chanNum );

        if (eUsrMC == SB_MSG_MC_USR_REQ_CONNECT_SOURCE)
        {
          eMC = SB_MSG_MC_CONNECT_SOURCE;
          SB_LOG_VERB_5("Got satellite connect source port request "
                      "(satLA: 0x%x) (channel: %d) (destLA: 0x%x) (port: %d) (EP: %d)",
                      LA, chanNum, destLA, portNum, endPointNum);
          if ((rtn = AddPortConnectionTracker(pDevCtxt, pDestDevice, pChanParam, portNum,
                                          endPointNum, SLIMBUS_PORT_SOURCE_FLOW)) != SB_SUCCESS)
          {
            return rtn;
          }
        }
        else
        {
          eMC = SB_MSG_MC_CONNECT_SINK;
          SB_LOG_VERB_5("Got satellite connect sink port request "
                      "(satLA: 0x%x) (channel: %d) (destLA: 0x%x) (port: %d) (EP: %d)",
                      LA, chanNum, destLA, portNum, endPointNum);
          if ((rtn = AddPortConnectionTracker(pDevCtxt, pDestDevice, pChanParam, portNum,
                                           endPointNum, SLIMBUS_PORT_SINK_FLOW)) != SB_SUCCESS)
          {
            return rtn;
          }
        }
        
        pBuf = pDevCtxt->TxMsgFifo.getBufFcn(NULL, NULL, &pDevCtxt->TxMsgFifo, 
                                             SB_MSG_MC_CONNECT_SOURCE_LEN);
        if (NULL == pBuf)
        {
          return SLIMBUS_ERR_RETRY;
        }
        *(pBuf++) = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_CONNECT_SOURCE_LEN,
                                          SB_MSG_MT_CORE, eMC,
                                          SB_MSG_DT_UNICAST,
                                          destLA) |
          SB_MSG_SET(portNum,CONNECT_SOURCE_0,PN) |
          SB_MSG_SET(endPointNum,CONNECT_SOURCE_0,EP);
        *(pBuf++) = SB_MSG_SET(chanNum,CONNECT_SOURCE_1,CN);

        pDevCtxt->TxMsgFifo.sendBufFcn(pDevCtxt, &pDevCtxt->TxMsgFifo);

        pBuf = pDevCtxt->TxMsgFifo.getBufFcn(NULL, NULL, &pDevCtxt->TxMsgFifo, 
                                             SB_MSG_MC_USR_GENERIC_ACK_LEN);
        if (NULL == pBuf)
        {
          return SLIMBUS_ERR_RETRY;
        }
        *(pBuf++) = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_USR_GENERIC_ACK_LEN,
                                          SB_MSG_MT_SRC_REFERRED_USER,
                                          SB_MSG_MC_USR_GENERIC_ACK,
                                          SB_MSG_DT_UNICAST,
                                          LA) |
          SB_MSG_SET(TID,USR_GENERIC_ACK_0,TID);
        *pBuf = SB_MSG_SET(TRUE,USR_GENERIC_ACK_1,RESP);
        pDevCtxt->TxMsgFifo.sendBufFcn(pDevCtxt, &pDevCtxt->TxMsgFifo);
      }
      else if ( SB_MSG_MC_USR_REQ_DISCONNECT_PORT == eUsrMC )
      {
        uint8 destLA = SB_MSG_GET(data,USR_REQ_DISCONNECT_PORT_0,DEST_LA);
        uint8 portNum, TID;
        uint32 *pBuf;

        data = *(pDataBuf++);

        portNum = SB_MSG_GET(data,USR_REQ_DISCONNECT_PORT_1,PORT_NUM);
        TID = SB_MSG_GET(data,USR_REQ_DISCONNECT_PORT_1,TID);

        pBuf = pDevCtxt->TxMsgFifo.getBufFcn(NULL, NULL, &pDevCtxt->TxMsgFifo, 
                                             SB_MSG_MC_DISCONNECT_PORT_LEN);
        if (NULL == pBuf)
        {
          return SLIMBUS_ERR_RETRY;
        }
  
        SB_LOG_VERB_3("Got satellite disconnect port request "
                      "(satLA: 0x%x) (destLA: 0x%x) (port: %d)",
                      LA, destLA, portNum);

        *(pBuf++) = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_DISCONNECT_PORT_LEN,
                                          SB_MSG_MT_CORE, SB_MSG_MC_DISCONNECT_PORT,
                                          SB_MSG_DT_UNICAST, destLA) |
          SB_MSG_SET(portNum,DISCONNECT_PORT_0,PN);

        pDevCtxt->TxMsgFifo.sendBufFcn(pDevCtxt, &pDevCtxt->TxMsgFifo);

        pBuf = pDevCtxt->TxMsgFifo.getBufFcn(NULL, NULL, &pDevCtxt->TxMsgFifo, 
                                             SB_MSG_MC_USR_GENERIC_ACK_LEN);
        if (NULL == pBuf)
        {
          return SLIMBUS_ERR_RETRY;
        }
        
        *(pBuf++) = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_USR_GENERIC_ACK_LEN,
                                          SB_MSG_MT_SRC_REFERRED_USER,
                                          SB_MSG_MC_USR_GENERIC_ACK,
                                          SB_MSG_DT_UNICAST,
                                          LA) |
          SB_MSG_SET(TID,USR_GENERIC_ACK_0,TID);
        *pBuf = SB_MSG_SET(TRUE,USR_GENERIC_ACK_1,RESP);
        pDevCtxt->TxMsgFifo.sendBufFcn(pDevCtxt, &pDevCtxt->TxMsgFifo);

        RemovePortConnectionTracker2(pDevCtxt, pDevice, portNum);
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
  else if ( SB_MSG_MT_SRC_REFERRED_USER == eMT )
  {
    if ( SB_MSG_MC_USR_REPORT_QC_SATELLITE == eUsrMC &&
         length >= SB_MSG_MC_USR_REPORT_QC_SATELLITE_MIN_LEN )
    {
      uint32 *pBuf;
      uint8 LA = (uint8) SB_MSG_GET(data,GENERIC_0,LA);
      uint32 uMagic = 
        (SB_MSG_GET(data,USR_REPORT_QC_SATELLITE_0,MAGIC_LSB)) | 
        (SB_MSG_GET(pDataBuf[0],USR_REPORT_QC_SATELLITE_1,MAGIC_MSB)<<
         (32-SB_MSG_MC_USR_REPORT_QC_SATELLITE_0__MAGIC_LSB_SHFT));
      uint32 uProtoVer = SB_MSG_GET(pDataBuf[0],USR_REPORT_QC_SATELLITE_1,PROTO_VER);
      uint32 uSatProto = SB_MSG_GET(pDataBuf[0],USR_REPORT_QC_SATELLITE_1,SAT_PROTO);
      SlimBusDeviceType *pDevice = pDevCtxt->pDevices[LA&SB_LA_IDX_MASK];

      if ( uMagic == SB_QC_MASTER_PROTOCOL_MAGIC &&
           uProtoVer >= 1 &&
           NULL != pDevice )
      {
        uSatProto &= SB_QC_SUPPORTED_PROTOCOLS;

        if ( 0 == uSatProto )
        {
          return SB_ERROR;
        }

        if ( NULL == pDevice->pSat )
        {
          if (pDevCtxt->bNoMalloc)
          {
            /* If we're in ISR then we can't alloc new struct */
            return SLIMBUS_ERR_RETRY;
          }
          SlimBus_AllocSatStruct(pDevice);
        }
        if (NULL == pDevice->pSat)
        {
          /* pSat should have been allocated above, if not there return error */
          return SB_ERROR;
        }
        pDevice->pSat->uSatProtoVer = uSatProto;
        
        if (!pDevice->pSat->bExpectAnnouce)
        {
          pBuf = pDevCtxt->TxMsgFifo.getBufFcn(NULL, NULL, &pDevCtxt->TxMsgFifo, 
                                               SB_MSG_MC_USR_QC_MASTER_CAPABILITY_LEN);
          if (NULL == pBuf )
          {
            return SLIMBUS_ERR_RETRY;
          }

          pBuf[0] = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_USR_QC_MASTER_CAPABILITY_LEN,
                                          SB_MSG_MT_SRC_REFERRED_USER,
                                          SB_MSG_MC_USR_QC_MASTER_CAPABILITY,
                                          SB_MSG_DT_UNICAST,
                                          LA) |
            SB_MSG_SET(SB_QC_MASTER_PROTOCOL_MAGIC_LSB,USR_QC_MASTER_CAPABILITY_0,MAGIC_LSB);
          pBuf[1] = 
            SB_MSG_SET(SB_QC_MASTER_PROTOCOL_MAGIC_MSB,USR_QC_MASTER_CAPABILITY_1,MAGIC_MSB) |
            SB_MSG_SET(SB_QC_MASTER_CAPABILITY_VER,USR_QC_MASTER_CAPABILITY_1,PROTO_VER) |
            SB_MSG_SET(uProtoVer,USR_QC_MASTER_CAPABILITY_1,SAT_PROTO);

          pDevCtxt->TxMsgFifo.sendBufFcn(pDevCtxt, &pDevCtxt->TxMsgFifo);
        }
        
        SB_LOG_INFO_2("Received report satellite message (LA: 0x%x) (uSatProto: 0x%lx)",
                      pDevice->LA, uSatProto);
        
        pDevice->pSat->bExpectAnnouce = FALSE;
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
  else
  {
    result = SLIMBUS_ERR_UNHANDLED_MSG;
  }

  return result;
}

/**
 * @brief Queue master message event, send message and wait for 
 *        the event
 *
 * This function queues the event structure for a 
 * master-workloop generated message, sends the message and 
 * waits for the message completion event. 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] pBuf  Pointer to the message buffer
 * 
 * @return SLIMBUS_ERR_NONE on success, error code on error 
 */
SlimBusErrorType SlimBus_MasterSendAndWaitForMsgEvent
(
  SlimBusDevCtxt *pDevCtxt,
  uint32 *pBuf
)
{
  SlimBusEventNodeType *pEvent = PopHeadEvent(&pDevCtxt->pSched->FreeSchedEventStack);
  SlimBusErrorType eMsgErr = SLIMBUS_ERR_NONE;
  SlimBusEventHandle hEvent = pDevCtxt->pSched->hSchedEvent;
  SBResult result = SB_SUCCESS;
  
  if (NULL == pEvent)
  {
    SB_LOG_ERR_0("Error popping head of FreeSchedEventStack");
    return SLIMBUS_ERR_OUT_OF_RESOURCES;
  }

  RemoveEventNode(pEvent);
  pEvent->hEvent = hEvent;
  pEvent->pTxBuf = pBuf;
  pEvent->pTxPhysBuf = NULL;
  pEvent->peMsgErr = &eMsgErr;
  pEvent->uRxMarker = 0;
  pEvent->pRxBuf = NULL;
  pEvent->pRxBufSize = NULL;
  PushTailEvent(&pDevCtxt->TxEventQueue, pEvent);

  pDevCtxt->TxMsgFifo.sendBufFcn(pDevCtxt, &pDevCtxt->TxMsgFifo);

  SlimBus_EventReset(hEvent);
  SlimBus_UnlockDevice(pDevCtxt);

  /* Wait for the completion event */
  result = SlimBus_MasterEventTimedWait(pDevCtxt, hEvent, SB_DEFAULT_CLIENT_TIMEOUT);
  if (SB_SUCCESS != result && 
      SLIMBUS_ERR_NONE == eMsgErr)
  { 
    eMsgErr = SLIMBUS_ERR_MSG_RESP_TIMEOUT;
  }

  SlimBus_LockDevice(pDevCtxt);

  return eMsgErr;
}

/**
 * @brief Is the Slimbus idle and ready to enter pause clock
 *
 * This function checks whether the Slimbus driver is idle and 
 * ready to enter the pause clock state 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return TRUE if ready to enter pause clock, FALSE otherwise 
 */
static SBBOOL SlimBus_ReadyForPause(SlimBusDevCtxt *pDevCtxt)
{
  if ( SlimBus_ReadyForLowPowerMode(pDevCtxt) &&
       !SlimBus_IsReconfigNeeded(pDevCtxt, NULL) &&
       !pDevCtxt->pSched->bEnumPending &&
       !pDevCtxt->pSched->bCheckFrmHandSat &&
       pDevCtxt->pSched->bEnablePause &&
       ( !pDevCtxt->pSched->bDataSlotsScheduled || !pDevCtxt->bActiveFramer ) )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/**
 * @brief Get a buffer and perform additional checks for a 
 *        message to be sent for a reconfiguration sequence.
 *
 * This function gets a buffer for a message to be sent for a
 * reconfiguration sequence.  If this message is the first to be
 * sent for a reconfiguration sequence, then get a buffer for 
 * and queue a BEGIN_RECONFIGURATION message. 
 * 
 * @param[in] pTxMsgFifo  Pointer to the Tx message FIFO 
 *       structure
 * @param[in] uMsgSizeBytes  The size of the message to send
 * @param[in,out] puTotalBytes  Pointer to the location that 
 *       stores the total number of bytes sent so far for the
 *       reconfiguration sequence
 * 
 * @return Pointer to the allocated buffer
 */
uint32* SlimBus_GetReconfigTxBuf
(
  SlimBusMsgFifoType *pTxMsgFifo,
  uint32 uMsgSizeBytes,
  uint32 *puTotalBytes
)
{
  uint32 *pTxBuf = NULL;
  
  if ( 0 == *puTotalBytes )
  {
    pTxBuf = pTxMsgFifo->getBufFcn(NULL, NULL, pTxMsgFifo,
                                   SB_MSG_MC_BEGIN_RECONFIGURATION_LEN);
    if (NULL == pTxBuf )
    {
      return NULL;
    }
    *pTxBuf = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_BEGIN_RECONFIGURATION_LEN,
                                    SB_MSG_MT_CORE,
                                    SB_MSG_MC_BEGIN_RECONFIGURATION,
                                    SB_MSG_DT_BROADCAST, 0);
    *puTotalBytes += SB_MSG_MC_BEGIN_RECONFIGURATION_LEN;
  }
  
  pTxBuf = pTxMsgFifo->getBufFcn(NULL, NULL, pTxMsgFifo, uMsgSizeBytes);
  if (NULL == pTxBuf )
  {
    return NULL;
  }
  *puTotalBytes += uMsgSizeBytes;
  
  return pTxBuf;
}

/**
 * @brief Send an updated data channel definition
 *
 * This function sends a NEXT_DEFINE_CHANNEL message for a data 
 * channel if the channel is new or has been modified as a 
 * result of the reconfiguration sequence 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] pParam  Pointer to the data channel definition
 * @param[in] uNewClkGear  The new clock gear 
 */
static SBResult SendUpdatedChannelDef
(
  SlimBusDevCtxt *pDevCtxt,
  SlimBusChannelParamType *pParam,
  uint32 uNewClkGear,
  uint32 *puTotalReconfigBytes
)
{
  SlimBusMsgFifoType *pTxMsgFifo = &pDevCtxt->TxMsgFifo;
  uint32 *pTxBuf;

  uint16 usSegDist = 0;
  uint32 uExpShft = SB_MAX_CLK_GEAR - uNewClkGear;
  uint8 ucCurExp = (uint8) (pParam->ucRootExp + uExpShft);

  if ( pParam->eStatus != SB_CHANNEL_PENDING_REMOVAL &&
       ( pParam->eStatus != SB_CHANNEL_ACTIVE ||
         pParam->uNewInterval != pParam->uCurInterval ||
         pParam->uNewOffset != pParam->uCurOffset ||
         pParam->uNewRefCnt > pParam->uCurRefCnt ||
         pParam->uNewDataLine != pParam->uCurDataLine ) )
  {
    usSegDist = (pParam->uNewOffset << ucCurExp) & 0x1ff;
    usSegDist |= 0x200;
    usSegDist >>= ucCurExp;
    if ( pParam->eCoeff == SB_RATE_COEFF_1 )
    {
      usSegDist |= (pParam->uNewOffset << (ucCurExp+1)) & 0xc00;
    }
    else
    {
      usSegDist |= 0xc00;
    }
  
    pTxBuf = SlimBus_GetReconfigTxBuf(pTxMsgFifo,
                                      SB_MSG_MC_NEXT_DEFINE_CHANNEL_LEN,
                                      puTotalReconfigBytes);          
    if (NULL == pTxBuf)
    {
      return SB_ERROR;
    }
    
    *(pTxBuf++) = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_NEXT_DEFINE_CHANNEL_LEN,
                                        SB_MSG_MT_CORE,
                                        SB_MSG_MC_NEXT_DEFINE_CHANNEL, 
                                        SB_MSG_DT_BROADCAST, 0) |
                  SB_MSG_SET(pParam->channelNum,NEXT_DEFINE_CHANNEL_0,CN) |
                  SB_MSG_SET(usSegDist,NEXT_DEFINE_CHANNEL_0,SD_LSB);
    
    *(pTxBuf++) = (SB_MSG_SET( (usSegDist >> (32 - SB_MSG_MC_NEXT_DEFINE_CHANNEL_0__SD_LSB_SHFT)), NEXT_DEFINE_CHANNEL_1, SD_MSB ) |
                   SB_MSG_SET( pParam->ucProtocol, NEXT_DEFINE_CHANNEL_1, TP ) |
                   SB_MSG_SET( pParam->ucSegLen, NEXT_DEFINE_CHANNEL_1, SL )   | 
                   SB_MSG_SET( pParam->uNewDataLine, NEXT_DEFINE_CHANNEL_1, DL ));
  }

  return SB_SUCCESS;
}


/**
 * @brief Remove a channel from a channel parameter list
 *
 * This function removes a channel from a channel parameter 
 * list, and updating group flags. 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] pParam  Pointer to a channel parameter data structure
 */
static void RemoveChannelParam(SlimBusDevCtxt *pDevCtxt, SlimBusChannelParamType *pParam)
{
  SlimBusLinkNodeType *pRoot;

  /* Source clean up first */
  RemovePortConnectionTracker(pDevCtxt, pParam->pSrcNode);

  /* Sink clean up next */
  while (pParam->pSinkList != NULL)
  {
    /*Logic added to set the dirty bit for sync port, when channel 
      associated with the port scheduling equation met with the  
      scenario of HWBUG QCTDD03351991.*/
    if(pParam->pSinkList->pDevice != NULL &&
       pParam->pSinkList->pDevice->LA == pDevCtxt->pgdLA)
    {
      uint32 portnum = pParam->pSinkList->portNum;
      uint32 idx = portnum - pDevCtxt->uBasePortNum;
      if( idx < pDevCtxt->numPorts &&
          pParam->uCurOffset + pParam->ucSegLen == pParam->uCurInterval )
      {
        pDevCtxt->Ports[idx].bDataShiftRemoval = TRUE;
        SB_LOG_INFO_1("Set datashift workaround bit (port %d)", portnum);
      }
    }

    RemovePortConnectionTracker(pDevCtxt, pParam->pSinkList);
  } 

  /* Choose the list to add to based on the channel rate coefficient */
  if (pParam->eCoeff == SB_RATE_COEFF_1)
  {
    pRoot = &pDevCtxt->pSched->ChanParamList1;
  }
  else
  {
    pRoot = &pDevCtxt->pSched->ChanParamList3;
  }


  if ((pParam->uGrpFlag & (SB_GROUP_FLAG_START|SB_GROUP_FLAG_END)) == SB_GROUP_FLAG_START)
  {
    /* Set group start flag in next channel if channel to remove is start of a group */
    SlimBusChannelParamType *pNextParam = NextParamNode(pRoot, pParam);
    if (NULL != pNextParam)
    {
      /* pNextParam should never be NULL */
      pNextParam->uGrpFlag |= SB_GROUP_FLAG_START;
    }
  }
  else if ((pParam->uGrpFlag & (SB_GROUP_FLAG_START|SB_GROUP_FLAG_END)) == SB_GROUP_FLAG_END)
  {
    /* Set group end flag in previous channel if channel to remove is end of a group */
    SlimBusChannelParamType *pPrevParam = PrevParamNode(pRoot, pParam);
    if (NULL != pPrevParam)
    {
      /* pPrevParam should never be NULL */
      pPrevParam->uGrpFlag |= SB_GROUP_FLAG_END;
    }
  }
  
  pParam->eStatus = SB_CHANNEL_INACTIVE;
  pParam->uDataLineMask = 0;
  pParam->uNumDataLines = 1;
  pParam->uCurDataLine = pParam->uNewDataLine = 0;
  pParam->uCurRefCnt = pParam->uNewRefCnt = 0;
  pParam->iClkGearDrift = 0;
  pParam->iMaxClkGearDrift = 0;
  RemoveParamNode(pParam);
}

/**
 * @brief Prepare to remove a channel from a channel parameter 
 *        list
 *
 * This function prepares to remove a channel from a channel 
 * parameter list. 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] pParam  Pointer to a channel parameter data structure
 */
static void PrepareRemoveChannelParam(SlimBusDevCtxt *pDevCtxt, SlimBusChannelParamType *pParam, SBBOOL *pbRemove)
{
  if ( pParam->uNewRefCnt > 0 )
  {
    pParam->uNewRefCnt--;

    if ( pParam->uNewRefCnt > 0 )
    {
      if (NULL != pbRemove)
      {
        *pbRemove = FALSE;
      }
    }
    else
    {
      pParam->eStatus = SB_CHANNEL_PENDING_REMOVAL;

      if (NULL != pbRemove)
      {
        *pbRemove = TRUE;
      }
    }
  }
  else
  {
    if (NULL != pbRemove)
    {
      *pbRemove = FALSE;
    }
  }
}

/**
 * @brief Sort the data channels within a group so that first 
 *        channel in the group is scheduled first in the frame
 *        structure
 *
 * This function sorts all the data channels within a channel 
 * group, such that the first channel in the group comes first 
 * in the frame schedule (has the smallest segment offset), 
 * second channel comes second, etc.
 * 
 * @param[in] pRoot  Pointer to the channel list root node
 * @param[in] pParam  Pointer to the channel parameter structure
 *       for the first channel in the group
 */
void SortChannelGroup(SlimBusLinkNodeType *pRoot, SlimBusChannelParamType *pParam)
{
  /* Initialize uLastIdx to MAXINT */
  uint32 uLastIdx = (uint32)-1;
  uint32 uSecondIdx = 0;
  SlimBusChannelParamType *pFirstParam, *pSecondParam;
  
  for ( ; uLastIdx > 0; uLastIdx-- )
  {
    pFirstParam = pParam;
    pSecondParam = NextParamNode(pRoot, pFirstParam);
    
    for (uSecondIdx = 1; (uSecondIdx <= uLastIdx && pSecondParam != NULL); uSecondIdx++)
    {
      if (pFirstParam->uNewOffset < pSecondParam->uNewOffset)
      {
        uint32 uTempOffset = pSecondParam->uNewOffset;
        pSecondParam->uNewOffset = pFirstParam->uNewOffset;
        pFirstParam->uNewOffset = uTempOffset;
      }
      if (pSecondParam->uGrpFlag & SB_GROUP_FLAG_END)
      {
        uLastIdx = uSecondIdx;
        break;
      }
      pFirstParam = pSecondParam;
      pSecondParam = NextParamNode(pRoot, pSecondParam);
    }
    if (NULL == pSecondParam)
    {
      uLastIdx = uSecondIdx-1;
      if (0 == uLastIdx)
      {
        break;
      }
    }
  }
}

/**
 * @brief Determine whether any local clients have active data
 *        channels
 *
 * This function determines whether any local clients have 
 * active data channels 
 * 
 * @param[in] paClientCtxt  Pointer to array of client context 
 *       pointers
 * @param[in] uNumClientSlots  Number of slots in the array 
 * 
 * @return TRUE if any local clients have active data channels, 
 *         FALSE otherwise
 */
static SBBOOL SlimBus_AreLocalChannelsActive
(
  SlimBusClientCtxt **paClientCtxt,
  uint32 uNumClientSlots
)
{
  uint32 uClientIdx, uIdx;
  
  for (uClientIdx = 0; uClientIdx < uNumClientSlots; uClientIdx++)
  {
    SlimBusClientCtxt *pClientCtxt = paClientCtxt[uClientIdx];

    if (NULL != pClientCtxt)
    {
      for (uIdx = 0; uIdx < SB_MAX_DATA_CHANNEL_PARAMS/BITS_PER_UINT32; uIdx++)
      {
        if (0 != pClientCtxt->BasicClient.uaChanBitMap[uIdx])
        {
          return TRUE;
        }
      }
    }
  }
  
  return FALSE;
}

/**
 * @brief Determine whether any satellite clients have active 
 *        data channels
 *
 * This function determines whether any clients corresponding to
 * a particular satellite have active data channels 
 * 
 * @param[in] paClientCtxt  Pointer to array of client context 
 *       pointers
 * @param[in] uNumClientSlots  Number of slots in the array 
 * 
 * @return TRUE if any clients have active data channels, 
 *         FALSE otherwise
 */
SBBOOL SlimBus_AreSatChannelsActive
(
  SlimBusBasicClientCtxt **paClientCtxt,
  uint32 uNumClientSlots
)
{
  uint32 uClientIdx, uIdx;
  
  for (uClientIdx = 0; uClientIdx < uNumClientSlots; uClientIdx++)
  {
    SlimBusBasicClientCtxt *pClientCtxt = paClientCtxt[uClientIdx];

    if (NULL != pClientCtxt)
    {
      for (uIdx = 0; uIdx < SB_MAX_DATA_CHANNEL_PARAMS/BITS_PER_UINT32; uIdx++)
      {
        if (0 != pClientCtxt->uaChanBitMap[uIdx])
        {
          return TRUE;
        }
      }
    }
  }
  
  return FALSE;
}

/**
 * @brief Check whether framer handover should be initiated
 *
 * This function checks whether framer handover should be 
 * initiated. 
 * 
 * @param[in] pDevCtxt  Pointer to the device context 
 *  
 * @return TRUE if framer handover is indicated, FALSE otherwise
 */
static SBBOOL SlimBus_IsFramerHandoverIndicated(SlimBusDevCtxt *pDevCtxt)
{
  SlimBusDeviceType *pFrmDevice = pDevCtxt->pDevices[pDevCtxt->pSched->FrmHandFrmLA&SB_LA_IDX_MASK];
  SlimBusDeviceType *pNgdDevice = pDevCtxt->pDevices[pDevCtxt->pSched->FrmHandNgdLA&SB_LA_IDX_MASK];

  if ( NULL != pFrmDevice && 
       SB_ENUM_ACTIVE == pFrmDevice->eState &&
       pDevCtxt->pSched->FrmHandFrmLA == pFrmDevice->LA &&
       NULL != pNgdDevice && 
       SB_ENUM_ACTIVE == pNgdDevice->eState &&
       pDevCtxt->pSched->FrmHandNgdLA == pNgdDevice->LA &&
       NULL != pNgdDevice->pSat )
  {
    if (pDevCtxt->bActiveFramer)
    {
      if ( !SlimBus_AreLocalChannelsActive(pDevCtxt->paClientCtxt, pDevCtxt->numClientSlots) &&
           SlimBus_AreSatChannelsActive(pNgdDevice->pSat->paClientCtxt, pNgdDevice->pSat->numClientSlots) )
      {
        return TRUE;
      }
    }
    else
    {
      if ( !SlimBus_AreSatChannelsActive(pNgdDevice->pSat->paClientCtxt, pNgdDevice->pSat->numClientSlots) )
      {
        return TRUE;
      }
    }
  }
  else if (!pDevCtxt->bActiveFramer)
  {
    /* If framer handover device has not enumerated but yet we are inactive framer,
       switch the framer back */
    return TRUE;
  }

  return FALSE;
}

/**
 * @brief Trigger client callbacks for clock gear change 
 *        notifications
 *
 * This function triggers client callbacks for clock gear change 
 * notifications 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] uOldClkGear  The old clock gear
 * @param[in] uSubFrmCode  The new subframe code
 * @param[in] UpdateType  Whether the notification is before or 
 *       after the clock gear has changed
 */
static void TriggerClockGearCallbacks
(
  SlimBusDevCtxt *pDevCtxt,
  uint32 uOldClkGear,
  uint32 uSubFrmCode,
  SlimBusSchedParamUpdateType UpdateType
)
{
  if ( pDevCtxt->uNewClkGear != uOldClkGear )
  {
    SlimBusSchedParamsType SchedParams;
    uint32 i;

    SchedParams.uSubFrmCode = uSubFrmCode;
    SchedParams.UpdateType = UpdateType;
    SchedParams.uClkGear = pDevCtxt->uNewClkGear;
    SchedParams.uPrevClkGear = uOldClkGear;

    for (i = 0; i < pDevCtxt->numClientSlots; i++)
    {
      SlimBusClientCtxt *pCtxt = pDevCtxt->paClientCtxt[i];
      if ( NULL != pCtxt )
      {
        SlimBusEventHandle hEvent = pCtxt->hClockGearEvent;

        if ( NULL != hEvent )
        {
          SlimBus_UnlockDevice(pDevCtxt);
          SlimBus_EventTriggerEx(hEvent, &SchedParams);
          SlimBus_LockDevice(pDevCtxt);
        }
      }
    }
  }
}

/**
 * @brief Process the reconfiguration sequence for the Next* 
 *        messages queued by the client
 *
 * This function processes the reconfiguration sequence for the 
 * Next* messages queued by the client 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] pBasicClient  Pointer to the base client context
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
_IRQL_requires_(DISPATCH_LEVEL)
static SBResult SlimBus_ProcessReconfigSeq
(
  SlimBusDevCtxt *pDevCtxt,
  SlimBusBasicClientCtxt *pBasicClient
)
{
  uint32 i;
  SBResult result = SB_SUCCESS;
  SlimBusMsgFifoType *pTxMsgFifo = &pDevCtxt->TxMsgFifo;
  uint32 *pTxBuf, *pCfgBuf;
  uint32 uSubFrmCode = pDevCtxt->pSched->uSubFrmCode;
  uint32 uClkGear = pDevCtxt->uMinClkGear;
  uint32 uOldClkGear = pDevCtxt->uFrmClkGear;
  SBBOOL bAllowFailure = TRUE;
  SBBOOL bReadyForPause = FALSE;
  SBBOOL bRefreshFramer = FALSE;
  SBBOOL bSkipSched = FALSE;
  uint32 uTotalReconfigBytes = 0;
  
  if ( pDevCtxt->pSched->bPurge ||
       SB_FRM_HANDOVER_IDLE != pDevCtxt->pSched->eFrmHandState ||
       ( NULL != pBasicClient &&
         ( pBasicClient->ConfigMsgFifo.uHeadIdx != pBasicClient->ConfigMsgFifo.uTailIdx ||
           pBasicClient->bPurgeNeeded ) ) ||
       ( bReadyForPause = (pDevCtxt->bActiveFramer && SlimBus_ReadyForPause(pDevCtxt)) ) )
  {
    SB_LOG_VERB_0("Processing reconfiguration sequence");

    do
    {
      if ( pDevCtxt->pSched->bPurge )
      {
        uint32 uChanNum, uClientNum, uDevIdx;

        SB_LOG_INFO_0("Purging all data channels");
        
        bReadyForPause = FALSE;
        
        /* If we are purging we have to remove all the channels from
           internal book keeping, even if we encounter an error */
        bAllowFailure = FALSE;
        
        pDevCtxt->pSched->bPurge = FALSE;
        for (uChanNum = 0; uChanNum < SB_MAX_DATA_CHANNEL_PARAMS; uChanNum++)
        {
          SlimBusChannelParamType *pParam = pDevCtxt->pSched->apChanParams[uChanNum];
          if ( NULL != pParam &&
               SB_CHANNEL_INACTIVE != pParam->eStatus )
          {
            while ( pParam->uNewRefCnt > 0 )
            {
              PrepareRemoveChannelParam(pDevCtxt, pParam, NULL);
            }
          }
          
          pTxBuf = SlimBus_GetReconfigTxBuf(pTxMsgFifo,
                                            SB_MSG_MC_NEXT_ACTIVATE_CHANNEL_LEN,
                                            &uTotalReconfigBytes);          
          if (NULL == pTxBuf )
          {
            result = SB_ERROR;
            break;
          }

          *pTxBuf = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_NEXT_ACTIVATE_CHANNEL_LEN,
                                          SB_MSG_MT_CORE, SB_MSG_MC_NEXT_REMOVE_CHANNEL,
                                          SB_MSG_DT_BROADCAST, uChanNum);
          
          /* Send each remove channel individually to prevent exhausting the 
             Tx FIFO during the flush */
          SlimBus_MasterSendAndWaitForMsgEvent(pDevCtxt, pTxBuf);
        }
        
        for (uClientNum = 0; uClientNum < pDevCtxt->numClientSlots; uClientNum++)
        {
          SlimBusClientCtxt *pCtxt = pDevCtxt->paClientCtxt[uClientNum];
          
          if (NULL != pCtxt)
          {
            memset(pCtxt->BasicClient.uaChanBitMap, 0,
                   sizeof(pCtxt->BasicClient.uaChanBitMap));
            pCtxt->BasicClient.uCurReservedMsgSlots = 0;
            pCtxt->BasicClient.uNewReservedMsgSlots = 0;
          }
        }
        
        for (uDevIdx = 0; uDevIdx < pDevCtxt->numDeviceSlots; uDevIdx++)
        {
          if (NULL != pDevCtxt->pDevices[uDevIdx] &&
              NULL != pDevCtxt->pDevices[uDevIdx]->pSat)
          {
            SlimBusSatelliteType *pSat = pDevCtxt->pDevices[uDevIdx]->pSat;
            
            for (uClientNum = 0; uClientNum < pSat->numClientSlots; uClientNum++)
            {
              SlimBusBasicClientCtxt *pCtxt = pSat->paClientCtxt[uClientNum];
              
              if (NULL != pCtxt)
              {
                memset(pCtxt->uaChanBitMap, 0, sizeof(pCtxt->uaChanBitMap));
                pCtxt->uCurReservedMsgSlots = pCtxt->uNewReservedMsgSlots = 0;
              }
            }
          }
        }

        pDevCtxt->pSched->uMsgSlotReq = SB_INTERNAL_RESV_MSG_SLOTS;
      }
      else if ( NULL != pBasicClient &&
                pBasicClient->bPurgeNeeded )
      {
        uint32 uChanNum;

        bReadyForPause = FALSE;
        
        /* If we are purging we have to remove all the channels from
           internal book keeping, even if we encounter an error */
        bAllowFailure = FALSE;
        pBasicClient->bPurgeNeeded = FALSE;
        for (uChanNum = 0; uChanNum < SB_MAX_DATA_CHANNEL_PARAMS; uChanNum++)
        {
          SlimBusChannelParamType *pParam = pDevCtxt->pSched->apChanParams[uChanNum];
          if ( NULL != pParam &&
               SB_IS_CHAN_BITMAP_SET(pBasicClient->uaChanBitMap,uChanNum) &&
               SB_CHANNEL_INACTIVE != pParam->eStatus )
          {
            pTxBuf = SlimBus_GetReconfigTxBuf(pTxMsgFifo,
                                              SB_MSG_MC_NEXT_ACTIVATE_CHANNEL_LEN,
                                              &uTotalReconfigBytes);          
            if (NULL == pTxBuf )
            {
              result = SB_ERROR;
              break;
            }

            *pTxBuf = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_NEXT_ACTIVATE_CHANNEL_LEN,
                                            SB_MSG_MT_CORE, SB_MSG_MC_NEXT_REMOVE_CHANNEL,
                                            SB_MSG_DT_BROADCAST, uChanNum);
            PrepareRemoveChannelParam(pDevCtxt, pParam, NULL);
            SB_CLEAR_CHAN_BITMAP(pBasicClient->uaChanBitMap,uChanNum);
          }
        }

        pDevCtxt->pSched->uMsgSlotReq -= pBasicClient->uCurReservedMsgSlots;
        pBasicClient->uCurReservedMsgSlots = pBasicClient->uNewReservedMsgSlots = 0;

        if (SB_TID_NOACK != pBasicClient->uConfigTID)
        {
          /* if TID is not NOACK, then client wants to send real reconfig, do it next */
          PushHeadClient(&pDevCtxt->ReconfigList, pBasicClient);
        }
      }
      else if ( NULL != pBasicClient )
      {
        SlimBusMsgFifoType *pCfgMsgFifo = &pBasicClient->ConfigMsgFifo;
        bReadyForPause = FALSE;
        
        while ( NULL != (pCfgBuf = SlimBus_PopBuffer(pCfgMsgFifo, NULL)) )
        {
          uint32 uData = *pCfgBuf;
          uint32 length = SB_MSG_GET(uData,GENERIC_0,LEN);
          uint8 CfgMC = SB_MSG_GET(uData,GENERIC_0,MC);
  
          if ( CfgMC == SB_MSG_MC_USR_CHANNEL_CONTROL )
          {
            SlimBusChannelCtrlType eCtrl =
              (SlimBusChannelCtrlType) SB_MSG_GET(uData,USR_CHANNEL_CONTROL_0,CTRL);
            uint8 TxMC;
  
            if ( eCtrl == SLIMBUS_CHAN_CTRL_ACTIVATE )
            {
              TxMC = SB_MSG_MC_NEXT_ACTIVATE_CHANNEL;
            }
            else if ( eCtrl == SLIMBUS_CHAN_CTRL_DEACTIVATE )
            {
              TxMC = SB_MSG_MC_NEXT_DEACTIVATE_CHANNEL;
            }
            else
            {
              TxMC = SB_MSG_MC_NEXT_REMOVE_CHANNEL;
            }
  
            {
              uint8 *pucBuf = (uint8*)&pCfgBuf[0] + SB_MSG_MC_USR_CHANNEL_CONTROL__CN_OFFSET;
          
              for ( i = SB_MSG_MC_USR_CHANNEL_CONTROL__CN_OFFSET; i < length; i++ )
              {
                uint8 uChanNum = *(pucBuf++);
                SlimBusChannelParamType *pParam;
                SBBOOL bSendMsg = TRUE;
  
                pParam = pDevCtxt->pSched->apChanParams[uChanNum];
  
                if ( !SB_IS_CHAN_BITMAP_SET(pBasicClient->uaChanBitMap,uChanNum) )
                {
                  SB_LOG_WARN_1("Got invalid request to control channel not owned "
                                "by client (channel: %ld)", uChanNum);
                  continue;
                }
                
                if ( SB_MSG_MC_NEXT_REMOVE_CHANNEL == TxMC &&
                     NULL != pParam )
                {
                  if ( pParam->eStatus == SB_CHANNEL_ACTIVE )
                  {
                    PrepareRemoveChannelParam(pDevCtxt, pParam, &bSendMsg);
                    SB_CLEAR_CHAN_BITMAP(pBasicClient->uaChanBitMap,uChanNum);
                  }
                  else if ( pParam->eStatus == SB_CHANNEL_NEW ||
                            pParam->eStatus == SB_CHANNEL_UPDATED )
                  {
                    SB_LOG_ERR_1("Got invalid request to control channel not "
                                 "in active state (channel: %ld)", uChanNum);
                    result = SB_ERROR;
                    break;
                  }
                }
  
                if (bSendMsg)
                {
                  pTxBuf = SlimBus_GetReconfigTxBuf(pTxMsgFifo,
                                                    SB_MSG_MC_NEXT_ACTIVATE_CHANNEL_LEN,
                                                    &uTotalReconfigBytes);          
                  if (NULL == pTxBuf )
                  {
                    result = SB_ERROR;
                    break;
                  }
              
                  *pTxBuf = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_NEXT_ACTIVATE_CHANNEL_LEN,
                                                  SB_MSG_MT_CORE, TxMC,
                                                  SB_MSG_DT_BROADCAST, uChanNum);
                }
              }
            }
          }
          if ( CfgMC == SB_MSG_MC_USR_CHANNEL_DEF ||
               CfgMC == SB_MSG_MC_USR_CHANNEL_DEF_ACTIVATE )
          {
            static const uint8 uaDrvToHalProto[SLIMBUS_PROTO_NUM_ENUMS] =
            {
              0, 0, 1, 2, 4, 5, 6, 7
            };
            SlimBusDataFormatType eDF =
              (SlimBusDataFormatType) SB_MSG_GET(uData,USR_CHANNEL_DEF_0,DF);
            uint8 ucDataLen = SB_MSG_GET(pCfgBuf[1],USR_CHANNEL_DEF_1,DL);
            SlimBusRateCoeffType eCoeff =
              (SlimBusRateCoeffType) SB_MSG_GET(pCfgBuf[1],USR_CHANNEL_DEF_1,COEFF);
            SlimBusSideBandFormatType eSF =
              (SlimBusSideBandFormatType) SB_MSG_GET(pCfgBuf[1],USR_CHANNEL_DEF_1,SF);
            SlimBusProtocolType eProto =
              (SlimBusProtocolType) SB_MSG_GET(pCfgBuf[1],USR_CHANNEL_DEF_1,PROTO);
            uint8 ucProto = 0;
            uint32 uExp = SB_MSG_GET(pCfgBuf[1],USR_CHANNEL_DEF_1,EXP);
            uint8 ucPR = (uint8) SB_MSG_GET(pCfgBuf[1],USR_CHANNEL_DEF_1,PR);
            uint8 ucSegLen = ucDataLen;
            uint8 *pucBuf = ((uint8*)pCfgBuf)+SB_MSG_MC_USR_CHANNEL_DEF__CN_OFFSET;
            uint32 uNumChans = length-SB_MSG_MC_USR_CHANNEL_DEF__CN_OFFSET;
      
            if ((uint32)eProto < SLIMBUS_PROTO_NUM_ENUMS)
            {
              ucProto = uaDrvToHalProto[eProto];
            }
            else
            {
              result = SB_ERROR;
              break;
            }
            
            if ( eSF == SLIMBUS_SIDEBAND_FORMAT_IEC60958_TUNNELING )
            {
              ucSegLen++;
            }
            if ( eProto != SLIMBUS_PROTO_HARD_ISO )
            {
              ucSegLen++;
            }
            if ( eProto >= SLIMBUS_PROTO_EXT_ASYNC_SIMPLEX )
            {
              ucSegLen++;
            }

            for ( i = SB_MSG_MC_USR_CHANNEL_DEF__CN_OFFSET; i < length; i++ )
            {
              uint8 ucChanNum = *(pucBuf++);
              SlimBusChannelParamType *pChanParam = SlimBus_GetChanParam( pDevCtxt, ucChanNum );
            
              if ( pChanParam->eStatus == SB_CHANNEL_INACTIVE )
              {
                pChanParam->ucSegLen = ucSegLen;
                pChanParam->ucProtocol = ucProto;
                pChanParam->eCoeff = eCoeff;
                pChanParam->ucRootExp = (uint8)uExp;
                pChanParam->eStatus = SB_CHANNEL_NEW;
                pChanParam->uNewRefCnt = 1;
                pChanParam->uGrpFlag = 0;
                pChanParam->uGrpTotalSlots = SlimBus_GetSlotsUsed(pChanParam) * uNumChans;
                if (i == SB_MSG_MC_USR_CHANNEL_DEF__CN_OFFSET)
                {
                  /* Note: when insertion sorting, the first node inserted 
                           will end up last once all nodes are inserted */
                  pChanParam->uGrpFlag |= SB_GROUP_FLAG_END;
                }
                if (i == length-1)
                {
                  pChanParam->uGrpFlag |= SB_GROUP_FLAG_START;
                }
                AddChannelParam(pDevCtxt, pChanParam);
                SB_SET_CHAN_BITMAP(pBasicClient->uaChanBitMap,pChanParam->channelNum);
              }
              else if ( pChanParam->eStatus == SB_CHANNEL_PENDING_REMOVAL )
              {
                SB_LOG_ERR_1("Got invalid request to define channel pending removal "
                             "(channel: %ld)", pChanParam->channelNum);
                result = SB_ERROR;
                break;
              }
              else
              {
                if ( pChanParam->ucSegLen == ucSegLen &&
                     pChanParam->ucProtocol == ucProto &&
                     pChanParam->eCoeff == eCoeff &&
                     pChanParam->ucRootExp == (uint8)uExp )
                {
                  if ( !SB_IS_CHAN_BITMAP_SET(pBasicClient->uaChanBitMap,pChanParam->channelNum) )
                  {
                    if ( pChanParam->channelNum >= pDevCtxt->uBaseSharedChanNum &&
                         pChanParam->channelNum < pDevCtxt->uBaseSharedChanNum+pDevCtxt->numSharedChannelSlots )
                    {
                      pChanParam->uNewRefCnt++;
                      SB_SET_CHAN_BITMAP(pBasicClient->uaChanBitMap,pChanParam->channelNum);
                    }
                    else
                    {
                      SB_LOG_ERR_1("Got invalid request to define non-shared channel "
                                   "not owned by client (channel: %ld)", pChanParam->channelNum);
                      result = SB_ERROR;
                      break;
                    }
                  }
                  
                  if ( pChanParam->eStatus == SB_CHANNEL_ACTIVE )
                  {
                    pChanParam->eStatus = SB_CHANNEL_UPDATED;
                  }
                }
                else
                {
                  /* Don't allow channel parameters to change after setup */
                  SB_LOG_ERR_1("Got invalid request to redefine active channel "
                               "(channel: %ld)", pChanParam->channelNum);
                  result = SB_ERROR;
                  break;
                }
              }
            
              if ( pChanParam->eStatus == SB_CHANNEL_NEW ||
                   pChanParam->eStatus == SB_CHANNEL_UPDATED )
              {
                /* only define content for first definition */
                pTxBuf = SlimBus_GetReconfigTxBuf(pTxMsgFifo,
                                                  SB_MSG_MC_NEXT_DEFINE_CONTENT_LEN,
                                                  &uTotalReconfigBytes);          
                if (NULL == pTxBuf )
                {
                  result = SB_ERROR;
                  break;
                }

                *(pTxBuf++) = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_NEXT_DEFINE_CONTENT_LEN,
                                                    SB_MSG_MT_CORE,
                                                    SB_MSG_MC_NEXT_DEFINE_CONTENT,
                                                    SB_MSG_DT_BROADCAST, 0) |
                  SB_MSG_SET(ucChanNum,NEXT_DEFINE_CONTENT_0,CN) |
                  SB_MSG_SET(ucPR,NEXT_DEFINE_CONTENT_0,PR);
                *(pTxBuf++) = 
                  SB_MSG_SET(eDF,NEXT_DEFINE_CONTENT_1,DF) |
                  SB_MSG_SET(eSF,NEXT_DEFINE_CONTENT_1,SF) |
                  SB_MSG_SET(ucDataLen,NEXT_DEFINE_CONTENT_1,DL);
              }
            }
          }
          if ( CfgMC == SB_MSG_MC_USR_CHANNEL_DEF_ACTIVATE )
          {
            uint8 *pucBuf = ((uint8*)pCfgBuf)+SB_MSG_MC_USR_CHANNEL_DEF__CN_OFFSET;

            for ( i = SB_MSG_MC_USR_CHANNEL_DEF__CN_OFFSET; i < length; i++ )
            {
              uint8 uChanNum = *(pucBuf++);
  
              pTxBuf = SlimBus_GetReconfigTxBuf(pTxMsgFifo,
                                                SB_MSG_MC_NEXT_ACTIVATE_CHANNEL_LEN,
                                                &uTotalReconfigBytes);          
              if (NULL == pTxBuf )
              {
                result = SB_ERROR;
                break;
              }
  
              *pTxBuf = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_NEXT_ACTIVATE_CHANNEL_LEN,
                                              SB_MSG_MT_CORE, 
                                              SB_MSG_MC_NEXT_ACTIVATE_CHANNEL,
                                              SB_MSG_DT_BROADCAST, uChanNum);
            }
          }
          if ( CfgMC == SB_MSG_MC_USR_REQ_MSG_BANDWIDTH )
          {
            uint32 uMsgSlots = 
              SB_MSG_GET(pCfgBuf[0],USR_REQ_MSG_BANDWIDTH_0,MSG_SLOTS_LSB) | 
              (SB_MSG_GET(pCfgBuf[1],USR_REQ_MSG_BANDWIDTH_1,MSG_SLOTS_MSB) <<
               (32-SB_MSG_MC_USR_REQ_MSG_BANDWIDTH_0__MSG_SLOTS_LSB_SHFT));
      
            pDevCtxt->pSched->uMsgSlotReq += 
              uMsgSlots - pBasicClient->uNewReservedMsgSlots;
            pBasicClient->uNewReservedMsgSlots = uMsgSlots;
          }
          if (SB_SUCCESS != result)
          {
            break;
          }
        }
      }
      else if ( SB_FRM_HANDOVER_IDLE != pDevCtxt->pSched->eFrmHandState )
      {
        bReadyForPause = FALSE;
        
        if ( SB_FRM_HANDOVER_PREPARE == pDevCtxt->pSched->eFrmHandState )
        {
          SB_LOG_INFO_0("Preparing for framer handover");

          /* Advance the next state */
          pDevCtxt->pSched->eFrmHandState = SB_FRM_HANDOVER_EXECUTE;

          uClkGear = SB_MAX_CLK_GEAR-1;
          bRefreshFramer = TRUE;
        }
        else if ( SB_FRM_HANDOVER_EXECUTE == pDevCtxt->pSched->eFrmHandState )
        {
          SlimBusMsgFifoType *pTxMsgFifo = &pDevCtxt->TxMsgFifo;
          SlimBusLogicalAddrType LAIF;
          uint32 *pTxBuf;

          SB_LOG_INFO_0("Executing framer handover");

          /* Advance the next state */
          pDevCtxt->pSched->eFrmHandState = SB_FRM_HANDOVER_CLEANUP;

          uClkGear = pDevCtxt->uFrmClkGear;
          bSkipSched = TRUE;

          if (pDevCtxt->bActiveFramer)
          {
            LAIF = pDevCtxt->pSched->FrmHandFrmLA;
          }
          else
          {
            LAIF = (SlimBusLogicalAddrType)HAL_sb_FrmGetAssignedLA(pDevCtxt->hSB);
          }
          
          pTxBuf = SlimBus_GetReconfigTxBuf(pTxMsgFifo,
                                            SB_MSG_MC_NEXT_ACTIVE_FRAMER_LEN,
                                            &uTotalReconfigBytes);          
          if (NULL == pTxBuf )
          {
            result = SB_ERROR;
            break;
          }

          *pTxBuf = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_NEXT_ACTIVE_FRAMER_LEN,
                                              SB_MSG_MT_CORE,
                                              SB_MSG_MC_NEXT_ACTIVE_FRAMER,
                                              SB_MSG_DT_BROADCAST,
                                           0);
          *(pTxBuf++) |= ( SB_MSG_SET(LAIF,NEXT_ACTIVE_FRAMER_0,LAIF) |
                           SB_MSG_SET(6/*NCo*/,NEXT_ACTIVE_FRAMER_0,NCO) );
          *(pTxBuf++) = SB_MSG_SET(5/*NCi*/,NEXT_ACTIVE_FRAMER_1,NCI);

        }
        else
        {
          SB_LOG_INFO_0("Cleaning up after framer handover");

          /* Advance the next state */
          pDevCtxt->pSched->eFrmHandState = SB_FRM_HANDOVER_IDLE;
        }
      }
      else if ( bReadyForPause )
      {
        SlimBusMsgFifoType *pTxMsgFifo = &pDevCtxt->TxMsgFifo;
        uint32 *pTxBuf;

        SB_LOG_INFO_0("Initiating transition to idle pause clock "
                      "at next reconfig boundary");

        HAL_sb_CompEnableRetention(pDevCtxt->hSB);

        pTxBuf = SlimBus_GetReconfigTxBuf(pTxMsgFifo,
                                          SB_MSG_MC_NEXT_PAUSE_CLOCK_LEN,
                                          &uTotalReconfigBytes);          
        if (NULL == pTxBuf )
        {
          result = SB_ERROR;
          break;
        }

        *pTxBuf = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_NEXT_PAUSE_CLOCK_LEN,
                                        SB_MSG_MT_CORE,
                                        SB_MSG_MC_NEXT_PAUSE_CLOCK,
                                        SB_MSG_DT_BROADCAST,
                                        SB_NPC_UNSPECIFIED_DELAY);

        bSkipSched = TRUE;
      }
      else
      {
        return SB_ERROR;
      }
    } while (FALSE);

    if ( SB_SUCCESS == result &&
         !bSkipSched )
    {
      SlimBusLinkNodeType *pList, *pNextList;
      SlimBusChannelParamType *pParam = NULL;
      uint32 uGroupDataLine = 0;
      
      /* Zero-out the number of used data channel slots */      
      pDevCtxt->pSched->aDataLineSchedules[0].uNewSlotsUsed = pDevCtxt->pSched->uMsgSlotReq;
      for ( i = 1; i < SB_MAX_DATA_LINES; i++)
      {
        pDevCtxt->pSched->aDataLineSchedules[i].uNewSlotsUsed = 0;
      }
      
      /* Assign data lines that are constrained to a single data line first */
      pList = &pDevCtxt->pSched->ChanParamList1;
      pNextList = &pDevCtxt->pSched->ChanParamList3;
      for (; pList != NULL; pList = pNextList, pNextList = NULL)
      {
        pParam = NULL;
        while ( NULL != (pParam = NextParamNode(pList, pParam)) )
        {
          if (pDevCtxt->uFrmClkGear > uClkGear)
          {
            if (pParam->eStatus != SB_CHANNEL_PENDING_REMOVAL &&
                pParam->iClkGearDrift >= pParam->iMaxClkGearDrift)
            {
              uClkGear = pDevCtxt->uFrmClkGear;
              SB_LOG_INFO_3( "Setting minimum clock gear limit due to max allowed "
                             "clock drift (channel: %d) (drift: %i) (allowed: %i)",
                             pParam->channelNum, pParam->iClkGearDrift,
                             pParam->iMaxClkGearDrift );
            }
          }
          if (pParam->eStatus != SB_CHANNEL_PENDING_REMOVAL &&
              pParam->uNumDataLines <= 1)
          {
            SlimBus_AssignDataLine( pDevCtxt, pParam, &uGroupDataLine);
          }
        }
      }
      /* Assign data lines that are not constrained to a single data line next */
      pList = &pDevCtxt->pSched->ChanParamList1;
      pNextList = &pDevCtxt->pSched->ChanParamList3;
      for (; pList != NULL; pList = pNextList, pNextList = NULL)
      {
        int32 iMinExp = (PeekHeadParam(pList) != NULL) ? PeekHeadParam(pList)->ucRootExp : 0;
        int32 iCurExp = (PeekTailParam(pList) != NULL) ? PeekTailParam(pList)->ucRootExp : 0;
        
        /* Assign data lines to channels starting from high frequency channels
           and going to lower frequency channels */
        for ( ; iCurExp >= iMinExp; iCurExp--)
        {
          pParam = NULL;
          while ( NULL != (pParam = NextParamNode(pList, pParam)) )
          {
            if (pParam->eStatus != SB_CHANNEL_PENDING_REMOVAL &&
                pParam->ucRootExp == (uint32) iCurExp &&
                pParam->uNumDataLines > 1)
            {
              SlimBus_AssignDataLine( pDevCtxt, pParam, &uGroupDataLine);
            }
          }
        }
      }

      result = SlimBus_DoBandwidthAlloc(pDevCtxt, &uSubFrmCode, &uClkGear);
    }
    else
    {
      uClkGear = pDevCtxt->uFrmClkGear;
    }
    
    if (SB_SUCCESS == result)
    {
      do
      {
        SlimBusChannelParamType *pParam = NULL;
        SlimBusLinkNodeType *pList, *pNextList;
        SlimBusMsgFifoType *pTxMsgFifo = &pDevCtxt->TxMsgFifo;
        uint32 *pTxBuf;
        SlimBusErrorType eMsgErr = SLIMBUS_ERR_NONE;

        pDevCtxt->pSched->bDataSlotsScheduled = FALSE;
    
        if (pDevCtxt->pSched->aDataLineSchedules[0].uNewSlotsUsed > pDevCtxt->pSched->uMsgSlotReq)
        {
          pDevCtxt->pSched->bDataSlotsScheduled = TRUE;
        }
        else
        {
          for ( i = 1; i < SB_MAX_DATA_LINES; i++)
          {
            if(pDevCtxt->pSched->aDataLineSchedules[i].uNewSlotsUsed > 0) 
            {
              pDevCtxt->pSched->bDataSlotsScheduled = TRUE;
              break;
            }
          }
        }

        if ( pDevCtxt->pSched->bEnablePause &&
             !pDevCtxt->pSched->bDataSlotsScheduled &&
             uClkGear < pDevCtxt->uDefClkGear )
        {
          /* Set the clock gear to default if we have no data channels and
             enable pause clock */
          uClkGear = pDevCtxt->uDefClkGear;
        }
      
        /* If the subframe mode has changed since the previous reconfiguration 
           sequence, we need to send out a subframe mode update message */
        if ( uSubFrmCode != pDevCtxt->pSched->uSubFrmCode ||
             bRefreshFramer )
        {
          pTxBuf = SlimBus_GetReconfigTxBuf(pTxMsgFifo,
                                            SB_MSG_MC_NEXT_SUBFRAME_MODE_LEN,
                                            &uTotalReconfigBytes);          
          if (NULL == pTxBuf )
          {
            result = SB_ERROR;
            break;
          }

          *pTxBuf = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_NEXT_SUBFRAME_MODE_LEN,
                                          SB_MSG_MT_CORE,
                                          SB_MSG_MC_NEXT_SUBFRAME_MODE,
                                          SB_MSG_DT_BROADCAST,
                                          uSubFrmCode);
        }

        /* If the clock gear has changed since the previous reconfiguration
           sequence, we need to send out a clock gear update message */
        if ( uClkGear != pDevCtxt->uFrmClkGear ||
             bRefreshFramer )
        {
          pTxBuf = SlimBus_GetReconfigTxBuf(pTxMsgFifo,
                                            SB_MSG_MC_NEXT_CLOCK_GEAR_LEN,
                                            &uTotalReconfigBytes);          
          if (NULL == pTxBuf )
          {
            result = SB_ERROR;
            break;
          }

          *pTxBuf = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_NEXT_CLOCK_GEAR_LEN,
                                          SB_MSG_MT_CORE,
                                          SB_MSG_MC_NEXT_CLOCK_GEAR,
                                          SB_MSG_DT_BROADCAST,
                                          uClkGear);
          
          pDevCtxt->uNewClkGear = uClkGear;
        }

        /* Sort all channels that belong to a group, and send out update
           channel definitions for data channels that are new or have changed */
        pList = &pDevCtxt->pSched->ChanParamList1;
        pNextList = &pDevCtxt->pSched->ChanParamList3;
        for ( ; pList != NULL; pList = pNextList, pNextList = NULL )
        {
          while ( NULL != (pParam = NextParamNode(pList, pParam)) )
          {
            if ((pParam->uGrpFlag&(SB_GROUP_FLAG_START|SB_GROUP_FLAG_END))==SB_GROUP_FLAG_START)
            {
              SortChannelGroup(pList, pParam);
            }
            if (SB_SUCCESS != SendUpdatedChannelDef(pDevCtxt, pParam, uClkGear, &uTotalReconfigBytes))
            {
              result = SB_ERROR;
            }
          }
        }
        
        if (SB_SUCCESS != result)
        {
          break;
        }
      
        if (0 == uTotalReconfigBytes)
        {
          SB_LOG_INFO_0("Skipping transmission of empty reconfiguration sequence");
        }
        else
        {
          pTxMsgFifo->sendBufFcn(pDevCtxt, pTxMsgFifo);
  
          /* Trigger callbacks to client registered clock gear change callbacks */
          TriggerClockGearCallbacks(pDevCtxt, uOldClkGear, uSubFrmCode,
                                    SLIMBUS_SCHED_PARAM_BEFORE_UPDATE);
          
          pTxBuf = pTxMsgFifo->getBufFcn(NULL, NULL, pTxMsgFifo, 
                                         SB_MSG_MC_RECONFIGURE_NOW_LEN);
          if (NULL == pTxBuf )
          {
            result = SB_ERROR;
            break;
          }
  
          pDevCtxt->pSched->bReconfigBusy = TRUE;
  
          if ( bReadyForPause )
          {
            pDevCtxt->pSched->ePause = SB_PAUSE_ENTERING_PAUSE;
          }
  
          if ( uClkGear > pDevCtxt->uFrmClkGear )
          {
            /* Perform voltage control before reconfig if clock gear is increasing */
            SLIMBUS_VOLTAGE_CTRL(pDevCtxt, uClkGear);
          }
          
          *pTxBuf = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_RECONFIGURE_NOW_LEN,
                                          SB_MSG_MT_CORE,
                                          SB_MSG_MC_RECONFIGURE_NOW,
                                          SB_MSG_DT_BROADCAST, 0);
  
          eMsgErr = SlimBus_MasterSendAndWaitForMsgEvent(pDevCtxt, pTxBuf);
          
          if ( pDevCtxt->pSched->bReconfigBusy &&
               SLIMBUS_ERR_NONE == eMsgErr )
          {
            SlimBus_EventReset(pDevCtxt->pSched->hReconfigEvent);
            SlimBus_UnlockDevice(pDevCtxt);

            /* Wait for the completion event */
            if (SB_SUCCESS != SlimBus_MasterEventTimedWait(pDevCtxt,
                                                           pDevCtxt->pSched->hReconfigEvent,
                                                           SB_RECONFIG_TIMEOUT))
            { 
              eMsgErr = SLIMBUS_ERR_MSG_RESP_TIMEOUT;
            }

            SlimBus_LockDevice(pDevCtxt);
          }

          if (pDevCtxt->pSched->bReconfigBusy ||
              SLIMBUS_ERR_MSG_RESP_TIMEOUT == eMsgErr ||
              pDevCtxt->bPendingSSR)
          {
            SB_LOG_ERR_0("Got NACK or timeout during reconfig seq, aborting!");
            result = SB_ERROR;
          }

          pDevCtxt->pSched->bReconfigBusy = FALSE;
        }
      } while (FALSE);
    }
  
    if ( SB_SUCCESS == result ||
         !bAllowFailure )
    {
      SlimBusChannelParamType *pParam = NULL;
      SlimBusLinkNodeType *pList, *pNextList;
      int32 iClkDriftNew = 1, iClkDriftOld = 1;

      pDevCtxt->pSched->uSubFrmCode = uSubFrmCode;

      if ( pDevCtxt->uNewClkGear < uOldClkGear )
      {
        iClkDriftNew = iClkDriftOld = 3;
      }
      iClkDriftNew <<= (SB_MAX_CLK_GEAR - pDevCtxt->uNewClkGear);
      iClkDriftOld <<= (SB_MAX_CLK_GEAR - uOldClkGear);
      
      /* Go ahead and actually remove the channels that are pending removal,
         now that we know that we have sucessfully scheduled the updated channels;
         also update the active channel parameters */
      pList = &pDevCtxt->pSched->ChanParamList1;
      pNextList = &pDevCtxt->pSched->ChanParamList3;
      for ( ; pList != NULL; pList = pNextList, pNextList = NULL )
      {
        pParam = NextParamNode(pList, pParam);
        while ( NULL != pParam )
        {
          SlimBusChannelParamType *pNextParam;
          pNextParam = NextParamNode(pList, pParam);

          if ( pParam->eStatus == SB_CHANNEL_PENDING_REMOVAL )
          {
            RemoveChannelParam(pDevCtxt, pParam);
          }
          else
          {
            if ( pParam->eStatus != SB_CHANNEL_NEW &&
                 pDevCtxt->uNewClkGear != uOldClkGear )
            {
              pParam->iClkGearDrift += iClkDriftNew - iClkDriftOld;
            }

            pParam->uCurInterval = pParam->uNewInterval;
            pParam->uCurOffset = pParam->uNewOffset;
            pParam->uCurRefCnt = pParam->uNewRefCnt;
            pParam->uCurDataLine = pParam->uNewDataLine;

            pParam->eStatus = SB_CHANNEL_ACTIVE;

            SB_LOG_VERB_5("Active channel parameters (channel#: %ld) (dl: %ld) "
                          "(interval: %ld) (offset: %ld) (refcnt: %ld)",
                          pParam->channelNum, pParam->uCurDataLine, 
                          pParam->uCurInterval, pParam->uCurOffset, pParam->uCurRefCnt);
          }
          pParam = pNextParam;
        }
      }

      for (i = 0; i < SB_MAX_DATA_LINES; i++)
      {
        uint32 uNewSlotsUsed = pDevCtxt->pSched->aDataLineSchedules[i].uNewSlotsUsed;

        pDevCtxt->pSched->aDataLineSchedules[i].uCurSlotsUsed = uNewSlotsUsed;

        if (0 == i)
        {
          SB_LOG_VERB_4("Bandwidth utilization stats on primary line (used data+msg slots: %ld+%ld / 1536) "
                        "(CG: %ld) (SM: %ld)", uNewSlotsUsed-pDevCtxt->pSched->uMsgSlotReq,
                        pDevCtxt->pSched->uMsgSlotReq, uClkGear, uSubFrmCode);
        }
        else
        {
          if(uNewSlotsUsed > 0) 
          {
            SB_LOG_VERB_2("Bandwidth utilization stats on secondary line #%ld (used data slots: %ld / 1536)",
                          i, uNewSlotsUsed);
          }
        }
      }

      if (NULL != pBasicClient)
      {
        pBasicClient->uCurReservedMsgSlots = pBasicClient->uNewReservedMsgSlots;
      }

      /* Trigger callbacks to client registered clock gear change callbacks */
      TriggerClockGearCallbacks(pDevCtxt, uOldClkGear, uSubFrmCode,
                                SLIMBUS_SCHED_PARAM_AFTER_UPDATE);
    }
    else
    {
      SlimBusChannelParamType *pParam = NULL;
      uint32 uIdx;
      SlimBusEventNodeType *pEventNode;

      SB_LOG_WARN_0("Unwinding reconfiguration sequence due to error");
      
      pDevCtxt->uNewClkGear = pDevCtxt->uFrmClkGear;

      HAL_sb_CompDisableRetention(pDevCtxt->hSB);

      pDevCtxt->pSched->ePause = SB_PAUSE_NOT_PAUSED;
      while (NULL != (pEventNode = PopHeadEvent(&pDevCtxt->PauseEventQueue)))
      {
        SlimBus_EventTrigger(pEventNode->hEvent);
      }
      
      for ( uIdx = 0; uIdx < SB_MAX_DATA_CHANNEL_PARAMS; uIdx++)
      {
        pParam = pDevCtxt->pSched->apChanParams[uIdx];
        if ( NULL != pParam )
        {
          if (NULL != pBasicClient)
          {
            if (pParam->uNewRefCnt > pParam->uCurRefCnt)
            {
              SB_CLEAR_CHAN_BITMAP(pBasicClient->uaChanBitMap,pParam->channelNum);
            }
            else if (pParam->uNewRefCnt < pParam->uCurRefCnt)
            {
              SB_SET_CHAN_BITMAP(pBasicClient->uaChanBitMap,pParam->channelNum);
            }
          }

          if ( pParam->eStatus == SB_CHANNEL_NEW )
          {
            RemoveChannelParam(pDevCtxt, pParam);
          }
          else if ( pParam->eStatus == SB_CHANNEL_PENDING_REMOVAL )
          {
            pParam->uNewRefCnt = pParam->uCurRefCnt;
            pParam->eStatus = SB_CHANNEL_ACTIVE;
          }
          else if ( pParam->eStatus != SB_CHANNEL_INACTIVE )
          {
            pParam->uNewDataLine = pParam->uCurDataLine;
            pParam->uNewInterval = pParam->uCurInterval;
            pParam->uNewOffset = pParam->uCurOffset;
            pParam->uNewRefCnt = pParam->uCurRefCnt;
            pParam->eStatus = SB_CHANNEL_ACTIVE;
          }
        }
      }
      
      for (i = 0; i < SB_MAX_DATA_LINES; i++)
      {
        pDevCtxt->pSched->aDataLineSchedules[i].uNewSlotsUsed = pDevCtxt->pSched->aDataLineSchedules[i].uCurSlotsUsed;
      }
      
      if (NULL != pBasicClient)
      {
        pDevCtxt->pSched->uMsgSlotReq +=
          pBasicClient->uCurReservedMsgSlots - pBasicClient->uNewReservedMsgSlots;
        pBasicClient->uNewReservedMsgSlots = pBasicClient->uCurReservedMsgSlots;
      }
    }

    pDevCtxt->pSched->bDataSlotsScheduled = FALSE;

    if (pDevCtxt->pSched->aDataLineSchedules[0].uNewSlotsUsed > pDevCtxt->pSched->uMsgSlotReq)
    {
      pDevCtxt->pSched->bDataSlotsScheduled = TRUE;
    }
    else
    {
      for ( i = 1; i < SB_MAX_DATA_LINES; i++)
      {
        if(pDevCtxt->pSched->aDataLineSchedules[i].uNewSlotsUsed > 0) 
        {
          pDevCtxt->pSched->bDataSlotsScheduled = TRUE;
          break;
        }
      }
    }
    
    /* Make sure voltage control is up to date following reconfig */
    SLIMBUS_VOLTAGE_CTRL(pDevCtxt, pDevCtxt->uFrmClkGear);
    
    /* Check for framer handover transitioning */
    if (SB_FRM_HANDOVER_IDLE == pDevCtxt->pSched->eFrmHandState)
    {
      if ( SlimBus_IsFramerHandoverIndicated(pDevCtxt) )
      {
        pDevCtxt->pSched->eFrmHandState = SB_FRM_HANDOVER_PREPARE;

        /* If switching to external framer, check to ensure that framer
           is still up and responsive */
        if (pDevCtxt->bActiveFramer)
        {
          uint32 *pTxBuf;

          pTxBuf = pTxMsgFifo->getBufFcn(NULL, NULL, pTxMsgFifo,
                                         SB_MSG_MC_CHANGE_VALUE__CM_OFFSET+1);

          pTxBuf[0] = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_CHANGE_VALUE__CM_OFFSET+1,
                                            SB_MSG_MT_CORE,
                                            SB_MSG_MC_REQUEST_INFORMATION,
                                            SB_MSG_DT_UNICAST,
                                            pDevCtxt->pSched->FrmHandFrmLA);
          pTxBuf[1] = 0x0088;
          if (SLIMBUS_ERR_NONE != SlimBus_MasterSendAndWaitForMsgEvent(pDevCtxt, pTxBuf))
          {
            SlimBusDeviceType *pFrmDevice = 
              pDevCtxt->pDevices[pDevCtxt->pSched->FrmHandFrmLA&SB_LA_IDX_MASK];

            SB_LOG_WARN_1("Aborting handover to external framer "
                          "since external framer is non-responsive (LA: 0x%x)",
                          pDevCtxt->pSched->FrmHandFrmLA);

            pDevCtxt->pSched->eFrmHandState = SB_FRM_HANDOVER_IDLE;
            if (NULL != pFrmDevice)
            {
              pFrmDevice->eState = SB_ENUM_RESERVED;
            }
          }
        }
      }
    }
  }
  
  if (pDevCtxt->bPendingSSR)
  {
    /* Signal the SSR thread in case it is waiting for master workloop to go idle */
    SlimBus_EventTrigger(pDevCtxt->pSched->hReconfigEvent);
  }
  
  return result;
}

/**
 * @brief Process the reconfiguration sequence for the Next* 
 *        messages queued by the local client
 *
 * This function processes the reconfiguration sequence for the 
 * Next* messages queued by the local client 
 * 
 * @param[in] pClientCtxt  Pointer to the client context 
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
_IRQL_requires_(DISPATCH_LEVEL)
static SBResult SlimBus_ProcessLocalReconfigSeq
(
  SlimBusClientCtxt *pClientCtxt
)
{
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SBResult result = SB_SUCCESS;

  /* Queue our reconfiguration sequence for the workloop to process */

  /* Create event handle again to grab current thread ID */
  SlimBus_EventReset(pClientCtxt->hClientEvent);
  PushTailClient(&pDevCtxt->ReconfigList, &pClientCtxt->BasicClient);

  if ( SB_PAUSE_NOT_PAUSED == pDevCtxt->pSched->ePause &&
       !pDevCtxt->pSched->bReconfigBusy )
  {
    /* signal the reconfiguration workloop */
    SlimBus_EventTrigger(pDevCtxt->hMasterWorkLoopEvent);
  }

  SlimBus_UnlockDevice(pDevCtxt);

  result = SlimBus_EventTimedWait(pClientCtxt,
                                  pClientCtxt->hClientEvent,
                                  SB_DEFAULT_CLIENT_TIMEOUT);

  SlimBus_LockDevice(pDevCtxt);

  if (SB_SUCCESS == result)
  {
    result = pClientCtxt->eLastReconfigErr;
  }
  else
  {
    /* If there was wait error then we need to do cleanup */
    RemoveClientNode(&pClientCtxt->BasicClient);
  }

  return result;
}

/**
 * @brief Send logical address assignment and master capability 
 *        messages.
 *
 * This function sends logical address assignment messages for 
 * devicse that have reported present, and sends master 
 * capability messages to potential satellite drivers. 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
static SBResult SlimBus_SendLogicalAddressAssignments(SlimBusDevCtxt *pDevCtxt)
{
  uint32 uIdx;

  pDevCtxt->pSched->bEnumPending = FALSE;

  for (uIdx = 0; uIdx < pDevCtxt->numDeviceSlots; uIdx++)
  {
    SlimBusDeviceType *pDevice = pDevCtxt->pDevices[uIdx];

    if ( NULL != pDevice &&
         pDevice->eState == SB_ENUM_PRESENT )
    {
      uint32 uAckRetryCnt;
      SlimBusErrorType eMsgErr;
      uint32 *pBuf = NULL;
      
      pDevice->eState = SB_ENUM_ACTIVE;
      while (pDevice->uAvailTrans < pDevice->uMaxTrans)
      {
        SlimBusEventNodeType *pEventNode = PopHeadEvent(&pDevice->TransEventQueue);
        if (NULL != pEventNode)
        {
          SlimBus_EventTrigger(pEventNode->hEvent);
        }
        pDevice->uAvailTrans++;
      }

      for ( uAckRetryCnt = 0, eMsgErr = SLIMBUS_ERR_MSG_RESP_TIMEOUT;
            uAckRetryCnt < SB_RECONFIG_TIMEOUT_COUNT && eMsgErr != SLIMBUS_ERR_NONE;
            uAckRetryCnt++ )
      {
        pBuf = pDevCtxt->TxMsgFifo.getBufFcn(NULL, NULL,
                                             &pDevCtxt->TxMsgFifo, 
                                             SB_MSG_MC_ASSIGN_LOGICAL_ADDRESS_LEN);
        if (NULL == pBuf )
        {
          return SB_ERROR;
        }

        /* Send assign logical addr message */
        pBuf[0] = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_ASSIGN_LOGICAL_ADDRESS_LEN,
                                          SB_MSG_MT_CORE,
                                          SB_MSG_MC_ASSIGN_LOGICAL_ADDRESS,
                                          SB_MSG_DT_UNICAST,
                                          0) |
          (pDevice->aEA[5] <<16) |
          (pDevice->aEA[4] <<24);

        pBuf[1] =
          (pDevice->aEA[3] << 0) |
          (pDevice->aEA[2] << 8) |
          (pDevice->aEA[1] << 16) |
          (pDevice->aEA[0] << 24);
        pBuf[2] =
          SB_MSG_SET(pDevice->LA,ASSIGN_LOGICAL_ADDRESS_2,LA);

        eMsgErr = SlimBus_MasterSendAndWaitForMsgEvent(pDevCtxt, pBuf);

        if (SLIMBUS_ERR_NONE != eMsgErr)
        {
          SB_LOG_ERR_2("Got nack for assign logical address message (EA: 0x%04lx%08lx)",
                       (((uint32)pDevice->aEA[5]<<8)|(uint32)pDevice->aEA[4]),
                       (((uint32)pDevice->aEA[3]<<24)|((uint32)pDevice->aEA[2]<<16)|
                        ((uint32)pDevice->aEA[1]<<8)|(uint32)pDevice->aEA[0]));
          SlimBus_UnlockDevice(pDevCtxt);
          SLIMBUS_SLEEP(SB_NACK_RETRY_DELAY);
          SlimBus_LockDevice(pDevCtxt);
        }
      }

      /* If the device is a QC device and one of the NGDs */
      if ( pDevice->aEA[5] == 0x2 &&
           pDevice->aEA[4] == 0x17 &&
           ( pDevice->aEA[1] == 0x3 ||
             pDevice->aEA[1] == 0x4 ) )
      {
        if ( NULL != pDevice->pSat )
        {
          pDevice->pSat->bSendAnnounce = TRUE;
        }
      }
      
      if (pDevice->LA == pDevCtxt->pSched->FrmHandNgd2LA)
      {
        SlimBusLogicalAddrType NgdLA = pDevCtxt->pSched->FrmHandNgdLA;
        SlimBusLogicalAddrType FrmLA = pDevCtxt->pSched->FrmHandFrmLA;

        SB_LOG_INFO_2("Detected secondary external framer frmLA=0x%x ngdLA=0x%x",
                      pDevCtxt->pSched->FrmHandFrm2LA,
                      pDevCtxt->pSched->FrmHandNgd2LA);
        
        pDevCtxt->pSched->FrmHandNgdLA = pDevCtxt->pSched->FrmHandNgd2LA;
        pDevCtxt->pSched->FrmHandFrmLA = pDevCtxt->pSched->FrmHandFrm2LA;
        pDevCtxt->pSched->FrmHandNgd2LA = NgdLA;
        pDevCtxt->pSched->FrmHandFrm2LA = FrmLA;
      }
    }
  }
  
  for (uIdx = 0; uIdx < pDevCtxt->numDeviceSlots; uIdx++)
  {
    SlimBusDeviceType *pDevice = pDevCtxt->pDevices[uIdx];

    /* If the device is a QC device and one of the NGDs */
    if ( NULL != pDevice &&
         SB_ENUM_ACTIVE == pDevice->eState &&
         pDevice->aEA[5] == 0x2 &&
         pDevice->aEA[4] == 0x17 &&
         ( pDevice->aEA[1] == 0x3 ||
           pDevice->aEA[1] == 0x4 ) )
    {
      uint32 uAckRetryCnt;
      SlimBusErrorType eMsgErr;
      uint32 *pBuf = NULL;

      if ( NULL == pDevice->pSat ||
           pDevice->pSat->bSendAnnounce )
      {
        if ( NULL == pDevice->pSat )
        {
          SlimBus_AllocSatStruct(pDevice);
        }
        else if (pDevCtxt->bActiveFramer)
        {
          uint32 uClientNum, uChanNum;

          for (uClientNum = 0; uClientNum < pDevice->pSat->numClientSlots; uClientNum++)
          {
            SlimBusBasicClientCtxt *pClientCtxt = pDevice->pSat->paClientCtxt[uClientNum];
            if ( NULL != pClientCtxt )
            {
              for (uChanNum = 0; uChanNum < SB_MAX_DATA_CHANNEL_PARAMS; uChanNum++)
              {
                SlimBusChannelParamType *pParam = pDevCtxt->pSched->apChanParams[uChanNum];
                if ( NULL != pParam &&
                     SB_IS_CHAN_BITMAP_SET(pClientCtxt->uaChanBitMap,uChanNum) &&
                     SB_CHANNEL_INACTIVE != pParam->eStatus )
                {
                  SB_LOG_INFO_2("Purging data channels for satellite client "
                                "(LA: 0x%x) (client: 0x%lx)", pDevice->LA, uClientNum);

                  PushTailClient(&pDevCtxt->ReconfigList, pClientCtxt);
                  pClientCtxt->uConfigTID = SB_TID_NOACK;
                  pClientCtxt->bPurgeNeeded = TRUE;
                  pClientCtxt->ConfigMsgFifo.uHeadIdx = 0;
                  pClientCtxt->ConfigMsgFifo.uTailIdx = 0;
                  break;
                }
              }
            }
          }
        }

        pDevice->pSat->bSendAnnounce = FALSE;
        pDevice->pSat->bExpectAnnouce = TRUE;

        for ( uAckRetryCnt = 0, eMsgErr = SLIMBUS_ERR_MSG_RESP_TIMEOUT;
              uAckRetryCnt < SB_RECONFIG_TIMEOUT_COUNT && eMsgErr != SLIMBUS_ERR_NONE;
              uAckRetryCnt++ )
        {
          /* Send the device a master capability message if we think it is a NGD */
          pBuf = pDevCtxt->TxMsgFifo.getBufFcn(NULL, NULL, &pDevCtxt->TxMsgFifo, 
                                               SB_MSG_MC_USR_QC_MASTER_CAPABILITY_LEN);
          if (NULL == pBuf )
          {
            return SB_ERROR;
          }

          pBuf[0] = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_USR_QC_MASTER_CAPABILITY_LEN,
                                          SB_MSG_MT_SRC_REFERRED_USER,
                                          SB_MSG_MC_USR_QC_MASTER_CAPABILITY,
                                          SB_MSG_DT_UNICAST,
                                          pDevice->LA) |
            SB_MSG_SET(SB_QC_MASTER_PROTOCOL_MAGIC_LSB,USR_QC_MASTER_CAPABILITY_0,MAGIC_LSB);
          pBuf[1] = 
            SB_MSG_SET(SB_QC_MASTER_PROTOCOL_MAGIC_MSB,USR_QC_MASTER_CAPABILITY_1,MAGIC_MSB) |
            SB_MSG_SET(SB_QC_MASTER_CAPABILITY_VER,USR_QC_MASTER_CAPABILITY_1,PROTO_VER) |
            SB_MSG_SET(SB_QC_SUPPORTED_PROTOCOLS,USR_QC_MASTER_CAPABILITY_1,SAT_PROTO);

          eMsgErr = SlimBus_MasterSendAndWaitForMsgEvent(pDevCtxt, pBuf);

          if (SLIMBUS_ERR_NONE != eMsgErr)
          {
            SB_LOG_ERR_0("Got nack for master capability message");
            SlimBus_UnlockDevice(pDevCtxt);
            SLIMBUS_SLEEP(SB_NACK_RETRY_DELAY);
            SlimBus_LockDevice(pDevCtxt);
          }
        }
      }
    }
  }
  
  return SB_SUCCESS;
}


/**
 * @brief Send manager power up/down notification to active 
 *        framer satellite
 *
 * This function sends power up/down notifications to any 
 * satellite drivers associated with an active framer device 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 */
static void SlimBus_SendManagerPowerNotifications(SlimBusDevCtxt *pDevCtxt)
{
  if ( !pDevCtxt->bActiveFramer )
  {
    SlimBusDeviceType *pNgdDevice = pDevCtxt->pDevices[pDevCtxt->pSched->FrmHandNgdLA&SB_LA_IDX_MASK];
    uint32 *pBuf = NULL;
    SBBOOL bReadyForPause = SlimBus_ReadyForPause(pDevCtxt);

    if ( NULL != pNgdDevice && 
         SB_ENUM_ACTIVE == pNgdDevice->eState &&
         pDevCtxt->pSched->FrmHandNgdLA == pNgdDevice->LA )
    {
      if ( bReadyForPause )
      {
        uint32 uAckRetryCnt = 0;
        SlimBusErrorType eMsgErr = SLIMBUS_ERR_MSG_RESP_TIMEOUT;

        pDevCtxt->pSched->ePause = SB_PAUSE_ENTERING_PAUSE;

        pDevCtxt->pSched->bSendPowerNotify = TRUE;

        for ( ; uAckRetryCnt < SB_RECONFIG_TIMEOUT_COUNT && eMsgErr != SLIMBUS_ERR_NONE; uAckRetryCnt++ )
        {
          pBuf = pDevCtxt->TxMsgFifo.getBufFcn(NULL, NULL, &pDevCtxt->TxMsgFifo,
                                               SB_MSG_MC_USR_NOTIFY_MGR_SLEEP_LEN);
          pBuf[0] = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_USR_NOTIFY_MGR_SLEEP_LEN,
                                          SB_MSG_MT_SRC_REFERRED_USER,
                                          SB_MSG_MC_USR_NOTIFY_MGR_SLEEP,
                                          SB_MSG_DT_UNICAST,
                                          pNgdDevice->LA) |
            SB_MSG_SET(pDevCtxt->pSched->uPowerNotifyTID,USR_NOTIFY_MGR_SLEEP,TID);

          SB_LOG_INFO_2("Sending manager powered down notification (LA: 0x%x) (TID: %d)",
                        pNgdDevice->LA, pDevCtxt->pSched->uPowerNotifyTID);

          eMsgErr = SlimBus_MasterSendAndWaitForMsgEvent(pDevCtxt, pBuf);

          if (SLIMBUS_ERR_NONE != eMsgErr)
          {
            SB_LOG_ERR_0("Got nack for manager powered down notification");
            SlimBus_UnlockDevice(pDevCtxt);
            SLIMBUS_SLEEP(SB_NACK_RETRY_DELAY);
            SlimBus_LockDevice(pDevCtxt);
          }
        }

        bReadyForPause = SlimBus_ReadyForPause(pDevCtxt);

        if ( !bReadyForPause )
        {
          SlimBusEventNodeType *pEventNode;
          
          pDevCtxt->pSched->ePause = SB_PAUSE_NOT_PAUSED;

          while (NULL != (pEventNode = PopHeadEvent(&pDevCtxt->PauseEventQueue)))
          {
            SlimBus_EventTrigger(pEventNode->hEvent);
          }
        }
      }

      if ( pDevCtxt->pSched->bSendPowerNotify &&
           SB_PAUSE_NOT_PAUSED == pDevCtxt->pSched->ePause &&
           SlimBus_HardwareIsEnumerated(pDevCtxt) )
      {
        uint32 uAckRetryCnt = 0;
        SlimBusErrorType eMsgErr = SLIMBUS_ERR_MSG_RESP_TIMEOUT;

        pDevCtxt->pSched->bSendPowerNotify = FALSE;

        for ( ; uAckRetryCnt < SB_RECONFIG_TIMEOUT_COUNT && eMsgErr != SLIMBUS_ERR_NONE; uAckRetryCnt++ )
        {
          pBuf = pDevCtxt->TxMsgFifo.getBufFcn(NULL, NULL, &pDevCtxt->TxMsgFifo,
                                               SB_MSG_MC_USR_NOTIFY_MGR_AWAKE_LEN);
          pBuf[0] = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_USR_NOTIFY_MGR_AWAKE_LEN,
                                          SB_MSG_MT_SRC_REFERRED_USER,
                                          SB_MSG_MC_USR_NOTIFY_MGR_AWAKE,
                                          SB_MSG_DT_UNICAST,
                                          pNgdDevice->LA) |
            SB_MSG_SET(pDevCtxt->pSched->uPowerNotifyTID,USR_NOTIFY_MGR_AWAKE_0,TID);
          pBuf[1] = SB_MSG_SET(pDevCtxt->bDeviceOpenTimeout,USR_NOTIFY_MGR_AWAKE_1,CODE);

          SB_LOG_INFO_2("Sending manager powered up notification (LA: 0x%x) (TID: %d)",
                        pNgdDevice->LA, pDevCtxt->pSched->uPowerNotifyTID);

          eMsgErr = SlimBus_MasterSendAndWaitForMsgEvent(pDevCtxt, pBuf);

          if (SLIMBUS_ERR_NONE != eMsgErr)
          {
            SB_LOG_ERR_0("Got nack for manager powered up notification");
            SlimBus_UnlockDevice(pDevCtxt);
            SLIMBUS_SLEEP(SB_NACK_RETRY_DELAY);
            SlimBus_LockDevice(pDevCtxt);
          }
        }

        pDevCtxt->bDeviceOpenTimeout = FALSE;
        pDevCtxt->pSched->uPowerNotifyTID++;
      }
    }

    if ( bReadyForPause )
    {
      pDevCtxt->bDeviceOpenTimeout = FALSE;
      pDevCtxt->pSched->ePause = SB_PAUSE_PAUSED;
    }
  }
}

/**
 * @brief Check whether the framer handover satellite is still 
 *        up.
 *
 * This function checks whether the satellite driver associated 
 * with the framer handover inactive framer is still up.
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
static void SlimBus_CheckFramerHandoverSat(SlimBusDevCtxt *pDevCtxt)
{
  SlimBusMsgFifoType *pTxMsgFifo = &pDevCtxt->TxMsgFifo;
  SlimBusDeviceType *pNgdDevice;
  uint32 *pTxBuf;
  uint32 uClientNum;

  pDevCtxt->pSched->bCheckFrmHandSat = FALSE;

  pNgdDevice = pDevCtxt->pDevices[pDevCtxt->pSched->FrmHandNgdLA&SB_LA_IDX_MASK];

  pTxBuf = pTxMsgFifo->getBufFcn(NULL, NULL, pTxMsgFifo,
                                 SB_MSG_MC_CHANGE_VALUE__CM_OFFSET+1);

  pTxBuf[0] = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_CHANGE_VALUE__CM_OFFSET+1,
                                    SB_MSG_MT_CORE,
                                    SB_MSG_MC_REQUEST_INFORMATION,
                                    SB_MSG_DT_UNICAST,
                                    pDevCtxt->pSched->FrmHandNgdLA);
  pTxBuf[1] = 0x0088;
  if (SLIMBUS_ERR_NONE != SlimBus_MasterSendAndWaitForMsgEvent(pDevCtxt, pTxBuf))
  {
    SB_LOG_WARN_1("Purging channels associated with non-responsive satellite (LA: 0x%x)",
                  pDevCtxt->pSched->FrmHandNgdLA);

    for (uClientNum = 0; uClientNum < pNgdDevice->pSat->numClientSlots; uClientNum++)
    {
      SlimBusBasicClientCtxt *pSatClientCtxt = pNgdDevice->pSat->paClientCtxt[uClientNum];
      if ( NULL != pSatClientCtxt )
      {
        PushTailClient(&pDevCtxt->ReconfigList, pSatClientCtxt);
        pSatClientCtxt->uConfigTID = SB_TID_NOACK;
        pSatClientCtxt->bPurgeNeeded = TRUE;
        pSatClientCtxt->ConfigMsgFifo.uHeadIdx = 0;
        pSatClientCtxt->ConfigMsgFifo.uTailIdx = 0;
      }
    }
  }
}

/**
 * @brief Callback for SlimBus master workloop for handling 
 *        remote reconfiguration sequences
 *
 * Callback function for SlimBus master workloop for handling 
 * remote reconfiguration sequences 
 * 
 * @param[in] hEvent  Workloop event handle
 * @param[in] pVoid  Workloop callback context
 * 
 * @return SB_SUCCESS 
 */
SBResult SlimBusMasterWorkLoopHandler(SlimBusEventHandle hEvent, void *pVoid)
{
  SlimBusDevCtxt *pDevCtxt = (SlimBusDevCtxt*)pVoid;
  SlimBusBasicClientCtxt *pBasicClient = NULL;
  SBResult result = SB_SUCCESS;

  UNREFERENCED_PARAMETER(hEvent);

  pDevCtxt->pSched->bMasterWorkerBusy = TRUE;
  
  if (NULL == pDevCtxt->pSched->hSchedEvent)
  {
    /* Create event here so the event has the workloop thread ID */  
    SlimBus_CreateSyncEvent(&pDevCtxt->pSched->hSchedEvent, NULL);
  }
  if (NULL == pDevCtxt->pSched->hReconfigEvent)
  {
    /* Create event here so the event has the workloop thread ID */  
    SlimBus_CreateSyncEvent(&pDevCtxt->pSched->hReconfigEvent, NULL);
  }
  if (NULL == pDevCtxt->pSched->hTimeoutEvent)
  {
    /* Create event here so the event has the workloop thread ID */  
    SlimBus_CreateTimeoutEvent(&pDevCtxt->pSched->hTimeoutEvent, NULL);
  }

  result = SlimBus_SendLogicalAddressAssignments(pDevCtxt);
  if (SB_SUCCESS != result)
  {
    SB_LOG_ERR_0("Error sending logical address assignments");
  }
 
  if (pDevCtxt->pSched->bCheckFrmHandSat)
  {
    SlimBus_CheckFramerHandoverSat(pDevCtxt);
  }
 
  while ( SB_PAUSE_NOT_PAUSED == pDevCtxt->pSched->ePause &&
          HAL_sb_DeviceIsEnumerated(pDevCtxt->hSB, HAL_SB_DEVICE_FRAMER) &&
          ( SlimBus_IsReconfigNeeded(pDevCtxt, &pBasicClient) ||
            ( pDevCtxt->bActiveFramer && 
              SlimBus_ReadyForPause(pDevCtxt) ) ) )
  {
    SBResult result = SB_SUCCESS;
    SBBOOL bSendAck = TRUE;
    SBBOOL R = FALSE;
    uint32 uLastHeadIdx = 0;
    
    if ( pDevCtxt->pSched->bPurge ||
         NULL == pBasicClient ||
         pBasicClient->bPurgeNeeded )
    {
      bSendAck = FALSE;
    }
    
    if (NULL != pBasicClient)
    {
      RemoveClientNode(pBasicClient);
      
      /* Cache the head index in case a new sequence comes in */
      uLastHeadIdx = pBasicClient->ConfigMsgFifo.uHeadIdx;
    }
    
    result = SlimBus_ProcessReconfigSeq(pDevCtxt, pBasicClient);
    
    if (SB_SUCCESS == result)
    {
      R = TRUE;
    }
    
    if (NULL != pBasicClient)
    {
      /* Make sure that head equals tail after reconfiguration */
      pBasicClient->ConfigMsgFifo.uTailIdx = uLastHeadIdx;

      if (NULL != pBasicClient->pClientCtxt)
      {
        pBasicClient->pClientCtxt->eLastReconfigErr = result;

        /* Signal the local client */
        SlimBus_EventTrigger(pBasicClient->pClientCtxt->hClientEvent);
      }
      else if (bSendAck &&
               !pDevCtxt->bPendingSSR)
      {
        uint32 uAckRetryCnt = 0;
        SlimBusErrorType eMsgErr = SLIMBUS_ERR_MSG_RESP_TIMEOUT;
        uint32 *pBuf = NULL;

        for ( ; uAckRetryCnt < SB_RECONFIG_TIMEOUT_COUNT && eMsgErr != SLIMBUS_ERR_NONE; uAckRetryCnt++ )
        {
          pBuf = pDevCtxt->TxMsgFifo.getBufFcn(NULL, NULL,
                                               &pDevCtxt->TxMsgFifo, 
                                               SB_MSG_MC_USR_GENERIC_ACK_LEN);
          if (NULL == pBuf )
          {
            /* TODO: need to unwind the reconfig */
            SB_LOG_ERR_0("Error allocating Tx buffer for satellite reconfig ack");
            result = SB_ERROR;
            break;
          }
          
          pBuf[0] = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_USR_GENERIC_ACK_LEN,
                                            SB_MSG_MT_SRC_REFERRED_USER,
                                            SB_MSG_MC_USR_GENERIC_ACK,
                                            SB_MSG_DT_UNICAST,
                                            pBasicClient->pDevice->LA) |
            SB_MSG_SET(pBasicClient->uConfigTID,USR_GENERIC_ACK_0,TID);
          pBuf[1] = 
            SB_MSG_SET(pBasicClient->uClientNum,USR_GENERIC_ACK_1,CLIENT_NUM) |
            SB_MSG_SET(R,USR_GENERIC_ACK_1,RESP);

          eMsgErr = SlimBus_MasterSendAndWaitForMsgEvent(pDevCtxt, pBuf);
          
          if (SLIMBUS_ERR_NONE != eMsgErr)
          {
            SB_LOG_ERR_0("Got nack for satellite reconfigure now acknowledgement message");
            SlimBus_UnlockDevice(pDevCtxt);
            SLIMBUS_SLEEP(SB_NACK_RETRY_DELAY);
            SlimBus_LockDevice(pDevCtxt);
          }
        }
      }
    }
  }
  
  SlimBus_SendManagerPowerNotifications(pDevCtxt);
  
  /* Signal clients waiting for hardware to enumerate */
  if ( NULL != PeekHeadClient(&pDevCtxt->EnumWaitList) &&
       SlimBus_HardwareIsEnumerated(pDevCtxt) )
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

  pDevCtxt->pSched->bMasterWorkerBusy = FALSE;
  
  pDevCtxt->checkEnterLowPowerFcn(pDevCtxt);
  
  return SB_SUCCESS;
}

/**
 * @brief This function performs the driver-specific 
 *        functionality to connect a port to a data channel
 *
 * This function performs the driver-specific functionality to 
 * connect a port to a data channel 
 * 
 * @param pClientCtxt[in]  Pointer to the client context
 * @param pDevice[in]  Pointer to the native SLIMbus device 
 *               structure corresponding to the device
 *               containing the port to connect
 * @param eFlow[in]  Flow direction for the port 
 * @param uPortNum[in]  Port number to connect 
 * @param uChanNum[in]  Channel number to connect 
 * 
 * @return SB_SUCCESS on success, error code otherwise
 */
static SBResult SlimBus_MasterConnectPortToChannel
(
  SlimBusClientCtxt *pClientCtxt,
  SlimBusDeviceType *pDevice,
  SlimBusPortFlowType eFlow,
  uint32 uPortNum,
  uint32 uEndPointNum,
  uint32 uChanNum
)
{
  SBResult rtn = SB_SUCCESS;
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SlimBusChannelParamType *pChanParam = SlimBus_GetChanParam( pDevCtxt, uChanNum );
  uint32 *pBuf;
  uint8 MC;
  
  if ((rtn = SlimBus_UpdateDataLineMask( pDevCtxt, uChanNum, pDevice )) != SB_SUCCESS)
  {
    return rtn;
  }
  
  /* If we are master driver, we directly send the port connect message */
  pBuf = pDevCtxt->TxMsgFifo.getBufFcn(pClientCtxt, pDevice, &pDevCtxt->TxMsgFifo, 
                                       SB_MSG_MC_CONNECT_SOURCE_LEN);
  if ( NULL == pBuf )
  {
    return SB_ERROR;
  }

  if (eFlow == SLIMBUS_PORT_SOURCE_FLOW)
  {
    MC = SB_MSG_MC_CONNECT_SOURCE;
  }
  else
  {
    MC = SB_MSG_MC_CONNECT_SINK;
  }
  *(pBuf++) = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_CONNECT_SOURCE_LEN,
                                    SB_MSG_MT_CORE, MC,
                                    SB_MSG_DT_UNICAST, pDevice->LA) |
    SB_MSG_SET(uPortNum,CONNECT_SOURCE_0,PN) |
    SB_MSG_SET(uEndPointNum,CONNECT_SOURCE_0,EP);
  *(pBuf++) = SB_MSG_SET(uChanNum,CONNECT_SOURCE_1,CN);

  pDevCtxt->TxMsgFifo.sendBufFcn(pDevCtxt, &pDevCtxt->TxMsgFifo);
  
  rtn = AddPortConnectionTracker(pDevCtxt, pDevice, pChanParam, uPortNum, uEndPointNum, eFlow);

  return rtn;
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
static SBResult SlimBus_MasterDisconnectPortFromChannel
(
  SlimBusClientCtxt *pClientCtxt,
  SlimBusDevCtxt *pDevCtxt,
  SlimBusDeviceType *pDevice,
  uint32 uPortNum
)
{
  uint32 *pBuf;

  /* If we are master driver, we directly send the port disconnect message */
  pBuf = pDevCtxt->TxMsgFifo.getBufFcn(pClientCtxt, pDevice, &pDevCtxt->TxMsgFifo, 
                                       SB_MSG_MC_DISCONNECT_PORT_LEN);
  if ( NULL == pBuf )
  {
    return SB_ERROR;
  }

  *(pBuf++) = SB_MSG_ASM_FIRST_WORD(SB_MSG_MC_DISCONNECT_PORT_LEN,
                                    SB_MSG_MT_CORE, SB_MSG_MC_DISCONNECT_PORT,
                                    SB_MSG_DT_UNICAST, pDevice->LA) |
    SB_MSG_SET(uPortNum,DISCONNECT_PORT_0,PN);

  pDevCtxt->TxMsgFifo.sendBufFcn(pDevCtxt, &pDevCtxt->TxMsgFifo);
  
  RemovePortConnectionTracker2(pDevCtxt, pDevice, uPortNum);
 
  return SB_SUCCESS;
}

/**
 * Leave Slimbus low-power mode if the bus is currently in 
 * low-power mode 
 *  
 * @param[in] pDevCtxt  Pointer to the Slimbus device context
 * 
 * @return SB_SUCCESS
 */
static SBResult SlimBus_MasterLeaveLowPowerMode(SlimBusDevCtxt *pDevCtxt)
{
  SBResult result = SB_SUCCESS;
  
  if ( !pDevCtxt->bClockIsOn )
  {
    SB_LOG_INFO_0("Turning on slimbus ref clock");

    result = SLIMBUS_CLOCK_ON(pDevCtxt);
  }
  if ( SB_SUCCESS == result )
  {
    /* Perform dummy HW read after POR for QCTDD01713765 */
    (void) HAL_sb_CompGetHwVersion(pDevCtxt->hSB);
    
    /* Check for power collapse and restore non-retained registers if necessary */
    SlimBus_CheckAndRestoreNonRetentionRegisters(pDevCtxt);
  }
  if ( SB_SUCCESS == result &&
       !pDevCtxt->bActiveFramer &&
       HAL_sb_CompIsEnabled(pDevCtxt->hSB) &&
       !HAL_sb_DeviceIsEnabled(pDevCtxt->hSB, HAL_SB_DEVICE_INTERFACE) )
  {
    SlimBus_InitGpios(pDevCtxt);
    SlimBus_CheckFramerStateInternal(pDevCtxt);

    if ( !pDevCtxt->bActiveFramer &&
         HAL_sb_CompIsEnabled(pDevCtxt->hSB) &&
         !HAL_sb_DeviceIsEnabled(pDevCtxt->hSB, HAL_SB_DEVICE_INTERFACE) )
    {
      HAL_sb_DeviceEnable(pDevCtxt->hSB, HAL_SB_DEVICE_INTERFACE);

      pDevCtxt->uSFTimerCnt = 0;
      pDevCtxt->uFrameSyncTimerCnt = 0;
      SlimBus_TimerStart(pDevCtxt,
                         pDevCtxt->hSFTimerHandle,
                         pDevCtxt->hSFTimerEvent,
                         SB_SUPERFRAME_BASE_TIMEOUT);
    }
  }
  if ( SB_SUCCESS == result &&
       SB_PAUSE_PAUSED == pDevCtxt->pSched->ePause )
  {
    if (!pDevCtxt->pSched->bUseRetention)
    {
      if (TRUE != HAL_sb_CompReset(pDevCtxt->hSB))
      {
        SB_LOG_ERR_0("Hardware reset failed while leaving low power mode");
      }
      else
      {
        SB_LOG_INFO_0("Hardware reset successful while leaving low power mode");
      }
    }

    if ( HAL_sb_FrmIsSbBooted(pDevCtxt->hSB) )
    {
      SB_LOG_INFO_0("Forcing wakeup from idle pause clock");

      SlimBus_GpioIntDisable(pDevCtxt);
      if (!pDevCtxt->bHwEnumLock)
      {
        if (SB_SUCCESS == SlimBus_StopIdle(pDevCtxt))
        {
          pDevCtxt->bHwEnumLock = TRUE;
        }
      }

      HAL_sb_CompDisableRetention(pDevCtxt->hSB);

      HAL_sb_FrmWakeUp(pDevCtxt->hSB);

      pDevCtxt->pSched->ePause = SB_PAUSE_RESUMING_FROM_PAUSE;
      
      SlimBus_TimerStart(pDevCtxt,
                         pDevCtxt->hEnumTimerHandle,
                         pDevCtxt->hEnumTimerEvent,
                         SB_WAKEUP_TIMEOUT);
    }
    else if ( HAL_sb_DeviceIsEnabled(pDevCtxt->hSB, pDevCtxt->eMsgDev) )
    {
      /* hardware is in a bad state, framer should be booted already 
         when leaving pause clock if manager is enabled */
      if (TRUE != HAL_sb_CompReset(pDevCtxt->hSB))
      {
        SB_LOG_ERR_0("Hardware reset failed while leaving low power mode");
      }
      else
      {
        SB_LOG_INFO_0("Hardware reset successful while leaving low power mode");
      }
    }
  }
  
  return result;
}

/**
 * @brief Check for and initiate entry to pause clock
 *
 * This function checks whether it is OK to enter pause clock 
 * and if so initiates the transition to pause clock 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return SB_SUCCESS if there were no errors, error code on 
 *         error
 */
static SBResult SlimBus_CheckEnterPauseClock
(
  SlimBusDevCtxt *pDevCtxt
)
{
  SBResult result = SB_SUCCESS;
  
  if ( pDevCtxt->bClockIsOn )
  {
    if ( SlimBus_ReadyForPause(pDevCtxt) &&
         !pDevCtxt->pSched->bMasterWorkerBusy )
    {
      if ( SB_PAUSE_NOT_PAUSED == pDevCtxt->pSched->ePause)
      {
        /* signal the reconfiguration workloop */
        SlimBus_EventTrigger(pDevCtxt->hMasterWorkLoopEvent);
      }
      else if ( SB_PAUSE_PAUSED == pDevCtxt->pSched->ePause )
      {
        if (!pDevCtxt->bActiveFramer)
        {
          SB_LOG_INFO_0("Disabling interface and turning off slimbus core clock");

          HAL_sb_DeviceDisable(pDevCtxt->hSB, HAL_SB_DEVICE_INTERFACE);

          SlimBus_DeinitGpios(pDevCtxt);
          
          pDevCtxt->pSched->ePause = SB_PAUSE_NOT_PAUSED;
        }
        else
        {
          SB_LOG_INFO_0("Turning off slimbus ref clock");
        }

        result = SLIMBUS_CLOCK_OFF(pDevCtxt);
      }
    }
    
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
  
  return result;
}

/**
 * @brief Handle master-specific interrupts
 *
 * This function processes master-specific interrupts
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] pIsrCtxt  Pointer to the interrupt context
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
SBResult SlimBus_HandleMasterInterrupts
(
  SlimBusDevCtxt *pDevCtxt,
  struct SlimBusIsrCtxt *pIsrCtxt
)
{
  if (pIsrCtxt->uMsgIntStatus & HAL_SB_MGR_INT_RECONFIG_DONE)
  {
    if (pDevCtxt->pSched->bReconfigBusy)
    {
      pDevCtxt->pSched->bReconfigBusy = FALSE;
      SlimBus_EventTrigger(pDevCtxt->pSched->hReconfigEvent);
    }

    SlimBus_SignalNextReconfig(pDevCtxt);
    
    if ( HAL_sb_FrmIsInIdleMode(pDevCtxt->hSB) )
    {
      if ( SlimBus_ReadyForPause(pDevCtxt) )
      {
        pDevCtxt->pSched->ePause = SB_PAUSE_PAUSED;
        if (pDevCtxt->bHwEnumLock)
        {
          SlimBus_ResumeIdle(pDevCtxt);
          pDevCtxt->bHwEnumLock = FALSE;
        }

        SlimBus_GpioIntEnable(pDevCtxt);

        SB_LOG_INFO_0("Framer entered idle pause clock");
      }
      else
      {
        SB_LOG_INFO_0("Reawakening from idle pause clock due to pending activity");

        HAL_sb_FrmWakeUp(pDevCtxt->hSB);
        pDevCtxt->pSched->ePause = SB_PAUSE_RESUMING_FROM_PAUSE;
      }
    }
    else if ( SB_PAUSE_ENTERING_PAUSE == pDevCtxt->pSched->ePause )
    {
      SB_LOG_INFO_0("Framer restarted spontaneously when entering idle pause clock");

      SlimBus_RecoverFromPauseClock(pDevCtxt);
    }
  }
  
  if (pIsrCtxt->uFrmIntStatus & HAL_SB_FRM_INT_IDLE_MODE_EXITED)
  {
    if (SB_PAUSE_PAUSED == pDevCtxt->pSched->ePause ||
        SB_PAUSE_ENTERING_PAUSE == pDevCtxt->pSched->ePause ||
        SB_PAUSE_RESUMING_FROM_PAUSE == pDevCtxt->pSched->ePause )
    {
      if ( !HAL_sb_FrmIsInIdleMode(pDevCtxt->hSB) &&
            SlimBus_HardwareIsEnumerated(pDevCtxt) )
      {
        SB_LOG_INFO_0("Framer restarted following idle pause clock");

        SlimBus_TimerStart(pDevCtxt,
                           pDevCtxt->hEnumTimerHandle,
                           pDevCtxt->hEnumTimerEvent,
                           0);

        if (SB_PAUSE_PAUSED == pDevCtxt->pSched->ePause )
        {
          SlimBus_GpioIntDisable(pDevCtxt);
          if (!pDevCtxt->bHwEnumLock)
          {
            if (SB_SUCCESS == SlimBus_StopIdle(pDevCtxt))
            {
              pDevCtxt->bHwEnumLock = TRUE;
            }
          }
        }

        SlimBus_RecoverFromPauseClock(pDevCtxt);
      }
      else
      {
        SB_LOG_WARN_2("Got spurious idle mode exited interrupt (bIdle: %d) (bEnum: %d)",
                      HAL_sb_FrmIsInIdleMode(pDevCtxt->hSB),
                      SlimBus_HardwareIsEnumerated(pDevCtxt));
      }
    }
  }

  if (SB_PAUSE_NOT_PAUSED == pDevCtxt->pSched->ePause)
  {
    SlimBusEventNodeType *pEventNode;
  
    while (NULL != (pEventNode = PopHeadEvent(&pDevCtxt->PauseEventQueue)))
    {
      SlimBus_EventTrigger(pEventNode->hEvent);
    }
  }

  return SB_SUCCESS;
}


/**
 * @brief Perform master-specific device driver initialization 
 *
 * This function performs master-specific device driver 
 * initialization 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return SB_SUCCESS on sucess, error code on error 
 */
SBResult SlimBus_DeviceInitMaster(SlimBusDevCtxt *pDevCtxt)
{
  SBResult result;

  if (NULL == pDevCtxt->pSched)
  {
    result = SlimBus_Malloc(sizeof(*pDevCtxt->pSched),
                            (void**)&pDevCtxt->pSched, SB_MEMTAG_15);
    if (SB_SUCCESS != result ||
        NULL == pDevCtxt->pSched)
    {
      return SB_ERROR;
    }
  }
  
  memset(pDevCtxt->pSched, 0, sizeof(*pDevCtxt->pSched));

  pDevCtxt->eMsgDev = HAL_SB_DEVICE_MANAGER;

  pDevCtxt->bIsHwOwner = TRUE;
  pDevCtxt->bActiveFramer = TRUE;

  pDevCtxt->pSched->ePause = SB_PAUSE_PAUSED;
  pDevCtxt->pSched->bEnablePause = TRUE; /* default value, can be overidden by InitMasterParams */

  pDevCtxt->pSched->eFrmHandState = SB_FRM_HANDOVER_IDLE;
  
  pDevCtxt->pSched->bUseRetention = TRUE; /* default value, can be overidden by InitMasterParams */
  
  result = SlimBus_InitMasterParams(pDevCtxt);
  if (SB_SUCCESS != result)
  {
    return result;
  }

  result = SlimBus_InitMasterServices(pDevCtxt);
  if (SB_SUCCESS != result)
  {
    return result;
  }

  pDevCtxt->connectPortFcn = SlimBus_MasterConnectPortToChannel;
  pDevCtxt->disconnectPortFcn = SlimBus_MasterDisconnectPortFromChannel;
  pDevCtxt->doReconfigSeqFcn = SlimBus_ProcessLocalReconfigSeq;
  pDevCtxt->checkEnterLowPowerFcn = SlimBus_CheckEnterPauseClock;
  pDevCtxt->leaveLowPowerFcn = SlimBus_MasterLeaveLowPowerMode;
  
  RootNodeInit(&pDevCtxt->pSched->ChanParamList3);
  RootNodeInit(&pDevCtxt->pSched->ChanParamList1);

  RootNodeInit(&pDevCtxt->ReconfigList);
  pDevCtxt->pSched->uSubFrmCode = 0;
  pDevCtxt->pSched->uMsgSlotReq = SB_INTERNAL_RESV_MSG_SLOTS;
  pDevCtxt->pSched->aDataLineSchedules[0].uNewSlotsUsed = pDevCtxt->pSched->uMsgSlotReq;
  pDevCtxt->pSched->aDataLineSchedules[0].uCurSlotsUsed = pDevCtxt->pSched->uMsgSlotReq;
  
  RootNodeInit(&pDevCtxt->pSched->FreeSchedEventStack);
  pDevCtxt->pSched->SchedEventNode.pFreeRoot = &pDevCtxt->pSched->FreeSchedEventStack;
  PushHeadEvent(&pDevCtxt->pSched->FreeSchedEventStack, &pDevCtxt->pSched->SchedEventNode);

  return SB_SUCCESS;
}


/**
 * @brief Reset routine before SSR restart is performed.
 *  
 * This function does the cleanup on Subsystem Reset 
 * 
 * @param[in] pDevCtxt      Device Ctxt 
 * 
 * @return None 
 */
void SlimBus_SSReset(SlimBusDevCtxt *pDevCtxt)
{
  SlimBusEventNodeType *pEvent, *pNextEvent;
  SlimBusBasicClientCtxt *pClientCtxt = NULL;
  uint32 uIdx, i;
  uint32 uToken;

  SB_LOG_WARN_0("Resetting hardware and clearing state due to pending SSR");

  /* Set SSR pending state */
  pDevCtxt->bPendingSSR = TRUE;

  /* Turn on Clocks if not on already*/
  if(!pDevCtxt->bClockIsOn)
  {
    SLIMBUS_CLOCK_ON(pDevCtxt);
    pDevCtxt->bClockIsOn = TRUE;
  }

  /* Reset Slimbus hardware. */
  HAL_sb_CompReset(pDevCtxt->hSB);

  /* Interrupts are now disabled.  Clear cached ISR status */
  SlimBus_InterruptAcquireLock(pDevCtxt, &uToken);
  memset(&pDevCtxt->IsrCtxt, 0, sizeof(pDevCtxt->IsrCtxt));
  SlimBus_InterruptReleaseLock(pDevCtxt, &uToken);
  
  /* Mark all clients device open FALSE */
  for (i = 0; i < pDevCtxt->numClientSlots; i++)
  {
     if (NULL != pDevCtxt->paClientCtxt[i])
     {
        pDevCtxt->paClientCtxt[i]->bOpenb4SSR = pDevCtxt->paClientCtxt[i]->bOpen;
        pDevCtxt->paClientCtxt[i]->bOpen = FALSE;
     }
  }

  /* Flush reconfiguration client list */
  while (NULL != (pClientCtxt = PopHeadClient(&pDevCtxt->ReconfigList)))
  {
    pClientCtxt->bPurgeNeeded = FALSE;
  }

  if ( SB_MASTER_DRIVER == pDevCtxt->eDrvType )
  {
    pDevCtxt->pSched->bPurge = FALSE;
    
    if ( pDevCtxt->pSched->bReconfigBusy )
    {
      /* Abort the reconfiguration sequence */
      pDevCtxt->pSched->bReconfigBusy = FALSE;
      SlimBus_EventTrigger(pDevCtxt->pSched->hReconfigEvent);
    }
  }
  
  /* Make sure all the BAM pipe handles are detached and
     Port disconnect events are triggered */
  for (uIdx = 0; uIdx < pDevCtxt->numPorts; uIdx++)
  {
      SlimBusMasterPortType *pPort = &pDevCtxt->Ports[uIdx];     
      uint32 endPointNum;

      /* Disassociate ports and channels */
      for (endPointNum = 0; endPointNum < pPort->numEndPoints; endPointNum++)
      {
        RemoveEndPointNode(&pPort->aEndPoints[endPointNum]);
        pPort->aEndPoints[endPointNum].pChannel = NULL;
      }

      if ( NULL != pPort->pBamCtxt )
      {
        /* Trigger Port disconnect event if enabled */
        if(pPort->eEnabledEvents & SLIMBUS_EVENT_O_PORT_DISCONNECT)
        {
          pPort->eActiveEvents |= SLIMBUS_EVENT_O_PORT_DISCONNECT;
          if (NULL != pPort->hEvent)
          {
            SlimBus_EventTriggerEx(pPort->hEvent, pPort->pEventUserData);
          }
        }

        SlimBus_DisconnectBAM(pPort);
        SlimBus_DetachBAM(&pPort->pBamCtxt);
      }
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
     if(NULL != pEvent->hEvent)
     {
       SlimBus_EventTrigger(pEvent->hEvent);
     }
     RemoveEventNode(pEvent);
     pEvent = pNextEvent;
  }
  
  /* Make sure all the Rx events are removed from the device event queue */
  pEvent = PeekHeadEvent(&pDevCtxt->RxEventQueue);
  while (NULL != pEvent)
  {
    uint8 TID, LA;
    SlimBusDeviceType *pDevice;
    pNextEvent = NextEventNode(&pDevCtxt->RxEventQueue, pEvent);
    
    RemoveEventNode(pEvent);
    LA = SB_BF_GET(pEvent->uRxMarker,RXMARKER,LA);
    TID = SB_BF_GET(pEvent->uRxMarker,RXMARKER,TID);
    pDevice = pDevCtxt->pDevices[LA&SB_LA_IDX_MASK];
    if(NULL != pEvent->hEvent)
    {
      SlimBus_EventTrigger(pEvent->hEvent);
    }
    if (NULL != pDevice)
    {
      pDevice->TIDMap[TID&SB_TID_IDX_MASK] = NULL;
    }   
    pEvent = pNextEvent;
  }
  
  /* Flush ConfigMsgFifo */
  for (i = 0; i < pDevCtxt->numClientSlots; i++)
  {
    if (NULL != pDevCtxt->paClientCtxt[i])
    {
      SlimBus_ResetMsgFifo(&pDevCtxt->paClientCtxt[i]->BasicClient.ConfigMsgFifo);
    }
  }

  /* Flush Tx queue */
  SlimBus_ResetMsgFifo(&pDevCtxt->TxMsgFifo);

  /* Flush Rx queue */
  SlimBus_ResetMsgFifo(&pDevCtxt->RxMsgFifo);
  
  /* Reset BAM device */
  SlimBus_ResetBAM(pDevCtxt);
  
  if (SB_MASTER_DRIVER == pDevCtxt->eDrvType)
  {
    /* Ensure the master workloop is idle */
    if (pDevCtxt->pSched->bReconfigBusy)
    {
      pDevCtxt->pSched->bReconfigBusy = FALSE;

      SlimBus_UnlockDevice(pDevCtxt);
      SlimBus_EventWait(pDevCtxt->pSched->hReconfigEvent);
      SlimBus_LockDevice(pDevCtxt);
    }

    /* Clear handover state */
    pDevCtxt->pSched->eFrmHandState = SB_FRM_HANDOVER_IDLE;
    pDevCtxt->pSched->ePause = SB_PAUSE_PAUSED;
  }

  /* Set local channel status to removed */
  for (uIdx = 0; uIdx < pDevCtxt->numChannelSlots; uIdx++)
  {
    SlimBusChannelType *pChannel = pDevCtxt->pChannels[uIdx];

    if( NULL != pChannel &&
        pChannel->eState == SB_HANDLE_ALLOCATED )
    {
      pChannel->eNewStatus = SLIMBUS_CHANNEL_STATUS_REMOVED;
      pChannel->eCurStatus = SLIMBUS_CHANNEL_STATUS_REMOVED;
    }
  }
}

/**
 * @brief Restart routine after SSR restart is done.
 *  
 * This function does the re-initialization on Subsystem Restart
 * 
 * @param[in] pDevCtxt      Device Ctxt 
 * 
 * @return None 
 */
void SlimBus_SSRestart(SlimBusDevCtxt *pDevCtxt)
{
  uint32 i;

  SB_LOG_INFO_0("Reinitializing hardware and resuming following SSR");

  /* Re-initialize hardware after Subsystem restart */
  if(SB_SUCCESS != SlimBus_DeviceInitHardware(pDevCtxt))
  {
    SB_LOG_ERR_0("Slimbus SSR Device Hardware Initialization failed!");
  }

  /* Restore clients device open status */
  for (i = 0; i < pDevCtxt->numClientSlots; i++)
  {
    if (NULL != pDevCtxt->paClientCtxt[i])
    {
      pDevCtxt->paClientCtxt[i]->bOpen = pDevCtxt->paClientCtxt[i]->bOpenb4SSR;
      pDevCtxt->paClientCtxt[i]->bOpenb4SSR = FALSE;
    }
  }
  pDevCtxt->bPendingSSR = FALSE;
  
  if (NULL != pDevCtxt->pSched)
  {
    pDevCtxt->pSched->bPurge = TRUE;
  }

  SB_LOG_INFO_0("Subsystem restart complete");
}

