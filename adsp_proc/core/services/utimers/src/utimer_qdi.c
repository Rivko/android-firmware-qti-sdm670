/*=============================================================================

                 utimer_qdi.c

GENERAL DESCRIPTION
   Implements QDI layer for UTimers

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2010 - 2017
   by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.
 
$Header: //components/rel/core.qdsp6/2.1/services/utimers/src/utimer_qdi.c#1 $ 
$DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
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
#include "qurt_rmutex.h"
#include "utimer.h"
#include "utimer_v.h"
#include "utimer_qdi_v.h"
#include "err.h"

/*===========================================================================
Data Structures
===========================================================================*/

utimer_qdi_opener utimer_qdi_clients[UTIMER_MAX_CLIENTS];
uint32 utimer_qdi_clients_cntr = 0;

utimer_qdi_pipe_type utimer_qdi_pipes[UTIMER_MAX_PIPES];

qurt_mutex_t utimer_qdi_mutex;
/*MACRO for locking/unlocking the qmutex*/
#define UTIMER_QDI_LOCK()  \
   qurt_rmutex_lock(&utimer_qdi_mutex); \
  
#define UTIMER_QDI_FREE()  \
   qurt_rmutex_unlock(&utimer_qdi_mutex); \

/* extern variables */
/* Pointer to the mutex object for protecting critical-section of timer 
   code */
extern qurt_mutex_t utimer_qemu_mutex;
extern void utimer_process_active_timers(void);

/*MACRO for locking/unlocking the qmutex*/
#define UTIMER_LOCK()  \
   qurt_rmutex_lock(&utimer_qemu_mutex); \
  
#define UTIMER_FREE()  \
   qurt_rmutex_unlock(&utimer_qemu_mutex); \

/* Assumes two incoming variables are expected to be uint32 and
combines them into 64bit */
#define COMBINE64(a,b) ((((unsigned long long)((unsigned int)(a)))<<32)\
                       |(((unsigned long long)((unsigned int)(b)))<< 0))   
   
#ifdef UTIMER_DELAY_PROFILE
/*****************************************************************************/
/*                      FUNCTION DEFINITIONS                                 */
/*****************************************************************************/
uint32 qurt_utimer_test_get_cpu_freq
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
#endif /* UTIMER_DELAY_PROFILE */


/*===========================================================================
Function Definitions
===========================================================================*/

/*=============================================================================

FUNCTION UTIMER_QDI_GET_CB_INFO

DESCRIPTION
 Client Worker thread calls into this function and waits for callback data from pipe

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  This is a Blocking call

=============================================================================*/
int utimer_qdi_get_cb_info(int client_handle, utimer_qdi_opener *obj, utimer_cb_info *cb)
{
   qurt_pipe_data_t data[3];
   qurt_pipe_data_t *cb_ptr;
   int result;
   
   cb_ptr = (qurt_pipe_data_t *)cb;
   
   /* Read callback details from the pipe. 
      As timer_cb_info is 4words, reading pipe twice */
   if( qurt_pipe_receive_cancellable(&obj->pipe, &data[0]) == QURT_ECANCEL ||
       qurt_pipe_receive_cancellable(&obj->pipe, &data[1]) == QURT_ECANCEL ||
       qurt_pipe_receive_cancellable(&obj->pipe, &data[2]) == QURT_ECANCEL )
   {
      return UTIMER_ERR_CANCEL_WAIT;
   }
   
   result = qurt_qdi_copy_to_user(client_handle, cb_ptr, &data[0], sizeof(utimer_cb_info));
   if(result >= 0)
   {
      return UTIMER_ERR_NONE;
   }
   else
   {
      return UTIMER_ERR_QDI_CPY_FAIL;
   }
} /* utimer_qdi_get_cb_info */

/*=============================================================================

FUNCTION UTIMER_QDI_REMOTE_HANDLER

DESCRIPTION
  Timer Expire function to push timer cb details into corresponding process pipe.
 
 
DEPENDENCIES
  None

RETURN VALUE


SIDE EFFECTS
  None

=============================================================================*/
void utimer_qdi_remote_handler(uint32 process_idx, utimer_ptr_type timer, utimer_notify_type cb_type,
                              utimer_osal_notify_obj_ptr sigs_func_addr, utimer_osal_notify_data sigs_mask_data,
                              utimer_timetick_type run_time_ms)
{
   qurt_pipe_data_t *ptr = 0;
   utimer_cb_info   temp_cb;
   static uint32    sequence = 0; 
   
   if(process_idx >= UTIMER_MAX_CLIENTS)
      ERR_FATAL("ProcessIdx %d >= %d func 0x%x", process_idx, UTIMER_MAX_CLIENTS, sigs_func_addr);
   
   UTIMER_QDI_LOCK();
   if(utimer_qdi_clients[process_idx].opener_allocated == FALSE)
   {
      UTIMER_QDI_FREE();
      
      /* Process got cleared or deleted, ignoring the timers related to that process */
      return;
   }
   UTIMER_QDI_FREE();

   temp_cb.timer   = timer;
   temp_cb.seq     = sequence++;    /* Just dummy */
   temp_cb.cb_type = cb_type;
   temp_cb.sig_func_addr = (uint32)sigs_func_addr;
   temp_cb.mask_data1 = (uint32)sigs_mask_data;
   temp_cb.data2 = (uint32)run_time_ms;
   
   ptr = (qurt_pipe_data_t *)&temp_cb;

   /* Todo: Need to see if there is a way, where i can figure out pipe is not full */
   qurt_pipe_send((qurt_pipe_t *)&utimer_qdi_clients[process_idx].pipe, ptr[0]);
   /* As first data is successfully sent, data2 should be able to send */
   qurt_pipe_send((qurt_pipe_t *)&utimer_qdi_clients[process_idx].pipe, ptr[1]);
      /* As first data is successfully sent, data3 should be able to send */
   qurt_pipe_send((qurt_pipe_t *)&utimer_qdi_clients[process_idx].pipe, ptr[2]);
   
   return;
} /* utimer_qdi_remote_handler */

/*=============================================================================

                   UTIMER API QDI Functions

=============================================================================*/

static inline utimer_error_type utimer_qdi_get_first_non_deferrable_timer_expiry(
                                                 int                       client_handle,
												 utimer_timetick_type      *ptick,
												 utimer_non_defer_data_ptr utimer_data_ptr)
{
   utimer_timetick_type expiry_time;
   int                  result;
   utimer_non_defer_data temp;
   
   expiry_time = utimer_drv_get_first_non_deferrable_timer_expiry(&temp);
   result = qurt_qdi_copy_to_user(client_handle, ptick, &expiry_time, sizeof(utimer_timetick_type));

   if(result)
      return UTE_INVALID_PARAMETERS;
  
   if(utimer_data_ptr != NULL)
    {
	   
      result = qurt_qdi_copy_to_user(client_handle, utimer_data_ptr, &temp, sizeof(utimer_non_defer_data));

      if(result)
         return UTE_INVALID_PARAMETERS;
    }
   
   return UTE_SUCCESS;
} /* utimer_qdi_get_first_non_deferrable_timer_expiry_64 */

#ifdef FEATURE_UTIMER_DEFER_SUPPORT
static inline utimer_error_type utimer_qdi_defer_match_interrupt_64(int client_handle, utimer_timetick_type *ptick)
{
   utimer_timetick_type expiry_time;
   int                result;
   
   expiry_time = utimer_drv_defer_match_interrupt_64();
   
   result = qurt_qdi_copy_to_user(client_handle, ptick, &expiry_time, sizeof(utimer_timetick_type));
   if(result)
      return UTE_INVALID_PARAMETERS;
   
   return UTE_SUCCESS;   
} /* utimer_qdi_defer_match_interrupt_64 */

static inline void utimer_qdi_undefer_match_interrupt_64(void)
{
   utimer_drv_undefer_match_interrupt_64();
} /* utimer_qdi_undefer_match_interrupt_64 */


static inline utimer_error_type utimer_qdi_set_deferrable(int client_handle, utimer_ptr_type ptimer, boolean condition)
{
   utimer_type       timer;
   int               result;
   utimer_error_type status;
   
   UTIMER_LOCK();
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(utimer_ptr_type));
   if(result)
   {
      UTIMER_FREE();
      return UTE_INVALID_PARAMETERS;
   }
   
   status = utimer_drv_set_deferrable(&timer, ptimer, condition);
   
   UTIMER_FREE();
   
   return status;
} /* utimer_qdi_set_deferrable */
#endif /* #ifdef FEATURE_UTIMER_DEFER_SUPPORT */

static inline utimer_error_type utimer_qdi_def_osal(int client_handle, utimer_ptr_type ptimer, 
                               utimer_notify_type cb_type, utimer_osal_notify_obj_ptr sigs_func_addr,
                               utimer_osal_notify_data sigs_mask_data, unsigned int process_idx)
{
   int result;
   utimer_error_type status;
   utimer_type timer;
   //timer_group_type group;
   
   UTIMER_LOCK();
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(utimer_type));
   if(result)
   {
      UTIMER_FREE();
      return UTE_INVALID_PARAMETERS;
   }

   status = utimer_drv_def_osal(&timer, ptimer, cb_type, sigs_func_addr, sigs_mask_data, process_idx);

   result = qurt_qdi_copy_to_user(client_handle, ptimer, &timer, sizeof(utimer_type));
   if(result)
   {
      UTIMER_FREE();
      return UTE_INVALID_PARAMETERS;
   }
      
   UTIMER_FREE();
   return status;
} /* utimer_qdi_def_osal */

static inline utimer_error_type utimer_qdi_set_absolute(int client_handle, utimer_ptr_type ptimer, utimer_timetick_type time, 
                                             utimer_timetick_type reload, utimer_unit_type unit)
{
   int result;
   utimer_error_type status;
   utimer_type timer;
   
   UTIMER_LOCK();
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(utimer_type));
   if(result)
   {
      UTIMER_FREE();
      return UTE_INVALID_PARAMETERS;
   }

   status = utimer_drv_set_absolute(&timer, ptimer, time, reload, unit);
   
   result = qurt_qdi_copy_to_user(client_handle, ptimer, &timer, sizeof(utimer_type));
   
   UTIMER_FREE();
   
   if(result)
      return UTE_INVALID_PARAMETERS;
   return status;
} /* utimer_qdi_set_absolute */

static inline utimer_error_type utimer_qdi_get_64(int client_handle, utimer_ptr_type ptimer, utimer_unit_type     unit,
                                             utimer_timetick_type *ptick)
{
   int result;
   utimer_type timer;
   utimer_timetick_type tick;
   
   UTIMER_LOCK();
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(utimer_type));
   if(result)
   {
      UTIMER_FREE();
      return UTE_INVALID_PARAMETERS;
   }
   
   /* Call timer_drv_get */
   tick = utimer_drv_get_64(&timer, ptimer, unit);

   UTIMER_FREE();
   
   /* Note: Get apis doesnt change any thing in client timer structure.. Therefore, copy_to_user for timer is not needed */
   
   result = qurt_qdi_copy_to_user(client_handle, ptick, &tick, sizeof(utimer_timetick_type));
   if(result)
      return UTE_INVALID_PARAMETERS;

   return UTE_SUCCESS;
} /* utimer_qdi_get */

static inline utimer_error_type utimer_qdi_clr_64(int client_handle, utimer_ptr_type ptimer, 
                                                utimer_unit_type     unit, 
                                                utimer_timetick_type *ptick)
{
   int result;
   utimer_type timer;
   utimer_error_type status;
   utimer_timetick_type tick;
   
   UTIMER_LOCK();
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(utimer_type));
   if(result)
   {
      UTIMER_FREE();
      return UTE_INVALID_PARAMETERS;
   }
      
   status = utimer_drv_clr_64(&timer, ptimer, unit, &tick);
   
   result = qurt_qdi_copy_to_user(client_handle, ptimer, &timer, sizeof(utimer_type));
   
   UTIMER_FREE();
      
   if(result)
   {
      return UTE_INVALID_PARAMETERS;
   }

   if(ptick != NULL)
   {
      result = qurt_qdi_copy_to_user(client_handle, ptick, &tick, sizeof(utimer_timetick_type));
      if(result)
      {
         return UTE_INVALID_PARAMETERS;
      }
   }
   
   return status;
} /* utimer_qdi_clr_64 */

static inline utimer_error_type utimer_qdi_undef(int client_handle, utimer_ptr_type ptimer)
{
   int result;
   utimer_type timer;
   utimer_error_type status;
   
   UTIMER_LOCK();
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(utimer_type));
   if(result)
   {
      UTIMER_FREE();
      return UTE_INVALID_PARAMETERS;
   }
      
   status = utimer_drv_undef(&timer, ptimer);
   
   result = qurt_qdi_copy_to_user(client_handle, ptimer, &timer, sizeof(utimer_type));
   UTIMER_FREE();
   
   if(result)
      return UTE_INVALID_PARAMETERS;
   return status;
} /* utimer_qdi_undef */

/* Following functions are defined in utimer_qdi_ddr.c file */
extern int utimer_qdi_open(int client_handle, utimer_qdi_opener *me);
extern utimer_error_type utimer_qdi_register_for_non_defer_expiry_change(
	                       utimer_notify_type		    notify_type,
	                       utimer_osal_notify_obj_ptr	notify_obj,
	                       utimer_osal_notify_data 	    notify_mask,
	                       uint32 					    process_idx);
/*=============================================================================

FUNCTION UTIMER_QDI_INVOKE

DESCRIPTION
 This function contains the switch which maps to all methods

DEPENDENCIES
  None

RETURN VALUE


SIDE EFFECTS
  None

=============================================================================*/
int utimer_qdi_invoke(int client_handle,
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
   utimer_qdi_opener *clntobj = (utimer_qdi_opener *)pobj;
   unsigned int      process_idx = clntobj->process_idx;
   
   switch(method)
   {
      case QDI_OPEN: 
         return utimer_qdi_open(client_handle, clntobj);
      
      case UTIMER_QDI_GET_FIRST_NON_DEFERRABLE_TIMER_EXPIRY:  
         return utimer_qdi_get_first_non_deferrable_timer_expiry(client_handle, a1.ptr, a2.ptr); 
         
      case UTIMER_QDI_REGISTER_FOR_NON_DEFER_EXPIRY_CHANGE:
         return utimer_qdi_register_for_non_defer_expiry_change(a1.num, a2.ptr, a3.num, process_idx);
         
#ifdef FEATURE_UTIMER_DEFER_SUPPORT
      case UTIMER_QDI_DEFER_MATCH_INTERRUPT_64: 
         return utimer_qdi_defer_match_interrupt_64(client_handle, a1.ptr); 
         
      case UTIMER_QDI_UNDEFER_MATCH_INTERRUPT_64: 
         utimer_qdi_undefer_match_interrupt_64(); 
         return UTE_SUCCESS;
      
      case UTIMER_QDI_SET_DEFERRABLE: 
         return utimer_qdi_set_deferrable(client_handle, a1.ptr, a2.num);
#endif /* #ifdef FEATURE_UTIMER_DEFER_SUPPORT */

      case UTIMER_QDI_DEF_OSAL:  
         return utimer_qdi_def_osal(client_handle, a1.ptr, a2.num, a3.ptr, a4.num, process_idx);
		 
         
      case UTIMER_QDI_SET_ABSOLUTE:  
         return utimer_qdi_set_absolute(client_handle, a1.ptr, (utimer_timetick_type)COMBINE64(a2.num, a3.num), (utimer_timetick_type)COMBINE64(a4.num, a5.num), a6.num);
         
      case UTIMER_QDI_GET_64: 
         return utimer_qdi_get_64(client_handle, a1.ptr, a2.num, a3.ptr); 
         
      case UTIMER_QDI_CLR_64: 
         return utimer_qdi_clr_64(client_handle, a1.ptr, a2.num, a3.ptr); 
         
      case UTIMER_QDI_UNDEF: 
         return utimer_qdi_undef(client_handle, a1.ptr);
         
      case UTIMER_QDI_GET_CB_INFO: return utimer_qdi_get_cb_info(client_handle, clntobj, a1.ptr);

      case QDI_ISLAND_CHECK_ALLOC:
         return 1;

      default:
         return qurt_qdi_method_default(client_handle, pobj, method,
                                     a1, a2, a3, a4, a5, a6, a7, a8, a9);
   }
} /* utimer_qdi_invoke */
