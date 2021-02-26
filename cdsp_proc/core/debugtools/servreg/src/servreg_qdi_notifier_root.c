/*
#============================================================================
#  Name:
#    servreg_qdi_notifier_root.c
#
#  Description:
#    Service Registry notifier file for root image. This module serves as the end-point
#    of communication via glink and qdi
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
#include "servreg_monitor_qurt.h"
#include "servreg_notifier.h"
#include "servreg_qdi_notifier_root.h"
#include "servreg_qdi.h"         /* Include for root PD only */
#include "servreg_qmi_notifier_client.h"
#include "servreg_qmi_notifier_server.h"

/* Service Registry Notifier ROOT node structure. List of SERVREG_MON_HANDLE'S*/
struct servreg_qdi_notif_node_s
{
   uint32_t notif_signature;
   SERVREG_MON_HANDLE sr_mon_handle;
   SERVREG_SERVICE_STATE curr_state;
   int32_t transaction_id;
   SERVREG_BOOL ack_set;
   struct servreg_qdi_notif_node_s* next;
};
typedef struct servreg_qdi_notif_node_s servreg_qdi_notif_node_t, * servreg_qdi_notif_node_p;

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
static servreg_qdi_notif_node_p servreg_qdi_notif_node_pool_free(servreg_qdi_notif_node_p servreg_qdi_notif_node);
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
         next_pool->servreg_qdi_notif_node_pool[i].curr_state = SERVREG_SERVICE_STATE_UNINIT;
         next_pool->servreg_qdi_notif_node_pool[i].transaction_id = 0;
         next_pool->servreg_qdi_notif_node_pool[i].ack_set = SERVREG_FALSE;
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
      sr_qdi_notif_node->curr_state = SERVREG_SERVICE_STATE_UNINIT;
      sr_qdi_notif_node->transaction_id = 0;
      sr_qdi_notif_node->ack_set = SERVREG_FALSE; 

      sr_qdi_notif_node->next = servreg_qdi_notif_node_internal.servreg_qdi_notif_node_pool_free_p;
      servreg_qdi_notif_node_internal.servreg_qdi_notif_node_pool_free_p = sr_qdi_notif_node;
      ret = sr_qdi_notif_node;
   }
   else
   {
      TMS_MSG_ERROR("sr_qdi_notif_node is NULL and cannot be freed");
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
 *     Checks if a SRM node already exists with the given name. If it does 
 *     exists it returns a pointer to that srm node.
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by just domain name.
 *     servreg_notif_pd_list_head : Head pointer of the notifier handle list for the user PD.
 *
 * Returns:
 *    SERVREG_QDI_NOTIF_HANDLE : handle to the sr notif node
 * =====================================================================  */
SERVREG_QDI_NOTIF_HANDLE servreg_get_qdi_notif_node(SERVREG_MON_HANDLE sr_mon_handle, SERVREG_QDI_NOTIF_HANDLE * servreg_notif_pd_list_head)
{
   servreg_qdi_notif_node_p sr_qdi_notif_node = SERVREG_NULL;
   SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle = SERVREG_NULL;

   servreg_mutex_lock_dal(&(servreg_qdi_notif_node_internal.mutex_create));

   sr_qdi_notif_node = sr_qdi_notif_handle2sr_qdi_notif_node(*servreg_notif_pd_list_head);

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
 *     Creates a srnotif node with the given sr_mon_handle
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by just domain name.
 *     servreg_notif_pd_list_head : Head pointer of the notifier handle list for the user PD.
 *
 * Returns:
 *    SERVREG_QDI_NOTIF_HANDLE : handle to the sr notif node
 * =====================================================================  */
SERVREG_QDI_NOTIF_HANDLE servreg_create_qdi_notif_node(SERVREG_MON_HANDLE sr_mon_handle, SERVREG_QDI_NOTIF_HANDLE * servreg_notif_pd_list_head)
{
   servreg_qdi_notif_node_p sr_qdi_notif_node = SERVREG_NULL;  
   SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle = SERVREG_NULL;

   /* Check if the srnotif node exists */
   sr_qdi_notif_handle = servreg_get_qdi_notif_node(sr_mon_handle, servreg_notif_pd_list_head);

   if(SERVREG_NULL == sr_qdi_notif_handle)
   {
      servreg_mutex_lock_dal(&(servreg_qdi_notif_node_internal.mutex_create));

      sr_qdi_notif_node = servreg_qdi_notif_node_pool_alloc();

      if(SERVREG_NULL != sr_qdi_notif_node)
      {
         /*  Insert to head of list */
         sr_qdi_notif_node->next = sr_qdi_notif_handle2sr_qdi_notif_node(*servreg_notif_pd_list_head);

         /* Update head */
         *servreg_notif_pd_list_head = sr_qdi_notif_node2sr_qdi_notif_handle(sr_qdi_notif_node);

         sr_qdi_notif_node->sr_mon_handle = sr_mon_handle;
         sr_qdi_notif_node->curr_state = servreg_get_service_curr_state(sr_mon_handle);
         sr_qdi_notif_node->transaction_id = servreg_get_transaction_id(sr_mon_handle);
         sr_qdi_notif_node->ack_set = SERVREG_FALSE;
         sr_qdi_notif_handle = sr_qdi_notif_node2sr_qdi_notif_handle(sr_qdi_notif_node);
      }
      else
      {
         ERR_FATAL( "SERVREG_NOTIF: sr_qdi_notif_node calloc failed", 0, 0, 0);
      }

      servreg_mutex_unlock_dal(&(servreg_qdi_notif_node_internal.mutex_create));
   }
   return sr_qdi_notif_handle;

}

/** =====================================================================
 * Function:
 *     servreg_delete_qdi_notif_node
 *
 * Description:
 *     Deletes a sr notif node given the sr notif handle
 *     servreg_notif_pd_list_head : Head pointer of the notifier handle list for the user PD.
 *
 * Parameters:
 *     sr_qdi_notif_handle  : Handle to the notifier node to be deleted
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_delete_qdi_notif_node(SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle, SERVREG_QDI_NOTIF_HANDLE *  servreg_notif_pd_list_head)
{
   servreg_qdi_notif_node_p sr_qdi_notif_node = SERVREG_NULL, sr_notif_prev = SERVREG_NULL, sr_qdi_notif_node_del = SERVREG_NULL;
   SERVREG_RESULT ret = SERVREG_FAILURE;

   servreg_mutex_lock_dal(&(servreg_qdi_notif_node_internal.mutex_create));

   sr_qdi_notif_node = sr_qdi_notif_handle2sr_qdi_notif_node(*servreg_notif_pd_list_head);
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
                  *servreg_notif_pd_list_head = sr_qdi_notif_node2sr_qdi_notif_handle(sr_qdi_notif_node->next);
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
 *     servreg_delete_qdi_notif_list
 *
 * Description:
 *     Deletes a sr notif node given the sr notif head pointer and also de-registers and de-allocates the listeners attached
 *
 * Parameters:
 *     servreg_notif_list_head : Head pointer of the notifier handle list for the user PD.
 *     signal                  : qurt signal associated to that pd
 *     mask                   : qurt mask associated to that pd
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_delete_qdi_notif_list(SERVREG_QDI_NOTIF_HANDLE * servreg_notif_list_head, qurt_anysignal_t* signal, uint32_t mask)
{
   servreg_qdi_notif_node_p sr_qdi_notif_node = SERVREG_NULL, sr_notif_next = SERVREG_NULL;
   SERVREG_RESULT ret = SERVREG_SUCCESS;

   servreg_mutex_lock_dal(&(servreg_qdi_notif_node_internal.mutex_create));

   sr_qdi_notif_node = sr_qdi_notif_handle2sr_qdi_notif_node(*servreg_notif_list_head);

   while(SERVREG_NULL != sr_qdi_notif_node)
   {
      sr_notif_next = sr_qdi_notif_node->next;

      ret = servreg_deregister_listener_qurt(sr_qdi_notif_node->sr_mon_handle, signal, mask);

      if(SERVREG_SUCCESS != ret)
      {
         TMS_MSG_ERROR("Deregister listener failed");
      }

      ret = servreg_free_monitor_handle(sr_qdi_notif_node->sr_mon_handle);

      if(SERVREG_SUCCESS != ret)
      {
         TMS_MSG_ERROR("Could not free monitor handle");
      }

      servreg_free(sr_qdi_notif_node);
      sr_qdi_notif_node = sr_notif_next;
   } /* while() */

   servreg_mutex_unlock_dal(&(servreg_qdi_notif_node_internal.mutex_create));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_get_qdi_notif_node_state_change
 *
 * Description:
 *     Check which notif node state has changed and return that notif node's handle
 *
 * Parameters:
 *     servreg_notif_pd_list_head : Head pointer of the notifier handle list for the user PD.
 *
 * Returns:
 *    SERVREG_QDI_NOTIF_HANDLE : handle to the sr notif node
 * =====================================================================  */
SERVREG_QDI_NOTIF_HANDLE servreg_get_qdi_notif_node_state_change(SERVREG_QDI_NOTIF_HANDLE * servreg_notif_pd_list_head)
{
   servreg_qdi_notif_node_p sr_qdi_notif_node = SERVREG_NULL;
   SERVREG_SERVICE_STATE new_curr_state;
   SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle = SERVREG_NULL;

   servreg_mutex_lock_dal(&(servreg_qdi_notif_node_internal.mutex_create));

   sr_qdi_notif_node = sr_qdi_notif_handle2sr_qdi_notif_node(*servreg_notif_pd_list_head);

   while(SERVREG_NULL != sr_qdi_notif_node)
   {
      new_curr_state = servreg_get_service_curr_state(sr_qdi_notif_node->sr_mon_handle);

      if(sr_qdi_notif_node->curr_state != new_curr_state)
      {
         /* Return the notif node whose state has changed */
         sr_qdi_notif_node->curr_state = new_curr_state;
         sr_qdi_notif_handle = sr_qdi_notif_node2sr_qdi_notif_handle(sr_qdi_notif_node);
         /* exit while() loop */
         break; 
      }
      sr_qdi_notif_node = sr_qdi_notif_node->next;
   }
   servreg_mutex_unlock_dal(&(servreg_qdi_notif_node_internal.mutex_create));

   return sr_qdi_notif_handle;
}

/** =====================================================================
 * Function:
 *     servreg_get_qdi_notif_node_ack_set
 *
 * Description:
 *     Check which notif node whose ack was set
 *
 * Parameters:
 *     servreg_notif_register_list_head : Head pointer of the notifier handle list for the user PD.
 *
 * Returns:
 *    SERVREG_QDI_NOTIF_HANDLE : handle to the sr notif node
 * =====================================================================  */
SERVREG_QDI_NOTIF_HANDLE servreg_get_qdi_notif_node_ack_set(SERVREG_QDI_NOTIF_HANDLE * servreg_notif_register_list_head)
{
   servreg_qdi_notif_node_p sr_qdi_notif_node = SERVREG_NULL;
   SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle = SERVREG_NULL;

   servreg_mutex_lock_dal(&(servreg_qdi_notif_node_internal.mutex_create));

   sr_qdi_notif_node = sr_qdi_notif_handle2sr_qdi_notif_node(*servreg_notif_register_list_head);

   while(SERVREG_NULL != sr_qdi_notif_node)
   {
      if(sr_qdi_notif_node->ack_set == SERVREG_TRUE)
      {
         /* After finding the node, set the ack to false to process the next ack */
         sr_qdi_notif_node->ack_set = SERVREG_FALSE;

         sr_qdi_notif_handle = sr_qdi_notif_node2sr_qdi_notif_handle(sr_qdi_notif_node);
         /* exit while() loop */
         break; 
      }
      sr_qdi_notif_node = sr_qdi_notif_node->next;
   }
   servreg_mutex_unlock_dal(&(servreg_qdi_notif_node_internal.mutex_create));

   return sr_qdi_notif_handle;
}

/** =====================================================================
 * Function:
 *     servreg_get_qdi_notif_register_node
 *
 * Description:
 *     Given the pointer to the head pointer return the notif node
 *
 * Parameters:
 *     servreg_notif_register_list_head : Head pointer of the notifier handle list for the user PD.
 *
 * Returns:
 *    SERVREG_QDI_NOTIF_HANDLE : handle to the sr notif node
 * =====================================================================  */
SERVREG_QDI_NOTIF_HANDLE servreg_get_qdi_notif_register_node(SERVREG_QDI_NOTIF_HANDLE * servreg_notif_register_list_head)
{
   servreg_qdi_notif_node_p sr_qdi_notif_node = SERVREG_NULL;
   SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle = SERVREG_NULL;

   servreg_mutex_lock_dal(&(servreg_qdi_notif_node_internal.mutex_create));
   sr_qdi_notif_node = sr_qdi_notif_handle2sr_qdi_notif_node(*servreg_notif_register_list_head);

   if(SERVREG_NULL != sr_qdi_notif_node)
   {
      sr_qdi_notif_handle = sr_qdi_notif_node2sr_qdi_notif_handle(sr_qdi_notif_node);
   }
   servreg_mutex_unlock_dal(&(servreg_qdi_notif_node_internal.mutex_create));
   return sr_qdi_notif_handle;
}

/** =====================================================================
 * Function:
 *     servreg_notif_get_curr_state
 *
 * Description:
 *     Get the state of the notifier node
 *
 * Parameters:
 *    SERVREG_QDI_NOTIF_HANDLE : handle to the sr notif node
 *
 * Returns:
 *    Check enum SERVREG_SERVICE_STATE for different possible service states
 * =====================================================================  */
SERVREG_SERVICE_STATE servreg_notif_get_curr_state(SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle)
{
   servreg_qdi_notif_node_p sr_qdi_notif_node = SERVREG_NULL;
   SERVREG_SERVICE_STATE ret = SERVREG_SERVICE_STATE_UNINIT;

   servreg_mutex_lock_dal(&(servreg_qdi_notif_node_internal.mutex_create));
   sr_qdi_notif_node = sr_qdi_notif_handle2sr_qdi_notif_node(sr_qdi_notif_handle);
   if(SERVREG_NULL != sr_qdi_notif_node)
   {
      if(SERVREG_QDI_NOTIF_SIGNATURE == sr_qdi_notif_node->notif_signature)
      {
         ret = sr_qdi_notif_node->curr_state;
      }
      else
      {
         TMS_MSG_ERROR("sr_qdi_notif_handle has invalid signature");
      }
   }
   else
   {
      TMS_MSG_ERROR("sr_qdi_notif_node NULL");
   }
   servreg_mutex_unlock_dal(&(servreg_qdi_notif_node_internal.mutex_create));
   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_notif_get_mon_handle
 *
 * Description:
 *     Get the monitor handle of the notifier node
 *
 * Parameters:
 *    SERVREG_QDI_NOTIF_HANDLE : handle to the sr notif node
 *
 * Returns:
 *    Check enum SERVREG_SERVICE_STATE for different possible service states
 * =====================================================================  */
SERVREG_MON_HANDLE servreg_notif_get_mon_handle(SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle)
{
   servreg_qdi_notif_node_p sr_qdi_notif_node = SERVREG_NULL;
   SERVREG_MON_HANDLE ret = SERVREG_NULL;

   servreg_mutex_lock_dal(&(servreg_qdi_notif_node_internal.mutex_create));
   sr_qdi_notif_node = sr_qdi_notif_handle2sr_qdi_notif_node(sr_qdi_notif_handle);
   if(SERVREG_NULL != sr_qdi_notif_node)
   {
      if(SERVREG_QDI_NOTIF_SIGNATURE == sr_qdi_notif_node->notif_signature)
      {
         ret = sr_qdi_notif_node->sr_mon_handle;
      }
      else
      {
         TMS_MSG_ERROR("sr_qdi_notif_handle has invalid signature");
      }
   }
   else
   {
      TMS_MSG_ERROR("sr_qdi_notif_node NULL");
   }
   servreg_mutex_unlock_dal(&(servreg_qdi_notif_node_internal.mutex_create));
   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_notif_get_transaction_id
 *
 * Description:
 *     Get the transaction id of the notifier node
 *
 * Parameters:
 *    SERVREG_QDI_NOTIF_HANDLE : handle to the sr notif node
 *
 * Returns:
 *    transaction id
 * =====================================================================  */
int32_t servreg_notif_get_transaction_id(SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle)
{
   servreg_qdi_notif_node_p sr_qdi_notif_node = SERVREG_NULL;
   int32_t ret = 0;

   servreg_mutex_lock_dal(&(servreg_qdi_notif_node_internal.mutex_create));
   sr_qdi_notif_node = sr_qdi_notif_handle2sr_qdi_notif_node(sr_qdi_notif_handle);
   if(SERVREG_NULL != sr_qdi_notif_node)
   {
      if(SERVREG_QDI_NOTIF_SIGNATURE == sr_qdi_notif_node->notif_signature)
      {
         ret = sr_qdi_notif_node->transaction_id;
      }
      else
      {
         TMS_MSG_ERROR("sr_qdi_notif_handle has invalid signature");
      }
   }
   else
   {
      TMS_MSG_ERROR("sr_qdi_notif_node NULL");
   }
   servreg_mutex_unlock_dal(&(servreg_qdi_notif_node_internal.mutex_create));
   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_notif_update_handle
 *
 * Description:
 *     Update the notif handle with the state and transaction id from the user pd
 *
 * Parameters:
 *    SERVREG_QDI_NOTIF_HANDLE : handle to the sr notif node
 *
 * Returns:
 *    Check enum SERVREG_SERVICE_STATE for different possible service states
 * =====================================================================  */
SERVREG_RESULT servreg_notif_update_handle(SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle, SERVREG_SERVICE_STATE state, int32_t transaction_id)
{
   servreg_qdi_notif_node_p sr_qdi_notif_node = SERVREG_NULL;
   SERVREG_RESULT ret = SERVREG_FAILURE;

   servreg_mutex_lock_dal(&(servreg_qdi_notif_node_internal.mutex_create));
   sr_qdi_notif_node = sr_qdi_notif_handle2sr_qdi_notif_node(sr_qdi_notif_handle);
   if(SERVREG_NULL != sr_qdi_notif_node)
   {
      if(SERVREG_QDI_NOTIF_SIGNATURE == sr_qdi_notif_node->notif_signature)
      {
         sr_qdi_notif_node->curr_state = state;
         sr_qdi_notif_node->transaction_id = transaction_id;

         /* Update the actual state of the user service in root */
         if(transaction_id != 0)
         {
            SERVREG_TRACE_2(QDI_UPDATE_STATE, sr_qdi_notif_node->sr_mon_handle, state);
            SERVREG_TRACE_2(QDI_UPDATE_TXID, sr_qdi_notif_node->sr_mon_handle, transaction_id);
            servreg_set_transaction_id(sr_qdi_notif_node->sr_mon_handle, (transaction_id - 1));
            servreg_set_state(sr_qdi_notif_node->sr_mon_handle, state);
         }
         else
         {
            SERVREG_TRACE_2(QDI_UPDATE_TXID, sr_qdi_notif_node->sr_mon_handle, transaction_id);
            servreg_set_transaction_id(sr_qdi_notif_node->sr_mon_handle, transaction_id);
         }

         ret = SERVREG_SUCCESS;
      }
      else
      {
         TMS_MSG_ERROR("sr_qdi_notif_handle has invalid signature");
      }
   }
   else
   {
      TMS_MSG_ERROR("sr_qdi_notif_node NULL");
   }
   servreg_mutex_unlock_dal(&(servreg_qdi_notif_node_internal.mutex_create));
   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_notif_set_user_ack
 *
 * Description:
 *     Set the ack for the notif handle
 *
 * Parameters:
 *    SERVREG_QDI_NOTIF_HANDLE : handle to the sr notif node
 *
 * Returns:
 *    Check enum SERVREG_SERVICE_STATE for different possible service states
 * =====================================================================  */
SERVREG_RESULT servreg_notif_set_user_ack(SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle)
{
   servreg_qdi_notif_node_p sr_qdi_notif_node = SERVREG_NULL;
   SERVREG_RESULT ret = SERVREG_FAILURE;

   servreg_mutex_lock_dal(&(servreg_qdi_notif_node_internal.mutex_create));
   sr_qdi_notif_node = sr_qdi_notif_handle2sr_qdi_notif_node(sr_qdi_notif_handle);
   if(SERVREG_NULL != sr_qdi_notif_node)
   {
      if(SERVREG_QDI_NOTIF_SIGNATURE == sr_qdi_notif_node->notif_signature)
      {
         sr_qdi_notif_node->ack_set = SERVREG_TRUE;
         SERVREG_TRACE_2(SET_USER_ACK, sr_qdi_notif_node, sr_qdi_notif_node->ack_set);
      }
      else
      {
         TMS_MSG_ERROR("sr_qdi_notif_handle has invalid signature");
      }
   }
   else
   {
      TMS_MSG_ERROR("sr_qdi_notif_node NULL");
   }
   servreg_mutex_unlock_dal(&(servreg_qdi_notif_node_internal.mutex_create));
   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_get_qdi_notif_next_node
 *
 * Description:
 *     Get the next notifier node
 *
 * Parameters:
 *    SERVREG_QDI_NOTIF_HANDLE : handle to the sr notif node
 *
 * Returns:
 *    Check enum SERVREG_SERVICE_STATE for different possible service states
 * =====================================================================  */
SERVREG_QDI_NOTIF_HANDLE servreg_get_qdi_notif_next_node(SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle)
{
   servreg_qdi_notif_node_p sr_qdi_notif_node = SERVREG_NULL;
   SERVREG_QDI_NOTIF_HANDLE ret = SERVREG_NULL;

   servreg_mutex_lock_dal(&(servreg_qdi_notif_node_internal.mutex_create));
   sr_qdi_notif_node = sr_qdi_notif_handle2sr_qdi_notif_node(sr_qdi_notif_handle);
   if(SERVREG_NULL != sr_qdi_notif_node)
   {
      if(SERVREG_QDI_NOTIF_SIGNATURE == sr_qdi_notif_node->notif_signature)
      {
         ret = sr_qdi_notif_node->next;
      }
      else
      {
         TMS_MSG_ERROR("sr_qdi_notif_handle has invalid signature");
      }
   }
   else
   {
      TMS_MSG_ERROR("sr_qdi_notif_node NULL");
   }
   servreg_mutex_unlock_dal(&(servreg_qdi_notif_node_internal.mutex_create));

   return ret;
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
   servreg_qdi_notif_node_internal_init();
   servreg_qmi_notif_server_init();
   //servreg_qmi_notif_client_init();
   return;
}
