/*
#============================================================================
#  Name:
#    servreg_qdi.c 
#
#  Description:
#    Implements QDI layer for Service registry that goes to the root image
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
#include "tms_utils.h"
#include "pd_mon_qurt.h"

#include "servreg_internal.h"
#include "servreg_locator.h"
#include "servreg_localdb.h"
#include "servreg_monitor.h"
#include "servreg_monitor_qurt.h"
#include "servreg_notifier.h"
#include "servreg_qdi_notifier_root.h"
#include "servreg_qdi.h"        /* Include for root PD only */

#define SERVREG_QDI_WAIT_ROOT_MASK_VALUE 0x1

/* Servreg QDI Data associated with each user - PD */
struct servnotif_qdi_device_data_s
{
   uint32_t pid;
   SERVREG_MON_HANDLE sr_mon_handle;                                      /* sr_mon_handle that represents the PD itself i.e soc/domain/subdomain */
   qurt_anysignal_t sig_t;

   /* Head node of the Service Registry Notifier node linked list for this user pd device */
   SERVREG_QDI_NOTIF_HANDLE servreg_qdi_notif_node_list_head;
   SERVREG_QDI_NOTIF_HANDLE servreg_qdi_notif_node_register_head;
};
typedef struct servnotif_qdi_device_data_s servnotif_qdi_device_data_t, * servnotif_qdi_device_data_p;

/* Servreg User PD QDI structure */
struct servnotif_qdi_device_s
{
   qurt_qdi_obj_t qdiobj;                                                  /* Required to be first */
   servnotif_qdi_device_data_t servreg_user_pd_data;
   struct servnotif_qdi_device_s *next;
};
typedef struct servnotif_qdi_device_s servnotif_qdi_device_t, * servnotif_qdi_device_p;

/* Internal QDI root Stucture */
struct servreg_qdi_root_internal_s
{
   qurt_mutex_t servnotif_qdi_mutex;
   qurt_qdi_obj_t servnotif_qdiobj;
   servnotif_qdi_device_p servreg_qdi_pd_head;
};
struct servreg_qdi_root_internal_s servreg_qdi_root_internal;

/* Servreg User service level clients associated with each user - PD */
struct servnotif_cache_pd_data_s
{
   SERVREG_NAME sr_name;                                      /* PD name */
   SERVREG_QDI_NOTIF_HANDLE servreg_qdi_notif_node_register_head;
   struct servnotif_cache_pd_data_s *next;
};
typedef struct servnotif_cache_pd_data_s servnotif_cache_pd_data_t, * servnotif_cache_pd_data_p;

servnotif_cache_pd_data_p servnotif_cache_pd_data_head;

/* Static functions in this file */
static void servreg_qdi_release(qurt_qdi_obj_t* qdiobj);
static int servreg_qdi_open(int client_handle, SERVREG_NAME name, uint32_t name_len);
static int servreg_qdi_close(servnotif_qdi_device_p clientobj);

/* User registrants in root */
static int servreg_qdi_worker_wait(int client_handle, servnotif_qdi_device_p clientobj, uint32_t * remote_mask);
static int servreg_qdi_register_remote_listener(int client_handle, servnotif_qdi_device_p clientobj, SERVREG_NAME name, uint32_t name_len, uint32_t *remote_addr, uint32_t *remote_state, int32_t * remote_transaction_id);
static int servreg_qdi_deregister_remote_listener(int client_handle, servnotif_qdi_device_p clientobj, SERVREG_NAME name, uint32_t name_len);
static int servreg_qdi_free_remote_handle(int client_handle, servnotif_qdi_device_p clientobj, SERVREG_NAME name, uint32_t name_len);
static int servreg_qdi_set_remote_ack(int client_handle, servnotif_qdi_device_p clientobj, SERVREG_NAME name, uint32_t name_len, int32_t transaction_id);
static int servreg_qdi_get_remote_handle(int client_handle, servnotif_qdi_device_p clientobj, uint32_t * remote_addr, uint32_t *remote_state, int32_t *transaction_id);
static int servreg_qdi_create_qmi_entry(int client_handle, servnotif_qdi_device_p clientobj, SERVREG_NAME name, uint32_t name_len, uint32_t qmi_instance_id);

/* Root registrants in user*/
static int servreg_qdi_register_handle(int client_handle, servnotif_qdi_device_p clientobj, uint32_t * remote_addr, SERVREG_NAME name);
static int servreg_qdi_set_state(int client_handle, servnotif_qdi_device_p clientobj, uint32_t remote_addr, uint32_t state);
static int servreg_qdi_update_handle(int client_handle, servnotif_qdi_device_p clientobj, uint32_t state, int32_t transaction_id, uint32_t remote_addr);
static int servreg_qdi_register_root_handles(int client_handle, servnotif_qdi_device_p clientobj, uint32_t * remote_addr, SERVREG_NAME name);
static int servreg_qdi_get_ack_handle(int client_handle, servnotif_qdi_device_p clientobj, SERVREG_NAME name, int32_t *transaction_id);

extern void servreg_record_pd_entry(SERVREG_NAME sr_pd_name);
extern void servreg_delete_pd_entry(SERVREG_NAME sr_pd_name);
extern PD_MON_STATUS pd_mon_set_servreg_handle(int client_asid, SERVREG_NAME servreg_n);

/** =====================================================================
 * Function:
 *     servreg_qdi_release
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
static void servreg_qdi_release(qurt_qdi_obj_t* qdiobj)
{
   servnotif_qdi_device_p clientobj_del = (servnotif_qdi_device_t *)qdiobj;
   servnotif_qdi_device_p clientobj_curr = SERVREG_NULL, clientobj_prev = SERVREG_NULL;
   servnotif_qdi_device_data_t *dataobj;
   SERVREG_NAME pd_name = SERVREG_NULL;
   SERVREG_RESULT ret = SERVREG_FAILURE;
   servnotif_cache_pd_data_p servnotif_cache_pd_data_node = SERVREG_NULL;

   /* Serialize driver access when deleting the client node */
   qurt_pimutex_lock(&servreg_qdi_root_internal.servnotif_qdi_mutex);

   dataobj = &(clientobj_del->servreg_user_pd_data);

   /* Delete the PD_name entry in the servreg_pd_list_head that is maintained in the root-pd */
   pd_name = servreg_get_service_name(dataobj->sr_mon_handle);

   if(SERVREG_NULL != pd_name)
   {
      servreg_delete_pd_entry(pd_name);
   }

   ret = servreg_delete_qdi_notif_list(&dataobj->servreg_qdi_notif_node_list_head, &dataobj->sig_t, SERVREG_QDI_REG_USER_MASK_VALUE);

   if(SERVREG_SUCCESS == ret)
   {
      qurt_anysignal_destroy(&dataobj->sig_t);
      dataobj->servreg_qdi_notif_node_list_head = SERVREG_NULL;

      servnotif_cache_pd_data_node = (servnotif_cache_pd_data_p)servreg_calloc(sizeof(servnotif_cache_pd_data_t), 1);
      if(SERVREG_NULL != servnotif_cache_pd_data_node)
      {
         servnotif_cache_pd_data_node->sr_name = servreg_get_pd_name(dataobj->sr_mon_handle);
         servnotif_cache_pd_data_node->servreg_qdi_notif_node_register_head = dataobj->servreg_qdi_notif_node_register_head;
         servnotif_cache_pd_data_node->next = servnotif_cache_pd_data_head;
         servnotif_cache_pd_data_head = servnotif_cache_pd_data_node;
      }

      dataobj->servreg_qdi_notif_node_register_head = SERVREG_NULL;

      clientobj_curr = servreg_qdi_root_internal.servreg_qdi_pd_head;
      while(SERVREG_NULL != clientobj_curr)
      {
         if(clientobj_curr == clientobj_del)
         {
            if(SERVREG_NULL == clientobj_prev)
            {
               servreg_qdi_root_internal.servreg_qdi_pd_head = clientobj_curr->next;
            }
            else
            {
               clientobj_prev->next = clientobj_curr->next;
            }

            servreg_free(clientobj_del);

            break;
         }
         else
         {
            clientobj_prev = clientobj_curr;
            clientobj_curr = clientobj_curr->next;  
         }
      } /* while() */
   }
   else
   {
      TMS_MSG_ERROR("Delete QDI notif list failed");
   }

   qurt_pimutex_unlock(&servreg_qdi_root_internal.servnotif_qdi_mutex);
   return;
}

/** =====================================================================
 * Function:
 *     servreg_qdi_open
 *
 * Description: 
 *     Opens a new qdi driver and obtains a specified handle
 *
 * Parameters:
 *     client_handle  : QDI handle which represents the client
 *                      which made this QDI request.
 *     name           : User pd name that will register with root srm as SERVICE_STATE_UP
 *     len            : name length
 *
 * Returns:
 *     SERVREG_SUCCESS (0) or SERVREG_FAILURE(-1)
 * =====================================================================  */
static int servreg_qdi_open(int client_handle, SERVREG_NAME name, uint32_t name_len)
{
   servnotif_qdi_device_p clientobj;
   servnotif_qdi_device_data_p dataobj;
   qurt_sysenv_procname_t pn;
   uint32_t sr_len = name_len;
   SERVREG_NAME domain = SERVREG_NULL;
   int ret = -1, ret_handle = -1;

   if (0 > (ret = qurt_qdi_handle_invoke(client_handle, QDI_OS_SYSENV, QDI_HANDLE_LOCAL_CLIENT, QURT_SYSENV_PROCNAME_TYPE, &pn)))
   {
      TMS_MSG_HIGH_2("Cannot obtain qdi client info %d %d", pn.asid, ret);
      return ret;
   }

   clientobj = (servnotif_qdi_device_p)servreg_calloc(sizeof(servnotif_qdi_device_t), 1);

   if(NULL == clientobj)
   {
      ERR_FATAL("SERVREG_QDI: calloc failed", 0, 0, 0);
   }

   clientobj->qdiobj.invoke = servreg_qdi_invoke;
   clientobj->qdiobj.refcnt = QDI_REFCNT_INIT;
   clientobj->qdiobj.release = servreg_qdi_release;

   dataobj = &(clientobj->servreg_user_pd_data);
   dataobj->pid = pn.asid;
   qurt_anysignal_init(&dataobj->sig_t);

   dataobj->servreg_qdi_notif_node_list_head = SERVREG_NULL;
   dataobj->servreg_qdi_notif_node_register_head = SERVREG_NULL;

   ret_handle = qurt_qdi_handle_create_from_obj_t(client_handle, &clientobj->qdiobj);

   /* Register user pd as service up */
   if(-1 != ret_handle)
   {
      // this pointer is stored in servreg_record_pd_entry and free'd in servreg_delete_pd_entry
      domain = servreg_calloc(sr_len, 1);

      if (SERVREG_NULL != domain)
      {
         /* Get the service name from user space */
         ret = qurt_qdi_copy_from_user(client_handle, domain, name, sr_len);

         if(-1 != ret)
         {
            dataobj->sr_mon_handle = servreg_alloc_monitor_handle(domain, SERVREG_NULL);

            if(SERVREG_NULL != dataobj->sr_mon_handle)
            {
               /* Insert to the head of the user-pd list */
               qurt_pimutex_lock(&servreg_qdi_root_internal.servnotif_qdi_mutex);
               clientobj->next = servreg_qdi_root_internal.servreg_qdi_pd_head;
               servreg_qdi_root_internal.servreg_qdi_pd_head = clientobj;
               qurt_pimutex_unlock(&servreg_qdi_root_internal.servnotif_qdi_mutex);

               /* Always set the pd service itself as synchronous */
               if(SERVREG_SUCCESS != servreg_set_synchronous(dataobj->sr_mon_handle))
               {
                  ERR_FATAL("SERVREG_QDI: servreg_set_synchronous() failed", 0, 0, 0);
               }

               if(SERVREG_SUCCESS == servreg_set_state(dataobj->sr_mon_handle, SERVREG_SERVICE_STATE_UP))
               {
                  TMS_MSG_SPRINTF_HIGH_1("User PD %s recorded in Root-PD as UP", domain);

                  /* Record the user-pd information in the servreg_pd_list_head */
                  servreg_record_pd_entry(domain);

                  /* Register the servreg user-pd handle with pd_mon */
                  if(PD_MON_STATUS_ERROR == pd_mon_set_servreg_handle(dataobj->pid, domain))
                  {
                     ERR_FATAL("SERVREG_QDI: pd_mon_set_servreg_handle() failed", 0, 0, 0);
                  }
               }
               else
               {
                  ERR_FATAL("SERVREG_QDI: servreg_set_state() failed", 0, 0, 0);
               }
            }
            else
            {
               ERR_FATAL("SERVREG_QDI: dataobj->sr_mon_handle NULL", 0, 0, 0);
            }
         }
         else
         {
            ERR_FATAL("SERVREG_QDI: qurt_qdi_copy_from_user() failed ", 0, 0, 0);
         }

      }
      else
      {
         ERR_FATAL("SERVREG_QDI: calloc failed ", 0, 0, 0);
      }
   }
   else
   {
      ERR_FATAL("SERVREG_QDI: qurt_qdi_handle_create_from_obj_t() failed ", 0, 0, 0);
   }

   /* Need to return back the handle created once the pd is set as UP*/
   return ret_handle;
}

/** =====================================================================
 * Function:
 *     servreg_qdi_close
 *
 * Description: 
 *     Closes the specified driver, releasing any resources associated with the open driver.
 *
 * Parameters:
 *     clientobj      : QDI object which represents the client
 *                      which made this QDI request.
 *
 * Returns:
 *     SERVREG_SUCCESS (0) or SERVREG_FAILURE(-1)
 * =====================================================================  */
static int servreg_qdi_close(servnotif_qdi_device_p clientobj)
{
   int ret = -1;
   servnotif_qdi_device_data_p dataobj;
   SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle = SERVREG_NULL;
   SERVREG_MON_HANDLE sr_mon_handle = SERVREG_NULL;

   dataobj = &(clientobj->servreg_user_pd_data);

   if(SERVREG_SUCCESS == servreg_set_state(dataobj->sr_mon_handle, SERVREG_SERVICE_STATE_DOWN))
   {
      TMS_MSG_HIGH("User-PD state set to DOWN");

      ret = servreg_free_monitor_handle(dataobj->sr_mon_handle);
      if(SERVREG_SUCCESS != ret)
      {
         TMS_MSG_ERROR_1("Could not free monitor handle %d", ret);
      }
   }
   else
   {
      TMS_MSG_ERROR("Set state failed");
   }

   sr_qdi_notif_handle = dataobj->servreg_qdi_notif_node_register_head;
   while(SERVREG_NULL != sr_qdi_notif_handle)
   {
      sr_mon_handle = servreg_notif_get_mon_handle(sr_qdi_notif_handle);
      servreg_set_asynchronous(sr_mon_handle);

      if(SERVREG_SUCCESS != servreg_set_state(sr_mon_handle, SERVREG_SERVICE_STATE_DOWN))
      {
         TMS_MSG_ERROR("Set state failed");
      }

      /* update the head pointing to the next root client */
      sr_qdi_notif_handle = servreg_get_qdi_notif_next_node(sr_qdi_notif_handle);
   }

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_qdi_worker_wait
 *
 * Description: 
 *     User pd invokes this function that waits on a qurt signal to be set
 *
 * Parameters:
 *     client_handle  : QDI handle which represents the client
 *                      which made this QDI request.
 *     clientobj      : QDI object which represents the client
 *                      which made this QDI request.
 *     remote_mask    : returns the set mask to the user pd
 *
 * Returns:
 *     SERVREG_SUCCESS (0) or SERVREG_FAILURE(-1)
 * =====================================================================  */
static int servreg_qdi_worker_wait(int client_handle, servnotif_qdi_device_p clientobj, uint32_t * remote_mask)
{
   int ret = -1;
   servnotif_qdi_device_data_p dataobj;
   unsigned int return_mask = 0;

   dataobj = &(clientobj->servreg_user_pd_data);

   /* Wait till you get notified of any service state change */
   qurt_signal_wait_cancellable(&dataobj->sig_t, SERVREG_QDI_REG_USER_MASK_VALUE | SERVREG_QDI_REG_ROOT_MASK_VALUE | SERVREG_QDI_ACK_ROOT_MASK_VALUE, QURT_SIGNAL_ATTR_WAIT_ANY, (unsigned int *)&return_mask);

   /* Clear the bit immediately so that we don't miss consecutive signal set */
   qurt_anysignal_clear(&dataobj->sig_t, return_mask);

   ret = qurt_qdi_copy_to_user(client_handle, remote_mask, &return_mask, sizeof(uint32_t));

   if(-1 == ret)
   {
      TMS_MSG_ERROR("qurt_qdi_copy_to_user failed");
   }

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_qdi_register_remote_listener
 *
 * Description: 
 *     Register the user pd proxy listener in the root pd's pool
 *
 * Parameters:
 *     client_handle  : QDI handle which represents the client
 *                      which made this QDI request. 
 *     clientobj      : QDI object which represents the client
 *                      which made this QDI request.
 *     name           : service name that the user-pd is interested in
 *     name_len       : length of the name
 *     remote_address : send back the root pd's monitor handle address to the user pd
 *     remote_state   : send back the root pd's state associated with the monitor handle
 *
 * Returns:
 *     SERVREG_SUCCESS (0) or SERVREG_FAILURE(-1)
 * =====================================================================  */
static int servreg_qdi_register_remote_listener(int client_handle, servnotif_qdi_device_p clientobj, SERVREG_NAME name, uint32_t name_len, uint32_t *remote_addr, uint32_t *remote_state, int32_t *remote_transaction_id)
{
   SERVREG_RESULT result = SERVREG_FAILURE;
   int ret = -1;
   SERVREG_NAME service_name = SERVREG_NULL;
   uint32_t sr_len = name_len;
   SERVREG_MON_HANDLE sr_mon_handle = SERVREG_NULL;
   SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle = SERVREG_NULL;
   uint32_t addr = 0;
   int32_t transaction_id = 0;
   SERVREG_SERVICE_STATE state;
   servnotif_qdi_device_data_p dataobj;

   //qurt_pimutex_lock(&servreg_qdi_root_internal.servnotif_qdi_mutex);

   dataobj = &(clientobj->servreg_user_pd_data);

   SERVREG_TRACE_2(QDI_REG, dataobj, client_handle);

   service_name = servreg_calloc(sr_len, 1);
   if(SERVREG_NULL != service_name)
   {
      /* Get the service name from user space */
      ret = qurt_qdi_copy_from_user(client_handle, service_name, name, sr_len);
      if (-1 != ret)
      {
         /* Allocate or get the existing SERVREG HANDLE */
         sr_mon_handle = servreg_alloc_monitor_handle(service_name, SERVREG_NULL);
         if(SERVREG_NULL != sr_mon_handle)
         {
            result = servreg_register_listener_qurt(sr_mon_handle, &dataobj->sig_t, SERVREG_QDI_REG_USER_MASK_VALUE);
            if(SERVREG_SUCCESS == result)
            {
               /* Create a Service Notifier node to list the sr_mon_handle to get remote notifications from root pd */
               sr_qdi_notif_handle = servreg_create_qdi_notif_node(sr_mon_handle, &(dataobj->servreg_qdi_notif_node_list_head));
               if(SERVREG_NULL != sr_qdi_notif_handle)
               {
                  /* Send the address of the remote handle to user space */
                  addr = (uint32_t)sr_qdi_notif_handle;
                  ret = qurt_qdi_copy_to_user(client_handle, remote_addr, &addr, sizeof(uint32_t));
                  if(-1 != ret)
                  {
                     state = servreg_get_service_curr_state(sr_mon_handle);
                     ret = qurt_qdi_copy_to_user(client_handle, remote_state, &state, sizeof(uint32_t));
                     if(-1 != ret)
                     {
                        transaction_id = servreg_get_transaction_id(sr_mon_handle);
                        ret = qurt_qdi_copy_to_user(client_handle, remote_transaction_id, &transaction_id, sizeof(uint32_t));

                        if(-1 == ret)
                        {
                           SERVREG_TRACE_0(FAIL);
                           TMS_MSG_ERROR("qurt_qdi_copy_to_user failed");
                        }
                     }
                     else
                     {
                        SERVREG_TRACE_0(FAIL);
                        TMS_MSG_ERROR("qurt_qdi_copy_to_user failed");
                     }
                  }
                  else
                  {
                     SERVREG_TRACE_0(FAIL);
                     TMS_MSG_ERROR("qurt_qdi_copy_to_user failed");
                  }
               }
               else
               {
                  SERVREG_TRACE_0(FAIL);
                  TMS_MSG_ERROR("sr_qdi_notif_handle NULL");
                  ret = -1;
               }
            }
            else
            {
               SERVREG_TRACE_0(FAIL);
               TMS_MSG_ERROR("Could not register as a listener in the monitor pool");
               ret = -1;
            }
         }
         else
         {
            SERVREG_TRACE_0(FAIL);
            TMS_MSG_ERROR("sr_mon_handle NULL");
            ret = -1;
         }
      }
      else
      {
         SERVREG_TRACE_0(FAIL);
         TMS_MSG_ERROR("qurt_qdi_copy_from_user() failed");
      }

      servreg_free(service_name);
   }
   else
   {
      SERVREG_TRACE_0(FAIL);
      TMS_MSG_ERROR("Calloc failed");
   }

   //qurt_pimutex_unlock(&servreg_qdi_root_internal.servnotif_qdi_mutex);
   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_qdi_deregister_remote_listener
 *
 * Description: 
 *     De-register the user-pd proxy listener
 *
 * Parameters:
 *     client_handle  : QDI handle which represents the client
 *                      which made this QDI request.
 *     clientobj      : QDI object which represents the client
 *                      which made this QDI request.
 *     name           : service name that the user-pd is wants to de-register
 *     name_len       : length of the name
 *
 * Returns:
 *     SERVREG_SUCCESS (0) or SERVREG_FAILURE(-1)
 * =====================================================================  */
static int servreg_qdi_deregister_remote_listener(int client_handle, servnotif_qdi_device_p clientobj, SERVREG_NAME name, uint32_t name_len)
{
   SERVREG_RESULT result = SERVREG_FAILURE;
   int ret = -1;
   SERVREG_NAME service_name = SERVREG_NULL;
   uint32_t sr_len = name_len;
   SERVREG_MON_HANDLE sr_mon_handle = SERVREG_NULL;
   servnotif_qdi_device_data_t *dataobj;

   //qurt_pimutex_lock(&servreg_qdi_root_internal.servnotif_qdi_mutex);
   dataobj = &(clientobj->servreg_user_pd_data); 

   SERVREG_TRACE_2(QDI_DEREG, dataobj, client_handle);

   service_name = servreg_calloc(sr_len, 1);
   if(SERVREG_NULL != service_name)
   {
      /* Get the service name from user space */
      ret = qurt_qdi_copy_from_user(client_handle, service_name, name, sr_len);
      if (-1 != ret)
      {
         /* Get the existing SERVREG HANDLE */
         sr_mon_handle = servreg_get_sr_mon_handle(service_name);
         if(SERVREG_NULL != sr_mon_handle)
         {
            result = servreg_deregister_listener_qurt(sr_mon_handle, &dataobj->sig_t, SERVREG_QDI_REG_USER_MASK_VALUE);
            if(SERVREG_SUCCESS != result)
            {
               TMS_MSG_ERROR("Deregister listener failed");
               ret = -1;
            }
         }
         else
         {
            TMS_MSG_ERROR("sr_mon_handle NULL");
            ret = -1;
         }
      }
      else
      {
         TMS_MSG_ERROR("qurt_qdi_copy_from_user() failed");
      }

      servreg_free(service_name);
   }
   else
   {
      TMS_MSG_ERROR("Calloc failed");
   }

   //qurt_pimutex_unlock(&servreg_qdi_root_internal.servnotif_qdi_mutex);
   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_qdi_free_remote_handle
 *
 * Description: 
 *     Free the handle associated with user pd proxy listener
 *
 * Parameters:
 *     client_handle  : QDI handle which represents the client
 *                      which made this QDI request.
 *     clientobj      : QDI object which represents the client
 *                      which made this QDI request.
 *     name           : service name that the user-pd is wants to de-register
 *     name_len       : length of the name
 *
 * Returns:
 *     SERVREG_SUCCESS (0) or SERVREG_FAILURE(-1)
 * =====================================================================  */
static int servreg_qdi_free_remote_handle(int client_handle, servnotif_qdi_device_p clientobj, SERVREG_NAME name, uint32_t name_len)
{
   SERVREG_RESULT result = SERVREG_FAILURE;
   int ret = -1;
   SERVREG_NAME service_name = SERVREG_NULL;
   uint32_t sr_len = name_len;
   SERVREG_MON_HANDLE sr_mon_handle = SERVREG_NULL;
   //servnotif_qdi_device_data_t *dataobj;

   //qurt_pimutex_lock(&servreg_qdi_root_internal.servnotif_qdi_mutex);
   //dataobj = &(clientobj->servreg_user_pd_data); 

   service_name = servreg_calloc(sr_len, 1);
   if(SERVREG_NULL != service_name)
   {
      /* Get the service name from user space */
      ret = qurt_qdi_copy_from_user(client_handle, service_name, name, sr_len);
      if (-1 != ret)
      {
         /* Get the existing SERVREG HANDLE */
         sr_mon_handle = servreg_get_sr_mon_handle(service_name);
         if(SERVREG_NULL != sr_mon_handle)
         {
            /* Free the proxy monitor handle */
            result = servreg_free_monitor_handle(sr_mon_handle);

            if(SERVREG_SUCCESS != result)
            {
               TMS_MSG_ERROR("servreg_free_monitor_handle() failed");
               ret = -1;
            }
         }
         else
         {
            TMS_MSG_ERROR("sr_mon_handle NULL");
            ret = -1;
         }
      }
      else
      {
         TMS_MSG_ERROR("qurt_qdi_copy_from_user() failed");
      }

      servreg_free(service_name);
   }
   else
   {
      TMS_MSG_ERROR("Calloc failed");
   }

   //qurt_pimutex_unlock(&servreg_qdi_root_internal.servnotif_qdi_mutex);
   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_qdi_set_remote_ack
 *
 * Description: 
 *     Set the ack count for the user pd proxy listener
 *
 * Parameters:
 *     client_handle  : QDI handle which represents the client
 *                      which made this QDI request.
 *     clientobj      : QDI object which represents the client
 *                      which made this QDI request.
 *     name           : service name that the user-pd is wants to de-register
 *     name_len       : length of the name
 *     transaction_id : id form the user pd
 *
 * Returns:
 *     SERVREG_SUCCESS (0) or SERVREG_FAILURE(-1)
 * =====================================================================  */
static int servreg_qdi_set_remote_ack(int client_handle, servnotif_qdi_device_p clientobj, SERVREG_NAME name, uint32_t name_len, int32_t transaction_id)
{
   SERVREG_RESULT result = SERVREG_FAILURE;
   int ret = -1;
   SERVREG_NAME service_name = SERVREG_NULL;
   uint32_t sr_len = name_len;
   SERVREG_MON_HANDLE sr_mon_handle = SERVREG_NULL;
   //servnotif_qdi_device_data_t *dataobj;

   //qurt_pimutex_lock(&servreg_qdi_root_internal.servnotif_qdi_mutex);
   //dataobj = &(clientobj->servreg_user_pd_data); 

   service_name = servreg_calloc(sr_len, 1);
   if(SERVREG_NULL != service_name)
   {
      /* Get the service name from user space */
      ret = qurt_qdi_copy_from_user(client_handle, service_name, name, sr_len);
      if (-1 != ret)
      {
         /* Get the existing SERVREG HANDLE */
         sr_mon_handle = servreg_get_sr_mon_handle(service_name);
         if(SERVREG_NULL != sr_mon_handle)
         {
            result = servreg_set_ack(sr_mon_handle, transaction_id);

            if(SERVREG_SUCCESS != result)
            {
               ERR_FATAL("SERVREG_QDI: in servreg_qdi_free_remote_handle() servreg_set_ack() failed ", 0, 0, 0);
               ret = -1;
            }
         }
         else
         {
            ERR_FATAL("SERVREG_QDI: in servreg_qdi_free_remote_handle() sr_mon_handle NULL ", 0, 0, 0);
            ret = -1;
         }
      }
      else
      {
         ERR_FATAL("SERVREG_QDI: in servreg_qdi_free_remote_handle() qurt_qdi_copy_from_user() failed ", 0, 0, 0);
      }

      servreg_free(service_name);
   }
   else
   {
      ERR_FATAL("SERVREG_QDI: in servreg_qdi_free_remote_handle() calloc failed", 0, 0, 0);
   }

   //qurt_pimutex_unlock(&servreg_qdi_root_internal.servnotif_qdi_mutex);
   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_qdi_get_remote_handle
 *
 * Description: 
 *     Function checks which notifier handle's state changed returns that handle info
 *
 * Parameters:
 *     client_handle         : QDI handle which represents the client
 *                             which made this QDI request
 *     clientobj             : QDI object which represents the client
 *                             which made this QDI request
 *     remote_addr           : address of the monitor handle address who's state
 *                             change sent to the user pd
 *     remote_state          : state of the monitor handle who's state changed
 *                             sent to the user pd
 *     remote_transaction_id : id from the root sent to the user pd
 *
 * Returns:
 *     SERVREG_SUCCESS (0) or SERVREG_FAILURE(-1)
 * =====================================================================  */
static int servreg_qdi_get_remote_handle(int client_handle, servnotif_qdi_device_p clientobj, uint32_t * remote_addr, uint32_t *remote_state, int32_t *remote_transaction_id)
{
   int ret = -1;
   SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle = SERVREG_NULL;
   SERVREG_SERVICE_STATE state;
   uint32_t addr = 0;
   int32_t transaction_id = 0;
   servnotif_qdi_device_data_t *dataobj;
   SERVREG_MON_HANDLE sr_mon_handle = SERVREG_NULL;

   //qurt_pimutex_lock(&servreg_qdi_root_internal.servnotif_qdi_mutex);
   dataobj = &(clientobj->servreg_user_pd_data); 

   sr_qdi_notif_handle = servreg_get_qdi_notif_node_state_change(&dataobj->servreg_qdi_notif_node_list_head);
   if(SERVREG_NULL != sr_qdi_notif_handle)
   {
      /* Send the address of the root handle to user space */
      addr = (uint32_t)sr_qdi_notif_handle;
      ret = qurt_qdi_copy_to_user(client_handle, remote_addr, &addr, sizeof(uint32_t));
      if (-1 != ret)
      {
         /* Send the state of the remote handle to user space */
         state = servreg_notif_get_curr_state(sr_qdi_notif_handle);
         ret = qurt_qdi_copy_to_user(client_handle, remote_state, &state, sizeof(uint32_t));
         if (-1 != ret)
         {
            /* Send the transaction id of the remote handle to user space */
            sr_mon_handle = servreg_notif_get_mon_handle(sr_qdi_notif_handle);
            transaction_id = servreg_get_transaction_id(sr_mon_handle);
            ret = qurt_qdi_copy_to_user(client_handle, remote_transaction_id, &transaction_id, sizeof(uint32_t));

            if(-1 == ret)
            {
               TMS_MSG_ERROR("qurt_qdi_copy_to_user failed");
            }
         }
         else
         {
            TMS_MSG_ERROR("qurt_qdi_copy_to_user failed");
         }
      }
      else
      {
         TMS_MSG_ERROR("qurt_qdi_copy_to_user failed");
      }
   }
   else
   {
      TMS_MSG_ERROR("sr_qdi_notif_handle NULL");
   }
   //qurt_pimutex_unlock(&servreg_qdi_root_internal.servnotif_qdi_mutex);
   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_qdi_get_ack_handle
 *
 * Description: 
 *     Function checks which notifier handle's ack was set and returns that handle info
 *
 * Parameters:
 *     client_handle           : QDI handle which represents the client
 *                               which made this QDI request
 *     clientobj               : QDI object which represents the client
 *                               which made this QDI request
 *     remote_name             : name of the monitor handle address who's ack was set
 *     remote_transaction_id   : id of the root sent to the user pd
 *
 * Returns:
 *     SERVREG_SUCCESS (0) or SERVREG_FAILURE(-1)
 * =====================================================================  */
static int servreg_qdi_get_ack_handle(int client_handle, servnotif_qdi_device_p clientobj, SERVREG_NAME remote_name, int32_t *remote_transaction_id)
{
   int ret = -1;
   SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle = SERVREG_NULL;
   SERVREG_NAME sr_name = SERVREG_NULL;
   uint32_t sr_len = 0;
   int32_t transaction_id = 0;
   servnotif_qdi_device_data_t *dataobj;
   SERVREG_MON_HANDLE sr_mon_handle = SERVREG_NULL;

   //qurt_pimutex_lock(&servreg_qdi_root_internal.servnotif_qdi_mutex);
   dataobj = &(clientobj->servreg_user_pd_data); 

   sr_qdi_notif_handle = servreg_get_qdi_notif_node_ack_set(&dataobj->servreg_qdi_notif_node_register_head);
   if(SERVREG_NULL != sr_qdi_notif_handle)
   {
      /* Send the name of the remote handle to user space */
      sr_mon_handle = servreg_notif_get_mon_handle(sr_qdi_notif_handle);
      sr_name = servreg_get_service_name(sr_mon_handle);
      if(SERVREG_NULL != sr_name)
      {
         sr_len = strlen(sr_name);
         ret = qurt_qdi_copy_to_user(client_handle, remote_name, sr_name, sizeof(char) * sr_len);
         if (-1 != ret)
         {
            /* Send the transaction id of the remote handle to user space */
            transaction_id = servreg_get_transaction_id(sr_mon_handle);
            ret = qurt_qdi_copy_to_user(client_handle, remote_transaction_id, &transaction_id, sizeof(uint32_t));

            if(-1 == ret)
            {
               TMS_MSG_ERROR("qurt_qdi_copy_to_user failed");
            }
         }
         else
         {
            TMS_MSG_ERROR("qurt_qdi_copy_to_user failed");
         }
      }
      else
      {
         TMS_MSG_ERROR("sr_name NULL");
      }
   }
   else
   {
      TMS_MSG_ERROR("sr_qdi_notif_handle NULL");
   }
   //qurt_pimutex_unlock(&servreg_qdi_root_internal.servnotif_qdi_mutex);
   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_qdi_create_qmi_entry
 *
 * Description: 
 *     Create a QMI entry for the remote service
 *
 * Parameters:
 *     client_handle  : QDI handle which represents the client
 *                      which made this QDI request
 *     clientobj      : QDI object which represents the client
 *                      which made this QDI request.
 *     name           : service name that the user-pd is wants to create a qmi entry for
 *     name_len       : length of the name
 *     qmi_instance_id: qmi id of the remote service stored in the qmi entry structure
 *
 * Returns:
 *     SERVREG_SUCCESS (0) or SERVREG_FAILURE(-1)
 * =====================================================================  */
static int servreg_qdi_create_qmi_entry(int client_handle, servnotif_qdi_device_p clientobj, SERVREG_NAME name, uint32_t name_len, uint32_t qmi_instance_id)
{
   int ret = -1;
   SERVREG_NAME service_name = SERVREG_NULL;
   uint32_t sr_len = name_len;
   //servnotif_qdi_device_data_t *dataobj;

   //qurt_pimutex_lock(&servreg_qdi_root_internal.servnotif_qdi_mutex);
   //dataobj = &(clientobj->servreg_user_pd_data); 

   service_name = servreg_calloc(sr_len, 1);
   if(SERVREG_NULL != service_name)
   {
      /* Get the service name from user space */
      ret = qurt_qdi_copy_from_user(client_handle, service_name, name, sr_len);
      if (-1 != ret)
      {
         servreg_create_qmi_table_entry(service_name, qmi_instance_id);
      }
      else
      {
         ERR_FATAL("SERVREG_QDI: in servreg_qdi_create_qmi_entry() qurt_qdi_copy_from_user() failed ", 0, 0, 0);
      }

      servreg_free(service_name);
   }
   else
   {
      ERR_FATAL("SERVREG_QDI: in servreg_qdi_create_qmi_entry() calloc failed", 0, 0, 0);
   }

   //qurt_pimutex_unlock(&servreg_qdi_root_internal.servnotif_qdi_mutex);
   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_qdi_register_handle
 *
 * Description: 
 *     Function registers the root handle in the user pd
 *
 * Parameters:
 *     client_handle  : QDI handle which represents the client
 *                      which made this QDI request
 *     clientobj      : QDI object which represents the client
 *                      which made this QDI request.
 *     remote_addr    : address of the monitor handle address who's state change
 *     name           : name of the service sent to the user pd to register
 *
 * Returns:
 *     SERVREG_SUCCESS (0) or SERVREG_FAILURE(-1)
 * =====================================================================  */
static int servreg_qdi_register_handle(int client_handle, servnotif_qdi_device_p clientobj, uint32_t * remote_addr, SERVREG_NAME name)
{
   int ret = -1;
   SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle = SERVREG_NULL;
   SERVREG_NAME sr_name = SERVREG_NULL;
   servnotif_qdi_device_data_t *dataobj;
   SERVREG_MON_HANDLE sr_mon_handle = SERVREG_NULL;
   uint32_t sr_len = 0, addr = 0;

   //qurt_pimutex_lock(&servreg_qdi_root_internal.servnotif_qdi_mutex);
   dataobj = &(clientobj->servreg_user_pd_data); 

   sr_qdi_notif_handle = dataobj->servreg_qdi_notif_node_register_head;
   if(SERVREG_NULL != sr_qdi_notif_handle)
   {
      /* Send the address of the root handle to user space */
      addr = (uint32_t)sr_qdi_notif_handle;
      ret = qurt_qdi_copy_to_user(client_handle, remote_addr, &addr, sizeof(uint32_t));
      if (-1 != ret)
      {
         /* Send the name of the remote handle to user space */
         /* The first entry will be the latest registrant */
         sr_mon_handle = servreg_notif_get_mon_handle(sr_qdi_notif_handle);
         sr_name = servreg_get_service_name(sr_mon_handle);
         if(SERVREG_NULL != sr_name)
         {
            sr_len = strlen(sr_name);
            ret = qurt_qdi_copy_to_user(client_handle, name, sr_name, sr_len);

            if(-1 == ret)
            {
               TMS_MSG_ERROR("qurt_qdi_copy_to_user failed");
            }
         }
         else
         {
            TMS_MSG_ERROR("sr_name NULL");
         }
      }
      else
      {
         TMS_MSG_ERROR("qurt_qdi_copy_to_user failed");
      }
   }
   else
   {
      TMS_MSG_ERROR("sr_qdi_notif_handle NULL");
   }
   //qurt_pimutex_unlock(&servreg_qdi_root_internal.servnotif_qdi_mutex);
   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_qdi_set_state
 *
 * Description: 
 *     Function sets the state of the user service
 *
 * Parameters:
 *     client_handle  : QDI handle which represents the client
 *                      which made this QDI request
 *     clientobj      : QDI object which represents the client
 *                      which made this QDI request.
 *     remote_addr    : address of the monitor handle address who's state change
 *     remote_state   : state of the monitor handle who's state changed
 *
 * Returns:
 *     SERVREG_SUCCESS (0) or SERVREG_FAILURE(-1)
 * =====================================================================  */
static int servreg_qdi_set_state(int client_handle, servnotif_qdi_device_p clientobj, uint32_t remote_addr, uint32_t remote_state)
{
   int ret = -1;
   SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle = SERVREG_NULL;
   SERVREG_MON_HANDLE sr_mon_handle = SERVREG_NULL;

   //qurt_pimutex_lock(&servreg_qdi_root_internal.servnotif_qdi_mutex);

   sr_qdi_notif_handle = (SERVREG_QDI_NOTIF_HANDLE)remote_addr;
   sr_mon_handle = servreg_notif_get_mon_handle(sr_qdi_notif_handle);

   if(SERVREG_SUCCESS != servreg_set_state(sr_mon_handle, remote_state))
   {
      TMS_MSG_ERROR("Set state failed");
   }


   //qurt_pimutex_unlock(&servreg_qdi_root_internal.servnotif_qdi_mutex);
   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_qdi_update_handle
 *
 * Description: 
 *     Function updates the client handle in the root.
 *
 * Parameters:
 *     client_handle    : QDI handle which represents the client
 *                        which made this QDI request
 *     clientobj        : QDI object which represents the client
 *                        which made this QDI request.
 *     state            : state of the monitor handle who's state changed
 *     transaction_id   : transaction ID of the monitor handle who's state changed
 *     remote_addr      : address of the monitor handle address who's state change
 *
 * Returns:
 *     SERVREG_SUCCESS (0) or SERVREG_FAILURE(-1)
 * =====================================================================  */
static int servreg_qdi_update_handle(int client_handle, servnotif_qdi_device_p clientobj, uint32_t state, int32_t transaction_id, uint32_t remote_addr)
{
   int ret = 0;
   SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle = SERVREG_NULL;
   sr_qdi_notif_handle = (SERVREG_QDI_NOTIF_HANDLE)remote_addr;
   
   if(SERVREG_NULL != sr_qdi_notif_handle)
   {
      SERVREG_TRACE_2(QDI_UPD_HL, sr_qdi_notif_handle, state);

      if(SERVREG_SUCCESS != servreg_notif_update_handle(sr_qdi_notif_handle, state, transaction_id))
      {
         ret = -1;
         TMS_MSG_ERROR("Update handle failed");
      }

   }
   else
   {
      ret = -1;
      TMS_MSG_ERROR("sr_qdi_notif_handle NULL");
   }
   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_qdi_register_root_handles
 *
 * Description: 
 *     Function registers the first client stored in the cache root qdi structure
 *
 * Parameters:
 *     client_handle  : QDI handle which represents the client
 *                      which made this QDI request
 *     clientobj      : QDI object which represents the client
 *                      which made this QDI request.
 *     remote_addr    : return address of the root stored in the user pd
 *     name           : service name in the user pd
 *
 * Returns:
 *     SERVREG_SUCCESS (0) or SERVREG_FAILURE(-1)
 * =====================================================================  */
static int servreg_qdi_register_root_handles(int client_handle, servnotif_qdi_device_p clientobj, uint32_t * remote_addr, SERVREG_NAME name)
{
   int ret = -1;
   SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle = SERVREG_NULL;
   SERVREG_MON_HANDLE sr_mon_handle = SERVREG_NULL;
   SERVREG_NAME sr_name = SERVREG_NULL;
   servnotif_qdi_device_data_t *dataobj;
   uint32_t sr_len = 0, addr = 0;
   servnotif_cache_pd_data_p servnotif_cache_pd_data_node = SERVREG_NULL, servnotif_qdi_pd_data_prev = SERVREG_NULL;

   //qurt_pimutex_lock(&servreg_qdi_root_internal.servnotif_qdi_mutex);
   dataobj = &(clientobj->servreg_user_pd_data); 
   sr_name = servreg_get_service_name(dataobj->sr_mon_handle);
   if(SERVREG_NULL == sr_name)
   {
      TMS_MSG_ERROR("sr_name NULL");
      return -1;
   }

   sr_len = strlen(sr_name);

   servnotif_cache_pd_data_node = servnotif_cache_pd_data_head;

   if(SERVREG_NULL == servnotif_cache_pd_data_head)
   {
      return 0;
   }

   while(SERVREG_NULL != servnotif_cache_pd_data_node)
   {
      /* Get the correct cache node by comparing the pd_names */
      if(strncmp(servnotif_cache_pd_data_node->sr_name, sr_name, sr_len) == 0)
      {
         /* Get the first registrant */
         sr_qdi_notif_handle = servnotif_cache_pd_data_node->servreg_qdi_notif_node_register_head;

         if(SERVREG_NULL == dataobj->servreg_qdi_notif_node_register_head)
         {
            /* Will be updated only the first time from servreg_qdi_init() from user-pd */
            dataobj->servreg_qdi_notif_node_register_head = servnotif_cache_pd_data_node->servreg_qdi_notif_node_register_head;
         }

         /* update the head pointing to the next root client */
         servnotif_cache_pd_data_node->servreg_qdi_notif_node_register_head = servreg_get_qdi_notif_next_node(servnotif_cache_pd_data_node->servreg_qdi_notif_node_register_head);

         if(SERVREG_NULL == servnotif_cache_pd_data_node->servreg_qdi_notif_node_register_head)
         {
            if(servnotif_qdi_pd_data_prev == SERVREG_NULL)
            {
               servnotif_cache_pd_data_head = servnotif_cache_pd_data_node->next;
            }
            else
            {
               servnotif_qdi_pd_data_prev->next = servnotif_cache_pd_data_node->next;
            }
            
            SERVREG_TRACE_0(FREE_CACHE_PD_DATA);
           
            servreg_free(servnotif_cache_pd_data_node->sr_name);
            servreg_free(servnotif_cache_pd_data_node);
         }
         break;
      }
      servnotif_qdi_pd_data_prev = servnotif_cache_pd_data_node;
      servnotif_cache_pd_data_node = servnotif_cache_pd_data_node->next;
   }

   if(SERVREG_NULL != sr_qdi_notif_handle)
   {
      /* Send the address of the root handle to user space */
      addr = (uint32_t)sr_qdi_notif_handle;
      sr_mon_handle = servreg_notif_get_mon_handle(sr_qdi_notif_handle);
      if (SERVREG_NULL != sr_mon_handle)
        SERVREG_TRACE_2(QDI_REG_ROOT_H, sr_mon_handle, addr);
      
      sr_name = servreg_get_service_name(sr_mon_handle);
      if(SERVREG_NULL != sr_name)
      {
         sr_len = strlen(sr_name);
         servreg_set_synchronous(sr_mon_handle);

         ret = qurt_qdi_copy_to_user(client_handle, remote_addr, &addr, sizeof(uint32_t));
         if (-1 != ret)
         {
            /* Send the name of the remote handle to user space */
            ret = qurt_qdi_copy_to_user(client_handle, name, sr_name, sr_len);

            if(-1 == ret)
            {
               TMS_MSG_ERROR("qurt_qdi_copy_to_user failed");
            }
         }
         else
         {
            TMS_MSG_ERROR("qurt_qdi_copy_to_user failed");
         }
      }
      else
      {
         TMS_MSG_ERROR("sr_name NULL");
      }
   }
   else
   {
      TMS_MSG_ERROR("sr_qdi_notif_handle NULL");
   }
   //qurt_pimutex_unlock(&servreg_qdi_root_internal.servnotif_qdi_mutex);
   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_register_user_listener
 *
 * Description:
 *     Service level registration. Root client registering for a service in the user pd
 *
 * Parameters:
 *     service_name             : Service name that the client is interested in 
 *     sr_mon_handle            : Opaque handle to existing event
 *     sr_curr_remote_state     : State of the service in the user pd returned
 *     sr_remote_transaction_id : Transaction id of the service in the user pd returned
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_register_user_listener(SERVREG_NAME service_name, SERVREG_MON_HANDLE sr_mon_handle, uint32_t * sr_curr_remote_state, int32_t * sr_remote_transaction_id)
{
   SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle = SERVREG_NULL;
   servnotif_qdi_device_p servreg_qdi_pd_node = SERVREG_NULL;
   SERVREG_NAME pd_name = SERVREG_NULL;
   servnotif_cache_pd_data_p servnotif_cache_pd_data_node = SERVREG_NULL;

   qurt_pimutex_lock(&servreg_qdi_root_internal.servnotif_qdi_mutex);
   servreg_qdi_pd_node = servreg_qdi_root_internal.servreg_qdi_pd_head;

   while(SERVREG_NULL != servreg_qdi_pd_node)
   {
      pd_name = servreg_get_pd_name(servreg_qdi_pd_node->servreg_user_pd_data.sr_mon_handle);

      if(SERVREG_NULL != pd_name && SERVREG_NULL != strstr(service_name , pd_name))
      {
         sr_qdi_notif_handle = servreg_create_qdi_notif_node(sr_mon_handle, &(servreg_qdi_pd_node->servreg_user_pd_data.servreg_qdi_notif_node_register_head));
         
         if(SERVREG_NULL != sr_qdi_notif_handle)
         {
            qurt_anysignal_set(&servreg_qdi_pd_node->servreg_user_pd_data.sig_t, SERVREG_QDI_REG_ROOT_MASK_VALUE);

            *sr_curr_remote_state = servreg_notif_get_curr_state(sr_qdi_notif_handle);
            *sr_remote_transaction_id = servreg_notif_get_transaction_id(sr_qdi_notif_handle);
         }
         servreg_free(pd_name);
         qurt_pimutex_unlock(&servreg_qdi_root_internal.servnotif_qdi_mutex);
         return SERVREG_SUCCESS;
      }
      if(SERVREG_NULL != pd_name)
        servreg_free(pd_name);
      servreg_qdi_pd_node = servreg_qdi_pd_node->next;
   }

   /* If the PD is not up yet, save the registration info in servnotif_cache_pd_data_head*/
   if(SERVREG_NULL == servreg_qdi_pd_node)
   {
      SERVREG_TRACE_0(NULL_QDI_PD_NODE);
      
      servnotif_cache_pd_data_node = servnotif_cache_pd_data_head;

      /* check if the servnotif_cache_pd_data_node already exists for that pd name */
      while(SERVREG_NULL != servnotif_cache_pd_data_node)
      {
         pd_name = servnotif_cache_pd_data_node->sr_name;

         if(SERVREG_NULL != strstr(service_name , pd_name))
         {
            sr_qdi_notif_handle = servreg_create_qdi_notif_node(sr_mon_handle, &(servnotif_cache_pd_data_node->servreg_qdi_notif_node_register_head));

            if(SERVREG_NULL != sr_qdi_notif_handle)
            {
               *sr_curr_remote_state = servreg_notif_get_curr_state(sr_qdi_notif_handle);
               *sr_remote_transaction_id = servreg_notif_get_transaction_id(sr_qdi_notif_handle);
            }
            qurt_pimutex_unlock(&servreg_qdi_root_internal.servnotif_qdi_mutex);
            return SERVREG_SUCCESS;
         }
         servnotif_cache_pd_data_node = servnotif_cache_pd_data_node->next;
      }

      /* if the servnotif_cache_pd_data_node does not exists for that pd name, create a new one and add to servnotif_cache_pd_data_head */
      servnotif_cache_pd_data_node = (servnotif_cache_pd_data_p)servreg_calloc(sizeof(servnotif_cache_pd_data_t), 1);
      if(SERVREG_NULL != servnotif_cache_pd_data_node)
      {
         servnotif_cache_pd_data_node->sr_name = servreg_get_pd_name(sr_mon_handle);
         servnotif_cache_pd_data_node->servreg_qdi_notif_node_register_head = SERVREG_NULL;

         sr_qdi_notif_handle = servreg_create_qdi_notif_node(sr_mon_handle, &(servnotif_cache_pd_data_node->servreg_qdi_notif_node_register_head));

         if(SERVREG_NULL != sr_qdi_notif_handle)
         {
            *sr_curr_remote_state = servreg_notif_get_curr_state(sr_qdi_notif_handle);
            *sr_remote_transaction_id = servreg_notif_get_transaction_id(sr_qdi_notif_handle);
         }

         /*add the new servnotif_cache_pd_data_node to the head of the list */
         servnotif_cache_pd_data_node->next = servnotif_cache_pd_data_head;
         servnotif_cache_pd_data_head = servnotif_cache_pd_data_node;
      }
   }

   qurt_pimutex_unlock(&servreg_qdi_root_internal.servnotif_qdi_mutex);
   return SERVREG_SUCCESS;
}

/** =====================================================================
 * Function:
 *     servreg_set_user_ack
 *
 * Description:
 *     Set the ack from root to user pd
 *
 * Parameters:
 *     service_name             : Service name that the client is interested in 
 *     sr_mon_handle            : Opaque handle to existing event
 *     sr_curr_remote_state     : State of the service
 *     sr_remote_transaction_id : Transaction id of the service
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_set_user_ack(SERVREG_NAME service_name, SERVREG_MON_HANDLE sr_mon_handle, uint32_t sr_curr_remote_state, int32_t sr_remote_transaction_id)
{
   SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle = SERVREG_NULL;
   servnotif_qdi_device_p servreg_qdi_pd_node = SERVREG_NULL;
   SERVREG_NAME pd_name = SERVREG_NULL;

   qurt_pimutex_lock(&servreg_qdi_root_internal.servnotif_qdi_mutex);
   servreg_qdi_pd_node = servreg_qdi_root_internal.servreg_qdi_pd_head;
   while(SERVREG_NULL != servreg_qdi_pd_node)
   {
      pd_name = servreg_get_pd_name(servreg_qdi_pd_node->servreg_user_pd_data.sr_mon_handle);
      if(SERVREG_NULL != pd_name && SERVREG_NULL != strstr(service_name , pd_name))
      {
         sr_qdi_notif_handle = servreg_get_qdi_notif_node(sr_mon_handle, &(servreg_qdi_pd_node->servreg_user_pd_data.servreg_qdi_notif_node_register_head));
         
         if(SERVREG_NULL != sr_qdi_notif_handle)
         {
            servreg_notif_set_user_ack(sr_qdi_notif_handle);
            qurt_anysignal_set(&servreg_qdi_pd_node->servreg_user_pd_data.sig_t, SERVREG_QDI_ACK_ROOT_MASK_VALUE);
         }
         servreg_free(pd_name);
         qurt_pimutex_unlock(&servreg_qdi_root_internal.servnotif_qdi_mutex);
         return SERVREG_SUCCESS;
      }
      servreg_free(pd_name);
      servreg_qdi_pd_node = servreg_qdi_pd_node->next;
   }

   qurt_pimutex_unlock(&servreg_qdi_root_internal.servnotif_qdi_mutex);
   return SERVREG_SUCCESS;
}

/** =====================================================================
 * Function:
 *     servreg_qdi_invoke
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
int servreg_qdi_invoke(int client_handle,
                       qurt_qdi_obj_t* obj,
                       int servnotif_qdi_method,
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
   servnotif_qdi_device_p clientobj = (servnotif_qdi_device_p)obj;

   SERVREG_TRACE_2(QDI_INVOKE, qurt_thread_get_id(), servnotif_qdi_method);

   /* Driver Methods */
   switch (servnotif_qdi_method)
   {
      /* API, servreg_qdi_client_init */
      case QDI_OPEN:
      {
         ret = servreg_qdi_open(client_handle, a2.ptr, a3.num);
         break;
      }

      /* API, servreg_qdi_invoke_close */
      case QDI_CLOSE:
      {
         if(SERVREG_SUCCESS != servreg_qdi_close(clientobj))
         {
            TMS_SHUTDOWN_MSG_ERROR("servreg_qdi_close() failed");
         }

         ret = qurt_qdi_method_default(client_handle, obj, servnotif_qdi_method, a1, a2, a3, a4, a5, a6, a7, a8, a9);
         break;
      }

      /* API, servreg_qdi_invoke_worker_wait */
      case SERVREG_QDI_WORKER_WAIT:
      {
         ret = servreg_qdi_worker_wait(client_handle, clientobj, a1.ptr);
         break;
      }

      /* API, servreg_qdi_invoke_register_remote_listener */
      case SERVREG_QDI_REG_REMOTE_LISTENER:
      {
         ret = servreg_qdi_register_remote_listener(client_handle, clientobj, a1.ptr, a2.num, a3.ptr, a4.ptr, a5.ptr);
         break;
      }

      /* API, servreg_qdi_invoke_deregister_remote_listener */
      case SERVREG_QDI_DEREG_REMOTE_LISTENER:
      {
         ret = servreg_qdi_deregister_remote_listener(client_handle, clientobj, a1.ptr, a2.num);
         break;
      }

      /* API, servreg_qdi_invoke_free_remote_handle */
      case SERVREG_QDI_FREE_REMOTE_HANDLE:
      {
         ret = servreg_qdi_free_remote_handle(client_handle, clientobj, a1.ptr, a2.num);
         break;
      }

      /* API, servreg_qdi_invoke_set_remote_ack */
      case SERVREG_QDI_SET_REMOTE_ACK:
      {
         ret = servreg_qdi_set_remote_ack(client_handle, clientobj, a1.ptr, a2.num, a3.num);
         break;
      }

      /* API, servreg_qdi_invoke_get_remote_handle */
      case SERVREG_QDI_GET_REMOTE_HANDLE:
      {
         ret = servreg_qdi_get_remote_handle(client_handle, clientobj, a1.ptr, a2.ptr, a3.ptr);
         break;
      }

      /* API, servreg_qdi_invoke_create_qmi_entry */
      case SERVREG_QDI_CREATE_QMI_ENTRY:
      {
         ret = servreg_qdi_create_qmi_entry(client_handle, clientobj, a1.ptr, a2.num, a3.num);
         break;
      }

      /* API, servreg_qdi_invoke_register_remote_handle */
      case SERVREG_QDI_REGISTER_NEW_HANDLE:
      {
         ret = servreg_qdi_register_handle(client_handle, clientobj, a1.ptr, a2.ptr);
         break;
      }

      /* API, servreg_qdi_invoke_set_state */
      case SERVREG_QDI_SET_STATE:
      {
         ret = servreg_qdi_set_state(client_handle, clientobj, a1.num, a2.num);
         break;
      }

      /* API, servreg_qdi_invoke_update_remote_handle */
      case SERVREG_QDI_UPDATE_HANDLE:
      {
         ret = servreg_qdi_update_handle(client_handle, clientobj, a1.num, a2.num, a3.num);
         break;
      }

      /* API, servreg_qdi_invoke_register_root_handles */
      case SERVREG_QDI_REGISTER_ROOT_HANDLES:
      {
         ret = servreg_qdi_register_root_handles(client_handle, clientobj, a1.ptr, a2.ptr);
         break;
      }

      /* API, servreg_qdi_invoke_get_ack_handle */
      case SERVREG_QDI_GET_ROOT_ACK_HANDLE:
      {
         ret = servreg_qdi_get_ack_handle(client_handle, clientobj, a1.ptr, a2.ptr);
         break;
      }

      /* Default Method Handler */
      default:
      {
         ret = qurt_qdi_method_default(client_handle, obj, servnotif_qdi_method, a1, a2, a3, a4, a5, a6, a7, a8, a9);
         break; 
      }
   } /* switch() */

   SERVREG_TRACE_2(QDI_INVOKE_RETURN, servnotif_qdi_method, ret);
   SERVREG_TRACE_2(QDI_INVOKE_EXIT, qurt_thread_get_id(), servnotif_qdi_method);

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_qdi_init
 *
 * Description:
 *     Initialization function. Registers a QDI device with the generic QDI object
 *
 * Parameters:
 *     None
 * Returns:
 *     None
 * =====================================================================  */
void servreg_qdi_init(void)
{
   int ret = -1;
   qurt_qdi_obj_t *opener = &servreg_qdi_root_internal.servnotif_qdiobj;

   qurt_pimutex_init(&servreg_qdi_root_internal.servnotif_qdi_mutex);

   qurt_pimutex_lock(&servreg_qdi_root_internal.servnotif_qdi_mutex);

   servreg_qdi_root_internal.servreg_qdi_pd_head = SERVREG_NULL;

   opener->invoke = servreg_qdi_invoke;
   opener->refcnt = QDI_REFCNT_INIT;
   opener->release = servreg_qdi_release;

   /* Register the Driver */
   ret = qurt_qdi_devname_register(SERVREG_QDI_DEVICE, opener);
   if(ret != 0)
   {
      ERR_FATAL( "SERVREG_QDI: qurt_qdi_devname_register failed", 0, 0, 0);
   }

   qurt_pimutex_unlock(&servreg_qdi_root_internal.servnotif_qdi_mutex);
}


