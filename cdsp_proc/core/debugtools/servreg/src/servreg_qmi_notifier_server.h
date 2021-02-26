#ifndef SERVREG_QMI_NOTIFIER_SERVER_H
#define SERVREG_QMI_NOTIFIER_SERVER_H
/*
#============================================================================
#  Name:
#    servreg_qmi_notifier_server.h
#
#  Description:
#    Common Service Registry notifier header file for root images. This module serves as the end-point
#    of communication via QMI.
#
# Copyright (c) 2015-2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
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
 *     servreg_qmi_notif_server_node_internal_init
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
void servreg_qmi_notif_server_node_internal_init(void);

/** =====================================================================
 * Function:
 *     sr_qmi_s_register_proxy_listener
 *
 * Description:
 *     Register a qmi ind listener to get event notifications for a service it is interested in
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by domain + service 
 *                     or just domain name. This is the service that the listener is interested in.
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT sr_qmi_s_register_proxy_listener(SERVREG_MON_HANDLE sr_mon_handle);

/** =====================================================================
 * Function:
 *     sr_qmi_s_deregister_proxy_listener
 *
 * Description:
 *     De-register a qmi ind listener to get event notifications for a service it is interested in
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by domain + service 
 *                     or just domain name. This is the service that the listener is interested in.
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT sr_qmi_s_deregister_proxy_listener(SERVREG_MON_HANDLE sr_mon_handle);

/** =====================================================================
 * Function:
 *     servreg_send_qmi_ind_msg_state_change
 *
 * Description:
 *     Check which notif node state has changed and return that notif node's handle
 *
 * Parameters:
 *     servreg_qmi_notif_server_list_head : Head pointer of the notifier handle list for the user PD.
 *
 * Returns:
 *    void
 * =====================================================================  */
void servreg_send_qmi_ind_msg_state_change(void);

#if defined(__cplusplus)
}
#endif

#endif
