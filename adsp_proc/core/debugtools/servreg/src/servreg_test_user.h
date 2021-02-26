#ifndef SERVREG_TEST_USER_H
#define SERVREG_TEST_USER_H
/*
#============================================================================
#  Name:
#    servreg_test_user.h 
#
#  Description:
#    Implements QDI layer for Service registry that goes to the root image
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
 *     servreg_test_case_trigger_local_state_change
 *
 * Description:
 *      This function tests if Service states can be changed and if clients
 *      get notified of the state change.
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
void servreg_test_case_trigger_local_state_change(void);

/** =====================================================================
 * Function:
 *     servreg_test_case_service_state_init
 *
 * Description:
 *      This function register a "SERVICE" with the SERVICE REGISTRY framework
 *      and marks it as UP
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
void servreg_test_case_service_state_init(void);

/** =====================================================================
 * Function:
 *     servreg_test_case_set_local_service_sync
 *
 * Description:
 *     Function sets the local servreg_test service as synchronous
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
void servreg_test_case_set_local_service_sync(void);

/** =====================================================================
 * Function:
 *     servreg_test_case_register_listener_local_service
 *
 * Description:
 *      This function tests registers a client as a listener for local service
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
void servreg_test_case_register_listener_local_service(void);

/** =====================================================================
 * Function:
 *     servreg_test_case_register_listener_remote_qdi_service
 *
 * Description:
 *      This function tests registers a client as a listener for remote service
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
void servreg_test_case_register_listener_remote_qdi_service(void);

/** =====================================================================
 * Function:
 *     servreg_test_case_register_listener_remote_qmi_service
 *
 * Description:
 *      This function tests registers a client as a listener for remote service
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
void servreg_test_case_register_listener_remote_qmi_service(void);

/** =====================================================================
 * Function:
 *     servreg_test_case_register_sync_notif
 *
 * Description:
 *     Function registers a callback function that gets called when the sync complete
 *     notification for that state is generated
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
void servreg_test_case_register_sync_notif(void);

/** =====================================================================
 * Function:
 *     servreg_test_case_inject_state_change
 *
 * Description:
 *     Function changes the state of its own pd.
 *     To be used in the qxdm injection command
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
void servreg_test_case_inject_state_change(void);

/** =====================================================================
 * Task:
 *     servreg_test_task
 *
 * Description:
 *     Test task that tests Service Registry API's
 *
 * Parameters:
 *
 * Returns:
 * =====================================================================  */
void servreg_test_task(dword param);

#if defined(__cplusplus)
}
#endif

#endif