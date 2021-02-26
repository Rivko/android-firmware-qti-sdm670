#ifndef SERVREG_MONITOR_H
#define SERVREG_MONITOR_H
/*
#============================================================================
#  Name:
#    servreg_monitor.h 
#
#  Description:
#    Service Registry Monitor(srm) feature.
#    Clients register to get event and sync notifications for services they are
#    interested in. 
#
# Copyright (c) 2015-2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
#============================================================================
*/
#include "stdlib.h"
#include "servreg_common.h"

#if defined(__cplusplus)
extern "C"
{
#endif

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
SERVREG_MON_HANDLE servreg_alloc_monitor_handle(SERVREG_NAME domain, SERVREG_NAME service);

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
SERVREG_RESULT servreg_free_monitor_handle(SERVREG_MON_HANDLE sr_mon_handle);

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
SERVREG_RESULT servreg_set_state(SERVREG_MON_HANDLE sr_mon_handle, SERVREG_SERVICE_STATE state);

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
SERVREG_NAME servreg_get_service_name(SERVREG_MON_HANDLE sr_mon_handle);

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
SERVREG_RESULT servreg_set_synchronous(SERVREG_MON_HANDLE sr_mon_handle);

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
SERVREG_BOOL servreg_get_synchronous(SERVREG_MON_HANDLE sr_mon_handle);

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
uint32_t servreg_get_pending_acks(SERVREG_MON_HANDLE sr_mon_handle, SERVREG_SERVICE_STATE state);

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
 *
 * Note:
 *     Look at servreg_monitor_xxx.h for NHLOS interface API
 * =====================================================================  */
SERVREG_RESULT servreg_register_listener(SERVREG_MON_HANDLE sr_mon_handle, SERVREG_SIGEX_TYPE servreg_signal_type, SERVREG_SIGEX servreg_signal);

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
 * Note:
 *     Look at servreg_monitor_xxx.h for NHLOS interface API.
 *     If clients are no longer interested in the service AND they have de-registered their listeners
 *     using servreg_deregister_listener(), they can free the handle by calling servreg_free_monitor_handle().
 * =====================================================================  */
SERVREG_RESULT servreg_deregister_listener(SERVREG_MON_HANDLE sr_mon_handle, SERVREG_SIGEX_TYPE servreg_signal_type, SERVREG_SIGEX servreg_signal);

/** =====================================================================
 * Function:
 *     servreg_get_listener_ref_count
 *
 * Description:
 *     Get the listener reference count for the given monitor handle
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by domain + service 
 *                     or just domain name
 *
 * Returns:
 *     Listener ref count
 * =====================================================================  */
uint32_t servreg_get_listener_ref_count(SERVREG_MON_HANDLE sr_mon_handle);

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
SERVREG_SERVICE_STATE servreg_get_service_curr_state(SERVREG_MON_HANDLE sr_mon_handle);

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
 *     Current transaction id for that service
 * =====================================================================  */
int32_t servreg_get_transaction_id(SERVREG_MON_HANDLE sr_mon_handle);

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
 *     transaction_id : Transaction id of the service state for which the ACK has to be set
 *                      Query this value using servreg_get_transaction_id()
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_set_ack(SERVREG_MON_HANDLE sr_mon_handle, int32_t transaction_id);

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
SERVREG_RESULT servreg_register_sync_notif_cb(SERVREG_MON_HANDLE sr_mon_handle, SERVREG_SERVICE_STATE state, SERVREG_FNSIG_CALLBACK const callback, SERVREG_CB_PARAM const cb_p1, SERVREG_CB_PARAM const cb_p2);

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
SERVREG_RESULT servreg_deregister_sync_notif_cb(SERVREG_MON_HANDLE sr_mon_handle, SERVREG_SERVICE_STATE state, SERVREG_FNSIG_CALLBACK const callback, SERVREG_CB_PARAM const cb_p1, SERVREG_CB_PARAM const cb_p2);

#if defined(__cplusplus)
}
#endif

#endif
