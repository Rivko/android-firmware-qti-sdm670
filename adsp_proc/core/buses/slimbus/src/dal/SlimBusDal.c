/*
===========================================================================

FILE:         SlimBusDal.c

DESCRIPTION:  This file implements the SLIMbus core driver functionality
              using DAL.

===========================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1/buses/slimbus/src/dal/SlimBusDal.c#2 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
08/31/17   DPK    To use the right  TLMM API to configure the SLIMBUS GPIO 
                           lines based on their tlmm type.
05/25/17   MJS     Add support to disable retention.
04/04/17   RC      Additional check as part of inactive configuaration.
03/06/17   MJS     Use DALSYS timer APIs rather than DAL Timer driver.
11/22/16   MJS     Porting cross-platform compatibility updates.
09/27/16   MJS     Add query PGD port index API.
07/07/16   MJS     Interrupt enablement changes for HLOS compatibility.
05/10/16   MJS     Add clock gear callback and max endpoint APIs.
04/11/16   MJS     Cleanup.
02/23/16   MJS     Remove unused timer callback handles.
10/14/15   MJS     Add support for batched messages.
06/19/15   SG      Add slimbus mutex lock/unlock in SlimBus_DeviceInit.
05/27/15   NSD     Fix in the BAM port flush event trigger logic.
05/05/15   NSD     Trigger port flush event when the number of used BAM IO 
                   vector slots become zero.
03/18/15   MJS     Fix to configure secondary data line GPIOs.
09/09/14   mq      Multi-dataline support.
09/24/14   MJS     Fix for banned API.
09/04/14   MJS     Fix for possible race condition while checking for external
                   clock toggle.
06/17/14   MJS     Add support for minimum clock gear.
05/22/14   MJS     Add workaround to phase shift frame sync after timeout.
05/07/14   MJS     Allow BAM trust registers to not be programmed.
04/09/14   MJS     Block client open calls until hardware is enumerated.
04/09/14   MJS     Use timed wait instead of clock toggle timer callback.
03/19/14   MJS     Consolidate the timer workloops.
03/12/14   MJS     Support framer handover to second external framer.
03/04/14   MJS     Fix Klocwork issue.
02/28/14   MJS     Streamline ISR and don't lock device mutex in ISR.
02/14/14   MJS     Use named DAL workloops.
01/31/14   MJS     Use interrupt-based clock toggle detection not polling.
01/24/14   MJS     Always configure TLMM input mode for clock toggle check.
01/20/14   MJS     Fix for qurt mutex unlock issue when device init fails.
11/07/13   MJS     Support for MDM restart when master is inactive framer.
09/20/13   MJS     Add ability to disable use of GPIO interrupt.
08/16/13   MJS     Use device lock during DAL ISR callback.
07/25/13   MJS     Add support for upper watermark, shadow counter and 
                   hardware capability APIs.
07/25/13   MJS     Support for GPIO interrupt wakeup.
07/25/13   MJS     Check for external GPIO toggle during hardware init.
04/30/13   MJS     Support for framer handover.
04/10/13   MJS     Remove hack for programming BAM secure registers.
03/21/13   MJS     Support for standalone satellite hardware configuration.
03/21/13   MJS     Allow shared channel to be allocated by multiple clients.
01/09/12   MJS     Support more than 32 channels using dynamic allocation.
11/29/12   MJS     Support BAM deferred processing within client context.
11/13/12   MJS     Satellite support for role reversal.
10/25/12   MJS     Master support for role reversal.
10/03/12   MJS     Add SlimBus_GetBamIOVecEx.
09/21/12   MJS     Move interrupt enable to common code.
07/24/12   MJS     Fix support for BAM reset timer workloop.
05/16/12   MJS     Updated common interface.
04/24/12   MJS     Detect if HWIO mapping fails.
12/07/11   MJS     Initial revision.

===========================================================================
             Copyright (c) 2011-2017 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

#include <stringl/stringl.h>
#include "SlimBusTarget.h"
#include "SlimBusDal.h"
#include "SlimBus.h"
#include "SlimBusConsts.h"
#include "SlimBusLists.h"
#include "SlimBusMaster.h"
#include "SlimBusSat.h"
#include "SlimBusBam.h"
#include "DDITlmm.h"
#include "DDIGPIOInt.h"
#include "uTlmm.h"


/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/

/* Typedef for the timer callback function */
typedef void(*SlimBusTimerCallbackFuncType)(SlimBusDevCtxt *pDevCtxt);

SBResult
SlimBus_DeviceAttach(const char *, DALDEVICEID,DalDeviceHandle **);

static uint32
SlimBus_AddRef(DalSlimBusHandle* h)
{
  return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/**
 * @brief SLIMbus driver DAL interrupt service routine
 *
 * SLIMbus driver DAL interrupt service routine
 * 
 * @param[in] pArg  placeholder 
 * 
 * @return Pointer to NULL 
 */
static void *SlimBusDalIsr( void *pArg )
{
  SlimBusDevCtxt *pDevCtxt = (SlimBusDevCtxt*) pArg;
  uint32 uPortDmaMask = 0;
  uint32 i = 0;
  uint32 numDesc = 0;
  uint32 uToken;
  
  /* Don't acquire device mutex here, since we do it again within the DPC */
  /* Acquire interrupt mutex instead */
  SlimBus_InterruptAcquireLock(pDevCtxt, &uToken);
  
  SlimBusCommonIsr(pDevCtxt, &pDevCtxt->IsrCtxt);
  
  if ( 0 != pDevCtxt->IsrCtxt.uMsgIntStatus ||
       0 != pDevCtxt->IsrCtxt.uPgdPortIntMaskedStatus ||
       0 != pDevCtxt->IsrCtxt.uPortDisMask ||
       0 != pDevCtxt->IsrCtxt.uFrmIntStatus ||
       0 != pDevCtxt->IsrCtxt.uIntfIntStatus ||
       0 != pDevCtxt->IsrCtxt.uPgdIntStatus )
  {
    SlimBusBasicClientCtxt *pMsgClient = NULL;

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
  }

  uPortDmaMask = pDevCtxt->IsrCtxt.uPortDmaMask;
  pDevCtxt->IsrCtxt.uPortDmaMask = 0;
  
  SlimBus_InterruptReleaseLock(pDevCtxt, &uToken);
  
  if ( uPortDmaMask != 0 )
  {
    uint32 uIntMask = 0;
    
    SlimBus_PortAcquireLock(pDevCtxt, NULL);

    for (i = 0; (i < pDevCtxt->numPorts) && (uIntMask < uPortDmaMask); i++)
    {
      SlimBusMasterPortType *pPort = &pDevCtxt->Ports[i];
      uIntMask = (1<<pPort->portNum);

      if (uIntMask & uPortDmaMask)
      {
        if ( !SlimBus_GetOnePipeDesc(pPort) )
        {
          uPortDmaMask &= ~uIntMask;

          SB_LOG_INFO_1("Port DMA interrupt asserted for port %d but no new descriptors", pPort->portNum);
        }
      }
    }
    SlimBus_PortReleaseLock(pDevCtxt, NULL);
  }
  
  if ( uPortDmaMask != 0 )
  {
    uint32 uIntMask = 0;

    for (i = 0; (i < pDevCtxt->numPorts) && (uIntMask < uPortDmaMask); i++)
    {
      SlimBusMasterPortType *pPort = &pDevCtxt->Ports[i];
      uint32 uIntMask = (1<<pPort->portNum);

      if (uIntMask & uPortDmaMask)
      {
        if (pPort->uSignalDescEmptyCnt > 0)
        {
          numDesc = SlimBus_BamGetIOVecActiveSlots(pPort->pBamCtxt);
          if ((numDesc == 0) && (NULL != pPort->pClientCtxt))
          {
            SlimBus_EventTrigger(pPort->pClientCtxt->hClientEvent);
            pPort->uSignalDescEmptyCnt = 0;
          }
        }
        SlimBus_SignalBamEvent(pPort);
      }
    }
  }
  
  return NULL;
}

/**
 * @brief Callback for SlimBus timer workloop for handling timer
 *        events
 *
 * Callback function for SlimBus timer workloop for handling 
 * timer events 
 * 
 * @param[in] hEvent  Workloop event handle
 * @param[in] pVoid  Workloop callback context
 * 
 * @return SB_SUCCESS 
 */
static SBResult SlimBusTimerWorkLoopHandler(SlimBusEventHandle hEvent, void *pVoid)
{
  SlimBusDevCtxt *pDevCtxt = (SlimBusDevCtxt*) pVoid;
  
  /* Need to create the events here so the event has the workloop thread ID */  
  DALSYS_EventCreate(DALSYS_EVENT_ATTR_TIMEOUT_EVENT|
                     DALSYS_EVENT_ATTR_NON_DEFERRED|
                     DALSYS_EVENT_ATTR_NORMAL,
                     &pDevCtxt->hEnumTimerEvent, NULL);
  DALSYS_EventCreate(DALSYS_EVENT_ATTR_TIMEOUT_EVENT|
                     DALSYS_EVENT_ATTR_NON_DEFERRED|
                     DALSYS_EVENT_ATTR_NORMAL,
                     &pDevCtxt->hSFTimerEvent, NULL);
  DALSYS_EventCreate(DALSYS_EVENT_ATTR_TIMEOUT_EVENT|
                     DALSYS_EVENT_ATTR_NON_DEFERRED|
                     DALSYS_EVENT_ATTR_NORMAL,
                     &pDevCtxt->hBamResetTimerEvent, NULL);
  
  /* Signal the device init to continue */
  SlimBus_EventTrigger(pDevCtxt->hInitEvent);

  while ( !pDevCtxt->bInExit )
  {
    uint32 uTimerIdx = 0;
    DALResult result = DAL_SUCCESS;
    DALSYSEventHandle hEvents[] = {
      pDevCtxt->hBamResetTimerEvent,
      pDevCtxt->hEnumTimerEvent,
      pDevCtxt->hSFTimerEvent
    };
    SlimBusTimerCallbackFuncType pfnCallback[] = {
      SlimBusBamResetTimerCallback,
      SlimBusEnumTimerCallback,
      SlimBusSFTimerCallback
    };

    /* Wait on our timer event */
    result = DALSYS_EventMultipleWait(hEvents, sizeof(hEvents)/sizeof(hEvents[0]),
                                      DALSYS_EVENT_TIMEOUT_INFINITE, &uTimerIdx);
    
    if ( DAL_SUCCESS == result &&
         uTimerIdx < sizeof(pfnCallback)/sizeof(pfnCallback[0]) )
    {
      pfnCallback[uTimerIdx](pDevCtxt);
    }
  }
  
  return DAL_WORK_LOOP_EXIT;
}

/**
 * @brief Initialize the driver type
 *
 * This function initializes the driver type, either master 
 * driver of satellite driver 
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return SB_SUCCESS on sucess, error code on error
 */
SBResult SlimBus_InitDriverType(SlimBusDevCtxt *pDevCtxt)
{
  DALSYSPropertyVar PropVar;
  SBResult result;
  
  /* Get the property handle for this device */
  result = DALSYS_GetDALPropertyHandle( pDevCtxt->Base.DevId, pDevCtxt->Base.hProp );
  if (SB_SUCCESS != result)
  {
    return result;
  }

  /* Determine whether the driver should run as the master or satellite */
  result = DALSYS_GetPropertyValue( pDevCtxt->Base.hProp, "is_master", 0, &PropVar );
  if ( SB_SUCCESS != result ||
       DALSYS_PROP_TYPE_UINT32 != PropVar.dwType )
  {
    return SB_ERROR;
  }

  if ( 0 != PropVar.Val.dwVal )
  {
    pDevCtxt->eDrvType = SB_MASTER_DRIVER;
  }
  else
  {
    pDevCtxt->eDrvType = SB_SATELLITE_DRIVER;
  }
  
  return SB_SUCCESS;
}

/**
 * @brief Initialize the Slimbus driver parameters
 *
 * This function initializes the Slimbus driver parameters
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SBResult SlimBus_InitParams(SlimBusDevCtxt *pDevCtxt)
{
  DALSYSPropertyVar PropVar;
  DALResult result;

  result = DALSYS_GetPropertyValue( pDevCtxt->Base.hProp, "no_tx_queue", 0, &PropVar );
  if ( DAL_SUCCESS == result &&
       DALSYS_PROP_TYPE_UINT32 == PropVar.dwType &&
       0 < PropVar.Val.dwVal )
  {
    pDevCtxt->bUseTxMsgQueue = FALSE;
  }
  else
  {
    pDevCtxt->bUseTxMsgQueue = TRUE;
  }

  result = DALSYS_GetPropertyValue( pDevCtxt->Base.hProp, "no_rx_queue", 0, &PropVar );
  if ( DAL_SUCCESS == result &&
       DALSYS_PROP_TYPE_UINT32 == PropVar.dwType &&
       0 < PropVar.Val.dwVal )
  {
    pDevCtxt->bUseRxMsgQueue = FALSE;
  }
  else
  {
    pDevCtxt->bUseRxMsgQueue = TRUE;
  }

  /* How many local ports does this instance of the driver use */
  result = DALSYS_GetPropertyValue( pDevCtxt->Base.hProp, "num_local_ports", 0, &PropVar );
  if ( DAL_SUCCESS == result &&
       DALSYS_PROP_TYPE_UINT32 == PropVar.dwType )
  {
    pDevCtxt->numPorts = PropVar.Val.dwVal;
  }
  else
  {
    pDevCtxt->numPorts = 0;
  }
  /* Get the base port number for the ports owned locally by this driver instance */
  result = DALSYS_GetPropertyValue( pDevCtxt->Base.hProp, "local_port_base", 0, &PropVar );
  if ( DAL_SUCCESS == result &&
       DALSYS_PROP_TYPE_UINT32 == PropVar.dwType )
  {
    pDevCtxt->uBasePortNum = PropVar.Val.dwVal;
  }
  else
  {
    pDevCtxt->numPorts = 0;
  }
  /* Get the base channel number for the channels owned locally by this driver instance */
  result = DALSYS_GetPropertyValue( pDevCtxt->Base.hProp, "local_channel_base", 0, &PropVar );
  if ( DAL_SUCCESS == result &&
       DALSYS_PROP_TYPE_UINT32 == PropVar.dwType )
  {
    pDevCtxt->uBaseChanNum = PropVar.Val.dwVal;
    pDevCtxt->numChannelSlots = sizeof(pDevCtxt->pChannels)/sizeof(pDevCtxt->pChannels[0]);
  }
  else
  {
    pDevCtxt->numChannelSlots = 0;
  }
  /* Get the base channel number for the channels owned locally by this driver instance */
  result = DALSYS_GetPropertyValue( pDevCtxt->Base.hProp, "shared_channel_base", 0, &PropVar );
  if ( DAL_SUCCESS == result &&
       DALSYS_PROP_TYPE_UINT32 == PropVar.dwType )
  {
    pDevCtxt->uBaseSharedChanNum = PropVar.Val.dwVal;
    pDevCtxt->numSharedChannelSlots = SB_MAX_DATA_CHANNEL_SLOTS;
  }
  else
  {
    pDevCtxt->numSharedChannelSlots = 0;
  }
  
  /* How many local counters does this instance of the driver use */
  result = DALSYS_GetPropertyValue( pDevCtxt->Base.hProp, "num_local_counters", 0, &PropVar );
  if ( DAL_SUCCESS == result &&
       DALSYS_PROP_TYPE_UINT32 == PropVar.dwType )
  {
    pDevCtxt->numCounterSlots = PropVar.Val.dwVal;
  }
  else
  {
    pDevCtxt->numCounterSlots = 0;
  }
  
  result = DALSYS_GetPropertyValue( pDevCtxt->Base.hProp, "default_clock_gear", 0, &PropVar );
  if ( DAL_SUCCESS == result &&
       DALSYS_PROP_TYPE_UINT32 == PropVar.dwType &&
       PropVar.Val.dwVal > 0 &&
       PropVar.Val.dwVal <= SB_MAX_CLK_GEAR )
  {
    pDevCtxt->uDefClkGear = PropVar.Val.dwVal;
  }

  result = DALSYS_GetPropertyValue( pDevCtxt->Base.hProp, "min_clock_gear", 0, &PropVar );
  if ( DAL_SUCCESS == result &&
       DALSYS_PROP_TYPE_UINT32 == PropVar.dwType &&
       PropVar.Val.dwVal > 0 &&
       PropVar.Val.dwVal <= SB_MAX_CLK_GEAR )
  {
    pDevCtxt->uMinClkGear = PropVar.Val.dwVal;
  }

  /* Whether we need to program the BAM trust registers or not */
  result = DALSYS_GetPropertyValue( pDevCtxt->Base.hProp, "prog_bam_trust", 0, &PropVar );
  if ( DAL_SUCCESS == result &&
       DALSYS_PROP_TYPE_UINT32 == PropVar.dwType )
  {
    pDevCtxt->bProgBamTrust = (SBBOOL) PropVar.Val.dwVal;
  }
  else
  {
    pDevCtxt->bProgBamTrust = TRUE;
  }

  SB_LOG_INFO_5("Driver Parameters (txq: %ld) (rxq: 0x%lx) (lports: %ld) "
                "(pbase: %ld) (cbase: %ld)", pDevCtxt->bUseTxMsgQueue,
                pDevCtxt->bUseRxMsgQueue, pDevCtxt->numPorts,
                pDevCtxt->uBasePortNum, pDevCtxt->uBaseChanNum);

  return SB_SUCCESS;
}

/**
 * @brief Initialize the master-specific Slimbus parameters
 *
 * This function initializes the master-specific Slimbus 
 * parameters 
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SBResult SlimBus_InitMasterParams(SlimBusDevCtxt *pDevCtxt)
{
  static DALSYSPropertyVar PropVar;
  DALResult result;

  result = DALSYS_GetPropertyValue( pDevCtxt->Base.hProp, "no_pause", 0, &PropVar );
  if ( DAL_SUCCESS == result &&
       DALSYS_PROP_TYPE_UINT32 == PropVar.dwType &&
       0 != PropVar.Val.dwVal )
  {
    pDevCtxt->pSched->bEnablePause = FALSE;
  }

  result = DALSYS_GetPropertyValue( pDevCtxt->Base.hProp, "no_retention", 0, &PropVar );
  if ( DAL_SUCCESS == result &&
       DALSYS_PROP_TYPE_UINT32 == PropVar.dwType &&
       0 != PropVar.Val.dwVal )
  {
    pDevCtxt->pSched->bUseRetention = FALSE;
  }

  result = DALSYS_GetPropertyValue( pDevCtxt->Base.hProp, "ee_assign", 0, &PropVar );
  if ( DAL_SUCCESS != result ||
       DALSYS_PROP_TYPE_BYTE_PTR != PropVar.dwType ||
       3 != PropVar.dwLen )
  {
    return SB_ERROR;
  }
  pDevCtxt->auEeAssign = PropVar.Val.pbVal;
  
  result = DALSYS_GetPropertyValue( pDevCtxt->Base.hProp, "la_map", 0, &PropVar );
  if ( DAL_SUCCESS == result &&
       DALSYS_PROP_TYPE_BYTE_PTR == PropVar.dwType )
  {
    uint32 uIdx = 0;
    
    while ( uIdx + SLIMBUS_ENUM_ADDR_BUF_SIZE + 1 <= PropVar.dwLen )
    {
      uint8 LA = PropVar.Val.pbVal[uIdx++];
      SlimBusDeviceType *pDevice = pDevCtxt->pDevices[LA&SB_LA_IDX_MASK];
      
      if ( NULL != pDevice )
      {
        return SB_ERROR;
      }
      pDevice = SlimBus_NewDeviceStruct();
      if ( NULL == pDevice )
      {
        return SB_ERROR;
      }
      pDevCtxt->pDevices[LA&SB_LA_IDX_MASK] = pDevice;

      pDevice->LA = LA;
      memscpy(pDevice->aEA, SLIMBUS_ENUM_ADDR_BUF_SIZE,
              &PropVar.Val.pbVal[uIdx], SLIMBUS_ENUM_ADDR_BUF_SIZE);
      uIdx += SLIMBUS_ENUM_ADDR_BUF_SIZE;
      
      pDevice->eState = SB_ENUM_RESERVED;
    }
    if ( uIdx != PropVar.dwLen )
    {
      return SB_ERROR;
    }
  }
  else if (DAL_SUCCESS == DALSYS_GetPropertyValue( pDevCtxt->Base.hProp, "device_props", 0, &PropVar ))
  {
    SlimBusDeviceDalProps* devProps = (SlimBusDeviceDalProps*)PropVar.Val.pStruct;
    
    if (DAL_SUCCESS == DALSYS_GetPropertyValue( pDevCtxt->Base.hProp, "num_device_props", 0, &PropVar ))
    {
      uint32 uIdx;
      uint32 uNumDevProps = *(uint32*)PropVar.Val.pStruct;

      for (uIdx = 0; uIdx < uNumDevProps; uIdx++)
      {
        uint8 LA = devProps[uIdx].LA;
        uint8 dataLineMask = devProps[uIdx].uDataLineMask;
                        
        SlimBusDeviceType *pDevice = pDevCtxt->pDevices[LA & SB_LA_IDX_MASK];

        if (NULL != pDevice)
        {
          return SB_ERROR;
        }
        pDevice = SlimBus_NewDeviceStruct();
        if (NULL == pDevice)
        {
          return SB_ERROR;
        }
        pDevCtxt->pDevices[LA & SB_LA_IDX_MASK] = pDevice;

        pDevice->LA = LA;
        pDevice->uDataLineMask = dataLineMask & SB_ALL_DATA_LINES_MASK;
        pDevice->eState = SB_ENUM_RESERVED;
        memscpy( pDevice->aEA, SLIMBUS_ENUM_ADDR_BUF_SIZE, 
                devProps[uIdx].uaEA, SLIMBUS_ENUM_ADDR_BUF_SIZE );
      }
    }
    else
    {
      return SB_ERROR;
    }
  }

  result = DALSYS_GetPropertyValue( pDevCtxt->Base.hProp, "handover_frm_la", 0, &PropVar );
  if ( DAL_SUCCESS == result &&
       DALSYS_PROP_TYPE_UINT32 == PropVar.dwType )
  {
    pDevCtxt->pSched->FrmHandFrmLA = (SlimBusLogicalAddrType)PropVar.Val.dwVal;
  }

  result = DALSYS_GetPropertyValue( pDevCtxt->Base.hProp, "handover_ngd_la", 0, &PropVar );
  if ( DAL_SUCCESS == result &&
       DALSYS_PROP_TYPE_UINT32 == PropVar.dwType )
  {
    pDevCtxt->pSched->FrmHandNgdLA = (SlimBusLogicalAddrType)PropVar.Val.dwVal;
  }

  result = DALSYS_GetPropertyValue( pDevCtxt->Base.hProp, "handover_frm2_la", 0, &PropVar );
  if ( DAL_SUCCESS == result &&
       DALSYS_PROP_TYPE_UINT32 == PropVar.dwType )
  {
    pDevCtxt->pSched->FrmHandFrm2LA = (SlimBusLogicalAddrType)PropVar.Val.dwVal;
  }

  result = DALSYS_GetPropertyValue( pDevCtxt->Base.hProp, "handover_ngd2_la", 0, &PropVar );
  if ( DAL_SUCCESS == result &&
       DALSYS_PROP_TYPE_UINT32 == PropVar.dwType )
  {
    pDevCtxt->pSched->FrmHandNgd2LA = (SlimBusLogicalAddrType)PropVar.Val.dwVal;
  }

  return SB_SUCCESS;
}

/**
 * @brief Initialize the satellite-specific Slimbus parameters
 *
 * This function initializes the satellite-specific Slimbus 
 * parameters 
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SBResult SlimBus_InitSatParams(SlimBusDevCtxt *pDevCtxt)
{
  static DALSYSPropertyVar PropVar;
  DALResult result;

  result = DALSYS_GetPropertyValue( pDevCtxt->Base.hProp, "satellite_hw_owner", 0, &PropVar );
  if ( DAL_SUCCESS == result &&
       DALSYS_PROP_TYPE_UINT32 == PropVar.dwType &&
       FALSE != PropVar.Val.dwVal )
  {
    pDevCtxt->bIsHwOwner = TRUE;
  }

  result = DALSYS_GetPropertyValue( pDevCtxt->Base.hProp, "ee_assign", 0, &PropVar );
  if ( DAL_SUCCESS == result &&
       DALSYS_PROP_TYPE_BYTE_PTR == PropVar.dwType &&
       3 == PropVar.dwLen )
  {
    pDevCtxt->auEeAssign = PropVar.Val.pbVal;
  }
  else if ( pDevCtxt->bIsHwOwner )
  {
    return SB_ERROR;
  }

  return SB_SUCCESS;
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
static SBResult SlimBusMsgWorkLoopHandler(SlimBusEventHandle hEvent, void *pVoid)
{
  SlimBusDevCtxt *pDevCtxt = (SlimBusDevCtxt*)pVoid;
  SlimBusLinkNodeType SignalList;  /* List of events that will need signalling */

  /* Initialize the signalling list structure */
  RootNodeInit(&SignalList);

  SlimBus_LockDevice(pDevCtxt);

  SlimBus_MsgWorkerCallback(pDevCtxt, &SignalList);

  SlimBus_UnlockDevice(pDevCtxt);

  SlimBus_ProcessSignalList(pDevCtxt, &SignalList);

  return SB_SUCCESS;
}

/**
 * @brief Initialize the Slimbus services
 *
 * This function initializes the services used by the Slimbus 
 * driver 
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SBResult SlimBus_InitServices(SlimBusDevCtxt *pDevCtxt)
{
  uint32 uIdx = 0;
  SBResult result;
  DALResult dalResult = DAL_SUCCESS;
  
  /* Determine the HWIO base for the Slimbus registers */
  if ( NULL != pDevCtxt->pBSP->pszHwioBase )
  {
    DalDeviceHandle *hHWIO;
    result = DAL_HWIODeviceAttach(DALDEVICEID_HWIO, &hHWIO);
    if ( SB_SUCCESS == result )
    {
      DalDevice_Open(hHWIO, DAL_OPEN_SHARED);
      result = DalHWIO_MapRegion(hHWIO, pDevCtxt->pBSP->pszHwioBase, (uint8**)&pDevCtxt->hSB);
      pDevCtxt->hSB += pDevCtxt->pBSP->uHwioBaseOffset;
      DalDevice_Close(hHWIO);
      DAL_DeviceDetach(hHWIO);
    }
    if ( SB_SUCCESS != result || DAL_SUCCESS != dalResult )
    {
      DALSYS_LogEvent(pDevCtxt->Base.DevId, DALSYS_LOGEVENT_FATAL_ERROR, "Unable to map Slimbus HWIO!");
    }
  }
  else 
  {
    pDevCtxt->hSB = pDevCtxt->pBSP->uHwioBase;
  }

  if ( 0 == pDevCtxt->hSB )
  {
    /* error */
    return SB_ERROR;
  }

  /* Register for the Slimbus interrupt */
  DAL_DeviceAttach( DALDEVICEID_INTERRUPTCONTROLLER,
                    &((DALDevCtxt*)pDevCtxt)->hDALInterrupt);
  DalDevice_Open( ((DALDevCtxt*)pDevCtxt)->hDALInterrupt, DAL_OPEN_SHARED );

  DalInterruptController_RegisterISR( ((DALDevCtxt*)pDevCtxt)->hDALInterrupt,
                                      pDevCtxt->pBSP->uIntId, SlimBusDalIsr, (DALISRCtx) pDevCtxt,
                                      DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER);

  /* Setup the timer workloop */
  DALSYS_RegisterWorkLoopEx("SlimBusTimer", 4096, 0, 4, &pDevCtxt->hTimerWorkLoop, NULL);
  DALSYS_EventCreate(DALSYS_EVENT_ATTR_WORKLOOP_EVENT,
                     &pDevCtxt->hTimerWorkLoopEvent, NULL);
  DALSYS_AddEventToWorkLoop(pDevCtxt->hTimerWorkLoop,
                            SlimBusTimerWorkLoopHandler,
                            pDevCtxt, pDevCtxt->hTimerWorkLoopEvent, NULL);

  {
    DALSYSEventObj InitEventObj;
    /* Create temporary initialization event handle */
    DALSYS_EventCreate(DALSYS_EVENT_ATTR_NORMAL, &pDevCtxt->hInitEvent, &InitEventObj);

    /* Signal the timer workloop and wait until it has initialized */
    DALSYS_EventCtrl(pDevCtxt->hTimerWorkLoopEvent, DALSYS_EVENT_CTRL_TRIGGER);
    DALSYS_EventWait(pDevCtxt->hInitEvent);

    /* We're done with the initialization event handle, destroy it now */
    DALSYS_DestroyObject(pDevCtxt->hInitEvent);
    pDevCtxt->hInitEvent = NULL;
  }

  /* Setup Rx messaging workloop */
  DALSYS_RegisterWorkLoopEx("SlimBusMsg", 4096, 0, 4, &pDevCtxt->hMsgWorkLoop, NULL);
  DALSYS_EventCreate(DALSYS_EVENT_ATTR_WORKLOOP_EVENT,
                     &pDevCtxt->hMsgWorkLoopEvent, NULL);
  DALSYS_AddEventToWorkLoop(pDevCtxt->hMsgWorkLoop, SlimBusMsgWorkLoopHandler,
                            pDevCtxt, pDevCtxt->hMsgWorkLoopEvent, NULL);

  SlimBus_CreateSyncEvent(&pDevCtxt->hExtClkEvent, &pDevCtxt->ExtClkEventObj);

  DALSYS_EventCreate(DALSYS_EVENT_ATTR_TIMEOUT_EVENT|
                     DALSYS_EVENT_ATTR_NON_DEFERRED|
                     DALSYS_EVENT_ATTR_NORMAL,
                     &pDevCtxt->hClkToggleTimerEvent, NULL);
  
  /* Initialize the HAL */
  HAL_sb_Init(pDevCtxt->hSB, NULL);
  
  if ( NULL == pDevCtxt->Plat.hTlmm )
  {
    if ( DAL_SUCCESS != DAL_TlmmDeviceAttach(DALDEVICEID_TLMM, &pDevCtxt->Plat.hTlmm) )
    {
      return SB_ERROR;
    }
  }

  /* Configure the inactive configurations for the GPIOs */
  for ( uIdx = 0; uIdx < SB_MAX_GPIOS; uIdx++ )
  {
    uint32 uGpioNum = 0;
    uint32 uCfg = pDevCtxt->pBSP->uaGpioConfig[uIdx];
    
    if(uCfg != 0)
    {
      if(!(pDevCtxt->pBSP->bIsLpiTlmm))
      {    
        if (DAL_SUCCESS != DalTlmm_GetGpioNumber(pDevCtxt->Plat.hTlmm, uCfg, &uGpioNum))
        {
          return SB_ERROR;
        }
        if (DAL_SUCCESS != DalTlmm_SetInactiveConfig(pDevCtxt->Plat.hTlmm, uGpioNum,
                             DAL_GPIO_CFG(uGpioNum, 0, DAL_GPIO_INPUT, DAL_GPIO_KEEPER, DAL_GPIO_2MA)))
        {
          return SB_ERROR;
        }
      }
      else
      {
        if(!(uTlmm_GetGpioNumber(uCfg, &uGpioNum)))
		{
	      return SB_ERROR;
		}		
        if(!(uTlmm_SetInactiveConfig(uGpioNum,
	                           UTLMM_GPIO_CFG(uGpioNum, 0, UTLMM_GPIO_INPUT, UTLMM_GPIO_KEEPER, UTLMM_GPIO_2MA))))
		{
	      return SB_ERROR;
		}
      }
    }
  }
  
  return SB_SUCCESS;
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
SBResult SlimBusDalMasterWorkLoopHandler(SlimBusEventHandle hEvent, void *pVoid)
{
  SBResult result;
  SlimBusDevCtxt *pDevCtxt = (SlimBusDevCtxt*)pVoid;

  SlimBus_LockDevice(pDevCtxt);

  result = SlimBusMasterWorkLoopHandler(hEvent, pVoid);
  
  SlimBus_UnlockDevice(pDevCtxt);

  return result;
}

/**
 * @brief Initialize the master-specific Slimbus services
 *
 * This function initializes the master-specific services use by 
 * the Slimbus driver 
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SBResult SlimBus_InitMasterServices(SlimBusDevCtxt *pDevCtxt)
{
  DALResult result;
  DALSYSPropertyVar PropVar;

  result = DALSYS_GetPropertyValue( pDevCtxt->Base.hProp, "use_gpio_int", 0, &PropVar );
  if ( DAL_SUCCESS != result ||
       DALSYS_PROP_TYPE_UINT32 != PropVar.dwType ||
       FALSE != PropVar.Val.dwVal )
  {
    pDevCtxt->bUseGpioInt = TRUE;
  }
  
  if(pDevCtxt->bUseGpioInt)
  {
    if(pDevCtxt->pBSP->bIsLpiTlmm)
    {
	  return SB_ERROR;
	}
  }

  
  if ( NULL == pDevCtxt->Plat.hGpioInt )
  {
    if ( DAL_SUCCESS != DAL_DeviceAttachEx(NULL, DALDEVICEID_GPIOINT, GPIOINT_INTERFACE_VERSION, &pDevCtxt->Plat.hGpioInt) )
    {
      return SB_ERROR;
    }
  }
  
  if ( NULL == pDevCtxt->hMasterWorkLoopEvent )
  {
    DALSYS_RegisterWorkLoopEx("SlimBusMaster", 4096, 0, 4, &pDevCtxt->hMasterWorkLoop, NULL);
    DALSYS_EventCreate(DALSYS_EVENT_ATTR_WORKLOOP_EVENT,
                       &pDevCtxt->hMasterWorkLoopEvent, NULL);
    DALSYS_AddEventToWorkLoop(pDevCtxt->hMasterWorkLoop, SlimBusDalMasterWorkLoopHandler,
                              pDevCtxt, pDevCtxt->hMasterWorkLoopEvent, NULL);
  }
  
  return SB_SUCCESS;
}

/**
 * @brief Initialize the Slimbus GPIOs
 *
 * This function initializes the Slimbus GPIOs
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SBResult SlimBus_InitGpios(SlimBusDevCtxt *pDevCtxt)
{
  SBResult result;
  uint32 uIdx;
  
  /* Configure the GPIOs starting first with data GPIOs */  
  for ( uIdx = SB_MAX_GPIOS; uIdx > 0; uIdx-- )
  {
    uint32 uCfg = pDevCtxt->pBSP->uaGpioConfig[uIdx-1];

    if ( 0 != uCfg )
    {
      if(!(pDevCtxt->pBSP->bIsLpiTlmm))
      {
        result = DalTlmm_ConfigGpio(pDevCtxt->Plat.hTlmm, uCfg, DAL_TLMM_GPIO_ENABLE);
      }
      else
      {
        if(uTlmm_ConfigGpio(uCfg , UTLMM_GPIO_ENABLE))
        {
          result = SB_SUCCESS;
        }
        else
        {
          result = SB_ERROR;
        }
      } 

      if ( SB_SUCCESS != result )
      {
        return result;
      }
    }
  }

  return SB_SUCCESS;
}

/**
 * @brief Check for external GPIO toggle
 *
 * This function checks for an external GPIO toggle on the clock 
 * line. 
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return TRUE if external GPIO toggle was detected, FALSE 
 *         otherwise
 */
SBBOOL SlimBus_CheckGpioClockToggle(SlimBusDevCtxt *pDevCtxt)
{
  SBBOOL bDetected = FALSE;
  uint32 uGpioNum = 0;
  uint32 uGpioCfg;
  DALSYSEventHandle hEvents[] = { pDevCtxt->hExtClkEvent, pDevCtxt->hClkToggleTimerEvent };
  uint32 uIdx = 0;
  
  if(!(pDevCtxt->pBSP->bIsLpiTlmm))
  {
    DalTlmm_GetGpioNumber(pDevCtxt->Plat.hTlmm, pDevCtxt->pBSP->uaGpioConfig[0], &uGpioNum);
    uGpioCfg = DAL_GPIO_CFG(uGpioNum, 0, DAL_GPIO_INPUT, DAL_GPIO_KEEPER, DAL_GPIO_2MA);
  }
  else
  {
    uTlmm_GetGpioNumber(pDevCtxt->pBSP->uaGpioConfig[0], &uGpioNum);
    uGpioCfg = UTLMM_GPIO_CFG(uGpioNum, 0, UTLMM_GPIO_INPUT, UTLMM_GPIO_KEEPER, UTLMM_GPIO_2MA);  	
  }
  
  if ( !HAL_sb_DeviceIsEnabled(pDevCtxt->hSB, HAL_SB_DEVICE_FRAMER) )
  {
	if(!(pDevCtxt->pBSP->bIsLpiTlmm))
	{
      DalTlmm_ConfigGpio(pDevCtxt->Plat.hTlmm, uGpioCfg, DAL_TLMM_GPIO_ENABLE);
	}
	else
	{
      uTlmm_ConfigGpio(uGpioCfg, UTLMM_GPIO_ENABLE);
	}
  }
  
  pDevCtxt->bCheckingExtClkToggle = TRUE;
  pDevCtxt->bExtClkToggleDetected = FALSE;
  
  SlimBus_EventReset(pDevCtxt->hExtClkEvent);
    
  SlimBus_ClkGpioIntEnable(pDevCtxt);

  DALSYS_EventMultipleWait(hEvents, sizeof(hEvents)/sizeof(hEvents[0]), 5000, &uIdx);
  
  SlimBus_LockExtClkToggle(pDevCtxt);
  
  if ( pDevCtxt->bCheckingExtClkToggle )
  {
    SlimBus_ClkGpioIntDisable(pDevCtxt);
    pDevCtxt->bCheckingExtClkToggle = FALSE;
  }
  if ( pDevCtxt->bExtClkToggleDetected )
  {
    bDetected = TRUE;
    pDevCtxt->bExtClkToggleDetected = FALSE;
  }
  
  SlimBus_UnlockExtClkToggle(pDevCtxt);

  if ( bDetected )
  {
    SB_LOG_VERB_0("Detected external clock toggle");
    
    return TRUE;
  }
  else
  {
    SB_LOG_VERB_0("External clock toggle not detected");

    return FALSE;
  }
}

#if 0
/**
 * @brief Forcefully toggle the data line GPIO
 *
 * This function will forcefully toggle the data line GPIO
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 */
void SlimBus_ToggleDataGpio(SlimBusDevCtxt *pDevCtxt)
{
  SBResult result;
  uint32 uGpioNum = 0;
  DALGpioValueType uVal, uVal2, uVal3;

  DalTlmm_GetGpioNumber(pDevCtxt->Plat.hTlmm, pDevCtxt->pBSP->uaGpioConfig[1], &uGpioNum);

  DalTlmm_GpioIn(pDevCtxt->Plat.hTlmm, pDevCtxt->pBSP->uaGpioConfig[1], &uVal);
  DalTlmm_GpioOut(pDevCtxt->Plat.hTlmm, pDevCtxt->pBSP->uaGpioConfig[1], uVal);
  DalTlmm_ConfigGpio(pDevCtxt->Plat.hTlmm,
                     DAL_GPIO_CFG(uGpioNum, 0, DAL_GPIO_OUTPUT, DAL_GPIO_NO_PULL, DAL_GPIO_16MA),
                     DAL_TLMM_GPIO_ENABLE);
  DalTlmm_GpioOut(pDevCtxt->Plat.hTlmm, pDevCtxt->pBSP->uaGpioConfig[1], !uVal);
  DalTlmm_GpioIn(pDevCtxt->Plat.hTlmm, pDevCtxt->pBSP->uaGpioConfig[1], &uVal2);

  SLIMBUS_BUSYWAIT(1000);

  result = DalTlmm_ConfigGpio(pDevCtxt->Plat.hTlmm,
                              pDevCtxt->pBSP->uaGpioConfig[1],
                              DAL_TLMM_GPIO_ENABLE);
  SLIMBUS_BUSYWAIT(1);
  DalTlmm_GpioIn(pDevCtxt->Plat.hTlmm, pDevCtxt->pBSP->uaGpioConfig[1], &uVal3);
  
  SB_LOG_VERB_5("Toggled data line GPIO (gpio: %d) (from: %d) (cfg: %d) (to: %d) (to2: %d)", uGpioNum, uVal, !uVal, uVal2, uVal3);
}
#endif

/**
 * @brief De-initialize the Slimbus GPIOs
 *
 * This function de-initializes the Slimbus GPIOs
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return SB_SUCCESS on success, error code on error
 */
SBResult SlimBus_DeinitGpios(SlimBusDevCtxt *pDevCtxt)
{
  SBResult result;
  uint32 uIdx;
  
  /* De-init the GPIOs starting first with the clock GPIO */
  for ( uIdx = 0; uIdx < SB_MAX_GPIOS; uIdx++ )
  {
    uint32 uCfg = pDevCtxt->pBSP->uaGpioConfig[uIdx];
    
    if ( 0 != uCfg )
    {
      if(!(pDevCtxt->pBSP->bIsLpiTlmm))
      {
        result = DalTlmm_ConfigGpio(pDevCtxt->Plat.hTlmm, uCfg, DAL_TLMM_GPIO_DISABLE);
      }
      else
      {
        if(uTlmm_ConfigGpio(uCfg , UTLMM_GPIO_DISABLE))
        {
          result = SB_SUCCESS;
        }
        else
        {
          result = SB_ERROR;
        }
      }	  
      if ( SB_SUCCESS != result )
      {
        return result;
      }
    }
  }

  return SB_SUCCESS;
}

/**
 * @brief Determine whether the driver in satellite mode 
 *        should use clock control functionality
 *
 * This function determines whether the driver in satellite mode 
 * should use clock control functionality 
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return TRUE if satellite clock control should be used, FALSE 
 *         otherwise
 */
SBBOOL SlimBus_UseSatelliteClockControl(SlimBusDevCtxt *pDevCtxt)
{
  DALResult result = DAL_SUCCESS;
  DALSYSPropertyVar PropVar;

  result = DALSYS_GetPropertyValue( pDevCtxt->Base.hProp, "satellite_clock_ctrl", 0, &PropVar );
  if ( DAL_SUCCESS == result &&
       DALSYS_PROP_TYPE_UINT32 == PropVar.dwType &&
       FALSE != PropVar.Val.dwVal )
  {
    return TRUE;
  }

  return FALSE;
}

/*------------------------------------------------------------------------------
Following functions are for DALDriver specific functionality
------------------------------------------------------------------------------*/
SBResult
SlimBus_DriverInit(SlimBusDrvCtxt *pdrvCtxt)
{
  return SB_SUCCESS;
}

SBResult
SlimBus_DriverDeInit(SlimBusDrvCtxt *pdrvCtxt)
{
  return SB_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

/**
 * @brief Detach from the DAL device
 *
 * This function detaches the client from the DAL device
 * 
 * @param[in] h Client handle to the DAL device
 * 
 * @return Reference count for the DAL device
 */
static uint32
SlimBus_DalDeviceDetach(uint32 ClientId, DalDeviceHandle* h)
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  uint32 dwref;
  DALSYSSyncHandle hClientSync = pClientCtxt->hSync;
  
  /* Make sure client handle is closed */
  DalDevice_Close(h);
  
  /* Do common detach */
  SlimBus_DeviceDetach(ClientId, h);
  
  dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
  if(0 == dwref)
  {
    /*Release the client ctxt;*/
    SlimBus_Free(h->pClientCtxt);
    h->pClientCtxt = NULL;
  }

  DALSYS_DestroyObject(hClientSync);

  return dwref;
}

/**
 * @brief Retrieve DAL information regarding the device
 *
 * This function retrieves DAL information regarding the device
 * 
 * @param[in] h  Client DAL device handle
 * @param[out] info  Location to store the DAL information
 * @param[in] infoSize  Size of the DAL information structure
 * 
 * @return SB_SUCCESS 
 */
static SBResult
SlimBus_DeviceInfo(uint32 ClientId, DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
  info->Version = DALSLIMBUS_INTERFACE_VERSION;
  return SB_SUCCESS;
}

/**
 * @brief Handle DAL power events
 *
 * This function handles DAL power events
 * 
 * @param[in] h  Client DAL device handle
 * @param[in] PowerCmd  DAL power command
 * @param[in] PowerDomain  DAL power domain
 * 
 * @return SB_SUCCESS 
 */
static SBResult
SlimBus_DevicePowerEvent(uint32 ClientId, DalDeviceHandle *h, DalPowerCmd PowerCmd,
                                DalPowerDomain PowerDomain )
{
  return SB_SUCCESS;
}

/**
 * @brief Handle DAL system requests
 *
 * This function handles DAL system requests
 * 
 * @param[in] h  Client DAL device handle
 * @param[in] ReqIdx  System request index
 * @param[in] SrcBuf  Input buffer for the system request
 * @param[in] SrcBufLen  Length of the input buffer
 * @param[out] DestBuf  Output buffer for the system request
 * @param[in] DestBufLen  Length of the output buffer
 * @param[out] DestBufLenReq  Amount of data written into the 
 *       output buffer
 * 
 * @return SB_SUCCESS 
 */
static SBResult
SlimBus_DeviceSysRequest(uint32 ClientId, DalDeviceHandle* h,DalSysReq ReqIdx,
                         const void* SrcBuf, uint32 SrcBufLen,
                         void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
  return SB_SUCCESS;
}

/**
 * @brief Initialize the DAL device
 *
 * This function initializes the DAL device
 * 
 * @param[in] h  Client handle to the DAL device
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
static SBResult
SlimBus_DeviceInit(uint32 ClientId, DalDeviceHandle *h)
{
  SlimBusClientCtxt *pClientCtxt = SB_GET_PCLIENTCTXT(h);
  SlimBusDevCtxt *pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt);
  SBResult result;

  SlimBus_LockDevice(pDevCtxt);

  result = SlimBus_DeviceInitInternal(pDevCtxt);
  
    SlimBus_UnlockDevice(pDevCtxt);
 
  return result;
}

/**
 * @brief De-initialize the DAL device
 *
 * This function de-initializes the DAL device
 * 
 * @param[in] h Client DAL device handle
 * 
 * @return SB_SUCCESS 
 */
static SBResult
SlimBus_DeviceDeInit(uint32 ClientId, DalDeviceHandle *h)
{
  /* Note: we have marked the DAL device as static, so we will never de-init */
  return SB_SUCCESS;
}


/**
 * @brief Initialize the vtable for a client handle
 *
 * This function intializes the vtable for a client handle
 * 
 * @param[in] pclientCtxt  Pointer to the DAL client context
 */
static void
SlimBus_InitInterface(SlimBusClientCtxt* pclientCtxt)
{
  static const DalSlimBus vtbl =
  {
    {
      SlimBus_DeviceAttach,
      SlimBus_DalDeviceDetach,
      SlimBus_DeviceInit,
      SlimBus_DeviceDeInit,
      SlimBus_DeviceOpen,
      SlimBus_DeviceClose,
      SlimBus_DeviceInfo,
      SlimBus_DevicePowerEvent,
      SlimBus_DeviceSysRequest
    },
   
    SlimBus_GetDeviceLogicalAddr,
    SlimBus_RequestRootFreq,
    SlimBus_GetLastError,
    SlimBus_AllocMasterPorts,
    SlimBus_DeallocMasterPorts,
    SlimBus_GetSlavePortHandle,
    SlimBus_ConfigMasterPort,
    SlimBus_ConfigMasterPipe,
    SlimBus_NextReserveMsgBandwidth,
    SlimBus_ReadValueElement,
    SlimBus_WriteValueElement,
    SlimBus_ExchangeValueElement,
    SlimBus_ReadInfoElement,
    SlimBus_ClearInfoElement,
    SlimBus_ReadAndClearInfoElement,
    SlimBus_AllocDataChannel,
    SlimBus_NextDefineDataChannel,
    SlimBus_ConnectPortToChannel,
    SlimBus_DisconnectPortFromChannel,
    SlimBus_NextDataChannelControl,
    SlimBus_DoReconfigureNow,
    SlimBus_GetDataChannelStatus,
    SlimBus_DeallocDataChannel,
    SlimBus_RegisterBamEvent,
    SlimBus_SubmitBamTransfer,
    SlimBus_GetBamIOVec,
    SlimBus_GetBamEvent,
    SlimBus_GetPortFifoStatus,
    SlimBus_RegisterPortEventEx,
    SlimBus_GetPortEvent,
    SlimBus_AllocProgressCounter,
    SlimBus_DeallocProgressCounter,
    SlimBus_InitProgressCounter,
    SlimBus_ReadProgressCounter,
    SlimBus_ReadProgressCounterVFRStatus,
    SlimBus_ClearProgressCounterVFRStatus,
    SlimBus_AllocResourceGroup,
    SlimBus_DeallocResourceGroup,
    SlimBus_AddResourceToGroup,
    SlimBus_AllocSharedDataChannel,
    SlimBus_GetBamIOVecEx,
    SlimBus_GetHardwareCapability,
    SlimBus_ReadProgressCounterTimestamp,
    SlimBus_SendUserDefinedMessage,
    SlimBus_SetBatchBufferSize,
    SlimBus_SendBatchedMessages,
    SlimBus_AllocMasterPortWithEndPoints,
    SlimBus_GetMaxEndPointsSupported,
    SlimBus_RegisterClockGearEvent,
    SlimBus_GetPortIndex
  };
  /*--------------------------------------------------------------------------
     Depending upon client type setup the vtables (entry points)
  --------------------------------------------------------------------------*/
  pclientCtxt->RefHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
  pclientCtxt->RefHandle.pVtbl  = &vtbl;
  pclientCtxt->RefHandle.pClientCtxt = pclientCtxt;
}

/**
 * @brief Attach to the DAL device
 *
 * This function attaches to the Slimbus DAL device
 * 
 * @param[in] pszArg  Pointer to null-terminated argument string
 * @param[in] DeviceId  DAL device ID for the DAL device
 * @param[in] phDalDevice  Pointer to the DAL client handle
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
SBResult
SlimBus_DeviceAttach(const char *pszArg, DALDEVICEID DeviceId,
                            DalDeviceHandle **phDalDevice)
{
  SBResult nErr;
  static SlimBusDrvCtxt drvCtxt =
  {
    {
      {
        SlimBus_DriverInit,
        SlimBus_DriverDeInit
      },
      SLIMBUS_TARGET_NUM_DEVICES,
      sizeof(SlimBusDevCtxt)
    }
  };

  SlimBusClientCtxt *pClientCtxt =  NULL;
  SlimBusDevCtxt *pDevCtxt;

  nErr = SlimBus_Malloc(sizeof(SlimBusClientCtxt),(void **)&pClientCtxt, NULL);
  if (NULL != pClientCtxt &&
      SB_SUCCESS == nErr)
  {
    memset(pClientCtxt,0,sizeof(SlimBusClientCtxt));
  
    *phDalDevice = NULL;
  
    nErr = DALFW_AttachToDevice(DeviceId,(DALDrvCtxt *)&drvCtxt,
                                (DALClientCtxt *)pClientCtxt);
    if (SB_SUCCESS == nErr)
    {
      SlimBus_InitInterface(pClientCtxt);
      SlimBus_AddRef(&(pClientCtxt->RefHandle));
      *phDalDevice = (DalDeviceHandle *)&(pClientCtxt->RefHandle);
      if ( NULL == (pDevCtxt = SB_GET_PDEVCTXT(pClientCtxt->hDevCtxt)) )
      {
        nErr = SB_ERROR;
      }
    }
    if (SB_SUCCESS == nErr)
    {
      nErr = SlimBus_CreateClientLock(pClientCtxt);
    }
    if (SB_SUCCESS == nErr)
    {
      if (NULL == pDevCtxt->hExtClkSync)
      {
        nErr = DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE_INTERRUPT,
                                 &pDevCtxt->hExtClkSync, NULL);
      }
    }
    if (SB_SUCCESS == nErr)
    {
      if (NULL == pDevCtxt->hIsrSync)
      {
        nErr = DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE_INTERRUPT,
                                 &pDevCtxt->hIsrSync, NULL);
      }
    }
    if (SB_SUCCESS == nErr)
    {
      if (NULL == pDevCtxt->hSync)
      {
        nErr = DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE_INTERRUPT,
                                 &pDevCtxt->hSync, NULL);
      }
    }
    
    if ( SB_SUCCESS != nErr )
    {
      SlimBus_Free(pClientCtxt);
      pClientCtxt = NULL;
    }
  }
  return nErr;
}

/**
 * @brief Start a timer
 *
 * This function starts a timer
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * @param[in] hTimer  Timer handle
 * @param[in] hEvent  Event handle
 * @param[in] uTimeoutUsec  Microsecond timeout
 */
void SlimBus_TimerStart(SlimBusDevCtxt *pDevCtxt, SlimBusTimerHandle hTimer, SlimBusEventHandle hEvent, uint32 uTimeoutUsec)
{
  if (0 == uTimeoutUsec)
  {
    DALSYS_TimerStop(hEvent);
  }
  else
  {
    DALSYS_TimerStart(hEvent, uTimeoutUsec);
  }
}
/**
 * @brief Stop a timer
 *
 * This function stops a timer
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * @param[in] hTimer  Timer handle
 * @param[in] hEvent  Event handle
 */
void SlimBus_TimerStop(SlimBusDevCtxt *pDevCtxt, SlimBusTimerHandle hTimer, SlimBusEventHandle hEvent)
{
  DALSYS_TimerStop(hEvent);
}

/**
 * @brief GPIO wakeup interrupt handler
 *
 * This function performs GPIO interrupt wakeup functionality.
 * 
 * @param[in] pCtxt  Pointer to the interrupt handler context
 * 
 * @return NULL
 */
void* SlimBus_GpioIntIsr(GPIOINTISRCtx pCtxt)
{
  SlimBusDevCtxt *pDevCtxt = (SlimBusDevCtxt*) pCtxt;
  
  SlimBus_LockDevice(pDevCtxt);
  
  if ( !pDevCtxt->bClockIsOn )
  {
    SB_LOG_INFO_0("Waking up bus due to external data line toggle");  

    pDevCtxt->leaveLowPowerFcn(pDevCtxt);
    SlimBus_DeviceInitHardware(pDevCtxt);
    
    /* Use a timer that will check for enter low power mode, if we don't have 
       a reason to stay active longer than a certain time */    
    SlimBus_TimerStart(pDevCtxt,
                       pDevCtxt->hBamResetTimerHandle,
                       pDevCtxt->hBamResetTimerEvent,
                       SB_DEFAULT_RX_TIMEOUT/SB_TIMEOUT_FACTOR);
  }
  
  SlimBus_UnlockDevice(pDevCtxt);
  
  return NULL;
}

/**
 * @brief Enable the GPIO wakeup interrupt
 *
 * This function enables the GPIO wakeup interrupt
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return SB_SUCCESS on success, error code otherwise
 */
SBResult SlimBus_GpioIntEnable(SlimBusDevCtxt *pDevCtxt)
{
  if ( NULL != pDevCtxt->Plat.hGpioInt && pDevCtxt->bUseGpioInt )
  {
    DALResult result;
    uint32 uGpioNum = 0;
    DALGpioValueType uVal;
    GPIOIntTriggerType eTrigger;

    DalTlmm_GetGpioNumber(pDevCtxt->Plat.hTlmm, pDevCtxt->pBSP->uaGpioConfig[1], &uGpioNum);

    DalTlmm_GpioIn(pDevCtxt->Plat.hTlmm, pDevCtxt->pBSP->uaGpioConfig[1], &uVal);
    if ( uVal )
    {
      eTrigger = GPIOINT_TRIGGER_FALLING;
    }
    else
    {
      eTrigger = GPIOINT_TRIGGER_RISING;
    }

    SB_LOG_VERB_1("Registering for GPIO interrupt (input: %d)", uVal);

    result = GPIOInt_RegisterIsr(pDevCtxt->Plat.hGpioInt, uGpioNum, eTrigger, SlimBus_GpioIntIsr, (GPIOINTISRCtx)pDevCtxt);
    if ( DAL_SUCCESS != result )
    {
      SB_LOG_ERR_0("Failed to register for GPIO interrupt");

      return SB_ERROR;
    }
  }
  
  return SB_SUCCESS;
}

/**
 * @brief Disable the GPIO wakeup interrupt
 *
 * This function disables the GPIO wakeup interrupt
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return SB_SUCCESS on success, error code otherwise
 */
SBResult SlimBus_GpioIntDisable(SlimBusDevCtxt *pDevCtxt)
{
  if ( NULL != pDevCtxt->Plat.hGpioInt && pDevCtxt->bUseGpioInt )
  {
    DALResult result;
    uint32 uGpioNum = 0;
    DALGpioValueType uVal;

    DalTlmm_GetGpioNumber(pDevCtxt->Plat.hTlmm, pDevCtxt->pBSP->uaGpioConfig[1], &uGpioNum);
    DalTlmm_GpioIn(pDevCtxt->Plat.hTlmm, pDevCtxt->pBSP->uaGpioConfig[1], &uVal);
  
    SB_LOG_VERB_1("Deregistering for GPIO interrupt (input: %d)", uVal);
    
    result = GPIOInt_DeregisterIsr(pDevCtxt->Plat.hGpioInt, uGpioNum, SlimBus_GpioIntIsr);
    if ( DAL_SUCCESS != result )
    {
      SB_LOG_ERR_0("Failed to deregister GPIO interrupt");
  
      return SB_ERROR;
    }
  }
  
  return SB_SUCCESS;
}

/**
 * @brief Clock GPIO wakeup interrupt handler
 *
 * This function performs clock GPIO interrupt wakeup 
 * functionality. 
 * 
 * @param[in] pCtxt  Pointer to the interrupt handler context
 * 
 * @return NULL
 */
void* SlimBus_ClkGpioIntIsr(GPIOINTISRCtx pCtxt)
{
  SlimBusDevCtxt *pDevCtxt = (SlimBusDevCtxt*) pCtxt;
  
  SlimBus_LockExtClkToggle(pDevCtxt);
  
  if ( pDevCtxt->bCheckingExtClkToggle )
  {
    pDevCtxt->bCheckingExtClkToggle = FALSE;
    pDevCtxt->bExtClkToggleDetected = TRUE;
    SlimBus_ClkGpioIntDisable(pDevCtxt);
    SlimBus_EventTrigger(pDevCtxt->hExtClkEvent);
  }
  
  SlimBus_UnlockExtClkToggle(pDevCtxt);
  
  return NULL;
}

/**
 * @brief Enable the clock GPIO wakeup interrupt
 *
 * This function enables the clock GPIO wakeup interrupt
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return SB_SUCCESS on success, error code otherwise
 */
SBResult SlimBus_ClkGpioIntEnable(SlimBusDevCtxt *pDevCtxt)
{
  if ( NULL != pDevCtxt->Plat.hGpioInt )
  {
    DALResult result;
    uint32 uGpioNum = 0;
    DALGpioValueType uVal;
    GPIOIntTriggerType eTrigger;

    DalTlmm_GetGpioNumber(pDevCtxt->Plat.hTlmm, pDevCtxt->pBSP->uaGpioConfig[0], &uGpioNum);

    DalTlmm_GpioIn(pDevCtxt->Plat.hTlmm, pDevCtxt->pBSP->uaGpioConfig[0], &uVal);
    if ( uVal )
    {
      eTrigger = GPIOINT_TRIGGER_FALLING;
    }
    else
    {
      eTrigger = GPIOINT_TRIGGER_RISING;
    }

    result = GPIOInt_RegisterIsr(pDevCtxt->Plat.hGpioInt, uGpioNum, eTrigger, SlimBus_ClkGpioIntIsr, (GPIOINTISRCtx)pDevCtxt);
    if ( DAL_SUCCESS != result )
    {
      SB_LOG_ERR_0("Failed to register for clock GPIO interrupt");

      return SB_ERROR;
    }
  }
  
  return SB_SUCCESS;
}

/**
 * @brief Disable the clock GPIO wakeup interrupt
 *
 * This function disables the clock GPIO wakeup interrupt
 * 
 * @param[in] pDevCtxt  Pointer to the Slimbus device
 * 
 * @return SB_SUCCESS on success, error code otherwise
 */
SBResult SlimBus_ClkGpioIntDisable(SlimBusDevCtxt *pDevCtxt)
{
  if ( NULL != pDevCtxt->Plat.hGpioInt )
  {
    DALResult result;
    uint32 uGpioNum = 0;

    DalTlmm_GetGpioNumber(pDevCtxt->Plat.hTlmm, pDevCtxt->pBSP->uaGpioConfig[0], &uGpioNum);
  
    result = GPIOInt_DeregisterIsr(pDevCtxt->Plat.hGpioInt, uGpioNum, SlimBus_ClkGpioIntIsr);
    if ( DAL_SUCCESS != result )
    {
      SB_LOG_ERR_0("Failed to deregister clock GPIO interrupt");
  
      return SB_ERROR;
    }
  }
  
  return SB_SUCCESS;
}

