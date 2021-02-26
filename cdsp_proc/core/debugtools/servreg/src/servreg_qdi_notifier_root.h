#ifndef SERVREG_QDI_NOTIFIER_ROOT_H
#define SERVREG_QDI_NOTIFIER_ROOT_H
/*
#============================================================================
#  Name:
#    servreg_qdi_notifier_root.h
#
#  Description:
#    Common Service Registry notifier header file for root images
#
# Copyright (c) 2014-2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
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
 *     servreg_get_qdi_notif_node
 *
 * Description:
 *     Checks if a SRM node already exists with the given name. If it does 
 *     exists it returns a pointer to that srm node.
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by just domain name.
 *     servreg_notif_list_head : Head pointer of the notifier handle list for the user PD.
 *
 * Returns:
 *    SERVREG_QDI_NOTIF_HANDLE : handle to the sr notif node
 * =====================================================================  */
SERVREG_QDI_NOTIF_HANDLE servreg_get_qdi_notif_node(SERVREG_MON_HANDLE sr_mon_handle, SERVREG_QDI_NOTIF_HANDLE * servreg_notif_list_head);

/** =====================================================================
 * Function:
 *     servreg_create_qdi_notif_node
 *
 * Description:
 *     Creates a srnotif node with the given sr_mon_handle
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by just domain name.
 *     servreg_notif_list_head : Head pointer of the notifier handle list for the user PD.
 *
 * Returns:
 *    SERVREG_QDI_NOTIF_HANDLE : handle to the sr notif node
 * =====================================================================  */
SERVREG_QDI_NOTIF_HANDLE servreg_create_qdi_notif_node(SERVREG_MON_HANDLE sr_mon_handle, SERVREG_QDI_NOTIF_HANDLE * servreg_notif_list_head);

/** =====================================================================
 * Function:
 *     servreg_delete_qdi_notif_node
 *
 * Description:
 *     Deletes a sr notif node given the node to delete and sr notif handle
 *
 * Parameters:
 *     sr_qdi_notif_handle  : Handle to the notifier node to be deleted
 *     servreg_notif_list_head : Head pointer of the notifier handle list for the user PD.
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_delete_qdi_notif_node(SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle, SERVREG_QDI_NOTIF_HANDLE * servreg_notif_list_head);

/** =====================================================================
 * Function:
 *     servreg_delete_qdi_notif_list
 *
 * Description:
 *     Deletes a sr notif node given the sr notif head pointer and also de-registers and de-allocates the listeners attached
 *
 * Parameters:
 *     servreg_notif_list_head : Head pointer of the notifier handle list for the user PD.
 *     signal                  : qurt signal associated to that pd
 *     mask                   : qurt mask associated to that pd
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_delete_qdi_notif_list(SERVREG_QDI_NOTIF_HANDLE * servreg_notif_list_head, qurt_anysignal_t* signal, uint32_t mask);

/** =====================================================================
 * Function:
 *     servreg_get_qdi_notif_node_state_change
 *
 * Description:
 *     Check which notif node state has changed and return that notif node's handle
 *
 * Parameters:
 *     servreg_notif_list_head : Head pointer of the notifier handle list for the user PD.
 *
 * Returns:
 *    SERVREG_QDI_NOTIF_HANDLE : handle to the sr notif node
 * =====================================================================  */
SERVREG_QDI_NOTIF_HANDLE servreg_get_qdi_notif_node_state_change(SERVREG_QDI_NOTIF_HANDLE * servreg_notif_list_head);

/** =====================================================================
 * Function:
 *     servreg_get_qdi_notif_node_ack_set
 *
 * Description:
 *     Check which notif node whose ack was set
 *
 * Parameters:
 *     servreg_notif_register_list_head : Head pointer of the notifier handle list for the user PD.
 *
 * Returns:
 *    SERVREG_QDI_NOTIF_HANDLE : handle to the sr notif node
 * =====================================================================  */
SERVREG_QDI_NOTIF_HANDLE servreg_get_qdi_notif_node_ack_set(SERVREG_QDI_NOTIF_HANDLE * servreg_notif_register_list_head);

/** =====================================================================
 * Function:
 *     servreg_get_qdi_notif_register_node
 *
 * Description:
 *     Given the pointer to the head pointer return the notif node
 *
 * Parameters:
 *     servreg_notif_register_list_head : Head pointer of the notifier handle list for the user PD.
 *
 * Returns:
 *    SERVREG_QDI_NOTIF_HANDLE : handle to the sr notif node
 * =====================================================================  */
SERVREG_QDI_NOTIF_HANDLE servreg_get_qdi_notif_register_node(SERVREG_QDI_NOTIF_HANDLE * servreg_notif_register_list_head);

/** =====================================================================
 * Function:
 *     servreg_notif_get_curr_state
 *
 * Description:
 *     Get the state of the notifier node
 *
 * Parameters:
 *    SERVREG_QDI_NOTIF_HANDLE : handle to the sr notif node
 *
 * Returns:
 *    Check enum SERVREG_SERVICE_STATE for different possible service states
 * =====================================================================  */
SERVREG_SERVICE_STATE servreg_notif_get_curr_state(SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle);

/** =====================================================================
 * Function:
 *     servreg_notif_get_transaction_id
 *
 * Description:
 *     Get the transaction id of the notifier node
 *
 * Parameters:
 *    SERVREG_QDI_NOTIF_HANDLE : handle to the sr notif node
 *
 * Returns:
 *    transaction id
 * =====================================================================  */
int32_t servreg_notif_get_transaction_id(SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle);

/** =====================================================================
 * Function:
 *     servreg_notif_update_handle
 *
 * Description:
 *     Update the notif handle with the state and transaction id from the user pd
 *
 * Parameters:
 *    SERVREG_QDI_NOTIF_HANDLE : handle to the sr notif node
 *
 * Returns:
 *    Check enum SERVREG_SERVICE_STATE for different possible service states
 * =====================================================================  */
SERVREG_RESULT servreg_notif_update_handle(SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle, SERVREG_SERVICE_STATE state, int32_t transaction_id);

/** =====================================================================
 * Function:
 *     servreg_notif_set_user_ack
 *
 * Description:
 *     Set the ack for the notif handle
 *
 * Parameters:
 *    SERVREG_QDI_NOTIF_HANDLE : handle to the sr notif node
 *
 * Returns:
 *    Check enum SERVREG_SERVICE_STATE for different possible service states
 * =====================================================================  */
SERVREG_RESULT servreg_notif_set_user_ack(SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle);

/** =====================================================================
 * Function:
 *     servreg_notif_get_mon_handle
 *
 * Description:
 *     Get the monitor handle of the notifier node
 *
 * Parameters:
 *    SERVREG_QDI_NOTIF_HANDLE : handle to the sr notif node
 *
 * Returns:
 *    Check enum SERVREG_SERVICE_STATE for different possible service states
 * =====================================================================  */
SERVREG_MON_HANDLE servreg_notif_get_mon_handle(SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle);

/** =====================================================================
 * Function:
 *     servreg_get_qdi_notif_next_node
 *
 * Description:
 *     Get the next notifier node
 *
 * Parameters:
 *    SERVREG_QDI_NOTIF_HANDLE : handle to the sr notif node
 *
 * Returns:
 *    Check enum SERVREG_SERVICE_STATE for different possible service states
 * =====================================================================  */
SERVREG_QDI_NOTIF_HANDLE servreg_get_qdi_notif_next_node(SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle);

#if defined(__cplusplus)
}
#endif

#endif
