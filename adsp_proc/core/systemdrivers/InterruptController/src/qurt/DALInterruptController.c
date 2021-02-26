/*==============================================================================

FILE:      DALInterruptController.c

DESCRIPTION: This file implements a Interrupt Controller for QDSP6 L2VIC harware 
interrupt controller

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2011 - 2017  QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/systemdrivers/InterruptController/src/qurt/DALInterruptController.c#2 $ 

when       who     what, where, why
--------   ---     ---------------------------------------------------------
08/22/2014 cpaulo  Initial version of DALInterruptController with SSCAON

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <DALInterruptController.h>
#include <DALSys.h>
#include <DALSysCmn.h>
#include "DALInterruptControllerConfig.h"
#include "DDITimetick.h"
#include "DALDeviceId.h"
#include <atomic_ops.h>
#include <qurt.h>
#include "qurt_int.h"
#include "qurt_anysignal.h"
#include "stringl.h"
#include "ULogFront.h"
#include "InterruptControllerInterface.h"
#include "uInterruptController.h"
#include "uInterruptControllerInternal.h"
#include "pdcInt.h"

#ifdef TIMETEST
#include <timetest.h>
#endif

#include "InterruptControllerLog.h"
#define INTERRUPT_THREAD_MEDIAN_PRIORITY  4
#define INTERRUPT_SIGNAL_MASK             0x1
#define INTERRUPT_TASK_START              0x00008000

/*=========================================================================

                           TYPE DEFINITIONS

===========================================================================*/

/*=========================================================================

                           DATA DEFINITIONS

===========================================================================*/

/*
 * This is the internal static data variable used for the functioning of the 
 * Interrupt controller.
 */
static InterruptDataType *pInterruptData;

/*=========================================================================

                           MACRO DEFINITIONS

===========================================================================*/

/*
 * INTERRUPT_TIMETEST
 *
 * Macro to define a timetest ID for an interrupt.
 */
#define TIMETEST_INTERRUPT_ISR_FLAG  0x1000
#define INTERRUPT_TIMETESTID(irq)      (TIMETEST_INTERRUPT_ISR_FLAG | irq)

#define INTERRUPTCONTROLLER_SET_PRIORITY(x)  \
((IST_THREAD_PRIORITY_BASE - x) >= IST_THREAD_HIGHEST_PRIORITY ?  \
(IST_THREAD_PRIORITY_BASE - x) : IST_THREAD_PRIORITY_BASE)

/*
 * Macro to set the IST stack
 *
 * The user defined flag for now should be no greater than 3.
 * the stack is increased in multiples of 2048 bytes.
 */
#define INTERRUPTCONTROLLER_SET_STACK(x)  \
((x) <= 3 ?  \
(DAL_IST_BASE_STACK_SIZE*x) : DAL_IST_BASE_STACK_SIZE)

/*==========================================================================

                         FUNCTION  DECLARATIONS

==========================================================================*/

static DALResult GetPlatformData(InterruptControllerClientCtxt * pclientCtxt);
static DALResult SetWakeupInterruptTrigger(uint32 IntID,uint32 nTrigger, boolean bEnable);
static void ClearInterruptFlag(uint32 nInterrupt,uint16 nFlag);
static void SetInterruptFlag(uint32 nInterrupt,uint16 nFlag);
static boolean GetInterruptFlag(uint32 nInterrupt,uint16 nFlag);
static void LogInterruptEvent(InterruptControllerClientCtxt  *pclientCtxt,
InterruptLogEventType eEvent,uint32 nInterrupt);
static void TriggerClientEvent(uint32 nInterruptVector,
InterruptControllerClientCtxt *pClientCtxt);
static DALResult SetTrigger(InterruptControllerDevCtxt *pDevice,
uint32 nInterruptVector,uint32 nTrigger, InterruptControllerClientCtxt * pclientCtxt);
static DALResult SetQURTTriggerConfig(InterruptControllerDevCtxt *pDevice,
uint32 nInterruptVector,uint32 nTrigger);
static void ISTMain(void* nParam);
static DALResult ConfigureDynamicIST( uint32 nInterruptVector, uint32 nPriority, char* pISTName, InterruptControllerClientCtxt * pclientCtxt);
static DALResult InterruptController_RegisterInterrupt(InterruptControllerClientCtxt * pclientCtxt, DALInterruptID  nInterruptVector, 
                                                        const DALISR isr,const DALISRCtx ctx, uint32  IntrFlags,char* pISTName);
/*==========================================================================

                         FUNCTION  DEFINITIONS

===========================================================================

  FUNCTION      ISTMain

  DESCRIPTION   This is the main Interrupt service thread function that 
  processes incoming interrupts from QURT.

  PARAMETERS   Arg this the actual InterruptVector passed in.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.


==========================================================================*/
static void ISTMain
(
  void* nParam
)
{
  InterruptControllerDevCtxt *pDevice; 
  int nStatus;
  InterruptArgsType* pArgs;
  uint32 nInterruptVector = (uint32)nParam;
  uint32 nTrigger;

  pDevice = pInterruptData->pClientCtxt->pInterruptControllerDevCtxt; 

  pInterruptData->pInterruptState[nInterruptVector].nThreadID = qurt_thread_get_id();

  /*
   * Main loop.  Process an interrupt IPC, then wait for another.
   */
  while(1)
  {

   /*
    * check if interrupt registration flag is set and only register if its still set.
    */
    DALSYS_SyncEnter(pInterruptData->pClientCtxt->hSyncIntrCtrlTbl); 
    if(!GetInterruptFlag(nInterruptVector, INTERRUPTF_REGISTERED))
    {
      DALSYS_SyncLeave(pInterruptData->pClientCtxt->hSyncIntrCtrlTbl);
      goto WaitForTaskStart;
    }

    pArgs = InterruptControllerSearchArg( nInterruptVector, &pInterruptData->pArgsList );

    if ( pArgs == NULL )
    {
      DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
        "ISTMain: Interrupt service thread cannot find arguments in list for interrupt %d", nInterruptVector);
      DALSYS_SyncLeave(pInterruptData->pClientCtxt->hSyncIntrCtrlTbl);
      goto WaitForTaskStart;
    }

   /*
    * Register with QURT using the interrupt vector
    */
   nStatus = qurt_interrupt_register(nInterruptVector,&pInterruptData->pInterruptState[nInterruptVector].ISTSignal, 
                                     INTERRUPT_SIGNAL_MASK);
   if (QURT_EOK != nStatus)
   {
     DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
      "ISTMain: failed to register interrupt %d, going to wait state", nInterruptVector);
     qurt_anysignal_set( &pArgs->pRetSig, INTERRUPT_REGISTER_FAIL );
     pArgs = NULL;
     DALSYS_SyncLeave(pInterruptData->pClientCtxt->hSyncIntrCtrlTbl);
     goto WaitForTaskStart;
   }

   qurt_anysignal_set( &pArgs->pRetSig, INTERRUPT_REGISTER_SUCCESS ); 
   pArgs = NULL;
   IntLog_Out(INTLOG_REG, pInterruptData->hULog, 1, "Register: Interrupt %d registered with kernel", nInterruptVector );
   DALSYS_SyncLeave(pInterruptData->pClientCtxt->hSyncIntrCtrlTbl);

   while (1)
   {
      unsigned int interruptSignalValue;

      interruptSignalValue = qurt_anysignal_wait( &pInterruptData->pInterruptState[nInterruptVector].ISTSignal, 
                                                  INTERRUPT_SIGNAL_MASK | SIG_INT_ABORT );

      if ( SIG_INT_ABORT & interruptSignalValue )
      {
        DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
           "ISTMain: SIG_INT_ABORT signal received from IST for interrupt %d!", nInterruptVector); 
        break;
      }
      
      if ( !(INTERRUPT_SIGNAL_MASK & interruptSignalValue) )
      {
        DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
          "ISTMain: invalid interrupt signal in IST for interrupt %d", nInterruptVector);
        break;
      }

      DALSYS_SyncEnter(pInterruptData->pClientCtxt->hSyncIntrCtrlTbl); 
      if (GetInterruptFlag(nInterruptVector, INTERRUPTF_RUNNING))
      {
        SetInterruptFlag(nInterruptVector, INTERRUPTF_SUSPENDED);
        DALSYS_SyncLeave(pInterruptData->pClientCtxt->hSyncIntrCtrlTbl);
        LogInterruptEvent(pInterruptData->pClientCtxt,INTERRUPT_LOG_EVENT_SUSPENDED, 
        nInterruptVector);
        DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
          "ISTMain: the interrupt is already being processed from the QURT Kernel for interrupt %d", nInterruptVector);
        break;
      }
      DALSYS_SyncLeave(pInterruptData->pClientCtxt->hSyncIntrCtrlTbl);
  
      /*
       * Log the start of the ISR.
       */
      LogInterruptEvent(pInterruptData->pClientCtxt,INTERRUPT_LOG_EVENT_ISR_START, 
            nInterruptVector);

      /*
       * As soon as we get an interrupt we lock interrupts so as to 
       * ensure no new interrupt comes in.  Lock will be released right before 
       * triggering the client event.
       */
      DALSYS_SyncEnter(pInterruptData->pClientCtxt->hSyncIntrCtrlTbl); 

      /*
       * Clear the interrupt before calling the ISR if edge triggered
       */
      nTrigger = pInterruptData->pInterruptState[nInterruptVector].nTrigger;
      if ( nTrigger != DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER &&
           nTrigger != DALINTRCTRL_ENABLE_LEVEL_LOW_TRIGGER )
      {
        /*
         * Clear signal and reactivate interrupt
         */
        qurt_anysignal_clear(&pInterruptData->pInterruptState[nInterruptVector].ISTSignal, INTERRUPT_SIGNAL_MASK );
        nStatus = qurt_interrupt_acknowledge(nInterruptVector);
      }

      /*
       * log the interrupt and trigger the registered event.
       */
      TriggerClientEvent(nInterruptVector, pInterruptData->pClientCtxt);
  
      /*
       * Clear the interrupt after calling the ISR if level triggered
       */
      nTrigger = pInterruptData->pInterruptState[nInterruptVector].nTrigger;
      if ( nTrigger == DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER ||
           nTrigger == DALINTRCTRL_ENABLE_LEVEL_LOW_TRIGGER )
      {
        /*
         * Clear signal and reactivate interrupt
         */
        qurt_anysignal_clear(&pInterruptData->pInterruptState[nInterruptVector].ISTSignal, INTERRUPT_SIGNAL_MASK );
        nStatus = qurt_interrupt_acknowledge(nInterruptVector);
      }

      /*
       * Release the synchronization object before waiting for next interrupt.
       */
      DALSYS_SyncLeave(pInterruptData->pClientCtxt->hSyncIntrCtrlTbl);
  
      /*
       * Log the entry
       */
      LogInterruptEvent(pInterruptData->pClientCtxt,INTERRUPT_LOG_EVENT_ISR_FINISH, 
        nInterruptVector);
  
      if (QURT_EOK != nStatus)
      {
        // unable to acknowledge interrupt
        // possibly because the callback function has de-register from this
        // interrupt.
        DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
           "ISTMain: failed to ACK interrupt %d", nInterruptVector); 
        break;
      }
    }

    WaitForTaskStart:

    /*
     * Wait for the next time this interrupt is registered with QURT.
     */

    if(INTERRUPT_TASK_START & qurt_anysignal_wait(&pInterruptData->pInterruptState[nInterruptVector].ISTSignal, 
                                                  INTERRUPT_TASK_START))
    {
      qurt_anysignal_clear(&pInterruptData->pInterruptState[nInterruptVector].ISTSignal, INTERRUPT_TASK_START);    
    }  

    /*
     * clear signals before re-registering.
     */
    qurt_anysignal_clear(&pInterruptData->pInterruptState[nInterruptVector].ISTSignal, 
                         INTERRUPT_SIGNAL_MASK | SIG_INT_ABORT );
  }
} /* END ISTMain */


/*===========================================================================

  FUNCTION      ConfigureDynamicIST

  DESCRIPTION   This function spawns a single IST thread for handling each interrupts.

  PARAMETERS   
  nInterruptVector : the actual interrupt number on the L2VIC.
  nPriority : The thread is spawned with the priority of the interrupt thread.

  DEPENDENCIES  Must be called within a mutex.

  RETURN VALUE  None.

  SIDE EFFECTS  None.


==========================================================================*/
static DALResult ConfigureDynamicIST
(
  uint32 nInterruptVector, 
  uint32 nPriority,
  char* pISTName,
  InterruptControllerClientCtxt * pclientCtxt
)
{
  int nResult;
  unsigned int StackSize;
  unsigned char * StackAddr;
  uint32 nRetSig;
  InterruptArgsType* pArgs;

  if(pInterruptData->pInterruptState[nInterruptVector].pISTStack != NULL)
  {

    /* 
     * An IST thread is already created.
     * It will only be restarted on a re registration.
     */

    IntLog_Out(INTLOG_REG, pInterruptData->hULog, 1, "Register: Interrupt %d, task previously created, sending startup signal", nInterruptVector );

    if ( ( pArgs = InterruptControllerPopArg( &pInterruptData->pEmptyList ) ) == NULL )
    {
      if (DALSYS_Malloc(sizeof(InterruptArgsType), (void **)&pArgs) == DAL_ERROR) 
      {
        return DAL_ERROR;
      }

      DALSYS_memset(pArgs, 0, sizeof(InterruptArgsType));

      qurt_anysignal_init( &pArgs->pRetSig );
    }

    pArgs->nInterruptVector = nInterruptVector;

    qurt_anysignal_clear( &pArgs->pRetSig, INTERRUPT_REGISTER_SUCCESS | INTERRUPT_REGISTER_FAIL );

    InterruptControllerPushArg( pArgs, &pInterruptData->pArgsList );

    qurt_anysignal_set( &pInterruptData->pInterruptState[nInterruptVector].ISTSignal,
                        INTERRUPT_TASK_START);

    DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);

    nRetSig = qurt_anysignal_wait( &pArgs->pRetSig, 
                                   INTERRUPT_REGISTER_SUCCESS | INTERRUPT_REGISTER_FAIL );

    DALSYS_SyncEnter(pclientCtxt->hSyncIntrCtrlTbl);

    InterruptControllerPushArg( pArgs, &pInterruptData->pEmptyList );

    if ( nRetSig | INTERRUPT_REGISTER_SUCCESS )
    {
      return DAL_SUCCESS;
    }
    else
    {
      return DAL_ERROR;
    }
  }

  if(DALSYS_Malloc(
  (sizeof(unsigned char))*(pInterruptData->pInterruptState[nInterruptVector].nISTStackSize),
  (void **)&pInterruptData->pInterruptState[nInterruptVector].pISTStack) == DAL_ERROR)
  {
    return DAL_ERROR;
  }

  /*
   * Allways initialize malloced data to 0.
   */
  DALSYS_memset(pInterruptData->pInterruptState[nInterruptVector].pISTStack, 0,
  ((sizeof(unsigned char))* (pInterruptData->pInterruptState[nInterruptVector].nISTStackSize)));

  /*
   * Allways initialize malloced data to 0.
   */
  DALSYS_memset(pInterruptData->pInterruptState[nInterruptVector].aISTName, 0, 
      (sizeof(char))*IST_TASKNAME_SIZE);

  /*
   * Set the IST name to IST<InterruptVector>
   */
  snprintf(pInterruptData->pInterruptState[nInterruptVector].aISTName, 
          (sizeof(unsigned char)*IST_TASKNAME_SIZE), "IST%d",(int)nInterruptVector);

  if(pISTName !=NULL)
  {
    strlcat((char *)pInterruptData->pInterruptState[nInterruptVector].aISTName,pISTName,IST_TASKNAME_SIZE);
  }

  IntLog_Out(INTLOG_REG, pInterruptData->hULog, 1, "Register: Interrupt %d, task created", nInterruptVector );

  /*
   *  Initialize any thread attribute and signal objects needed by the interrupt service thread.
   */
  qurt_anysignal_init(&pInterruptData->pInterruptState[nInterruptVector].ISTSignal);
  qurt_thread_attr_init (&pInterruptData->pInterruptState[nInterruptVector].ThreadAttr);

  #ifdef TIMETEST
  qurt_thread_attr_set_timetest_id(&pInterruptData->pInterruptState[nInterruptVector].ThreadAttr, 
                                  (unsigned short)INTERRUPT_TIMETESTID(nInterruptVector));
  #endif

  /*
   * 64 bit alignment of the stack ptr.
   */
  StackAddr = (unsigned char *)
              (((unsigned long)pInterruptData->pInterruptState[nInterruptVector].pISTStack -1) & (~0x7)) + 0x8;

  StackSize = (unsigned int)( pInterruptData->pInterruptState[nInterruptVector].nISTStackSize - 
              ((unsigned long)StackAddr - 
              (unsigned long)pInterruptData->pInterruptState[nInterruptVector].pISTStack) );

  /* 
   * To ensure that the entire stack (start to end) is 64-bit aligned
   * so the last odd bytes are dropped off.
   */
  StackSize &= ~(0x7);

  pInterruptData->pInterruptState[nInterruptVector].nInterruptVector = nInterruptVector;
  qurt_thread_attr_set_stack_size(&pInterruptData->pInterruptState[nInterruptVector].ThreadAttr, 
                                  StackSize);
  qurt_thread_attr_set_stack_addr (&pInterruptData->pInterruptState[nInterruptVector].ThreadAttr, 
                                   (void *)StackAddr);
  qurt_thread_attr_set_priority (&pInterruptData->pInterruptState[nInterruptVector].ThreadAttr, 
                                (unsigned short)nPriority);
  qurt_thread_attr_set_name(&pInterruptData->pInterruptState[nInterruptVector].ThreadAttr, 
                            pInterruptData->pInterruptState[nInterruptVector].aISTName);
   
  nResult = qurt_thread_create((qurt_thread_t *)&pInterruptData->pInterruptState[nInterruptVector].nThreadID, 
                               &pInterruptData->pInterruptState[nInterruptVector].ThreadAttr,
                               ISTMain,(void *)pInterruptData->pInterruptState[nInterruptVector].nInterruptVector);
   
  if (nResult == QURT_EFATAL) 
  {
    qurt_anysignal_destroy(&pInterruptData->pInterruptState[nInterruptVector].ISTSignal);
    DALSYS_Free((void *)pInterruptData->pInterruptState[nInterruptVector].pISTStack);
    return DAL_ERROR;
  }

  if ( ( pArgs = InterruptControllerPopArg( &pInterruptData->pEmptyList ) ) == NULL )
  {
    if (DALSYS_Malloc(sizeof(InterruptArgsType), (void **)&pArgs) == DAL_ERROR) 
    {
      return DAL_ERROR;
    }

    DALSYS_memset(pArgs, 0, sizeof(InterruptArgsType));

    qurt_anysignal_init( &pArgs->pRetSig );
  }

  pArgs->nInterruptVector = nInterruptVector;

  qurt_anysignal_clear( &pArgs->pRetSig, INTERRUPT_REGISTER_SUCCESS | INTERRUPT_REGISTER_FAIL );

  InterruptControllerPushArg( pArgs, &pInterruptData->pArgsList );

  qurt_anysignal_set( &pInterruptData->pInterruptState[nInterruptVector].ISTSignal, 
                      INTERRUPT_TASK_START);

  DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);

  nRetSig = qurt_anysignal_wait( &pArgs->pRetSig, 
                                 INTERRUPT_REGISTER_SUCCESS | INTERRUPT_REGISTER_FAIL );

  DALSYS_SyncEnter(pclientCtxt->hSyncIntrCtrlTbl);

  InterruptControllerPushArg( pArgs, &pInterruptData->pEmptyList );

  if ( nRetSig | INTERRUPT_REGISTER_SUCCESS )
  {
    return DAL_SUCCESS;
  }
  else
  {
    return DAL_ERROR;
  }
} /* END ConfigureDynamicIST */


/*------------------------------------------------------------------------------
Following functions are for DALDriver specific functionality
------------------------------------------------------------------------------*/
DALResult InterruptController_DriverInit( InterruptControllerDrvCtxt *pclientCtxt )
{
  return DAL_SUCCESS;

}

DALResult InterruptController_DriverDeInit( InterruptControllerDrvCtxt *pclientCtxt )
{
  return DAL_SUCCESS;

}

/*=============================================================================

  Following functions are declared in IDevice Interface. 

===============================================================================

  FUNCTION      InterruptController_DeviceInit

  DESCRIPTION   This function is called when a driver tries to attach from the 
  DALInterruptController.The purpose of this function is to initialize the 
  device specific data for the interrupt controller.

  PARAMETERS    InterruptControllerClientCtxt * Pointer to the indivisual client 
  context.

  DEPENDENCIES  DALSYS should be initialized.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/
DALResult InterruptController_DeviceInit
( 
  InterruptControllerClientCtxt *pclientCtxt 
)
{

  if(DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
    &pclientCtxt->hSyncIntrCtrlTbl, NULL))
  {
    return DAL_ERROR;
  }

  if(GetPlatformData(pclientCtxt) == DAL_ERROR)
  {
    return DAL_ERROR;
  }

  /*
   * Acquire system timer.
   */
  if(DalTimetick_Attach("SystemTimer", &pInterruptData->pTimetickHandle) != DAL_SUCCESS)
  {
    return DAL_ERROR;
  }

  /*
   * Initialize uLogs
   */
  IntLog_Init( &pInterruptData->hULog );


  return DAL_SUCCESS;

} /* END InterruptController_DeviceInit */


/*==========================================================================

  FUNCTION      InterruptController_DeviceDeInit

  DESCRIPTION   This function is called when a driver tries to detach from 
  the DALInterruptController.The purpose of this function is to cleanup all 
  dynamically allocated data and reset the DALInterruptController state.

  PARAMETERS    InterruptControllerClientCtxt * Pointer to the indivisual 
  client context.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/
DALResult InterruptController_DeviceDeInit
( 
  InterruptControllerClientCtxt *pclientCtxt
)
{
  return DAL_SUCCESS;

} /* END InterruptController_DeviceDeInit */


/*=============================================================================

  Following functions are extended in IInterruptController Interface.

===============================================================================

  FUNCTION      InterruptController_RegisterEvent
  DESCRIPTION   See DDIInterruptController.h  This function for now is being 
  used on the Modem until all WCDMA issues are resolved.

=============================================================================*/
DALResult  InterruptController_RegisterEvent
(
  InterruptControllerClientCtxt * pclientCtxt,   
  DALInterruptID  nInterruptVector,
  const DALSYSEventHandle hEvent,
  uint32 IntrFlags
) 
{

  /* Event trigger api will be supported when we get a confirmation from WCDMA 
   * that they have resolved their interrupt usage issues.  
   */
  return DAL_ERROR;

} /* END InterruptController_RegisterEvent */


/*==========================================================================

  FUNCTION      InterruptController_RegisterISR
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult  InterruptController_RegisterISR
(
  InterruptControllerClientCtxt * pclientCtxt,   
  DALInterruptID  nInterruptVector, 
  const DALISR isr,
  const DALISRCtx ctx, 
  uint32  IntrFlags
) 
{
   return InterruptController_RegisterInterrupt(pclientCtxt, nInterruptVector, isr, ctx, 
                                       IntrFlags, NULL);

} /* END InterruptController_RegisterISR */


/*==========================================================================

  FUNCTION      InterruptController_RegisterIST
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult  InterruptController_RegisterIST
(
  InterruptControllerClientCtxt * pclientCtxt,   
  DALInterruptID  nInterruptVector, 
  const DALISR isr,
  const DALISRCtx ctx, 
  uint32  IntrFlags,
  char* pISTName
) 
{

  return InterruptController_RegisterInterrupt(pclientCtxt, nInterruptVector, isr, ctx, 
                                       IntrFlags, pISTName);

} /* END InterruptController_RegisterIST */


/*==========================================================================

  FUNCTION      InterruptController_RegisterInterrupt
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
static DALResult  InterruptController_RegisterInterrupt
(
  InterruptControllerClientCtxt * pclientCtxt,   
  DALInterruptID  nInterruptVector, 
  const DALISR isr,
  const DALISRCtx ctx, 
  uint32  IntrFlags,
  char* pISTName
) 
{
  uint32 nInterruptPriority,nTrigger,nStackSize, uInterrupt;
  InterruptControllerDevCtxt *pDevice;
  DALISR OldIsr;

  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  OldIsr = (DALISR)pInterruptData->pInterruptState[nInterruptVector].Isr;
  
  if((nInterruptVector == INVALID_INTERRUPT) ||(isr == NULL) ||(nInterruptVector >= pInterruptData->pPlatformConfig->nMaxIRQ))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
    "InterruptController_RegisterInterrupt: Could not Register Interrupt %d, invalid", nInterruptVector);
    return DAL_ERROR;
  }

  DALSYS_SyncEnter(pclientCtxt->hSyncIntrCtrlTbl);
  uInterrupt = (IntrFlags >> DAL_UINTERRUPT_CONTROLLER_SHFT) & DAL_UINTERRUPT_CONTROLLER_MASK;
  nTrigger = IntrFlags & DAL_INTERRUPT_TRIGGER_MASK;
  
  if ((nTrigger == DALINTRCTRL_ENABLE_DEFAULT_SETTINGS) ||
      (nTrigger >= DALINTRCTRL_INVALID_TRIGGER))
  {
    nTrigger = pInterruptData->pInterruptState[nInterruptVector].nTrigger;
  }
  else
  {
     pInterruptData->pInterruptState[nInterruptVector].nTrigger = nTrigger;
  }

  /*
   * Configure the interrupt as a wakeup source if necessary.
   */
  (void)SetWakeupInterruptTrigger(nInterruptVector,nTrigger,TRUE);  

  /*
   * Set the interrupt trigger before any further registration
   */
  if(SetQURTTriggerConfig(pclientCtxt->pInterruptControllerDevCtxt, nInterruptVector,nTrigger) == DAL_ERROR)
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
      "InterruptController_RegisterISR : Could not set Interrupt trigger");
  }

  DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);

  if (pInterruptData->uInterruptController != 0) 
  {
    if (uInterrupt != 0)
    {
      SetInterruptFlag(nInterruptVector, INTERRUPTF_UINTERRUPT);

      return uInterruptController_RegisterInterrupt(nInterruptVector, nTrigger,
               (uIRQ) isr,(uIRQCtx) ctx, 0, 0);

    }
  }

  nInterruptPriority = (IntrFlags >> DAL_INTERRUPT_PRIORITY_SHFT) & DAL_INTERRUPT_PRIORITY_MASK;
  
  nStackSize = (IntrFlags >> DAL_INTERRUPT_ISTSTACK_SHFT) & DAL_INTERRUPT_ISTSTACK_MASK;

  DALSYS_SyncEnter(pclientCtxt->hSyncIntrCtrlTbl);

  /*
   * In the new architecture the user has to specify the interrupt priority. 
   * If this is not given the interrupt controller checks if it has stored 
   * platform data that has this information.
   * In the absense of this information the default base priority is assigned.
   */
  if(nInterruptPriority == 0)
  {
    nInterruptPriority = pInterruptData->pInterruptState[nInterruptVector].nPriority;
    nInterruptPriority = INTERRUPTCONTROLLER_SET_PRIORITY(nInterruptPriority);
  }

  /*
   * In the new architecture stack can be specified in two ways ;
   * 1. InterruptController Platform data can specify the interrupt stack for each IST thread.
   * 2. The User of the interrupt can also request for a higher stack via the flags that are passed in the registratino api.
   * In the absense of this information the default base stack size is assigned.
   */

  if (nStackSize != 0) 
  {
    pInterruptData->pInterruptState[nInterruptVector].nISTStackSize = INTERRUPTCONTROLLER_SET_STACK(nStackSize);
  }
  if(pInterruptData->pInterruptState[nInterruptVector].nISTStackSize == 0)
  {
    pInterruptData->pInterruptState[nInterruptVector].nISTStackSize = DAL_IST_STACK_SIZE;
  }
  
  pInterruptData->pInterruptState[nInterruptVector].Isr = isr;
  pInterruptData->pInterruptState[nInterruptVector].nParam = ctx;

  /*
   * The interrupt is already registered. There is no need to spawn the IST
   */
  if((OldIsr != NULL) && (GetInterruptFlag(nInterruptVector, INTERRUPTF_REGISTERED)))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
      "InterruptController_RegisterInterrupt : Interrupt %d already registered! Exiting early", nInterruptVector);
    DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);
    return DAL_SUCCESS;
  }
  SetInterruptFlag(nInterruptVector, INTERRUPTF_REGISTERED);

  if(ConfigureDynamicIST(nInterruptVector,nInterruptPriority, pISTName, pclientCtxt) == DAL_ERROR)
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
      "InterruptController_RegisterInterrupt: could not spawn an IST thread for interrupt %d registration", nInterruptVector);
    ClearInterruptFlag(nInterruptVector, INTERRUPTF_REGISTERED);
    DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);
    return DAL_ERROR;
  }
 
  DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);

  return DAL_SUCCESS;

} /* END InterruptController_RegisterInterrupt */


/*==========================================================================

  FUNCTION      InterruptController_Unregister
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult  InterruptController_Unregister
( 
  InterruptControllerClientCtxt * pclientCtxt, 
  DALInterruptID  nInterruptVector
) 
{
  int nStatus;
  InterruptControllerDevCtxt *pDevice;

  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  
  if((nInterruptVector == INVALID_INTERRUPT) ||(nInterruptVector >= pInterruptData->pPlatformConfig->nMaxIRQ))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
    "InterruptController_Unregister: Could not Unregister Interrupt %d, invalid", nInterruptVector);
    return DAL_ERROR;
  }
  if (pInterruptData->uInterruptController != 0) 
  {
    if (GetInterruptFlag(nInterruptVector, INTERRUPTF_UINTERRUPT))
    {
      ClearInterruptFlag(nInterruptVector, INTERRUPTF_UINTERRUPT);
      return uInterruptController_UnRegister(nInterruptVector);
    }
  }
  DALSYS_SyncEnter(pclientCtxt->hSyncIntrCtrlTbl);
  if (!GetInterruptFlag(nInterruptVector, INTERRUPTF_REGISTERED))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
    "InterruptController_Unregister: Failed to unregister interrupt %d, was not registered", nInterruptVector);

    /*
     * If the interrupt is not registered we clean up our state in any case just as a precaution.
     */
    pInterruptData->pInterruptState[nInterruptVector].Isr = NULL;
    pInterruptData->pInterruptState[nInterruptVector].nParam = 0;
    DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);
    return DAL_ERROR;
  }

  /* 
   * Deregister IST from QURT so it can stop waiting for an interrupt.
   */
  nStatus = qurt_interrupt_deregister(nInterruptVector); 

  if (QURT_EOK != nStatus) 
  { 
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
    "InterruptController_Unregister: Failed to Unregister Interrupt %d, attempting to unregister through abort signal", nInterruptVector);
    DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);
    return DAL_ERROR;
  } 
  else
  {
    IntLog_Out(INTLOG_REG, pInterruptData->hULog, 1, "Deregister: Interrupt %d deregistered from client task", nInterruptVector );
  }

  /*
   * Only Deregister the interrupt handler after the IST stops waiting for an interrupt.
   * i.e. let the IST stop waiting for the interrupt.
   */
  pInterruptData->pInterruptState[nInterruptVector].Isr = NULL;
  pInterruptData->pInterruptState[nInterruptVector].nParam = 0;

  /*
   * De-configure the interrupt as a wakeup source if necessary.
   */
  if ( PDC_SUCCESS == pdcInt_disable(nInterruptVector) )
  {
    IntLog_Out(INTLOG_PDC, pInterruptData->hULog, 1, "InterruptPDC: Interrupt %d deconfigured in PDC", nInterruptVector );
  }

  ClearInterruptFlag(nInterruptVector, INTERRUPTF_REGISTERED);  
  DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);

  return DAL_SUCCESS;

} /* END InterruptController_UnRegister */


/*==========================================================================

  FUNCTION      InterruptController_InterruptDone
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult  InterruptController_InterruptDone
( 
  InterruptControllerClientCtxt * pclientCtxt,   
  DALInterruptID  nInterruptVector
) 
{

  /*
   * QURT does not support the InterruptDone API.
   */
  return DAL_ERROR;

} /* END InterruptController_InterruptDone */


/*==========================================================================

  FUNCTION      InterruptController_InterruptEnable
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult  InterruptController_InterruptEnable
( 
  InterruptControllerClientCtxt * pclientCtxt,
  DALInterruptID  nInterruptVector
) 
{
  InterruptControllerDevCtxt *pDevice;

  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
 
  if((nInterruptVector == INVALID_INTERRUPT) ||
     (nInterruptVector >= pInterruptData->pPlatformConfig->nMaxIRQ))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
                    "Interrupt Controller: Invalid interrupt data. Could not Enable Interrupt");
    return DAL_ERROR;
  }

  /*
   * Acquire synchronization, set disable flag and disable
   */
  DALSYS_SyncEnter(pclientCtxt->hSyncIntrCtrlTbl);

  IntLog_Out(INTLOG_ENABLE, pInterruptData->hULog, 1, "Enable: Interrupt %d attempting to be enabled", nInterruptVector );

  ClearInterruptFlag(nInterruptVector, INTERRUPTF_DISABLED);

  qurt_interrupt_enable( nInterruptVector );

  DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);

  return DAL_SUCCESS;  

} /* END InterruptController_InterruptEnable */


/*==========================================================================

  FUNCTION      InterruptController_InterruptDisable
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult  InterruptController_InterruptDisable
( 
  InterruptControllerClientCtxt * pclientCtxt,
  DALInterruptID  nInterruptVector
) 
{
  InterruptControllerDevCtxt *pDevice;

  pDevice = pclientCtxt->pInterruptControllerDevCtxt;

  if((nInterruptVector == INVALID_INTERRUPT) ||
     (nInterruptVector >= pInterruptData->pPlatformConfig->nMaxIRQ))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
                    "Interrupt Controller: Invalid interrupt data. Could not Disable Interrupt");
    return DAL_ERROR;
  }

  /*
   * Acquire synchronization, set disable flag and disable
   */
  DALSYS_SyncEnter(pclientCtxt->hSyncIntrCtrlTbl);

  IntLog_Out(INTLOG_ENABLE, pInterruptData->hULog, 1, "Disable: Interrupt %d attempting to be disabled", nInterruptVector );

  SetInterruptFlag(nInterruptVector, INTERRUPTF_DISABLED);

  qurt_interrupt_disable( nInterruptVector );

  DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);

  return DAL_SUCCESS;

} /* END InterruptController_InterruptDisable */


/*==========================================================================

  FUNCTION      InterruptController_InterruptTrigger
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult  InterruptController_InterruptTrigger
( 
  InterruptControllerClientCtxt * pclientCtxt,
  DALInterruptID  nInterruptVector
) 
{
  InterruptControllerDevCtxt *pDevice;

  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  if((nInterruptVector == INVALID_INTERRUPT) ||(nInterruptVector >= pInterruptData->pPlatformConfig->nMaxIRQ))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
    "InterruptController_InterruptTrigger: Could not trigger interrupt %d.", nInterruptVector);
    return DAL_ERROR;
  }
  if (pInterruptData->uInterruptController != 0) 
  {
    if (GetInterruptFlag(nInterruptVector, INTERRUPTF_UINTERRUPT))
    {
      return uInterruptController_TriggerInterrupt(nInterruptVector);
    }
  }
  DALSYS_SyncEnter(pclientCtxt->hSyncIntrCtrlTbl);
  IntLog_Out(INTLOG_TRIGGER, pInterruptData->hULog, 1, "Trigger: Interrupt %d attempting to be triggered", nInterruptVector );
  /*
   * I assume that the interrupt doesn't need to be triggered at the PMU 
   * as well if this is software generated? 
   */
  qurt_interrupt_raise((unsigned int) nInterruptVector); 
  DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);

  return DAL_SUCCESS;

} /* END InterruptController_InterruptTrigger */


/*==========================================================================

  FUNCTION      InterruptController_InterruptClear
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult  InterruptController_InterruptClear
( 
  InterruptControllerClientCtxt * pclientCtxt,
  DALInterruptID  nInterruptVector
) 
{
  InterruptControllerDevCtxt *pDevice;

  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  if((nInterruptVector == INVALID_INTERRUPT) ||(nInterruptVector >= pInterruptData->pPlatformConfig->nMaxIRQ))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
    "InterruptController_InterruptClear: Could not clear interrupt %d, invalid", nInterruptVector);
    return DAL_ERROR;
  }
  DALSYS_SyncEnter(pclientCtxt->hSyncIntrCtrlTbl);
  IntLog_Out(INTLOG_CLEAR, pInterruptData->hULog, 1, "Clear: Interrupt %d attempting to be cleared", nInterruptVector );
  if (QURT_EOK != qurt_interrupt_clear(nInterruptVector))
  {
     DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
    "InterruptController_InterruptClear: Could not clear interrupt in QURT for interrupt %d", nInterruptVector);
  }
  DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);

  return DAL_SUCCESS;

} /* END InterruptController_InterruptClear */


/*==========================================================================

  FUNCTION      InterruptController_InterruptStatus
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult InterruptController_InterruptStatus
( 
  InterruptControllerClientCtxt * pclientCtxt,   
  DALInterruptID  nInterruptVector
) 
{
  DALResult nResult;
  
  int nStatus;
  InterruptControllerDevCtxt *pDevice;

  pDevice = pclientCtxt->pInterruptControllerDevCtxt; 
  if((nInterruptVector == INVALID_INTERRUPT) ||(nInterruptVector >= pInterruptData->pPlatformConfig->nMaxIRQ))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
    "InterruptController_InterruptStatus: Could not get status for interrupt %d", nInterruptVector);
    return DAL_ERROR;
  }
  if (pInterruptData->uInterruptController != 0) 
  {
    if (GetInterruptFlag(nInterruptVector, INTERRUPTF_UINTERRUPT))
    {
      return DAL_ERROR;
    }
  }
  DALSYS_SyncEnter(pclientCtxt->hSyncIntrCtrlTbl);
  if (QURT_EOK != qurt_interrupt_status(nInterruptVector, &nStatus))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
    "InterruptController_InterruptStatus: Could not get status for interrupt %d from Qurt", nInterruptVector);
    DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);
    return DAL_ERROR;
  }
  if(nStatus == 1)
  {
      nResult = DAL_INTERRUPT_SET;
  }
  else
  {
    nResult = DAL_ERROR;
  }
  DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);

  return nResult;

} /* END InterruptController_InterruptStatus */


/*==========================================================================

   FUNCTION      SetQURTTriggerConfig

  DESCRIPTION   This function sets the trigger for the interrupt vector
                using the QURT OS apis.

  PARAMETERS    
  uint32 nInterrupt  : The interrupt vector number.
  uint32 nTrigger : The interrupt trigger type.

  DEPENDENCIES  None.
 
  REQUIREMENTS  Must be called within a critical section.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/
static DALResult SetQURTTriggerConfig
(
  InterruptControllerDevCtxt *pDevice,
  uint32 nInterruptVector,
  uint32 nTrigger
)
{
  uint32 nDetectType,nPolarity;

  switch (nTrigger)
  {
    case DALINTRCTRL_ENABLE_FALLING_EDGE_TRIGGER:
      nDetectType = 1;
      nPolarity = 1;
      break;

    case DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER:
      nDetectType = 1;
      nPolarity = 0;
      break;

    case DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER:
      nDetectType = 0;
      nPolarity = 0;
      break;

    case DALINTRCTRL_ENABLE_LEVEL_LOW_TRIGGER:
      nDetectType = 0;
      nPolarity = 1;
      break;

    default:
       return DAL_ERROR;    
  }

  /*
  * Change the interrupt trigger only when the interrupt is not registered.
  */
  if (QURT_EOK != qurt_interrupt_set_config(nInterruptVector, nDetectType, nPolarity))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
    "Interrupt Controller: Qurt Could not set the trigger for interrupt %d", nInterruptVector);
  } 

  return DAL_SUCCESS;

} /* SetQURTTriggerConfig */


/*==========================================================================

   FUNCTION      SetTrigger

  DESCRIPTION   This function sets the trigger for the interrupt vector

  PARAMETERS    
  uint32 nInterrupt  : The interrupt vector number.
  uint32 nTrigger : The interrupt trigger type.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/
static DALResult SetTrigger
(
  InterruptControllerDevCtxt *pDevice,
  uint32 nInterruptVector,
  uint32 nTrigger,
  InterruptControllerClientCtxt * pclientCtxt
)
{
  int nStatus;
  uint32 nRetSig;
  InterruptArgsType* pArgs;

  nTrigger = nTrigger & DAL_INTERRUPT_TRIGGER_MASK;

  if ((nTrigger == DALINTRCTRL_ENABLE_DEFAULT_SETTINGS) ||
      (nTrigger >= DALINTRCTRL_INVALID_TRIGGER))
  {
    nTrigger = pInterruptData->pInterruptState[nInterruptVector].nTrigger;
  }
  else
  {
     pInterruptData->pInterruptState[nInterruptVector].nTrigger = nTrigger;
  }

  /*
   * Change the interrupt trigger only before the interrupt is registered by the 
   * client or in the interrupt handler of the interrupt.
   * In the thread handler if the trigger is changed we need to register the 
   * interrupt change the trigger and register the interrupt again. 
   */
  if((GetInterruptFlag(nInterruptVector, INTERRUPTF_REGISTERED)) && 
  (pInterruptData->pInterruptState[nInterruptVector].nThreadID == qurt_thread_get_id()))
  {
    IntLog_Out(INTLOG_SETTRIGGER, pInterruptData->hULog, 2, "SetTrigger: Interrupt %d trigger being set to %d for registered interrupt from IST", nInterruptVector, nTrigger );
    nStatus = qurt_interrupt_deregister(nInterruptVector); 

    if(SetQURTTriggerConfig(pDevice,nInterruptVector,nTrigger)==DAL_ERROR)
    {
      DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
      "SetTrigger: Could not set the trigger using qurt API for interrupt %d", nInterruptVector);
    }

    (void)SetWakeupInterruptTrigger(nInterruptVector,nTrigger,FALSE);  

    return DAL_SUCCESS;
  }
  else if((GetInterruptFlag(nInterruptVector, INTERRUPTF_REGISTERED)) && 
  (pInterruptData->pInterruptState[nInterruptVector].nThreadID != qurt_thread_get_id()))
  {
    IntLog_Out(INTLOG_SETTRIGGER, pInterruptData->hULog, 2, "SetTrigger: Interrupt %d trigger being set to %d for registered interrupt", nInterruptVector, nTrigger );
    if ( ( pArgs = InterruptControllerPopArg( &pInterruptData->pEmptyList ) ) == NULL )
    {
      if( DALSYS_Malloc( sizeof( InterruptArgsType ), (void **)&pArgs ) == DAL_ERROR )
      {
        DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
        "SetTrigger: Could not allocate memory for args for interrupt %d", nInterruptVector);
        return DAL_ERROR;
      }

      DALSYS_memset(pArgs, 0, sizeof(InterruptArgsType));

      qurt_anysignal_init( &pArgs->pRetSig );
    }

    pArgs->nInterruptVector = nInterruptVector;

    InterruptControllerPushArg( pArgs, &pInterruptData->pArgsList );

    nStatus = qurt_interrupt_deregister(nInterruptVector); 
    if (QURT_EOK != nStatus) 
    { 
      DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
      "SetTrigger: Failed to deregister interrupt %d at Qurt", nInterruptVector);
    } 

    if(SetQURTTriggerConfig(pDevice,nInterruptVector,nTrigger)==DAL_ERROR)
    {
      DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
      "SetTrigger: Could not set the trigger using qurt API for interrupt %d", nInterruptVector);
    }

    qurt_anysignal_set(&pInterruptData->pInterruptState[nInterruptVector].ISTSignal,
                                    INTERRUPT_TASK_START);

    DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);

    nRetSig = qurt_anysignal_wait( &pArgs->pRetSig, 
                                   INTERRUPT_REGISTER_SUCCESS | INTERRUPT_REGISTER_FAIL );

    qurt_anysignal_clear( &pArgs->pRetSig, INTERRUPT_REGISTER_SUCCESS | INTERRUPT_REGISTER_FAIL );

    DALSYS_SyncEnter(pclientCtxt->hSyncIntrCtrlTbl);

    InterruptControllerPushArg( pArgs, &pInterruptData->pEmptyList );

    (void)SetWakeupInterruptTrigger(nInterruptVector,nTrigger,FALSE);  

    if ( nRetSig | INTERRUPT_REGISTER_SUCCESS )
    {
      return DAL_SUCCESS;
    }
    else
    {
      return DAL_ERROR;
    }
  }
  else if(!GetInterruptFlag(nInterruptVector, INTERRUPTF_REGISTERED))
  {
    /*
     * Log trigger set
     */
    IntLog_Out(INTLOG_SETTRIGGER, pInterruptData->hULog, 2, "SetTrigger: Interrupt %d trigger being set to %d for non-registered interrupt", nInterruptVector, nTrigger );

   /*
    * Change the interrupt trigger only when the interrupt is not registered.
    */
    if(SetQURTTriggerConfig(pDevice,nInterruptVector,nTrigger)==DAL_ERROR)
    {
      return DAL_ERROR;
    } 
  }
 
  (void)SetWakeupInterruptTrigger(nInterruptVector,nTrigger,FALSE);  

  return DAL_SUCCESS;

} /* SetTrigger */


/*==========================================================================

  FUNCTION      InterruptController_SetTrigger
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult InterruptController_SetTrigger
(
  InterruptControllerClientCtxt * pclientCtxt, 
  DALInterruptID  nInterruptVector,
  uint32 nTrigger
)
{
  InterruptControllerDevCtxt *pDevice;
  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  if((nInterruptVector == INVALID_INTERRUPT) ||(nInterruptVector >= pInterruptData->pPlatformConfig->nMaxIRQ))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
    "InterruptController_SetTrigger: Could not set the trigger for interrupt %d", nInterruptVector);
    return DAL_ERROR;
  }
  if (pInterruptData->uInterruptController != 0) 
  {
    if (GetInterruptFlag(nInterruptVector, INTERRUPTF_UINTERRUPT))
    {
      return DAL_ERROR;
    }
  }
  DALSYS_SyncEnter(pclientCtxt->hSyncIntrCtrlTbl);
  /*
   * Set the trigger in L2VIC and PMU
   */
  (void)SetTrigger(pDevice, nInterruptVector, nTrigger, pclientCtxt);
  DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);

  return DAL_SUCCESS;

} /* END InterruptController_SetTrigger */


/*==========================================================================

  FUNCTION      InterruptController_IsInterruptPending
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult
InterruptController_IsInterruptPending
(
  InterruptControllerClientCtxt * pclientCtxt, 
  DALInterruptID  nInterruptVector,
  uint32* bState
)
{
  int nStatus;
  InterruptControllerDevCtxt *pDevice;

  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  if((nInterruptVector == INVALID_INTERRUPT) ||(nInterruptVector >= pInterruptData->pPlatformConfig->nMaxIRQ))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
    "InterruptController_IsInterruptPending: Could not get interrupt %d pending status, invalid", nInterruptVector);
    return DAL_ERROR;
  }
  if (pInterruptData->uInterruptController != 0) 
  {
    if (GetInterruptFlag(nInterruptVector, INTERRUPTF_UINTERRUPT))
    {
      return uInterruptController_IsInterruptPending(nInterruptVector,bState);
    }
  }
  DALSYS_SyncEnter(pclientCtxt->hSyncIntrCtrlTbl);
  if (QURT_EOK != qurt_interrupt_status(nInterruptVector, &nStatus))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
    "InterruptController_IsInterruptPending: Could not get interrupt %d pending status from Qurt", nInterruptVector);
    DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);
    *bState = 0;
    return DAL_ERROR;
  }
  if((nStatus == 1)&&(GetInterruptFlag(nInterruptVector, INTERRUPTF_REGISTERED)))
  {
    *bState = 1;
  }
  else
  {
    *bState = 0;
  }
  DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);

  return DAL_SUCCESS;

} /* END InterruptController_IsInterruptPending */


/*==========================================================================

  FUNCTION      InterruptController_IsInterruptEnabled
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult
InterruptController_IsInterruptEnabled
(
  InterruptControllerClientCtxt * pclientCtxt, 
  DALInterruptID  nInterruptVector,
  uint32* bState
)
{
  InterruptControllerDevCtxt *pDevice;

  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  if((nInterruptVector == INVALID_INTERRUPT) ||(nInterruptVector >= pInterruptData->pPlatformConfig->nMaxIRQ))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
    "InterruptController_IsInterruptEnabled: Could not get interrupt %d enabled status", nInterruptVector);
    return DAL_ERROR;
  }
  if (pInterruptData->uInterruptController != 0) 
  {
    if (GetInterruptFlag(nInterruptVector, INTERRUPTF_UINTERRUPT))
    {
      return DAL_ERROR;
    }
  }
  DALSYS_SyncEnter(pclientCtxt->hSyncIntrCtrlTbl);
  *bState = GetInterruptFlag(nInterruptVector, INTERRUPTF_REGISTERED);
  DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);

  return DAL_SUCCESS;

} /* END InterruptController_IsInterruptEnabled */

/*==========================================================================

  FUNCTION      InterruptController_MapWakeupInterrupt
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult
InterruptController_MapWakeupInterrupt
(
  InterruptControllerClientCtxt * pclientCtxt, 
  DALInterruptID  nInterruptVector,
  uint32 nWakeupIntID
)
{

  return DAL_SUCCESS;


} /* END InterruptController_MapWakeupInterrupt */


/*==========================================================================

  FUNCTION      InterruptController_IsAnyInterruptPending
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult
InterruptController_IsAnyInterruptPending
(
  InterruptControllerClientCtxt * pclientCtxt, 
  uint32* bState
)
{
  uint32 nIdx;
  int nStatus;
  InterruptControllerDevCtxt *pDevice;

  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  *bState = 0;

  DALSYS_SyncEnter(pclientCtxt->hSyncIntrCtrlTbl);
  for(nIdx = 0; nIdx < MAX_QURT_IRQ ; nIdx ++)
  {
    if(GetInterruptFlag(nIdx, INTERRUPTF_REGISTERED))
    {
      if (QURT_EOK != qurt_interrupt_status(nIdx, &nStatus))
      {
        DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
        "InterruptController_IsAnyInterruptPending: Could not get interrupt %d pending status from qurt", nIdx);
        DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);
        return DAL_ERROR;
      }
      if (nStatus == 1) 
      {
        *bState = 1;
        break;
      }
    }
  }
  DALSYS_SyncLeave(pclientCtxt->hSyncIntrCtrlTbl);

  return DAL_SUCCESS;


} /* END InterruptController_IsAnyInterruptPending */


/*==========================================================================

  FUNCTION      InterruptController_Sleep
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult 
InterruptController_Sleep
(
  InterruptControllerClientCtxt * pclientCtxt,
  InterruptControllerSleepType sleep
)
{
  /* Nothing to be done for L2VIC*/

  return DAL_SUCCESS;

} /* END InterruptController_Sleep */


/*==========================================================================

  FUNCTION      InterruptController_GetInterruptTrigger
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult InterruptController_GetInterruptTrigger
(
  InterruptControllerClientCtxt *pclientCtxt, 
  DALInterruptID  nInterruptVector,
  uint32* eTrigger
)
{
  uint32 nPolarity,nDetectType;
  InterruptControllerDevCtxt *pDevice;

  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  if((nInterruptVector == INVALID_INTERRUPT) ||(eTrigger ==NULL) ||(nInterruptVector >= pInterruptData->pPlatformConfig->nMaxIRQ))
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
    "InterruptController_GetInterruptTrigger: Could not get interrupt %d trigger", nInterruptVector);
    return DAL_ERROR;
  }
  qurt_interrupt_get_config((unsigned int)nInterruptVector,
    (unsigned int *)&nDetectType, (unsigned int *)&nPolarity);

  if((nPolarity == 1)&&(nDetectType == 1))
  {
    *eTrigger = DALINTRCTRL_ENABLE_FALLING_EDGE_TRIGGER;
  }
  else if((nPolarity == 0)&&(nDetectType == 0))
  {
    *eTrigger = DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER;
  }
  else if((nPolarity == 0)&&(nDetectType == 1))
  {
    *eTrigger = DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER;
  }
  else if((nPolarity == 1)&&(nDetectType == 0))
  {
    *eTrigger = DALINTRCTRL_ENABLE_LEVEL_LOW_TRIGGER;
  }
  else
  {
    return DAL_ERROR;    
  }

  return DAL_SUCCESS;

} /* END InterruptController_GetInterruptTrigger */


/*==========================================================================

  FUNCTION      InterruptController_RestoreWakeupConfigs
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult 
InterruptController_Wakeup
(
  InterruptControllerClientCtxt * pclientCtxt,
  InterruptControllerSleepType sleep
)
{

  /* Nothing to be done for L2VIC*/

  return DAL_SUCCESS;

}/* END InterruptController_Wakeup */


/*=============================================================================

  FUNCTION      GetPlatformData

  DESCRIPTION   This function queries DALSYS to get platform specific information
  for the interrupt Controller GPIO interrupt. The interrupt is disabled.

  PARAMETERS     
  InterruptControllerClientCtxt * : The client context pointer of the interrupt 
    controller DAL.

  DEPENDENCIES  None.

  RETURN VALUE  
  DALResult.  : 
  DAL_ERROR : 
  If the interrupt controller was not able to query the target specific information.
  DAL_SUCCESS :
  Returns a success if all relevant data was 

  SIDE EFFECTS  None.

=============================================================================*/

static DALResult 
GetPlatformData
(
  InterruptControllerClientCtxt * pclientCtxt
)
{
  InterruptControllerDevCtxt *pDevice;
  qurt_mem_region_t      sQmemRegion;
  qurt_mem_region_attr_t sQmemAttr;
  qurt_mem_pool_t        sPool;
  int nStatus;
  uint32 nIdx,nIntIdx;
  InterruptConfigType *pIRQconfigs;

  /* 
   * Structure which holds properties 
   */
  DALSYSPropertyVar system_property_var;
  
  /*
   *  Declare property handle variable 
   */
  DALSYS_PROPERTY_HANDLE_DECLARE(interrupt_property); 
  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  
  /* 
   * Get the property handle 
   */
  DALSYS_GetDALPropertyHandle(DALDEVICEID_INTERRUPTCONTROLLER,interrupt_property); 
  if(DALSYS_GetPropertyValue(interrupt_property,"INTERRUPT_CONFIG_DATA",
                           0,&system_property_var) == DAL_SUCCESS)
  {
    if(DALSYS_Malloc(sizeof(InterruptDataType),(void **)&pInterruptData) == DAL_ERROR)
    {
       DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
      "Interrupt Controller: could not allocate data for InterruptController in GetPlatformData");
      return DAL_ERROR;
    }

    /*
     * Allways initialize malloced data to 0.
     */
    DALSYS_memset(pInterruptData, 0, sizeof(InterruptDataType));
    pInterruptData->pPlatformConfig  = (InterruptPlatformDataType *)system_property_var.Val.pdwVal;
  }
  else
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
    "Interrupt Controller: could not get configuration data for Device");
    return DAL_ERROR;
  } 

  if(pInterruptData->pPlatformConfig == NULL)
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
    "Interrupt Controller: could not get Interrupts supported for this Device");
    return DAL_ERROR;
  }

  if(pInterruptData->pPlatformConfig->nMaxIRQ)
  {
    if(DALSYS_Malloc((sizeof(InterruptStateType)*pInterruptData->pPlatformConfig->nMaxIRQ),
                               (void **)&pInterruptData->pInterruptState) == DAL_ERROR)
    {
      DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
      "Interrupt Controller: could not allocate state data for InterruptController");
      return DAL_ERROR;
    }

    /*
     * Allways initialize malloced data to 0.
     */
    DALSYS_memset(pInterruptData->pInterruptState, 0, 
      (sizeof(InterruptStateType)*pInterruptData->pPlatformConfig->nMaxIRQ));
  }
  else
  {
    DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
    "Interrupt Controller: Max IRQ number is corrupt.");
    return DAL_ERROR;
  }
  pInterruptData->pClientCtxt = pclientCtxt;
  
  if(DALSYS_GetPropertyValue(interrupt_property,
  "UINTERRUPT_CONTROLLER",0,&system_property_var) == DAL_SUCCESS)
  {
    pInterruptData->uInterruptController = 
        system_property_var.Val.dwVal;
    uInterruptController_Init();
  }
  else
  {
    /*
     * we do not support the ugpioint  driver.
     */
    pInterruptData->uInterruptController = 0;
  }

  if(INTERRUPT_LOG_ENTRIES)
  {
    pInterruptData->pLog.nLogSize = INTERRUPT_LOG_ENTRIES;

    /*
     * Create uncached memory for interrupt log
     */
    nStatus = qurt_mem_pool_attach("DEFAULT_PHYSPOOL", &sPool);
    if (QURT_EOK != nStatus)
    {
      DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
      "Unable to attach to default memory pool");
      return DAL_ERROR;
    }
    qurt_mem_region_attr_init( &sQmemAttr );
    qurt_mem_region_attr_set_mapping(&sQmemAttr, QURT_MEM_MAPPING_VIRTUAL);
    qurt_mem_region_attr_set_cache_mode(&sQmemAttr, QURT_MEM_CACHE_NONE);
    nStatus = qurt_mem_region_create(
        &sQmemRegion, 
        INTERRUPT_LOG_ENTRIES * sizeof(InterruptLogEntryType), 
        sPool, 
        &sQmemAttr);
    if (QURT_EOK != nStatus)
    {
      DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
      "Unable to create uncached memory for DALint log!");
      return DAL_ERROR;
    }

    /*
     * Get the allocation address
     */
    qurt_mem_region_attr_get(sQmemRegion, (qurt_mem_region_attr_t *) &sQmemAttr);
    pInterruptData->pLog.pEntries = (InterruptLogEntryType*) sQmemAttr.virtaddr;
  }

  /* 
   * We first default to a median priority and predefined stack size.
   */
  for(nIdx = 0; nIdx < pInterruptData->pPlatformConfig->nMaxIRQ; nIdx ++)
  { 
    pInterruptData->pInterruptState[nIdx].nPriority = INTERRUPT_THREAD_MEDIAN_PRIORITY;
    pInterruptData->pInterruptState[nIdx].nISTStackSize = DAL_IST_STACK_SIZE;
    pInterruptData->pInterruptState[nIdx].nTrigger = DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER;
  }

  /* 
   * Here we setup the lookup table for the BSP. This is only for preconfiguration 
   * puposes for legacy targets.
   */
  if(pInterruptData->pPlatformConfig->pIRQConfigs != NULL)
  {
    pIRQconfigs = pInterruptData->pPlatformConfig->pIRQConfigs;
   
    /*
     * Go over supported interrupts from the Interrupt Config Data and populate 
     * the existing table
     */
    for(nIdx = 0; pIRQconfigs[nIdx].nInterruptVector != INVALID_INTERRUPT; nIdx ++)
    {     
      nIntIdx = (uint32)pIRQconfigs[nIdx].nInterruptVector;
      pInterruptData->pInterruptState[nIntIdx].nISTStackSize  = DAL_IST_STACK_SIZE;
      pInterruptData->pInterruptState[nIntIdx].pInterruptName = pIRQconfigs[nIdx].pInterruptName;
      pInterruptData->pInterruptState[nIntIdx].nTrigger = (uint32)pIRQconfigs[nIdx].nTrigger;
      pInterruptData->pInterruptState[nIntIdx].nPriority = INTERRUPT_THREAD_MEDIAN_PRIORITY;

    }
  }

  return DAL_SUCCESS;


} /* END InterruptController_GetPlatformData */


/*==========================================================================

  FUNCTION      SetWakeupInterruptTrigger

  DESCRIPTION   This function configures the interrupt trigger for the 
                relevant pdc interrupt.

  PARAMETERS    IntID   - The interrupt whose isr is needed.
                nTrigger -  The trigger to be set.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  See implementation header file.


==========================================================================*/
static DALResult SetWakeupInterruptTrigger
(
  uint32 nInterrupt,
  uint32 nTrigger,
  boolean bEnable
)
{
  pdc_trigger_config  sConfig;

  /* 
   * TODO: What does drive do here?
   */
  sConfig.drv_num = PDC_DRV0;

  switch (nTrigger)
  {
    default:
    return DAL_ERROR;

    case DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER:
      sConfig.trig_type = TRIGGER_LEVEL_HIGH;
      break;

    case DALINTRCTRL_ENABLE_LEVEL_LOW_TRIGGER:
      sConfig.trig_type = TRIGGER_LEVEL_LOW;
      break;

    case DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER:
      sConfig.trig_type = TRIGGER_RISING_EDGE;
      break;

    case DALINTRCTRL_ENABLE_FALLING_EDGE_TRIGGER:
      sConfig.trig_type = TRIGGER_FALLING_EDGE;
      break;
  }

  /*
   * Configure pdcInt.
   */
  if ( ( PDC_SUCCESS == pdcInt_config( nInterrupt, &sConfig ) ) && bEnable )
  {
    pdcInt_enable( nInterrupt );
    IntLog_Out(INTLOG_PDC, pInterruptData->hULog, 1, "InterruptPDC: Interrupt %d configured and enabled in PDC", nInterrupt );
  }

  return DAL_SUCCESS;
 
} /* END SetWakeupInterruptTrigger */


/*==========================================================================

  FUNCTION      GetInterruptFlag

==========================================================================*/

static boolean GetInterruptFlag
(
  uint32 nInterrupt,
  uint16 nFlag
)
{
  return (pInterruptData->pInterruptState[nInterrupt].nFlags & nFlag) ? TRUE : FALSE;

} /* END GetInterruptFlag */


/*==========================================================================

  FUNCTION      SetInterruptFlag

==========================================================================*/

static void SetInterruptFlag
(
  uint32 nInterrupt,
  uint16 nFlag
)
{
  pInterruptData->pInterruptState[nInterrupt].nFlags |= nFlag;

} /* END SetInterruptFlag */


/*==========================================================================

  FUNCTION      ClearInterruptFlag

==========================================================================*/

static void ClearInterruptFlag
(
  uint32 nInterrupt,
  uint16 nFlag
)
{
  pInterruptData->pInterruptState[nInterrupt].nFlags &= ~nFlag;

} /* END ClearInterruptFlag */


/*==========================================================================

  FUNCTION      LogEvent

==========================================================================*/

static void LogInterruptEvent
(
  InterruptControllerClientCtxt  *pclientCtxt,
  InterruptLogEventType          eEvent,
  uint32                         nInterrupt
)
{
  uint64 nCount;
  atomic_plain_word_t nIdx;
  if(pInterruptData->pLog.nLogSize)
  {

    /*
     * Obtain log index from an atomic operation
     * Note: log size must be power of 2 for wrapping aound to zero to work.
     */
    nIdx = atomic_inc_return(&pInterruptData->pLog.AtomicIdx);
    nIdx &= (INTERRUPT_LOG_ENTRIES-1);

    /*
     * Update log entry
     */
    pInterruptData->pLog.pEntries[nIdx].eEvent = eEvent;
    pInterruptData->pLog.pEntries[nIdx].nInterrupt = nInterrupt;
    DalTimetick_GetTimetick64(pInterruptData->pTimetickHandle, &nCount);
    pInterruptData->pLog.pEntries[nIdx].pInterruptName = pInterruptData->pInterruptState[nInterrupt].pInterruptName;
    pInterruptData->pLog.pEntries[nIdx].nTimeStamp = nCount;
    pInterruptData->pLog.pEntries[nIdx].pcycles = qurt_get_core_pcycles();

    /* 
     * A static internal copy is kept for Interrupt Log cmm scripts. 
     */
    pInterruptData->pLog.nIdx = nIdx;
  }

} /* END LogEvent */


/*==========================================================================

  FUNCTION      TriggerClientEvent

==========================================================================*/
static void TriggerClientEvent
(
  uint32 nInterruptVector,
  InterruptControllerClientCtxt *pClientCtxt
)
{
  uint64                     nCount;
  DALISRCtx                  ClientParam;
  InterruptControllerDevCtxt *pDevice; 
  DALISR                     ClientIsr; 

  pDevice = pClientCtxt->pInterruptControllerDevCtxt;

  if (pInterruptData->pInterruptState[nInterruptVector].Isr == NULL)
  {
    if(GetInterruptFlag(nInterruptVector, INTERRUPTF_REGISTERED))
    {

      /*
       * The Interrupt Controller IST thread should never be called for a 
       * fired interrupt that has no handler.
       */
      DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
      "TriggerClientEvent: Interrupt %d does not have a client registered", nInterruptVector);
      return;
    }
    else
    {
      /*
       * Somehow the InterruptController state is messed up . 
       * We need to deregister the interrupt.
       */
      DALSYS_LogEvent(pDevice->DevId, DALSYS_LOGEVENT_WARNING, 
      "TriggerClientEvent: Interrupt %d does not have a client registered, deregistering", nInterruptVector);
      LogInterruptEvent(pClientCtxt,INTERRUPT_LOG_EVENT_UNHANDLED, nInterruptVector);
      InterruptController_Unregister(pClientCtxt,nInterruptVector);
      return;
    }
  }
  ClientIsr = pInterruptData->pInterruptState[nInterruptVector].Isr;
  ClientParam = pInterruptData->pInterruptState[nInterruptVector].nParam;
 
  /*
   * Set the timetest ID to the executing ISR.
   */
  #ifdef TIMETEST
  TIMETEST_ISR_ID(INTERRUPT_TIMETESTID(nInterruptVector));
  #endif
  /*
   * Set the running flag.
   */
  SetInterruptFlag(nInterruptVector, INTERRUPTF_RUNNING);

  DALSYS_SyncLeave(pClientCtxt->hSyncIntrCtrlTbl);
 
  ClientIsr(ClientParam);

  /*
   * Re-lock  if we unlocked.  Use local flag since the global flag may
   * have been cleared during ISR execution.
   */
  DALSYS_SyncEnter(pClientCtxt->hSyncIntrCtrlTbl); 
 
  /*
   * Clear the running flag.
   */
  ClearInterruptFlag(nInterruptVector, INTERRUPTF_RUNNING);

  /*
   * Increase debug count.
   */
  pInterruptData->pInterruptState[nInterruptVector].nCount ++;
  DalTimetick_GetTimetick64(pInterruptData->pTimetickHandle, &nCount);
  pInterruptData->pInterruptState[nInterruptVector].nLastFired = nCount;

} /* END TriggerClientEvent */


/*==========================================================================

  FUNCTION      InterruptController_GetInterruptID

==========================================================================*/
DALResult InterruptController_GetInterruptID
(
  InterruptControllerClientCtxt  *pclientCtxt,
  const char* szIntrName,
  uint32* pnIntrID
)
{
  uint32 nIdx;
  DALResult nResult;

  nResult = DAL_ERROR;
  *pnIntrID = INVALID_INTERRUPT;
  
  if(!szIntrName)
  {
    return DAL_ERROR;
  }
  
  for (nIdx=0;nIdx < pInterruptData->pPlatformConfig->nMaxIRQ;nIdx++) 
  {
    //Make sure we are iterating only through interrupts valid for this subsystem
    if(pInterruptData->pInterruptState[nIdx].pInterruptName)
    {
      if (strcmp(szIntrName, pInterruptData->pInterruptState[nIdx].pInterruptName) == 0)
      {
        nResult = DAL_SUCCESS;
        *pnIntrID = nIdx;
        break;
      }
    }
  }
  return nResult;
} /* END InterruptController_GetInterruptID */


/*==========================================================================

  FUNCTION      InterruptController_LogState

==========================================================================*/
DALResult InterruptController_LogState
(
  InterruptControllerClientCtxt  *pclientCtxt,
  void *pULog
)
{
  uint32 nIdx,nTrigger;
  int bIsEnabled,bIsFired; 
  for(nIdx = 0; nIdx < pInterruptData->pPlatformConfig->nMaxIRQ; nIdx ++)
  {
    qurt_interrupt_status(nIdx, &bIsFired); 
    bIsEnabled = GetInterruptFlag(nIdx, INTERRUPTF_REGISTERED);
    if ((bIsFired != 0)||(bIsEnabled != 0)) 
    {
      InterruptController_GetInterruptTrigger(pclientCtxt,nIdx,&nTrigger);
      ULOG_RT_PRINTF_4(pULog,"Interrupt Vector : %d Enable state %d Fired State %d Trigger type  %d ",nIdx,bIsEnabled,
                     bIsFired,nTrigger);
    }
    
  }

  return DAL_SUCCESS;

} /* END InterruptController_LogState */


