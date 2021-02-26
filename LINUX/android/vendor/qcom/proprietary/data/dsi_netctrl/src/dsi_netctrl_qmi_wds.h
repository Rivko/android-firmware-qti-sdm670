/******************************************************************************

                     D S I _ N E T C T R L _ Q M I _ W D S . H

******************************************************************************/

/******************************************************************************

  @file    dsi_netctrl_qmi_wds.h
  @brief   QMI WDS interface file header for DSI Netctrl

  DESCRIPTION
  QMI WDS interface file header for DSI Netctrl

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#ifndef DSI_NETCTRL_WDS_H
#define DSI_NETCTRL_WDS_H

#include "qmi_client.h"
#include "qmi.h"
#include "dsi_netctrl_qmi.h"
#include "wireless_data_service_v01.h"

/* Additional EP TYPE */
#define DSI_QMI_WDS_PER_EP_TYPE_BAM_DMUX  0x05

/* Parameter indication bits */
#define DSI_QMI_WDS_ROUTE_LOOKUP_TECH_PREF_PARAM                      0x00000001
#define DSI_QMI_WDS_ROUTE_LOOKUP_PROFILE_IDX_PARAM                    0x00000002
#define DSI_QMI_WDS_ROUTE_LOOKUP_APN_NAME_PARAM                       0x00000004
#define DSI_QMI_WDS_ROUTE_LOOKUP_IP_FAMILY_PREF_PARAM                 0x00000008
#define DSI_QMI_WDS_ROUTE_LOOKUP_PROFILE_IDX_3GPP2_PARAM              0x00000010
#define DSI_QMI_WDS_ROUTE_LOOKUP_XTENDED_TECH_PREF_PARAM              0x00000020
#define DSI_QMI_WDS_ROUTE_LOOKUP_DATA_CALL_ORIGIN_PARAM               0x00000040

#define DSI_QMI_WDS_EMBMS_TMGI_LIST_INVALID   0xFF
#define DSI_QMI_WDS_EMBMS_TMGI_STATUS_IND_LIST_SIZE 1

#define DSI_QMI_WDS_EMBMS_CONTENT_PARAM_NUM_SIZE 64

/************************************************************************
* Definitions associated with TMGI list query req/resp
************************************************************************/
#define DSI_QMI_WDS_EMBMS_LIST_QUERY_REQ_TRANX_ID_PARAM_MASK    0x0001
#define DSI_QMI_WDS_EMBMS_LIST_QUERY_RESP_TMGI_LIST_PARAM_MASK  0x0001

typedef enum
{
  DSI_INTERFACE_LOOKUP  = 1,
  DSI_DATAPATH_LOOKUP   = 2
}dsi_qmi_wds_route_lookup_type;

typedef struct
{
  /* Bitmask which indicates which of the below
  ** parameters has been set
  */
  /*Mandatory only valid for route lookup*/
  dsi_qmi_wds_route_lookup_type     route_lookup;

  unsigned long                     params_mask;

  /* Parameters, more will be added later */
  unsigned char                     profile_index;
  unsigned char                     profile_index_3gpp2;
  unsigned char                     tech_pref;
  wds_technology_name_enum_v01      xtended_tech_pref;
  wds_ip_family_preference_enum_v01 ip_family_pref;
  char                              apn_name[QMI_WDS_APN_NAME_MAX_V01 + 1];
  wds_call_type_enum_v01            data_call_origin;
} dsi_qmi_wds_route_look_up_params_type;

typedef struct dsi_qmi_wds_route_look_up_rsp
{
  unsigned long               iface_handle;
  unsigned long               priority;
  wds_technology_name_enum_v01   tech_name;
  unsigned char               qmi_inst_is_valid; /* TRUE if qmi_inst is valid, FALSE if not */
  unsigned char               qmi_inst;
  unsigned char               mux_id_is_valid; /* TRUE if mux_id is valid, FALSE if not */
  unsigned char               mux;
  qmi_result_type_v01         result;
} dsi_qmi_wds_route_look_up_rsp_type;

typedef struct
{
  wds_embms_tmgi_type_v01                   tmgi_list;
  unsigned int                              content_desc_len;
  dsi_embms_content_desc_type              *content_desc_ptr;
  int                                       dbg_trace_id;
}dsi_qmi_wds_embms_content_desc_update_info_type;

/*===========================================================================
  FUNCTION  dsi_qmi_wds_init_qmi_client
===========================================================================*/
/*!
@brief
  Initialize QMI WDS handle

@return
  int - DSI_SUCCESS on successful operation,
        DSI_ERROR if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_wds_init_qmi_client
(
  const char               *dev_str,
  qmi_client_type          *clnt_handle
);


/*===========================================================================
  FUNCTION  dsi_qmi_wds_release_qmi_client
===========================================================================*/
/*!
@brief
  Release QMI WDS handle

@return
  int - DSI_SUCCESS on successful operation,
        DSI_ERROR if there was an error sending QMI message

*/
/*=========================================================================*/
int dsi_qmi_wds_release_qmi_client
(
  qmi_client_type          clnt_hndl
);


/*===========================================================================
    FUNCTION  dsi_qmi_wds_bind_mux_data_port
===========================================================================*/
/*!
@brief
  Bind Mux ID for a give data port instance

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int dsi_qmi_wds_bind_mux_data_port
(
  qmi_client_type clnt_handle,
  const wds_bind_mux_data_port_req_msg_v01 *params
);


/*===========================================================================
  FUNCTION  dsi_qmi_wds_route_look_up
===========================================================================*/
/*!
@brief
  Queries modem for route look up

@return
  int - DSI_SUCCESS on successful operation,
        DSI_ERROR if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_wds_route_look_up
(
  qmi_client_type                        clnt_handle,
  dsi_qmi_wds_route_look_up_params_type *params,
  dsi_qmi_wds_route_look_up_rsp_type    *rsp_data
);


/*===========================================================================
    FUNCTION  dsi_qmi_wds_bind_subscription
===========================================================================*/
/*!
@brief
  Bind the subscription to given configuration

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR     if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_wds_bind_subscription
(
  qmi_client_type                     clnt_handle,
  wds_bind_subscription_enum_v01      subs_id
);


/*===========================================================================
    FUNCTION  dsi_qmi_wds_get_pkt_srvc_status
===========================================================================*/
/*!
@brief
  Get the packet service status from the modem

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int dsi_qmi_wds_get_pkt_srvc_status
(
  qmi_client_type         clnt_handle,
  wds_ip_family_enum_v01  ip_family,
  dsi_link_status_type   *pkt_srvc_status
);


/*===========================================================================
    FUNCTION  dsi_qmi_wds_get_pkt_statistics
===========================================================================*/
/*!
@brief
  Resets the packet statistics

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int dsi_qmi_wds_get_pkt_statistics
(
  qmi_client_type                       clnt_handle,
  wds_stats_mask_v01                    stats_mask,
  wds_get_pkt_statistics_resp_msg_v01  *stats_resp
);


/*===========================================================================
    FUNCTION  dsi_qmi_wds_reset_pkt_statistics
===========================================================================*/
/*!
@brief
  Resets the packet statistics

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int dsi_qmi_wds_reset_pkt_statistics
(
  qmi_client_type clnt_handle
);


/*===========================================================================
    FUNCTION  dsi_qmi_wds_indication_register
===========================================================================*/
/*!
@brief
  Register for indication messages from the modem

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int dsi_qmi_wds_indication_register
(
  qmi_client_type clnt_handle,
  wds_indication_register_req_msg_v01   *params
);


/*===========================================================================
    FUNCTION  dsi_qmi_wds_get_runtime_settings
===========================================================================*/
/*!
@brief
  Queries Modem for runtime settings

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR     if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_wds_get_runtime_settings
(
  qmi_client_type                         clnt_handle,
  wds_req_settings_mask_v01               req_mask,
  wds_get_runtime_settings_resp_msg_v01  *runtime_set
);


/*===========================================================================
    FUNCTION  dsi_qmi_wds_remove_delegated_ipv6_prefix
===========================================================================*/
/*!
@brief
  Removes delegated IPv6 prefix

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR     if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_wds_remove_delegated_ipv6_prefix
(
  qmi_client_type                                clnt_handle,
  const wds_remove_delegated_ipv6_prefix_req_msg_v01  *params
);


/*===========================================================================
    FUNCTION  dsi_qmi_wds_get_current_channel_rate
===========================================================================*/
/*!
@brief
  Gets the current channel rate

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR     if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_wds_get_current_channel_rate
(
  qmi_client_type                                clnt_handle,
  wds_get_current_channel_rate_resp_msg_v01     *ch_rate_resp
);



/*===========================================================================
    FUNCTION  dsi_qmi_wds_get_current_bearer_tech_ex
===========================================================================*/
/*!
@brief
  Gets the data bearer technology

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR     if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_wds_get_current_bearer_tech_ex
(
  qmi_client_type                                 clnt_handle,
  wds_get_data_bearer_technology_ex_resp_msg_v01 *bearer_tech_ex
);


/*===========================================================================
    FUNCTION  dsi_qmi_wds_get_current_bearer_tech
===========================================================================*/
/*!
@brief
  Gets the data bearer technology

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR     if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_wds_get_current_bearer_tech
(
  qmi_client_type                                       clnt_handle,
  wds_get_current_data_bearer_technology_resp_msg_v01  *bearer_tech
);


/*===========================================================================
    FUNCTION  dsi_qmi_wds_embms_tmgi_activate
===========================================================================*/
/*!
@brief
  Send EMBMS TMGI activate message

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int dsi_qmi_wds_embms_tmgi_activate
(
  qmi_client_type                              clnt_handle,
  const wds_embms_tmgi_activate_req_msg_v01   *params,
  int                                         *ext_err_code
);


/*===========================================================================
    FUNCTION  dsi_qmi_wds_embms_tmgi_deactivate
===========================================================================*/
/*!
@brief
  Send EMBMS TMGI deactivate message

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int dsi_qmi_wds_embms_tmgi_deactivate
(
  qmi_client_type                                clnt_handle,
  const wds_embms_tmgi_deactivate_req_msg_v01   *params,
  int                                           *ext_err_code
);


/*===========================================================================
    FUNCTION  dsi_qmi_wds_embms_tmgi_activate_deactivate
===========================================================================*/
/*!
@brief
  Send EMBMS TMGI activate - deactivate message

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int dsi_qmi_wds_embms_tmgi_activate_deactivate
(
  qmi_client_type                               clnt_handle,
  const wds_embms_tmgi_act_deact_req_msg_v01   *params,
  int                                          *ext_err_code
);


/*===========================================================================
    FUNCTION  dsi_qmi_wds_embms_tmgi_list_query
===========================================================================*/
/*!
@brief
  Queries modem for EMBMS TMGI list

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int dsi_qmi_wds_embms_tmgi_list_query
(
  qmi_client_type                         clnt_handle,
  const wds_embms_tmgi_list_query_req_msg_v01  *params,
  wds_embms_tmgi_list_query_resp_msg_v01 *list_info
);


/*===========================================================================
    FUNCTION  dsi_qmi_wds_embms_content_desc_update
===========================================================================*/
/*!
@brief
  Send EMBMS content description updaate message

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_wds_embms_content_desc_update
(
  qmi_client_type                                   clnt_handle,
  const dsi_qmi_wds_embms_content_desc_update_info_type  *params
);

/*===========================================================================
    FUNCTION  dsi_qmi_wds_embms_svc_interest
===========================================================================*/
/*!
@brief
  Send EMBMS service interest info message

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_wds_embms_svc_interest
(
  qmi_client_type                                  clnt_handle,
  const wds_embms_svc_interest_info_req_msg_v01   *params
);

/*===========================================================================
    FUNCTION  dsi_qmi_wds_keep_alive_data_session
===========================================================================*/
/*!
@brief
  Send data session keep alive message

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_wds_keep_alive_data_session
(
  qmi_client_type                                  clnt_handle,
  const wds_keep_alive_data_session_req_msg_v01    *params
);

#endif /* DSI_NETCTRL_WDS_H */
