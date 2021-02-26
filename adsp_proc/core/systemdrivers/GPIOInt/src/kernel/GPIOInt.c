/*==============================================================================

FILE:      DalGPIOInt.c

DESCRIPTION
  This modules implements the API to utilize the Dal GPIO interrupt controller. 
  The definitions in this module are implemented internally and should not be 
  exposed for external client usage directly without using the accompanying DDI
  interface file for this module.

REFERENCES

       Copyright © 2010-2015 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/systemdrivers/GPIOInt/src/kernel/GPIOInt.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
1/1/2010   aratin  First draft created. 
===========================================================================*/
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DALStdErr.h"
#include "DDITimetick.h"
#include "DDIGPIOMgr.h"
#include "DDITlmm.h"
#include "busywait.h"
#include "DDIInterruptController.h"
#include "pdcGpio.h"
#include "HALgpioint.h"
#include "DALGlbCtxt.h"
#include "GPIOInt.h"
#include "DDIHWIO.h"
#include "qurt.h"
#include "GPIOInt_DirConn.h"
#include "GPIOInt_Summary.h"
#include "GPIOIntLog.h"
#include "uGPIOInt.h"
#include "uGPIOIntInternal.h"

/*------------------------------------------------------------------------------
 GPIOInt Constants.
------------------------------------------------------------------------------*/ 
/*
 * Disable TCXO shutdown if any non PDC interrupts are enabled.
 */
#define GPIOINT_XO_SHUTDOWN_DISABLE 1

/*------------------------------------------------------------------------------
 GPIOInt Data Declarations.
------------------------------------------------------------------------------*/ 
/*
 * This is static GPIOInt state data. It can be accessed for debugging 
 * GPIOInterrupts to see what is the current registration state of the GPIO.
 */
static GPIOIntCntrlType GPIOIntData;
/*------------------------------------------------------------------------------
Following functions are internal MAO related functions that are turned off in 
WM until we have api support.
------------------------------------------------------------------------------*/
#ifdef GPIOINT_USE_NPA
static void GPIOIntr_UpdateNPARequest(GPIOIntDevCtxt* device);
static void GPIOIntr_Init_NPA(void *context,unsigned int  event_type, 
void *resource_name, unsigned int  resource_strlen);
#endif
DALResult GPIOIntr_ConfigureGPIOInterrupt( GPIOIntClientCtxt * pCtxt,uint32 gpio,GPIOIntTriggerType trigger);
uint32 GPIOIntr_GetDirectIntrTrigger(GPIOIntDevCtxt *device,GPIOIntTriggerType trigger);

/*==========================================================================

  FUNCTION      GPIOIntr_Init_NPA

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

#ifdef GPIOINT_USE_NPA
static void GPIOIntr_Init_NPA
( 
  void          *context,
  unsigned int  event_type, 
  void          *resource_name,
  unsigned int  resource_strlen
)
{
  GPIOIntDevCtxt* device;
  device = (GPIOIntDevCtxt*)context;
  device->gpioint_cntrl->npa_client = npa_create_sync_client(resource_name,"tramp_gpio", NPA_CLIENT_REQUIRED);
  if(device->gpioint_cntrl->npa_client != NULL)
  {
    GPIOIntr_UpdateNPARequest(device);
  }
  else
  {
    DALSYS_LogEvent(device->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
    "GPIO Interrupt Controller: Device could not create a sync object in GPIOIntr_Init_NPA");
  }

}/* END GPIOIntr_init_npa */


/*==========================================================================

  FUNCTION      GPIOIntr_UpdateNPARequest

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

static void GPIOIntr_UpdateNPARequest
(
  GPIOIntDevCtxt* device
)
{

  /* 
   * If the server is not ready we cannot make this request
   */
  if(device->gpioint_cntrl->npa_client == NULL)
  {
    return;
  }
  /*
   * Disable interrupts
   */
  (void)DALSYS_SyncEnter(device->gpioint_synchronization);
  
  if (device->gpioint_cntrl->non_mpm_interrupts)
  {
    /*
     * Log request
     */
    GPIOIntLog_Out(GPIOINTLOG_NPAREQ, device->hULog, 1, "NPA Issue: Issue request for CXO, number of non pdc interrupts: %d", 
                     device->gpioint_cntrl->non_mpm_interrupts );
    
    /*
     *  If there are are interrupts that are not wakeup sources we need to 
     *  request for xo clk source so that we can keep state of the interrupt 
     *  controller and not have it wiped out at a lower voltage.
     */
    npa_issue_required_request(device->gpioint_cntrl->npa_client, GPIOINT_XO_SHUTDOWN_DISABLE);
  }

  /*
   *  If  only interrupts that are wakeup sources are enabled we can cancel request 
   *  for xo clk and save power. We don't have to maintain state at 
   *  the interrupt controller as the PDC interrupt controller already has this state.
   */
  else 
  {
    /*
     * Log request
     */
    GPIOIntLog_Out(GPIOINTLOG_NPAREQ, device->hULog, 1, "NPA Cancel: Cancel request for CXO, number of non pdc interrupts: %d", 
                     device->gpioint_cntrl->non_mpm_interrupts );

    npa_cancel_request(device->gpioint_cntrl->npa_client);
  }

  /*
   * Enable interrupts
   */
  (void)DALSYS_SyncLeave(device->gpioint_synchronization);

}  /* END GPIOIntr_update_npa_request */

#endif /* GPIOINT_USE_NPA */


/*------------------------------------------------------------------------------
Following functions are for DALDriver specific functionality
------------------------------------------------------------------------------*/
DALResult 
GPIOIntr_DriverInit(GPIOIntDrvCtxt *pCtxt)
{
	return DAL_SUCCESS;
}

DALResult 
GPIOIntr_DriverDeInit(GPIOIntDrvCtxt *pCtxt)
{
	return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are declared in DalDevice Interface. 
------------------------------------------------------------------------------*/

/*==========================================================================

  FUNCTION      GPIOIntr_DeviceInit

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_DeviceInit
(
  GPIOIntClientCtxt *pCtxt
)
{
  GPIOIntCntrlType *gpioint_cntrl;
  GPIOIntDevCtxt* device;
  DALResult result;
  uint32 nIdx;

  device = pCtxt->pGPIOIntDevCtxt;
  result = DAL_SUCCESS;

  if( device == NULL )
  {
    return DAL_ERROR;
  }
  
  device->gpioint_cntrl = &GPIOIntData;
  gpioint_cntrl = device->gpioint_cntrl;

  /*
   * GPIOInt_Init has to be called first before anyone can attach to this 
   * driver. This is to ensure that GPIOInt dependencies of NPA and DALSYS 
   * are met.
   */
  if(GPIOIntData.GPIOInt_Init != 1)
  {
    DALSYS_LogEvent(device->DevId, DALSYS_LOGEVENT_WARNING, 
      "GPIO Interrupt Controller: Client cannot attach before GPIOInt_Init ");
    return DAL_ERROR;
  }
 
  if(!pCtxt->is_initialized)
  {
  
    /* Each processor has its own internal synchronization object to access 
     * data that it owns */
    if(DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_NO_PREEMPTION,
             &(device->gpioint_synchronization), &(device->GPIOIntSyncObject)))
    {
      DALSYS_LogEvent(device->DevId, DALSYS_LOGEVENT_WARNING, 
      "GPIO Interrupt Controller: Device could not create a sync object in GPIOIntr_DeviceInit");
      return DAL_ERROR;
    }

    /*
     * GPIOInt dal communicates with the main level1 interrupt controller
     * and the TLMM dal. So in Init we need to attach to the respective dals 
     * to get their handles in order to use their API.
     */
    if(DAL_SUCCESS != GPIOIntr_AttachToDals(device))
    { 
      return DAL_ERROR;
    }
   
    /*
     * Initialize the GPIOInt HAL
     */
    (void)DALSYS_SyncEnter(device->gpioint_synchronization);
    if(DAL_SUCCESS != GPIOIntr_GetTargetData(pCtxt))
    {  
      (void)DALSYS_SyncLeave(device->gpioint_synchronization);
      return DAL_ERROR;
    }

    /*
     * Setup uLog buffer
     */
    GPIOIntLog_Init(&device->hULog);
    
    /*
     * Install the top-level interrupt handler for the SUMMARY class interrupts.
     */
    gpioint_cntrl->summary_param = (uint32)HAL_GPIOINT_GROUP1;
    result = DAL_SUCCESS;
    (void)DALSYS_SyncLeave(device->gpioint_synchronization);
  }

  for( nIdx = 0; nIdx < gpioint_cntrl->direct_intr_number; nIdx++ )
  {
    if( gpioint_cntrl->gpioint_config_map[nIdx].flags & GPIOINTF_DIRECTINT_CONFIG )
    {
      gpioint_cntrl->state[gpioint_cntrl->gpioint_config_map[nIdx].gpio].gpio_intr_flags |= GPIOINTF_DIRECTINT_CONFIG;
      gpioint_cntrl->state[gpioint_cntrl->gpioint_config_map[nIdx].gpio].intr_type = HAL_GPIOINT_SPECIALCONN;
    }
  }

  GPIOIntrDirConn_Init(device->intr_handle);
  GPIOIntrSummary_Init(device, device->intr_handle);

  if(result == DAL_SUCCESS)
  {
    /*
     * flag the driver as initialised.
     */
    pCtxt->is_initialized = 1;
    GPIOIntData.is_initialized = 1;
    return result;
  }
  else
  {
    pCtxt->is_initialized = 0;
    GPIOIntData.is_initialized = 0;
    return DAL_ERROR;
  }

} /* END GPIOIntr_DeviceInit */


/*==========================================================================

  FUNCTION      GPIOIntr_GetTargetData

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_GetTargetData
(
  GPIOIntClientCtxt *pCtxt
)
{
  DALResult result;
  GPIOIntDevCtxt *device;

  /* 
   * structure which holds properties for dal.
   */
  DALSYSPropertyVar system_property_var;  
  HAL_gpioint_ContextType Target;
  HAL_gpioint_TargetDataType TargetData;
  char * xo_shutdown_rsrc;
  uint32 gpio;

  /* 
   * declare property handle variable.
   */
  DALSYS_PROPERTY_HANDLE_DECLARE(interrupt_property); 

  result = DAL_SUCCESS;
  device = pCtxt->pGPIOIntDevCtxt;
  
  /* Get the property handle */
  DALSYS_GetDALPropertyHandle(DALDEVICEID_GPIOINT,interrupt_property);
  result = DALSYS_GetPropertyValue(interrupt_property,"PROCESSOR",
                           0,&system_property_var); 
  if(result == DAL_SUCCESS)
  {
    device->gpioint_cntrl->processor = 
    (HAL_gpioint_ProcessorType)system_property_var.Val.dwVal;
  }
  else
  {
    DALSYS_LogEvent(pCtxt->pGPIOIntDevCtxt->DevId, DALSYS_LOGEVENT_WARNING, 
    "GPIO Interrupt Controller: Target Data is corrupt");
    return DAL_ERROR;
  }

  result = DALSYS_GetPropertyValue(interrupt_property,"XO_SHUTDOWN_RSRC",
                           0,&system_property_var); 
  if(result == DAL_SUCCESS)
  {
    xo_shutdown_rsrc = 
    (char *)system_property_var.Val.pszVal;

    /*
     * GPIOInt needs to communicate to the PDC for power saving features 
     * implemented.
     */
    #ifdef GPIOINT_USE_NPA
    npa_resource_available_cb(xo_shutdown_rsrc, GPIOIntr_Init_NPA, device);
    #endif
  }
  else
  {
    DALSYS_LogEvent(pCtxt->pGPIOIntDevCtxt->DevId, DALSYS_LOGEVENT_WARNING, 
    "GPIO Interrupt Controller: Target Data is corrupt");
    return DAL_ERROR;
  }  

  TargetData.eProcessor = device->gpioint_cntrl->processor;
  Target.pTargetData = &TargetData;

  result = DALSYS_GetPropertyValue(interrupt_property,"GPIOINT_PHYSICAL_ADDRESS",
                           0,&system_property_var); 
  if(result == DAL_SUCCESS)
  {
    device->gpioint_cntrl->gpioint_physical_address = (uint32 *)system_property_var.Val.dwVal;
  }
  else
  {
    DALSYS_LogEvent(pCtxt->pGPIOIntDevCtxt->DevId, DALSYS_LOGEVENT_WARNING, 
    "GPIO Interrupt Controller: could not get a base address for GPIOInt");
    return DAL_ERROR;
  } 

  TargetData.nGPIOIntBase = device->gpioint_cntrl->gpioint_virtual_address;
  TargetData.nGPIOIntBasePhys = device->gpioint_cntrl->gpioint_physical_address;

  result = DALSYS_GetPropertyValue(interrupt_property,"INTERRUPT_PLATFORM",
                           0,&system_property_var); 
  if(result == DAL_SUCCESS)
  {
    device->gpioint_cntrl->interrupt_controller = (uint8)system_property_var.Val.dwVal;
  }
  else
  {
    DALSYS_LogEvent(pCtxt->pGPIOIntDevCtxt->DevId, DALSYS_LOGEVENT_WARNING, 
    "GPIO Interrupt Controller: could not the main interrupt controller for GPIOInt");
    return DAL_ERROR;
  } 

  HAL_gpioint_Init(&Target);
  HAL_gpioint_GetNumber(&device->gpioint_cntrl->gpio_number);
  
  result = DALSYS_GetPropertyValue(interrupt_property,
  "NUMBER_OF_DIRECT_CONNECT_INTERRUPTS",0,&system_property_var); 
  if(result == DAL_SUCCESS)
  {
    device->gpioint_cntrl->direct_intr_number = 
        system_property_var.Val.dwVal;
  }
  else
  {
    device->gpioint_cntrl->direct_intr_number = 0;
  }

  if(device->gpioint_cntrl->direct_intr_number > device->gpioint_cntrl->gpio_number)
  {
    DALSYS_LogEvent(pCtxt->pGPIOIntDevCtxt->DevId, DALSYS_LOGEVENT_WARNING, 
    "GPIO Interrupt Controller: Invalid Config Data, DirConn Int Number is Invalid ");
    return DAL_ERROR;
  }

  result = DALSYS_GetPropertyValue(interrupt_property,"SUMMARY_INTR_ID",
                           0,&system_property_var); 
  if(result == DAL_SUCCESS)
  {
    device->gpioint_cntrl->summary_intr_id = 
        system_property_var.Val.dwVal;
  }
  else
  {
    DALSYS_LogEvent(pCtxt->pGPIOIntDevCtxt->DevId, DALSYS_LOGEVENT_WARNING, 
    "GPIO Interrupt Controller: summary intr id is corrupt");
    return DAL_ERROR;
  }
 
  result = DALSYS_GetPropertyValue(interrupt_property,"UGPIOINT_DRIVER",
                           0,&system_property_var); 
  if(result == DAL_SUCCESS)
  {
    device->gpioint_cntrl->gpioint_uimage = 1;
    uGPIOInt_Init();
  }
  else
  {
    device->gpioint_cntrl->gpioint_uimage = 0;
  }
 
  /*
   *  Initialize all GPIOs as summary.
   */ 
  for(gpio =0; gpio < device->gpioint_cntrl->gpio_number; gpio++)
  {
    device->gpioint_cntrl->state[gpio].intr_type = HAL_GPIOINT_SUMMARY;
    device->gpioint_cntrl->state[gpio].qurt_intr_id = GPIOINT_NO_QURT_ID;
    device->gpioint_cntrl->state[gpio].qurt_intr_id_secondary = GPIOINT_NO_QURT_ID;
  }
  if(device->gpioint_cntrl->direct_intr_number)
  {
    result = DALSYS_GetPropertyValue(interrupt_property,
    "DIRECT_CONNECT_CONFIG_MAP",0,&system_property_var); 
    if(result == DAL_SUCCESS)
    {
      device->gpioint_cntrl->gpioint_config_map = 
      (GPIOIntConfigMapType *)system_property_var.Val.pStruct;
    }
    else
    {
      DALSYS_LogEvent(pCtxt->pGPIOIntDevCtxt->DevId, DALSYS_LOGEVENT_WARNING, 
      "GPIO Interrupt Controller: gpioint_config_map is corrupt");
      return result;
    }
  }  
  return result;

} /* END GPIOIntr_GetTargetData */


/*==========================================================================

  FUNCTION      GPIOIntr_Save (No implementation perceived as of now.)

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_Save
( 
  GPIOIntClientCtxt * pCtxt
)
{
    return DAL_ERROR;
}
/*==========================================================================

  FUNCTION      GPIOIntr_AttachToDals

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_AttachToDals
(
  GPIOIntDevCtxt *device
)
{
  /* attach to the Main Interrupt Controller dal*/
  if(DAL_SUCCESS != DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, 
                                     &device->intr_handle))
      return DAL_ERROR;

  /*
   * Acquire system timer.
   */
  if(DalTimetick_Attach("SystemTimer", &device->timetick_handle) != DAL_SUCCESS)
  {
    return DAL_ERROR;
  }

  /* 
   * Acquire GPIO Manager
   */
  if( DalGPIOMgr_Attach( &device->hGPIOMgr ) != DAL_SUCCESS )
  {
    return DAL_ERROR;
  }

  return DAL_SUCCESS;

} /* END GPIOIntr_AttachToDals */


DALResult 
GPIOIntr_DeviceDeInit(GPIOIntClientCtxt *pCtxt)
{
    return DAL_SUCCESS;
}

DALResult 
GPIOIntr_PowerEvent(GPIOIntClientCtxt *pCtxt, DalPowerCmd PowerCmd, 
                    DalPowerDomain PowerDomain)
{
    return DAL_SUCCESS;
}

DALResult 
GPIOIntr_Open(GPIOIntClientCtxt *pCtxt, uint32 dwaccessMode )
{
    return DAL_SUCCESS;
}

DALResult 
GPIOIntr_Close(GPIOIntClientCtxt *pCtxt)
{
    return DAL_SUCCESS;
}

DALResult 
GPIOIntr_Info(GPIOIntClientCtxt *pCtxt,DalDeviceInfo *pdeviceInfo, uint32 dwSize)
{
    return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalGPIOInt Interface. 
------------------------------------------------------------------------------*/
/*==========================================================================

  FUNCTION      GPIOIntr_SetTrigger

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_SetTrigger
( 
  GPIOIntClientCtxt * pCtxt, 
  uint32  gpio, 
  GPIOIntTriggerType  trigger
) 
{
  GPIOIntDevCtxt *device;
  uint32 interrupt_id,direct_connect_trigger;
  device = pCtxt->pGPIOIntDevCtxt;

  /*
   * Validate the given GPIO interrupt
   */
  if (!HAL_gpioint_IsSupported(gpio))
  {
    DALSYS_LogEvent(device->DevId, DALSYS_LOGEVENT_WARNING, 
    "GPIOIntr_SetTrigger : GPIO %d is invalid", gpio);
    return DAL_ERROR;
  }

  /*
   * get the synchronization object.
   */
  (void)DALSYS_SyncEnter(device->gpioint_synchronization);
  
  /*
   * If the interrupt is a direct connect interrupt we need to set the trigger
   * on the main level 1 interrupt controller and set the polarity on the 
   * secondary GPIOInt interrupt controller. In doing so there might be 
   * spurious interrupts generated hence we need to :
   * 1. Disable the interrupt.
   * 2. Set the trigger.
   * 3. Clear it.
   * 4. Enable it.
   */
  if(device->gpioint_cntrl->state[gpio].intr_type != HAL_GPIOINT_SUMMARY)
  {
    interrupt_id = GPIOIntr_GetInterruptId(device,gpio);
    if( device->gpioint_cntrl->state[gpio].gpio_intr_flags & GPIOINTF_DIRECTINT_CONFIG )
    {
      GPIOIntr_MapGPIOIntTrigger( trigger, &direct_connect_trigger );
    }
    else
    {
      direct_connect_trigger = GPIOIntr_GetDirectIntrTrigger(device, trigger); 
    }

    if ( trigger == GPIOINT_TRIGGER_DUAL_EDGE || device->gpioint_cntrl->state[gpio].intr_trigger == GPIOINT_TRIGGER_DUAL_EDGE )
    {
      GPIOIntLog_Out(GPIOINTLOG_SETTRIGGER, device->hULog, 2, "SetTrigger: GPIO %d failed setting trigger %d", 
                 gpio, (uint32)trigger );
      (void)DALSYS_SyncLeave(device->gpioint_synchronization);
      return DAL_ERROR;
    }

    GPIOIntrDirConn_SetTrigger( gpio, 
                                interrupt_id, 
                                trigger, 
                                direct_connect_trigger, 
                                ( device->gpioint_cntrl->state[gpio].gpio_intr_flags 
                                  & GPIOINTF_DIRECTINT_CONFIG ) );
   
    GPIOIntLog_Out(GPIOINTLOG_SETTRIGGER, device->hULog, 2, "SetTrigger: GPIO %d setting direct connect trigger %d", 
                     gpio, (uint32)trigger );

  }
  
  /*
   * The setting of the summary interrupt trigger is kept according to the 
   * legacy implementation.
   */
  else
  {
    GPIOIntrSummary_SetTrigger(gpio, trigger);

    GPIOIntLog_Out(GPIOINTLOG_SETTRIGGER, device->hULog, 2, "SetTrigger: GPIO %d setting summary trigger %d", 
                     gpio, (uint32)trigger );
  }

  /*
   * Update static GPIOInt map with Trigger 
   */ 
  device->gpioint_cntrl->state[gpio].intr_trigger = (HAL_gpioint_TriggerType)trigger;

  /*
   * Setup interrupt wake up.
   */ 
  GPIOIntr_ConfigMPMWakeup(pCtxt,gpio,FALSE);

  (void)DALSYS_SyncLeave(device->gpioint_synchronization);
  return DAL_SUCCESS;

} /* END GPIOIntr_SetTrigger */


/*==========================================================================

  FUNCTION      GPIOIntr_RegisterIsr

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_RegisterIsr
(
  GPIOIntClientCtxt * pCtxt, 
  uint32  gpio, 
  GPIOIntTriggerType trigger, 
  GPIOINTISR isr,
  GPIOINTISRCtx param
) 
{
  DALResult result;
  GPIOIntDevCtxt *device;
  
  device = pCtxt->pGPIOIntDevCtxt;

  /*
   * Validate the given GPIO interrupt
   */
  if (!HAL_gpioint_IsSupported(gpio))
  {
    DALSYS_LogEvent(device->DevId, DALSYS_LOGEVENT_WARNING, 
    "GPIOIntr_RegisterIsr : GPIO %d is not supported", gpio);
    return DAL_ERROR;
  }
  
  /*
   * Validate the given trigger type
   */
  if (trigger >= GPIOINT_TRIGGER_INVALID)
  {
    DALSYS_LogEvent(
      device->DevId, DALSYS_LOGEVENT_WARNING,
      "GPIOIntr_RegisterIsr: Invalid trigger type %d passed for GPIO %d",
      trigger, gpio);
    return DAL_ERROR;
  }
  
  (void)DALSYS_SyncEnter(device->gpioint_synchronization);
  if(((device->gpioint_cntrl->state[gpio].isr != NULL) &&
     (device->gpioint_cntrl->state[gpio].isr != isr)) ||
    (device->gpioint_cntrl->state[gpio].event != NULL))
  {
    /*
     * Restore interrupts and return an error.
     */
    (void)DALSYS_SyncLeave(device->gpioint_synchronization);
    DALSYS_LogEvent(device->DevId, DALSYS_LOGEVENT_WARNING, 
    "GPIOIntr_RegisterIsr: GPIO %d is already assigned", gpio);
    return DAL_ERROR;
  }

  if ( DalGPIOMgr_LockGPIO( device->hGPIOMgr, gpio, qurt_process_get_id() ) != DAL_SUCCESS )
  {
    /*
     * Restore interrupts and return an error.
     */
    (void)DALSYS_SyncLeave(device->gpioint_synchronization);
    DALSYS_LogEvent(device->DevId, DALSYS_LOGEVENT_WARNING, 
    "GPIOIntr_RegisterIsr: GPIOMgr failed to lock GPIO %d", gpio);
    return DAL_ERROR;
  }

  device->gpioint_cntrl->state[gpio].isr = isr;
  device->gpioint_cntrl->state[gpio].isr_param = param; 
  result = GPIOIntr_ConfigureGPIOInterrupt(pCtxt, gpio,trigger);
  (void)DALSYS_SyncLeave(device->gpioint_synchronization);
  return result;

} /* END GPIOIntr_RegisterIsr */


/*==========================================================================

  FUNCTION      GPIOIntr_UnConfigureGPIOInterrupt

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

void 
GPIOIntr_UnConfigureGPIOInterrupt
(
  GPIOIntDevCtxt * device, 
  uint32 gpio
)
{
  uint32 interrupt_id;
  HAL_gpioint_InterruptType interrupt;
  interrupt = device->gpioint_cntrl->state[gpio].intr_type;
 
  if(interrupt == HAL_GPIOINT_SPECIALCONN)
  {
    DalInterruptController_Unregister(device->intr_handle, 
                                      device->gpioint_cntrl->state[gpio].qurt_intr_id);

    if ( device->gpioint_cntrl->state[gpio].qurt_intr_id_secondary != GPIOINT_NO_QURT_ID )
    {
      DalInterruptController_Unregister(device->intr_handle, 
                                        device->gpioint_cntrl->state[gpio].qurt_intr_id_secondary);
    }

    device->gpioint_cntrl->state[gpio].qurt_intr_id = GPIOINT_NO_QURT_ID;
    device->gpioint_cntrl->state[gpio].qurt_intr_id_secondary = GPIOINT_NO_QURT_ID;
    device->gpioint_cntrl->state[gpio].intr_type = HAL_GPIOINT_SUMMARY;
    device->gpioint_cntrl->state[gpio].gpio_intr_flags &= ~(GPIOINTF_DIRECTINT_CONFIG | GPIOINTF_REGISTERED);

    return;
  }
  else if(interrupt != HAL_GPIOINT_SUMMARY)
  {
    interrupt_id = GPIOIntr_GetInterruptId(device, gpio);

    GPIOIntrDirConn_UnConfigureGPIOInterrupt( gpio, 
                                              interrupt_id, 
                                              device->gpioint_cntrl->gpioint_uimage );


    device->gpioint_cntrl->state[gpio].qurt_intr_id = GPIOINT_NO_QURT_ID;
    device->gpioint_cntrl->state[gpio].qurt_intr_id_secondary = GPIOINT_NO_QURT_ID;
    device->gpioint_cntrl->state[gpio].intr_type = HAL_GPIOINT_SUMMARY;

    DalGPIOMgr_ReleaseDirectConnectGPIO(device->hGPIOMgr, gpio, interrupt);

    if( device->gpioint_cntrl->state[gpio].gpio_intr_flags & GPIOINTF_DIRECTINT_CONFIG )
    {
      device->gpioint_cntrl->state[gpio].gpio_intr_flags &= ~GPIOINTF_REGISTERED; 
      return;
    }

  }
  
  /*
   * Disable this interrupt be it Direct or Summary and default the information
   * back to summary.
   */
  GPIOIntrSummary_UnConfigureGPIOInterrupt( device,
                                            gpio,
                                            device->gpioint_cntrl->summary_intr_id,
                                            device->gpioint_cntrl->gpioint_uimage);

  device->gpioint_cntrl->state[gpio].intr_type = HAL_GPIOINT_SUMMARY;

  GPIOIntLog_Out(GPIOINTLOG_REG, device->hULog, 1, "Deregister: GPIO %d", gpio );

  /*
   * Disable interrupt wake up.
   */ 
  pdcGpio_disable( gpio );
  GPIOIntLog_Out(GPIOINTLOG_MPM, device->hULog, 2, "PDC Disable: Disabling GPIO %d", gpio );


  device->gpioint_cntrl->state[gpio].gpio_intr_flags &= ~GPIOINTF_REGISTERED;

} /* END GPIOIntr_UnConfigureGPIOInterrupt */


/*==========================================================================

  FUNCTION      GPIOIntr_ConfigureGPIOInterrupt

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_ConfigureGPIOInterrupt
(
  GPIOIntClientCtxt * pCtxt, 
  uint32 gpio,
  GPIOIntTriggerType trigger
)
{
  GPIOIntDevCtxt * device;
  uint32 interrupt_id = GPIOINT_NONE, direct_connect_trigger, nDirConnID = HAL_GPIOINT_SUMMARY;
  DALResult eResult = DAL_ERROR;
  
  device = pCtxt->pGPIOIntDevCtxt;

  /*
   * Setup interrupt wake up.
   */ 
  device->gpioint_cntrl->state[gpio].intr_trigger = (HAL_gpioint_TriggerType)trigger;
  GPIOIntr_ConfigMPMWakeup(pCtxt,gpio,TRUE);

  if( device->gpioint_cntrl->state[gpio].gpio_intr_flags & GPIOINTF_DIRECTINT_CONFIG )
  {
    interrupt_id = GPIOIntr_GetInterruptId(device,gpio);
  }
  else
  {
    device->gpioint_cntrl->state[gpio].intr_type = HAL_GPIOINT_SUMMARY;
    interrupt_id = GPIOINT_NONE;
    if(trigger != GPIOINT_TRIGGER_DUAL_EDGE) 
    {
      if( DalGPIOMgr_GetDirectConnectGPIO(device->hGPIOMgr, gpio, &nDirConnID) == DAL_SUCCESS )
      {
        DalGPIOMgr_GetDirectConnectInterruptID(device->hGPIOMgr, gpio, &interrupt_id);
        device->gpioint_cntrl->state[gpio].intr_type = nDirConnID;
        device->gpioint_cntrl->state[gpio].qurt_intr_id = interrupt_id;
      }
    }
  }

  if(device->gpioint_cntrl->state[gpio].intr_type == HAL_GPIOINT_SUMMARY)
  {
    /*
     * Update static GPIOInt map with Trigger 
     */ 
    GPIOIntrSummary_ConfigureGPIOInterrupt( device,
                                            gpio,
                                            (HAL_gpioint_TriggerType)trigger,
                                            device->gpioint_cntrl->processor,
                                            device->gpioint_cntrl->state[gpio].intr_type );

    GPIOIntLog_Out(GPIOINTLOG_REG, device->hULog, 2, "Register: GPIO %d for summary interrupt with trigger %d", gpio, (uint32)trigger );
  } 
  else
  { 

    if( device->gpioint_cntrl->state[gpio].gpio_intr_flags & GPIOINTF_DIRECTINT_CONFIG )
    {
      if( GPIOIntr_MapGPIOIntTrigger( trigger, &direct_connect_trigger ) != DAL_SUCCESS )
      {
        return DAL_ERROR;
      }
    }
    else
    {
      /*
       * Install the client event if there is a client registration or an ISR if the client registers one.
       */
      direct_connect_trigger = GPIOIntr_GetDirectIntrTrigger(device,trigger);
    }

    if ( device->gpioint_cntrl->state[gpio].gpio_intr_flags & GPIOINTF_DIRECTINT_CONFIG &&
         trigger == GPIOINT_TRIGGER_DUAL_EDGE )
    {
      if(DAL_SUCCESS != DalInterruptController_RegisterISR(device->intr_handle, 
                                                           (DALInterruptID)device->gpioint_cntrl->state[gpio].qurt_intr_id,
                                                           (DALISR)device->gpioint_cntrl->state[gpio].isr,
                                                           (DALISRCtx)device->gpioint_cntrl->state[gpio].isr_param,
                                                           DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER))
      {
        return DAL_ERROR;
      }
      if(DAL_SUCCESS != DalInterruptController_RegisterISR(device->intr_handle, 
                                                           (DALInterruptID)device->gpioint_cntrl->state[gpio].qurt_intr_id_secondary,
                                                           (DALISR)device->gpioint_cntrl->state[gpio].isr,
                                                           (DALISRCtx)device->gpioint_cntrl->state[gpio].isr_param,
                                                           DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER))
      {
        return DAL_ERROR;
      }

      eResult = DAL_SUCCESS;
    }
    else
    {
      eResult = GPIOIntrDirConn_ConfigureGPIOInterrupt(gpio,interrupt_id,direct_connect_trigger,trigger,
                                             &device->gpioint_cntrl->state[gpio],
                                             device->gpioint_cntrl->state[gpio].gpio_intr_flags & GPIOINTF_DIRECTINT_CONFIG,
                                             device->gpioint_cntrl->gpioint_uimage );
    }

    if( eResult != DAL_SUCCESS )
    {
      return eResult;
    }

    /*
     * Update static GPIOInt map with Trigger 
     */ 
    device->gpioint_cntrl->state[gpio].intr_trigger = (HAL_gpioint_TriggerType)trigger;

    GPIOIntLog_Out(GPIOINTLOG_REG, device->hULog, 4, "Register: GPIO %d for direct connect ID %d and interrupt number %d with trigger %d", 
                     gpio, (uint32)device->gpioint_cntrl->state[gpio].intr_type, interrupt_id, (uint32)trigger );
  }

  device->gpioint_cntrl->state[gpio].gpio_intr_flags |= GPIOINTF_REGISTERED;
  return DAL_SUCCESS;

} /* END GPIOIntr_ConfigureGPIOInterrupt */


/*==========================================================================

  FUNCTION      GPIOIntr_RegisterEvent

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_RegisterEvent
(
  GPIOIntClientCtxt * pCtxt, 
  uint32 gpio,
  GPIOIntTriggerType trigger, 
  DALSYSEventHandle event
) 
{
  GPIOIntDevCtxt *device;
  DALResult result;
  device    = pCtxt->pGPIOIntDevCtxt;

  /*
   * Validate the given GPIO interrupt
   */
  if (!HAL_gpioint_IsSupported(gpio))
  {
    DALSYS_LogEvent(device->DevId, DALSYS_LOGEVENT_WARNING, 
    "GPIOIntr_RegisterEvent : GPIO %d is not supported", gpio);
    return DAL_ERROR;
  }
  (void)DALSYS_SyncEnter(device->gpioint_synchronization);
  if(((device->gpioint_cntrl->state[gpio].event != NULL) &&
  (device->gpioint_cntrl->state[gpio].event != event))||
  (device->gpioint_cntrl->state[gpio].isr != NULL))
  {
    /*
     * Restore interrupts
     */
    (void)DALSYS_SyncLeave(device->gpioint_synchronization);
    DALSYS_LogEvent(device->DevId, DALSYS_LOGEVENT_WARNING, 
    "GPIOIntr_RegisterEvent : GPIO %d is already assigned", gpio);
    return DAL_ERROR;
  }

  if ( DalGPIOMgr_LockGPIO( device->hGPIOMgr, gpio, qurt_process_get_id() ) != DAL_SUCCESS )
  {
    /*
     * Restore interrupts and return an error.
     */
    (void)DALSYS_SyncLeave(device->gpioint_synchronization);
    DALSYS_LogEvent(device->DevId, DALSYS_LOGEVENT_WARNING, 
    "GPIOIntr_RegisterEvent: GPIOMgr failed to lock GPIO %d", gpio);
    return DAL_ERROR;
  }

  device->gpioint_cntrl->state[gpio].event = event;
  result = GPIOIntr_ConfigureGPIOInterrupt(pCtxt, gpio,trigger);
  (void)DALSYS_SyncLeave(device->gpioint_synchronization);
  return result;

} /* GPIOIntr_RegisterEvent */


/*==========================================================================

  FUNCTION      GPIOIntr_DeRegisterEvent

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_DeRegisterEvent
(
  GPIOIntClientCtxt * pCtxt, 
  uint32 gpio,
  DALSYSEventHandle event
) 
{
  GPIOIntDevCtxt *device;
  device = pCtxt->pGPIOIntDevCtxt;

  if (!HAL_gpioint_IsSupported(gpio))
  {
    DALSYS_LogEvent(device->DevId, DALSYS_LOGEVENT_WARNING, 
    "GPIOIntr_DeRegisterEvent : GPIO %d is invalid", gpio);
    return DAL_ERROR;
  }

  /*
   * Access synchronisation object so we can prevent race conditions.
   */
  (void)DALSYS_SyncEnter(device->gpioint_synchronization);
  
  if((device->gpioint_cntrl->state[gpio].event != NULL) &&
     (device->gpioint_cntrl->state[gpio].event != event))
  {
    /*
     * Restore interrupts
     */
    (void)DALSYS_SyncLeave(device->gpioint_synchronization);
    return DAL_ERROR;
  }

  if ( DalGPIOMgr_ReleaseGPIO( device->hGPIOMgr, gpio ) != DAL_SUCCESS )
  {
    /*
     * Restore interrupts and return an error.
     */
    DALSYS_LogEvent(device->DevId, DALSYS_LOGEVENT_WARNING, 
    "GPIOIntr_DeregisterEvent:GPIOMgr failed to release GPIO %d", gpio);
  }

  /*
   * Here, the event and isr associated with the gpio pin are cleaned up.
   */
  device->gpioint_cntrl->state[gpio].isr = NULL;
  device->gpioint_cntrl->state[gpio].event = NULL;
  GPIOIntr_UnConfigureGPIOInterrupt(device,gpio); 

  /*
   * Restore interrupts
   */
  (void)DALSYS_SyncLeave(device->gpioint_synchronization);
  return DAL_SUCCESS;

} /* END GPIOIntr_DeRegisterEvent */


/*==========================================================================

  FUNCTION      GPIOIntr_DeregisterIsr

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_DeregisterIsr
( 
  GPIOIntClientCtxt * pCtxt, 
  uint32  gpio, 
  GPIOINTISR isr
) 
{
  GPIOIntDevCtxt *device;

  device = pCtxt->pGPIOIntDevCtxt;
  if (!HAL_gpioint_IsSupported(gpio))
  {
    DALSYS_LogEvent(device->DevId, DALSYS_LOGEVENT_WARNING, 
    "GPIOIntr_DeregisterIsr : GPIO %d is invalid", gpio);
    return DAL_ERROR;
  }
  
  /*
   * Disable interrupts by accessing the processor's internal sync variable
   */
  (void)DALSYS_SyncEnter(device->gpioint_synchronization);

  /*
   * Check that the ISR is correct
   */
  if (device->gpioint_cntrl->state[gpio].isr != isr)
  {
    DALSYS_LogEvent(device->DevId, DALSYS_LOGEVENT_WARNING, 
    "GPIOIntr_DeregisterIsr: : GPIO %d has a different ISR assigned", gpio);
    /*
     * Restore interrupts
     */
    (void)DALSYS_SyncLeave(
    device->gpioint_synchronization);
    return DAL_ERROR;
  }

  if ( DalGPIOMgr_ReleaseGPIO( device->hGPIOMgr, gpio ) != DAL_SUCCESS )
  {
    DALSYS_LogEvent(device->DevId, DALSYS_LOGEVENT_WARNING, 
    "GPIOIntr_DeregisterIsr: GPIOMgr failed to release GPIO %d", gpio);
  }

  /*
   * Clear out the handler and remove the event. Here the direct connect 
   * interrupt handler is just saved for error checking purposes as it is 
   * largely handled by the Main GPIO interrupt controller.
   */
  
  device->gpioint_cntrl->state[gpio].isr = NULL;
  device->gpioint_cntrl->state[gpio].event = NULL;
  device->gpioint_cntrl->state[gpio].isr_param = 0;
  device->gpioint_cntrl->state[gpio].intr_trigger = 0;

  GPIOIntr_UnConfigureGPIOInterrupt(device,gpio); 

  /*
   * Restore interrupts
   */
 (void)DALSYS_SyncLeave(device->gpioint_synchronization);

  return DAL_SUCCESS;

} /* END GPIOIntr_DeregisterIsr */


/*==========================================================================

  FUNCTION      GPIOIntr_IsInterruptEnabled

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_IsInterruptEnabled
(
  GPIOIntClientCtxt *pCtxt, 
  uint32  gpio, 
  uint32 *  state
) 
{
  uint32 irq;
  HAL_gpioint_InterruptType interrupt;
  GPIOIntDevCtxt *device;

  device = pCtxt->pGPIOIntDevCtxt;

  /*
   * Validate the given GPIO interrupt
   */
  if (!HAL_gpioint_IsSupported(gpio))
  {
    DALSYS_LogEvent(device->DevId, 
      DALSYS_LOGEVENT_WARNING, 
      "GPIOIntr_IsInterruptEnabled : GPIO %d is invalid", gpio);
    return DAL_ERROR;
  }

  interrupt = device->gpioint_cntrl->state[gpio].intr_type;
  if(interrupt != HAL_GPIOINT_SUMMARY)
  {
    irq = GPIOIntr_GetInterruptId(device,gpio);
    return GPIOIntrDirConn_IsInterruptEnabled( state, irq );
  } 
  /*
   * Check the HAL for summary interrupts.
   */
  return GPIOIntrSummary_IsInterruptEnabled( gpio, state );

} /* GPIOIntr_IsInterruptEnabled */


/*==========================================================================

  FUNCTION      GPIOIntr_IsInterruptRegistered

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_IsInterruptRegistered
(
  GPIOIntClientCtxt *pCtxt, 
  uint32  gpio, 
  uint32 *  state
) 
{
  
  GPIOIntDevCtxt *device;
  device = pCtxt->pGPIOIntDevCtxt;

  /*
   * Validate the given GPIO interrupt
   */
  if (!HAL_gpioint_IsSupported(gpio))
  {
    DALSYS_LogEvent(device->DevId, 
      DALSYS_LOGEVENT_WARNING, 
      "GPIOIntr_IsInterruptEnabled : GPIO %d is invalid", gpio);
    return DAL_ERROR;
  }

  (void)DALSYS_SyncEnter(device->gpioint_synchronization);
  if(device->gpioint_cntrl->state[gpio].gpio_intr_flags & GPIOINTF_REGISTERED)
  {
     *state = 1;
  }
  else
  {
     *state = 0;
  }
  (void)DALSYS_SyncLeave(device->gpioint_synchronization);
  return DAL_SUCCESS;

} /* GPIOIntr_IsInterruptRegistered */


/*==========================================================================

  FUNCTION      GPIOIntr_IsInterruptPending

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_IsInterruptPending
( 
  GPIOIntClientCtxt *pCtxt, 
  uint32  gpio, 
  uint32 *  state 
) 
{  
  uint32 irq,pending = 0,alt_pending = 0;
  HAL_gpioint_InterruptType interrupt;
  GPIOIntDevCtxt *device;

  device = pCtxt->pGPIOIntDevCtxt;

  /*
   * Validate the given GPIO interrupt
   */
  if (!HAL_gpioint_IsSupported(gpio))
  {
    DALSYS_LogEvent(device->DevId, DALSYS_LOGEVENT_WARNING, 
    "GPIOIntr_IsInterruptPending : GPIO %d is invalid", gpio);
    return DAL_ERROR;
  }

  interrupt = device->gpioint_cntrl->state[gpio].intr_type; 
  if(interrupt != HAL_GPIOINT_SUMMARY)
  {
    irq = GPIOIntr_GetInterruptId(device,gpio);
    DalInterruptController_IsInterruptPending(device->intr_handle, 
                                     (DALInterruptID)irq,&pending);

    if ( device->gpioint_cntrl->state[gpio].gpio_intr_flags & GPIOINTF_DIRECTINT_CONFIG &&
         device->gpioint_cntrl->state[gpio].intr_trigger == GPIOINT_TRIGGER_DUAL_EDGE )
    {
      DalInterruptController_IsInterruptPending(device->intr_handle, 
                                                (DALInterruptID)device->gpioint_cntrl->state[gpio].qurt_intr_id_secondary,
                                                &alt_pending);
    }

    *state = pending | alt_pending;
     return DAL_SUCCESS;
  }
  else
  {

    /*
     * Check the HAL for summary interrupts.
     */
    GPIOIntrSummary_IsInterruptPending(gpio,state);
  }
  return DAL_SUCCESS;

} /* END GPIOIntr_IsInterruptPending */


/*==========================================================================

  FUNCTION      GPIOIntr_IsInterruptSet

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_IsInterruptSet
( 
  GPIOIntClientCtxt *pCtxt, 
  uint32  gpio, 
  uint32 *  state 
) 
{  
  uint32 irq;
  HAL_gpioint_InterruptType interrupt;
  GPIOIntDevCtxt *device;

  device = pCtxt->pGPIOIntDevCtxt;

  /*
   * Validate the given GPIO interrupt
   */
  if (!HAL_gpioint_IsSupported(gpio))
  {
    DALSYS_LogEvent(device->DevId, DALSYS_LOGEVENT_WARNING, 
    "GPIOIntr_IsInterruptPending : GPIO %d is invalid", gpio);
    return DAL_ERROR;
  }

  interrupt = device->gpioint_cntrl->state[gpio].intr_type; 
  if(interrupt != HAL_GPIOINT_SUMMARY)
  {
    irq = GPIOIntr_GetInterruptId(device,gpio);
    return GPIOIntrDirConn_IsInterruptSet(state, irq);
  }
  else
  {

    /*
     * Check the HAL for summary interrupts.
     */
    GPIOIntrSummary_IsInterruptSet(gpio,state);
  }
  return DAL_SUCCESS;

} /* END GPIOIntr_IsInterruptSet */


/*==========================================================================

  FUNCTION      GPIOIntr_GetInterruptId

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

uint32 
GPIOIntr_GetInterruptId
(
  GPIOIntDevCtxt *device, 
  uint32 gpio
) 
{
  return device->gpioint_cntrl->state[gpio].qurt_intr_id;
} /* END GPIOIntr_GetInterruptId */


/*==========================================================================

  FUNCTION      GPIOIntr_ConfigMaointWakeup

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

void
GPIOIntr_ConfigMPMWakeup
(
  GPIOIntClientCtxt * pCtxt, 
  uint32 gpio, 
  boolean bEnable
)
{
  HAL_gpioint_TriggerType  trigger;
  pdc_trigger_config       sConfig;
  GPIOIntDevCtxt *         device;
  uint32                   nIntId    = GPIOINT_NONE;
  uint32                   nAltIntId = GPIOINT_NONE;
  
  sConfig.drv_num = PDC_DRV0;

  device   = pCtxt->pGPIOIntDevCtxt;
  
  trigger = device->gpioint_cntrl->state[gpio].intr_trigger;

  /*
   * Determine the PDC interrupt configuration
   */
  switch (trigger)
  {

    case HAL_GPIOINT_TRIGGER_DUAL_EDGE:
      sConfig.trig_type = TRIGGER_DUAL_EDGE;
      break;

    case HAL_GPIOINT_TRIGGER_HIGH:
      sConfig.trig_type = TRIGGER_LEVEL_HIGH;
      break;

    case HAL_GPIOINT_TRIGGER_LOW:
      sConfig.trig_type = TRIGGER_LEVEL_LOW;
      break;

    case HAL_GPIOINT_TRIGGER_RISING:
      sConfig.trig_type = TRIGGER_RISING_EDGE;
      break;

    case HAL_GPIOINT_TRIGGER_FALLING:
      sConfig.trig_type = TRIGGER_FALLING_EDGE;
      break;

    default:
      sConfig.trig_type = TRIGGER_LEVEL_HIGH;
      break;
  }

  /*
   * Inform PDC that the GPIO interrupt is enabled
   */
  if( PDC_SUCCESS == pdcGpio_config(gpio, &sConfig, &nIntId, &nAltIntId) )
  {
    device->gpioint_cntrl->state[gpio].gpio_intr_flags |= GPIOINTF_DIRECTINT_CONFIG;
    device->gpioint_cntrl->state[gpio].qurt_intr_id = nIntId;
    device->gpioint_cntrl->state[gpio].intr_type = HAL_GPIOINT_SPECIALCONN;
    if ( nAltIntId != GPIOINT_NONE )
    {
      device->gpioint_cntrl->state[gpio].qurt_intr_id_secondary = nAltIntId;
    }

    if ( bEnable )
    {
      pdcGpio_enable(gpio);
    }
  }
} /* END GPIOIntr_ConfigMPMWakeup */


/*==========================================================================

  FUNCTION      GPIOIntr_GetDirectIntrTrigger

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

uint32 
GPIOIntr_GetDirectIntrTrigger
(
  GPIOIntDevCtxt *device,
  GPIOIntTriggerType trigger
)
{
  uint32 direct_intr_trigger;

  switch(trigger)
  {
    case GPIOINT_TRIGGER_HIGH:
      direct_intr_trigger = DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER;
      break;
    case GPIOINT_TRIGGER_LOW:
      /*
       * This check is added because Polarity for the  main interrupt 
       * controller may not be negative. Hence a trigger translation is 
       * needed. 
       */
      if((device->gpioint_cntrl->interrupt_controller == GPIOINT_QGIC) ||
      (device->gpioint_cntrl->interrupt_controller == GPIOINT_L2VIC))
      {
        direct_intr_trigger = DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER;
      }
      else
      {
        direct_intr_trigger = DALINTRCTRL_ENABLE_LEVEL_LOW_TRIGGER;
      }
      break;
    case GPIOINT_TRIGGER_RISING:
      direct_intr_trigger = DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER;
      break;
    case GPIOINT_TRIGGER_FALLING:
       if((device->gpioint_cntrl->interrupt_controller == GPIOINT_QGIC) ||
      (device->gpioint_cntrl->interrupt_controller == GPIOINT_L2VIC))
      {
        direct_intr_trigger = DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER;
      }
      else
      {
        direct_intr_trigger = DALINTRCTRL_ENABLE_FALLING_EDGE_TRIGGER;
      }
      break;
    default:
      direct_intr_trigger = DALINTRCTRL_ENABLE_DEFAULT_SETTINGS;
      break;
  }
  return direct_intr_trigger;

} /* END GPIOIntr_GetDirectIntrTrigger */


/*==========================================================================

  FUNCTION      GPIOIntr_Restore

  DESCRIPTION   See GPIOInt.h

==========================================================================*/
DALResult GPIOIntr_Restore
( 
  GPIOIntClientCtxt * pCtxt
)
{
    return DAL_SUCCESS;
}

/*==========================================================================

  FUNCTION      GPIOIntr_DisableInterrupt

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_DisableInterrupt
( 
  GPIOIntClientCtxt * pCtxt, 
  uint32 gpio
)
{
  GPIOIntDevCtxt *device;
  uint32 interrupt_id;
  device = pCtxt->pGPIOIntDevCtxt;

  /*
   *  Validate given GPIO interrupt
   */
  if (!HAL_gpioint_IsSupported(gpio))
  {
    DALSYS_LogEvent(device->DevId, DALSYS_LOGEVENT_WARNING, 
    "GPIOIntr_DisableInterrupt :GPIO %d is not supported", gpio);
    return DAL_ERROR;
  }

  (void)DALSYS_SyncEnter(device->gpioint_synchronization);
  if(device->gpioint_cntrl->state[gpio].intr_type != HAL_GPIOINT_SUMMARY)
  {
    GPIOIntLog_Out(GPIOINTLOG_ENABLE, device->hULog, 1, "Disable: Disabling direct connect GPIO %d", gpio );
    interrupt_id = GPIOIntr_GetInterruptId(device,gpio);
    if ( device->gpioint_cntrl->state[gpio].gpio_intr_flags & GPIOINTF_DIRECTINT_CONFIG &&
         device->gpioint_cntrl->state[gpio].intr_trigger == GPIOINT_TRIGGER_DUAL_EDGE )
    {
      DalInterruptController_InterruptDisable(device->intr_handle,device->gpioint_cntrl->state[gpio].qurt_intr_id);
      DalInterruptController_InterruptDisable(device->intr_handle,device->gpioint_cntrl->state[gpio].qurt_intr_id_secondary);
    }
    else
    {
      GPIOIntrDirConn_DisableInterrupt( gpio, interrupt_id,
                                        device->gpioint_cntrl->state[gpio].intr_type );
    }
  }
  else
  {
    GPIOIntLog_Out(GPIOINTLOG_ENABLE, device->hULog, 1, "Disable: Disabling summary GPIO %d", gpio );
    GPIOIntrSummary_DisableInterrupt(gpio);
  }

  /*
   * Disable interrupt wake up.
   */ 
  pdcGpio_disable( gpio );
  GPIOIntLog_Out(GPIOINTLOG_MPM, device->hULog, 2, "PDC Disable: Disabling GPIO %d", gpio );

  (void)DALSYS_SyncLeave(device->gpioint_synchronization);
  return DAL_SUCCESS;

} /* END GPIOIntr_DisableInterrupt */


/*==========================================================================

  FUNCTION      GPIOIntr_DisableGPIOInterrupt

  DESCRIPTION   See GPIOInt.h

==========================================================================*/
DALResult 
GPIOIntr_DisableGPIOInterrupt
( 
  GPIOIntClientCtxt * pCtxt, 
  uint32 gpio
)
{
  GPIOIntDevCtxt *device;
  uint32 interrupt_id;
  device = pCtxt->pGPIOIntDevCtxt;

  /*
   *  Validate given GPIO interrupt
   */
  if (!HAL_gpioint_IsSupported(gpio))
  {
    DALSYS_LogEvent(device->DevId, DALSYS_LOGEVENT_WARNING, 
    "GPIOIntr_DisableInterrupt : GPIO %d is not supported", gpio);
    return DAL_ERROR;
  }

  (void)DALSYS_SyncEnter(device->gpioint_synchronization);
  if(device->gpioint_cntrl->state[gpio].intr_type != HAL_GPIOINT_SUMMARY)
  {
    GPIOIntLog_Out(GPIOINTLOG_ENABLE, device->hULog, 1, "Disable: Disabling direct connect GPIO %d", gpio );
    interrupt_id = GPIOIntr_GetInterruptId(device,gpio);
    if ( device->gpioint_cntrl->state[gpio].gpio_intr_flags & GPIOINTF_DIRECTINT_CONFIG &&
         device->gpioint_cntrl->state[gpio].intr_trigger == GPIOINT_TRIGGER_DUAL_EDGE )
    {
      DalInterruptController_InterruptDisable(device->intr_handle,device->gpioint_cntrl->state[gpio].qurt_intr_id);
      DalInterruptController_InterruptDisable(device->intr_handle,device->gpioint_cntrl->state[gpio].qurt_intr_id_secondary);
    }
    else
    {
      GPIOIntrDirConn_DisableInterrupt( gpio, interrupt_id,
                                        device->gpioint_cntrl->state[gpio].intr_type );
    }
  }
  else
  {
    GPIOIntLog_Out(GPIOINTLOG_ENABLE, device->hULog, 1, "Disable: Disabling summary GPIO %d", gpio );
    GPIOIntrSummary_DisableInterrupt(gpio);
  }

  (void)DALSYS_SyncLeave(device->gpioint_synchronization);
  return DAL_SUCCESS;

} /* END GPIOIntr_DisableGPIOInterrupt */


/*==========================================================================

  FUNCTION      GPIOIntr_EnableInterrupt

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_EnableInterrupt
(
  GPIOIntClientCtxt * pCtxt, 
  uint32 gpio
)
{
  GPIOIntDevCtxt *device;
  uint32 interrupt_id;

  device = pCtxt->pGPIOIntDevCtxt;

  /*
   *  Validate given GPIO interrupt
   */
  if (!HAL_gpioint_IsSupported(gpio))
  {
    DALSYS_LogEvent(device->DevId, DALSYS_LOGEVENT_WARNING, 
    "GPIOIntr_EnableInterrupt: GPIO %d is not supported", gpio);
    return DAL_ERROR;
  }
  (void)DALSYS_SyncEnter(device->gpioint_synchronization);

  /*
   * If this interrupt is not externally managed only then return an error.
   */
  if(!(device->gpioint_cntrl->state[gpio].gpio_intr_flags & GPIOINTF_REGISTERED) &&
     (!(device->gpioint_cntrl->state[gpio].gpio_intr_flags & GPIOINTF_EXTERNAL_CONFIG)) &&
     (!(device->gpioint_cntrl->state[gpio].gpio_intr_flags & GPIOINTF_DIRECTINT_CONFIG)))
  {
    /*
     * This GPIO is not registered for a handler.we should not enable it.
     */
    DALSYS_LogEvent(device->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
    "GPIOIntr_EnableInterrupt: GPIO %d does not have a client ", gpio);
    (void)DALSYS_SyncLeave(device->gpioint_synchronization);

    return DAL_ERROR;
  }
  if(device->gpioint_cntrl->state[gpio].intr_type != HAL_GPIOINT_SUMMARY)
  {
    GPIOIntLog_Out(GPIOINTLOG_ENABLE, device->hULog, 1, "Enable: Enabling direct connect GPIO %d", gpio );

    interrupt_id = GPIOIntr_GetInterruptId(device,gpio);
    if ( device->gpioint_cntrl->state[gpio].gpio_intr_flags & GPIOINTF_DIRECTINT_CONFIG &&
         device->gpioint_cntrl->state[gpio].intr_trigger == GPIOINT_TRIGGER_DUAL_EDGE )
    {
      DalInterruptController_InterruptEnable(device->intr_handle,device->gpioint_cntrl->state[gpio].qurt_intr_id);
      DalInterruptController_InterruptEnable(device->intr_handle,device->gpioint_cntrl->state[gpio].qurt_intr_id_secondary);
    }
    else
    {
      GPIOIntrDirConn_EnableInterrupt( gpio, interrupt_id,
                                       device->gpioint_cntrl->state[gpio].intr_type );
    }
  }
  else
  {  
    GPIOIntLog_Out(GPIOINTLOG_ENABLE, device->hULog, 1, "Enable: Enabling summary GPIO %d", gpio );

    GPIOIntrSummary_EnableInterrupt( gpio, 
                                     device->gpioint_cntrl->processor, 
                                     device->gpioint_cntrl->state[gpio].intr_type );

  }

  /*
   * Setup interrupt wake up.
   */ 
  GPIOIntr_ConfigMPMWakeup(pCtxt,gpio,TRUE);

  (void)DALSYS_SyncLeave(device->gpioint_synchronization);
  return DAL_SUCCESS;

} /* END GPIOIntr_EnableInterrupt */

/*==========================================================================

  FUNCTION      GPIOIntr_EnableGPIOInterrupt

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_EnableGPIOInterrupt
(
  GPIOIntClientCtxt * pCtxt, 
  uint32 gpio
)
{
  GPIOIntDevCtxt *device;
  uint32 interrupt_id;

  device = pCtxt->pGPIOIntDevCtxt;

  /*
   *  Validate given GPIO interrupt
   */
  if (!HAL_gpioint_IsSupported(gpio))
  {
    DALSYS_LogEvent(device->DevId, DALSYS_LOGEVENT_WARNING, 
    "GPIOIntr_EnableGPIOInterrupt: GPIO %d is not supported", gpio);
    return DAL_ERROR;
  }
  (void)DALSYS_SyncEnter(device->gpioint_synchronization);

  /*
   * If this interrupt is not externally managed only then return an error.
   */
  if(!(device->gpioint_cntrl->state[gpio].gpio_intr_flags & GPIOINTF_REGISTERED) &&
     (!(device->gpioint_cntrl->state[gpio].gpio_intr_flags & GPIOINTF_EXTERNAL_CONFIG)) &&
     (!(device->gpioint_cntrl->state[gpio].gpio_intr_flags & GPIOINTF_DIRECTINT_CONFIG)))
  {
    /*
     * This GPIO is not registered for a handler.we should not enable it.
     */
    DALSYS_LogEvent(device->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
    "GPIOIntr_EnableGPIOInterrupt: GPIO %d does not have a client ", gpio);
    (void)DALSYS_SyncLeave(device->gpioint_synchronization);

    return DAL_ERROR;
  }
  if(device->gpioint_cntrl->state[gpio].intr_type != HAL_GPIOINT_SUMMARY)
  {
    GPIOIntLog_Out(GPIOINTLOG_ENABLE, device->hULog, 1, "Enable: Enabling direct connect GPIO %d", gpio );
    interrupt_id = GPIOIntr_GetInterruptId(device,gpio);
    if ( device->gpioint_cntrl->state[gpio].gpio_intr_flags & GPIOINTF_DIRECTINT_CONFIG &&
         device->gpioint_cntrl->state[gpio].intr_trigger == GPIOINT_TRIGGER_DUAL_EDGE )
    {
      DalInterruptController_InterruptEnable(device->intr_handle,device->gpioint_cntrl->state[gpio].qurt_intr_id);
      DalInterruptController_InterruptEnable(device->intr_handle,device->gpioint_cntrl->state[gpio].qurt_intr_id_secondary);
    }
    else
    {
      GPIOIntrDirConn_EnableInterrupt( gpio, interrupt_id,
                                       device->gpioint_cntrl->state[gpio].intr_type );
    }
  }
  else
  {  
    GPIOIntLog_Out(GPIOINTLOG_ENABLE, device->hULog, 1, "Enable: Enabling summary GPIO %d", gpio );
    GPIOIntrSummary_EnableInterrupt( gpio,
                                     device->gpioint_cntrl->processor,
                                     device->gpioint_cntrl->state[gpio].intr_type );
  }

  (void)DALSYS_SyncLeave(device->gpioint_synchronization);
  return DAL_SUCCESS;

} /* END GPIOIntr_EnableGPIOInterrupt */


/*==========================================================================

  FUNCTION      GPIOIntr_MapMPMInterrupts

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_MapMPMInterrupt
(
  GPIOIntClientCtxt * pCtxt,
  uint32 gpio, 
  uint32 mpm_interrupt_id
) 
{
  return DAL_ERROR;
} /* END GPIOIntr_MapMPMInterrupts */


/*==========================================================================

   FUNCTION      GPIOIntr_ClearInterrupt

   DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_ClearInterrupt
(
  GPIOIntClientCtxt * pCtxt, 
  uint32 gpio
)
{
  GPIOIntDevCtxt *device;
  uint32 interrupt_id;
  
  device = pCtxt->pGPIOIntDevCtxt;

  /*
   *  Validate the given GPIO interrupt
   */
  if (!HAL_gpioint_IsSupported(gpio))
  {
    DALSYS_LogEvent(device->DevId, DALSYS_LOGEVENT_WARNING, 
    "GPIOIntr_ClearInterrupt : GPIO %d is not supported", gpio);
    return DAL_ERROR;
  }
  (void)DALSYS_SyncEnter(device->gpioint_synchronization);
  if( device->gpioint_cntrl->state[gpio].intr_type != HAL_GPIOINT_SUMMARY || 
      device->gpioint_cntrl->state[gpio].gpio_intr_flags & GPIOINTF_DIRECTINT_CONFIG ) 
  {
    interrupt_id = GPIOIntr_GetInterruptId(device,gpio);
    GPIOIntLog_Out(GPIOINTLOG_CLEAR, device->hULog, 2, "Clear: Clearing GPIO %d direct connect interrupt ID %d", gpio, interrupt_id );
    GPIOIntrDirConn_ClearInterrupt( interrupt_id );
  }
  else
  {  
    GPIOIntLog_Out(GPIOINTLOG_CLEAR, device->hULog, 1, "Clear: Clearing GPIO %d summary interrupt", gpio );
    GPIOIntrSummary_ClearInterrupt(gpio);
  }
  (void)DALSYS_SyncLeave(device->gpioint_synchronization);
  return DAL_SUCCESS;

} /* END GPIOIntr_ClearInterrupt */


/*==========================================================================

   FUNCTION      GPIOIntr_TriggerInterrupt

   DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_TriggerInterrupt
(
  GPIOIntClientCtxt * pCtxt, 
  uint32  gpio
)
{
  uint32 interrupt_id;
  GPIOIntDevCtxt *device;
  DALResult eResult;

  device = (GPIOIntDevCtxt *)pCtxt->pGPIOIntDevCtxt;

  /*
   * Validate the given gpio interrupt
   */
  if (!HAL_gpioint_IsSupported(gpio))
  {
    DALSYS_LogEvent(device->DevId, DALSYS_LOGEVENT_WARNING, 
    "GPIOIntr_TriggerInterrupt: GPIO %d is not supported", gpio);
    return DAL_ERROR;
  }

  /*
   * Disable interrupts
   */
  (void)DALSYS_SyncEnter(device->gpioint_synchronization);

  if((device->gpioint_cntrl->state[gpio].isr == NULL) && 
    (device->gpioint_cntrl->state[gpio].event == NULL))
  {
    DALSYS_LogEvent(device->DevId, DALSYS_LOGEVENT_WARNING, 
    "GPIOIntr_TriggerInterrupt: GPIO %d does not have a client ", gpio);
    (void)DALSYS_SyncLeave(device->gpioint_synchronization);
    return DAL_ERROR;
  }

  if(device->gpioint_cntrl->state[gpio].intr_type == HAL_GPIOINT_SUMMARY)
  {
    GPIOIntLog_Out(GPIOINTLOG_TRIGGER, device->hULog, 1, "Trigger: Triggering GPIO %d summary interrupt", gpio );
    eResult = GPIOIntrSummary_TriggerInterrupt(gpio);
  }
  else
  {
    interrupt_id = GPIOIntr_GetInterruptId(device,gpio);
    GPIOIntLog_Out(GPIOINTLOG_TRIGGER, device->hULog, 2, "Trigger: Triggering GPIO %d interrupt ID %d", gpio, interrupt_id );
    eResult = GPIOIntrDirConn_TriggerInterrupt( gpio, interrupt_id,
                                                device->gpioint_cntrl->gpioint_uimage );
  }

  /* 
   * Restore interrupts
   */
  (void)DALSYS_SyncLeave(device->gpioint_synchronization);
  return eResult;
                                        
} /* END GPIOIntr_TriggerInterrupt */

#ifdef TEST_GPIOINT
static void GPIOInt_TestIsr(uint32 nData)
{

}
#endif /*TEST_GPIOINT*/

/*==========================================================================

   FUNCTION      GPIOInt_Init

   DESCRIPTION   See DDIGPIOInt.h

==========================================================================*/
#ifdef TEST_GPIOINT
static volatile uint32 cpnTestGpio = 123; 
static volatile GPIOIntTriggerType cpeTrigger = GPIOINT_TRIGGER_DUAL_EDGE;
static volatile uint32 cpnWaitTimeUs = 50;
#endif /*TEST_GPIOINT*/
void GPIOInt_Init(void)
{

  DalDeviceHandle *  gpioint_handle;

  GPIOIntData.GPIOInt_Init = TRUE;
  /* 
   * attach to the gpioint dal.
   */
  DAL_DeviceAttach(DALDEVICEID_GPIOINT, &gpioint_handle);

  #ifdef TEST_GPIOINT

  GPIOInt_RegisterIsr(gpioint_handle, cpnTestGpio, cpeTrigger, (GPIOINTISR)GPIOInt_TestIsr, 0);
     
  busywait(cpnWaitTimeUs);

  GPIOInt_TriggerInterrupt(gpioint_handle, cpnTestGpio);

  busywait(cpnWaitTimeUs);

  GPIOInt_DeregisterIsr(gpioint_handle, cpnTestGpio, (GPIOINTISR)GPIOInt_TestIsr);

  #endif /*TEST_GPIOINT*/
  
} /* END GPIOInt_Init */


/*==========================================================================

   FUNCTION      GPIOIntr_SetDirectConnectGPIOMapping

   DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_SetDirectConnectGPIOMapping
(
  GPIOIntClientCtxt * pCtxt, 
  uint32  gpio,
  uint32  direct_connect_interrupt
)
{
  uint32 index;
  DALResult nResult;
  GPIOIntDevCtxt *device;

  nResult = DAL_ERROR;
  device = (GPIOIntDevCtxt *)pCtxt->pGPIOIntDevCtxt;

  /*
   * Validate the given gpio interrupt
   */
  if (!HAL_gpioint_IsSupported(gpio))
  {
    DALSYS_LogEvent(device->DevId, DALSYS_LOGEVENT_WARNING, 
    "GPIOIntr_SetDirectConnectGPIOMapping: GPIO %d is not supported", gpio);
    return DAL_ERROR;
  }

  (void)DALSYS_SyncEnter(device->gpioint_synchronization);
  if ((device->gpioint_cntrl->state[gpio].gpio_intr_flags & GPIOINTF_REGISTERED)!=0)
  {
    /*
     * The interrupt is already registered. The client is mapping the wrong GPIO.
     */
    (void)DALSYS_SyncLeave(device->gpioint_synchronization);
    return DAL_ERROR;
  }

  for(index =0;index < device->gpioint_cntrl->direct_intr_number; index++)
  {
    if((device->gpioint_cntrl->gpioint_config_map[index].interrupt_id == direct_connect_interrupt)
       &&((device->gpioint_cntrl->gpioint_config_map[index].flags & GPIOINTF_EXTERNAL_CONFIG)!=0))
    {
      if (device->gpioint_cntrl->gpioint_config_map[index].gpio != GPIOINT_NONE) 
      {
        /*
         * This direct connect line has already been taken.
         */
        nResult = DAL_ERROR;
        break;
      }
      device->gpioint_cntrl->state[gpio].gpio_intr_flags |= GPIOINTF_EXTERNAL_CONFIG;
      device->gpioint_cntrl->state[gpio].intr_type = 
      device->gpioint_cntrl->gpioint_config_map[index].interrupt;
      device->gpioint_cntrl->gpioint_config_map[index].gpio = gpio;
      HAL_gpioint_SetDirConnIntr(gpio,(HAL_gpioint_InterruptType)device->gpioint_cntrl->state[gpio].intr_type);
            
      nResult = DAL_SUCCESS;
      break;
    }
  }

  (void)DALSYS_SyncLeave(device->gpioint_synchronization);

  return nResult;
                                        
} /* END GPIOIntr_SetDirectConnectGPIOMapping */


/*==========================================================================

   FUNCTION      GPIOIntr_MapGPIOIntTrigger

   DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult
GPIOIntr_MapGPIOIntTrigger
(
  uint32  nGPIOTrigger,
  uint32* nIntCTrigger
)
{
  if( nIntCTrigger == NULL )
  {
    return DAL_ERROR;
  }

  switch(nGPIOTrigger) 
  {
    case GPIOINT_TRIGGER_HIGH:
      *nIntCTrigger = DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER;
      break;
    case GPIOINT_TRIGGER_LOW:
      *nIntCTrigger = DALINTRCTRL_ENABLE_LEVEL_LOW_TRIGGER;
      break;
    case GPIOINT_TRIGGER_FALLING:
      *nIntCTrigger = DALINTRCTRL_ENABLE_FALLING_EDGE_TRIGGER;
      break;
    case GPIOINT_TRIGGER_DUAL_EDGE:
    case GPIOINT_TRIGGER_RISING:
    default:
      *nIntCTrigger = DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER;
      break;
  }
  return DAL_SUCCESS;
}

