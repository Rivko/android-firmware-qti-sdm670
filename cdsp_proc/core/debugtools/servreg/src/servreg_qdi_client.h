#ifndef SERVREG_QDI_CLIENT_H
#define SERVREG_QDI_CLIENT_H
/*
#============================================================================
#  Name:
#    servreg_qdi_client.c 
#
#  Description:
#    Service Registry QDI Client Code for user pd images only
#
# Copyright (c) 2015-2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
#============================================================================
*/
#include "stdlib.h"
#include "servreg_qdi.h"    /* Including the qdi driver header file for root here as clients also shares common enum */

#if defined(__cplusplus)
extern "C"
{
#endif

/** =====================================================================
 * Function:
 *     servreg_qdi_invoke_worker_wait
 *
 * Description:
 *     Invokes the qdi driver function that does a qurt_anysignal_wait.
 *     It waits for any state change that happens to the remote services the 
 *     clients in this user pd are interested in.
 *
 * Parameters:
 *     remote_mask : mask retrieved from the root pd to figure out which signal was set
 *
 * Returns:
 *    SERVREG_SUCCESS (0) or SERVREG_FAILURE(-1)
 * =====================================================================  */
SERVREG_RESULT servreg_qdi_invoke_worker_wait(uint32_t * remote_mask);

/** =====================================================================
 * Function:
 *     servreg_qdi_invoke_register_remote_listener
 *
 * Description:
 *     Registers the client as a remote proxy listener via QDI layer. A Proxy client will
 *     be listed as a listener in the root PD rcesn and srm node via QDI.
 *
 * Parameters:
 *    service_name             : "soc/domain/subdomain" info
 *    sr_remote_handle         : The remote monitor handle address (root-pd) will be stored here
 *    sr_remote_state          : The remote monitor handle's state (root-pd) will be stored here
 *    sr_remote_transaction_id : Id from the root pd
 *
 * Returns:
 *    SERVREG_SUCCESS (0) or SERVREG_FAILURE(-1)
 * =====================================================================  */
SERVREG_RESULT servreg_qdi_invoke_register_remote_listener(SERVREG_NAME service_name, uint32_t * sr_remote_handle, uint32_t * sr_remote_state, int32_t * sr_remote_transaction_id);

/** =====================================================================
 * Function:
 *     servreg_qdi_invoke_deregister_remote_listener
 *
 * Description:
 *     De-Registers the client as a remote listener via QDI layer. The Proxy client will
 *     be removed as a listener in the root PD rcesn and srm node via QDI.
 *
 * Parameters:
 *    service_name   : "soc/domain/subdomain" info
 *
 * Returns:
 *    SERVREG_SUCCESS (0) or SERVREG_FAILURE(-1)
 * =====================================================================  */
SERVREG_RESULT servreg_qdi_invoke_deregister_remote_listener(SERVREG_NAME service_name);

/** =====================================================================
 * Function:
 *     servreg_qdi_invoke_free_remote_handle
 *
 * Description:
 *     Invoke the free handle method if the clients in this user pd are no longer
 *     interested in the service.
 *
 * Parameters:
 *    service_name   : "soc/domain/subdomain" info
 *
 * Returns:
 *    SERVREG_SUCCESS (0) or SERVREG_FAILURE(-1)
 * =====================================================================  */
SERVREG_RESULT servreg_qdi_invoke_free_remote_handle(SERVREG_NAME service_name);

/** =====================================================================
 * Function:
 *     servreg_qdi_invoke_set_remote_ack
 *
 * Description:
 *     Invoke the qdi method to set the ack count of the service from the remote proxy listener
 *
 * Parameters:
 *    service_name   : "soc/domain/subdomain" info
 *    sr_transaction_id : Transaction id of the service state for which the ack is being set
 *
 * Returns:
 *    SERVREG_SUCCESS (0) or SERVREG_FAILURE(-1)
 * =====================================================================  */
SERVREG_RESULT servreg_qdi_invoke_set_remote_ack(SERVREG_NAME service_name, int32_t sr_transaction_id);

/** =====================================================================
 * Function:
 *     servreg_qdi_invoke_get_remote_handle
 *
 * Description:
 *     Invoke the qdi method which returns the remote monitor handle who's state changed
 * Parameters:
 *     sr_remote_handle : remote (root-pd's) monitor handle address who's state changed
 *     curr_state       : new state of the remote (root-pd's) monitor handle who's state changed
 *     transaction_id   : id from the root pd
 *
 * Returns:
 *    SERVREG_SUCCESS (0) or SERVREG_FAILURE(-1)
 * =====================================================================  */
SERVREG_RESULT servreg_qdi_invoke_get_remote_handle(uint32_t * name_addr, uint32_t * curr_state, int32_t * transaction_id);

/** =====================================================================
 * Function:
 *     servreg_qdi_invoke_create_qmi_entry
 *
 * Description:
 *     Invoke the qdi method to create an entry for the domain name and qmi_instance_id queried from the locator
 *
 * Parameters:
 *    domain_name   : "soc/domain/subdomain" info
 *    qmi_instance_id : qmi instance id required by the root pd to establish a connection with the remote server
 *
 * Returns:
 *    SERVREG_SUCCESS (0) or SERVREG_FAILURE(-1)
 * =====================================================================  */
SERVREG_RESULT servreg_qdi_invoke_create_qmi_entry(SERVREG_NAME domain_name, uint32_t qmi_instance_id);

/** =====================================================================
 * Function:
 *     servreg_qdi_invoke_register_remote_handle
 *
 * Description:
 *     Invoke the qdi method to register a client to a service in the user pd
 *
 * Parameters:
 *     sr_remote_handle : remote (root-pd's) monitor handle
 *     sr_name          : user service name to be registerd against
 *
 * Returns:
 *    SERVREG_SUCCESS (0) or SERVREG_FAILURE(-1)
 * =====================================================================  */
SERVREG_RESULT servreg_qdi_invoke_register_remote_handle(uint32_t * sr_remote_handle, SERVREG_NAME sr_name);

/** =====================================================================
 * Function:
 *     servreg_qdi_invoke_set_state
 *
 * Description:
 *     Invoke the qdi method which sets the state of user pd service in the root pd
 *
 * Parameters:
 *     sr_remote_handle : remote (root-pd's) monitor handle address who's state changed
 *     sr_state         : new state of the user service to be set
 *
 * Returns:
 *    SERVREG_SUCCESS (0) or SERVREG_FAILURE(-1)
 * =====================================================================  */
SERVREG_RESULT servreg_qdi_invoke_set_state(uint32_t sr_remote_handle, SERVREG_SERVICE_STATE sr_state);

/** =====================================================================
 * Function:
 *     servreg_qdi_invoke_update_remote_handle
 *
 * Description:
 *     Invoke the qdi method which updates the state and the transaction id in the root pd
 *
 * Parameters:
 *     sr_state          : state to be set in the root pd
 *     sr_transaction_id : id to be set in the root pd 
 *     sr_remote_handle : remote (root-pd's) monitor handle address who's state changed
 *
 * Returns:
 *    SERVREG_SUCCESS (0) or SERVREG_FAILURE(-1)
 * =====================================================================  */
SERVREG_RESULT servreg_qdi_invoke_update_remote_handle(SERVREG_SERVICE_STATE sr_state, int32_t sr_transaction_id, uint32_t sr_remote_handle);

/** =====================================================================
 * Function:
 *     servreg_qdi_invoke_register_root_handles
 *
 * Description:
 *     Invoke the qdi method which registers the first client in the cache head
 *
 * Parameters:
 *     sr_remote_handle : remote (root-pd's) monitor handle address
 *     sr_name          : user service name to be registered against
 *
 * Returns:
 *    SERVREG_SUCCESS (0) or SERVREG_FAILURE(-1)
 * =====================================================================  */
SERVREG_RESULT servreg_qdi_invoke_register_root_handles(uint32_t * sr_remote_handle, SERVREG_NAME sr_name);

/** =====================================================================
 * Function:
 *     servreg_qdi_invoke_get_ack_handle
 *
 * Description:
 *     Invoke the qdi method which gives the info on the client that acked
 *
 * Parameters:
 *     sr_name           : user servic name for which the ack was sent
 *     sr_transaction_id : id associated with the ack
 *
 * Returns:
 *    SERVREG_SUCCESS (0) or SERVREG_FAILURE(-1)
 * =====================================================================  */
SERVREG_RESULT servreg_qdi_invoke_get_ack_handle(SERVREG_NAME sr_name, int32_t * sr_transaction_id);

/** =====================================================================
 * Function:
 *     servreg_qdi_invoke_close
 *
 * Description:
 *     Closes the specified driver, releasing any resources associated with the open driver
 *
 * Parameters:
 *     handle : handle to be released
 *
 * Returns:
 *     SERVREG_SUCCESS (0) or SERVREG_FAILURE(-1)
 * =====================================================================  */
SERVREG_RESULT servreg_qdi_invoke_close(void);

#if defined(__cplusplus)
}
#endif

#endif
