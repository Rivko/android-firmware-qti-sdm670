/*
#============================================================================
#  Name:
#    servreg_qmi_notifier_client.c
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
#include "qmi_client.h"
#include "qmi_cci_target_ext.h"
#include "timer.h"
#include "rcecb.h"
#include "rcinit.h"

#include "service_registry_notifier_v01.h"
#include "servreg_internal.h"
#include "servreg_locator.h"
#include "servreg_localdb.h"
#include "servreg_monitor.h"
#include "servreg_notifier.h"
#include "servreg_qmi_notifier_client.h"
#include "gen_cb_ctxt.h"

#define SERVREG_QMI_NOTIF_CLIENT_REQ_TIMEOUT_MS           1000
#define SERVREG_QMI_NOTIF_RETRY_TIME_MS                   200
#define SERVREG_QMI_NOTIF_MAX_RETRY_COUNT                 5
#define SERVREG_NOTIF_EVT_SYNC                            0x00000001        /* Indicates service down syncronization is complete */

#define SERVREG_QMI_REMOTE_ACK_REQ_SIG                    0x0002
#define SERVREG_QMI_REMOTE_QMI_IND_SIG                    0x0004

typedef enum
{
   SERVREG_NOTIF_CLIENT_REGISTER_LISTENER_REQ      = QMI_SERVREG_NOTIF_REGISTER_LISTENER_REQ_V01,
   SERVREG_NOTIF_CLIENT_STATE_UPDATED_IND_ACK_REQ  = QMI_SERVREG_NOTIF_STATE_UPDATED_IND_ACK_REQ_V01,
}servreg_notif_client_msg_req_type;

/* Flag indicating if the QMI client initialization is done */
static SERVREG_SERVICE_STATE servreg_qmi_notifier_client_init_status = SERVREG_SERVICE_STATE_UNINIT;

/* Service Registry QMI Remote Request Queue entries */
q_type servreg_remote_ack_req_q, servreg_remote_qmi_ind_q;
qurt_anysignal_t servreg_remote_ack_signal;

typedef struct servreg_remote_ack_req_s
{
   /* Link for the next queue */
   q_link_type link;

   SERVREG_NAME service_name;

   SERVREG_MON_HANDLE mon_handle;

   /* SERVREG_SERVICE_STATE service_state; */

   int32_t transaction_id;

} servreg_remote_ack_req_t, *servreg_remote_ack_req_p;

typedef struct servreg_remote_qmi_ind_s
{
   /* Link for the next queue */
   q_link_type link;

   qmi_servreg_notif_state_updated_ind_msg_v01 rcvd_ind_qmi_msg;

} servreg_remote_qmi_ind_t, *servreg_remote_qmi_ind_p;

/* QMI message related and keeping it global because it should be valid for the lifetime of the user_handle */
static qmi_idl_service_object_type servreg_notif_service_object;
static qmi_client_type servreg_qmi_client_notifier;

/* Service Registry Notifier QMI entry table */
struct servreg_qmi_entry_table_s
{
   SERVREG_NAME domain_name;

   uint32_t qmi_instance_id;

   qmi_service_info servreg_notif_server_info;

   qmi_client_type servreg_qmi_notifier_client_handle;

   struct servreg_qmi_entry_table_s* next;
};
typedef struct servreg_qmi_entry_table_s servreg_qmi_entry_table_t, * servreg_qmi_entry_table_p;

/* Service Registry Notifier ROOT node structure. List of SERVREG_MON_HANDLE'S*/
struct servreg_qmi_notif_client_node_s
{
   uint32_t notif_signature;

   SERVREG_MON_HANDLE sr_mon_handle;

   qurt_anysignal_t sync_sig;

   uint32_t server_retry_count;

   timer_type server_retry_timer;                       /* Server availability timer specific */

   timer_group_type server_retry_group;

   struct servreg_qmi_notif_client_node_s* next;
};
typedef struct servreg_qmi_notif_client_node_s servreg_qmi_notif_client_node_t, * servreg_qmi_notif_client_node_p;

/* Internal QMI Client Stucture */
struct servreg_qmi_client_internal_s
{
    qurt_mutex_t                    mutex;
   servreg_qmi_entry_table_p servreg_qmi_table_head;                    /* Head node for qmi table linked list */
   servreg_qmi_notif_client_node_p servreg_qmi_notif_client_list_head;  /* Head node of the servreg notification list */
};
struct servreg_qmi_client_internal_s servreg_qmi_client_internal;

/* Type casts as accessor functions */
#define sr_qmi_notif_client_node2sr_qmi_notif_client_handle(x)        ((SERVREG_QMI_NOTIF_CLIENT_HANDLE)x)
#define sr_qmi_notif_client_handle2sr_qmi_notif_client_node(x)        ((servreg_qmi_notif_client_node_p)x)

/* Pool allocation internals */
struct servreg_qmi_notif_client_node_pool_s
{
   struct servreg_qmi_notif_client_node_s servreg_qmi_notif_client_node_pool[SERVREG_QMI_NOTIF_CLIENT_NODE_POOL_SIZE];
   struct servreg_qmi_notif_client_node_pool_s * next;
};
typedef struct servreg_qmi_notif_client_node_pool_s servreg_qmi_notif_client_node_pool_t, * servreg_qmi_notif_client_node_pool_p;

struct servreg_qmi_notif_client_node_internal_s
{
   servreg_qmi_notif_client_node_pool_p servreg_qmi_notif_client_node_pool_head_p;
   servreg_qmi_notif_client_node_p servreg_qmi_notif_client_node_pool_free_p;
   servreg_mutex_t mutex;
   servreg_mutex_t mutex_create;
   SERVREG_BOOL dynamic_use;
   unsigned long init_flag;
};

struct servreg_qmi_notif_client_node_internal_s servreg_qmi_notif_client_node_internal;
servreg_qmi_notif_client_node_pool_t servreg_qmi_notif_client_node_pool_static;

/* Static functions in this file */
static servreg_qmi_notif_client_node_p servreg_qmi_notif_client_node_pool_init(void);
static servreg_qmi_notif_client_node_p servreg_qmi_notif_client_node_pool_alloc(void);
static servreg_qmi_notif_client_node_p servreg_qmi_notif_client_node_pool_free(servreg_qmi_notif_client_node_p servreg_qmi_notif_client_node);
static void servreg_qmi_notif_client_node_internal_init(void);
static SERVREG_RESULT servreg_qmi_connect_to_server(SERVREG_NAME domain_name);
static void servreg_ssr_down_event_cb(uint32_t sr_qmi_notif_handle_addr);
void servreg_notif_retry_timout_cb(void* sr_qmi_notif_handle_addr);
static void servreg_generic_task_cb(timer_cb_data_type sr_qmi_notif_handle_addr);
static SERVREG_RESULT servreg_reconnect_remote_listener(SERVREG_QMI_NOTIF_CLIENT_HANDLE sr_qmi_notif_client_handle);
static void servreg_ssr_up_event_cb(uint32_t sr_qmi_notif_handle_addr);
static void servreg_register_ssr_event(SERVREG_NAME service_name, SERVREG_QMI_NOTIF_CLIENT_HANDLE sr_qmi_notif_client_handle);
static void servreg_deregister_ssr_event(SERVREG_NAME service_name, SERVREG_QMI_NOTIF_CLIENT_HANDLE sr_qmi_notif_client_handle);
static servreg_qmi_entry_table_p servreg_get_qmi_table_entry(SERVREG_NAME domain_name);
static SERVREG_RESULT servreg_remote_queue_action(SERVREG_NAME service_name, SERVREG_MON_HANDLE sr_mon_handle, int32_t sr_transaction_id);
static SERVREG_RESULT sr_ind_process(qmi_servreg_notif_state_updated_ind_msg_v01* indication);

/** =====================================================================
 * Function:
 *     servreg_qmi_notif_client_node_pool_init
 *
 * Description:
 *     Initializes the memory pool for notifier node structure
 *
 * Parameters:
 *     None
 *
 * Returns:
 *    servreg_qmi_notif_client_node_p : Returns the first free notifier node space from the pool
 * =====================================================================  */
static servreg_qmi_notif_client_node_p servreg_qmi_notif_client_node_pool_init(void)
{
   servreg_qmi_notif_client_node_pool_p next_pool = SERVREG_NULL;

   if (SERVREG_NULL == servreg_qmi_notif_client_node_internal.servreg_qmi_notif_client_node_pool_head_p)
   {
      next_pool = &servreg_qmi_notif_client_node_pool_static;
   }
   else if (SERVREG_TRUE == servreg_qmi_notif_client_node_internal.dynamic_use)
   {
      next_pool = (servreg_qmi_notif_client_node_pool_p)servreg_calloc(sizeof(servreg_qmi_notif_client_node_pool_t), 1);
   }

   if (SERVREG_NULL != next_pool)
   {
      int i;

      for (i = 0; i < SERVREG_QMI_NOTIF_CLIENT_NODE_POOL_SIZE; i++)
      {
         if (i != (SERVREG_QMI_NOTIF_CLIENT_NODE_POOL_SIZE - 1))
         {
            next_pool->servreg_qmi_notif_client_node_pool[i].next = &(next_pool->servreg_qmi_notif_client_node_pool[i + 1]);
         }
         else
         {
            next_pool->servreg_qmi_notif_client_node_pool[i].next = servreg_qmi_notif_client_node_internal.servreg_qmi_notif_client_node_pool_free_p;
         }

         next_pool->servreg_qmi_notif_client_node_pool[i].notif_signature = SERVREG_QMI_NOTIF_CLIENT_SIGNATURE;
         next_pool->servreg_qmi_notif_client_node_pool[i].sr_mon_handle = SERVREG_NULL;
         next_pool->servreg_qmi_notif_client_node_pool[i].server_retry_count = 0;
         next_pool->servreg_qmi_notif_client_node_pool[i].server_retry_timer = 0;
         next_pool->servreg_qmi_notif_client_node_pool[i].server_retry_group = 0;
      }

      servreg_qmi_notif_client_node_internal.servreg_qmi_notif_client_node_pool_free_p = &(next_pool->servreg_qmi_notif_client_node_pool[0]);
      next_pool->next = servreg_qmi_notif_client_node_internal.servreg_qmi_notif_client_node_pool_head_p;
      servreg_qmi_notif_client_node_internal.servreg_qmi_notif_client_node_pool_head_p = next_pool;
   }
   else
   {
      return SERVREG_NULL;
   }

   return servreg_qmi_notif_client_node_internal.servreg_qmi_notif_client_node_pool_free_p;
}

/** =====================================================================
 * Function:
 *     servreg_qmi_notif_client_node_pool_alloc
 *
 * Description:
 *     Gives the first available free and allocated space from the memory
 *
 * Parameters:
 *     None
 *
 * Returns:
 *    servreg_qmi_notif_client_node_p : the first available free and allocated space from the memory
 * =====================================================================  */
static servreg_qmi_notif_client_node_p servreg_qmi_notif_client_node_pool_alloc(void)
{
   servreg_qmi_notif_client_node_p ret;
   servreg_qmi_notif_client_node_p sr_qmi_notif_client_node;

   servreg_mutex_lock_dal(&(servreg_qmi_notif_client_node_internal.mutex));

   if (SERVREG_NULL == servreg_qmi_notif_client_node_internal.servreg_qmi_notif_client_node_pool_free_p)
   {
      sr_qmi_notif_client_node = servreg_qmi_notif_client_node_pool_init();
   }
   else
   {
      sr_qmi_notif_client_node = servreg_qmi_notif_client_node_internal.servreg_qmi_notif_client_node_pool_free_p;
   }

   if (SERVREG_NULL != sr_qmi_notif_client_node)
   {
      servreg_qmi_notif_client_node_internal.servreg_qmi_notif_client_node_pool_free_p = sr_qmi_notif_client_node->next;
      sr_qmi_notif_client_node->next = SERVREG_NULL;
      ret = sr_qmi_notif_client_node;
   }
   else
   {
      ret = SERVREG_NULL;
   }

   servreg_mutex_unlock_dal(&(servreg_qmi_notif_client_node_internal.mutex));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_qmi_notif_client_node_pool_free
 *
 * Description:
 *     Reclaims back the sr_qmi_notif_client_node to the memory pool
 *
 * Parameters:
 *     sr_qmi_notif_client_node : space to be reclaimed back
 *
 * Returns:
 *    servreg_qmi_notif_client_node_p : The next available free space in the memory pool
 * =====================================================================  */
static servreg_qmi_notif_client_node_p servreg_qmi_notif_client_node_pool_free(servreg_qmi_notif_client_node_p sr_qmi_notif_client_node)
{
   servreg_qmi_notif_client_node_p ret = SERVREG_NULL;

   servreg_mutex_lock_dal(&(servreg_qmi_notif_client_node_internal.mutex));

   if(SERVREG_NULL != sr_qmi_notif_client_node)
   {
      sr_qmi_notif_client_node->sr_mon_handle = SERVREG_NULL;
      sr_qmi_notif_client_node->server_retry_count = 0;
      qurt_anysignal_destroy(&(sr_qmi_notif_client_node->sync_sig));
      sr_qmi_notif_client_node->server_retry_timer = 0;
      sr_qmi_notif_client_node->server_retry_group = 0;

      sr_qmi_notif_client_node->next = servreg_qmi_notif_client_node_internal.servreg_qmi_notif_client_node_pool_free_p;
      servreg_qmi_notif_client_node_internal.servreg_qmi_notif_client_node_pool_free_p = sr_qmi_notif_client_node;
      ret = sr_qmi_notif_client_node;
   }
   else
   {
      TMS_MSG_ERROR("sr_qmi_notif_client_node NULL and cannot be freed");
   }

   servreg_mutex_unlock_dal(&(servreg_qmi_notif_client_node_internal.mutex));

   return ret;
}


/** =====================================================================
 * Function:
 *     servreg_qmi_notif_client_node_internal_init
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
static void servreg_qmi_notif_client_node_internal_init(void)
{
   servreg_mutex_init_dal(&(servreg_qmi_notif_client_node_internal.mutex));
   servreg_mutex_init_dal(&(servreg_qmi_notif_client_node_internal.mutex_create));

   servreg_mutex_lock_dal(&(servreg_qmi_notif_client_node_internal.mutex));

   servreg_qmi_notif_client_node_internal.dynamic_use = SERVREG_TRUE;
   servreg_qmi_notif_client_node_pool_init();

   servreg_mutex_unlock_dal(&(servreg_qmi_notif_client_node_internal.mutex));
}

/** =====================================================================
 * Function:
 *     servreg_get_qmi_notif_client_node
 *
 * Description:
 *     Checks if a node already exists with the given name. If it does 
 *     exists it returns a pointer to that node.
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by just domain name.
 *
 * Returns:
 *    SERVREG_QMI_NOTIF_CLIENT_HANDLE : handle to the sr notif node
 * =====================================================================  */
SERVREG_QMI_NOTIF_CLIENT_HANDLE servreg_get_qmi_notif_client_node(SERVREG_MON_HANDLE sr_mon_handle)
{
   servreg_qmi_notif_client_node_p sr_qmi_notif_client_node = SERVREG_NULL;
   SERVREG_QMI_NOTIF_CLIENT_HANDLE sr_qmi_notif_client_handle = SERVREG_NULL;

   servreg_mutex_lock_dal(&(servreg_qmi_notif_client_node_internal.mutex_create));
   
   sr_qmi_notif_client_node = servreg_qmi_client_internal.servreg_qmi_notif_client_list_head;

   while(SERVREG_NULL != sr_qmi_notif_client_node)
   {
      if(sr_mon_handle == sr_qmi_notif_client_node->sr_mon_handle)
      {
         sr_qmi_notif_client_handle = sr_qmi_notif_client_node2sr_qmi_notif_client_handle(sr_qmi_notif_client_node);
         break;
      }
      else
      {
         sr_qmi_notif_client_node = sr_qmi_notif_client_node->next;
      }
   }

   servreg_mutex_unlock_dal(&(servreg_qmi_notif_client_node_internal.mutex_create));

   return sr_qmi_notif_client_handle;
}

/** =====================================================================
 * Function:
 *     servreg_create_qmi_notif_client_node
 *
 * Description:
 *     Creates a srnotif node with the given sr_mon_handle
 *     This function does not check if the srnotif node exists in order 
 *     to avoid duplication of effort. Caller must perform the check.
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by just domain name.
 *     curr_state : curr_state of the service
 *
 * Returns:
 *    SERVREG_QMI_NOTIF_CLIENT_HANDLE : handle to the sr notif node
 * =====================================================================  */
SERVREG_QMI_NOTIF_CLIENT_HANDLE servreg_create_qmi_notif_client_node(SERVREG_MON_HANDLE sr_mon_handle)
{
   servreg_qmi_notif_client_node_p sr_qmi_notif_client_node = SERVREG_NULL;  
   SERVREG_QMI_NOTIF_CLIENT_HANDLE sr_qmi_notif_client_handle = SERVREG_NULL;

    servreg_mutex_lock_dal(&(servreg_qmi_notif_client_node_internal.mutex_create));

    sr_qmi_notif_client_node = servreg_qmi_notif_client_node_pool_alloc();

   if(SERVREG_NULL != sr_qmi_notif_client_node)
   {
      /*  Insert to head of list */
      sr_qmi_notif_client_node->next = servreg_qmi_client_internal.servreg_qmi_notif_client_list_head;

      /* Update head */
      servreg_qmi_client_internal.servreg_qmi_notif_client_list_head = sr_qmi_notif_client_node;

      sr_qmi_notif_client_node->sr_mon_handle = sr_mon_handle;

      /* Initialize sync signal */
      qurt_anysignal_init(&(sr_qmi_notif_client_node->sync_sig));

      sr_qmi_notif_client_handle = sr_qmi_notif_client_node2sr_qmi_notif_client_handle(sr_qmi_notif_client_node);
   }
   else
   {
      ERR_FATAL( "SERVREG_QMI_NOTIF: sr_qmi_notif_client_node calloc failed", 0, 0, 0);
   }

   servreg_mutex_unlock_dal(&(servreg_qmi_notif_client_node_internal.mutex_create));
    
   return sr_qmi_notif_client_handle;

}

/** =====================================================================
 * Function:
 *     servreg_delete_qmi_notif_client_node
 *
 * Description:
 *     Deletes a sr notif node given the sr notif handle
 *
 * Parameters:
 *     sr_qmi_notif_client_handle  : Handle to the notifier node to be deleted
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_delete_qmi_notif_client_node(SERVREG_QMI_NOTIF_CLIENT_HANDLE sr_qmi_notif_client_handle)
{
   servreg_qmi_notif_client_node_p sr_qmi_notif_client_node = SERVREG_NULL, sr_notif_prev = SERVREG_NULL, sr_qmi_notif_client_node_del = SERVREG_NULL;
   SERVREG_RESULT ret = SERVREG_FAILURE;

   servreg_mutex_lock_dal(&(servreg_qmi_notif_client_node_internal.mutex_create));

   sr_qmi_notif_client_node = servreg_qmi_client_internal.servreg_qmi_notif_client_list_head;
   sr_qmi_notif_client_node_del = sr_qmi_notif_client_handle2sr_qmi_notif_client_node(sr_qmi_notif_client_handle);

   if(SERVREG_NULL != sr_qmi_notif_client_node_del)
   {
      if(SERVREG_QMI_NOTIF_CLIENT_SIGNATURE == sr_qmi_notif_client_node_del->notif_signature)
      {
         while(SERVREG_NULL != sr_qmi_notif_client_node)
         {
            if(sr_qmi_notif_client_node == sr_qmi_notif_client_node_del)
            {
               if(SERVREG_NULL == sr_notif_prev)
               {
                  servreg_qmi_client_internal.servreg_qmi_notif_client_list_head = sr_qmi_notif_client_node->next;
               }
               else
               {
                  sr_notif_prev->next = sr_qmi_notif_client_node->next;
               }
 
               sr_qmi_notif_client_node->next = SERVREG_NULL;

              /* Reclaim back the notif msg to free pool */
               if(SERVREG_NULL != servreg_qmi_notif_client_node_pool_free(sr_qmi_notif_client_node))
               {
                  ret = SERVREG_SUCCESS;
               }
               else
               {
                  TMS_MSG_ERROR("pool_free returned NULL");
               }
               break;
            }
            else
            {
               sr_notif_prev = sr_qmi_notif_client_node;
               sr_qmi_notif_client_node = sr_qmi_notif_client_node->next;  
            }
         } /* while() */
      }
      else
      {
         TMS_MSG_ERROR("sr_qmi_notif_client_handle has invalid signature");
         ret = SERVREG_INVALID_HANDLE;
      }
   }
   else
   {
      TMS_MSG_ERROR("sr_qmi_notif_client_handle NULL");
      ret = SERVREG_INVALID_HANDLE;
   }

   servreg_mutex_unlock_dal(&(servreg_qmi_notif_client_node_internal.mutex_create));

   return ret;
}


/** =====================================================================
 * Function:
 *     servreg_notif_sync_cb
 *
 * Description:
 *     Callback function invoked when servreg sync complete event that was registered for is received
 *
 * Parameters:
 *     sr_qmi_notif_handle_addr : address of the notifier handle
 *
 * Returns:
 *     None
 * =====================================================================  */
static void servreg_notif_sync_cb(SERVREG_CB_PARAM sr_qmi_notif_handle_addr, SERVREG_CB_PARAM __attribute__((unused)) ignored)
{
   SERVREG_QMI_NOTIF_CLIENT_HANDLE sr_qmi_notif_client_handle = SERVREG_NULL;
   servreg_qmi_notif_client_node_p sr_qmi_notif_client_node = SERVREG_NULL;

   sr_qmi_notif_client_handle = (SERVREG_QMI_NOTIF_CLIENT_HANDLE)sr_qmi_notif_handle_addr;
   sr_qmi_notif_client_node = sr_qmi_notif_client_handle2sr_qmi_notif_client_node(sr_qmi_notif_client_handle);
   
   if(SERVREG_NULL != sr_qmi_notif_client_node)
   {
      qurt_anysignal_set(&(sr_qmi_notif_client_node->sync_sig), SERVREG_NOTIF_EVT_SYNC);
   }
}

/** =====================================================================
 * Function:
 *     servreg_ssr_down_event_cb
 *
 * Description:
 *     Callback function invoked when ssr event that was registered for is received
 *
 * Parameters:
 *     ssr_name_addr      : address of the name of the SSR event registered
 *     sr_qmi_notif_handle_addr : address of the notifier handle
 *
 * Returns:
 *     None
 * =====================================================================  */
static void servreg_ssr_down_event_cb(uint32_t sr_qmi_notif_handle_addr)
{
   SERVREG_QMI_NOTIF_CLIENT_HANDLE sr_qmi_notif_client_handle = SERVREG_NULL;
   servreg_qmi_notif_client_node_p sr_qmi_notif_client_node = SERVREG_NULL;
   SERVREG_RESULT ret = SERVREG_FAILURE;
   int32_t remote_transaction_id = 0;
   time_timetick_type remaining_time = 0;
   servreg_qmi_entry_table_p sr_qmi_entry = SERVREG_NULL;
   SERVREG_NAME service_name = SERVREG_NULL;

   sr_qmi_notif_client_handle = (SERVREG_QMI_NOTIF_CLIENT_HANDLE)sr_qmi_notif_handle_addr;
   sr_qmi_notif_client_node = sr_qmi_notif_client_handle2sr_qmi_notif_client_node(sr_qmi_notif_client_handle);

   if(SERVREG_NULL != sr_qmi_notif_client_node)
   {
      /* Update the current remote state and remote transaction_id */
      remote_transaction_id = remote_transaction_id - 1;
      servreg_set_transaction_id(sr_qmi_notif_client_node->sr_mon_handle, remote_transaction_id);

      /* For SSR Down event, transaction_id = 0 */
      ret = servreg_set_state(sr_qmi_notif_client_node->sr_mon_handle, SERVREG_SERVICE_STATE_DOWN);

      if(SERVREG_SUCCESS == ret)
      {
         TMS_MSG_HIGH("User-PD DOWN due to SSR");

         /* Register sync complete callback */
         if(SERVREG_SUCCESS > servreg_register_sync_notif_cb(sr_qmi_notif_client_node->sr_mon_handle, SERVREG_SERVICE_STATE_DOWN, servreg_notif_sync_cb, (SERVREG_CB_PARAM)sr_qmi_notif_handle_addr, (SERVREG_CB_PARAM)0))
         {
            ERR_FATAL("Could not register servreg sync notification", 0, 0, 0);
         }

         /* Ensure sync is pending before waiting */
         if (0 < servreg_get_pending_acks(sr_qmi_notif_client_node->sr_mon_handle, SERVREG_SERVICE_STATE_DOWN))
         {
            qurt_anysignal_wait(&(sr_qmi_notif_client_node->sync_sig), SERVREG_NOTIF_EVT_SYNC);
            qurt_anysignal_clear(&(sr_qmi_notif_client_node->sync_sig), SERVREG_NOTIF_EVT_SYNC);
         }

         /* Sync complete -- internal clean up */
         servreg_deregister_sync_notif_cb(sr_qmi_notif_client_node->sr_mon_handle, SERVREG_SERVICE_STATE_DOWN, servreg_notif_sync_cb, (SERVREG_CB_PARAM)sr_qmi_notif_handle_addr, (SERVREG_CB_PARAM)0);
      }
      else
      {
         TMS_MSG_ERROR("Set state failed");
      }

      /* Clear the timer */
      timer_stop(&sr_qmi_notif_client_node->server_retry_timer, T_NONE, &remaining_time);

      timer_undef(&sr_qmi_notif_client_node->server_retry_timer);

      /* Release the QMI handle */
      service_name = servreg_get_service_name(sr_qmi_notif_client_node->sr_mon_handle);
      if(SERVREG_NULL == service_name)
      {
        ERR_FATAL("servreg_ssr_down_event_cb() servreg_get_service_name failed", 0, 0, 0);
        return;
      }

      sr_qmi_entry = servreg_get_qmi_table_entry(service_name);
      if(SERVREG_NULL == sr_qmi_entry)
      {
        ERR_FATAL("servreg_ssr_down_event_cb() servreg_get_qmi_table_entry failed", 0, 0, 0);
        return;
      }

      if(SERVREG_NULL != sr_qmi_entry->servreg_qmi_notifier_client_handle)
      {
        if(QMI_NO_ERR != qmi_client_release(sr_qmi_entry->servreg_qmi_notifier_client_handle))
        {
          ERR_FATAL("servreg_ssr_down_event_cb() qmi_client_release failed", 0, 0, 0);
        }
        sr_qmi_entry->servreg_qmi_notifier_client_handle = SERVREG_NULL;
      }
   }
}

/** =====================================================================
 * Function:
 *     servreg_reconnect_remote_listener
 *
 * Description:
 *     Reconnects existing client as a remote listener after ssr.
 *     QMI used as the communication medium for registering the listener
 *     with the remote service registry framework
 *
 * Parameters:
 *     sr_qmi_notif_client_handle : Handle of the notifier client node
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
static SERVREG_RESULT servreg_reconnect_remote_listener(SERVREG_QMI_NOTIF_CLIENT_HANDLE sr_qmi_notif_client_handle)
{
   qmi_client_error_type qmi_client_return;
   qmi_servreg_notif_register_listener_req_msg_v01 servreg_notif_req;
   qmi_servreg_notif_register_listener_resp_msg_v01 servreg_notif_resp;
   SERVREG_RESULT ret = SERVREG_FAILURE;
   servreg_qmi_notif_client_node_p sr_qmi_notif_client_node = SERVREG_NULL;
   SERVREG_NAME service_name = SERVREG_NULL;
   servreg_qmi_entry_table_p sr_qmi_entry = SERVREG_NULL;

   sr_qmi_notif_client_node = sr_qmi_notif_client_handle2sr_qmi_notif_client_node(sr_qmi_notif_client_handle);

   /* Reconnect an existing notifier handle only */
   if(SERVREG_NULL != sr_qmi_notif_client_node)
   {
      service_name = servreg_get_service_name(sr_qmi_notif_client_node->sr_mon_handle);

      if(SERVREG_NULL != service_name)
      {
        sr_qmi_entry = servreg_get_qmi_table_entry(service_name);
        if(SERVREG_NULL == sr_qmi_entry)
        {
          TMS_MSG_ERROR("sr_qmi_entry NULL");
          return SERVREG_FAILURE;
        }

        /* Establish a qmi connection with the correct handle during registration of remote service only */
        if(SERVREG_NULL == sr_qmi_entry->servreg_qmi_notifier_client_handle)
          ret = servreg_qmi_connect_to_server(service_name);
        else 
          ret = SERVREG_SUCCESS; //since qmi connection is already present

        if(SERVREG_SUCCESS == ret)
        {
           /* Enable registration */
           servreg_notif_req.enable = SERVREG_TRUE;

           strlcpy(servreg_notif_req.service_name, service_name, QMI_SERVREG_NOTIF_NAME_LENGTH_V01+1);

           /* QMI message to send request to register as remote listener */
           qmi_client_return = qmi_client_send_msg_sync(sr_qmi_entry->servreg_qmi_notifier_client_handle,  /* user_handle */
                                          SERVREG_NOTIF_CLIENT_REGISTER_LISTENER_REQ,               /* msg_id */
                                          (void*)&servreg_notif_req,                         /* *req_c_table */
                                          sizeof(servreg_notif_req),                         /* req_c_table_len*/
                                          (void*)&servreg_notif_resp,                        /* *resp_c_table*/
                                          sizeof(servreg_notif_resp),                        /* resp_c_table_len*/
                                          SERVREG_QMI_NOTIF_CLIENT_REQ_TIMEOUT_MS                   /* timeout_msecs */
                                          );
           if(QMI_NO_ERR == qmi_client_return)
           {
              if(QMI_RESULT_SUCCESS_V01 == servreg_notif_resp.resp.result)
              {

                 TMS_MSG_HIGH("Proxy client registered via QMI");

                 if(servreg_notif_resp.curr_state == SERVREG_SERVICE_STATE_UP)
                 {
                    /* Update the current remote state and remote transaction_id */
                    /* Remote transaction id is set to -1 at this point because the subsystem just came back up after ssr */
                    servreg_set_transaction_id(sr_qmi_notif_client_node->sr_mon_handle, -1);

                    ret = servreg_set_state(sr_qmi_notif_client_node->sr_mon_handle, servreg_notif_resp.curr_state);
                 }
              }
              else
              {
                 TMS_MSG_ERROR_1("QMI error, Response: %d", servreg_notif_resp.resp.result);
              }
           }
           else
           {
              TMS_MSG_ERROR_1("QMI send error: %d", qmi_client_return);
           }

        }
        else
        {
           TMS_MSG_ERROR("Cannot connect to QMI server");
        }
      }
      else
      {
         TMS_MSG_ERROR("Service name NULL");
      }
   }
   else
   {
      TMS_MSG_HIGH("sr_qmi_notif_client_handle NULL");
   }

   return ret;
}

/** =====================================================================
* Function:
*     servreg_notif_retry_timout_cb
*
* Description:
*     Generic callback task driven function called that checks if Service Notifier Server
*     on the subsystem is available to establish QMI connection again
*
* Parameters:
*     sr_qmi_notif_handle_addr : Address of the notifier client node
*
* Returns:
*     none
* =====================================================================  */
void servreg_notif_retry_timout_cb(void* sr_qmi_notif_handle_addr)
{
   SERVREG_QMI_NOTIF_CLIENT_HANDLE sr_qmi_notif_client_handle = SERVREG_NULL;
   servreg_qmi_notif_client_node_p sr_qmi_notif_client_node = SERVREG_NULL;
   SERVREG_RESULT ret = SERVREG_FAILURE;
   time_timetick_type remaining_time = 0;

   sr_qmi_notif_client_handle = (SERVREG_QMI_NOTIF_CLIENT_HANDLE)sr_qmi_notif_handle_addr;
   sr_qmi_notif_client_node = sr_qmi_notif_client_handle2sr_qmi_notif_client_node(sr_qmi_notif_client_handle);

   if(SERVREG_NULL != sr_qmi_notif_client_node)
   {
      if(sr_qmi_notif_client_node->server_retry_count != 0)
      {
         sr_qmi_notif_client_node->server_retry_count = sr_qmi_notif_client_node->server_retry_count - 1;

         TMS_MSG_ERROR_2("Server connect retry attempt: %d of %d", (SERVREG_QMI_NOTIF_MAX_RETRY_COUNT - sr_qmi_notif_client_node->server_retry_count), SERVREG_QMI_NOTIF_MAX_RETRY_COUNT);

         /* via QMI try connecting to the QMI server again and register */
         ret = servreg_reconnect_remote_listener(sr_qmi_notif_client_handle);

         if(SERVREG_SUCCESS == ret)
         {      
            /* Clear the timer */
            timer_stop(&sr_qmi_notif_client_node->server_retry_timer, T_NONE, &remaining_time);

            timer_undef(&sr_qmi_notif_client_node->server_retry_timer);
         }
      }
      else
      {
         /* Clear the timer */
         timer_stop(&sr_qmi_notif_client_node->server_retry_timer, T_NONE, &remaining_time);

         timer_undef(&sr_qmi_notif_client_node->server_retry_timer);

         TMS_MSG_ERROR("Could not connect to Subsystem Server post SSR");
      }
   }
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
static void servreg_generic_task_cb(timer_cb_data_type sr_qmi_notif_handle_addr)
{
  gen_cb_ctxt_invoke_cb(servreg_notif_retry_timout_cb, (void *)sr_qmi_notif_handle_addr);
}

/** =====================================================================
 * Function:
 *     servreg_ssr_up_event_cb
 *
 * Description:
 *     Callback function invoked when ssr event that was registered for is received
 *
 * Parameters:
 *     ssr_name_addr      : address of the name of the SSR event registered
 *     sr_qmi_notif_handle_addr : address of the notifier handle
 *
 * Returns:
 *     None
 * =====================================================================  */
static void servreg_ssr_up_event_cb(uint32_t sr_qmi_notif_handle_addr)
{
   SERVREG_QMI_NOTIF_CLIENT_HANDLE sr_qmi_notif_client_handle = SERVREG_NULL;
   servreg_qmi_notif_client_node_p sr_qmi_notif_client_node = SERVREG_NULL;

   sr_qmi_notif_client_handle = (SERVREG_QMI_NOTIF_CLIENT_HANDLE)sr_qmi_notif_handle_addr;
   sr_qmi_notif_client_node = sr_qmi_notif_client_handle2sr_qmi_notif_client_node(sr_qmi_notif_client_handle);

   if(SERVREG_NULL != sr_qmi_notif_client_node)
   {
      TMS_MSG_HIGH("Subsystem UP after SSR");

      sr_qmi_notif_client_node->server_retry_count = SERVREG_QMI_NOTIF_MAX_RETRY_COUNT;

      /* Set timer to check for the locator availability as deferrable timer to avoid unnecessary wakeup of Q6 */
      timer_group_set_deferrable(&sr_qmi_notif_client_node->server_retry_group, TRUE);

      if(TE_SUCCESS != timer_def_osal(&sr_qmi_notif_client_node->server_retry_timer,
                                     &sr_qmi_notif_client_node->server_retry_group,
                                     TIMER_FUNC1_CB_TYPE,
                                     servreg_generic_task_cb, sr_qmi_notif_handle_addr))
      {
          ERR_FATAL("SERVREG_LOC: Timer def failed", 0, 0, 0);
      }

      if(TE_SUCCESS != timer_set_64(&sr_qmi_notif_client_node->server_retry_timer,
                                   SERVREG_QMI_NOTIF_RETRY_TIME_MS,
                                   SERVREG_QMI_NOTIF_RETRY_TIME_MS,
                                   T_MSEC))
      {
          ERR_FATAL("SERVREG_LOC: Timer set failed", 0, 0, 0);
      }
   }
}

/** =====================================================================
 * Function:
 *     servreg_register_ssr_event
 *
 * Description:
 *     Registers for the SSR event based on the service_name
 *     SSR event registration will happen if the remote service is in root-pd or in user-pd.
 *     eg: PD-Down notification will be sent out even when SSR happens. 
 *
 * Parameters:
 *     service_name : string which has the "soc/domain/subdomain" info
 *     sr_qmi_notif_client_handle : Opaque handle to existing event
 *
 * Returns:
 *     None
 * =====================================================================  */
static void servreg_register_ssr_event(SERVREG_NAME service_name, SERVREG_QMI_NOTIF_CLIENT_HANDLE sr_qmi_notif_client_handle)
{
   SERVREG_NAME ssr_down_name = SERVREG_NULL, ssr_up_name = SERVREG_NULL;

   if(SERVREG_NULL != service_name)
   {
      ssr_down_name = servreg_create_ssr_name(service_name, SERVREG_SERVICE_STATE_DOWN);
      ssr_up_name = servreg_create_ssr_name(service_name, SERVREG_SERVICE_STATE_UP);

      if(SERVREG_NULL != ssr_down_name && SERVREG_NULL != ssr_up_name)
      {
         /* Every remote service will be registered for a SSR events */
         /* SSR - Down Event */
         if(RCECB_NULL != rcecb_register_parm1_name(ssr_down_name, (RCECB_CALLBACK_FNSIG_P1)servreg_ssr_down_event_cb, (RCECB_PARM)sr_qmi_notif_client_handle))
         {
            TMS_MSG_HIGH("Root PD registered with SSR DOWN");
         }
         else
         {
            TMS_MSG_ERROR("Could not register with SSR DOWN");
         }

         /* Every remote service will be registered for a SSR events */
         /* SSR - Up Event */
         if(RCECB_NULL != rcecb_register_parm1_name(ssr_up_name, (RCECB_CALLBACK_FNSIG_P1)servreg_ssr_up_event_cb, (RCECB_PARM)sr_qmi_notif_client_handle))
         {
            TMS_MSG_HIGH("Root PD registered with SSR UP");
         }
         else
         {
            TMS_MSG_ERROR("Could not register with SSR UP");
         }


         /* Free the space allocated for ssr_down_name and ssr_up_name as rcecb internally does a strlcpy of the name */
         servreg_free(ssr_down_name);
         servreg_free(ssr_up_name);
      }
      else
      {
         TMS_MSG_ERROR("Root PD could not register with SSR");
      }
   }
   else
   {
      TMS_MSG_HIGH("User PD need not register for SSR");
   }
}

/** =====================================================================
 * Function:
 *     servreg_deregister_ssr_event
 *
 * Description:
 *     deregisters for the SSR event based on the service_name
 *
 * Parameters:
 *     service_name : string which has the "soc/domain/subdomain" info
 *     sr_qmi_notif_client_handle : Opaque handle to existing event
 *
 * Returns:
 *     None
 * =====================================================================  */
static void servreg_deregister_ssr_event(SERVREG_NAME service_name, SERVREG_QMI_NOTIF_CLIENT_HANDLE sr_qmi_notif_client_handle)
{
   SERVREG_NAME ssr_down_name = SERVREG_NULL, ssr_up_name = SERVREG_NULL;

   if(SERVREG_NULL != service_name)
   {
      ssr_down_name = servreg_create_ssr_name(service_name, SERVREG_SERVICE_STATE_DOWN);
      ssr_up_name = servreg_create_ssr_name(service_name, SERVREG_SERVICE_STATE_UP);

      if(SERVREG_NULL != ssr_down_name && SERVREG_NULL != ssr_up_name)
      {
         /* Remote service will be deregistered for a SSR event */
         /* SSR - Down Event */
         if(RCECB_NULL != rcecb_unregister_parm1_name(ssr_down_name, (RCECB_CALLBACK_FNSIG_P1)servreg_ssr_down_event_cb, (RCECB_PARM)sr_qmi_notif_client_handle))
         {
            TMS_MSG_HIGH("Root PD deregistered with SSR DOWN");
         }
         else
         {
            TMS_MSG_ERROR("Could not deregister with SSR DOWN");
         }

         /* Remote service will be unregistered for a SSR event */
         /* SSR - Up Event */
         if(RCECB_NULL != rcecb_unregister_parm1_name(ssr_up_name, (RCECB_CALLBACK_FNSIG_P1)servreg_ssr_up_event_cb, (RCECB_PARM)sr_qmi_notif_client_handle))
         {
            TMS_MSG_HIGH("Root PD deregistered with SSR UP");
         }
         else
         {
            TMS_MSG_ERROR("Could not deregister with SSR UP");
         }


         /* Free the space allocated for ssr_down_name and ssr_up_name as rcecb internally does a strlcpy of the name */
         servreg_free(ssr_down_name);
         servreg_free(ssr_up_name);
      }
      else
      {
         TMS_MSG_ERROR("Root PD could not deregister with SSR");
      }
   }
   else
   {
      TMS_MSG_HIGH("User PD need not deregister for SSR");
   }
}

/** =====================================================================
 * Function:
 *     servreg_get_qmi_table_entry
 *
 * Description:
 *     Checks if the qmi_instance_id field is already stored for the given domain name
 *
 * Parameters:
 *     domain_name : string which has the "soc/domain/subdomain" info
 *
 * Returns:
 *     Pointer to the qmi entry
 * =====================================================================  */
static servreg_qmi_entry_table_p servreg_get_qmi_table_entry(SERVREG_NAME domain_name)
{
   servreg_qmi_entry_table_p sr_qmi_entry = SERVREG_NULL;
   SERVREG_NAME pd_name = SERVREG_NULL;

   pd_name = servreg_get_pd_name_from_domain(domain_name);

   if(SERVREG_NULL == pd_name)
   {
      return SERVREG_NULL;
   }
   
   qurt_rmutex_lock(&servreg_qmi_client_internal.mutex);
   sr_qmi_entry = servreg_qmi_client_internal.servreg_qmi_table_head;
   
   while(SERVREG_NULL != sr_qmi_entry)
   {
      if(strcmp(pd_name, sr_qmi_entry->domain_name) == 0)
      {
         qurt_rmutex_unlock(&servreg_qmi_client_internal.mutex);
         servreg_free(pd_name);
         return sr_qmi_entry;
      }
      else
      {
         sr_qmi_entry = sr_qmi_entry->next;
      }
   }
   qurt_rmutex_unlock(&servreg_qmi_client_internal.mutex);
   
   servreg_free(pd_name);
   return NULL;
}

/** =====================================================================
 * Function:
 *     servreg_create_qmi_table_entry
 *
 * Description:
 *     Stores the qmi_instance_id value for that domain_name
 *
 * Parameters:
 *     domain_name : string which has the "soc/domain/subdomain" info
 *     qmi_instance_id : instance id used to establish a qmi connection for remote service updates
 *
 * Returns:
 *     None
 * =====================================================================  */
void servreg_create_qmi_table_entry(SERVREG_NAME domain_name, uint32 qmi_instance_id)
{
   servreg_qmi_entry_table_p sr_qmi_entry = SERVREG_NULL;
   uint32_t len = 0;
   SERVREG_NAME local_subdomain_name = SERVREG_NULL;

   local_subdomain_name = servreg_get_local_subdomain_scope();

   if(strcmp(local_subdomain_name, "root_pd") == 0)
   {
      sr_qmi_entry = servreg_get_qmi_table_entry(domain_name);

      qurt_rmutex_lock(&servreg_qmi_client_internal.mutex);

      if(SERVREG_NULL == sr_qmi_entry)
      {
         sr_qmi_entry = (servreg_qmi_entry_table_p)servreg_calloc(sizeof(servreg_qmi_entry_table_t), 1);

         if(SERVREG_NULL != sr_qmi_entry)
         {
            /*  Insert to head of list */
            sr_qmi_entry->next = servreg_qmi_client_internal.servreg_qmi_table_head;
            /* Update head */
            servreg_qmi_client_internal.servreg_qmi_table_head = sr_qmi_entry;

            len = servreg_nmelen(domain_name) + 1;
            sr_qmi_entry->domain_name = (SERVREG_NAME)servreg_calloc(sizeof(char), len);

            if(SERVREG_NULL != sr_qmi_entry->domain_name)
            {
               strlcpy(sr_qmi_entry->domain_name, domain_name, len);
            }
            else
            {
               ERR_FATAL( "SERVREG_QMI_NOTIF: in servreg_create_qmi_table_entry() calloc failed", 0, 0, 0);
            }

            sr_qmi_entry->qmi_instance_id = qmi_instance_id;
            sr_qmi_entry->servreg_qmi_notifier_client_handle = SERVREG_NULL;
         }
         else
         {
            ERR_FATAL( "SERVREG_QMI_NOTIF: in servreg_create_qmi_table_entry() calloc failed", 0, 0, 0);
         }
      }

      qurt_rmutex_unlock(&servreg_qmi_client_internal.mutex);
   }
   else
   {
      TMS_MSG_ERROR("No need to record qmi_instance_id in user PD");
   }
}

/** =====================================================================
 * Function:
 *     servreg_register_remote_listener
 *
 * Description:
 *     Register client as a remote listener because the service is not a local service.
 *     QMI or sockets used as the communication medium for registering the listener
 *     with the remote service registry framework
 *
 *     For every service, remote registration is done only once and one proxy listener in 
 *     the root PD of the other QMI end point will be registered that represents all 
 *     the listeners in the clients root PD for that service.
 *
 * Parameters:
 *     service_name             : Service name that the client is interested in 
 *     sr_mon_handle            : Opaque handle to existing event
 *     sr_curr_remote_state     : State of the service received from the QMI message
 *     sr_remote_transaction_id : Transaction id of the service received from the QMI message
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_register_remote_listener(SERVREG_NAME service_name, SERVREG_MON_HANDLE sr_mon_handle, uint32_t * sr_curr_remote_state, int32_t * sr_remote_transaction_id)
{
   qmi_client_error_type qmi_client_return;
   qmi_servreg_notif_register_listener_req_msg_v01 servreg_notif_req;
   qmi_servreg_notif_register_listener_resp_msg_v01 servreg_notif_resp;
   SERVREG_RESULT ret = SERVREG_FAILURE;
   SERVREG_QMI_NOTIF_CLIENT_HANDLE sr_qmi_notif_client_handle = SERVREG_NULL;
   servreg_qmi_entry_table_p sr_qmi_entry = SERVREG_NULL;

   /* Check if the srnotif node exists */
   sr_qmi_notif_client_handle = servreg_get_qmi_notif_client_node(sr_mon_handle);

   /* Create a new notif node and register a new proxy listener for that service in root-pd of the other subsystem via QMI only if the srnotif node does not exist */
   if(SERVREG_NULL == sr_qmi_notif_client_handle)
   {
      servreg_mutex_lock_dal(&(servreg_qmi_notif_client_node_internal.mutex_create));

      sr_qmi_entry = servreg_get_qmi_table_entry(service_name);
      if(SERVREG_NULL == sr_qmi_entry)
      {
        TMS_MSG_ERROR("sr_qmi_entry NULL");
        servreg_mutex_unlock_dal(&(servreg_qmi_notif_client_node_internal.mutex_create));
        return ret;
      }

      /* Create a Service Notifier node to list the sr_mon_handle to get remote notifications from root pd */
      sr_qmi_notif_client_handle = servreg_create_qmi_notif_client_node(sr_mon_handle);

      if(SERVREG_NULL != sr_qmi_notif_client_handle)
      {
        /* Establish a qmi connection with the correct handle during registration of remote service only */
        if(SERVREG_NULL == sr_qmi_entry->servreg_qmi_notifier_client_handle)
          ret = servreg_qmi_connect_to_server(service_name);
        else 
          ret = SERVREG_SUCCESS; //since qmi connection is already present

        if(SERVREG_SUCCESS == ret)
        {
           /* Enable registration */
           servreg_notif_req.enable = SERVREG_TRUE;

           strlcpy(servreg_notif_req.service_name, service_name, QMI_SERVREG_NOTIF_NAME_LENGTH_V01+1);

           /* QMI message to send request to register as remote listener */
           qmi_client_return = qmi_client_send_msg_sync(sr_qmi_entry->servreg_qmi_notifier_client_handle,  /* user_handle */
                                          SERVREG_NOTIF_CLIENT_REGISTER_LISTENER_REQ,               /* msg_id */
                                          (void*)&servreg_notif_req,                         /* *req_c_table */
                                          sizeof(servreg_notif_req),                         /* req_c_table_len*/
                                          (void*)&servreg_notif_resp,                        /* *resp_c_table*/
                                          sizeof(servreg_notif_resp),                        /* resp_c_table_len*/
                                          SERVREG_QMI_NOTIF_CLIENT_REQ_TIMEOUT_MS                   /* timeout_msecs */
                                          );
           if(QMI_NO_ERR == qmi_client_return)
           {
              if(QMI_RESULT_SUCCESS_V01 == servreg_notif_resp.resp.result)
              {
                 /* Get the initial state of the remote handle and set that state in the local handle also */
                 *sr_curr_remote_state = servreg_notif_resp.curr_state;

                 /* IDL file does not support to get the remote transaction id, so update the transaction id when ind msg is received */
                 //*sr_remote_transaction_id = servreg_notif_resp.transaction_id;

                 /* Register with SSR to get notified if that subsystem does down */
                 SERVREG_TRACE_2(REG_SSR_E,service_name,sr_qmi_notif_client_handle);
                 servreg_register_ssr_event(service_name, sr_qmi_notif_client_handle);

                 TMS_MSG_HIGH("Proxy client registered via QMI");
              }
              else
              {
                 TMS_MSG_ERROR_1("QMI error, Response: %d", servreg_notif_resp.resp.result);
                 ret = SERVREG_FAILURE;
              }
           }
           else
           {
              TMS_MSG_ERROR_1("QMI send error: %d", qmi_client_return);
              ret = SERVREG_FAILURE;
           }

        }
        else
        {
           TMS_MSG_ERROR("Cannot connect to QMI server");
        }

        if(SERVREG_FAILURE == ret)
        {
           if(SERVREG_SUCCESS != servreg_delete_qmi_notif_client_node(sr_qmi_notif_client_handle))
           {
              TMS_MSG_ERROR("Delete QMI notif node failed");
           }
        }
      }
      else
      {
         TMS_MSG_ERROR("Could not register as remote listener in the notifier pool");
      }

      servreg_mutex_unlock_dal(&(servreg_qmi_notif_client_node_internal.mutex_create));
   }
   else
   {
      TMS_MSG_HIGH("Added to proxy client list");
      ret = SERVREG_SUCCESS;
   }

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_deregister_remote_listener
 *
 * Description:
 *     De-register the remote proxy listener via QMI only if there are no more local listeners 
 *     for that service.
 *
 * Parameters:
 *     service_name       : Service name that the client is interested in 
 *     SERVREG_MON_HANDLE : Opaque handle to existing event
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_deregister_remote_listener(SERVREG_NAME service_name, SERVREG_MON_HANDLE sr_mon_handle)
{
   qmi_client_error_type qmi_client_return;
   qmi_servreg_notif_register_listener_req_msg_v01 servreg_notif_req;
   qmi_servreg_notif_register_listener_resp_msg_v01 servreg_notif_resp;
   SERVREG_RESULT ret = SERVREG_FAILURE;
   SERVREG_QMI_NOTIF_CLIENT_HANDLE sr_qmi_notif_client_handle = SERVREG_NULL;
   servreg_qmi_entry_table_p sr_qmi_entry = SERVREG_NULL;
   
   /* Check if the srnotif node exists */
   sr_qmi_notif_client_handle = servreg_get_qmi_notif_client_node(sr_mon_handle);

   /* Delete the notif node and de-register the proxy listener for that service in root-pd of the other subsystem via QMI only if the srnotif node already exist and the number of local listeners are zero*/
   if(SERVREG_NULL != sr_qmi_notif_client_handle)
   {
      servreg_mutex_lock_dal(&(servreg_qmi_notif_client_node_internal.mutex_create));

      sr_qmi_entry = servreg_get_qmi_table_entry(service_name);

      if(SERVREG_NULL != sr_qmi_entry)
      {
         /* Disable registration */
         servreg_notif_req.enable = SERVREG_FALSE;

         strlcpy(servreg_notif_req.service_name, service_name, QMI_SERVREG_NOTIF_NAME_LENGTH_V01+1);

         /* QMI message to send request to register as remote listener */
         qmi_client_return = qmi_client_send_msg_sync(sr_qmi_entry->servreg_qmi_notifier_client_handle,  /* user_handle */
                                        SERVREG_NOTIF_CLIENT_REGISTER_LISTENER_REQ,               /* msg_id */
                                        (void*)&servreg_notif_req,                         /* *req_c_table */
                                        sizeof(servreg_notif_req),                         /* req_c_table_len*/
                                        (void*)&servreg_notif_resp,                        /* *resp_c_table*/
                                        sizeof(servreg_notif_resp),                        /* resp_c_table_len*/
                                        SERVREG_QMI_NOTIF_CLIENT_REQ_TIMEOUT_MS                   /* timeout_msecs */
                                        );
         if(QMI_NO_ERR == qmi_client_return)
         {
            if(QMI_RESULT_SUCCESS_V01 == servreg_notif_resp.resp.result)
            {
               SERVREG_TRACE_2(DEREG_SSR_E,service_name,sr_qmi_notif_client_handle);
               servreg_deregister_ssr_event(service_name,sr_qmi_notif_client_handle);
               /* Delete the Service Notifier node as there is only one remote registrant for that service per root PD */
               ret = servreg_delete_qmi_notif_client_node(sr_qmi_notif_client_handle);
            }
            else
            {
               TMS_MSG_ERROR_1("QMI error, Response: %d", servreg_notif_resp.resp.result);
               ret = SERVREG_FAILURE;
            }
         }
         else
         {
            TMS_MSG_ERROR_1("QMI send error: %d", qmi_client_return);
            ret = SERVREG_FAILURE;
         }
      }
      else
      {
         TMS_MSG_ERROR("sr_qmi_entry NULL");
         ret = SERVREG_FAILURE;
      }

      servreg_mutex_unlock_dal(&(servreg_qmi_notif_client_node_internal.mutex_create));
   }

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_free_remote_handle
 *
 * Description:
 *     Free the handle created when clients register as a remote listener. 
 *
 * Parameters:
 *     service_name       : Service name that the client is interested in 
 *     SERVREG_MON_HANDLE : Opaque handle to existing event
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_free_remote_handle(SERVREG_NAME service_name, SERVREG_MON_HANDLE sr_mon_handle)
{
   /* No QMI message currently in IDL file that supports servreg_free_remote_handle() */
   /* Currently we delete the notif node if the number of registrants = 0 */
   return SERVREG_SUCCESS;
}

/** =====================================================================
 * Function:
 *     servreg_set_remote_ack
 *
 * Description:
 *     Set the remote ack count for the remote proxy listener via QMI only if 
 *     all the local acks have been received
 *
 * Parameters:
 *     service_name       : Service name that the client is interested in 
 *     SERVREG_MON_HANDLE : Opaque handle to existing event
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_set_remote_ack(SERVREG_NAME service_name, SERVREG_MON_HANDLE sr_mon_handle, SERVREG_SERVICE_STATE curr_state, int32_t sr_transaction_id)
{
   servreg_remote_ack_req_p sr_remote_ack_req_entry = SERVREG_NULL;
   
   sr_remote_ack_req_entry = (servreg_remote_ack_req_p)servreg_calloc(sizeof(servreg_remote_ack_req_t), 1);
   if(SERVREG_NULL != sr_remote_ack_req_entry)
   {
     sr_remote_ack_req_entry->service_name = service_name;
     sr_remote_ack_req_entry->mon_handle = sr_mon_handle;
     /* UNUSED: sr_remote_ack_req_entry->service_state = curr_state; */
     sr_remote_ack_req_entry->transaction_id = sr_transaction_id;
   }
   else
   {
      ERR_FATAL("servreg_set_remote_ack() calloc failed", 0, 0, 0);
   }
   
   /* Put the sr_remote_ack_req_entry into the queue and remove it only after you get the ACK back */
   q_put(&servreg_remote_ack_req_q, q_link(sr_remote_ack_req_entry, &sr_remote_ack_req_entry->link));
   
   /* Signal task about new queue entry for remote ack request */
   qurt_anysignal_set(&servreg_remote_ack_signal, SERVREG_QMI_REMOTE_ACK_REQ_SIG);

   SERVREG_TRACE_2(SET_REMOTE_ACK_1, sr_mon_handle,0);
 
   return SERVREG_SUCCESS;
}

/** =====================================================================
 * Function:
 *     servreg_remote_queue_action
 *
 * Description: 
 *     Called by the servreg_remote_ack_req_task to service remote ACK requests
 *     queued up by the client from servreg_set_remote_ack
 *
 * Parameters:
 *     service_name       : Service name that the client is interested in 
 *     SERVREG_MON_HANDLE : Opaque handle to existing event
 *     sr_transaction_id  : Transaction ID
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
static SERVREG_RESULT servreg_remote_queue_action(SERVREG_NAME service_name, SERVREG_MON_HANDLE sr_mon_handle, int32_t sr_transaction_id)
{
   qmi_client_error_type qmi_client_return;
   qmi_servreg_notif_set_ack_req_msg_v01 servreg_notif_req;
   qmi_servreg_notif_set_ack_resp_msg_v01 servreg_notif_resp;
   SERVREG_RESULT ret = SERVREG_FAILURE;
   SERVREG_QMI_NOTIF_CLIENT_HANDLE sr_qmi_notif_client_handle = SERVREG_NULL;
   servreg_qmi_entry_table_p sr_qmi_entry = SERVREG_NULL;

   /* Check if the srnotif node exists */
   sr_qmi_notif_client_handle = servreg_get_qmi_notif_client_node(sr_mon_handle);

   if(SERVREG_NULL != sr_qmi_notif_client_handle)
   {
      if(0 < sr_transaction_id)
      {
         servreg_mutex_lock_dal(&(servreg_qmi_notif_client_node_internal.mutex_create));

         sr_qmi_entry = servreg_get_qmi_table_entry(service_name);

         if(SERVREG_NULL != sr_qmi_entry)
         {
            strlcpy(servreg_notif_req.service_name, service_name, QMI_SERVREG_NOTIF_NAME_LENGTH_V01+1);
            servreg_notif_req.transaction_id = sr_transaction_id;
            
            SERVREG_TRACE_2(REMOTE_ACK_QMI_SEND, sr_mon_handle, sr_transaction_id);
            
            /* QMI message to the ACK message req */
            qmi_client_return = qmi_client_send_msg_sync(sr_qmi_entry->servreg_qmi_notifier_client_handle,  /* user_handle */
                                           SERVREG_NOTIF_CLIENT_STATE_UPDATED_IND_ACK_REQ,           /* msg_id */
                                           (void*)&servreg_notif_req,                         /* *req_c_table */
                                           sizeof(servreg_notif_req),                         /* req_c_table_len*/
                                           (void*)&servreg_notif_resp,                        /* *resp_c_table*/
                                           sizeof(servreg_notif_resp),                        /* resp_c_table_len*/
                                           SERVREG_QMI_NOTIF_CLIENT_REQ_TIMEOUT_MS                   /* timeout_msecs */
                                           );
            if(QMI_NO_ERR == qmi_client_return)
            {
               if(QMI_RESULT_SUCCESS_V01 == servreg_notif_resp.resp.result)
               {
                  ret = SERVREG_SUCCESS;
                  TMS_MSG_HIGH("QMI ACK sent");
                  SERVREG_TRACE_0(REMOTE_ACK_QMI_SUCCESS);
               }
               else
               {
                  TMS_MSG_ERROR_1("QMI error, Response: %d", servreg_notif_resp.resp.result);
                  SERVREG_TRACE_2(RESULT, servreg_notif_resp.resp.result, 0);
                  ret = SERVREG_FAILURE;
               }
            }
            else
            {
               TMS_MSG_ERROR_1("QMI send error: %d", qmi_client_return);
               SERVREG_TRACE_2(RESULT, qmi_client_return, 0);
               ret = SERVREG_FAILURE;
            }
         }
         else
         {
            TMS_MSG_ERROR("sr_qmi_entry NULL");
            ret = SERVREG_FAILURE;
         }

         servreg_mutex_unlock_dal(&(servreg_qmi_notif_client_node_internal.mutex_create));
      }
      else
      {
         TMS_MSG_HIGH_1("Sent ACK. transaction_id: %d", sr_transaction_id);
         ret = SERVREG_SUCCESS;
      }
   }

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_notif_client_ind_cb
 *
 * Description: 
 *     Called by the QCCI infrastructure when an INDICATION message
 *     is received by the client
 *
 * Parameters:
 *     client_handle : Handle used by the infrastructure to 
 *                     identify different clients.
 *     msg_id        : Message ID
 *     ind_buf       : Pointer to the raw/un-decoded indication
 *     ind_buf_len   : Length of the indication
 *     ind_cb_data   : User-data
 *
 * Returns:
 *             None
 * =====================================================================  */
static void servreg_notif_client_ind_cb(qmi_client_type client_handle, uint32_t msg_id, void *ind_buf, uint32_t ind_buf_len, void *ind_cb_data)
{
   void *ind_msg = SERVREG_NULL;
   qmi_client_error_type qmi_err;
   uint32_t decoded_size;

   TMS_MSG_HIGH_2("Request: client_handle: %d, msg_id: %d", client_handle, msg_id);
   SERVREG_TRACE_0(QMI_IND_CB);
 
   qmi_err = qmi_idl_get_message_c_struct_len(servreg_notif_service_object, QMI_IDL_INDICATION, msg_id, &decoded_size);

   if(QMI_NO_ERR != qmi_err)
   {
      TMS_MSG_ERROR_1("QMI get error: %d", qmi_err);
      return;
   }

   ind_msg = servreg_calloc(decoded_size, 1);
   if(!ind_msg) 
   {
      TMS_MSG_ERROR("Indication buff allocation failed");
      return;
   }

   qmi_err = qmi_client_message_decode(client_handle, QMI_IDL_INDICATION, msg_id, ind_buf, ind_buf_len, ind_msg, decoded_size);
   if (qmi_err != QMI_NO_ERR)
   {
      TMS_MSG_ERROR_1("QMI decode error: %d", qmi_err);
      servreg_free(ind_msg);
      return;
   }
 
   /* Insert queue entry for remote_req_task */
   servreg_remote_qmi_ind_p sr_remote_qmi_ind_entry = SERVREG_NULL;
   
   sr_remote_qmi_ind_entry = (servreg_remote_qmi_ind_p)servreg_calloc(sizeof(servreg_remote_qmi_ind_t), 1);
   if(SERVREG_NULL == sr_remote_qmi_ind_entry)
   {
      TMS_MSG_ERROR("Could not get notif node");
      servreg_free(ind_msg);
      return;
   }

   /* copy decoded_size bites of ind_msg to sr_remote_qmi_ind_entry->rcvd_ind_qmi_msg */
   memscpy((void*)&sr_remote_qmi_ind_entry->rcvd_ind_qmi_msg, sizeof(sr_remote_qmi_ind_entry->rcvd_ind_qmi_msg), ind_msg, decoded_size);
 
   /* Put the sr_remote_qmi_ind_entry into the queue and remove it after processing */
   q_put(&servreg_remote_qmi_ind_q, q_link(sr_remote_qmi_ind_entry, &sr_remote_qmi_ind_entry->link));
  
   /* Send signal to remote_req_task for decoupled SR internal processing */
   qurt_anysignal_set(&servreg_remote_ack_signal, SERVREG_QMI_REMOTE_QMI_IND_SIG); 
 
   servreg_free(ind_msg);

   return;
}

/** =====================================================================
 * Function:
 *     servreg_qmi_connect_to_server
 *
 * Description:
 *     Initializes the service qmi_notifier QMI client
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     SERVREG_SUCCESS, SERVREG_FAILURE
 * =====================================================================  */
SERVREG_RESULT servreg_qmi_connect_to_server(SERVREG_NAME domain_name)
{
   qmi_client_error_type qmi_client_return;
   SERVREG_RESULT ret = SERVREG_FAILURE;
   qmi_service_info servreg_notif_server_info;
   qmi_client_type servreg_qmi_notifier_client_handle;
   servreg_qmi_entry_table_p sr_qmi_entry = SERVREG_NULL;

   sr_qmi_entry = servreg_get_qmi_table_entry(domain_name);

   if(SERVREG_NULL != sr_qmi_entry)
   {
      qmi_client_return = qmi_client_get_service_instance(servreg_notif_service_object, sr_qmi_entry->qmi_instance_id, &servreg_notif_server_info);
      if(QMI_NO_ERR == qmi_client_return)
      {
         qmi_client_return = qmi_client_init(&servreg_notif_server_info, servreg_notif_service_object, (qmi_client_ind_cb)servreg_notif_client_ind_cb, NULL, NULL, &servreg_qmi_notifier_client_handle);
         if(QMI_NO_ERR == qmi_client_return)
         {
            /* Record server connection specific info */
            sr_qmi_entry->servreg_notif_server_info = servreg_notif_server_info;
            sr_qmi_entry->servreg_qmi_notifier_client_handle = servreg_qmi_notifier_client_handle;

            ret = SERVREG_SUCCESS;
            TMS_MSG_ERROR_1("QMI client init for Instance ID: %d", sr_qmi_entry->qmi_instance_id);
         }
         else
         {
            TMS_MSG_ERROR_1("QMI client init failed: %d", qmi_client_return);
            //qmi_client_release(servreg_qmi_client_notifier);
            ret = SERVREG_FAILURE;
         }
      }
      else
      {
         TMS_MSG_ERROR_1("Could not get QMI service instance: %d", qmi_client_return);
         //qmi_client_release(servreg_qmi_client_notifier);
         ret = SERVREG_FAILURE;
      }
   }
   else
   {
      TMS_MSG_ERROR("QMI Instance ID not found");
      ret = SERVREG_INVALID_HANDLE;
   }

   return ret;
}

/** =====================================================================
 * Function:
 *     sr_ind_process
 *
 * Description: 
 *     Offloaded to handle QCCI infrastructure INDICATION service
 *     registry side processing
 *
 * Parameters:
 *     qmi_servreg_notif_state_updated_ind_msg_v01*
 *
 * Returns:
 *     SERVREG_RESULT
 * =====================================================================  */
static SERVREG_RESULT sr_ind_process(qmi_servreg_notif_state_updated_ind_msg_v01* indication)
{
   SERVREG_MON_HANDLE sr_mon_handle = SERVREG_NULL;
   SERVREG_RESULT ret = SERVREG_FAILURE;
   SERVREG_QMI_NOTIF_CLIENT_HANDLE sr_qmi_notif_client_handle = SERVREG_NULL;
   servreg_qmi_notif_client_node_p sr_qmi_notif_client_node = SERVREG_NULL;
   int32_t transaction_id = 0;
      
   /* Get the existing sr_mon_handle */
   sr_mon_handle = servreg_get_sr_mon_handle(indication->service_name);
   SERVREG_TRACE_2(QMI_IND_PROCESS, sr_mon_handle, 0);
   
   /* Get the Service Notifier node that has already listed to get remote notifications from root pd */
   sr_qmi_notif_client_handle = servreg_get_qmi_notif_client_node(sr_mon_handle);
   sr_qmi_notif_client_node = sr_qmi_notif_client_handle2sr_qmi_notif_client_node(sr_qmi_notif_client_handle);
   
   if(SERVREG_NULL == sr_qmi_notif_client_node)
   {
      TMS_MSG_ERROR("Could not get notif node for QMI ind process");
      return ret;
   }
   
   /* Decrease the transaction_id by one as servreg_set_state will increase it by one */
   transaction_id = indication->transaction_id - 1;
   servreg_set_transaction_id(sr_mon_handle, transaction_id);
   SERVREG_TRACE_2(QMI_IND_TRANS_ID, transaction_id, 0);
   
   ret = servreg_set_state(sr_mon_handle, (SERVREG_SERVICE_STATE)indication->curr_state);

   if(SERVREG_SUCCESS == ret)
   {
      TMS_MSG_HIGH("Set state success");
      SERVREG_TRACE_2(RESULT, ret, 0);
   }
   else
   {
      ERR_FATAL( "Ind msg set state error", 0, 0, 0);
   }
   
   return ret;
}

/** =====================================================================
 * Task:
 *     sr_remote_req_task
 *
 * Description:
 *     Task used for processing queued up remote ACK requests
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
void sr_remote_req_task( unsigned long int unused_param )
{
   uint32_t q_mask;
   
   /* Initialize the remote requests queue and signal */
   (void) q_init(&servreg_remote_ack_req_q);
   (void) q_init(&servreg_remote_qmi_ind_q);
   qurt_anysignal_init(&servreg_remote_ack_signal);
   
   /* Block for start signal */
   rcinit_handshake_startup();

   /* Task forever loop */
   for (;;)
   {
      q_mask = qurt_anysignal_wait(&servreg_remote_ack_signal, SERVREG_QMI_REMOTE_ACK_REQ_SIG | SERVREG_QMI_REMOTE_QMI_IND_SIG);

      if (q_mask & SERVREG_QMI_REMOTE_ACK_REQ_SIG)
      {
         qurt_anysignal_clear(&servreg_remote_ack_signal, SERVREG_QMI_REMOTE_ACK_REQ_SIG);
                  
         servreg_remote_ack_req_p sr_remote_ack_req_entry = SERVREG_NULL;
         sr_remote_ack_req_entry = (servreg_remote_ack_req_p)q_get(&servreg_remote_ack_req_q);

         while(SERVREG_NULL != sr_remote_ack_req_entry)
         {
            SERVREG_TRACE_2(REMOTE_ACK_Q_GET,sr_remote_ack_req_entry->mon_handle, 0);
            if (SERVREG_SUCCESS == servreg_remote_queue_action(sr_remote_ack_req_entry->service_name, sr_remote_ack_req_entry->mon_handle, sr_remote_ack_req_entry->transaction_id))
            {
               TMS_MSG_HIGH( "SUCCESS sr_remote_req_task freed queue entry");
            }
            else
            {
               TMS_MSG_HIGH( "FAILURE sr_remote_req_task freed queue entry");
            }
            servreg_free(sr_remote_ack_req_entry);
            sr_remote_ack_req_entry = (servreg_remote_ack_req_p)q_get(&servreg_remote_ack_req_q);
         }
      }
      
      if (q_mask & SERVREG_QMI_REMOTE_QMI_IND_SIG)
      {
         qurt_anysignal_clear(&servreg_remote_ack_signal, SERVREG_QMI_REMOTE_QMI_IND_SIG);
         
         SERVREG_TRACE_0(REMOTE_QMI_IND_SIG);
         
         servreg_remote_qmi_ind_p sr_remote_qmi_ind_entry = SERVREG_NULL;
         sr_remote_qmi_ind_entry = (servreg_remote_qmi_ind_p)q_get(&servreg_remote_qmi_ind_q);
         while(SERVREG_NULL != sr_remote_qmi_ind_entry)
         {
            if (SERVREG_SUCCESS == sr_ind_process(&sr_remote_qmi_ind_entry->rcvd_ind_qmi_msg))
            {
               TMS_MSG_HIGH("SUCCESS sr_ind_process freed queue entry");
            }
            
            servreg_free(sr_remote_qmi_ind_entry);
            sr_remote_qmi_ind_entry = (servreg_remote_qmi_ind_p)q_get(&servreg_remote_qmi_ind_q);
         }
      }

   } /* for(;;) */
}

/** =====================================================================
 * Function:
 *     servreg_qmi_notif_client_init
 *
 * Description:
 *     Initialization function for the service registry QMI notifier module
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
void servreg_qmi_notif_client_init(void)
{
   qmi_client_error_type qmi_client_return;

   servreg_qmi_client_internal.servreg_qmi_table_head = SERVREG_NULL;
   servreg_qmi_client_internal.servreg_qmi_notif_client_list_head = SERVREG_NULL;

   qurt_rmutex_init(&servreg_qmi_client_internal.mutex);
   
   servreg_qmi_notif_client_node_internal_init();

   if(SERVREG_SERVICE_STATE_UP == servreg_qmi_notifier_client_init_status)
   {
      return;
   }
   else
   {
      servreg_notif_service_object = servreg_notif_get_service_object_v01();

      if (servreg_notif_service_object)
      {
         /* Initialize the qmi client notifier */
         qmi_client_return = qmi_client_notifier_init(servreg_notif_service_object, NULL, &servreg_qmi_client_notifier);

         if(QMI_NO_ERR == qmi_client_return)
         {
            servreg_qmi_notifier_client_init_status = SERVREG_SERVICE_STATE_UP;
         }
         else
         {
            TMS_MSG_ERROR_1("qmi_client_notifier_init failed: %d",qmi_client_return);
         }
      }
      else
      {
         TMS_MSG_ERROR("servreg_notif_service_object NULL");
      }
   }

   return;
}
