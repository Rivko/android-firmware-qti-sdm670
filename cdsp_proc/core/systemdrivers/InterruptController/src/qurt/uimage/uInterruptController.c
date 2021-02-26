/*==============================================================================

FILE:      uInterruptController.c

DESCRIPTION
  This modules implements the API to utilize the Micro interrupt controller.
  This Micro Interrupt Controller module is strictly used in Kernel Protection
  Domain and in Micro image.The definitions in this module are implemented internally
  and should not be exposed for external client usage directly without using the
  accompanying uInterruptController.h in the core/api/systemdrivers interface file for this module.

REFERENCES

       Copyright (c) 2014-2015 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/InterruptController/src/qurt/uimage/uInterruptController.c#1 $

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

/*------------------------------------------------------------------------------
 uInterrupt Data Declarations.
------------------------------------------------------------------------------*/ 

/*
 * This is static uInterrupt state data. It can be accessed for debugging 
 * uInterrupts to see what is the current registration state of the uInterrupt.
 */
extern uIntCntrlType uIntData;

/*------------------------------------------------------------------------------
 uInterrupt Macro Declarations.
------------------------------------------------------------------------------*/ 
#define UINT_PRIORITY 4

#define VALID_UINT(x) ((x) < MAX_NUMBER_OF_INTS)
#define VALID_UINT_IDX(x) (uIntData.auIntLUT[x] != UINT_NONE)
#define UINT_IDX(x) (uIntData.auIntLUT[x])

/* 
 * For each interrupt handler thread a Default IST stack size is declared.
 */
#define UINT_IST_STACK_SIZE  1792

/*------------------------------------------------------------------------------
Following functions are internal functions.
------------------------------------------------------------------------------*/

extern void uInterruptController_ISTMain(void * ISTParam);
static int32 uInterruptController_ConfigureIST(void);

/*==========================================================================

  FUNCTION      uInterruptController_AssignInterruptIndex

  DESCRIPTION   See uInterruptControllerInternal.h

==========================================================================*/

int32
uInterruptController_AssignInterruptIndex(uint32 nInterruptID)
{
  uint32 index;

  if ( !VALID_UINT_IDX(nInterruptID) ) 
  {
    for(index =0;index < MAX_NUMBER_OF_UINTS; index++)
    {
      if (uIntData.state[index].qurt_intr_id == UINT_NONE) 
      {
        uIntData.state[index].qurt_intr_id = (uint8)nInterruptID;
        uIntData.auIntLUT[nInterruptID] = (uint8)index;
        break;
      }   
    }
    if (index >= MAX_NUMBER_OF_UINTS) 
    {
      return UINTERRUPT_ERROR;
    }
    else
    {
      return UINTERRUPT_SUCCESS;
    }
  }
  else
  {
    return UINTERRUPT_SUCCESS;
  }
 
}

/*==========================================================================

  FUNCTION      uInterrupt_EnableInterrupt

  DESCRIPTION   See uInterruptController.h

==========================================================================*/

int32
uInterruptController_EnableInterrupt
(
  uint32 nInterruptID
)
{
 
  return UINTERRUPT_ERROR;

} /* END uInterruptController_EnableInterrupt */

/*==========================================================================

   FUNCTION      uInterrupt_ClearInterrupt

   DESCRIPTION   This function is used internally to clear a micro
                 interrupt.

==========================================================================*/

int32
uInterruptController_ClearInterrupt
(
  uint32 nInterruptID
)
{
 
  return UINTERRUPT_ERROR;

} /* END uInterruptController_ClearInterrupt */


/*==========================================================================

   FUNCTION      uInterruptController_Init

   DESCRIPTION   See uInterruptController.h

==========================================================================*/
int32 uInterruptController_Init(void)
{
  uint32 index;
  uint32 BaseOffset;
  BaseOffset = 0x11;
  int32 nResult;

  /*
   * Initialize the uInterrupt state table.
   * The interrupt configuration is fixed for the 10 configurations possible in hw for 10 direct connects.
   */
  qurt_mutex_init(&uIntData.uIntLock);

  qurt_mutex_lock(&uIntData.uIntLock);
  for(index =0;index < MAX_NUMBER_OF_UINTS; index++)
  {
    uIntData.state[index].qurt_intr_id = UINT_NONE;
    uIntData.state[index].nInterruptMask = (1<< index);
    uIntData.nIntMask |= uIntData.state[index].nInterruptMask;
    uIntData.state[index].nIntRegMask = 1<< (index + BaseOffset);
  }
      
  /*
   * Initialize the uInterrupt LUT table. this the full size of 200 Interrupts
   */
  for(index =0;index < MAX_NUMBER_OF_INTS; index++)
  {
    uIntData.auIntLUT[index] = UINT_NONE;
  }
 
  uIntData.nThreadID = 0;
  uIntData.nIntRegistrationMask = UINT_TASK_INTMASK;

  /*
   * Spawn the IST here in init so it can wait for registration commands when the client registers.
   */
  
  nResult =  uInterruptController_ConfigureIST();
  if(nResult == UINTERRUPT_SUCCESS)
  {
    uIntData.uInt_Init = 1;
  }
  qurt_mutex_unlock(&uIntData.uIntLock); 
  return nResult;

} /* END uInterruptController_Init */


/*===========================================================================

  FUNCTION      uInterruptController_ConfigureIST

  DESCRIPTION   This function spawns a single IST thread for handling
                each Micro interrupt.

  PARAMETERS
  Parameters : None. 

  DEPENDENCIES  None.

  RETURN VALUE  DALResult
  UINTERRUPT_SUCCESS : If the IST was spawned successfully.
  UINTERRUPT_ERROR : If the IST Spawning encountered an error.

  SIDE EFFECTS  None.


==========================================================================*/
static int32 uInterruptController_ConfigureIST(void)
{
  int nResult;
  unsigned int StackSize;
  unsigned char * StackAddr;
 
  nResult = 0;

  /*
   * Set the IST name to UINTERRUPT_IST
   */
  snprintf(uIntData.aISTName, 
          (sizeof(unsigned char)*UIST_TASKNAME_SIZE), "UINTERRUPT_IST");

  /*
   *  Initialize any thread attribute objects needed by the interrupt service thread.
   */
  qurt_thread_attr_init (&uIntData.ThreadAttr);

  /*
   *  Initialize any thread attribute and signal objects needed by
   *  the interrupt service thread.
   */
  qurt_anysignal_init(&uIntData.ISTSignal);

  /*
   * 64 bit alignment of the stack ptr.
   */
  StackAddr = (unsigned char *)
              (((unsigned long)uIntData.aISTStack -1) & (~0x7)) + 0x8;

  StackSize = (unsigned int)( UINT_IST_STACK_SIZE - 
              ((unsigned long)StackAddr - 
              (unsigned long)uIntData.aISTStack) );

  /* 
   * To ensure that the entire stack (start to end) is 64-bit aligned
   * so the last odd bytes are dropped off.
   */
  StackSize &= ~(0x7);

  
  qurt_thread_attr_set_stack_size(&uIntData.ThreadAttr, 
                                  StackSize);
  qurt_thread_attr_set_stack_addr (&uIntData.ThreadAttr, 
                                   (void *)StackAddr);
  qurt_thread_attr_set_priority (&uIntData.ThreadAttr, 
                                (unsigned short)UINT_PRIORITY);

  qurt_thread_attr_set_tcb_partition(&uIntData.ThreadAttr,1); // This task should reside in TCM Memory for UInterrupt Delivery.

  qurt_thread_attr_set_name(&uIntData.ThreadAttr, 
                            uIntData.aISTName);
   
  nResult = qurt_thread_create((qurt_thread_t *)&uIntData.nThreadID, 
                               &uIntData.ThreadAttr,
                               uInterruptController_ISTMain,NULL);
   
  if (nResult == QURT_EFATAL) 
  {
    return UINTERRUPT_ERROR;
  }

  return UINTERRUPT_SUCCESS;

} /* END uInterruptController_ConfigureIST */


/*==========================================================================

   FUNCTION      uInterruptController_RegisterInterrupt

   DESCRIPTION   See uInterruptController.h

==========================================================================*/
int32
uInterruptController_RegisterInterrupt( uint32 nInterruptID,uint32 trigger,
                            uIRQ isr,uIRQCtx param, uint32 nFlags, uint16 nAonID)
{
  int nStatus;
  nStatus = 0;
  if (!VALID_UINT(nInterruptID))
  {
    return UINTERRUPT_ERROR;
  }
  if (trigger > UINTERRUPTCONTROLLER_FALLING_EDGE_TRIGGER)
  {
    return UINTERRUPT_ERROR;
  }

  qurt_mutex_lock(&uIntData.uIntLock);
  
  if (uInterruptController_AssignInterruptIndex(nInterruptID) == UINTERRUPT_ERROR) 
  {
    qurt_mutex_unlock(&uIntData.uIntLock); 
    return UINTERRUPT_ERROR;
  }
  if((uIntData.state[UINT_IDX(nInterruptID)].isr != NULL) &&
     (uIntData.state[UINT_IDX(nInterruptID)].isr != isr))
  {
    /*
     * Restore interrupts and return an error.
     */
    (void)qurt_mutex_unlock(&uIntData.uIntLock); 
    return UINTERRUPT_ERROR;
  }
  uIntData.state[UINT_IDX(nInterruptID)].isr = isr;
  uIntData.state[UINT_IDX(nInterruptID)].isr_param = param; 

  /*
   * Update static Interrupt map with Trigger 
   */ 
  uIntData.state[UINT_IDX(nInterruptID)].intr_trigger = (uint8)trigger;
  uIntData.state[UINT_IDX(nInterruptID)].intr_flags |= UINTF_REGISTERED;

  /* 
   * An IST thread is already created.
   * It will only be restarted on a re registration.
   */
  qurt_anysignal_set(&uIntData.ISTSignal,
                       uIntData.state[UINT_IDX(nInterruptID)].nIntRegMask);
  
  qurt_mutex_unlock(&uIntData.uIntLock); 
  if (QURT_EOK != nStatus)
  {
    return UINTERRUPT_ERROR;
  }

  return UINTERRUPT_SUCCESS;

} /* END uInterruptController_RegisterInterrupt */


/*==========================================================================

   FUNCTION      uInterruptController_DeregisterInterrupt

   DESCRIPTION   See uInterruptController.h

==========================================================================*/
int32
uInterruptController_UnRegister(uint32 nInterruptID)
{
  int nStatus;
  /*
   * if the Interrupt is not configured by UInterrupt module then we will
   * not have any valid index etc. Hence its better to abandon
   * any invalid accesses from  this point.
   */
  if ((!VALID_UINT(nInterruptID))||(!VALID_UINT_IDX(nInterruptID)))
  {
    return UINTERRUPT_ERROR;
  }

  qurt_mutex_lock(&uIntData.uIntLock);
  
  /*
   * Clear out the handler and remove the event.
   */
  uIntData.state[UINT_IDX(nInterruptID)].isr = NULL;
  uIntData.state[UINT_IDX(nInterruptID)].isr_param = 0;
  uIntData.state[UINT_IDX(nInterruptID)].intr_trigger = 0;
  
  /* 
   * Deregister IST from QURT so it can stop waiting for an interrupt.
   */
  nStatus = qurt_interrupt_deregister(uIntData.state[UINT_IDX(nInterruptID)].qurt_intr_id); 

  if (QURT_EOK != nStatus) 
  { 
    qurt_mutex_unlock(&uIntData.uIntLock); 
    return UINTERRUPT_ERROR;
  } 

  uIntData.state[UINT_IDX(nInterruptID)].intr_flags &= ~UINTF_REGISTERED;
  uIntData.state[UINT_IDX(nInterruptID)].qurt_intr_id = UINT_NONE; 
  uIntData.auIntLUT[nInterruptID] = UINT_NONE;
  
  /*
   * Restore interrupts
   */
  qurt_mutex_unlock(&uIntData.uIntLock); 

  return UINTERRUPT_SUCCESS;

} /* END uInterruptController_DeregisterInterrupt */


