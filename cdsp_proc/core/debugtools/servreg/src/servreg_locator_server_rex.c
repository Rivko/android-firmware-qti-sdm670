/*
#============================================================================
#  Name:
#    servreg_locator_server_rex.c
#
#  Description:
#    Service Registry locator server file for root image.
#
# Copyright (c) 2015-2017 by Qualcomm Technologies Incorporated. All Rights Reserved.
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

#include "service_registry_locator_v01.h"
#include "servreg_locator_internal.h"
#include "servreg_internal.h"
#include "servreg_locator.h"
#include "servreg_localdb.h"

#define SERVREG_QMI_LOC_SERVER_WAIT_MASK                0x20

/* QMI message related and keeping it global because it should be valid for the lifetime of the user_handle */
struct servreg_locator_client_type_s
{
   qmi_client_handle  client_handle;
   boolean            connected;
   struct servreg_locator_client_type_s* next;
};
typedef struct servreg_locator_client_type_s servreg_locator_client_type_t, * servreg_locator_client_type_p;

servreg_locator_client_type_p qmi_locator_client_head = SERVREG_NULL;
static qmi_csi_service_handle servreg_locator_qmi_server_handler = NULL;

/* QMI client list node structure */
struct servreg_qmi_locator_server_node_s
{
   uint32_t locator_signature;
   /* Client handle that is registering a listener */
   qmi_client_handle client_handle;

   struct servreg_qmi_locator_server_node_s * next;
};
typedef struct servreg_qmi_locator_server_node_s servreg_qmi_locator_server_node_t, * servreg_qmi_locator_server_node_p;

/* Type casts as accessor functions */
#define sr_qmi_locator_server_node2sr_qmi_locator_server_handle(x)        ((SERVREG_QMI_LOC_SERVER_HANDLE)x)
#define sr_qmi_locator_server_handle2sr_qmi_locator_server_node(x)        ((servreg_qmi_locator_server_node_p)x)

/* Pool Allocations */
/* Pool allocation for QMI client list node */
struct servreg_qmi_locator_server_node_pool_s
{
   struct servreg_qmi_locator_server_node_s servreg_qmi_locator_server_node_pool[SERVREG_QMI_LOC_SERVER_NODE_POOL_SIZE];
   struct servreg_qmi_locator_server_node_pool_s * next;
};
typedef struct servreg_qmi_locator_server_node_pool_s servreg_qmi_locator_server_node_pool_t, * servreg_qmi_locator_server_node_pool_p;

/* Internal structure */
struct servreg_qmi_locator_server_node_internal_s
{
   servreg_qmi_locator_server_node_pool_p servreg_qmi_locator_server_node_pool_head_p;
   servreg_qmi_locator_server_node_p servreg_qmi_locator_server_node_pool_free_p;
   servreg_mutex_t mutex;
   servreg_mutex_t mutex_create;
   SERVREG_BOOL dynamic_use;
   unsigned long init_flag;
};

struct servreg_qmi_locator_server_node_internal_s servreg_qmi_locator_server_node_internal;
servreg_qmi_locator_server_node_pool_t servreg_qmi_locator_server_node_pool_static;

/* Head node of the servreg notification list */
servreg_qmi_locator_server_node_p servreg_qmi_locator_server_list_head = SERVREG_NULL;

/* Pool allocation internals for domain list table generation */
servreg_dl_internal_t servreg_dl_internal_server;
servreg_dl_entry_pool_t servreg_dl_entry_pool_static_server;
servreg_dl_table_pool_t servreg_dl_table_pool_static_server;

/* Head node of the domain list db table */
servreg_dl_table_p servreg_dl_table_hash[SERVREG_DL_DB_HASHTABLE_BUCKETS];

/* Hash Functions */
static servreg_dl_table_p servreg_dl_table_hashtab_get(servreg_hash_t hash);
static void servreg_dl_table_hashtab_put(servreg_dl_table_p sr_dl_table, servreg_hash_t hash);
//static void servreg_dl_table_hashtab_delete(servreg_dl_table_p sr_dl_entry_del, servreg_hash_t hash);

static SERVREG_DL_HANDLE servreg_get_sr_dl_handle(SERVREG_NAME const name);
static SERVREG_DL_HANDLE servreg_create_dl_table(SERVREG_NAME service_name);

/* Static functions in this file */
static servreg_qmi_locator_server_node_p servreg_qmi_locator_server_node_pool_init(void);
static servreg_qmi_locator_server_node_p servreg_qmi_locator_server_node_pool_alloc(void);
static servreg_qmi_locator_server_node_p servreg_qmi_locator_server_node_pool_free(servreg_qmi_locator_server_node_p sr_qmi_locator_server_node);

static SERVREG_QMI_LOC_SERVER_HANDLE servreg_get_qmi_locator_server_node(qmi_client_handle client_handle);
static SERVREG_QMI_LOC_SERVER_HANDLE servreg_create_qmi_locator_server_node(qmi_client_handle client_handle);
static SERVREG_RESULT servreg_delete_qmi_locator_server_node(SERVREG_QMI_LOC_SERVER_HANDLE sr_qmi_locator_server_handle);

/** =====================================================================
 * Function:
 *     servreg_dl_table_hashtab_get
 *
 * Description:
 *     Given the hash value, get the corresponding sr monitor node
 *
 * Parameters:
 *     hash : hash value which is based on the service name
 *
 * Returns:
 *     servreg_dl_table_p : sr monitor node that corresponds to the hash value
 * =====================================================================  */
static servreg_dl_table_p servreg_dl_table_hashtab_get(servreg_hash_t hash)
{
   servreg_dl_table_p sr_dl_table = SERVREG_NULL;

   servreg_mutex_lock_dal(&(servreg_dl_internal_server.mutex));

   sr_dl_table = servreg_dl_table_hash[hash];

   servreg_mutex_unlock_dal(&(servreg_dl_internal_server.mutex));

   return sr_dl_table;
}

/** =====================================================================
 * Function:
 *     servreg_dl_table_hashtab_put
 *
 * Description:
 *     Put the sr monitor node to the hash table with the given hash value
 *
 * Parameters:
 *     sr_dl_table : sr monitor node to be put into the hash table
 *     hash        : hash value associated with the sr montior node
 *
 * Returns:
 *     None
 * =====================================================================  */
static void servreg_dl_table_hashtab_put(servreg_dl_table_p sr_dl_table, servreg_hash_t hash)
{
   servreg_mutex_lock_dal(&(servreg_dl_internal_server.mutex));

   /*  Insert to head of list for that hash value */
   sr_dl_table->next = servreg_dl_table_hash[hash];

   /* Update head for that hash value */
   servreg_dl_table_hash[hash] = sr_dl_table;

   servreg_mutex_unlock_dal(&(servreg_dl_internal_server.mutex));
}

/** =====================================================================
 * Function:
 *     servreg_dl_table_hashtab_delete
 *
 * Description:
 *     Delete the sr monitor node from the hash table with the given hash value
 *
 * Parameters:
 *     sr_dl_table : sr monitor node to be put into the hash table
 *     hash        : hash value associated with the sr montior node
 *
 * Returns:
 *     None
 * =====================================================================  */
/*static void servreg_dl_table_hashtab_delete(servreg_dl_table_p sr_dl_entry_del, servreg_hash_t hash)
{
   servreg_dl_table_p sr_dl_entry_curr = SERVREG_NULL, sr_dl_entry_prev = SERVREG_NULL;

   servreg_mutex_lock_dal(&(servreg_dl_internal_server.mutex));

   sr_dl_entry_curr = servreg_dl_table_hash[hash];

   while(SERVREG_NULL != sr_dl_entry_curr)
   {
      if(sr_dl_entry_curr == sr_dl_entry_del)
      {
         if(SERVREG_NULL == sr_dl_entry_prev)
         {
            servreg_dl_table_hash[hash] = sr_dl_entry_curr->next;
         }
         else
         {
            sr_dl_entry_prev->next = sr_dl_entry_curr->next;
         }

         sr_dl_entry_curr->next = SERVREG_NULL;

         break;
      }

      sr_dl_entry_prev = sr_dl_entry_curr;
      sr_dl_entry_curr = sr_dl_entry_curr->next;
   }

   servreg_mutex_unlock_dal(&(servreg_dl_internal_server.mutex));

   return;
}
*/

/** =====================================================================
 * Function:
 *     servreg_get_sr_dl_handle
 *
 * Description:
 *     Checks if a DL node already exists with the given name.
 *
 * Parameters:
 *     name : "soc/domain/subdomain/provider/service" or just "soc/domain/subdomain" name
 *
 * Returns:
 *    SERVREG_DL_HANDLE : handle to the dl node
 *
 * Note :
 *    No need of locks as this function will be called internally only by servreg_create_dl_table()
 * =====================================================================  */
static SERVREG_DL_HANDLE servreg_get_sr_dl_handle(SERVREG_NAME const name)
{
   servreg_dl_table_p sr_dl_table = SERVREG_NULL;
   SERVREG_DL_HANDLE sr_dl_handle = SERVREG_NULL;
   uint32_t len1 = 0, len2 = 0;

   sr_dl_table = servreg_dl_table_hashtab_get(servreg_nmehash(name, (sizeof(servreg_dl_table_hash) / sizeof(servreg_dl_table_p))));

   while(SERVREG_NULL != sr_dl_table)
   {
      /* If they dont have the same address */
      if(sr_dl_table->service_name != name)
      {
         len1 = servreg_nmelen(sr_dl_table->service_name);
         len2 = servreg_nmelen(name);
         if(len1 == len2 && 0 == servreg_nmecmp(sr_dl_table->service_name, name, len1))
         {
            sr_dl_handle = sr_dl_table2sr_dl_handle(sr_dl_table);
            break;
         }
      }
      else
      {
         sr_dl_handle = sr_dl_table2sr_dl_handle(sr_dl_table);
         break;
      }
      sr_dl_table = sr_dl_table->next;
   }

   return sr_dl_handle;
}

/** =====================================================================
 * Function:
 *     servreg_create_dl_table
 *
 * Description:
 *     Creates a dl node with the given service_name and rcesn handle
 *
 * Parameters:
 *    service_name : service name
 *    domain_name : domain name
 *
 * Returns:
 *    SERVREG_DL_HANDLE : handle to the dl node
 *
 * Note :
 *    No need of locks as this function will be called internally only by servreg_alloc_monitor_handle()
 * =====================================================================  */
static SERVREG_DL_HANDLE servreg_create_dl_table(SERVREG_NAME service_name)
{
   servreg_dl_table_p sr_dl_table = SERVREG_NULL;
   SERVREG_DL_HANDLE sr_dl_handle = SERVREG_NULL;
   uint32_t len1 = 0;

   /* Check if the DL node exists */
   sr_dl_handle = servreg_get_sr_dl_handle(service_name);

   if(SERVREG_NULL == sr_dl_handle)
   {
      /* Allocate a new dl node from the node pool */
      sr_dl_table = servreg_dl_table_pool_alloc(&servreg_dl_internal_server, &servreg_dl_table_pool_static_server);

      if(SERVREG_NULL != sr_dl_table)
      {
         /* Allocate memory for service_name */
         len1 = servreg_nmelen(service_name) + 1;
         sr_dl_table->service_name = (SERVREG_NAME)servreg_calloc(sizeof(char), len1);

         if (SERVREG_NULL != sr_dl_table->service_name)
         {
            strlcpy(sr_dl_table->service_name, service_name, len1);
         }
         else
         {
            servreg_dl_table_pool_free(sr_dl_table, &servreg_dl_internal_server);
            ERR_FATAL( "buf_p calloc failed", 0, 0, 0);
         }

         sr_dl_table->domain_list_status = SERVREG_SUCCESS;
         sr_dl_table->total_domains = 0;
         sr_dl_table->domain_list_head = SERVREG_NULL;
         sr_dl_table->next = SERVREG_NULL;

         servreg_dl_table_hashtab_put(sr_dl_table, servreg_nmehash(sr_dl_table->service_name, sizeof(servreg_dl_table_hash) / sizeof(servreg_dl_table_p)));

         sr_dl_handle = sr_dl_handle2sr_dl_table(sr_dl_table);
      }
      else
      {
         TMS_MSG_ERROR("sr_dl_table NULL");
      }
   }
   else
   {
      sr_dl_table = sr_dl_handle2sr_dl_table(sr_dl_handle);
   }

   return sr_dl_handle;
}

/** =====================================================================
 * Function:
 *     servreg_qmi_locator_server_node_pool_init
 *
 * Description:
 *     Initializes the memory pool for domain list node structure
 *
 * Parameters:
 *     None
 *
 * Returns:
 *    servreg_qmi_locator_server_node_p : Returns the first free dl_node space from the pool
 * =====================================================================  */
static servreg_qmi_locator_server_node_p servreg_qmi_locator_server_node_pool_init(void)
{
   servreg_qmi_locator_server_node_pool_p next_pool = SERVREG_NULL;

   if (SERVREG_NULL == servreg_qmi_locator_server_node_internal.servreg_qmi_locator_server_node_pool_head_p)
   {
      next_pool = &servreg_qmi_locator_server_node_pool_static;
   }
   else if (SERVREG_TRUE == servreg_qmi_locator_server_node_internal.dynamic_use)
   {
      next_pool = (servreg_qmi_locator_server_node_pool_p)servreg_calloc(sizeof(servreg_qmi_locator_server_node_pool_t), 1);
   }

   if (SERVREG_NULL != next_pool)
   {
      int i;

      for (i = 0; i < SERVREG_QMI_LOC_SERVER_NODE_POOL_SIZE; i++)
      {
         if (i != (SERVREG_QMI_LOC_SERVER_NODE_POOL_SIZE - 1))
         {
            next_pool->servreg_qmi_locator_server_node_pool[i].next = &(next_pool->servreg_qmi_locator_server_node_pool[i + 1]);
         }
         else
         {
            next_pool->servreg_qmi_locator_server_node_pool[i].next = servreg_qmi_locator_server_node_internal.servreg_qmi_locator_server_node_pool_free_p;
         }

         next_pool->servreg_qmi_locator_server_node_pool[i].locator_signature = SERVREG_QMI_NOTIF_SERVER_SIGNATURE;
         next_pool->servreg_qmi_locator_server_node_pool[i].client_handle = NULL;
      }

      servreg_qmi_locator_server_node_internal.servreg_qmi_locator_server_node_pool_free_p = &(next_pool->servreg_qmi_locator_server_node_pool[0]);
      next_pool->next = servreg_qmi_locator_server_node_internal.servreg_qmi_locator_server_node_pool_head_p;
      servreg_qmi_locator_server_node_internal.servreg_qmi_locator_server_node_pool_head_p = next_pool;
   }
   else
   {
      TMS_MSG_ERROR("Calloc failed");
      return SERVREG_NULL;
   }

   return servreg_qmi_locator_server_node_internal.servreg_qmi_locator_server_node_pool_free_p;
}

/** =====================================================================
 * Function:
 *     servreg_qmi_locator_server_node_pool_alloc
 *
 * Description:
 *     Gives the first available free and allocated space from the memory
 *
 * Parameters:
 *     None
 *
 * Returns:
 *    servreg_qmi_locator_server_node_p : the first available free and allocated space from the memory
 * =====================================================================  */
static servreg_qmi_locator_server_node_p servreg_qmi_locator_server_node_pool_alloc(void)
{
   servreg_qmi_locator_server_node_p ret;
   servreg_qmi_locator_server_node_p sr_qmi_locator_server_node;

   servreg_mutex_lock_dal(&(servreg_qmi_locator_server_node_internal.mutex));

   if (SERVREG_NULL == servreg_qmi_locator_server_node_internal.servreg_qmi_locator_server_node_pool_free_p)
   {
      sr_qmi_locator_server_node = servreg_qmi_locator_server_node_pool_init();
   }
   else
   {
      sr_qmi_locator_server_node = servreg_qmi_locator_server_node_internal.servreg_qmi_locator_server_node_pool_free_p;
   }

   if (SERVREG_NULL != sr_qmi_locator_server_node)
   {
      servreg_qmi_locator_server_node_internal.servreg_qmi_locator_server_node_pool_free_p = sr_qmi_locator_server_node->next;
      sr_qmi_locator_server_node->next = SERVREG_NULL;
      ret = sr_qmi_locator_server_node;
   }
   else
   {
      TMS_MSG_ERROR("Alloc failed");
      ret = SERVREG_NULL;
   }

   servreg_mutex_unlock_dal(&(servreg_qmi_locator_server_node_internal.mutex));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_qmi_locator_server_node_pool_free
 *
 * Description:
 *     Reclaims back the sr_qmi_locator_server_node to the memory pool
 *
 * Parameters:
 *     sr_qmi_locator_server_node : space to be reclaimed back
 *
 * Returns:
 *    servreg_qmi_locator_server_node_p : The next available free space in the memory pool
 * =====================================================================  */
static servreg_qmi_locator_server_node_p servreg_qmi_locator_server_node_pool_free(servreg_qmi_locator_server_node_p sr_qmi_locator_server_node)
{
   servreg_qmi_locator_server_node_p ret = SERVREG_NULL;

   servreg_mutex_lock_dal(&(servreg_qmi_locator_server_node_internal.mutex));

   if(SERVREG_NULL != sr_qmi_locator_server_node)
   {
      sr_qmi_locator_server_node->client_handle = NULL;

      sr_qmi_locator_server_node->next = servreg_qmi_locator_server_node_internal.servreg_qmi_locator_server_node_pool_free_p;
      servreg_qmi_locator_server_node_internal.servreg_qmi_locator_server_node_pool_free_p = sr_qmi_locator_server_node;
      ret = sr_qmi_locator_server_node;
   }
   else
   {
      TMS_MSG_ERROR("sr_qmi_locator_server_node is NULL and cannot be freed");
   }

   servreg_mutex_unlock_dal(&(servreg_qmi_locator_server_node_internal.mutex));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_qmi_locator_server_node_internal_init
 *
 * Description:
 *     Initialization of the internal memory pools and other internals
 *     for sr locator nodes
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
void servreg_qmi_locator_server_node_internal_init(void)
{
   servreg_mutex_init_dal(&(servreg_qmi_locator_server_node_internal.mutex));
   servreg_mutex_init_dal(&(servreg_qmi_locator_server_node_internal.mutex_create));

   servreg_mutex_lock_dal(&(servreg_qmi_locator_server_node_internal.mutex));

   servreg_qmi_locator_server_node_internal.dynamic_use = SERVREG_TRUE;
   servreg_qmi_locator_server_node_pool_init();

   servreg_mutex_unlock_dal(&(servreg_qmi_locator_server_node_internal.mutex));
}

/** =====================================================================
 * Function:
 *     servreg_get_qmi_locator_server_node
 *
 * Description:
 *     Checks if a node already exists with the given name. If it does 
 *     exists it returns a pointer to that node.
 *
 * Parameters:
 *     client_handle : QMI client handle
 *
 * Returns:
 *    SERVREG_QMI_LOC_SERVER_HANDLE : handle to the sr locator node
 * =====================================================================  */
static SERVREG_QMI_LOC_SERVER_HANDLE servreg_get_qmi_locator_server_node(qmi_client_handle client_handle)
{
   servreg_qmi_locator_server_node_p sr_qmi_locator_server_node = SERVREG_NULL;
   SERVREG_QMI_LOC_SERVER_HANDLE sr_qmi_locator_server_handle = SERVREG_NULL;

   sr_qmi_locator_server_node = servreg_qmi_locator_server_list_head;

   servreg_mutex_lock_dal(&(servreg_qmi_locator_server_node_internal.mutex_create));

   while(SERVREG_NULL != sr_qmi_locator_server_node)
   {
      /* Check if the same handle is already registered */
      if(client_handle == sr_qmi_locator_server_node->client_handle)
      {
         sr_qmi_locator_server_handle = sr_qmi_locator_server_node2sr_qmi_locator_server_handle(sr_qmi_locator_server_node);
         break;
      }
      sr_qmi_locator_server_node = sr_qmi_locator_server_node->next;
   }

   servreg_mutex_unlock_dal(&(servreg_qmi_locator_server_node_internal.mutex_create));

   return sr_qmi_locator_server_handle;
}

/** =====================================================================
 * Function:
 *     servreg_create_qmi_locator_server_node
 *
 * Description:
 *     Creates a sr_loctor node with the given qmi client_handle
 *
 * Parameters:
 *    client_handle : QMI client handle
 *
 * Returns:
 *    SERVREG_QMI_LOC_SERVER_HANDLE : handle to the sr locator node
 * =====================================================================  */
static SERVREG_QMI_LOC_SERVER_HANDLE servreg_create_qmi_locator_server_node(qmi_client_handle client_handle)
{
   SERVREG_QMI_LOC_SERVER_HANDLE sr_qmi_locator_server_handle = SERVREG_NULL;
   servreg_qmi_locator_server_node_p sr_qmi_locator_server_node = SERVREG_NULL;

   /* Check if the sr_loctor node with that event name exists */
   sr_qmi_locator_server_handle = servreg_get_qmi_locator_server_node(client_handle);

   servreg_mutex_lock_dal(&(servreg_qmi_locator_server_node_internal.mutex_create));

   if(SERVREG_NULL == sr_qmi_locator_server_handle)
   {
      /* qmi node */
      sr_qmi_locator_server_node = servreg_qmi_locator_server_node_pool_alloc();

      if(SERVREG_NULL != sr_qmi_locator_server_node)
      {
         sr_qmi_locator_server_node->client_handle = client_handle;

         /* Insert the domain_node in the head of the list */
         sr_qmi_locator_server_node->next = servreg_qmi_locator_server_list_head;

         /* Update the head */
         servreg_qmi_locator_server_list_head = sr_qmi_locator_server_node;

         sr_qmi_locator_server_handle = sr_qmi_locator_server_node2sr_qmi_locator_server_handle(sr_qmi_locator_server_node);
      }
      else
      {
         ERR_FATAL( "SERVREG_QMI_LOC: sr_qmi_locator_server_node calloc failed", 0, 0, 0);
      }
   }

   servreg_mutex_unlock_dal(&(servreg_qmi_locator_server_node_internal.mutex_create));

   return sr_qmi_locator_server_handle;
}

/** =====================================================================
 * Function:
 *     servreg_delete_qmi_locator_server_node
 *
 * Description:
 *     Deletes a sr locator node given the sr locator handle
 *
 * Parameters:
 *     sr_qmi_locator_server_handle  : Handle to the locator node to be deleted
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
static SERVREG_RESULT servreg_delete_qmi_locator_server_node(SERVREG_QMI_LOC_SERVER_HANDLE sr_qmi_locator_server_handle)
{
   servreg_qmi_locator_server_node_p sr_qmi_locator_server_node_curr = SERVREG_NULL, sr_qmi_locator_server_node_del = SERVREG_NULL, sr_qmi_locator_server_node_prev = SERVREG_NULL;
   SERVREG_RESULT ret = SERVREG_FAILURE;

   servreg_mutex_lock_dal(&(servreg_qmi_locator_server_node_internal.mutex_create));

   sr_qmi_locator_server_node_curr = servreg_qmi_locator_server_list_head;
   sr_qmi_locator_server_node_del = sr_qmi_locator_server_handle2sr_qmi_locator_server_node(sr_qmi_locator_server_handle);

   if(SERVREG_NULL != sr_qmi_locator_server_node_del)
   {
      if(SERVREG_QMI_LOC_SERVER_SIGNATURE == sr_qmi_locator_server_node_del->locator_signature)
      {
         while(SERVREG_NULL != sr_qmi_locator_server_node_curr)
         {
            if(sr_qmi_locator_server_node_curr == sr_qmi_locator_server_node_del)
            {
               if(SERVREG_NULL == sr_qmi_locator_server_node_prev)
               {
                  servreg_qmi_locator_server_list_head = sr_qmi_locator_server_node_del->next;
               }
               else
               {
                  sr_qmi_locator_server_node_prev = sr_qmi_locator_server_node_del->next;
               }

               sr_qmi_locator_server_node_del->next = SERVREG_NULL;

               if(SERVREG_NULL != servreg_qmi_locator_server_node_pool_free(sr_qmi_locator_server_node_del))
               {
                  ret = SERVREG_SUCCESS;
               }
               else
               {
                  TMS_MSG_ERROR("pool_free returned NULL");
               }

               break;
            }

            sr_qmi_locator_server_node_prev = sr_qmi_locator_server_node_curr;
            sr_qmi_locator_server_node_curr = sr_qmi_locator_server_node_curr->next;  
            
         } /* while() */
      }
      else
      {
         TMS_MSG_ERROR("sr_qmi_locator_server_handle has invalid signature");
         ret = SERVREG_INVALID_HANDLE;
      }
   }
   else
   {
      TMS_MSG_ERROR("sr_qmi_locator_server_handle NULL");
      ret = SERVREG_INVALID_HANDLE;
   }

   servreg_mutex_unlock_dal(&(servreg_qmi_locator_server_node_internal.mutex_create));

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
   servreg_locator_client_type_p client = SERVREG_NULL;
   SERVREG_BOOL ret = SERVREG_FALSE;

   client = qmi_locator_client_head;

   /* Validate handle. */
   if(connection_handle != NULL)
   {
      while(SERVREG_NULL != client)
      {
         if(client == (servreg_locator_client_type_p)connection_handle)
         {
            ret = SERVREG_TRUE;
            TMS_MSG_ERROR("connection_handle matches");
            break;
         }
         client = client->next;
      }
   }
   else
   {
      TMS_MSG_ERROR("connection_handle NULL");
   }

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_send_qmi_ind_msg_db_updated
 *
 * Description:
 *     Check which locator node state has changed and return that locator node's handle
 *
 * Parameters:
 *     servreg_qmi_locator_server_list_head : Head pointer of the locator handle list for the user PD.
 *
 * Returns:
 *    SERVREG_QMI_LOC_SERVER_HANDLE : handle to the sr locator node
 * =====================================================================  */
void servreg_send_qmi_ind_msg_db_updated(void)
{
   servreg_qmi_locator_server_node_p sr_qmi_locator_server_node = SERVREG_NULL;
   qmi_csi_error err;
   qmi_servreg_loc_database_updated_ind_msg_v01 ind;

   servreg_mutex_lock_dal(&(servreg_qmi_locator_server_node_internal.mutex_create));

   sr_qmi_locator_server_node = servreg_qmi_locator_server_list_head;

   while(SERVREG_NULL != sr_qmi_locator_server_node)
   {
      err = qmi_csi_send_ind((sr_qmi_locator_server_node->client_handle),
                             SERVREG_LOC_DATABASE_UPDATED_IND,
                             &ind,
                             sizeof(qmi_servreg_loc_database_updated_ind_msg_v01));

      TMS_MSG_HIGH_1("QMI send error: %d", err);

      sr_qmi_locator_server_node = sr_qmi_locator_server_node->next;
   }

   servreg_mutex_unlock_dal(&(servreg_qmi_locator_server_node_internal.mutex_create));

   return;
}

/** =====================================================================
 * Function:
 *     servreg_locator_server_process_req
 *
 * Description:
 *     This function processes incoming requests for the QMI servreg_locator
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
qmi_csi_cb_error servreg_locator_server_process_req(void *connection_handle,
                                          qmi_req_handle  req_handle,
                                          unsigned int    msg_id,
                                          void           *req_c_struct,
                                          unsigned int    req_c_struct_len,
                                          void           *service_cookie)
{
   qmi_csi_cb_error retval = QMI_CSI_CB_NO_ERR;
   servreg_locator_client_type_p client = (servreg_locator_client_type_p)connection_handle;
   SERVREG_RESULT ret = SERVREG_FAILURE;
   SERVREG_QMI_LOC_SERVER_HANDLE sr_qmi_locator_server_handle = SERVREG_NULL;

   if(NULL != req_handle)
   {
      if(SERVREG_TRUE == servreg_qmi_validate_connection_handle(connection_handle) && SERVREG_TRUE == client->connected)
      {
         switch(msg_id)
         {
            case SERVREG_LOC_INDICATION_REGISTER_REQ:
            {
               qmi_servreg_loc_indication_register_req_msg_v01 *request = (qmi_servreg_loc_indication_register_req_msg_v01 *)(req_c_struct);
               qmi_servreg_loc_indication_register_resp_msg_v01 resp_send;

               /* LOC Indication Registration */
               if(SERVREG_TRUE == request->enable_database_updated_indication)
               {
                  /* Create a qmi locator server node that keep tracks of all the remote qmi listener clients */
                  sr_qmi_locator_server_handle = servreg_create_qmi_locator_server_node(client->client_handle);

                  if(SERVREG_NULL != sr_qmi_locator_server_handle)
                  {
                     TMS_MSG_HIGH("LOC_INDICATION_REGISTER_REQ success");

                     resp_send.resp.result = QMI_RESULT_SUCCESS_V01;
                     resp_send.resp.error  = QMI_ERR_NONE_V01;
                  }
                  else
                  {
                     ERR_FATAL( "servreg_create_qmi_locator_server_node() failed", 0, 0, 0);
                  }
               }
               else /* LOC Deregister Listener */
               {
                  /* Check if the remote qmi client has registered as a LOC listener */
                  sr_qmi_locator_server_handle = servreg_get_qmi_locator_server_node(client->client_handle);

                  if(SERVREG_NULL != sr_qmi_locator_server_handle)
                  {
                     /* Delete the client handle from the qmi locator server node */
                     ret = servreg_delete_qmi_locator_server_node(sr_qmi_locator_server_handle);

                     if(SERVREG_SUCCESS == ret)
                     {
                        TMS_MSG_HIGH("LOC_INDICATION_DEREGISTER_REQ success");
                        resp_send.resp.result = QMI_RESULT_SUCCESS_V01;
                        resp_send.resp.error  = QMI_ERR_NONE_V01;
                     }
                     else
                     {
                        ERR_FATAL( "servreg_delete_qmi_locator_server_node() failed", 0, 0, 0);
                     }
                  }
                  else
                  {
                     TMS_MSG_ERROR("DE-REGISTER_LISTENER_REQ fail");

                     resp_send.resp.result = QMI_RESULT_FAILURE_V01;;
                     resp_send.resp.error  = QMI_ERR_INVALID_HANDLE_V01;
                  }
               }

               qmi_csi_send_resp(req_handle, msg_id, &resp_send, sizeof(qmi_servreg_loc_indication_register_resp_msg_v01));

            }
            break;

            case SERVREG_LOC_GET_DOMAIN_LIST_REQ:
            {
               qmi_servreg_loc_get_domain_list_req_msg_v01 *request = (qmi_servreg_loc_get_domain_list_req_msg_v01 *)(req_c_struct);
               qmi_servreg_loc_get_domain_list_resp_msg_v01 *resp_send;
               servreg_dl_table_p sr_dl_table = SERVREG_NULL;
               servreg_dl_entry_p sr_dl_entry = SERVREG_NULL;
               uint32_t i = 0, domains_send = 0;

               sr_dl_table = servreg_dl_table_hashtab_get(servreg_nmehash(request->service_name, (sizeof(servreg_dl_table_hash) / sizeof(servreg_dl_table_p))));

               /* Do heap allocation of the req message because the req packet is huge */
               resp_send = (qmi_servreg_loc_get_domain_list_resp_msg_v01*)servreg_calloc(sizeof(qmi_servreg_loc_get_domain_list_resp_msg_v01), 1);

               if(SERVREG_NULL != sr_dl_table && SERVREG_NULL != resp_send)
               {
                  resp_send->total_domains_valid = 1;
                  resp_send->total_domains = sr_dl_table->total_domains;

                  resp_send->db_rev_count_valid = 1;
                  resp_send->db_rev_count = sr_dl_table->db_rev_count;

                  resp_send->domain_list_valid = 1;

                  sr_dl_entry = sr_dl_table->domain_list_head;

                  if(request->domain_offset_valid == 1 && request->domain_offset > 1)
                  {
                     for(i = 0; i < (request->domain_offset - 1); i++)
                     {
                        sr_dl_entry = sr_dl_entry->next;
                     }

                     domains_send = sr_dl_table->total_domains - request->domain_offset + 1;
                  }
                  else
                  {
                     domains_send = sr_dl_table->total_domains;
                  }

                  if(domains_send > QMI_SERVREG_LOC_LIST_LENGTH_V01)
                  {
                     domains_send = QMI_SERVREG_LOC_LIST_LENGTH_V01;
                  }

                  resp_send->domain_list_len = domains_send;

                  for(i = 0; i < domains_send; i++)
                  {
                     if(SERVREG_NULL != sr_dl_entry)
                     {
                        strlcpy(resp_send->domain_list[i].name, sr_dl_entry->domain_name, servreg_nmelen(sr_dl_entry->domain_name) + 1);
                        resp_send->domain_list[i].instance_id = sr_dl_entry->instance_id;
                        
                        if(sr_dl_entry->service_data_valid == 1)
                        {
                           resp_send->domain_list[i].service_data_valid = sr_dl_entry->service_data_valid;
                           resp_send->domain_list[i].service_data = sr_dl_entry->service_data;
                        }

                        sr_dl_entry = sr_dl_entry->next;
                     }
                     else
                     {
                        TMS_MSG_ERROR("SERVREG_LOC_GET_DOMAIN_LIST_REQ total_domains mismatch with sr_dl_entry");
                        resp_send->resp.result = QMI_RESULT_FAILURE_V01;
                        resp_send->resp.error  = QMI_ERR_INVALID_HANDLE_V01;
                        break;
                     }
                  }

                  if(i == sr_dl_table->total_domains)
                  {
                     resp_send->resp.result = QMI_RESULT_SUCCESS_V01;
                     resp_send->resp.error  = QMI_ERR_NONE_V01;
                  }
               }
               else
               {
                  TMS_MSG_ERROR("SERVREG_LOC_GET_DOMAIN_LIST_REQ or Calloc failed");

                  resp_send->resp.result = QMI_RESULT_FAILURE_V01;;
                  resp_send->resp.error  = QMI_ERR_INVALID_HANDLE_V01;
               }

               qmi_csi_send_resp(req_handle, msg_id, resp_send, sizeof(qmi_servreg_loc_get_domain_list_resp_msg_v01));
            }
            break;

            case SERVREG_LOC_REGISTER_SERVICE_LIST_REQ:
            {
               qmi_servreg_loc_register_service_list_req_msg_v01 *request = (qmi_servreg_loc_register_service_list_req_msg_v01 *)(req_c_struct);
               qmi_servreg_loc_register_service_list_resp_msg_v01 resp_send;
               SERVREG_DL_HANDLE dlhandle = SERVREG_NULL;
               servreg_dl_table_p sr_dl_table = SERVREG_NULL;
               servreg_dl_entry_p srdl_entry = SERVREG_NULL;
               uint32_t len1 = 0, i = 0;

               for(i = 0; i < request->service_list_len; i++)
               {
                  dlhandle = servreg_create_dl_table(request->service_list[i].name);
                  sr_dl_table = sr_dl_handle2sr_dl_table(dlhandle);

                  srdl_entry = servreg_dl_entry_pool_alloc(&servreg_dl_internal_server, &servreg_dl_entry_pool_static_server);

                  if(SERVREG_NULL != sr_dl_table && SERVREG_NULL != srdl_entry)
                  {
                     len1 = servreg_nmelen((SERVREG_NAME)&request->domain_name) + 1;
                     srdl_entry->domain_name = (SERVREG_NAME)servreg_calloc(sizeof(char), len1);

                     if (SERVREG_NULL != srdl_entry->domain_name)
                     {
                        strlcpy(srdl_entry->domain_name, (char *)&request->domain_name, len1);
                     }
                     else
                     {
                        servreg_dl_table_pool_free(sr_dl_table, &servreg_dl_internal_server);
                        ERR_FATAL( "SERVREG_LOC_S: calloc failed", 0, 0, 0);
                     }

                     srdl_entry->instance_id = request->service_list[i].instance_id;
                     srdl_entry->service_data_valid = request->service_list[i].service_data_valid;
                     srdl_entry->service_data = request->service_list[i].service_data;

                     /* Insert the domain_entry in the head of the list */
                     srdl_entry->next = sr_dl_table->domain_list_head;

                      /* Update the head */
                     sr_dl_table->domain_list_head = srdl_entry;
                     sr_dl_table->total_domains = sr_dl_table->total_domains + 1;
                     sr_dl_table->domain_list_status = SERVREG_SUCCESS;
                  }
                  else
                  {
                     ERR_FATAL( "SERVREG_LOC_S: srdl_entry or sr_dl_table calloc failed", 0, 0, 0);
                  }
               } /* for () */

               resp_send.resp.result = QMI_RESULT_SUCCESS_V01;
               resp_send.resp.error  = QMI_ERR_NONE_V01;
               qmi_csi_send_resp(req_handle, msg_id, &resp_send, sizeof(qmi_servreg_loc_register_service_list_resp_msg_v01));
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

   return retval;
}

/** =====================================================================
 * Function:
 *     servreg_locator_server_disconnect
 *
 * Description:
 *       This function processes incoming requests for the servreg_locator 
 *       QMI services to disconnect a client.
 *
 * Parameters:
 *      connection_handle: Client connection to disconnect
 *      service_cookie   : Ignored
 *
 * Returns:
 *     none
 * =====================================================================  */
void servreg_locator_server_disconnect(void *connection_handle, void *service_cookie)
{
   servreg_locator_client_type_p client_curr = SERVREG_NULL, client_prev = SERVREG_NULL;
   servreg_qmi_locator_server_node_p sr_qmi_locator_server_node_curr = SERVREG_NULL, sr_qmi_locator_server_node_prev = SERVREG_NULL;

   client_curr = qmi_locator_client_head;

   if(SERVREG_NULL != connection_handle)
   {
      while(SERVREG_NULL != client_curr)
      {
         if(client_curr == (servreg_locator_client_type_p)connection_handle && (SERVREG_FALSE != client_curr->connected))
         {
            if(SERVREG_NULL == client_prev)
            {
               /* Delete the first node */
               qmi_locator_client_head = client_curr->next;
            }
            else
            {
               client_prev->next = client_curr->next;
            }

            client_curr->next = SERVREG_NULL;


            /* Also delete the reference to the qmi client handle in the qmi server table */
            sr_qmi_locator_server_node_curr = servreg_qmi_locator_server_list_head;

            servreg_mutex_lock_dal(&(servreg_qmi_locator_server_node_internal.mutex_create));

            while(SERVREG_NULL != sr_qmi_locator_server_node_curr)
            {
               /* Walk through the qmi_client list and find the matching qmi_client handle */
               if(client_curr->client_handle == sr_qmi_locator_server_node_curr->client_handle)
               {
                  if(SERVREG_NULL == sr_qmi_locator_server_node_prev)
                  {
                     /* Delete the first node */
                     servreg_qmi_locator_server_list_head = sr_qmi_locator_server_node_curr->next;
                  }
                  else
                  {
                     sr_qmi_locator_server_node_prev->next = sr_qmi_locator_server_node_curr->next;
                  }

                  sr_qmi_locator_server_node_curr->next = SERVREG_NULL;

                  /* Reclaim back the qmi node node */
                  servreg_qmi_locator_server_node_pool_free(sr_qmi_locator_server_node_curr);
                  break;
               }
               else
               {
                  sr_qmi_locator_server_node_prev = sr_qmi_locator_server_node_curr;
                  sr_qmi_locator_server_node_curr = sr_qmi_locator_server_node_curr->next;
               }
            }

            servreg_mutex_unlock_dal(&(servreg_qmi_locator_server_node_internal.mutex_create));

            /* Delete the client node */
            servreg_free(client_curr);
            TMS_MSG_HIGH("Client released connection");
            break;
         }

         client_prev = client_curr;
         client_curr = client_curr->next;
      }
      //TMS_MSG_ERROR("Disconnect invalid client");
   }
   else
   {
      TMS_MSG_ERROR("connection handle NULL");
   }
   return;
}

/** =====================================================================
 * Function:
 *     servreg_locator_server_connect
 *
 * Description:
 *       This function processes incoming requests for the servreg_locator
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
qmi_csi_cb_error servreg_locator_server_connect(qmi_client_handle client_handle, void *service_cookie, void **connection_handle)
{
   qmi_csi_cb_error retval = QMI_CSI_CB_NO_MEM;
   servreg_locator_client_type_p client = SERVREG_NULL; //(servreg_locator_client_type*)(&qmi_locator_client_head);

   client = (servreg_locator_client_type_p)servreg_calloc(sizeof(servreg_locator_client_type_t), 1);

   if(SERVREG_NULL != client)
   {
      /* Found empty slot */
      client->client_handle = client_handle;
      client->connected = SERVREG_TRUE;
      *connection_handle = client;
      client->next = SERVREG_NULL;

      /* Insert to the head of the list */
      client->next = qmi_locator_client_head;
      /* Update head */
      qmi_locator_client_head = client;

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
 *     servreg_qmi_locator_server_init
 *
 * Description:
 *     This function initializes the QMI servreg_locator services handler.
 *
 * Parameters:
 *    os_params: OS parameters.
 *
 * Returns:
 *     qmi_csi_service_handle
 * =====================================================================  */
qmi_csi_service_handle servreg_qmi_locator_server_init(qmi_csi_os_params *os_params)
{
   qmi_idl_service_object_type servreg_locator_server_obj = servreg_loc_get_service_object_v01();
   qmi_csi_options options;
   qmi_csi_error qmi_csi_return;

   QMI_CSI_OPTIONS_INIT(options);

   QMI_CSI_OPTIONS_SET_INSTANCE_ID(options, QMI_SERVREG_LOC_SERVICE_INSTANCE_APSS_V01);

   qmi_csi_return = qmi_csi_register_with_options(servreg_locator_server_obj,
                                                  servreg_locator_server_connect,
                                                  servreg_locator_server_disconnect,
                                                  servreg_locator_server_process_req,
                                                  NULL,
                                                  os_params,
                                                  &options,
                                                  &servreg_locator_qmi_server_handler);

   if(QMI_CSI_NO_ERR != qmi_csi_return)
   {
      TMS_MSG_ERROR("QMI locator server init failed");
   }
   else
   {
      TMS_MSG_HIGH("QMI locator server init success");
   }

   return servreg_locator_qmi_server_handler;
}

/** =====================================================================
 * Function:
 *     servreg_dl_internal_server_init
 *
 * Description:
 *     Initialization of the internal memory pools and other internals
 *     for domain list table and entries
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
static void servreg_dl_internal_server_init(void)
{
   DALSYS_InitMod(NULL);

   servreg_mutex_init_dal(&(servreg_dl_internal_server.mutex));
   servreg_mutex_init_dal(&(servreg_dl_internal_server.mutex_create));

   servreg_mutex_lock_dal(&(servreg_dl_internal_server.mutex));

   servreg_dl_internal_server.dynamic_use = SERVREG_TRUE;
   servreg_dl_table_pool_init(&servreg_dl_internal_server, &servreg_dl_table_pool_static_server);
   servreg_dl_entry_pool_init(&servreg_dl_internal_server, &servreg_dl_entry_pool_static_server);
   secure_memset(&servreg_dl_table_hash, 0, sizeof(servreg_dl_table_hash));

   servreg_mutex_unlock_dal(&(servreg_dl_internal_server.mutex));
}

/** =====================================================================
 * Thread: REX
 *     servreg_qmi_locator_server_req_task
 *
 * Description:
 *     Service Registry QMI locator server task
 *
 * Parameters:
 *    param: Task init parameter
 *
 * Returns:
 *     None
 * =====================================================================  */
static void servreg_qmi_locator_server_req_task(uint32 handle)
{
   uint32_t q_mask;
   qmi_csi_os_params os_params;
   qmi_csi_service_handle servreg_qmi_server_handle;

   os_params.tcb = rex_self();
   os_params.sig = SERVREG_QMI_LOC_SERVER_WAIT_MASK;

   servreg_qmi_server_handle = servreg_qmi_locator_server_init(&os_params);

   /* Block for start signal */
   rcinit_handshake_startup();    

   /* Task forever loop */
   for (;;)
   {
      q_mask = rex_wait(SERVREG_QMI_LOC_SERVER_WAIT_MASK );
      if (q_mask & SERVREG_QMI_LOC_SERVER_WAIT_MASK)
      {
         rex_clr_sigs(rex_self(), SERVREG_QMI_LOC_SERVER_WAIT_MASK);
         qmi_csi_handle_event(servreg_qmi_server_handle, &os_params);
      }
   }
}

/** =====================================================================
 * Function:
 *     servreg_locator_server_init
 *
 * Description:
 *     Initialization function for the service registry locator server module
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
void servreg_locator_server_init(void)
{
   char req_task_name[] = SERVREG_QMI_LOC_REQ_SERVER_TASK_NAME;
   static rex_tcb_type req_tcb;
   static rex_task_attributes_type req_attr;
   static char req_stack[SERVREG_QMI_LOC_SERVER_TASK_STACK];

   servreg_dl_internal_server_init();
   servreg_qmi_locator_server_node_internal_init();

   /* Rex task to handle QMI locator req messages */
   rex_task_attr_init(&req_attr);
   rex_task_attr_set_stack_addr(&req_attr, (rex_stack_pointer_type)req_stack);
   rex_task_attr_set_stack_size(&req_attr, SERVREG_QMI_LOC_SERVER_TASK_STACK);
   rex_task_attr_set_prio(&req_attr, 10);
   rex_task_attr_set_entry_func(&req_attr, servreg_qmi_locator_server_req_task, NULL);
   rex_task_attr_set_initially_suspended(&req_attr, FALSE);
   rex_task_attr_set_task_name(&req_attr, req_task_name);
   rex_common_def_task(&req_tcb, &req_attr);

   return;
}
