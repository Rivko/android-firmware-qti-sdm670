/*
#============================================================================
#  Name:
#    servreg_qdi_notifier_user.c
#
#  Description:
#    Service Registry notifier file for user image. This module serves as the end-point
#    of communication via qdi.
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

#include "servreg_internal.h"
#include "servreg_locator.h"
#include "servreg_localdb.h"
#include "servreg_monitor.h"
#include "servreg_notifier.h"
#include "servreg_qdi_notifier_user.h"
#include "servreg_qdi_client.h"    /* Include for user PD only */

#include "qurt_printf.h"

#define SERVREG_NOTIF_SIGNAL_MASK           0x1

static struct
{
   qurt_thread_t tid;
   qurt_thread_t tid_sig;

} servreg_listen_internal;

/* Service Registry Notifier node structure. List of SERVREG_QDI_NOTIF_HANDLE'S */
struct servreg_qdi_notif_node_s
{
   uint32_t notif_signature;
   SERVREG_MON_HANDLE sr_mon_handle;
   SERVREG_REMOTE_HANDLE sr_remote_handle;
   SERVREG_SERVICE_STATE curr_state;
   struct servreg_qdi_notif_node_s* next;
};
typedef struct servreg_qdi_notif_node_s servreg_qdi_notif_node_t, * servreg_qdi_notif_node_p;

extern int32_t servnotif_qdi_client_handle;

/* Internal QDI User Stucture */
struct servreg_qdi_user_internal_s
{
   int32_t servreg_qdi_client_handle;

   /* User registrants in root */
   servreg_qdi_notif_node_p servreg_qdi_notif_user_list_head;      /* Head node of the servreg notification list */

   /* Root registrants in user*/
   servreg_qdi_notif_node_p servreg_qdi_notif_register_list_head;  /* Head node of the servreg registration list */
   qurt_anysignal_t servreg_notif_signal;
};
struct servreg_qdi_user_internal_s servreg_qdi_user_internal;

/* Type casts as accessor functions */
#define sr_qdi_notif_node2sr_qdi_notif_handle(x)        ((SERVREG_QDI_NOTIF_HANDLE)x)
#define sr_qdi_notif_handle2sr_qdi_notif_node(x)        ((servreg_qdi_notif_node_p)x)

/* Pool allocation internals */
struct servreg_qdi_notif_node_pool_s
{
   struct servreg_qdi_notif_node_s servreg_qdi_notif_node_pool[SERVREG_QDI_NOTIF_NODE_POOL_SIZE];
   struct servreg_qdi_notif_node_pool_s * next;
};
typedef struct servreg_qdi_notif_node_pool_s servreg_qdi_notif_node_pool_t, * servreg_qdi_notif_node_pool_p;

struct servreg_qdi_notif_node_internal_s
{
   servreg_qdi_notif_node_pool_p servreg_qdi_notif_node_pool_head_p;
   servreg_qdi_notif_node_p servreg_qdi_notif_node_pool_free_p;
   servreg_mutex_t mutex;
   servreg_mutex_t mutex_create;
   SERVREG_BOOL dynamic_use;
   unsigned long init_flag;
};

struct servreg_qdi_notif_node_internal_s servreg_qdi_notif_node_internal;
servreg_qdi_notif_node_pool_t servreg_qdi_notif_node_pool_static;

/* Static functions in this file */
static servreg_qdi_notif_node_p servreg_qdi_notif_node_pool_init(void);
static servreg_qdi_notif_node_p servreg_qdi_notif_node_pool_alloc(void);
static servreg_qdi_notif_node_p servreg_qdi_notif_node_pool_free(servreg_qdi_notif_node_p servreg_qdi_notif_node_p);
static void servreg_qdi_notif_node_internal_init(void);

/** =====================================================================
 * Function:
 *     servreg_qdi_notif_node_pool_init
 *
 * Description:
 *     Initializes the memory pool for notifier node structure
 *
 * Parameters:
 *     None
 *
 * Returns:
 *    servreg_qdi_notif_node_p : Returns the first free notifier node space from the pool
 * =====================================================================  */
static servreg_qdi_notif_node_p servreg_qdi_notif_node_pool_init(void)
{
   servreg_qdi_notif_node_pool_p next_pool = SERVREG_NULL;

   if (SERVREG_NULL == servreg_qdi_notif_node_internal.servreg_qdi_notif_node_pool_head_p)
   {
      next_pool = &servreg_qdi_notif_node_pool_static;
   }
   else if (SERVREG_TRUE == servreg_qdi_notif_node_internal.dynamic_use)
   {
      next_pool = (servreg_qdi_notif_node_pool_p)servreg_calloc(sizeof(servreg_qdi_notif_node_pool_t), 1);
   }

   if (SERVREG_NULL != next_pool)
   {
      int i;

      for (i = 0; i < SERVREG_QDI_NOTIF_NODE_POOL_SIZE; i++)
      {
         if (i != (SERVREG_QDI_NOTIF_NODE_POOL_SIZE - 1))
         {
            next_pool->servreg_qdi_notif_node_pool[i].next = &(next_pool->servreg_qdi_notif_node_pool[i + 1]);
         }
         else
         {
            next_pool->servreg_qdi_notif_node_pool[i].next = servreg_qdi_notif_node_internal.servreg_qdi_notif_node_pool_free_p;
         }

         next_pool->servreg_qdi_notif_node_pool[i].notif_signature = SERVREG_QDI_NOTIF_SIGNATURE;
         next_pool->servreg_qdi_notif_node_pool[i].sr_mon_handle = SERVREG_NULL;
         next_pool->servreg_qdi_notif_node_pool[i].sr_remote_handle = 0;
         next_pool->servreg_qdi_notif_node_pool[i].curr_state = SERVREG_SERVICE_STATE_UNINIT;
      }

      servreg_qdi_notif_node_internal.servreg_qdi_notif_node_pool_free_p = &(next_pool->servreg_qdi_notif_node_pool[0]);
      next_pool->next = servreg_qdi_notif_node_internal.servreg_qdi_notif_node_pool_head_p;
      servreg_qdi_notif_node_internal.servreg_qdi_notif_node_pool_head_p = next_pool;
   }
   else
   {
      TMS_MSG_ERROR("Calloc failed");
      return SERVREG_NULL;
   }

   return servreg_qdi_notif_node_internal.servreg_qdi_notif_node_pool_free_p;
}

/** =====================================================================
 * Function:
 *     servreg_qdi_notif_node_pool_alloc
 *
 * Description:
 *     Gives the first available free and allocated space from the memory
 *
 * Parameters:
 *     None
 *
 * Returns:
 *    servreg_qdi_notif_node_p : the first available free and allocated space from the memory
 * =====================================================================  */
static servreg_qdi_notif_node_p servreg_qdi_notif_node_pool_alloc(void)
{
   servreg_qdi_notif_node_p ret;
   servreg_qdi_notif_node_p sr_qdi_notif_node;

   servreg_mutex_lock_dal(&(servreg_qdi_notif_node_internal.mutex));

   if (SERVREG_NULL == servreg_qdi_notif_node_internal.servreg_qdi_notif_node_pool_free_p)
   {
      sr_qdi_notif_node = servreg_qdi_notif_node_pool_init();
   }
   else
   {
      sr_qdi_notif_node = servreg_qdi_notif_node_internal.servreg_qdi_notif_node_pool_free_p;
   }

   if (SERVREG_NULL != sr_qdi_notif_node)
   {
      servreg_qdi_notif_node_internal.servreg_qdi_notif_node_pool_free_p = sr_qdi_notif_node->next;
      sr_qdi_notif_node->next = SERVREG_NULL;
      ret = sr_qdi_notif_node;
   }
   else
   {
      TMS_MSG_ERROR("Alloc failed");
      ret = SERVREG_NULL;
   }

   servreg_mutex_unlock_dal(&(servreg_qdi_notif_node_internal.mutex));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_qdi_notif_node_pool_free
 *
 * Description:
 *     Reclaims back the sr_qdi_notif_node to the memory pool
 *
 * Parameters:
 *     sr_qdi_notif_node : space to be reclaimed back
 *
 * Returns:
 *    servreg_qdi_notif_node_p : The next available free space in the memory pool
 * =====================================================================  */
static servreg_qdi_notif_node_p servreg_qdi_notif_node_pool_free(servreg_qdi_notif_node_p sr_qdi_notif_node)
{
   servreg_qdi_notif_node_p ret = SERVREG_NULL;

   servreg_mutex_lock_dal(&(servreg_qdi_notif_node_internal.mutex));

   if(SERVREG_NULL != sr_qdi_notif_node)
   {
      sr_qdi_notif_node->sr_mon_handle = SERVREG_NULL;
      sr_qdi_notif_node->sr_remote_handle = 0;

      sr_qdi_notif_node->next = servreg_qdi_notif_node_internal.servreg_qdi_notif_node_pool_free_p;
      servreg_qdi_notif_node_internal.servreg_qdi_notif_node_pool_free_p = sr_qdi_notif_node;
      ret = sr_qdi_notif_node;
   }
   else
   {
      TMS_MSG_ERROR("sr_qdi_notif_node NULL and cannot be freed");
   }

   servreg_mutex_unlock_dal(&(servreg_qdi_notif_node_internal.mutex));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_qdi_notif_node_internal_init
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
static void servreg_qdi_notif_node_internal_init(void)
{
   servreg_mutex_init_dal(&(servreg_qdi_notif_node_internal.mutex));
   servreg_mutex_init_dal(&(servreg_qdi_notif_node_internal.mutex_create));

   servreg_mutex_lock_dal(&(servreg_qdi_notif_node_internal.mutex));

   servreg_qdi_notif_node_internal.dynamic_use = TRUE;
   servreg_qdi_notif_node_pool_init();

   servreg_mutex_unlock_dal(&(servreg_qdi_notif_node_internal.mutex));
}

/** =====================================================================
 * Function:
 *     servreg_get_qdi_notif_node
 *
 * Description:
 *     Checks if a notifier node already exists with the sr monitor handle. If it does 
 *     exists it returns a pointer to that notif node.
 *
 * Parameters:
 *     sr_type       : root client registing in the user pd or user client registering in the root pd
 *     sr_mon_handle : Handle to an existing service state which is mapped by domain + service 
 *                     or just domain name
 *
 * Returns:
 *    SERVREG_QDI_NOTIF_HANDLE : handle to the notifier node
 * =====================================================================  */
SERVREG_QDI_NOTIF_HANDLE servreg_get_qdi_notif_node(SERVREG_QDI_TYPE sr_type, SERVREG_MON_HANDLE sr_mon_handle)
{
   servreg_qdi_notif_node_p sr_qdi_notif_node = SERVREG_NULL;
   SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle = SERVREG_NULL;

   servreg_mutex_lock_dal(&(servreg_qdi_notif_node_internal.mutex_create));

   if(sr_type == SERVREG_USER_REGISTER)
   {
      sr_qdi_notif_node = servreg_qdi_user_internal.servreg_qdi_notif_user_list_head;
   }
   else if(sr_type == SERVREG_ROOT_REGISTER)
   {
      sr_qdi_notif_node = servreg_qdi_user_internal.servreg_qdi_notif_register_list_head;
   }

   while(SERVREG_NULL != sr_qdi_notif_node)
   {
      if(sr_mon_handle == sr_qdi_notif_node->sr_mon_handle)
      {
         sr_qdi_notif_handle = sr_qdi_notif_node2sr_qdi_notif_handle(sr_qdi_notif_node);
         break;
      }
      else
      {
         sr_qdi_notif_node = sr_qdi_notif_node->next;
      }
   }

   servreg_mutex_unlock_dal(&(servreg_qdi_notif_node_internal.mutex_create));

   return sr_qdi_notif_handle;
}

/** =====================================================================
 * Function:
 *     servreg_create_qdi_notif_node
 *
 * Description:
 *     Creates a sr notif node with the given sr_mon_handle and sr_remote_handle
 *
 * Parameters:
 *     sr_type          : root client registing in the user pd or user client registering in the root pd
 *     sr_mon_handle    : Handle to an existing service state which is mapped by domain + service 
 *                        or just domain name
 *     sr_remote_handle : Remote (root PD) notifier node address
 *
 * Returns:
 *    SERVREG_QDI_NOTIF_HANDLE : handle to the notifier node
 * =====================================================================  */
SERVREG_QDI_NOTIF_HANDLE servreg_create_qdi_notif_node(SERVREG_QDI_TYPE sr_type, SERVREG_MON_HANDLE sr_mon_handle, SERVREG_REMOTE_HANDLE sr_remote_handle)
{
   servreg_qdi_notif_node_p sr_qdi_notif_node = SERVREG_NULL;  
   SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle = SERVREG_NULL;

   servreg_mutex_lock_dal(&(servreg_qdi_notif_node_internal.mutex_create));

   sr_qdi_notif_node = servreg_qdi_notif_node_pool_alloc();

   if(SERVREG_NULL != sr_qdi_notif_node)
   {
      if(sr_type == SERVREG_USER_REGISTER)
      {
         /*  Insert to head of list */
         sr_qdi_notif_node->next = servreg_qdi_user_internal.servreg_qdi_notif_user_list_head;
         /* Update head */
         servreg_qdi_user_internal.servreg_qdi_notif_user_list_head = sr_qdi_notif_node;
      }
      else if(sr_type == SERVREG_ROOT_REGISTER)
      {
         /*  Insert to head of list */
         sr_qdi_notif_node->next = servreg_qdi_user_internal.servreg_qdi_notif_register_list_head;
         /* Update head */
         servreg_qdi_user_internal.servreg_qdi_notif_register_list_head = sr_qdi_notif_node;
      }

      sr_qdi_notif_node->sr_mon_handle = sr_mon_handle;
      sr_qdi_notif_node->sr_remote_handle = sr_remote_handle;
      sr_qdi_notif_handle = sr_qdi_notif_node2sr_qdi_notif_handle(sr_qdi_notif_node);
   }
   else
   {
      TMS_MSG_ERROR("sr_qdi_notif_node alloc failed");
   }

   servreg_mutex_unlock_dal(&(servreg_qdi_notif_node_internal.mutex_create));
   return sr_qdi_notif_handle;
}

/** =====================================================================
 * Function:
 *     servreg_delete_qdi_notif_node
 *
 * Description:
 *     Deletes a sr notif node given the sr notif handle
 *
 * Parameters:
 *     sr_type              : root client registing in the user pd or user client registering in the root pd
 *     sr_qdi_notif_handle  : Handle to the notifier node to be deleted
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_delete_qdi_notif_node(SERVREG_QDI_TYPE sr_type, SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle)
{
   servreg_qdi_notif_node_p sr_qdi_notif_node = SERVREG_NULL, sr_notif_prev = SERVREG_NULL, sr_qdi_notif_node_del = SERVREG_NULL;
   SERVREG_RESULT ret = SERVREG_FAILURE;

   servreg_mutex_lock_dal(&(servreg_qdi_notif_node_internal.mutex_create));

   if(sr_type == SERVREG_USER_REGISTER)
   {
      sr_qdi_notif_node = servreg_qdi_user_internal.servreg_qdi_notif_user_list_head;
   }
   else if(sr_type == SERVREG_ROOT_REGISTER)
   {
      sr_qdi_notif_node = servreg_qdi_user_internal.servreg_qdi_notif_register_list_head;
   }

   sr_qdi_notif_node_del = sr_qdi_notif_handle2sr_qdi_notif_node(sr_qdi_notif_handle);

   if(SERVREG_NULL != sr_qdi_notif_node_del)
   {
      if(SERVREG_QDI_NOTIF_SIGNATURE == sr_qdi_notif_node_del->notif_signature)
      {
         while(SERVREG_NULL != sr_qdi_notif_node)
         {
            if(sr_qdi_notif_node == sr_qdi_notif_node_del)
            {
               if(SERVREG_NULL == sr_notif_prev)
               {
                  if(sr_type == SERVREG_USER_REGISTER)
                  {
                     servreg_qdi_user_internal.servreg_qdi_notif_user_list_head = sr_qdi_notif_node->next;
                  }
                  else if(sr_type == SERVREG_ROOT_REGISTER)
                  {
                     servreg_qdi_user_internal.servreg_qdi_notif_register_list_head = sr_qdi_notif_node->next;
                  }
               }
               else
               {
                  sr_notif_prev->next = sr_qdi_notif_node->next;
               }

               sr_qdi_notif_node->next = SERVREG_NULL;

               /* Reclaim back the notif msg to free pool */
               if(SERVREG_NULL != servreg_qdi_notif_node_pool_free(sr_qdi_notif_node))
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
               sr_notif_prev = sr_qdi_notif_node;
               sr_qdi_notif_node = sr_qdi_notif_node->next;  
            }
         } /* while() */
      }
      else
      {
         TMS_MSG_ERROR("sr_qdi_notif_handle has invalid signature");
         ret = SERVREG_INVALID_HANDLE;
      }
   }
   else
   {
      TMS_MSG_ERROR("sr_qdi_notif_handle NULL");
      ret = SERVREG_INVALID_HANDLE;
   }

   servreg_mutex_unlock_dal(&(servreg_qdi_notif_node_internal.mutex_create));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_map_remote_handle
 *
 * Description:
 *     Given the remote notif handle i.e root pd notif node address, return
 *     the user pd's corresponding monitor handle
 *
 * Parameters:
 *     sr_type          : root client registing in the user pd or user client registering in the root pd
 *     sr_remote_handle : Remote (root PD) notifier node address
 *
 * Returns:
 *     SERVREG_MON_HANDLE :  Handle to an existing service state which is mapped by domain + service 
 *                           or just domain name
 * =====================================================================  */
SERVREG_MON_HANDLE servreg_map_remote_handle(SERVREG_QDI_TYPE sr_type, SERVREG_REMOTE_HANDLE sr_remote_handle)
{
   SERVREG_MON_HANDLE sr_mon_handle = SERVREG_NULL;
   servreg_qdi_notif_node_p sr_qdi_notif_node = SERVREG_NULL;

   servreg_mutex_lock_dal(&(servreg_qdi_notif_node_internal.mutex_create));

   if(sr_type == SERVREG_USER_REGISTER)
   {
      sr_qdi_notif_node = servreg_qdi_user_internal.servreg_qdi_notif_user_list_head;
   }
   else if(sr_type == SERVREG_ROOT_REGISTER)
   {
      sr_qdi_notif_node = servreg_qdi_user_internal.servreg_qdi_notif_register_list_head;
   }

   while(SERVREG_NULL != sr_qdi_notif_node)
   {
      if(sr_remote_handle == sr_qdi_notif_node->sr_remote_handle)
      {
         sr_mon_handle = sr_qdi_notif_node->sr_mon_handle;
         break;
      }
      else
      {
         sr_qdi_notif_node = sr_qdi_notif_node->next;
      }
   }

   servreg_mutex_unlock_dal(&(servreg_qdi_notif_node_internal.mutex_create));

   return sr_mon_handle;
}

/** =====================================================================
 * Task:
 *     servreg_notif_worker_task
 *
 * Description:
 *     This is the service register notifier worker task which keeps track of state
 *     changes in the root PD's notifier nodes and reports back to the user
 *     PD notifier nodes
 *     This task also waits for any root pd clients that want to register witht for
 *     user pd service
 *
 * Parameters:
 *     None
 * 
 * Returns:
 *     None
 * =====================================================================  */
static void servreg_notif_worker_task(void *argv __attribute__((unused)))
{
   SERVREG_RESULT ret = SERVREG_FAILURE;
   SERVREG_REMOTE_HANDLE sr_remote_handle;
   uint32_t new_curr_state;
   SERVREG_MON_HANDLE sr_mon_handle = SERVREG_NULL;
   int32_t transaction_id = 0;
   uint32_t mask = 0;
   SERVREG_NAME sr_name = SERVREG_NULL;

   //ret = servreg_qdi_client_init();

   //if (SERVREG_SUCCESS == ret)
   //{
      do
      {
         /* Blocking wait within qdi driver */
         ret = servreg_qdi_invoke_worker_wait(&mask);

         if (SERVREG_SUCCESS == ret)
         {
            if(mask & SERVREG_QDI_REG_USER_MASK_VALUE)
            {
               servreg_qdi_invoke_get_remote_handle(&sr_remote_handle, &new_curr_state, &transaction_id);
               sr_mon_handle = servreg_map_remote_handle(SERVREG_USER_REGISTER, sr_remote_handle);
               if(SERVREG_NULL != sr_mon_handle)
               {
                  /* Get the transaction_id and decrease the value by one as servreg_set_state increases it by one */
                  transaction_id = transaction_id - 1;
                  servreg_set_transaction_id(sr_mon_handle, transaction_id);
                  servreg_set_state(sr_mon_handle, new_curr_state);
               }
            }

            if(mask & SERVREG_QDI_REG_ROOT_MASK_VALUE)
            {
               /* Get the root client info that wants to register in user pd*/
               sr_name = (SERVREG_NAME)servreg_calloc(sizeof(char), QDI_SERVREG_NAME_LENGTH_V01);
               if ( sr_name == SERVREG_NULL )
               {
                 TMS_MSG_ERROR("Calloc failed");
                 continue;
               }
               servreg_qdi_invoke_register_remote_handle(&sr_remote_handle, sr_name);
               sr_mon_handle = servreg_alloc_monitor_handle(sr_name, SERVREG_NULL);
               if(SERVREG_NULL != sr_mon_handle)
               {
                  servreg_register_listener_qurt(sr_mon_handle, &servreg_qdi_user_internal.servreg_notif_signal, SERVREG_NOTIF_SIGNAL_MASK);
                  if(SERVREG_NULL == servreg_create_qdi_notif_node(SERVREG_ROOT_REGISTER, sr_mon_handle, sr_remote_handle))
                  {
                     TMS_MSG_ERROR("Could not register root client in user pd");
                  }
                  new_curr_state = servreg_get_service_curr_state(sr_mon_handle);
                  transaction_id = servreg_get_transaction_id(sr_mon_handle);
                  servreg_qdi_invoke_update_remote_handle(new_curr_state, transaction_id, sr_remote_handle);
               }
               servreg_free(sr_name);
            }

            if(mask & SERVREG_QDI_ACK_ROOT_MASK_VALUE)
            {
               /* Get the root client info that wants to ack in user pd*/
               sr_name = (SERVREG_NAME)servreg_calloc(sizeof(char), QDI_SERVREG_NAME_LENGTH_V01);
               if(SERVREG_NULL != sr_name)
               {
                  servreg_qdi_invoke_get_ack_handle(sr_name, &transaction_id);
                  sr_mon_handle = servreg_get_sr_mon_handle(sr_name);
                  if(SERVREG_NULL != sr_mon_handle)
                  {
                     servreg_set_ack(sr_mon_handle, transaction_id);
                     qurt_printf("\n servreg_notif_worker_task(): received ack from root \n"); //TODO: Why is this here?
                  }
                  servreg_free(sr_name);
               }
            }
         }

      } while (1);
   //}
   //else
   //{
   //   qurt_thread_exit(1);
   //}

}

/** =====================================================================
 * Task:
 *     servreg_notif_signal_task
 *
 * Description:
 *     This is the service register signal task which keeps track of state
 *     changes in the user PD's notifier nodes and reports back to the root
 *     PD notifier nodes
 *
 * Parameters:
 *     None
 * 
 * Returns:
 *     None
 * =====================================================================  */
static void servreg_notif_signal_task(void *argv __attribute__((unused)))
{
   uint32_t q_mask = 0;
   servreg_qdi_notif_node_p sr_qdi_notif_node = SERVREG_NULL;
   SERVREG_SERVICE_STATE new_curr_state = SERVREG_SERVICE_STATE_UNINIT;

   /* Task forever loop */
   for (;;)
   {
      q_mask = qurt_anysignal_wait(&servreg_qdi_user_internal.servreg_notif_signal, SERVREG_NOTIF_SIGNAL_MASK);

      if(q_mask & SERVREG_NOTIF_SIGNAL_MASK)
      {
         qurt_anysignal_clear(&servreg_qdi_user_internal.servreg_notif_signal, SERVREG_NOTIF_SIGNAL_MASK);

         TMS_MSG_HIGH("State change notif received");

         servreg_mutex_lock_dal(&(servreg_qdi_notif_node_internal.mutex_create));

         sr_qdi_notif_node = servreg_qdi_user_internal.servreg_qdi_notif_register_list_head;

         while(SERVREG_NULL != sr_qdi_notif_node)
         {
            new_curr_state = servreg_get_service_curr_state(sr_qdi_notif_node->sr_mon_handle);

            if(sr_qdi_notif_node->curr_state != new_curr_state)
            {
               sr_qdi_notif_node->curr_state = new_curr_state;
               servreg_qdi_invoke_set_state(sr_qdi_notif_node->sr_remote_handle, new_curr_state);
            }
            sr_qdi_notif_node = sr_qdi_notif_node->next;
         }

         servreg_mutex_unlock_dal(&(servreg_qdi_notif_node_internal.mutex_create));
      }
   }

}

/** =====================================================================
 * Function:
 *     servreg_register_root_listeners
 *
 * Description:
 *     Register the root clients for the user service if any
 *
 * Parameters:
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
void servreg_register_root_listeners()
{
   SERVREG_REMOTE_HANDLE sr_remote_handle = 0;
   uint32_t new_curr_state = SERVREG_SERVICE_STATE_UNINIT;
   SERVREG_MON_HANDLE sr_mon_handle = SERVREG_NULL;
   int32_t transaction_id = 0;
   SERVREG_NAME sr_name = SERVREG_NULL;

   qurt_anysignal_init(&servreg_qdi_user_internal.servreg_notif_signal);

   while(1)
   {
      /* Get the root client info that wants to register in user pd*/
      sr_remote_handle = 0;
      sr_name = (SERVREG_NAME)servreg_calloc(sizeof(char), QDI_SERVREG_NAME_LENGTH_V01);
      if(SERVREG_NULL == sr_name)
      {
         return;
      }
      servreg_qdi_invoke_register_root_handles(&sr_remote_handle, sr_name);
      if(sr_remote_handle != 0)
      {
         sr_mon_handle = servreg_alloc_monitor_handle(sr_name, SERVREG_NULL);
         if(SERVREG_NULL != sr_mon_handle)
         {
            servreg_register_listener_qurt(sr_mon_handle, &servreg_qdi_user_internal.servreg_notif_signal, SERVREG_NOTIF_SIGNAL_MASK);
            if(SERVREG_NULL == servreg_create_qdi_notif_node(SERVREG_ROOT_REGISTER, sr_mon_handle, sr_remote_handle))
            {
               TMS_MSG_ERROR("Could not register root client in user PD");
            }
            new_curr_state = servreg_get_service_curr_state(sr_mon_handle);
            transaction_id = servreg_get_transaction_id(sr_mon_handle);
            servreg_qdi_invoke_update_remote_handle(new_curr_state, transaction_id, sr_remote_handle);
         }
         servreg_free(sr_name);
      }
      else
      {
         servreg_free(sr_name);
         break;
      }
   }
}

/** =====================================================================
 * Function:
 *     servreg_register_remote_listener
 *
 * Description:
 *     Register client as a remote listener because the service is not a local service.
 *     QDI is used as the communication medium for registering the listener
 *     with the remote service registry framework.
 *
 *     For every service, remote registration is done only once and one proxy listener in 
 *     the root PD will be registered that represents all the listeners in the user PD
 *     for that service.
 *
 * Parameters:
 *     service_name       : Service name that the client is interested in 
 *     SERVREG_MON_HANDLE : Opaque handle to existing event
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_register_remote_listener(SERVREG_NAME service_name, SERVREG_MON_HANDLE sr_mon_handle, uint32_t * sr_curr_remote_state, int32_t * sr_remote_transaction_id)
{
   SERVREG_RESULT ret = SERVREG_FAILURE;
   SERVREG_REMOTE_HANDLE sr_remote_handle = 0;
   uint32_t  sr_remote_state = 0x7fffffff;
   int32_t  sr_transaction_id = 0;
   SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle = SERVREG_NULL;

   /* Check if the srnotif node exists */
   sr_qdi_notif_handle = servreg_get_qdi_notif_node(SERVREG_USER_REGISTER, sr_mon_handle);

   /* Create a new notif node and register a new proxy listener in root-pd only if the srnotif node does not exist */
   if(SERVREG_NULL == sr_qdi_notif_handle)
   {
      servreg_mutex_lock_dal(&(servreg_qdi_notif_node_internal.mutex_create));

      ret = servreg_qdi_invoke_register_remote_listener(service_name, &sr_remote_handle, &sr_remote_state, &sr_transaction_id);
      if(ret == SERVREG_SUCCESS)
      {
         /* Create a Service Notifier node to list the sr_mon_handle to get remote notifications from root pd */
         sr_qdi_notif_handle = servreg_create_qdi_notif_node(SERVREG_USER_REGISTER, sr_mon_handle, sr_remote_handle);
         if(SERVREG_NULL != sr_qdi_notif_handle)
         {
            /* Get the initial state of the remote handle and set that state in the local handle also */
            *sr_curr_remote_state = sr_remote_state;
            *sr_remote_transaction_id = sr_transaction_id;
         }
         else
         {
            TMS_MSG_ERROR("Could not register as a remote listener in the notifier pool");
            ret = SERVREG_FAILURE;
         }
      }
      else
      {
         TMS_MSG_ERROR("QDI register for remote listener failed");
      }

      servreg_mutex_unlock_dal(&(servreg_qdi_notif_node_internal.mutex_create));
   }
   else
   {
      TMS_MSG_HIGH("Proxy client already registered via QDI");
      ret = SERVREG_SUCCESS;
   }

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_deregister_remote_listener
 *
 * Description:
 *     De-register the remote proxy listener only if there are no more local listeners 
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
   SERVREG_RESULT ret = SERVREG_FAILURE;
   SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle = SERVREG_NULL;

   servreg_mutex_lock_dal(&(servreg_qdi_notif_node_internal.mutex_create));

   /* Check if the srnotif node exists */
   sr_qdi_notif_handle = servreg_get_qdi_notif_node(SERVREG_USER_REGISTER, sr_mon_handle);

   /* Delete the notif node and de-register the proxy listener*/
   if(SERVREG_NULL != sr_qdi_notif_handle)
   {
      ret = servreg_qdi_invoke_deregister_remote_listener(service_name);
      if(SERVREG_SUCCESS == ret)
         ret = servreg_delete_qdi_notif_node(SERVREG_USER_REGISTER,sr_qdi_notif_handle);
   }

   servreg_mutex_unlock_dal(&(servreg_qdi_notif_node_internal.mutex_create));
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
   SERVREG_RESULT ret = SERVREG_FAILURE;
   SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle = SERVREG_NULL;

   sr_qdi_notif_handle = servreg_get_qdi_notif_node(SERVREG_USER_REGISTER, sr_mon_handle);

   servreg_mutex_lock_dal(&(servreg_qdi_notif_node_internal.mutex_create));

   if(SERVREG_NULL != sr_qdi_notif_handle)
   {
      ret = servreg_qdi_invoke_free_remote_handle(service_name);
   }

   servreg_mutex_unlock_dal(&(servreg_qdi_notif_node_internal.mutex_create));
   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_set_remote_ack
 *
 * Description:
 *     Set the remote ack count for the remote proxy listener via QDI only if 
 *     all the local acks have been received
 *
 * Parameters:
 *     service_name       : Service name that the client is interested in 
 *     SERVREG_MON_HANDLE : Opaque handle to existing event
 *     curr_state         : state for which the ACK will be set. Not used in local service registry
 *                          cause the next state is not updated until all the ACK's for the current
 *                          state have received.
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_set_remote_ack(SERVREG_NAME service_name, SERVREG_MON_HANDLE sr_mon_handle, SERVREG_SERVICE_STATE curr_state, int32_t sr_transaction_id)
{
   SERVREG_RESULT ret = SERVREG_FAILURE;

   servreg_mutex_lock_dal(&(servreg_qdi_notif_node_internal.mutex_create));

   ret = servreg_qdi_invoke_set_remote_ack(service_name, sr_transaction_id);

   servreg_mutex_unlock_dal(&(servreg_qdi_notif_node_internal.mutex_create));

   return ret;
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
   SERVREG_RESULT ret = SERVREG_FAILURE;

   servreg_mutex_lock_dal(&(servreg_qdi_notif_node_internal.mutex_create));

   ret = servreg_qdi_invoke_create_qmi_entry(domain_name, qmi_instance_id);

   if(SERVREG_SUCCESS != ret)
   {
      TMS_MSG_ERROR("Failed to create QMI table entry");
   }
   
   servreg_mutex_unlock_dal(&(servreg_qdi_notif_node_internal.mutex_create));
}

/** =====================================================================
 * Function:
 *     servreg_qmi_notif_server_init
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
void servreg_qmi_notif_server_init(void)
{
   return;
}

/** =====================================================================
 * Function:
 *     servreg_notifier_init
 *
 * Description:
 *     Initialization function for the service registry notifier module
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
void servreg_notifier_init(void)
{
   qurt_thread_attr_t attr, attr_sig;
   static unsigned long stack[SERVREG_NOTIF_WORKER_TASK_STACK/sizeof(unsigned long)];
   static unsigned long stack_sig[SERVREG_NOTIF_WORKER_TASK_STACK/sizeof(unsigned long)];

   /* Client QDI driver should be initialized at this point */
   servreg_qdi_user_internal.servreg_qdi_client_handle = servnotif_qdi_client_handle;
   servreg_qdi_user_internal.servreg_qdi_notif_user_list_head = SERVREG_NULL;

   servreg_qdi_notif_node_internal_init();

   servreg_register_root_listeners();

   qurt_thread_attr_init(&attr);
   qurt_thread_attr_set_name(&attr, SERVREG_NOTIF_WORKER_TASK_NAME);
   qurt_thread_attr_set_stack_addr(&attr, stack);
   qurt_thread_attr_set_stack_size(&attr, sizeof(stack));
   qurt_thread_attr_set_priority(&attr, qurt_thread_get_priority(qurt_thread_get_id()) - 1);

   qurt_thread_create(&servreg_listen_internal.tid, &attr, servreg_notif_worker_task, NULL);

   qurt_thread_attr_init(&attr_sig);
   qurt_thread_attr_set_name(&attr_sig, SERVREG_NOTIF_SIGNAL_TASK_NAME);
   qurt_thread_attr_set_stack_addr(&attr_sig, stack_sig);
   qurt_thread_attr_set_stack_size(&attr_sig, sizeof(stack_sig));
   qurt_thread_attr_set_priority(&attr_sig, qurt_thread_get_priority(qurt_thread_get_id()) - 1);

   qurt_thread_create(&servreg_listen_internal.tid_sig, &attr_sig, servreg_notif_signal_task, NULL);
}

