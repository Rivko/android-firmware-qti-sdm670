/*
===============================================================================

FILE:         DALDmovi.c

DESCRIPTION:  
  This file implements the data mover device driver internal functionality.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

===============================================================================

                                Edit History


when       who     what, where, why
--------   ---     ------------------------------------------------------------ 
08/12/10   py      Add support for using the same channel in different modes in parallel 
04/12/10   an      legacy structures cleanup
03/05/10   an      reuse of cmd ptr list pool for legacy and async xfers
02/19/10   an      split shared data structures among devices to avoid race condition
11/02/09   an      removed validation check for TransferRequest->dwDevice,
                   added error handling for unknown transfer types
10/06/09   an      renamed Dmovi_NotifyClient to Dmovi_MoveToNotifyQueue,
                   all functions here are supoposed to be called within 
				   synchronization section
09/24/09   an      put transfer into DONE queue in case of flush or error
05/28/09   an      use DALFW_MemoryBarrier instead of memory_barrier
04/29/09   an      fix compilation warnings 
04/12/09   MK      Added clock management 
04/09/09   an      Added context info into logs
02/09/09   an      typo fix in Dmovi_FlushRemoteChan
01/12/09   ah      Added global debug info
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
#include "DALDmov.h"
#include "DALDmovi.h"
#include "DALDmoviClkControl.h"
#include "DALDeviceId.h"

/* Debug Info */
typedef struct Dmov_ErrorInfoType Dmov_ErrorInfoType;
struct Dmov_ErrorInfoType
{
  uint32  dwDmovDevice;
  uint32  dwDmovChannel;
  Dmov_ChanError ChanError;
};

Dmov_ErrorInfoType Dmov_ErrorInfo;


/*------------------------------------------------------------------------------
Following functions are for DALDriver internal functionality
------------------------------------------------------------------------------*/

/* ============================================================================
**
**  Dmovi_DevInit
**
**  Description:
**       Place the Application Data Mover (aDM) out of reset.
**  Parameters:
**    pDevCtxt - Pointer to the DMOV Device context
**
**  Return:
**    None
**
**  Dependencies:
**    The device clock must be initialized prior to this call.
**
** ========================================================================= */

void Dmovi_DevInit
(
  DmovDevCtxt * pDevCtxt
)
{
   Dmovi_ControlDevInit(pDevCtxt);   
}

/* ============================================================================
**
**  Dmovi_DevReset
**
**  Description:
**       Place the Application Data Mover (aDM) into reset.
**  Parameters:
**    pDevCtxt - Pointer to the DMOV Device context
**
**  Return:
**    None
**
**  Dependencies:
**    None
**
** ========================================================================= */

void Dmovi_DevReset
(
  DmovDevCtxt * pDevCtxt
)
{
   Dmovi_ControlDevReset(pDevCtxt);
}

/* ============================================================================
**
**  Dmovi_DevEnable
**
**  Description:
**     Enables the clocks for the device
**  Parameters:
**    pDevCtxt - Pointer to the DMOV Device context
**
**  Return:
**    None
**
**  Dependencies:
**    Expected to be called in the pDevCtxt critical section
**
** ========================================================================= */

void Dmovi_DevEnable
(
  DmovDevCtxt * pDevCtxt
)
{
   HAL_dmov_SecurityLevel eSecurityLevel;

   eSecurityLevel = HAL_dmov_GetSecurityLevel(pDevCtxt->dwDeviceInstance);

   /* If it is the First client to open this device - turn on the clock */
   if(++pDevCtxt->dwOpenCount == 1)
   {
     if (HAL_DMOV_SECURE_BOOT == eSecurityLevel ||
         HAL_DMOV_SECURE_AMSS == eSecurityLevel  ||  //till we freeze Clock management support
         TRUE == pDevCtxt->bClocksManaged)
     {
       Dmovi_ControlDevEnable(pDevCtxt);
       Dmovi_ControlSetPerfLvl(pDevCtxt);
     }
   }

}

/* ============================================================================
**
**  Dmovi_DevDisable
**
**  Description:
**     Disables the clocks for the device
**  Parameters:
**    pDevCtxt - Pointer to the DMOV Device context
**
**  Return:
**    None
**
**  Dependencies:
**    Expected to be called in the pDevCtxt critical section
**
** ========================================================================= */

void Dmovi_DevDisable
(
  DmovDevCtxt * pDevCtxt
)
{

   /* if no more clients own this device turn off the clocks */
   if(--pDevCtxt->dwOpenCount == 0)
   {
     if (TRUE == pDevCtxt->bClocksManaged)
     {
       Dmovi_ControlSetPerfLvl(pDevCtxt);
       Dmovi_ControlDevDisable(pDevCtxt);

     }
   }
}

/* ============================================================================
**
**  Dmovi_GetClockIDs
**
**  Description:
**     Attached to the DAL Clock driver and get the clock IDs for all the required
**     clocks. Required clocks :
**                   - DM core clock
**                   - PBUS clock
**     Note: MDM is not yet supported.
**
**  Parameters:
**    pDevCtxt - Pointer to the DMOV Device context
**
**  Return:
**    None
**
**  Dependencies:
**    Expected to be called in the pDevCtxt critical section
**
** ========================================================================= */

DALResult Dmovi_GetClockIDs(DmovDevCtxt* pDevCtxt)
{
#ifdef DMOV_USE_DAL_CLOCK_DRIVER
  if ( TRUE == pDevCtxt->bClocksManaged )
  {
      if ( DAL_SUCCESS == DAL_DeviceAttach(DALDEVICEID_CLOCK,&pDevCtxt->hDALClkRegim))
      {
          DalDeviceHandle*  hDALClock = pDevCtxt->hDALClkRegim;

          switch(pDevCtxt->DevId)
          {
              case DALDEVICEID_DMOV_DEVICE_0:
                  if ( DAL_SUCCESS != DalClock_GetClockId(hDALClock,ADM0_CLK_STRING,&pDevCtxt->hClockID[DMOV_CLK_ADM0]))
                  {
                      DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                              "Dmov_DeviceInit : Cannot Get Clock ID for ADM0_CLK");
                      return DAL_ERROR;
                  }
                  if ( DAL_SUCCESS != DalClock_GetClockId(hDALClock,ADM0_PBUS_CLK_STRING,&pDevCtxt->hClockID[DMOV_CLK_ADM0_PBUS]))
                  {
                      DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                              "Dmov_DeviceInit : Cannot Get Clock ID for ADM0_PBUS_CLK");
                      return DAL_ERROR;
                  }
                  break;

              case DALDEVICEID_DMOV_DEVICE_2:
                  if ( DAL_SUCCESS != DalClock_GetClockId(hDALClock,ADM1_CLK_STRING,&pDevCtxt->hClockID[DMOV_CLK_ADM1]))
                  {
                      DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                              "Dmov_DeviceInit : Cannot Get Clock ID for ADM1_CLK");
                      return DAL_ERROR;
                  }

                  if ( DAL_SUCCESS != DalClock_GetClockId(hDALClock,ADM1_PBUS_CLK_STRING,&pDevCtxt->hClockID[DMOV_CLK_ADM1_PBUS]))
                  {
                      DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                              "Dmov_DeviceInit : Cannot Get Clock ID for ADM1_PBUS_CLK");
                      return DAL_ERROR;
                  }
                  break;

              default:
                      DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                                    "Dmov_DeviceInit : Invalid Clock ID given");
                      return DAL_ERROR;           
                      break;
          }
      }
      else
      {
          DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                       "Dmov_DeviceInit : Cannot Attach to DAL Clock driver");
          return DAL_ERROR;
      }
  }
#endif
  return DAL_SUCCESS;
}

/* ============================================================================
**
**  Dmovi_DevSave
**
**  Description:
**
**  Parameters:
**    pDevCtxt - Pointer to the DMOV Device context
**
**  Return:
**    None
**
**  Dependencies:
**    None
**
** ========================================================================= */

void Dmovi_DevSave
(
  DmovDevCtxt * pDevCtxt
)
{
  /* Restore secure configuration registers */
  HAL_dmov_Save(pDevCtxt->dwDeviceInstance);
}

/* ============================================================================
**
**  Dmovi_DevRestore
**
**  Description:
**
**  Parameters:
**    pDevCtxt - Pointer to the DMOV Device context
**
**  Return:
**    None
**
**  Dependencies:
**    None
**
** ========================================================================= */

void Dmovi_DevRestore
(
  DmovDevCtxt * pDevCtxt
)
{
  /* Restore secure configuration registers */
  HAL_dmov_Restore(pDevCtxt->dwDeviceInstance);
}

/* ============================================================================
**
**  Dmovi_CompareToken
**
**  Description:
**    Checks whether the value to compare with matches the value stored in the
**    queue item.
**
**  Parameters:
**    p_Item - Pointer to the queue item
**    p_CompareValue - Value to compare to
**
**  Return:
**    -1  - in case of error
**     1  - if there's a match
**     0  - no match
**
**  Dependencies:
**    None
**
** ========================================================================= */

int Dmovi_CompareToken
(
  void * p_Item, 
  void * p_CompareValue
)
{
  DmovTransferQItem * pTransferDescriptor;
  uint32  dwToken;
  
  if((p_Item == NULL) || (p_CompareValue == NULL))
  {
    return -1;
  }
  
  pTransferDescriptor = (DmovTransferQItem *)p_Item;
  dwToken = *((uint32 *)p_CompareValue);

  return (pTransferDescriptor->dwToken == dwToken? 1: 0);
}

/* ============================================================================
**
**  Dmovi_ClearTransferDescriptor
**
**  Description:
**    Resets the result of a Transfer Descriptor queue item.
**
**  Parameters:
**    pTransferDescriptor - Pointer to the queue item
**
**  Return:
**    None
**
**  Dependencies:
**    None
**
** ========================================================================= */

void Dmovi_ClearTransferDescriptor
(
  DmovTransferQItem * pTransferDescriptor
)
{
  memset(&pTransferDescriptor->Results, 0, sizeof(DmovResult));
}

/* ============================================================================
**
**  Dmovi_IssueTransfer
**
**  Description:
**    If the channel is ready, waiting commands will be issued on a priority 
**    basis.
**
**  Parameters:
**    pDevCtxt - Pointer to the DMOV Device context
**    pChan - Pointer to the DMOV channel
**
**  Return:
**    TRUE  - a command was issued
**    FALSE - no command was issued
**
**  Dependencies:
**    Should be called within synchronization.
**
** ========================================================================= */

DALBOOL Dmovi_IssueTransfer
(
  DmovDevCtxt   * pDevCtxt, 
  Dmov_ChanType * pChan
)
{
  DmovTransferQItem * pTransferDescriptor;
  uint32 dwCmd = 0;
  uint32 dwPriority;
  uint32 dwCurrentChanMode;
  uint32 dwNextChanMode;
  DmovTransferQItem *pNextTransferDescriptor;

  /* Do not issue commands unless the channel is running or stopping */
  if (pChan->eState != DMOV_CHAN_STATE_RUN &&
      pChan->eState != DMOV_CHAN_STATE_STOPPING)
  {
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR, 
                    "DMOV Issue Cmd: Invalid channel %d state: %d", pChan->dwChan, pChan->eState);
    return FALSE;
  }

  /* Find the highest priority queued transfer */
  for (dwPriority = 0; dwPriority < pChan->dwNumQueues; dwPriority++)
  {
    if (0 != DALFW_Q_Cnt(&pChan->CMD_Q[dwPriority]))
    {
      break;
    }
  }
  if (dwPriority >= pChan->dwNumQueues)
  {
    return FALSE;  /* No queued transfer was found */
  }

  /* Determine if the channel can accept another command */
  if (!HAL_dmov_ChanCmdQIsReady(pChan->dwDevice, pChan->dwChan))
  {
    return FALSE;
  }

  /* Compare current channel mode with the channel mode for the next transfer */
  pNextTransferDescriptor = DALFW_Q_Check(&pChan->CMD_Q[dwPriority]);

  if (pNextTransferDescriptor == NULL)
  {
     return FALSE;
  }

  dwNextChanMode = DMOV_GET_TRANSFER_MODE(pNextTransferDescriptor->dwToken);
  dwCurrentChanMode = pDevCtxt->Chans[pChan->dwChan]->dwChanOpMode;

  if (dwNextChanMode != dwCurrentChanMode)
  {
      /* Check if there is any pending transfer before issusing the next transfer */
      if (DALFW_Q_Cnt(&pChan->PENDING_Q) > 0)
      {
          return FALSE;
      }
      else
      {
          pDevCtxt->Chans[pChan->dwChan]->dwChanOpMode = dwNextChanMode;
          HAL_dmov_SetChanMode(pDevCtxt->dwDeviceInstance, pChan->dwChan, (HAL_dmov_ChanOpMode)dwNextChanMode);
      }
  }

  /* Increment active transfer counter */
  pDevCtxt->dwActiveCount++;

  /* Get the highest priority queued transfer */
  pTransferDescriptor = DALFW_Q_Get(&pChan->CMD_Q[dwPriority]);

  /* to satisfy KW */
  if (pTransferDescriptor == NULL)
  {
     return FALSE;
  }

  switch (pTransferDescriptor->TransferType)
  {
    case DMOV_TRANSFER_LEGACY:
    case DMOV_TRANSFER_SYNCHRONOUS:
        /* Build the final command */
        if (DAL_DMOV_CMD_PTR_LIST == pTransferDescriptor->CommandType)
        {
          HAL_dmov_BuildCmd(&dwCmd, pTransferDescriptor->CommandPtr);
        }
        else
        {
          HAL_dmov_BuildCmd(&dwCmd, pTransferDescriptor->pCmdListPtrPhysBuffer);
        }
        break;
    case DMOV_TRANSFER_ASYNCHRONOUS:
        /* Build the final command */
        HAL_dmov_BuildCmd(&dwCmd, pTransferDescriptor->pCmdListPtrPhysBuffer);
        break;
  default:
        // ERROR: this can only happen in case of memory corruption
        DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR, 
                    "DMOV Issue Cmd: Invalid transfer type %d for channel: %d", 
                    pTransferDescriptor->TransferType, pChan->dwChan);

        pTransferDescriptor->Results.dwResultValue = 0;
        pTransferDescriptor->Results.eState = DMOV_TRANSFER_ERROR;
        pTransferDescriptor->Results.eChanError = DMOV_CHAN_NO_ERROR;

        Dmovi_MoveToNotifyQueue(pDevCtxt, pChan, pTransferDescriptor, TRUE);

        // ready for the next command
        return TRUE;
  }

  /* Move the transfer to the pending queue */
  DALFW_Q_Put(&pChan->PENDING_Q, &pTransferDescriptor->Link);

  /* Issue the command to the Data Mover channel */
  HAL_dmov_IssueCmd(pChan->dwDevice, pChan->dwChan, dwCmd);

  pChan->dwCmdsIssuedCount++;

  DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO, 
                  "DMOV Issue Cmd: Command issued");

#ifdef WINSIM
  /* For SIMULATION PURPOSES ONLY... Simulating a fake interrupt with a delay
   * of 50 ms
   */
  TriggerSimInterrupt(pDevCtxt->InterruptId,50);
#endif

  return TRUE;
}

/* ============================================================================
**
**  Dmovi_PrepareAsyncTransfer
**
**  Description:
**
**  Parameters:
**    TransferRequest - Pointer to the transfer request
**    CommandList - Pointer to the Command List
**    pTransferDescriptor - Pointer to the transfer descriptor queue item
**
**  Return:
**    None
**
**  Dependencies:
**    None
**
** ========================================================================= */

void Dmovi_PrepareAsyncTransfer
(
  const DmovTransferRequest * TransferRequest, 
  uint32                    * CommandList,
  DmovTransferQItem         * pTransferDescriptor
)
{
  uint32 *pCmdWord, *pUserCmdWord;
  uint32 dwCmdIdx = 0;
  DmovSingleCommand *pSingleCommand, *pUserSingleCommand;
  DmovSGCommand *pSGCommand, *pUserSGCommand;
  DmovBoxCommand *pBoxCommand, *pUserBoxCommand;

  /* Build the command list */
  pCmdWord = pTransferDescriptor->pCmdBuffer;
  pUserCmdWord = CommandList;

  for (dwCmdIdx = 0; dwCmdIdx < TransferRequest->dwNumCmds; dwCmdIdx++)
  {
    switch (*pUserCmdWord & DMOV_CMD_ADDR_MODE_BMSK)
    {
       case DMOV_CMD_ADDR_MODE_SINGLE:
          pUserSingleCommand = (DmovSingleCommand *)pUserCmdWord;
          pSingleCommand = (DmovSingleCommand *)pCmdWord;
          *pSingleCommand = *pUserSingleCommand;
          pCmdWord += DMOV_NUM_SINGLE_CMD_WORDS;
          pUserCmdWord += DMOV_NUM_SINGLE_CMD_WORDS;
          break;

       case DMOV_CMD_ADDR_MODE_SCATTER_GATHER:
          pUserSGCommand = (DmovSGCommand *)pUserCmdWord;
          pSGCommand = (DmovSGCommand *)pCmdWord;
          *pSGCommand = *pUserSGCommand;
          pCmdWord += DMOV_NUM_SG_CMD_WORDS;
          pUserCmdWord += DMOV_NUM_SG_CMD_WORDS;
          break;

       case DMOV_CMD_ADDR_MODE_BOX:
          pBoxCommand = (DmovBoxCommand *)pCmdWord;
          pUserBoxCommand = (DmovBoxCommand *)pUserCmdWord;
          *pBoxCommand = *pUserBoxCommand;
          pCmdWord += DMOV_NUM_BOX_CMD_WORDS;
          pUserCmdWord += DMOV_NUM_BOX_CMD_WORDS;
          break;

      default:
          break;
    }
  }

  HAL_dmov_BuildCmdListPtr(pTransferDescriptor->pCmdListPtrBuffer,
                           pTransferDescriptor->pCmdPhysBuffer,
                           (uint32)DAL_DMOV_CMD_LIST,
                           TRUE);
}

/* ============================================================================
**
**  Dmovi_QueueTransfer
**
**  Description:
**    Queue a Data Mover command on the specified channel.  The command
**    may be written directly to the device if device is ready, or it
**    wlll be queued for later priority-based processing.
**
**  Parameters:
**    pCtxt - Pointer to the Client's Context
**    TransferRequest - Pointer to the transfer request
**    CommandList - Pointer to the Command List
**    RequestStatus - Info about the status of the current request
**
**  Return:
**    DAL_SUCCESS - Success, command was queued (to device or list)
**    DAL_ERROR - Failure, command was not queued
**
**  Dependencies:
**    None
**
** ========================================================================= */

DALResult Dmovi_QueueTransfer
(
  DmovClientCtxt            * pCtxt,
  const DmovTransferRequest * TransferRequest, 
  uint32                    * CommandList,
  DmovReqStatus             * RequestStatus
)
{
  DmovTransferQItem * pTransferDescriptor;
  DmovDevCtxt * pDevCtxt = pCtxt->pDmovDevCtxt;
  Dmov_ChanType * pChan = pDevCtxt->Chans[TransferRequest->dwChan];

  /* Allocate the local transfer descriptor */
  pTransferDescriptor = DALFW_Q_Get(&pDevCtxt->DmovXferFreePool);

  if (NULL == pTransferDescriptor)
  {
    pDevCtxt->eError = DMOV_DEV_OUT_OF_XFER_DESCRIPTORS;
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR, 
                    "DMOV Queue Cmd: Out of free Transfer Descriptors");
    return DAL_ERROR;
  }

  switch (TransferRequest->TransferType)
  {
#ifdef DMOV_LEGACY_SUPPORT
    case DMOV_TRANSFER_LEGACY:
        pTransferDescriptor->CommandType = TransferRequest->CommandType;
        pTransferDescriptor->CommandPtr = (uint32*)TransferRequest->cmd_ptr;

        if (pTransferDescriptor->CommandType != DAL_DMOV_CMD_PTR_LIST)
        {
          /* Convert command list to command pointer list.
           * This should be done before cache maintenance memory barrier.
           */
          HAL_dmov_BuildCmdListPtr(pTransferDescriptor->pCmdListPtrBuffer, 
                                   pTransferDescriptor->CommandPtr, 
                                   (uint32)pTransferDescriptor->CommandType,
                                   TRUE);
        }

        pTransferDescriptor->callback_ptr = 
            (dmov_xfer_callback_f_type_legacy)TransferRequest->pCallbackFunction;
        break;

#endif

    case DMOV_TRANSFER_SYNCHRONOUS:
        pTransferDescriptor->CommandType = TransferRequest->CommandType;
        pTransferDescriptor->CommandPtr = CommandList;

        if (pTransferDescriptor->CommandType != DAL_DMOV_CMD_PTR_LIST)
        {
          /* Convert command list to command pointer list. */
          HAL_dmov_BuildCmdListPtr(pTransferDescriptor->pCmdListPtrBuffer, 
                                   CommandList, 
                                   TransferRequest->CommandType,
                                   TRUE);
        }

        break;

    case DMOV_TRANSFER_ASYNCHRONOUS:
        Dmovi_PrepareAsyncTransfer(TransferRequest, 
                                     CommandList, 
                                     pTransferDescriptor);
        break;
    default:
        DALFW_Q_Put(&pDevCtxt->DmovXferFreePool, &pTransferDescriptor->Link);
        DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR, 
                    "DMOV Queue Cmd: Unknown transfer type %d for channel %d", 
                    TransferRequest->TransferType, pChan->dwChan);
        return DAL_ERROR;
  }

  /* copy transfer type */
  pTransferDescriptor->TransferType = TransferRequest->TransferType;

  /* copy the event to be issued to the client when this transfer is done */
  pTransferDescriptor->hEvent = TransferRequest->hEvent;

  /* Commented out - Update the channel event if needed */
  /* (channel event should have a special handler) */
  //if (pChan->hEvent == NULL && pChan->hEvent != TransferRequest->hEvent)
  //{
  //  pChan->hEvent = TransferRequest->hEvent;
  //}

  /* Copy user data */
  pTransferDescriptor->Results.pUserData = TransferRequest->pUserData;

  /* Token sent to the client for this transfer */
  pChan->dwTransferID++;
  /* Reset transfer id counter if it overruns */
  pChan->dwTransferID &= DMOV_TRANSFER_ID_BMSK;
  RequestStatus->dwToken = (((pCtxt->dwChannelsModes >> TransferRequest->dwChan) & 0x1) << DMOV_TRANSFER_MODE_SHFT) | 
                           (TransferRequest->dwChan << DMOV_TRANSFER_CHAN_SHFT) | 
                           pChan->dwTransferID << DMOV_TRANSFER_ID_SHFT;

  /* Keep a copy of the token */
  pTransferDescriptor->dwToken = RequestStatus->dwToken;

  /* Perform memory barrier to guarantee that all user data has been
   * written to memory.  This is necessary even for uncached regions.
   */
  DALFW_MemoryBarrier(0);

  /* Place the transfer on the wait queue */
  DALFW_Q_Put(&pChan->CMD_Q[TransferRequest->dwPriority], &pTransferDescriptor->Link);

  pDevCtxt->dwQueuedCount++;
  pChan->dwCmdsQueuedCount++;

  /* Issue the command immediately if the channel is ready */
  while (FALSE != Dmovi_IssueTransfer(pDevCtxt, pChan))
  {
  }

  DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO, 
                  "DMOV Queue Cmd: Transfer successfully queued!");

  return DAL_SUCCESS;
}

/* ============================================================================
**
**  Dmovi_RequestIsValid
**
**  Description:
**    Checks whether a client's request is valid.
**
**  Parameters:
**    pCtxt - Pointer to the Client's Context
**    TransferRequest - Pointer to the transfer request
**    CommandList - Pointer to the Command List
**    RequestStatus - Info about the status of the current request
**
**  Return:
**    DAL_SUCCESS - Transfer Request is valid
**    DAL_ERROR - Transfer Request not valid
**
**  Dependencies:
**    None
**
** ========================================================================= */

DALResult Dmovi_RequestIsValid
( 
  DmovClientCtxt            * pCtxt,
  const DmovTransferRequest * TransferRequest,
  uint32                    * CommandList,
  DmovReqStatus             * RequestStatus
)
{
  DmovDevCtxt *pDevCtxt = pCtxt->pDmovDevCtxt;
  uint32 dwChan = TransferRequest->dwChan;
  Dmov_ChanType *pChan; 

  /* Check whether the channel is valid */
  if (dwChan >= pDevCtxt->dwNumChans)
  {
    RequestStatus->eState = DMOV_REQUEST_INVALID_CHAN_NUMBER;
    RequestStatus->dwToken = DMOV_INVALID_TOKEN;
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR, 
                    "DMOV Request not Valid: invalid channel number");
    return DAL_ERROR;
  }

  pChan = pDevCtxt->Chans[dwChan];

  /* Do we have access to this channel ? */
  if (NULL == pChan )
  {
    RequestStatus->dwToken = DMOV_INVALID_TOKEN;
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR, 
                    "DMOV Request not Valid: Device does not have access to channel");
    return DAL_ERROR;
  }

  /* Boot and Tools should allocate their own command list memory */
  if (HAL_dmov_GetSecurityLevel(pDevCtxt->dwDeviceInstance) <= HAL_DMOV_NON_SECURE_BOOT &&
       TransferRequest->TransferType != DMOV_TRANSFER_SYNCHRONOUS )
  {
    RequestStatus->eState = DMOV_REQUEST_BOOT_SHOULD_BE_SYNCHRONOUS;
    RequestStatus->dwToken = DMOV_INVALID_TOKEN;
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR, 
                    "DMOV Request not Valid: Boot and Tools should allocate their own command list memory");
    return DAL_ERROR;
  }

  /* Verify that the command buffer is aligned on a 64-bit boundary */
  if (TransferRequest->TransferType != DMOV_TRANSFER_ASYNCHRONOUS &&
      ((uint32)CommandList & 0x07) != 0)
  {
    RequestStatus->eState = DMOV_REQUEST_CMD_LIST_NOT_ALIGNED;
    RequestStatus->dwToken = DMOV_INVALID_TOKEN;
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR, 
                    "DMOV Request not Valid: Command list not on 64-bit boundary");
    return DAL_ERROR;
  }

  /* Check whether interrupts are available for this device if the transfer 
     channel is in interrupt mode */
  if ( FALSE == pDevCtxt->bInterruptsAvailable &&
       HAL_DMOV_CHAN_MODE_INTERRUPT == HAL_dmov_GetChanMode(pDevCtxt->dwDeviceInstance, dwChan) )
  {
    RequestStatus->eState = DMOV_REQUEST_INVALID_INTERRUPT_MODE;
    RequestStatus->dwToken = DMOV_INVALID_TOKEN;
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR, 
                    "DMOV Request not Valid: Interrupt mode mismatch device/channel");
    return DAL_ERROR;
  }

  /* Check whether this client exceeded the max of 4 queued transfers */
  if (DALFW_Q_Cnt(&pChan->CMD_Q[0]) >= DMOV_AVG_NUM_CLP_PER_CHAN)
  {
    RequestStatus->eState = DMOV_MAX_REQUESTS_ISSUED;
    RequestStatus->dwToken = DMOV_INVALID_TOKEN;
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR, 
                    "DMOV Request not Valid: Client issued max transfers");
    return DAL_ERROR;
  }

  /* Check the number of cmds in this transfer */
  if ( TransferRequest->dwNumCmds <= 0 ||
       (TransferRequest->TransferType != DMOV_TRANSFER_SYNCHRONOUS &&
        TransferRequest->dwNumCmds > DMOV_MAX_NUM_CMDS_PER_LIST) 
     )
  {
    RequestStatus->eState = DMOV_REQUEST_INVALID_NUM_CMDS;
    RequestStatus->dwToken = DMOV_INVALID_TOKEN;
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR, 
                    "DMOV Request not Valid: Invalid number of commands");
    return DAL_ERROR;
  }

  /* Check whether device has been initialized */
  if (pDevCtxt->eState != DMOV_DEV_RUN)
  {
    RequestStatus->eState = DMOV_REQUEST_INVALID_DEVICE_STATE;
    RequestStatus->dwToken = DMOV_INVALID_TOKEN;
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR, 
                    "DMOV Request not Valid: invalid device state");
    return DAL_ERROR;
  }


  /* Verify that channel is running */
  if (pChan->eState != DMOV_CHAN_STATE_RUN)
  {
    RequestStatus->eState = DMOV_REQUEST_INVALID_CHAN_STATE;
    RequestStatus->dwToken = DMOV_INVALID_TOKEN;
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR, 
                    "DMOV Request not Valid: invalid channel state");
    return DAL_ERROR;
  }

  return DAL_SUCCESS;
}

/* ============================================================================
**
**  Dmovi_FlushQueues
**
**  Description:
**    Drain all queses of a Data Mover channel. For each queued transfer, the
**    user callback will be invoked with result FLUSH flag set and flush_state[]
**    values set to DMOV_FLUSH_STATE_INVALID.
**
**  Parameters:
**    pDevCtxt - Pointer to the Device Context
**    pChan - Pointer to the dmov channel
**
**  Return:
**    None
**
**  Dependencies:
**    Called within synchronization.
**
** ========================================================================= */

void Dmovi_FlushQueues
(
  DmovDevCtxt   * pDevCtxt,
  Dmov_ChanType * pChan
)
{
  DmovTransferQItem *pTransferDescriptor;
  uint32 dwPriority, dwNumFlushRegisters;
  uint32* pFlushBuffer;

  for (dwPriority = 0; dwPriority < pChan->dwNumQueues; dwPriority++)
  {
    while ((pTransferDescriptor = DALFW_Q_Get(&pChan->CMD_Q[dwPriority])) != NULL)
    {
      pTransferDescriptor->Results.eState = DMOV_TRANSFER_FLUSH;
      pTransferDescriptor->Results.dwResultValue = 0;
      pFlushBuffer = pTransferDescriptor->Results.FlushState;

      for ( dwNumFlushRegisters = 0; 
            dwNumFlushRegisters < pDevCtxt->dwNumFlushRegisters; 
            dwNumFlushRegisters++ 
          )
      {
          pFlushBuffer[dwNumFlushRegisters] = HAL_DMOV_FLUSH_STATE_INVALID;
      }

      Dmovi_MoveToNotifyQueue(pDevCtxt, pChan, pTransferDescriptor, TRUE);
    }
  }
}


/* ============================================================================
**
**  Dmovi_GetChanError
**
**  Description:
**
**  Parameters:
**    pDevCtxt - Pointer to the Device Context
**    pChan - Pointer to the dmov channel
**
**  Return:
**    None
**
**  Dependencies:
**    None
**
** ========================================================================= */

void Dmovi_GetChanError
(
  DmovDevCtxt   * pDevCtxt,
  Dmov_ChanType * pChan
)
{
  HAL_dmov_ErrorType eChanError = 
      HAL_dmov_GetChanError(pDevCtxt->dwDeviceInstance, pChan->dwChan);

  switch (eChanError)
  {
    case HAL_DMOV_CPH_BUS_ERROR:
        DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                        "DMOV Get Chan %d Error: Command-phase bus error occured", pChan->dwChan);
        pChan->eError = DMOV_CHAN_CPH_BUS_ERROR;
        Dmov_ErrorInfo.ChanError = DMOV_CHAN_CPH_BUS_ERROR;
        break;
    case HAL_DMOV_DPH_BUS_ERROR:
        DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                        "DMOV Get Chan %d Error: Data-phase bus error occured", pChan->dwChan);
        pChan->eError = DMOV_CHAN_DPH_BUS_ERROR;
        Dmov_ErrorInfo.ChanError = DMOV_CHAN_DPH_BUS_ERROR;
        break;
    case HAL_DMOV_MPU_ERROR:
        DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                        "DMOV Get Chan %d Error: MPU error occured", pChan->dwChan);
        pChan->eError = DMOV_CHAN_MPU_ERROR;
        Dmov_ErrorInfo.ChanError = DMOV_CHAN_MPU_ERROR;
        break;
    case HAL_DMOV_LEN_ERROR:
        DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                        "DMOV Get Chan %d Error: Length error occured", pChan->dwChan);
        pChan->eError = DMOV_CHAN_LEN_ERROR;
        Dmov_ErrorInfo.ChanError = DMOV_CHAN_LEN_ERROR;
        break;
    case HAL_DMOV_CMD_ERROR:
        DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                        "DMOV Get Chan %d Error: Command error occured", pChan->dwChan);
        pChan->eError = DMOV_CHAN_CMD_ERROR;
        Dmov_ErrorInfo.ChanError = DMOV_CHAN_CMD_ERROR;
        break;
    default:
        DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                        "DMOV Get Chan %d Error: Unknown error occured", pChan->dwChan);
  }
}

/* ============================================================================
**
**  Dmovi_MoveToNotifyQueue
**
**  Description:
**    Puts transfer request into notification queue.
**    In case of bFreeTransfer is FALSE it will allocate a copy of descriptor
**    from the free queue
**
**  Parameters:
**    pDevCtxt - Pointer to the Device Context
**    pChan - Pointer to the dmov channel
**    pTransferDescriptor - Pointer to the transfer descriptor
**    bFreeTransfer - flags to move the descriptor into notification queue
**
**  Return:
**    None
**
**  Dependencies:
**    must be called within pDevCtxt->hDeviceSynchronization section
**
** ========================================================================= */

void Dmovi_MoveToNotifyQueue
(
  DmovDevCtxt*       pDevCtxt,
  Dmov_ChanType*     pChan,
  DmovTransferQItem* pTransferDescriptor,
  DALBOOL            bFreeTransfer
)
{
  DmovTransferQItem* pDescriptor = pTransferDescriptor;

  if (bFreeTransfer == FALSE)
  {
	  // allocate a copy of transfer descriptor

	  pDescriptor = DALFW_Q_Get(&pDevCtxt->DmovXferFreePool);

      if (!pDescriptor)
      {
        DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR, 
                    "Dmovi_MoveToNotifyQueue: no transfer descriptor available");
		return;
      }

	  DALFW_memcpy(pDescriptor, pTransferDescriptor, sizeof(pDescriptor[0]));
	  pDevCtxt->dwQueuedCount++;
  }
  else
  {
	  pDevCtxt->dwActiveCount--;
  }

  if (pDescriptor->hEvent == NULL)
  {
#ifdef DMOV_LEGACY_SUPPORT
	  if (DMOV_TRANSFER_LEGACY == pDescriptor->TransferType)
	  {
		if (pDescriptor->callback_ptr == NULL)
		{
		  DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO,
						"Dmovi_MoveToNotifyQueue: discard pending transfer without notification");

		  /* Clear the Transfer Descriptor */
		  Dmovi_ClearTransferDescriptor(pDescriptor);
		  /* Return transfer descriptor to the free pool */
		  DALFW_Q_Put(&pDevCtxt->DmovXferFreePool, &pDescriptor->Link);
	  	  pDevCtxt->dwQueuedCount--;
		}
		else
		{
			DALFW_Q_Put(&pDevCtxt->DmovDevXferNotify, &pDescriptor->Link);
		}
	  }
	  else
#endif
	  {
		  DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO,
						"Dmovi_MoveToNotifyQueue: put pending transfer to DONE queue without notification");
		  DALFW_Q_Put(&pChan->DONE_Q, &pDescriptor->Link);
	  }
  }
  else
  {
	  DALFW_Q_Put(&pDevCtxt->DmovDevXferNotify, &pDescriptor->Link);
  }
}

/* ============================================================================
**
**  Dmovi_FlushHandler
**
**  Description:
**    Handle a Data Mover channel flush request.  Flush conditions are
**    checked and when satisfied, all software queues are drained.
**    For each queued transfer, the user callback will be invoked with
**    result ERR flag set and flush_state[] values set to DMOV_FLUSH_STATE_INVALID.
**
**  Parameters:
**    pDevCtxt - Pointer to the Device Context
**    pChan - Pointer to the dmov channel
**    dwResult - RSLT register value indicating flush
**
**  Return:
**    None
**
**  Dependencies:
**    None
**
** ========================================================================= */
#ifdef DMOV_TZOS
void Dmovi_FlushHandler
(
  DmovDevCtxt   * pDevCtxt,
  Dmov_ChanType * pChan,
  uint32          dwResult
)
{
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,"DMOV Flush Handler:" 
		    "Flushing is not supported in trust zone dmov" );
}
#else
void Dmovi_FlushHandler
(
  DmovDevCtxt   * pDevCtxt,
  Dmov_ChanType * pChan,
  uint32          dwResult
)
{
  uint32 dwDevice = pDevCtxt->dwDeviceInstance;
  uint32 dwChan = pChan->dwChan;
  DmovTransferQItem *pTransferDescriptor;
  uint32 n;
  DALBOOL bTopResultDone = FALSE;

  HAL_dmov_ResultInfo ResultInfo;

  /* Record the flush state */
  pChan->ErrorResult.dwResultValue = dwResult;

  for (n = 0; n < pDevCtxt->dwNumFlushRegisters; n++)
  {
    /* FLUSH_STATE[2} register is reserved and inaccessible. Reading from it may cause a crash */
    if ( n != 2 )
        pChan->ErrorResult.FlushState[n] = HAL_dmov_GetChanFlushState(dwDevice, dwChan, n);
  }

  pChan->dwResultFlushCount++;

  ResultInfo = HAL_dmov_CheckChanResult(dwResult);

  /* If flush was caused by an error or was not user-initiated */
  if ( ResultInfo == HAL_DMOV_ERROR_RESULT )
  {
    pChan->bErrorValid = TRUE;
    Dmov_ErrorInfo.dwDmovDevice = pDevCtxt->dwDeviceInstance;
    Dmov_ErrorInfo.dwDmovChannel = pChan->dwChan;

    /* Check the channel HW for error type */
    Dmovi_GetChanError(pDevCtxt, pChan);

    /* A hardware error caused the flush.  Channel is now stalled. */
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR, 
                    "DMOV Flush Handler: hardware channel %d halted", pChan->dwChan);
    pChan->eState = DMOV_CHAN_STATE_ERROR;
    pChan->dwResultErrorCount++;

    /* Notify the owner of this channel that an error occured */
    if (pChan->hEvent != NULL)
    {
      DALSYS_EventCtrl(pChan->hEvent,DALSYS_EVENT_CTRL_TRIGGER);
    }
  }

  /* Check for end of user-initiated flush state */
  if (pChan->eState == DMOV_CHAN_STATE_FLUSHING)
  {
    /* Return to normal operation state */
    pChan->eState = DMOV_CHAN_STATE_RUN;
  }
  else if (pChan->eState == DMOV_CHAN_STATE_FLUSHING_ALL)
  {
    if (DALFW_Q_Cnt(&pChan->PENDING_Q) == 1)
    {
      uint32 dwTotalQueued = 0;
      uint32 dwPriority;
      for (dwPriority = 0; dwPriority < pChan->dwNumQueues; dwPriority++)
      {
        dwTotalQueued += DALFW_Q_Cnt(&pChan->CMD_Q[dwPriority]);
      }
      if (dwTotalQueued == 0)
      {
        pChan->eState = DMOV_CHAN_STATE_RUN;
      }
    }
  }

  /* Determine which transfers on the pending queue were affected
   * by the flush.
   */
  while ((pTransferDescriptor = DALFW_Q_Get(&pChan->PENDING_Q)) != NULL)
  {
    uint32* pFlushBuffer;

    /* Set result value for user callback to pick up */
    pTransferDescriptor->Results.dwResultValue = dwResult;
    pFlushBuffer = pTransferDescriptor->Results.FlushState;

    if (ResultInfo == HAL_DMOV_ERROR_RESULT)
    {
	  pTransferDescriptor->Results.eState = DMOV_TRANSFER_ERROR;
	  pTransferDescriptor->Results.eChanError = Dmov_ErrorInfo.ChanError;
    }
    else
    {
	  pTransferDescriptor->Results.eState = DMOV_TRANSFER_FLUSH;
    }

    if (!bTopResultDone)
    {
	    /* copy flush state for the user notification */
        for (n = 0; n < pDevCtxt->dwNumFlushRegisters; n++)
        {
          pFlushBuffer[n] = pChan->ErrorResult.FlushState[n];
        }

	bTopResultDone = TRUE;
    }
    else
    {
        for (n = 0; n < pDevCtxt->dwNumFlushRegisters; n++)
        {
          pFlushBuffer[n] = HAL_DMOV_FLUSH_STATE_INVALID;
        }
    }
	  
    Dmovi_MoveToNotifyQueue(pDevCtxt, pChan, pTransferDescriptor, TRUE);

    /* In case of user flush only the first pending transfer is affected for DM rev2 HW */
    if (ResultInfo != HAL_DMOV_ERROR_RESULT)
      break;
  }

  /* Clear all software priority queues if the specified by the user */
  if (pChan->eState == DMOV_CHAN_STATE_FLUSHING_ALL)
  {
    if (DALFW_Q_Cnt(&pChan->PENDING_Q) == 0)
    {
      Dmovi_FlushQueues(pDevCtxt, pChan);
      pChan->eState = DMOV_CHAN_STATE_RUN;
    }
    else
    {
      /* Flush remaining transfer, graceful flush */
      HAL_dmov_FlushChan(dwDevice, dwChan, (boolean)pChan->bDiscardFlush);
    }
  }
  else if (pChan->eState == DMOV_CHAN_STATE_ERROR)
  {
	  Dmovi_FlushQueues(pDevCtxt, pChan);
  }
}
#endif  //DMOV_TZOS
/* ============================================================================
**
**  Dmovi_StopHandler
**
**  Description:
**    Handle a Data Mover channel stop request.  Stop conditions are
**    checked and the user is notified, if event is registered.
**
**  Parameters:
**    pDevCtxt - Pointer to the Device Context
**    pChan - Pointer to the dmov channel
**
**  Return:
**    None
**
**  Dependencies:
**    Should be called within synchronization.
**
** ========================================================================= */

void Dmovi_StopHandler
(
  DmovDevCtxt   * pDevCtxt,
  Dmov_ChanType * pChan
)
{
  uint32 dwDevice = pDevCtxt->dwDeviceInstance;
  uint32 dwChan = pChan->dwChan;
  uint32 dwNumQueues;
  HAL_dmov_SecurityLevel eSecLevel = HAL_dmov_GetSecurityLevel(dwDevice);

  /* Is channel already stopped? */
  if (pChan->eState == DMOV_CHAN_STATE_STOP)
  {
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO,
                    "DMOV Stop Handler: Channel already stopped");
    return;
  }

  /* Verify that channel state is valid */
  if (pChan->eState != DMOV_CHAN_STATE_STOPPING &&
      pChan->eState != DMOV_CHAN_STATE_ERROR)
  {
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR, 
		"DMOV Stop Handler: channel %d internal state error: %d", pChan->dwChan, pChan->eState);
    pChan->eState = DMOV_CHAN_STATE_FATAL;

    /* Notify the owner of this channel that an error occured */
    if (pChan->hEvent != NULL)
    {
      DALSYS_EventCtrl(pChan->hEvent,DALSYS_EVENT_CTRL_TRIGGER);
    }
    return;
  }

  if (pChan->eState == DMOV_CHAN_STATE_STOPPING)
  {
    /* Check for stop conditions */
    if (DALFW_Q_Cnt(&pChan->PENDING_Q) != 0)
    {
      DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR, 
                      "DMOV Stop Handler: cannot stop channel %d, commands pending", pChan->dwChan);
      return;
    }
    for (dwNumQueues = 0; dwNumQueues < pChan->dwNumQueues; dwNumQueues++)
    {
      if (DALFW_Q_Cnt(&pChan->CMD_Q[dwNumQueues]) != 0)
      {
        DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR, 
                        "DMOV Stop Handler: cannot stop channel %d, issuing commands", pChan->dwChan);
        return;
      }
    }
  }

  /* Stop channel hardware if we are running on the secure processor */
  if ( eSecLevel == HAL_DMOV_SECURE_BOOT ||
       eSecLevel == HAL_DMOV_SECURE_AMSS )
  {
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO, 
                    "DMOV Stop Handler: Stopping the channel hardware");
    HAL_dmov_DisableChan(dwDevice, dwChan);
  }

  /* Verify that an error has not occurred */
  if (pChan->eState == DMOV_CHAN_STATE_ERROR)
  {
    /* Indicate a hardware error occurred during stop operation */
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR, 
                    "DMOV Stop Handler: hardware channel %d error occured during stop", pChan->dwChan);

    /* Notify the owner of this channel that an error occured */
    if (pChan->hEvent != NULL)
    {
      DALSYS_EventCtrl(pChan->hEvent,DALSYS_EVENT_CTRL_TRIGGER);
    }
  }
  else
  {
    /* Stop conditions are satisfied */
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO, 
                    "DMOV Stop Handler: Channel stopped");
    pChan->eState = DMOV_CHAN_STATE_STOP;
  }
}

/* ============================================================================
**
**  Dmovi_ReadResult
**
**  Description:
**    If a result is pending on the specified Data Mover channel, it will be
**    read and the user callback will be invoked.
**
**  Parameters:
**    pDevCtxt - Pointer to the Device Context
**    pChan - Pointer to the dmov channel
**
**  Return:
**    TRUE  - a result was read
**    FALSE - no result was read
**
**  Dependencies:
**    It is assumed that this function is called from interrupt context
**    if the Data Mover driver is in interrupt mode, or that channel
**    interrupts are disabled if the Data Mover driver is in poll mode.
**
** ========================================================================= */

DALBOOL Dmovi_ReadResult
(
  DmovDevCtxt   * pDevCtxt,
  Dmov_ChanType * pChan     /* Pointer to channel state struct */
)
{
  DmovTransferQItem *pTransferDescriptor;
  uint32 dwResult;
  HAL_dmov_ResultInfo ResultInfo;
  uint32 dwDevice = pDevCtxt->dwDeviceInstance;
  uint32 dwChan = pChan->dwChan;
  DALBOOL bFreeTransfer;

  if (pDevCtxt->dwActiveCount <= 0)
  {
    /* DMOV clock may be off, don't continue and access HW registers */
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO, 
                    "DMOV Read Result: no active transfers, clock may be off, returning!");
    return FALSE;
  }

  /* Read next result and check whether it is valid */
  dwResult = HAL_dmov_GetChanResult(dwDevice, dwChan);

  ResultInfo = HAL_dmov_CheckChanResult(dwResult);

  if (HAL_DMOV_INVALID_RESULT == ResultInfo)
  {
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO, 
                    "DMOV Read Result: Result FIFO is empty");
    return FALSE;  /* Result FIFO is empty */
  }

  pChan->dwResultsCount++;

  /* Check whether a channel error occurred */
  if ((HAL_DMOV_ERROR_RESULT == ResultInfo) ||
      (HAL_DMOV_FLUSH_RESULT == ResultInfo))
  {
    if (HAL_DMOV_ERROR_RESULT == ResultInfo)
	{
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR, 
                    "DMOV Read Result: Channel %d error occurred", pChan->dwChan);
	}
	else
	{
       DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_INFO, 
                    "DMOV Read Result: Channel %d flush occurred", pChan->dwChan);
	}

    /* Call the channel flush handler to gather the flush state.
     * This function will invoke the required user callback(s).
     */
    Dmovi_FlushHandler(pDevCtxt, pChan, dwResult);
    return TRUE;
  }

  pChan->bErrorValid = FALSE;

  /* Dequeue the transfer descriptor if this was a top-level result.
   * Otherwise, leave it queued because the result was generated
   * due to a command with FR (force result) set.
   */
  if ((HAL_DMOV_TPD_RESULT == ResultInfo) |
      (HAL_DMOV_FLUSH_RESULT == ResultInfo))
  {
    pTransferDescriptor = DALFW_Q_Get(&pChan->PENDING_Q);
    bFreeTransfer = TRUE;
  }
  else
  {
    pTransferDescriptor = DALFW_Q_Check(&pChan->PENDING_Q);
    bFreeTransfer = FALSE;
  }

  if (NULL == pTransferDescriptor)
  {
    pChan->dwInternalErrorCount++;
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR, 
                    "DMOV Read Result: result with no pending Transfer on channel %d!", pChan->dwChan);
    pChan->dwInternalErrorCount++;
    return TRUE;
  }

  /* Set result value for user callback to pick up */
  pTransferDescriptor->Results.dwResultValue = dwResult;
  pTransferDescriptor->Results.eState = DMOV_TRANSFER_SUCCESSFULL;

  /* Check for race condition where flush was requested but all commands
   * had already been completed by the DM hardware.
   */
  if (pChan->eState == DMOV_CHAN_STATE_FLUSHING ||
      pChan->eState == DMOV_CHAN_STATE_FLUSHING_ALL)
  {
    if (DALFW_Q_Cnt(&pChan->PENDING_Q) == 0)
    {
      if (pChan->eState == DMOV_CHAN_STATE_FLUSHING_ALL)
      {
        Dmovi_FlushQueues(pDevCtxt, pChan);
      }
      pChan->eState = DMOV_CHAN_STATE_RUN;
    }
  }

  Dmovi_MoveToNotifyQueue(pDevCtxt, pChan, pTransferDescriptor, bFreeTransfer);

  return TRUE;
}

/* ============================================================================
**
**  Dmovi_ChanHandler
**
**  Description:
**    Handle an interrupt/poll on the specified Data Mover channel.
**    Pending results will be read and user callbacks will be invoked.
**    If the channel is ready, waiting commands will be issued on a
**    priority basis.
**
**  Parameters:
**    pDevCtxt - Pointer to the Device Context
**    pChan - Pointer to the dmov channel
**
**  Return:
**    None
**
**  Dependencies:
**    Should be called within synchronization.
**    It is assumed that this function is called from interrupt context
**    if the Data Mover driver is in interrupt mode, or that channel
**    interrupts are disabled if the Data Mover driver is in poll mode.
**
** ========================================================================= */

void Dmovi_ChanHandler
(
  DmovDevCtxt   * pDevCtxt,
  Dmov_ChanType * pChan
)
{
  Dmov_ChanState eState;
  DALBOOL bDone;

  /* Check and record channel's initial state */
  eState = pChan->eState;

 //if (!DMOVI_CHAN_IS_ACTIVE_LOCAL(eState))
  if((eState < DMOV_CHAN_STATE_RUN) || (eState > DMOV_CHAN_STATE_FLUSHING_ALL))
  {
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR, "DMOV Chan Handler: Channel %d not active", pChan->dwChan);
    /* Do not service a channel that is not in a local active state */
    return;
  }

  /* Loop until the command FIFO is full or there are no more transfers
   * queued, and the result FIFO is empty.
   */
  do
  {
    /* Assume there will be no processing performed during this pass */
    bDone = TRUE;

    /* Process pending results */
    while (TRUE == Dmovi_ReadResult(pDevCtxt, pChan))
    {
      bDone = FALSE;
    }

    /* Feed the command pointer register */
    if (TRUE == Dmovi_IssueTransfer(pDevCtxt, pChan))
    {
      bDone = FALSE;
    }

  } while (!bDone);

  /* If channel's initial state was STOPPING, call the stop handler */
  if (eState == DMOV_CHAN_STATE_STOPPING)
  {
    Dmovi_StopHandler(pDevCtxt, pChan);
  }
}

/* ============================================================================
**
**  Dmovi_FlushRemoteChan
**
**  Description:
**    This function flushes a channel which is *not* controlled by the
**    local processor.  This function is intended to clear a channel of
**    a host that is being reset.
**
**  Parameters:
**    pDevCtxt - Pointer to the Device Context
**    dwChan - The dmov channel number
**
**  Return:
**    TRUE  - Success, channel was flushed
**    FALSE - Failure
**
**  Dependencies:
**    The current transfer in progress on the channel will be aborted.
**    This could cause errors or indeterminate state on the controlling host.
**
** ========================================================================= */
#ifdef DMOV_TZOS
DALBOOL Dmovi_FlushRemoteChan
(
  DmovDevCtxt * pDevCtxt,
  uint32        dwChan
)
{
	DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                    "DMOV Flush Domain: Flusing not supported in trust zone dmov");
    	return FALSE;
}
#else
DALBOOL Dmovi_FlushRemoteChan
(
  DmovDevCtxt * pDevCtxt,
  uint32        dwChan
)
{
  uint32 dwConfigValue;
  uint32 dwResultValue;
  int dwCount;
  DALBOOL bFlushed;
  DALBOOL bResult;
  uint32 dwDevice = pDevCtxt->dwDeviceInstance;
  HAL_dmov_SecurityLevel eSecLevel = HAL_dmov_GetSecurityLevel(dwDevice);

  HAL_dmov_ResultInfo eResultInfo;

  /* Make sure callres of this function are running on the secure processor */
  if (eSecLevel == HAL_DMOV_NON_SECURE_BOOT || 
      eSecLevel == HAL_DMOV_NON_SECURE_AMSS)
  {
    DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                    "DMOV Flush Domain: DMOV device not running on secure proc");
    return FALSE;
  }

  /* save chan configuration */
  dwConfigValue = HAL_dmov_GetChanConfig(dwDevice, dwChan);

  /* Reconfigure the channel to force a flush result and disable interrupts */
  HAL_dmov_SetChanMode(dwDevice, dwChan, (HAL_dmov_ChanOpMode)DMOV_CHAN_MODE_POLLING);
  HAL_dmov_SetChanForceFlushResult(dwDevice, dwChan, TRUE);

  /* Perform channel flush with discard */
  HAL_dmov_FlushChan(dwDevice, dwChan, TRUE);

  /* Use a do loop to insure proper clean up on failure */
  bResult = FALSE;   /* Break from loop means failure occurred */
  do
  {
    /* Verify the channel flush state */
    for (dwCount = DMOV_FLUSH_POLL_MAX_COUNT; dwCount > 0; dwCount--)
    {
      if (HAL_dmov_ChanFlushIsComplete(dwDevice, dwChan))
      {
        /* Flush is complete */
        break;
      }
    }
    if (dwCount <= 0)
    {
      DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                      "DMOV Flush Remote Chan %d: flush state time out", dwChan);
      break;
    }

    /* Was the channel idle? */
    bFlushed = FALSE;
    if (HAL_dmov_FlushChanStateIsIdle (dwDevice, dwChan))
    {
      /* If the channel was idle, then there will be no flush result.
       * Enter result loop below anyway, to clear any pending results.
       */
      bFlushed = TRUE;
    }
    else
    {
      pDevCtxt->dwFlushNotIdleCounter++;
    }

    /* Wait for the flush result to insure that all bus activity is complete */
    for (dwCount = (DMOV_FLUSH_POLL_MAX_COUNT * 64); dwCount > 0; dwCount--)
    {
      dwResultValue = HAL_dmov_GetChanResult(dwDevice, dwChan);

      eResultInfo = HAL_dmov_CheckChanResult(dwResultValue);

      if (HAL_DMOV_INVALID_RESULT == eResultInfo)
      {
        /* Not a valid result */
        if (bFlushed)
        {
          /* Flush result was already received, so stop processing */
          break;
        }
        /* Continue polling for flush result */
        continue; 
      }
      pDevCtxt->dwFlushTotalResultsCounter++;

      /* Is this a flush result? */
      if (HAL_DMOV_FLUSH_RESULT == eResultInfo)
      {
        /* Flush result was received */
        bFlushed = TRUE;
        pDevCtxt->dwFlushResultsCounter++;
      }
    }
    if (!bFlushed)
    {
      DMOV_LOGEVENT(pDevCtxt->DevId, DALSYS_LOGEVENT_ERROR,
                      "DMOV Flush Remote Chan %d: result time out", dwChan);
      break;
    }

    /* Flush was completed successfully */
    bResult = TRUE;

  } while (FALSE);  /* Just one pass through the loop */

  /* Restore channel configuration */
  HAL_dmov_SetChanConfig(dwDevice, dwChan, dwConfigValue);
  pDevCtxt->dwFlushedChanCounter++;

  return bResult;
}
#endif  //DAL_TZOS
