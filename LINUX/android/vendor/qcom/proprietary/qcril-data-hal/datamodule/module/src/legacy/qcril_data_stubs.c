/******************************************************************************

                  Q C R I L _ D A T A _ S T U B S . C

******************************************************************************/

/******************************************************************************

  @file    qcril_data_stubs.C
  @brief   QCRIL Data test stubs

  DESCRIPTION
  QCRIL_Data test stub functions.

******************************************************************************/
/*===========================================================================

  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/******************************************************************************

                      EDIT HISTORY FOR FILE

  $Id:$

when       who        what, where, why
--------   ---        -------------------------------------------------------
04/22/11   ar         Initial version

******************************************************************************/

#ifdef QCRIL_DATA_OFFTARGET

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <lqe.h>
#include <framework/legacy.h>
#include <framework/Log.h>
#include "qmi.h"
#include "qmi_wds_srvc.h"
#include "qmi_wds_utils.h"
#include "dsi_netctrl.h"
#include "qcril_reqlist.h"
#include "qcril_data_test.h"
#include "qcril_data_defs.h"
#include "qcril_data_stubs.h"
#include "data_system_determination_v01.h"

#define STUB_ENTRY  fprintf( stderr, "STUB %s: entry\n", __func__ );
#define STUB_EXIT   fprintf( stderr, "STUB %s: exit\n", __func__ );
#define QCRIL_DATA_MAX_CLIENTS_SUPPORTED  (1)
#define QOS_FLOW_ID (0x01020304)
typedef void          qcril_data_evt_payload_t;
int handle[10] = { 0 };
static int* next_handle = handle;
typedef unsigned int  qcril_data_hndl_t;
typedef void (*qcril_data_net_ev_cb)( qcril_data_hndl_t         hndl,
                                      void                     *user_data,
                                      qcril_data_net_evt_t      evt,
                                      qcril_data_evt_payload_t *payload );
static struct client_s
{
  qcril_stub_response_cb resp_cb_fn;
  void                  *resp_user_data;
  dsi_net_ev_cb          dsi_cb_fn;
  void                  *dsi_user_data;
  dsi_hndl_t             handle;
} qcril_data_stub_client;

qcril_data_test_client_info_t qcril_data_client_info;
//typedef struct qcril_data_client_s
//{
//  qcril_data_hndl_t    hndl;
//  qcril_data_net_ev_cb cb_f;
//  void                *user_data;
//  struct qcril_data_client_s *self;
//} qcril_data_client_t;

//struct {
//  int                  cnt;
//  qcril_data_client_t  tbl[QCRIL_DATA_MAX_CLIENTS_SUPPORTED];
//  pthread_mutex_t      mutex;
//} qcril_data_client_info;

dsi_qos_granted_info_type dummy_qos =
{
  .tx_granted_flow_data_is_valid = TRUE,
  .tx_granted_flow_data.ip_flow_index = 0,
  .tx_granted_flow_data.qos_flow_granted.umts_flow_desc.param_mask = ( QMI_QOS_UMTS_FLOW_PARAM_DATA_RATE |
                                                                       QMI_QOS_UMTS_FLOW_PARAM_TRAFFIC_CLASS
                                                                       ),
  .tx_granted_flow_data.qos_flow_granted.umts_flow_desc.traffic_class = QMI_QOS_UMTS_TC_INTERACTIVE,
  .tx_granted_flow_data.qos_flow_granted.umts_flow_desc.data_rate.guaranteed_rate = 64000,
  .tx_granted_flow_data.qos_flow_granted.umts_flow_desc.data_rate.max_rate = 64000,
  .tx_granted_flow_data.qos_flow_granted.umts_flow_desc.max_delay = 100,
  .tx_granted_flow_data.qos_flow_granted.umts_flow_desc.flow_priority_3gpp2 = 5,

  .tx_granted_flow_data.qos_flow_granted.cdma_flow_desc.param_mask = QMI_QOS_CDMA_FLOW_PARAM_PROFILE_ID,
  .tx_granted_flow_data.qos_flow_granted.cdma_flow_desc.profile_id = 42,


  .rx_granted_flow_data_is_valid = TRUE,
  .rx_granted_flow_data.ip_flow_index = 0,
  .rx_granted_flow_data.qos_flow_granted.umts_flow_desc.param_mask = QMI_QOS_UMTS_FLOW_PARAM_DATA_RATE,
  .rx_granted_flow_data.qos_flow_granted.umts_flow_desc.data_rate.guaranteed_rate = 512000,
  .rx_granted_flow_data.qos_flow_granted.umts_flow_desc.data_rate.max_rate = 1024000,

  .dss_errno_type.param_mask = 0,
  .dss_errno_type.dss_errno  = 0
};


/*-----------------------------------------------------------------------*/

int dsi_init(int mode)
{
  (void)mode;
  STUB_ENTRY;
  STUB_EXIT;
  return QCRIL_DATA_SUCCESS;
}

dsi_hndl_t dsi_get_data_srvc_hndl
(
  dsi_net_ev_cb dsi_cb_fn,
  void * dsi_user_data
)
{
  (void)dsi_cb_fn; (void)dsi_user_data;
  STUB_ENTRY;

  qcril_data_stub_client.dsi_cb_fn = dsi_cb_fn;
  qcril_data_stub_client.dsi_user_data = dsi_user_data;
  qcril_data_stub_client.handle = next_handle++;

  STUB_EXIT;
  return qcril_data_stub_client.handle;
}

void dsi_rel_data_srvc_hndl(dsi_hndl_t hndl)
{
  (void)hndl;
  STUB_ENTRY;

  qcril_data_stub_client.dsi_cb_fn = NULL;
  qcril_data_stub_client.dsi_user_data = NULL;
  int i = 0;
  qcril_data_stub_client.handle = (void*)&i;

  STUB_EXIT;
  return;
}

int dsi_start_data_call(dsi_hndl_t hndl)
{
  (void)hndl;
  STUB_ENTRY;
  STUB_EXIT;
  return QCRIL_DATA_SUCCESS;
}

int dsi_stop_data_call(dsi_hndl_t hndl)
{
  (void)hndl;
  STUB_ENTRY;
  STUB_EXIT;
  return QCRIL_DATA_SUCCESS;
}

int dsi_set_data_call_param
(
  dsi_hndl_t hndl,
  dsi_call_param_identifier_t identifier,
  dsi_call_param_value_t *info
)
{
  (void)hndl; (void)identifier; (void)info;
  STUB_ENTRY;
  STUB_EXIT;
  return QCRIL_DATA_SUCCESS;
}

int dsi_get_device_name(dsi_hndl_t hndl, char * buf, int len)
{
  (void)hndl; (void)buf; (void)len;
  STUB_ENTRY;
  STUB_EXIT;
  return QCRIL_DATA_SUCCESS;
}

int dsi_get_call_end_reason
(
  dsi_hndl_t hndl,
  dsi_ce_reason_t * ce_reason,
  dsi_ip_family_t ipf
)
{
  (void)hndl; (void)ce_reason; (void)ipf;
  STUB_ENTRY;
  STUB_EXIT;
  return QCRIL_DATA_SUCCESS;
}

int dsi_get_ip_addr
(
  dsi_hndl_t hndl,
  dsi_addr_info_t * info_ptr,
  int len
)
{
  (void)hndl; (void)info_ptr; (void)len;
  STUB_ENTRY;
  STUB_EXIT;
  return QCRIL_DATA_SUCCESS;
}


unsigned int dsi_get_ip_addr_count
(
  dsi_hndl_t hndl
)
{
  (void)hndl;
  STUB_ENTRY;
  STUB_EXIT;
  return QCRIL_DATA_SUCCESS;
}

int dsi_get_addr(dsi_hndl_t hndl, sockaddr_storage_t *addr_ptr)
{
  (void)hndl; (void)addr_ptr;
  STUB_ENTRY;
  STUB_EXIT;
  return QCRIL_DATA_SUCCESS;
}

int dsi_request_qos
(
  dsi_hndl_t              hndl,
  unsigned int            num_qos_specs,
  dsi_qos_spec_type      *qos_spec_list,
  dsi_qos_req_opcode_type req_opcode,
  dsi_qos_id_type        *qos_id_list,
  dsi_qos_err_rsp_type   *qos_spec_err_list
)
{
  dsi_evt_payload_t  payload;

  (void)hndl; (void)num_qos_specs; (void)qos_spec_list;
  (void)req_opcode; (void)qos_id_list; (void)qos_spec_err_list;
  STUB_ENTRY;

  if( -1 == *((int*)qcril_data_client_info.handle) )
  {
    /* Error case */
    fprintf( stderr, "STUB %s: return general error\n", __func__ );
    STUB_EXIT;
    return QCRIL_DATA_FAILURE;
  }
  else
  {
    /* Invoke registered callback */
    if( qcril_data_stub_client.dsi_cb_fn )
    {
      if( -2 == *((int*)qcril_data_client_info.handle) )
      {
        fprintf( stderr, "STUB %s: flow gone error\n", __func__ );
        payload.qos_info.flow_id = QOS_FLOW_ID;
        payload.qos_info.status_evt = DSI_QOS_GONE_EV;
      }
      else
      {
        /* Success case */
        payload.qos_info.flow_id = QOS_FLOW_ID;
        payload.qos_info.status_evt = DSI_QOS_ACTIVATED_EV;
        qos_id_list[0] = QOS_FLOW_ID;
      }

      /* Override dsi_user_data with qos_spec_list for validation */
      qcril_data_stub_client.dsi_cb_fn( hndl,
                                        (void*)qos_spec_list,
                                        DSI_EVT_QOS_STATUS_IND,
                                        &payload );
    }
  }

  STUB_EXIT;
  return QCRIL_DATA_SUCCESS;
}

int dsi_release_qos
(
  dsi_hndl_t              hndl,
  unsigned int            num_qos_ids,
  dsi_qos_id_type        *qos_id_list
)
{
  dsi_evt_payload_t  payload;

  (void)hndl; (void)num_qos_ids; (void)qos_id_list;
  STUB_ENTRY;

  if( -1 == *((int*)qcril_data_client_info.handle) )
  {
    /* Error case */
    fprintf( stderr, "STUB %s: return general error\n", __func__ );
    STUB_EXIT;
    return QCRIL_DATA_FAILURE;
  }
  else
  {
    /* Invoke registered callback */
    if( qcril_data_stub_client.dsi_cb_fn )
    {
      payload.qos_info.flow_id = qos_id_list[0];
      payload.qos_info.status_evt = DSI_QOS_GONE_EV;
      qcril_data_stub_client.dsi_cb_fn( qcril_data_stub_client.handle,
                                        qcril_data_stub_client.dsi_user_data,
                                        DSI_EVT_QOS_STATUS_IND,
                                        &payload );
    }
  }

  STUB_EXIT;
  return QCRIL_DATA_SUCCESS;
}

int dsi_modify_qos
(
  dsi_hndl_t              hndl,
  unsigned int            num_qos_specs,
  dsi_qos_spec_type      *qos_spec_list,
  dsi_qos_err_rsp_type   *qos_spec_err_list
)
{
  dsi_evt_payload_t  payload;

  (void)hndl; (void)num_qos_specs; (void)qos_spec_list; (void)qos_spec_err_list;
  STUB_ENTRY;

  if( -1 == *((int*)qcril_data_client_info.handle) )
  {
    /* Error case */
    fprintf( stderr, "STUB %s: return general error\n", __func__ );
    STUB_EXIT;
    return QCRIL_DATA_FAILURE;
  }
  else
  {
    /* Invoke registered callback */
    if( qcril_data_stub_client.dsi_cb_fn )
    {
      if( -2 == *((int*)qcril_data_client_info.handle ))
      {
        fprintf( stderr, "STUB %s: flow gone error\n", __func__ );
        payload.qos_info.flow_id = QOS_FLOW_ID;
        payload.qos_info.status_evt = DSI_QOS_GONE_EV;
      }
      else if( -3 == *((int*)qcril_data_client_info.handle ))
      {
        fprintf( stderr, "STUB %s: flow modify rejected\n", __func__ );
        payload.qos_info.flow_id = QOS_FLOW_ID;
        payload.qos_info.status_evt = DSI_QOS_MODIFY_REJECTED_EV;
      }
      else
      {
        /* Success case */
        fprintf( stderr, "STUB %s: flow modify accespted\n", __func__ );
        payload.qos_info.flow_id = QOS_FLOW_ID;
        payload.qos_info.status_evt = DSI_QOS_MODIFY_ACCEPTED_EV;
      }

      /* Override dsi_user_data with qos_spec_list for validation */
      qcril_data_stub_client.dsi_cb_fn( hndl,
                                        (void*)qos_spec_list,
                                        DSI_EVT_QOS_STATUS_IND,
                                        &payload );
    }
  }

  STUB_EXIT;
  return QCRIL_DATA_SUCCESS;
}

int dsi_suspend_qos
(
  dsi_hndl_t              hndl,
  unsigned int            num_qos_ids,
  dsi_qos_id_type        *qos_id_list
)
{
  dsi_evt_payload_t  payload;

  (void)hndl; (void)num_qos_ids; (void)qos_id_list;
  STUB_ENTRY;

  if( -1 == *((int*)qcril_data_client_info.handle ))
  {
    /* Error case */
    fprintf( stderr, "STUB %s: return general error\n", __func__ );
    STUB_EXIT;
    return QCRIL_DATA_FAILURE;
  }
  else
  {
    /* Invoke registered callback */
    if( qcril_data_stub_client.dsi_cb_fn )
    {
      /* Success case */
      payload.qos_info.flow_id = qos_id_list[0];
      payload.qos_info.status_evt = DSI_QOS_SUSPENDED_EV;
      qcril_data_stub_client.dsi_cb_fn( qcril_data_stub_client.handle,
                                        qcril_data_stub_client.dsi_user_data,
                                        DSI_EVT_QOS_STATUS_IND,
                                        &payload );
    }
  }

  STUB_EXIT;
  return QCRIL_DATA_SUCCESS;
}

int dsi_resume_qos
(
  dsi_hndl_t              hndl,
  unsigned int            num_qos_ids,
  dsi_qos_id_type        *qos_id_list
)
{
  dsi_evt_payload_t  payload;

  (void)hndl; (void)num_qos_ids; (void)qos_id_list;
  STUB_ENTRY;

  if( -1 == *((int*)qcril_data_client_info.handle ))
  {
    /* Error case */
    fprintf( stderr, "STUB %s: return general error\n", __func__ );
    STUB_EXIT;
    return QCRIL_DATA_FAILURE;
  }
  else
  {
    /* Invoke registered callback */
    if( qcril_data_stub_client.dsi_cb_fn )
    {
      if( -2 == *((int*)qcril_data_client_info.handle ))
      {
        fprintf( stderr, "STUB %s: flow gone error\n", __func__ );
        payload.qos_info.flow_id = qos_id_list[0];
        payload.qos_info.status_evt = DSI_QOS_GONE_EV;
      }
      else
      {
        /* Success case */
        payload.qos_info.flow_id = qos_id_list[0];
        payload.qos_info.status_evt = DSI_QOS_ACTIVATED_EV;
      }

      /* Override dsi_user_data with qos_spec_list for validation */
      qcril_data_stub_client.dsi_cb_fn( hndl,
                                        qcril_data_stub_client.dsi_user_data,
                                        DSI_EVT_QOS_STATUS_IND,
                                        &payload );
    }
  }

  STUB_EXIT;
  return QCRIL_DATA_SUCCESS;
}

int dsi_get_granted_qos
(
  dsi_hndl_t                 hndl,
  dsi_qos_id_type            qos_id,
  QCRIL_UNUSED(int                        ip_family),
  dsi_qos_granted_info_type *qos_info
)
{
  int ret = QCRIL_DATA_SUCCESS;
  (void)hndl; (void)qos_info;
  STUB_ENTRY;

  if( QOS_FLOW_ID == qos_id )
    memcpy( qos_info, &dummy_qos, sizeof(dsi_qos_granted_info_type) );
  else
    ret = QCRIL_DATA_FAILURE;

  STUB_EXIT;
  return ret;
}


int dsi_get_qos_status
(
  dsi_hndl_t           hndl,
  dsi_qos_id_type      qos_id,
  dsi_qos_status_type *qos_status
)
{
  (void)hndl; (void)qos_status;
  STUB_ENTRY;

  if( QOS_FLOW_ID == qos_id )
    *qos_status = QMI_QOS_STATUS_ACTIVATED;
  else
    *qos_status = QMI_QOS_STATUS_GONE;

  STUB_EXIT;
  return QCRIL_DATA_SUCCESS;
}

/*-----------------------------------------------------------------------*/

int qdp_profile_look_up
(
  const char  ** param_strings,    /* the order of params must match with the
                                      order specified in qdp_ril_param_idx_t */
  unsigned int * profile_id_3gpp,  /* value (not pointer it-self) must
                                      be set to zero by caller */
  qdp_profile_pdn_type *profile_pdn_type_3gpp,  /* 3gpp profile PDN type */
  unsigned int * profile_id_3gpp2, /* value must be set to zero by caller */
  qdp_profile_pdn_type *profile_pdn_type_3gpp2, /* 3gpp2 profile PDN type */
  QCRIL_UNUSED(qdp_error_info_t * error_info)
)
{
  (void)param_strings; (void)profile_id_3gpp; (void)profile_id_3gpp2;
  (void)profile_pdn_type_3gpp; (void)profile_pdn_type_3gpp2;
  STUB_ENTRY;
  STUB_EXIT;
  return QCRIL_DATA_SUCCESS;
}

int qdp_profile_look_up_by_param
(
 /* the order of params must match with the order specified in qdp_ril_param_idx_t */
 const char                             **param_strings,
 QCRIL_UNUSED(int                                    param_to_match),
 QCRIL_UNUSED(int                                    param_value),
 unsigned int                           *profile_id_3gpp_list,
 uint8                            *profile_id_3gpp_list_len,
 qdp_profile_pdn_type                   *profile_pdn_type_3gpp,
 unsigned int                           *profile_id_3gpp2_list,
 uint8                           *profile_id_3gpp2_list_len,
 qdp_profile_pdn_type                   *profile_pdn_type_3gpp2,
 QCRIL_UNUSED(qdp_tech_t                             *tech_type),
 QCRIL_UNUSED(qdp_error_info_t                       *error_info)
)
{
  (void)param_strings; (void)profile_id_3gpp_list; (void)profile_id_3gpp2_list;
  (void)profile_id_3gpp_list_len; (void)profile_id_3gpp2_list_len;
  (void)profile_pdn_type_3gpp; (void)profile_pdn_type_3gpp2;
  STUB_ENTRY;
  STUB_EXIT;
  return QCRIL_DATA_SUCCESS;
}

int qdp_3gpp_profile_update_ex
(
  QCRIL_UNUSED(qmi_wds_profile_params_type * p_params),
  QCRIL_UNUSED(unsigned int                  profile_id),
  QCRIL_UNUSED(int                         * error_code)
)
{
  STUB_ENTRY;
  STUB_EXIT;
  return QCRIL_DATA_SUCCESS;
}
int qdp_profile_release
(
  unsigned int profile_id
)
{
  (void)profile_id;
  STUB_ENTRY;
  STUB_EXIT;
  return QCRIL_DATA_SUCCESS;
}

int qdp_profile_release_ex
(
 unsigned int profile_id
)
{
  (void)profile_id;
  STUB_ENTRY;
  STUB_EXIT;
  return QCRIL_DATA_SUCCESS;
}

void qdp_deinit(void)
{
  STUB_ENTRY;
  STUB_EXIT;
}

/*-----------------------------------------------------------------------*/
int qmi_wds_client = 0;
int qmi_qos_client = 0;

int
qmi_init
(
  QCRIL_UNUSED(qmi_sys_event_rx_hdlr   event_rx_hdlr),
  QCRIL_UNUSED(void                    *event_user_data)
)
{
  STUB_ENTRY;
  STUB_EXIT;
  return QMI_NO_ERR;
}

int
qmi_release( int init_handle )
{
  (void)init_handle;
  STUB_ENTRY;
  STUB_EXIT;
  return QMI_NO_ERR;
}


int
qmi_dev_connection_init
(
  QCRIL_UNUSED(const char  *dev_id),
  QCRIL_UNUSED(int         *qmi_err_code)
)
{
  STUB_ENTRY;
  STUB_EXIT;
  return QMI_NO_ERR;
}

int
qmi_set_port_data_format
(
  QCRIL_UNUSED(const char                            *dev_id),
  QCRIL_UNUSED(qmi_data_format_qos_hdr_state_type    qos_hdr_state),
  QCRIL_UNUSED(qmi_link_layer_protocol_type          *link_protocol),
  QCRIL_UNUSED(int                                   *qmi_err_code)
)
{
  STUB_ENTRY;
  STUB_EXIT;
  return QMI_NO_ERR;
}


qmi_client_handle_type
qmi_wds_srvc_init_client
(
  QCRIL_UNUSED(const char                    *dev_id),
  QCRIL_UNUSED(qmi_wds_indication_hdlr_type  user_ind_msg_hdlr),
  QCRIL_UNUSED(void                          *user_ind_msg_hdlr_user_data),
  QCRIL_UNUSED(int                           *qmi_err_code)
)
{
  STUB_ENTRY;
  STUB_EXIT;
  return qmi_wds_client++;;
}

int
qmi_wds_srvc_release_client
(
  QCRIL_UNUSED(int      user_handle),
  QCRIL_UNUSED(int      *qmi_err_code)
)
{
  STUB_ENTRY;
  STUB_EXIT;
  return QMI_NO_ERR;
}


int
qmi_wds_query_profile
(
  QCRIL_UNUSED(int                         user_handle),
  QCRIL_UNUSED(qmi_wds_profile_id_type     *profile_id),
  QCRIL_UNUSED(qmi_wds_profile_params_type *profile_params),
  QCRIL_UNUSED(int                         *qmi_err_code)
)
{
  STUB_ENTRY;
  STUB_EXIT;
  return QMI_NO_ERR;
}


int
qmi_wds_get_curr_call_info
(
  QCRIL_UNUSED(int                                        user_handle),
  QCRIL_UNUSED(qmi_wds_req_runtime_settings_params_type   requested_settings),
  QCRIL_UNUSED(qmi_wds_profile_id_type                    *profile_id),
  QCRIL_UNUSED(qmi_wds_profile_params_type                *profile_params),
  QCRIL_UNUSED(qmi_wds_curr_call_info_type                *call_settings),
  QCRIL_UNUSED(int                                        *qmi_err_code)
)
{
  STUB_ENTRY;
  call_settings->mask = (QMI_WDS_CURR_CALL_INFO_IP_FAMILY|
                         QMI_WDS_CURR_CALL_INFO_IPV4_ADDR);
  call_settings->ip_family = QMI_WDS_IPV4_TYPE;
  call_settings->ipv4_addr = 0x01020304;
  STUB_EXIT;
  return QMI_NO_ERR;
}

int
qmi_wds_get_pref_data_system
(
  QCRIL_UNUSED(int                         user_handle),
  QCRIL_UNUSED(qmi_wds_pref_data_sys_type  *cur_pref_sys),
  QCRIL_UNUSED(int                         *qmi_err_code)
)
{
  STUB_ENTRY;
  STUB_EXIT;
  return QMI_NO_ERR;
}



int
qmi_qos_set_event_report_state
(
  QCRIL_UNUSED(int                               client_handle),
  QCRIL_UNUSED( qmi_qos_event_report_state_type   *report_state),
  QCRIL_UNUSED(  int                               *qmi_err_code)
)
{
  STUB_ENTRY;
  STUB_EXIT;
  return QMI_NO_ERR;
}

int
qmi_qos_get_primary_granted_qos_info
(
  QCRIL_UNUSED(qmi_client_handle_type                  client_handle),
  QCRIL_UNUSED(unsigned long                           qos_identifier),
  qmi_qos_granted_info_rsp_type           *granted_info,
  QCRIL_UNUSED(int                                     *qmi_err_code)
)
{
  STUB_ENTRY;

  // Fake CDMA profile
  granted_info->tx_granted_flow_data_is_valid = TRUE;
  granted_info->tx_granted_flow_data.qos_flow_granted.cdma_flow_desc.param_mask = QMI_QOS_CDMA_FLOW_PARAM_PROFILE_ID;
  granted_info->tx_granted_flow_data.qos_flow_granted.cdma_flow_desc.profile_id = 5;

  STUB_EXIT;
  return QMI_NO_ERR;
}

int
qmi_qos_get_secondary_granted_qos_info
(
  QCRIL_UNUSED(qmi_client_handle_type                  client_handle),
  QCRIL_UNUSED(unsigned long                           qos_identifier),
  qmi_qos_granted_info_rsp_type           *granted_info,
  QCRIL_UNUSED(int                                     *qmi_err_code)
)
{
  STUB_ENTRY;

  // Fake CDMA profile
  granted_info->tx_granted_flow_data_is_valid = TRUE;
  granted_info->tx_granted_flow_data.qos_flow_granted.cdma_flow_desc.param_mask = QMI_QOS_CDMA_FLOW_PARAM_PROFILE_ID;
  granted_info->tx_granted_flow_data.qos_flow_granted.cdma_flow_desc.profile_id = 7;

  STUB_EXIT;
  return QMI_NO_ERR;
}

int
qmi_wds_utils_get_profile_list
(
  QCRIL_UNUSED(int                             user_handle),
  QCRIL_UNUSED(qmi_wds_profile_tech_type       *profile_tech),
  QCRIL_UNUSED(qmi_wds_profile_node_list_type  *profile_search_list),
  QCRIL_UNUSED(qmi_wds_profile_list_type       *profile_list),
  QCRIL_UNUSED(int                             *num_profile_list_elements),
  QCRIL_UNUSED(int                             *qmi_err_code)
)
{
  STUB_ENTRY;
  STUB_EXIT;
  return QMI_NO_ERR;
}

int
qmi_wds_utils_get_profile_list2
(
  QCRIL_UNUSED(int                              user_handle),
  QCRIL_UNUSED(qmi_wds_profile_tech_type        profile_tech),
  QCRIL_UNUSED(uint64_t                         param_mask),
  QCRIL_UNUSED(void                            *param_value),
  QCRIL_UNUSED(qmi_wds_profile_list_type       *profile_list),
  QCRIL_UNUSED(int                             *num_profile_list_elements),
  QCRIL_UNUSED(int                             *qmi_err_code)
)
{
  STUB_ENTRY;
  STUB_EXIT;
  return QMI_NO_ERR;
}


int
qmi_wds_set_event_report
(
  int                               user_handle,
  qmi_wds_event_report_params_type  *event_params,
  int                               *qmi_err_code
)
{
  STUB_ENTRY;
  STUB_EXIT;
  return QMI_NO_ERR;
}

int
qmi_wds_go_dormant_req
(
  int   user_handle,
  int   *qmi_err_code
)
{
  STUB_ENTRY;
  STUB_EXIT;
  return QMI_NO_ERR;
}

int
qmi_wds_get_dormancy_status
(
  int                       user_handle,
  qmi_wds_dorm_status_type  *dorm_status,
  int                       *qmi_err_code
)
{
  STUB_ENTRY;
  STUB_EXIT;
  return QMI_NO_ERR;
}

int
qmi_wds_get_current_bearer_tech
(
  int                            user_handle,
  qmi_wds_data_bearer_tech_type  *rsp_data,
  int                            *qmi_err_code
)
{
  STUB_ENTRY;
  STUB_EXIT;
  return QMI_NO_ERR;
}

/*-----------------------------------------------------------------------*/


int qcril_dsds_ipc_send_func(int lvl,char *log_buffer)
{
  STUB_ENTRY;
  STUB_EXIT;
  return QCRIL_DATA_SUCCESS;
}

/*-----------------------------------------------------------------------*/

int property_get(const char *key, char *value, const char *default_value)
{
  STUB_ENTRY;
  STUB_EXIT;
  return 0;
}


/*-----------------------------------------------------------------------*/

//int atexit(void (*function)(void))
//{
//  return 0;
//}


void qcril_stubs_init( qcril_stub_response_cb resp_cb_fn, void* user_data )
{
  qcril_data_stub_client.resp_cb_fn = resp_cb_fn;
  qcril_data_stub_client.resp_user_data = user_data;
}

void qcril_stubs_release( void )
{
  qcril_data_stub_client.resp_cb_fn = NULL;
  qcril_data_stub_client.resp_user_data = NULL;
}

int timer_create(clockid_t clock_id, struct sigevent* evp, timer_t* timer_id)
{
  STUB_ENTRY;
  STUB_EXIT;
  return(0);
}

int timer_delete(timer_t id)
{
  STUB_ENTRY;
  STUB_EXIT;
  return(0);
}

int timer_settime(timer_t id, int flags, const struct itimerspec* ts, struct itimerspec* ots)
{
  STUB_ENTRY;
  STUB_EXIT;
  return(0);
}


void ds_sl_list_init( list_type *list_ptr) {
  STUB_ENTRY;
  STUB_EXIT;
}

int dsi_embms_content_desc_update
(
  dsi_hndl_t                    hndl,
  char                         *content_desc_update_tmgi,
  unsigned char                 content_desc_valid,
  unsigned int                  content_desc_len,
  dsi_embms_content_desc_type  *content_desc,
  int                           dbg_trace_id
) {

  STUB_ENTRY;
  STUB_EXIT;
  return(0);
}

int dsi_embms_enable
(
  dsi_hndl_t hndl
) {

  STUB_ENTRY;
  STUB_EXIT;
  return(0);
}
int dsi_embms_get_tmgi_deactivate_reason
(
  dsi_hndl_t                               hndl,
  dsi_embms_tmgi_status_field_type         status_field,
  int                                     *dsi_tmgi_deactivate_reason
) {

  STUB_ENTRY;
  STUB_EXIT;
  return(0);
}
int dsi_embms_svc_interest_list
(
  dsi_hndl_t          hndl,
  char              **svc_interest_tmgi_list,
  unsigned int        svc_interest_tmgi_list_len,
  int                 dbg_trace_id
) {

  STUB_ENTRY;
  STUB_EXIT;
  return(0);
}

int dsi_embms_tmgi_activate
(
  dsi_hndl_t                          hndl,
  char                               *tmgi,
  unsigned int                       *earfcnlist,
  unsigned char                       earfcnlist_len,
  unsigned long                       preemption_priority,
  unsigned int                       *sailist,
  unsigned char                       sailist_len,
  int                                 dbg_trace_id
) {

  STUB_ENTRY;
  STUB_EXIT;
  return(0);
}

int dsi_embms_tmgi_activate_deactivate
(
  dsi_hndl_t                         hndl,
  char                              *activate_tmgi,
  unsigned int                      *earfcnlist,
  unsigned char                      earfcnlist_len,
  unsigned long                      preemption_priority,
  char                              *deactivate_tmgi,
  unsigned int                      *sailist,
  unsigned char                      sailist_len,
  int                                dbg_trace_id
) {
  STUB_ENTRY;
  STUB_EXIT;
  return(0);
}

int dsi_embms_tmgi_deactivate
(
  dsi_hndl_t                           hndl,
  char                                *tmgi,
  int                                  dbg_trace_id
) {

  STUB_ENTRY;
  STUB_EXIT;
  return(0);
}

int dsi_embms_tmgi_list_query
(
  dsi_hndl_t                               hndl,
  wds_embms_tmgi_list_type_enum_v01        embms_tmgi_list_query,
  wds_embms_tmgi_list_query_resp_msg_v01  *list_info,
  int                                      dbg_trace_id
) {

  STUB_ENTRY;
  STUB_EXIT;
  return(0);
}
int dsi_get_call_tech
(
  dsi_hndl_t hndl,
  dsi_call_tech_type *call_tech
) {

  STUB_ENTRY;
  STUB_EXIT;
  return(0);
}


void*
list_peek_front(
               list_type *list_ptr
               ) {

  STUB_ENTRY;
  STUB_EXIT;
  return NULL;
}

void*
list_peek_next(
  list_type      *list_ptr,
  /*lint -esym(715,list_ptr)
  ** Have lint not complain about the ignored parameter 'list_ptr'.
  */
  list_link_type *item_after_which_to_peek
) {
  STUB_ENTRY;
  STUB_EXIT;
  return NULL;
}

void list_pop_item(
                  list_type      *list_ptr,
                  list_link_type *item_to_pop_ptr
                  ) {
  STUB_ENTRY;
  STUB_EXIT;
}
void list_push_back(
                   list_type      *list_ptr,
                   list_link_type *item_link_ptr
                   ) {
  STUB_ENTRY;
  STUB_EXIT;
}

int
lqei_client_register
(
  lqei_ind_f              cb_f,
  void                   *data,
  lqe_client_hdl_t       *client_handle
) {
  STUB_ENTRY;
  STUB_EXIT;
  return 0;
}

int
lqei_client_release
(
  lqe_client_hdl_t       client_handle
) {
  STUB_ENTRY;
  STUB_EXIT;
  return 0;
}
int
lqei_get_downlink_throughput_info
(
  lqe_client_hdl_t       client_handle,
  int                    *dl_rate,
  int                    *confidence_level
) {
  STUB_ENTRY;
  STUB_EXIT;
  return 0;
}

int
lqei_get_min_downlink_throughput_frequency
(
  lqe_client_hdl_t                        client_handle,
  lqe_downlink_throughput_report_status  *status
) {
  STUB_ENTRY;
  STUB_EXIT;
  return 0;
}

int
lqei_init
(
  void
) {
  STUB_ENTRY;
  STUB_EXIT;
  return 0;
}
int
lqei_release
(
  void
) {
  STUB_ENTRY;
  STUB_EXIT;
  return 0;
}
int
lqei_set_downlink_throughput_indication_frequency
(
  lqe_client_hdl_t            client_handle,
  int                         report_interval_timer,
  lqe_indications_switch_e    indications
) {
  STUB_ENTRY;
  STUB_EXIT;
  return 0;
}
int ds_atoi (const char * str) {
  STUB_ENTRY;
  STUB_EXIT;
  return 0;
}
const char *ds_get_target_str
(
  ds_target_t target
) {
  STUB_ENTRY;
  STUB_EXIT;
  return 0;
}
dsi_data_bearer_tech_t
dsi_get_current_data_bearer_tech
(
  dsi_hndl_t hndl
) {
  STUB_ENTRY;
  STUB_EXIT;
  return DSI_DATA_BEARER_TECH_LTE;
}

int dsi_get_qmi_port_name(dsi_hndl_t hndl, char * buf, int len) {
  STUB_ENTRY;
  STUB_EXIT;
  return 0;
}
int dsi_release(int mode) {
  STUB_ENTRY;
  STUB_EXIT;
  return 0;
}
int dsi_set_modem_subs_id
(
  int subs_id
) {
  STUB_ENTRY;
  STUB_EXIT;
  return 0;
}
int dsi_set_ril_instance
(
  int instance
) {
  STUB_ENTRY;
  STUB_EXIT;
  return 0;
}
int qdp_init
(
  const char * default_port
) {
  STUB_ENTRY;
  STUB_EXIT;
  return 0;
}
int qdp_set_subscription
(
  int subs_id
) {
  STUB_ENTRY;
  STUB_EXIT;
  return 0;
}
int dsi_process_screen_state_change(int screen_state) {
  STUB_ENTRY;
  STUB_EXIT;
  return 0;
}
int qdp_3gpp2_profile_update_params
(
  const char                               **param_strings,
  unsigned int                             profile_id,
  wds_modify_profile_settings_req_msg_v01  *p_params
) {
  STUB_ENTRY;
  STUB_EXIT;
  return 0;
}
int qdp_3gpp_profile_update_params
(
  const char                               **param_strings,
  unsigned int                             profile_id,
  wds_modify_profile_settings_req_msg_v01  *p_params
) {
  STUB_ENTRY;
  STUB_EXIT;
  return 0;
}
int qdp_epc_profile_update_params
(
  const char                               **param_strings,
  unsigned int                             profile_id,
  wds_modify_profile_settings_req_msg_v01  *p_params
) {
  STUB_ENTRY;
  STUB_EXIT;
  return 0;
}
int qdp_lte_attach_profile_lookup
(
  const char             **param_strings,
  unsigned int           *profile_id_lte,
  qdp_profile_pdn_type   *profile_pdp_type_lte,
  unsigned int           *input_prof_id,
  qdp_profile_info_type  *prof_params,
  qdp_error_info_t       *error_info
) {
  STUB_ENTRY;
  STUB_EXIT;
  return 0;
}
int qdp_match_lte_attach_profile_params
(
  unsigned int  profile_id,
  const char    **param_strings
) {
  STUB_ENTRY;
  STUB_EXIT;
  return 0;
}
int qdp_modify_profile
(
  qdp_profile_info_type  *profile,
  int                    *error_code
) {
  STUB_ENTRY;
  STUB_EXIT;
  return 0;
}
int qdp_profile_update_ex
(
  wds_modify_profile_settings_req_msg_v01  *p_params,
  int                                      *error_code
) {
  STUB_ENTRY;
  STUB_EXIT;
  return 0;
}



int dsi_get_link_mtu
(
  dsi_hndl_t    hndl,
  unsigned int  *mtu
) {
  STUB_ENTRY;
  STUB_EXIT;
  return 0;
}
int dsi_iface_ioctl
(
  dsi_hndl_t dsi_hndl,
  dsi_iface_ioctl_enum_t ioctl_name,
  void *argval_ptr,
  int  *err_code
) {
  STUB_ENTRY;
  STUB_EXIT;
  return 0;
}

ds_target_t ds_get_target() {
  STUB_ENTRY;
  STUB_EXIT;
  return DS_TARGET_MSM8996;
}
qmi_client_error_type qmi_client_wds_init_instance
(
 const char                 *dev_str,
 qmi_idl_service_object_type service_obj,
 qmi_service_instance        instance_id,
 qmi_client_ind_cb           ind_cb,
 void                        *ind_cb_data,
 qmi_client_os_params        *os_params,
 uint32_t                    timeout,
 qmi_client_type             *user_handle
) {
  STUB_ENTRY;
  STUB_EXIT;
  return 0;
}

#pragma GCC diagnostic pop

#endif /* QCRIL_DATA_OFFTARGET */
