/*
#============================================================================
#  Name:                                                                     
#    servreg_locator.c 
#
#  Description:                                                              
#    Service Registry Locator feature. Clients query APSS to get the list of 
#    domains the service is running on.
#                                                                            
# Copyright (c) 2015-2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.        
#============================================================================
*/
#include "stdarg.h"
#include "stdlib.h"
#include <stringl/stringl.h>
#include "comdef.h"             /* Definitions for byte, word, etc.     */
#include "tms_utils_msg.h"
#include "qmi_client.h"
#include "qmi_cci_target_ext.h"
#include "err.h"
#include "timer.h"
#include "rcecb.h"
#include "rcinit.h"

#include "service_registry_locator_v01.h"
#include "servreg_locator_internal.h"
#include "servreg_internal.h"
#include "servreg_locator.h"
#include "servreg_localdb.h"
#include "servreg_notifier.h"

#define SERVREG_LOC_INIT_NAME                "servreg_loc_init"
#define SERVREG_SIG_LOC_TIMER_MASK           0x1

/* Internal Locator structure */
struct servreg_loc_internal_s
{
   SERVREG_SERVICE_STATE servreg_loc_client_init_status;     /* Flag indicating if the QMI client initialization is done */
   servreg_pd_list_p servreg_pd_list_head;                   /* List that maintains all the pd's that established a successuful qdi connection to the SR root-pd */

   qmi_idl_service_object_type servreg_loc_service_object;   /* QMI message related */
   qmi_client_type servreg_locator_client_handle;

   qurt_thread_t tid;
   uint32_t pfr_send_failure;
}; 
struct servreg_loc_internal_s servreg_loc_internal;

/* Pool allocation internals */
servreg_dl_internal_t servreg_dl_client_internal;
servreg_dl_entry_pool_t servreg_dl_entry_pool_static_client;
servreg_dl_table_pool_t servreg_dl_table_pool_static_client;

extern servreg_entry_t servreg_local_services[];

extern void servreg_monitor_init(void);

/* Static Functions defined in this file */
static void servreg_dl_internal_client_init(void);

static void servreg_loc_client_ind_cb(qmi_client_type client_handle, uint32_t msg_id, void *ind_buf, uint32_t ind_buf_len, void *ind_cb_data);
static SERVREG_RESULT servreg_locator_client_init(void);
static void servreg_set_servreg_loc_availability(void);

/** =====================================================================
 * Function:
 *     servreg_dl_entry_pool_init
 *
 * Description:
 *     Initializes the memory pool for domain list entry structure
 *
 * Parameters:
 *     None
 *
 * Returns:
 *    servreg_dl_entry_p : Returns the first free dl_entry space from the pool
 * =====================================================================  */
servreg_dl_entry_p servreg_dl_entry_pool_init(servreg_dl_internal_p servreg_dl_internal, servreg_dl_entry_pool_p servreg_dl_entry_pool_static)
{
   servreg_dl_entry_pool_p next_pool = SERVREG_NULL;

   if (SERVREG_NULL == servreg_dl_internal->servreg_dl_entry_pool_head_p)
   {
      next_pool = servreg_dl_entry_pool_static;
   }
   else if (SERVREG_TRUE == servreg_dl_internal->dynamic_use)
   {
      next_pool = (servreg_dl_entry_pool_p)servreg_calloc(sizeof(servreg_dl_entry_pool_t), 1);
   }

   if (SERVREG_NULL != next_pool)
   {
      int i;

      for (i = 0; i < SERVREG_LOC_ENTRY_POOL_SIZE; i++)
      {
         if (i != (SERVREG_LOC_ENTRY_POOL_SIZE - 1))
         {
            next_pool->servreg_dl_entry_pool[i].next = &(next_pool->servreg_dl_entry_pool[i + 1]);
         }
         else
         {
            next_pool->servreg_dl_entry_pool[i].next = servreg_dl_internal->servreg_dl_entry_pool_free_p;
         }

         next_pool->servreg_dl_entry_pool[i].domain_name = SERVREG_NULL;
         next_pool->servreg_dl_entry_pool[i].instance_id = 0;
         next_pool->servreg_dl_entry_pool[i].service_data_valid = 0;
         next_pool->servreg_dl_entry_pool[i].service_data = 0;
      }

      servreg_dl_internal->servreg_dl_entry_pool_free_p = &(next_pool->servreg_dl_entry_pool[0]);
      next_pool->next = servreg_dl_internal->servreg_dl_entry_pool_head_p;
      servreg_dl_internal->servreg_dl_entry_pool_head_p = next_pool;
   }
   else
   {
      TMS_MSG_ERROR("Calloc failed");
      return SERVREG_NULL;
   }

   return servreg_dl_internal->servreg_dl_entry_pool_free_p;
}

/** =====================================================================
 * Function:
 *     servreg_dl_entry_pool_alloc
 *
 * Description:
 *     Gives the first available free and allocated space from the memory
 *
 * Parameters:
 *     None
 *
 * Returns:
 *    servreg_dl_entry_p : the first available free and allocated space from the memory
 * =====================================================================  */
servreg_dl_entry_p servreg_dl_entry_pool_alloc(servreg_dl_internal_p servreg_dl_internal, servreg_dl_entry_pool_p servreg_dl_entry_pool_static)
{
   servreg_dl_entry_p ret;
   servreg_dl_entry_p sr_dl_entry;

   servreg_mutex_lock_dal(&(servreg_dl_internal->mutex));

   if (SERVREG_NULL == servreg_dl_internal->servreg_dl_entry_pool_free_p)
   {
      sr_dl_entry = servreg_dl_entry_pool_init(servreg_dl_internal, servreg_dl_entry_pool_static);
   }
   else
   {
      sr_dl_entry = servreg_dl_internal->servreg_dl_entry_pool_free_p;
   }

   if (SERVREG_NULL != sr_dl_entry)
   {
      servreg_dl_internal->servreg_dl_entry_pool_free_p = sr_dl_entry->next;
      sr_dl_entry->next = SERVREG_NULL;
      ret = sr_dl_entry;
   }
   else
   {
      TMS_MSG_ERROR("Alloc failed");
      ret = SERVREG_NULL;
   }

   servreg_mutex_unlock_dal(&(servreg_dl_internal->mutex));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_dl_entry_pool_free
 *
 * Description:
 *     Reclaims back the sr_dl_entry to the memory pool
 *
 * Parameters:
 *     sr_dl_entry : space to be reclaimed back
 *
 * Returns:
 *    servreg_dl_entry_p : The next available free space in the memory pool
 * =====================================================================  */
servreg_dl_entry_p servreg_dl_entry_pool_free(servreg_dl_entry_p sr_dl_entry, servreg_dl_internal_p servreg_dl_internal)
{
   servreg_dl_entry_p ret = SERVREG_NULL;

   servreg_mutex_lock_dal(&(servreg_dl_internal->mutex));

   if(SERVREG_NULL != sr_dl_entry)
   {
      /* Free the space that was dynamically allocated for the domain name */
      servreg_free(sr_dl_entry->domain_name);
      sr_dl_entry->domain_name = SERVREG_NULL;
      sr_dl_entry->instance_id = 0;
      sr_dl_entry->service_data_valid = 0;
      sr_dl_entry->service_data = 0;

      sr_dl_entry->next = servreg_dl_internal->servreg_dl_entry_pool_free_p;
      servreg_dl_internal->servreg_dl_entry_pool_free_p = sr_dl_entry;
      ret = sr_dl_entry;
   }
   else
   {
      TMS_MSG_ERROR("sr_dl_entry NULL and cannot be freed");
   }

   servreg_mutex_unlock_dal(&(servreg_dl_internal->mutex));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_dl_table_pool_init
 *
 * Description:
 *     Initializes the memory pool for domain list table structure
 *
 * Parameters:
 *     None
 *
 * Returns:
 *    servreg_dl_table_p : Returns the first free dl_table space from the pool
 * =====================================================================  */
servreg_dl_table_p servreg_dl_table_pool_init(servreg_dl_internal_p servreg_dl_internal, servreg_dl_table_pool_p servreg_dl_table_pool_static)
{
   servreg_dl_table_pool_p next_pool = SERVREG_NULL;

   if (SERVREG_NULL == servreg_dl_internal->servreg_dl_table_pool_head_p)
   {
      next_pool = servreg_dl_table_pool_static;
   }
   else if (SERVREG_TRUE == servreg_dl_internal->dynamic_use)
   {
      next_pool = (servreg_dl_table_pool_p)servreg_calloc(sizeof(servreg_dl_table_pool_t), 1);
   }

   if (SERVREG_NULL != next_pool)
   {
      int i;

      for (i = 0; i < SERVREG_LOC_STRUCT_POOL_SIZE; i++)
      {
         if (i != (SERVREG_LOC_STRUCT_POOL_SIZE - 1))
         {
            next_pool->servreg_dl_table_pool[i].next = &(next_pool->servreg_dl_table_pool[i + 1]);
         }
         else
         {
            next_pool->servreg_dl_table_pool[i].next = servreg_dl_internal->servreg_dl_table_pool_free_p;
         }

         next_pool->servreg_dl_table_pool[i].domain_list_signature = SERVREG_DL_SIGNATURE;
         next_pool->servreg_dl_table_pool[i].service_name = SERVREG_NULL;
         next_pool->servreg_dl_table_pool[i].domain_list_status = SERVREG_FAILURE;
         next_pool->servreg_dl_table_pool[i].total_domains = 0;
         next_pool->servreg_dl_table_pool[i].db_rev_count = 0;
         next_pool->servreg_dl_table_pool[i].domain_list_head = SERVREG_NULL;
      }

      servreg_dl_internal->servreg_dl_table_pool_free_p = &(next_pool->servreg_dl_table_pool[0]);
      next_pool->next = servreg_dl_internal->servreg_dl_table_pool_head_p;
      servreg_dl_internal->servreg_dl_table_pool_head_p = next_pool;
   }
   else
   {
      TMS_MSG_ERROR("Alloc failed");
      return SERVREG_NULL;
   }

   return servreg_dl_internal->servreg_dl_table_pool_free_p;
}

/** =====================================================================
 * Function:
 *     servreg_dl_table_pool_alloc
 *
 * Description:
 *     Gives the first available free and allocated space from the memory
 *
 * Parameters:
 *     None
 *
 * Returns:
 *    servreg_dl_table_p : the first available free and allocated space from the memory
 * =====================================================================  */
servreg_dl_table_p servreg_dl_table_pool_alloc(servreg_dl_internal_p servreg_dl_internal, servreg_dl_table_pool_p servreg_dl_table_pool_static)
{
   servreg_dl_table_p ret;
   servreg_dl_table_p sr_dl_table;

   servreg_mutex_lock_dal(&(servreg_dl_internal->mutex));

   if (SERVREG_NULL == servreg_dl_internal->servreg_dl_table_pool_free_p)
   {
      /* If free static node not available then get a dynamic node */
      sr_dl_table = servreg_dl_table_pool_init(servreg_dl_internal, servreg_dl_table_pool_static);
   }
   else
   {
      sr_dl_table = servreg_dl_internal->servreg_dl_table_pool_free_p;
   }

   if (SERVREG_NULL != sr_dl_table)
   {
      servreg_dl_internal->servreg_dl_table_pool_free_p = sr_dl_table->next;
      sr_dl_table->next = SERVREG_NULL;
      ret = sr_dl_table;
   }
   else
   {
      TMS_MSG_ERROR("Alloc failed");
      ret = SERVREG_NULL;
   }

   servreg_mutex_unlock_dal(&(servreg_dl_internal->mutex));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_dl_table_pool_free
 *
 * Description:
 *     Reclaims back the sr_dl_entry to the memory pool
 *
 * Parameters:
 *     sr_dl_table : space to be reclaimed back
 *
 * Returns:
 *    servreg_dl_table_p : The next available free space in the memory pool
 * =====================================================================  */
servreg_dl_table_p servreg_dl_table_pool_free(servreg_dl_table_p sr_dl_table, servreg_dl_internal_p servreg_dl_internal)
{
   servreg_dl_table_p ret = SERVREG_NULL;

   servreg_mutex_lock_dal(&(servreg_dl_internal->mutex));

   if(SERVREG_NULL != sr_dl_table)
   {
      /* Free the space that was dynamically allocated for the domain name */
      servreg_free(sr_dl_table->service_name);
      sr_dl_table->service_name = SERVREG_NULL;
      sr_dl_table->domain_list_status = SERVREG_FAILURE;
      sr_dl_table->total_domains = 0;
      sr_dl_table->db_rev_count = 0;
      sr_dl_table->domain_list_head = SERVREG_NULL;

      sr_dl_table->next = servreg_dl_internal->servreg_dl_table_pool_free_p; 
      servreg_dl_internal->servreg_dl_table_pool_free_p = sr_dl_table;
      ret = sr_dl_table;
   }
   else
   {
      TMS_MSG_ERROR("sr_dl_table NULL and cannot be freed");
   }
   servreg_mutex_unlock_dal(&(servreg_dl_internal->mutex));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_dl_internal_client_init
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
static void servreg_dl_internal_client_init(void)
{
   DALSYS_InitMod(NULL);

   servreg_mutex_init_dal(&(servreg_dl_client_internal.mutex));
   servreg_mutex_init_dal(&(servreg_dl_client_internal.mutex_create));

   servreg_mutex_lock_dal(&(servreg_dl_client_internal.mutex));

   servreg_dl_client_internal.dynamic_use = SERVREG_TRUE;
   servreg_dl_table_pool_init(&servreg_dl_client_internal, &servreg_dl_table_pool_static_client);
   servreg_dl_entry_pool_init(&servreg_dl_client_internal, &servreg_dl_entry_pool_static_client);

   servreg_mutex_unlock_dal(&(servreg_dl_client_internal.mutex));
}

/** =====================================================================
 * Function:
 *     servreg_loc_client_ind_cb
 *
 * Description: 
 *     Called by the QCCI infrastructure when an INDICATION message
 *     is received by the server on APSS (TN in our case when there 
 *     is a database update)
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
static void servreg_loc_client_ind_cb(qmi_client_type client_handle, uint32_t msg_id, void *ind_buf, uint32_t ind_buf_len, void *ind_cb_data)
{
   void *ind_msg;
   timetick_type response_time = 0;
   qmi_client_error_type qmi_err;
   uint32_t decoded_size;

   TMS_MSG_HIGH_3("Request: client_handle: %d, msg_id: %d, response_time: 0x%x", client_handle, msg_id, response_time);

   qmi_err = qmi_idl_get_message_c_struct_len(servreg_loc_internal.servreg_loc_service_object, QMI_IDL_INDICATION, msg_id, &decoded_size);

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
      TMS_MSG_ERROR_1("QMI decode error: %d" , qmi_err);
      servreg_free(ind_msg);
      return;
   }

   servreg_free(ind_msg);

   return;
}

/** =====================================================================
 * Function:
 *     servreg_locator_client_init
 *
 * Description:
 *     Initializes the service locator QMI client
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     SERVREG_SUCCESS, SERVREG_FAILURE
 * =====================================================================  */
static SERVREG_RESULT servreg_locator_client_init(void)
{
   qmi_client_error_type qmi_client_return;
   SERVREG_RESULT ret = SERVREG_FAILURE;

   if(SERVREG_SERVICE_STATE_UP == servreg_loc_internal.servreg_loc_client_init_status)
   {
      ret = SERVREG_SUCCESS;
   }
   else
   {
      servreg_loc_internal.servreg_loc_service_object = servreg_loc_get_service_object_v01();

      if (servreg_loc_internal.servreg_loc_service_object)
      {
         qmi_client_return = qmi_client_init_instance(servreg_loc_internal.servreg_loc_service_object,
                                                      QMI_SERVREG_LOC_SERVICE_INSTANCE_APSS_V01,
                                                      (qmi_client_ind_cb)servreg_loc_client_ind_cb,
                                                      NULL,
                                                      NULL, 
                                                      0,                       /* no timeout - just rot this thread */
                                                      &servreg_loc_internal.servreg_locator_client_handle
                                                      );
         if(QMI_NO_ERR == qmi_client_return)
         {
            ret = SERVREG_SUCCESS;
         }
         else
         {
            TMS_MSG_ERROR_1("QMI Locator client init failed: %d", qmi_client_return);
            ret = SERVREG_FAILURE;
         }
      }
      else
      {
         TMS_MSG_ERROR("servreg_loc_service_object NULL");
         ret = SERVREG_FAILURE;
      }
   }
   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_set_servreg_loc_availability
 *
 * Description:
 *     Set the state of Service Locator feature as UP
 *
 * Parameters:
 *     None

 * Returns:
 *     None
 * =====================================================================  */
static void servreg_set_servreg_loc_availability(void)
{
   RCECB_HANDLE rcecb_handle = RCECB_NULL;
   RCEVT_HANDLE rcevt_handle = RCEVT_NULL;

   SERVREG_SEND_SERVICE_LIST();

   rcecb_handle = rcecb_create_name(SERVREG_LOC_INIT_NAME);
   rcevt_handle = rcevt_create_name(SERVREG_LOC_INIT_NAME);

   if(RCECB_NULL != rcecb_handle)
   {
      if(0 < rcecb_getregistrants_handle(rcecb_handle))
      {
         /* Signal the listeners who have registered for servreg_loc availability via callbacks */
         if(RCECB_NULL == rcecb_signal_handle(rcecb_handle))
         {
            ERR_FATAL( "In servreg_locator_init() rcecb_signal_name() failed", 0, 0, 0);
         }
      }
   }
   else
   {
      ERR_FATAL( "In servreg_locator_init() rcecb_create_name() failed", 0, 0, 0);
   }

   if(RCEVT_NULL != rcevt_handle)
   {
      if(0 < rcevt_getregistrants_handle(rcecb_handle))
      {
         /* Signal the listeners who have registered for servreg_loc availability */
         if(RCEVT_NULL == rcevt_signal_handle(rcecb_handle))
         {
            ERR_FATAL( "In servreg_locator_init() rcevt_signal_name() failed", 0, 0, 0);
         }
      }
   }
   else
   {
      ERR_FATAL( "In servreg_locator_init() rcevt_create_name() failed", 0, 0, 0);
   }

   servreg_loc_internal.servreg_loc_client_init_status = SERVREG_SERVICE_STATE_UP;
}

/** =====================================================================
 * Function:
 *     servreg_alloc_domainlist_handle
 *
 * Description:
 *     Allocates domain list handle. 
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     SERVREG_DL_HANDLE : domain list handle
 *                         Check for return value and if it is NOT NULL
 * =====================================================================  */
SERVREG_DL_HANDLE servreg_alloc_domainlist_handle(void)
{
   SERVREG_DL_HANDLE dlhandle = SERVREG_NULL;
   servreg_dl_table_p sr_dl_table = SERVREG_NULL;

   servreg_mutex_lock_dal(&(servreg_dl_client_internal.mutex_create));

   /* Allocate new dl table from the pool */
   sr_dl_table = servreg_dl_table_pool_alloc(&servreg_dl_client_internal, &servreg_dl_table_pool_static_client);
   if (SERVREG_NULL != sr_dl_table)
   {
      dlhandle = sr_dl_table2sr_dl_handle(sr_dl_table);
   }
   else
   {
      TMS_MSG_ERROR("Calloc failed");
   }

   servreg_mutex_unlock_dal(&(servreg_dl_client_internal.mutex_create));
   return dlhandle;
}

/** =====================================================================
 * Function:
 *     servreg_get_domainlist
 *
 * Description:
 *     This queries the APSS(HLOS or TN) via a QMI message to get the list of
 *     all domains where the service runs
 *
 * Parameters:
 *    service_name : Service name that the client is interested in i.e "provider/service" info
 *    dlhandle     : Allocated domain list handle.
 *
 * Returns:
 *    Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_get_domainlist(SERVREG_NAME service_name, SERVREG_DL_HANDLE dlhandle)
{
  uint32_t i = 0;
  servreg_dl_table_p sr_dl_table = SERVREG_NULL;
  servreg_dl_entry_p srdl_entry = SERVREG_NULL;
  qmi_client_error_type qmi_client_return;
  qmi_servreg_loc_get_domain_list_req_msg_v01 servreg_loc_dl_req;
  qmi_servreg_loc_get_domain_list_resp_msg_v01 *servreg_loc_dl_resp = SERVREG_NULL;
  uint32_t domians_recvd = 0, len1 = 0, len2 = 0;

  sr_dl_table = sr_dl_handle2sr_dl_table(dlhandle);
  if (SERVREG_NULL == sr_dl_table)
  {
    TMS_MSG_ERROR("sr_dl_table NULL");
    return SERVREG_INVALID_HANDLE;
  }

  if(SERVREG_DL_SIGNATURE != sr_dl_table->domain_list_signature)
  {
    TMS_MSG_ERROR("dlhandle has invalid signature");
    return SERVREG_INVALID_HANDLE;
  }

  if(SERVREG_NULL == service_name)
  {
    TMS_MSG_ERROR("service_name NULL");
    return SERVREG_INVALID_PARAM;
  }
  
  len1 = servreg_nmelen(service_name) + 1;
  sr_dl_table->service_name = (SERVREG_NAME)servreg_calloc(sizeof(char), len1);
  if (SERVREG_NULL == sr_dl_table->service_name)
  {
    ERR_FATAL("sr_dl_table->service_name calloc failed", 0, 0, 0);
    return SERVREG_NO_MEMORY;
  }
  strlcpy(sr_dl_table->service_name, service_name, len1);

  /* Do heap allocation of the response message because the response packet is huge */
  servreg_loc_dl_resp = (qmi_servreg_loc_get_domain_list_resp_msg_v01*)servreg_calloc(sizeof(qmi_servreg_loc_get_domain_list_resp_msg_v01), 1);
  if (SERVREG_NULL == servreg_loc_dl_resp)
  {
    ERR_FATAL("servreg_loc_dl_resp calloc failed", 0, 0, 0);
    return SERVREG_NO_MEMORY;
  }

  /* Update the service_name in sr_dl_table */
  strlcpy(servreg_loc_dl_req.service_name, service_name, len1);
  servreg_loc_dl_req.domain_offset = 0;
  servreg_loc_dl_req.domain_offset_valid = 0;

  do
  {
    /* QMI message to send request for the domain list */
    qmi_client_return = qmi_client_send_msg_sync(servreg_loc_internal.servreg_locator_client_handle,  /* user_handle */
                                                 SERVREG_LOC_GET_DOMAIN_LIST_REQ,                     /* msg_id */
                                                 (void*)&servreg_loc_dl_req,                          /* *req_c_table */
                                                 sizeof(servreg_loc_dl_req),                          /* req_c_table_len*/
                                                 (void*)servreg_loc_dl_resp,                          /* *resp_c_table*/
                                                 sizeof(qmi_servreg_loc_get_domain_list_resp_msg_v01),/* resp_c_table_len*/
                                                 SERVREG_LOC_REQ_TIMEOUT_MS);                         /* timeout_msecs */
    if(QMI_NO_ERR != qmi_client_return)
    {
      sr_dl_table->domain_list_status = SERVREG_FAILURE;
      TMS_MSG_ERROR_1("QMI send error: %d", qmi_client_return);
      servreg_free(servreg_loc_dl_resp);
      return SERVREG_FAILURE;
    }

    if(QMI_RESULT_SUCCESS_V01 != servreg_loc_dl_resp->resp.result)
    {
      sr_dl_table->domain_list_status = SERVREG_FAILURE;
      TMS_MSG_ERROR_1("QMI error, Response: %d", servreg_loc_dl_resp->resp.error);
      servreg_free(servreg_loc_dl_resp);
      return SERVREG_FAILURE;
    }

    /* Update the total_domains and db_rev_count in sr_dl_table */
    sr_dl_table->total_domains = servreg_loc_dl_resp->total_domains;
    sr_dl_table->db_rev_count = servreg_loc_dl_resp->db_rev_count;

    /* Update the list in sr_dl_table with domain entry names */
    for(i = 0; i < servreg_loc_dl_resp->domain_list_len; i++)
    {
      srdl_entry = servreg_dl_entry_pool_alloc(&servreg_dl_client_internal, &servreg_dl_entry_pool_static_client);
      if (SERVREG_NULL == srdl_entry)
      {
        servreg_free(servreg_loc_dl_resp);
        ERR_FATAL("srdl_entry calloc failed", 0, 0, 0);
        return SERVREG_NO_MEMORY;
      }

      len2 = servreg_nmelen((SERVREG_NAME)&servreg_loc_dl_resp->domain_list[i].name) + 1;
      srdl_entry->domain_name = (SERVREG_NAME)servreg_calloc(sizeof(char), len2);
      if (SERVREG_NULL == srdl_entry->domain_name)
      {
        servreg_free(servreg_loc_dl_resp);
        ERR_FATAL("srdl_entry->domain_name calloc failed", 0, 0, 0);
        return SERVREG_NO_MEMORY;
      }
      strlcpy(srdl_entry->domain_name, (char *)&servreg_loc_dl_resp->domain_list[i].name, len2);

      srdl_entry->instance_id = servreg_loc_dl_resp->domain_list[i].instance_id;

      if(SERVREG_TRUE == servreg_loc_dl_resp->domain_list[i].service_data_valid)
      {
        srdl_entry->service_data_valid = servreg_loc_dl_resp->domain_list[i].service_data_valid;
        srdl_entry->service_data = servreg_loc_dl_resp->domain_list[i].service_data;
      }

      /* Keep a record of the qmi_instance id for that domain */
      SERVREG_CREATE_TABLE_ENTRY(srdl_entry->domain_name, srdl_entry->instance_id);

      /* Insert the domain_entry in the head of the list */
      srdl_entry->next = sr_dl_table->domain_list_head;

       /* Update the head */
      sr_dl_table->domain_list_head = srdl_entry;
    }

    /* Update the offset field in case the entire domain list is not received */
    servreg_loc_dl_req.domain_offset_valid = 1;
    domians_recvd = domians_recvd + servreg_loc_dl_resp->domain_list_len;
    servreg_loc_dl_req.domain_offset = domians_recvd + 1;

    sr_dl_table->domain_list_status = SERVREG_SUCCESS;

    if(domians_recvd != servreg_loc_dl_resp->total_domains)
    {
      TMS_MSG_HIGH_2("Getting domains list. Received: %d of %d", 
                      domians_recvd, servreg_loc_dl_resp->total_domains);
    }
    else
    {
      TMS_MSG_HIGH("Received all domains");
    }
  }while(domians_recvd != servreg_loc_dl_resp->total_domains);

  servreg_free(servreg_loc_dl_resp);
  return SERVREG_SUCCESS;
}

/** =====================================================================
 * Function:
 *     servreg_record_pd_entry
 *
 * Description:
 *     Record the pd entry in the pd-list
 *
 * Parameters:
 *    sr_pd_name     : Actual sr pd name associated to the pd monitor handle.
 *
 * Returns:
 *    None
 * =====================================================================  */
void servreg_record_pd_entry(SERVREG_NAME sr_pd_name)
{
   servreg_pd_list_p pd_entry = SERVREG_NULL;

   /* Record the user-pd in the pd-list */
   pd_entry = (servreg_pd_list_p)servreg_calloc(sizeof(servreg_pd_list_t), 1);

   if(SERVREG_NULL != pd_entry)
   {
      /* Will point to the actual pd monitor handle name. Hence no need to allocate memory again */
      pd_entry->pd_name = sr_pd_name;

      /* Insert to the head of the list */
      pd_entry->next = servreg_loc_internal.servreg_pd_list_head;

      /* Update head */
      servreg_loc_internal.servreg_pd_list_head = pd_entry;
   }
   else
   {
      ERR_FATAL("In servreg_qdi_open() calloc failed", 0, 0, 0);
   }

   return;
}

/** =====================================================================
 * Function:
 *     servreg_delete_pd_entry
 *
 * Description:
 *     Delete the pd entry in the pd-list
 *
 * Parameters:
 *    sr_pd_name     : Actual sr pd name associated to the pd monitor handle.
 *
 * Returns:
 *    None
 * =====================================================================  */
void servreg_delete_pd_entry(SERVREG_NAME sr_pd_name)
{
   servreg_pd_list_p pd_entry_curr = SERVREG_NULL, pd_entry_prev = SERVREG_NULL;

   /* Record the user-pd in the pd-list */
   pd_entry_curr = servreg_loc_internal.servreg_pd_list_head;

   if(SERVREG_NULL != sr_pd_name)
   {
      while(SERVREG_NULL != pd_entry_curr)
      {
         int len1 = servreg_nmelen(pd_entry_curr->pd_name);
         int len2 = servreg_nmelen(sr_pd_name);
         if(len1 == len2 && 0 == servreg_nmecmp(pd_entry_curr->pd_name, sr_pd_name, len1))
         {
            if(SERVREG_NULL == pd_entry_prev)
            {
               servreg_loc_internal.servreg_pd_list_head = pd_entry_curr->next;
            }
            else
            {
               pd_entry_prev->next = pd_entry_curr->next;
            }

            //free the data calloc'd in the original caller now that we are done with it
            servreg_free(pd_entry_curr->pd_name);
            servreg_free(pd_entry_curr);
            break;
         }

         pd_entry_prev = pd_entry_curr;
         pd_entry_curr = pd_entry_curr->next;
      }
   }
   return;
}

/** =====================================================================
 * Function:
 *     servreg_get_pdlist
 *
 * Description:
 *     Get the list of all pd's ("soc/domain/subdomain" strings) attached 
 *     to the SR framework in root-pd.
 *     If API used in user-pd, populates nothing.
 *
 * Parameters:
 *    dlhandle     : Allocated domain list handle.
 *
 * Returns:
 *    Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_get_pdlist(SERVREG_DL_HANDLE dlhandle)
{
   servreg_dl_table_p sr_dl_table = SERVREG_NULL;
   servreg_dl_entry_p sr_dl_entry = SERVREG_NULL;
   servreg_pd_list_p servreg_pd_list_curr = SERVREG_NULL;
   SERVREG_RESULT ret = SERVREG_FAILURE;
   uint32_t len1 = 0;

   servreg_mutex_lock_dal(&(servreg_dl_client_internal.mutex_create));

   sr_dl_table = sr_dl_handle2sr_dl_table(dlhandle);

   if (SERVREG_NULL != sr_dl_table)
   {
      if(SERVREG_DL_SIGNATURE == sr_dl_table->domain_list_signature)
      {
         /* Service name not populated in this API */
         sr_dl_table->service_name = SERVREG_NULL;
         sr_dl_table->db_rev_count = 0;
         sr_dl_table->total_domains = 0;
         sr_dl_table->domain_list_head = SERVREG_NULL;

         /* Get the pd list from the servreg qdi layer as this is populated only after the PD estalishes qdi connection to the root */
         servreg_pd_list_curr = servreg_loc_internal.servreg_pd_list_head;

         while(SERVREG_NULL != servreg_pd_list_curr)
         {
            /* Fill up the Pd entries */
            sr_dl_entry = servreg_dl_entry_pool_alloc(&servreg_dl_client_internal, &servreg_dl_entry_pool_static_client);

            if(SERVREG_NULL != sr_dl_entry)
            {
               sr_dl_entry->instance_id = 0;
               sr_dl_entry->service_data_valid = 0;
               sr_dl_entry->service_data = 0;

               if(SERVREG_NULL == servreg_pd_list_curr->pd_name)
               {
                  ERR_FATAL("pd_name in pd_list is NULL", 0, 0, 0);
               }

               len1 = servreg_nmelen(servreg_pd_list_curr->pd_name) + 1;
               sr_dl_entry->domain_name = (SERVREG_NAME)servreg_calloc(sizeof(char), len1);

               if (SERVREG_NULL != sr_dl_entry->domain_name)
               {
                  strlcpy(sr_dl_entry->domain_name, servreg_pd_list_curr->pd_name, len1);
               }
               else
               {
                  servreg_dl_table_pool_free(sr_dl_table, &servreg_dl_client_internal);
                  ERR_FATAL( "Calloc failed", 0, 0, 0);
               }

               /* Insert the domain_entry in the head of the list */
               sr_dl_entry->next = sr_dl_table->domain_list_head;

                /* Update the head */
               sr_dl_table->domain_list_head = sr_dl_entry;

               sr_dl_table->total_domains = sr_dl_table->total_domains + 1;
               servreg_pd_list_curr = servreg_pd_list_curr->next;
               ret = SERVREG_SUCCESS;
            }
            else
            {
               ERR_FATAL( "sr_dl_entry alloc failed", 0, 0, 0);
            }
         }

         sr_dl_table->domain_list_status = ret;
      }
      else
      {
         TMS_MSG_ERROR("dlhandle has invalid signature");
         ret = SERVREG_INVALID_HANDLE;
      }
   }
   else
   {
      TMS_MSG_ERROR("sr_dl_table NULL");
      ret = SERVREG_INVALID_HANDLE;
   }

   servreg_mutex_unlock_dal(&(servreg_dl_client_internal.mutex_create));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_get_num_entries
 *
 * Description:
 *      This gives the total number of domains the service runs on
 *
 * Parameters:
 *      dlhandle :  Allocated handle.
 *
 * Returns:
 *      Total number of domains the service runs on
 * =====================================================================  */
uint32_t servreg_get_num_entries(SERVREG_DL_HANDLE dlhandle)
{
   servreg_dl_table_p sr_dl_table = SERVREG_NULL;
   uint32_t ret = 0;

   servreg_mutex_lock_dal(&(servreg_dl_client_internal.mutex_create));

   sr_dl_table = sr_dl_handle2sr_dl_table(dlhandle);
   if (SERVREG_NULL != sr_dl_table)
   {
      if(SERVREG_DL_SIGNATURE == sr_dl_table->domain_list_signature)
      {
         ret = sr_dl_table->total_domains;
         TMS_MSG_HIGH_1("Total domains: %d", ret);
      }
      else
      {
         TMS_MSG_ERROR("dlhandle has invalid signature");
      }
   }
   else
   {
      TMS_MSG_ERROR("sr_dl_table NULL");
   }

   servreg_mutex_unlock_dal(&(servreg_dl_client_internal.mutex_create));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_get_entry
 *
 * Description:
 *     Given an offset this function returns the domain name
 *
 * Parameters:
 *     dlhandle : Allocated handle.
 *     offset   : offset at which the client wants the domain name.
 *     domain   : Result stored here. The Domain name at the offset.
 *
 * Returns:
 *    Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_get_entry(SERVREG_DL_HANDLE dlhandle, uint32_t num, char** domain)
{
   servreg_dl_entry_p sr_entry_p = SERVREG_NULL;
   uint32_t count = 1;
   servreg_dl_table_p sr_dl_table = SERVREG_NULL;
   SERVREG_RESULT ret = SERVREG_FAILURE;

   servreg_mutex_lock_dal(&(servreg_dl_client_internal.mutex_create));
   sr_dl_table = sr_dl_handle2sr_dl_table(dlhandle);

   if(SERVREG_NULL != sr_dl_table)
   {
      if(SERVREG_DL_SIGNATURE == sr_dl_table->domain_list_signature)
      {
         if(num <= sr_dl_table->total_domains && num > 0)
         {
            sr_entry_p = sr_dl_table->domain_list_head;

            while (SERVREG_NULL != sr_entry_p)
            {
               if (num == count)
               {
                  *domain = sr_entry_p->domain_name;
                  break;
               }
               count++;
               sr_entry_p = sr_entry_p->next;
            }

            ret = SERVREG_SUCCESS;
         }
         else
         {
            TMS_MSG_ERROR("Given number is out of bounds");
            ret = SERVREG_INVALID_PARAM;
         }
      }
      else
      {
         TMS_MSG_ERROR("dlhandle has invalid signature");
         ret = SERVREG_INVALID_HANDLE;
      }
   }
   else
   {
      TMS_MSG_ERROR("dlhandle NULL");
      ret = SERVREG_INVALID_HANDLE;
   }

   servreg_mutex_unlock_dal(&(servreg_dl_client_internal.mutex_create));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_free_domainlist_handle
 *
 * Description:
 *     Deallocates the domain list handle
 *
 * Parameters:
 *     dlhandle : Allocated handle.
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_free_domainlist_handle(SERVREG_DL_HANDLE dlhandle)
{
   servreg_dl_entry_p srdl_entry = SERVREG_NULL;
   servreg_dl_table_p sr_dl_table = SERVREG_NULL;
   SERVREG_RESULT ret = SERVREG_FAILURE;

   servreg_mutex_lock_dal(&(servreg_dl_client_internal.mutex_create));

   sr_dl_table = sr_dl_handle2sr_dl_table(dlhandle);
   if(SERVREG_NULL != sr_dl_table)
   {
      if(SERVREG_DL_SIGNATURE == sr_dl_table->domain_list_signature)
      {
         while(SERVREG_NULL != sr_dl_table->domain_list_head)
         {
            srdl_entry = sr_dl_table->domain_list_head;
            sr_dl_table->domain_list_head = sr_dl_table->domain_list_head->next;

            /* Reclaim the locator entry back to the free pool */
            servreg_dl_entry_pool_free(srdl_entry, &servreg_dl_client_internal);
         }

         /* Reclaim the locator table back to the free pool */
         if(SERVREG_NULL != servreg_dl_table_pool_free(sr_dl_table, &servreg_dl_client_internal))
         {
            ret = SERVREG_SUCCESS;
            TMS_MSG_HIGH("Given dlhandle freed");
         }
      }
      else
      {
         TMS_MSG_ERROR("dlhandle has invalid signature");
         ret = SERVREG_INVALID_HANDLE;
      }
   }
   else
   {
      TMS_MSG_ERROR("sr_dl_table NULL");
      ret = SERVREG_INVALID_HANDLE;
   }

   servreg_mutex_unlock_dal(&(servreg_dl_client_internal.mutex_create));

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_register_servloc_availability_signal
 *
 * Description:
 *     Clients register to get notified when Service Locator feature is available
 *
 * Parameters:
 *     servreg_signal_type : NHLOS specific notification signal type information
 *     servreg_signal      : NHLOS specific notification signal and mask information
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_register_servloc_availability_signal(SERVREG_SIGEX_TYPE servreg_signal_type, SERVREG_SIGEX servreg_signal)
{
   SERVREG_RESULT ret = SERVREG_FAILURE;
   RCEVT_HANDLE rcevt_handle = RCEVT_NULL;

   /* Event indicates startup completes */
   if(servreg_signal_type == SERVREG_SIGEX_TYPE_SIGQURT)
   {
      rcevt_handle = SERVREG_RCEVT_REGISTER_NAME_QURT(SERVREG_LOC_INIT_NAME, servreg_signal);
   }
   else
   {
      rcevt_handle = SERVREG_RCEVT_REGISTER_NAME_REX(SERVREG_LOC_INIT_NAME, servreg_signal);
   }

   if(RCEVT_NULL != rcevt_handle)
   {
      ret = SERVREG_SUCCESS;
      TMS_MSG_HIGH("Client registered for locator availability signal");
   }
   else
   {
      TMS_MSG_ERROR("Client registration failed for locator availability signal");
   }

   return ret;
}

/** =====================================================================
 * Function:
 *     servreg_register_servloc_availability_cb
 *
 * Description:
 *     Clients register to get notified when Service Locator feature is available
 *
 * Parameters:
 *     callback : Callback function registered
 *     cb_p1    : Callback function parameter
 *     cb_p2    : Callback function parameter 2
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_register_servloc_availability_cb(SERVREG_FNSIG_CALLBACK const callback, SERVREG_CB_PARAM const cb_p1, SERVREG_CB_PARAM const cb_p2)
{
   if(SERVREG_NULL == callback)
   {
      return SERVREG_INVALID_PARAM;
   }

   /* Event indicates startup completes */
   if(RCECB_NULL == rcecb_register_parm2_name(SERVREG_LOC_INIT_NAME, (RCECB_CALLBACK_FNSIG_P2)callback, (RCECB_PARM)cb_p1, (RCECB_PARM)cb_p2))
   {
      return SERVREG_INVALID_PARAM;
   }
   
   return SERVREG_SUCCESS;
}

/** =====================================================================
 * Function:
 *     servreg_deregister_servloc_availability_cb
 *
 * Description:
 *     Clients deregister if they do not want to get notified via  
 *     callback when Service Locator feature is available
 *
 * Parameters:
 *     callback : Callback function previously registered
 *     cb_p1    : Callback function parameter
 *     cb_p2    : Callback function parameter 2
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_deregister_servloc_availability_cb(SERVREG_FNSIG_CALLBACK const callback, SERVREG_CB_PARAM const cb_p1, SERVREG_CB_PARAM const cb_p2)
{
   if(SERVREG_NULL == callback)
   {
      return SERVREG_INVALID_PARAM;
   }

   /* Event indicates startup completes */
   if(RCECB_NULL == rcecb_unregister_parm2_name(SERVREG_LOC_INIT_NAME, (RCECB_CALLBACK_FNSIG_P2)callback, (RCECB_PARM)cb_p1, (RCECB_PARM)cb_p2))
   {
      return SERVREG_INVALID_PARAM;
   }
   
   return SERVREG_SUCCESS;
}

/** =====================================================================
 * Function:
 *     servreg_get_servloc_availability
 *
 * Description:
 *     Clients use this function to query the current state of Service Locator feature
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     Service state of Service Locator feature.
 *     Refer to the enum SERVREG_RESULT for list of possible results.
 * =====================================================================  */
SERVREG_SERVICE_STATE servreg_get_servloc_availability(void)
{
   TMS_MSG_HIGH_1("Service Locator status: 0x%X", servreg_loc_internal.servreg_loc_client_init_status);
   return servreg_loc_internal.servreg_loc_client_init_status;
}

/** =====================================================================
 * Function:
 *     servreg_loc_send_service_list
 *
 * Description:
 *     Send the service list to TN apps
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
void servreg_loc_send_service_list(void)
{
   qmi_client_error_type qmi_client_return;
   qmi_servreg_loc_register_service_list_req_msg_v01 *servreg_loc_dl_req;
   qmi_servreg_loc_register_service_list_resp_msg_v01 servreg_loc_dl_resp;
   SERVREG_NAME sr_local_domain = SERVREG_NULL;
   uint32_t service_count_total = 0, service_count_send = 0, service_list_len = 0, i = 0, j = 0;

   sr_local_domain = servreg_get_local_domain();

   /* Do heap allocation of the req message because the req packet is huge */
   servreg_loc_dl_req = (qmi_servreg_loc_register_service_list_req_msg_v01*)servreg_calloc(sizeof(qmi_servreg_loc_register_service_list_req_msg_v01), 1);

   if(SERVREG_NULL != servreg_loc_dl_req)
   {
      /* Update the domain_name in sr_dl_table */
      strlcpy((char *)&servreg_loc_dl_req->domain_name, sr_local_domain, servreg_nmelen(sr_local_domain) + 1);

      service_count_total = servreg_get_service_list_count();

      do
      {
         service_list_len = service_count_total - service_count_send;

         if(service_list_len > QMI_SERVREG_LOC_LIST_LENGTH_V01)
         {
            service_list_len = QMI_SERVREG_LOC_LIST_LENGTH_V01;
         }

         servreg_loc_dl_req->service_list_len = service_list_len;

         for(i = 0; i < service_list_len; i++)
         {
            strlcpy(servreg_loc_dl_req->service_list[i].name, servreg_local_services[j].service_name, servreg_nmelen(servreg_local_services[j].service_name) + 1);
            servreg_loc_dl_req->service_list[i].service_data_valid = servreg_local_services[j].service_data_valid;
            servreg_loc_dl_req->service_list[i].service_data = servreg_local_services[j].service_data;
            servreg_loc_dl_req->service_list[i].instance_id = servreg_get_local_instance_id();
            j++;
         }

         /* QMI message to send request for the service list */
         qmi_client_return = qmi_client_send_msg_sync(servreg_loc_internal.servreg_locator_client_handle,  /* user_handle */
                                        SERVREG_LOC_REGISTER_SERVICE_LIST_REQ,              /* msg_id */
                                        (void*)servreg_loc_dl_req,                        /* *req_c_table */
                                        sizeof(qmi_servreg_loc_register_service_list_req_msg_v01),                        /* req_c_table_len*/
                                        (void*)&servreg_loc_dl_resp,                       /* *resp_c_table*/
                                        sizeof(servreg_loc_dl_resp),                       /* resp_c_table_len*/
                                        SERVREG_LOC_REQ_TIMEOUT_MS                    /* timeout_msecs */
                                        );

         if(QMI_NO_ERR != qmi_client_return)
         {
            ERR_FATAL("SERVREG_LOC: servreg_loc_send_service_list failed", 0, 0, 0);
         }

         service_count_send = service_count_send + service_list_len;

      }while(service_count_send != service_count_total);
      servreg_free(servreg_loc_dl_req);
   }
   else
   {
      ERR_FATAL("SERVREG_LOC: calloc failed", 0, 0, 0);
   }

}

/** =====================================================================
 * Function:
 *     servreg_loc_pfr_qmi_resp_cb
 *
 * Description:
 *     Response callback fired when PFR send is done using async QMI 
 *
 * Parameters:
 *     unused
 *
 * Returns:
 *     None
 * =====================================================================  */
static void servreg_loc_pfr_qmi_resp_cb(qmi_client_type hClntHandle,
                                        unsigned int iMsgId,
                                        void *pRespCStruct,
                                        unsigned int iRespCStructLen,
                                        void *pRespCbData,
                                        qmi_client_error_type iTranspErr)
{
  /* If NO framework error */
  if (QMI_NO_ERR == iTranspErr)
  {
    /* Print receipt notice and response (if provided) */
    TMS_MSG_HIGH("PFR QMI msg receipt success");      
  }
  else
  {
    ++servreg_loc_internal.pfr_send_failure;
    TMS_MSG_ERROR_1("PFR QMI FW error: %d", servreg_loc_internal.pfr_send_failure);
  }

  /* Free response buffer */
  if (SERVREG_NULL != pRespCStruct)
  {
    servreg_free(pRespCStruct);
  }
}

/** =====================================================================
 * Function:
 *     servreg_loc_send_pfr
 *
 * Description:
 *     Send the PD failure reason
 *
 * Parameters:
 *     pd_name:     pointer to pd_name
 *     pd_name_len: pd name length
 *     pfr:         pointer to pd failure reason
 *     pfr_len:     pd failure reason string length
 *
 * Returns:
 *     None
 * =====================================================================  */
void servreg_loc_send_pfr(char * pd_name, int pd_name_len, char * pfr, int pfr_len)
{
   qmi_client_error_type qmi_client_return;
   qmi_servreg_loc_pfr_req_msg_v01  *servreg_loc_pfr_req = SERVREG_NULL;
   qmi_servreg_loc_pfr_resp_msg_v01 *servreg_loc_pfr_resp = SERVREG_NULL;
   qmi_txn_handle servreg_loc_pfr_trans_id = 0;

   /* Do heap allocation of messages */
   servreg_loc_pfr_req = (qmi_servreg_loc_pfr_req_msg_v01*)servreg_calloc(sizeof(qmi_servreg_loc_pfr_req_msg_v01), 1);
   servreg_loc_pfr_resp = (qmi_servreg_loc_pfr_resp_msg_v01*)servreg_calloc(sizeof(qmi_servreg_loc_pfr_resp_msg_v01), 1);

   if(SERVREG_NULL == servreg_loc_pfr_req || SERVREG_NULL == servreg_loc_pfr_resp)
   {
      ERR_FATAL("calloc failed", 0, 0, 0);
   }
   
   strlcpy((char *)&servreg_loc_pfr_req->serv_name, pd_name, pd_name_len + 1); /* PD name */
   strlcpy((char *)&servreg_loc_pfr_req->pd_failure_reason, pfr, pfr_len + 1); /* PD Failure Reason string */
   qmi_client_return = qmi_client_send_msg_async(servreg_loc_internal.servreg_locator_client_handle,  /* user_handle */
                                                 SERVREG_LOC_PFR_REQ,                                 /* msg_id */
                                                 (void*)servreg_loc_pfr_req,                          /* *req_c_table */
                                                 sizeof(qmi_servreg_loc_pfr_req_msg_v01),             /* req_c_table_len */
                                                 (void*)servreg_loc_pfr_resp,                         /* *resp_c_table */
                                                 sizeof(qmi_servreg_loc_pfr_resp_msg_v01),            /* resp_c_table_len */
                                                 servreg_loc_pfr_qmi_resp_cb,                         /* callback */
                                                 (void*) 0, &servreg_loc_pfr_trans_id);               /* arg & transaction ID */
   if(QMI_NO_ERR != qmi_client_return)
   {
      ++servreg_loc_internal.pfr_send_failure;
      TMS_MSG_ERROR_1("PFR QMI msg send failure: %d", servreg_loc_internal.pfr_send_failure);
      servreg_free(servreg_loc_pfr_resp);
   }
   
   /* Free request after sending QMI msg */
   servreg_free(servreg_loc_pfr_req);
}

/** =====================================================================
 * Thread:
 *     servreg_loc_init_task
 *
 * Description:
 *     Service Registry QMI ret-try connect to APSS locater task
 *
 * Parameters:
 *    param: Task init parameter
 *
 * Returns:
 *     None
 * =====================================================================  */
static void servreg_loc_init_task(void *argv __attribute__((unused)))
{
   SERVREG_RESULT ret = SERVREG_FAILURE;

   TMS_MSG_HIGH("Attempting APSS Service Locator Server connection");
   
   /* Task forever loop */
   for (;;)
   {
         ret = servreg_locator_client_init();

         if(SERVREG_SUCCESS == ret)
         {
            servreg_set_servreg_loc_availability();

            TMS_MSG_HIGH("Connected to APSS Service Locator Server");
         }

         qurt_thread_exit(1);
   }
}

/** =====================================================================
 * Function:
 *     servreg_locator_init
 *
 * Description:
 *     Initialization function for the service locator module
 *
 * Parameters:
 *      None
 *
 * Returns:
 *      None
 * =====================================================================  */
DLL_API_GLOBAL void servreg_locator_init(void)
{
   qurt_thread_attr_t req_attr;
   static unsigned long req_stack[SERVREG_QMI_LOC_CONNECT_TASK_STACK/sizeof(unsigned long)];

   /* Static memory pool allocation init */
   servreg_dl_internal_client_init();

   servreg_loc_internal.servreg_loc_client_init_status = SERVREG_SERVICE_STATE_UNINIT;
   servreg_loc_internal.servreg_pd_list_head = SERVREG_NULL;

   SERVREG_LOCATOR_SERVER_INITIALIZE();

   /* Initialize the service monitor framework */
   SERVREG_MONITOR_INITIALIZE();

   /* Initialize the QDI driver for servreg for root/user pd */
   SERVREG_QDI_INITIALIZE();

   /* Initialize the service notifier (QDI or/and QMI) framework */
   SERVREG_NOTIFIER_INITIALIZE();

   // long term everything above this should be moved into the task that is now always spawned and it
   // can be rcinit managed - for now, ensure no-timeout qmi connect is in it's own context
   TMS_MSG_HIGH("Spawning servreg_loc_init_task");

   /* Qurt task to handle QMI re-try connection attempts to APSS Locator */
   qurt_thread_attr_init(&req_attr);
   qurt_thread_attr_set_name(&req_attr, SERVREG_QMI_LOC_CONNECT_TASK_NAME);
   qurt_thread_attr_set_stack_addr(&req_attr, req_stack);
   qurt_thread_attr_set_stack_size(&req_attr, sizeof(req_stack));
   qurt_thread_attr_set_priority(&req_attr, qurt_thread_get_priority(qurt_thread_get_id()) - 1);

   qurt_thread_create(&servreg_loc_internal.tid, &req_attr, servreg_loc_init_task, NULL);

}