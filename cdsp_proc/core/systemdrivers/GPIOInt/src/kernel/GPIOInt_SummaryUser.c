/*==============================================================================

FILE:      GPIOInt_SummaryUser.c

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

$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/GPIOInt/src/kernel/GPIOInt_SummaryUser.c#1 $

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
#include "GPIOIntQDI.h"
#include "DDIHWIO.h"
#include "qurt.h"
#include "stringl.h"

/* 
 * For each interrupt handler thread a Default IST stack size is declared.
 */
#define GPIOINT_IST_STACK_SIZE  2048
#define GPIOIST_TASKNAME_SIZE   16
#define MEDIAN_GPIOINT_PRIORITY 4


typedef struct
{
  unsigned char                  pSummaryStack[GPIOINT_IST_STACK_SIZE];
  char                           aSummaryName[GPIOIST_TASKNAME_SIZE];
  qurt_thread_t                  nSummaryThreadID;
  qurt_thread_attr_t             ThreadAttr;
}
GPIOInt_SummaryUserData;

static DalDeviceHandle* intr_handle = NULL;
static GPIOInt_SummaryUserData GPIOInt_SummaryData;


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


/*=============================================================================

  FUNCTION      GPIOInt_ISTMain

  DESCRIPTION   This is the server thread for handling summary interrupts on
                User PD.

  PARAMETERS
  ISTParam      Unused.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.


==========================================================================*/
static void GPIOIntSummary_SummaryServer
(
  void * ISTParam
)
{
  GPIOIntDevCtxt *device = (GPIOIntDevCtxt *)ISTParam; 
  uint32 gpio = 0;
  int nQDIResult;


  while (1)
  {

    nQDIResult = qurt_qdi_handle_invoke(device->gpioint_qdi, GPIOINT_QDI_RECEIVE_SUMMARY_SIGNAL, qurt_process_get_id(), &gpio);

    if ( nQDIResult != GPIOINT_QDI_SUCCESS ||
         gpio >= MAX_NUMBER_OF_GPIOS )
    {
      continue;
    }

    /*
     * Log the GPIO that fired
     */
    GPIOIntr_LogEvent(device,gpio);
    
    /*
     * Get the client registered ISR and run it.
     */
    if(device->gpioint_cntrl->state[gpio].event)
    {
      
      DALSYS_EventCtrl(device->gpioint_cntrl->state[gpio].event, DALSYS_EVENT_CTRL_TRIGGER);
    }
    else 
    {
      /*
       * Call the registered client callback.
       */
      device->gpioint_cntrl->state[gpio].isr(device->gpioint_cntrl->state[gpio].isr_param);
    }
    
    /*
     * Clear signal and reactivate interrupt.
     */
     qurt_qdi_handle_invoke(device->gpioint_qdi, GPIOINT_QDI_ACK_SUMMARY_SIGNAL, qurt_process_get_id());
    
  }

} /* END GPIOInt_ISTMain */

static 
DALResult 
GPIOIntSummaryInternal_SpawnServer(GPIOIntDevCtxt* device)
{
  int nResult;

  /*
   * Set the IST name to GPIOIST<GpioNumber>
   */
  snprintf(&GPIOInt_SummaryData.aSummaryName[0], 
           (sizeof(unsigned char)*GPIOIST_TASKNAME_SIZE), 
           "GPIOINT_SRV");

  /*
   *  Initialize any thread attribute objects needed by the interrupt service thread.
   */
  qurt_thread_attr_init (&GPIOInt_SummaryData.ThreadAttr);

  /* 
   * To ensure that the entire stack (start to end) is 64-bit aligned
   * so the last odd bytes are dropped off.
   */
  qurt_thread_attr_set_stack_size(&GPIOInt_SummaryData.ThreadAttr, 
                                  GPIOINT_IST_STACK_SIZE);
  qurt_thread_attr_set_stack_addr (&GPIOInt_SummaryData.ThreadAttr, 
                                   (void *)&GPIOInt_SummaryData.pSummaryStack[0]);
  qurt_thread_attr_set_priority (&GPIOInt_SummaryData.ThreadAttr, 
                                (unsigned short)MEDIAN_GPIOINT_PRIORITY);
  qurt_thread_attr_set_name(&GPIOInt_SummaryData.ThreadAttr, 
                            &GPIOInt_SummaryData.aSummaryName[0]);

  nResult = qurt_thread_create((qurt_thread_t *)&GPIOInt_SummaryData.nSummaryThreadID, 
                               &GPIOInt_SummaryData.ThreadAttr,
                               GPIOIntSummary_SummaryServer,device);

  return nResult;
}


DALResult
GPIOIntrSummaryInternal_Init(GPIOIntDevCtxt *device, DalDeviceHandle* pInterruptHandle)
{
  intr_handle = pInterruptHandle;

  if ( device == NULL )
  {
    return DAL_ERROR;
  }

  /*
   * Get QDI Handle
   */
  device->gpioint_qdi = qurt_qdi_open(GPIOIntQdiName,qurt_process_get_id());
  if (device->gpioint_qdi < 0)
  {
    return DAL_ERROR;
  }

  /*
   * Initialize QDI for this PD
   */
  qurt_qdi_handle_invoke(device->gpioint_qdi, GPIOINT_QDI_REGISTER_SUMMARY_TASK, qurt_process_get_id());

  /*
   * Spawn summary server task
   */
  GPIOIntSummaryInternal_SpawnServer(device);

  return DAL_SUCCESS;
}

DALResult
GPIOIntrSummaryInternal_UnConfigureGPIOInterrupt( GPIOIntDevCtxt * device, uint32 gpio, uint8 uimage )
{
  return DAL_SUCCESS;
}

DALResult
GPIOIntrSummaryInternal_ConfigureGPIOInterrupt( GPIOIntDevCtxt * device, uint32 gpio )
{
  return DAL_SUCCESS;
}

