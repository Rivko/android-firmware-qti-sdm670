#ifndef _DAL_INTRCTRLR_CONFIG_H
#define _DAL_INTRCTRLR_CONFIG_H
/*==============================================================================

FILE:      DALInterruptController_config.h

DESCRIPTION: This file implements a Interrupt Controller for QDSP6 hardware 
             interrupt controller

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2012-2016 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/InterruptController/src/qurt/DALInterruptControllerConfig.h#1 $ 

when       who     what, where, why
--------   ---     ---------------------------------------------------------
07/28/07   an     Initial version. Conversion of Tramp to DALInterruptController

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "DALInterruptController.h"
#include "atomic_ops.h"
#include "qurt_anysignal.h"
#include "qurt_thread.h"
#include "DALInterruptController_utils.h"
#include "InterruptControllerInterface.h"
#include "ULog.h"
#include "ULogFront.h"
/*=========================================================================

                           CONSTANTS

===========================================================================*/


/*=========================================================================

                           TYPE DEFINITIONS

===========================================================================*/

/*
 * InterruptLogEventType
 *
 * Log event type.
 *
 *  NULL:          NULL event
 *  NEST_START:    Start of an interrupt processing nesting level.
 *  ISR_START:     Start of ISR processing.
 *  ISR_FINISH:    Finished ISR processing.
 *  NEST_FINISH:   Completion of an interrupt processing nesting level.
 *  SET:           An interrupt was manually triggered.
 *  SEND:          Sent an interrupt to another processor.
 *  SLEEP:         Went to sleep.
 *  WAKEUP:        Woke-up from sleep.
 *  UNHANDLED:     Unhandled interrupt received.
 *  REENTRY:       Interrupt received while still being processed.  This
 *                 is an error since re-entrance is not allowed.
 */
typedef enum
{
  INTERRUPT_LOG_EVENT_NULL,

  /*
   * Common log events
   */
  INTERRUPT_LOG_EVENT_ISR_START,
  INTERRUPT_LOG_EVENT_ISR_FINISH,
  INTERRUPT_LOG_EVENT_SUBISR_START,
  INTERRUPT_LOG_EVENT_SUBISR_FINISH,
  INTERRUPT_LOG_EVENT_TRIGGERED,
  INTERRUPT_LOG_EVENT_SENT,
  INTERRUPT_LOG_EVENT_UNHANDLED,
  INTERRUPT_LOG_EVENT_SUSPENDED,
  INTERRUPT_LOG_EVENT_UNSUPPORTED,
  INTERRUPT_LOG_EVENT_SLEEP,
  INTERRUPT_LOG_EVENT_WAKEUP,
  INTERRUPT_LOG_EVENT_SHUTDOWN,
  INTERRUPT_NUM_LOG_EVENTS,
  PLACEHOLDER_InterruptLogEventType = 0x7fffffff
} InterruptLogEventType;

/*
 * InterruptLogEntriesType
 *
 * Log entry type.
 *
 *  event        - The event that is being logged.
 *  irq:         - The interrupt number, or NULL_IRQ if not applicable.
 *  timestamp    - The timestamp in sclks.
 *  pcycles      - the number of pcycles that has elapsed since the qdsp6 core is up.
 */
typedef struct
{
  InterruptLogEventType     eEvent;
  uint32                    nInterrupt;
  char*                     pInterruptName;
  uint64                    nTimeStamp;
  unsigned long long int    pcycles;
} InterruptLogEntryType;


/*
 * InterruptLogType
 *
 * Log type container.
 */
typedef struct
{
  atomic_word_t                AtomicIdx;
  uint32                       nIdx;
  uint32                       nLogSize;
  InterruptLogEntryType*       pEntries;
} InterruptLogType;

typedef void (*DALISR_HandlerType) (uint32 param);

/*
 * InterruptStateType
 *
 * Data structure containing all information about an interrupt
 *
 *  Isr               - Interrupt service routine to invoke.
 *  nParam            - Parameter to pass the ISR.
 *  nTrigger          - The interrupt trigger type.  
 *  flags             - Interrupt flags.
 *  nCount            - Count of how many times the interrupt fired.  
 *  nLastFired        - The time stamp of the last time the interrupt fired.
 * *pTCB              - The task control block of the IST task.
 *  nThreadID         - The thread id of the IST task.
 * *pISTStack         - The IST stack size.
 * *pISTName          - The name of the interrupt service thread.
 * nTimeTestProfileID - The time test id is used only for profiling purposes.
 * nPriority          - Priority of the interrupt thread.
 * nISTStackSize      - Interrupt Service Thread Stack Size.
 */
typedef struct
{
  DALISR                    Isr;
  DALISRCtx                 nParam;
  uint32                    nCount;
  uint64                    nLastFired;
  qurt_thread_t             nThreadID;
  unsigned char *           pISTStack;
  uint32                    nInterruptVector;
  char                      aISTName[IST_TASKNAME_SIZE];
  qurt_anysignal_t          ISTSignal;
  qurt_thread_attr_t        ThreadAttr;
  uint16                    nTimeTestProfileID;
  uint8                     nTrigger;
  uint8                     nFlags;
  uint32                    nPriority;
  uint32                    nISTStackSize;
  char*                     pInterruptName;
} InterruptStateType;


 /*
  * InterruptDataType
  * *pTimetickHandle       - Client handle to the timetick dal. 
  * pClientCtxt            - Handle to the Client context.
  * pInterruptState        - pointer to the array keeping track of the interrupt state.       
  * *pLog                  - Log for interrupts that come in.
  * *pPlatformConfig       - Interrupt Controller platform configuration data. 
  * *pArgsList             - List of args to be passed to ISTs.
  * *pEmptyList            - List of free args that can be used instead of mallocing.
  */
typedef struct
{
  DalDeviceHandle                *pTimetickHandle;
  InterruptControllerClientCtxt  *pClientCtxt;
  InterruptStateType             *pInterruptState;
  InterruptLogType               pLog;
  InterruptPlatformDataType      *pPlatformConfig;
  uint32                         uInterruptController;
  InterruptArgsType              *pArgsList;
  InterruptArgsType              *pEmptyList;
  ULogHandle                     hULog;
} InterruptDataType;



#endif //_DAL_INTRCTRLR_CONFIG_H

