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

$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/GPIOInt/src/kernel/GPIOInt_Summary.c#1 $

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
GPIOIntrSummary_Init(GPIOIntDevCtxt *device, DalDeviceHandle* pInterruptHandle)
{
  intr_handle = pInterruptHandle;
  GPIOIntrSummaryInternal_Init(device, intr_handle);
  return DAL_SUCCESS;
}

DALResult
GPIOIntrSummary_TriggerInterrupt( uint32 gpio )
{
  HAL_gpioint_TriggerInterrupt(gpio);
  return DAL_SUCCESS;
}

DALResult
GPIOIntrSummary_ClearInterrupt( uint32 gpio )
{
  HAL_gpioint_Clear(gpio);
  return DAL_SUCCESS;
}

DALResult
GPIOIntrSummary_EnableInterrupt( uint32 gpio, 
                                 HAL_gpioint_ProcessorType processor,
                                 HAL_gpioint_InterruptType intr_type  )
{
  HAL_gpioint_SetTargetProcessor(processor,gpio,intr_type);

  HAL_gpioint_EnableRawStatus(gpio);

  /*
   * Wait for the configuration to set in. 
   */ 
  busywait(5);

  /*
   * Clear and enable the interrupt
   */
  HAL_gpioint_Clear(gpio);
  HAL_gpioint_Enable(gpio);

  return DAL_SUCCESS;
}

DALResult
GPIOIntrSummary_DisableInterrupt( uint32 gpio )
{
  HAL_gpioint_Disable(gpio);
  return DAL_SUCCESS;
}

DALResult
GPIOIntrSummary_IsInterruptSet( uint32 gpio, uint32* state )
{
  *state = (uint32)HAL_gpioint_IsSet(gpio);
  return DAL_SUCCESS;
}

DALResult
GPIOIntrSummary_IsInterruptPending( uint32 gpio, uint32* state )
{
  *state = (uint32)HAL_gpioint_IsPending(gpio);
  return DAL_SUCCESS;
}

DALResult
GPIOIntrSummary_IsInterruptEnabled( uint32 gpio, uint32* state )
{
  *state = (uint32)HAL_gpioint_IsEnabled(gpio);
  return DAL_SUCCESS;
}

DALResult
GPIOIntrSummary_ConfigureGPIOInterrupt( GPIOIntDevCtxt * device,
                                        uint32 gpio, 
                                        HAL_gpioint_TriggerType trigger,
                                        HAL_gpioint_ProcessorType processor,
                                        HAL_gpioint_InterruptType intr_type )
{
  HAL_gpioint_SetTargetProcessor(processor, gpio, intr_type);
  HAL_gpioint_EnableRawStatus(gpio);

  /*
   * Set SUMMARY interrupt trigger if the gpio is not a DIRECT CONNECT.
   */ 
  HAL_gpioint_SetTrigger(gpio,trigger);

  /*
   * Wait for the configuration to set in. 
   */ 
  busywait(5);

  GPIOIntrSummaryInternal_ConfigureGPIOInterrupt(device, gpio);

  /*
   * Clear and enable the interrupt
   */
  HAL_gpioint_Clear(gpio);
  HAL_gpioint_Enable(gpio);

  return DAL_SUCCESS;
}

DALResult
GPIOIntrSummary_UnConfigureGPIOInterrupt( GPIOIntDevCtxt * device,
                                          uint32 gpio, 
                                          uint32 interrupt_id,
                                          uint8  uimage )
{
  HAL_gpioint_Disable(gpio);

    /*
   * Clear the uimage mapping for this direct connect interrupt.
   */
  GPIOIntrSummaryInternal_UnConfigureGPIOInterrupt(device, gpio, uimage);

  return DAL_SUCCESS;
}

DALResult
GPIOIntrSummary_SetTrigger( uint32  gpio, GPIOIntTriggerType  trigger )
{
  HAL_gpioint_SetTrigger(gpio,(HAL_gpioint_TriggerType)trigger);
  return DAL_SUCCESS;
}



