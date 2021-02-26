/*========================================================================

*//** @file qurt_elite_interrupt.cpp
This file contains utilities for registering with  non-fast interrupts.
and de-registering with non-fast interrupts

Copyright (c) 2010-2016 QUALCOMM Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/qurt_elite/src/qurt_elite_interrupt.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
11/04/10   psr     Created file.

========================================================================== */

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite_interrupt.h"
#include "elite_thrd_prio.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
 ** Function prototypes
 ** ----------------------------------------------------------------------- */
//Main work loop for IST thread

 static ADSPResult qurt_elite_interrupt_workloop( void *arg_ptr);


/* =======================================================================
**                          Function Definitions
** ======================================================================= */

//ToDo : Right now no unit test is added. This code is checked by changing files in Afeosal.
// Planning to add cosim which triggers interrupts and service the interrupt




// Register with the interrupt

ADSPResult qurt_elite_interrupt_register( qurt_elite_interrupt_ist_t *ist_ptr, uint16_t intr_id,
                              void (*callback_ptr)(void *),void *arg_ptr,char_t *thread_name, uint32_t stack_size)
{
   uint32_t result;

   if (NULL == ist_ptr)
   {
      return ADSP_EBADPARAM;
   }

   //initialise the structure members
   ist_ptr->thread_id = 0;
   ist_ptr->ist_state = FALSE;
   ist_ptr->intr_id = intr_id;
   ist_ptr->ist_callback = callback_ptr;
   ist_ptr->arg_ptr  = arg_ptr;

   //copy the ist thread name
   qurt_elite_strl_cpy(ist_ptr->ist_thread_name, thread_name, sizeof(ist_ptr->ist_thread_name));

   // initialise the semaphore with value 0
   qurt_sem_init_val(&(ist_ptr->semaphore), 0);

   // Launch the IST thread. This launching thread will call callback function once the
   // interrupt is triggered.

   if(ADSP_FAILED(
         result = qurt_elite_thread_launch(&(ist_ptr->thread_id), ist_ptr->ist_thread_name,
                                           NULL, stack_size, elite_get_thrd_prio(ELITETHREAD_DEFAULT_IST_PRIO_ID),
                                           qurt_elite_interrupt_workloop, (void*)ist_ptr, QURT_ELITE_HEAP_DEFAULT)))

   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Failed to launch IST thread %lu!! \n",result);
      return result;
   }

   // This thread will wait here until the semaphore is up/incremented
   qurt_sem_down(&(ist_ptr->semaphore));

   //Destroy  the semaphore
   qurt_sem_destroy(&(ist_ptr->semaphore));

   //Check whether interupt is succesfully register or not

   if(ist_ptr->ist_state != TRUE)
   {
     return ADSP_EFAILED;
   }
   return ADSP_EOK;

}

// IST woorkloop

static ADSPResult qurt_elite_interrupt_workloop( void *arg_ptr)
{
   qurt_anysignal_t ist_signal;
   uint32_t channel_status,result;
   const uint32_t IST_MASK = 0x1;
   
   uint32_t signal_mask = SIG_INT_ABORT | IST_MASK ; //31 bit reserved for qurt
   qurt_elite_interrupt_ist_t *client_ist_ptr = (qurt_elite_interrupt_ist_t *)arg_ptr;

   // Initialise the signal
   qurt_anysignal_init(&ist_signal);

   /*Registering the interrupt with qurt API. Return the follwing values
     EOK Interrupt successfully registered.
     EINT(101) Invalid interrupt number:
     ESIG(102) Invalid signal bitmask
     EVAL(1) Interrupt already registered  */

   if (ADSP_FAILED(result = qurt_interrupt_register(client_ist_ptr->intr_id, &ist_signal, IST_MASK)))
   {
     MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to register interrupt with error = %lu!!\n", result);
     // If it is failed destroy  the signal and wakeup client by semaphore
     qurt_sem_up(&(client_ist_ptr->semaphore));
     qurt_anysignal_destroy(&ist_signal);
     
     return result;
   }


   // If interrupt is succesfully registerd set this state to TRUE and send this to client
   client_ist_ptr->ist_state = TRUE;

   // wake up client indicating succesful registration
   qurt_sem_up(&(client_ist_ptr->semaphore));

   while(1)
   {
     /* block to receive interrupt */

     channel_status =   qurt_anysignal_wait(&ist_signal, signal_mask);

     /*SIG_INT_ABORT signal comes from QURT kernel if interrupt is deregistered*/

     if (channel_status&SIG_INT_ABORT)
     {
      // Destroy signal and stop the thread
       qurt_anysignal_destroy(&ist_signal);
       return ADSP_EOK;
     }
     /* Call the call back function */
     //time1 = qurt_elite_profile_get_pcycles();
     client_ist_ptr->ist_callback(client_ist_ptr->arg_ptr);

     /* clear the ist signal */
     qurt_anysignal_clear(&ist_signal, channel_status);
     /* To re enable the interrupt, need to acknowledge the interrupt */
     qurt_interrupt_acknowledge(client_ist_ptr->intr_id);
   }
}


// Unregister Interrupt

ADSPResult qurt_elite_interrupt_unregister(qurt_elite_interrupt_ist_t *ist_ptr)
{
   int result;
// By unregistering the interrupt it will send the signal SIG_INT_ABORT
//to ist thread which will make  ist to come out of the loop
   if (ADSP_FAILED(result =qurt_interrupt_deregister(ist_ptr->intr_id)))
   {
     MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid interrupt number or not registered = %d!!\n", result);
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Before thread joining %s!! with thread id %d\n", ist_ptr->ist_thread_name,ist_ptr->thread_id);

   qurt_elite_thread_join(ist_ptr->thread_id,&result);

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " Thread joined succesfully %s!! with thread id %d\n", ist_ptr->ist_thread_name,ist_ptr->thread_id);
   return ADSP_EOK;

}
