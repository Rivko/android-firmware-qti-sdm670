/*
#============================================================================
#  Name:
#    servreg_qmi_notifier_server.c
#
#  Description:
#    Service Registry notifier file for root image. This module serves as the end-point
#    of communication via QMI.
#
# Copyright (c) 2015-2018 by Qualcomm Technologies, Inc.  All Rights Reserved.
#============================================================================
*/
#include "stdarg.h"
#include "stdlib.h"
#include <stringl/stringl.h>
#include "comdef.h"             /* Definitions for byte, word, etc.     */
#include "err.h"
#include "tms_utils_msg.h"
#include "tms_utils.h"
#include "qmi_csi.h"
#include "qmi_csi_target_ext.h"
#include "timer.h"
#include "rcinit.h"
#include "qurt_qdi_constants.h"
#include "qurt_qdi.h"
#include "pd_mon_restart.h"

#include "service_registry_notifier_v01.h"
#include "servreg_internal.h"
#include "servreg_locator.h"
#include "servreg_localdb.h"
#include "servreg_monitor.h"
#include "servreg_notifier.h"
#include "servreg_qmi_notifier_server.h"

typedef enum
{
   SERVREG_NOTIF_REGISTER_LISTENER_REQ      = QMI_SERVREG_NOTIF_REGISTER_LISTENER_REQ_V01,
   SERVREG_NOTIF_QUERY_STATE_REQ            = QMI_SERVREG_NOTIF_QUERY_STATE_REQ_V01,
   SERVREG_NOTIF_STATE_UPDATED_IND          = QMI_SERVREG_NOTIF_STATE_UPDATED_IND_V01,
   SERVREG_NOTIF_STATE_UPDATED_IND_ACK_REQ  = QMI_SERVREG_NOTIF_STATE_UPDATED_IND_ACK_REQ_V01,
   SERVREG_NOTIF_RESTART_PD_REQ             = QMI_SERVREG_NOTIF_RESTART_PD_RESP_V01,
}servreg_notif_msg_req_type;

/* QMI message related and keeping it global because it should be valid for the lifetime of the user_handle */
struct sr_client_type_s
{
   qmi_client_handle  client_handle;
   boolean            connected;
   struct sr_client_type_s* next;
};
typedef struct sr_client_type_s sr_client_type_t, * sr_client_type_p;

/* QMI client list entry structure */
struct sr_qmi_s_client_s
{
   /* Client handle that is registering a listener */
   qmi_client_handle client_handle;
   int32_t transaction_id; // transaction_id of the last transaction
   int32_t last_ack_id; // cached transaction_id of the last ack rx'd

   struct sr_qmi_s_client_s * next;
};
typedef struct sr_qmi_s_client_s sr_qmi_s_client_t, * sr_qmi_s_client_p;

/* Service Registry Notifier proxy. List of SERVREG_MON_HANDLE'S*/
/* This listens for a single service on behalf of many clients */
struct sr_qmi_s_proxy_s
{
   uint32_t notif_signature;

   /* Service mon handle for the service the client is interested in */
   SERVREG_MON_HANDLE sr_mon_handle;

   sr_qmi_s_client_p qmi_client_list_head;

   int32_t transaction_id; // cached transaction_id of the last/active transaction
   int32_t last_ack_id;    // cached transaction_id of the last upstream ack

   /* Service state */
   SERVREG_SERVICE_STATE curr_state;

   struct sr_qmi_s_proxy_s* next;
};
typedef struct sr_qmi_s_proxy_s sr_qmi_s_proxy_t, * sr_qmi_s_proxy_p;

/* Internal QMI Server Stucture */
struct servreg_qmi_server_internal_s
{
   qmi_csi_service_handle servreg_notif_qmi_server_handler;
   sr_client_type_p qmi_notif_client_head;
   sr_qmi_s_proxy_p servreg_qmi_notif_server_list_head;  /* Head node of the servreg notification list */
   servreg_mutex_t mutex;
};
struct servreg_qmi_server_internal_s servreg_qmi_server_internal;

static sr_qmi_s_proxy_p sr_qmi_s_listener_attach(SERVREG_MON_HANDLE sr_mon_handle, qmi_client_handle client_handle);
static SERVREG_RESULT   sr_qmi_s_listener_detach(sr_qmi_s_proxy_p sr_qmi_s_proxy, qmi_client_handle client_handle);

static sr_qmi_s_proxy_p sr_qmi_get_proxy(SERVREG_MON_HANDLE sr_mon_handle);
static sr_qmi_s_proxy_p sr_qmi_get_proxy_iff_listener(SERVREG_MON_HANDLE sr_mon_handle, qmi_client_handle client_handle, sr_qmi_s_client_p *opt_client_pp);
static SERVREG_RESULT sr_qmi_set_ack(sr_qmi_s_proxy_p sr_qmi_s_proxy, qmi_client_handle client_h, int32_t ack_id);
static SERVREG_BOOL sr_qmi_check_upstream_ack(sr_qmi_s_proxy_p sr_qmi_s_proxy);
static SERVREG_RESULT servreg_qmi_handle_false_ack_req(sr_qmi_s_proxy_p sr_qmi_s_proxy, int32_t transaction_id);

//Message handlers
static SERVREG_RESULT servreg_qmi_handle_ack_req(qmi_servreg_notif_set_ack_req_msg_v01 *request, 
                                                 qmi_client_handle client_h);
static SERVREG_RESULT servreg_qmi_handle_dereg_req(qmi_servreg_notif_register_listener_req_msg_v01 *request,
                                                   qmi_client_handle client_h);

extern boolean err_qdi_check_pdr_enabled(int asid);
/** =====================================================================
 * Function:
 *     sr_qmi_s_proxy_pool_alloc
 *
 * Description:
 *     Gives the first available free and allocated space from the memory
 *
 * Parameters:
 *     None
 *
 * Returns:
 *    sr_qmi_s_proxy_p : the first available free and allocated space from the memory
 * =====================================================================  */
static sr_qmi_s_proxy_p sr_qmi_s_proxy_pool_alloc(void)
{
   sr_qmi_s_proxy_p ret = SERVREG_NULL;
   ret = (sr_qmi_s_proxy_p)servreg_calloc(sizeof(sr_qmi_s_proxy_t),1);
   
   if ( SERVREG_NULL != ret ) {
     ret->notif_signature = SERVREG_QMI_NOTIF_SERVER_SIGNATURE;
     ret->curr_state = SERVREG_SERVICE_STATE_UNINIT;
   } else {
      ERR_FATAL( "sr_qmi_s_proxy_p calloc failed", 0, 0, 0);
   }
   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_qmi_notif_server_node_internal_init
 *
 * Description:
 *     Initialization of the internal memory pools and other internals
 *     for sr notifier nodes
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
void servreg_qmi_notif_server_node_internal_init(void)
{
   servreg_mutex_init_dal(&servreg_qmi_server_internal.mutex);
}

/** =====================================================================
 * Function:
 *     sr_qmi_get_proxy
 *
 * Description:
 *     Checks if a node already exists with the given name. If it does 
 *     exists it returns a pointer to that node.
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by just domain name.
 *
 * Returns:
 *    sr_qmi_s_proxy_p : handle to the sr notif node
 * =====================================================================  */
static sr_qmi_s_proxy_p sr_qmi_get_proxy(SERVREG_MON_HANDLE sr_mon_handle)
{
   sr_qmi_s_proxy_p sr_qmi_s_proxy = SERVREG_NULL;

   servreg_mutex_lock_dal(&servreg_qmi_server_internal.mutex);

   sr_qmi_s_proxy = servreg_qmi_server_internal.servreg_qmi_notif_server_list_head;

   while(SERVREG_NULL != sr_qmi_s_proxy)
   {
      /* Check if the same service is already registered */
      if(sr_mon_handle == sr_qmi_s_proxy->sr_mon_handle)
      {
         break;
      }
      sr_qmi_s_proxy = sr_qmi_s_proxy->next;
   }

   servreg_mutex_unlock_dal(&servreg_qmi_server_internal.mutex);

   return sr_qmi_s_proxy;
}


/** =====================================================================
 * Function:
 *     sr_qmi_get_proxy_iff_listener
 *
 * Description:
 *     Checks if a qmi entry already exists with the given name. If it does 
 *     exists it returns a pointer to that node.
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by just domain name.
 *     qmi_client_handle: client to verify is registered
 *     sr_qmi_s_client_p*: pointer reference populated with
 *                         internal client data if not NULL
 *
 * Returns:
 *    sr_qmi_s_proxy_p : handle to the sr notif node
 * =====================================================================  */
static sr_qmi_s_proxy_p sr_qmi_get_proxy_iff_listener(SERVREG_MON_HANDLE sr_mon_handle, qmi_client_handle client_handle, sr_qmi_s_client_p *opt_client_pp)
{
   sr_qmi_s_proxy_p sr_qmi_s_proxy = SERVREG_NULL;
   sr_qmi_s_client_p sr_qmi_s_client = SERVREG_NULL;
   SERVREG_BOOL sr_entry_found = SERVREG_FALSE;

   servreg_mutex_lock_dal(&servreg_qmi_server_internal.mutex);

   sr_qmi_s_proxy = servreg_qmi_server_internal.servreg_qmi_notif_server_list_head;

   while(SERVREG_NULL != sr_qmi_s_proxy)
   {
      /* Check if the same remote qmi client is already registered as listener for the same service */
      if(sr_mon_handle == sr_qmi_s_proxy->sr_mon_handle)
      {
         sr_qmi_s_client = sr_qmi_s_proxy->qmi_client_list_head;

         /* Walk through the qmi_client list and find the matching qmi_client handle */
         while(SERVREG_NULL != sr_qmi_s_client)
         {
            if(client_handle == sr_qmi_s_client->client_handle)
            {
               if (SERVREG_NULL != opt_client_pp)
               {
                  *opt_client_pp = sr_qmi_s_client;
               }
               sr_entry_found = SERVREG_TRUE;
               break;
            }
            else
            {
               sr_qmi_s_client = sr_qmi_s_client->next;
            }
         }
      }

      if(SERVREG_TRUE != sr_entry_found)
      {
         sr_qmi_s_proxy = sr_qmi_s_proxy->next;
      }
      else
      {
         break;
      }
   }

   servreg_mutex_unlock_dal(&servreg_qmi_server_internal.mutex);

   return sr_qmi_s_proxy;
}

/** =====================================================================
 * Function:
 *     sr_qmi_s_listener_attach
 *
 * Description:
 *     Creates a srnotif node with the given sr_mon_handle
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by just domain name.
 *     curr_state : curr_state of the service
 *
 * Returns:
 *    sr_qmi_s_proxy_p : handle to the sr notif node
 * =====================================================================  */
static sr_qmi_s_proxy_p sr_qmi_s_listener_attach(SERVREG_MON_HANDLE sr_mon_handle, qmi_client_handle client_handle)
{
   sr_qmi_s_proxy_p sr_qmi_s_proxy = SERVREG_NULL;  
   sr_qmi_s_proxy_p sr_qmi_s_proxy_with_entry = SERVREG_NULL;
   sr_qmi_s_client_p sr_qmi_s_client = SERVREG_NULL;

   /* Check if the srnotif node with that event name exists */
   sr_qmi_s_proxy = sr_qmi_get_proxy(sr_mon_handle);

   servreg_mutex_lock_dal(&servreg_qmi_server_internal.mutex);

   if(SERVREG_NULL == sr_qmi_s_proxy)
   {
      sr_qmi_s_proxy = sr_qmi_s_proxy_pool_alloc();

      if(SERVREG_NULL != sr_qmi_s_proxy)
      {
         /*  Insert to head of list */
         sr_qmi_s_proxy->next = servreg_qmi_server_internal.servreg_qmi_notif_server_list_head;

         /* Update head */
         servreg_qmi_server_internal.servreg_qmi_notif_server_list_head = sr_qmi_s_proxy;

         /* qmi entry */
         sr_qmi_s_client = (sr_qmi_s_client_p)servreg_calloc(sizeof(sr_qmi_s_client_t),1);
         
         if(SERVREG_NULL != sr_qmi_s_client)
         {
            sr_qmi_s_client->client_handle = client_handle;
         }
         else
         {
            ERR_FATAL( "sr_qmi_s_client calloc failed", 0, 0, 0);
         }

         /* Insert the domain_entry in the head of the list */

         sr_qmi_s_client->next = sr_qmi_s_proxy->qmi_client_list_head;
         /* Update the head */
         sr_qmi_s_proxy->qmi_client_list_head = sr_qmi_s_client;

         sr_qmi_s_proxy->sr_mon_handle = sr_mon_handle;
         sr_qmi_s_proxy->curr_state = servreg_get_service_curr_state(sr_mon_handle);
      }
      else
      {
         ERR_FATAL( "sr_qmi_s_proxy calloc failed", 0, 0, 0);
      }
   }
   else
   {
      /* Check if the srnotif entry exists */
      sr_qmi_s_proxy_with_entry = sr_qmi_get_proxy_iff_listener(sr_mon_handle, client_handle, SERVREG_NULL);

      if(SERVREG_NULL == sr_qmi_s_proxy_with_entry)
      {
         /* qmi entry */
         sr_qmi_s_client = (sr_qmi_s_client_p)servreg_calloc(sizeof(sr_qmi_s_client_t),1);
         
         if(SERVREG_NULL != sr_qmi_s_client)
         {
            sr_qmi_s_client->client_handle = client_handle;
         }
         else
         {
            ERR_FATAL( "sr_qmi_s_client calloc failed", 0, 0, 0);
         }

         /* Insert the domain_entry in the head of the list */
         sr_qmi_s_client->next = sr_qmi_s_proxy->qmi_client_list_head;

         /* Update the head */
         sr_qmi_s_proxy->qmi_client_list_head = sr_qmi_s_client;

         //sr_qmi_s_proxy->curr_state = servreg_get_service_curr_state(sr_mon_handle);
      }
   }

   servreg_mutex_unlock_dal(&servreg_qmi_server_internal.mutex);

   return sr_qmi_s_proxy;
}

/** =====================================================================
 * Function:
 *     sr_qmi_s_listener_detach
 *
 * Description:
 *     Deletes a sr notif node given the sr notif handle
 *
 * Parameters:
 *     sr_qmi_s_proxy  : Handle to the notifier node to be
 *     deleted
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
static SERVREG_RESULT sr_qmi_s_listener_detach(sr_qmi_s_proxy_p sr_qmi_s_proxy, qmi_client_handle client_handle_del)
{
   sr_qmi_s_proxy_p sr_qmi_s_proxy_curr = SERVREG_NULL, sr_qmi_s_proxy_del = SERVREG_NULL, sr_qmi_s_proxy_prev = SERVREG_NULL;
   sr_qmi_s_client_p sr_qmi_s_client_curr = SERVREG_NULL, sr_qmi_s_client_prev = SERVREG_NULL;
   SERVREG_RESULT ret = SERVREG_FAILURE;

   servreg_mutex_lock_dal(&servreg_qmi_server_internal.mutex);

   sr_qmi_s_proxy_curr = servreg_qmi_server_internal.servreg_qmi_notif_server_list_head;
   sr_qmi_s_proxy_del = sr_qmi_s_proxy;

   if(SERVREG_NULL != sr_qmi_s_proxy_del)
   {
      if(SERVREG_QMI_NOTIF_SERVER_SIGNATURE == sr_qmi_s_proxy_del->notif_signature)
      {
         /* First look for the correct service handle in the handle list */
         while(SERVREG_NULL != sr_qmi_s_proxy_curr)
         {
            if(sr_qmi_s_proxy_curr == sr_qmi_s_proxy_del)
            {
               sr_qmi_s_client_curr = sr_qmi_s_proxy_del->qmi_client_list_head;
               sr_qmi_s_client_prev = SERVREG_NULL;

               /* Once the handle is found, then look for the exact client in the client list */
               while(SERVREG_NULL != sr_qmi_s_client_curr)
               {
                  if(client_handle_del == sr_qmi_s_client_curr->client_handle)
                  {
                     if(SERVREG_NULL == sr_qmi_s_client_prev)
                     {
                        sr_qmi_s_proxy_del->qmi_client_list_head = sr_qmi_s_client_curr->next;
                     }
                     else
                     {
                        sr_qmi_s_client_prev->next = sr_qmi_s_client_curr->next;
                     }

                     sr_qmi_s_client_curr->next = SERVREG_NULL;

                     /* free the qmi notif entry */
                     servreg_free(sr_qmi_s_client_curr);
                     ret = SERVREG_SUCCESS;

                     break;
                  }

                  sr_qmi_s_client_prev = sr_qmi_s_client_curr;
                  sr_qmi_s_client_curr = sr_qmi_s_client_curr->next;
               }

               /* Delete the server table entry if no clients listening for that event */
               if(SERVREG_NULL == sr_qmi_s_proxy_del->qmi_client_list_head)
               {
                  if(SERVREG_NULL == sr_qmi_s_proxy_prev)
                  {
                     servreg_qmi_server_internal.servreg_qmi_notif_server_list_head = sr_qmi_s_proxy_del->next;
                  }
                  else
                  {
                     sr_qmi_s_proxy_prev->next = sr_qmi_s_proxy_del->next;
                  }

                  sr_qmi_s_proxy_del->next = SERVREG_NULL;

                  //nothing left to proxy, unhook from servreg proper
                  SERVREG_TRACE_2(QMI_DEREG_ALL, sr_qmi_s_proxy_del->sr_mon_handle, 0);
                  if (SERVREG_SUCCESS != sr_qmi_s_deregister_proxy_listener(sr_qmi_s_proxy_del->sr_mon_handle))
                  {
                     SERVREG_TRACE_0(FAIL);
                     //should not be fatal, signals will arrive at task which won't find anything to process
                     TMS_MSG_ERROR("Could not deregister from framework, expect spurious signals");
                  }

                 servreg_free(sr_qmi_s_proxy_del);
                 ret = SERVREG_SUCCESS;
               }

               break;
            }

            sr_qmi_s_proxy_prev = sr_qmi_s_proxy_curr;
            sr_qmi_s_proxy_curr = sr_qmi_s_proxy_curr->next;  
            
         } /* while() */
      }
      else
      {
         TMS_MSG_ERROR("sr_qmi_s_proxy has invalid signature");
         ret = SERVREG_INVALID_HANDLE;
      }
   }
   else
   {
      TMS_MSG_ERROR("sr_qmi_s_proxy NULL");
      ret = SERVREG_INVALID_HANDLE;
   }

   servreg_mutex_unlock_dal(&servreg_qmi_server_internal.mutex);

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_qmi_validate_connection_handle
 *
 * Description:
 *     Validate the qmi connection handle received.
 *
 * Parameters:
 *     sconnection_handle : QMI connection handle
 *
 * Returns:
 *    SERVREG_TRUE or SERVREG_FALSE
 * =====================================================================  */
static SERVREG_BOOL servreg_qmi_validate_connection_handle(void *connection_handle)
{
   sr_client_type_p client = SERVREG_NULL;
   SERVREG_BOOL ret = SERVREG_FALSE;
   
   servreg_mutex_lock_dal(&servreg_qmi_server_internal.mutex);

   client = servreg_qmi_server_internal.qmi_notif_client_head;

   /* Validate handle. */
   if(connection_handle != NULL)
   {
      while(SERVREG_NULL != client)
      {
         if(client == (sr_client_type_p)connection_handle)
         {
            ret = SERVREG_TRUE;
            break;
         }
         client = client->next;
      }
   }
   else
   {
      TMS_MSG_ERROR("connection_handle NULL");
   }

   servreg_mutex_unlock_dal(&servreg_qmi_server_internal.mutex);

   return ret;
}


/** =====================================================================
 * Function:
 *     servreg_get_asid_from_proc_spc
 *
 * Description:
 *     Match the process specifier in servreg handle from pd_mon_internal
       if matched then get the asid and return
 *
 * Parameters:
        cahr *proc_spc
 *      out parm: asid
 * Returns:
        SERVREG_FALSE
        SERVREG_TRUE
 * =====================================================================  */


int servreg_get_asid_from_proc_spc(char* proc_spc, int *asid)
{

    int len1 = servreg_nmelen(proc_spc);
    int len2 = 0;
    int i;

    TMS_MSG_SPRINTF_HIGH_1("Process specifier: %s", proc_spc);

    for (i=0; i< PD_MON_MAX_PD; i++)
    {
        if (pd_mon_internal_get_asid(i) != -1)
        {
            len2 = servreg_nmelen(((servreg_mon_node_p)pd_mon_internal_get_servreg_h(i))->service_name);
            if (len1 == len2 && 0 == servreg_nmecmp(((servreg_mon_node_p)pd_mon_internal_get_servreg_h(i))->service_name, proc_spc, len1))
            {
               *asid = pd_mon_internal_get_asid(i);
               
               TMS_MSG_HIGH_1("ASID: %d", *asid);
            
               return SERVREG_TRUE;
            }
        }
    }

    return SERVREG_FALSE;
}


/** =====================================================================
 * Function:
 *     servreg_send_qmi_ind_msg_state_change
 *
 * Description:
 *     Check which notif node state has changed and return that notif node's handle
 *
 * Parameters:
 *     servreg_qmi_server_internal.servreg_qmi_notif_server_list_head : Head pointer of the notifier handle list for the user PD.
 *
 * Returns:
 *    sr_qmi_s_proxy_p : handle to the sr notif node
 * =====================================================================  */
void servreg_send_qmi_ind_msg_state_change(void)
{
   sr_qmi_s_proxy_p sr_qmi_s_proxy = SERVREG_NULL;
   sr_qmi_s_client_p sr_qmi_s_client = SERVREG_NULL;
   SERVREG_SERVICE_STATE new_curr_state;
   qmi_csi_error err;
   qmi_servreg_notif_state_updated_ind_msg_v01 ind;
   SERVREG_NAME service_name = SERVREG_NULL;
   servreg_mon_queue_p sr_q_entry = SERVREG_NULL;
   servreg_mon_node_p sr_mon_node = SERVREG_NULL;

   servreg_mutex_lock_dal(&servreg_qmi_server_internal.mutex);

   sr_qmi_s_proxy = servreg_qmi_server_internal.servreg_qmi_notif_server_list_head;

   while(SERVREG_NULL != sr_qmi_s_proxy)
   {
      new_curr_state = servreg_get_service_curr_state(sr_qmi_s_proxy->sr_mon_handle);

      if(sr_qmi_s_proxy->curr_state != new_curr_state)
      {
         /* Send IND message to all the qmi clients if the notif node state has changed */
         sr_qmi_s_proxy->curr_state = new_curr_state;
         
         SERVREG_TRACE_2(STATE_CHANGE_IND, sr_qmi_s_proxy->sr_mon_handle, new_curr_state);

         ind.curr_state = (qmi_servreg_notif_service_state_enum_type_v01)new_curr_state;
         service_name = servreg_get_service_name(sr_qmi_s_proxy->sr_mon_handle);

         if(SERVREG_NULL != service_name)
         {
            strlcpy(ind.service_name, service_name, QMI_SERVREG_NOTIF_NAME_LENGTH_V01+1);
            ind.transaction_id = sr_qmi_s_proxy->transaction_id = servreg_get_transaction_id(sr_qmi_s_proxy->sr_mon_handle);

            /* Get the head of the qmi client list */
            sr_qmi_s_client = sr_qmi_s_proxy->qmi_client_list_head;

            /* Walk through the qmi_client list and send out notifications via IND QMI msg */
            while(SERVREG_NULL != sr_qmi_s_client)
            {
               /* Save the transaction id for each qmi client */
               sr_qmi_s_client->transaction_id = sr_qmi_s_proxy->transaction_id;
               SERVREG_TRACE_2(QMI_SEND, sr_qmi_s_proxy->sr_mon_handle, sr_qmi_s_client->client_handle);

               err = qmi_csi_send_ind((sr_qmi_s_client->client_handle),
                                      SERVREG_NOTIF_STATE_UPDATED_IND,
                                      &ind,
                                      sizeof(qmi_servreg_notif_state_updated_ind_msg_v01));

               if (err == 0)
               {
                  TMS_MSG_HIGH_1("QMI IND TX to: 0x%x", sr_qmi_s_client->client_handle);
               }
               else
               {
                  TMS_MSG_HIGH_1("QMI IND TX failed to: 0x%x", sr_qmi_s_client->client_handle);
               }

               SERVREG_TRACE_2(RESULT, err, 0);
               sr_qmi_s_client = sr_qmi_s_client->next;
            }
         }

         /* Keep checking each proxy in case its state has changed even though node signaled was found */
      }
      else
      {
        sr_mon_node = (servreg_mon_node_p)sr_qmi_s_proxy->sr_mon_handle;
        if(SERVREG_NULL != sr_mon_node)
        {
          sr_q_entry = (servreg_mon_queue_p)q_check(sr_mon_node->state_q);
       
          if(SERVREG_NULL != sr_q_entry)
          {
            if(0 == sr_qmi_s_proxy->transaction_id)
            {
              /* An entry in the queue suggests here that the head q_entry state is
              set but its not awaiting for the QMI ACKs because client already got
              updated state. */
              SERVREG_TRACE_2(STATE_CHANGE_IND_E, sr_mon_node, sr_q_entry);
              sr_qmi_s_proxy->transaction_id = servreg_get_transaction_id(sr_qmi_s_proxy->sr_mon_handle);
              servreg_qmi_handle_false_ack_req(sr_qmi_s_proxy,sr_qmi_s_proxy->transaction_id);
            }
          }
        }
      }
      sr_qmi_s_proxy = sr_qmi_s_proxy->next;
   }

   servreg_mutex_unlock_dal(&servreg_qmi_server_internal.mutex);

   return;
}

/** =====================================================================
 * Function:
 *     servreg_notif_server_process_req
 *
 * Description:
 *     This function processes incoming requests for the QMI servreg_notif
 *     services handler.
 *
 * Parameters:
 *      connection_handle: Client connection making request.
 *      req_handle       : QMI request handle
 *      msg_id           : QMI msg request ID
 *      req_c_struct     : QMI request structure
 *      req_c_struct_len : QMI request structure length
 *      service_cookie   : Ignored
 *
 * Returns:
 *     qmi_csi_cb_error
 * =====================================================================  */
qmi_csi_cb_error servreg_notif_server_process_req(void *connection_handle,
                                          qmi_req_handle  req_handle,
                                          unsigned int    msg_id,
                                          void           *req_c_struct,
                                          unsigned int    req_c_struct_len,
                                          void           *service_cookie)
{
   qmi_csi_cb_error retval = QMI_CSI_CB_NO_ERR;
   sr_client_type_p client = (sr_client_type_p)connection_handle;
   SERVREG_RESULT ret = SERVREG_FAILURE;
   sr_qmi_s_proxy_p sr_qmi_s_proxy = SERVREG_NULL;
   SERVREG_MON_HANDLE sr_mon_handle = SERVREG_NULL;

   SERVREG_TRACE_0(QMI_ENTER);

   if(NULL != req_handle)
   {
      if(SERVREG_TRUE == servreg_qmi_validate_connection_handle(connection_handle) && SERVREG_TRUE == client->connected)
      {
         SERVREG_TRACE_2(QMI_MSG, client->client_handle, msg_id);
         switch(msg_id)
         {
            case SERVREG_NOTIF_REGISTER_LISTENER_REQ:
            {
               qmi_servreg_notif_register_listener_req_msg_v01 *request = (qmi_servreg_notif_register_listener_req_msg_v01 *)(req_c_struct);
               qmi_servreg_notif_register_listener_resp_msg_v01 resp_send;
               //qmi_csi_error err;

               /* Check if the service belongs to the correct subsystem*/
               if(SERVREG_TRUE == servreg_is_domain_local(request->service_name))
               {
                  /* Register Listener */
                  if(SERVREG_TRUE == request->enable)
                  {
                     //create/lookup singleton rcesn and servreg entries
                     sr_mon_handle = servreg_alloc_monitor_handle(request->service_name, SERVREG_NULL);

                     // check for duplicate client registration
                     // note: not worrying about being atomic (a duplicate registering between check and register)
                     //       since a given client should only be sending one registration request
                     if(SERVREG_NULL != sr_qmi_get_proxy_iff_listener(sr_mon_handle, connection_handle, SERVREG_NULL))
                     {
                        //this client handle is already registered as a listener for the service
                        SERVREG_TRACE_2(QMI_REG_DUP, sr_mon_handle, client->client_handle);
                        //return success as we do not know how HLOS will handle failure (yes we asked)
                        resp_send.curr_state_valid = 1;
                        resp_send.curr_state = (qmi_servreg_notif_service_state_enum_type_v01)servreg_get_service_curr_state(sr_mon_handle);
                        resp_send.resp.result = QMI_RESULT_SUCCESS_V01;
                        resp_send.resp.error  = QMI_ERR_NONE_V01;
                        TMS_MSG_ERROR("Duplicate QMI listener registration");
                     }
                     else //not a complete duplicate
                     {
                        //check if we are already listening for this service for a different client
                        //only want to proxy register into servreg proper once/service
                        if(SERVREG_NULL == sr_qmi_get_proxy(sr_mon_handle))
                        {
                           ret = sr_qmi_s_register_proxy_listener(sr_mon_handle);

                           SERVREG_TRACE_2(QMI_REG_NEW, sr_mon_handle, client->client_handle);
                        }
                        else //already registered, add new client only
                        {
                           ret = SERVREG_SUCCESS;
                           SERVREG_TRACE_2(QMI_REG_ADD, sr_mon_handle, client->client_handle);
                        }

                     if(SERVREG_SUCCESS == ret)
                     {
                        /* Create a qmi notif server node that keep tracks of all the remote qmi listener clients */
                           sr_qmi_s_proxy = sr_qmi_s_listener_attach(sr_mon_handle, client->client_handle);

                           if(SERVREG_NULL != sr_qmi_s_proxy)
                        {
                           resp_send.curr_state_valid = 1;
                           resp_send.curr_state = (qmi_servreg_notif_service_state_enum_type_v01)servreg_get_service_curr_state(sr_mon_handle);
                           resp_send.resp.result = QMI_RESULT_SUCCESS_V01;
                           resp_send.resp.error  = QMI_ERR_NONE_V01;
                           TMS_MSG_HIGH_1("QMI REGISTER_LISTENER_REQ success, state: 0x%x", resp_send.curr_state);
                        }
                        else
                        {
                           ERR_FATAL("Create QMI notif server node failed", 0, 0, 0);
                        }
                     }
                     else //failed to register with servreg framework
                     {
                        SERVREG_TRACE_0(FAIL);

                        TMS_MSG_ERROR("REGISTER_LISTENER_REQ failed");

                        ret = servreg_free_monitor_handle(sr_mon_handle);

                        if(SERVREG_SUCCESS != ret)
                        {
                           TMS_MSG_ERROR("Could not free sr_mon_handle");
                        }

                           resp_send.resp.result = QMI_RESULT_FAILURE_V01;
                        resp_send.resp.error  = QMI_ERR_INVALID_HANDLE_V01;
                        } //end failed to register with servreg framework
                     } // end not a complete duplicate
                  } // end register listener
                  else /* Deregister Listener */
                  {
                     if (SERVREG_SUCCESS == servreg_qmi_handle_dereg_req(request, client->client_handle))
                     {
                        SERVREG_TRACE_0(SUCCESS);
                        resp_send.resp.result = QMI_RESULT_SUCCESS_V01;
                        resp_send.resp.error  = QMI_ERR_NONE_V01;
                     }
                     else
                     {
                        SERVREG_TRACE_0(FAIL);
                        resp_send.resp.result = QMI_RESULT_FAILURE_V01;
                        resp_send.resp.error  = QMI_ERR_INVALID_HANDLE_V01;
                     }
                  }
               }
               else // !servreg_is_domain_local()
               {
                  resp_send.resp.result = QMI_RESULT_FAILURE_V01;
                  resp_send.resp.error  = QMI_ERR_INVALID_DATA_FORMAT_V01;
               }

               qmi_csi_send_resp(req_handle, msg_id, &resp_send, sizeof(qmi_servreg_notif_register_listener_resp_msg_v01));

            }
            break;

            case SERVREG_NOTIF_QUERY_STATE_REQ:
            {
               qmi_servreg_notif_query_state_req_msg_v01 *request = (qmi_servreg_notif_query_state_req_msg_v01 *)(req_c_struct);
               qmi_servreg_notif_query_state_resp_msg_v01 resp_send;

               /* First check if the monitor node exists */
               sr_mon_handle = servreg_get_sr_mon_handle(request->service_name);
               if(SERVREG_NULL != sr_mon_handle)
               {
                  TMS_MSG_HIGH("Get STATE_REQ success");
                  resp_send.curr_state_valid = 1;
                  resp_send.curr_state = (qmi_servreg_notif_service_state_enum_type_v01)servreg_get_service_curr_state(sr_mon_handle);
                  resp_send.resp.result = QMI_RESULT_SUCCESS_V01;
                  resp_send.resp.error  = QMI_ERR_NONE_V01;
               }
               else
               {
                  TMS_MSG_HIGH("Get STATE_REQ failure");
                  resp_send.curr_state_valid = 1;
                  resp_send.curr_state = SERVREG_NOTIF_SERVICE_STATE_UNINIT_V01;
                  resp_send.resp.result = QMI_RESULT_SUCCESS_V01;
                  resp_send.resp.error  = QMI_ERR_NONE_V01;
               }

               qmi_csi_send_resp(req_handle, msg_id, &resp_send, sizeof(qmi_servreg_notif_query_state_resp_msg_v01));
            }
            break;

            case SERVREG_NOTIF_STATE_UPDATED_IND_ACK_REQ:
            {
               qmi_servreg_notif_set_ack_req_msg_v01 *request = (qmi_servreg_notif_set_ack_req_msg_v01 *)(req_c_struct);
               qmi_servreg_notif_set_ack_resp_msg_v01 resp_send;

               if(SERVREG_SUCCESS == servreg_qmi_handle_ack_req(request, client->client_handle))
               {
                  SERVREG_TRACE_0(QMI_ACK_RX_SUCCESS);
                  TMS_MSG_HIGH_1("QMI ACK RX from: 0x%x", client->client_handle);
                  resp_send.resp.result = QMI_RESULT_SUCCESS_V01;
                  resp_send.resp.error  = QMI_ERR_NONE_V01;
               }
               else
               {
                  SERVREG_TRACE_0(QMI_ACK_RX_FAIL);
                  TMS_MSG_HIGH_1("QMI ACK RX failed from: 0x%x", client->client_handle);
                  resp_send.resp.result = QMI_RESULT_FAILURE_V01;
                  resp_send.resp.error  = QMI_ERR_INVALID_HANDLE_V01;
               }
               //send response
               qmi_csi_send_resp(req_handle, msg_id, &resp_send, sizeof(qmi_servreg_notif_set_ack_resp_msg_v01));
            }
            break;

            case SERVREG_NOTIF_RESTART_PD_REQ:
            {
                qmi_servreg_notif_restart_pd_req_msg_v01 *request = (qmi_servreg_notif_restart_pd_req_msg_v01*)(req_c_struct);
                qmi_servreg_notif_restart_pd_resp_msg_v01 resp_send;
                int asid, restart_pd = SERVREG_FALSE;

                TMS_MSG_HIGH("PDR request received");
                
                if (SERVREG_FALSE == servreg_get_asid_from_proc_spc(request->process_spc_name, &asid))
                {
                    resp_send.resp.result = QMI_RESULT_FAILURE_V01;
                    resp_send.resp.error  = QMI_ERR_INVALID_HANDLE_V01;
                    TMS_MSG_HIGH("Failed to get ASID");
                }
                else if (SERVREG_FALSE == err_qdi_check_pdr_enabled(asid)) //Check if PDR is NOT enabled for that PD and subsystem
                {
                    resp_send.resp.result = QMI_RESULT_FAILURE_V01;
                    resp_send.resp.error  = QMI_ERR_DISABLED_V01;
                    TMS_MSG_HIGH("PDR DISABLED");
                }
                else /* SERVREG_TRUE */
                {
                    resp_send.resp.result = QMI_RESULT_SUCCESS_V01;
                    resp_send.resp.error  = QMI_ERR_NONE_V01;
                    restart_pd = SERVREG_TRUE; //Flag set if PD can be restarted
                    TMS_MSG_HIGH("Proceeding with PDR");
                }

                qmi_csi_send_resp(req_handle, msg_id, &resp_send, sizeof(qmi_servreg_notif_restart_pd_resp_msg_v01));
                
                /*after sending the response to the client now do restart the PD if the process specifier was identified*/
                if(SERVREG_TRUE == restart_pd)
                {
                    qurt_qdi_handle_invoke(QDI_HANDLE_GENERIC, QDI_OS_PROCESS_KILL, asid, 0);
                }
            }
            break;

            default:
            {
               qmi_response_type_v01 resp_send;

               TMS_MSG_ERROR_1("Unhandled QMI request id: 0x%x", msg_id);

               resp_send.result = QMI_RESULT_FAILURE_V01;
               resp_send.error  = QMI_ERR_INVALID_MESSAGE_ID_V01;
               qmi_csi_send_resp(req_handle, msg_id, &resp_send, sizeof(qmi_response_type_v01));
            }
            break;
         }
      }
      else
      {
         TMS_MSG_ERROR("connection_handle invalid");
         retval = QMI_CSI_CB_INTERNAL_ERR;
      }
  }
  else
  {
      TMS_MSG_ERROR("req_handle NULL");
      retval = QMI_CSI_CB_INTERNAL_ERR;
  }

   SERVREG_TRACE_2(QMI_EXIT, connection_handle, msg_id);
   return retval;
}

/** =====================================================================
 * Function:
 *     servreg_notif_server_disconnect
 *
 * Description:
 *       This function processes incoming requests for the servreg_notif 
 *       QMI services to disconnect a client.
 *
 * Parameters:
 *      connection_handle: Client connection to disconnect
 *      service_cookie   : Ignored
 *
 * Returns:
 *     none
 * =====================================================================  */
void servreg_notif_server_disconnect(void *connection_handle, void *service_cookie)
{
   sr_client_type_p client_curr = SERVREG_NULL, client_prev = SERVREG_NULL;
   sr_qmi_s_proxy_p sr_qmi_s_proxy = SERVREG_NULL;
   sr_qmi_s_proxy_p sr_qmi_s_proxy_next = SERVREG_NULL;
   sr_qmi_s_client_p sr_qmi_s_client = SERVREG_NULL;
   qmi_client_handle client_h = SERVREG_NULL;

   servreg_mutex_lock_dal(&servreg_qmi_server_internal.mutex);
   
   client_curr = servreg_qmi_server_internal.qmi_notif_client_head;
   
   if(SERVREG_NULL != connection_handle)
   {
      while(SERVREG_NULL != client_curr)
      {
         if(client_curr == (sr_client_type_p)connection_handle && (SERVREG_FALSE != client_curr->connected))
         {
            if(SERVREG_NULL == client_prev)
            {
               /* Delete the first node */
               servreg_qmi_server_internal.qmi_notif_client_head = client_curr->next;
            }
            else
            {
               client_prev->next = client_curr->next;
            }

            client_curr->next = SERVREG_NULL;
            client_h = client_curr->client_handle;
            /* Delete the client node */
            servreg_free(client_curr);
            TMS_MSG_HIGH("Client released connection");
            break; /* exit while() loop */
         }

         client_prev = client_curr;
         client_curr = client_curr->next;
      }

      /* Also delete the reference to the qmi client handle in the qmi server table and ERR FATAL , if ack pending*/
      sr_qmi_s_proxy = servreg_qmi_server_internal.servreg_qmi_notif_server_list_head;

      while(SERVREG_NULL != sr_qmi_s_proxy)
      {
        sr_qmi_s_proxy_next = sr_qmi_s_proxy->next; //sr_qmi_s_proxy may get free while listener_detach
        sr_qmi_s_client = sr_qmi_s_proxy->qmi_client_list_head;

        /* Walk through the qmi_client list and find the matching qmi_client handle */
        while(SERVREG_NULL != sr_qmi_s_client)
        {
          if(client_h == sr_qmi_s_client->client_handle)
          {
            /* check if the client has an outstanding ack */
            if (sr_qmi_s_client->last_ack_id < sr_qmi_s_client->transaction_id)
            {
               SERVREG_TRACE_2(QMI_DISCONN_ACK_MISS, sr_qmi_s_proxy->transaction_id, sr_qmi_s_client->last_ack_id);
               TMS_MSG_ERROR_2("QMI client disconnected with outstanding ack, 0x%x, 0x%x ",client_h,sr_qmi_s_proxy);
               sr_qmi_s_client->last_ack_id = sr_qmi_s_client->transaction_id; //set ack
               sr_qmi_check_upstream_ack(sr_qmi_s_proxy);                      //check if this was only pending client
            }

            /* Delete the client handle from the qmi notif server node and garbage collect */
            sr_qmi_s_listener_detach(sr_qmi_s_proxy, client_h);
            break; //since in one proxy , repetition of client_handle is not possible
          }
          sr_qmi_s_client = sr_qmi_s_client->next;
        }
        sr_qmi_s_proxy = sr_qmi_s_proxy_next;
      }
   }
   else
   {
     TMS_MSG_ERROR("connection_handle NULL");
   }

   servreg_mutex_unlock_dal(&servreg_qmi_server_internal.mutex);
   return;
}

/** =====================================================================
 * Function:
 *     servreg_notif_server_connect
 *
 * Description:
 *       This function processes incoming requests for the servreg_notif 
 *       QMI services to connect a client.
 *
 * Parameters:
 *      client_handle    : Handle used by the infrastructure
 *                         to identify different services.
 *      service_cookie   : Ignored
 *      connection_handle: Client connection to created
 *                         internally.
 *
 * Returns:
 *     qmi_csi_cb_error
 * =====================================================================  */
qmi_csi_cb_error servreg_notif_server_connect(qmi_client_handle client_handle, void *service_cookie, void **connection_handle)
{
   qmi_csi_cb_error retval = QMI_CSI_CB_NO_MEM;
   sr_client_type_p client = SERVREG_NULL; //(sr_client_type*)(&servreg_qmi_server_internal.qmi_notif_client_head);

   client = (sr_client_type_p)servreg_calloc(sizeof(sr_client_type_t), 1);

   if(SERVREG_NULL != client)
   {
      /* Found empty slot */
      client->client_handle = client_handle;
      client->connected = SERVREG_TRUE;
      *connection_handle = client;
      client->next = SERVREG_NULL;
      
      servreg_mutex_lock_dal(&servreg_qmi_server_internal.mutex);

      /* Insert to the head of the list */
      client->next = servreg_qmi_server_internal.qmi_notif_client_head;
      /* Update head */
      servreg_qmi_server_internal.qmi_notif_client_head = client;
      
      servreg_mutex_unlock_dal(&servreg_qmi_server_internal.mutex);

      retval = QMI_CSI_CB_NO_ERR;
   }
   else
   {
      TMS_MSG_ERROR("Calloc failed and connection failed with server");
   }

   return retval;
}

/** =====================================================================
 * Function:
 *     servreg_qmi_server_init
 *
 * Description:
 *     This function initializes the QMI servreg_notif services handler.
 *
 * Parameters:
 *    os_params: OS parameters.
 *
 * Returns:
 *     qmi_csi_service_handle
 * =====================================================================  */
qmi_csi_service_handle servreg_qmi_server_init(qmi_csi_os_params *os_params)
{
   qmi_idl_service_object_type servreg_notif_server_obj = servreg_notif_get_service_object_v01();
   qmi_csi_options options;
   qmi_csi_error qmi_csi_return;
   unsigned int ser_instance_id;

   servreg_qmi_server_internal.servreg_notif_qmi_server_handler = NULL;
   servreg_qmi_server_internal.qmi_notif_client_head = SERVREG_NULL;
   servreg_qmi_server_internal.servreg_qmi_notif_server_list_head = SERVREG_NULL;

   QMI_CSI_OPTIONS_INIT(options);

   ser_instance_id = servreg_get_local_instance_id();

   QMI_CSI_OPTIONS_SET_INSTANCE_ID(options,ser_instance_id);

   qmi_csi_return = qmi_csi_register_with_options(servreg_notif_server_obj,
                                                  servreg_notif_server_connect,
                                                  servreg_notif_server_disconnect,
                                                  servreg_notif_server_process_req,
                                                  NULL,
                                                  os_params,
                                                  &options,
                                                  &servreg_qmi_server_internal.servreg_notif_qmi_server_handler);

   if(QMI_CSI_NO_ERR != qmi_csi_return)
   {
      TMS_MSG_ERROR("QMI notifier server init failure");
   }
   else
   {
      TMS_MSG_HIGH("QMI notif server init");
   }

   return servreg_qmi_server_internal.servreg_notif_qmi_server_handler;
}

/** =====================================================================
 * Function:
 *     servreg_qmi_handle_ack_req
 *
 * Description:
 *     This function handles incoming ack's from QMI clients.
 *
 * Parameters: 
 *    qmi_servreg_notif_set_ack_req_msg_v01 *request
 *    qmi_client_handle client_h
 *
 * Returns:
 *     SERVREG_RESULT
 * =====================================================================  */
static SERVREG_RESULT servreg_qmi_handle_ack_req(qmi_servreg_notif_set_ack_req_msg_v01 *request,
                                          qmi_client_handle client_h)
{
   SERVREG_MON_HANDLE sr_mon_handle;
   sr_qmi_s_proxy_p sr_qmi_s_proxy;

   /* First check if the monitor node exists */
   sr_mon_handle = servreg_get_sr_mon_handle(request->service_name);
   SERVREG_TRACE_2(QMI_ACK, sr_mon_handle, client_h);
   if (SERVREG_NULL == sr_mon_handle)
   {
      return SERVREG_FAILURE;
   }

   /* find the QMI proxy for given service*/
   sr_qmi_s_proxy = sr_qmi_get_proxy(sr_mon_handle);

   return (sr_qmi_set_ack(sr_qmi_s_proxy, client_h, request->transaction_id));
}

/** =====================================================================
 * Function:
 *     servreg_qmi_handle_false_ack_req
 *
 * Description:
 *     This function handles incoming false qmi ack.
 *
 * Parameters: 
 *    sr_qmi_s_proxy_p sr_qmi_s_proxy
 *    int32_t transaction_id
 *
 * Returns:
 *     SERVREG_RESULT
 * =====================================================================  */
static SERVREG_RESULT servreg_qmi_handle_false_ack_req(sr_qmi_s_proxy_p sr_qmi_s_proxy, int32_t transaction_id)
{
   sr_qmi_s_client_p sr_qmi_s_client = SERVREG_NULL;
   sr_qmi_s_client_p sr_qmi_s_client_last = SERVREG_NULL;

   if(SERVREG_NULL == sr_qmi_s_proxy)
   {
     TMS_MSG_ERROR("sr_qmi_s_proxy NULL");
     return SERVREG_FAILURE;
   }

   /* find the first client which registered for this proxy,it will be last
      node in the list */
   sr_qmi_s_client =  sr_qmi_s_proxy->qmi_client_list_head;
   while (SERVREG_NULL != sr_qmi_s_client)
   {
     sr_qmi_s_client_last = sr_qmi_s_client;
     sr_qmi_s_client = sr_qmi_s_client->next;
   }

   if(SERVREG_NULL == sr_qmi_s_client_last)
   {
     TMS_MSG_ERROR("sr_qmi_s_client_last NULL");
     return SERVREG_FAILURE;
   }

   sr_qmi_s_client_last->transaction_id = transaction_id;
   SERVREG_TRACE_2(QMI_FALSE_ACK, sr_qmi_s_proxy, sr_qmi_s_client_last->client_handle);
   SERVREG_TRACE_2(QMI_FALSE_ACK, sr_qmi_s_proxy, transaction_id);
   return (sr_qmi_set_ack(sr_qmi_s_proxy, sr_qmi_s_client_last->client_handle, transaction_id));
}

/** =====================================================================
 * Function:
 *     servreg_qmi_handle_dereg_req
 *
 * Description:
 *     This function handles incoming dereg's from QMI clients.
 *
 * Parameters: 
 *    qmi_servreg_notif_register_listener_req_msg_v01 *request
 *    qmi_client_handle client_h
 *
 * Returns:
 *     SERVREG_RESULT
 * =====================================================================  */
static SERVREG_RESULT servreg_qmi_handle_dereg_req(qmi_servreg_notif_register_listener_req_msg_v01 *request,
                                                   qmi_client_handle client_h)
{
   SERVREG_MON_HANDLE sr_mon_handle;
   sr_qmi_s_proxy_p sr_qmi_s_proxy;
   sr_qmi_s_client_p sr_qmi_s_client;

   sr_mon_handle = servreg_get_sr_mon_handle(request->service_name);
   SERVREG_TRACE_2(QMI_DEREG, sr_mon_handle, client_h);
   if (SERVREG_NULL == sr_mon_handle)
   {
      return SERVREG_FAILURE;
   }

   servreg_mutex_lock_dal(&servreg_qmi_server_internal.mutex);

   /* Check if the remote qmi client has registered as a listener */
   sr_qmi_s_proxy = sr_qmi_get_proxy_iff_listener(sr_mon_handle, client_h, &sr_qmi_s_client);

   if(SERVREG_NULL != sr_qmi_s_proxy)
   {
      /* check if the client has an outstanding ack */
      if (sr_qmi_s_client->last_ack_id < sr_qmi_s_client->transaction_id)
      {
         SERVREG_TRACE_2(QMI_DEREG_ACK_MISS, sr_qmi_s_proxy->transaction_id, sr_qmi_s_client->last_ack_id);
         TMS_MSG_ERROR("Deregister with outstanding ack");
         sr_qmi_s_client->last_ack_id = sr_qmi_s_client->transaction_id; //set ack
         sr_qmi_check_upstream_ack(sr_qmi_s_proxy);                     //check if this was only pending client
      }

      servreg_mutex_unlock_dal(&servreg_qmi_server_internal.mutex);
      /* Delete the client handle from the qmi notif server node and garbage collect */
      return(sr_qmi_s_listener_detach(sr_qmi_s_proxy, client_h));
   }
   else
   {
      servreg_mutex_unlock_dal(&servreg_qmi_server_internal.mutex);
      SERVREG_TRACE_0(FAIL);
      return SERVREG_FAILURE;
   }
}

/** =====================================================================
 * Function:
 *     sr_qmi_set_ack
 *
 * Description:
 *     update the last transaction ack'd field for a specific
 *     listener client and check the proxy for upstream ack
 *
 * Parameters: 
 *    sr_qmi_s_proxy_p sr_qmi_s_proxy
 *    qmi_client_handle client_h
 *    int32_t ack_id
 *
 * Returns:
 *     SERVREG_RESULT
 * =====================================================================  */
static SERVREG_RESULT sr_qmi_set_ack(sr_qmi_s_proxy_p sr_qmi_s_proxy, qmi_client_handle client_h, int32_t ack_id)
{
   sr_qmi_s_client_p curr_client_p;
   SERVREG_RESULT ret = SERVREG_FAILURE;
   int32_t transaction_id;

   servreg_mutex_lock_dal(&servreg_qmi_server_internal.mutex);

   if (SERVREG_NULL != sr_qmi_s_proxy)
   {

      curr_client_p = sr_qmi_s_proxy->qmi_client_list_head;
      transaction_id = sr_qmi_s_proxy->transaction_id;

      //validate active transaction
      if (transaction_id != ack_id)
      {
         SERVREG_TRACE_2(QMI_BAD_TXID, transaction_id, ack_id);
         servreg_mutex_unlock_dal(&servreg_qmi_server_internal.mutex);
         return SERVREG_FAILURE;
      }

      /* Walk through the qmi_client list */
      while(SERVREG_NULL != curr_client_p)
      {
         if(client_h == curr_client_p->client_handle)
         {
            // update the client sending the ack
            if (curr_client_p->last_ack_id == transaction_id)
            {
               //duplicate: any work needing to be done has been done so bail
               SERVREG_TRACE_0(QMI_ACK_DUP);
               TMS_MSG_ERROR("Duplicate qmi_servreg ack received");
               servreg_mutex_unlock_dal(&servreg_qmi_server_internal.mutex);
               return SERVREG_FAILURE;
            }
            else
            {
               curr_client_p->last_ack_id = transaction_id;
               ret = SERVREG_SUCCESS;
               break;
            }
         }
	 curr_client_p = curr_client_p->next;

      } //end qmi_client list
   }

   //if we made it here && ack was valid, check for upstream the ack
   if (SERVREG_SUCCESS == ret)
   {
      sr_qmi_check_upstream_ack(sr_qmi_s_proxy);
   }
   else
   {
      SERVREG_TRACE_0(FAIL);
      TMS_MSG_ERROR("Spurious QMI ack received and dropped");
   }

   servreg_mutex_unlock_dal(&servreg_qmi_server_internal.mutex);

   return ret;
}

/** =====================================================================
 * Function:
 *     sr_qmi_check_upstream_ack
 *
 * Description:
 *     Helper function to send ack to servreg framework if proxy
 *     has acks from all listeners and upstream ack pending
 *
 * Parameters: 
 *    sr_qmi_s_proxy_p sr_qmi_s_proxy
 *
 * Returns:
 *     SERVREG_BOOL
 * =====================================================================  */
static SERVREG_BOOL sr_qmi_check_upstream_ack(sr_qmi_s_proxy_p sr_qmi_s_proxy)
{
   sr_qmi_s_client_p curr_client_p;
   SERVREG_MON_HANDLE sr_mon_handle;
   SERVREG_BOOL ret = SERVREG_FALSE;
   int32_t transaction_id;

   servreg_mutex_lock_dal(&servreg_qmi_server_internal.mutex);

   if (SERVREG_NULL != sr_qmi_s_proxy)
   {
      transaction_id = sr_qmi_s_proxy->transaction_id;

      // already up-to-date?
      if (transaction_id == sr_qmi_s_proxy->last_ack_id)
      {
         servreg_mutex_unlock_dal(&servreg_qmi_server_internal.mutex);
         return SERVREG_FALSE;
      }
     
      curr_client_p = sr_qmi_s_proxy->qmi_client_list_head;

      /* Walk through the qmi_client list */
      while(SERVREG_NULL != curr_client_p)
      {         
         // abort if any expected clients have acks pending
         // by checking last transaction id for each client
         if (transaction_id == curr_client_p->transaction_id && 
         curr_client_p->last_ack_id != curr_client_p->transaction_id)
         {
            servreg_mutex_unlock_dal(&servreg_qmi_server_internal.mutex);
            return SERVREG_FALSE;
         }    
         curr_client_p = curr_client_p->next;    
      } //end qmi_client list

      sr_mon_handle = sr_qmi_s_proxy->sr_mon_handle; //capture to stack before releasing mutex

       //if we made it here, upstream the ack
      SERVREG_TRACE_2(QMI_ACK_ALL, sr_mon_handle, transaction_id);
      sr_qmi_s_proxy->last_ack_id = transaction_id;
      ret = servreg_set_ack(sr_mon_handle, transaction_id);
   }

   servreg_mutex_unlock_dal(&servreg_qmi_server_internal.mutex);

   return ret;
}

