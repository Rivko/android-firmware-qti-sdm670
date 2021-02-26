#ifndef __DALIINTERRUPTCONTROLLER_H__
#define __DALIINTERRUPTCONTROLLER_H__


      /*      D A L   I N T E R R U P T   C O N T R O L L E R    */

/**
  @file DDIInterruptController.h
  @brief  This modules contains enumeration definitions and API to utilize the Dal 
  Interrupt controller.The definitions in this module are used externally by the clients of the 
  Interrupt controller.
*/

/* NOTE: For the output PDF generated using Doxygen and Latex, all file and 
         group descriptions are maintained in the mainpage file. To change 
         any of the the file/group text for the PDF, edit this 
         file, or contact Tech Pubs.
         The above description for this file is part of the "gpioint_api" group 
         description in the mainpage file. 
*/

/*===========================================================================
               Copyright (c) 2011 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                      QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/api/systemdrivers/DDIInterruptController.h#1 $

when         who what, where, why
--------     --- ----------------------------------------------------------
******** PLEASE ADD PAST EDIT HISTORY ********
10/10/2011   leo  (Tech Pubs) edited Doxygen comments and markup.
07/13/2011   an   modified to Extend the Interrupt Controller interface to 
                     encompass the QGIC and QUBE Interrupt Controllers.
==============================================================================*/
#include "DalDevice.h"
#include "DALStdErr.h"


/** @addtogroup constants_macros
@{ */

/** Defines the interface version for the DAL interrupt controller driver.
*/
#define DALINTERRUPTCONTROLLER_INTERFACE_VERSION DALINTERFACE_VERSION(1,2)

/** Provides the legacy support for priority; this is no longer in use.
*/
#define DALINTRCTRL_ENABLE_CUSTOM_SETTINGS(edge,level,priority) \
                                          (edge|level|(priority<<24))

/** Provides the new support for priority, trigger and stack.
 *  This is for increased stack usage.the stack is increased in
 *  multiples of 1024 bytes. The user should use flags 2 for
 *  stack size  2048 bytes, 4 for 4096 bytes.
 */
#define DALINTRCTRL_CONFIG_STACK_1K (1<<4)
#define DALINTRCTRL_CONFIG_STACK_2K (2<<4)
#define DALINTCTRL_CONFIG_STACK_4K (4<<4)

/** Used to set the trigger of an interrupt along with its priority.
*/
#define DALINTRCTRL_ENABLE_CONFIG(trigger,priority) \
                                          (trigger|(priority<<24))

/** Used to set the trigger of an interrupt along with its
 *  priority and to tell the Interrupt Controller this interrupt
 *  is a micro image interrupt controller. its 1 bit set to
 *  value =1 for uInterrupt and zero otherwise.
*/
#define DALINTRCTRL_ENABLE_CONFIG_EX(trigger,priority,uInterrupt) \
                                          (trigger|(priority<<24)|(uInterrupt<<8))


/** Default interrupt trigger value.
*/
#define DALINTRCTRL_ENABLE_DEFAULT_SETTINGS        0x00000001 

/** Interrupt is triggered only if the input signal at the source is high.
*/
#define DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER      0x00000002 

/** Interrupt is triggered only if the input signal at the source is low.
*/
#define DALINTRCTRL_ENABLE_LEVEL_LOW_TRIGGER       0x00000003 

/** Interrupt is triggered only if the input signal at the source transitions
from low to high.
*/
#define DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER     0x00000004 

/** Interrupt is triggered only if the input signal at the source transitions
from high  to low.
*/
#define DALINTRCTRL_ENABLE_FALLING_EDGE_TRIGGER    0x00000005 

/** Macro for invalid trigger type. This should not be used.
*/
#define DALINTRCTRL_INVALID_TRIGGER                0x00000006


/** @} */ /* end_addtogroup constants_macros */

/** @addtogroup data_types
@{ */

typedef uint32  DALInterruptID;      /**< Interrupt vector number. */
typedef void * DALISRCtx;            /**< Client callback parameter. */
typedef void * (*DALISR)(DALISRCtx); /**< Client callback. */

/** @cond 
*/
typedef void * DALIRQCtx;            /* Client callback parameter. */
typedef void * (*DALIRQ)(DALIRQCtx); /* Client callback. */

/** @endcond 
*/

/*
 * InterruptControllerSleepType
 *
 * The type of sleep that will be performed.
 *
 *  DALINTCNTLR_NO_POWER_COLLAPSE: power collapse will not be done,
 *                                 or did not happen
 *  DALINTCNTLR__POWER_COLLAPSE:      power collapse will be done, or did happen
 */

/** Defines the types of sleep modes in which the DAL interrupt controller
can operate.
*/
typedef enum
{
  DALINTCNTLR_NO_POWER_COLLAPSE,  /**< Power collapse will not be done or did
                                       not occur. */
  DALINTCNTLR_POWER_COLLAPSE,     /**< Power collapse will be done or did
                                       occur. */
  PLACEHOLDER_InterruptControllerSleepType = 0x7fffffff
} InterruptControllerSleepType;
/** @} */ /* end_data_types */

/** @cond 
*/

typedef struct DalInterruptController DalInterruptController;
struct DalInterruptController
{
   struct DalDevice DalDevice;
   DALResult (*RegisterISR)(uint32 ClientId, DalDeviceHandle * _h, DALInterruptID  intrID,
                           const DALISR  isr, const DALISRCtx  ctx, uint32  bEnable);
   DALResult (*RegisterIST)(uint32 ClientId, DalDeviceHandle * _h, DALInterruptID  intrID,
                           const DALISR  isr, const DALISRCtx  ctx, uint32  bEnable,char* pISTName);
   DALResult (*RegisterEvent)(uint32 ClientId, DalDeviceHandle * _h, DALInterruptID  intrID,
                             const DALSYSEventHandle  hEvent, uint32  bEnable);
   DALResult (*Unregister)(uint32 ClientId, DalDeviceHandle * _h, DALInterruptID  intrID);
   DALResult (*InterruptDone)(uint32 ClientId, DalDeviceHandle * _h, DALInterruptID  intrID);
   DALResult (*InterruptEnable)(uint32 ClientId, DalDeviceHandle * _h, DALInterruptID  intrID);
   DALResult (*InterruptDisable)(uint32 ClientId, DalDeviceHandle * _h, DALInterruptID  intrID);
   DALResult (*InterruptTrigger)(uint32 ClientId, DalDeviceHandle * _h, DALInterruptID  intrID);
   DALResult (*InterruptClear)(uint32 ClientId, DalDeviceHandle * _h, DALInterruptID  intrID);
   DALResult (*InterruptStatus)(uint32 ClientId, DalDeviceHandle * _h, DALInterruptID  intrID);
   DALResult (*RegisterIRQHandler)(uint32 ClientId, DalDeviceHandle * _h, DALInterruptID  intrID,
                           const DALIRQ  irq, const DALIRQCtx  ctx, uint32  bEnable);
  DALResult (*SetInterruptTrigger)(uint32 ClientId, DalDeviceHandle * _h, DALInterruptID intrID, uint32 nTrigger);
  DALResult (*IsInterruptPending)(uint32 ClientId, DalDeviceHandle * _h, DALInterruptID intrID,void* bState,uint32 size);
  DALResult (*IsInterruptEnabled)(uint32 ClientId, DalDeviceHandle * _h, DALInterruptID intrID,void* bState,uint32 size);
  DALResult (*MapWakeupInterrupt)(uint32 ClientId, DalDeviceHandle * _h, DALInterruptID intrID,uint32 nWakeupIntID);
  DALResult (*IsAnyInterruptPending)(uint32 ClientId, DalDeviceHandle * _h, uint32* bState,uint32 size);
  DALResult (*Sleep)(uint32 ClientId, DalDeviceHandle * _h, InterruptControllerSleepType sleep);
  DALResult (*Wakeup)(uint32 ClientId, DalDeviceHandle * _h, InterruptControllerSleepType sleep);
  DALResult (*GetInterruptTrigger)(uint32 ClientId, DalDeviceHandle * _h, DALInterruptID  intrID,uint32* eTrigger, uint32 size);
  DALResult (*GetInterruptID)(uint32 ClientId, DalDeviceHandle * _h, const char *szIntrName, uint32* pnIntrID, uint32 size);
  DALResult (*LogState)(uint32 ClientId, DalDeviceHandle * _h,  void *pULog);
};

typedef struct DalInterruptControllerHandle DalInterruptControllerHandle;
struct DalInterruptControllerHandle
{
   uint32 dwDalHandleId;
   const DalInterruptController * pVtbl;
   void * pClientCtxt;
   uint32 dwVtblen;
};

#define DAL_InterruptControllerDeviceAttach(DevId,hDalDevice)\
 DAL_DeviceAttachEx(NULL,DevId,DALINTERRUPTCONTROLLER_INTERFACE_VERSION,hDalDevice)

/** @endcond */

/** @addtogroup register_callback
@{ */

/*==========================================================================

  FUNCTION      DalInterruptController_RegisterISR

==========================================================================*/
/**
  Registers a client callback function.

  This function is used by the client to indicate to the 
  interrupt controller that it wants to start receiving interrupt firing 
  notifications by associating its registered callback function with 
  the specified interrupt. The interrupt is enabled by calling this function.

  The legacy tramp name for this function is tramp_register_isr(). The function
  tramp_set_isr() also can be replaced with DalInterruptController_RegisterISR().
  
  @datatypes
  \ref DALInterruptID \n
  \ref DALISR \n
  \ref DALISRCtx

  @param[in] _h        Handle to the DAL interrupt controller client context.
  @param[in] intrID    ID of the interrupt vector with which the client
                       wants to register a callback. 
  @param[in] isr       Callback function to be registered.
  @param[in] ctx       Parameter associated with the callback function.
  @param[in] IntrFlags Client must use one of the interrupt flags, which
                       contain the interrupt triggers. The trigger values are: \n
                       - DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER -- 0x00000002
                       - DALINTRCTRL_ENABLE_LEVEL_LOW_TRIGGER  -- 0x00000003
                       - DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER -- 0x00000004
                       - DALINTRCTRL_ENABLE_FALLING_EDGE_TRIGGER -- 0x00000005
                         @tablebulletend
  
  @return
  DAL_SUCCESS -- Interrupt was enabled successfully and
                 client notification was registered. \n
  DAL_ERROR -- Client context pointer passed is NULL, or the
               interrupt vector passed is an invalid or unsupported number.

  @dependencies
  None.
*/
static __inline DALResult
DalInterruptController_RegisterISR(DalDeviceHandle * _h, DALInterruptID  intrID,
                           const DALISR  isr, const DALISRCtx  ctx, uint32  IntrFlags)
{
  if(_h != NULL)
  {
    return ((DalInterruptControllerHandle *)_h)->pVtbl->RegisterISR(DALDEVICE_LOCAL_CLIENTID, _h, intrID,
                                                                   isr, ctx, IntrFlags);
  }
  else
  {
    return DAL_ERROR;
  }
}

/*==========================================================================

  FUNCTION      DalInterruptController_RegisterIST

==========================================================================*/
/**
  Registers a client callback function.

  This function is used by the client to indicate to the 
  interrupt controller that it wants to start receiving interrupt firing 
  notifications by associating its registered callback function with 
  the specified interrupt. The interrupt is enabled by calling this function.
  The Client can specify an eight character string that can be attached to the Interrupt Service Thread.
  
  The legacy tramp name for this function is tramp_register_isr(). The function
  tramp_set_isr() also can be replaced with DalInterruptController_RegisterISR().

  @datatypes
  \ref DALInterruptID \n
  \ref DALISR \n
  \ref DALISRCtx

  @param[in] _h        Handle to the DAL interrupt controller client context.
  @param[in] intrID    ID of the interrupt vector with which the client
                       wants to register a callback. 
  @param[in] isr       Callback function to be registered.
  @param[in] ctx       Parameter associated with the callback function.
  @param[in] pISTName       Parameter associated with the client specified name for the interrupt service thread.
  @param[in] IntrFlags Client must use one of the interrupt flags, which
                       contain the interrupt triggers. The trigger values are: \n
                       - DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER -- 0x00000002
                       - DALINTRCTRL_ENABLE_LEVEL_LOW_TRIGGER  -- 0x00000003
                       - DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER -- 0x00000004
                       - DALINTRCTRL_ENABLE_FALLING_EDGE_TRIGGER -- 0x00000005
                         @tablebulletend
 
  @return
  DAL_SUCCESS -- Interrupt was enabled successfully and
                 client notification was registered. \n
  DAL_ERROR -- Client context pointer passed is NULL, or the
               interrupt vector passed is an invalid or unsupported number.
  
  @dependencies
  None.
*/
static __inline DALResult
DalInterruptController_RegisterIST(DalDeviceHandle * _h, DALInterruptID  intrID,
                           const DALISR  isr, const DALISRCtx  ctx, uint32  IntrFlags, char* pISTName)
{
  if(_h != NULL)
  {
    return ((DalInterruptControllerHandle *)_h)->pVtbl->RegisterIST(DALDEVICE_LOCAL_CLIENTID,  _h, intrID,
                                                                   isr, ctx, IntrFlags,pISTName);
  }
  else
  {
    return DAL_ERROR;
  }
}

/*==========================================================================

  FUNCTION      DalInterruptController_RegisterEvent

==========================================================================*/
/**
  Registers a client event handle.

  @note1hang This function is currently not ready to be used by clients.

  This function is used by the client to indicate to the interrupt 
  controller from which it wants to start getting interrupt firing 
  notifications by associating its registered callback event with 
  the specified interrupt.

  @datatypes
  \ref DALInterruptID \n

  @param[in] _h        Handle to the DAL interrupt controller client context.
  @param[in] intrID    ID of the interrupt vector with which the client wants
                       to register a callback.
  @param[in] hEvent    Callback event to be registered.
  @param[in] IntrFlags Client must use the DALINTRCTRL_ENABLE_CONFIG (trigger,
                       priority) macro. The interrupt trigger values are: \n
                       - DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER   -- 0x00000002 \n
                       - DALINTRCTRL_ENABLE_LEVEL_LOW_TRIGGER    -- 0x00000003 \n
                       - DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER  -- 0x00000004 \n
                       - DALINTRCTRL_ENABLE_FALLING_EDGE_TRIGGER -- 0x00000005
                         @tablebulletend
  
  @return
  DAL_SUCCESS -- Interrupt was registered successfully. \n
  DAL_ERROR -- Client context pointer passed is NULL, or the
               interrupt vector passed is an invalid or unsupported number.

  @dependencies
  None.
*/
static __inline DALResult
DalInterruptController_RegisterEvent(DalDeviceHandle * _h, DALInterruptID  intrID,
                            const DALSYSEventHandle  hEvent, uint32  IntrFlags)
{
  if(_h != NULL)
  {
    return ((DalInterruptControllerHandle *)_h)->pVtbl->RegisterEvent(DALDEVICE_LOCAL_CLIENTID,  _h, intrID,
                                                                        hEvent, IntrFlags);
  }
  else
  {
    return DAL_ERROR;
  }                                                                     
}

/** @} */ /* end_addtogroup_register_client_event */

/** @addtogroup unregister_client_cb
@{ */

/*==========================================================================

  FUNCTION      DalInterruptController_Unregister

==========================================================================*/
/**
  Unregisters a client callback.

  This function is used by the client to indicate to the interrupt controller 
  that it wants to stop getting interrupt firing notifications by disconnecting 
  its registered callback with the specified interrupt. The interrupt is
  disabled by calling this function.

  The legacy tramp name for this function is tramp_deregister_isr(). The
  function tramp_set_isr() also can be replaced with
  DalInterruptController_UnregisterISR().

  @datatypes
  \ref DALInterruptID

  @param[in] _h      Handle to the DAL interrupt controller client context.
  @param[in] intrID  ID of the interrupt vector with which the client wants
                     to unregister a callback.
 
  @return
  DAL_SUCCESS -- Interrupt was unregistered successfully. \n
  DAL_ERROR -- Client context pointer passed is NULL, or the
               interrupt vector passed is an invalid or unsupported number.

  @dependencies
  None.
*/
static __inline DALResult
DalInterruptController_Unregister(DalDeviceHandle * _h, DALInterruptID  intrID)
{
  if(_h != NULL)
  {
    return ((DalInterruptControllerHandle *)_h)->pVtbl->Unregister(DALDEVICE_LOCAL_CLIENTID,  _h, intrID);
  }
  else
  {
    return DAL_ERROR;
  }  
}

/** @} */ /* end_addtogroup_unregister_client_cb */

/** @addtogroup interrupt_done
@{ */

/*==========================================================================

  FUNCTION      DalInterruptController_InterruptDone

==========================================================================*/
/**
  Notifies when interrupt processing is completed by the client.

  This function is used by the client to indicate to the interrupt controller 
  that it has finished processing the interrupt and the interrupt state can 
  be cleaned up.

  @datatypes
  \ref DALInterruptID

  @param[in] _h      Handle to the DAL interrupt controller client context.
  @param[in] intrID  ID of the interrupt vector whose state is to be cleaned
                     up.
 
  @return
  DAL_SUCCESS -- Interrupt state was cleaned up successfully.\n
  DAL_ERROR -- Client context pointer passed is NULL, or the
               interrupt vector passed is an invalid or unsupported number.

  @dependencies
  None.
*/
static __inline DALResult
DalInterruptController_InterruptDone(DalDeviceHandle * _h, DALInterruptID  intrID)
{
  if(_h != NULL)
  {
    return ((DalInterruptControllerHandle *)_h)->pVtbl->InterruptDone(DALDEVICE_LOCAL_CLIENTID,  _h, intrID);
  }
  else
  {
    return DAL_ERROR;
  }  
}

/** @} */ /* end_addtogroup_interrupt_done */

/** @addtogroup interrupt_enable
@{ */

/*==========================================================================

  FUNCTION      DalInterruptController_InterruptEnable

==========================================================================*/
/**
  Enables an interrupt in the interrupt controller hardware.
  
  @note1hang Clients are discouraged from using this function. On some
  platforms, this function is not supported, and using it independently may
  cause race conditions (unpredictable behavior).

  This is a new function added to the InterruptController module. It is used
  internally by the interrupt controller. Only in very special driver usage
  conditions is this function sanctioned. Contact Tech Support (see
  Section @latexonly \ref{sec:TechAssist}@endlatexonly) for usage advice
  should such a need arise.

  @datatypes
  \ref DALInterruptID

  @param[in] _h      Handle to the DAL interrupt controller client context.
  @param[in] intrID  ID of the interrupt vector to be enabled.
 
  @return
  DAL_SUCCESS -- Interrupt was enabled successfully. \n
  DAL_ERROR -- Client context pointer passed is NULL, or the
               interrupt vector passed is an invalid or unsupported number.

  @dependencies
  None.
*/
static __inline DALResult
DalInterruptController_InterruptEnable(DalDeviceHandle * _h, DALInterruptID  intrID)
{
  if(_h != NULL)
  {
    return ((DalInterruptControllerHandle *)_h)->pVtbl->InterruptEnable(DALDEVICE_LOCAL_CLIENTID,  _h, intrID);
  }
  else
  {
    return DAL_ERROR;
  }  
}

/** @} */ /* end_addtogroup_interrupt_enable */

/** @addtogroup interrupt_disable
@{ */

/*==========================================================================

  FUNCTION      DalInterruptController_InterruptDisable

==========================================================================*/
/**
  Disables an interrupt.

  This function is used to disable an interrupt in the interrupt controller
  hardware.
  
  @note1hang Clients are discouraged from using this function. Only in very
  special driver usage conditions is this function sanctioned. On some
  platforms, this function is not supported, and using it independently may
  cause race conditions.

  This is a new function added to the interrupt controller module. It is used
  internally by the interrupt controller. Contact Tech Support (see
  Section @latexonly \ref{sec:TechAssist}@endlatexonly) for usage advice
  should such a need arise.

  @datatypes
  \ref DALInterruptID

  @param[in] _h      Handle to the DAL interrupt controller client context.
  @param[in] intrID  ID of the interrupt vector to be disabled.
 
  @return
  DAL_SUCCESS -- Interrupt was disabled successfully. \n
  DAL_ERROR -- Client context pointer passed is NULL, or the
               interrupt vector passed is an invalid or unsupported number.

  @dependencies
  None.
*/
static __inline DALResult
DalInterruptController_InterruptDisable(DalDeviceHandle * _h, DALInterruptID  intrID)
{
  if(_h != NULL)
  {
    return ((DalInterruptControllerHandle *)_h)->pVtbl->InterruptDisable(DALDEVICE_LOCAL_CLIENTID,  _h, intrID);
  }
  else
  {
    return DAL_ERROR;
  }  
}

/** @} */ /* end_addtogroup_interrupt_disable */

/** @addtogroup interrupt_trigger
@{ */

/*==========================================================================

  FUNCTION      DalInterruptController_InterruptTrigger

==========================================================================*/
/**
  Used by the sleep controller to raise an interrupt.

  This function is used to force triggering an interrupt without source
  stimulus in the interrupt controller hardware.

  The legacy tramp name for this function is tramp_trigger_interrupt().

  @datatypes
  \ref DALInterruptID

  @param[in] _h      Handle to the DAL interrupt controller client context.
  @param[in] intrID  ID of the interrupt vector that is to be raised.
 
  @return
  DAL_SUCCESS -- Interrupt was triggered successfully. \n
  DAL_ERROR -- Client context pointer passed is NULL, or the
               interrupt vector passed is an invalid or unsupported number.

  @dependencies
  None.
*/
static __inline DALResult
DalInterruptController_InterruptTrigger(DalDeviceHandle * _h, DALInterruptID  intrID)
{
  if(_h != NULL)
  {
    return ((DalInterruptControllerHandle *)_h)->pVtbl->InterruptTrigger(DALDEVICE_LOCAL_CLIENTID,  _h, intrID);
  }
  else
  {
    return DAL_ERROR;
  }  

}

/** @} */ /* end_addtogroup_interrupt_trigger */

/** @addtogroup interrupt_clear
@{ */

/*==========================================================================

  FUNCTION      DalInterruptController_InterruptClear

==========================================================================*/
/**
  Clears the interrupt status.

  This function is used when drivers are polling for the interrupt and have
  not actually registered for it. Such usage is typically discouraged due to
  a waste of resources. The functionality of the interrupt controller is set
  aside in these cases.

  @datatypes
  \ref DALInterruptID

  @param[in] _h      Handle to the DAL interrupt controller client context.
  @param[in] intrID  ID of the interrupt vector whose pending status is to be
                     queried.
 
  @return
  DAL_SUCCESS -- Interrupt status was cleared successfully. \n
  DAL_ERROR -- Client context pointer passed is NULL, or the
               interrupt vector that was to be cleared passed.

  @dependencies
  None.
*/
static __inline DALResult
DalInterruptController_InterruptClear(DalDeviceHandle * _h, DALInterruptID  intrID)
{
  if(_h != NULL)
  {
    return ((DalInterruptControllerHandle *)_h)->pVtbl->InterruptClear(DALDEVICE_LOCAL_CLIENTID,  _h, intrID);
  }
  else
  {
    return DAL_ERROR;
  }  
}

/** @} */ /* end_addtogroup_interrupt_clear */

/** @addtogroup interrupt_status
@{ */

/*==========================================================================

  FUNCTION      DalInterruptController_InterruptStatus

==========================================================================*/
/**
  Gets the interrupt firing status.

  This function is used to determine whether the specified interrupt 
  vector status is active in the interrupt controller hardware. This is the
  raw interrupt status that indicates whether the interrupt is fired. The state
  is independent of whether the interrupt is enabled in the same hardware.

  The legacy tramp name for this function is tramp_is_interrupt_set().

  @datatypes
  \ref DALInterruptID

  @param[in] _h      Handle to the DAL interrupt controller client context.
  @param[in] intrID  ID of the interrupt vector whose status is to be
                     queried.
 
  @return
  DAL_INTERRUPT_SET -- Interrupt is fired and the status is not zero. \n
  DAL_ERROR -- Client context pointer passed is NULL, or the
               interrupt is not fired.

  @dependencies
  None.
*/
static __inline DALResult
DalInterruptController_InterruptStatus
(
  DalDeviceHandle * _h, 
  DALInterruptID  intrID
)
{
  if(_h != NULL)
  {
    return ((DalInterruptControllerHandle *)_h)->pVtbl->InterruptStatus(DALDEVICE_LOCAL_CLIENTID,  _h, intrID);
  }
  else
  {
    return DAL_ERROR;
  }  
}

/** @} */ /* end_addtogroup_interrupt_status */


/** @cond 
*/

/*==========================================================================

  FUNCTION      DalInterruptController_RegisterIRQHandler
  
==========================================================================*/
/**
  This function registers a client callback for a particular interrupt. 
  The client callback is triggered when this interrupt fires. 
  Currently this api is being deprecated.    \n  
*/

static __inline DALResult
DalInterruptController_RegisterIRQHandler(DalDeviceHandle * _h, DALInterruptID  intrID,const DALIRQ  irq, const DALIRQCtx  ctx, uint32  bEnable)
{
  if(_h != NULL)
  {
    return ((DalInterruptControllerHandle *)_h)->pVtbl->RegisterIRQHandler(DALDEVICE_LOCAL_CLIENTID,  _h, intrID,
                                                                   irq, ctx, bEnable);
  }
  else
  {
    return DAL_ERROR;
  }  
}

/** @endcond 
*/

/** @addtogroup set_trigger
@{ */

/*==========================================================================

  FUNCTION      DalInterruptController_SetTrigger

==========================================================================*/
/**
  Sets the trigger of an interrupt.

  The legacy tramp name for this function is tramp_set_interrupt_trigger().

  @datatypes
  \ref DALInterruptID

  @param[in] _h        Handle to the DAL interrupt controller client context.
  @param[in] intrID    ID of the interrupt vector whose trigger is to be set.
  @param[in] nTrigger  Trigger condition to set the interrupt to fire. The
                       interrupt trigger values are: \n
                       - DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER   -- 0x00000002 \n                 
                       - DALINTRCTRL_ENABLE_LEVEL_LOW_TRIGGER    -- 0x00000003 \n                   
                       - DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER  -- 0x00000004 \n                  
                       - DALINTRCTRL_ENABLE_FALLING_EDGE_TRIGGER -- 0x00000005
                         @tablebulletend

  @return
  DAL_SUCCESS -- Interrupt trigger was set successfully. \n
  DAL_ERROR -- Client context pointer passed is NULL, or the
               interrupt vector passed is an invalid or unsupported number.
 
  @dependencies
  None.
*/
static __inline DALResult
DalInterruptController_SetTrigger(DalDeviceHandle * _h, DALInterruptID  intrID,uint32 nTrigger)
{
  if(_h != NULL)
  {
    return ((DalInterruptControllerHandle *)_h)->pVtbl->SetInterruptTrigger(DALDEVICE_LOCAL_CLIENTID,  _h, 
            intrID, nTrigger);
  }
  else
  {
    return DAL_ERROR;
  }  
}

/** @} */ /* end_addtogroup_set_trigger */

/** @addtogroup is_interrupt_pending
@{ */

/*==========================================================================

  FUNCTION      DalInterruptController_IsInterruptPending

==========================================================================*/
/**
  Gets the interrupt pending status.

  This function is used to determine whether the specified interrupt vector 
  is pending in the interrupt controller hardware. For an interrupt to be 
  pending, it must be enabled and its status must be nonzero.

  The legacy tramp name for this function is tramp_is_interrupt_pending().

  @datatypes
  \ref DALInterruptID

  @param[in] _h      Handle to the DAL interrupt controller client context.
  @param[in] intrID  ID of the interrupt vector whose pending status is to be
                     queried.
  @param[in] bState  Pointer to a uint32 variable allocated by the client 
                     calling the function. This points to 1 if the interrupt is 
                     pending (i.e., the interrupt is set and enabled). If the
                     interrupt is not pending, it containes a zero.
  
  @return
  DAL_SUCCESS -- Interrupt pending state was queried successfully.\n
  DAL_ERROR -- Client context pointer passed is NULL, or the
               interrupt vector passed is an invalid or unsupported number.
 
  @dependencies
  None.
*/
static __inline DALResult
DalInterruptController_IsInterruptPending(DalDeviceHandle * _h, DALInterruptID  intrID,uint32* bState)
{
  if(_h != NULL)
  {
    return ((DalInterruptControllerHandle *)_h)->pVtbl->IsInterruptPending(DALDEVICE_LOCAL_CLIENTID,  _h, 
            intrID, (void*)bState,1);
  }
  else
  {
    return DAL_ERROR;
  } 
}

/** @} */ /* end_addtogroup_is_interrupt_pending */

/** @addtogroup is_interrupt_enabled
@{ */

/*==========================================================================

  FUNCTION      DalInterruptController_IsInterruptEnabled

==========================================================================*/
/**
  Gets the interrupt status.

  This function is used to determine whether the specified interrupt vector
  is enabled in the interrupt controller hardware.

  The legacy tramp name for this function is tramp_is_interrupt_enabled().

  @datatypes
  \ref DALInterruptID

  @param[in] _h          Handle to the DAL interrupt controller client context.
  @param[in] intrID      ID of the interrupt vector whose enabled status is to
                         be queried.
  @param[in,out] bState  Pointer to a uint32 variable allocated by the client 
                         calling the function. Values: \n
                         - 1 -- There is a valid interrupt that is enabled. \n
                         - 0 -- There is no valid interrupt that is enabled.
  
  @return
  DAL_SUCCESS -- Interrupt enabled state was queried successfully. \n
  DAL_ERROR -- Client context pointer passed is NULL, or the
               interrupt vector passed is an invalid or unsupported number.
 
  @dependencies
  None.
*/
static __inline DALResult
DalInterruptController_IsInterruptEnabled(DalDeviceHandle * _h, DALInterruptID  intrID,uint32* bState)
{
  if(_h != NULL)
  {
    return ((DalInterruptControllerHandle *)_h)->pVtbl->IsInterruptEnabled(DALDEVICE_LOCAL_CLIENTID,  _h, 
           intrID, (void*)bState,1);
  }
  else
  {
    return DAL_ERROR;
  } 
}

/** @} */ /* end_addtogroup_is_interrupt_enabled */

/** @addtogroup map_wakeup_interrupt
@{ */

/*==========================================================================

  FUNCTION      DalInterruptController_MapWakeupInterrupt

==========================================================================*/
/**
  Used by the sleep controller to mark its interrupts in 
  the generic interrupt controller.

  This function is called by the sleep controller during its initialization 
  when it goes through the list of interrupt vectors that are connected as 
  wakeup sources in the always-on MPM interrupt controller. It maps these
  with the main interrupt controller. Should a client choose to register an 
  interrupt with the main interrupt controller, this MPM ID is queried and 
  the MPM interrupt controller is configured for this interrupt.
  
  The legacy tramp name for this function is tramp_map_mpm_interrupt().

  @datatypes
  \ref DALInterruptID

  @param[in] _h           Handle to the DAL interrupt controller client
                          context.
  @param[in] intrID       Interrupt vector number.
  @param[in] WakeupIntID  Wakeup MPM interrupt ID to be mapped with the
                          interrupt vector.
 
  
  @return
  DAL_SUCCESS -- Wakeup interrupt was mapped successfully. \n
  DAL_ERROR -- Client context pointer passed is NULL, or the
               interrupt vector passed is an invalid or unsupported number.
 
  @dependencies
  None.
*/
static __inline DALResult
DalInterruptController_MapWakeupInterrupt(DalDeviceHandle * _h, DALInterruptID  intrID, uint32 WakeupIntID)
{
  if(_h != NULL)
  {
    return ((DalInterruptControllerHandle *)_h)->pVtbl->MapWakeupInterrupt(DALDEVICE_LOCAL_CLIENTID,  _h, 
           intrID, WakeupIntID);
  }
  else
  {
    return DAL_ERROR;
  } 
}

/** @} */ /* end_addtogroup map__wakeup_interrupt */

/** @addtogroup is_any_interrupt_pending
@{ */

/*==========================================================================

  FUNCTION      DalInterruptController_IsAnyInterruptPending

==========================================================================*/
/**
  Checks if any interrupt is pending.

  This function is called by the sleep controller when it prepares for one 
  of the sleep modes. This is to check if, between the time the sleep
  controller cleans up its state and enables the always-on Modem Power Manager
  (MPM) interrupt controller, there are any interrupts pending. If so, it
  cancels its sleep procedure and lets the main interrupt controller process 
  the interrupt.

  The legacy tramp name for this function is tramp_is_any_interrupt_pending().

  @param[in] _h          Handle to the DAL interrupt controller client context.
  @param[in,out] bState  Pointer to a uint32 variable allocated by the client
                         calling the function. Values: \n
                         - 1 -- There is a valid interrupt that is pending
                                (i.e., the interrupt is set and enabled). \n
                         - 0 -- There is no interrupt pending.
 
  @return
  DAL_SUCCESS -- Interrupt state was queried successfully. \n
  DAL_ERROR -- Client context pointer passed is NULL.
 
  @dependencies
  None.

  @sa
  DalInterruptController_IsInterruptPending()
*/
static __inline DALResult
DalInterruptController_IsAnyInterruptPending(DalDeviceHandle * _h, uint32* bState)
{
  if(_h != NULL)
  {
    return ((DalInterruptControllerHandle *)_h)->pVtbl->IsAnyInterruptPending(DALDEVICE_LOCAL_CLIENTID,  _h, 
           (uint32*)bState,1);
  }
  else
  {
    return DAL_ERROR;
  } 
}

/** @} */ /* end_addtogroup is_any_interrupt_pending */

/** @addtogroup controller_sleep
@{ */

/*==========================================================================

  FUNCTION      DalInterruptController_Sleep

==========================================================================*/
/**
  Called by the sleep controller when it prepares for one of the sleep modes.
  Depending on the interrupt hardware platform, the interrupt controller does
  a number of houskeeping tasks to save its state before the 
  MSM&tm;&nbsp; goes to sleep.

  The legacy tramp name for this function is tramp_sleep().

  @datatypes
  \ref InterruptControllerSleepType

  @param[in] _h     Handle to the DAL interrupt controller client context.
  @param[in] sleep  Type of sleep the MSM is entering.
  
  @return
  DAL_SUCCESS -- Interrupt state was correctly saved while
                 entering the Sleep mode. \n
  DAL_ERROR -- Client context pointer passed is NULL.

  @dependencies
  None.

  @sa
  DalInterruptController_Wakeup()
*/

static __inline DALResult
DalInterruptController_Sleep(DalDeviceHandle * _h,InterruptControllerSleepType sleep)
{
  if(_h != NULL)
  {
    return ((DalInterruptControllerHandle *)_h)->pVtbl->Sleep(DALDEVICE_LOCAL_CLIENTID,  _h, sleep);
  }
  else
  {
    return DAL_ERROR;
  } 
}

/** @} */ /* end_addtogroup controller_sleep */

/** @addtogroup controller_wakeup
@{ */

/*==========================================================================

  FUNCTION      DalInterruptController_Wakeup

==========================================================================*/
/**
  Called by the sleep controller when it starts up the system after it gets an
  interrupt. Depending on the interrupt hardware platform, the interrupt
  controller does a number of houskeeping tasks to get it into the 
  right state for the Online mode.

  The legacy tramp name for this function is tramp_wakeup().

  @datatypes
  \ref InterruptControllerSleepType

  @param[in] _h     Handle to the DAL interrupt controller client context.
  @param[in] sleep  Type of sleep the MSM is exiting.
  
  @return
  DAL_SUCCESS -- Interrupt state was correctly restored after 
                 exiting the Sleep mode. \n
  DAL_ERROR -- Client context pointer passed is NULL.

  @dependencies
  None.

  @sa
  DalInterruptController_Sleep()
*/

static __inline DALResult
DalInterruptController_Wakeup(DalDeviceHandle * _h, InterruptControllerSleepType sleep)
{
  if(_h != NULL)
  {
    return ((DalInterruptControllerHandle *)_h)->pVtbl->Wakeup(DALDEVICE_LOCAL_CLIENTID,  _h, sleep);
  }
  else
  {
    return DAL_ERROR;
  } 
}

/** @} */ /* end_addtogroup controller_wakeup */

/** @addtogroup get_interrupt_trigger
@{ */

/*==========================================================================

  FUNCTION      DalInterruptController_GetInterruptTrigger

==========================================================================*/
/**
  Gets the configured interrupt's trigger in the hardware.

  This function reads the interrupt fire trigger set for the interrupt in the
  hardware.

  @datatypes
  \ref DALInterruptID

  @param[in] _h            Handle to the DAL interrupt controller client
                           context.
  @param[in] intrID        Interrupt vector whose interrupt trigger is being 
                           queried.
  @param[in,out] eTrigger  Pointer to a uint32 variable sent by the client.
                           Depending on the trigger being set, it can have one
                           of the following trigger values: \n
                           - DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER   -- 0x00000002
                           - DALINTRCTRL_ENABLE_LEVEL_LOW_TRIGGER    -- 0x00000003
                           - DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER  -- 0x00000004
                           - DALINTRCTRL_ENABLE_FALLING_EDGE_TRIGGER -- 0x00000005
                             @tablebulletend

  @return
  DAL_SUCCESS -- Interrupt was valid and the interrupt controller 
  driver was able to read the interrupt trigger correctly. \n
  DAL_ERROR -- Returned for any of the following reasons: \n
  - The client context pointer passed is NULL. \n
  - The interrupt vector passed is an invalid or unsupported number. \n
  - The trigger pointer passed is a NULL pointer.

  @dependencies
  None.

  @sa
  DalInterruptController_SetInterruptTrigger()
*/

static __inline DALResult
DalInterruptController_GetInterruptTrigger(DalDeviceHandle * _h, DALInterruptID  intrID,uint32* eTrigger)
{
  if(_h != NULL)
  {
    return ((DalInterruptControllerHandle *)_h)->pVtbl->GetInterruptTrigger(DALDEVICE_LOCAL_CLIENTID,  _h, 
             intrID,eTrigger,1);
  }
  else
  {
    return DAL_ERROR;
  } 
}

/** @} */ /* end_addtogroup get_interrupt_trigger */

/** @addtogroup get_interrupt_id
@{ */

/*==========================================================================

  FUNCTION      DalInterruptController_GetInterruptID

==========================================================================*/
/**
  Gets the interrupt vector by matching the name provided by the user.

  This function provides the 

  @datatypes
  \ref DALInterruptID

  @param[in] _h            Handle to the DAL interrupt controller client
                           context.
  @param[in] szIntrName    Interrupt string name whose interrupt vector is being 
                           queried.
  @param[in,out] pnIntrID  Pointer to a uint32 variable sent by the client.
                           Depending on the string name being set, it can have any of the supported vectors
                           if there is no match an invalid value of 7FFF is passed back by the driver. \n
                             @tablebulletend

  @return
  DAL_SUCCESS -- Interrupt name was valid and the interrupt controller 
  driver was able to fetch the interrupt vector correctly. \n
  DAL_ERROR -- Returned for any of the following reasons: \n
  - The client context pointer passed is NULL. \n
  - The interrupt name passed is an invalid or matches an unsupported interrupt vector number. \n
  - The user defined uint32 vector pointer is null.

  @dependencies
  None.
*/

static __inline DALResult
DalInterruptController_GetInterruptID(DalDeviceHandle * _h, const char * szIntrName,DALInterruptID*  pnIntrID)

{
  if(_h != NULL)
  {
    return ((DalInterruptControllerHandle *)_h)->pVtbl->GetInterruptID(DALDEVICE_LOCAL_CLIENTID,  _h, szIntrName, pnIntrID,1);
  }
  else
  {
    return DAL_ERROR;
  } 
}
/** @} */ /* end_addtogroup get_interrupt_id */

/** @addtogroup log_interrupt_state
@{ */

/* =========================================================================
**  Function : DalInterruptController_LogState
** =========================================================================*/
/**
  Logs the state of the clock driver.

  This function logs the state of all interruptd
 
  @param _h     [in]  Handle to the DAL device.
  @param pULog  [in]  ULog in which to log data.

  @return
  DAL_SUCCESS -- State logged successfully.
  DAL_ERROR   -- ULog handle is NULL, or there was an error writing to the ULog.
*/

static __inline DALResult
DalInterruptController_LogState(DalDeviceHandle * _h,  void *pULog)
{
   return ((DalInterruptControllerHandle *)_h)->pVtbl->LogState(DALDEVICE_LOCAL_CLIENTID,  _h, pULog);
}


/** @} */ /* end_addtogroup log_interrupt_state */

#endif
