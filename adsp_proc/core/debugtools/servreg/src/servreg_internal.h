#ifndef SERVREG_INTERNAL_H
#define SERVREG_INTERNAL_H
/*
#============================================================================
#  Name:
#    servreg_internal.h 
#
#  Description:
#     Common header file for Service Registry feature
#
# Copyright (c) 2015-2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
#============================================================================
*/
#include "stdlib.h"
#include "servreg_common.h"
#include "DALSys.h"

#include "rcesn.h"
#include "rcecb.h"
#include "queue.h"
#include "servreg_trace.h"

#if !defined(SERVREG_EXCLUDE_KERNEL_QURT)
#include "qurt.h"
#include "rcevt_qurt.h"
#include "rcesn_qurt.h"
#include "servreg_monitor_qurt.h"
#endif

#if !defined(SERVREG_EXCLUDE_KERNEL_REX)
#include "rex.h"
#include "rcevt_rex.h"
#include "rcesn_rex.h"
#include "servreg_monitor_rex.h"
#endif

#include "tms_dll_api.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#define servreg_malloc(x)              (malloc(x))
#define servreg_calloc(size, count)    (calloc(size, count))
#define servreg_free(x)                (free(x))

/* All tasks information in the Service Registry framework */
#define SERVREG_QMI_NOTIF_REQ_SERVER_TASK_NAME            "sr_notif_task"
#define SERVREG_QMI_NOTIF_SERVER_TASK_STACK               2048

#define SERVREG_NOTIF_WORKER_TASK_NAME                    "sr_notif_worker"
#define SERVREG_NOTIF_SIGNAL_TASK_NAME                    "sr_notif_signal"

#define SERVREG_NOTIF_WORKER_TASK_STACK                   2048

#define SERVREG_QMI_LOC_CONNECT_TASK_NAME                 "sr_retry_task"
#define SERVREG_QMI_LOC_CONNECT_TASK_STACK                2048

#define SERVREG_QMI_LOC_REQ_SERVER_TASK_NAME              "sr_loc_task"
#define SERVREG_QMI_LOC_SERVER_TASK_STACK                 2048

/* Pool sizes */
#define SERVREG_LOC_ENTRY_POOL_SIZE                3
#define SERVREG_LOC_STRUCT_POOL_SIZE               3

//#define SERVREG_IND_MSG_ENTRY_POOL_SIZE            3

#define SERVREG_MON_NODE_POOL_SIZE                 3
#define SERVREG_MON_NODE_HASHTABLE_BUCKETS         10

#define SERVREG_QDI_NOTIF_NODE_POOL_SIZE           3

#define SERVREG_QMI_NOTIF_CLIENT_NODE_POOL_SIZE    3
#define SR_QMI_S_PROXY_POOL_SIZE                   3  // number of services for which we can proxy per pool
#define SR_QMI_S_CLIENT_POOL_SIZE                  3  // number of clients who can listen per pool

#define SERVREG_QMI_LOC_SERVER_NODE_POOL_SIZE      3
#define SERVREG_DL_DB_HASHTABLE_BUCKETS            10

/* Service Registry Signatures */
#define SERVREG_DL_SIGNATURE                       0xaaaabbbb
//#define SERVREG_IND_SIGNATURE                      0xbbbbcccc
#define SERVREG_MON_SIGNATURE                      0xccccdddd
#define SERVREG_QDI_NOTIF_SIGNATURE                0xeeeeffff
#define SERVREG_QMI_NOTIF_CLIENT_SIGNATURE         0xffffaaaa
#define SERVREG_QMI_NOTIF_SERVER_SIGNATURE         0xffffbbbb
#define SERVREG_QMI_LOC_SERVER_SIGNATURE           0xffffcccc

#define SERVREG_SERVICE_NAME                       "tms/servreg"

//todo: eliminate private opaque handles
typedef void* SERVREG_IND_HANDLE;
typedef void* SERVREG_QDI_NOTIF_HANDLE;
typedef void* SERVREG_QMI_NOTIF_CLIENT_HANDLE;
typedef void* SERVREG_QMI_LOC_SERVER_HANDLE;

/* Service Registry Monitor node list structure */
struct servreg_mon_node_s
{
   /* Service Registry monitor signature that is unique to this handle = 0xccccdddd */
   uint32_t mon_signature;

   /* Handle for the rcesn block that sends out notifications when there is a service state change */
   RCESN_HANDLE rcesn_handle;

   /* Service name */
   SERVREG_NAME service_name;

   /* Represents the number of clients who have alloc the monitor handle */
   uint32_t handle_count;

   /* Represents the number of listeners who have registered to get the service state change notifications */
   uint32_t listener_ref_count;

   /* Set this prameter to TRUE if you want the state change notification of this service to by synchronous */
   SERVREG_BOOL servreg_sync;

   /* Trasaciton_id refers to the number of times the state has changed. This number is required to set the correct ack */
   int32_t transaction_id;

   /* Rcecb handle that reperesents all the registrants who have registered for sync complete up notification */
   RCECB_HANDLE rcecb_up_handle;

   /* Rcecb handle that reperesents all the registrants who have registered for sync complete down notification */
   RCECB_HANDLE rcecb_down_handle;

   /* state_q represents the q pointer with pending state of the services that has to be set */
   /* The next state in the q is set only after all the ACK's from the previous state have arrived */
   q_type *state_q;

   /* Pointer to the next monitor node */
   struct servreg_mon_node_s* next;
};
typedef struct servreg_mon_node_s servreg_mon_node_t, * servreg_mon_node_p;

/* Service Registry Service state entry structure */
struct servreg_mon_queue_s
{
   /* Link for the next queue */
   q_link_type link;

   /* Represents the number of pending ACK's that we are expecting from the listeners */
   uint32_t ack_pending_count;

   /* Service state for this queue entry */
   SERVREG_SERVICE_STATE curr_state;

   /* Timeout value monitors the time taken for the ACK's to be received */
   uint32_t timeout;
};
typedef struct servreg_mon_queue_s servreg_mon_queue_t, *servreg_mon_queue_p;

struct servreg_mutex_s
{
   DALSYSSyncHandle mutex;
   DALSYSSyncObj mutexObject;

};
typedef struct servreg_mutex_s servreg_mutex_t, * servreg_mutex_p;

typedef unsigned long servreg_hash_t;

/** =====================================================================
 * MACRO:
 *     SERVREG_RCEVT_REGISTER_NAME_QURT & SERVREG_RCEVT_REGISTER_NAME_REX
 *
 * Description:
 *     Registers name with rcevt
 *
 * Parameters:
 *     sr_name        : Service Name to be registered
 *     sr_signal   : NHLOS specific notification signal information
 *
 * Returns:
 *     RCEVT_HANDLE
 * =====================================================================  */
#if !defined(SERVREG_EXCLUDE_KERNEL_QURT)
#define SERVREG_RCEVT_REGISTER_NAME_QURT(sr_name, sr_signal) rcevt_register_name_qurt(SERVREG_LOC_INIT_NAME, ((SERVREG_MON_SIGQURT*)sr_signal)->signal, ((SERVREG_MON_SIGQURT*)sr_signal)->mask)
#else
#define SERVREG_RCEVT_REGISTER_NAME_QURT(sr_name, sr_signal) (RCEVT_NULL)
#endif


#if !defined(SERVREG_EXCLUDE_KERNEL_REX)
#define SERVREG_RCEVT_REGISTER_NAME_REX(sr_name, sr_signal) rcevt_register_name_rex(SERVREG_LOC_INIT_NAME, ((SERVREG_MON_SIGREX*)sr_signal)->mask)
#else
#define SERVREG_RCEVT_REGISTER_NAME_REX(sr_name, sr_signal) (RCEVT_NULL)
#endif

/** =====================================================================
 * MACRO:
 *     SERVREG_RCESN_REGISTER_HANDLE_QURT & SERVREG_RCESN_REGISTER_HANDLE_REX
 *
 * Description:
 *     Registers handle with rcesn
 *
 * Parameters:
 *     sr_rcesn_handle     : rcesn handle
 *     sr_rcesn_comp_func  : rcesn compare function
 *     sr_signal           : NHLOS specific notification signal information
 *
 * Returns:
 *     RCESN_HANDLE
 * =====================================================================  */
#if !defined(SERVREG_EXCLUDE_KERNEL_QURT)
#define SERVREG_RCESN_REGISTER_HANDLE_QURT(sr_rcesn_handle, sr_rcesn_comp_func, sr_signal) rcesn_register_handle_qurt(sr_rcesn_handle, sr_rcesn_comp_func, ((SERVREG_MON_SIGQURT*)sr_signal)->signal, ((SERVREG_MON_SIGQURT*)sr_signal)->mask)
#else
#define SERVREG_RCESN_REGISTER_HANDLE_QURT(sr_rcesn_handle, sr_rcesn_comp_func, sr_signal) (RCESN_NULL)
#endif


#if !defined(SERVREG_EXCLUDE_KERNEL_REX)
#define SERVREG_RCESN_REGISTER_HANDLE_REX(sr_rcesn_handle, sr_rcesn_comp_func, sr_signal) rcesn_register_handle_rex(sr_rcesn_handle, sr_rcesn_comp_func, ((SERVREG_MON_SIGREX*)sr_signal)->mask)
#else
#define SERVREG_RCESN_REGISTER_HANDLE_REX(sr_rcesn_handle, sr_rcesn_comp_func, sr_signal) (RCESN_NULL)
#endif

/** =====================================================================
 * MACRO:
 *     SERVREG_RCESN_UNREGISTER_HANDLE_QURT & SERVREG_RCESN_UNREGISTER_HANDLE_REX
 *
 * Description:
 *     De-Registers handle with rcesn
 *
 * Parameters:
 *     sr_rcesn_handle     : rcesn handle
 *     sr_rcesn_comp_func  : rcesn compare function
 *     sr_signal           : NHLOS specific notification signal information
 *
 * Returns:
 *     RCESN_HANDLE
 * =====================================================================  */
#if !defined(SERVREG_EXCLUDE_KERNEL_QURT)
#define SERVREG_RCESN_UNREGISTER_HANDLE_QURT(sr_rcesn_handle, sr_rcesn_comp_func, sr_signal) rcesn_unregister_handle_qurt(sr_rcesn_handle, sr_rcesn_comp_func, ((SERVREG_MON_SIGQURT*)sr_signal)->signal, ((SERVREG_MON_SIGQURT*)sr_signal)->mask)
#else
#define SERVREG_RCESN_UNREGISTER_HANDLE_QURT(sr_rcesn_handle, sr_rcesn_comp_func, sr_signal) (RCESN_NULL)
#endif


#if !defined(SERVREG_EXCLUDE_KERNEL_REX)
#define SERVREG_RCESN_UNREGISTER_HANDLE_REX(sr_rcesn_handle, sr_rcesn_comp_func, sr_signal) rcesn_unregister_handle_rex(sr_rcesn_handle, sr_rcesn_comp_func, ((SERVREG_MON_SIGREX*)sr_signal)->mask)
#else
#define SERVREG_RCESN_UNREGISTER_HANDLE_REX(sr_rcesn_handle, sr_rcesn_comp_func, sr_signal) (RCESN_NULL)
#endif

/** =====================================================================
 * MACRO:
 *     SERVREG_MONITOR_INITIALIZE
 *
 * Description:
 *     Initialize the monitor module
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
#if !defined(SERVREG_INCLUDE_NOTIFIER)
#define SERVREG_MONITOR_INITIALIZE()
#else
extern void servreg_monitor_init(void);
#define SERVREG_MONITOR_INITIALIZE() servreg_monitor_init()
#endif

/** =====================================================================
 * MACRO:
 *     SERVREG_QDI_INITIALIZE
 *
 * Description:
 *     Initialize the QDI driver for servreg for root/user pd
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
#if !defined(SERVREG_INCLUDE_QDI)
#define SERVREG_QDI_INITIALIZE()
#else
extern void servreg_qdi_init(void);
#define SERVREG_QDI_INITIALIZE() servreg_qdi_init()
#endif

/** =====================================================================
 * MACRO:
 *     SERVREG_NOTIFIER_INITIALIZE
 *
 * Description:
 *     Initialize the service notifier (QDI or/and QMI) framework
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
#if !defined(SERVREG_INCLUDE_QDI)
   #if defined(SERVREG_INCLUDE_NOTIFIER)
#define SERVREG_NOTIFIER_INITIALIZE() servreg_qmi_notif_server_init()
   #else
#define SERVREG_NOTIFIER_INITIALIZE()
   #endif
#else
#define SERVREG_NOTIFIER_INITIALIZE() servreg_notifier_init()
#endif

#if !defined(SERVREG_EXCLUDE_LOCATOR)
/** =====================================================================
 * Function:
 *     servreg_qmi_locator_server_init
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
void servreg_locator_server_init(void);
#endif

/** =====================================================================
 * MACRO:
 *     SERVREG_LOCATOR_SERVER_INITIALIZE
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
#if !defined(SERVREG_EXCLUDE_LOCATOR)
#define SERVREG_LOCATOR_SERVER_INITIALIZE() servreg_locator_server_init()
#else
#define SERVREG_LOCATOR_SERVER_INITIALIZE()
#endif

/** =====================================================================
 * MACRO:
 *     SERVREG_SEND_SERVICE_LIST
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
#if !defined(SERVREG_EXCLUDE_UPDATE_SERVICE_LIST)
#define SERVREG_SEND_SERVICE_LIST() servreg_loc_send_service_list()
#else
#define SERVREG_SEND_SERVICE_LIST()
#endif

/** =====================================================================
 * MACRO:
 *     SERVREG_CREATE_TABLE_ENTRY
 *
 * Description:
 *     Create qmi table entry holding the values for qmi instance id for each domain
 *     to be used by the notifier module
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
#if !defined(SERVREG_INCLUDE_NOTIFIER)
#define SERVREG_CREATE_TABLE_ENTRY(domain_name, instance_id)
#else
#define SERVREG_CREATE_TABLE_ENTRY(domain_name, instance_id) servreg_create_qmi_table_entry(domain_name, instance_id)
#endif

/** =====================================================================
 * Function: INTERNAL not for PUBLIC use
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
SERVREG_MON_HANDLE servreg_get_sr_mon_handle(SERVREG_NAME const name);

/** =====================================================================
 * Function: INTERNAL not for PUBLIC use
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
void servreg_set_transaction_id(SERVREG_MON_HANDLE sr_mon_handle, int32_t sr_transaction_id);

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
SERVREG_NAME servreg_get_pd_name(SERVREG_MON_HANDLE sr_mon_handle);

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
SERVREG_NAME servreg_get_pd_name_from_domain(SERVREG_NAME domain_name);

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
SERVREG_RESULT servreg_set_asynchronous(SERVREG_MON_HANDLE sr_mon_handle);

/** =====================================================================
 * Function:
 *     servreg_mutex_init_dal
 *
 * Description:
 *     Initialize a DAL mutex
 *
 * Parameters:
 *     mutex_p : mutex to be initialized
 *
 * Returns:
 *     None
 * =====================================================================  */
void servreg_mutex_init_dal(servreg_mutex_p const mutex_p);

/** =====================================================================
 * Function:
 *     servreg_mutex_lock_dal
 *
 * Description:
 *     Lock the mutex
 *
 * Parameters:
 *    mutex_p : mutex to be locked
 *
 * Returns:
 *    None
 * =====================================================================  */
void servreg_mutex_lock_dal(servreg_mutex_p const mutex_p);

/** =====================================================================
 * Function:
 *     servreg_mutex_unlock_dal
 *
 * Description:
 *     Unlock the mutex
 *
 * Parameters:
 *     mutex_p : mutex to be unlocked
 *
 * Returns:
 *     None
 * =====================================================================  */
void servreg_mutex_unlock_dal(servreg_mutex_p const mutex_p);

/** =====================================================================
 * Function:
 *     servreg_nmelen
 *
 * Description:
 *     Function to find out the string length
 *
 * Parameters:
 *     name : string
 *
 * Returns:
 *     int : string length
 * =====================================================================  */
int servreg_nmelen(SERVREG_NAME const name);

/** =====================================================================
 * Function:
 *     servreg_nmecmp
 *
 * Description:
 *     Compares two strings
 *
 * Parameters:
 *     name_1 : string 1
 *     name_2 : string 2
 *     len    : length of the string
 *
 * Returns:
 *     -1, 0 or 1
 * =====================================================================  */
int servreg_nmecmp(SERVREG_NAME const name_1, SERVREG_NAME const name_2, int len);

/** =====================================================================
 * Function:
 *     servreg_nmehash
 *
 * Description:
 *     Function calculated the hash value given a name
 *
 * Parameters:
 *     name : name given to calculate the hash value
 *
 * Returns:
 *     hash : hash value calculated for the given name
 * =====================================================================  */
servreg_hash_t servreg_nmehash(SERVREG_NAME const name, uint32_t size);

/** =====================================================================
 * Function:
 *     servreg_check_and_set_down_notification
 *
 * Description:
 *     Function checks and sets the down notification
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by domain + service 
 *                     or just domain name
 *
 * Returns:
 *     SERVREG_TRUE if down notification is set
 *     SERVREG_FALSE if down notification was already set
 * =====================================================================  */
SERVREG_BOOL servreg_check_and_set_down_notification(SERVREG_MON_HANDLE sr_mon_handle);

/** =====================================================================
 * Function:
 *     servreg_clear_down_notification
 *
 * Description:
 *     Toggle the flag for next PDR
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by domain + service 
 *                     or just domain name
 *
 * Returns:
 *     SERVREG_TRUE if down notification is set
 *     SERVREG_FALSE if down notification was already set
 * =====================================================================  */
SERVREG_BOOL servreg_clear_down_notification(SERVREG_MON_HANDLE sr_mon_handle);

#if defined(__cplusplus)
}
#endif

#endif
