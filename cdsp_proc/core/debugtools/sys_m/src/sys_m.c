/**
@file sys_m.c
@brief This file contains the API for the System Monitor Framework API 1.0 service.
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2013-17 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/sys_m/src/sys_m.c#1 $
$DateTime: 2018/07/30 01:21:36 $
$Author: pwbldsvc $
===============================================================================*/
#include "err.h"
#include "rcevt_dal.h"
#include "sys_m.h"

#include "sys_m_internal.h"
#include "sys_m_smsm.h"
#include "err_crumb_trail_mask.h"
#include "sys_m_messages.h"
#include "tms_utils_msg.h"
#include "rcecb.h"
#include "rcevt_internal.h"
#include "rcinit.h"
#include "DALSys.h"
#include "smem.h"

#include "sys_m_fatal.h"
#include "qurt.h"
#include "sys_m_sizes.h"
#include "qurt.h"
#include "qurt_event.h"
#include "timer.h"   /* For shutdown_STM_delay_timer */
extern void HAL_dogDisable( void );
static volatile int always_true = 1;
static volatile int  always_false = 0;

extern void dog_force_bite(void);
extern void dog_force_kick( void );

extern  void SYS_M_LOCK(void);
extern  void SYS_M_UNLOCK(void);

int sys_m_xpu_registered = 0;

int sys_m_forced_shutdown = 0;

void sys_m_register_fatal_notification(void);
extern void sys_m_qurt_fatal_notification(void* no_param);

static timer_type       shutdown_STM_delay_timer;
static timer_group_type sd_delay_timer_group = {0};
int sys_m_shutdown_delay_secs =0;

void sys_m_shutdown_delay_secs_set(unsigned int secs)
{
  sys_m_shutdown_delay_secs = secs;
}

/**
 *
 * @brief sys_m_signal_modules
 *
 * Signal interested modules of a system monitor event.  After all clients have completed
 * notification, acknowledge to apps
 *
*/
void sys_m_signal_modules(RCEVT_HANDLE handle)
{
    int orig_ack_count = 0;
    int req_count = 0;


    if(NULL == handle){
       return;
    }
    //callbacks
    if(rcecb_signal_handle(handle)){}

    //get count of current clients
    req_count = rcevt_getregistrants_handle(handle);
    orig_ack_count = rcevt_getcount_name(SYS_M_ACK);
    //signal
    if(rcevt_signal_handle(handle)){}
    //wait for all clients to ack
    if(rcevt_wait_count_name(SYS_M_ACK, req_count + orig_ack_count)){} // blocking wait
}


/**
 *
 * @brief sys_m_shutdown_internal
 *
 * Signal interested modules to prepare for shutdown.  After all clients have completed
 * notification, acknowledge to apps
 *
*/
void sys_m_shutdown_internal(void)
{
   int req_count = 0;
   int orig_ack_count = 0;

   //callbacks
   if(rcecb_signal_name(SYS_M_SHUTDOWN)){}

   req_count = rcevt_getregistrants_name(SYS_M_SHUTDOWN);
   orig_ack_count = rcevt_getcount_name(SYS_M_SHUTDOWN_ACK);
   if(rcevt_signal_name(SYS_M_SHUTDOWN)){}
   if(rcevt_wait_count_name(SYS_M_SHUTDOWN_ACK, req_count + orig_ack_count)){} // blocking wait
}


/**
 *
 * @brief sys_m_init_sfr_buffer
 *
 * Initialize the SFR buffer.  Only error services should call
 * this function
 *
 *  @return
 *  NULL if smem_boot_init has not been called yet
 *  Otherwise, returns either a valid SMEM address to the requested buffer or a
 *  fatal error
 *
*/
void *sys_m_init_sfr_buffer(void)
{
   return smem_alloc (SYS_M_SMEM_SSR_RES, ERR_SSR_REASON_SIZE_BYTES);
}


/**
 *
 * @brief sys_m_shutdown
 *
 * Begin shutdown process
 *
*/
void sys_m_shutdown(SYS_M_COMMUNICATION_TYPE sys_m_comm)
{
   int32 return_val = 0;

   // commanded shutdown begins, this is a time sensitive operation, with
   // the time of the operation not under control of the peripheral shutting down.
   // all operations must be expedient and complete in the minimum amount of time
   // possible. technology areas must not prolong their "cleanup," the shutdown
   // process must be considered a software exception, and the technology areas
   // do not get infinite time to handle their part of the software exception.

   TMS_SHUTDOWN_MSG_HIGH_1("sys_m_shutdown process begins, expect degraded runtime operation sys_m_comm: %d", sys_m_comm);

   SYS_M_LOCK();

   // shutdown
   sys_m_shutdown_internal();

   // completion indication
   switch(sys_m_comm)
   {
      case SYS_M_SMSM_P2P:
         //ACKing using SMP2P bit after shutdown completion
         return_val = sys_m_smsm_apps_set(SYS_M_SUBSYS2AP_SMSM_SHUT_DWN_ACK);
         break;
      case SYS_M_QMI:
         break; /*indication will be sent by SSCTLv02 QMI*/
      default:
         break;
   }

   SYS_M_UNLOCK();

   // at this point, the peripheral is operating in a degraded state. technology
   // areas cannot make any assumptions about the availability of any specific
   // services. all technology areas at this point should have parked their services
   // into a logical non-processing state.

   TMS_SHUTDOWN_MSG_HIGH_1("sys_m_shutdown process ends, expect degraded runtime operation return_val: %d", return_val);
}


static uint32 wait_counter = 10;

/**
 *
 * @brief sys_m_qurt_fatal_notification
 *
 * Special function run in a unique context by qurt post error
 * handling to notify apps of failure
 *
*/
void sys_m_qurt_fatal_notification_helper(void)
{
   err_update_crumb_trail_mask(ERR_CRUMB_TRAIL_BMSK_FATAL_NOTIFY_INVOKED);
   
   //special callback for XPU handling
   if(rcecb_signal_name_nolocks(SYS_M_XPU_ERR_STRING)){}

   err_update_crumb_trail_mask(ERR_CRUMB_TRAIL_BMSK_FATAL_NOTIFY_XPU_HANDLED);

   if (!sys_m_forced_shutdown)
   {
      sys_m_smsm_apps_set_stm(SYS_M_SUBSYS2AP_SMSM_ERRFATAL);
   }
   else
   {
      sys_m_smsm_apps_set_stm(SYS_M_SUBSYS2AP_SMSM_SHUT_DWN_ACK);
   }

   err_update_crumb_trail_mask(ERR_CRUMB_TRAIL_BMSK_FATAL_NOTIFY_SUBSYS2AP);

   // Per requirement, delay 2 seconds before letting wdog expire
   while (--wait_counter > 0)
   {
      dog_force_kick();
      DALSYS_BusyWait(200000); //wait 200ms
   }

   err_update_crumb_trail_mask(ERR_CRUMB_TRAIL_BMSK_FATAL_NOTIFY_BUSYWAIT);

   dog_force_bite();
   
   err_update_crumb_trail_mask(ERR_CRUMB_TRAIL_BMSK_FATAL_NOTIFY_DOG_BITE);

   while (1)
   {
      DALSYS_BusyWait(200000); //wait 200ms
   }
}

/**
 *
 * @brief sys_m_register_fatal_notification
 *
 * Registers fatal notification handler with the kernel
 *
 * @param func: callback func
 *
*/
void sys_m_xpu_handler_register(RCECB_CALLBACK_FNSIG func)
{
   SYS_M_LOCK();

   if (sys_m_xpu_registered == 0)
   {
      if (rcecb_register_name(SYS_M_XPU_ERR_STRING , func)) {;}
      sys_m_xpu_registered = 1;
   }
   else
   {
      ERR_FATAL("Multiple XPU registrants", 0, 0, 0);
   }

   SYS_M_UNLOCK();
}

/**
 *
 * @brief sys_m_register_fatal_notification
 *
 * Registers fatal notification handler with the kernel
 *
*/
void sys_m_register_fatal_notification(void)
{
   // as long as we do not get an error, we are done
   if ( QURT_EVAL == qurt_exception_register_fatal_notification(sys_m_qurt_fatal_notification, NULL) )
   {
      ERR_FATAL("Could not register qurt fatal notification handler", 0, 0, 0);
   }
}

void sys_m_shutdown_terminate_in_STM(timer_cb_data_type delay)
{

  if (delay)
  {
     
    TMS_SHUTDOWN_MSG_HIGH_1("In sys_m_shutdown_stm with delay: %d", delay);
	 // define callback timer to call this function without delay
    timer_def_osal(&shutdown_STM_delay_timer, &sd_delay_timer_group, TIMER_FUNC1_CB_TYPE, sys_m_shutdown_terminate_in_STM, 0);
    timer_set_64(&shutdown_STM_delay_timer, delay, 0, T_SEC);
  }
  else
  {

     HAL_dogDisable();

     qurt_exception_raise_fatal();

     sys_m_smsm_apps_set_stm(SYS_M_SUBSYS2AP_QMI_REQ_ACK);

     rcecb_signal_name_nolocks(SYS_M_SHUTDOWN_STM_CLEANUP);

     /*flusing cache since if we get any issue as a side effect of 
     shutdown in STM then we may have cache flushed RAM dumps*/
     qurt_mem_cache_clean(0,0, QURT_MEM_CACHE_FLUSH_ALL, QURT_MEM_DCACHE); 
     
     while(always_true)
     {
       if(always_false)
         break;
     }
  }
}


