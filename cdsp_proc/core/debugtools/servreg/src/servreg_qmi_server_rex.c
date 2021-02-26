/*
#============================================================================
#  Name:
#    servreg_qmi_notifier_server_rex.c
#
#  Description:
#    Service Registry notifier file for root image. This module serves as the end-point
#    of communication via QMI.
#
# Copyright (c) 2015-2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
#============================================================================
*/
#include "stdarg.h"
#include "stdlib.h"
#include <stringl/stringl.h>
#include "comdef.h"             /* Definitions for byte, word, etc.     */
#include "err.h"
#include "rex.h"
#include "qmi_csi.h"
#include "qmi_csi_target_ext.h"
#include "rcinit.h"

#include "servreg_internal.h"
#include "servreg_locator.h"
#include "servreg_localdb.h"
#include "servreg_monitor.h"
#include "servreg_notifier.h"
#include "servreg_qmi_notifier_server.h"
#include "servreg_qmi_notifier_client.h"

#define SERVREG_NOTIF_STATE_CHANGE_MASK                   0x2
#define SERVREG_QMI_NOTIF_SERVER_WAIT_MASK                0x20

rex_tcb_type  *servreg_qmi_ind_signal_rex;
extern qmi_csi_service_handle servreg_qmi_server_init(qmi_csi_os_params *os_params);

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
SERVREG_RESULT sr_qmi_s_register_proxy_listener(SERVREG_MON_HANDLE sr_mon_handle)
{
   SERVREG_MON_SIGREX sigex;
   sigex.signal = servreg_qmi_ind_signal_rex;
   sigex.mask = SERVREG_NOTIF_STATE_CHANGE_MASK;
   return servreg_register_listener(sr_mon_handle, SERVREG_SIGEX_TYPE_SIGREX, &sigex);
}

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
SERVREG_RESULT sr_qmi_s_deregister_proxy_listener(SERVREG_MON_HANDLE sr_mon_handle)
{
   SERVREG_MON_SIGREX sigex;
   sigex.signal = servreg_qmi_ind_signal_rex;
   sigex.mask = SERVREG_NOTIF_STATE_CHANGE_MASK;
   return servreg_deregister_listener(sr_mon_handle, SERVREG_SIGEX_TYPE_SIGREX, &sigex);
}

/** =====================================================================
 * Thread: REX
 *     servreg_qmi_server_req_task
 *
 * Description:
 *     Service Registry QMI notifier server task
 *
 * Parameters:
 *    param: Task init parameter
 *
 * Returns:
 *     None
 * =====================================================================  */
static void servreg_qmi_server_req_task(uint32 handle)
{
   uint32_t q_mask;
   qmi_csi_os_params os_params;
   qmi_csi_service_handle servreg_qmi_server_handle;

   servreg_qmi_ind_signal_rex = rex_self();

   os_params.tcb = rex_self();
   os_params.sig = SERVREG_QMI_NOTIF_SERVER_WAIT_MASK;

   servreg_qmi_server_handle = servreg_qmi_server_init(&os_params);
   servreg_qmi_notif_client_init();

   /* Block for start signal */
   rcinit_handshake_startup();    

   /* Task forever loop */
   for (;;)
   {
      q_mask = rex_wait(SERVREG_NOTIF_STATE_CHANGE_MASK | SERVREG_QMI_NOTIF_SERVER_WAIT_MASK );

      if (q_mask & SERVREG_NOTIF_STATE_CHANGE_MASK)
      {
         rex_clr_sigs(rex_self(), SERVREG_NOTIF_STATE_CHANGE_MASK);
         servreg_send_qmi_ind_msg_state_change();
      }

      if (q_mask & SERVREG_QMI_NOTIF_SERVER_WAIT_MASK)
      {
         rex_clr_sigs(rex_self(), SERVREG_QMI_NOTIF_SERVER_WAIT_MASK);
         qmi_csi_handle_event(servreg_qmi_server_handle, &os_params);
      }
   }
}

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
void servreg_qmi_notif_server_init(void)
{
   static rex_tcb_type req_tcb;
   static rex_task_attributes_type req_attr;
   static char req_stack[SERVREG_QMI_NOTIF_SERVER_TASK_STACK];

   servreg_qmi_notif_server_node_internal_init();

   /* Rex task to handle QMI req and ind messages */
   rex_task_attr_init(&req_attr);
   rex_task_attr_set_stack_addr(&req_attr, (rex_stack_pointer_type)req_stack);
   rex_task_attr_set_stack_size(&req_attr, SERVREG_QMI_NOTIF_SERVER_TASK_STACK);
   rex_task_attr_set_prio(&req_attr, 10);
   rex_task_attr_set_entry_func(&req_attr, servreg_qmi_server_req_task, NULL);
   rex_task_attr_set_initially_suspended(&req_attr, FALSE);
   rex_task_attr_set_task_name(&req_attr, SERVREG_QMI_NOTIF_REQ_SERVER_TASK_NAME);
   rex_common_def_task(&req_tcb, &req_attr);

   return;
}
