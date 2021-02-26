#ifndef SERVREG_QMI_NOTIFIER_CLIENT_H
#define SERVREG_QMI_NOTIFIER_CLIENT_H
/*
#============================================================================
#  Name:
#    servreg_qmi_notifier_client.h
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
 *     servreg_get_qmi_notif_client_node
 *
 * Description:
 *     Checks if a SRM node already exists with the given name. If it does 
 *     exists it returns a pointer to that srm node.
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by just domain name.
 *
 * Returns:
 *    SERVREG_QDI_NOTIF_HANDLE : handle to the sr notif node
 * =====================================================================  */
SERVREG_QDI_NOTIF_HANDLE servreg_get_qmi_notif_client_node(SERVREG_MON_HANDLE sr_mon_handle);

/** =====================================================================
 * Function:
 *     servreg_create_qmi_notif_client_node
 *
 * Description:
 *     Creates a srnotif node with the given sr_mon_handle
 *
 * Parameters:
 *     sr_mon_handle : Handle to an existing service state which is mapped by just domain name.
 *
 * Returns:
 *    SERVREG_QDI_NOTIF_HANDLE : handle to the sr notif node
 * =====================================================================  */
SERVREG_QDI_NOTIF_HANDLE servreg_create_qmi_notif_client_node(SERVREG_MON_HANDLE sr_mon_handle);

/** =====================================================================
 * Function:
 *     servreg_delete_qmi_notif_client_node
 *
 * Description:
 *     Deletes a sr notif node given the sr notif handle
 *
 * Parameters:
 *     sr_qmi_notif_client_handle  : Handle to the notifier node to be deleted
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_delete_qmi_notif_client_node(SERVREG_QDI_NOTIF_HANDLE sr_qmi_notif_client_handle);

/** =====================================================================
 * Function:
 *     servreg_qmi_notif_client_init
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
void servreg_qmi_notif_client_init(void);

#if defined(__cplusplus)
}
#endif

#endif
