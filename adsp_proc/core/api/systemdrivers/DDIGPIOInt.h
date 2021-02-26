#ifndef __GPIOInt_H__
#define __GPIOInt_H__
/*===========================================================================*/

  /* D A L   G P I O   I N T E R R U P T   C O N T R O L L E R    */

/**
  @file DDIGPIOInt.h 
  @brief This file contains the API specification for the Dal GPIO interrupt controller device 
  driver.The definitions in this module are used externally by the clients of the GPIO 
  interrupt controller.
*/

/* NOTE: For the output PDF generated using Doxygen and Latex, all file and 
         group descriptions are maintained in the mainpage file. To change 
         any of the the file/group text for the PDF, edit this 
         file, or contact Tech Pubs.
         The above description for this file is part of the "gpioint_api" group 
         description in the mainpage file. 
*/

/*===========================================================================
       Copyright (c) 2010 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/api/systemdrivers/DDIGPIOInt.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
1/1/2010   aratin  First draft created. 
===========================================================================*/
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DALSys.h"
#include "DALStdErr.h"

/** @addtogroup gpioint_macros
@{ */

/**
  Defines the interface version for GPIO Interrupt Driver.
 */
#define GPIOINT_INTERFACE_VERSION DALINTERFACE_VERSION(1,0)

/**
  This macro defines an invalid value that tells the different processors how to treat the GPIO configuration.
 */
#define GPIOINT_NONE  0xFFFF
/** @cond */
/**
  This macro defines the default class of GPIO interrupts.
  Summary GPIO interrupts are supported on all qualcomm chipsets. 
  These are leaf interrupts that each general purpose IO pin can be configured to.
  These interrupts have a single interrupt line feeding into the main interrupt controller of each processor subsystem.
  Direct connect GPIOs is a class of interrupts that was added on the new Qualcomm chipsets. Essentially any GPIO
  can be connected as a direct connect interrupt. This means that the interrupt now has a dedicated interrupt port on the Main
  Interrupt controller on the processor. Since The Direct connect Interrupts have a costly overhead on hardware there are only a few
  dedicated lines and these are hardwired to each processor in the subsystem. \n
 */
#define GPIOINT_SUMMARY 0x7fffffff
/** @endcond */
/**
  These macros define the polarity of the GPIO Interrupt trigger condition.
  A positive polarity can be associated with a rising edge trigger and a level high trigger. \n
  A negative polarity can be associated with a falling edge trigger and a level low trigger. \n
 */
#define GPIOINT_POLARITY_POSITIVE 1
#define GPIOINT_POLARITY_NEGATIVE 0


/**
  These macros define where the GPIOInterrupt is going to be 
  used.\n 
 */
#define GPIOINT_ISLAND_MODE 1
#define GPIOINT_NORMAL_MODE 0

/** @} */ /* end_gpioint_macros */

/** @addtogroup gpioint_typedefs
@{ */

typedef uint32 GPIOINTISRCtx; /**< This is the parameter to be passed to client callback when the gpio interrupt fires. */
typedef void * (*GPIOINTISR)(GPIOINTISRCtx); /**< This is the void function pointer client callback to be registered with the GPIOInt driver. */

/** @} */ /* end_gpioint_typedefs */

/** @addtogroup gpioint_enums
@{ */

/**
  This enum is used to define the destination of the GPIO interrupt i.e. the receiving processor for the incoming interrupt.
  In multiprocessor subsystem the Top level mode Mux pins or general purpose IO pins (GPIO)
  are shared between processors. Hence it becomes necessary to define their destination processor.
 */
typedef enum{
  GPIOINT_DEVICE_MODEM, /**< The Modem device is usually the QDSP6 processor. */
  GPIOINT_DEVICE_SPS,    /**< The SPS device usually is the Sensors processor. */
  GPIOINT_DEVICE_LPA_DSP,  /**< The LPA DSP device is usually the LPASS processor */
  GPIOINT_DEVICE_RPM,  /**< The RPM device usually is the resource power manager processor */
  GPIOINT_DEVICE_APPS, /**< The APPS device usually is the applications processor */
  GPIOINT_DEVICE_WCN, /**< The WCN device usually is the RIVA processor */
  GPIOINT_DEVICE_DSP, /**< On legacy 9k devices the DSP is the modem processor */
  GPIOINT_DEVICE_TURING, /**< Turing processor for compute */
  GPIOINT_DEVICE_NONE, /**< The default target processor value. */
/** @cond */
  PLACEHOLDER_GPIOIntProcessorType = 0x7fffffff
/** @endcond */
}GPIOIntProcessorType;


/**
  This enum is used to define the trigger condition of the GPIO interrupt.
 */
typedef enum{
  GPIOINT_TRIGGER_HIGH,   /**< The GPIO interrupt will trigger only if the input signal is high */
  GPIOINT_TRIGGER_LOW,  /**< The GPIO interrupt will trigger only if the input signal is low */
  GPIOINT_TRIGGER_RISING, /**< The GPIO interrupt will trigger only if the input signal level transitions from low to high */
  GPIOINT_TRIGGER_FALLING, /**< The GPIO interrupt will trigger only if the input signal level transitions from high to low */
  GPIOINT_TRIGGER_DUAL_EDGE, /**< The GPIO interrupt will trigger only if the input signal level transitions from high to low or from low to high.*/
  GPIOINT_TRIGGER_INVALID, /**< Enum to check for invalid trigger types which might be passed. Client should not use this.*/
  /** @cond */
  PLACEHOLDER_GPIOIntTriggerType = 0x7fffffff 
  /** @endcond */
}GPIOIntTriggerType;
  
/** @} */ /* end_gpioint_enums */  

/** @cond 
*/ /* Internal GPIOInt function pointer structure to be skipped by Documentation */
typedef struct GPIOInt GPIOInt;
struct GPIOInt
{
   struct DalDevice DalDevice;
   DALResult (*SetTrigger)(uint32 ClientId, DalDeviceHandle * _h, uint32  gpio, GPIOIntTriggerType  trigger);
   DALResult (*RegisterIsr)(uint32 ClientId, DalDeviceHandle * _h, uint32 gpio,GPIOIntTriggerType  trigger,
                            GPIOINTISR isr,GPIOINTISRCtx param);
   DALResult (*RegisterEvent)(uint32 ClientId, DalDeviceHandle * _h, uint32 gpio, GPIOIntTriggerType trigger,
                              DALSYSEventHandle event);
   DALResult (*DeRegisterEvent)(uint32 ClientId, DalDeviceHandle * _h, uint32 gpio,DALSYSEventHandle event);
   DALResult (*DeregisterIsr)(uint32 ClientId, DalDeviceHandle * _h, uint32  gpio, GPIOINTISR  isr);
   DALResult (*IsInterruptEnabled)(uint32 ClientId, DalDeviceHandle * _h, uint32  gpio, void *  state, uint32  size);
   DALResult (*IsInterruptPending)(uint32 ClientId, DalDeviceHandle * _h, uint32  gpio, void *  state, uint32  size);
   DALResult (*Save)(uint32 ClientId, DalDeviceHandle * _h);
   DALResult (*Restore)(uint32 ClientId, DalDeviceHandle * _h);
   DALResult (*DisableInterrupt)(uint32 ClientId, DalDeviceHandle * _h, uint32 gpio);
   DALResult (*EnableInterrupt)(uint32 ClientId, DalDeviceHandle * _h, uint32 gpio);
   DALResult (*InterruptNotify)(uint32 ClientId, DalDeviceHandle * _h, uint32 gpio);
   DALResult (*MonitorInterrupts)(uint32 ClientId, DalDeviceHandle * _h,  GPIOINTISR isr,uint32 enable);
   DALResult (*MapMPMInterrupt)(uint32 ClientId, DalDeviceHandle * h, uint32 gpio, uint32 mpm_interrupt_id);
   DALResult (*AttachRemote)(uint32 ClientId, DalDeviceHandle * h, uint32 processor);
   DALResult (*TriggerInterrupt)(uint32 ClientId, DalDeviceHandle * h, uint32 gpio);
   DALResult (*ClearInterrupt)(uint32 ClientId, DalDeviceHandle * h, uint32 gpio);
   DALResult (*IsInterruptSet)(uint32 ClientId, DalDeviceHandle * _h, uint32  gpio, void *  state, uint32  size);
   DALResult (*SetDirectConnectGPIOMapping)(uint32 ClientId, DalDeviceHandle * _h, uint32  gpio, uint32 direct_connect_line);
   DALResult (*IsInterruptRegistered)(uint32 ClientId, DalDeviceHandle * _h, uint32  gpio, void *  state, uint32  size);
   DALResult (*DisableGPIOInterrupt)(uint32 ClientId, DalDeviceHandle * _h, uint32 gpio);
   DALResult (*EnableGPIOInterrupt)(uint32 ClientId, DalDeviceHandle * _h, uint32 gpio);
   DALResult (*GPIOInt_RegisterIsrEx)(uint32 ClientId, DalDeviceHandle * _h, uint32 gpio,GPIOIntTriggerType  trigger,
                            GPIOINTISR isr,GPIOINTISRCtx param, uint32 nFlags);
};

typedef struct GPIOIntHandle GPIOIntHandle; 
struct GPIOIntHandle 
{
   uint32 dwDalHandleId;
   const GPIOInt * pVtbl;
   void * pClientCtxt;
   uint32 dwVtblen;
};

#define GPIOIntDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,GPIOInt_INTERFACE_VERSION,hDalDevice)

/** @endcond */

/** @addtogroup gpioint_apis
@{ */
/* =========================================================================
**  Function : GPIOInt_SetTrigger
** =========================================================================*/
/**
  Sets the trigger of a given GPIO interrupt.

  This function configures the GPIO pin interrupts trigger. \n
  The legacy tramp_gpio name for this API is tramp_gpio_set_trigger

  @param[in] _h The client context pointer of the interrupt controller DAL.
  @param[in] gpio       The GPIO pin number whose interrupt trigger needs to be changed.
  @param[in] trigger    The new interrupt trigger type to be changed to.
  @return
  DAL_SUCCESS -- is returned if the GPIO interrupt trigger is successfuly changed. \n
  DAL_ERROR -- is returned if the GPIO is unsupported or if the trigger type is invalid.
  @dependencies
  None.
*/

static __inline DALResult
GPIOInt_SetTrigger(DalDeviceHandle * _h, uint32  gpio, GPIOIntTriggerType  trigger)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_1(DALVTBLIDX(((GPIOIntHandle *)_h)->pVtbl, SetTrigger ), _h, gpio, trigger);
   }
   return ((GPIOIntHandle *)_h)->pVtbl->SetTrigger(DALDEVICE_LOCAL_CLIENTID, _h, gpio, trigger);
}


/* =========================================================================
**  Function : GPIOInt_RegisterIsr
** =========================================================================*/
/**
  Registers a client callback notification for a GPIO interrupt.

  This function registers the client callback isr routine to a given GPIO 
  interrupt.Whenever an interrupt is fired the isr routine registered by the 
  user is called. \n
  The legacy tramp_gpio name for this API is tramp_gpio_register_isr.

  @param[in] _h        The client context pointer of the GPIO interrupt controller DAL.
  @param[in] gpio      The GPIO pin number.
  @param[in] trigger   The interrupt trigger condition for which the client callback is being registered.
  @param[in] isr       The client ISR callback.
  @param[in] param     The Client specified parameter to be given to the client callback when the interrupt fires. 
  
  DEPENDENCIES  None.
  @return
  DAL_SUCCESS -- Is returned if the ISR registration was a success. \n
  DAL_ERROR -- An Error if the GPIOInt driver was not able to register the interrupt
  service routine (ISR). This can happen if the GPIO interrupt already has a 
  prior event handle or a callback function interrupt Service routine registered 
  to it which is different from the current one in attempt at event registration. 
  An error is also returned if the GPIO pin is out of bounds or not supported. \n
  @dependencies
  None.
*/

static __inline DALResult
GPIOInt_RegisterIsr(DalDeviceHandle * _h, uint32 gpio,GPIOIntTriggerType trigger,
GPIOINTISR isr,GPIOINTISRCtx param)
{
   if(!DALISREMOTEHANDLE(_h))
   {
     return ((GPIOIntHandle *)_h)->pVtbl->RegisterIsr(DALDEVICE_LOCAL_CLIENTID, _h, gpio, trigger,isr,param);
   }
   else
       return  DAL_ERROR;
}

/* =========================================================================
**  Function : GPIOInt_RegisterIsrEX
** =========================================================================*/
/**
  Registers a client callback notification for a GPIO interrupt.

  This function registers the client callback isr routine to a given GPIO 
  interrupt.Whenever an interrupt is fired the isr routine registered by the 
  user is called. \n
  The legacy tramp_gpio name for this API is tramp_gpio_register_isr.

  @param[in] _h        The client context pointer of the GPIO interrupt controller DAL.
  @param[in] gpio      The GPIO pin number.
  @param[in] trigger   The interrupt trigger condition for which the client callback is being registered.
  @param[in] isr       The client ISR callback.
  @param[in] param     The Client specified parameter to be given to the client callback when the interrupt fires.
  @param[in] nFlags    The Client specified flags to be given to the interrupt controller so that internally it
                       can decide whether the interrupt is to be used in Uimage or big image. \n
                       For uimage the user need to send GPIOINT_ISLAND_MODE 1 \n
                       For normal image user can send GPIOINT_NORMAL_MODE 0 \n
  
  DEPENDENCIES  None.
  @return
  DAL_SUCCESS -- Is returned if the ISR registration was a success. \n
  DAL_ERROR -- An Error if the GPIOInt driver was not able to register the interrupt
  service routine (ISR). This can happen if the GPIO interrupt already has a 
  prior event handle or a callback function interrupt Service routine registered 
  to it which is different from the current one in attempt at event registration. 
  An error is also returned if the GPIO pin is out of bounds or not supported. \n
  @dependencies
  None.
*/

static __inline DALResult
GPIOInt_RegisterIsrEx(DalDeviceHandle * _h, uint32 gpio,GPIOIntTriggerType trigger,
GPIOINTISR isr,GPIOINTISRCtx param, uint32 nFlags)
{
   if(!DALISREMOTEHANDLE(_h))
   {
     return ((GPIOIntHandle *)_h)->pVtbl->GPIOInt_RegisterIsrEx(DALDEVICE_LOCAL_CLIENTID, _h, gpio, trigger,isr,param, nFlags);
   }
   else 
     return  DAL_ERROR;
}

/*=============================================================================
  FUNCTION      GPIOInt_RegisterEvent
=============================================================================*/
/**
  Registerts a client event notification for a GPIO interrupt.

  This function registers the client event to a given GPIO 
  interrupt.Whenever an interrupt is fired the event handle registered by the 
  user is called. This is a new API added to the GPIOInt module.

  @param[in] _h         The client context pointer of the GPIO interrupt controller DAL.
  @param[in] gpio       The GPIO pin number.
  @param[in] trigger    The interrupt trigger condition for which the client callback event is being registered.
  @param[in] event      The client event handle callback.
 
  
  DEPENDENCIES  None.
  @return
  DAL_SUCCESS -- Is returned if the ISR registration was a success. \n
  DAL_ERROR -- An Error if the GPIOInt driver was not able to register
 the interrupt trigger event. This can happen if the GPIO interrupt already has a 
  prior event handle or a callback function interrupt Service routine registered 
  to it which is different from the current one in attempt at event registration. 
  An error is also returned if the GPIO pin is out of bounds or not supported. \n
  @dependencies
  None.
*/

static __inline DALResult
GPIOInt_RegisterEvent(DalDeviceHandle * _h, uint32 gpio, 
                                         GPIOIntTriggerType trigger,DALSYSEventHandle event)
{
   if(!DALISREMOTEHANDLE(_h))
   {
     return ((GPIOIntHandle *)_h)->pVtbl->RegisterEvent(DALDEVICE_LOCAL_CLIENTID, _h, gpio, trigger, event);
   }
   else
       return  DAL_ERROR;
}

/*=============================================================================
  FUNCTION      GPIOInt_DeRegisterEvent
=============================================================================*/
/**
  Deregisters the client callback event to a given GPIO interrupt.

  This function deregisters the client callback event to a given GPIO 
  interrupt.As a result of which the interrupt is disabled. \n
  This is a new API added to the GPIOInt module.

  @param[in] _h         The client context pointer of the GPIO interrupt controller DAL.
  @param[in] gpio       The GPIO pin number.
  @param[in] event      The client event handle callback.
 
  
  DEPENDENCIES  None.
  @return
  DAL_SUCCESS -- Is returned if the event deregistration was a success. \n
  DAL_ERROR -- an Error if the GPIOInt driver was not able to deregister 
  the client event. This can happen if the GPIO interrupt already has a prior 
  event handle or a callback function interrupt Service routine registered to it 
  which is different from the current one in attempt at event deregistration. 
  An error is also returned if the GPIO pin is out of bounds or not supported.\n
  @dependencies
  None.
*/

static __inline DALResult
GPIOInt_DeRegisterEvent(DalDeviceHandle * _h, uint32 gpio,DALSYSEventHandle event)
{
   if(!DALISREMOTEHANDLE(_h))
   {
     return ((GPIOIntHandle *)_h)->pVtbl->DeRegisterEvent(DALDEVICE_LOCAL_CLIENTID, _h, gpio,event);
   }
   else
     return  DAL_ERROR;
}


/*=============================================================================
  FUNCTION      GPIOInt_DeregisterIsr
=============================================================================*/
 /**
  Deregisters the client callback routine to a given GPIO interrupt.

  This function deregisters the client callback ISR function to a given GPIO 
  interrupt.As a result of which the interrupt is disabled. \n
  The legacy tramp_gpio name for this API is tramp_gpio_deregister_isr.

  @param[in] _h         The client context pointer of the GPIO interrupt controller DAL.
  @param[in] gpio       The GPIO pin number.
  @param[in] isr        The client interrupt service callback.
 
  
  DEPENDENCIES  None.
  @return
  DAL_SUCCESS -- Is returned if the ISR deregistration was a success. \n
  DAL_ERROR -- Is returned if the GPIOInt driver was not able to deregister 
  the client interrupt service routine. This can happen if the GPIO interrupt already has a prior 
  event handle or a callback function interrupt Service routine registered to it 
  which is different from the current one in attempt at event deregistration. 
  An error is also returned if the GPIO pin is out of bounds or not supported.\n
  @dependencies
  None.
*/

static __inline DALResult
GPIOInt_DeregisterIsr(DalDeviceHandle * _h, uint32  gpio, GPIOINTISR  isr)
{
  if(!DALISREMOTEHANDLE(_h))
  {
    return ((GPIOIntHandle *)_h)->pVtbl->DeregisterIsr(DALDEVICE_LOCAL_CLIENTID, _h, gpio, isr);
  }
  else
    return  DAL_ERROR;
}


/*=============================================================================
  FUNCTION      GPIOInt_IsInterruptEnabled
=============================================================================*/
/**
  Queries the GPIO interrupt enable state.

  This function tells the user if the given GPIO interrupt is 
  enabled or not. For Direct connect interrupts. It tells the user if the main
  level 1 interrupt to which the secondary GPIO interrupt line is hooked up to 
  is enabled. \n
  The legacy tramp_gpio name for this API is tramp_gpio_is_interrupt_enabled.

  @param[in] _h         The client context pointer of the GPIO interrupt controller DAL.
  @param[in] gpio       The GPIO pin number.
  @param[out]  state     The pointer to a client allocated uint32 variable which is populated by the GPIOInt 
                           driver to indicate whether the interrupt is enabled or not.
 
  
  DEPENDENCIES  None.
  @return
  DAL_SUCCESS -- Is returned if the gpio state query was successful. \n
  DAL_ERROR --  Is returned if the GPIO is unsupported. \n
  @dependencies
  None.
*/

static __inline DALResult
GPIOInt_IsInterruptEnabled(DalDeviceHandle * _h, uint32  gpio, uint32*  state)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_6(DALVTBLIDX(((GPIOIntHandle *)_h)->pVtbl, IsInterruptEnabled ), _h, gpio, (void *)state, 1);
   }
   return ((GPIOIntHandle *)_h)->pVtbl->IsInterruptEnabled(DALDEVICE_LOCAL_CLIENTID, _h, gpio, (void *)state, 1);
}


/*=============================================================================
  FUNCTION      GPIOInt_IsInterruptPending
=============================================================================*/
/**
  Queries the GPIO interrupt pending state.

  This function tells the user if the given gpio interrupt is pending or not. \n
  The legacy tramp_gpio name for this API is tramp_gpio_is_interrupt_pending.

  @param[in] _h         The client context pointer of the GPIO interrupt controller DAL.
  @param[in] gpio       The GPIO pin number.
  @param[in] state      The pointer to a client allocated uint32 
                           variable which is populated by the GPIOInt driver 
                           to indicate whether the interrupt is pending or not.
 
  
  DEPENDENCIES  None.
  @return
  DAL_SUCCESS -- Is returned if the gpio state query was successful. \n
  DAL_ERROR --  Is returned if the GPIO is unsupported. \n
  @dependencies
  None.
*/

static __inline DALResult
GPIOInt_IsInterruptPending(DalDeviceHandle * _h, uint32  gpio, uint32 *  state)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_6(DALVTBLIDX(((GPIOIntHandle *)_h)->pVtbl, IsInterruptPending ), _h, gpio, (void *)state, 1);
   }
   return ((GPIOIntHandle *)_h)->pVtbl->IsInterruptPending(DALDEVICE_LOCAL_CLIENTID, _h, gpio, (void *)state, 1);
}


/*=============================================================================
  FUNCTION      GPIOInt_IsInterruptPending
=============================================================================*/
/**
  Queries the GPIO interrupt set state.

  This function tells the user if the given gpio interrupt is set or not. \n
  This is a new API so there is no legacy tramp_gpio name for this API.

  @param[in] _h         The client context pointer of the GPIO interrupt controller DAL.
  @param[in] gpio       The GPIO pin number.
  @param[in] state      The pointer to a client allocated uint32 
                           variable which is populated by the GPIOInt driver 
                           to indicate whether the interrupt is set or not.
 
  
  DEPENDENCIES  None.
  @return
  DAL_SUCCESS -- Is returned if the gpio state query was successful. \n
  DAL_ERROR --  Is returned if the GPIO is unsupported. \n
  @dependencies
  None.
*/

static __inline DALResult
GPIOInt_IsInterruptSet(DalDeviceHandle * _h, uint32  gpio, uint32 *  state)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_6(DALVTBLIDX(((GPIOIntHandle *)_h)->pVtbl, IsInterruptSet ), _h, gpio, (void *)state, 1);
   }
   return ((GPIOIntHandle *)_h)->pVtbl->IsInterruptSet(DALDEVICE_LOCAL_CLIENTID, _h, gpio, (void *)state, 1);
}

/*=============================================================================
  FUNCTION      GPIOInt_Save
=============================================================================*/
/**
  Can be called when the processor enters power collapse.

  This function is the default power saving function that saves any volatile data 
  or state during power collapse. \n
  This API is not used in the current code base.

  @param[in] _h              The client context pointer of the GPIO interrupt controller DAL.
  
  DEPENDENCIES  None.
  @return
  DAL_SUCCESS -- Is returned if the gpio interrupt driver state was saved successfully. \n
  DAL_ERROR --  Is returned if the power collapse type is not valid . \n
  @dependencies
  None.
*/


static __inline DALResult
GPIOInt_Save(DalDeviceHandle * _h)
{
  if(!DALISREMOTEHANDLE(_h))
  {
    return ((GPIOIntHandle *)_h)->pVtbl->Save(DALDEVICE_LOCAL_CLIENTID, _h);   
  }
  else
  {
    return DAL_ERROR;
  }
}

/*=============================================================================
  FUNCTION      GPIOInt_Restore
=============================================================================*/
/**
  Can be called when the processor recovers from power collapse.

  This function is the default power restore function that restores any saved 
  data or state during power collapse upon coming out of power collapse. 
  This API is not used in the current code base.

  @param[in] _h             The client context pointer of the GPIO interrupt controller DAL.

  DEPENDENCIES  None.
  @return
  DAL_SUCCESS -- Is returned if the gpio interrupt driver state was restored successfully. \n
  DAL_ERROR --  Is returned if the power collapse type is not valid . \n
  @dependencies
  None.
*/

static __inline DALResult
GPIOInt_Restore(DalDeviceHandle * _h)
{
  if(!DALISREMOTEHANDLE(_h))
  {
    return ((GPIOIntHandle *)_h)->pVtbl->Restore(DALDEVICE_LOCAL_CLIENTID, _h);
  }
  else
  {
    return DAL_ERROR;
  }
}


/*=============================================================================
  FUNCTION      GPIOInt_DisableInterrupt
=============================================================================*/
/**
  Disables a GPIO interrupt.

  This is an internal function that is used to disable the SUMMARY and 
  DIRECT CONNECT interrupts. This should not be used by clients when they have already registered a callback .
  It should only be used externally when the client is monitoring the interrupt in polling mode.
 Clients are discoraged from using this API without consultation with the driver technology team. \n
  This is a new API added to the GPIOInt module.

  @param[in] _h             The client context pointer of the GPIO interrupt controller DAL.
  @param[in] gpio           The GPIO pin number.
 
 
  
  DEPENDENCIES  None.
  @return
  DAL_SUCCESS -- Is returned if the gpio interrupt was disabled successfully. \n
  DAL_ERROR --  Is returned if the GPIO number is invalid. \n
  @dependencies
  None.
*/

static __inline DALResult
GPIOInt_DisableInterrupt(DalDeviceHandle * _h, uint32 gpio)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_0(DALVTBLIDX(((GPIOIntHandle *)_h)->pVtbl, DisableInterrupt ), _h, gpio);
   }
   return ((GPIOIntHandle *)_h)->pVtbl->DisableInterrupt(DALDEVICE_LOCAL_CLIENTID, _h, gpio);
}


/*=============================================================================
  FUNCTION      GPIOInt_EnableInterrupt
=============================================================================*/
/**
  Enables a GPIO interrupt.

  This is an internal function that is used to enable the SUMMARY and 
  DIRECT CONNECT interrupts. This should not be used by clients when they have already registered a callback .
  It should only be used externally when the client is monitoring the interrupt in polling mode.
 Clients are discoraged from using this API without consultation with the driver technology team. \n
 This is a new API added to the GPIOInt module.

  @param[in] _h             The client context pointer of the GPIO interrupt controller DAL.
  @param[in] gpio           The GPIO pin number.
 
 
  
  DEPENDENCIES  None.
  @return
  DAL_SUCCESS -- Is returned if the gpio interrupt was enabled successfully. \n
  DAL_ERROR --  Is returned if the GPIO number is invalid. \n
  @dependencies
  None.
*/

static __inline DALResult
GPIOInt_EnableInterrupt(DalDeviceHandle * _h, uint32 gpio)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_0(DALVTBLIDX(((GPIOIntHandle *)_h)->pVtbl, EnableInterrupt ), _h, gpio);
   }
   return ((GPIOIntHandle *)_h)->pVtbl->EnableInterrupt(DALDEVICE_LOCAL_CLIENTID, _h, gpio);
}

/** @cond */
/*=============================================================================
  FUNCTION      GPIOInt_InterruptNotify
=============================================================================*/
/**
  Notifies the peripheral processor that a GPIO interrupt has fired.

  This is an MDM 9k specific function that notifies the peripheral processor 
  via inter processor communication that an interrupt has occured. This is not 
  valid on any other platform excepting 9k.On the 9k platform this API is used 
  exclusively by the GPIO interupt driver and not by any client. It should not 
  be used by any client. \n
  This is a new API added to the GPIOInt module.

  @param[in] _h             The client context pointer of the GPIO interrupt controller DAL.
  @param[in] gpio           The GPIO pin number.
 
 
  
  DEPENDENCIES  None.
  @return
  DAL_SUCCESS -- Is returned if the peripheral processor was notified successfully. \n
  DAL_ERROR --  Is returned if the GPIO number is invalid. \n
  @dependencies
  None.
*/

static __inline DALResult
GPIOInt_InterruptNotify(DalDeviceHandle * _h, uint32 gpio)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_0(DALVTBLIDX(((GPIOIntHandle *)_h)->pVtbl, InterruptNotify ), _h, gpio);
   }
   return ((GPIOIntHandle *)_h)->pVtbl->InterruptNotify(DALDEVICE_LOCAL_CLIENTID, _h, gpio);
}

/*=============================================================================
  FUNCTION      GPIOInt_MonitorInterrupts
=============================================================================*/
/**
  Monitors all GPIO interrupts during the peripheral processor's power collapse state on 9k.

  This is an MDM9k specific function monitors all the gpio interrupts on the
  master processor so that the respective processor can be woken up depending 
  on the intended GPIO interrupt destination. This is used by the MPM (allways on
  domain.) \n
  tramp_gpio_switch_to_modem and tramp_gpio_switch_to_apps are the legacy tramp_gpio functions corresponding to
  this API.
  

  @param[in] _h             The client context pointer of the GPIO interrupt controller DAL.
  @param[in] isr            The allways on domain module's monitor interrupts isr callback.
  @param[in] enable         Flag to enable monitoring going into power collapse and disable 
                               interrupt monitoring coming out of power collapse.
  
  DEPENDENCIES  None.
  @return
  DAL_SUCCESS -- Is returned if the gpio interrupts were set up successfully to be monitored by the master processor. \n
  DAL_ERROR --  Is returned if the parameters given to this function are invalid. \n
  @dependencies
  None.
*/

static __inline DALResult
GPIOInt_MonitorInterrupts(DalDeviceHandle * _h, GPIOINTISR isr,uint32 enable)
{
  if(!DALISREMOTEHANDLE(_h))
   {
   return ((GPIOIntHandle *)_h)->pVtbl->MonitorInterrupts(DALDEVICE_LOCAL_CLIENTID, _h,isr,enable);
}
  else
    return DAL_ERROR;
   
}

/** @endcond */
/*=============================================================================
  FUNCTION      GPIOInt_MapMPMInterrupts
=============================================================================*/
 /**
  Maps those GPIO interrupts that are marked and routed in the Always on domain 
  with the GPIO interrupt controller driver.
   
  This function is used by the power manager to map the GPIO interrupts
  that can wakeup the msm from low power mode on being triggered. \n
  The legacy tramp_gpio name for this API is tramp_gpio_map_mpm_interrupt.

  @param[in] _h             The client context pointer of the GPIO interrupt 
                               controller DAL.
  @param[in] gpio           The GPIO pin number.
  @param[in] mpm_id         The corresponding interrupt id in the power manager 
                               interrupt controller
  DEPENDENCIES  None.
  @return
  DAL_SUCCESS -- Is returned if the gpio interrupts were mapped successfully \n
  DAL_ERROR --  Is returned if the GPIO number given is invalid. \n
  @dependencies
  None.
*/

static __inline DALResult
GPIOInt_MapMPMInterrupt(DalDeviceHandle * _h, uint32 gpio, uint32 mpm_interrupt_id)
{
  if(!DALISREMOTEHANDLE(_h))
  {
    return ((GPIOIntHandle *)_h)->pVtbl->MapMPMInterrupt(DALDEVICE_LOCAL_CLIENTID, _h, gpio,mpm_interrupt_id);
  }
  else
    return  DAL_ERROR;
}

/*=============================================================================
  FUNCTION      GPIOInt_AttachRemote
=============================================================================*/
/**
  Attaches to the remote GPIO interrupt module on the 9k target.
   
  This is mdm9k specific function that is used by GPIOINt to attach to its 
  remote component on master and peripheral processors. This attach is 
  primarily used for inter processor communication for relaying interrupts
  fired on the master processor.

  @param[in] _h             The client context pointer of the GPIO interrupt 
                               controller DAL.
  @param[in] processor      The remote processor id used to specify which 
                               GPIOINT device to communicate to interrupt controller
  DEPENDENCIES  None.
  @return
  DAL_SUCCESS -- Is returned if the Attach to the remote device went through successfully \n
  DAL_ERROR --  Is returned if there was an error in communicating with the remote module. \n
  @dependencies
  None.
*/

static __inline DALResult
GPIOInt_AttachRemote(DalDeviceHandle * _h, uint32 processor)
{
  if(!DALISREMOTEHANDLE(_h))
  {
    return ((GPIOIntHandle *)_h)->pVtbl->AttachRemote(DALDEVICE_LOCAL_CLIENTID, _h, processor);
  }
  else
    return  DAL_ERROR;
}


/*=============================================================================
  FUNCTION      GPIOInt_TriggerInterrupt
=============================================================================*/
/**
  Artificially force triggers an interrupt
   
  This function is used to trigger an interrupt without the GPIO interrupt 
  being actually set in hardware.
  The legacy tramp_gpio name for this API is tramp_gpio_trigger_interrupt.

  @param[in] _h             The client context pointer of the GPIO interrupt 
                               controller DAL.
  @param[in] gpio           The GPIO pin number.

  DEPENDENCIES  None.
  @return
  DAL_SUCCESS -- Is returned if the gpio interrupt was triggered successfully. \n
  DAL_ERROR --  Is returned if the GPIO number is invalid. \n
  @dependencies
  None.
*/

static __inline DALResult
GPIOInt_TriggerInterrupt(DalDeviceHandle * _h, uint32 gpio)
{

  if(!DALISREMOTEHANDLE(_h))
  {
    return ((GPIOIntHandle *)_h)->pVtbl->TriggerInterrupt(DALDEVICE_LOCAL_CLIENTID, _h, gpio); 
  }
  else
  {
    return DAL_ERROR;
  }  
  
}

/*=============================================================================
  FUNCTION      GPIOInt_Init
=============================================================================*/
 /**
  is used to complete GPIOInt dependencies on other modules in the build.
   
  This function is used for GPIO Interrupt driver initialization. 
  This has to be called first at bootup so that it completes its 
  set of dependencies for NPA and DALSYS. 
  The legacy tramp_gpio name for this API is tramp_gpio_init.

  @param 
  None
  @return
  None
  @dependencies
  None.
*/
void GPIOInt_Init(void);


/*=============================================================================
  FUNCTION      GPIOInt_ClearInterrupt
=============================================================================*/
/**
  Clears a GPIO interrupt.

  This is an internal function that is used to disable the SUMMARY and 
  DIRECT CONNECT interrupts. This should not be used by clients when they have 
  already registered a callback . It should only be used externally when the 
  client is monitoring the interrupt in polling mode. Clients are discoraged from 
  using this API without consultation with the driver technology team. \n
  This is a new API added to the GPIOInt module.

  @param[in] _h             The client context pointer of the GPIO interrupt controller DAL.
  @param[in] gpio           The GPIO pin number who's interrupt needs to be cleared.
 
 
  
  DEPENDENCIES  None.
  @return
  DAL_SUCCESS -- Is returned if the gpio interrupt was cleared successfully. \n
  DAL_ERROR --  Is returned if the GPIO number is invalid. \n
  @dependencies
  None.
*/

static __inline DALResult
GPIOInt_ClearInterrupt(DalDeviceHandle * _h, uint32 gpio)
{

  if(!DALISREMOTEHANDLE(_h))
  {
    return ((GPIOIntHandle *)_h)->pVtbl->ClearInterrupt(DALDEVICE_LOCAL_CLIENTID, _h, gpio); 
  }
  else
  {
    return DAL_ERROR;
  }  
  
}

/*=============================================================================
 FUNCTION      GPIOInt_SetDirectConnectGPIOMapping
=============================================================================*/
/**
  Maps a GPIO interrupt to the specified direct connect interrupt vector. \n

  
  This function is used to map a GPIO interrupt to a direct connect line. \n

  PARAMETERS    
  @param[in] _h             The client context pointer of the GPIO interrupt controller DAL.
  @param[in] gpio           The GPIO pin number who's interrupt needs to be cleared.
  @param[in] direct_connect_line The main interrupt controller line.
  DEPENDENCIES  None.

  DEPENDENCIES  None.
  @return
  DAL_ERROR --   is returned if the GPIO is not supported or if the direct connect line is not supported. \n
  DAL_SUCCESS --  is returned otherwise. \n
  @dependencies
  None.
*/

static __inline DALResult
GPIOInt_SetDirectConnectGPIOMapping(DalDeviceHandle * _h, uint32 gpio, uint32 direct_connect_line)
{

  if(!DALISREMOTEHANDLE(_h))
  {
    return ((GPIOIntHandle *)_h)->pVtbl->SetDirectConnectGPIOMapping(DALDEVICE_LOCAL_CLIENTID, _h, gpio, direct_connect_line); 
  }
  else
  {
    return DAL_ERROR;
  }  
  
}


/**
  Queries the GPIO interrupt Registration state.

  This function tells the user if the given gpio interrupt is 
  registered or not. \n This is a new API so there is no legacy
  tramp_gpio name for this API. 

  @param[in] _h         The client context pointer of the GPIO interrupt controller DAL.
  @param[in] gpio       The GPIO pin number.
  @param[in] state      The pointer to a client allocated uint32 
                           variable which is populated by the GPIOInt driver 
                           to indicate whether the interrupt is
                           registered or not.
 
  
  DEPENDENCIES  None.
  @return
  DAL_SUCCESS -- Is returned if the gpio state query was successful. \n
  DAL_ERROR --  Is returned if the GPIO is unsupported. \n
  @dependencies
  None.
*/

static __inline DALResult
GPIOInt_IsInterruptRegistered(DalDeviceHandle * _h, uint32  gpio, uint32 *  state)
{
   if(DALISREMOTEHANDLE(_h))
   {
      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
      return hRemote->pVtbl->FCN_6(DALVTBLIDX(((GPIOIntHandle *)_h)->pVtbl, IsInterruptRegistered ), _h, gpio, (void *)state, 1);
   }
   return ((GPIOIntHandle *)_h)->pVtbl->IsInterruptRegistered(DALDEVICE_LOCAL_CLIENTID, _h, gpio, (void *)state, 1);
}



#endif  //__GPIOInt_H__
