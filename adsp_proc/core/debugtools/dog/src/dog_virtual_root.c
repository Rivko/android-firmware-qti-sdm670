/*
#============================================================================
#  Name:
#    dog_virtual_root.c 
#
#  Description:
#    Virtual watchdog component in ROOT-PD
#
# Copyright (c) 2015,2017-2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
#============================================================================
*/

#include "stdarg.h"
#include "stdlib.h"
#include <stringl/stringl.h>
#include "comdef.h"             /* Definitions for byte, word, etc.     */
#include "err.h"
#include "qurt.h"
#include "tms_utils_msg.h"
#include "timer.h"
#include "tms_utils.h"
#include "pd_mon_qurt.h"
#include "dog_virtual.h"
#include "qurt_event.h"

/* Internal structure */
struct dog_qdi_internal_s
{
   qurt_mutex_t dog_qdi_mutex;
   qurt_qdi_obj_t dog_qdiobj;
   dog_qdi_device_p dog_qdi_pd_head;
   dog_virtual_grace_timer_data_p dog_virtual_grace_timer_head;
   qurt_anysignal_t anysignal;              /* Internal dog_hb client signal */
   dog_report_type dog_hb_vir_handle;
};

struct dog_qdi_internal_s dog_qdi_internal;

/* If this flag is set to TRUE, virtual wdog monitoring of user_pd state will be disabled
*  This flag should be set to TRUE only for DEBUG purposes eg TRACE32 Instrument to Disable hw wdog At Runtime
*/
static volatile boolean dog_virtual_disable = FALSE;

/* If this flag is set to TRUE, wdog pd grace timer will not be started
*  This flag should be set to TRUE only for DEBUG purposes eg TRACE32 Instrument to Disable hw wdog At Runtime
*/
static volatile boolean dog_disable_pd_grace_timer = FALSE;

/* Timer for dog grace timer for user-pd */
static void dog_virtual_grace_failureCb(timer_cb_data_type);
static void dog_virtual_start_grace_timer(int client_handle, void *data_ptr);
static void dog_virtual_stop_grace_timer(int asid);

static void dog_qdi_release(qurt_qdi_obj_t* qdiobj);
static int dog_qdi_open(int client_handle, char* name, uint32_t name_len);
static int dog_qdi_close(dog_qdi_device_p clientobj);
static int dog_qdi_hb_register(dog_qdi_device_p clientobj);
static int dog_qdi_worker_wait(int client_handle, dog_qdi_device_p clientobj, uint32_t * remote_mask);
static int dog_qdi_pong(dog_qdi_device_p clientobj);

extern PD_MON_HANDLE pd_mon_get_mgmt_tlskey(void);

/** =====================================================================
* Function:
*     dog_virtual_grace_failureCb
*
* Description:
*      Timer driven function called when grace timer for the user-pd expires
*
* Parameters:
*     pn_data : Information from qurt about the user-pd
*
* Returns:
*     none
* =====================================================================  */
static void dog_virtual_grace_failureCb(timer_cb_data_type pn_data)
{
   qurt_sysenv_procname_t *pn;
   char msg[TMS_UTILS_BUFFER_SIZE];
   
   /* CPU & DDR unvote for vote made in dog_virtual_start_grace_timer() */
   tms_utils_unvote_cpu_ddr();
   
   pn = (qurt_sysenv_procname_t *)pn_data;
   (void)tms_utils_fmt(msg, TMS_UTILS_BUFFER_SIZE, "User-PD grace timer expired for %s (ASID: %d)", pn->name, pn->asid);
   
   ERR_FATAL_AUX_MSG("User-PD grace timer expired, check coredump.err.aux_msg", msg, TMS_UTILS_BUFFER_SIZE);
}

/** =====================================================================
* Function:
*     dog_virtual_start_grace_timer
*
* Description:
*      OnSpawn Function to start the grace timer for the user-pd
*
* Parameters:
*     none
*
* Returns:
*     none
* =====================================================================  */
static void dog_virtual_start_grace_timer(int client_handle, void *data_ptr)
{
   qurt_sysenv_procname_t pn = {0};
   PD_MON_HANDLE dog_pd_mon_handle = 0;
   dog_virtual_grace_timer_data_p dog_virtual_grace_timer_curr = NULL;
   timer_error_type t_err;
   dog_virtual_grace_timer_data_p dog_vir_grace_curr = NULL;

   /* Obtain client info */
   if (0 > qurt_qdi_handle_invoke(client_handle, QDI_OS_SYSENV, QDI_HANDLE_LOCAL_CLIENT, QURT_SYSENV_PROCNAME_TYPE, &pn))
   {
     TMS_MSG_ERROR("Could not obtain qdi client info. Grace timer not started.");
     return;
   }
   
   qurt_pimutex_lock(&dog_qdi_internal.dog_qdi_mutex);

   dog_pd_mon_handle = pd_mon_get_mgmt_tlskey();

   /* Check if the PD is spawed by TMS owned pd_monitor */
   if(0 < (unsigned int)dog_pd_mon_handle)
   {
      /* Vote for CPU & DDR during PD bootup start. This will unvote in 
         dog_qdi_open() or in case PD bootup stuck: unvote in 
         dog_virtual_grace_failureCb(). */
      tms_utils_vote_cpu_ddr();

      /*check if there is already a grace timer created for this PD*/
      dog_vir_grace_curr = dog_qdi_internal.dog_virtual_grace_timer_head;
      while(NULL != dog_vir_grace_curr)
      {
         if(0==strncmp(dog_vir_grace_curr->pn.name,pn.name, QURT_MAX_NAME_LEN))
         {
              qurt_pimutex_unlock(&dog_qdi_internal.dog_qdi_mutex);
              return; /* since already a grace timer created for it */
         }
         dog_vir_grace_curr = dog_vir_grace_curr->next;
      }
         
      dog_virtual_grace_timer_curr = (dog_virtual_grace_timer_data_p)malloc(sizeof(dog_virtual_grace_timer_data_t));

      if(NULL != dog_virtual_grace_timer_curr)
      {
         dog_virtual_grace_timer_curr->pn.asid = pn.asid;
         strlcpy(dog_virtual_grace_timer_curr->pn.name, pn.name, QURT_MAX_NAME_LEN);

         /* Insert to the head of the list */
         dog_virtual_grace_timer_curr->next = dog_qdi_internal.dog_virtual_grace_timer_head;
         dog_qdi_internal.dog_virtual_grace_timer_head = dog_virtual_grace_timer_curr;

         /* Define watchdog timer */
         if(TE_SUCCESS != (t_err = timer_def_osal(&dog_virtual_grace_timer_curr->dog_pd_grace_timer, 
                                        &timer_non_defer_group, 
                                        TIMER_FUNC1_CB_TYPE, 
                                        dog_virtual_grace_failureCb, (timer_cb_data_type)(&(dog_virtual_grace_timer_curr->pn) ))))
         {
            ERR_FATAL("Dog startup grace Timer def failed", t_err, 0, 0);
         }

         /* Set watchdog timer */
         if(TE_SUCCESS != (t_err = timer_set_64(&dog_virtual_grace_timer_curr->dog_pd_grace_timer, 
                                      DOG_STARTUP_GRACE_TIME_SEC, 
                                      0, T_SEC )))
         {
            ERR_FATAL("Dog startup grace Timer set failed", t_err, 0, 0);
         }

         dog_virtual_grace_timer_curr->dog_pd_grace_timer_set = TRUE;

         TMS_MSG_SPRINTF_HIGH_2("Started grace timer for %s (ASID: %u)", pn.name, pn.asid);
      }
      else
      {
         TMS_MSG_ERROR("Malloc failed");
      }
   }
   else
   {
      TMS_MSG_SPRINTF_HIGH_2("User PD %s with asid: %u not started by TMS-owned PD_MON", pn.name, pn.asid);
   }

   qurt_pimutex_unlock(&dog_qdi_internal.dog_qdi_mutex);
   return;

}

/** =====================================================================
* Function:
*     dog_virtual_stop_grace_timer
*
* Description:
*      Stops the user-pd grace timer if started.
*      No need of locks as called internally in dog_qdi_open only.
*
* Parameters:
*     asid : asid of the user-pd whose grace timer need to be stopped
*
* Returns:
*     none
* =====================================================================  */
static void dog_virtual_stop_grace_timer(int asid)
{
   dog_virtual_grace_timer_data_p dog_vir_grace_curr = NULL, dog_vir_grace_prev = NULL;

   dog_vir_grace_curr = dog_qdi_internal.dog_virtual_grace_timer_head;
   while(NULL != dog_vir_grace_curr)
   {
      if(asid == dog_vir_grace_curr->pn.asid)
      {
         if (dog_vir_grace_curr->dog_pd_grace_timer_set == TRUE)
         {
            (void)timer_clr_64(&(dog_vir_grace_curr->dog_pd_grace_timer), T_SEC);
            (void)timer_undef(&(dog_vir_grace_curr->dog_pd_grace_timer));
            dog_vir_grace_curr->dog_pd_grace_timer_set = FALSE;
            TMS_MSG_SPRINTF_HIGH_2("Stopped grace timer for %s (ASID: %u)", dog_vir_grace_curr->pn.name, dog_vir_grace_curr->pn.asid);
         }

         if(NULL == dog_vir_grace_prev)
         {
            dog_qdi_internal.dog_virtual_grace_timer_head = dog_vir_grace_curr->next;
         }
         else
         {
            dog_vir_grace_prev->next = dog_vir_grace_curr->next;
         }

         free(dog_vir_grace_curr);
         break;
      }

      dog_vir_grace_prev = dog_vir_grace_curr;
      dog_vir_grace_curr = dog_vir_grace_curr->next;
   }
   return;
}

/** =====================================================================
 * Function:
 *     dog_qdi_release
 *
 * Description: 
 *     Deallocates the specified device handle.
 *
 * Parameters:
 *    qdiobj : Pointer to opener object for the device
 *
 * Returns:
 *    None
 * =====================================================================  */
static void dog_qdi_release(qurt_qdi_obj_t* qdiobj)
{
   dog_qdi_device_p clientobj_del = (dog_qdi_device_p)qdiobj;
   dog_qdi_device_p clientobj_curr = NULL, clientobj_prev = NULL;

   qurt_pimutex_lock(&dog_qdi_internal.dog_qdi_mutex);

   clientobj_curr = dog_qdi_internal.dog_qdi_pd_head;

   while(NULL != clientobj_curr)
   {
      if(clientobj_curr == clientobj_del)
      {
         if(NULL == clientobj_prev)
         {
            dog_qdi_internal.dog_qdi_pd_head = clientobj_curr->next;
         }
         else
         {
            clientobj_prev->next = clientobj_curr->next;
         }

         free(clientobj_del);

         break;
      }

      clientobj_prev = clientobj_curr;
      clientobj_curr = clientobj_curr->next;  
   } /* while() */

   qurt_pimutex_unlock(&dog_qdi_internal.dog_qdi_mutex);

   return;
}

/** =====================================================================
 * Function:
 *     dog_qdi_open
 *
 * Description: 
 *     Opens a new qdi driver and obtains a specified handle
 *
 * Parameters:
 *     client_handle  : QDI handle which represents the client
 *                      which made this QDI request.
 *     name           : User pd name that will register with root dog
 *     len            : name length
 *
 * Returns:
 *     DOG_VIRTUAL_SUCCESS (0) or DOG_VIRTUAL_FAILURE(-1)
 * =====================================================================  */
static int dog_qdi_open(int client_handle, char* name, uint32_t name_len)
{
   dog_qdi_device_p clientobj;
   dog_qdi_device_data_p dataobj;
   qurt_sysenv_procname_t pn = {0};
   uint32_t pd_name_len = name_len;
   int ret = -1, ret_handle = -1;

   /* CPU & DDR unvote for PD bootup end, 
   vote made in dog_virtual_start_grace_timer() */
   tms_utils_unvote_cpu_ddr();

   qurt_pimutex_lock(&dog_qdi_internal.dog_qdi_mutex);

   if (0 > (ret = qurt_qdi_handle_invoke(client_handle, QDI_OS_SYSENV, QDI_HANDLE_LOCAL_CLIENT, QURT_SYSENV_PROCNAME_TYPE, &pn)))
   {
      TMS_MSG_ERROR_2("Cannot obtain qdi client info %d %d", pn.asid, ret);
      qurt_pimutex_unlock(&dog_qdi_internal.dog_qdi_mutex);
      return ret;
   }

   clientobj = (dog_qdi_device_p)malloc(sizeof(dog_qdi_device_t));

   if(NULL == clientobj)
   {
      ERR_FATAL("DOG_QDI: malloc failed", 0, 0, 0);
   }

   clientobj->qdiobj.invoke = dog_qdi_invoke;
   clientobj->qdiobj.refcnt = QDI_REFCNT_INIT;
   clientobj->qdiobj.release = dog_qdi_release;

   dataobj = &(clientobj->dog_user_pd_data);
   dataobj->pn = pn;
   qurt_anysignal_init(&dataobj->sig_t);

   /* Insert to the head of the user-pd list */
   clientobj->next = dog_qdi_internal.dog_qdi_pd_head;
   dog_qdi_internal.dog_qdi_pd_head = clientobj;

   if(dog_disable_pd_grace_timer != TRUE)
   {
      /* User-Pd has successfully established a connection within 40sec */
      /* Clear the grace timer if set*/
      dog_virtual_stop_grace_timer(pn.asid);
   }

   ret_handle = qurt_qdi_handle_create_from_obj_t(client_handle, &clientobj->qdiobj);

   if(-1 != ret_handle)
   {
      /* Get the service name from user space */
      ret = qurt_qdi_copy_from_user(client_handle, dataobj->name, name, pd_name_len);

      if(-1 != ret)
      {
         TMS_MSG_HIGH("User PD VDOG recorded with Root PD");
      }
      else
      {
         ERR_FATAL("qurt_qdi_copy_from_user() failed", ret, 0, 0);
      }
   }
   else
   {
      ERR_FATAL("qurt_qdi_handle_create_from_obj_t() failed", ret_handle, 0, 0);
   }

   qurt_pimutex_unlock(&dog_qdi_internal.dog_qdi_mutex);

   /* Need to return back the handle created once the pd is set as UP*/
   return ret_handle;
}

/** =====================================================================
 * Function:
 *     dog_qdi_close
 *
 * Description: 
 *     Closes the specified driver, releasing any resources associated with the open driver.
 *
 * Parameters:
 *     clientobj      : QDI object which represents the client
 *
 * Returns:
 *     DOG_VIRTUAL_SUCCESS (0) or DOG_VIRTUAL_FAILURE(-1)
 * =====================================================================  */
static int dog_qdi_close(dog_qdi_device_p clientobj)
{
   dog_qdi_device_data_p dataobj;

   if(dog_virtual_disable != TRUE)
   {
      dataobj = &(clientobj->dog_user_pd_data);

      if ( dataobj->dog_vir_hb_handle != NULL )
      {
        dog_hb_deregister(dataobj->dog_vir_hb_handle);
        dataobj->dog_vir_hb_handle = NULL;
      }

      TMS_MSG_LOW("User PD virtual dog de-registered with Root PD HB");
   }

   return DOG_VIRTUAL_SUCCESS;
}

/** =====================================================================
 * Function:
 *     dog_qdi_hb_register
 *
 * Description: 
 *     User PD register with root hb dog
 *
 * Parameters:
 *     clientobj      : QDI object which represents the client
 *                      which made this QDI request.
 *
 * Returns:
 *     DOG_VIRTUAL_SUCCESS (0) or DOG_VIRTUAL_FAILURE(-1)
 * =====================================================================  */
static int dog_qdi_hb_register(dog_qdi_device_p clientobj)
{
   dog_qdi_device_data_p dataobj;

   if(dog_virtual_disable != TRUE)
   {
      dataobj = &(clientobj->dog_user_pd_data);

      dataobj->dog_vir_hb_handle = dog_hb_register_qurt(&dataobj->sig_t, DOG_VIR_SIGNAL_MASK);

      if(dataobj->dog_vir_hb_handle == 0)
      {
         ERR_FATAL("dog_hb_register_qurt failed", dataobj->dog_vir_hb_handle, 0, 0);
      }

      TMS_MSG_LOW("User PD virtual dog registered with Root PD HB");
   }

   return DOG_VIRTUAL_SUCCESS;
}

/** =====================================================================
 * Function:
 *     dog_qdi_worker_wait
 *
 * Description: 
 *     User PD waits to get the dog hb root signal
 *
 * Parameters:
 *     clientobj      : QDI object which represents the client
 *                      which made this QDI request.
 *
 * Returns:
 *     DOG_VIRTUAL_SUCCESS (0) or DOG_VIRTUAL_FAILURE(-1)
 * =====================================================================  */
static int dog_qdi_worker_wait(int client_handle, dog_qdi_device_p clientobj, uint32_t * remote_mask)
{
   int ret = DOG_VIRTUAL_FAILURE;
   dog_qdi_device_data_p dataobj;
   unsigned int return_mask = 0;

   dataobj = &(clientobj->dog_user_pd_data);

   /* Wait till you get notified of any service state change */
   if ( QURT_ECANCEL == qurt_signal_wait_cancellable(&dataobj->sig_t, DOG_VIR_SIGNAL_MASK, QURT_SIGNAL_ATTR_WAIT_ANY, (unsigned int *)&return_mask) )
   {
      dog_qdi_close(clientobj);
   }

   /* Clear the bit immediately so that we don't miss consecutive signal set */
   qurt_anysignal_clear(&dataobj->sig_t, return_mask);

   ret = qurt_qdi_copy_to_user(client_handle, remote_mask, &return_mask, sizeof(uint32_t));

   if(-1 == ret)
   {
      TMS_MSG_ERROR("qurt_qdi_copy_to_user failed");
   }

   return DOG_VIRTUAL_SUCCESS;
}

/** =====================================================================
 * Function:
 *     dog_qdi_pong
 *
 * Description: 
 *     User PD reports back (PONGS) to the virtual watchdog on Root-PD
 *
 * Parameters:
 *     clientobj      : QDI object which represents the client
 *                      which made this QDI request.
 *
 * Returns:
 *     DOG_VIRTUAL_SUCCESS (0) or DOG_VIRTUAL_FAILURE(-1)
 * =====================================================================  */
static int dog_qdi_pong(dog_qdi_device_p clientobj)
{
   dog_qdi_device_data_p dataobj;

   if(dog_virtual_disable != TRUE)
   {
      dataobj = &(clientobj->dog_user_pd_data);

      dog_hb_report(dataobj->dog_vir_hb_handle);

      TMS_MSG_LOW("User PD virtual dog pinged Root PD");
   }

   return DOG_VIRTUAL_SUCCESS;
}

/** =====================================================================
 * Function:
 *     dog_qdi_invoke
 *
 * Description: 
 *     This gives the canonical form for the arguments to a QDI
 *     driver invocation function.
 * 
 * Parameters:
 *     client_handle  : QDI handle which represents the client
 *                      which made this QDI request.  
 *
 *     qurt_qdi_obj_t : Points at the qdi_object_t structure
 *                      on which this QDI request is being made.
 *
 *     int method     : The integer QDI method which represents
 *                      the request type.
 *
 *     qurt_qdi_arg_t a1 to a3 :  The first three general purpose arguments
 *                                to the invocation function are passed in
 *                                these slots.
 *
 *     qurt_qdi_arg_t a4 to a9 :   Arguments beyond the first three are
 *                                 passed on the stack.
 * Returns:
 *     -1 for failure and 0 for success
 * =====================================================================  */
int dog_qdi_invoke(int client_handle,
                       qurt_qdi_obj_t* obj,
                       int dog_qdi_method,
                       qurt_qdi_arg_t a1,
                       qurt_qdi_arg_t a2,
                       qurt_qdi_arg_t a3,
                       qurt_qdi_arg_t a4,
                       qurt_qdi_arg_t a5,
                       qurt_qdi_arg_t a6,
                       qurt_qdi_arg_t a7,
                       qurt_qdi_arg_t a8,
                       qurt_qdi_arg_t a9
                      )
{
   int ret = -1;
   dog_qdi_device_p clientobj = (dog_qdi_device_p)obj;

   /* Driver Methods */
   switch (dog_qdi_method)
   {
      /* API, dog_qdi_client_init */
      case QDI_OPEN:
      {
         ret = dog_qdi_open(client_handle, a2.ptr, a3.num);
         break;
      }

      /* API, dog_qdi_invoke_close */
      case QDI_CLOSE:
      {
         if(0 != dog_qdi_close(clientobj))
         {
            TMS_SHUTDOWN_MSG_ERROR("DOG QDI close failed");
         }

         ret = qurt_qdi_method_default(client_handle, obj, dog_qdi_method, a1, a2, a3, a4, a5, a6, a7, a8, a9);
         TMS_SHUTDOWN_MSG_HIGH_2("QDI DOG close %d: %d", qurt_thread_get_id(), ret);
         break;
      }

      /* API, dog_qdi_invoke_hb_register */
      case DOG_QDI_HB_REGISTER:
      {
         ret = dog_qdi_hb_register(clientobj);
         break;
      }

      /* API, dog_qdi_invoke_worker_wait */
      case DOG_QDI_WAIT:
      {
         ret = dog_qdi_worker_wait(client_handle, clientobj, a1.ptr);
         break;
      }

      /* API, dog_qdi_invoke_pong */
      case DOG_QDI_PONG:
      {
         ret = dog_qdi_pong(clientobj);
         break;
      }

      /* Default Method Handler */
      default:
      {
         ret = qurt_qdi_method_default(client_handle, obj, dog_qdi_method, a1, a2, a3, a4, a5, a6, a7, a8, a9);
         break; 
      }
   } /* switch() */

   return ret;
}

/** =====================================================================
 * Function:
 *     dog_qdi_init
 *
 * Description:
 *     Initialization function. Registers a QDI device with the generic QDI object
 *
 * Parameters:
 *     None
 * Returns:
 *     None
 * =====================================================================  */
void dog_qdi_init(void)
{
   int ret = -1;
   qurt_qdi_obj_t *opener = &dog_qdi_internal.dog_qdiobj;

   qurt_pimutex_init(&dog_qdi_internal.dog_qdi_mutex);

   qurt_pimutex_lock(&dog_qdi_internal.dog_qdi_mutex);

   opener->invoke = dog_qdi_invoke;
   opener->refcnt = QDI_REFCNT_INIT;
   opener->release = dog_qdi_release;

   /* Register the Driver */
   ret = qurt_qdi_devname_register(DOG_QDI_DEVICE, opener);
   if(ret != 0)
   {
      ERR_FATAL("qurt_qdi_devname_register failed", ret, 0, 0);
   }

   qurt_pimutex_unlock(&dog_qdi_internal.dog_qdi_mutex);

   if(dog_disable_pd_grace_timer != TRUE)
   {
      /* Register a cb function on spawn of a user pd to monitor the grace timer */
      if( 0 > qurt_callback_register_onspawn(dog_virtual_start_grace_timer, NULL))
      {
         ERR_FATAL("Virtual dog Could not register onspawn callback with QURT",0,0,0);
      }
   }

   return;
}
