/*
#============================================================================
#  Name:
#    dog_virtual_user.c 
#
#  Description:
#    Virtual watchdog component in USER-PD
#
# Copyright (c) 2015-2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
#============================================================================
*/

#include "stdarg.h"
#include "stdlib.h"
#include <stringl/stringl.h>
#include "comdef.h"
#include "err.h"
#include "qurt.h"
#include "tms_utils_msg.h"
#include "tms_utils.h"
#include "tms_dll_api.h"
#include "timer.h"
#include "rcecb.h"
#include "rcinit.h"
#include "servreg_locator.h"

#include "dog_virtual.h"
#include "dog_hal.h"

#define DOG_VIRTUAL_TASK_NAME               "dog_vir_rtask"

static struct
{
   qurt_thread_t tid;
   int32_t qdi_client_handle;
   timer_type startup_grace_timer; /* One time non-deferrable timer tightly coupled to RCINIT_RCECB_INITGROUPS */
                                   /* A fail safe mechanism to detect RCINIT groups initialization has failed. */
} dog_vir_user_internal;

/* If this flag is set to TRUE, 40sec startup gracetimer for user_pd will not be initialized.
*  This flag should be set to TRUE only for DEBUG purposes eg TRACE32 Instrument to Disable hw wdog At Runtime
*/
static volatile boolean dog_disable_user_startup_timer = FALSE;

/* Static functions in this file */
static void dog_virtual_report(void);
static void dog_virtual_StartupSuccessCb(void);
static void dog_virtual_StartupGraceFailureCb(timer_cb_data_type);

/* Extern for signaling dog heartbeart task */
extern void dog_hb_event_signal(void);

/** =====================================================================
 * Function:
 *     dog_qdi_invoke_pong
 *
 * Description:
 *     Invoke the qdi method to report back to the virtual dog
 *
 * Parameters:
 *
 * Returns:
 *    Refer to the enum DOG_VIRTUAL_RESULT for list of possible results
 * =====================================================================  */
DOG_VIRTUAL_RESULT dog_qdi_invoke_hb_register(void)
{
   int ret = -1;

   ret = qurt_qdi_handle_invoke(dog_vir_user_internal.qdi_client_handle, DOG_QDI_HB_REGISTER);

   if(-1 != ret)
   {
      return DOG_VIRTUAL_SUCCESS;
   }
   else
   {
      return DOG_VIRTUAL_FAILURE;
   }
}

/** =====================================================================
 * Function:
 *     dog_qdi_invoke_worker_wait
 *
 * Description:
 *     Invoke the qdi method to wait for the dog hb root signal
 *
 * Parameters:
 *
 * Returns:
 *    Refer to the enum DOG_VIRTUAL_RESULT for list of possible results
 * =====================================================================  */
DOG_VIRTUAL_RESULT dog_qdi_invoke_worker_wait(uint32_t * remote_mask)
{
   int ret = -1;

   ret = qurt_qdi_handle_invoke(dog_vir_user_internal.qdi_client_handle, DOG_QDI_WAIT, remote_mask);

   if(-1 != ret)
   {
      return DOG_VIRTUAL_SUCCESS;
   }
   else
   {
      return DOG_VIRTUAL_FAILURE;
   }
}

/** =====================================================================
 * Function:
 *     dog_qdi_invoke_pong
 *
 * Description:
 *     Invoke the qdi method to report back to the virtual dog
 *
 * Parameters:
 *
 * Returns:
 *    Refer to the enum DOG_VIRTUAL_RESULT for list of possible results
 * =====================================================================  */
DOG_VIRTUAL_RESULT dog_qdi_invoke_pong(void)
{
   int ret = -1;

   ret = qurt_qdi_handle_invoke(dog_vir_user_internal.qdi_client_handle, DOG_QDI_PONG);

   if(-1 != ret)
   {
      return DOG_VIRTUAL_SUCCESS;
   }
   else
   {
      return DOG_VIRTUAL_FAILURE;
   }
}

/** =====================================================================
 * Function:
 *     dog_qdi_invoke_close
 *
 * Description:
 *     Closes the specified driver, releasing any resources associated with the open driver
 *
 * Parameters:
 *     handle : handle to be released
 *
 * Returns:
 *     Refer to the enum DOG_VIRTUAL_RESULT for list of possible results
 * =====================================================================  */
DOG_VIRTUAL_RESULT dog_qdi_invoke_close(void)
{
   int ret = -1;

   ret = qurt_qdi_close(dog_vir_user_internal.qdi_client_handle);

   if(-1 != ret)
   {
      return DOG_VIRTUAL_SUCCESS;
   }
   else
   {
      return DOG_VIRTUAL_FAILURE;
   }
}

/** =====================================================================
* Function:
*     dog_virtual_report
*
* Description:
*       hb driven function for virtual dog
*
* Parameters:
*     none
*
* Returns:
*     none
* =====================================================================  */
static void dog_virtual_report(void)
{
   if(DOG_VIRTUAL_FAILURE == dog_qdi_invoke_pong())
   {
      ERR_FATAL("DOG_VIRTUAL: User-pd dog report failed", 0, 0, 0);
   }
   return;
}

/** =====================================================================
* Function:
*     dog_virtual_StartupSuccessCb
*
* Description:
*   This function is called after rc group initialization is done.  
*   It clears the timer and de-registers with RCvent for any future events.
*
* Parameters:
*     none
*
* Returns:
*     none
* =====================================================================  */
static void dog_virtual_StartupSuccessCb(void)
{
   time_timetick_type remaining_time= 0;

   /* Clear the timer */
   timer_stop(&dog_vir_user_internal.startup_grace_timer, T_NONE, &remaining_time);

   /* Undefine the timer */
   timer_undef(&dog_vir_user_internal.startup_grace_timer);

   /* Unregister with RC */
   (void)rcecb_unregister_name(RCINIT_RCECB_INITGROUPS, dog_virtual_StartupSuccessCb);
}

/** =====================================================================
* Function:
*     dog_virtual_StartupGraceFailureCb
*
* Description:
*   This function is called when the dog start up grace timer expires.  
*   It results in Error fatal.
*
* Parameters:
*     none
*
* Returns:
*     none
* =====================================================================  */
static void dog_virtual_StartupGraceFailureCb(timer_cb_data_type unused)
{
   time_timetick_type remaining_time= 0;

   /* Clear the timer */
   timer_stop(&dog_vir_user_internal.startup_grace_timer, T_NONE, &remaining_time);

   /* Undefine the timer */
   timer_undef(&dog_vir_user_internal.startup_grace_timer);

   ERR_FATAL("USER-PD DOG detects stalled initialization, triage with IMAGE OWNER", 0, 0, 0);
}

/** =====================================================================
 * Task:
 *
 * Description:
 *
 * Parameters:
 *     None
 * 
 * Returns:
 *     None
 * =====================================================================  */
static void dog_virtual_user_task(void *argv __attribute__((unused)))
{
   uint32_t mask = 0;

   dog_qdi_invoke_hb_register();

   /* Task forever loop */
   for (;;)
   {
      dog_qdi_invoke_worker_wait(&mask);

      if(mask & DOG_VIR_SIGNAL_MASK)
      {
         dog_virtual_report();
         dog_hb_event_signal(); /* signal DOG_HB_EVENT */
         TMS_MSG_LOW("Dog reported back to root and signaled user dog_hb");
      }
   }
}

/** =====================================================================
 * Function:
 *     dog_qdi_init
 *
 * Description:
 *     Initialization function in user pd.
 *     Function to obtain a QDI handle given the qdi device name.
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
DLL_API_GLOBAL void dog_qdi_init(void)
{
   char* domain = NULL;
   char taskname[20];
   int len = 0;
   qurt_thread_attr_t attr;
   static unsigned long stack[DOG_VIRTUAL_TASK_STACK/sizeof(unsigned long)];

   domain = servreg_get_local_domain();

   if (NULL != domain)
   {
      len = strlen(domain) + 1;
      dog_vir_user_internal.qdi_client_handle = qurt_qdi_open(DOG_QDI_DEVICE, domain, len);
   }

   if (0 > dog_vir_user_internal.qdi_client_handle)
   {
      ERR_FATAL("User-pd dog_qdi_init failed", 0, 0, 0);
   }

   (void)tms_utils_fmt(taskname, 20, "%s%d", DOG_VIRTUAL_TASK_NAME, qurt_process_get_id());

   qurt_thread_attr_init(&attr);
   qurt_thread_attr_set_name(&attr, taskname);
   qurt_thread_attr_set_stack_addr(&attr, stack);
   qurt_thread_attr_set_stack_size(&attr, sizeof(stack));
   qurt_thread_attr_set_priority(&attr, qurt_thread_get_priority(qurt_thread_get_id()) - 1);

   qurt_thread_create(&dog_vir_user_internal.tid, &attr, dog_virtual_user_task, NULL);

   if(dog_disable_user_startup_timer != TRUE)
   {
      /* Event indicates startup completes */
      (void)rcecb_register_name(RCINIT_RCECB_INITGROUPS, dog_virtual_StartupSuccessCb); 

      /* Define watchdog timer */
      if(TE_SUCCESS != timer_def_osal(&dog_vir_user_internal.startup_grace_timer, 
                                     &timer_non_defer_group, 
                                     TIMER_FUNC1_CB_TYPE, 
                                     dog_virtual_StartupGraceFailureCb, NULL ))
      {
        ERR_FATAL("Dog vir_user startup_grace_timer def failed", 0, 0, 0);
      }

      /* Set watchdog timer */
      if(TE_SUCCESS != timer_set_64(&dog_vir_user_internal.startup_grace_timer, 
                                   DOG_STARTUP_GRACE_TIME_SEC, 
                                   0, T_SEC ))
      {
        ERR_FATAL("Dog vir_user startup_grace_timer set failed", 0, 0, 0);
      }
   }
}


