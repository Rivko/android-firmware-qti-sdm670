#ifndef SERVREG_LOCATOR_INTERNAL_H
#define SERVREG_LOCATOR_INTERNAL_H
/*
#============================================================================
#  Name:
#    servreg_locator_internal.h 
#
#  Description:
#     Common header file for Service Registry feature
#
# Copyright (c) 2015-2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
#============================================================================
*/
#include "stdlib.h"
#include "servreg_common.h"
#include "servreg_internal.h"
#include "DALSys.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/* Domain list entry structure */
struct servreg_dl_entry_s
{
   SERVREG_NAME domain_name;
   uint32_t instance_id;
   uint8_t service_data_valid;
   uint32_t service_data;
   struct servreg_dl_entry_s * next;
};
typedef struct servreg_dl_entry_s servreg_dl_entry_t, * servreg_dl_entry_p;

/* Domain list table structure */
struct servreg_dl_table_s
{
   uint32_t domain_list_signature;
   SERVREG_NAME service_name;
   SERVREG_RESULT domain_list_status;
   uint16_t total_domains;
   uint16_t db_rev_count;
   struct servreg_dl_entry_s * domain_list_head;
   struct servreg_dl_table_s * next;
};
typedef struct servreg_dl_table_s servreg_dl_table_t, * servreg_dl_table_p;

/* Type casts as accessor functions */
#define sr_dl_table2sr_dl_handle(x)    ((SERVREG_DL_HANDLE)x)
#define sr_dl_handle2sr_dl_table(x)    ((servreg_dl_table_p)x)

/* Pool Allocations */
/* Pool allocation for domain list entry */
struct servreg_dl_entry_pool_s
{
   struct servreg_dl_entry_s servreg_dl_entry_pool[SERVREG_LOC_ENTRY_POOL_SIZE];
   struct servreg_dl_entry_pool_s * next;
};
typedef struct servreg_dl_entry_pool_s servreg_dl_entry_pool_t, * servreg_dl_entry_pool_p;

/* Pool allocation for domain list table */
struct servreg_dl_table_pool_s
{
   struct servreg_dl_table_s servreg_dl_table_pool[SERVREG_LOC_STRUCT_POOL_SIZE];
   struct servreg_dl_table_pool_s* next;
};
typedef struct servreg_dl_table_pool_s servreg_dl_table_pool_t, * servreg_dl_table_pool_p;

struct servreg_pd_list_s
{
   SERVREG_NAME pd_name;
   struct servreg_pd_list_s *next;
};

typedef struct servreg_pd_list_s servreg_pd_list_t, *servreg_pd_list_p;

/* Internal structure for domain list */
struct servreg_dl_internal_s
{
   servreg_dl_entry_pool_p servreg_dl_entry_pool_head_p;
   servreg_dl_table_pool_p servreg_dl_table_pool_head_p;
   servreg_dl_entry_p servreg_dl_entry_pool_free_p;
   servreg_dl_table_p servreg_dl_table_pool_free_p;
   servreg_mutex_t mutex;
   servreg_mutex_t mutex_create;
   SERVREG_BOOL dynamic_use;
   unsigned long init_flag;
};
typedef struct servreg_dl_internal_s servreg_dl_internal_t, * servreg_dl_internal_p;

typedef enum
{
   SERVREG_LOC_INDICATION_REGISTER_REQ      = QMI_SERVREG_LOC_INDICATION_REGISTER_REQ_V01,
   SERVREG_LOC_GET_DOMAIN_LIST_REQ          = QMI_SERVREG_LOC_GET_DOMAIN_LIST_REQ_V01,
   SERVREG_LOC_REGISTER_SERVICE_LIST_REQ    = QMI_SERVREG_LOC_REGISTER_SERVICE_LIST_REQ_V01,
   SERVREG_LOC_DATABASE_UPDATED_IND         = QMI_SERVREG_LOC_DATABASE_UPDATED_IND_V01,
   SERVREG_LOC_PFR_REQ                      = QMI_SERVREG_LOC_PFR_RESP_V01,   
}servreg_locator_msg_req_type;

#define SERVREG_LOC_REQ_TIMEOUT_MS           1000

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
servreg_dl_entry_p servreg_dl_entry_pool_init(servreg_dl_internal_p servreg_dl_internal, servreg_dl_entry_pool_p servreg_dl_entry_pool_static);

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
servreg_dl_entry_p servreg_dl_entry_pool_alloc(servreg_dl_internal_p servreg_dl_internal, servreg_dl_entry_pool_p servreg_dl_entry_pool_static);

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
servreg_dl_entry_p servreg_dl_entry_pool_free(servreg_dl_entry_p srdl_entry, servreg_dl_internal_p servreg_dl_internal);

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
servreg_dl_table_p servreg_dl_table_pool_init(servreg_dl_internal_p servreg_dl_internal, servreg_dl_table_pool_p servreg_dl_table_pool_static);

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
servreg_dl_table_p servreg_dl_table_pool_alloc(servreg_dl_internal_p servreg_dl_internal, servreg_dl_table_pool_p servreg_dl_table_pool_static);

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
servreg_dl_table_p servreg_dl_table_pool_free(servreg_dl_table_p sr_dl_table, servreg_dl_internal_p servreg_dl_internal);

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
void servreg_record_pd_entry(SERVREG_NAME sr_pd_name);

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
void servreg_delete_pd_entry(SERVREG_NAME sr_pd_name);

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
void servreg_loc_send_service_list(void);

#if defined(__cplusplus)
}
#endif

#endif
