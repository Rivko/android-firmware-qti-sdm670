/*
#============================================================================
#  Name:
#    servreg_qdi_client.c 
#
#  Description:
#    Service Registry QDI Client Process Code that goes to user image 
#
# Copyright (c) 2015-2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
#============================================================================
*/
#include "stdarg.h"
#include "stdlib.h"
#include <stringl/stringl.h>
#include "comdef.h"             /* Definitions for byte, word, etc. */
#include "err.h"
#include "tms_utils_msg.h"
#include "tms_utils.h"

#include "servreg_internal.h"
#include "servreg_locator.h"
#include "servreg_localdb.h"
#include "servreg_monitor.h"
#include "servreg_notifier.h"
#include "servreg_qdi_notifier_user.h"
#include "servreg_qdi_client.h"  /* Include for user PD only */

int32_t servnotif_qdi_client_handle = -1 ;

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
SERVREG_RESULT servreg_qdi_invoke_worker_wait(uint32_t * remote_mask)
{
   int ret = -1;

   ret = qurt_qdi_handle_invoke(servnotif_qdi_client_handle, SERVREG_QDI_WORKER_WAIT, remote_mask);

   if(-1 != ret)
   {
      return SERVREG_SUCCESS;
   }
   else
   {
      return SERVREG_FAILURE;
   }
}

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
SERVREG_RESULT servreg_qdi_invoke_register_remote_listener(SERVREG_NAME service_name, uint32_t * sr_remote_handle, uint32_t * sr_remote_state, int32_t * sr_remote_transaction_id)
{
   int ret = -1;
   int len = 0;

   len = servreg_nmelen(service_name) + 1;
   ret = qurt_qdi_handle_invoke(servnotif_qdi_client_handle, SERVREG_QDI_REG_REMOTE_LISTENER, service_name, len, sr_remote_handle, sr_remote_state, sr_remote_transaction_id);

   if(-1 != ret)
   {
      return SERVREG_SUCCESS;
   }
   else
   {
      return SERVREG_FAILURE;
   }
}

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
SERVREG_RESULT servreg_qdi_invoke_deregister_remote_listener(SERVREG_NAME service_name)
{
   int ret = -1;
   int len = 0;

   len = servreg_nmelen(service_name) + 1;
   ret = qurt_qdi_handle_invoke(servnotif_qdi_client_handle, SERVREG_QDI_DEREG_REMOTE_LISTENER, service_name, len);

   if(-1 != ret)
   {
      return SERVREG_SUCCESS;
   }
   else
   {
      return SERVREG_FAILURE;
   }
}

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
SERVREG_RESULT servreg_qdi_invoke_free_remote_handle(SERVREG_NAME service_name)
{
   int ret = -1;
   int len = 0;

   len = servreg_nmelen(service_name) + 1;
   ret = qurt_qdi_handle_invoke(servnotif_qdi_client_handle, SERVREG_QDI_FREE_REMOTE_HANDLE, service_name, len);

   if(-1 != ret)
   {
      return SERVREG_SUCCESS;
   }
   else
   {
      return SERVREG_FAILURE;
   }
}

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
SERVREG_RESULT servreg_qdi_invoke_set_remote_ack(SERVREG_NAME service_name, int32_t sr_transaction_id)
{
   int ret = -1;
   int len = 0;

   len = servreg_nmelen(service_name) + 1;
   ret = qurt_qdi_handle_invoke(servnotif_qdi_client_handle, SERVREG_QDI_SET_REMOTE_ACK, service_name, len, sr_transaction_id);

   if(-1 != ret)
   {
      return SERVREG_SUCCESS;
   }
   else
   {
      return SERVREG_FAILURE;
   }
}

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
SERVREG_RESULT servreg_qdi_invoke_get_remote_handle(uint32_t * sr_remote_handle, uint32_t * curr_state, int32_t * transaction_id)
{
   int ret = -1;

   ret = qurt_qdi_handle_invoke(servnotif_qdi_client_handle, SERVREG_QDI_GET_REMOTE_HANDLE, sr_remote_handle, curr_state, transaction_id);

   if(-1 != ret)
   {
      return SERVREG_SUCCESS;
   }
   else
   {
      return SERVREG_FAILURE;
   }
}

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
SERVREG_RESULT servreg_qdi_invoke_create_qmi_entry(SERVREG_NAME domain_name, uint32_t qmi_instance_id)
{
   int ret = -1;
   int len = 0;

   len = servreg_nmelen(domain_name) + 1;
   ret = qurt_qdi_handle_invoke(servnotif_qdi_client_handle, SERVREG_QDI_CREATE_QMI_ENTRY, domain_name, len, qmi_instance_id);

   if(-1 != ret)
   {
      return SERVREG_SUCCESS;
   }
   else
   {
      return SERVREG_FAILURE;
   }
}

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
SERVREG_RESULT servreg_qdi_invoke_register_remote_handle(uint32_t * sr_remote_handle, SERVREG_NAME sr_name)
{
   int ret = -1;

   ret = qurt_qdi_handle_invoke(servnotif_qdi_client_handle, SERVREG_QDI_REGISTER_NEW_HANDLE, sr_remote_handle, sr_name);

   if(-1 != ret)
   {
      return SERVREG_SUCCESS;
   }
   else
   {
      return SERVREG_FAILURE;
   }
}

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
SERVREG_RESULT servreg_qdi_invoke_set_state(uint32_t sr_remote_handle, SERVREG_SERVICE_STATE sr_state)
{
   int ret = -1;

   ret = qurt_qdi_handle_invoke(servnotif_qdi_client_handle, SERVREG_QDI_SET_STATE, sr_remote_handle, (uint32_t)sr_state);

   if(-1 != ret)
   {
      return SERVREG_SUCCESS;
   }
   else
   {
      return SERVREG_FAILURE;
   }
}

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
 *     sr_remote_handle  : remote (root-pd's) monitor handle address who's state changed
 *
 * Returns:
 *    SERVREG_SUCCESS (0) or SERVREG_FAILURE(-1)
 * =====================================================================  */
SERVREG_RESULT servreg_qdi_invoke_update_remote_handle(SERVREG_SERVICE_STATE sr_state, int32_t sr_transaction_id, uint32_t sr_remote_handle)
{
   int ret = -1;

   ret = qurt_qdi_handle_invoke(servnotif_qdi_client_handle, SERVREG_QDI_UPDATE_HANDLE, (uint32_t)sr_state, sr_transaction_id, sr_remote_handle);

   if(-1 != ret)
   {
      return SERVREG_SUCCESS;
   }
   else
   {
      return SERVREG_FAILURE;
   }
}

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
SERVREG_RESULT servreg_qdi_invoke_register_root_handles(uint32_t * sr_remote_handle, SERVREG_NAME sr_name)
{
   int ret = -1;

   ret = qurt_qdi_handle_invoke(servnotif_qdi_client_handle, SERVREG_QDI_REGISTER_ROOT_HANDLES, sr_remote_handle, sr_name);

   if(-1 != ret)
   {
      return SERVREG_SUCCESS;
   }
   else
   {
      return SERVREG_FAILURE;
   }
}

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
SERVREG_RESULT servreg_qdi_invoke_get_ack_handle(SERVREG_NAME sr_name, int32_t * sr_transaction_id)
{
   int ret = -1;

   ret = qurt_qdi_handle_invoke(servnotif_qdi_client_handle, SERVREG_QDI_GET_ROOT_ACK_HANDLE, sr_name, sr_transaction_id);

   if(-1 != ret)
   {
      return SERVREG_SUCCESS;
   }
   else
   {
      return SERVREG_FAILURE;
   }
}

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
SERVREG_RESULT servreg_qdi_invoke_close(void)
{
   int ret = -1;

   ret = qurt_qdi_close(servnotif_qdi_client_handle);

   if(-1 != ret)
   {
      return SERVREG_SUCCESS;
   }
   else
   {
      return SERVREG_FAILURE;
   }
}

/** =====================================================================
 * Function:
 *     servreg_register_user_listener
 *
 * Description:
 *     Register client as a user listener because the service is not a local service.
 *
 * Parameters:
 *     service_name             : Service name that the client is interested in 
 *     sr_mon_handle            : Opaque handle to existing event
 *     sr_curr_remote_state     : State of the service received from the QMI message
 *     sr_remote_transaction_id : Transaction id of the service received from the QMI message
 *
 * Returns:
 *     SERVREG_SUCCESS (0) or SERVREG_FAILURE(-1)
 * =====================================================================  */
SERVREG_RESULT servreg_register_user_listener(SERVREG_NAME service_name, SERVREG_MON_HANDLE sr_mon_handle, uint32_t * sr_curr_remote_state, int32_t * sr_remote_transaction_id)
{
   /* Should not come here according to all the checks done while registering */
   return SERVREG_SUCCESS;
}

/** =====================================================================
 * Function:
 *     servreg_set_user_ack
 *
 * Description:
 *     Register client as a user listener because the service is not a local service.
 *
 * Parameters:
 *     service_name             : Service name that the client is interested in 
 *     sr_mon_handle            : Opaque handle to existing event
 *     sr_curr_remote_state     : State of the service received from the QMI message
 *     sr_remote_transaction_id : Transaction id of the service received from the QMI message
 *
 * Returns:
 *     SERVREG_SUCCESS (0) or SERVREG_FAILURE(-1)
 * =====================================================================  */
SERVREG_RESULT servreg_set_user_ack(SERVREG_NAME service_name, SERVREG_MON_HANDLE sr_mon_handle, uint32_t sr_curr_remote_state, int32_t sr_remote_transaction_id)
{
   /* Should not come here according to all the checks done while acking */
   return SERVREG_SUCCESS;
}


/** =====================================================================
 * Function:
 *     servreg_qdi_init
 *
 * Description:
 *     Initialization function in user pd.
 *     Function to obtain a QDI handle given the qdi device name.
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
void servreg_qdi_init(void)
{
   SERVREG_NAME domain = SERVREG_NULL;
   int len = 0;

   domain = servreg_get_local_domain();

   if (SERVREG_NULL != domain)
   {
      len = servreg_nmelen(domain) + 1;
      servnotif_qdi_client_handle = qurt_qdi_open(SERVREG_QDI_DEVICE, domain, len);
   }

   if (servnotif_qdi_client_handle < 0)
   {
      ERR_FATAL( "SERVREG_QDI: user-pd servreg_qdi_init failed", 0, 0, 0);
   }
}


