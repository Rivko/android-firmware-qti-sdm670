/******************************************************************************

                       N E T M G R _ Q M I _Q O S . H

******************************************************************************/

/******************************************************************************

  @file    netmgr_qmi_qos.h
  @brief   Network Manager QMI quality of service header

  DESCRIPTION
  Network Manager QMI quality of service header

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#ifndef __NETMGR_QMI_QOS_H__
#define __NETMGR_QMI_QOS_H__
#include <stdint.h>
#include "qmi_client.h"
#include "qmi.h"
#include "quality_of_service_v01.h"


#define NETMGR_QOS_MAX_FLOW_FILTER     16
#define NETMGR_QOS_MAX_INDS          NETMGR_QOS_MAX_FLOW_FILTER
#define NETMGR_QOS_MAX_FLOW_EVENTS   8
#define NETMGR_QOS_MAX_PROFILES      8
#define NETMGR_QOS_MAX_ERR_CODES_IN_RSP    10
#define NETMGR_QOS_IPV6_ADDR_SIZE_IN_BYTES 16

typedef unsigned char netmgr_qos_ipv6_addr_type[NETMGR_QOS_IPV6_ADDR_SIZE_IN_BYTES];

/* typedefs related to flow descriptors */
typedef enum
{
  NETMGR_QOS_UMTS_TC_CONVERSATIONAL = 0,
  NETMGR_QOS_UMTS_TC_STREAMING      = 1,
  NETMGR_QOS_UMTS_TC_INTERACTIVE    = 2,
  NETMGR_QOS_UMTS_TC_BACKGROUND     = 3
} netmgr_qos_umts_traffic_class_type;

typedef enum
{
  NETMGR_QOS_UMTS_BER_5X10_NEG2 = 0,
  NETMGR_QOS_UMTS_BER_1X10_NEG2 = 1,
  NETMGR_QOS_UMTS_BER_5X10_NEG3 = 2,
  NETMGR_QOS_UMTS_BER_4X10_NEG3 = 3,
  NETMGR_QOS_UMTS_BER_1X10_NEG3 = 4,
  NETMGR_QOS_UMTS_BER_1X10_NEG4 = 5,
  NETMGR_QOS_UMTS_BER_1X10_NEG5 = 6,
  NETMGR_QOS_UMTS_BER_1X10_NEG6 = 7,
  NETMGR_QOS_UMTS_BER_6X10_NEG8 = 8
} netmgr_qos_umts_residual_ber_type;


typedef enum
{
  NETMGR_QOS_UMTS_PRIO_1 = 0,
  NETMGR_QOS_UMTS_PRIO_2 = 1,
  NETMGR_QOS_UMTS_PRIO_3 = 2
} netmgr_qos_umts_handling_prio_type;


typedef enum
{
  NETMGR_QOS_UMTS_TECH_TYPE,
  NETMGR_QOS_CDMA_TECH_TYPE,
  NETMGR_QOS_DVBH_TECH_TYPE,
  NETMGR_QOS_MBMS_TECH_TYPE,
  NETMGR_QOS_BCMCS_TECH_TYPE,
  NETMGR_QOS_MEDIAFLO_TECH_TYPE,
  NETMGR_QOS_INVALID_TECH_TYPE = 0xFFFF
}netmgr_qos_technology_type;

typedef int             netmgr_qos_dss_errno;
#define NETMGR_QOS_EXTENDED_ERROR_INFO_PARAM       0x0001

typedef struct
{
  unsigned short              param_mask;
  netmgr_qos_dss_errno           dss_errno;
}netmgr_qos_dss_errno_type;


typedef struct
{
  unsigned long max_rate;
  unsigned long guaranteed_rate;
} netmgr_qos_umts_data_rate_type;


typedef struct
{
  unsigned long peak_rate;
  unsigned long token_rate;
  unsigned long bucket_size;
} netmgr_qos_umts_token_bucket_type;

typedef struct
{
  unsigned long multiplier;
  unsigned long exponent;
} netmgr_qos_umts_packet_err_rate_type;

typedef enum
{
  NETMGR_QOS_FALSE   = 0x00,
  NETMGR_QOS_TRUE    = 0x01
}netmgr_qos_bool_type;

#define NETMGR_QOS_UMTS_FLOW_PARAM_TRAFFIC_CLASS      0x0000000000000001ull
#define NETMGR_QOS_UMTS_FLOW_PARAM_DATA_RATE          0x0000000000000002ull
#define NETMGR_QOS_UMTS_FLOW_PARAM_BUCKET_INFO        0x0000000000000004ull
#define NETMGR_QOS_UMTS_FLOW_PARAM_MAX_DELAY          0x0000000000000008ull
#define NETMGR_QOS_UMTS_FLOW_PARAM_MAX_JITTER         0x0000000000000010ull
#define NETMGR_QOS_UMTS_FLOW_PARAM_PKT_ERR_RATE       0x0000000000000020ull
#define NETMGR_QOS_UMTS_FLOW_PARAM_MIN_POL_PKT_SZ     0x0000000000000040ull
#define NETMGR_QOS_UMTS_FLOW_PARAM_MAX_ALLOW_PKT_SZ   0x0000000000000080ull
#define NETMGR_QOS_UMTS_FLOW_PARAM_RESIDUAL_BER       0x0000000000000100ull
#define NETMGR_QOS_UMTS_FLOW_PARAM_HANDLING_PRIO      0x0000000000000200ull
#define NETMGR_QOS_CDMA_FLOW_PARAM_PROFILE_ID         0x0000000000000400ull
#define NETMGR_QOS_UMTS_FLOW_PARAM_3GPP2_FLOW_PRIO    0x0000000000000800ull
#define NETMGR_QOS_UMTS_FLOW_PARAM_IM_CN_FLAG         0x0000000000001000ull
#define NETMGR_QOS_UMTS_FLOW_PARAM_IP_FLOW_SIG_IND    0x0000000000002000ull
#define NETMGR_QOS_LTE_FLOW_PARAM_QCI_IND             0x0000000000004000ull


/* This bit definition is overloaded.  But since this is a CDMA
** definition which is exclusive of the UMTS definitions, this
** is OK.  This is only used in the error reporting mask, not
** in QoS initiation, sice the CMDA profile ID is a required
** parameter for CDMA technology
*/
#define NETMGR_QOS_CDMA_FLOW_PARAM_PROFILE_ID_ERR  NETMGR_QOS_UMTS_FLOW_PARAM_TRAFFIC_CLASS


typedef struct
{
  /* Mask of which optional parameters below are valid */
  unsigned short param_mask;

  /* UMTS optional QoS parameters.  At least one of these
  ** must be included in a setup request
  */
  netmgr_qos_umts_traffic_class_type     traffic_class;
  netmgr_qos_umts_data_rate_type         data_rate;
  netmgr_qos_umts_token_bucket_type      bucket_info;
  unsigned long                       max_delay;
  unsigned long                       max_jitter;
  netmgr_qos_umts_packet_err_rate_type   pkt_err_rate;
  unsigned long                       min_policed_pkt_sz;
  unsigned long                       max_allowed_pkt_sz;
  netmgr_qos_umts_residual_ber_type      residual_ber;
  netmgr_qos_umts_handling_prio_type     handling_prio;
  unsigned char                       flow_priority_3gpp2;
  netmgr_qos_bool_type                   im_cn_flag;/*Valid only for 3gpp*/
  netmgr_qos_bool_type                   ip_flow_sig_ind;/*valid only for 3gpp*/
  unsigned char                       lte_qci;
} netmgr_qos_umts_flow_desc_type;

typedef struct
{
  /* Mask of which optional parameters below are valid */
  unsigned short param_mask;

  unsigned long profile_id;
} netmgr_qos_cdma_flow_desc_type;

typedef struct
{
  netmgr_qos_cdma_flow_desc_type   cdma_flow_desc;
  netmgr_qos_umts_flow_desc_type   umts_flow_desc;
} netmgr_qos_flow_req_type;


/* typedefs related to filter descriptors */
typedef struct
{
  unsigned long ipv4_ip_addr;
  unsigned long ipv4_subnet_mask;
} netmgr_qos_ipv4_addr_filter_type;

typedef struct
{
  netmgr_qos_ipv6_addr_type   ipv6_ip_addr;
  unsigned char            ipv6_filter_prefix_len;
}netmgr_qos_ipv6_addr_filter_type;

typedef struct
{
  unsigned char tos_value;
  unsigned char tos_mask;
} netmgr_qos_tos_filter_type;


typedef struct
{
  unsigned short start_port;
  unsigned short range;
} netmgr_qos_port_range_filter_type;

typedef enum
{
  NETMGR_QOS_TRANS_PROT_TCP     = 0x06,
  NETMGR_QOS_TRANS_PROT_UDP     = 0x11,
  NETMGR_QOS_TRANS_PROT_TCP_UDP = 0xFD
} netmgr_qos_trans_protocol_filter_type;

typedef enum
{
  NETMGR_QOS_IP_VERSION_4 = 0x04,
  NETMGR_QOS_IP_VERSION_6 = 0x06
} netmgr_qos_ip_version_type;


#define NETMGR_QOS_FILTER_PARAM_SRC_ADDR             0x00000001
#define NETMGR_QOS_FILTER_PARAM_DEST_ADDR            0x00000002
#define NETMGR_QOS_FILTER_PARAM_TRANS_PROTOCOL       0x00000004
#define NETMGR_QOS_FILTER_PARAM_TOS                  0x00000008
#define NETMGR_QOS_FILTER_PARAM_TCP_SRC_PORTS        0x00000010
#define NETMGR_QOS_FILTER_PARAM_TCP_DEST_PORTS       0x00000020
#define NETMGR_QOS_FILTER_PARAM_UDP_SRC_PORTS        0x00000040
#define NETMGR_QOS_FILTER_PARAM_UDP_DEST_PORTS       0x00000080
#define NETMGR_QOS_FILTER_PARAM_IPV6_SRC_ADDR        0x00000100
#define NETMGR_QOS_FILTER_PARAM_IPV6_DEST_ADDR       0x00000200
#define NETMGR_QOS_FILTER_PARAM_IPV6_TRAFFIC_CLASS   0x00000400
#define NETMGR_QOS_FILTER_PARAM_IPV6_FLOW_LABEL      0x00000800
#define NETMGR_QOS_FILTER_PARAM_ESP_SECURITY_POLICY  0x00001000
#define NETMGR_QOS_FILTER_PARAM_PRECEDENCE           0x00002000
#define NETMGR_QOS_FILTER_PARAM_FILTER_ID            0x00004000
#define NETMGR_QOS_FILTER_PARAM_TRANSPORT_SRC_PORTS  0x00008000
#define NETMGR_QOS_FILTER_PARAM_TRANSPORT_DEST_PORTS 0x00010000

/* This parameter is only used for error report mask, since
** thie IP version is a required field for all requests
*/
#define NETMGR_QOS_FILTER_PARAM_IP_VER_ERR      0x80000000



typedef struct
{
  /* Mask of which optional parameters below are valid */
  unsigned long param_mask;

  /* UMTS optional QoS parameters.  At least one of these
  ** must be included in a setup request
  */
  netmgr_qos_ipv4_addr_filter_type       src_addr;
  netmgr_qos_ipv4_addr_filter_type       dest_addr;
  netmgr_qos_trans_protocol_filter_type  protocol;
  netmgr_qos_tos_filter_type             tos;
  netmgr_qos_port_range_filter_type      tcp_src_ports;
  netmgr_qos_port_range_filter_type      tcp_dest_ports;
  netmgr_qos_port_range_filter_type      udp_src_ports;
  netmgr_qos_port_range_filter_type      udp_dest_ports;
  netmgr_qos_ipv6_addr_filter_type       ipv6_src_addr;
  netmgr_qos_ipv6_addr_filter_type       ipv6_dest_addr;
  struct
  {
    unsigned char     traffic_class_value;
    unsigned char     traffic_class_mask;
  }ipv6_traffic_class;
  unsigned long                       ipv6_flow_label;
  unsigned long                       esp_security_policy_index;
  unsigned short                       precedence;
  unsigned short                       filter_id;
  netmgr_qos_port_range_filter_type      transport_src_ports;
  netmgr_qos_port_range_filter_type      transport_dest_ports;

} netmgr_qos_filter_desc_type;


/* This is the filter request data structure.  Note that the ip_version
** parameter is required and must be set for each request.  The filter_desc
** optional parameters must follow rules outlined in QC QoS document
*/
typedef struct
{
  netmgr_qos_ip_version_type        ip_version;
  netmgr_qos_filter_desc_type         filter_desc;
} netmgr_qos_filter_req_type;

typedef enum
{
  NETMGR_QOS_UNASSIGNED_TYPE   = 0x00,
  NETMGR_QOS_REQUEST           = 0x01,
  NETMGR_QOS_CONFIGURE         = 0x02
}netmgr_qos_req_opcode_type;

/* This is the main QoS specification data structure.  For each
** of the request types, there is a count of the number
** request elements provided (can be an array of elements,
** treated in priority order) and a pointer to the request
** elements.
*/
typedef struct
{
  unsigned long             qos_identifier;     /*To be used only for QOS Modify*/

  unsigned long             num_tx_flow_req;
  netmgr_qos_flow_req_type     *tx_flow_req_array;

  unsigned long             num_rx_flow_req;
  netmgr_qos_flow_req_type     *rx_flow_req_array;

  unsigned long             num_tx_filter_req;
  netmgr_qos_filter_req_type   *tx_filter_req_array;

  unsigned long             num_rx_filter_req;
  netmgr_qos_filter_req_type   *rx_filter_req_array;

} netmgr_qos_spec_type;

/* QOS request error response data type.  If the err_present flag is
** set then one or more of the included error masks will have bit(s)
** set.  The bits will indicate which of the required or optional
** parameters had errors for each of the request types.  The array
** elements correspond to the possible array of requests passed in
** in the request.
*/
typedef struct
{
  int            errs_present;
  unsigned short tx_flow_req_err_mask[NETMGR_QOS_MAX_FLOW_FILTER];
  unsigned short rx_flow_req_err_mask[NETMGR_QOS_MAX_FLOW_FILTER];
  unsigned short tx_filter_req_err_mask[NETMGR_QOS_MAX_FLOW_FILTER];
  unsigned short rx_filter_req_err_mask[NETMGR_QOS_MAX_FLOW_FILTER];
} netmgr_qos_err_rsp_type;

/************************************************************************
* Definitions associated with netmgr_qos_get_nw_supported_qos_profiles()
************************************************************************/
typedef enum
{
  NETMGR_QOS_INVALID_IFACE_NAME = 0x0000,

  NETMGR_QOS_ANY_DEFAULT_GROUP  = 0x0001,
  NETMGR_QOS_WWAN_GROUP         = 0x0002,
  NETMGR_QOS_RM_GROUP           = 0x0004,
  NETMGR_QOS_BCAST_MCAST_GROUP  = 0x0008,
  NETMGR_QOS_IFACE_3GPP_GROUP   = 0x0010,
  NETMGR_QOS_IFACE_3GPP2_GROUP  = 0x0020,
  NETMGR_QOS_IFACE_EPC_GROUP    = 0x0040,
  NETMGR_QOS_ANY_IFACE_GROUP    = 0x7FFF,

  NETMGR_QOS_IFACE_MASK         = 0x8000,
  NETMGR_QOS_CDMA_SN_IFACE      = 0x8001,
  NETMGR_QOS_CDMA_AN_IFACE      = 0x8002,
  NETMGR_QOS_UMTS_IFACE         = 0x8004,
  NETMGR_QOS_SIO_IFACE          = 0x8008,
  NETMGR_QOS_CDMA_BCAST_IFACE   = 0x8010,
  NETMGR_QOS_WLAN_IFACE         = 0x8020,
  NETMGR_QOS_DUN_IFACE          = 0x8040,
  NETMGR_QOS_FLO_IFACE          = 0x8080,
  NETMGR_QOS_DVBH_IFACE         = 0x8100,
  NETMGR_QOS_STA_IFACE          = 0x8200,
  NETMGR_QOS_IPSEC_IFACE        = 0x8400,
  NETMGR_QOS_LO_MODE_B_IFACE    = 0x8401,
  NETMGR_QOS_LO_IFACE           = 0x8800,
  NETMGR_QOS_MBMS_IFACE         = 0x8801,
  NETMGR_QOS_IWLAN_3GPP_IFACE   = 0x8802,
  NETMGR_QOS_IWLAN_3GPP2_IFACE  = 0x8804,
  NETMGR_QOS_MIP6_IFACE         = 0x8808,
  NETMGR_QOS_SLIP_IFACE         = 0x8810,
  NETMGR_QOS_UICC_IFACE         = 0x8820,
  NETMGR_QOS_UW_FMC_IFACE       = 0x8840,
  NETMGR_QOS_EPC_IFACE          = 0x8880,
  NETMGR_QOS_NAT_IFACE          = 0x8881
} netmgr_qos_iface_name_type;

typedef enum
{
  NETMGR_QOS_PROFILE_TECH_3GPP  = 0x01,
  NETMGR_QOS_PROFILE_TECH_3GPP2 = 0x02
} netmgr_qos_profile_tech_type;

typedef struct
{
  netmgr_qos_iface_name_type     iface_type;
  unsigned char               num_profiles;
  unsigned short              profile[NETMGR_QOS_MAX_PROFILES]; //TODO:?
}netmgr_qos_profile_info_type;

typedef struct
{
  unsigned char             num_instances;
  netmgr_qos_profile_info_type profile_info[NETMGR_QOS_MAX_PROFILES];
}netmgr_qos_nw_supported_qos_profiles;


typedef struct
{
  netmgr_qos_nw_supported_qos_profiles     qos_profiles;
  netmgr_qos_dss_errno_type                  dss_err;
}netmgr_qos_nw_supported_qos_profiles_rsp_type;

/*Event Report related structures*/

/* For turning event reporting on/off */
#define NETMGR_QOS_GLOBAL_FLOW_REPORTING_STATE_PARAM             0x0001
#define NETMGR_QOS_NW_SUPPORTED_PROFILE_CHANGE_STATE_PARAM       0x0002

typedef enum
{
  NETMGR_QOS_EVENT_REPORTING_OFF = 0,
  NETMGR_QOS_EVENT_REPORTING_ON = 1
} netmgr_qos_global_flow_reporting_state_type;

typedef enum
{
  NETMGR_QOS_PROFILE_CHANGE_REPORTING_ENABLE   = 0x01,
  NETMGR_QOS_PROFILE_CHANGE_REPORTING_DISABLE  = 0x00
}netmgr_qos_profile_change_status;

typedef struct
{
  netmgr_qos_profile_change_status      profile_change_status;
  netmgr_qos_iface_name_type            iface_type;/*IMPORTANT: USE ONLY "3GPP2"*/
}netmgr_qos_net_supported_profile_change_type;

typedef struct
{
  unsigned short               param_mask;

  netmgr_qos_global_flow_reporting_state_type      flow_state;
  netmgr_qos_net_supported_profile_change_type     profile_change_state;

}netmgr_qos_event_report_state_type;

/*Status report set response enum*/

typedef enum
{
  NETMGR_QOS_STATUS_ACTIVATED = 0x01,
  NETMGR_QOS_STATUS_SUSPENDED = 0x02,
  NETMGR_QOS_STATUS_GONE = 0x03
} netmgr_qos_status_info;

typedef enum
{
  NETMGR_QOS_ACTIVATED_EV          = 0x01,
  NETMGR_QOS_SUSPENDED_EV          = 0x02,
  NETMGR_QOS_GONE_EV               = 0x03,
  NETMGR_QOS_MODIFY_ACCEPTED_EV    = 0x04,
  NETMGR_QOS_MODIFY_REJECTED_EV    = 0x05,
  NETMGR_QOS_INFO_CODE_UPDATED_EV  = 0x06,
} netmgr_qos_status_event;

typedef enum
{
  NETMGR_QOS_INVALID_PARAMS                          = 0x01,
  NETMGR_QOS_INTERNAL_CALL_ENDED                     = 0x02,
  NETMGR_QOS_INTERNAL_ERROR                          = 0x03,
  NETMGR_QOS_INSUFFICIENT_LOCAL_RESOURCES            = 0x04,
  NETMGR_QOS_TIMED_OUT_OPERATION                     = 0x05,
  NETMGR_QOS_INTERNAL_UNKNOWN_CAUSE_CODE             = 0x06,
  NETMGR_QOS_INTERNAL_MODIFY_IN_PROGRESS             = 0x07,
  NETMGR_QOS_NOT_SUPPORTED                           = 0x08,
  NETMGR_QOS_NOT_AVAILABLE                           = 0x09,
  NETMGR_QOS_NOT_GUARANTEED                          = 0x0A,
  NETMGR_QOS_INSUFFICIENT_NETWORK_RESOURCES          = 0x0B,
  NETMGR_QOS_AWARE_SYSTEM                            = 0x0C,
  NETMGR_QOS_UNAWARE_SYSTEM                          = 0x0D,
  NETMGR_QOS_REJECTED_OPERATION                      = 0x0E,
  NETMGR_QOS_WILL_GRANT_WHEN_QOS_RESUMED             = 0x0F,
  NETMGR_QOS_NETWORK_CALL_ENDED                      = 0x10,
  NETMGR_QOS_NETWORK_SERVICE_NOT_AVAILABLE           = 0x11,
  NETMGR_QOS_NETWORK_L2_LINK_RELEASED                = 0x12,
  NETMGR_QOS_NETWORK_L2_LINK_REESTAB_REJ             = 0x13,
  NETMGR_QOS_NETWORK_L2_LINK_REESTAB_IND             = 0x14,
  NETMGR_QOS_NETWORK_UNKNOWN_CAUSE_CODE              = 0x15,
  NETMGR_QOS_NETWORK_BUSY                            = 0x16,
  NETMGR_QOS_NETWORK_DISJOINT_PROFILE_SET_SUGGESTED  = 0x17,
  NETMGR_QOS_NETWORK_NULL_PROFILE_SUGGESTED          = 0x18,
  NETMGR_QOS_NETWORK_UE_NOT_AUTHORIZED               = 0x19
} netmgr_qos_reason_code;

typedef struct
{
  struct
  {
    unsigned long          qos_identifier;
    netmgr_qos_status_info    qos_status;
    netmgr_qos_status_event   qos_event;
  }qos_status_information;

  int qos_reason_is_valid;
  netmgr_qos_reason_code   qos_reason;

}netmgr_qos_status_report_type;

/************************************************************************
* Definitions associated with netmgr_qos_get_granted_qos_info()
************************************************************************/

typedef enum
{
  NETMGR_QOS_FLOW_ACTIVATED = 0x00,
  NETMGR_QOS_FLOW_MODIFIED  = 0x01,
  NETMGR_QOS_FLOW_DELETED   = 0x02,
  NETMGR_QOS_FLOW_SUSPENDED = 0x03,
  NETMGR_QOS_FLOW_ENABLED   = 0x04,
  NETMGR_QOS_FLOW_DISABLED  = 0x05
}netmgr_global_flow_state_change;

typedef struct
{
  unsigned short           ip_flow_index;
  netmgr_qos_flow_req_type    qos_flow_granted;
}netmgr_qos_granted_flow_data_type;

typedef struct
{
  unsigned short           filter_index;
  netmgr_qos_filter_req_type    qos_filter;
}netmgr_qos_granted_filter_data_type;


/*current granted qos info*/
typedef struct
{
  int                             tx_granted_flow_data_is_valid;
  netmgr_qos_granted_flow_data_type  tx_granted_flow_data;

  int                             rx_granted_flow_data_is_valid;
  netmgr_qos_granted_flow_data_type  rx_granted_flow_data;

  int                              tx_filter_count;
  netmgr_qos_granted_filter_data_type tx_granted_filter_data[NETMGR_QOS_MAX_FLOW_FILTER];

  int                              rx_filter_count;
  netmgr_qos_granted_filter_data_type rx_granted_filter_data[NETMGR_QOS_MAX_FLOW_FILTER];

  netmgr_qos_dss_errno_type          dss_errno_type;
}netmgr_qos_granted_info_rsp_type;


#define NETMGR_QOS_EVENT_REPORT_FLOW_INFO_FLOW_STATE_PARAM     0x0001
#define NETMGR_QOS_EVENT_REPORT_FLOW_INFO_FLOW_TYPE_PARAM      0x0002

typedef struct
{
  unsigned short          param_mask;

  struct
  {
    unsigned long     qos_identifier;
    unsigned char     new_flow;
    netmgr_global_flow_state_change  report_flow_state_chng;
  } qos_flow_state;

  unsigned char           flow_type;

  /*If the Corresponding bit is set in the event mask*/
  int                     tx_granted_flow_data_is_valid;
  netmgr_qos_granted_flow_data_type   tx_granted_flow_data;

  /*If the Corresponding bit is set in the event mask*/
  int                     rx_granted_flow_data_is_valid;
  netmgr_qos_granted_flow_data_type   rx_granted_flow_data;

  unsigned int            tx_filter_count;/* other suggestions for this variable ? */
  netmgr_qos_granted_filter_data_type  tx_granted_filter_data[NETMGR_QOS_MAX_FLOW_FILTER];

  unsigned int            rx_filter_count;
  netmgr_qos_granted_filter_data_type  rx_granted_filter_data[NETMGR_QOS_MAX_FLOW_FILTER];

}netmgr_qos_event_report_flow_info_type;


/*typedef event report indication structure*/
#define NETMGR_QOS_EVENT_REPORT_NW_SUPPORTED_QOS_PROFILES_PARAM     0x0001
#define NETMGR_QOS_EVENT_REPORT_GLOBAL_FLOW_INFO_PARAM              0x0002

typedef struct
{
  unsigned short                            param_mask; //PARAM MASK for nw_supported_qos_profiles

  int                                       num_flows; // Number of valid flows in the indication message.
  netmgr_qos_event_report_flow_info_type       flow_info[NETMGR_QOS_MAX_FLOW_EVENTS];

  netmgr_qos_profile_info_type                 nw_supported_qos_profiles;

}netmgr_qos_event_report_type;


/* Distinguishes indication message types */
typedef enum
{
  NETMGR_QOS_SRVC_INVALID_IND_MSG,
  NETMGR_QOS_SRVC_EVENT_REPORT_IND_MSG,
  NETMGR_QOS_SRVC_STATUS_REPORT_IND_MSG,
  NETMGR_QOS_SRVC_NW_STATUS_REPORT_IND_MSG,
  NETMGR_QOS_SRVC_PRIMARY_QOS_EVENT_IND_MSG,
  NETMGR_QOS_SRVC_PROFILES_CHANGE_EVENT_IND_MSG
  /* To be filled in in future release */
} netmgr_qos_indication_id_type;

typedef enum
{
  NW_DOESNOT_SUPPORT_QOS = 0x00,
  NW_DOES_SUPPORTS_QOS       = 0x01
} netmgr_qos_nw_support_status;

typedef struct
{
  netmgr_qos_nw_support_status  qos_supported;
  netmgr_qos_iface_name_type    iface_name;
} netmgr_qos_nw_status_type;

typedef enum
{
  NETMGR_QOS_PRIMARY_FLOW_MODIFY_SUCCESS    = 0x0001,
  NETMGR_QOS_PRIMARY_FLOW_MODIFY_FAILURE   = 0x0002
}netmgr_qos_primrary_qos_event_type;


/* Async notification reporting structure */
typedef union
{
  netmgr_qos_nw_status_type            nw_report;
  netmgr_qos_status_report_type        status_report;
  netmgr_qos_event_report_type         event_report;
  netmgr_qos_primrary_qos_event_type   primary_qos_modify_result;
  netmgr_qos_event_report_state_type   event_report_state;
  /* To be filled in in future release */
} netmgr_qos_indication_data_type;


typedef void (*netmgr_qos_indication_hdlr_type)
(
int                           user_handle,
qmi_service_id_type           service_id,
void                          *user_data,
netmgr_qos_indication_id_type    ind_id,
netmgr_qos_indication_data_type  *ind_data
);

/************************************************************************
* Definitions associated with netmgr_qos_bind_mux_data_port()
************************************************************************/

typedef enum
{
  NETMGR_QOS_PER_EP_TYPE_MIN      = 0x00,
  NETMGR_QOS_PER_EP_TYPE_RESERVED = 0x00,
  NETMGR_QOS_PER_EP_TYPE_HSIC     = 0x01,
  NETMGR_QOS_PER_EP_TYPE_HSUSB    = 0x02,
  NETMGR_QOS_PER_EP_TYPE_PCIE     = 0x03,
  NETMGR_QOS_PER_EP_TYPE_EMBEDDED = 0x04,
  NETMGR_QOS_PER_EP_TYPE_MAX
} netmgr_qos_per_ep_type;

/** Structure for specifying EP ID information */
typedef struct
{
  netmgr_qos_per_ep_type ep_type;          /** Peripheral end point type */
  unsigned long       iface_id;         /** Data end-point ID */
} netmgr_qos_per_ep_id_type;

#define NETMGR_QOS_BIND_MUX_DATA_PORT_PARAMS_EP_ID     0x00000001
#define NETMGR_QOS_BIND_MUX_DATA_PORT_PARAMS_MUX_ID    0x00000002
#define NETMGR_QOS_BIND_MUX_DATA_PORT_PARAMS_REVERSED  0x00000004

/** Parameter structure for binding QOS client to a mux data
 *  port */
typedef struct
{
  unsigned long           params_mask;  /** Params mask */
  netmgr_qos_per_ep_id_type  ep_id;        /** EP ID information */
  unsigned char           mux_id;       /** Mux ID to bind to */
  unsigned char           reversed;     /** Whether this is a reverse port */
} netmgr_qos_bind_mux_data_port_params_type;

typedef enum
{
  NETMGR_QOS_DEFAULT_SUBS = 0x0000,
  NETMGR_QOS_PRIMARY_SUBS = 0x0001,
  NETMGR_QOS_SECONDARY_SUBS = 0x0002,
  NETMGR_QOS_TERITIARY_SUBS = 0x0003,
  NETMGR_QOS_DONT_CARE_SUBS = 0x00FF
} netmgr_qos_bind_subscription_type;





typedef union
{
  netmgr_qos_event_report_type  event_report;
} netmgr_qmi_qos_indication_data_type;

/*===========================================================================
  FUNCTION  netmgr_qmi_qos_init_client
===========================================================================*/
/*!
@brief
  Initialize QMI QoS handle

@return
  int - NETMGR_SUCCESS on successful operation,
        NETMGR_FAILURE on error

*/
/*=========================================================================*/
int netmgr_qmi_qos_init_client
(
  int                      link,
  const char               *dev_str,
  qos_ip_family_enum_v01   ip_family,
  qmi_client_type          *clnt_hndl,
  qmi_response_type_v01    *err
);

/*===========================================================================
  FUNCTION  netmgr_qmi_qos_release_qmi_client
===========================================================================*/
/*!
@brief
  Release QMI QoS handle

@return
  int - NETMGR_SUCCESS on successful operation,
        NETMGR_FAILURE on error

*/
/*=========================================================================*/
int netmgr_qmi_qos_release_client
(
  int                      link,
  qmi_client_type          clnt_hndl
);

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
);

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
);

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
);

#endif /* __NETMGR_QMI_QOS_H__ */
