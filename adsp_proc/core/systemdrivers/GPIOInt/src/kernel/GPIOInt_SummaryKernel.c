/*==============================================================================

FILE:      GPIOInt_SummaryKernel.c

DESCRIPTION
  This modules implements the API to utilize the Dal GPIO interrupt controller. 
  The definitions in this module are implemented internally and should not be 
  exposed for external client usage directly without using the accompanying DDI
  interface file for this module.

REFERENCES

       Copyright © 2015 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/systemdrivers/GPIOInt/src/kernel/GPIOInt_SummaryKernel.c#1 $

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
#include "GPIOIntQDI.h"

static DalDeviceHandle* intr_handle = NULL;


/*==========================================================================

  FUNCTION      GPIOIntr_Isr

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

void 
GPIOIntr_Isr
(
  void *pCtxt
)
{
  uint32  gpio, pending;
  uint32  retry_count = 0;
  uint32 nPD = 0;
  HAL_gpioint_TriggerType nTrigger = HAL_GPIOINT_TRIGGER_HIGH;
  GPIOIntDevCtxt *device = (GPIOIntDevCtxt *)pCtxt;
  boolean bIsEdgeTriggered = TRUE;

  (void)DALSYS_SyncEnter(device->gpioint_synchronization);

  /*
   *The idea is to	- loop over the known gpio interrupts and if none are pending, 
   *check the summary interrupt.  Repeat this 10 times and if the summary line continues to 
   *be asserted and we cannot find any pending GPIO interrupts then abort.
   */
  do
  {
   /*
    * We would only poll the GPIO INTR Status Regs for which an Handler is registered.          
    */      
    for(gpio = 0; gpio < device->gpioint_cntrl->gpio_number; gpio++)
    {
      nPD = 0;

      if ( DAL_SUCCESS != DalGPIOMgr_GetGPIOCurrentPD( device->hGPIOMgr, gpio, &nPD ) )
      {
        nPD = 0;
      }

      if(((device->gpioint_cntrl->state[gpio].gpio_intr_flags & GPIOINTF_REGISTERED) &&
          (device->gpioint_cntrl->state[gpio].intr_type == HAL_GPIOINT_SUMMARY) ) ||
          nPD != 0 )
      {
        if(HAL_gpioint_IsPending(gpio) == TRUE)
        {
          if (nPD == 0) 
          {
            /*
             * Run the ISR
             */
            GPIOIntr_RunIsr(device,gpio);
          }
          else
          {

            /* 
             * Get the trigger type to determine when to clear the interrupt
             */
            HAL_gpioint_GetTrigger( gpio, &nTrigger );
      	  
            if( nTrigger == HAL_GPIOINT_TRIGGER_HIGH ||
                nTrigger == HAL_GPIOINT_TRIGGER_LOW )
            {
               bIsEdgeTriggered = FALSE;
            }

            /*
              * Clear the interrupt before if it's edge triggered
              */
            if( bIsEdgeTriggered )
            {
              HAL_gpioint_Clear(gpio);
            }

           /*
             * Send the GPIO Interrupt to the correct PD
             */
            GPIOIntQDI_SetSummarySignal(nPD, gpio);

            /*
             * Wait for Ack from User PD
             */
            GPIOIntQDI_WaitOnAckSummarySignal(nPD);

            /*
              * Clear the interrupt after if it's level triggered
              */
            if( !bIsEdgeTriggered )
            {
              HAL_gpioint_Clear(gpio);
            }

         }

         /* 
          * Get the summary INTR pending status to check if we have serviced all the pending ones.
          */
          DalInterruptController_IsInterruptPending(device->intr_handle, 
                (DALInterruptID)device->gpioint_cntrl->summary_intr_id,&pending);

          if(!pending)
          {
          /* Summary INTR is not pending, all the pending GPIOINTRs are serviced
           * No need to check the remaining GPIO INTR status Registers.
           */
            (void)DALSYS_SyncLeave(device->gpioint_synchronization);
            return;
          }

         /*
          * we need to loop over 10 times consecutively with summary INTR status pending.
          * Reset retry_count = 0 if a valid GPIOINTR is triggered in the middle.   		  
          */		   
          retry_count  = 0;		   
        } 	  
      }
    }

    /* 
    * Get the summary INTR pending status to check if the summary line is still asserted.
    */
    DalInterruptController_IsInterruptPending(device->intr_handle, 
                           (DALInterruptID)device->gpioint_cntrl->summary_intr_id,&pending);

  }while((pending) && (++retry_count < GPIOINT_SUMMARY_INTR_NO_OF_TRIALS));

  if(retry_count >= GPIOINT_SUMMARY_INTR_NO_OF_TRIALS)
  {
    device->gpioint_cntrl->gpioint_unhandled++;		
    /*
     *The summary line continues to be asserted and we could not find any pending GPIO interrupts.		
     *ERROR_FATAL if a GPIOINT is not found in 10 retrials.
     */
    DALSYS_LogEvent(device->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
          "GPIOIntr_Isr: GPIO IRQ is wrongly targeted to LPASS QDSP by another Subsystem"); 
  }

  (void)DALSYS_SyncLeave(device->gpioint_synchronization);

} /* END GPIOIntr_isr */


/*==========================================================================

  FUNCTION      GPIOIntr_LogEvent

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

void 
GPIOIntr_LogEvent
(
  GPIOIntDevCtxt *device,
  uint32 gpio
)
{
#ifdef GPIOINT_KERNELPDLOG
  GPIOIntCntrlType *gpioint_cntrl = device->gpioint_cntrl;
  uint32 index = gpioint_cntrl->log.index;
  gpioint_cntrl->log.entries[index].gpio = gpio;
  DalTimetick_GetTimetick64(device->timetick_handle, &gpioint_cntrl->log.entries[index].timestamp);

  gpioint_cntrl->state[gpio].last =
  gpioint_cntrl->log.entries[index].timestamp;
  gpioint_cntrl->state[gpio].cnt++;

  if (++gpioint_cntrl->log.index >= MAX_INTERRUPT_LOG_ENTRIES)
  {
    gpioint_cntrl->log.index = 0;
  }
#endif
} /* END GPIOIntr_log_event */


/*==========================================================================

  FUNCTION      GPIOIntr_RunIsr

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

void 
GPIOIntr_RunIsr
(
  GPIOIntDevCtxt *device,
  uint32 gpio
)
{ 
  boolean bIsEdgeTriggered = TRUE;
  HAL_gpioint_TriggerType nTrigger = HAL_GPIOINT_TRIGGER_HIGH;
  GPIOINTISR local_isr = NULL;
  DALSYSEventHandle local_event = NULL;
  GPIOINTISRCtx local_isr_param;

  /* 
   * Get the trigger type to determine when to clear the interrupt
   */
  HAL_gpioint_GetTrigger( gpio, &nTrigger );

  if( nTrigger == HAL_GPIOINT_TRIGGER_HIGH ||
      nTrigger == HAL_GPIOINT_TRIGGER_LOW )
  {
    bIsEdgeTriggered = FALSE;
  }

  /*
   * Log the GPIO that fired
   */
  GPIOIntr_LogEvent(device,gpio);

  /*
   * Clear the interrupt before if it's edge triggered
   */
  if( bIsEdgeTriggered )
  {
    HAL_gpioint_Clear(gpio);
  }

  local_isr = device->gpioint_cntrl->state[gpio].isr;
  local_event = device->gpioint_cntrl->state[gpio].event;
  local_isr_param = device->gpioint_cntrl->state[gpio].isr_param;

  (void)DALSYS_SyncLeave(device->gpioint_synchronization);

  /*
   * Get the ISR and run it
   */
  if(local_event)
  {
    DALSYS_EventCtrl(local_event, DALSYS_EVENT_CTRL_TRIGGER);
  }
  else 
  {
    local_isr(local_isr_param);
  }

  (void)DALSYS_SyncEnter(device->gpioint_synchronization);

  /*
   * Clear the gpio interrupt if it is not owned by the apps processor.
   * During power collapse, the apps interrupt will be preserved to trigger
   * an apps interrupt once gpios are switched back to the apps processor.
   * The apps GPIO monitoring ISR (GPIOIntr_monitor_apps_isr) has disabled
   * the interrupt at this point. 
   * Clear the interrupt after the ISR is called if it's level triggered. 
   */
  if( !bIsEdgeTriggered )
  {
    HAL_gpioint_Clear(gpio);
  }

} /* END GPIOIntr_run_isr */

DALResult
GPIOIntrSummaryInternal_Init(GPIOIntDevCtxt *device, DalDeviceHandle* pInterruptHandle)
{
  DALResult eResult;
  uint32 gpio;
  uint32 req_size;
  uint32 gpioint_bitmask;  
  uint32 gpioint_glbctxt_idx;
  GPIOIntGlbCtxtType *pgpioint_glb_ctxt;    

  intr_handle = pInterruptHandle;

  eResult = DalInterruptController_RegisterISR(device->intr_handle, 
                   device->gpioint_cntrl->summary_intr_id,
                   (DALISR)GPIOIntr_Isr, (DALISRCtx)device, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER);


  if ( eResult != DAL_SUCCESS )
  {
    return DAL_ERROR;
  }

  /* Initialize QDI server */
  GPIOIntQDI_Init();

  device->gpioint_cntrl->gpioint_glb_ctxt = NULL;
  eResult = DALGLBCTXT_FindCtxt("GPIOINTLPASS", (void **)&device->gpioint_cntrl->gpioint_glb_ctxt);

  /*
   * If GPIOInt DAL global context does not exist, we need to allocate it.
   * If it already exists i.e. after SSR, there is no need to allocate. 
   */
  if(device->gpioint_cntrl->gpioint_glb_ctxt == NULL)
  {
   /*
    * Allocate the DAL global context.                                                 
    * Each bit in gpioint_reg_state[] represent a GPIO interrupt.
    * req_size is number bytes required to save the registration status of all the gpios.
    * req_size = ((gpio_number / 32) + 1) Words.
    * req_size = (((gpio_number / 32) + 1) * 4) Bytes.
    */
    req_size = (((device->gpioint_cntrl->gpio_number / 32) + 1) * 4);

    eResult     = DALGLBCTXT_AllocCtxt("GPIOINTLPASS",sizeof(GPIOIntGlbCtxtType) + req_size,
                      DALGLBCTXT_LOCK_TYPE_SPINLOCK, (void **)&device->gpioint_cntrl->gpioint_glb_ctxt);

    if (eResult != DAL_SUCCESS)
    {
      DALSYS_LogEvent(device->DevId, DALSYS_LOGEVENT_FATAL_ERROR, 
                    "GPIOIntrSummaryInternal_Init: not able to allocate DAL global context memory"); 

      return DAL_ERROR;
    }

    /*
     * Clear the DAL global context excluding its Header.
     */	 
    memset((void *)device->gpioint_cntrl->gpioint_glb_ctxt->gpioint_reg_state, 0x0, req_size);
  }
  else
  { 
    /*  Get the GPIOInt registration status from DAL global context,
     *  If an interrupt is enabled and target processor is current processor,
     *  then disable it and clear the pending status.   
     */ 
    pgpioint_glb_ctxt   = device->gpioint_cntrl->gpioint_glb_ctxt;	

    for(gpio = 0; gpio < device->gpioint_cntrl->gpio_number; gpio++)
    {
      /* 
       * 32 is the size of data type of gpioint_reg_state[] in bits.
       * Each bit in gpioint_reg_state[] represents a GPIO interrupt.
       * We need ((gpio_number / 32) + 1) words to store the registration 
       * status of gpio_number GPIOs.
       */	
      gpioint_glbctxt_idx = gpio / 32;
      gpioint_bitmask     = 0x1 << (gpio % 32);

      if(pgpioint_glb_ctxt->gpioint_reg_state[gpioint_glbctxt_idx] & gpioint_bitmask)
      {
        pgpioint_glb_ctxt->gpioint_reg_state[gpioint_glbctxt_idx] &= ~(gpioint_bitmask);

        if(HAL_gpioint_IsEnabled(gpio))
        {
          HAL_gpioint_Clear(gpio);
          HAL_gpioint_Disable(gpio);
        }
      }
    }
  }

  return eResult;
}


DALResult
GPIOIntrSummaryInternal_UnConfigureGPIOInterrupt( GPIOIntDevCtxt * device, uint32 gpio, uint8 uimage )
{
  uint32 gpioint_bitmask;  
  uint32 gpioint_glbctxt_idx;
  GPIOIntGlbCtxtType *pgpioint_glb_ctxt;    

  /* 
   * Clear the registration status of summary interrupt in DAL global context.
   */   
  if(device->gpioint_cntrl->state[gpio].intr_type == HAL_GPIOINT_SUMMARY)
  { 
   /* 
    * 32 is the size of data type of gpioint_reg_state[] in bits.
    * Each bit in gpioint_reg_state[] represents a GPIO interrupt.
    * We need ((gpio_number / 32) + 1) words to store the registration 
    * status of gpio_number GPIOs.
    */
    gpioint_glbctxt_idx = gpio / 32;	
    gpioint_bitmask     = 0x1 << (gpio % 32);			
    pgpioint_glb_ctxt   = device->gpioint_cntrl->gpioint_glb_ctxt;	 

    pgpioint_glb_ctxt->gpioint_reg_state[gpioint_glbctxt_idx] &= ~(gpioint_bitmask);
  }

  return DAL_SUCCESS;
}

DALResult
GPIOIntrSummaryInternal_ConfigureGPIOInterrupt( GPIOIntDevCtxt * device, uint32 gpio )
{
  uint32 gpioint_bitmask;  
  uint32 gpioint_glbctxt_idx;
  GPIOIntGlbCtxtType *pgpioint_glb_ctxt;    

  /* 
   * Update the registration status of summary interrupt in DAL global context.
   */  
  if(device->gpioint_cntrl->state[gpio].intr_type == HAL_GPIOINT_SUMMARY)
  { 
   /* 
    * 32 is the size of data type of gpioint_reg_state[] in bits.
    * Each bit in gpioint_reg_state[] represents a GPIO interrupt.
    * We need ((gpio_number / 32) + 1) words to store the registration 
    * status of gpio_number GPIOs.
    */
    gpioint_glbctxt_idx = gpio / 32;	
    gpioint_bitmask     = 0x1 << (gpio % 32);			
    pgpioint_glb_ctxt   = device->gpioint_cntrl->gpioint_glb_ctxt;	 

    pgpioint_glb_ctxt->gpioint_reg_state[gpioint_glbctxt_idx] |= gpioint_bitmask;	
  }

  return DAL_SUCCESS;
}

