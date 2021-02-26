/*==============================================================================

FILE:      uGPIOInt.c

DESCRIPTION
  This modules implements the API to utilize the Micro GPIO interrupt controller.
  This Micro GPIO Interrupt Controller module is strictly used in USER Protection
  Domain and in Micro image.The definitions in this module are implemented internally
  and should not be exposed for external client usage directly without using the
  accompanying uGPIOInt.h in the core/api/systemdrivers interface file for this module.

REFERENCES

       Copyright (c) 2014-2015 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/systemdrivers/GPIOInt/src/user/uimage/uGPIOIntUserIsland.c#4 $

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
#include "uGPIOIntUser.h"
#include "uPdcTimestamp.h"

/*------------------------------------------------------------------------------
 GPIOInt Data Declarations.
------------------------------------------------------------------------------*/ 

/*
 * This is static uGPIOInt state data. It can be accessed for debugging 
 * uGPIO Interrupts to see what is the current registration state of the GPIO.
 */
uGPIOIntCntrlType uGPIOIntData;

/*------------------------------------------------------------------------------
 GPIOInt Macro Declarations.
------------------------------------------------------------------------------*/ 
#define UGPIOINT_PRIORITY 4

#define VALID_UGPIO(x) ((x) < MAX_NUMBER_OF_GPIOS)
#define VALID_UGPIO_IDX(x) (uGPIOIntData.aGPIOLUT[x] != UGPIOINT_NONE)
#define UGPIOINT_IDX(x) (uGPIOIntData.aGPIOLUT[x])
#define UGPIOINT_ALT_IDX(x) (uGPIOIntData.state[UGPIOINT_IDX(x)].nAltIdx)
#define UGPIOINT_VALID_ALT_IDX(x) (uGPIOIntData.state[UGPIOINT_IDX(x)].nAltIdx < MAX_NUMBER_OF_UGPIOS)

#define SET_UPPER_16(x) (((x) & 0xffff) << 16)
#define SET_LOWER_16(x)  ((x) & 0xffff)
#define GET_UPPER_16(x) (((x) >> 16) & 0xffff)
#define GET_LOWER_16(x)  ((x) & 0xffff)


/* 
 * For each interrupt handler thread a Default IST stack size is declared.
 */
#define GPIOINT_IST_STACK_SIZE  2048

/*------------------------------------------------------------------------------
Following functions are internal functions.
------------------------------------------------------------------------------*/

void uGPIOInt_ISTMain(void * ISTParam);


static uint32 uGPIOInt_GetHighestBmsk
(
   uint32 nSignalMask
)
{
  uint32 nIter;

  if ( nSignalMask == 0x0 || nSignalMask == SIG_INT_ABORT )
  {
    return 0;
  }

  for ( nIter = 0; nIter < 31; nIter++ )
  {
    if ( nSignalMask & ( 0x1 << nIter ) )
    {
      return ( 0x1 << nIter );
    }
  }

  return 0;
}

/*==========================================================================

  FUNCTION      GetGPIOFromSignalMask
 
  DESCRIPTION   This function returns the gpio to be registered by the IST
  that matches the given signal mask.
  
==========================================================================*/

static uint32 GetGPIOFromSignalMask
(
  uint32  nSignalMask,
  uint32* pnGpio
 
)
{
  uint32 nIdx;
  for (nIdx=0; nIdx < MAX_NUMBER_OF_UGPIOS; nIdx++) 
  {
    if (uGPIOIntData.state[nIdx].nInterruptMask & nSignalMask) 
    {
      if ( pnGpio != NULL )
      {
        *pnGpio = (uint32)uGPIOIntData.state[nIdx].nGPIO;
      }
      return nIdx;
    }
  }
  return UGPIOINT_NONE;
} /* END GetGPIOFromSignalMask */


/*=============================================================================

  FUNCTION      uGPIOInt_ISTMain

  DESCRIPTION   This is the main Micro Interrupt service thread function that 
  processes incoming GPIO interrupts from QURT.

  PARAMETERS
  void * ISTParam  this parameter for now is ignored.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.


==========================================================================*/
void uGPIOInt_ISTMain
(
  void * ISTParam
)
{
  int                            nStatus;
  uint32                         nTrigger;
  uint8                          nFlags;
  uint32                         gpio, nGpioIdx, nIntId;
  uGPIOINTISR                    ClientIsr;
  uGPIOINTISRCtx                 ClientIsrParam;
  //unsigned int nSignalValue;
  
  gpio = UGPIOINT_NONE;
  uGPIOIntData.nThreadID = qurt_thread_get_id();

  /*
   * Main loop.  Process an interrupt IPC, then wait for another.
   */
  while(1)
  {

    while (1)
    {
      unsigned int nSignalValue;
      
      nSignalValue = qurt_anysignal_wait( &uGPIOIntData.ISTSignal, 
                                          UGPIOINT_TASK_INTMASK | SIG_INT_ABORT );
      
      if (SIG_INT_ABORT & nSignalValue)
      {
        /*
         * Clear task stop signal.
         */
        qurt_anysignal_clear(&uGPIOIntData.ISTSignal,SIG_INT_ABORT);
        break;
      }

      nSignalValue = uGPIOInt_GetHighestBmsk(nSignalValue);

      qurt_mutex_lock(&uGPIOIntData.uGPIOIntLock);
      if (nSignalValue & uGPIOIntData.nGPIOIntRegistrationMask)
      {
        /*
         * we get the GPIO and clear the interrupt registration mask that was set.
         */
        nGpioIdx = GetGPIOFromSignalMask(nSignalValue & uGPIOIntData.nGPIOIntRegistrationMask, &gpio);
        if ( nGpioIdx >= MAX_NUMBER_OF_UGPIOS ||
             gpio == UGPIOINT_NONE ||
             gpio >= MAX_NUMBER_OF_GPIOS ) 
        {
          /*
           * Clear signal that may be recently deregistered.
           */
          nSignalValue &= uGPIOIntData.nGPIOIntRegistrationMask;
          qurt_anysignal_clear(&uGPIOIntData.ISTSignal, nSignalValue);
          qurt_mutex_unlock(&uGPIOIntData.uGPIOIntLock);
          continue;
        }
         /* 
          * Register with QURT using the interrupt vector
          */
        nStatus = qurt_interrupt_register(uGPIOIntData.state[nGpioIdx].qurt_intr_id,&uGPIOIntData.ISTSignal,
                                    uGPIOIntData.state[nGpioIdx].nInterruptMask);

        /*
         * Clear registration signal
         */
        uGPIOIntData.nGPIOIntRegistrationMask &= ~nSignalValue;
        qurt_anysignal_clear(&uGPIOIntData.ISTSignal,nSignalValue);

        /*
         * Send registration feedback
         */
        if (uGPIOIntData.state[nGpioIdx].pAckSig != NULL)
        {
          qurt_anysignal_set(uGPIOIntData.state[nGpioIdx].pAckSig, uGPIOIntData.state[nGpioIdx].nInterruptMask);
          uGPIOIntData.state[nGpioIdx].pAckSig = NULL;
        }

        qurt_mutex_unlock(&uGPIOIntData.uGPIOIntLock); 
        if (nStatus != QURT_EOK) 
        {
          /* throw an error  */
          break;
        }
        continue;     
      }
      else
      {
        qurt_mutex_unlock(&uGPIOIntData.uGPIOIntLock);
      }

      /*
       * If its not a task stop due to deregistration or an error due to qdi
       * we should error fatal if we still do not get the interrupt registration signal.
       */
      if ( !(uGPIOIntData.nGPIOIntMask  & nSignalValue) )
      {
        /* Ideally throw an error in Uimage*/
        break;
      }

      qurt_mutex_lock(&uGPIOIntData.uGPIOIntLock);

      /*
       * find the interrupt that fired.
       */
      nGpioIdx = GetGPIOFromSignalMask(nSignalValue, &gpio);
 
      if((nGpioIdx >= MAX_NUMBER_OF_UGPIOS) || (gpio == UGPIOINT_NONE) || (gpio >= MAX_NUMBER_OF_GPIOS))
      {
        /*
         * Could not find the interrupt that fired.
         */
        qurt_mutex_unlock(&uGPIOIntData.uGPIOIntLock);
        continue; 
      }
 
      ClientIsr = uGPIOIntData.state[nGpioIdx].isr;
      ClientIsrParam = uGPIOIntData.state[nGpioIdx].isr_param;

      nTrigger = uGPIOIntData.state[nGpioIdx].intr_trigger;
      nFlags   = uGPIOIntData.state[nGpioIdx].gpio_intr_flags;
      nIntId   = uGPIOIntData.state[nGpioIdx].qurt_intr_id;

      qurt_anysignal_clear(&uGPIOIntData.ISTSignal,(uGPIOIntData.state[nGpioIdx].nInterruptMask | SIG_INT_ABORT));

      qurt_mutex_unlock(&uGPIOIntData.uGPIOIntLock); 

      if ( nTrigger != UGPIOINT_TRIGGER_HIGH &&
           nTrigger != UGPIOINT_TRIGGER_LOW )
      {
        if ( ( nFlags & UGPIOINTF_DSR ) == 0 )
        {
          nStatus = qurt_interrupt_acknowledge(nIntId);
        }
      }

      if (ClientIsr != NULL) 
      {
        ClientIsr(ClientIsrParam);
      }
      
      if ( nTrigger == UGPIOINT_TRIGGER_HIGH ||
           nTrigger == UGPIOINT_TRIGGER_LOW )
      {
        if ( ( nFlags & UGPIOINTF_DSR ) == 0 )
        {
          nStatus = qurt_interrupt_acknowledge(nIntId);
        }
      }
    }
  }
} /* END GPIOInt_ISTMain */


/*==========================================================================

   FUNCTION      uGPIOInt_IsInterruptPending

   DESCRIPTION   See uGPIOInt.h

==========================================================================*/
int32
uGPIOInt_IsInterruptPending( uint32  gpio, uint32 *  state)
{
  int nStatus, nRet;
  int32 nResult;
  nResult = UGPIOINT_SUCCESS;
  if ((!VALID_UGPIO(gpio))||(!VALID_UGPIO_IDX(gpio))||(state == NULL))
  {
    return UGPIOINT_ERROR;
  }
  qurt_mutex_lock(&uGPIOIntData.uGPIOIntLock);
  nRet = qurt_interrupt_status(uGPIOIntData.state[UGPIOINT_IDX(gpio)].qurt_intr_id, &nStatus);
  if (QURT_EOK != nRet)
  {
    *state = 0;

    nResult = UGPIOINT_ERROR;
  }
  else 
  {
    if(nStatus == 1)
    {
      *state = 1;
    }
    else if (uGPIOIntData.state[UGPIOINT_IDX(gpio)].nAltIdx < MAX_NUMBER_OF_UGPIOS)
    {
      nRet = qurt_interrupt_status(uGPIOIntData.state[UGPIOINT_ALT_IDX(gpio)].qurt_intr_id, &nStatus);
      if (QURT_EOK != nRet)
      {
        *state = 0;
        nResult = UGPIOINT_ERROR;
      }
      else
      {
        if ( nStatus == 1 )
        {
          *state = 1;
        }
      }
    }
    else
    {
      *state = 0;
    }
  }
 
  qurt_mutex_unlock(&uGPIOIntData.uGPIOIntLock); 

  return nResult;


} /* END uGPIOInt_IsInterruptPending */


/*==========================================================================

   FUNCTION      uGPIOInt_TriggerInterrupt

   DESCRIPTION   See uGPIOInt.h

==========================================================================*/
int32
uGPIOInt_TriggerInterrupt(uint32 gpio)
{
  if ((!VALID_UGPIO(gpio))||(!VALID_UGPIO_IDX(gpio)))
  {
    return UGPIOINT_ERROR;
  }
  qurt_mutex_lock(&uGPIOIntData.uGPIOIntLock);
  qurt_interrupt_raise((unsigned int) uGPIOIntData.state[UGPIOINT_IDX(gpio)].qurt_intr_id); 
  qurt_mutex_unlock(&uGPIOIntData.uGPIOIntLock); 

  return UGPIOINT_SUCCESS;

} /* END uGPIOInt_TriggerInterrupt */


/*=============================================================================
 
   FUNCTION      uGPIOInt_GetInterruptID

   DESCRIPTION   See uGPIOInt.h

=============================================================================*/
int32 
uGPIOInt_GetInterruptID
(
  
  uint32 gpio,
  uint32* pnInterruptID
)
{
  if ((!VALID_UGPIO(gpio)) || (!VALID_UGPIO_IDX(gpio)) || pnInterruptID == NULL)
  {
    return UGPIOINT_ERROR;
  }

  qurt_mutex_lock(&uGPIOIntData.uGPIOIntLock);
  *pnInterruptID = (uint32)uGPIOIntData.state[UGPIOINT_IDX(gpio)].qurt_intr_id;
  qurt_mutex_unlock(&uGPIOIntData.uGPIOIntLock); 

  return UGPIOINT_SUCCESS;
}

/*=============================================================================
  FUNCTION      uGPIOInt_EnableInterrupt
=============================================================================*/
int32 
uGPIOInt_EnableInterrupt
(
  
  uint32 gpio
)
{
  unsigned int nStatus;
  if ((!VALID_UGPIO(gpio))||(!VALID_UGPIO_IDX(gpio)))
  {
    return UGPIOINT_ERROR;
  }
  qurt_mutex_lock(&uGPIOIntData.uGPIOIntLock);
  nStatus = qurt_interrupt_enable((unsigned int) uGPIOIntData.state[UGPIOINT_IDX(gpio)].qurt_intr_id);
  if ( nStatus == QURT_EOK && uGPIOIntData.state[UGPIOINT_IDX(gpio)].nAltIdx < MAX_NUMBER_OF_UGPIOS )
  {
    nStatus = qurt_interrupt_enable((unsigned int) uGPIOIntData.state[UGPIOINT_ALT_IDX(gpio)].qurt_intr_id);
  }
  qurt_mutex_unlock(&uGPIOIntData.uGPIOIntLock); 

  if (nStatus != QURT_EOK)
  {
    return UGPIOINT_ERROR;
  }

  return UGPIOINT_SUCCESS;
}

/*=============================================================================
  FUNCTION      uGPIOInt_DisableInterrupt
=============================================================================*/
int32 
uGPIOInt_DisableInterrupt
(
  
  uint32 gpio
)
{
  unsigned int nStatus;
  if ((!VALID_UGPIO(gpio))||(!VALID_UGPIO_IDX(gpio)))
  {
    return UGPIOINT_ERROR;
  }
  qurt_mutex_lock(&uGPIOIntData.uGPIOIntLock);
  nStatus = qurt_interrupt_disable((unsigned int) uGPIOIntData.state[UGPIOINT_IDX(gpio)].qurt_intr_id); 
  if ( nStatus == QURT_EOK && uGPIOIntData.state[UGPIOINT_IDX(gpio)].nAltIdx < MAX_NUMBER_OF_UGPIOS )
  {
    nStatus = qurt_interrupt_disable((unsigned int) uGPIOIntData.state[UGPIOINT_ALT_IDX(gpio)].qurt_intr_id);
  }
  qurt_mutex_unlock(&uGPIOIntData.uGPIOIntLock); 

  if (nStatus != QURT_EOK)
  {
    return UGPIOINT_ERROR;
  }

  return UGPIOINT_SUCCESS;
}

/*=============================================================================
  FUNCTION      uGPIOInt_InterruptDone
=============================================================================*/
int32 
uGPIOInt_InterruptDone
(
  uint32 gpio
)
{
  unsigned int nStatus = QURT_EFAILED;
  if ((!VALID_UGPIO(gpio))||(!VALID_UGPIO_IDX(gpio)))
  {
    return UGPIOINT_ERROR;
  }
  qurt_mutex_lock(&uGPIOIntData.uGPIOIntLock);
  if ( uGPIOIntData.state[UGPIOINT_IDX(gpio)].gpio_intr_flags & UGPIOINTF_DSR )
  {
    nStatus = qurt_interrupt_acknowledge((unsigned int) uGPIOIntData.state[UGPIOINT_IDX(gpio)].qurt_intr_id); 
    if ( nStatus == QURT_EOK && uGPIOIntData.state[UGPIOINT_IDX(gpio)].nAltIdx < MAX_NUMBER_OF_UGPIOS )
    {
      nStatus = qurt_interrupt_acknowledge((unsigned int) uGPIOIntData.state[UGPIOINT_ALT_IDX(gpio)].qurt_intr_id);
    }
  }
  qurt_mutex_unlock(&uGPIOIntData.uGPIOIntLock); 

  if (nStatus != QURT_EOK)
  {
    return UGPIOINT_ERROR;
  }

  return UGPIOINT_SUCCESS;
}


/*=============================================================================
  FUNCTION      uGPIOInt_TimestampRead
=============================================================================*/
int32 
uGPIOInt_TimestampRead
(
  uint32                    gpio,
  uGPIOIntTimestampStatus*  pnStatus,
  uint64*                   pnTimestamp
)
{
  #ifdef UGPIOTIMESTAMP_EN
  pdcTimestamp_unit ts = {0, FALSE, FALSE};
  int32 nRet = UGPIOINT_ERROR;
  pdcTimestamp_status status;
   

  if ((!VALID_UGPIO(gpio)) ||
      (!VALID_UGPIO_IDX(gpio)) ||
      pnStatus == NULL ||
      pnTimestamp == NULL )
  {
    return UGPIOINT_ERROR;
  }

  qurt_mutex_lock(&uGPIOIntData.uGPIOIntLock);
  
  if(qurt_island_get_status())
    status = uPdcTimestamp_getUnitData(&ts, uGPIOIntData.state[UGPIOINT_IDX(gpio)].qurt_intr_id);
  else
    status = pdcTimestamp_getUnitData(&ts, uGPIOIntData.state[UGPIOINT_IDX(gpio)].qurt_intr_id);

  if( PDC_TS_NOT_IN_ISLAND == status ) 
  {
    qurt_mutex_unlock(&uGPIOIntData.uGPIOIntLock); 
    return UGPIOINT_ERROR;
  }

  if( PDC_TS_SUCCESS == status ) {
    nRet = UGPIOINT_SUCCESS;

    *pnTimestamp = ts.timeStamp;

    if ( ts.valid == TRUE && ts.overflow == FALSE )
    {
      *pnStatus = UGPIOINT_TSS_VALID;
    }
    else if ( ts.valid == TRUE && ts.overflow == TRUE )
    {
      *pnStatus = UGPIOINT_TSS_VALID_OVERFLOW;
    }
    else if ( ts.valid == FALSE && ts.overflow == TRUE )
    {
      *pnStatus = UGPIOINT_TSS_INVALID_OVERFLOW;
    }
    else
    {
      *pnStatus = UGPIOINT_TSS_INVALID;
    }
  }

  qurt_mutex_unlock(&uGPIOIntData.uGPIOIntLock); 

  return nRet;
  #else /*UGPIOTIMESTAMP_EN*/
  return UGPIOINT_ERROR;
  #endif /*UGPIOTIMESTAMP_EN*/
}
