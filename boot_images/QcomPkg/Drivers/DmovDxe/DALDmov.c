/*
===============================================================================

FILE:         DALDmov.c

DESCRIPTION:
  This file implements the data mover device driver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

===============================================================================

                                Edit History


when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/02/11   ss      Added Return Error condition in Dmov_FlushChannel() API
10/18/10   an      moved client registration to Dmov_Open
08/12/10   py      Add support for using the same channel in different modes in parallel
04/12/10   an      legacy structures cleanup
03/05/10   an      dynamic xfer pool allocation and
                   reuse of cmd ptr list pool for legacy and async xfers
02/19/10   an      split shared data structures among devices to avoid race condition
11/24/09   an      fix warnings
10/06/09   an      added Dmov_NotifyClients
09/24/09   an      save flush type for flush queues option
06/19/09   MK      Changed to enable ISR when we register & Check for unmatched
                   Dmov_Close Calls
05/06/09   an      moved ISR registration to the end of Dmov_DeviceInit
04/29/09   an      fix compilation warnings
04/12/09   MK      Added clock management
04/09/09   an      Added context info into logs
01/12/09   ah      Fixed some comments
01/18/08   ah      Created

===============================================================================
                   Copyright (c) 2008-2011 Qualcomm Technologies Incorporated.
                          All Rights Reserved.
                        QUALCOMM Proprietary/GTDR
===============================================================================
*/

/* ----------------------------------------------------------------------------
**                           INCLUDES
** ------------------------------------------------------------------------- */

#include "HALdmov.h"
#include "DDIDmov.h"
#include "DALDmov.h"
#include "DALDmovi.h"
#include "DALDmoviClkControl.h"

#include "string.h"
#include "DALDeviceId.h"

#ifdef DMOV_LEGACY_SUPPORT
  uint32 DMOV_LEGACY_FLAG = 0x11111111;
#endif

/* ============================================================================
**
**  Dmov_NotifyClients
**
**  Description:
**    Moves ready transfers from NOTIFY queue to DONE queue and
**    notifies clients for ready transfers
**
**  Parameters:
**    pDevCtxt - Pointer to the DMOV device context
**
**  Return:
**
**  Dependencies: must not be called under device critical section context
**
** ========================================================================= */
void Dmov_NotifyClients
(
  DmovDevCtxt* pDevCtxt
)
{
  uint32 dwChan, dwToken = 0x0;
  Dmov_ChanType* pChan;
  DmovTransferQItem* pTransferDescriptor;
  DALSYSEventHandle hEvent;
#ifdef DMOV_LEGACY_SUPPORT
  dmov_xfer_callback_f_type_legacy  callback_ptr;
  dmov_result_s_type_legacy         ResultsLegacy;
  uint32 pUserData = 0, i;
#endif

  while (DALFW_Q_Cnt(&pDevCtxt->DmovDevXferNotify) > 0)
  {
	  (void)DALSYS_SyncEnter(pDevCtxt->hDeviceSynchronization);

	  pTransferDescriptor = DALFW_Q_Get(&pDevCtxt->DmovDevXferNotify);

	  if (pTransferDescriptor != NULL)
	  {
	      dwChan = DMOV_GET_TRANSFER_CHAN(pTransferDescriptor->dwToken);
          //check if this channel number is valid
          //to satisfy KW:
          if (dwChan < DMOV_MAX_NUM_CHANNELS_PER_DEV && dwChan < pDevCtxt->dwNumChans )
          {
            pChan = pDevCtxt->Chans[dwChan];
          }
          else
          {
              //we got a wrong channel number somehow ( shouldn't happen )
              DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR,
                            "Channel number contained in transfer token invalid");
              return;
          }
		  // copy transfer event for notification

		  hEvent = pTransferDescriptor->hEvent;

		  if (hEvent != NULL)
		  {
			  dwToken = pTransferDescriptor->dwToken;

			  // will trigger it later, but need increment counter in sync context
			  pChan->dwEventTriggeredCount++;
		  }

#ifdef DMOV_LEGACY_SUPPORT
		  if (DMOV_TRANSFER_LEGACY == pTransferDescriptor->TransferType)
		  {
			  callback_ptr = pTransferDescriptor->callback_ptr;

			  if (callback_ptr)
			  {
				  ResultsLegacy.result = pTransferDescriptor->Results.dwResultValue;

				  for (i = 0; i < DMOV_MAX_NUM_FLUSH_STATE_REGS; i++)
				  {
				    ResultsLegacy.flush_state[i] = pTransferDescriptor->Results.FlushState[i];
				  }

				  pUserData = pTransferDescriptor->Results.pUserData;

				  // will call it later, but need increment counter in sync context
				  pChan->dwXferCallbackCount_Legacy++;
			  }

			  /* Clear the Transfer Descriptor */
			  Dmovi_ClearTransferDescriptor(pTransferDescriptor);
			  /* Return transfer descriptor to the free pool */
			  DALFW_Q_Put(&pDevCtxt->DmovXferFreePool, &pTransferDescriptor->Link);

			  // update stats
			  pDevCtxt->dwQueuedCount--;
		  }
		  else
		  {
		      callback_ptr = NULL;
#endif

		      // put descriptor into DONE queue
		      DALFW_Q_Put(&pChan->DONE_Q, &pTransferDescriptor->Link);
#ifdef DMOV_LEGACY_SUPPORT
		  }
#endif
	  }
	  else
	  {
#ifdef DMOV_LEGACY_SUPPORT
		  callback_ptr = NULL;
#endif

		  hEvent = NULL;
	  }

	  (void)DALSYS_SyncLeave(pDevCtxt->hDeviceSynchronization);

      // notify clients outside of sync context

#ifdef DMOV_LEGACY_SUPPORT
      if (callback_ptr != NULL)
      {
        callback_ptr(&ResultsLegacy, (void *)pUserData);
      }
#endif

      if (hEvent != NULL)
      {
        DALSYS_EventCtrlEx(hEvent, DALSYS_EVENT_CTRL_TRIGGER, dwToken, NULL, 0);
      }
  }
}

/* ============================================================================
**
**  Dmov_DevISR
**
**  Description:
**    Service routine for the Data Mover device interrupt.
**
**  Parameters:
**    pArg - Pointer to void representing the context of the device causing the
**           interrupt
**
**  Return:
**    None
**
**  Dependencies:
**    None
**
** ========================================================================= */

void * Dmov_DevISR
(
  void * pArg
)
{
  Dmov_ChanType * pChan;
  uint32 dwIRQStatus, dwIRQMask;
  uint32 dwDomain, dwChan;

  /* Cast the Arg to DmovDevCtxt */
  DmovDevCtxt *pDevCtxt = (DmovDevCtxt *) pArg;

  (void)DALSYS_SyncEnter(pDevCtxt->hDeviceSynchronization);
  /* Increment the dwOpenCount here so that the clocks are not turned off
     even if clients close their handle in the callback
   */
  Dmovi_DevEnable(pDevCtxt);

  do
  {
    /* Get the interrupt status and clear pending interrupts.
     * Reading all IRQ_STATUS registers in a loop minimizes
     * that chance that another interrupt will occur before
     * all outstanding interrups are cleared.  This is important
     * because the current Modem Subsystem PIC expects an edge
     * interrupt, but the DM generates a level.  Unless all
     * interrupts are cleared, the level is not negated and
     * another edge will not be detected.
     */
    dwIRQStatus = 0;
    for (dwDomain = 0; dwDomain < pDevCtxt->dwNumSecDomains; dwDomain++)
    {
      if (HAL_dmov_SecDomainHasAccess(pDevCtxt->dwDeviceInstance, dwDomain) != TRUE)
      {
        /* Domain is *not* controlled by local processor */
        continue;
      }

      dwIRQStatus |= HAL_dmov_GetDomainPendingInterrupts(pDevCtxt->dwDeviceInstance, dwDomain);
    }

    /* Process pending interrupts for all channels in all domains
     * controlled by the local processor
     */
    dwIRQMask = dwIRQStatus;
    for (dwChan = 0;  dwIRQMask != 0 && dwChan < pDevCtxt->dwNumChans; dwChan++)
    {
      /* Is an IRQ pending for this channel? */
      if ((dwIRQMask & 1) != 0 &&
		  HAL_dmov_ChanIsAccessible(pDevCtxt->dwDeviceInstance, dwChan))
      {
        /* Yes, so call the channel handler for this channel */
        pChan = pDevCtxt->Chans[dwChan];
        Dmovi_ChanHandler(pDevCtxt, pChan);
      }

      /* Shift the next channel IRQ status bit down into bit 0 */
      dwIRQMask >>= 1;
    }

  } while (dwIRQStatus != 0);

  /* Update statistics */
  pDevCtxt->dwIRQCounter++;

  DalInterruptController_InterruptDone(((DALDevCtxt *)pDevCtxt)->hDALInterrupt,
                                       pDevCtxt->InterruptId);

  Dmovi_DevDisable(pDevCtxt);
  (void)DALSYS_SyncLeave(pDevCtxt->hDeviceSynchronization);

  Dmov_NotifyClients(pDevCtxt);

  return DAL_SUCCESS;
}

/* ============================================================================
**
**  Dmov_DriverInit
**
**  Description:
**    DMOV driver initialization.
**
**  Parameters:
**    pCtxt - Pointer to the driver context
**
**  Return:
**    DAL_SUCCESS - If the driver initialized successfully
**    DAL_ERROR   - If initialization encountered an error
**
**  Dependencies:
**    None
**
** ========================================================================= */

DALResult Dmov_DriverInit
(
  DmovDrvCtxt *pCtxt
)
{
  pCtxt->DrvInitialized = FALSE;

  /* Initialize Synchronization Object.
   * This is used to synchronize between client calls and interrupt context
   * on our variables and HAL.
   */
  if( DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
  								       &(pCtxt->hDriverSynchronization),
  									   NULL) )
  {
    pCtxt->eError = DMOV_DRV_SYNC_OBJECT_CREATION_FAILED;
  	return DAL_ERROR;
  }


  pCtxt->DrvInitialized = TRUE;
  DMOV_LOGEVENT(0, DALSYS_LOGEVENT_INFO, "Driver initialized successfully");

  return DAL_SUCCESS;
}

/* ============================================================================
**
**  Dmov_DriverDeInit
**
**  Description:
**    DMOV driver de-initialization.
**
**  Parameters:
**    pCtxt - Pointer to the driver context
**
**  Return:
**    DAL_SUCCESS - If the driver de-initialized successfully
**    DAL_ERROR   - If de-initialization encountered an error
**
**  Dependencies:
**    None
**
** ========================================================================= */

DALResult Dmov_DriverDeInit
(
  DmovDrvCtxt *pCtxt
)
{
  if(DAL_SUCCESS != DALSYS_DestroyObject(pCtxt->hDriverSynchronization))
  {
    pCtxt->eError = DMOV_DRV_SYNC_OBJECT_DESTRUCTION_FAILED;
    DMOV_LOGEVENT(0, DALSYS_LOGEVENT_ERROR,
                    "Driver failed to destroy sync object");
  	return DAL_ERROR;
  }

  DMOV_LOGEVENT(0, DALSYS_LOGEVENT_INFO, "Driver DeInit successfully");
  return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are declared in DalDevice Interface.
------------------------------------------------------------------------------*/
/* ============================================================================
**
**  Dmov_DeviceInit
**
**  Description:
**    DMOV device initialization.
**
**  Parameters:
**    pCtxt - Pointer to the DMOV Client context
**
**  Return:
**    DAL_SUCCESS - If the driver de-initialized successfully
**    DAL_ERROR   - If de-initialization encountered an error
**
**  Dependencies:
**    None
**
** ========================================================================= */

DALResult Dmov_DeviceInit
(
  DmovClientCtxt *pCtxt
)
{
  char* pszBSPVersion = NULL;
  uint32 dwChan, dwQueue;
  Dmov_ChanType * pChan;
  DALSYSPropertyVar PropVar;
  HAL_dmov_SecurityLevel eSecurityLevel;
  DmovDevCtxt * pDevCtxt = pCtxt->pDmovDevCtxt;
  uint32 * pCmdListPtrMem = NULL;
  uint32 * pCmdListPtrPhysMem = NULL;
  uint32 * pCmdListMem = NULL;
  uint32 * pCmdListPhysMem = NULL;
  uint32   dwTransfer;
  DmovTransferQItem * pTransferDescriptor;


  (void)DALSYS_SyncEnter(pDevCtxt->pDmovDrvCtxt->hDriverSynchronization);


  /* Initial state */
  pDevCtxt->eState = DMOV_DEV_NOT_INITILIZED;
  pDevCtxt->eError = DMOV_DEV_NO_ERROR;
  pDevCtxt->eCurrentPerfLvl =DMOV_PERFLVL_MAX;

  switch (pDevCtxt->DevId)
  {
    case DALDEVICEID_DMOV_DEVICE_0:
        pDevCtxt->dwDeviceInstance = HAL_DMOV_DEV_ADM;
        break;
    case DALDEVICEID_DMOV_DEVICE_1:
        pDevCtxt->dwDeviceInstance = HAL_DMOV_DEV_MDM;
        break;
    case DALDEVICEID_DMOV_DEVICE_2:
        pDevCtxt->dwDeviceInstance = HAL_DMOV_DEV_ADM_1;
        break;
    default:
        /* non existing device id */
        DMOV_LOGEVENT(0, DALSYS_LOGEVENT_FATAL_ERROR,"Non existing device id ");
        (void)DALSYS_SyncLeave(pDevCtxt->pDmovDrvCtxt->hDriverSynchronization);
        return DAL_ERROR;
  }

  /* Check if this DMOV device is present */
  if (FALSE == HAL_dmov_DeviceExists(pDevCtxt->dwDeviceInstance))
  {
    (void)DALSYS_SyncLeave(pDevCtxt->pDmovDrvCtxt->hDriverSynchronization);

    return DAL_ERROR;
  }

  eSecurityLevel = HAL_dmov_GetSecurityLevel(pDevCtxt->dwDeviceInstance);

  if( DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE_INTERRUPT,
  								       &(pDevCtxt->hDeviceSynchronization),
  									   NULL) )
  {
    pDevCtxt->eError = DMOV_DEV_SYNC_OBJECT_CREATION_FAILED;

    (void)DALSYS_SyncLeave(pDevCtxt->pDmovDrvCtxt->hDriverSynchronization);

    return DAL_ERROR;
  }

  /* Query if we have to manage clocks for this device or not */

  if( DAL_SUCCESS == DALSYS_GetPropertyValue(pDevCtxt->hProp,
                                             "clocks_mgd",
                                             0,
                                             &PropVar) )
  {
     if(0 != PropVar.Val.dwVal)
     {
        pDevCtxt->bClocksManaged = TRUE;
     }
     else
     {
        pDevCtxt->bClocksManaged = FALSE;
     }
  }
  else
  {
     DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_WARNING,
                     "Dmov_DeviceInit : Clocks are not going to be managed");
     pDevCtxt->bClocksManaged = FALSE;
  }
  /* Attach to the clock driver and get the required clock IDs*/
  if ( DAL_SUCCESS != Dmovi_GetClockIDs(pDevCtxt))
  {

     DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                     "Dmov_DeviceInit : Failed to get clock IDs");
    return DAL_ERROR;
  }

  /* nothing running thus far */
  pDevCtxt->dwOpenCount = 0;

  /* Initialize the device's tranfers descriptor memory pools */
  (void)DALFW_Q_Init(&pDevCtxt->DmovXferFreePool);

  /* initialize notification queue */
  (void)DALFW_Q_Init(&pDevCtxt->DmovDevXferNotify);

  /* Query the size of transfer request pool */
  if( DAL_SUCCESS == DALSYS_GetPropertyValue(pDevCtxt->hProp,
                                             "req_pool_size",
                                             0,
                                             &PropVar) )
  {
    pDevCtxt->nDmovTransferCount = PropVar.Val.dwVal;
  }
  else
  {
    /* property not found, use default value */
    pDevCtxt->nDmovTransferCount = DMOV_MAX_NUM_TRANSFERS_QUEUED;
  }

  if (pDevCtxt->nDmovTransferCount > 0)
  {
    /* Allocate transfer request pool */
    if ( DAL_SUCCESS != DALSYS_Malloc(pDevCtxt->nDmovTransferCount*sizeof(pDevCtxt->pDmovTransfer[0]),
	                              (void**)&pDevCtxt->pDmovTransfer))
    {
      pDevCtxt->eError = DMOV_DEV_MEM_ALLOCATION_FAILED;
      DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR,
                      "Driver failed to allocate memory for the transfer request pool");

      (void)DALSYS_SyncLeave(pDevCtxt->pDmovDrvCtxt->hDriverSynchronization);

      return DAL_ERROR;
    }

    /* Memory needed for Command List Pointers and Command lists*/
    pDevCtxt->dwCmdMemNeeded = pDevCtxt->nDmovTransferCount * DMOV_CMD_LIST_PTR_SIZE;

    /* Boot and Tools allocate their own command list memory */
    if (eSecurityLevel > HAL_DMOV_NON_SECURE_BOOT)
    {
      /* Memory needed for Command lists*/
      pDevCtxt->dwCmdMemNeeded += pDevCtxt->nDmovTransferCount *
                                  DMOV_MAX_NUM_CMDS_PER_LIST *
                                  DMOV_MAX_CMD_SIZE;
    }

    /* Allocate the Command Memory pool from uncached physical memory */
    if ( DAL_SUCCESS != DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_UNCACHED |
                                              DALSYS_MEM_PROPS_PHYS_CONT,
                                              DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                              DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                              pDevCtxt->dwCmdMemNeeded,
                                              &pDevCtxt->hCmdMem,
                                              NULL) )
    {
      pDevCtxt->eError = DMOV_DEV_MEM_ALLOCATION_FAILED;
      DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR,
                      "Driver failed to allocate CLP memory");
      (void)DALSYS_SyncLeave(pDevCtxt->pDmovDrvCtxt->hDriverSynchronization);

      return DAL_ERROR;
    }

    DALSYS_MemInfo(pDevCtxt->hCmdMem, &pDevCtxt->CmdMemInfo);

    pCmdListPtrMem = (uint32 *)(pDevCtxt->CmdMemInfo.VirtualAddr);
    pCmdListPtrPhysMem = (uint32 *)(pDevCtxt->CmdMemInfo.PhysicalAddr);

    if (eSecurityLevel > HAL_DMOV_NON_SECURE_BOOT)
    {
      /* Command lists */
      uint32 dwCmdListOffset = pDevCtxt->nDmovTransferCount *
					 (DMOV_CMD_LIST_PTR_SIZE/sizeof(*pCmdListPtrMem));

      pCmdListMem = pCmdListPtrMem + dwCmdListOffset;
      pCmdListPhysMem = pCmdListPtrPhysMem + dwCmdListOffset;
    }

    for (dwTransfer = 0; dwTransfer < pDevCtxt->nDmovTransferCount; dwTransfer++)
    {
      /* Init transfer descriptor queue link and place it in the free pool */
      pTransferDescriptor = &pDevCtxt->pDmovTransfer[dwTransfer];
      DALSYS_memset(pTransferDescriptor, 0, sizeof(*pTransferDescriptor));
      (void)DALFW_Q_Link(pTransferDescriptor, &pTransferDescriptor->Link);
      DALFW_Q_Put(&pDevCtxt->DmovXferFreePool, &pTransferDescriptor->Link);

      /* CLP */
      pTransferDescriptor->pCmdListPtrBuffer = pCmdListPtrMem;
      pTransferDescriptor->pCmdListPtrPhysBuffer = pCmdListPtrPhysMem;

      /* Increment by double words for 8-byte alignment */
      pCmdListPtrMem += DMOV_CMD_LIST_PTR_SIZE/sizeof(*pCmdListPtrMem);
      pCmdListPtrPhysMem += DMOV_CMD_LIST_PTR_SIZE/sizeof(*pCmdListPtrMem);

      if (eSecurityLevel > HAL_DMOV_NON_SECURE_BOOT)
      {
        /* Command lists */
        pTransferDescriptor->pCmdBuffer = pCmdListMem;
        pTransferDescriptor->pCmdPhysBuffer = pCmdListPhysMem;

        /* Command lists */
        pCmdListMem += ( DMOV_MAX_NUM_CMDS_PER_LIST *
                       (DMOV_MAX_CMD_SIZE / sizeof(*pCmdListMem)));
        pCmdListPhysMem += ( DMOV_MAX_NUM_CMDS_PER_LIST *
                       (DMOV_MAX_CMD_SIZE / sizeof(*pCmdListMem)));
      }
    }
  }

  /* Enable clocks for initialization */
  Dmovi_DevEnable(pDevCtxt);

  /* Boot time, reset the device HW */
  if (HAL_DMOV_SECURE_BOOT == eSecurityLevel)
  {
    Dmovi_DevReset(pDevCtxt);
  }

   /* Secure processor will turn DMOV clock on and initialize the HW */
   if (HAL_DMOV_SECURE_BOOT == eSecurityLevel ||
       HAL_DMOV_SECURE_AMSS == eSecurityLevel )
   {
     Dmovi_DevInit(pDevCtxt);
   }

  /* HAL initialization */
  HAL_dmov_Init(pDevCtxt->dwDeviceInstance, &pszBSPVersion);

  pDevCtxt->dwNumChans = HAL_dmov_GetNumChannels( pDevCtxt->dwDeviceInstance );
  pDevCtxt->dwNumSecDomains = HAL_dmov_GetNumSecDomains(pDevCtxt->dwDeviceInstance);
  pDevCtxt->dwNumFlushRegisters = HAL_dmov_GetNumFlushRegisters(pDevCtxt->dwDeviceInstance);

  /* Allocate channel structures */
  for (dwChan = 0; dwChan < pDevCtxt->dwNumChans ; dwChan++)
  {
    if ( HAL_dmov_ChanIsAccessible(pDevCtxt->dwDeviceInstance,dwChan))
    {
        if ( DAL_SUCCESS != DALSYS_Malloc(sizeof(Dmov_ChanType),(void**)&pChan))
        {
            /* no need to freeup the already allocated memory. If this malloc fails
             * then the driver won't be operational anyway and it's a fatal error.
             */
      		pDevCtxt->eError = DMOV_DEV_MEM_ALLOCATION_FAILED;
      		DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_FATAL_ERROR,
                      "Driver failed to allocate memory for Channels");
      		(void)DALSYS_SyncLeave(pDevCtxt->pDmovDrvCtxt->hDriverSynchronization);
      		return DAL_ERROR;
    	}

    }
    else
    {
        pDevCtxt->Chans[dwChan] = NULL;
    	continue;
    }
    pDevCtxt->Chans[dwChan] = pChan;
    DALSYS_memset(pChan, 0, sizeof(*pChan));
    pChan->dwChan = dwChan;
    pChan->dwDevice = pDevCtxt->dwDeviceInstance;
    pChan->eState = DMOV_CHAN_STATE_INVALID;
    pChan->eError = DMOV_CHAN_NO_ERROR;
    pChan->hEvent = NULL;
    /* Get the security domain controlling this channel */
    pChan->dwDomain = HAL_dmov_GetChanSecDomain(pDevCtxt->dwDeviceInstance, dwChan);

      if (HAL_DMOV_CHAN_MODE_INTERRUPT ==
          HAL_dmov_GetChanMode(pDevCtxt->dwDeviceInstance, dwChan))
      {
        pChan->dwChanOpMode = DMOV_CHAN_MODE_INTERRUPT;
      }
      else
      {
        pChan->dwChanOpMode = DMOV_CHAN_MODE_POLLING;
      }

    /* Initialize command queues, one queue per channel for now */
    pChan->dwNumQueues = DMOV_MAX_NUM_QUEUES;

    for (dwQueue = 0; dwQueue < pChan->dwNumQueues; dwQueue++)
    {
      (void)DALFW_Q_Init(&pChan->CMD_Q[dwQueue]);
    }
    (void)DALFW_Q_Init(&pChan->PENDING_Q);
    (void)DALFW_Q_Init(&pChan->DONE_Q);

      /* Hardware is operational */
      pChan->eState = DMOV_CHAN_STATE_RUN;

  }

  /* Indicate that the device has been initialized */
  DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO,
                  "DMOV Device Initialized!");

  pDevCtxt->eState = DMOV_DEV_RUN;

  /* Obtain an handle to the interrupt controller */
  if( DAL_SUCCESS == DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER,
        			      &(((DALDevCtxt *)pDevCtxt)->hDALInterrupt)) )
  {
    /* Per DAL protocol invoke Open on the DAL Driver component. On success
     * this is ready to use
     */
    if( DAL_SUCCESS == DalDevice_Open(((DALDevCtxt *)pDevCtxt)->hDALInterrupt,
  							    DAL_OPEN_SHARED) )
    {
      /* From DAL Props get the Int Id we wish to use for this device, also
       * get Base Virtual Addr
       */
      if( DAL_SUCCESS == DALSYS_GetPropertyValue(pDevCtxt->hProp,
      					         "interrupt_id",
      						 0,
      						 &PropVar) )
      {
        pDevCtxt->InterruptId = PropVar.Val.dwVal;
        pDevCtxt->bInterruptsAvailable = TRUE;
      }
      else
      {
        DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_WARNING,
                      "Device failed to get interrupt_id property");
      }
    }
    else
    {
      DAL_DeviceDetach(((DALDevCtxt *)pDevCtxt)->hDALInterrupt);

      pDevCtxt->eError = DMOV_DEV_OPEN_INTERRUPTCONTROLLER_FAILED;
      DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_WARNING,
                      "Device failed to open Interrupt Controller");
      pDevCtxt->bInterruptsAvailable = FALSE;
    }
  }
  else
  {
    pDevCtxt->eError = DMOV_DEV_ATTACH_INTERRUPTCONTROLLER_FAILED;
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_WARNING,
                    "Device failed to attach to Interrupt Controller");
    pDevCtxt->bInterruptsAvailable = FALSE;
  }

  /* If we have interrupts available attach now */
  /* Register an ISR with the DAL Interrupt Controller for this Int Id */
  /* For boot and tools Interrupt Controller is not present */
  if (TRUE == pDevCtxt->bInterruptsAvailable)
  {
    if( DAL_SUCCESS != DalInterruptController_RegisterISR(((DALDevCtxt *)pDevCtxt)->hDALInterrupt,
                                                        pDevCtxt->InterruptId,
                                                        Dmov_DevISR,
                                                        (DALISRCtx)pDevCtxt,
                                                        TRUE) )
    {
      pDevCtxt->eError = DMOV_DEV_REGISTER_ISR_FAILED;
      DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_WARNING,
                      "Device failed to register ISR");
      pDevCtxt->bInterruptsAvailable = FALSE;
    }
  }

  /* If we don't have interrupts available put channels into polling mode */
  if (FALSE == pDevCtxt->bInterruptsAvailable)
  {
    for (dwChan = 0; dwChan < pDevCtxt->dwNumChans; dwChan++)
    {
      if (HAL_dmov_ChanIsAccessible(pDevCtxt->dwDeviceInstance, dwChan))
      {
	    HAL_dmov_SetChanMode(pDevCtxt->dwDeviceInstance, dwChan, HAL_DMOV_CHAN_MODE_POLLING);
        pDevCtxt->Chans[dwChan]->dwChanOpMode = DMOV_CHAN_MODE_POLLING;
      }
    }
  }

  /* Turn off device clocks after initialization, note ADM clock will remain ON
     but that is policed by clkrgm -- Modem side DM Driver knows nothing about
     that.

     Note that this is only for AMSS. At Boot this clock is always on.
     */
  Dmovi_DevDisable(pDevCtxt);

  (void)DALSYS_SyncLeave(pDevCtxt->pDmovDrvCtxt->hDriverSynchronization);

  return DAL_SUCCESS;
}

/* ============================================================================
**
**  Dmov_DeviceDeInit
**
**  Description:
**    DMOV device de-initialization.
**
**  Parameters:
**    pCtxt - Pointer to the DMOV Client context
**
**  Return:
**    DAL_SUCCESS - If the driver de-initialized successfully
**    DAL_ERROR   - If de-initialization encountered an error
**
**  Dependencies:
**    None
**
** ========================================================================= */

DALResult Dmov_DeviceDeInit
(
  DmovClientCtxt *pCtxt
)
{
  DmovDevCtxt * pDevCtxt = pCtxt->pDmovDevCtxt;
  uint32 dwChan;

  (void)DALSYS_SyncEnter(pDevCtxt->pDmovDrvCtxt->hDriverSynchronization);


  if (TRUE == pDevCtxt->bInterruptsAvailable)
  {

    /* Unregister ISR from the DAL Interrupt Controller. */
    if( DAL_SUCCESS != DalInterruptController_Unregister(((DALDevCtxt *)pDevCtxt)->hDALInterrupt,
                                                       pDevCtxt->InterruptId) )
    {
      pDevCtxt->eError = DMOV_DEV_UNREGISTER_ISR_FAILED;
      DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_WARNING,
                      "Device failed to unregister ISR");
    }

    /* Per DAL protocol Close this DAL Driver component. On success detach. */
    if(DAL_SUCCESS == DalDevice_Close(((DALDevCtxt *)pDevCtxt)->hDALInterrupt))
    {
      /* This driver is being de-initalized close all connection to the Int Ctrl */
      if(DAL_SUCCESS != DAL_DeviceDetach(((DALDevCtxt *)pDevCtxt)->hDALInterrupt))
      {
        DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_WARNING,
                        "DMOV Device DeInit: Failed to detach from Int Ctrl");
      }
    }
    else
    {
      DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_WARNING,
                      "DMOV Device DeInit: Failed to close Int Ctrl");
    }
  }

  /* Free memory allocated by the driver for the device*/
  if (DAL_SUCCESS != DALSYS_DestroyObject(pDevCtxt->hCmdMem))
  {
    pDevCtxt->eError = DMOV_DEV_MEM_FREE_FAILED;
    DMOV_LOGEVENT(0, DALSYS_LOGEVENT_ERROR,
                    "Driver failed to free CL/CLP memory");
  }

  (void)DALSYS_Free(pDevCtxt->pDmovTransfer);
  pDevCtxt->pDmovTransfer = NULL;
  /* Free channel memory */
  for (dwChan = 0; dwChan < DMOV_MAX_NUM_CHANNELS_PER_DEV; dwChan++)
  {
      if ( pDevCtxt->Chans[dwChan] )
      {
          if (DAL_SUCCESS != DALSYS_Free(pDevCtxt->Chans[dwChan]))
          {
             pDevCtxt->eError = DMOV_DEV_MEM_FREE_FAILED;
             DMOV_LOGEVENT(0, DALSYS_LOGEVENT_ERROR,
                             "Driver failed to free channel memory");
          }
      }
      else
      {
          continue;
      }
  }
  (void)DALSYS_SyncLeave(pDevCtxt->pDmovDrvCtxt->hDriverSynchronization);


  return DAL_SUCCESS;
}

/* ============================================================================
**
**  Dmov_PowerEvent
**
**  Description:
**    Select the minimum power level for operation with the client
**
**  Parameters:
**    pCtxt - Pointer to the DMOV Client context
**
**  Return:
**    DAL_SUCCESS - If the perf level was recorded/set for the client
**    DAL_ERROR   - If invalid perf level was supplied
**
**  Dependencies:
**    None
**
** ========================================================================= */

DALResult Dmov_PowerEvent
(
  DmovClientCtxt * pCtxt,
  DalPowerCmd      PowerCmd,
  DalPowerDomain   PowerDomain
)
{
   DmovDevCtxt * pDevCtxt = pCtxt->pDmovDevCtxt;
   /* Currently we support only discrete power levels */
   if(PowerCmd >= DMOV_NUM_PERFLVLS)
   {
      return DAL_ERROR;
   }

   if (pCtxt->nHandleID == DMOV_CLIENT_NOT_REGISTERED)
   {
      if (Dmov_RegisterClient(pCtxt) != DAL_SUCCESS)
      {
         return DAL_ERROR;
      }
   }

   (void)DALSYS_SyncEnter(pDevCtxt->hDeviceSynchronization);

   /* See if the client has already a open handle */
   if(pCtxt->dwOpenCount)
   {

      /* We will remove the client's previous power level mask */
      pDevCtxt->nActiveClientsPerfLvls[pCtxt->ePerfLvl] &= ~(1ULL << pCtxt->nHandleID);
      pDevCtxt->nActiveClientsPerfLvls[PowerCmd]  |= 1ULL << pCtxt->nHandleID;
      /* We will directly invoke Control API, as it doesnt affect dwOpenCount */
      Dmovi_ControlSetPerfLvl(pDevCtxt);
   }
   pCtxt->ePerfLvl = (DmovPerfLvlType)PowerCmd;

   (void)DALSYS_SyncLeave(pDevCtxt->hDeviceSynchronization);


  return DAL_SUCCESS;
}

/* ============================================================================
**
**  Dmov_RegisterClient
**
**  Description:
**    Registers the client's handle with the given DMOV Device Context.
**
**  Parameters:
**    pCtxt - Pointer to the DMOV Client context
**
**  Return:
**    DAL_SUCCESS - If the client registered successfully
**    DAL_ERROR   - If DMOV device ran out of handle IDs
**
**  Dependencies:
**    None
**
** ========================================================================= */
DALResult Dmov_RegisterClient
(
   DmovClientCtxt * pCtxt
)
{
   uint32 index;
   DmovDevCtxt *pDevCtxt = pCtxt->pDmovDevCtxt;
   DALResult ret = DAL_ERROR;

   (void)DALSYS_SyncEnter(pDevCtxt->hDeviceSynchronization);


   if (pDevCtxt->bInterruptsAvailable == FALSE)
   {
      pCtxt->dwChannelsModes = (uint32)(-DMOV_CHAN_MODE_POLLING);
   }
   else
   {
      pCtxt->dwChannelsModes = (uint32)(-DMOV_CHAN_MODE_INTERRUPT);
   }

   if(pDevCtxt->hClientsCount < (DMOV_MAX_CLIENTS-1)) // size -1 : to accomadate this entry
   {
      /* Find the first free node in the list */
      index = 0;
      while(index <= pDevCtxt->hClientsCount)
      {
         if(pDevCtxt->hClients[index] == NULL)
         {
            pDevCtxt->hClients[index]=pCtxt;
            pCtxt->nHandleID = index;
            pDevCtxt->hClientsCount++;
            ret = DAL_SUCCESS;
            break;
         }
         index++;
      }
   }
   else
   {
      DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                      "Failed to register the client - Ran out of handles!");
      ret = DAL_ERROR;
   }

   (void)DALSYS_SyncLeave(pDevCtxt->hDeviceSynchronization);

   return ret;
}

/* ============================================================================
**
**  Dmov_DeregisterClient
**
**  Description:
**    Deregisters the client's handle from the given DMOV Device Context.
**
**  Parameters:
**    pCtxt - Pointer to the DMOV Client context
**
**  Return:
**    DAL_SUCCESS - If the client was de-registered successfully
**    DAL_ERROR   - If de-registration encountered an error
**
**  Dependencies:
**    None
**
** ========================================================================= */

DALResult Dmov_DeregisterClient
(
   DmovClientCtxt * pCtxt
)
{
   DmovDevCtxt *pDevCtxt = pCtxt->pDmovDevCtxt;
   DALResult ret = DAL_ERROR;

   if(pDevCtxt == NULL)
   {
      ret = DAL_SUCCESS;
   }
   else
   {
      (void)DALSYS_SyncEnter(pDevCtxt->hDeviceSynchronization);

      if(pCtxt->nHandleID < DMOV_MAX_CLIENTS)
      {
         pDevCtxt->hClients[pCtxt->nHandleID]=NULL;
         pDevCtxt->hClientsCount--;
         ret = DAL_SUCCESS;
      }
      else
      {
         DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                         "Failed to deregister the client");
         ret = DAL_ERROR;
      }

      pCtxt->nHandleID = DMOV_CLIENT_NOT_REGISTERED;

      (void)DALSYS_SyncLeave(pDevCtxt->hDeviceSynchronization);

   }


   return ret;
}

/* ============================================================================
**
**  Dmov_Open
**
**  Description:
**
**  Parameters:
**    pCtxt - Pointer to the DMOV Client context
**    nAccessMode - Access mode to the dmov device by different clients
**
**  Return:
**    DAL_SUCCESS - If the driver de-initialized successfully
**    DAL_ERROR   - If de-initialization encountered an error
**
**  Dependencies:
**    None
**
** ========================================================================= */

DALResult Dmov_Open
(
  DmovClientCtxt * pCtxt,
  uint32           dwaccessMode
)
{
  DmovDevCtxt *pDevCtxt = pCtxt->pDmovDevCtxt;

  if (pCtxt->nHandleID == DMOV_CLIENT_NOT_REGISTERED)
  {
       if (Dmov_RegisterClient(pCtxt) != DAL_SUCCESS)
       {
         return DAL_ERROR;
       }
  }

  (void)DALSYS_SyncEnter(pDevCtxt->hDeviceSynchronization);

  if(++pCtxt->dwOpenCount==1)
  {
       /* Add the client to the active clients' list */
       pDevCtxt->nActiveClientsMask |= 1ULL << pCtxt->nHandleID;
       pDevCtxt->nActiveClientsPerfLvls[pCtxt->ePerfLvl]  |= 1ULL << pCtxt->nHandleID;
  }

  Dmovi_DevEnable(pDevCtxt);

  (void)DALSYS_SyncLeave(pDevCtxt->hDeviceSynchronization);


  return DAL_SUCCESS;
}

/* ============================================================================
**
**  Dmov_Close
**
**  Description:
**
**  Parameters:
**    pCtxt - Pointer to the DMOV Client context
**
**  Return:
**    DAL_SUCCESS - If the driver de-initialized successfully
**    DAL_ERROR   - If de-initialization encountered an error
**
**  Dependencies:
**    None
**
** ========================================================================= */

DALResult Dmov_Close
(
  DmovClientCtxt *pCtxt
)
{
  DmovDevCtxt *pDevCtxt = pCtxt->pDmovDevCtxt;

  if(pCtxt->dwOpenCount == 0)
  {
    /* Legacy clients who don't maintain the matching open/close calls */
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_WARNING,
                    "Dmov_Close: Client %s was not open",pCtxt->pszClientName);
    return DAL_SUCCESS;
  }

  (void)DALSYS_SyncEnter(pDevCtxt->hDeviceSynchronization);

  if(--pCtxt->dwOpenCount == 0)
  {
      /* Remove the client from the active clients' list */
      pDevCtxt->nActiveClientsMask &= ~(1 << pCtxt->nHandleID);
      pDevCtxt->nActiveClientsPerfLvls[pCtxt->ePerfLvl] &= ~(1 << pCtxt->nHandleID);
  }

  Dmovi_DevDisable(pDevCtxt);

  (void)DALSYS_SyncLeave(pDevCtxt->hDeviceSynchronization);


  return DAL_SUCCESS;
}

/* ============================================================================
**
**  Dmov_Info
**
**  Description:
**
**  Parameters:
**    pCtxt - Pointer to the DMOV Client context
**    pdeviceInfo
**
**  Return:
**    DAL_SUCCESS - If the driver de-initialized successfully
**    DAL_ERROR   - If de-initialization encountered an error
**
**  Dependencies:
**    None
**
** ========================================================================= */

DALResult Dmov_Info
(
  DmovClientCtxt * pCtxt,
  DalDeviceInfo  * pdeviceInfo,
  uint32           dwSize
)
{
  return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalDmov Interface.
------------------------------------------------------------------------------*/

/* ============================================================================
**
**  Dmov_Transfer
**
**  Description:
**    Queue a Data Mover Command List.
**
**  Parameters:
**    pCtxt - Pointer to the DMOV Client context
**    TransferRequest -
**    RequestLength -
**    CommandList -
**    RequestStatus -
**
**  Return:
**    DAL_SUCCESS  - Success, command list was queued
**    DAL_ERROR    - Failure, command list was not queued
**
**  Dependencies:
**    Boot/Tools transfers should always be DMOV_TRANSFER_SYNCHRONOUS
**    Channel must first be initialized/started by a call to Dmov_DeviceInit().
**
** ========================================================================= */

DALResult Dmov_Transfer
(
  DmovClientCtxt            * pCtxt,
  const DmovTransferRequest * pTransferRequest,
  uint32                    * pCommandList,
  DmovReqStatus             * pRequestStatus
)
{
  DmovDevCtxt *pDevCtxt = pCtxt->pDmovDevCtxt;

  /*
     Check if the client has an open handle if not for now issue a warning for
     now.  Once stabilized this check if true will exit the function
   */
  if(NULL == pCtxt->dwOpenCount)
  {
     if(NULL != pCtxt->pszClientName) //Check needed for now (DAL 22 has the fix)
     {
        DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                     "DMOV Transfer: called by %s without opening the handle",
                     pCtxt->pszClientName);
     }
     else
     {
        DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                     "DMOV Transfer: called by %d without opening the handle",
                     pCtxt->nHandleID);

     }
  }

  (void)DALSYS_SyncEnter(pDevCtxt->hDeviceSynchronization);

  pCtxt->dwTransferID++;
  pCtxt->dwTransferID &= 0x0000FFFF;

  /* Verify whether the transfer is valid */
  if( DAL_SUCCESS != Dmovi_RequestIsValid(pCtxt,
                                          pTransferRequest,
                                          pCommandList,
                                          pRequestStatus) )
  {
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                    "Device Transfer: Transfer request is invalid");
	(void)DALSYS_SyncLeave(pDevCtxt->hDeviceSynchronization);
    return DAL_ERROR;
  }

  /* Queue the command */
  if( DAL_SUCCESS != Dmovi_QueueTransfer(pCtxt,
                                         pTransferRequest,
                                         pCommandList,
                                         pRequestStatus) )
  {
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                    "Device Transfer: Failed to queue the transfer command");
	(void)DALSYS_SyncLeave(pDevCtxt->hDeviceSynchronization);
    return DAL_ERROR;
  }
  else
  {
    pRequestStatus->eState = DMOV_REQUEST_OK;
  }

  (void)DALSYS_SyncLeave(pDevCtxt->hDeviceSynchronization);

  return DAL_SUCCESS;
}

/* ============================================================================
**
**  Dmov_GetTransferResult
**
**  Description:
**    Clients fetch the result of their transfers using this API.
**
**  Parameters:
**    pCtxt - Pointer to the DMOV Client context
**    dwToken -
**    Result -
**
**  Return:
**    DAL_SUCCESS  - Success, result returned
**    DAL_ERROR    - Failure, unable to return result
**
**  Dependencies:
**    None
**
** ========================================================================= */

DALResult  Dmov_GetTransferResult
(
  DmovClientCtxt * pCtxt,
  uint32           dwToken,
  DmovResult     * pResult
)
{
  DmovTransferQItem * pTransferDescriptor;
  DmovDevCtxt * pDevCtxt = pCtxt->pDmovDevCtxt;
  uint32 dwChan = DMOV_GET_TRANSFER_CHAN(dwToken);
  Dmov_ChanType * pChan;
  uint32 dwNumFlushRegs;
  DALBOOL bNotify = FALSE;

  /*
     Check if the client has an open handle if not for now issue a warning for
     now.  Once stabilized this check if true will exit the function
   */
  if(NULL == pCtxt->dwOpenCount)
  {
     if(NULL != pCtxt->pszClientName) //Check needed for now (DAL 22 has the fix)
     {
        DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                     "DMOV_GetTransferResult: called by %s without opening the handle",
                     pCtxt->pszClientName);
     }
     else
     {
        DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                     "DMOV_GetTransferResult: called by %d without opening the handle",
                     pCtxt->nHandleID);

     }
  }

  /* Check whether device is valid */
  if (FALSE == HAL_dmov_DeviceExists(pDevCtxt->dwDeviceInstance))
  {
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                    "DMOV GetTransferResult: DMOV device doesn't exist");
    return DAL_ERROR;
  }

  /* Check whether the channel obtained from token is valid */
  if (dwChan >= pDevCtxt->dwNumChans)
  {
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                    "DMOV_GetTransferResult: token is invalid: %d",
                    dwToken);

    return DAL_ERROR;
  }

  /* stupid: to satisfy KW */
  if ( pDevCtxt->dwDeviceInstance >= HAL_DMOV_MAX_NUM_DEVICES )
  {
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                    "DMOV GetTransferResult: Nonexistant DMOV device");
    return DAL_ERROR;
  }

  /* Is this channel local? */
  if (!HAL_dmov_ChanIsAccessible(pDevCtxt->dwDeviceInstance, dwChan))
  {
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                    "DMOV GetTransferResult: poll a chan whose domain NOT controlled by local processor");
    return DAL_ERROR;
  }
  /* to satisfy KW */
  if ( dwChan >= DMOV_MAX_NUM_CHANNELS_PER_DEV )
  {
      DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                    "DMOV GetTransferResult: Channel number given is invalid");
      return DAL_ERROR;
  }

  pChan = pDevCtxt->Chans[dwChan];

  (void)DALSYS_SyncEnter(pDevCtxt->hDeviceSynchronization);

  /* check for channel error */
  pResult->eChanError = pChan->eError;

  if (pResult->eChanError == DMOV_CHAN_NO_ERROR &&
      pChan->eState > DMOV_CHAN_STATE_FATAL)
  {
      /* Poll for result only if the channel is in polling mode */
      if ( HAL_DMOV_CHAN_MODE_POLLING ==
           HAL_dmov_GetChanMode(pDevCtxt->dwDeviceInstance, dwChan) )
      {
        /* Poll the channel */
        Dmovi_ChanHandler(pDevCtxt, pChan);

		bNotify = TRUE;
	  }
  }

  (void)DALSYS_SyncLeave(pDevCtxt->hDeviceSynchronization);

  if (bNotify)
  {
	  Dmov_NotifyClients(pDevCtxt);
  }

#ifdef DMOV_LEGACY_SUPPORT
  /* Legacy hack, return if it was a legacy call */
  if (DMOV_LEGACY_FLAG == pResult->pUserData)
  {
      return DAL_SUCCESS;
  }
#endif

  (void)DALSYS_SyncEnter(pDevCtxt->hDeviceSynchronization);

  pCtxt->dwCurrentResultToken = dwToken;

  /* Check if this is transfer is done */
  pTransferDescriptor =
      (DmovTransferQItem *)DALFW_Q_LinearSearch( &pChan->DONE_Q,
                                            (DALFW_Q_CompareFuncType)Dmovi_CompareToken,
                                            (void *)&(pCtxt->dwCurrentResultToken) );

  if(pTransferDescriptor != NULL)
  {
    /* Transfer done */
    pResult->eState = pTransferDescriptor->Results.eState;
    pResult->dwResultValue = pTransferDescriptor->Results.dwResultValue;
    for (dwNumFlushRegs = 0;
         dwNumFlushRegs < pDevCtxt->dwNumFlushRegisters;
         dwNumFlushRegs++)
    {
      pResult->FlushState[dwNumFlushRegs] = pTransferDescriptor->Results.FlushState[dwNumFlushRegs];
    }
    pResult->pUserData = pTransferDescriptor->Results.pUserData;

    /* Clear the Transfer Descriptor */
    Dmovi_ClearTransferDescriptor(pTransferDescriptor);

    /* Take it out from the DONE queue */
    DALFW_Q_Delete(&pChan->DONE_Q, &pTransferDescriptor->Link);

    /* Return transfer descriptor to the free pool */
    DALFW_Q_Put(&pDevCtxt->DmovXferFreePool, &pTransferDescriptor->Link);

    pDevCtxt->dwQueuedCount--;
  }
  else /* Transfer not done */
  {
    /* Check if this transfer is awaiting notification */
    pTransferDescriptor =
        (DmovTransferQItem *)DALFW_Q_LinearSearch( &pDevCtxt->DmovDevXferNotify,
                                              (DALFW_Q_CompareFuncType)Dmovi_CompareToken,
                                              (void *)&(pCtxt->dwCurrentResultToken) );
    if(pTransferDescriptor != NULL)
    {
      pResult->eState = DMOV_TRANSFER_IN_PROGRESS;
    }
	else
	{
      /* Check if this transfer is issued and pending completion */
      pTransferDescriptor =
        (DmovTransferQItem *)DALFW_Q_LinearSearch( &pChan->PENDING_Q,
                                              (DALFW_Q_CompareFuncType)Dmovi_CompareToken,
                                              (void *)&(pCtxt->dwCurrentResultToken) );
      if(pTransferDescriptor != NULL)
      {
        pResult->eState = DMOV_TRANSFER_IN_PROGRESS;
      }
      else /* Transfer is not issued */
      {
        /* Check if this transfer is not issued yet */
        pTransferDescriptor =
          (DmovTransferQItem *)DALFW_Q_LinearSearch( &pChan->CMD_Q[0], // only 1 queue for now
                                                (DALFW_Q_CompareFuncType)Dmovi_CompareToken,
                                                (void *)(&pCtxt->dwCurrentResultToken) );
        if(pTransferDescriptor != NULL)
        {
          pResult->eState = DMOV_TRANSFER_WAITING_TO_BE_ISSUED;
        }
        else
        {
          /* Data mover has no record of this transfer! */
          pResult->eState = DMOV_TRANSFER_NON_EXISTENT;
        }
      }
	}
  }

  if (DALFW_Q_Cnt(&pChan->DONE_Q) > 0)
  {
    /* Tell the client he has other transfer resuts available */
    pResult->bMoreResultsAvailable = TRUE;
  }
  else
  {
    pResult->bMoreResultsAvailable = FALSE;
  }

  (void)DALSYS_SyncLeave(pDevCtxt->hDeviceSynchronization);
  return DAL_SUCCESS;
}

/* ============================================================================
**
**  Dmov_SetChanOperationMode
**
**  Description:
**    Sets the channel operating mode to interrupt or polling.
**
**  Parameters:
**    pCtxt - Pointer to the DMOV Client context
**    dwChan -
**    dwChanOpMode -
**
**  Return:
**    DAL_SUCCESS  - Success, Channel mode set
**    FALSE - Failure
**
**  Dependencies:
**    None
**
** ========================================================================= */

DALResult Dmov_SetChanOperationMode
(
  DmovClientCtxt * pCtxt,
  uint32           dwChan,
  uint32           dwChanOpMode
)
{
  DmovDevCtxt * pDevCtxt = pCtxt->pDmovDevCtxt;

  /*
     Check if the client has an open handle if not for now issue a warning for
     now.  Once stabilized this check if true will exit the function
   */
  if(NULL == pCtxt->dwOpenCount)
  {
     if(NULL != pCtxt->pszClientName) //Check needed for now (DAL 22 has the fix)
     {
        DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                     "Dmov_SetChanOperationMode: called by %s without opening the handle",
                     pCtxt->pszClientName);
     }
     else
     {
        DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                     "Dmov_SetChanOperationMode: called by %d without opening the handle",
                     pCtxt->nHandleID);
     }
  }

  /* Check whether device is valid */
  if (FALSE == HAL_dmov_DeviceExists(pDevCtxt->dwDeviceInstance))
  {
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                    "DMOV Get Transfer Result: Client's DMOV device doesn't exist");
    return DAL_ERROR;
  }

  /* Check channel parameter */
  if (dwChan >= pDevCtxt->dwNumChans)
  {
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                    "DMOV set chan mode: invalid channel number: %d", dwChan);
    return DAL_ERROR;
  }

  /* stupid: to satisfy KW */
  if ( pDevCtxt->dwDeviceInstance >= HAL_DMOV_MAX_NUM_DEVICES )
  {
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                    "DMOV set chan mode: Nonexistant DMOV device");
    return DAL_ERROR;
  }

  if (!HAL_dmov_ChanIsAccessible(pDevCtxt->dwDeviceInstance, dwChan))
  {
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                    "DMOV set chan mode: channel %d not controlled by this processor", dwChan);
    return DAL_ERROR;
  }

  if (FALSE == pDevCtxt->bInterruptsAvailable &&
	  dwChanOpMode == DMOV_CHAN_MODE_INTERRUPT)
  {
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                    "DMOV set chan mode: channel %d can not be configured to interrupt mode", dwChan);
    return DAL_ERROR;
  }

  if (pCtxt->nHandleID == DMOV_CLIENT_NOT_REGISTERED)
  {
       if (Dmov_RegisterClient(pCtxt) != DAL_SUCCESS)
       {
         return DAL_ERROR;
       }
  }

  (void)DALSYS_SyncEnter(pDevCtxt->hDeviceSynchronization);

  /* Store the desired channel mode for the client,
     but not reconfigure the channel mode until Dmovi_IssueTransfer() */
  pCtxt->dwChannelsModes = (dwChanOpMode << dwChan) | (pCtxt->dwChannelsModes & (~((uint32)0x1 << dwChan)));

  (void)DALSYS_SyncLeave(pDevCtxt->hDeviceSynchronization);

  return DAL_SUCCESS;
}

/* ============================================================================
**
**  Dmov_FlushChannel
**
**  Description:
**    Flush a Data Mover channel.  Transfers that are pending in DMOV
**    driver software queues will be dequeued and user callbacks will
**    be invoked with result F flag set and flush_state[] values set
**    to DMOV_FLUSH_STATE_INVALID.
**
**  Parameters:
**    pCtxt - Pointer to the DMOV Client context
**    dwChan -
**    bFlushQueues -
**    bDiscardCurrentCmd -
**
**  Return:
**    TRUE  - Success, channel was flushed
**    FALSE - Failure
**
**  Dependencies:
**    User clean up may be required...
**
** ========================================================================= */
#ifdef DMOV_TZOS
/* For TZOS remove flush functionality */
DALResult  Dmov_FlushChannel
(
  DmovClientCtxt * pCtxt,
  uint32           dwChan,
  uint32           bFlushQueues,
  uint32           bDiscardCurrentCmd)
{
   (void)pCtxt;
   (void)dwChan;
   (void)bFlushQueues;
   (void)bDiscardCurrentCmd;
   return  DAL_ERROR;
}
#else
DALResult  Dmov_FlushChannel
(
  DmovClientCtxt * pCtxt,
  uint32           dwChan,
  uint32           bFlushQueues,
  uint32           bDiscardCurrentCmd)
{
  DmovDevCtxt * pDevCtxt = pCtxt->pDmovDevCtxt;
  Dmov_ChanType * pChan;

  /*
     Check if the client has an open handle if not for now issue a warning for
     now.  Once stabilized this check if true will exit the function
   */
  if(NULL == pCtxt->dwOpenCount)
  {
     if(NULL != pCtxt->pszClientName) //Check needed for now (DAL 22 has the fix)
     {
        DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                     "Dmov_FlushChannel: called by %s without opening the handle",
                     pCtxt->pszClientName);
     }
     else
     {
        DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                     "Dmov_FlushChannel: called by %d without opening the handle",
                     pCtxt->nHandleID);

     }
  }


  /* Check whether device is valid */
  if (FALSE == HAL_dmov_DeviceExists(pDevCtxt->dwDeviceInstance))
  {
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                    "DMOV Get Transfer Result: Client's DMOV device doesn't exist");
    return DAL_ERROR;
  }

  if (dwChan >= pDevCtxt->dwNumChans)
  {
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                    "DMOV Flush Channel: invalid channel number: %d", dwChan);
    return DAL_ERROR;
  }

  pChan = pDevCtxt->Chans[dwChan];

  /* check if this channel can be accessed */
  if ( NULL == pChan )
  {
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                  "DMOV Flush Channel : Cannot flush an inaccessible channel %d",dwChan);
	return DAL_ERROR;

  }
  (void)DALSYS_SyncEnter(pDevCtxt->hDeviceSynchronization);
  /* Verify channel hardware initialization is complete and  channel is not
   * already flushing, stopping, stopped, or halted (hardware error).
   */
  if (pChan->eState != DMOV_CHAN_STATE_RUN)
  {
    (void)DALSYS_SyncLeave(pDevCtxt->hDeviceSynchronization);
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                    "DMOV Flush Channel: invalid channel %d state: %d", dwChan, pChan->eState);
    return DAL_ERROR;
  }

  /* If there are pending transfers, issue flush command */
  if (DALFW_Q_Cnt(&pChan->PENDING_Q) > 0)
  {
    /* Change channel state to flushing */
    if (bFlushQueues != 0)
    {
      pChan->eState = DMOV_CHAN_STATE_FLUSHING_ALL;
    }
    else
    {
      pChan->eState = DMOV_CHAN_STATE_FLUSHING;
    }

	/* save the last flush type */
	pChan->bDiscardFlush = bDiscardCurrentCmd != 0;

    /* to satisfy KW */
    if ( pDevCtxt->dwDeviceInstance >= HAL_DMOV_MAX_NUM_DEVICES )
    {
        (void)DALSYS_SyncLeave(pDevCtxt->hDeviceSynchronization);
        DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                "DMOV Flush Domain: Nonexistant DMOV device");
        return DAL_ERROR;
    }
    /* Issue the flush command to the device */
	HAL_dmov_FlushChan(pDevCtxt->dwDeviceInstance, dwChan, (boolean)pChan->bDiscardFlush);
  }
  else if (bFlushQueues)
  {
    /* Flush all transfers that have not been written to the DM device */
    Dmovi_FlushQueues(pDevCtxt, pChan);
  }
  (void)DALSYS_SyncLeave(pDevCtxt->hDeviceSynchronization);

  return DAL_SUCCESS;
}
#endif //ifdef DMOV_TZOS
/* ============================================================================
**
**  Dmov_FlushDomain
**
**  Description:
**    This function flushes all channels associated with the specified security
**    domain.  The domain must *not* be controlled by the local processor.
**    This function is intended to clear all channels of a host that is
**    being reset.
**
**  Parameters:
**    pCtxt - Pointer to the DMOV Client context
**    dwDomain -
**
**  Return:
**    TRUE  - Success, domain was flushed
**    FALSE - Failure
**
**  Dependencies:
**    The current transfer in progress on each channel assigned to the specified
**    security domain will be aborted.  This could cause errors or indeterminate
**    state on the controlling host.
**
** ========================================================================= */
#ifdef DMOV_TZOS
/* For TZOS remove flush functionality */
DALResult Dmov_FlushDomain
(
  DmovClientCtxt * pCtxt,
  uint32           dwDomain)
{
   (void)pCtxt;
   (void)dwDomain;
   return DAL_ERROR;
}
#else
DALResult Dmov_FlushDomain
(
  DmovClientCtxt * pCtxt,
  uint32           dwDomain)
{
  DmovDevCtxt * pDevCtxt = pCtxt->pDmovDevCtxt;
  Dmov_ChanType ** ppChan;
  uint32  dwChan;

  HAL_dmov_SecurityLevel eSecLevel = HAL_dmov_GetSecurityLevel(pDevCtxt->dwDeviceInstance);


  /*
     Check if the client has an open handle if not for now issue a warning for
     now.  Once stabilized this check if true will exit the function
   */
  if(NULL == pCtxt->dwOpenCount)
  {
     if(NULL != pCtxt->pszClientName) //Check needed for now (DAL 22 has the fix)
     {
        DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                     "Dmov_FlushDomain: called by %s without opening the handle",
                     pCtxt->pszClientName);
     }
     else
     {
        DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                     "Dmov_FlushDomain: called by %d without opening the handle",
                     pCtxt->nHandleID);

     }
  }


  /* return if we are not running on the secure processor */
  if ( HAL_DMOV_NON_SECURE_BOOT == eSecLevel ||
       HAL_DMOV_NON_SECURE_AMSS == eSecLevel )
  {
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                    "DMOV Flush Domain: DMOV device not running on secure proc");
    return DAL_ERROR;
  }

  if (FALSE == HAL_dmov_DeviceExists(pDevCtxt->dwDeviceInstance))
  {
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                    "DMOV Flush Domain: DMOV device doesn't exist");
    return DAL_ERROR;
  }

  /* Check security domain argument */
  if (dwDomain >= pDevCtxt->dwNumSecDomains)
  {
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                    "DMOV Flush Domain: Security Domain %d doesn't exist", dwDomain);
    return DAL_ERROR;
  }

  /* to satisfy KW */
  if ( pDevCtxt->dwDeviceInstance >= HAL_DMOV_MAX_NUM_DEVICES )
  {
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                    "DMOV Flush Domain: Nonexistant DMOV device");
    return DAL_ERROR;
  }

  if (TRUE == HAL_dmov_SecDomainHasAccess(pDevCtxt->dwDeviceInstance, dwDomain))
  {
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                    "DMOV Flush Domain Error: Domain %d is local", dwDomain);
    return DAL_ERROR;
  }

  (void)DALSYS_SyncEnter(pDevCtxt->hDeviceSynchronization);

  /* Flush all channels assigned to this security domain */
  for (dwChan = 0, ppChan = pDevCtxt->Chans;
       dwChan < pDevCtxt->dwNumChans;
       dwChan++, ppChan++)

  {
    /* Is this channel a valid channel or
     * Is this channel assigned to the security domain? */
    if ( NULL == *ppChan  || (*ppChan)->dwDomain != dwDomain)
    {
      continue;   /* No, check next channel */
    }

    /* Perform "Flush With Discard" */
    if (!Dmovi_FlushRemoteChan(pDevCtxt, dwChan))
    {
      DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                      "DMOV Flush Domain Error: failed to flush channel %d", dwChan);
      break;
    }
  }

  /* Clear any pending interrupts */
  pDevCtxt->dwDmovDomainFlushIRQStatus =
      HAL_dmov_GetDomainPendingInterrupts(pDevCtxt->dwDeviceInstance, dwDomain);

  (void)DALSYS_SyncLeave(pDevCtxt->hDeviceSynchronization);

  return dwChan < pDevCtxt->dwNumChans ? DAL_ERROR : DAL_SUCCESS;
}
#endif //ifdef DMOV_TZOS
/* ============================================================================
**
**  Dmov_PrepareSGDescList
**
**  Description:
**    Modifies the user's S/G descriptor list by adding DMOV specific formatting
**
**  Parameters:
**    pCtxt - Pointer to the DMOV Client context
**    dwNumDescriptors - Number of descriptors in S/G descriptor list
**    pDescList - Pointer to the S/G descriptor list
**
**  Return:
**    TRUE  - Success, domain was flushed
**    FALSE - Failure
**
**  Dependencies:
**
** ========================================================================= */

DALResult Dmov_PrepareSGDescList
(
  DmovClientCtxt * pCtxt,
  uint32           dwNumDescriptors,
  uint32         * pDescList
)
{
  HAL_dmov_PrepareSGDescList((uint32)pDescList, dwNumDescriptors);
  return DAL_SUCCESS;
}

/* ============================================================================
**
**  Dmov_PrepareCommand
**
**  Description:
**    Modifies the user's command by adding DMOV specific formatting
**
**  Parameters:
**    pCtxt - Pointer to the DMOV Client context
**    pCommandOptions - Pointer to the command options
**    pCommand - Pointer to the command
**
**  Return:
**    TRUE  - Success, domain was flushed
**    FALSE - Failure
**
**  Dependencies:
**
** ========================================================================= */

DALResult Dmov_PrepareCommand
(
  DmovClientCtxt     * pCtxt,
  DmovCommandOptions * pCommandOptions,
  uint32             * pCommand
)
{
  *pCommand = 0;

  /* Address mode */
  HAL_dmov_SetCmdWordOptions(pCommand,
                             HAL_DMOV_CMD_OPTION_ADDR_MODE,
                             pCommandOptions->dwAddrMode);

  /* Source CRCI */
  if (pCommandOptions->dwSrcCRCI != 0)
  {
    HAL_dmov_SetCmdWordOptions(pCommand,
                               HAL_DMOV_CMD_OPTION_SRC_CRCI_CTL,
                               pCommandOptions->dwSrcCRCI);
  }

  /* Destination CRCI */
  if (pCommandOptions->dwDstCRCI != 0)
  {
    HAL_dmov_SetCmdWordOptions(pCommand,
                               HAL_DMOV_CMD_OPTION_DST_CRCI_CTL,
                               pCommandOptions->dwDstCRCI);
  }

  /* Source endianness */
  if (pCommandOptions->dwSrcEndianness != 0)
  {
    HAL_dmov_SetCmdWordOptions(pCommand,
                               HAL_DMOV_CMD_OPTION_SRC_END_CTL,
                               pCommandOptions->dwSrcEndianness);
  }

  /* Destination endianness */
  if (pCommandOptions->dwDstEndianness != 0)
  {
    HAL_dmov_SetCmdWordOptions(pCommand,
                               HAL_DMOV_CMD_OPTION_DST_END_CTL,
                               pCommandOptions->dwDstEndianness);
  }

  /* Block this channel */
  if (TRUE == pCommandOptions->bBlockThisChannel)
  {
    HAL_dmov_SetCmdWordOptions(pCommand,
                               HAL_DMOV_CMD_OPTION_BLOCK_THIS_CHAN,
                               1);
  }

  /* Block other channel */
  if (TRUE == pCommandOptions->bBlockOtherChannel)
  {
    HAL_dmov_SetCmdWordOptions(pCommand,
                               HAL_DMOV_CMD_OPTION_BLOCK_OTHER_CHAN,
                               1);
  }

  /* Unblock other channel */
  if (TRUE == pCommandOptions->bUnblockOtherChannel)
  {
    HAL_dmov_SetCmdWordOptions(pCommand,
                               HAL_DMOV_CMD_OPTION_UNBLOCK_OTHER_CHAN,
                               1);
  }

  /* Last command in the list */
  if (TRUE == pCommandOptions->bLastCommand)
  {
    HAL_dmov_SetCmdWordOptions(pCommand,
                               HAL_DMOV_CMD_OPTION_LC,
                               1);
  }

  if (DMOV_CMD_ADDR_MODE_SCATTER_GATHER == pCommandOptions->dwAddrMode)
  {
    HAL_dmov_PrepareScatterGatherModeCmd(pCommand);
  }

  return DAL_SUCCESS;
}

/* ============================================================================
**
**  Dmov_PrepareCommandListPtr
**
**  Description:
**    Modifies the user's CommandListPtr by adding DMOV specific formatting
**
**  Parameters:
**    pCtxt - Pointer to the DMOV Client context
**    pCommandList - Pointer to the command list
**    bLastPtr - TRUE if this pointer is the last in the list, FALSE otherwise
**    pCommandListPtr - Pointer to the command list pointer buffer
**
**  Return:
**    TRUE  - Success, domain was flushed
**    FALSE - Failure
**
**  Dependencies:
**
** ========================================================================= */

DALResult Dmov_PrepareCommandListPtr
(
  DmovClientCtxt * pCtxt,
  uint32         * pCommandList,
  uint32           bLastPtr,
  uint32         * pCommandListPtr
)
{
  /* Convert command list to command pointer list. */
  HAL_dmov_BuildCmdListPtr(pCommandListPtr, pCommandList, DAL_DMOV_CMD_LIST, bLastPtr == (uint32)TRUE);

  return DAL_SUCCESS;
}

