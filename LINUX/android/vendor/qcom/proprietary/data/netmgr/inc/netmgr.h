/******************************************************************************

                                N E T M G R  . H

******************************************************************************/

/******************************************************************************

  @file    netmgr.h
  @brief   Network Manager public header file

  DESCRIPTION
  Public header file for Network Manager daemon

******************************************************************************/
/*===========================================================================

  Copyright (c) 2010-2015, 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/******************************************************************************

                      EDIT HISTORY FOR FILE

  $Id:$

when       who        what, where, why
--------   ---        -------------------------------------------------------
04/12/10   ar         Initial version

******************************************************************************/

#ifndef __NETMGR_H__
#define __NETMGR_H__

/* Undo DSS namespace clash workaround */
#ifdef cmsg_nxthdr
#undef cmsg_nxthdr
#endif

#include <sys/socket.h>
#include <string.h>

/*===========================================================================
                     GLOBAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

typedef signed char  netmgr_op_status_t;
#define NETMGR_SUCCESS (0)
#define NETMGR_FAILURE (-1)

#define NETMGR_INVALID_IFACE -1

#define NETMGR_NL_MULTICAST_SEND (0)

/*---------------------------------------------------------------------------
   Constant representing maximum length of interface/device name
---------------------------------------------------------------------------*/
#define NETMGR_IF_NAME_MAX_LEN  16 //IFNAMSIZ

/*---------------------------------------------------------------------------
   Constant representing maximum timeout to be used for waiting on a poll
   event. Timeout value is in milliseconds.
---------------------------------------------------------------------------*/
#define NETMGR_UNIX_MESSAGE_RECV_TIMEOUT  5000

/*---------------------------------------------------------------------------
   Type representing enumeration of NetLink event indication messages
---------------------------------------------------------------------------*/
typedef enum netmgr_nl_events_e {
  NET_PLATFORM_INVALID_EV,         /* Internal value                       */
  NET_PLATFORM_UP_EV,              /* Network interface entered UP state   */
  NET_PLATFORM_DOWN_EV,            /* Network interface entered DOWN state */
  NET_PLATFORM_RECONFIGURED_EV,    /* Network interface reconfigured       */
  NET_PLATFORM_FLOW_ACTIVATED_EV,  /* QoS flow entered ACTIVATED state     */
  NET_PLATFORM_FLOW_DELETED_EV,    /* QoS flow entered DELETED state       */
  NET_PLATFORM_FLOW_MODIFIED_EV,   /* QoS flow entered MODIFIED state      */
  NET_PLATFORM_FLOW_SUSPENDED_EV,  /* QoS flow entered SUSPENDED state     */
  NET_PLATFORM_FLOW_ENABLED_EV,    /* QoS flow datapath enabled            */
  NET_PLATFORM_FLOW_DISABLED_EV,   /* QoS flow datapath disabled           */
  NET_PLATFORM_RESET_EV,           /* Process satte reset occurred         */
  NET_PLATFORM_NEWADDR_EV,         /* Network interface address updated    */
  NET_PLATFORM_DELADDR_EV,         /* Network interface address removed    */
  NETMGR_READY_REQ,                /* NETMGR readiness query               */
  NETMGR_READY_RESP,               /* NETMGR readiness response            */
  NETMGR_USER_CMD,                 /* NETMGR user generated command        */
  NET_PLATFORM_MTU_UPDATE_EV,      /* Network interface MTU updated        */
  NET_PLATFORM_MAX_EV              /* Internal value                       */
} netmgr_nl_events_t;

/*---------------------------------------------------------------------------
   Type representing enumeration of network link/interface identifiers
---------------------------------------------------------------------------*/
typedef enum netmgr_link_id_e {
  /* Forward Rmnet ports */
  NETMGR_LINK_RMNET_0,
  NETMGR_LINK_RMNET_1,
  NETMGR_LINK_RMNET_2,
  NETMGR_LINK_RMNET_3,
  NETMGR_LINK_RMNET_4,
  NETMGR_LINK_RMNET_5,
  NETMGR_LINK_RMNET_6,
  NETMGR_LINK_RMNET_7,
  NETMGR_LINK_RMNET_8,
  NETMGR_LINK_RMNET_9,
  NETMGR_LINK_RMNET_10,

  /* Forward Rmnet MDM ports */
  NETMGR_LINK_RMNET_11,
  NETMGR_LINK_RMNET_12,
  NETMGR_LINK_RMNET_13,
  NETMGR_LINK_RMNET_14,
  NETMGR_LINK_RMNET_15,
  NETMGR_LINK_RMNET_16,
  NETMGR_LINK_RMNET_17,
  NETMGR_LINK_RMNET_18,
  NETMGR_LINK_RMNET_19,
  NETMGR_LINK_RMNET_20,
  NETMGR_LINK_RMNET_21,

#ifdef FEATURE_DATA_IWLAN
  /* Reverse Rmnet MSM ports */
  NETMGR_LINK_REV_RMNET_0,
  NETMGR_LINK_REV_RMNET_1,
  NETMGR_LINK_REV_RMNET_2,
  NETMGR_LINK_REV_RMNET_3,
  NETMGR_LINK_REV_RMNET_4,
  NETMGR_LINK_REV_RMNET_5,
  NETMGR_LINK_REV_RMNET_6,
  NETMGR_LINK_REV_RMNET_7,
  NETMGR_LINK_REV_RMNET_8,

  /* Reverse Rmnet MDM ports */
  NETMGR_LINK_REV_RMNET_9,
  NETMGR_LINK_REV_RMNET_10,
  NETMGR_LINK_REV_RMNET_11,
  NETMGR_LINK_REV_RMNET_12,
  NETMGR_LINK_REV_RMNET_13,
  NETMGR_LINK_REV_RMNET_14,
  NETMGR_LINK_REV_RMNET_15,
  NETMGR_LINK_REV_RMNET_16,
  NETMGR_LINK_REV_RMNET_17,
#endif /* FEATURE_DATA_IWLAN */

  NETMGR_LINK_MAX,
  NETMGR_LINK_NONE,                     /* Used for link-independent event */
  NETMGR_LINK_FORCE_32_BIT_SIGNED = 0x7FFFFFFF        /* Force to int size */
} netmgr_link_id_t;

/*---------------------------------------------------------------------------
   Netmgr user commands
---------------------------------------------------------------------------*/
#define NETMGR_USER_CMD_SCREEN_OFF 0
#define NETMGR_USER_CMD_SCREEN_ON  1
#define NETMGR_USER_CMD_ENABLE_PORT_FORWARDING  2
#define NETMGR_USER_CMD_DISABLE_PORT_FORWARDING 3
#define NETMGR_USER_CMD_QUERY_PORT_FORWARDING   4
#define NETMGR_USER_CMD_QUERY_ROUTE_LOOK_UP     5
#define NETMGR_USER_CMD_LINK_RELEASE            6

/*---------------------------------------------------------------------------
   Type representing NetLink event indication payload
---------------------------------------------------------------------------*/
#define NETMGR_EVT_PARAM_NONE         (0x0000)
#define NETMGR_EVT_PARAM_LINK         (0x0001)
#define NETMGR_EVT_PARAM_FLOWINFO     (0x0002)
#define NETMGR_EVT_PARAM_ADDRINFO     (0x0004)
#define NETMGR_EVT_PARAM_GTWYINFO     (0x0008)
#define NETMGR_EVT_PARAM_DNSPADDR     (0x0010)
#define NETMGR_EVT_PARAM_DNSSADDR     (0x0020)
#define NETMGR_EVT_PARAM_DEVNAME      (0x0040)
#define NETMGR_EVT_PARAM_USER_CMD     (0x0080)
#define NETMGR_EVT_PARAM_CMD_DATA     (0x0100)
#define NETMGR_EVT_PARAM_MTU          (0x0200)
#define NETMGR_EVT_PARAM_MASK         ( NETMGR_EVT_PARAM_LINK     | \
                                        NETMGR_EVT_PARAM_FLOW     | \
                                        NETMGR_EVT_PARAM_ADDRINFO | \
                                        NETMGR_EVT_PARAM_GTWYINFO | \
                                        NETMGR_EVT_PARAM_DNSPINFO | \
                                        NETMGR_EVT_PARAM_DNSSINFO | \
                                        NETMGR_EVT_PARAM_DEVNAME  | \
                                        NETMGR_EVT_PARAM_USR_CMD  | \
                                        NETMGR_EVT_PARAM_MTU )
#define NETMGR_EVT_PARAM_IPADDR       (0x1000 | NETMGR_EVT_PARAM_ADDRINFO)
#define NETMGR_EVT_PARAM_CACHE        (0x2000 | NETMGR_EVT_PARAM_ADDRINFO)

#define NETMGR_QMI_WDS_APN_NAME_MAX_V01 150

/* Must match layout of ifa_cacheinfo */
typedef struct netmgr_nl_cacheinfo_s {
  unsigned int   prefered;
  unsigned int   valid;
  unsigned int   cstamp;
  unsigned int   tstamp;
} netmgr_nl_cacheinfo_t;

typedef struct netmgr_nl_addr_s {
  struct sockaddr_storage        ip_addr;
  unsigned int                   mask;
} netmgr_nl_addr_t;

typedef struct netmgr_nl_addrinfo_s {
  unsigned int                   flags;
  netmgr_nl_addr_t               addr;
  netmgr_nl_cacheinfo_t          cache_info;
} netmgr_nl_addrinfo_t;

#define NETMGR_FLOW_TYPE_INVALID      (-1)

typedef struct netmgr_nl_flowinfo_s {
  unsigned int                     flow_id;
  char                             flow_type;
} netmgr_nl_flowinfo_t;

#define  NETMGR_USER_CMD_STATUS_FAILED   -1
#define  NETMGR_USER_CMD_STATUS_SUCCESS  1

typedef struct netmgr_usr_cmd_txn_s
{
  unsigned int pid;
  unsigned int txn_id;
  int txn_status;
}netmgr_usr_cmd_txn_t;

typedef enum
{
  NETMGR_INTERFACE_LOOKUP  = 1,
  NETMGR_DATAPATH_LOOKUP   = 2
}netmgr_qmi_wds_route_lookup_type;

typedef enum netmgr_call_param_app_type
{
  NETMGR_CALL_PARAM_APP_TYPE_INVALID = -1,
  NETMGR_CALL_PARAM_APP_TYPE_DEFAULT = 0,
  NETMGR_CALL_PARAM_APP_TYPE_INTERNET = NETMGR_CALL_PARAM_APP_TYPE_DEFAULT,
  NETMGR_CALL_PARAM_APP_TYPE_IMS
} netmgr_param_app_type_t;

typedef struct
{
  /* Bitmask which indicates which of the below
  ** parameters has been set
  */
  /*Mandatory only valid for route lookup*/
  netmgr_qmi_wds_route_lookup_type     route_lookup;

  unsigned long                        params_mask;

  /* Parameters, more will be added later */
  unsigned char                        profile_index;
  unsigned char                        profile_index_3gpp2;
  unsigned char                        tech_pref;
  /*xtended_tech_pref is wds_technology_name_enum_v01*/
  int                                  xtended_tech_pref;
  /*ip_family_pref is wds_ip_family_preference_enum_v01*/
  int                                  ip_family_pref;
  /*NETMGR_QMI_WDS_APN_NAME_MAX_V01 is same as QMI_WDS_APN_NAME_MAX_V01*/
  char                                 apn_name[NETMGR_QMI_WDS_APN_NAME_MAX_V01 + 1];
  /*data_call_origin is wds_call_type_enum_v01*/
  int                                  data_call_origin;
} netmgr_qmi_wds_route_look_up_params_type;

typedef struct netmgr_qmi_wds_route_look_up_rsp
{
  unsigned long               iface_handle;
  unsigned long               priority;
  /*tech_name is wds_technology_name_enum_v01*/
  int                         tech_name;
  unsigned char               qmi_inst_is_valid; /* TRUE if qmi_inst is valid, FALSE if not */
  unsigned char               qmi_inst;
  unsigned char               mux_id_is_valid; /* TRUE if mux_id is valid, FALSE if not */
  unsigned char               mux;
  /*result is qmi_result_type_v01*/
  int                         result;
} netmgr_qmi_wds_route_look_up_rsp_type;

typedef struct netmgr_qmi_wds_route_look_up_dsi_rsp
{
  unsigned int is_qmi_inst_valid;
  unsigned int is_companion_ip;
  int qmi_inst;
  int link;
  int pmodem;
  int ptech;
} netmgr_qmi_wds_route_look_up_dsi_rsp_type;


typedef struct netmgr_user_cmd_data_s {
  netmgr_usr_cmd_txn_t txn;
  unsigned int cmd_id;

  union{
    struct {
      /* ip_family to enable or disable forwarding
         For QUERY_PORT_FORWARDING cmd it stores the family
              for which forwarding is enabled */
      int ip_family;
    }port_forwarding_data;
  }data;

  unsigned int                                  link;
  netmgr_qmi_wds_route_look_up_params_type      rl_params;
  netmgr_qmi_wds_route_look_up_dsi_rsp_type     rl_resp;
  int                                           app_type;
  int                                           modem_subs_id;
}netmgr_user_cmd_data_t;

typedef struct netmgr_nl_event_info_s {
  unsigned int                     param_mask;
  netmgr_nl_events_t               event;
  netmgr_link_id_t                 link;
  netmgr_nl_flowinfo_t             flow_info;
  netmgr_nl_addrinfo_t             addr_info;
  netmgr_nl_addr_t                 gtwy_info;
  struct sockaddr_storage          dnsp_addr;
  struct sockaddr_storage          dnss_addr;
  char                             dev_name[ NETMGR_IF_NAME_MAX_LEN ];
  unsigned int                     mtu;
  unsigned int                     user_cmd;
  netmgr_user_cmd_data_t           cmd_data;
} netmgr_nl_event_info_t;

typedef unsigned int  netmgr_client_hdl_t;

/*---------------------------------------------------------------------------
  Type representing callback function registered by client subscribing
  for asynchronous event indications.
---------------------------------------------------------------------------*/
typedef void (*netmgr_event_ind_f) ( netmgr_nl_events_t       event,
                                     netmgr_nl_event_info_t * info,
                                     void *                   data );


/*===========================================================================
                            GLOBAL FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  netmgr_client_register
===========================================================================*/
/*!
@brief
  Function to register client for NetMgr asynchronous envents.

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int netmgr_client_register
(
  netmgr_event_ind_f    cb_f,
  void                * data,
  netmgr_client_hdl_t * client_hndl
);

/*===========================================================================
  FUNCTION  netmgr_register_client
===========================================================================*/
/*!
@brief
  Function to register client for NetMgr asynchronous envents.

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int netmgr_client_release
(
  const netmgr_client_hdl_t client_hndl
);

/*===========================================================================
  FUNCTION  netmgr_client_send_ping_msg
===========================================================================*/
/*!
@brief
  Function to query NetMgr for its readiness status. @param timeout is the
  interval to wait for a response.

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
netmgr_client_send_ping_msg
(
  int resp_timeout
);

/*===========================================================================
  FUNCTION  netmgr_client_send_user_cmd
===========================================================================*/
/*!
@brief
  Function to send proprietary commands to netmgr. @param timeout is the
  interval to wait for a response.

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise

*/
/*=========================================================================*/
int
netmgr_client_send_user_cmd
(
  int                     cmd,
  netmgr_user_cmd_data_t  *cmd_data,
  netmgr_nl_event_info_t  *resp_info,
  int                     resp_timeout
);

#endif /* __NETMGR_H__ */
