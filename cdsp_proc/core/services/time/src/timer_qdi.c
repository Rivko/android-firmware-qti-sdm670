/*=============================================================================

                 timer_qdi.c

GENERAL DESCRIPTION
   Implements QDI layer for ATS Timers

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2010 - 2017
   by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.
 
$Header: //components/rel/core.qdsp6/2.1.c4/services/time/src/timer_qdi.c#1 $ 
$DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/28/17   abh     Added changes to use queue instead of pipes.
10/19/15   abh     Added debug code changes for better logging.
04/24/12   abalanag   File created.

=============================================================================*/


/*****************************************************************************/
/*                           INCLUDE FILES                                   */
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stringl/stringl.h>
#include "qurt.h"
#include "qurt_qdi.h"
#include "qurt_qdi_driver.h"
#include "qurt_pimutex.h"
#include "timer.h"
#include "timer_v.h"
#include "timer_qdi_v.h"
#include "msg.h"
#include "err.h"

/*===========================================================================
Data Structures
===========================================================================*/

#define TIMER_CLIENT_QUEUE_THRESHOLD     100
#define TIMER_MAX_CLIENTS          8
static timer_qdi_opener *timer_clients[TIMER_MAX_CLIENTS];
uint32 timer_clients_cntr = 0;

qurt_mutex_t timer_qdi_mutex;
/*MACRO for locking/unlocking the qmutex*/
#define TIMER_QDI_LOCK()  \
   qurt_pimutex_lock(&timer_qdi_mutex); \
  
#define TIMER_QDI_FREE()  \
   qurt_pimutex_unlock(&timer_qdi_mutex); \

/* extern variables */
/* Pointer to the mutex object for protecting critical-section of timer 
   code */
extern qurt_mutex_t timer_qemu_mutex;
extern void timer_process_active_timers(void);

/*MACRO for locking/unlocking the qmutex*/
#define ATS_INTLOCK()  \
   qurt_pimutex_lock(&timer_qemu_mutex); \
  
#define ATS_INTFREE()  \
   qurt_pimutex_unlock(&timer_qemu_mutex); \

#define ATS_INTLOCK_SAV(X) \
   qurt_pimutex_lock(&timer_qemu_mutex); \
  
#define ATS_INTFREE_SAV(X)   \
   qurt_pimutex_unlock(&timer_qemu_mutex); \

#define TIMER_TRACE_MUTEX_LOCK()  \
   qurt_pimutex_lock(&timer_trace_mutex); \
  
#define TIMER_TRACE_MUTEX_UNLOCK()  \
   qurt_pimutex_unlock(&timer_trace_mutex); \
   
/* Assumes two incoming variables are expected to be uint32 and
combines them into 64bit */
#define COMBINE64(a,b) ((((unsigned long long)((unsigned int)(a)))<<32)\
                       |(((unsigned long long)((unsigned int)(b)))<< 0))

/*===========================================================================
Declarations
===========================================================================*/
static int timer_qdi_invoke(int client_handle,
                          qurt_qdi_obj_t *pobj,
                          int method,
                          qurt_qdi_arg_t a1,
                          qurt_qdi_arg_t a2,
                          qurt_qdi_arg_t a3,
                          qurt_qdi_arg_t a4,
                          qurt_qdi_arg_t a5,
                          qurt_qdi_arg_t a6,
                          qurt_qdi_arg_t a7,
                          qurt_qdi_arg_t a8,
                          qurt_qdi_arg_t a9);
                          
#ifdef TIMER_DELAY_PROFILE
/*****************************************************************************/
/*                      FUNCTION DEFINITIONS                                 */
/*****************************************************************************/
uint32 qurt_timer_test_get_cpu_freq
(
  void
)
{
  uint32 cpu_freq = -1;
  if( NULL != timer_test_CpuQueryHandle )
  {
          /* Get the current CPU frequency. */
    if( NPA_QUERY_SUCCESS == npa_query( timer_test_CpuQueryHandle,
                                                 NPA_QUERY_CURRENT_STATE,
                                                 &qres ) );
    cpu_freq = qres.data.value;
  }
  else /*try to initialize the handle now*/
  {
    timer_test_CpuQueryHandle = npa_create_query_handle("/clk/cpu");
  }

  return cpu_freq;
}
#endif /* TIMER_DELAY_PROFILE */

/*===========================================================================
Queue Initialization Apis
===========================================================================*/
int timer_qdi_create_queue(timer_qdi_opener *clntobj)
{
   /* Initialize per PD signal object to be used by the queue */
   (void) qurt_anysignal_init(&clntobj->timer_qdi_sig);

   /* Initialize the per PD queue */
   if (NULL != q_init(&clntobj->timer_qdi_q))
   	return QURT_EOK;
   else
   	return QURT_EFAILED;

} /* timer_qdi_create_queue */

/*===========================================================================
Function Definitions
===========================================================================*/

/*=============================================================================

FUNCTION TIMER_QDI_GET_CB_INFO

DESCRIPTION
 Client Worker thread calls into this function and waits for callback data from queue

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  This is a Blocking call

=============================================================================*/
int timer_qdi_get_cb_info(int client_handle, timer_qdi_opener *obj, timer_cb_info *cb)
{
   timer_qdi_queue_type *q_data = NULL;
   unsigned int sigs = 0;
   int result;

/* GOTO MARKER FOR GETTING ITEM FROM THE QUEUE */
GET_QUEUE_DATA:
   
   /* Get the data from the queue */
   if( NULL != (q_data = q_get( &obj->timer_qdi_q)) )
   	{
      /* copy the data back to User Process */
      result = qurt_qdi_copy_to_user(client_handle, cb, &q_data->cb_info, sizeof(timer_cb_info));
      
      /* free the de-queued object */
      free(q_data);

      if(result >= 0)
        {
        return TIMER_ERR_NONE;
        }
      else
        {
      	return TIMER_ERR_QDI_CPY_FAIL;
        }
   	}

   /* Queue is empty, wait on Enqueue signal */
   sigs = qurt_anysignal_wait (&obj->timer_qdi_sig, TIMER_REMOTE_EXPIRY_ENQUEUED);
   if (sigs & TIMER_REMOTE_EXPIRY_ENQUEUED) 
   {
	 /* Clear the timer expiry signal */
	 qurt_anysignal_clear(&obj->timer_qdi_sig, TIMER_REMOTE_EXPIRY_ENQUEUED);
	 goto GET_QUEUE_DATA;
   }
   else
   {
     return TIMER_ERR_CANCEL_WAIT;
   }
   
} /* timer_qdi_get_cb_info */

/*=============================================================================

FUNCTION TIMER_QDI_REMOTE_HANDLER

DESCRIPTION
  Timer Expire function to push timer cb details into corresponding process queue.
 
 
DEPENDENCIES
  None

RETURN VALUE


SIDE EFFECTS
  None

=============================================================================*/
void timer_qdi_remote_handler(uint32 process_idx, timer_ptr_type timer, timer_notify_type cb_type,
                              time_osal_notify_obj_ptr sigs_func_addr, time_osal_notify_data sigs_mask_data,
                              time_timetick_type run_time_ms)
{
   timer_qdi_queue_type *q_data = NULL;
   uint32    q_item_count = 0;

   if(process_idx >= TIMER_MAX_CLIENTS)
      ERR_FATAL("ProcessIdx %d >= %d func 0x%x", process_idx, TIMER_MAX_CLIENTS, sigs_func_addr);
   
   TIMER_QDI_LOCK();
   if(timer_clients[process_idx] == NULL)
   {
      /* Process got cleared or deleted, ignoring the timers related to that process */
	  TIMER_QDI_FREE();
      return;
   }
   
   /* Check how many items are queued till now */
   q_item_count = q_cnt(&timer_clients[process_idx]->timer_qdi_q);

   if(q_item_count >= TIMER_CLIENT_QUEUE_THRESHOLD)
   	{
   	/* Threshold reached, release the timer qdi mutex, throw an error & return */
	TIMER_QDI_FREE();
	MSG_ERROR("Queue thold reached, Timer=%x;sig_func=%x;data=%x",
				timer,
				(uint32)sigs_func_addr,
				(uint32)sigs_mask_data);
	return;
   	}

   /* Allocate memory for buffer to enque */
   q_data = calloc(1,sizeof(timer_qdi_queue_type));
   if (q_data == NULL)
   	{
		TIMER_QDI_FREE();
   		ERR_FATAL("Memory allocation failed for Timer user PD queue data",0,0,0);
		return;	
   	}   

   q_data->cb_info.timer   = timer;
   q_data->cb_info.cb_type = cb_type;
   q_data->cb_info.sig_func_addr = (uint32)sigs_func_addr;
   q_data->cb_info.mask_data1 = (uint32)sigs_mask_data;
   q_data->cb_info.data2 = (uint32)run_time_ms;	

   /* Enque the allocated queue object to per PD queue oject */
   (void) q_link( &q_data, &q_data->link );
   (void) q_put( &timer_clients[process_idx]->timer_qdi_q, &q_data->link );

   /* Set the signal to the per PD QDI signal object to notify of the enqueud item */
   (void) qurt_anysignal_set(&timer_clients[process_idx]->timer_qdi_sig, TIMER_REMOTE_EXPIRY_ENQUEUED);

   TIMER_QDI_FREE();

   return;
} /* timer_qdi_remote_handler */

/*=============================================================================

                   TIMER API QDI Functions

=============================================================================*/

static inline timer_error_type timer_qdi_group_set_deferrable(int client_handle, timer_group_ptr pgroup, boolean condition,
                                                                  unsigned int process_idx)
{
   timer_group_type group;
   int              result;
   timer_error_type status;
   
   ATS_INTLOCK();
   
   result = qurt_qdi_copy_from_user(client_handle, &group, pgroup, sizeof(timer_group_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }
   
   status = timer_drv_group_set_deferrable(&group, pgroup, condition, process_idx);
   
   result = qurt_qdi_copy_to_user(client_handle, pgroup, &group, sizeof(timer_group_type));
   
   ATS_INTFREE();
   
   if(result)
      return TE_INVALID_PARAMETERS;
   return status;
} /* timer_qdi_group_set_deferrable */

static inline timer_error_type timer_qdi_group_enable(int client_handle, timer_group_ptr pgroup)
{
   timer_group_type group;
   int              result;
   timer_error_type status;
   
   ATS_INTLOCK();
   
   result = qurt_qdi_copy_from_user(client_handle, &group, pgroup, sizeof(timer_group_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }
   
   status = timer_drv_group_enable(&group);
   
   result = qurt_qdi_copy_to_user(client_handle, pgroup, &group, sizeof(timer_group_type));
   
   ATS_INTFREE();
   
   if(result)
      return TE_INVALID_PARAMETERS;
   return status;
} /* timer_qdi_group_enable */

static inline timer_error_type timer_qdi_group_disable(int client_handle, timer_group_ptr pgroup)
{
   timer_group_type group;
   int              result;
   timer_error_type status;
   
   ATS_INTLOCK();
   
   result = qurt_qdi_copy_from_user(client_handle, &group, pgroup, sizeof(timer_group_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }
   
   status = timer_drv_group_disable(&group);
   
   result = qurt_qdi_copy_to_user(client_handle, pgroup, &group, sizeof(timer_group_type));
   
   ATS_INTFREE();
   
   if(result)
      return TE_INVALID_PARAMETERS;
   return status;
} /* timer_qdi_group_disable */

static __attribute__((noinline)) timer_error_type timer_qdi_get_first_non_deferrable_timer_expiry(
                                 int                      client_handle, 
								 time_timetick_type       *ptick,
								 timer_non_defer_data_ptr timer_data_ptr)
{
   time_timetick_type expiry_time;
   int                result;
   timer_non_defer_data temp;
   
   expiry_time = timer_drv_get_first_non_deferrable_timer_expiry(&temp);
   result = qurt_qdi_copy_to_user(client_handle, ptick, &expiry_time, sizeof(time_timetick_type));

   if(result)
      return TE_INVALID_PARAMETERS;
  
   if(timer_data_ptr != NULL)
    {
        result = qurt_qdi_copy_to_user(client_handle, timer_data_ptr, &temp, sizeof(timer_non_defer_data));

        if(result)
           return TE_INVALID_PARAMETERS;
	}
	
   return TE_SUCCESS;
} /* timer_qdi_get_first_non_deferrable_timer_expiry */

static inline timer_error_type timer_qdi_defer_match_interrupt_64(int client_handle, time_timetick_type *ptick)
{
   time_timetick_type expiry_time;
   int                result;
   
   expiry_time = timer_drv_defer_match_interrupt_64();
   
   result = qurt_qdi_copy_to_user(client_handle, ptick, &expiry_time, sizeof(time_timetick_type));
   if(result)
      return TE_INVALID_PARAMETERS;
   
   return TE_SUCCESS;   
} /* timer_qdi_defer_match_interrupt */

static inline void timer_qdi_undefer_match_interrupt(void)
{
   timer_drv_undefer_match_interrupt();
} /* timer_qdi_undefer_match_interrupt */

static inline timer_error_type timer_qdi_def_osal(int client_handle, timer_ptr_type ptimer, timer_group_ptr pgroup, 
                               timer_notify_type cb_type, time_osal_notify_obj_ptr sigs_func_addr,
                               time_osal_notify_data sigs_mask_data, unsigned int process_idx)
{
   int result;
   timer_error_type status;
   timer_type timer;
   timer_group_type group;
   
   ATS_INTLOCK();
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(timer_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }
   
   result = qurt_qdi_copy_from_user(client_handle, &group, pgroup, sizeof(timer_group_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }
         
   status = timer_drv_def_osal(&timer, ptimer, &group, pgroup, cb_type, sigs_func_addr, sigs_mask_data, process_idx);
   
   result = qurt_qdi_copy_to_user(client_handle, ptimer, &timer, sizeof(timer_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }
      
   result = qurt_qdi_copy_to_user(client_handle, pgroup, &group, sizeof(timer_group_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }
   
   ATS_INTFREE();
   return status;
} /* timer_qdi_def_osal */

static inline timer_error_type timer_qdi_def2(int client_handle, timer_ptr_type ptimer,
                                              timer_group_ptr pgroup, unsigned int process_idx)
{
   int result;
   timer_error_type status;
   timer_type timer;
   timer_group_type group;
   
   ATS_INTLOCK();
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(timer_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }
      
   result = qurt_qdi_copy_from_user(client_handle, &group, pgroup, sizeof(timer_group_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }
      
   status = timer_drv_def2(&timer, ptimer, &group, pgroup, process_idx);
   
   result = qurt_qdi_copy_to_user(client_handle, ptimer, &timer, sizeof(timer_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }
      
   result = qurt_qdi_copy_to_user(client_handle, pgroup, &group, sizeof(timer_group_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }
   
   ATS_INTFREE();
   return status;
} /* timer_qdi_def2 */

static inline timer_error_type timer_qdi_reg(int client_handle, timer_ptr_type ptimer, 
                               timer_t2_cb_type func, timer_cb_data_type data,
                               timetick_type time_ms, timetick_type reload_ms, unsigned int process_idx)
{
   int result;
   timer_error_type status;
   timer_type timer;
   
   ATS_INTLOCK();
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(timer_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }

   status = timer_drv_reg(&timer, ptimer, func, data, time_ms, reload_ms, process_idx);
   
   ATS_INTFREE();

   return status;
} /* timer_qdi_reg */

static inline timer_error_type timer_qdi_set_64(int client_handle, timer_ptr_type ptimer, time_timetick_type time, 
                                             time_timetick_type reload, timer_unit_type     unit, unsigned int process_idx)
{
   int result;
   timer_error_type status;
   timer_type timer;
   
   ATS_INTLOCK();
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(timer_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }

   status = timer_drv_set_64(&timer, ptimer, time, reload, unit, process_idx);
   
   ATS_INTFREE();

   return status;
} /* timer_qdi_set */

static inline timer_error_type timer_qdi_set_absolute(int client_handle, timer_ptr_type ptimer, time_timetick_type time, unsigned int process_idx)
{
   int result;
   timer_error_type status;
   timer_type timer;
   
   ATS_INTLOCK();
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(timer_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }

   status = timer_drv_set_absolute(&timer, ptimer, time, process_idx);
   
   ATS_INTFREE();
   
   return status;
} /* timer_qdi_set_absolute */

static inline timer_error_type timer_qdi_get_64(int client_handle, timer_ptr_type ptimer, timer_unit_type     unit,
                                             unsigned int process_idx, time_timetick_type *ptick)
{
   int result;
   timer_type timer;
   time_timetick_type tick;
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(timer_type));
   if(result)
      return TE_INVALID_PARAMETERS;
   
   /* Call timer_drv_get */
   tick = timer_drv_get_64(&timer, ptimer, unit, process_idx);
   
   /* Note: Get apis doesnt change any thing in client timer structure.. Therefore, copy_to_user for timer is not needed */
   
   result = qurt_qdi_copy_to_user(client_handle, ptick, &tick, sizeof(time_timetick_type));
   if(result)
      return TE_INVALID_PARAMETERS;
      
   return TE_SUCCESS;
} /* timer_qdi_get */

static inline boolean timer_qdi_is_active(int client_handle, timer_ptr_type ptimer, unsigned int process_idx)
{
   int result;
   boolean is_active = FALSE;
   timer_type timer;
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(timer_type));
   if(result)
      return FALSE;

   is_active = timer_drv_is_active(&timer, ptimer, process_idx);
   
   /* Note: Get apis doesnt change any thing in client timer structure.. Therefore, copy_to_user for timer is not needed */
   
   return is_active;
} /* timer_qdi_is_active */

static inline timer_error_type timer_qdi_expires_at_64(int client_handle, timer_ptr_type ptimer, 
                                                       unsigned int process_idx, time_timetick_type *ptick)
{
   int result;
   timer_type timer;
   time_timetick_type tick = 0;
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(timer_type));
   if(result)
      return TE_INVALID_PARAMETERS;
      
   tick = timer_drv_expires_at_64(&timer, ptimer, process_idx);
   
   result = qurt_qdi_copy_to_user(client_handle, ptick, &tick, sizeof(time_timetick_type));
   if(result)
      return TE_INVALID_PARAMETERS;
      
   return TE_SUCCESS;
} /* timer_qdi_expires_at */

static inline timer_error_type timer_qdi_get_start_64(int client_handle, timer_ptr_type ptimer, 
                                                      unsigned int process_idx, time_timetick_type *ptick)
{
   int result;
   timer_type timer;
   time_timetick_type tick = 0;
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(timer_type));
   if(result)
      return TE_INVALID_PARAMETERS;
      
   tick = timer_drv_get_start_64(&timer, ptimer, process_idx);
   
   result = qurt_qdi_copy_to_user(client_handle, ptick, &tick, sizeof(time_timetick_type));
   if(result)
      return TE_INVALID_PARAMETERS;
      
   return TE_SUCCESS;
} /* timer_qdi_get_start */

static inline timer_error_type timer_qdi_clr_64(int client_handle, timer_ptr_type ptimer, 
                                                timer_unit_type     unit, unsigned int process_idx, 
                                                time_timetick_type *ptick)
{
   int result;
   timer_type timer;
   timer_error_type status;
   time_timetick_type tick = 0;
   
   ATS_INTLOCK();
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(timer_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }
      
   status = timer_drv_clr_64(&timer, ptimer, unit, process_idx, &tick);
   
   ATS_INTFREE();
      
   if(ptick != NULL)
   {
      result = qurt_qdi_copy_to_user(client_handle, ptick, &tick, sizeof(time_timetick_type));
      if(result)
      {
         return TE_INVALID_PARAMETERS;
      }
   }
   
   return status;
} /* timer_qdi_clr */

static inline timer_error_type timer_qdi_undef(int client_handle, timer_ptr_type ptimer, unsigned int process_idx)
{
   int result;
   timer_type timer;
   timer_error_type status;
   
   ATS_INTLOCK();
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(timer_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }
      
   status = timer_drv_undef(&timer, ptimer, process_idx);
   
   result = qurt_qdi_copy_to_user(client_handle, ptimer, &timer, sizeof(timer_type));
   ATS_INTFREE();
   
   if(result)
      return TE_INVALID_PARAMETERS;
   return status;
} /* timer_qdi_undef */

static inline timer_error_type timer_qdi_pause(int client_handle, timer_ptr_type ptimer, unsigned int process_idx)
{
   int result;
   timer_type timer;
   timer_error_type status;
   
   ATS_INTLOCK();
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(timer_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }
      
   status = timer_drv_pause(&timer, ptimer, process_idx);
   
   ATS_INTFREE();

   return status;
} /* timer_qdi_pause */

static inline timer_error_type timer_qdi_resume(int client_handle, timer_ptr_type ptimer, unsigned int process_idx)
{
   int result;
   timer_type timer;
   timer_error_type status;
   
   ATS_INTLOCK();
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(timer_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }   
   status = timer_drv_resume(&timer, ptimer, process_idx);
   
   ATS_INTFREE();
   
   return status;
} /* timer_qdi_resume */

static inline void timer_qdi_clr_osal_thread_timers(time_osal_thread_id thread_id)
{
   timer_drv_clr_osal_thread_timers(thread_id);
} /* timer_qdi_clr_osal_thread_timers */

static inline timer_error_type timer_qdi_get_sclk_till_expiry_64(int client_handle, time_timetick_type *ptick)
{
   time_timetick_type expiry_time = 0;
   int                result;
   
   expiry_time = timer_drv_get_sclk_till_expiry_64();
   result = qurt_qdi_copy_to_user(client_handle, ptick, &expiry_time, sizeof(time_timetick_type));
   if(result)
      return TE_INVALID_PARAMETERS;
      
   return TE_SUCCESS;
} /* timer_qdi_get_sclk_till_expiry */

static inline timer_error_type timer_qdi_set_wakeup_64(time_timetick_type tick)
{
   return timer_drv_set_wakeup_64(tick);
} /* timer_qdi_set_wakeup */

static inline timer_error_type timer_qdi_register_for_non_defer_expiry_change(
	                       timer_notify_type		notify_type,
	                       time_osal_notify_obj_ptr	notify_obj,
	                       time_osal_notify_data 	notify_mask,
	                       uint32 					process_idx)
{
   return timer_drv_register_for_non_defer_expiry_change(notify_type,notify_obj,notify_mask,process_idx);
} /* timer_qdi_register_for_non_defer_expiry_change */


static inline void timer_qdi_restart(void)
{
   timer_drv_restart();
} /* timer_qdi_restart */


/*=============================================================================

FUNCTION TIMER_QDI_RELEASE

DESCRIPTION
 Releases the Qdi object that corresponding to a process that got destroyed

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  Timers related to this process will be ignored from here

=============================================================================*/
void timer_qdi_release(qurt_qdi_obj_t *obj)
{
   timer_qdi_opener *clntobj = (timer_qdi_opener *)obj;
   uint32 process_idx = clntobj->process_idx;
   timer_qdi_queue_type *temp_q_item = NULL;
   
   /* Note: Remove INTLOCK and use LOCK */
   TIMER_QDI_LOCK();
   /* Deallocate all the queue items */
   while(NULL != (temp_q_item = q_get( &clntobj->timer_qdi_q)) )
   	{
   	  /* Item is de-queued, deallocate it as well */
	  free(temp_q_item);
   	}

   /* Destroy the queue */
   q_destroy( &clntobj->timer_qdi_q );
   
   /* Free the object related to this process */
   free(timer_clients[process_idx]);
   timer_clients[process_idx] = NULL;

   /* Remove the respective PD's associated client timers */
   timer_drv_delete_process_timers(process_idx);

   /* Remove the respective PD's associated client groups */
   timer_drv_delete_process_groups(process_idx);
  
   timer_clients_cntr--;
   
   TIMER_QDI_FREE();
   return;
} /* timer_qdi_release */


/*=============================================================================

FUNCTION TIMER_QDI_OPEN

DESCRIPTION
 Creates timer driver object for the client and initializes required details

DEPENDENCIES
  None

RETURN VALUE
 QURT_EOK if the timer is defined and set correctly.

SIDE EFFECTS
  None

=============================================================================*/
int timer_qdi_open(int client_handle, timer_qdi_opener *me)
{
   timer_qdi_opener *clntobj;
   int index;
   int status;
   
   TIMER_QDI_LOCK();
   
   /* Get a free pd slot */
   for(index=0; index<TIMER_MAX_CLIENTS && timer_clients[index] != NULL; index++);

   if(index == TIMER_MAX_CLIENTS)
   {
      TIMER_QDI_FREE();
      ERR_FATAL("Max Processes %d reached", timer_clients_cntr, 0, 0);
      return -1;
   }

   clntobj = calloc(1,sizeof(timer_qdi_opener));
   if(NULL == clntobj)
   {
      TIMER_QDI_FREE();
      printf("malloc err for %d bytes", sizeof(timer_qdi_opener));
      return -1;
   }
   
   clntobj->qdiobj.invoke = timer_qdi_invoke;
   clntobj->qdiobj.refcnt = QDI_REFCNT_INIT;
   clntobj->qdiobj.release = timer_qdi_release;
   
   /* Create Pipe only for remote processes */
   if(client_handle != QDI_HANDLE_LOCAL_CLIENT)
   {      
      status = timer_qdi_create_queue(clntobj);
      if(status == QURT_EFAILED)
      {
         TIMER_QDI_FREE();
         free(clntobj);
         return -1;
      }
   }
   else
   {
      timer_drv_set_local_process_idx(index);
   }
   
   timer_clients[index] = clntobj;
   clntobj->process_idx = index;
   timer_clients_cntr++;
   
   TIMER_QDI_FREE();

   return qurt_qdi_handle_create_from_obj_t(client_handle, &clntobj->qdiobj);
} /* timer_qdi_open */

/*=============================================================================

FUNCTION TIMER_QDI_INVOKE

DESCRIPTION
 This function contains the switch which maps to all methods

DEPENDENCIES
  None

RETURN VALUE


SIDE EFFECTS
  None

=============================================================================*/
static int timer_qdi_invoke(int client_handle,
                          qurt_qdi_obj_t *pobj,
                          int method,
                          qurt_qdi_arg_t a1,
                          qurt_qdi_arg_t a2,
                          qurt_qdi_arg_t a3,
                          qurt_qdi_arg_t a4,
                          qurt_qdi_arg_t a5,
                          qurt_qdi_arg_t a6,
                          qurt_qdi_arg_t a7,
                          qurt_qdi_arg_t a8,
                          qurt_qdi_arg_t a9)
{
   timer_qdi_opener *clntobj = (timer_qdi_opener *)pobj;
   unsigned int      process_idx = clntobj->process_idx;
   
   switch(method)
   {
      case QDI_OPEN: 
         return timer_qdi_open(client_handle, clntobj);
      
      case TIMER_QDI_GROUP_SET_DEFERRABLE: 
         return timer_qdi_group_set_deferrable(client_handle, a1.ptr, a2.num, process_idx);
         
      case TIMER_QDI_GROUP_ENABLE: 
         return timer_qdi_group_enable(client_handle, a1.ptr);
         
      case TIMER_QDI_GROUP_DISABLE:  
         return timer_qdi_group_disable(client_handle, a1.ptr);

      case TIMER_QDI_GET_FIRST_NON_DEFERRABLE_TIMER_EXPIRY:  
         return timer_qdi_get_first_non_deferrable_timer_expiry(client_handle, a1.ptr, a2.ptr);		 
         
      case TIMER_QDI_DEFER_MATCH_INTERRUPT_64: 
         return timer_qdi_defer_match_interrupt_64(client_handle, a1.ptr); 
         
      case TIMER_QDI_UNDEFER_MATCH_INTERRUPT: 
         timer_qdi_undefer_match_interrupt(); 
         return TE_SUCCESS;
      
      case TIMER_QDI_DEF_OSAL:  
         return timer_qdi_def_osal(client_handle, a1.ptr, a2.ptr, a3.num, a4.ptr, a5.num, process_idx);
         
      case TIMER_QDI_DEF2: 
         return timer_qdi_def2(client_handle, a1.ptr, a2.ptr, process_idx);
         
      case TIMER_QDI_REG:   
         return timer_qdi_reg(client_handle, a1.ptr, a2.ptr, a3.num, a4.num, a5.num, process_idx);
         
      case TIMER_QDI_SET_64:  
         return timer_qdi_set_64(client_handle, a1.ptr, (time_timetick_type) COMBINE64(a2.num, a3.num), (time_timetick_type) COMBINE64(a4.num, a5.num), a6.num, process_idx);
         
      case TIMER_QDI_SET_ABSOLUTE: 
         return timer_qdi_set_absolute(client_handle, a1.ptr, (time_timetick_type) COMBINE64(a2.num, a3.num), process_idx);
         
      case TIMER_QDI_GET_64: 
         return timer_qdi_get_64(client_handle, a1.ptr, a2.num, process_idx, a3.ptr); 
         
      case TIMER_QDI_IS_ACTIVE: 
         return timer_qdi_is_active(client_handle, a1.ptr, process_idx);
         
      case TIMER_QDI_EXPIRES_AT_64: 
         return timer_qdi_expires_at_64(client_handle, a1.ptr, process_idx, a2.ptr); 
         
      case TIMER_QDI_GET_START_64: 
         return timer_qdi_get_start_64(client_handle, a1.ptr, process_idx, a2.ptr); 
         
      case TIMER_QDI_CLR_64: 
         return timer_qdi_clr_64(client_handle, a1.ptr, a2.num, process_idx, a3.ptr); 
         
      case TIMER_QDI_UNDEF: 
         return timer_qdi_undef(client_handle, a1.ptr, process_idx);
         
      case TIMER_QDI_PAUSE: 
         return timer_qdi_pause(client_handle, a1.ptr, process_idx);
         
      case TIMER_QDI_RESUME: 
         return timer_qdi_resume(client_handle, a1.ptr, process_idx);
         
      case TIMER_QDI_CLR_OSAL_THREAD_TIMERS: 
         timer_qdi_clr_osal_thread_timers(a1.num); 
         return TE_SUCCESS;
         
      case TIMER_QDI_GET_SCLK_TILL_EXPIRY_64: 
         return timer_qdi_get_sclk_till_expiry_64(client_handle, a1.ptr); 
         
      case TIMER_QDI_SET_WAKEUP_64: 
         return timer_qdi_set_wakeup_64((time_timetick_type) COMBINE64(a1.num, a2.num));

      case TIMER_QDI_REGISTER_FOR_NON_DEFER_EXPIRY_CHANGE:
         return timer_qdi_register_for_non_defer_expiry_change(a1.num, a2.ptr, a3.num, process_idx);

      case TIMER_QDI_RESTART: 
         timer_qdi_restart(); 
         return TE_SUCCESS;

      case TIMER_QDI_GET_CB_INFO: return timer_qdi_get_cb_info(client_handle, clntobj, a1.ptr);

      default:
         return qurt_qdi_method_default(client_handle, pobj, method,
                                     a1, a2, a3, a4, a5, a6, a7, a8, a9);
   }
} /* timer_qdi_invoke */


/*=============================================================================

FUNCTION TIMER_QDI_INIT

DESCRIPTION
 Registers with QDI Framework for ATS Timers
 Registers callback handler that sends timer callback details to user process.
 
DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None

=============================================================================*/
void timer_qdi_init 
(
  void
) 
{
  timer_qdi_opener *p_opener;
  int i;
   
  /* initialize the mutex here */
  qurt_pimutex_init ( &timer_qdi_mutex );
  
  /* Initialize the timer processes */
  for(i=0; i<TIMER_MAX_CLIENTS; i++)
     timer_clients[i] = NULL;

   p_opener = (timer_qdi_opener *)calloc(1,sizeof(timer_qdi_opener));
   if(NULL == p_opener)
   {
      printf("malloc err for %d bytes", sizeof(timer_qdi_opener));
      return;
   }
   
   p_opener->qdiobj.invoke = timer_qdi_invoke;
   p_opener->qdiobj.refcnt = QDI_REFCNT_INIT;
   p_opener->qdiobj.release = timer_qdi_release;
   qurt_qdi_register_devname(TIMER_DRIVER_NAME, p_opener);
   
   /* Register Process Callback Handler with ATS Timers. 
      Assumption is timer_qdi_open() gets called first for local process */
   timer_drv_set_remote_process_cb_handler(timer_qdi_remote_handler);
} /* timer_qdi_init */