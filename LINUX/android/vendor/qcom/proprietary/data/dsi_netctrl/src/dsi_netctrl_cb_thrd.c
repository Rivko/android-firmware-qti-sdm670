/*!
  @file
  dsi_netctrl_cb_thrd.c

  @brief
  implements dsi_netctrl callback processing thread

*/

/*===========================================================================

  Copyright (c) 2010-2015 Qualcomm Technologies, Inc. All Rights Reserved

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

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:  $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/08/10   js      created

===========================================================================*/
#include "dsi_netctrli.h"
#include "dsi_netctrl_platform.h"
#include "dsi_netctrl_cb_thrd.h"
#include "dsi_netctrl_mni_cb.h"
#include "dsi_netctrl_netmgr.h"
#include "ds_cmdq.h"
#include "qdi.h"

/*===========================================================================
                    LOCAL DATA STRUCTURES
===========================================================================*/

/* global queue to hold dsi_netctrl_cb commands */
ds_cmdq_info_t dsi_netctrl_cb_cmdq;

/* if we accumulate more than these many commands
 * ds_cmdq will print a warning msg */
#define DSI_NETCTRL_CB_MAX_CMDS 20

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
void dsi_netctrl_cb_cmd_free (ds_cmd_t * cmd, void * data)
{
  dsi_netctrl_cb_cmd_t * cmd_buf = NULL;

  DSI_LOG_VERBOSE("%s", "dsi_netctrl_cb_cmd_free ENTRY");

  if (NULL == data ||
      NULL == cmd)
  {
    DSI_LOG_FATAL("%s", "*** memory corruption : rcvd invalid data ***");
    DSI_LOG_DEBUG("%s", "dsi_netctrl_cb_cmd_free EXIT");
    return;
  }

  cmd_buf = (dsi_netctrl_cb_cmd_t *)data;
  /* verify self-reference */
  if (cmd != &cmd_buf->cmd)
  {
    DSI_LOG_FATAL("%s", "*** memory corruption : rcvd invalid data ***");
    DSI_LOG_DEBUG("%s", "dsi_netctrl_cb_cmd_free EXIT");
    return;
  }

  /* release mem */  
  dsi_free(cmd_buf);

  DSI_LOG_VERBOSE("%s", "dsi_netctrl_cb_cmd_free EXIT");
  return;
}

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
void dsi_netctrl_cb_cmd_exec (struct ds_cmd_s * cmd, void * data)
{
  int ret = DSI_ERROR;
  int reti = DSI_SUCCESS;
  dsi_netctrl_cb_cmd_t * cmd_buf = NULL;

  DSI_LOG_VERBOSE("%s", "dsi_netctrl_cb_cmd_exec: ENTRY");

  do
  {
    if (NULL == cmd ||
        NULL == data)
    {
      DSI_LOG_FATAL("%s", "*** memory corruption : rcvd invalid data ***");
      break;
    }

    cmd_buf = (dsi_netctrl_cb_cmd_t *)data;
    /* verify self-refernce */
    if (&cmd_buf->cmd != cmd)
    {
      DSI_LOG_FATAL("%s", "*** memory corruption : rcvd invalid data ***");
      break;
    }

    reti = DSI_SUCCESS;
    switch(cmd_buf->cmd_data.type)
    {
    case DSI_NETCTRL_CB_CMD_QMI_WDS_IND:
      dsi_process_wds_ind
        (
          cmd_buf->cmd_data.data_union.wds_ind.wds_hndl,
          cmd_buf->cmd_data.data_union.wds_ind.qmi_service_id,
          cmd_buf->cmd_data.data_union.wds_ind.user_data,
          cmd_buf->cmd_data.data_union.wds_ind.ind_id,
          &cmd_buf->cmd_data.data_union.wds_ind.ind_data
          );
      break;
    case DSI_NETCTRL_CB_CMD_QMI_ASYNC_RSP:
      dsi_process_async_wds_rsp
        (
          cmd_buf->cmd_data.data_union.async_rsp.user_handle,
          cmd_buf->cmd_data.data_union.async_rsp.qmi_service_id,
          cmd_buf->cmd_data.data_union.async_rsp.sys_err_code,
          cmd_buf->cmd_data.data_union.async_rsp.qmi_err_code,
          cmd_buf->cmd_data.data_union.async_rsp.user_data,
          cmd_buf->cmd_data.data_union.async_rsp.rsp_id,
          &cmd_buf->cmd_data.data_union.async_rsp.rsp_data
        );
      break;
    case DSI_NETCTRL_CB_CMD_QMI_QOS_IND:
      dsi_process_qos_ind
        (
          cmd_buf->cmd_data.data_union.qos_ind.qos_hndl,
          cmd_buf->cmd_data.data_union.qos_ind.qmi_service_id,
          cmd_buf->cmd_data.data_union.qos_ind.user_data,
          cmd_buf->cmd_data.data_union.qos_ind.ind_id,
          &cmd_buf->cmd_data.data_union.qos_ind.ind_data
        );
      break;
    case DSI_NETCTRL_CB_CMD_QMI_SYS_IND:
      dsi_process_qmi_sys_ind
        (
          cmd_buf->cmd_data.data_union.qmi_sys_ind.event_id,
          cmd_buf->cmd_data.data_union.qmi_sys_ind.user_data
        );
      break;
    case DSI_NETCTRL_CB_CMD_NETMGR:
      dsi_process_netmgr_ev
        (
          cmd_buf->cmd_data.data_union.netmgr.event,
          &cmd_buf->cmd_data.data_union.netmgr.info,
          cmd_buf->cmd_data.data_union.netmgr.data
        );
      break;
    default:
      DSI_LOG_FATAL("%s", "*** memory corruption: rcvd invalid data ***");
      reti = DSI_ERROR;
      break;
    }
    if (DSI_ERROR == reti)
    {
      break;
    }

    ret = DSI_SUCCESS;
  } while (0);

  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_VERBOSE("%s", "dsi_netctrl_cb_cmd_exec: EXIT with suc");
  }
  else
  {
    DSI_LOG_VERBOSE("%s", "dsi_netctrl_cb_cmd_exec: EXIT with err");
  }

}

/*===========================================================================
  FUNCTION:  dsi_qmi_wds_cmd_cb
===========================================================================*/
/*!
    @brief
    callback function registered for asynchronous qmi wds commands
    currently used for
    start_nw_if
    stop_nw_if
    This function will post a command to dsi_netctrl_cb thread for
    later processing.

    @return
    none
*/
/*=========================================================================*/
void dsi_qmi_wds_cmd_cb
(
  int                           user_handle,     /* QDI client ID  */
  qmi_service_id_type           service_id,      /* QMI service ID         */
  int                           sys_err_code,    /* QMI Msg Lib error      */
  int                           qmi_err_code,    /* QMI error              */
  void                         *user_data,       /* Callback context       */
  int                           rsp_id,          /* QMI Msg Lib txn ID     */
  qdi_wds_async_rsp_data_type  *rsp_data         /* QMI Msg Lib txn data   */
)
{
  int ret = DSI_ERROR;
  dsi_netctrl_cb_cmd_t * cmd_buf = NULL;

  DSI_LOG_VERBOSE( "%s", ">>>dsi_qmi_wds_cmd_cb: ENTRY" );

  do
  {
    DSI_LOG_VERBOSE(">>>qmi wds cmd_cb: user_handle=0x%x", user_handle);
    DSI_LOG_VERBOSE(">>>qmi wds cmd_cb: service_id=%d", (int)service_id);
    DSI_LOG_VERBOSE(">>>qmi wds cmd_cb: sys_err_code=%d", sys_err_code);
    DSI_LOG_VERBOSE(">>>qmi wds cmd_cb: qmi_err_code=%d", qmi_err_code);
    DSI_LOG_VERBOSE(">>>qmi wds cmd_cb: rsp_id=%d", rsp_id);
    DSI_LOG_VERBOSE(">>>qmi wds cmd_cb: rsp_data=%p", rsp_data);

    if (NULL == rsp_data)
    {
      DSI_LOG_FATAL("%s", ">>>*** rcvd NULL rsp_data ***");
      break;
    }

    cmd_buf = (dsi_netctrl_cb_cmd_t *)dsi_malloc(sizeof(dsi_netctrl_cb_cmd_t));
    if (NULL == cmd_buf)
    {
      DSI_LOG_ERROR("%s",">>>malloc failed for dsi_netctrl_cb_cmd_t");
      break;
    }

    /* set parameters in our internal structure  */
    cmd_buf->cmd_data.data_union.async_rsp.user_handle = user_handle;
    cmd_buf->cmd_data.data_union.async_rsp.qmi_service_id = service_id;
    cmd_buf->cmd_data.data_union.async_rsp.sys_err_code = sys_err_code;
    cmd_buf->cmd_data.data_union.async_rsp.qmi_err_code = qmi_err_code;
    cmd_buf->cmd_data.data_union.async_rsp.user_data = user_data;
    cmd_buf->cmd_data.data_union.async_rsp.rsp_id = rsp_id;
    /* there are no embedded pointers inside rsp_data structure, so
     * memcpy should be enough to copy everything */
    memcpy(&(cmd_buf->cmd_data.data_union.async_rsp.rsp_data), 
           rsp_data,
           sizeof(cmd_buf->cmd_data.data_union.async_rsp.rsp_data));

    /* set broad category to discriminate data, at the end
       dsc_cmd_q is going to call the execute_f with data */
    cmd_buf->cmd_data.type = DSI_NETCTRL_CB_CMD_QMI_ASYNC_RSP;

    /* prepare ds_cmd_t required by ds_cmdq */
    cmd_buf->cmd.execute_f = dsi_netctrl_cb_cmd_exec;
    cmd_buf->cmd.free_f = dsi_netctrl_cb_cmd_free;
    /* self pointer. this will be freed later */
    cmd_buf->cmd.data = cmd_buf;

    /* post command to global dsi_netctrl_cb queue */
    DSI_LOG_VERBOSE(">>>posting cmd [%p] to dsi_netctrl_cb queue",
                  &cmd_buf->cmd);
    ds_cmdq_enq(&dsi_netctrl_cb_cmdq, &cmd_buf->cmd);

    ret = DSI_SUCCESS;
  } while (0);

  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_VERBOSE( "%s", ">>>dsi_qmi_wds_cmd_cb: EXIT with suc" );
  }
  else
  {
    DSI_LOG_VERBOSE( "%s", ">>>dsi_qmi_wds_cmd_cb: EXIT with err" );
  }

}

/*===========================================================================
  FUNCTION:  dsi_qmi_wds_ind_cb
===========================================================================*/
/*!
    @brief
    callback function registered for wds indications.
    This function will post a command to separate dsi_netctrl_cb thread
    for later processing.

    @return
    none
*/
/*=========================================================================*/
void dsi_qmi_wds_ind_cb
(
  qmi_client_type                   wds_hndl,
  qmi_service_id_type               sid,
  void                             *user_data,
  int                               ind_id,
  dsi_qmi_wds_indication_data_type *ind_data
)
{
  int ret = DSI_ERROR;
  dsi_netctrl_cb_cmd_t * cmd_buf = NULL;

  int count;
  int reti = DSI_SUCCESS;

  DSI_LOG_VERBOSE("%s", ">>>dsi_qmi_wds_ind_cb: ENTRY");

  do
  {
    if (NULL == ind_data)
    {
      DSI_LOG_FATAL("%s", "*** rcvd invalid ind_data ***");
      break;
    }

    cmd_buf = (dsi_netctrl_cb_cmd_t *)dsi_malloc(sizeof(dsi_netctrl_cb_cmd_t));
    if (NULL == cmd_buf)
    {
      DSI_LOG_FATAL("%s","*** malloc failed for dsi_netctrl_cb cmd ***");
      break;
    }

    DSI_LOG_VERBOSE(">>>received wds_ind with wds_hndl [%p] "
                    "sid [%d] user_data [%p] ind_id [%d] ind_data [%p]",
                    wds_hndl, sid, user_data, ind_id, ind_data);
    /* set parameters in our internal structure  */
    cmd_buf->cmd_data.data_union.wds_ind.wds_hndl = wds_hndl;
    cmd_buf->cmd_data.data_union.wds_ind.qmi_service_id = sid;
    cmd_buf->cmd_data.data_union.wds_ind.user_data = user_data;
    cmd_buf->cmd_data.data_union.wds_ind.ind_id = ind_id;

    switch(ind_id)
    {
      case QMI_WDS_EMBMS_TMGI_ACTIVATE_IND_V01:
        {
          DSI_LOG_VERBOSE("%s", "process QMI_WDS_EMBMS_TMGI_ACTIVATE_IND_V01");

          memcpy(&(cmd_buf->cmd_data.data_union.wds_ind.ind_data.embms_activate_status),
                 &(ind_data->embms_activate_status),
                 sizeof(ind_data->embms_activate_status));
        }
        break;

      case QMI_WDS_EMBMS_TMGI_DEACTIVATE_IND_V01:
        {
          DSI_LOG_VERBOSE("%s", "process QMI_WDS_EMBMS_TMGI_DEACTIVATE_IND_V01");

          memcpy(&(cmd_buf->cmd_data.data_union.wds_ind.ind_data.embms_deactivate_status),
                 &(ind_data->embms_deactivate_status),
                 sizeof(ind_data->embms_deactivate_status));
        }
        break;

      case QMI_WDS_EMBMS_TMGI_ACT_DEACT_IND_V01:
        {
          DSI_LOG_VERBOSE("%s", "process QMI_WDS_EMBMS_TMGI_ACT_DEACT_IND_V01");

          memcpy(&(cmd_buf->cmd_data.data_union.wds_ind.ind_data.embms_act_deact_status),
                 &(ind_data->embms_act_deact_status),
                 sizeof(ind_data->embms_act_deact_status));
        }
        break;

      case QMI_WDS_EMBMS_TMGI_LIST_IND_V01:
        {
          DSI_LOG_VERBOSE("%s", "process QMI_WDS_EMBMS_TMGI_LIST_IND_V01");

          memcpy(&(cmd_buf->cmd_data.data_union.wds_ind.ind_data.embms_list),
                 &(ind_data->embms_list),
                 sizeof(ind_data->embms_list));
        }
        break;

      case QMI_WDS_EMBMS_CONTENT_DESC_CONTROL_IND_V01:
        {
          DSI_LOG_VERBOSE("%s", "process QMI_WDS_EMBMS_CONTENT_DESC_CONTROL_IND_V01");

          memcpy(&(cmd_buf->cmd_data.data_union.wds_ind.ind_data.embms_content_desc_control),
                 &(ind_data->embms_content_desc_control),
                 sizeof(ind_data->embms_content_desc_control));
        }
        break;

      case QMI_WDS_EMBMS_SAI_LIST_IND_V01:
        {
          DSI_LOG_VERBOSE("%s", "process QMI_WDS_EMBMS_SAI_LIST_IND_V01");
          memcpy(&(cmd_buf->cmd_data.data_union.wds_ind.ind_data.sai_list),
                 &(ind_data->sai_list),
                 sizeof((cmd_buf->cmd_data.data_union.wds_ind.ind_data.sai_list)));
        }
        break;

      default:
        {
          /* there are no embedded pointers inside ind_data structure, so
           * memcpy should be enough to copy everything */
          memcpy(&(cmd_buf->cmd_data.data_union.wds_ind.ind_data),
                 ind_data,
                 sizeof(cmd_buf->cmd_data.data_union.wds_ind.ind_data));
        }
        break;
    }

    /* set broad category to discriminate data, at the end
       dsc_cmd_q is going to call the execute_f with data */
    cmd_buf->cmd_data.type = DSI_NETCTRL_CB_CMD_QMI_WDS_IND;

    /* prepare ds_cmd_t required by ds_cmdq */
    cmd_buf->cmd.execute_f = dsi_netctrl_cb_cmd_exec;
    cmd_buf->cmd.free_f = dsi_netctrl_cb_cmd_free;
    /* self pointer. this will be freed later */
    cmd_buf->cmd.data = cmd_buf;

    /* post command to global dsi_netctrl_cb queue */
    DSI_LOG_VERBOSE(">>>posting cmd [%p] to dsi_netctrl_cb queue",
                  &cmd_buf->cmd);
    ds_cmdq_enq(&dsi_netctrl_cb_cmdq, &cmd_buf->cmd);

    if(DSI_SUCCESS == reti)
    {
      ret = DSI_SUCCESS;
    }
    else
    {
      break;
    }
  } while (0);

  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_VERBOSE("%s", ">>>dsi_qmi_wds_ind_cb: EXIT with suc");
  }
  else
  {
    DSI_LOG_VERBOSE("%s", ">>>dsi_qmi_wds_ind_cb: EXIT with err");
  }

}


/*===========================================================================
  FUNCTION:  dsi_netmgr_cb
===========================================================================*/
/*!
    @brief
    callback function registered with netmgr

    @return
    void
*/
/*=========================================================================*/
void dsi_netmgr_cb
(
  netmgr_nl_events_t event,
  netmgr_nl_event_info_t * info,
  void * data
)
{
  int ret = DSI_ERROR;
  dsi_netctrl_cb_cmd_t * cmd_buf = NULL;

  DSI_LOG_VERBOSE("%s", ">>>dsi_netmgr_cb ENTRY");

  do
  {
    ret = DSI_ERROR;

    if (NULL == info)
    {
      DSI_LOG_FATAL("%s","*** NULL info rcvd ***");
      break;
    }

    cmd_buf = (dsi_netctrl_cb_cmd_t *)dsi_malloc(sizeof(dsi_netctrl_cb_cmd_t));
    if (NULL == cmd_buf)
    {
      DSI_LOG_FATAL("%s","*** malloc failed for dsi_netctrl_cb cmd ***");
      break;
    }

    DSI_LOG_VERBOSE(">>>received netmgr_cb with event [%d] "
                  "info [%p] data [%p]", (int)event, info, data);
    DSI_LOG_VERBOSE(">>>found info->link = [%d]", info->link);
                  
    /* set parameters in our internal structure  */
    cmd_buf->cmd_data.data_union.netmgr.event = event;
    /* there are no embedded pointers inside info structure, so
     * memcpy should be enough to copy everything */
    memcpy(&(cmd_buf->cmd_data.data_union.netmgr.info),
           info, 
           sizeof(cmd_buf->cmd_data.data_union.netmgr.info));
    cmd_buf->cmd_data.data_union.netmgr.data = data;

    /* set broad category to discriminate data, at the end
       dsc_cmd_q is going to call the execute_f with data */
    cmd_buf->cmd_data.type = DSI_NETCTRL_CB_CMD_NETMGR;

    /* prepare ds_cmd_t required by ds_cmdq */
    cmd_buf->cmd.execute_f = dsi_netctrl_cb_cmd_exec;
    cmd_buf->cmd.free_f = dsi_netctrl_cb_cmd_free;
    /* self pointer. this will be freed later */
    cmd_buf->cmd.data = cmd_buf;

    /* post command to global dsi_netctrl_cb queue */
    DSI_LOG_VERBOSE(">>>posting cmd [%p] to dsi_netctrl_cb queue",
                  &cmd_buf->cmd);
    ds_cmdq_enq(&dsi_netctrl_cb_cmdq, &cmd_buf->cmd);

    ret = DSI_SUCCESS;
  } while(0);

  if (ret == DSI_ERROR)
  {
    DSI_LOG_VERBOSE("%s", ">>>dsi_netmgr_cb EXIT with err");
  }
  else
  {
    DSI_LOG_VERBOSE("%s", ">>>dsi_netmgr_cb EXIT with suc");
  }

}

/*===========================================================================
  FUNCTION:  dsi_netctrl_cb_deinit
===========================================================================*/
/*!
    @brief
    This function must be called at init time

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
void dsi_netctrl_cb_deinit()
{
  DSI_LOG_DEBUG("%s", "dsi_netctrl_cb_deinit ENTRY");
  if (0 != ds_cmdq_deinit(&dsi_netctrl_cb_cmdq))
  {
    DSI_LOG_ERROR("%s","*** could not deinit dsi_netctrl_cb_cmdq ***");
  }
  DSI_LOG_DEBUG("%s", "dsi_netctrl_cb_deinit EXIT");
}

/*===========================================================================
  FUNCTION:  dsi_netctrl_cb_init
===========================================================================*/
/*!
    @brief
    This function must be called at init time

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
int dsi_netctrl_cb_init()
{
  int rc;
  int ret = DSI_SUCCESS;

  DSI_LOG_DEBUG("%s", "dsi_netctrl_cb_init ENTRY");
  /* init ds_cmdq queue */
  rc = ds_cmdq_init(&dsi_netctrl_cb_cmdq, DSI_NETCTRL_CB_MAX_CMDS);
  if (0 != rc)
  {
    DSI_LOG_FATAL("*** ds_cmdq_init failed with err [%d] ***", rc);
    ret = DSI_ERROR;
  }

  DSI_LOG_DEBUG("%s", "dsi_netctrl_cb_init EXIT");
  return ret;
}
