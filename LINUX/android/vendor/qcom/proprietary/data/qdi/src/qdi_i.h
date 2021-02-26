/*!
  @file
  qdi_i.h

  @brief
  This is an internal header file for QDI module. It declares
  data used internally by the QDI module.
*/

/*===========================================================================
  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

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

#include "ds_util.h"
#include "ds_list.h"
#include "ds_cmdq.h"

#include "dsi_netctrl.h"
#include "wireless_data_service_v01.h"
#include "qdi.h"

#ifndef _QDI_I_H_
#define _QDI_I_H_

/* Storage for QMI WDS indication messages */
typedef struct
{
  qmi_client_type               user_handle;
  qmi_service_id_type           service_id;
  void                          *user_data;
  int                            ind_id;
  qdi_qmi_wds_indication_data_type  ind_data;
} qdi_wds_ind_data_t;

/* Storage for QMI WDS async response messages */
typedef struct
{
  qmi_client_type              user_handle;
  qmi_service_id_type          service_id;
  int                          sys_err_code;
  int                          qmi_err_code;
  void                         *user_data;
  int                           rsp_id;
  qdi_qmi_wds_async_rsp_data_type  rsp_data;
} qdi_wds_async_rsp_t;

/* Type of data being stored in command buffer */
typedef enum
{
  QDI_CMDQ_CMD_TYPE_WDS_IND,
  QDI_CMDQ_CMD_TYPE_WDS_ASYNC_RESP
} qdi_cmdq_cmd_t;

/* Typedef for command data */
typedef struct
{
  qdi_cmdq_cmd_t            cmd;

  union
  {
    qdi_wds_ind_data_t      wds_ind;
    qdi_wds_async_rsp_t     async_resp;
  } data;
} qdi_cmdq_cmd_data_type;

/* Typedef for qdi command type */
typedef struct
{
  ds_cmd_t                ds_cmd;   /* Should be the first entry in the structure */
  qdi_cmdq_cmd_data_type  cmd_data;
} qdi_cmdq_cmd_type;

/* QDI Command Queue */
extern ds_cmdq_info_t  qdi_cmdq;

void qdi_process_wds_ind
(
  struct ds_cmd_s *ds_cmd,
  void            *data
);

void qdi_process_wds_async_cb
(
  struct ds_cmd_s *ds_cmd,
  void            *data
);

void qdi_cmdq_free
(
  struct ds_cmd_s *ds_cmd,
  void            *data
);

#endif
