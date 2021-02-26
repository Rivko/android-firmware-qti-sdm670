#ifndef SERVREG_NOTIFIER_H
#define SERVREG_NOTIFIER_H
/*
#============================================================================
#  Name:
#    servreg_notifier.h
#
#  Description:
#    Common Service Registry notifier header file for root and user images
#
# Copyright (c) 2014 - 2016 by Qualcomm Technologies Incorporated. All Rights Reserved.
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
 *     servreg_register_remote_listener
 *
 * Description:
 *     Register client as a remote listener because the service is not a local service.
 *     QDI or Glink used as the communication medium for registering the listener
 *     with the remote service registry framework
 *
 * Parameters:
 *     service_name       : Service name that the client is interested in 
 *     SERVREG_MON_HANDLE : Opaque handle to existing event
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_register_remote_listener(SERVREG_NAME service_name, SERVREG_MON_HANDLE sr_mon_handle, uint32_t * sr_curr_remote_state, int32_t * sr_remote_transaction_id);

/** =====================================================================
 * Function:
 *     servreg_deregister_remote_listener
 *
 * Description:
 *     De-register client as a remote listener because the service is not a local service.
 *     QDI or Glink used as the communication medium for registering the listener
 *     with the remote service registry framework
 *
 * Parameters:
 *     service_name       : Service name that the client is interested in 
 *     SERVREG_MON_HANDLE : Opaque handle to existing event
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_deregister_remote_listener(SERVREG_NAME service_name, SERVREG_MON_HANDLE sr_mon_handle);

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
SERVREG_RESULT servreg_free_remote_handle(SERVREG_NAME service_name, SERVREG_MON_HANDLE sr_mon_handle);

/** =====================================================================
 * Function:
 *     servreg_set_remote_ack
 *
 * Description:
 *     Set the remote ack count for the remote listener via QDI or Glink
 *
 * Parameters:
 *     service_name       : Service name that the client is interested in 
 *     SERVREG_MON_HANDLE : Opaque handle to existing event
 *     curr_state         : State for which the ACK needs to be set
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_set_remote_ack(SERVREG_NAME service_name, SERVREG_MON_HANDLE sr_mon_handle, SERVREG_SERVICE_STATE curr_state, int32_t sr_transaction_id);

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
void servreg_create_qmi_table_entry(SERVREG_NAME domain_name, uint32 qmi_instance_id);

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
void servreg_qmi_notif_server_init(void);

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
void servreg_notifier_init(void);

#if defined(__cplusplus)
}
#endif

#endif
