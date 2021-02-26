#ifndef SERVREG_QDI_NOTIFIER_USER_H
#define SERVREG_QDI_NOTIFIER_USER_H
/*
#============================================================================
#  Name:
#    servreg_qdi_notifier_user.h
#
#  Description:
#    Common Service Registry notifier header file for user images only
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

#define QDI_SERVREG_NAME_LENGTH_V01 64 //QMI_SERVREG_LOC_NAME_LENGTH_V01

typedef uint32_t SERVREG_REMOTE_HANDLE;

typedef enum 
{
   SERVREG_USER_REGISTER        = 0,
   SERVREG_ROOT_REGISTER        =1,
}SERVREG_QDI_TYPE;

/** =====================================================================
 * Function:
 *     servreg_get_qdi_notif_node
 *
 * Description:
 *     Checks if a notifier node already exists with the sr monitor handle. If it does 
 *     exists it returns a pointer to that notif node.
 *
 * Parameters:
 *     sr_type       : root client registing in the user pd or user client registering in the root pd
 *     sr_mon_handle : Handle to an existing service state which is mapped by domain + service 
 *                     or just domain name
 *
 * Returns:
 *    SERVREG_QDI_NOTIF_HANDLE : handle to the notifier node
 * =====================================================================  */
SERVREG_QDI_NOTIF_HANDLE servreg_get_qdi_notif_node(SERVREG_QDI_TYPE sr_type, SERVREG_MON_HANDLE sr_mon_handle);

/** =====================================================================
 * Function:
 *     servreg_create_qdi_notif_node
 *
 * Description:
 *     Creates a sr notif node with the given sr_mon_handle and sr_remote_handle
 *
 * Parameters:
 *     sr_type          : root client registing in the user pd or user client registering in the root pd
 *     sr_mon_handle    : Handle to an existing service state which is mapped by domain + service 
 *                        or just domain name
 *     sr_remote_handle : Remote (root PD) notifier node address
 *
 * Returns:
 *    SERVREG_QDI_NOTIF_HANDLE : handle to the notifier node
 * =====================================================================  */
SERVREG_QDI_NOTIF_HANDLE servreg_create_qdi_notif_node(SERVREG_QDI_TYPE sr_type, SERVREG_MON_HANDLE sr_mon_handle, SERVREG_REMOTE_HANDLE sr_notif_remote_node);

/** =====================================================================
 * Function:
 *     servreg_delete_qdi_notif_node
 *
 * Description:
 *     Deletes a sr notif node given the sr notif handle
 *
 * Parameters:
 *     sr_type              : root client registing in the user pd or user client registering in the root pd
 *     sr_qdi_notif_handle  : Handle to the notifier node to be deleted
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_delete_qdi_notif_node(SERVREG_QDI_TYPE sr_type, SERVREG_QDI_NOTIF_HANDLE sr_qdi_notif_handle);

/** =====================================================================
 * Function:
 *     servreg_map_remote_handle
 *
 * Description:
 *     Given the remote notif handle i.e root pd notif node address, return
 *     the user pd's corresponding monitor handle
 *
 * Parameters:
 *     sr_type          : root client registing in the user pd or user client registering in the root pd
 *     sr_remote_handle : Remote (root PD) notifier node address
 *
 * Returns:
 *     SERVREG_MON_HANDLE :  Handle to an existing service state which is mapped by domain + service 
 *                           or just domain name
 * =====================================================================  */
SERVREG_MON_HANDLE servreg_map_remote_handle(SERVREG_QDI_TYPE sr_type, SERVREG_REMOTE_HANDLE sr_remote_handle);

/** =====================================================================
 * Function:
 *     servreg_register_root_listeners
 *
 * Description:
 *     Register the root clients for the user service if any
 *
 * Parameters:
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
void servreg_register_root_listeners(void);

#if defined(__cplusplus)
}
#endif

#endif
