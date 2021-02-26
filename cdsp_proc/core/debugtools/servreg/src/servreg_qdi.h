#ifndef SERVREG_QDI_H
#define SERVREG_QDI_H
/*
#============================================================================
#  Name:
#    servreg_qdi.h 
#
#  Description:
#    Implements QDI layer for Service registry that goes to the root image
#
# Copyright (c) 2015-2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
#============================================================================
*/
#include "stdlib.h"
#include "qurt_qdi_driver.h"   /* Including header only in this file */

#if defined(__cplusplus)
extern "C"
{
#endif

#define SERVREG_QDI_DEVICE_PATH_LEN                    32                             /* INTERNAL, Maximum Device Path Length */
#define SERVREG_QDI_DEVICE_NAME_LEN                    QURT_MAX_NAME_LEN              /* INTERNAL, Maximum Device Basename Length */
#define SERVREG_QDI_DEVICE                             "/dev/servnotif"               /* INTERNAL, Device Prefix */
#define SERVREG_QDI_MAX_PDS                            2
#define SERVREG_QDI_REG_USER_MASK_VALUE                0x1
#define SERVREG_QDI_REG_ROOT_MASK_VALUE                0x2
#define SERVREG_QDI_ACK_ROOT_MASK_VALUE                0x4

/* User registrants in root */
#define SERVREG_QDI_WORKER_WAIT                        (0+QDI_PRIVATE)
#define SERVREG_QDI_REG_REMOTE_LISTENER                (1+QDI_PRIVATE)
#define SERVREG_QDI_DEREG_REMOTE_LISTENER              (2+QDI_PRIVATE)
#define SERVREG_QDI_FREE_REMOTE_HANDLE                 (3+QDI_PRIVATE)
#define SERVREG_QDI_SET_REMOTE_ACK                     (4+QDI_PRIVATE)
#define SERVREG_QDI_GET_REMOTE_HANDLE                  (5+QDI_PRIVATE)
#define SERVREG_QDI_CREATE_QMI_ENTRY                   (6+QDI_PRIVATE)

/* Root registrants in user*/
#define SERVREG_QDI_REGISTER_NEW_HANDLE                (7+QDI_PRIVATE)
#define SERVREG_QDI_SET_STATE                          (8+QDI_PRIVATE)
#define SERVREG_QDI_UPDATE_HANDLE                      (9+QDI_PRIVATE)
#define SERVREG_QDI_REGISTER_ROOT_HANDLES              (10+QDI_PRIVATE)
#define SERVREG_QDI_GET_ROOT_ACK_HANDLE                (11+QDI_PRIVATE)

/** =====================================================================
 * Function:
 *     servreg_register_user_listener
 *
 * Description:
 *     Service level registration. Root client registering for a service in the user pd
 *
 * Parameters:
 *     service_name             : Service name that the client is interested in 
 *     sr_mon_handle            : Opaque handle to existing event
 *     sr_curr_remote_state     : State of the service in the user pd returned
 *     sr_remote_transaction_id : Transaction id of the service in the user pd returned
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_register_user_listener(SERVREG_NAME service_name, SERVREG_MON_HANDLE sr_mon_handle, uint32_t * sr_curr_remote_state, int32_t * sr_remote_transaction_id);

/** =====================================================================
 * Function:
 *     servreg_set_user_ack
 *
 * Description:
 *     Set the ack from root to user pd
 *
 * Parameters:
 *     service_name             : Service name that the client is interested in 
 *     sr_mon_handle            : Opaque handle to existing event
 *     sr_curr_remote_state     : State of the service
 *     sr_remote_transaction_id : Transaction id of the service
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_set_user_ack(SERVREG_NAME service_name, SERVREG_MON_HANDLE sr_mon_handle, uint32_t sr_curr_remote_state, int32_t sr_remote_transaction_id);

/** =====================================================================
 * Function:
 *     servreg_qdi_invoke
 *
 * Description: 
 *     This gives the canonical form for the arguments to a QDI
 *     driver invocation function.
 * 
 * Parameters:
 *     client_handle  : QDI handle which represents the client
 *                      which made this QDI request.  
 *
 *     qurt_qdi_obj_t : Points at the qdi_object_t structure
 *                      on which this QDI request is being made.
 *
 *     int method     : The integer QDI method which represents
 *                      the request type.
 *
 *     qurt_qdi_arg_t a1 to a3 :  The first three general purpose arguments
 *                                to the invocation function are passed in
 *                                these slots.
 *
 *     qurt_qdi_arg_t a4 to a9 :   Arguments beyond the first three are
 *                                 passed on the stack.
 * Returns:
 *     -1 for failure and 0 for success
 * =====================================================================  */
int servreg_qdi_invoke(int client_handle,
                       qurt_qdi_obj_t* obj,
                       int servnotif_qdi_method,
                       qurt_qdi_arg_t a1,
                       qurt_qdi_arg_t a2,
                       qurt_qdi_arg_t a3,
                       qurt_qdi_arg_t a4,
                       qurt_qdi_arg_t a5,
                       qurt_qdi_arg_t a6,
                       qurt_qdi_arg_t a7,
                       qurt_qdi_arg_t a8,
                       qurt_qdi_arg_t a9
                      );

/** =====================================================================
 * Function:
 *     servreg_qdi_init
 *
 * Description:
 *     Initialization function. Registers a QDI device with the generic QDI object
 *
 * Parameters:
 *     None
 * Returns:
 *     None
 * =====================================================================  */
void servreg_qdi_init(void);

#if defined(__cplusplus)
}
#endif

#endif
