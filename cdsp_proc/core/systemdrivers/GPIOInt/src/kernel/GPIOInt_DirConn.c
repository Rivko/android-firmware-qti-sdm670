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

$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/GPIOInt/src/kernel/GPIOInt_DirConn.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
3/18/2015  cpaulo  First draft created. 
===========================================================================*/
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DALStdErr.h"
#include "DDITimetick.h"
#include "DDIGPIOMgr.h"
#include "DDITlmm.h"
#include "busywait.h"
#include "DDIInterruptController.h"
#include "HALgpioint.h"
#include "DALGlbCtxt.h"
#include "GPIOInt.h"
#include "GPIOInt_Summary.h"
#include "DDIHWIO.h"
#include "qurt.h"

static DalDeviceHandle* intr_handle = NULL;

DALResult
GPIOIntrDirConn_Init(DalDeviceHandle* pInterruptHandle)
{
  intr_handle = pInterruptHandle;
  return DAL_SUCCESS;
}

DALResult
GPIOIntrDirConn_DeInit(void)
{
  intr_handle = NULL;
  return DAL_SUCCESS;
}

DALResult
GPIOIntrDirConn_TriggerInterrupt( uint32 gpio, 
                                  uint32 interrupt_id,
                                  uint8  uImage )
{
  if ( DalInterruptController_InterruptTrigger( intr_handle,interrupt_id) != DAL_SUCCESS )
  {
    return DAL_ERROR;
  }
  return DAL_SUCCESS;
}

DALResult
GPIOIntrDirConn_ClearInterrupt( uint32 interrupt_id )
{
  return DalInterruptController_InterruptClear(intr_handle,interrupt_id);
}

DALResult
GPIOIntrDirConn_EnableInterrupt( uint32 gpio, 
                                 uint32 interrupt_id,
                                 HAL_gpioint_InterruptType intr_type  )
{
  if( intr_type != HAL_GPIOINT_SPECIALCONN )
  {
    HAL_gpioint_SetDirConnIntr(gpio,intr_type);
  }
  return DalInterruptController_InterruptEnable(intr_handle,interrupt_id);
}

DALResult
GPIOIntrDirConn_DisableInterrupt( uint32 gpio, 
                                  uint32 interrupt_id,
                                  HAL_gpioint_InterruptType intr_type )
{
  if( intr_type != HAL_GPIOINT_SPECIALCONN )
  {
    HAL_gpioint_DisableDirConnIntr(gpio);
  }
  return DalInterruptController_InterruptDisable(intr_handle,interrupt_id);
}

DALResult
GPIOIntrDirConn_IsInterruptSet( uint32* state, uint32 irq )
{
  DALResult bSet;
  bSet =  DalInterruptController_InterruptStatus(intr_handle, 
                                                 (DALInterruptID)irq);
  if(bSet == DAL_INTERRUPT_SET)
  {
    *state = 1;
  }
  else
  {
    *state = 0;
  }

  return DAL_SUCCESS;
}

DALResult
GPIOIntrDirConn_IsInterruptPending( uint32* state, uint32 irq )
{
  return DalInterruptController_IsInterruptPending(intr_handle, 
                                                   (DALInterruptID)irq,state);
}

DALResult
GPIOIntrDirConn_IsInterruptEnabled( uint32* state, uint32 irq )
{
  return DalInterruptController_IsInterruptEnabled(intr_handle, 
                                                   (DALInterruptID)irq,state);
}

DALResult
GPIOIntrDirConn_ConfigureGPIOInterrupt( uint32 gpio, 
                                        uint32 interrupt_id,
                                        uint32 direct_connect_trigger,
                                        HAL_gpioint_TriggerType trigger,
                                        GPIOIntDataType* state,
                                        boolean bIsDirectConfig,
                                        uint8 uImage )
{
  if( !bIsDirectConfig )
  {
    HAL_gpioint_SetDirConnIntr(gpio, state->intr_type);
    HAL_gpioint_SetDirConnIntrPolarity((HAL_gpioint_TriggerType)trigger,gpio);
  }

  if(state->event)
  {
    if(DAL_SUCCESS != DalInterruptController_RegisterEvent(intr_handle, 
       interrupt_id,state->event,direct_connect_trigger))
    {
      return DAL_ERROR;
    }
  }
  else if(DAL_SUCCESS != DalInterruptController_RegisterISR(intr_handle, 
    (DALInterruptID)interrupt_id,(DALISR)state->isr,
    (DALISRCtx)state->isr_param,direct_connect_trigger))
  {
    return DAL_ERROR;
  }

  return DAL_SUCCESS;
}

DALResult
GPIOIntrDirConn_UnConfigureGPIOInterrupt( uint32 gpio, 
                                          uint32 interrupt_id,
                                          uint8  uimage )
{
  /* 
   * The Main interrupt controller handles the direct connects
   */
  DalInterruptController_Unregister(intr_handle,interrupt_id);

	/*
	 * Clear direct connect mapping from Direct Connect select field
	 */
	HAL_gpioint_DisableDirConnIntr(gpio);

  return DAL_SUCCESS;
}

DALResult
GPIOIntrDirConn_SetTrigger( uint32 gpio, 
                            uint32 interrupt_id,
                            GPIOIntTriggerType trigger,
                            uint32 direct_connect_trigger,
                            boolean bIsDirectInt )
{
  DalInterruptController_InterruptDisable(intr_handle,interrupt_id);

  if( !bIsDirectInt )
  {
    HAL_gpioint_SetDirConnIntrPolarity((HAL_gpioint_TriggerType)trigger, gpio); 
  }

  DalInterruptController_SetTrigger(intr_handle,interrupt_id,direct_connect_trigger);
  DalInterruptController_InterruptClear(intr_handle, interrupt_id);
  DalInterruptController_InterruptEnable(intr_handle,interrupt_id);

  return DAL_SUCCESS;
}



