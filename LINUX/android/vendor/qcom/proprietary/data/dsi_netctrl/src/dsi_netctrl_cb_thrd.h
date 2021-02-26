/*!
  @file
  dsi_netctrl_cb_thrd.h

  @brief
  header for dsi_netctrl_cb_thread

*/

/*===========================================================================

  Copyright (c) 2010,2015 Qualcomm Technologies, Inc. All Rights Reserved

  Qualcomm Technologies Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

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
#include "qmi_qos_srvc.h"
#include "dsi_netctrl_qmi.h"
#include "ds_cmdq.h"
#include "qdi.h"
#include "netmgr.h"

#ifndef _DSI_NETCTRL_CB_THRD_
#define _DSI_NETCTRL_CB_THRD_

/* dsi_netctrl_cb cmd types : contains cmd type enums */
typedef enum dsi_netctrl_cb_cmd_type_e
{
  DSI_NETCTRL_CB_CMD_QMI_WDS_IND,
  DSI_NETCTRL_CB_CMD_QMI_QOS_IND,
  DSI_NETCTRL_CB_CMD_QMI_ASYNC_RSP,
  DSI_NETCTRL_CB_CMD_QMI_SYS_IND,
  DSI_NETCTRL_CB_CMD_NETMGR,
  DSI_NETCTRL_CB_CMD_MAX
} dsi_netctrl_cb_cmd_type_t;

/* dsi_netctrl_cb cmd : contains data for pkt_srvc_ind */
typedef struct dsi_netctrl_cb_wds_ind_data_s
{
  qmi_client_type wds_hndl;
  qmi_service_id_type qmi_service_id;
  void * user_data;
  int ind_id;
  dsi_qmi_wds_indication_data_type ind_data;
} dsi_netctrl_cb_wds_ind_data_t;

/* dsi_netctrl_cb cmd : contains data for start/stop_nw_async_rsp */
typedef struct dsi_netctrl_cb_async_rsp_s
{
  int user_handle;
  qmi_service_id_type qmi_service_id;
  int sys_err_code;
  int qmi_err_code;
  void * user_data;
  int rsp_id;
  qdi_wds_async_rsp_data_type rsp_data;
} dsi_netctrl_cb_async_rsp_t;

/* dsi_netctrl_cb cmd : contains data for qmi sys ind */
typedef struct dsi_netctrl_cb_qmi_sys_ind_data_s
{
  dsi_qmi_sys_event_type event_id;
  void *user_data;
} dsi_netctrl_cb_qmi_sys_ind_data_t;

/* dsi_netctrl_cb cmd : contains data for pkt_srvc_ind */
typedef struct dsi_netctrl_cb_qos_ind_data_s
{
  qmi_client_type qos_hndl;
  qmi_service_id_type qmi_service_id;
  void * user_data;
  int ind_id;
  qmi_qos_indication_data_type ind_data;
} dsi_netctrl_cb_qos_ind_data_t;

/* data for netmgr cb */
typedef struct dsi_netmgr_cb_s
{
  netmgr_nl_events_t event;
  netmgr_nl_event_info_t info;
  void * data;
} dsi_netmgr_cb_t;

/* global queue to hold dsi_netctrl_cb commands */
/* dsi_netctrl_cb cmd : contains data */
typedef struct dsi_netctrl_cb_cmd_data_s
{
  /* type is used to discriminate union */
  dsi_netctrl_cb_cmd_type_t type;
  union
  {
    dsi_netctrl_cb_qmi_sys_ind_data_t qmi_sys_ind;
    dsi_netctrl_cb_wds_ind_data_t wds_ind;
    dsi_netctrl_cb_qos_ind_data_t qos_ind;
    dsi_netctrl_cb_async_rsp_t async_rsp;
    dsi_netmgr_cb_t netmgr;
  } data_union;
} dsi_netctrl_cb_cmd_data_t;

/* dsi_netctrl_cb cmd : contains type and data */
typedef struct dsi_netctrl_cb_cmd_s
{
  ds_cmd_t cmd;
  dsi_netctrl_cb_cmd_data_t cmd_data;
} dsi_netctrl_cb_cmd_t;

extern ds_cmdq_info_t dsi_netctrl_cb_cmdq;
/*===========================================================================
                  QMI WDS service command callback
===========================================================================*/
/* async command callback. currently used for
 *  start_nw_if
 *  stop_nw_if
*/
extern void dsi_qmi_wds_cmd_cb
(
  int                           user_handle,     /* QDI client ID  */
  qmi_service_id_type           service_id,      /* QMI service ID         */
  int                           sys_err_code,    /* QMI Msg Lib error      */
  int                           qmi_err_code,    /* QMI error              */
  void                         *user_data,       /* Callback context       */
  int                           rsp_id,          /* QMI Msg Lib txn ID     */
  qdi_wds_async_rsp_data_type  *rsp_data         /* QMI Msg Lib txn data   */
);

/*===========================================================================
                  QMI WDS service event callback
===========================================================================*/
void dsi_qmi_wds_ind_cb
(
  qmi_client_type                   wds_hndl,
  qmi_service_id_type               sid,
  void                             *user_data,
  int                               ind_id,
  dsi_qmi_wds_indication_data_type *ind_data
);


/*===========================================================================
                   NETMGR CALLBACK
===========================================================================*/
void dsi_netmgr_cb
(
  netmgr_nl_events_t event,
  netmgr_nl_event_info_t * info,
  void * data
);

/*===========================================================================
  FUNCTION:  dsi_netctrl_cb_cmd_exec
===========================================================================*/
/*!
    @brief
    This function is registered as executive function in each command that
    is posted to dsi_netctrl_cb global queue.
    When called, this function further calls appropriate functions based
    on the command types.

    @return
    none
*/
/*=========================================================================*/
void dsi_netctrl_cb_cmd_exec
(
  struct ds_cmd_s * cmd,
  void * data
);

/*===========================================================================
  FUNCTION:  dsi_netctrl_cb_cmd_free
===========================================================================*/
/*!
    @brief
    releases memory for the dsi_netctrl_cb cmd

    @return
    none
*/
/*=========================================================================*/
void dsi_netctrl_cb_cmd_free
(
  ds_cmd_t * cmd,
  void * data
);
/*===========================================================================
                   INIT/DEINIT ROUTINES
===========================================================================*/
extern int dsi_netctrl_cb_init();
extern void dsi_netctrl_cb_deinit();

#endif /* _DSI_NETCTRL_CB_THRD_ */
