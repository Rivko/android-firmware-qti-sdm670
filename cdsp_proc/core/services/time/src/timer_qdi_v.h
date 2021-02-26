/*=============================================================================

                Timer_Qdi_v.h  --  Header File

GENERAL DESCRIPTION
      Timer QDI Layer Header File

EXTERNAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2009 - 2017
      by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.qdsp6/2.1.c4/services/time/src/timer_qdi_v.h#1 $ 
$DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/28/17   abh     Added changes to use queue instead of pipes.
06/20/13   ab      Added header file
=============================================================================*/
#ifndef TIMER_QDI_V_H
#define TIMER_QDI_V_H

#include "qurt_qdi.h"
#include "qurt_qdi_driver.h"
#include "queue.h"
#include "timer.h"

#define TIMER_ERR_NONE                       0x0
#define TIMER_ERR_QDI_CPY_FAIL               0x1
#define TIMER_ERR_CANCEL_WAIT                0x2

#define TIMER_DRIVER_NAME              "/dev/timers"

#define TIMER_QDI_GROUP_SET_DEFERRABLE       (0+QDI_PRIVATE)
#define TIMER_QDI_GROUP_ENABLE               (1+QDI_PRIVATE)
#define TIMER_QDI_GROUP_DISABLE              (2+QDI_PRIVATE)

#define TIMER_QDI_GET_TIME_TILL_FIRST_NON_DEFERRABLE_TIMER     (3+QDI_PRIVATE)
#define TIMER_QDI_GET_FIRST_NON_DEFERRABLE_TIMER_EXPIRY        (4+QDI_PRIVATE)

#define TIMER_QDI_DEFER_MATCH_INTERRUPT      (5+QDI_PRIVATE)
#define TIMER_QDI_DEFER_MATCH_INTERRUPT_64   (6+QDI_PRIVATE)
#define TIMER_QDI_UNDEFER_MATCH_INTERRUPT    (7+QDI_PRIVATE)

#define TIMER_QDI_DEF                        (8+QDI_PRIVATE)
#define TIMER_QDI_DEF2                       (9+QDI_PRIVATE)
#define TIMER_QDI_DEF_OSAL                   (10+QDI_PRIVATE)
#define TIMER_QDI_DEF_PRIORITY               (11+QDI_PRIVATE)
#define TIMER_QDI_DEF2_PRIORITY              (12+QDI_PRIVATE)
#define TIMER_QDI_DEF_OSAL_PRIORITY          (13+QDI_PRIVATE)
#define TIMER_QDI_REG                        (14+QDI_PRIVATE)
#define TIMER_QDI_SET                        (15+QDI_PRIVATE)
#define TIMER_QDI_SET_64                     (16+QDI_PRIVATE)
#define TIMER_QDI_SET_ABSOLUTE               (17+QDI_PRIVATE)
#define TIMER_QDI_GET                        (18+QDI_PRIVATE)
#define TIMER_QDI_GET_64                     (19+QDI_PRIVATE)
#define TIMER_QDI_IS_ACTIVE                  (20+QDI_PRIVATE)
#define TIMER_QDI_EXPIRES_AT_64              (21+QDI_PRIVATE)
#define TIMER_QDI_EXPIRES_AT                 (22+QDI_PRIVATE)
#define TIMER_QDI_GET_START                  (23+QDI_PRIVATE)
#define TIMER_QDI_GET_START_64               (24+QDI_PRIVATE)
#define TIMER_QDI_CLR                        (25+QDI_PRIVATE)
#define TIMER_QDI_CLR_64                     (26+QDI_PRIVATE)
#define TIMER_QDI_UNDEF                      (27+QDI_PRIVATE)
#define TIMER_QDI_PAUSE                      (28+QDI_PRIVATE)
#define TIMER_QDI_RESUME                     (29+QDI_PRIVATE)
#define TIMER_QDI_CLR_TASK_TIMERS            (30+QDI_PRIVATE)
#define TIMER_QDI_CLR_OSAL_THREAD_TIMERS     (31+QDI_PRIVATE)
#define TIMER_QDI_GET_SCLK_TILL_EXPIRY       (32+QDI_PRIVATE)
#define TIMER_QDI_GET_SCLK_TILL_EXPIRY_64    (33+QDI_PRIVATE)
#define TIMER_QDI_SET_WAKEUP                 (34+QDI_PRIVATE)
#define TIMER_QDI_SET_WAKEUP_64              (35+QDI_PRIVATE)
#define TIMER_QDI_RESTART                    (36+QDI_PRIVATE)
#define TIMER_QDI_REGISTER_FOR_NON_DEFER_EXPIRY_CHANGE (37+QDI_PRIVATE)

/* Call back method */
#define TIMER_QDI_GET_CB_INFO                (38+QDI_PRIVATE)

/* Signal mask for signalling data enque status */
#define TIMER_REMOTE_EXPIRY_ENQUEUED  0x00000001

typedef struct timer_qdi_opener_type {
  qurt_qdi_obj_t   qdiobj;
  uint32           process_idx;
  q_type           timer_qdi_q;
  qurt_anysignal_t timer_qdi_sig;
}timer_qdi_opener;

typedef struct timer_cb_info_type {
   timer_ptr_type timer;
   uint32 seq;
   uint32 cb_type;
   uint32 sig_func_addr;
   uint32 mask_data1;
   uint32 data2; /* For padding */
}timer_cb_info;

typedef struct timer_qdi_queue_s {
  q_link_type    link;      /* Queue field */
  timer_cb_info  cb_info;   /* CB Info */
}timer_qdi_queue_type;

/*==============================================================================

                        TIMER GROUP INTERNAL INLINE MACROS

=============================================================================*/
static inline int timer_invoke_group_set_deferrable(int handle, timer_group_ptr group, boolean condition)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_GROUP_SET_DEFERRABLE, group, condition);
}

static inline int timer_invoke_group_enable(int handle, timer_group_ptr group)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_GROUP_ENABLE, group);
}

static inline int timer_invoke_group_disable(int handle, timer_group_ptr group)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_GROUP_DISABLE, group);
}

/*==============================================================================

                            DEFER-UNDEFER INTERNAL INLINE MACROS

=============================================================================*/

static inline int timer_invoke_get_first_non_deferrable_timer_expiry(
                                 int                        handle,
                                 time_timetick_type         *tick,
                                 timer_non_defer_data_ptr   timer_data_ptr)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_GET_FIRST_NON_DEFERRABLE_TIMER_EXPIRY, tick, timer_data_ptr);
}

static inline int timer_invoke_defer_match_interrupt_64(int handle, time_timetick_type *tick)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_DEFER_MATCH_INTERRUPT_64, tick);
}

static inline int timer_invoke_undefer_match_interrupt(int handle)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_UNDEFER_MATCH_INTERRUPT);
}

/*==============================================================================

                            TIMER INTERNAL INLINE MACROS

=============================================================================*/
static inline timer_error_type timer_invoke_def_osal(
                                   int                             handle, 
                                   timer_ptr_type                  timer,
                                   timer_group_ptr                 group, 
                                   timer_notify_type               cb_type,
                                   time_osal_notify_obj_ptr        sigs_func_addr,
                                   time_osal_notify_data           sigs_mask_data)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_DEF_OSAL, timer, group, cb_type, sigs_func_addr, sigs_mask_data);
}

static inline timer_error_type timer_invoke_def2(
                                   int                            handle, 
                                   timer_ptr_type                  timer,   
                                   timer_group_ptr                 group)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_DEF2, timer, group);
}

static inline timer_error_type timer_invoke_reg(
                                   int                             handle,
                                   timer_ptr_type                  timer,   
                                   timer_t2_cb_type                func,
                                   timer_cb_data_type              data,
                                   timetick_type                   time_ms,
                                   timetick_type                   reload_ms)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_REG, timer, func, data, time_ms, reload_ms);
}

static inline timer_error_type timer_invoke_set_64(
                                   int                             handle,
                                   timer_ptr_type                  timer,
                                   time_timetick_type              time,
                                   time_timetick_type              reload,
                                   timer_unit_type                     unit)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_SET_64, timer, time>>32, time, reload>>32, reload, unit);
}

static inline timer_error_type timer_invoke_set_absolute(
                                   int                             handle,
                                   timer_ptr_type                  timer,
                                   time_timetick_type              time)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_SET_ABSOLUTE, timer, time>>32, time);
}

static inline int timer_invoke_get_64(
                                   int                             handle,
                                   timer_ptr_type                  timer,
                                   timer_unit_type                     unit,
                                   time_timetick_type             *tick)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_GET_64, timer, unit, tick);
}

static inline int timer_invoke_is_active(
                                   int                             handle,
                                   timer_ptr_type                  timer)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_IS_ACTIVE, timer);
}

static inline int timer_invoke_expires_at_64(
                                   int                             handle,
                                   timer_ptr_type                  timer,
                                   time_timetick_type             *tick)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_EXPIRES_AT_64, timer, tick);
}

static inline int timer_invoke_get_start_64(
                                   int                             handle,
                                   timer_ptr_type                  timer,
                                   time_timetick_type             *tick)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_GET_START_64, timer, tick);
}

static inline int timer_invoke_clr_64(
                                   int                             handle,
                                   timer_ptr_type                  timer,
                                   timer_unit_type                     unit,
                                   time_timetick_type             *tick)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_CLR_64, timer, unit, tick);
}

static inline timer_error_type timer_invoke_undef(
                                   int                             handle,
                                   timer_ptr_type                  timer)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_UNDEF, timer);
}

static inline timer_error_type timer_invoke_pause(
                                   int                             handle,
                                   timer_ptr_type                  timer)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_PAUSE, timer);
}

static inline timer_error_type timer_invoke_resume(
                                   int                             handle,
                                   timer_ptr_type                  timer)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_RESUME, timer);
}

static inline int timer_invoke_clr_osal_thread_timers(
                                   int                             handle,
                                   time_osal_thread_id               thread_id)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_CLR_OSAL_THREAD_TIMERS, thread_id);
}

static inline int timer_invoke_get_sclk_till_expiry_64(
                                   int                             handle,
                                   time_timetick_type             *tick)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_GET_SCLK_TILL_EXPIRY_64, tick);
}

static inline int timer_invoke_set_wakeup_64(
                                   int                             handle,
                                   time_timetick_type              wakeup_sclks)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_SET_WAKEUP_64, wakeup_sclks>>32, wakeup_sclks);
}

static inline int timer_invoke_restart(int                             handle)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_RESTART);
}

static inline int timer_invoke_register_for_non_defer_expiry_change(
	                                   int							handle,
	                                   timer_notify_type			notify_type,
                                       time_osal_notify_obj_ptr 	notify_obj,
                                       time_osal_notify_data 		notify_mask)
{
   return qurt_qdi_handle_invoke(handle, 
   	                             TIMER_QDI_REGISTER_FOR_NON_DEFER_EXPIRY_CHANGE,
   	                             notify_type,
   	                             notify_obj,
   	                             notify_mask);
}

static inline int timer_invoke_get_cb_info(int handle, timer_cb_info *cb_details)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_GET_CB_INFO, cb_details);
}

#endif /* TIMER_QDI_V_H */