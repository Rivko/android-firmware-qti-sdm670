/*
#============================================================================
#  Name:
#    servreg_monitor.c 
#
#  Description:
#    Service Registry Monitor(mon) feature. 
#
# Copyright (c) 2015-2018 by Qualcomm Technologies Incorporated. All Rights Reserved.
#============================================================================
*/
#include "stdarg.h"
#include "stdlib.h"
#include <stringl/stringl.h>
#include "comdef.h"             /* Definitions for byte, word, etc.     */
#include "err.h"
#include "queue.h"
#include "timer.h"
#include "rcesn.h"
#include "rcecb.h"

#include "servreg_internal.h"
#include "servreg_locator.h"
#include "servreg_localdb.h"
#include "servreg_monitor.h"
#include "servreg_notifier.h"
#include "servreg_qdi.h"
#include "gen_cb_ctxt.h"

#include "tms_utils_msg.h"
#include "tms_utils.h"

#define SERVREG_MON_ACK_TIMEOUT_COUNT         10
#define SERVREG_MON_ACK_TIME_CHK_SEC          1

/* Type casts as accessor functions */
#define sr_mon_node2sr_mon_handle(x)        ((SERVREG_MON_HANDLE)x)
#define sr_mon_handle2sr_mon_node(x)        ((servreg_mon_node_p)x)

/* Internal Monitor Structure */
struct servreg_mon_internal_s
{
   servreg_mon_node_p servreg_mon_node_hashtab[SERVREG_MON_NODE_HASHTABLE_BUCKETS];
   timer_type servreg_mon_ack_timer;
   timer_group_type servreg_group;
};
struct servreg_mon_internal_s servreg_mon_internal;

/* Pool allocation internals */
struct servreg_mon_node_pool_s
{
   struct servreg_mon_node_s servreg_mon_node_pool[SERVREG_MON_NODE_POOL_SIZE];
   struct servreg_mon_node_pool_s * next;
};
typedef struct servreg_mon_node_pool_s servreg_mon_node_pool_t, * servreg_mon_node_pool_p;

struct servreg_mon_node_internal_s
{
   servreg_mon_node_pool_p servreg_mon_node_pool_head_p;
   servreg_mon_node_p servreg_mon_node_pool_free_p;
   servreg_mutex_t mutex;
   servreg_mutex_t mutex_create;
   SERVREG_BOOL dynamic_use;
   unsigned long init_flag;
};

struct servreg_mon_node_internal_s servreg_mon_node_internal;
servreg_mon_node_pool_t servreg_mon_node_pool_static;

/* Static Functions defined in this file */
static servreg_mon_node_p servreg_mon_node_pool_init(void);
static servreg_mon_node_p servreg_mon_node_pool_alloc(void);
static servreg_mon_node_p servreg_mon_node_pool_free(servreg_mon_node_p servreg_mon_node_p);
/* Hash Functions */
static servreg_mon_node_p servreg_mon_node_hashtab_get(servreg_hash_t hash);
static void servreg_mon_node_hashtab_put(servreg_mon_node_p sr_mon_node, servreg_hash_t hash);
static void servreg_mon_node_hashtab_delete(servreg_mon_node_p sr_mon_node_del, servreg_hash_t hash);
static void servreg_mon_node_internal_init(void);

//static SERVREG_MON_HANDLE servreg_get_sr_mon_handle(SERVREG_NAME const name);
static SERVREG_MON_HANDLE servreg_create_mon_node(SERVREG_NAME sr_name, RCESN_HANDLE rc_handle);
static void servreg_service_queue(SERVREG_MON_HANDLE sr_mon_handle);
static int servreg_compare_state(void* item_ptr, void* compare_val);
void servreg_mon_ack_timer_cb(void* unused);
static void servreg_generic_task_cb(timer_cb_data_type unused);

/** =====================================================================
 * Function:
 *     servreg_mon_node_pool_init
 *
 * Description:
 *     Initializes the memory pool for monitor node structure
 *
 * Parameters:
 *     None
 *
 * Returns:
 *    servreg_mon_node_p : Returns the first free monitor node space from the pool
 * =====================================================================  */
static servreg_mon_node_p servreg_mon_node_pool_init(void)
{
   servreg_mon_node_pool_p next_pool = SERVREG_NULL;

   if (SERVREG_NULL == servreg_mon_node_internal.servreg_mon_node_pool_head_p)
   {
      next_pool = &servreg_mon_node_pool_static;
   }
   else if (SERVREG_TRUE == servreg_mon_node_internal.dynamic_use)
   {
      next_pool = (servreg_mon_node_pool_p)servreg_calloc(sizeof(servreg_mon_node_pool_t), 1);
   }

   if (SERVREG_NULL != next_pool)
   {
      int i;

      for (i = 0; i < SERVREG_MON_NODE_POOL_SIZE; i++)
      {
         if (i != (SERVREG_MON_NODE_POOL_SIZE - 1))
         {
            next_pool->servreg_mon_node_pool[i].next = &(next_pool->servreg_mon_node_pool[i + 1]);
         }
         else
         {
            next_pool->servreg_mon_node_pool[i].next = servreg_mon_node_internal.servreg_mon_node_pool_free_p;
         }

         next_pool->servreg_mon_node_pool[i].mon_signature = SERVREG_MON_SIGNATURE;
         next_pool->servreg_mon_node_pool[i].rcesn_handle = SERVREG_NULL;
         next_pool->servreg_mon_node_pool[i].service_name = SERVREG_NULL;
         next_pool->servreg_mon_node_pool[i].handle_count = 0;
         next_pool->servreg_mon_node_pool[i].listener_ref_count = 0;
         next_pool->servreg_mon_node_pool[i].servreg_sync = SERVREG_FALSE;
         next_pool->servreg_mon_node_pool[i].transaction_id = 0;
         next_pool->servreg_mon_node_pool[i].rcecb_up_handle = RCECB_NULL;
         next_pool->servreg_mon_node_pool[i].rcecb_down_handle = RCECB_NULL;
         next_pool->servreg_mon_node_pool[i].state_q = NULL;
      }

      servreg_mon_node_internal.servreg_mon_node_pool_free_p = &(next_pool->servreg_mon_node_pool[0]);
      next_pool->next = servreg_mon_node_internal.servreg_mon_node_pool_head_p;
      servreg_mon_node_internal.servreg_mon_node_pool_head_p = next_pool;
   }
   else
   {
      TMS_MSG_ERROR("Calloc failed");
      return SERVREG_NULL;
   }

   return servreg_mon_node_internal.servreg_mon_node_pool_free_p;
}

/** =====================================================================
 * Function:
 *     servreg_mon_node_pool_alloc
 *
 * Description:
 *     Gives the first available free and allocated space from the memory
 *
 * Parameters:
 *     None
 *
 * Returns:
 *    servreg_mon_node_p : the first available free and allocated space from the memory
 * =====================================================================  */
static servreg_mon_node_p servreg_mon_node_pool_alloc(void)
{
   servreg_mon_node_p ret;
   servreg_mon_node_p sr_mon_node;

   servreg_mutex_lock_dal(&(servreg_mon_node_internal.mutex));

   if (SERVREG_NULL == servreg_mon_node_internal.servreg_mon_node_pool_free_p)
   {
      sr_mon_node = servreg_mon_node_pool_init();
   }
   else
   {
      sr_mon_node = servreg_mon_node_internal.servreg_mon_node_pool_free_p;
   }

   if (SERVREG_NULL != sr_mon_node)
   {
      servreg_mon_node_internal.servreg_mon_node_pool_free_p = sr_mon_node->next;
      sr_mon_node->next = SERVREG_NULL;
      ret = sr_mon_node;
   }
   else
   {
      TMS_MSG_ERROR("Alloc failed");
      ret = SERVREG_NULL;
   }

   servreg_mutex_unlock_dal(&(servreg_mon_node_internal.mutex));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_mon_node_pool_free
 *
 * Description:
 *     Reclaims back the sr_mon_node to the memory pool
 *
 * Parameters:
 *     sr_mon_node : space to be reclaimed back
 *
 * Returns:
 *    servreg_mon_node_p : The next available free space in the memory pool
 * =====================================================================  */
static servreg_mon_node_p servreg_mon_node_pool_free(servreg_mon_node_p sr_mon_node)
{
   servreg_mon_node_p ret = SERVREG_NULL;
   servreg_mon_queue_p sr_q_entry = SERVREG_NULL;

   servreg_mutex_lock_dal(&(servreg_mon_node_internal.mutex));

   if(SERVREG_NULL != sr_mon_node)
   {
      sr_mon_node->rcesn_handle = SERVREG_NULL;

      if (SERVREG_NULL != sr_mon_node->service_name)
      {
        servreg_free(sr_mon_node->service_name);
      }
      sr_mon_node->handle_count = 0;
      sr_mon_node->listener_ref_count = 0;
      sr_mon_node->servreg_sync = SERVREG_FALSE;
      sr_mon_node->transaction_id = 0;
      sr_mon_node->rcecb_up_handle = RCECB_NULL;
      sr_mon_node->rcecb_down_handle = RCECB_NULL;

      sr_q_entry = (servreg_mon_queue_p)q_get(sr_mon_node->state_q);
      while(SERVREG_NULL != sr_q_entry)
      {
         servreg_free(sr_q_entry);
         sr_q_entry = (servreg_mon_queue_p)q_get(sr_mon_node->state_q);
      }

      q_destroy(sr_mon_node->state_q);
      servreg_free(sr_mon_node->state_q);

      sr_mon_node->next = servreg_mon_node_internal.servreg_mon_node_pool_free_p;
      servreg_mon_node_internal.servreg_mon_node_pool_free_p = sr_mon_node;
      ret = sr_mon_node;
   }
   else
   {
      TMS_MSG_ERROR("sr_mon_node NULL and cannot be freed");
   }
   servreg_mutex_unlock_dal(&(servreg_mon_node_internal.mutex));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_mon_node_hashtab_get
 *
 * Description:
 *     Given the hash value, get the corresponding sr monitor node
 *
 * Parameters:
 *     hash : hash value which is based on the service name
 *
 * Returns:
 *     servreg_mon_node_p : sr monitor node that corresponds to the hash value
 * =====================================================================  */
static servreg_mon_node_p servreg_mon_node_hashtab_get(servreg_hash_t hash)
{
   servreg_mon_node_p sr_mon_node = SERVREG_NULL;

   servreg_mutex_lock_dal(&(servreg_mon_node_internal.mutex));

   sr_mon_node = servreg_mon_internal.servreg_mon_node_hashtab[hash];

   servreg_mutex_unlock_dal(&(servreg_mon_node_internal.mutex));

   return sr_mon_node;
}

/** =====================================================================
 * Function:
 *     servreg_mon_node_hashtab_put
 *
 * Description:
 *     Put the sr monitor node to the hash table with the given hash value
 *
 * Parameters:
 *     sr_mon_node : sr monitor node to be put into the hash table
 *     hash        : hash value associated with the sr montior node
 *
 * Returns:
 *     None
 * =====================================================================  */
static void servreg_mon_node_hashtab_put(servreg_mon_node_p sr_mon_node, servreg_hash_t hash)
{
   servreg_mutex_lock_dal(&(servreg_mon_node_internal.mutex));

   /*  Insert to head of list for that hash value */
   sr_mon_node->next = servreg_mon_internal.servreg_mon_node_hashtab[hash];

   /* Update head for that hash value */
   servreg_mon_internal.servreg_mon_node_hashtab[hash] = sr_mon_node;

   servreg_mutex_unlock_dal(&(servreg_mon_node_internal.mutex));
}

/** =====================================================================
 * Function:
 *     servreg_mon_node_hashtab_delete
 *
 * Description:
 *     Delete the sr monitor node from the hash table with the given hash value
 *
 * Parameters:
 *     sr_mon_node : sr monitor node to be put into the hash table
 *     hash        : hash value associated with the sr montior node
 *
 * Returns:
 *     None
 * =====================================================================  */
static void servreg_mon_node_hashtab_delete(servreg_mon_node_p sr_mon_node_del, servreg_hash_t hash)
{
   servreg_mon_node_p sr_mon_node_curr = SERVREG_NULL, sr_mon_node_prev = SERVREG_NULL;

   servreg_mutex_lock_dal(&(servreg_mon_node_internal.mutex));

   /* Get the head of the list */
   sr_mon_node_curr = servreg_mon_internal.servreg_mon_node_hashtab[hash];

   while(SERVREG_NULL != sr_mon_node_curr)
   {
      if(sr_mon_node_curr == sr_mon_node_del)
      {
         if(SERVREG_NULL == sr_mon_node_prev)
         {
            /* Delete the first node */
            servreg_mon_internal.servreg_mon_node_hashtab[hash] = sr_mon_node_curr->next;
         }
         else
         {
            sr_mon_node_prev->next = sr_mon_node_curr->next;
         }

         sr_mon_node_curr->next = SERVREG_NULL;

         break; /* exit while() loop */
      }

      sr_mon_node_prev = sr_mon_node_curr;
      sr_mon_node_curr = sr_mon_node_curr->next;
   }

   servreg_mutex_unlock_dal(&(servreg_mon_node_internal.mutex));

   return;
}

/** =====================================================================
 * Function:
 *     servreg_mon_node_internal_init
 *
 * Description:
 *     Initialization of the internal memory pools and other internals
 *     for sr monitor nodes
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
static void servreg_mon_node_internal_init(void)
{
   servreg_mutex_init_dal(&(servreg_mon_node_internal.mutex));
   servreg_mutex_init_dal(&(servreg_mon_node_internal.mutex_create));

   servreg_mutex_lock_dal(&(servreg_mon_node_internal.mutex));

   servreg_mon_node_internal.dynamic_use = SERVREG_TRUE;
   secure_memset(&servreg_mon_internal.servreg_mon_node_hashtab, 0, sizeof(servreg_mon_internal.servreg_mon_node_hashtab));
   servreg_mon_node_pool_init();

   servreg_mutex_unlock_dal(&(servreg_mon_node_internal.mutex));
}

/** =====================================================================
 * Function:
 *     servreg_create_mon_node
 *
 * Description:
 *     Creates a mon node with the given sr_name and rcesn handle
 *
 * Parameters:
 *    sr_name : domain+service or just domain name
 *    rc_handle : rcesn handle
 *
 * Returns:
 *    SERVREG_MON_HANDLE : handle to the mon node
 *
 * Note :
 *    No need of locks as this function will be called internally only by servreg_alloc_monitor_handle()
 * =====================================================================  */
static SERVREG_MON_HANDLE servreg_create_mon_node(SERVREG_NAME sr_name, RCESN_HANDLE rc_handle)
{
   servreg_mon_node_p sr_mon_node = SERVREG_NULL;
   SERVREG_MON_HANDLE sr_mon_handle = SERVREG_NULL;

   /* Check if the MON node exists */
   sr_mon_handle = servreg_get_sr_mon_handle(sr_name);

   if(SERVREG_NULL == sr_mon_handle)
   {
      /* Allocate a new mon node from the node pool */
      sr_mon_node = servreg_mon_node_pool_alloc();

      SERVREG_TRACE_2(NODE_CREATE, sr_mon_node, 1);

      if(SERVREG_NULL != sr_mon_node && RCESN_NULL != rc_handle)
      {
         /* In Servreg Local Db servreg_concat() allocates memory for sr_name */
         sr_mon_node->service_name = sr_name;
         sr_mon_node->rcesn_handle = rc_handle;
         if(rcesn_setstatecurr_handle(sr_mon_node->rcesn_handle, SERVREG_SERVICE_STATE_UNINIT)){;}
         sr_mon_node->handle_count = 1;
         sr_mon_node->listener_ref_count = 0;
         sr_mon_node->servreg_sync = SERVREG_FALSE;
         sr_mon_node->transaction_id = 0;

         sr_mon_node->state_q = (q_type *)servreg_calloc(sizeof(q_type), 1);

         if(SERVREG_NULL != sr_mon_node->state_q)
         {
            /* Initialize the queue */
            memset((q_type *)sr_mon_node->state_q, 0, sizeof(q_type));
            q_init(sr_mon_node->state_q);
         }
         else
         {
            ERR_FATAL("Calloc failed", 0, 0, 0);
         }

         sr_mon_node->next = SERVREG_NULL;
         servreg_mon_node_hashtab_put(sr_mon_node, servreg_nmehash(sr_mon_node->service_name, (sizeof(servreg_mon_internal.servreg_mon_node_hashtab) / sizeof(servreg_mon_node_p))));
         if(rcesn_signal_handle(rc_handle, SERVREG_SERVICE_STATE_UNINIT)){;}

         sr_mon_handle = sr_mon_node2sr_mon_handle(sr_mon_node);
      }
      else
      {
         /* Free sr_name since entry failed */
         servreg_free(sr_name);
         SERVREG_TRACE_0(FAIL);
         TMS_MSG_ERROR("sr_mon_node NULL");
      }
   }
   else
   {
      /* Free sr_name since its a duplicate entry */
      servreg_free(sr_name);
      sr_mon_node = sr_mon_handle2sr_mon_node(sr_mon_handle);
      sr_mon_node->handle_count = sr_mon_node->handle_count + 1;

      SERVREG_TRACE_2(NODE_CREATE_DUP, sr_mon_node, sr_mon_node->handle_count);
   }

   return sr_mon_handle;
}

/** =====================================================================
 * Function:
 *     servreg_service_queue
 *
 * Description:
 *     Function services the state queue for the given monitor handle.
 *     It will set the next pending state of the service.
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by domain + service 
 *                     or just domain name
 *
 * Returns:
 *     None
 * =====================================================================  */
static void servreg_service_queue(SERVREG_MON_HANDLE sr_mon_handle)
{
   servreg_mon_node_p sr_mon_node = SERVREG_NULL;
   servreg_mon_queue_p sr_q_entry = SERVREG_NULL;

   //make get count and signal atomic so the number of listeners doesn't change in between
   //todo: we should consider a mutex within each node to give finer granularity
   servreg_mutex_lock_dal(&(servreg_mon_node_internal.mutex_create));     

   sr_mon_node = sr_mon_handle2sr_mon_node(sr_mon_handle);
   sr_q_entry = (servreg_mon_queue_p)q_check(sr_mon_node->state_q);

   //ack_pending of 0 indicates unserviced since we remove it from Q otherwise
   while (SERVREG_NULL != sr_q_entry && 0 == sr_q_entry->ack_pending_count)
   {
      sr_q_entry->ack_pending_count = rcesn_getregistrants_handle(sr_mon_node->rcesn_handle);

      SERVREG_TRACE_2(SERVICE_Q, sr_mon_node, sr_q_entry->ack_pending_count);

      sr_mon_node->transaction_id = sr_mon_node->transaction_id + 1;
      /* Always signal for rcesn to get correct state */
      if(rcesn_signal_handle(sr_mon_node->rcesn_handle, sr_q_entry->curr_state)){;}

      /* Signal listeners that the state of the service has changed for the first entry in the queue */
      if (sr_q_entry->ack_pending_count > 0)
      {
         /* There are listeners, so wait for acknowledgement */
	 break;        
      }
      else
      {
         /* Continue processing the remainder of the queue */
	 /* Remove from the queue since no listeners attached to it */
         sr_q_entry = (servreg_mon_queue_p)q_get(sr_mon_node->state_q);
         servreg_free(sr_q_entry);

         /* Get the next entry in the queue and loop*/
         sr_q_entry = (servreg_mon_queue_p)q_check(sr_mon_node->state_q);
      }
   }

   servreg_mutex_unlock_dal(&(servreg_mon_node_internal.mutex_create));
   return;
}

/** =====================================================================
 * Function:
 *     servreg_compare_state
 *
 * Description:
 *     Callback function given to the queue API's
 *     Function compares the state of q_entry state and the one passes in as a parameter
 *
 * Parameters:
 *     item_ptr : pointer to the item in the queue
 *     compare_val : service state value to be compared against
 *
 * Returns:
 *     1 : if state matches
 *     0 : if state does not match
 * =====================================================================  */
static int servreg_compare_state(void* item_ptr, void* compare_val)
{
   int ret = 0;

   if(((servreg_mon_queue_p)item_ptr)->curr_state == *((uint32_t *)compare_val))
   {
      ret = 1;
   }
   else
   {
      ret = 0;
   }

   return ret;
}

/** =====================================================================
* Function:
*     servreg_mon_ack_timer_cb
*
* Description:
*     Generic callback task driven function to check the state of the
*     ACKs for all the monitor nodes
*
* Parameters:
*     unused
*
* Returns:
*     none
* =====================================================================  */
void servreg_mon_ack_timer_cb(void* unused)
{
   servreg_mon_node_p sr_mon_node = SERVREG_NULL;
   servreg_mon_queue_p sr_q_entry = SERVREG_NULL;
   uint32_t i = 0;

   //TMS_MSG_HIGH("checking ACKs ");

   servreg_mutex_lock_dal(&(servreg_mon_node_internal.mutex_create));

   for(i = 0; i < SERVREG_MON_NODE_HASHTABLE_BUCKETS; i++)
   {
      sr_mon_node = servreg_mon_internal.servreg_mon_node_hashtab[i];

      while(SERVREG_NULL != sr_mon_node)
      {
         sr_q_entry = (servreg_mon_queue_p)q_check(sr_mon_node->state_q);

         /* An entry in the queue suggests that the head q_entry state is set and its awaiting for the ACKs */
         if(SERVREG_NULL != sr_q_entry)
         {
            /* Decrement the timeout value */
            sr_q_entry->timeout = sr_q_entry->timeout - 1;
            
            SERVREG_TRACE_2(ACK_TIMER_CB, sr_mon_node, sr_q_entry->timeout);
            
            if(sr_q_entry->timeout == 0)
            {
               char * temp = servreg_calloc(sizeof (char), TMS_UTILS_BUFFER_SIZE);
               if ( temp != NULL )
               {
                 (void)tms_utils_fmt(temp, TMS_UTILS_BUFFER_SIZE, "Servreg ack timedout:%s:State:0x%x", sr_mon_node->service_name,sr_q_entry->curr_state );
		             ERR_FATAL_AUX_MSG( "Servreg ack timedout. Check coredump.err.aux_msg", temp, TMS_UTILS_BUFFER_SIZE);
               }
               else
               {
                 ERR_FATAL( "Servreg ack timedout:state:0x%x", sr_q_entry->curr_state, 0, 0);
               }
            }
         }
         sr_mon_node = sr_mon_node->next;
      }
   }

   servreg_mutex_unlock_dal(&(servreg_mon_node_internal.mutex_create));

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
   gen_cb_ctxt_invoke_cb(servreg_mon_ack_timer_cb, NULL);
}

/** =====================================================================
 * Function:
 *     servreg_get_sr_mon_handle
 *
 * Description:
 *     Checks if a MON node already exists with the given name.
 *
 * Parameters:
 *     name : "soc/domain/subdomain/provider/service" or just "soc/domain/subdomain" name
 *
 * Returns:
 *    SERVREG_MON_HANDLE : handle to the mon node
 *
 * Note :
 *    No need of locks as this function will be called internally only by servreg_create_mon_node()
 * =====================================================================  */
SERVREG_MON_HANDLE servreg_get_sr_mon_handle(SERVREG_NAME const name)
{
   servreg_mon_node_p sr_mon_node = SERVREG_NULL;
   SERVREG_MON_HANDLE sr_mon_handle = SERVREG_NULL;

   sr_mon_node = servreg_mon_node_hashtab_get(servreg_nmehash(name, (sizeof(servreg_mon_internal.servreg_mon_node_hashtab) / sizeof(servreg_mon_node_p))));

   while(SERVREG_NULL != sr_mon_node)
   {
      /* If they dont have the same address */
      if(sr_mon_node->service_name != name)
      {
         int len1 = servreg_nmelen(sr_mon_node->service_name);
         int len2 = servreg_nmelen(name);

         if(len1 == len2 && 0 == servreg_nmecmp(sr_mon_node->service_name, name, len1))
         {
            sr_mon_handle = sr_mon_node2sr_mon_handle(sr_mon_node);
            //TMS_MSG_HIGH_1("handle = 0x%x ", sr_mon_handle);
            break;
         }
      }
      else
      {
         sr_mon_handle = sr_mon_node2sr_mon_handle(sr_mon_node);
         //TMS_MSG_HIGH_1("handle = 0x%x", sr_mon_handle);
         break;
      }
      sr_mon_node = sr_mon_node->next;
   }

   return sr_mon_handle;
}

/** =====================================================================
 * Function:
 *     servreg_set_transaction_id
 *
 * Description:
 *     Set the current transaction_id.
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by domain + service 
 *                     or just domain name
 *
 * Returns:
 * =====================================================================  */
void servreg_set_transaction_id(SERVREG_MON_HANDLE sr_mon_handle, int32_t sr_transaction_id)
{
   servreg_mon_node_p sr_mon_node = SERVREG_NULL;

   sr_mon_node = sr_mon_handle2sr_mon_node(sr_mon_handle);

   servreg_mutex_lock_dal(&(servreg_mon_node_internal.mutex_create));
   if(SERVREG_NULL != sr_mon_node)
   {
      if(SERVREG_MON_SIGNATURE == sr_mon_node->mon_signature)
      {
         sr_mon_node->transaction_id =  sr_transaction_id;
         //TMS_MSG_HIGH_1("transaction_id = %d\n ", sr_transaction_id);
      }
      else
      {
         ERR_FATAL("In servreg_set_transaction_id() sr_mon_handle has invalid signature", 0, 0, 0);
      }
   }
   else
   {
      ERR_FATAL("sr_mon_node is NULL", 0, 0, 0);
   }
   servreg_mutex_unlock_dal(&(servreg_mon_node_internal.mutex_create));

   return ;
}

/** =====================================================================
 * Function:
 *     servreg_alloc_monitor_handle
 *
 * Description:
 *     Given the domain + service name, this functions returns a handle
 *
 * Parameters:
 *     domain : "soc/domain/subdomain" info
 *     service : "provider/service" info. This field can be SERVREG_NULL also.
 *
 * Returns:
 *     SERVREG_MON_HANDLE : Opaque handle to a service state
 *                          Check for return value and if it is NOT NULL
 * =====================================================================  */
SERVREG_MON_HANDLE servreg_alloc_monitor_handle(SERVREG_NAME domain, SERVREG_NAME service)
{
   SERVREG_NAME servreg_name = SERVREG_NULL, rcesn_name = SERVREG_NULL;
   SERVREG_MON_HANDLE sr_mon_handle = SERVREG_NULL;
   RCESN_HANDLE rcesn_handle = RCESN_NULL;

   if(SERVREG_SUCCESS == servreg_name_check(domain, service))
   {
      //if(SERVREG_TRUE == servreg_is_local(domain))
      //{
         servreg_name = servreg_concat(domain, service);
      //}
      //else
      //{
      //   servreg_name = servreg_concat(domain, SERVREG_NULL);
      //}

      if(SERVREG_NULL != servreg_name)
      {
         servreg_mutex_lock_dal(&(servreg_mon_node_internal.mutex_create));

         /* First create the rcesn node with "sr:xxx" name */
         rcesn_name = servreg_create_rcesn_name(servreg_name);

         if(SERVREG_NULL != rcesn_name)
         {
            rcesn_handle = rcesn_create_name(rcesn_name);

            /* Free the space allocated for rcesn_name as rcesn internally does a strlcpy of the name */
            servreg_free(rcesn_name);
         }

         if(RCESN_NULL != rcesn_handle)
         {
            /* Then Create the MON node */
            sr_mon_handle = servreg_create_mon_node(servreg_name, rcesn_handle);

            if(SERVREG_NULL != sr_mon_handle)
            {
               //TMS_MSG_HIGH_1("handle = 0x%x ", sr_mon_handle);
            }
            else
            {
               TMS_MSG_ERROR("sr_mon_handle NULL");
            }
         }
         else
         {
            ERR_FATAL( "In servreg_alloc_monitor_handle() rcesn_handle not created", 0, 0, 0);
         }

         servreg_mutex_unlock_dal(&(servreg_mon_node_internal.mutex_create));
      }
      else
      {
         TMS_MSG_ERROR("servreg_name NULL");
      }
   }
   else
   {
      TMS_MSG_ERROR("Invalid domain and/or service name");
   }

   return sr_mon_handle;
}

/** =====================================================================
 * Function:
 *     servreg_free_monitor_handle
 *
 * Description:
 *     Free the monitor handle that is associated to an event mapped by domain+service name
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by domain + service 
 *                     or just domain name
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 *
 * Note : 
 *     If clients are no longer interested in the service AND they have de-registered their listeners
 *     using servreg_deregister_listener(), they can free the handle by calling servreg_free_monitor_handle().
 * =====================================================================  */
SERVREG_RESULT servreg_free_monitor_handle(SERVREG_MON_HANDLE sr_mon_handle)
{
   servreg_mon_node_p sr_mon_node = SERVREG_NULL;
   SERVREG_RESULT ret = SERVREG_FAILURE;

   servreg_mutex_lock_dal(&(servreg_mon_node_internal.mutex_create));

   sr_mon_node = sr_mon_handle2sr_mon_node(sr_mon_handle);

   if(SERVREG_NULL != sr_mon_node)
   {
      if(SERVREG_MON_SIGNATURE == sr_mon_node->mon_signature)
      {
         if(sr_mon_node->handle_count != 0)
         {
            sr_mon_node->handle_count = sr_mon_node->handle_count - 1;

            if(sr_mon_node->handle_count == 0)
            {
               /* If its a remote service */
               if(SERVREG_TRUE != servreg_is_domain_local(sr_mon_node->service_name))
               {
                  /* via QMI and/or QDI */
                  ret = servreg_free_remote_handle(sr_mon_node->service_name, sr_mon_handle);
               }

               /* Delete the entry from the hash table */
               servreg_mon_node_hashtab_delete(sr_mon_node, servreg_nmehash(sr_mon_node->service_name, (sizeof(servreg_mon_internal.servreg_mon_node_hashtab) /  sizeof(servreg_mon_node_p))));

               /* Reclaim back the mon node to free pool */
               if(SERVREG_NULL != servreg_mon_node_pool_free(sr_mon_node))
               {
                  ret = SERVREG_SUCCESS;
               }
            }
            else
            {
               /* Will free monitor handle if handle_count is 0: All clients have de-registered */
               ret = SERVREG_SUCCESS;
            }
         }
      }
      else
      {
         TMS_MSG_ERROR("sr_mon_handle has invalid signature");
         ret = SERVREG_INVALID_HANDLE;
      }
   }
   else
   {
      TMS_MSG_ERROR("sr_mon_handle NULL");
      ret = SERVREG_INVALID_HANDLE;
   }

   servreg_mutex_unlock_dal(&(servreg_mon_node_internal.mutex_create));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_set_state
 *
 * Description:
 *     Function sets the state of the service
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by domain + service 
 *                     or just domain name
 *     state         : State of the service. See enum SERVREG_SERVICE_STATE for options
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_set_state(SERVREG_MON_HANDLE sr_mon_handle, SERVREG_SERVICE_STATE state)
{
   servreg_mon_node_p sr_mon_node = SERVREG_NULL;
   servreg_mon_queue_p sr_q_entry = SERVREG_NULL;
   SERVREG_RESULT ret = SERVREG_FAILURE;
   SERVREG_SERVICE_STATE curr_state = SERVREG_SERVICE_STATE_UNINIT;
   SERVREG_BOOL valid_state = SERVREG_TRUE;

   servreg_mutex_lock_dal(&(servreg_mon_node_internal.mutex_create));
   sr_mon_node = sr_mon_handle2sr_mon_node(sr_mon_handle);

   if(SERVREG_NULL != sr_mon_node)
   {
      if(SERVREG_MON_SIGNATURE == sr_mon_node->mon_signature)
      {
         curr_state = (SERVREG_SERVICE_STATE)rcesn_getstatecurr_handle(sr_mon_node->rcesn_handle);

         switch(curr_state)
         {
            case SERVREG_SERVICE_STATE_DOWN :
            case SERVREG_SERVICE_STATE_UNINIT:
            {
               if(state != SERVREG_SERVICE_STATE_UP)
               {
                  ret = SERVREG_SUCCESS;
                  valid_state = SERVREG_FALSE;
                  TMS_MSG_ERROR_2("Invalid transition in set_state(): 0x%x -> 0x%x", curr_state, state);
               }
            }
            break;
            case SERVREG_SERVICE_STATE_UP :
            {
               if(state != SERVREG_SERVICE_STATE_DOWN)
               {
                  ret = SERVREG_SUCCESS;
                  valid_state = SERVREG_FALSE;
               }
            }
            break;
            default : 
            {
               ret = SERVREG_FAILURE;
               valid_state = SERVREG_FALSE;
            }
            break;
         }

         /* Signal only for state change */
         if(SERVREG_TRUE == sr_mon_node->servreg_sync && valid_state == SERVREG_TRUE)
         {
            sr_q_entry = (servreg_mon_queue_p)servreg_calloc(sizeof(servreg_mon_queue_t), 1);

            if(SERVREG_NULL != sr_q_entry)
            {
               SERVREG_TRACE_2(SET_STATE, sr_mon_node, state);

               sr_q_entry->ack_pending_count = 0; //set this when we send notifications
               sr_q_entry->curr_state = state;
               sr_q_entry->timeout = SERVREG_MON_ACK_TIMEOUT_COUNT;
            }
            else
            {
               ERR_FATAL("In servreg_set_state() Calloc failed", 0, 0, 0);
            }

            q_put(sr_mon_node->state_q, q_link(sr_q_entry, &sr_q_entry->link));
            servreg_service_queue(sr_mon_node);
            ret = SERVREG_SUCCESS;
         }
         else if(valid_state == SERVREG_TRUE)
         {
            /* Signal all the listeners that the state of the service has changed if not a sync service */
            if (RCESN_NULL == rcesn_signal_handle(sr_mon_node->rcesn_handle, state))
            {
               TMS_MSG_ERROR("sr_mon_node->rcesn_handle not created");
               ret = SERVREG_FAILURE;
            }
            else
            {
               sr_mon_node->transaction_id = sr_mon_node->transaction_id + 1;
               ret = SERVREG_SUCCESS;
               //TMS_MSG_HIGH_1("success, service name and state = 0x%x\n ", state);
            }
         }
      }
      else
      {
         TMS_MSG_ERROR("sr_mon_handle has invalid signature");
         ret = SERVREG_INVALID_HANDLE;
      }
   }
   else
   {
      TMS_MSG_ERROR("sr_mon_node NULL");
      ret = SERVREG_INVALID_HANDLE;
   }
   servreg_mutex_unlock_dal(&(servreg_mon_node_internal.mutex_create));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_get_service_name
 *
 * Description:
 *     Given the monitor handle, the function returns the service name 
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by domain + service 
 *                     or just domain name
 *
 * Returns:
 *     service_name or SERVREG_NULL
 * =====================================================================  */
SERVREG_NAME servreg_get_service_name(SERVREG_MON_HANDLE sr_mon_handle)
{
   servreg_mon_node_p sr_mon_node = SERVREG_NULL;
   SERVREG_NAME ret = SERVREG_NULL;

   sr_mon_node = sr_mon_handle2sr_mon_node(sr_mon_handle);

   servreg_mutex_lock_dal(&(servreg_mon_node_internal.mutex_create));
   if(SERVREG_NULL != sr_mon_node)
   {
      if(SERVREG_MON_SIGNATURE == sr_mon_node->mon_signature)
      {
         ret = sr_mon_node->service_name;
         //TMS_MSG_HIGH_1("name = %s ", *ret);
      }
      else
      {
         TMS_MSG_ERROR("sr_mon_handle has invalid signature");
      }
   }
   else
   {
      TMS_MSG_ERROR("sr_mon_node NULL");
   }
   servreg_mutex_unlock_dal(&(servreg_mon_node_internal.mutex_create));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_set_synchronous
 *
 * Description:
 *     Function sets the service state to be synchronous 
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by domain + service 
 *                     or just domain name
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_set_synchronous(SERVREG_MON_HANDLE sr_mon_handle)
{
   servreg_mon_node_p sr_mon_node = SERVREG_NULL;
   SERVREG_RESULT ret = SERVREG_FAILURE;

   sr_mon_node = sr_mon_handle2sr_mon_node(sr_mon_handle);

   servreg_mutex_lock_dal(&(servreg_mon_node_internal.mutex_create));
   if(SERVREG_NULL != sr_mon_node)
   {
      if(SERVREG_MON_SIGNATURE == sr_mon_node->mon_signature)
      {
         sr_mon_node->servreg_sync = SERVREG_TRUE;
         ret = SERVREG_SUCCESS;
         //TMS_MSG_HIGH("service set as sync ");
      }
      else
      {
         TMS_MSG_ERROR("sr_mon_handle has invalid signature");
         ret = SERVREG_INVALID_HANDLE;
      }
   }
   else
   {
      TMS_MSG_ERROR("sr_mon_node NULL");
      ret = SERVREG_INVALID_HANDLE;
   }
   servreg_mutex_unlock_dal(&(servreg_mon_node_internal.mutex_create));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_set_asynchronous
 *
 * Description:
 *     Function sets the service state to be asynchronous 
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by domain + service 
 *                     or just domain name
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_set_asynchronous(SERVREG_MON_HANDLE sr_mon_handle)
{
   servreg_mon_node_p sr_mon_node = SERVREG_NULL;
   SERVREG_RESULT ret = SERVREG_FAILURE;

   sr_mon_node = sr_mon_handle2sr_mon_node(sr_mon_handle);

   servreg_mutex_lock_dal(&(servreg_mon_node_internal.mutex_create));
   if(SERVREG_NULL != sr_mon_node)
   {
      if(SERVREG_MON_SIGNATURE == sr_mon_node->mon_signature)
      {
         sr_mon_node->servreg_sync = SERVREG_FALSE;
         ret = SERVREG_SUCCESS;
         //TMS_MSG_HIGH("service set as async ");
      }
      else
      {
         TMS_MSG_ERROR("sr_mon_handle has invalid signature");
         ret = SERVREG_INVALID_HANDLE;
      }
   }
   else
   {
      TMS_MSG_ERROR("sr_mon_node NULL");
      ret = SERVREG_INVALID_HANDLE;
   }
   servreg_mutex_unlock_dal(&(servreg_mon_node_internal.mutex_create));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_get_synchronous
 *
 * Description:
 *     Function to get the sync state of the service
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by domain + service 
 *                     or just domain name
 *
 * Returns:
 *    TRUE if the sync state is set to TRUE, else FALSE
 * =====================================================================  */
SERVREG_BOOL servreg_get_synchronous(SERVREG_MON_HANDLE sr_mon_handle)
{
   servreg_mon_node_p sr_mon_node = SERVREG_NULL;
   SERVREG_BOOL ret = SERVREG_FALSE;

   sr_mon_node = sr_mon_handle2sr_mon_node(sr_mon_handle);

   servreg_mutex_lock_dal(&(servreg_mon_node_internal.mutex_create));
   if(SERVREG_NULL != sr_mon_node)
   {
      if(SERVREG_MON_SIGNATURE == sr_mon_node->mon_signature)
      {
         ret = sr_mon_node->servreg_sync;
         //TMS_MSG_HIGH_1("service sync state = %d ", ret);
      }
      else
      {
         ERR_FATAL("In servreg_get_synchronous() sr_mon_handle has invalid signature", 0, 0, 0);
      }
   }
   else
   {
      ERR_FATAL("In servreg_get_synchronous() sr_mon_node is NULL", 0, 0, 0);
   }
   servreg_mutex_unlock_dal(&(servreg_mon_node_internal.mutex_create));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_get_pending_acks
 *
 * Description:
 *     Function gets the pending ack count for the service state
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by domain + service 
 *                     or just domain name
 *     state         : The service state for which the pending ack is requested
 * Returns:
 *     Ack count
 * =====================================================================  */
uint32_t servreg_get_pending_acks(SERVREG_MON_HANDLE sr_mon_handle, SERVREG_SERVICE_STATE state)
{
   servreg_mon_node_p sr_mon_node = SERVREG_NULL;
   servreg_mon_queue_p sr_q_entry = SERVREG_NULL;
   uint32_t ret = 0;

   sr_mon_node = sr_mon_handle2sr_mon_node(sr_mon_handle);

   servreg_mutex_lock_dal(&(servreg_mon_node_internal.mutex_create));
   if(SERVREG_NULL != sr_mon_node)
   {
      if(SERVREG_MON_SIGNATURE == sr_mon_node->mon_signature)
      {
         sr_q_entry = (servreg_mon_queue_p)q_linear_search(sr_mon_node->state_q, servreg_compare_state, &state);

         if(SERVREG_NULL != sr_q_entry)
         {
            ret = sr_q_entry->ack_pending_count;
            TMS_MSG_HIGH_1("Service has pending acks: %d", ret);
         }
         else
         {
            TMS_MSG_HIGH("State does not exist in queue, so ACK:0");
         }
      }
      else
      {
         ERR_FATAL("In servreg_get_pending_acks() sr_mon_handle has invalid signature", 0, 0, 0);
      }
   }
   else
   {
      ERR_FATAL("sr_mon_node NULL", 0, 0, 0);
   }
   servreg_mutex_unlock_dal(&(servreg_mon_node_internal.mutex_create));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_register_listener
 *
 * Description:
 *     Register a listener to get event notifications for a service it is interested in
 *
 * Parameters:
 *     sr_mon_handle        : Handle to an existing service state which is mapped by domain + service 
 *                            or just domain name. This is the service that the listener is interested in.
 *     servreg_signal_type  : NHLOS specific notification signal type information
 *     signal               : NHLOS specific notification signal & mask information
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_register_listener(SERVREG_MON_HANDLE sr_mon_handle, SERVREG_SIGEX_TYPE servreg_signal_type, SERVREG_SIGEX servreg_signal)
{
   servreg_mon_node_p sr_mon_node = SERVREG_NULL;
   SERVREG_RESULT ret = SERVREG_FAILURE;
   /* Initialized as SERVREG_SERVICE_STATE_UNINIT */
   uint32_t curr_remote_state = 0x7FFFFFFF;
   int32_t remote_transaction_id = 0;
   RCESN_HANDLE rcesn_h = NULL;

   sr_mon_node = sr_mon_handle2sr_mon_node(sr_mon_handle);
   SERVREG_TRACE_2(COMMON_REG, sr_mon_node, qurt_thread_get_id());

   servreg_mutex_lock_dal(&(servreg_mon_node_internal.mutex_create));

   if(SERVREG_NULL != sr_mon_node)
   {
      if(SERVREG_MON_SIGNATURE == sr_mon_node->mon_signature)
      {
         if(servreg_signal_type == SERVREG_SIGEX_TYPE_SIGQURT)
         {
            rcesn_h = SERVREG_RCESN_REGISTER_HANDLE_QURT(sr_mon_node->rcesn_handle, (RCESN_FNSIG_COMPARE)rcesn_compare_ne_prev, servreg_signal);
         }
         else
         {
            rcesn_h = SERVREG_RCESN_REGISTER_HANDLE_REX(sr_mon_node->rcesn_handle, (RCESN_FNSIG_COMPARE)rcesn_compare_ne_prev, servreg_signal);
         }

         if (RCESN_NULL != rcesn_h)
         {
            sr_mon_node->rcesn_handle = rcesn_h; 
            sr_mon_node->listener_ref_count = sr_mon_node->listener_ref_count + 1;

            /* TRUE == service is in same "asic/domain/subd" regardless of provider/service */
            if(SERVREG_TRUE != servreg_is_local(sr_mon_node->service_name))
            {
               SERVREG_TRACE_0(COMMON_REG_R);
               /* If its a remote service, then the service is automatically treated as synchronous service */
               sr_mon_node->servreg_sync = SERVREG_TRUE;
            }

            /* TRUE == we are "root_PD" and service is in same subsystem
                       OR
                       we are user pd and service is in same user pd */
            if(SERVREG_TRUE != servreg_is_domain_local(sr_mon_node->service_name))
            {
               SERVREG_TRACE_0(COMMON_REG_RD);
               /* If there is already a registered remote listener then curr state and transaction id will already have the state of the remote service, else it will be updated via the QMI call */
               curr_remote_state = (uint32_t)rcesn_getstatecurr_handle(sr_mon_node->rcesn_handle);
               remote_transaction_id = sr_mon_node->transaction_id;

               /* via QMI and/or QDI */
               ret = servreg_register_remote_listener(sr_mon_node->service_name, sr_mon_handle, &curr_remote_state, &remote_transaction_id);

               if(SERVREG_SUCCESS == ret)
               {
                  SERVREG_TRACE_0(SUCCESS);
                  /* Update the current remote state and remote transaction_id */
                  if(rcesn_setstatecurr_handle(sr_mon_node->rcesn_handle, curr_remote_state)){;}
                  sr_mon_node->transaction_id = remote_transaction_id;
                  TMS_MSG_HIGH("Client registered for remote service");
               }
               else
               {
                  SERVREG_TRACE_0(FAIL);
                  if(servreg_signal_type == SERVREG_SIGEX_TYPE_SIGQURT)
                     rcesn_h = SERVREG_RCESN_UNREGISTER_HANDLE_QURT(sr_mon_node->rcesn_handle, (RCESN_FNSIG_COMPARE)rcesn_compare_ne_prev, servreg_signal);
                  else
                     rcesn_h = SERVREG_RCESN_UNREGISTER_HANDLE_REX(sr_mon_node->rcesn_handle, (RCESN_FNSIG_COMPARE)rcesn_compare_ne_prev, servreg_signal);
                  
                  if (RCESN_NULL != rcesn_h)
                     sr_mon_node->rcesn_handle = rcesn_h;
                  sr_mon_node->listener_ref_count = sr_mon_node->listener_ref_count - 1;
                  if(sr_mon_node->listener_ref_count != 0)
                  {
                    SERVREG_TRACE_2(COMMON_REG_FAIL, sr_mon_node, sr_mon_node->listener_ref_count);
                    TMS_MSG_ERROR_1("Remote listener_ref_count NON ZERO %d", sr_mon_node->listener_ref_count);
                  }
                  TMS_MSG_ERROR_1("Client registration failed for remote service ret: %d", ret);
               }
            }
            else
            {
               SERVREG_TRACE_0(COMMON_REG_L);
               /* Service level registration */
               /* Register only one proxy listener */
               /* TRUE == the PD name matches our PD name
                          OR
                          there is only subd (PD name), no service (LA proxy) */
               if(SERVREG_TRUE != servreg_is_service_local(sr_mon_node->service_name) && sr_mon_node->listener_ref_count <= 1)
               {
                  SERVREG_TRACE_0(COMMON_REG_LS);
                  ret = servreg_register_user_listener(sr_mon_node->service_name, sr_mon_handle, &curr_remote_state, &remote_transaction_id);

                  if(SERVREG_SUCCESS == ret)
                  {
                     /* this is where we should increment the listener count */
                     SERVREG_TRACE_0(SUCCESS);
                     /* Update the current remote state and remote transaction_id */
                     if(rcesn_setstatecurr_handle(sr_mon_node->rcesn_handle, curr_remote_state)){;}
                     sr_mon_node->transaction_id = remote_transaction_id;
                     TMS_MSG_HIGH("Client registered for remote service");
                  }
                  else
                  {
                     SERVREG_TRACE_0(FAIL);
                     if(servreg_signal_type == SERVREG_SIGEX_TYPE_SIGQURT)
                        rcesn_h = SERVREG_RCESN_UNREGISTER_HANDLE_QURT(sr_mon_node->rcesn_handle, (RCESN_FNSIG_COMPARE)rcesn_compare_ne_prev, servreg_signal);
                     else
                        rcesn_h = SERVREG_RCESN_UNREGISTER_HANDLE_REX(sr_mon_node->rcesn_handle, (RCESN_FNSIG_COMPARE)rcesn_compare_ne_prev, servreg_signal);                     
                     if (RCESN_NULL != rcesn_h)
                        sr_mon_node->rcesn_handle = rcesn_h;
                    
                     sr_mon_node->listener_ref_count = sr_mon_node->listener_ref_count - 1;
                     if(sr_mon_node->listener_ref_count != 0)
                     {
                       SERVREG_TRACE_2(COMMON_REG_FAIL, sr_mon_node, sr_mon_node->listener_ref_count);
                       TMS_MSG_ERROR_1("User listener_ref_count NON ZERO %d", sr_mon_node->listener_ref_count);
                     }
                     TMS_MSG_ERROR_1("Client registration failed for remote service ret: %d", ret);
                  }
               }
               else
               {
                  SERVREG_TRACE_0(SUCCESS);
                  /* Only a local service */
                  ret = SERVREG_SUCCESS;
                  TMS_MSG_HIGH("Client registered for local service");
               }
            }
         }
         else
         {
            SERVREG_TRACE_0(FAIL);
            TMS_MSG_ERROR("sr_mon_node->rcesn_handle not created");
            ret = SERVREG_FAILURE;
         }
      }
      else
      {
         SERVREG_TRACE_0(FAIL);
         TMS_MSG_ERROR("sr_mon_handle has invalid signature");
         ret = SERVREG_INVALID_HANDLE;
      }
   }
   else
   {
      SERVREG_TRACE_0(FAIL);
      TMS_MSG_ERROR("sr_mon_node NULL");
      ret = SERVREG_INVALID_HANDLE;
   }
   SERVREG_TRACE_2(COMMON_REG_EXIT, sr_mon_node, qurt_thread_get_id());
   servreg_mutex_unlock_dal(&(servreg_mon_node_internal.mutex_create));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_deregister_listener
 *
 * Description:
 *     De-Register a listener to get event notifications for a service
 *
 * Parameters:
 *     sr_mon_handle        : Handle to an existing service state which is mapped by domain + service 
 *                            or just domain name
 *     servreg_signal_type  : NHLOS specific notification signal type information
 *     signal               : NHLOS specific notification signal & mask information
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 *
 * Note : 
 *     If clients are no longer interested in the service AND they have de-registered their listeners
 *     using servreg_deregister_listener(), they can free the handle by calling servreg_free_monitor_handle().
 * =====================================================================  */
SERVREG_RESULT servreg_deregister_listener(SERVREG_MON_HANDLE sr_mon_handle, SERVREG_SIGEX_TYPE servreg_signal_type, SERVREG_SIGEX servreg_signal)
{
   servreg_mon_node_p sr_mon_node = SERVREG_NULL;
   SERVREG_RESULT ret = SERVREG_FAILURE;
   RCESN_HANDLE rcesn_h = NULL;

   sr_mon_node = sr_mon_handle2sr_mon_node(sr_mon_handle);
   SERVREG_TRACE_2(COMMON_DEREG, sr_mon_node, 0);

   servreg_mutex_lock_dal(&(servreg_mon_node_internal.mutex_create));
   if(SERVREG_NULL != sr_mon_node)
   {
      if(SERVREG_MON_SIGNATURE == sr_mon_node->mon_signature)
      {
         if(servreg_signal_type == SERVREG_SIGEX_TYPE_SIGQURT)
            rcesn_h = SERVREG_RCESN_UNREGISTER_HANDLE_QURT(sr_mon_node->rcesn_handle, (RCESN_FNSIG_COMPARE)rcesn_compare_ne_prev, servreg_signal);
         else
            rcesn_h = SERVREG_RCESN_UNREGISTER_HANDLE_REX(sr_mon_node->rcesn_handle, (RCESN_FNSIG_COMPARE)rcesn_compare_ne_prev, servreg_signal);
         
         if (RCESN_NULL != rcesn_h)
         {
            sr_mon_node->rcesn_handle = rcesn_h;
            if(sr_mon_node->listener_ref_count != 0)
            {
               sr_mon_node->listener_ref_count = sr_mon_node->listener_ref_count - 1;

               /* If its registered as a remote service, de-register it */
               if(SERVREG_TRUE != servreg_is_domain_local(sr_mon_node->service_name))
               {
                  /* Call only if listener_ref_count is 0: All clients have de-registered */
                  if(sr_mon_node->listener_ref_count == 0)
                  {
                     /* via QMI and/or QDI */
                     ret = servreg_deregister_remote_listener(sr_mon_node->service_name, sr_mon_handle);

                     if(SERVREG_SUCCESS == ret)
                     {
                        TMS_MSG_HIGH("All clients deregistered for remote service");
                     }
                     else
                     {
                        TMS_MSG_ERROR_1("Client deregistration failed for remote service ret: %d", ret);
                     }
                  }
                  else
                  {
                     TMS_MSG_HIGH("Client deregistered for remote service");
                     ret = SERVREG_SUCCESS;
                  }
               }
               else
               {
                  /* Only a local service */
                  ret = SERVREG_SUCCESS;
                  TMS_MSG_HIGH("Client deregistered for local service");
               }
            }
            else
            {
               TMS_MSG_ERROR("No listeners registered to de-register them");
               ret = SERVREG_FAILURE;
            }
         }
         else
         {
            TMS_MSG_ERROR("sr_mon_node->rcesn_handle not created");
            ret = SERVREG_FAILURE;
         }
      }
      else
      {
         TMS_MSG_ERROR("sr_mon_handle has invalid signature");
         ret = SERVREG_INVALID_HANDLE;
      }
   }
   else
   {
      TMS_MSG_ERROR("sr_mon_node NULL");
      ret = SERVREG_INVALID_HANDLE;
   }
   servreg_mutex_unlock_dal(&(servreg_mon_node_internal.mutex_create));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_get_listener_ref_count
 *
 * Description:
 *     Get the listener reference count for the given montior handle
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by domain + service 
 *                     or just domain name
 *
 * Returns:
 *     Listener ref count
 * =====================================================================  */
uint32_t servreg_get_listener_ref_count(SERVREG_MON_HANDLE sr_mon_handle)
{
   servreg_mon_node_p sr_mon_node = SERVREG_NULL;
   uint32_t ret = 0;

   sr_mon_node = sr_mon_handle2sr_mon_node(sr_mon_handle);

   servreg_mutex_lock_dal(&(servreg_mon_node_internal.mutex_create));
   if(SERVREG_NULL != sr_mon_node)
   {
      if(SERVREG_MON_SIGNATURE == sr_mon_node->mon_signature)
      {
         ret = sr_mon_node->listener_ref_count;
         //TMS_MSG_HIGH_1("Ref count: %d", ret);
      }
      else
      {
         ERR_FATAL("In servreg_get_listener_ref_count() sr_mon_handle has invalid signature", 0, 0, 0);
      }
   }
   else
   {
      ERR_FATAL("sr_mon_node NULL", 0, 0, 0);
   }
   servreg_mutex_unlock_dal(&(servreg_mon_node_internal.mutex_create));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_get_service_curr_state
 *
 * Description:
 *     Get the current state of a service that the listener is interested in
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by domain + service 
 *                     or just domain name
 *
 * Returns:
 *     SERVREG_SERVICE_STATE : Returns the state of the service. 
 *                             Refer to the enum SERVREG_SERVICE_STATE for list of states
 * =====================================================================  */
SERVREG_SERVICE_STATE servreg_get_service_curr_state(SERVREG_MON_HANDLE sr_mon_handle)
{
   servreg_mon_node_p sr_mon_node = SERVREG_NULL;
   SERVREG_SERVICE_STATE ret = SERVREG_SERVICE_STATE_UNINIT;

   sr_mon_node = sr_mon_handle2sr_mon_node(sr_mon_handle);

   servreg_mutex_lock_dal(&(servreg_mon_node_internal.mutex_create));
   if(SERVREG_NULL != sr_mon_node)
   {
      if(SERVREG_MON_SIGNATURE == sr_mon_node->mon_signature)
      {
         ret = (SERVREG_SERVICE_STATE)rcesn_getstatecurr_handle(sr_mon_node->rcesn_handle);
         //TMS_MSG_HIGH_1("curr state = 0x%x\n ", ret);
      }
      else
      {
         ERR_FATAL("In servreg_get_service_curr_state() sr_mon_handle has invalid signature", 0, 0, 0);
      }
   }
   else
   {
      ERR_FATAL("In servreg_get_service_curr_state() sr_mon_node NULL", 0, 0, 0);
   }
   servreg_mutex_unlock_dal(&(servreg_mon_node_internal.mutex_create));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_get_transaction_id
 *
 * Description:
 *     Get the current transaction_id.
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by domain + service 
 *                     or just domain name
 *
 * Returns:
 *     Current transaction if for that service
 * =====================================================================  */
int32_t servreg_get_transaction_id(SERVREG_MON_HANDLE sr_mon_handle)
{
   servreg_mon_node_p sr_mon_node = SERVREG_NULL;
   int32_t ret = 0;

   sr_mon_node = sr_mon_handle2sr_mon_node(sr_mon_handle);

   servreg_mutex_lock_dal(&(servreg_mon_node_internal.mutex_create));
   if(SERVREG_NULL != sr_mon_node)
   {
      if(SERVREG_MON_SIGNATURE == sr_mon_node->mon_signature)
      {
         ret = sr_mon_node->transaction_id;
         //TMS_MSG_HIGH_1("transaction_id = %d\n ", ret);
      }
      else
      {
         ERR_FATAL("In servreg_get_transaction_id() sr_mon_handle has invalid signature", 0, 0, 0);
      }
   }
   else
   {
      ERR_FATAL("sr_mon_node NULL", 0, 0, 0);
   }
   servreg_mutex_unlock_dal(&(servreg_mon_node_internal.mutex_create));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_set_ack
 *
 * Description:
 *     Function increases the ack count by one for that service
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by domain + service 
 *                     or just domain name
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_set_ack(SERVREG_MON_HANDLE sr_mon_handle, int32_t transaction_id)
{
   servreg_mon_node_p sr_mon_node = SERVREG_NULL;
   SERVREG_RESULT ret = SERVREG_FAILURE;
   servreg_mon_queue_p sr_q_entry = SERVREG_NULL;

   sr_mon_node = sr_mon_handle2sr_mon_node(sr_mon_handle);

   servreg_mutex_lock_dal(&(servreg_mon_node_internal.mutex_create));

   if(SERVREG_NULL != sr_mon_node)
   {
      if(SERVREG_MON_SIGNATURE == sr_mon_node->mon_signature)
      {
         if(SERVREG_TRUE == sr_mon_node->servreg_sync && transaction_id == sr_mon_node->transaction_id)
         {
            sr_q_entry = (servreg_mon_queue_p)q_check(sr_mon_node->state_q);

            if(SERVREG_NULL != sr_q_entry)
            {
               sr_q_entry->ack_pending_count = sr_q_entry->ack_pending_count - 1;
               SERVREG_TRACE_2(COMMON_ACK, sr_mon_handle, sr_q_entry->ack_pending_count);

               if(sr_q_entry->ack_pending_count == 0)
               {
                  if(SERVREG_SERVICE_STATE_UP == sr_q_entry->curr_state && RCECB_NULL != sr_mon_node->rcecb_up_handle)
                  {
                     SERVREG_TRACE_2(COMMON_ACK_ALL, sr_mon_handle, SERVREG_SERVICE_STATE_UP);

                     /* Callback function called only after all the ACK's are received */
                     if(rcecb_signal_handle(sr_mon_node->rcecb_up_handle)){;}
                  }
                  else if(SERVREG_SERVICE_STATE_DOWN == sr_q_entry->curr_state && RCECB_NULL != sr_mon_node->rcecb_down_handle)
                  {
                     SERVREG_TRACE_2(COMMON_ACK_ALL, sr_mon_handle, SERVREG_SERVICE_STATE_DOWN);
                     /* Callback function called only after all the ACK's are received */
                     if(rcecb_signal_handle(sr_mon_node->rcecb_down_handle)){;}
                  }

                  /* If its a remote service */
                  if(SERVREG_TRUE != servreg_is_domain_local(sr_mon_node->service_name))
                  {
                     SERVREG_TRACE_2(COMMON_ACK_REMOTE, sr_mon_node, sr_mon_node->transaction_id);
                     /* via QMI and/or QDI */
                     ret = servreg_set_remote_ack(sr_mon_node->service_name, sr_mon_handle, sr_q_entry->curr_state, sr_mon_node->transaction_id);
                  }
                  else
                  {
                     /* Service level registration */
                     /* Set only one ack as only one proxy listener */
                     if(SERVREG_TRUE != servreg_is_service_local(sr_mon_node->service_name))
                     {
                        ret = servreg_set_user_ack(sr_mon_node->service_name, sr_mon_handle, sr_q_entry->curr_state, sr_mon_node->transaction_id);

                        if(SERVREG_SUCCESS != ret)
                        {
                           TMS_MSG_ERROR_1("Client registration failed for remote service ret: %d", ret);
                        }
                     }
                     else
                     {
                        ret = SERVREG_SUCCESS;
                        TMS_MSG_HIGH_1("One ack set, pending acks: %d", sr_q_entry->ack_pending_count);
                     }
                  }

                  /* Remove from the Queue when all the local ACK's are received */
                  sr_q_entry = (servreg_mon_queue_p)q_get(sr_mon_node->state_q);
                  servreg_free(sr_q_entry);

                  /* Process the next entry in the queue */
                  servreg_service_queue(sr_mon_handle);
               }
               else
               {
                  ret = SERVREG_SUCCESS;
                  TMS_MSG_HIGH_1("One ack set, pending acks: %d", sr_q_entry->ack_pending_count);
               }
            }
            else
            {
               TMS_MSG_ERROR("Sending ACK before state change notification sent out");
               //ERR_FATAL("SERVREG_MON: in servreg_set_ack() sending ACK before state change notification sent out ", 0, 0, 0);
               ret = SERVREG_FAILURE;
            }
         }
         else
         {
            if (SERVREG_TRUE != sr_mon_node->servreg_sync)
            {
               TMS_MSG_ERROR("Cannot set ACK: ASYNC service");
            }
            else if (transaction_id != sr_mon_node->transaction_id)
            {
               TMS_MSG_ERROR("Cannot set ACK: given transaction_id invalid");
            }
            
            ret = SERVREG_SUCCESS;
         }
      }
      else
      {
         TMS_MSG_ERROR("sr_mon_handle has invalid signature");
         //ERR_FATAL("SERVREG_MON: in servreg_set_ack() sr_mon_handle has invalid signature ", 0, 0, 0);
         ret = SERVREG_INVALID_HANDLE;
      }
   }
   else
   {
      TMS_MSG_ERROR("sr_mon_node NULL");
      //ERR_FATAL("ERVREG_MON: in servreg_set_ack() sr_mon_node is NULL ", 0, 0, 0);
      ret = SERVREG_INVALID_HANDLE;
   }
   servreg_mutex_unlock_dal(&(servreg_mon_node_internal.mutex_create));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_register_sync_notif_cb
 *
 * Description:
 *     Register with a callback function to get sync notification for that service state
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by domain + service 
 *                     or just domain name
 *     state         : The service state for which the client wants to get the sync notification
 *                     Should preferably be SERVREG_SERVICE_STATE_DOWN or SERVREG_SERVICE_STATE_UP
 *     callback      : Callback function registered
 *     cb_p1         : Callback function parameter. Can be set to sr_mon_handle
 *     cb_p2         : Callback function parameter 2. Can be set to the service state.
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_register_sync_notif_cb(SERVREG_MON_HANDLE sr_mon_handle, SERVREG_SERVICE_STATE state, SERVREG_FNSIG_CALLBACK const callback, SERVREG_CB_PARAM cb_p1, SERVREG_CB_PARAM cb_p2)
{
   servreg_mon_node_p sr_mon_node = SERVREG_NULL;
   SERVREG_NAME sync_name = SERVREG_NULL;
   SERVREG_RESULT ret = SERVREG_FAILURE;

   sr_mon_node = sr_mon_handle2sr_mon_node(sr_mon_handle);

   servreg_mutex_lock_dal(&(servreg_mon_node_internal.mutex_create));
   if(SERVREG_NULL != sr_mon_node && SERVREG_NULL != callback)
   {
      if(SERVREG_MON_SIGNATURE == sr_mon_node->mon_signature)
      {
         sync_name = servreg_create_sync_name(sr_mon_node->service_name, state);

         if(SERVREG_NULL != sync_name)
         {
            if(SERVREG_SERVICE_STATE_UP == state)
            {
               /* Should return an existing handle if event already created, else crate a new one */
               sr_mon_node->rcecb_up_handle = rcecb_create_name(sync_name);

               if(RCECB_NULL != sr_mon_node->rcecb_up_handle)
               {
                  sr_mon_node->rcecb_up_handle = rcecb_register_parm2_handle(sr_mon_node->rcecb_up_handle, (RCECB_CALLBACK_FNSIG_P2)callback, (RCECB_PARM)cb_p1, (RCECB_PARM)cb_p2);
                  TMS_MSG_HIGH("Client registered for up sync notification");
                  ret = SERVREG_SUCCESS;
               }
               else
               {
                  ERR_FATAL("In servreg_register_sync_notif_cb() rcecb_up_handle NULL", 0, 0, 0);
               }
            }
            else if(SERVREG_SERVICE_STATE_DOWN == state)
            {
               /* Should return an existing handle if event already created, else crate a new one */
               sr_mon_node->rcecb_down_handle = rcecb_create_name(sync_name);

               if(RCECB_NULL != sr_mon_node->rcecb_down_handle)
               {
                  sr_mon_node->rcecb_down_handle = rcecb_register_parm2_handle(sr_mon_node->rcecb_down_handle, (RCECB_CALLBACK_FNSIG_P2)callback, (RCECB_PARM)cb_p1, (RCECB_PARM)cb_p2);
                  TMS_MSG_HIGH("Client registered for down sync notification");
                  ret = SERVREG_SUCCESS;
               }
               else
               {
                  ERR_FATAL("In servreg_register_sync_notif_cb() rcecb_down_handle NULL", 0, 0, 0);
               }
            }
            else
            {
               TMS_MSG_ERROR("Invalid state, should be UP/DOWN");
               ret = SERVREG_FAILURE;
            }

            /* Free the space allocated for sync_name as rcecb internally does a strlcpy of the name */
            servreg_free(sync_name);
         }
         else
         {
            TMS_MSG_ERROR("servreg_create_sync_name() ret NULL");
            ret = SERVREG_FAILURE;
         }
      }
      else
      {
         TMS_MSG_ERROR("sr_mon_handle has invalid signature");
         ret = SERVREG_INVALID_HANDLE;
      }
   }
   else
   {
      TMS_MSG_ERROR("sr_mon_node/callback NULL");
      ret = SERVREG_INVALID_HANDLE;
   }
   servreg_mutex_unlock_dal(&(servreg_mon_node_internal.mutex_create));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_deregister_sync_notif_cb
 *
 * Description:
 *     De-Register to get sync notification for that service and de-register the callback function
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by domain + service 
 *                     or just domain name
 *     state         : The service state for which the client wants to dereigster to get sync notification
 *                     Should preferably be SERVREG_SERVICE_STATE_DOWN or SERVREG_SERVICE_STATE_UP
 *     callback      : callback function de-registered
 *     cb_p1         : Callback function parameter. Can be set to sr_mon_handle
 *     cb_p2         : Callback function parameter 2. Can be set to the service state.
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_deregister_sync_notif_cb(SERVREG_MON_HANDLE sr_mon_handle, SERVREG_SERVICE_STATE state, SERVREG_FNSIG_CALLBACK const callback, SERVREG_CB_PARAM cb_p1, SERVREG_CB_PARAM cb_p2)
{
   servreg_mon_node_p sr_mon_node = SERVREG_NULL;
   SERVREG_RESULT ret = SERVREG_FAILURE;

   sr_mon_node = sr_mon_handle2sr_mon_node(sr_mon_handle);

   servreg_mutex_lock_dal(&(servreg_mon_node_internal.mutex_create));

   if(SERVREG_NULL != sr_mon_node && SERVREG_NULL != callback)
   {
      if(SERVREG_MON_SIGNATURE == sr_mon_node->mon_signature)
      {
         if(SERVREG_SERVICE_STATE_UP == state && RCECB_NULL != sr_mon_node->rcecb_up_handle)
         {
            sr_mon_node->rcecb_up_handle = rcecb_unregister_parm2_handle(sr_mon_node->rcecb_up_handle, (RCECB_CALLBACK_FNSIG_P2)callback, (RCECB_PARM)cb_p1, (RCECB_PARM)cb_p2);
            TMS_MSG_HIGH("Client un-registered for up sync notification");
            ret = SERVREG_SUCCESS;
         }
         else if(SERVREG_SERVICE_STATE_DOWN == state && RCECB_NULL != sr_mon_node->rcecb_down_handle)
         {
            sr_mon_node->rcecb_down_handle = rcecb_unregister_parm2_handle(sr_mon_node->rcecb_down_handle, (RCECB_CALLBACK_FNSIG_P2)callback, (RCECB_PARM)cb_p1, (RCECB_PARM)cb_p2);
            TMS_MSG_HIGH("Client un-registered for down sync notification");
            ret = SERVREG_SUCCESS;
         }
         else
         {
            TMS_MSG_ERROR("Invalid state/client not registerd");
            ret = SERVREG_FAILURE;
         }
      }
      else
      {
         TMS_MSG_ERROR("sr_mon_handle has invalid signature");
         ret = SERVREG_INVALID_HANDLE;
      }
   }
   else
   {
      TMS_MSG_ERROR("sr_mon_node/callback NULL");
      ret = SERVREG_FAILURE;
   }
   servreg_mutex_unlock_dal(&(servreg_mon_node_internal.mutex_create));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_get_pd_name
 *
 * Description:
 *     This function returns the pd name i.e format "soc/domain/subdomain".
 *     Memory is allocated for the string returned.
 *
 * Parameters:
 *     name : "soc/domain/subdomain/provider/service" or just "soc/domain/subdomain" info
 *
 * Returns:
 *     	pd name of the format "soc/domain/subdomain"
 * =====================================================================  */
SERVREG_NAME servreg_get_pd_name(SERVREG_MON_HANDLE sr_mon_handle)
{
   SERVREG_NAME pd_name = SERVREG_NULL;
   uint32_t i, count = 0, len = 0;
   servreg_mon_node_p sr_mon_node = SERVREG_NULL;

   sr_mon_node = sr_mon_handle2sr_mon_node(sr_mon_handle);
   if(SERVREG_NULL != sr_mon_node && SERVREG_NULL != sr_mon_node->service_name)
   {
      for(i = 0; i < strlen(sr_mon_node->service_name); i++)
      {
         if(sr_mon_node->service_name[i] == '/')
         {
            count++;
         }

         len++;

         if(count == 3)
         {
            break;
         }
      }
      /* if sr_mon_node->service_name is pd_name itself , len is already +1 if third "/" is reached 
       * but not if we hit NULL*/
      if(i == len)
      {
         len++;
      }
      pd_name = (SERVREG_NAME)servreg_calloc(sizeof(char), len);
      if(SERVREG_NULL != pd_name)
      {
         strlcpy(pd_name, sr_mon_node->service_name, len);
      }
   }

   return pd_name;
}

/** =====================================================================
 * Function:
 *     servreg_get_pd_name_from_domain
 *
 * Description:
 *     This function returns the pd name i.e format "soc/domain/subdomain".
 *     Memory is allocated for the string returned.
 *
 * Parameters:
 *     name : "soc/domain/subdomain/provider/service" or just "soc/domain/subdomain" info
 *
 * Returns:
 *     	pd name of the format "soc/domain/subdomain"
 * =====================================================================  */
SERVREG_NAME servreg_get_pd_name_from_domain(SERVREG_NAME domain_name)
{
   SERVREG_NAME pd_name = SERVREG_NULL;
   uint32_t i, count = 0, len = 0;
  
   if( SERVREG_NULL != domain_name )
   {
      for(i = 0; i < strlen(domain_name); i++)
      {
         if(domain_name[i] == '/')
         {
            count++;
         }

         len++;

         if(count == 3)
         {
            break;
         }
      }
      /* if sr_mon_node->service_name is pd_name itself , len is already +1 if third "/" is reached 
       * but not if we hit NULL*/
      if(i == len)
      {
         len++;
      }
      pd_name = (SERVREG_NAME)servreg_calloc(sizeof(char), len);
      if(SERVREG_NULL != pd_name)
      {
         strlcpy(pd_name, domain_name, len);
      }
   }

   return pd_name;
}

/** =====================================================================
 * Function:
 *     servreg_monitor_init
 *
 * Description:
 *     Initialization function for Service Registry Monitor feature.
 *     Service Registry will register itself as a service and sets its state
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
void servreg_monitor_init(void)
{
   SERVREG_NAME domain = SERVREG_NULL;
   SERVREG_MON_HANDLE sr_mon_handle = SERVREG_NULL, sr_service_mon_handle = SERVREG_NULL;

   /* Static memory pool allocation init */
   servreg_mon_node_internal_init();

   domain = servreg_get_local_domain();

   if(SERVREG_NULL != domain)
   {
      /* Register the state of the PD as UP with Service Registry */
      sr_mon_handle = servreg_alloc_monitor_handle(domain, SERVREG_NULL);

      if(SERVREG_NULL != sr_mon_handle)
      {
         /* Always set the pd service itself as synchronous */
         if(SERVREG_SUCCESS != servreg_set_synchronous(sr_mon_handle))
         {
            ERR_FATAL("In servreg_monitor_init() servreg_set_synchronous() failed", 0, 0, 0);
         }

         if(SERVREG_SUCCESS != servreg_set_state(sr_mon_handle, SERVREG_SERVICE_STATE_UP))
         {
            ERR_FATAL("In servreg_monitor_init() servreg_set_state() failed", 0, 0, 0);
         }
      }
      else
      {
         ERR_FATAL("In servreg_monitor_init() sr_mon_handle NULL", 0, 0, 0);
      }

      /* Register the servreg service itself as UP */
      sr_service_mon_handle = servreg_alloc_monitor_handle(domain, SERVREG_SERVICE_NAME);

      if(SERVREG_NULL != sr_service_mon_handle)
      {
         if(SERVREG_SUCCESS != servreg_set_state(sr_service_mon_handle, SERVREG_SERVICE_STATE_UP))
         {
            ERR_FATAL("In servreg_monitor_init() servreg_set_state() failed", 0, 0, 0);
         }
      }
      else
      {
         ERR_FATAL("In servreg_monitor_init() sr_service_mon_handle NULL", 0, 0, 0);
      }
   }

   /* Set timer to check for the ACK state as deferrable timer to avoid unnecessary wakeup of Q6 */
   timer_group_set_deferrable(&servreg_mon_internal.servreg_group, TRUE);

   if(TE_SUCCESS != timer_def_osal(&servreg_mon_internal.servreg_mon_ack_timer,
                                  &servreg_mon_internal.servreg_group,
                                  TIMER_FUNC1_CB_TYPE,
                                  servreg_generic_task_cb, NULL))
   {
       ERR_FATAL("In servreg_monitor_init() Timer def failed", 0, 0, 0);
   }

   if(TE_SUCCESS != timer_set_64(&servreg_mon_internal.servreg_mon_ack_timer,
                                SERVREG_MON_ACK_TIME_CHK_SEC,
                                SERVREG_MON_ACK_TIME_CHK_SEC,
                                T_SEC))
   {
       ERR_FATAL("In servreg_monitor_init() Timer set failed", 0, 0, 0);
   }

   return;
}

