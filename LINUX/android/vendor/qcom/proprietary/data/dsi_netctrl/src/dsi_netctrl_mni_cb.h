/*!
  @file
  dsi_netctrl_mni_cb.h

  @brief
  implements QMI callback events

*/

/*===========================================================================

  Copyright (c) 2010,2014-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:  $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/19/10   js      created

===========================================================================*/
#include "qdi.h"

#ifndef _DSI_NETCTRL_CB_
#define _DSI_NETCTRL_CB_
/*===========================================================================
                    QMI cmd callback
===========================================================================*/
/* async command callback. currently used for
 *  start_nw_if
 *  stop_nw_if
*/
extern void dsi_process_async_wds_rsp
(
  int                           user_handle,
  qmi_service_id_type           service_id,
  int                           sys_err_code,
  int                           qmi_err_code,
  void                         *user_data,
  int                           rsp_id,
  qdi_wds_async_rsp_data_type  *rsp_data
);

/*===========================================================================
                   QMI WDS IND CALLBACK
===========================================================================*/
void dsi_process_wds_ind
(
  qmi_client_type     wds_hndl,
  qmi_service_id_type sid,
  void               *user_data,
  int                 ind_id,
  dsi_qmi_wds_indication_data_type * ind_data
);

/*===========================================================================
                   QMI SYS IND CALLBACK
===========================================================================*/
void dsi_process_qmi_sys_ind
(
  dsi_qmi_sys_event_type event_id,
  void *user_data
);

#endif /* _DSI_NETCTRL_CB_ */
