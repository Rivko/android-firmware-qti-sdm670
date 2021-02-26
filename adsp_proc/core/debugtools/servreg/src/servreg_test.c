/*
#============================================================================
#  Name:
#    servreg_glink.c 
#
#  Description:
#    Implements GLINK layer for Service registry
#
# Copyright (c) 2015-2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
#============================================================================
*/
#include "stdarg.h"
#include "stdlib.h"
#include <stringl/stringl.h>
#include "comdef.h"             /* Definitions for byte, word, etc.     */
#include "err.h"
#include "tms_utils_msg.h"
#include "rcinit.h"
#include "timer.h"
#include "gen_cb_ctxt.h"

/* Include Service Registry public header files */
#include "servreg_notifier.h"
#include "servreg_localdb.h"
#include "servreg_common.h"
#include "servreg_locator.h"
#include "servreg_monitor.h"
#include "servreg_test.h"

#define SERVREG_TEST_TASK_NAME                    "servreg_test"
#define SERVREG_TEST_TASK_STACK                   4096

#define SERVREG_TEST_SERVICE_NAME             "tms/servreg_test"
#define SR_TEST_STATE_CHANGE_TIME_SEC         5              // sec period for servreg_state_change_cb

static timer_group_type sr_test_group = { 0 };
static timer_type sr_test_state_change_timer = { 0 };
static void servreg_state_change_cb(void* unused);
static void servreg_generic_task_cb(timer_cb_data_type unused);

/* SR handles in this test file */
SERVREG_MON_HANDLE sr_mon_service_handle = SERVREG_NULL;
SERVREG_MON_HANDLE sr_mon_pd_handle = SERVREG_NULL;
SERVREG_MON_HANDLE sr_mon_local_list_handle = SERVREG_NULL;
SERVREG_MON_HANDLE sr_mon_remote_list_qmi_handle = SERVREG_NULL;
SERVREG_SERVICE_STATE sr_curr_pd_state, sr_curr_state = SERVREG_SERVICE_STATE_UNINIT;

/** =====================================================================
* Function:
*     servreg_state_change_cb
*
* Description:
*       Generic callback task to change the state of the service.
*
* Parameters:
*     none
*
* Returns:
*     none
* =====================================================================  */
static void servreg_state_change_cb(void* unused)
{
   SERVREG_SERVICE_STATE sr_next_state = SERVREG_SERVICE_STATE_UNINIT;
   SERVREG_SERVICE_STATE sr_next_pd_state = SERVREG_SERVICE_STATE_UNINIT;

   /* Change the service = "tms/servreg_test" state */
   if(SERVREG_NULL != sr_mon_service_handle)
   {
      if(SERVREG_SERVICE_STATE_DOWN == sr_curr_state)
      {
         sr_next_state = SERVREG_SERVICE_STATE_UP;
      }
      else if(SERVREG_SERVICE_STATE_UP == sr_curr_state)
      {
         sr_next_state = SERVREG_SERVICE_STATE_DOWN;
      }
      else if(SERVREG_SERVICE_STATE_UNINIT == sr_curr_state)
      {
         sr_next_state = SERVREG_SERVICE_STATE_UP;
      }

      if(SERVREG_SUCCESS == servreg_set_state(sr_mon_service_handle, sr_next_state))
      {
         TMS_MSG_HIGH_1("Service state changed to 0x%x \n", sr_next_state);
      }
      else
      {
         TMS_MSG_ERROR("servreg_set_state() failed");
      }

      sr_curr_state = sr_next_state;
   }
   else
   {
      TMS_MSG_ERROR("sr_mon_service_handle NULL");
   }

   /* Change the PD state */
   if(SERVREG_NULL != sr_mon_pd_handle)
   {
      if(SERVREG_SERVICE_STATE_DOWN == sr_curr_pd_state)
      {
         sr_next_pd_state = SERVREG_SERVICE_STATE_UP;
      }
      else if(SERVREG_SERVICE_STATE_UP == sr_curr_pd_state)
      {
         sr_next_pd_state = SERVREG_SERVICE_STATE_DOWN;
      }
      else if(SERVREG_SERVICE_STATE_UNINIT == sr_curr_pd_state)
      {
         sr_next_pd_state = SERVREG_SERVICE_STATE_UP;
      }

      if(SERVREG_SUCCESS == servreg_set_state(sr_mon_pd_handle, sr_next_pd_state))
      {
         TMS_MSG_HIGH_1("PD state changed to 0x%x \n", sr_next_pd_state);
      }
      else
      {
         TMS_MSG_ERROR("servreg_set_state() failed");
      }

      sr_curr_pd_state = sr_next_pd_state;
   }
   else
   {
      TMS_MSG_ERROR("sr_mon_pd_handle NULL");
   }

   return;
}

/** =====================================================================
* Function:
*     servreg_generic_task_cb
*
* Description:
*     Timer driven function to offload and run from generic callback task 
*     context instead of timer context
*
* Parameters:
*     unused
*
* Returns:
*     none
* =====================================================================  */
static void servreg_generic_task_cb(timer_cb_data_type unused)
{
   gen_cb_ctxt_invoke_cb(servreg_state_change_cb, NULL);
}

/** =====================================================================
 * Function:
 *     servreg_test_case_trigger_local_state_change
 *
 * Description:
 *      This function tests if Service states can be changed and if clients
 *      get notified of the state change.
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
void servreg_test_case_trigger_local_state_change(void)
{
   //timer_group_set_deferrable(&sr_test_group, TRUE);

   if(TE_SUCCESS != timer_def_osal(&sr_test_state_change_timer,
                                  &sr_test_group,
                                  TIMER_FUNC1_CB_TYPE,
                                  servreg_generic_task_cb, NULL))
   {
       TMS_MSG_ERROR("State Change Timer def failed");
   }

   if(TE_SUCCESS != timer_set_64(&sr_test_state_change_timer,
                                SR_TEST_STATE_CHANGE_TIME_SEC,
                                SR_TEST_STATE_CHANGE_TIME_SEC,
                                T_SEC))
   {
       TMS_MSG_ERROR("State Change Timer set failed");
   }
   return;
}

/** =====================================================================
 * Function:
 *     servreg_test_case_service_state_init
 *
 * Description:
 *      This function register a "SERVICE" with the SERVICE REGISTRY framework
 *      and marks it as UP
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
void servreg_test_case_service_state_init(void)
{
   SERVREG_NAME domain = SERVREG_NULL;
   SERVREG_RESULT ret = SERVREG_FAILURE;

   domain = servreg_get_local_domain();

   sr_mon_pd_handle = servreg_get_sr_mon_handle(domain);

   if(SERVREG_NULL == sr_mon_pd_handle)
   {
      TMS_MSG_ERROR("could not get pd handle - check if servreg_monitor_init() is successful");
   }

   if(SERVREG_NULL != domain)
   {
      /* Allocate a new monitor handle for the service */
      sr_mon_service_handle = servreg_alloc_monitor_handle(domain, SERVREG_TEST_SERVICE_NAME);

      if(SERVREG_NULL != sr_mon_service_handle)
      {
         ret = servreg_set_state(sr_mon_service_handle, SERVREG_SERVICE_STATE_UP);

         if(SERVREG_SUCCESS == ret)
         {
            TMS_MSG_HIGH("in servreg_test_case_service_state_init() Service registered successfully with SR \n");
         }
         else
         {
            TMS_MSG_ERROR("in servreg_test_case_service_state_init() Service did not register with SR \n");

            ret = servreg_free_monitor_handle(sr_mon_service_handle);
            if(SERVREG_SUCCESS != ret)
            {
               TMS_MSG_ERROR("in servreg_test_case_service_state_init() could not free the monitor handle");
            }

            TMS_MSG_ERROR("in servreg_test_case_service_state_init() failed");
         }
      }
      else
      {
         TMS_MSG_ERROR("in servreg_test_case_service_state_init() sr_mon_service_handle is null");
      }
   }
   else
   {
      TMS_MSG_ERROR("in servreg_test_case_service_state_init() servreg_get_local_domain() is null");
   }
   return;
}

/** =====================================================================
 * Function:
 *     servreg_test_case_set_local_service_sync
 *
 * Description:
 *     Function sets the local servreg_test service as synchronous
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
void servreg_test_case_set_local_service_sync(void)
{
   SERVREG_RESULT ret = SERVREG_FAILURE;

   if(SERVREG_NULL != sr_mon_service_handle)
   {
      ret = servreg_set_synchronous(sr_mon_service_handle);

      if(SERVREG_SUCCESS == ret)
      {
         TMS_MSG_HIGH("servreg_test service set as synchronous \n");
      }
      else
      {
         TMS_MSG_ERROR("servreg_test service failed to set as synchronous");
      }
   }
   else
   {
      TMS_MSG_ERROR("sr_mon_service_handle is null");
   }

   return;
}

/** =====================================================================
 * Function:
 *     servreg_test_case_register_listener_local_service
 *
 * Description:
 *      This function tests registers a client as a listener for local service
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
void servreg_test_case_register_listener_local_service(void)
{
   SERVREG_NAME domain = SERVREG_NULL;
   SERVREG_RESULT ret = SERVREG_FAILURE;

   domain = servreg_get_local_domain();

   if(SERVREG_NULL != domain)
   {
      sr_mon_local_list_handle = servreg_alloc_monitor_handle(domain, SERVREG_TEST_SERVICE_NAME);

      if(SERVREG_NULL != sr_mon_local_list_handle)
      {
         ret = SR_TEST_REGISTER_LISTENER(sr_mon_local_list_handle, SERVREG_TEST_SIG_LOCAL_LIST_MASK);

         if(SERVREG_SUCCESS == ret)
         {
            TMS_MSG_HIGH("Client registered successfully with servreg_register_listener() \n");
         }
         else
         {
            TMS_MSG_ERROR("Client did not register with  servreg_register_listener() \n");

            ret = servreg_free_monitor_handle(sr_mon_local_list_handle);
            if(SERVREG_SUCCESS != ret)
            {
               TMS_MSG_ERROR("could not free the monitor handle");
            }

            TMS_MSG_ERROR("servreg_test_case_register_listener_local_service() failed ");
         }
      }
      else
      {
         TMS_MSG_ERROR("sr_mon_local_list_handle is null");
      }
   }
   else
   {
      TMS_MSG_ERROR("servreg_get_local_domain() is null");
   }

   return;
}

/** =====================================================================
 * Function:
 *     servreg_test_case_register_listener_remote_qmi_service
 *
 * Description:
 *      This function tests registers a client as a listener for remote service
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
void servreg_test_case_register_listener_remote_qmi_service(void)
{
   SERVREG_DL_HANDLE dlhandle = SERVREG_NULL;
   SERVREG_NAME domain = SERVREG_NULL, domain_scope = SERVREG_NULL;
   SERVREG_RESULT ret = SERVREG_FAILURE;
   uint32_t domain_count = 0;


   dlhandle = servreg_alloc_domainlist_handle();
   if(SERVREG_NULL != dlhandle)
   {
      ret = servreg_get_domainlist(SERVREG_TEST_SERVICE_NAME, dlhandle);

      if(SERVREG_SUCCESS == ret)
      {
         domain_count = servreg_get_num_entries(dlhandle);
         
         if(domain_count > 0)
         {
            ret = servreg_get_entry(dlhandle, 1, &domain);

            if(SERVREG_SUCCESS != ret)
            {
               TMS_MSG_ERROR("servreg_get_entry() failed \n");
            }
         }
      }
      else
      {
         TMS_MSG_ERROR("servreg_get_domainlist() failed \n");
      }
   }
   else
   {
      TMS_MSG_ERROR("servreg_alloc_domainlist_handle() failed \n");
   }

   if(SERVREG_NULL == dlhandle || SERVREG_SUCCESS != ret)
   {
      domain_scope = servreg_get_local_domain_scope();

      if(SERVREG_NULL != domain_scope)
      {
         if(strcmp(domain_scope, "slpi") == 0)
         {
            domain = "msm/adsp/audio_pd";
            /* 74 is the qmi_instance_id for adsp */
            servreg_create_qmi_table_entry(domain, 74);
            TMS_MSG_ERROR("Could not get the domain info from locator, so assigning one = msm/adsp/root_pd \n");
         }
         else if(strcmp(domain_scope, "modem") == 0)
         {
            domain = "msm/slpi/root_pd";
            /* 90 is the qmi_instance_id for slpi */
            servreg_create_qmi_table_entry(domain, 90);
            TMS_MSG_ERROR("Could not get the domain info from locator, so assigning one = msm/slpi/root_pd \n");
         }
         else if(strcmp(domain_scope, "adsp") == 0)
         {
            domain = "msm/modem/root_pd";
            /* 180 is the qmi_instance_id for modem */
            servreg_create_qmi_table_entry(domain, 180);
            TMS_MSG_ERROR("Could not get the domain info from locator, so assigning one = msm/modem/root_pd \n");
         }
         else
         {
            domain = SERVREG_NULL;
         }
      }
      else
      {
         TMS_MSG_ERROR("SERVREG_TEST_U: servreg_get_local_domain_scope() is NULL \n");
         domain = SERVREG_NULL;
      }
   }

   if(SERVREG_NULL != domain)
   {
      sr_mon_remote_list_qmi_handle = servreg_alloc_monitor_handle(domain, SERVREG_NULL);

      if(SERVREG_NULL != sr_mon_remote_list_qmi_handle)
      {
         ret = SR_TEST_REGISTER_LISTENER(sr_mon_remote_list_qmi_handle, SERVREG_TEST_SIG_REMOTE_QMI_LIST_MASK);

         if(SERVREG_SUCCESS == ret)
         {
            TMS_MSG_HIGH("Client registered successfully with servreg_register_listener() \n");
         }
         else
         {
            TMS_MSG_ERROR("Client did not register with  servreg_register_listener() \n");

            ret = servreg_free_monitor_handle(sr_mon_remote_list_qmi_handle);
            if(SERVREG_SUCCESS != ret)
            {
               TMS_MSG_ERROR("could not free the monitor handle \n");
            }

            TMS_MSG_ERROR("SERVREG_TEST_U: servreg_test_case_register_listener_remote_qmi_service() failed ");
         }
      }
      else
      {
         TMS_MSG_ERROR("sr_mon_remote_list_qmi_handle is null");
      }
   }
   else
   {
     TMS_MSG_ERROR("domain entry from remote locator is null");
   }

   /* Free the domain list handle once monitor allocation is done */
   if(SERVREG_SUCCESS != servreg_free_domainlist_handle(dlhandle))
   {
      TMS_MSG_ERROR("servreg_test_case_register_listener_remote_qmi_service() dlhandle free failed");
   }

   return;
}

/** =====================================================================
 * Function:
 *     servreg_test_sync_cb
 *
 * Description:
 *     Function registers a callback function that gets called when the sync complete
 *     notification for that state is generated
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
static void servreg_test_sync_cb(SERVREG_CB_PARAM cb_p1, SERVREG_CB_PARAM cb_p2)
{
   TMS_MSG_HIGH("sync notif cb function called \n");
}

/** =====================================================================
 * Function:
 *     servreg_test_case_register_sync_notif
 *
 * Description:
 *     Function registers a callback function that gets called when the sync complete
 *     notification for that state is generated
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
void servreg_test_case_register_sync_notif(void)
{
   SERVREG_RESULT ret = SERVREG_FAILURE;

   /* In root pd you can get sync complete notification for local handles and remote handles */
   if(SERVREG_NULL != sr_mon_local_list_handle) // TO DO : CHANGE IT TO remote handle 
   {
      ret = servreg_register_sync_notif_cb(sr_mon_local_list_handle, SERVREG_SERVICE_STATE_DOWN, servreg_test_sync_cb, 0, 0);

      if(SERVREG_SUCCESS == ret)
      {
         TMS_MSG_HIGH("sync notif cb registered \n");
      }
      else
      {
         TMS_MSG_ERROR("sync notif cb did not register ");
      }
   }
   else
   {
      TMS_MSG_ERROR("sr_mon_local_list_handle is null");
   }

   return;
}

/** =====================================================================
 * Function:
 *     servreg_test_case_inject_state_change
 *
 * Description:
 *     Function changes the state of its own pd.
 *     To be used in the qxdm injection command
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
void servreg_test_case_inject_state_change(void)
{
   //SERVREG_SERVICE_STATE sr_next_state = SERVREG_SERVICE_STATE_UNINIT;
   SERVREG_SERVICE_STATE sr_next_pd_state = SERVREG_SERVICE_STATE_UNINIT;

   /* Change the service = "tms/servreg_test" state */
   /*if(SERVREG_NULL != sr_mon_service_handle)
   {
      if(SERVREG_SERVICE_STATE_DOWN == sr_curr_state)
      {
         sr_next_state = SERVREG_SERVICE_STATE_UP;
      }
      else if(SERVREG_SERVICE_STATE_UP == sr_curr_state)
      {
         sr_next_state = SERVREG_SERVICE_STATE_DOWN;
      }
      else if(SERVREG_SERVICE_STATE_UNINIT == sr_curr_state)
      {
         sr_next_state = SERVREG_SERVICE_STATE_UP;
      }

      if(SERVREG_SUCCESS == servreg_set_state(sr_mon_service_handle, sr_next_state))
      {
         TMS_MSG_HIGH_2("Service state changed from 0x%x to 0x%x \n", sr_curr_state, sr_next_state);
      }
      else
      {
         TMS_MSG_ERROR("servreg_set_state() failed");
      }

      sr_curr_state = sr_next_state;
   }
   else
   {
      TMS_MSG_ERROR("sr_mon_service_handle NULL \n");
      //TMS_MSG_ERROR("sr_mon_service_handle NULL");
   }*/

   /* Change the PD state */
   if(SERVREG_NULL != sr_mon_pd_handle)
   {
      if(SERVREG_SERVICE_STATE_DOWN == sr_curr_pd_state)
      {
         sr_next_pd_state = SERVREG_SERVICE_STATE_UP;
      }
      else if(SERVREG_SERVICE_STATE_UP == sr_curr_pd_state)
      {
         sr_next_pd_state = SERVREG_SERVICE_STATE_DOWN;
      }
      else if(SERVREG_SERVICE_STATE_UNINIT == sr_curr_pd_state)
      {
         sr_next_pd_state = SERVREG_SERVICE_STATE_UP;
      }

      if(SERVREG_SUCCESS == servreg_set_state(sr_mon_pd_handle, sr_next_pd_state))
      {
         TMS_MSG_HIGH_2("PD state changed from 0x%x to 0x%x \n", sr_curr_pd_state, sr_next_pd_state);
      }
      else
      {
         TMS_MSG_ERROR("servreg_set_state() failed");
      }

      sr_curr_pd_state = sr_next_pd_state;
   }
   else
   {
      TMS_MSG_ERROR("sr_mon_pd_handle NULL");
   }

   return;

}

/** =====================================================================
 * Task:
 *     servreg_test_task
 *
 * Description:
 *     Test task that tests Service Registry API's
 *
 * Parameters:
 *
 * Returns:
 * =====================================================================  */
void servreg_test_task(dword param)
{
   unsigned long int sigs;
   SERVREG_SERVICE_STATE sr_new_state;
   SERVREG_RESULT ret = SERVREG_FAILURE;
   uint32_t transaction_id = 0;

   /* Notify RCInit that all basic initialization is complete. */
   rcinit_handshake_startup();

   SR_TEST_SIGNAL_INIT();

   if(SERVREG_SERVICE_STATE_UP == servreg_get_servloc_availability())
   {
      /* Test cases */
      servreg_test_case_service_state_init();

      //servreg_test_case_set_local_service_sync();

      servreg_test_case_register_listener_local_service();

      servreg_test_case_register_listener_remote_qmi_service();

      //servreg_test_case_register_sync_notif();

      //servreg_test_case_trigger_local_state_change();

   }
   else
   {
      ret = SR_TEST_REGISTER_SERVLOC_AVAILABILITY();

      if(SERVREG_SUCCESS != ret)
      {
         TMS_MSG_ERROR("servreg_register_servloc_availability_signal() failed");
      }
   }

   for(;;)
   {
      sigs = SR_TEST_SIGNAL_WAIT();

      if(sigs & SERVREG_TEST_SIG_LOC_AVAILABLE)
      {
         /* Clear the signal */
         SR_TEST_SIGNAL_CLEAR(SERVREG_TEST_SIG_LOC_AVAILABLE);

         /* Test cases */
         servreg_test_case_service_state_init();

         //servreg_test_case_set_local_service_sync();

         servreg_test_case_register_listener_local_service();

         servreg_test_case_register_listener_remote_qmi_service();

         //servreg_test_case_register_sync_notif();

         //servreg_test_case_trigger_local_state_change();
      }

      if (sigs & SERVREG_TEST_SIG_LOCAL_LIST_MASK) 
      {
         /* Clear the timer expiry signal */
         SR_TEST_SIGNAL_CLEAR( SERVREG_TEST_SIG_LOCAL_LIST_MASK);

         sr_new_state = servreg_get_service_curr_state(sr_mon_local_list_handle);

         transaction_id = servreg_get_transaction_id(sr_mon_local_list_handle);
         ret = servreg_set_ack(sr_mon_local_list_handle, transaction_id);

         if(SERVREG_SUCCESS != ret)
         {
            TMS_MSG_ERROR("servreg_set_ack() failed");
         }

         TMS_MSG_HIGH_1("Local state change notification received new state = 0x%x \n", sr_new_state);
      }

      if (sigs & SERVREG_TEST_SIG_REMOTE_QMI_LIST_MASK) 
      {
         /* Clear the timer expiry signal */
         SR_TEST_SIGNAL_CLEAR(SERVREG_TEST_SIG_REMOTE_QMI_LIST_MASK);

         sr_new_state = servreg_get_service_curr_state(sr_mon_remote_list_qmi_handle);

         transaction_id = servreg_get_transaction_id(sr_mon_remote_list_qmi_handle);
         ret = servreg_set_ack(sr_mon_remote_list_qmi_handle, transaction_id);

         if(SERVREG_SUCCESS != ret)
         {
            TMS_MSG_ERROR("servreg_set_ack() failed");
         }

         TMS_MSG_HIGH_1("Remote QMI state change notification received new state = 0x%x \n", sr_new_state);
      }
   }
} /* servreg_test_task */

