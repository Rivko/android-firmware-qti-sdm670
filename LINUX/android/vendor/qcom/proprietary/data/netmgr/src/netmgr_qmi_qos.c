/******************************************************************************

                       N E T M G R _ Q M I _Q O S . C

******************************************************************************/

/******************************************************************************

  @file    netmgr_qmi_qos.c
  @brief   Network Manager QMI wireless data service helper

  DESCRIPTION
  Network Manager QMI wireless data service helper

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include "qmi_client.h"
#include "qmi_cci_target.h"
#include "qmi_idl_lib.h"
#include "qmi_cci_common.h"
#include "netmgr_exec.h"
#include "netmgr_main.h"
#include "qmi_client_instance_defs.h"
#include "qmi_platform.h"
#include "qmi_util.h"
#include "qmi_client_helper.h"
#include "netmgr_qmi_qos.h"
#include "netmgr_util.h"
#include "netmgr_kif.h"

#define NETMGR_QMI_QOS_EVENT_FLOW_INFO_TLV_ID  0x10

#define NETMGR_QMI_QOS_GET_GRNTD_QOS_INFO_MSG_ID  0x0025
#define NETMGR_QMI_QOS_GET_FILTER_PARAMS_MSG_ID   0x002C

/*===========================================================================
                     LOCAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

static qmi_cci_os_signal_type netmgr_qmi_qos_os_params;

extern int
qmi_qos_srvc_process_event_report_ind
(
  unsigned char                 *rx_buf,
  int                            rx_buf_len,
  netmgr_qos_event_report_type     *ind_data,
  short                          index
);

extern int
qmi_qos_process_get_granted_qos_resp
(
  unsigned char                  *msg,
  int                            msg_size,
  netmgr_qos_granted_info_rsp_type  *granted_info
);

extern int
qmi_qos_process_get_filter_params_resp
(
  unsigned char                  *msg,
  int                            msg_size,
  netmgr_qos_granted_info_rsp_type  *granted_info
);

extern int
qmi_qos_process_get_primary_granted_qos_req
(
  unsigned char  **msg,
  int            *msg_size,
  unsigned long  qos_identifier
);

extern int
qmi_qos_process_get_secondary_granted_qos_req
(
  unsigned char  **msg,
  int            *msg_size,
  unsigned long  qos_identifier
);

extern int
qmi_qos_process_get_filter_params_req
(
  unsigned char  **msg,
  int            *msg_size,
  unsigned long  qos_identifier
);

extern void
qmi_qos_srvc_process_nested_event_report_ind

(
  unsigned char                 *rx_msg_buf,
  int                           rx_msg_len,
  netmgr_qos_event_report_type     *ind_data
);

int
qmi_qos_srvc_extract_return_code
(
  unsigned char          *msg,
  int                    msg_size,
  qmi_response_type_v01  *resp
);

int netmgr_qmi_qos_process_filter_info
(
  netmgr_qos_filter_req_type    *netmgr_qos_filter_data,
  qos_filter_rule_resp_ind_type_v01 *qmi_filter_data
)
{
  int local_param_mask = 0;

  NETMGR_LOG_FUNC_ENTRY;

  if (!netmgr_qos_filter_data || !qmi_filter_data)
  {
    netmgr_log_err("%s(): invalid input params!\n", __func__);
    return NETMGR_FAILURE;
  }

  netmgr_qos_filter_data->filter_desc.protocol =
            qmi_filter_data->fltr_info.xport_info.xport_protocol;

  netmgr_qos_filter_data->ip_version =
            qmi_filter_data->fltr_info.ip_info.ip_version;

  netmgr_qos_filter_data->filter_desc.precedence =
            qmi_filter_data->filter_precedence;


  if(netmgr_qos_filter_data->filter_desc.protocol == NETMGR_QOS_TRANS_PROT_TCP)
  {
    netmgr_qos_filter_data->filter_desc.tcp_src_ports.start_port =
            qmi_filter_data->fltr_info.xport_info.tcp_info.src_port_info.port;
    netmgr_qos_filter_data->filter_desc.tcp_src_ports.range =
            qmi_filter_data->fltr_info.xport_info.tcp_info.src_port_info.range;
    netmgr_qos_filter_data->filter_desc.tcp_dest_ports.start_port =
            qmi_filter_data->fltr_info.xport_info.tcp_info.dest_port_info.port;
    netmgr_qos_filter_data->filter_desc.tcp_dest_ports.range =
            qmi_filter_data->fltr_info.xport_info.tcp_info.dest_port_info.range;
  }
  else if (netmgr_qos_filter_data->filter_desc.protocol ==
                                             NETMGR_QOS_TRANS_PROT_UDP)
  {
    netmgr_qos_filter_data->filter_desc.udp_src_ports.start_port =
            qmi_filter_data->fltr_info.xport_info.udp_info.src_port_info.port;
    netmgr_qos_filter_data->filter_desc.udp_src_ports.range =
            qmi_filter_data->fltr_info.xport_info.udp_info.src_port_info.range;
    netmgr_qos_filter_data->filter_desc.udp_dest_ports.start_port =
            qmi_filter_data->fltr_info.xport_info.udp_info.dest_port_info.port;
    netmgr_qos_filter_data->filter_desc.udp_dest_ports.range =
            qmi_filter_data->fltr_info.xport_info.udp_info.dest_port_info.range;
  }

  if(qmi_filter_data->fltr_info.ip_info.ip_version ==
                                            QOS_IP_FAMILY_IPV4_V01 )
  {
    netmgr_qos_filter_data->filter_desc.tos.tos_value =
            qmi_filter_data->fltr_info.ip_info.v4_info.tos.val;
    netmgr_qos_filter_data->filter_desc.tos.tos_mask =
            qmi_filter_data->fltr_info.ip_info.v4_info.tos.mask;

    netmgr_qos_filter_data->filter_desc.src_addr.ipv4_ip_addr =
            qmi_filter_data->fltr_info.ip_info.v4_info.src_addr.ipv4_addr;
    netmgr_qos_filter_data->filter_desc.src_addr.ipv4_subnet_mask =
            qmi_filter_data->fltr_info.ip_info.v4_info.src_addr.subnet_mask;
    netmgr_qos_filter_data->filter_desc.dest_addr.ipv4_ip_addr =
            qmi_filter_data->fltr_info.ip_info.v4_info.dest_addr.ipv4_addr;
    netmgr_qos_filter_data->filter_desc.dest_addr.ipv4_subnet_mask =
            qmi_filter_data->fltr_info.ip_info.v4_info.dest_addr.subnet_mask;

    if(qmi_filter_data->fltr_info.ip_info.v4_info.valid_params &
                                       QMI_QOS_IPV4_FILTER_MASK_SRC_ADDR_V01)
    {
      local_param_mask |= NETMGR_QOS_FILTER_PARAM_SRC_ADDR;
    }

    if(qmi_filter_data->fltr_info.ip_info.v4_info.valid_params &
                                       QMI_QOS_IPV4_FILTER_MASK_DEST_ADDR_V01)
    {
      local_param_mask |= NETMGR_QOS_FILTER_PARAM_DEST_ADDR;
    }

    if(qmi_filter_data->fltr_info.xport_info.xport_protocol &
                                       QOS_PROTO_TCP_V01)
    {
      local_param_mask |= NETMGR_QOS_FILTER_PARAM_TRANS_PROTOCOL;

      if(qmi_filter_data->fltr_info.xport_info.tcp_info.valid_params &
                                   QMI_QOS_PORT_INFO_FILTER_MASK_SRC_PORT_V01)
      {
        local_param_mask |= NETMGR_QOS_FILTER_PARAM_TCP_SRC_PORTS;
      }

      if(qmi_filter_data->fltr_info.xport_info.tcp_info.valid_params &
                                  QMI_QOS_PORT_INFO_FILTER_MASK_DEST_PORT_V01)
      {
        local_param_mask |= NETMGR_QOS_FILTER_PARAM_TCP_DEST_PORTS;
      }
    }

    if(qmi_filter_data->fltr_info.xport_info.xport_protocol & QOS_PROTO_UDP_V01)
    {
      local_param_mask |= NETMGR_QOS_FILTER_PARAM_TRANS_PROTOCOL;

      if(qmi_filter_data->fltr_info.xport_info.udp_info.valid_params &
                                    QMI_QOS_PORT_INFO_FILTER_MASK_SRC_PORT_V01)
      {
        local_param_mask |= NETMGR_QOS_FILTER_PARAM_UDP_SRC_PORTS;
      }

      if(qmi_filter_data->fltr_info.xport_info.udp_info.valid_params &
                                   QMI_QOS_PORT_INFO_FILTER_MASK_DEST_PORT_V01)
      {
        local_param_mask |= NETMGR_QOS_FILTER_PARAM_UDP_DEST_PORTS;
      }
    }

    netmgr_qos_filter_data->filter_desc.param_mask = local_param_mask;

  }
  else if(qmi_filter_data->fltr_info.ip_info.ip_version == QOS_IP_FAMILY_IPV6_V01)
  {
    netmgr_qos_filter_data->filter_desc.ipv6_flow_label =
          qmi_filter_data->fltr_info.ip_info.v6_info.flow_label;

    memcpy(&netmgr_qos_filter_data->filter_desc.ipv6_src_addr.ipv6_ip_addr,
            &qmi_filter_data->fltr_info.ip_info.v6_info.src_addr.ipv6_address,
            QMI_QOS_IPV6_ADDR_LEN_V01);
    netmgr_qos_filter_data->filter_desc.ipv6_src_addr.ipv6_filter_prefix_len =
            qmi_filter_data->fltr_info.ip_info.v6_info.src_addr.prefix_len;

    memcpy(&netmgr_qos_filter_data->filter_desc.ipv6_dest_addr.ipv6_ip_addr,
            &qmi_filter_data->fltr_info.ip_info.v6_info.dest_addr.ipv6_address,
           QMI_QOS_IPV6_ADDR_LEN_V01);
    netmgr_qos_filter_data->filter_desc.ipv6_dest_addr.ipv6_filter_prefix_len =
           qmi_filter_data->fltr_info.ip_info.v6_info.dest_addr.prefix_len;

    netmgr_qos_filter_data->filter_desc.ipv6_traffic_class.traffic_class_value =
           qmi_filter_data->fltr_info.ip_info.v6_info.trf_cls.val;
    netmgr_qos_filter_data->filter_desc.ipv6_traffic_class.traffic_class_mask =
           qmi_filter_data->fltr_info.ip_info.v6_info.trf_cls.mask;

    if(qmi_filter_data->fltr_info.ip_info.v6_info.valid_params &
                                  QMI_QOS_IPV6_FILTER_MASK_SRC_ADDR_V01)
    {
      local_param_mask |= NETMGR_QOS_FILTER_PARAM_IPV6_SRC_ADDR;
    }

    if(qmi_filter_data->fltr_info.ip_info.v6_info.valid_params &
                                  QMI_QOS_IPV6_FILTER_MASK_DEST_ADDR_V01)
    {
      local_param_mask |= NETMGR_QOS_FILTER_PARAM_IPV6_DEST_ADDR;
    }

    if(qmi_filter_data->fltr_info.xport_info.xport_protocol &
                                  QOS_PROTO_TCP_V01)
    {
      local_param_mask |= NETMGR_QOS_FILTER_PARAM_TRANS_PROTOCOL;

      if(qmi_filter_data->fltr_info.xport_info.tcp_info.valid_params &
                                  QMI_QOS_PORT_INFO_FILTER_MASK_SRC_PORT_V01)
      local_param_mask |= NETMGR_QOS_FILTER_PARAM_TCP_SRC_PORTS;

      if(qmi_filter_data->fltr_info.xport_info.tcp_info.valid_params &
                                  QMI_QOS_PORT_INFO_FILTER_MASK_DEST_PORT_V01)
      local_param_mask |= NETMGR_QOS_FILTER_PARAM_TCP_DEST_PORTS;
    }

    if(qmi_filter_data->fltr_info.xport_info.xport_protocol & QOS_PROTO_UDP_V01)
    {

      local_param_mask |= NETMGR_QOS_FILTER_PARAM_TRANS_PROTOCOL;

      if(qmi_filter_data->fltr_info.xport_info.udp_info.valid_params &
                                     QMI_QOS_PORT_INFO_FILTER_MASK_SRC_PORT_V01)
      local_param_mask |= NETMGR_QOS_FILTER_PARAM_UDP_SRC_PORTS;

      if(qmi_filter_data->fltr_info.xport_info.udp_info.valid_params &
                                     QMI_QOS_PORT_INFO_FILTER_MASK_DEST_PORT_V01)
      local_param_mask |= NETMGR_QOS_FILTER_PARAM_UDP_DEST_PORTS;
    }

    netmgr_qos_filter_data->filter_desc.param_mask = local_param_mask;

  }

  NETMGR_LOG_FUNC_EXIT;

  return NETMGR_SUCCESS;

}


int netmgr_qmi_qos_process_event_report_ind
(
  netmgr_qos_event_report_type  *ind_data,
  qos_global_qos_flow_ind_msg_v01 qmi_ind
)
{

  uint8 index;
  int flow_id = 0;
  int local_param_mask = 0;
  netmgr_qos_filter_req_type   *filter_data = NULL;
  netmgr_qos_event_report_flow_info_type *flow_info = NULL;

  NETMGR_LOG_FUNC_ENTRY;

  if(ind_data == NULL)
  {
    netmgr_log_err("Indication data is NULL");
    return NETMGR_FAILURE;
  }

  flow_info = &ind_data->flow_info[flow_id];

  ind_data->param_mask = NETMGR_QOS_EVENT_REPORT_GLOBAL_FLOW_INFO_PARAM;
  ind_data->num_flows = 1;
  flow_info->flow_type = (char)qmi_ind.flow_type;
  flow_info->qos_flow_state.qos_identifier =
                                       qmi_ind.qos_flow_state.qos_id;
  flow_info->qos_flow_state.new_flow =
                                  (char)qmi_ind.qos_flow_state.new_flow;
  flow_info->qos_flow_state.report_flow_state_chng =
                                     qmi_ind.qos_flow_state.state_change;

  flow_info->param_mask =
                      NETMGR_QOS_EVENT_REPORT_FLOW_INFO_FLOW_STATE_PARAM;

  if(qmi_ind.flow_type_valid)
  {
    flow_info->param_mask |=
                NETMGR_QOS_EVENT_REPORT_FLOW_INFO_FLOW_TYPE_PARAM;
  }

  flow_info->tx_granted_flow_data_is_valid =
                                            qmi_ind.tx_qos_flow_granted_valid;
  flow_info->rx_granted_flow_data_is_valid =
                                            qmi_ind.rx_qos_flow_granted_valid;

  netmgr_log_low("flow_id[%d] of flow_type[%d] is in %d state\n" ,
              qmi_ind.qos_flow_state.qos_id,qmi_ind.flow_type,
              qmi_ind.qos_flow_state.state_change);

  if(qmi_ind.tx_qos_flow_granted_valid)
  {
    flow_info->tx_granted_flow_data.qos_flow_granted.cdma_flow_desc.profile_id =
                        qmi_ind.tx_qos_flow_granted.ip_flow_3gpp2_profile_id;
    flow_info->tx_granted_flow_data.qos_flow_granted.cdma_flow_desc.param_mask =
                        qmi_ind.tx_qos_flow_granted.flow_valid_params;
    flow_info->tx_granted_flow_data.qos_flow_granted.umts_flow_desc.param_mask =
                        qmi_ind.tx_qos_flow_granted.flow_valid_params;
    flow_info->tx_granted_flow_data.qos_flow_granted.umts_flow_desc.data_rate.max_rate =
                        qmi_ind.tx_qos_flow_granted.data_rate_min_max.data_rate_max; //kbps
    flow_info->tx_granted_flow_data.qos_flow_granted.umts_flow_desc.data_rate.guaranteed_rate =
                        qmi_ind.tx_qos_flow_granted.data_rate_min_max.guaranteed_rate; //kbps
    flow_info->tx_granted_flow_data.qos_flow_granted.umts_flow_desc.traffic_class =
                        qmi_ind.tx_qos_flow_granted.ip_flow_trf_cls;
  }
  if(qmi_ind.rx_qos_flow_granted_valid)
  {
    flow_info->rx_granted_flow_data.qos_flow_granted.cdma_flow_desc.profile_id =
                        qmi_ind.rx_qos_flow_granted.ip_flow_3gpp2_profile_id;
    flow_info->rx_granted_flow_data.qos_flow_granted.cdma_flow_desc.param_mask =
                        qmi_ind.rx_qos_flow_granted.flow_valid_params;
    flow_info->rx_granted_flow_data.qos_flow_granted.umts_flow_desc.param_mask =
                        qmi_ind.rx_qos_flow_granted.flow_valid_params;
    flow_info->rx_granted_flow_data.qos_flow_granted.umts_flow_desc.data_rate.max_rate =
                        qmi_ind.rx_qos_flow_granted.data_rate_min_max.data_rate_max; //kbps
    flow_info->rx_granted_flow_data.qos_flow_granted.umts_flow_desc.data_rate.guaranteed_rate =
                        qmi_ind.rx_qos_flow_granted.data_rate_min_max.guaranteed_rate; //kbps
    flow_info->rx_granted_flow_data.qos_flow_granted.umts_flow_desc.traffic_class =
                        qmi_ind.rx_qos_flow_granted.ip_flow_trf_cls;
  }

  netmgr_log_low("tx_qos_flow[%d] rx_qos_flow[%d]\n",
              qmi_ind.tx_qos_filters_len,qmi_ind.rx_qos_filters_len);

  netmgr_log_low("tx_qos_filter_len[%d] rx_qos_filter_len[%d]\n",
              qmi_ind.tx_qos_filters_len,qmi_ind.rx_qos_filters_len);

  if(qmi_ind.tx_qos_filters_len > 0 && qmi_ind.tx_qos_filters_valid)
  {
    flow_info->tx_filter_count = qmi_ind.tx_qos_filters_len;

    for(index=0;index < qmi_ind.tx_qos_filters_len ;index++)
    {
      flow_info->tx_granted_filter_data[index].filter_index =
                        qmi_ind.tx_qos_filters[index].filter_id;
      filter_data = &flow_info->tx_granted_filter_data[index].qos_filter;
      netmgr_qmi_qos_process_filter_info(filter_data,&qmi_ind.tx_qos_filters[index]);
    }
  }

  if(qmi_ind.rx_qos_filters_len > 0 && qmi_ind.rx_qos_filters_valid)
  {
    flow_info->rx_filter_count = qmi_ind.rx_qos_filters_len;

    for(index=0;index < qmi_ind.rx_qos_filters_len ;index++)
    {
      flow_info->rx_granted_filter_data[index].filter_index =
                         qmi_ind.rx_qos_filters[index].filter_id;
      filter_data = &flow_info->rx_granted_filter_data[index].qos_filter;
      netmgr_qmi_qos_process_filter_info(filter_data,&qmi_ind.rx_qos_filters[index]);
    }
  }

  NETMGR_LOG_FUNC_EXIT;

  return NETMGR_SUCCESS;
}
/*===========================================================================
  FUNCTION  netmgr_qmi_qos_ind_cb
===========================================================================*/
/*!
@brief
 Main callback function regisgterd during client init. It posts a command to
 do the required processing in the Command Thread context.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

static
void netmgr_qmi_qos_ind_cb
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  void                           *ind_buf,
  unsigned int                   ind_buf_len,
  void                           *ind_cb_data
)
{
  int link;
  netmgr_exec_cmd_t * cmd = NULL;
  qmi_client_error_type   qmi_err = QMI_INTERNAL_ERR;
  int dest_len = 0;
  int i;

  (void) user_handle;

  NETMGR_LOG_FUNC_ENTRY;

  NETMGR_ASSERT(ind_buf != NULL);

  /* Get link id from user data ptr */
  link = (int)((uintptr_t) ind_cb_data);

  /* Verify link id */
  NETMGR_ASSERT(netmgr_kif_verify_link(link) == NETMGR_SUCCESS);

  /* Allocate a command object */
  cmd = netmgr_exec_get_cmd();
  NETMGR_ASSERT(cmd);

/*#if defined(FEATURE_DS_LINUX_ANDROID) && !defined(NETMGR_OFFTARGET)
  netmgr_util_acquire_wakelock();
/#endif*/

  netmgr_log_med("%s(): received indication=%d on link=%d\n", __func__, msg_id, link);

  switch (msg_id)
  {
    case QMI_QOS_EVENT_REPORT_IND_V01:
    {
      if(!netmgr_main_get_qos_via_idl_enabled())
      {
        unsigned char *ind = (unsigned char *)ind_buf;
        int ind_len = (int) ind_buf_len;
        netmgr_qos_event_report_type  *ind_data = &cmd->data.info.qmi_msg.data.qos_ind.info.event_report;

        qmi_qos_srvc_process_nested_event_report_ind(ind,
                                                     ind_len,
                                                     ind_data);

        /* flow_state enumeration starts from 0 in QOS_SET_EVENT_REPORT while it
           starts from 1 in  GLOBAL_QOS_FLOW indication. Hence Shifting the
           flow_state_change value to align with newer definitions
           (netmgr_global_flow_state_change) */

        for(i =0; i< ind_data->num_flows; i++)
        {
          ind_data->flow_info[i].qos_flow_state.report_flow_state_chng -= 1;
        }
      }
      else
      {
        goto bail;
      }
      break;
    }

    case QMI_QOS_GLOBAL_QOS_FLOW_IND_V01:
    {
      qos_global_qos_flow_ind_msg_v01 qmi_ind;
      memset(&qmi_ind,0,sizeof(qos_global_qos_flow_ind_msg_v01));
      qmi_err = qmi_client_message_decode(user_handle,
                                      QMI_IDL_INDICATION,
                                      msg_id,
                                      ind_buf,
                                      ind_buf_len,
                                      &qmi_ind,
                                      sizeof(qos_global_qos_flow_ind_msg_v01));
      if (QMI_NO_ERR != qmi_err)
      {
        goto bail;
      }
      else
      {
        netmgr_qos_event_report_type  *netmgr_ind_data =
              &cmd->data.info.qmi_msg.data.qos_ind.info.event_report;
        netmgr_qmi_qos_process_event_report_ind(netmgr_ind_data,qmi_ind);
      }
    }
    break;

    default:
      netmgr_log_err("received unsupported indication type %d\n", msg_id);
      goto bail;
  }


  cmd->data.type                     = NETMGR_QMI_MSG_CMD;
  cmd->data.link                     = link;
  cmd->data.info.qmi_msg.type        = NETMGR_QMI_QOS_EVENT_REPORT_IND_CMD;
  cmd->data.info.qmi_msg.data.qos_ind.link   = link;
  cmd->data.info.qmi_msg.data.qos_ind.ind_id = msg_id;

  /* Post command for processing in the command thread context */
  if( NETMGR_SUCCESS != netmgr_exec_put_cmd( cmd ) ) {
    NETMGR_ABORT("%s(): failed to put commmand\n", __func__);
    goto bail;

  }

  NETMGR_LOG_FUNC_EXIT;
  return;

bail:
  if (NULL != cmd)
  {
    netmgr_exec_release_cmd( cmd );
  }
  NETMGR_LOG_FUNC_EXIT;
}

/*===========================================================================
  FUNCTION  netmgr_qmi_qos_get_primary_granted_qos
===========================================================================*/
/*!
@brief
  Return the QoS parameters for a particular QOS flow.

@return
  int - NETMGR_SUCCESS on successful operation,
        NETMGR_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
static int
netmgr_qmi_qos_get_filter_params
(
  int                            link,
  qmi_client_type                clnt_hndl,
  uint32                         flow_id,
  netmgr_qos_granted_info_rsp_type  *granted_info
)
{
  int rc = QMI_INTERNAL_ERR;
  int ret = NETMGR_FAILURE;
  unsigned char msg[QMI_MAX_STD_MSG_SIZE], *tmp_msg_ptr;
  int           msg_size;
  unsigned int actual_resp_len = 0;
  qmi_response_type_v01  resp;

  NETMGR_LOG_FUNC_ENTRY;

  if (NETMGR_QMI_CLIENT_INVALID == clnt_hndl)
  {
    netmgr_log_err("%s(): invalid clnt_hndl on link=%d!\n",
                   __func__, link);
    goto bail;
  }

  tmp_msg_ptr = msg;
  msg_size = sizeof(msg);

  if (QMI_NO_ERR != qmi_qos_process_get_filter_params_req(&tmp_msg_ptr,
                                                          &msg_size,
                                                          flow_id))
  {
    goto bail;
  }

  rc = qmi_client_send_raw_msg_sync(clnt_hndl,
                                    NETMGR_QMI_QOS_GET_FILTER_PARAMS_MSG_ID,
                                    msg,
                                    sizeof(msg)-msg_size,
                                    msg,
                                    sizeof(msg),
                                    &actual_resp_len,
                                    NETMGR_QMI_TIMEOUT);

  if (rc != QMI_NO_ERR || actual_resp_len == 0)
  {
    netmgr_log_err("%s(): failed for link=%d with rc=%d!\n",
                   __func__, link, rc);
    goto bail;
  }

  if (QMI_NO_ERR != qmi_qos_srvc_extract_return_code(msg,
                                                     actual_resp_len,
                                                     &resp))
  {
    netmgr_log_err("%s(): failed to extract return code for link=%d!\n",
                   __func__, link);
    goto bail;
  }
  else if (QMI_RESULT_SUCCESS_V01 != resp.result)
  {
    netmgr_log_err("%s(): get_filter_params req failed for link=%d with rc=%d!\n",
                   __func__, link, resp.error);
    goto bail;
  }

  rc = qmi_qos_process_get_filter_params_resp(msg,
                                              actual_resp_len,
                                              granted_info);

  if (QMI_NO_ERR != rc)
  {
    goto bail;
  }

  ret = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return ret;
}

/*===========================================================================
                            GLOBAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  netmgr_qmi_qos_init_client
===========================================================================*/
/*!
@brief
  Initialize QMI QoS handle

@return
  int - NETMGR_SUCCESS on successful operation,
        NETMGR_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
int netmgr_qmi_qos_init_client
(
  int                      link,
  const char               *dev_str,
  qos_ip_family_enum_v01   ip_family,
  qmi_client_type          *clnt_hndl,
  qmi_response_type_v01    *err
)
{
  qmi_connection_id_type conn_id;
  qmi_client_error_type rc = QMI_INTERNAL_ERR;

  if (!clnt_hndl || !dev_str || !err)
  {
    netmgr_log_err("%s(): invalid input params!\n", __func__);
    return NETMGR_FAILURE;
  }

  NETMGR_LOG_FUNC_ENTRY;

  memset(&netmgr_qmi_qos_os_params, 0, sizeof(qmi_cci_os_signal_type));

  *clnt_hndl = NETMGR_QMI_CLIENT_INVALID;

  err->result = QMI_RESULT_SUCCESS_V01;
  err->error = QMI_ERR_NONE_V01;

  /* Create client using QCCI libary - this is a blocking call. */
  rc = qmi_client_qos_init_instance(dev_str,
                                    qos_get_service_object_v01(),
                                    QMI_CLIENT_INSTANCE_ANY,
                                    netmgr_qmi_qos_ind_cb,
                                    (void *)(uintptr_t)link,
                                    &netmgr_qmi_qos_os_params,
                                    NETMGR_QMI_TIMEOUT,
                                    clnt_hndl);

  if (QMI_NO_ERR != rc)
  {
    netmgr_log_err("%s(): failed on qmi_client_qos_init_instance with rc=%d!\n", __func__, rc);
    err->result = QMI_RESULT_FAILURE_V01;
    err->error = QMI_ERR_INVALID_SERVICE_TYPE_V01;
    goto bail;
  }
  else
  {
    qos_set_client_ip_pref_req_msg_v01 req;
    qos_set_client_ip_pref_resp_msg_v01 resp;
    qos_indication_register_req_msg_v01 request;
    qos_indication_register_resp_msg_v01 response;

    memset (&req, 0, sizeof(req));
    memset (&resp, 0, sizeof(resp));
    memset (&req, 0, sizeof(request));
    memset (&resp, 0, sizeof(response));

    req.ip_preference = ip_family;

    request.report_global_qos_flows = TRUE;
    request.report_global_qos_flows_valid = TRUE;

    /* Set the ip preference */
    rc = qmi_client_send_msg_sync(*clnt_hndl,
                                  QMI_QOS_SET_CLIENT_IP_PREF_REQ_V01,
                                  (void *)&req,
                                  sizeof(req),
                                  (void*)&resp,
                                  sizeof(resp),
                                  NETMGR_QMI_TIMEOUT);

    if (rc != QMI_NO_ERR || QMI_RESULT_SUCCESS_V01 != resp.resp.result)
    {
      netmgr_log_err("%s(): failed to set ip_family=%d with rc=%d result=%d err=%d!\n",
                     __func__, ip_family, rc, resp.resp.result, resp.resp.error);
      *err = resp.resp;
      goto bail;
    }

    rc = qmi_client_send_msg_sync(*clnt_hndl,
                                  QMI_QOS_INDICATION_REGISTER_REQ_V01,
                                  (void *)&request,
                                  sizeof(request),
                                  (void*)&response,
                                  sizeof(response),
                                  NETMGR_QMI_TIMEOUT);

    if (rc != QMI_NO_ERR || QMI_RESULT_SUCCESS_V01 != resp.resp.result)
    {
      netmgr_log_err("%s(): failed to set ip_family=%d with rc=%d result=%d err=%d!\n",
                     __func__, ip_family, rc, resp.resp.result, resp.resp.error);
      *err = resp.resp;
      goto bail;
    }
  }

  NETMGR_LOG_FUNC_EXIT;

  return NETMGR_SUCCESS;

bail:
  if (NETMGR_QMI_CLIENT_INVALID != *clnt_hndl)
  {
    qmi_client_release(*clnt_hndl);
  }

  NETMGR_LOG_FUNC_EXIT;

  return NETMGR_FAILURE;
}

/*===========================================================================
  FUNCTION  netmgr_qmi_qos_release_client
===========================================================================*/
/*!
@brief
  Release QMI QOS handle

@return
  int - NETMGR_SUCCESS on successful operation,
        NETMGR_FAILURE on failure

*/
/*=========================================================================*/
int netmgr_qmi_qos_release_client
(
  int                      link,
  qmi_client_type          clnt_hndl
)
{
  int rc = QMI_INTERNAL_ERR;
  int ret = NETMGR_FAILURE;

  NETMGR_LOG_FUNC_ENTRY;

  if (NETMGR_QMI_CLIENT_INVALID == clnt_hndl)
  {
    netmgr_log_err("%s(): invalid clnt_hndl on link=%d!\n",
                   __func__, link);
    goto bail;
  }

  rc = qmi_client_release(clnt_hndl);

  if (QMI_NO_ERR != rc)
  {
    netmgr_log_err("%s(): failed to release clnt_hndl=%p on link=%d rc=%d!\n",
                   __func__, clnt_hndl, link, rc);
    goto bail;
  }

  ret = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return ret;
}

/*===========================================================================
  FUNCTION  netmgr_qmi_qos_bind_subscription
===========================================================================*/
/*!
@brief
  Bind the subscription to QOS_DONT_CARE_SUBS_V01

@return
  int - NETMGR_SUCCESS on successful operation,
        NETMGR_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
int netmgr_qmi_qos_bind_subscription
(
  int                      link,
  qmi_client_type          clnt_hndl
)
{
  int rc = QMI_INTERNAL_ERR;
  int ret = NETMGR_FAILURE;
  qos_bind_subscription_req_msg_v01 req;
  qos_bind_subscription_resp_msg_v01 resp;

  NETMGR_LOG_FUNC_ENTRY;

  if (NETMGR_QMI_CLIENT_INVALID == clnt_hndl)
  {
    netmgr_log_err("%s(): invalid clnt_hndl on link=%d!\n",
                   __func__, link);
    goto bail;
  }

  memset (&req, 0, sizeof(req));
  memset (&resp, 0, sizeof(resp));

  req.bind_subs = 0x00FF; /* Don't care subs */

  /* Send ext ip config registration */
  rc = qmi_client_send_msg_sync(clnt_hndl,
                                QMI_QOS_BIND_SUBSCRIPTION_REQ_V01,
                                (void *)&req,
                                sizeof(req),
                                (void*)&resp,
                                sizeof(resp),
                                NETMGR_QMI_TIMEOUT);

  if (rc != QMI_NO_ERR || QMI_RESULT_SUCCESS_V01 != resp.resp.result)
  {
    netmgr_log_err("%s(): failed for link=%d with rc=%d result=%d err=%d!\n",
                   __func__, link, rc, resp.resp.result, resp.resp.error);
    goto bail;
  }

  ret = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return ret;
}

/*===========================================================================
  FUNCTION  netmgr_qmi_qos_enable_global_flow_report
===========================================================================*/
/*!
@brief
  Enable global flow reporting via QMI_QOS_SET_EVENT_REPORT

@return
  int - NETMGR_SUCCESS on successful operation,
        NETMGR_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
int netmgr_qmi_qos_enable_global_flow_report
(
  int                      link,
  qmi_client_type          clnt_hndl
)
{
  int rc = QMI_INTERNAL_ERR;
  int ret = NETMGR_FAILURE;
  qos_set_event_report_req_msg_v01 req;
  qos_set_event_report_resp_msg_v01 resp;

  NETMGR_LOG_FUNC_ENTRY;

  if (NETMGR_QMI_CLIENT_INVALID == clnt_hndl)
  {
    netmgr_log_err("%s(): invalid clnt_hndl on link=%d!\n",
                   __func__, link);
    goto bail;
  }

  memset (&req, 0, sizeof(req));
  memset (&resp, 0, sizeof(resp));

  req.global_flow_reporting_valid = TRUE;
  req.global_flow_reporting = TRUE;

  /* Enable global flow report */
  rc = qmi_client_send_msg_sync(clnt_hndl,
                                QMI_QOS_SET_EVENT_REPORT_REQ_V01,
                                (void *)&req,
                                sizeof(req),
                                (void*)&resp,
                                sizeof(resp),
                                NETMGR_QMI_TIMEOUT);

  if (rc != QMI_NO_ERR || QMI_RESULT_SUCCESS_V01 != resp.resp.result)
  {
    netmgr_log_err("%s(): failed for link=%d with rc=%d result=%d err=%d!\n",
                   __func__, link, rc, resp.resp.result, resp.resp.error);
    goto bail;
  }

  ret = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return ret;
}

int netmgr_qmi_qos_process_granted_qos
(
  netmgr_qos_granted_info_rsp_type  *granted_info,
  qos_get_qos_info_resp_msg_v01 *granted_qos
)
{
  uint8 index;
  netmgr_qos_filter_req_type *filter_data;

  NETMGR_LOG_FUNC_ENTRY;

  if (!granted_info || !granted_qos)
  {
    netmgr_log_err("%s(): invalid input params!\n", __func__);
    return NETMGR_FAILURE;
  }

  granted_info->rx_granted_flow_data_is_valid = granted_qos->rx_qos_flow_valid;
  granted_info->tx_granted_flow_data_is_valid = granted_qos->tx_qos_flow_valid;

  granted_info->rx_filter_count = granted_qos->rx_qos_filter_len;
  granted_info->tx_filter_count = granted_qos->tx_qos_filter_len;

  granted_info->tx_granted_flow_data.qos_flow_granted.cdma_flow_desc.param_mask =
                                 granted_qos->tx_qos_flow.flow_valid_params;

  granted_info->tx_granted_flow_data.qos_flow_granted.umts_flow_desc.param_mask =
                                 granted_qos->tx_qos_flow.flow_valid_params;

  granted_info->rx_granted_flow_data.qos_flow_granted.cdma_flow_desc.param_mask =
                                 granted_qos->rx_qos_flow.flow_valid_params;

  granted_info->rx_granted_flow_data.qos_flow_granted.umts_flow_desc.param_mask =
                                 granted_qos->rx_qos_flow.flow_valid_params;

  if(granted_qos->tx_qos_flow_valid)
  {
    granted_info->tx_granted_flow_data.qos_flow_granted.cdma_flow_desc.profile_id =
                        granted_qos->tx_qos_flow.ip_flow_3gpp2_profile_id;
    granted_info->tx_granted_flow_data.qos_flow_granted.umts_flow_desc.data_rate.max_rate =
                        granted_qos->tx_qos_flow.data_rate_min_max.data_rate_max; //kbps
    granted_info->tx_granted_flow_data.qos_flow_granted.umts_flow_desc.data_rate.guaranteed_rate =
                        granted_qos->tx_qos_flow.data_rate_min_max.guaranteed_rate; //kbps
    granted_info->tx_granted_flow_data.qos_flow_granted.umts_flow_desc.traffic_class =
                        granted_qos->tx_qos_flow.ip_flow_trf_cls;
  }

  if(granted_qos->rx_qos_flow_valid)
  {
    granted_info->rx_granted_flow_data.qos_flow_granted.cdma_flow_desc.profile_id =
                              granted_qos->rx_qos_flow.ip_flow_3gpp2_profile_id;
    granted_info->rx_granted_flow_data.qos_flow_granted.umts_flow_desc.data_rate.max_rate =
                              granted_qos->rx_qos_flow.data_rate_min_max.data_rate_max; //kbps
    granted_info->rx_granted_flow_data.qos_flow_granted.umts_flow_desc.data_rate.guaranteed_rate =
                              granted_qos->rx_qos_flow.data_rate_min_max.guaranteed_rate; //kbps
    granted_info->rx_granted_flow_data.qos_flow_granted.umts_flow_desc.traffic_class =
                              granted_qos->rx_qos_flow.ip_flow_trf_cls;
  }

  if(granted_qos->tx_qos_filter_len > 0 && granted_qos->tx_qos_filter_valid)
  {
    granted_info->tx_filter_count = granted_qos->tx_qos_filter_len;

    for(index=0;index < granted_qos->tx_qos_filter_len ;index++)
    {
      granted_info->tx_granted_filter_data[index].filter_index =
          granted_qos->tx_qos_filter[index].filter_id;
      filter_data = &granted_info->tx_granted_filter_data[index].qos_filter;

      netmgr_qmi_qos_process_filter_info(filter_data,
                                   &granted_qos->tx_qos_filter[index]);
    }
  }

  if(granted_qos->rx_qos_filter_len > 0 && granted_qos->rx_qos_filter_valid)
  {
    granted_info->rx_filter_count = granted_qos->rx_qos_filter_len;

    for(index=0;index < granted_qos->rx_qos_filter_len ;index++)
    {
      granted_info->rx_granted_filter_data[index].filter_index =
          granted_qos->rx_qos_filter[index].filter_id;
      filter_data = &granted_info->rx_granted_filter_data[index].qos_filter;

      netmgr_qmi_qos_process_filter_info(filter_data,
                                   &granted_qos->rx_qos_filter[index]);
    }
  }

bail:
  NETMGR_LOG_FUNC_EXIT;
  return NETMGR_SUCCESS;

}


/*===========================================================================
  FUNCTION  netmgr_qmi_qos_get_primary_granted_qos
===========================================================================*/
/*!
@brief
  Return the QoS parameters for a particular QOS flow.

@return
  int - NETMGR_SUCCESS on successful operation,
        NETMGR_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
int netmgr_qmi_qos_get_granted_qos
(
  int                            link,
  qmi_client_type                clnt_hndl,
  boolean                        is_primary,
  uint32                         flow_id,
  netmgr_qos_granted_info_rsp_type  *granted_info
)
{
  int rc = QMI_INTERNAL_ERR;
  int ret = NETMGR_FAILURE;
  unsigned char msg[QMI_MAX_STD_MSG_SIZE], *tmp_msg_ptr;
  int           msg_size;
  unsigned int actual_resp_len = 0;
  qmi_response_type_v01  resp;

  qos_get_qos_info_req_msg_v01 get_qos_req;
  qos_get_qos_info_resp_msg_v01 get_qos_resp;


  NETMGR_LOG_FUNC_ENTRY;

  if (NETMGR_QMI_CLIENT_INVALID == clnt_hndl)
  {
    netmgr_log_err("%s(): invalid clnt_hndl on link=%d!\n",
                   __func__, link);
    goto bail;
  }

  tmp_msg_ptr = msg;
  msg_size = sizeof(msg);

  if(netmgr_main_get_qos_via_idl_enabled())
  {
    memset (&get_qos_req, 0, sizeof(get_qos_req));
    memset (&get_qos_resp, 0, sizeof(get_qos_resp));

    get_qos_req.qos_id = flow_id;

    rc = qmi_client_send_msg_sync(clnt_hndl,
                    QMI_QOS_GET_QOS_INFO_REQ_V01,
                    (void *)&get_qos_req,
                    sizeof(get_qos_req),
                    (void*)&get_qos_resp,
                    sizeof(get_qos_resp),
                    NETMGR_QMI_TIMEOUT);

    if (rc != QMI_NO_ERR )
    {
      netmgr_log_err("%s(): failed for link=%d with rc=%d!\n",
                  __func__, link, rc);
      goto bail;
    }
    else
    {
      ret = netmgr_qmi_qos_process_granted_qos(granted_info,&get_qos_resp);
      goto bail;
    }
  }
  else
  {
    if (is_primary)
    {
      if (QMI_NO_ERR != qmi_qos_process_get_primary_granted_qos_req(&tmp_msg_ptr,
                                                                    &msg_size,
                                                                    flow_id))
      {
        goto bail;
      }
    }
    else
    {
      if (QMI_NO_ERR != qmi_qos_process_get_secondary_granted_qos_req(&tmp_msg_ptr,
                                                                      &msg_size,
                                                                      flow_id))
      {
        goto bail;
      }
    }

    rc = qmi_client_send_raw_msg_sync(clnt_hndl,
                                      NETMGR_QMI_QOS_GET_GRNTD_QOS_INFO_MSG_ID,
                                      msg,
                                      sizeof(msg)-msg_size,
                                      msg,
                                      sizeof(msg),
                                      &actual_resp_len,
                                      NETMGR_QMI_TIMEOUT);

    if (rc != QMI_NO_ERR || actual_resp_len == 0)
    {
      netmgr_log_err("%s(): failed for link=%d with rc=%d!\n",
                     __func__, link, rc);
      goto bail;
    }

    if (QMI_NO_ERR != qmi_qos_srvc_extract_return_code(msg,
                                                       actual_resp_len,
                                                       &resp))
    {
      netmgr_log_err("%s(): failed to extract return code for link=%d!\n",
                     __func__, link);
      goto bail;
    }
    else if (QMI_RESULT_SUCCESS_V01 != resp.result)
    {
      netmgr_log_err("%s(): get_granted_qos req failed for link=%d with rc=%d!\n",
                     __func__, link, resp.error);
      goto bail;
    }

    rc = qmi_qos_process_get_granted_qos_resp(msg,
                                              actual_resp_len,
                                              granted_info);

    if (QMI_NO_ERR != rc)
    {
      goto bail;
    }

    ret = netmgr_qmi_qos_get_filter_params(link,
                                           clnt_hndl,
                                           flow_id,
                                           granted_info);
  }
bail:
  NETMGR_LOG_FUNC_EXIT;
  return ret;
}

