/*!
  @file
  ssreq_svc_poll_client.c

  @brief
  Task to query the qmi service availability status for mem_share client.

*/

/*===========================================================================

  Copyright (c) 2015-2017 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/sys_m/src/ssreq_svc_poll_client.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/06/15   rks      Initial Version

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "stdio.h"
#include "stdlib.h"
#include "qmi_client.h"
#include "qmi_cci_target_ext.h"
#include "rcinit.h"
#include "subsystem_request_v01.h"
#include "qmi_ssreq_client.h"
#include "sys_m.h"
#include "tms_utils_msg.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define QMI_CLNT_WAIT_SIG      (0x1 << 5)
#define QMI_CLNT_TIMER_SIG     (0x1 << 6)
#define QMI_POLL_TIMEOUT_MS    10000  /* 10 secs */

qmi_idl_service_object_type   ssreq_service_object;
static boolean                       ssreq_client_init_status = FALSE;
qmi_client_type                      ssreq_client_handle;

/*===========================================================================

  FUNCTION:  get_ssreq_client_init_status

===========================================================================*/
/*!
    @brief
    Provides the status for ssreq qmi service polling.

    @detail
    Provides the status for SSREQ qmi service availability.

    @return
    get_ssreq_client_init_status variable value, which is updated in ssreq_qmi_poll_task().

*/
/*=========================================================================*/
boolean get_ssreq_client_init_status( void )
{
  TMS_MSG_HIGH_1("ssreq_client_init_status: %d", ssreq_client_init_status);
  return ssreq_client_init_status;
}

/*===========================================================================

  FUNCTION:  ssreq_qmi_poll_task

===========================================================================*/
/*!
    @brief
    Initialize SSREQ QMI client

    @return
    None.
*/
/*=========================================================================*/
void ssreq_svc_poll_task
(
  /* Unused parameter - required to match rex_task_func_type prototype */
  dword                           ignored    /*lint -esym(715,ignored) */

)
{
   rcinit_handshake_startup(); 
   unsigned int ser_instance_id = SSREQ_QMI_SERVICE_INSTANCE_APSS_V01 + SYSM_CHIP_ID * QMI_SSREQ_SERVICE_INSTANCE_OFFSET_V01;
   qmi_cci_os_signal_type        os_params;
   os_params.tcb = rex_self();
   os_params.sig = QMI_CLNT_WAIT_SIG;
   os_params.timer_sig = QMI_CLNT_TIMER_SIG;

   ssreq_service_object = ssreq_get_service_object_v01();
   if (!ssreq_service_object)
   {
      ssreq_client_init_status = FALSE;
      return;
   }

   /* Initialize the qmi client notifier */
   if(QMI_NO_ERR != qmi_client_init_instance( ssreq_service_object,
                                              ser_instance_id,
                                              ssreq_client_ind_cb,
                                              NULL,
                                              &os_params,
                                              QMI_POLL_TIMEOUT_MS,
                                              &ssreq_client_handle))
   {
       ssreq_client_init_status = FALSE;
       return;
   }

   ssreq_client_init_status = TRUE;
   return;   
}
