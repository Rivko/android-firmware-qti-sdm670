/*==============================================================================

FILE:      uInterruptController.c

DESCRIPTION
  This modules implements the API to utilize the Micro interrupt controller.
  This Micro Interrupt Controller module is strictly used in Kernel Protection
  Domain and in Micro image.The definitions in this module are implemented internally
  and should not be exposed for external client usage directly without using the
  accompanying uInterruptController.h in the core/api/systemdrivers interface file for this module.

REFERENCES

       Copyright (c) 2014-2017 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/systemdrivers/InterruptController/src/qurt/uimage/uInterruptControllerIsland.c#6 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
1/1/2010   aratin  First draft created. 
===========================================================================*/

// #include "uTimetick.h" so far now not sure about the logging plan.

#include <atomic_ops.h>
#include <qurt.h>
#include "qurt_int.h"
#include "qurt_anysignal.h"
#include "stringl.h"
#include "uInterruptController.h"
#include "uInterruptControllerInternal.h"
#include "err.h"

/*------------------------------------------------------------------------------
 uInterrupt Data Declarations.
------------------------------------------------------------------------------*/ 

/*
 * This is static uInterrupt state data. It can be accessed for debugging 
 * uInterrupts to see what is the current registration state of the uInterrupt.
 */
uIntCntrlType uIntData;

/*------------------------------------------------------------------------------
 uInterrupt Macro Declarations.
------------------------------------------------------------------------------*/ 
#define UINT_PRIORITY 4

#define VALID_UINT(x) ((x) < MAX_NUMBER_OF_INTS)
#define VALID_UINT_IDX(x) (uIntData.auIntLUT[x] != UINT_NONE)
#define UINT_IDX(x) (uIntData.auIntLUT[x])



/*------------------------------------------------------------------------------
Following functions are internal functions.
------------------------------------------------------------------------------*/

void uInterruptController_ISTMain(void * ISTParam);

/*==========================================================================

  FUNCTION      GetInterruptFromSignalMask
 
  DESCRIPTION   This function returns the interrupt to be registered by the IST
  that matches the given signal mask.
  
==========================================================================*/

static uint32 GetInterruptFromSignalMask
(
  uint32 nSignalMask
 
)
{
  uint32 nIdx;
  for (nIdx = 0;nIdx < MAX_NUMBER_OF_UINTS ; nIdx ++) 
  {
    if (uIntData.state[nIdx].nIntRegMask & nSignalMask) 
    {
      qurt_anysignal_clear(&uIntData.ISTSignal,uIntData.state[nIdx].nIntRegMask);
      return (uint32)uIntData.state[nIdx].qurt_intr_id;
    }
  }
  return UINT_NONE;
} /* END GetInterruptFromSignalMask */


/*=============================================================================

  FUNCTION      uInterruptController_ISTMain

  DESCRIPTION   This is the main Micro Interrupt service thread function that 
  processes incoming L2VIC interrupts from QURT.

  PARAMETERS
  void * ISTParam  this parameter for now is ignored.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.


==========================================================================*/
void uInterruptController_ISTMain
(
  void * ISTParam
)
{
  int                            nStatus;
  uint32                         nIdx, nIntIdx;
  uint32                         nInterruptID;
  uint32                         nTrigger;
  uIRQ                           ClientIsr;
  uIRQCtx                        ClientIsrParam;
  unsigned int                   nSignalValue;
  
  nInterruptID = UINT_NONE;
  uIntData.nThreadID = qurt_thread_get_id();

  /*
   * Main loop.  Process an interrupt IPC, then wait for another.
   */
  while(1)
  {

    while (1)
    {
      unsigned int nSignalValue;
      
      nSignalValue = qurt_anysignal_wait( &uIntData.ISTSignal, 
                                          uIntData.nIntRegistrationMask | uIntData.nIntMask | SIG_INT_ABORT | UINT_TASK_STOP);
      
      if (SIG_INT_ABORT & nSignalValue)
      {
        /*
         * Clear task stop signal.
         */
        uIntData.nBreakSigMask = nSignalValue;
        uIntData.nBreakSigLoc = 1;
        qurt_anysignal_clear(&uIntData.ISTSignal,SIG_INT_ABORT);
        continue;
      }
      if (UINT_TASK_STOP & nSignalValue)
      {
        /*
         * Clear task stop signal.
         */
        uIntData.nBreakSigMask = nSignalValue;
        uIntData.nBreakSigLoc = 2;
        qurt_anysignal_clear(&uIntData.ISTSignal,UINT_TASK_STOP);
        continue;
      }
      qurt_mutex_lock(&uIntData.uIntLock);
      if (nSignalValue & uIntData.nIntRegistrationMask)
      {
        /*
         * we get the QURT Interrupt and clear the interrupt registration mask that was set.
         */
        nInterruptID = GetInterruptFromSignalMask(nSignalValue & uIntData.nIntRegistrationMask);
        if (nInterruptID == UINT_NONE || nInterruptID >= MAX_NUMBER_OF_INTS ) 
        {
          /*
           * Clear signal that may be recently deregistered.
           */
          nSignalValue &= uIntData.nIntRegistrationMask;
          qurt_anysignal_clear(&uIntData.ISTSignal, nSignalValue);
          qurt_mutex_unlock(&uIntData.uIntLock); 
          continue;
        }
         /* 
          * Register with QURT using the interrupt vector
          */
        nStatus = qurt_interrupt_register(nInterruptID,&uIntData.ISTSignal,
                                    uIntData.state[UINT_IDX(nInterruptID)].nInterruptMask);

        qurt_mutex_unlock(&uIntData.uIntLock); 
        if (nStatus != QURT_EOK) 
        {
          ERR_FATAL("uInterruptController Register Failed for interrupt %d",nInterruptID,0,0);
          uIntData.nBreakSigMask = nSignalValue;
          uIntData.nBreakSigLoc = 3;
        }
        continue;     
      }
      qurt_mutex_unlock(&uIntData.uIntLock); 
      /*
       * If its not a task stop due to deregistration or an error due to qdi
       * we should error fatal if we still do not get the interrupt registration signal.
       */
      if ( !(uIntData.nIntMask  & nSignalValue) )
      {
        /* Ideally throw an error in Uimage*/
        uIntData.nBreakSigMask = nSignalValue;
        uIntData.nBreakSigLoc = 4;
        qurt_anysignal_clear(&uIntData.ISTSignal, nSignalValue);
      }

      qurt_mutex_lock(&uIntData.uIntLock);

      /*
       * find the interrupt that fired.
       */
      nIntIdx = UINT_NONE;
      for (nIdx=0;nIdx < MAX_NUMBER_OF_UINTS;nIdx++) 
      {
        if (uIntData.state[nIdx].nInterruptMask & nSignalValue) 
        {
          nInterruptID = (uint32)uIntData.state[nIdx].qurt_intr_id;
          nIntIdx = nIdx;
          break;
        }
      }

      if (nInterruptID == UINT_NONE || nInterruptID >= MAX_NUMBER_OF_INTS || nIntIdx == UINT_NONE ) 
      {
        // Ideally throw an error here .
        /* This can be an infinite loop and disallow power collapse... */
        qurt_anysignal_clear(&uIntData.ISTSignal,nSignalValue);
        qurt_mutex_unlock(&uIntData.uIntLock); 
        continue;
      }
 
      ClientIsr = uIntData.state[nIntIdx].isr;
      ClientIsrParam = uIntData.state[nIntIdx].isr_param;

      nTrigger = uIntData.state[nIntIdx].intr_trigger;

      qurt_mutex_unlock(&uIntData.uIntLock); 

      if ( nTrigger != UINTERRUPTCONTROLLER_LEVEL_HIGH_TRIGGER &&
           nTrigger != UINTERRUPTCONTROLLER_LEVEL_LOW_TRIGGER )
      {
        /*
         * Clear signal and reactivate interrupt.
         */
        qurt_anysignal_clear(&uIntData.ISTSignal,(uIntData.state[nIntIdx].nInterruptMask | SIG_INT_ABORT | UINT_TASK_STOP));
        nStatus = qurt_interrupt_acknowledge(uIntData.state[nIntIdx].qurt_intr_id);
      }

      if (ClientIsr == NULL) 
      {
        uInterruptController_UnRegister(nInterruptID);
      }
      else 
      {
        ClientIsr(ClientIsrParam);
      }

      if ( nTrigger == UINTERRUPTCONTROLLER_LEVEL_HIGH_TRIGGER ||
           nTrigger == UINTERRUPTCONTROLLER_LEVEL_LOW_TRIGGER )
      {
        /*
         * Clear signal and reactivate interrupt.
         */
        qurt_anysignal_clear(&uIntData.ISTSignal,(uIntData.state[nIntIdx].nInterruptMask | SIG_INT_ABORT | UINT_TASK_STOP));
        nStatus = qurt_interrupt_acknowledge(uIntData.state[nIntIdx].qurt_intr_id);
      }

      if (QURT_EOK != nStatus)
      {
        /* Ideally throw an error*/
        uIntData.nBreakSigMask = nSignalValue;
        uIntData.nBreakSigLoc = 5;
      }
      // Continue here
    }

    nSignalValue = qurt_anysignal_wait(&uIntData.ISTSignal,
                                     (UINT_TASK_START | SIG_INT_ABORT | UINT_TASK_STOP));
    /*
     * Wait for the next time this interrupt is registered with QURT.
     */
    if (UINT_TASK_START & nSignalValue)
    {
      /*
       * Clear signal and reactivate interrupt.
       */
      qurt_anysignal_clear(&uIntData.ISTSignal,(UINT_TASK_START | SIG_INT_ABORT | UINT_TASK_STOP));
    }  
  }
} /* END uInterruptController_ISTMain */

/*==========================================================================

   FUNCTION      uInterruptController_IsInterruptPending

   DESCRIPTION   See uInterruptController.h

==========================================================================*/
int32
uInterruptController_IsInterruptPending( uint32 nInterruptID, uint32 *  state)
{

  if ( !VALID_UINT( nInterruptID ) || state == NULL )
  {
    return UINTERRUPT_ERROR;
  }

  if(QURT_EOK != qurt_interrupt_status((unsigned int)nInterruptID, (int *)state))
  {
    return UINTERRUPT_ERROR;
  }
 
  return UINTERRUPT_SUCCESS;

} /* END uInterruptController_IsInterruptPending */



/*==========================================================================

   FUNCTION      uInterruptController_TriggerInterrupt

   DESCRIPTION   See uInterruptController.h

==========================================================================*/
int32
uInterruptController_TriggerInterrupt(uint32 nInterruptID)
{
  if (!VALID_UINT(nInterruptID))
  {
    return UINTERRUPT_ERROR;
  }

  if( QURT_EOK != qurt_interrupt_raise((unsigned int) nInterruptID) )
  {
    return UINTERRUPT_ERROR;
  }

  return UINTERRUPT_SUCCESS;

} /* END uInterruptController_TriggerInterrupt */


/*==========================================================================

   FUNCTION      uInterruptController_Enable

   DESCRIPTION   See uInterruptController.h

==========================================================================*/
int32
uInterruptController_Enable(uint32 nInterruptID, boolean bEnable)
{
  int32 nRet = UINTERRUPT_SUCCESS;
  unsigned int nErr;

  if (!VALID_UINT(nInterruptID))
  {
    return UINTERRUPT_ERROR;
  }

  qurt_mutex_lock(&uIntData.uIntLock);

  if ( bEnable == TRUE )
  {
    nErr = qurt_interrupt_enable((unsigned int) nInterruptID);
    if( QURT_EOK == nErr )
    {
      uIntData.state[UINT_IDX(nInterruptID)].intr_flags &= ~UINTF_DISABLED;
    }
    else if ( QURT_EINT == nErr )
    {
      nRet = UINTERRUPT_ERROR_EINT;
    } 
    else if ( QURT_EVAL == nErr )
    {
      nRet = UINTERRUPT_ERROR_EVAL;
    } 
    else
    {
      nRet = UINTERRUPT_ERROR;
    }
  }
  else
  {
    nErr = qurt_interrupt_disable((unsigned int) nInterruptID);
    if( QURT_EOK == nErr )
    {
      uIntData.state[UINT_IDX(nInterruptID)].intr_flags |= UINTF_DISABLED;
    }
    else if ( QURT_EINT == nErr )
    {
      nRet = UINTERRUPT_ERROR_EINT;
    }
    else if ( QURT_EVAL == nErr )
    {
      nRet = UINTERRUPT_ERROR_EVAL;
    }
    else
    {
      nRet = UINTERRUPT_ERROR;
    }
  }

  qurt_mutex_unlock(&uIntData.uIntLock);

  return nRet;
}


