/*!
  @file
  dsi_netctrli.h

  @brief
  This is an internal header file for dsi_netctrl module. It declares
  data used internally by the dsi_netctrl module.
*/

/*===========================================================================

  Copyright (c) 2008-2017 Qualcomm Technologies, Inc.
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
04/19/10   js      modified original to suit multimodem requirements
===========================================================================*/
#ifdef DSI_NETCTRL_OFFTARGET
#include <sys/socket.h>
#include <sys/types.h>
#else
#include <linux/socket.h> /* sockaddr_storage */
#include <log.h>
#endif
#include <pthread.h>
#include <assert.h>
#include <string.h>

#include "qdi.h"
#include "dsi_netctrl.h"
#include "dsi_config.h"
#include "dsi_netctrl_qmi_wds.h"
#include "netmgr.h"

#ifndef _DSI_NETCTRLI_H
#define _DSI_NETCTRLI_H

/* Entry and Exit macros */
#define DSI_LOG_ENTRY  DSI_LOG_DEBUG( "%s: ENTRY", __func__ );
#define DSI_LOG_EXIT                                     \
  if( DSI_SUCCESS == ret ) {                             \
    DSI_LOG_DEBUG( "%s: EXIT with suc", __func__ );      \
  } else {                                               \
    DSI_LOG_DEBUG( "%s: EXIT with err", __func__ );      \
  }

#define DSI_ARR_SIZE(x) sizeof(x)/sizeof(x[0])
/* ---------------------------------------------------------------------------
-----------------------L2S Entry and Exit macros -----------------------------
------------------------------------------------------------------------------*/
#define DSI_L2S_MSG_MAX_SIZE   256
#define DSI_L2S_PNAME_SIZE     50
#define DSI_L2S_INVALID_IFACE  -1

extern char l2s_proc_name[DSI_L2S_PNAME_SIZE];
extern int  l2s_pid;

#define DSI_L2S_SETUP()                                                      \
  do{                                                                        \
    l2s_pid = -1;                                                            \
    memset(l2s_proc_name,0, DSI_L2S_PNAME_SIZE);                             \
    l2s_pid = getpid();                                                      \
    ds_get_proc_name(l2s_pid,l2s_proc_name,DSI_L2S_PNAME_SIZE);              \
    DSI_LOG_DEBUG("L2S[pname:%s pid:%d tid:%d]",                             \
                   l2s_proc_name,l2s_pid, gettid() );                        \
  }while (0);

#define DSI_L2S_ENTRY_PARAMS(fmt,...)                                        \
  DSI_LOG_DEBUG("L2S[pname:%s pid:%d tid:%d type:%s fname:%s params:" fmt "]",\
                 l2s_proc_name,                                              \
                 l2s_pid,                                                    \
                 gettid(),                                                   \
                 "FE",                                                       \
                 __func__,                                                   \
                 ##__VA_ARGS__);

#define DSI_L2S_ENTRY() DSI_L2S_ENTRY_PARAMS("");

#define DSI_L2S_EXIT_WITH_STATUS(fmt,...)                                    \
  DSI_LOG_DEBUG("L2S[pname:%s pid:%d tid:%d type:%s fname:%s status:%s params:"fmt"]",\
                l2s_proc_name,                                               \
                l2s_pid,                                                     \
                gettid(),                                                    \
                "FX",                                                        \
                __func__,                                                    \
                (ret==DSI_ERROR)?"ERROR":"SUCCESS",                          \
                ##__VA_ARGS__);

#define DSI_L2S_EXIT()                                                       \
  DSI_LOG_DEBUG("L2S[pname:%s pid:%d tid:%d type:%s fname:%s]",              \
                l2s_proc_name,                                               \
                l2s_pid,                                                     \
                gettid(),                                                    \
                "FX",                                                        \
                __func__);

#define DSI_L2S_MSG(iface_id, msg)                                           \
  DSI_LOG_DEBUG("L2S[pname:%s pid:%d tid:%d type:%s fname:%s iface_id:%d msg:\"%s\"]",\
                l2s_proc_name,                                               \
                l2s_pid,                                                     \
                gettid(),                                                    \
                "ME",                                                        \
                __func__,                                                    \
                iface_id,                                                    \
                msg);

#define DSI_L2S_MSG_SEQ(iface_id, msg)                                       \
  DSI_LOG_DEBUG("L2S[pname:%s pid:%d tid:%d type:%s fname:%s iface_id:%d msg:\"%s\"]",\
                l2s_proc_name,                                               \
                l2s_pid,                                                     \
                gettid(),                                                    \
                "MS",                                                        \
                __func__,                                                    \
                iface_id,                                                    \
                msg);

#define DSI_L2S_FORMAT_MSG(iface_id,msg,...)                                 \
  do{                                                                        \
    char l2s_msg[DSI_L2S_MSG_MAX_SIZE] = "\0" ;                              \
    memset(l2s_msg,0,DSI_L2S_MSG_MAX_SIZE);                                  \
    snprintf(l2s_msg, DSI_L2S_MSG_MAX_SIZE,msg, __VA_ARGS__);                \
    DSI_L2S_MSG(iface_id,l2s_msg);                                           \
  }while (0);

#define DSI_EVT_STR_MAX_LENGTH 50

static char dsi_evt_str[DSI_EVT_MAX][DSI_EVT_STR_MAX_LENGTH] =
  {
    "INVALID",                     /**< Invalid event. */
    "NET_IS_CONN",                /**< Call is connected. */
    "NET_NO_NET",                 /**< Call is disconnected. */
    "PHYSLINK_DOWN_STATE",        /**< Physlink becomes dormant. */
    "PHYSLINK_UP_STATE",          /**< Physlink becomes active. */
    "NET_RECONFIGURED",           /**< Interface is reconfigured. */
    "QOS_STATUS_IND",             /**< A status for the associated QoS has
                                       changed. */
    "NET_NEWADDR",                /**< New address is generated. */
    "NET_DELADDR",                /**< An address for the interface has been deleted. */
    "NET_PARTIAL_CONN",           /**< Address is available for either IPv4
                                       or IPv6 only. @newpage */
    "TMGI_ACTIVATED",
    "TMGI_DEACTIVATED",
    "TMGI_ACTIVATED_DEACTIVATED",
    "TMGI_LIST_CHANGED",
    "SAI_LIST_CHANGED",

    "NET_HANDOFF",
    "WDS_CONNECTED",
    "NET_NEWMTU",                 /**< MTU update. */

    "DSI_EVT_MAX",
  };

typedef struct dsi_id_str_map_s
{
  int         str_id;
  const char  str[DSI_EVT_STR_MAX_LENGTH];
}dsi_id_str_map_t;

static dsi_id_str_map_t embms_act_status_map[] =
  {
    {WDS_TMGI_ACTIVATE_SUCCESS_V01                       ,"TMGI_ACTIVATE_SUCCESS" },
    {WDS_TMGI_ACTIVATE_SUCCESS_DUPLICATE_V01,"TMGI_ACTIVATE_SUCCESS_DUP_ACTIVATE"},
    {WDS_TMGI_ACTIVATE_SUCCESS_IDLE_RADIO_TUNE_V01 ,"TMGI_ACTIVATE_SUCCESS_IDLE_RADIO_TUNE" },
    {WDS_TMGI_ACTIVATE_SUCCESS_CONN_RADIO_TUNE_V01,"TMGI_ACTIVATE_SUCCESS_CONN_RADIO_TUNE"},
    {WDS_TMGI_ACTIVATE_FAILURE_UNKNOWN_V01 ,"TMGI_ACTIVATE_FAIL_UNKNOWN"},
    {WDS_TMGI_ACTIVATE_FAILURE_NOT_ALLOWED_V01 ,"TMGI_ACTIVATE_FAIL_NOT_ALLOWED"},
    {WDS_TMGI_ACTIVATE_FAILURE_MISSING_CONTROL_INFO_V01,"TMGI_ACTIVATE_FAIL_MISSING_CTL_INFO"},
    {WDS_TMGI_ACTIVATE_FAILURE_MISSING_TMGI_V01,"TMGI_ACTIVATE_FAIL_MISSING_TMGI"},
    {WDS_TMGI_ACTIVATE_FAILURE_MCAST_OOS_V01,"TMGI_ACTIVATE_FAIL_MCAST_OOS"},
    {WDS_TMGI_ACTIVATE_FAILURE_UCAST_OOS_V01,"TMGI_ACTIVATE_FAIL_UNICAST_OOS"},
    {WDS_TMGI_ACTIVATE_FAILURE_CAMPED_ON_OTHER_FREQ_V01,"TMGI_ACTIVATE_FAIL_CAMP_ON_OTHER_FREQ"},
    {WDS_TMGI_ACTIVATE_FAILURE_SAI_MISMATCH_V01,"TMGI_ACTIVATE_FAIL_SAI_MISMATCH "},
    {WDS_TMGI_ACTIVATION_FAILURE_MAX_TMGI_ALREADY_ACTIVE_V01,"TMGI_ACTIVATION_FAIL_MAX_TMGI_ALREADY_ACTIVE"},
    {WDS_EMBMS_ECC_TMGI_ACTIVATION_IN_PROGRESS_V01,"TMGI_ACTIVATE_FAIL_ACTIVATIOIN_IN_PROGRESS"},
    {WDS_EMBMS_EEC_TMGI_DEACTIVATION_IN_PROGRESS_V01 ,"TMGI_ACTIVATE_FAIL_DEACTIVATION_IN_PROGRESS"},
  };

static dsi_id_str_map_t embms_deact_status_map[]=
  {
    {WDS_TMGI_DEACTIVATE_SUCCESS_V01 ,"TMGI_DEACTIVATE_SUCCESS "},
    {WDS_EMBMS_EEC_TMGI_NOT_SUPPORTED_V01,"TMGI_DEACTIVATE_FAIL_NOT_ACTIVATED_CTRL_POINT"},
    {WDS_EMBMS_EEC_TMGI_INVALID_V01 ,"TMGI_DEACTIVATE_FAIL_NOT_ACTIVATED"},
    {WDS_EMBMS_EEC_TMGI_DEACTIVATION_IN_PROGRESS_V01,"TMGI_DEACTIVATE_FAIL_DEACTIVATION_IN_PROGRESS"},
  };


/*---------------------------------------------------------------------------
                           UTILITY MACROS
---------------------------------------------------------------------------*/
#define DSI_ASSERT( cond, msg ) \
  if ( !( cond ) ) \
  { \
    DSI_LOG_FATAL( "%s", "*************ASSERTION FAILED (soft)***************" ); \
    DSI_LOG_FATAL( "File: %s, Line: %d, [%s]", __FILE__, __LINE__, msg );         \
    DSI_LOG_FATAL( "%s", "***************************************************" ); \
  }

/* Hard Assert */
#define DSI_ASSERT_H( cond, msg ) \
  if ( !( cond ) ) \
  { \
    DSI_LOG_FATAL( "%s", "*************ASSERTION FAILED (hard)***************" ); \
    DSI_LOG_FATAL( "File: %s, Line: %d, [%s]", __FILE__, __LINE__, msg );         \
    DSI_LOG_FATAL( "%s", "***************************************************" ); \
    assert(0); \
  }

#define DSI_INET4_NTOP(prefix,data)                                                  \
  DS_INET4_NTOP(med, prefix, data)

#define DSI_INET6_NTOP(prefix,data)                                                  \
  DS_INET6_NTOP(med, prefix, data)

#ifdef DSI_NETCTRL_OFFTARGET
#define DSI_INET_NTOP(prefix,sockaddr)                                         \
        if( AF_INET == (sockaddr)->ss_family ) {                               \
          DSI_INET4_NTOP( prefix, (sockaddr)->__ss_padding );                  \
        } else if( AF_INET6 == (sockaddr)->ss_family ) {                       \
          DSI_INET6_NTOP( prefix, (sockaddr)->__ss_padding );                  \
        } else                                                                 \
          DSI_LOG_DEBUG("Unsupported addr family: 0x%x", (sockaddr)->ss_family)
#else
#define DSI_INET_NTOP(prefix,sockaddr)                                         \
        if( AF_INET == (sockaddr)->ss_family )                                 \
          DSI_INET4_NTOP( prefix, (sockaddr)->__data )                         \
        else if( AF_INET6 == (sockaddr)->ss_family )                           \
          DSI_INET6_NTOP( prefix, (sockaddr)->__data )                         \
        else                                                                   \
          DSI_LOG_DEBUG("Unsupported addr family: 0x%x", (sockaddr)->ss_family)
#endif

#define DSI_IS_ADDR_FAMILY_VALID(addr)                                      \
        ((AF_INET == (addr)->ss_family) || (AF_INET6 == (addr)->ss_family))


#define DSI_LOCK_MUTEX(mutex) \
  { \
    pthread_mutex_lock(mutex); \
    DSI_LOG_VERBOSE("mutex [%p] locked", mutex); \
  }

#define DSI_UNLOCK_MUTEX(mutex) \
  { \
    pthread_mutex_unlock(mutex); \
    DSI_LOG_VERBOSE("mutex [%p] unlocked", mutex); \
  }

#define DSI_MINIMUM(a,b) ((a)<(b)?(a):(b))

#define _DSI_UPDATE_CALL_STATE(_st, _call_state, _call_state_str) \
  (_st)->priv.call_state = (_call_state); \
  DSI_LOG_DEBUG("DSI call state logging: Handle %d, call state %s", (_st)->priv.dsi_iface_id, _call_state_str);\
  DSI_L2S_MSG((_st)->priv.dsi_iface_id, _call_state_str);

#define DSI_UPDATE_CALL_STATE(_st, _call_state) \
  _DSI_UPDATE_CALL_STATE(_st, _call_state, #_call_state)

/* IP address sizes */
#define DSI_SIZEOF_IPV4_ADDRESS 4
#define DSI_SIZEOF_IPV6_ADDRESS 16

/* ================= ps_iface_defs ============================*/
/* unfortunately, they decided to use definitions from
   ps_iface_defs.h to be used as value for technology
   that is returned by qmi_wds_route_look_up

   NOTE: this could have some defines that are not part of
   qmi_wds_iface_name_type defined in qmi_wds_srvc.h
   some unpublished iface (from QMI_WDS spec)could be returned*/
#define DSI_PS_CDMA_SN_IFACE 0x8001
#define DSI_PS_CDMA_AN_IFACE 0x8002
#define DSI_PS_UMTS_IFACE 0x8004
#define DSI_PS_EPC_IFACE 0x8880 /* LTE, HRPD, TDSCDMA */
#define DSI_PS_FMC_IFACE 0x8840

/* ================= MODEMS  ============================*/
/* used as index in table. Always start with 0 and update MAX */
#define DSI_RADIO_4GMODEM 0
#define DSI_RADIO_3GMODEM 1
#define DSI_MAX_MODEMS 2

#define DSI_DEFAULT_MODEM 0
#define DSI_INVALID_MODEM -1
typedef struct dsi_modem_s
{
  int technology; /* identifies tech(s) supported by this modem */
  int default_dsi_iface; /* default QMI port for this modem */
  /* dsi_iface_start..dsi_iface_end identify the interfaces in
     dsi_iface_tbl that are attached to this modem */
  int dsi_iface_start; /* start of index in dsi_iface_tbl */
  int dsi_iface_end; /* end of index in dsi_iface_tbl */
  int qmi_inst_offset; /* QMI instance offset for route lookup */
  qmi_client_type default_wds_hndl;
} dsi_modem_t;

extern dsi_modem_t dsi_modem_tbl[DSI_MAX_MODEMS];

#define DSI_MODEM_GET_DEFAULT_IFACE(m) \
  (dsi_modem_tbl[m].default_dsi_iface)

#define DSI_MODEM_SET_DEFAULT_IFACE(m, i) \
  (dsi_modem_tbl[m].default_dsi_iface = i)

#define DSI_MODEM_GET_TECH(m) \
  (dsi_modem_tbl[m].technology)

#define DSI_MODEM_SET_TECH(m, t) \
  (dsi_modem_tbl[m].technology |= t)

#define DSI_MODEM_GET_IFACE_START(m) \
  (dsi_modem_tbl[m].dsi_iface_start)

#define DSI_MODEM_SET_IFACE_START(m, i) \
  (dsi_modem_tbl[m].dsi_iface_start = i)

#define DSI_MODEM_GET_IFACE_END(m) \
  (dsi_modem_tbl[m].dsi_iface_end)

#define DSI_MODEM_SET_IFACE_END(m, i) \
  (dsi_modem_tbl[m].dsi_iface_end = i)

#define DSI_MODEM_GET_QMI_OFFSET(m) \
  (dsi_modem_tbl[m].qmi_inst_offset)

#define DSI_MODEM_SET_QMI_OFFSET(m, o) \
  (dsi_modem_tbl[m].qmi_inst_offset = o)

#define DSI_MODEM_SET_DEFAULT_WDS_HNDL(m, h) \
  (dsi_modem_tbl[m].default_wds_hndl = h)

#define DSI_MODEM_GET_DEFAULT_WDS_HNDL(m) \
  (dsi_modem_tbl[m].default_wds_hndl)

/* ================= DSI INTERFACES ============================*/

/* The iface start and end values are dependent on DSI_MAX_IFACES */
#define DSI_LOCAL_MODEM_IFACE_START  (0)
#define DSI_LOCAL_MODEM_IFACE_END    (10)
#define DSI_REMOTE_MODEM_IFACE_START (11)
#define DSI_REMOTE_MODEM_IFACE_END   (21)

/* Apart from Not In Use and In Use, added another state to
  indicate the iface has received a WDS disconnected indication.
  This does not imply the link is going down as there still might be
  an iface family up.*/
typedef enum dsi_iface_state_enum_e
{
  DSI_IFACE_STATE_NOT_IN_USE = 0,
  DSI_IFACE_STATE_IN_USE,
  DSI_IFACE_STATE_IN_DISCONNECT
} dsi_iface_state_t;

/* dsi interface type */
/* there is one interface per qmi port */
typedef struct dsi_iface_s
{
  /* used to arbitrate the use of this interface among
     various clients
     lifetime: starting from when start_nw_if() is issued
     to the point where the interface is disconnected
  */
  dsi_iface_state_t state;
  qmi_client_type wds_hndl;       /* QMI WDS service handle */
  qmi_client_type qos_hndl;       /* QMI QOS service handle */
  int qdi_hndl;       /* QDI handle */
  char * wds_str;     /* QMI port identifying string */
  char * dev_str;     /* Device name string */
  /* incremented only when start_nw_if succeeds for a client
     decremetned only when client wanted to stop call this interface
     lifetime: starting from when call is CONNECTED to
     the point where client issues stop_data_call
  */
  unsigned short v4_ref_count; /* v4 ref count for a wds client */
  unsigned short v6_ref_count; /* v6 ref count for a wds client */
  pthread_mutex_t mutex; /* protect this interface */
  int modem; /* each iface belongs to a unique modem */
  int ipv6_profile_index; /* temporary: save ipv6 profile
                          * so can be delelated when iface
                          * is brought down */
  dsi_addr_info_t  addr_info_s[DSI_NUM_IP_FAMILIES]; /* address info structure */
  unsigned int     mtu; /* MTU of the interface */
} dsi_iface_t;

#define DSI_DEFAULT_IFACE 0
#define DSI_INVALID_IFACE -1
#define DSI_INVALID_WDS_TXN -1
#define DSI_INVALID_WDS_HNDL QDI_INVALID_QMI_HANDLE
#define DSI_INVALID_QOS_HNDL QDI_INVALID_QMI_HANDLE
#define DSI_INVALID_PROFILE_INDEX -1
#define DSI_INVALID_MTU 0

extern dsi_iface_t dsi_iface_tbl[DSI_MAX_IFACES];

#define DSI_QMI_PORT_RMNET_INVALID ""

/* QMI Control port names */
/* smd ports */
#define DSI_QMI_PORT_RMNET_0  QMI_PORT_RMNET_0
#define DSI_QMI_PORT_RMNET_1  QMI_PORT_RMNET_1
#define DSI_QMI_PORT_RMNET_2  QMI_PORT_RMNET_2
#define DSI_QMI_PORT_RMNET_3  QMI_PORT_RMNET_3
#define DSI_QMI_PORT_RMNET_4  QMI_PORT_RMNET_4
#define DSI_QMI_PORT_RMNET_5  QMI_PORT_RMNET_5
#define DSI_QMI_PORT_RMNET_6  QMI_PORT_RMNET_6
#define DSI_QMI_PORT_RMNET_7  QMI_PORT_RMNET_7
#define DSI_QMI_PORT_RMNET_8  QMI_PORT_RMNET_8
#define DSI_QMI_PORT_RMNET_9  QMI_PORT_RMNET_9
#define DSI_QMI_PORT_RMNET_10 QMI_PORT_RMNET_10
/* sdio ports */
#define DSI_QMI_PORT_RMNET_SDIO_0 QMI_PORT_RMNET_SDIO_0
#define DSI_QMI_PORT_RMNET_SDIO_1 QMI_PORT_RMNET_SDIO_1
#define DSI_QMI_PORT_RMNET_SDIO_2 QMI_PORT_RMNET_SDIO_2
#define DSI_QMI_PORT_RMNET_SDIO_3 QMI_PORT_RMNET_SDIO_3
#define DSI_QMI_PORT_RMNET_SDIO_4 QMI_PORT_RMNET_SDIO_4
#define DSI_QMI_PORT_RMNET_SDIO_5 QMI_PORT_RMNET_SDIO_5
#define DSI_QMI_PORT_RMNET_SDIO_6 QMI_PORT_RMNET_SDIO_6
#define DSI_QMI_PORT_RMNET_SDIO_7 QMI_PORT_RMNET_SDIO_7
/* usb ports */
#define DSI_QMI_PORT_RMNET_USB_0 QMI_PORT_RMNET_USB_0
#define DSI_QMI_PORT_RMNET_USB_1 QMI_PORT_RMNET_USB_1
#define DSI_QMI_PORT_RMNET_USB_2 QMI_PORT_RMNET_USB_2
#define DSI_QMI_PORT_RMNET_USB_3 QMI_PORT_RMNET_USB_3
#define DSI_QMI_PORT_RMNET_USB_4 QMI_PORT_RMNET_USB_4
#define DSI_QMI_PORT_RMNET_USB_5 QMI_PORT_RMNET_USB_5
#define DSI_QMI_PORT_RMNET_USB_6 QMI_PORT_RMNET_USB_6
#define DSI_QMI_PORT_RMNET_USB_7 QMI_PORT_RMNET_USB_7
/*smux ports*/
#define DSI_QMI_PORT_RMNET_SMUX_0 QMI_PORT_RMNET_SMUX_0
/* usb ports (2nd modem) */
#define DSI_QMI_PORT_RMNET2_USB_0 QMI_PORT_RMNET2_USB_0
#define DSI_QMI_PORT_RMNET2_USB_1 QMI_PORT_RMNET2_USB_1
#define DSI_QMI_PORT_RMNET2_USB_2 QMI_PORT_RMNET2_USB_2
#define DSI_QMI_PORT_RMNET2_USB_3 QMI_PORT_RMNET2_USB_3
#define DSI_QMI_PORT_RMNET2_USB_4 QMI_PORT_RMNET2_USB_4
#define DSI_QMI_PORT_RMNET2_USB_5 QMI_PORT_RMNET2_USB_5
#define DSI_QMI_PORT_RMNET2_USB_6 QMI_PORT_RMNET2_USB_6
#define DSI_QMI_PORT_RMNET2_USB_7 QMI_PORT_RMNET2_USB_7
/* mhi ports*/
#define DSI_QMI_PORT_RMNET_MHI_0   QMI_PORT_RMNET_MHI_0
#define DSI_QMI_PORT_RMNET_MHI_1   QMI_PORT_RMNET_MHI_1

/* Device name prefixes */
/* smd prefix */
#define DSI_DEV_RMNET_SMD_PREFIX  "rmnet"
/* sdio prefix */
#define DSI_DEV_RMNET_SDIO_PREFIX "rmnet_sdio"
/* usb prefix */
#define DSI_DEV_RMNET_USB_PREFIX  "rmnet_usb"
/*smux prefix*/
#define DSI_DEV_RMNET_SMUX_PREFIX "rmnet_smux"
/* usb prefix (2nd modem) */
#define DSI_DEV_RMNET2_USB_PREFIX "rmnet2_usb"
/*rmnet data prefix*/
#define DSI_DEV_RMNET_DATA_PREFIX "rmnet_data"
/* invalid prefix */
#define DSI_DEV_RMNET_INVALID_PREFIX  (NULL)

#define DSI_IS_IFACE_VALID(i) \
  (( i >= DSI_LOCAL_MODEM_IFACE_START && i <= DSI_LOCAL_MODEM_IFACE_END  ) || \
   ( i >= DSI_REMOTE_MODEM_IFACE_START && i <= DSI_REMOTE_MODEM_IFACE_END  ))

#define DSI_GET_DEFAULT_WDS_HNDL \
  ( dsi_iface_tbl[DSI_DEFAULT_IFACE].wds_hndl )

#define DSI_SET_WDS_HNDL(i, hndl) \
  ( dsi_iface_tbl[i].wds_hndl = hndl )

#define DSI_GET_QDI_HNDL(i) \
  ( dsi_iface_tbl[i].qdi_hndl )

#define DSI_SET_QDI_HNDL(i, hndl) \
  ( dsi_iface_tbl[i].qdi_hndl = hndl )

#define DSI_GET_WDS_HNDL(i) \
  ( dsi_iface_tbl[i].wds_hndl )

#define DSI_IS_WDS_HNDL_VALID(i) \
  ( DSI_GET_WDS_HNDL(i) != DSI_INVALID_WDS_HNDL )

#define DSI_SET_WDS_STR(i,str,ret) \
  do \
  { \
    if (NULL != str && \
        strlen(str) > DSI_QMI_PORT_MAX_LEN) \
    { \
      DSI_LOG_ERROR( "string %s length too long for qmi conn", \
                      str ); \
      ret = DSI_ERROR; \
    } \
    else \
    { \
      dsi_iface_tbl[i].wds_str = str; \
      DSI_LOG_DEBUG("dsi_iface_tbl[%d].wds_str is set to [%s]", \
                    i, str); \
      ret = DSI_SUCCESS; \
    } \
  } while (0)

#define DSI_SET_DEV_STR(i,str,ret) \
  do \
  { \
    if (NULL != str && \
        strlen(str) > DSI_DEV_STR_MAX_LEN) \
    { \
      DSI_LOG_ERROR( "string %s length too long for device", \
                      str ); \
      ret = DSI_ERROR; \
    } \
    else \
    { \
      dsi_iface_tbl[i].dev_str = (NULL != str)? str : "unknown"; \
      DSI_LOG_DEBUG("dsi_iface_tbl[%d].dev_str is set to [%s]", \
                    i, dsi_iface_tbl[i].dev_str); \
      ret = DSI_SUCCESS; \
    } \
  } while (0)

#define DSI_GET_WDS_STR(i) \
  ( dsi_iface_tbl[i].wds_str )

#define DSI_GET_DEV_STR(i) \
  ( dsi_iface_tbl[i].dev_str )

#define DSI_SET_WDS_V4_TXN(st, txn) \
  ( st->priv.wds_txn_v4_hndl = txn )

#define DSI_GET_WDS_V4_TXN(st) \
  ( st->priv.wds_txn_v4_hndl )

#define DSI_SET_WDS_V6_TXN(st, txn) \
      ( st->priv.wds_txn_v6_hndl = txn )

#define DSI_GET_WDS_V6_TXN(st) \
      ( st->priv.wds_txn_v6_hndl )

#define DSI_SET_QOS_HNDL(i, hndl) \
  ( dsi_iface_tbl[i].qos_hndl = hndl )

#define DSI_GET_QOS_HNDL(i) \
  ( dsi_iface_tbl[i].qos_hndl )

#define DSI_IS_QOS_HNDL_VALID(i) \
  (DSI_GET_QOS_HNDL(i) != DSI_INVALID_QOS_HNDL)

#define DSI_GET_STATE(i) \
  ( dsi_iface_tbl[i].state )

#define DSI_SET_STATE(i,s) \
  ( dsi_iface_tbl[i].state = s )

#define DSI_RESERVED(i) \
  ( dsi_iface_tbl[i].state != \
    DSI_IFACE_STATE_NOT_IN_USE )

#define DSI_INCR_COUNT(i,fam) \
  do { \
    if(DSI_IP_VERSION_4 == fam)\
    {\
      dsi_iface_tbl[i].v4_ref_count++; \
      DSI_LOG_DEBUG("iface [%d] v4 ref count incr [%d]", i, dsi_iface_tbl[i].v4_ref_count); \
    }\
    else if(DSI_IP_VERSION_6 == fam)\
    {\
      dsi_iface_tbl[i].v6_ref_count++; \
      DSI_LOG_DEBUG("iface [%d] v6 ref count incr [%d]", i, dsi_iface_tbl[i].v6_ref_count); \
    }\
    else if(DSI_IP_VERSION_4_6 == fam)\
    {\
      dsi_iface_tbl[i].v4_ref_count++; \
      dsi_iface_tbl[i].v6_ref_count++; \
      DSI_LOG_DEBUG("iface [%d] ref count incr v4[%d] v6[%d]", i, dsi_iface_tbl[i].v4_ref_count, \
                    dsi_iface_tbl[i].v6_ref_count); \
    }\
  } while (0)

#define DSI_DECR_COUNT(i,client,fam) \
  do \
  { \
    if (client->priv.dsi_iface_id != i) \
    { \
      DSI_LOG_ERROR("client [%p] attempted to decr ref count " \
                    "on dsi iface [%d] that it does not refer to", \
                    (unsigned int*)client, i); \
    } \
    else \
    { \
      if(DSI_IP_VERSION_4 == fam)\
      {\
        if(dsi_iface_tbl[i].v4_ref_count <= 0) \
        { \
          DSI_LOG_ERROR("client [%p] attempted to decr v4 ref count " \
                        "that is not positive [%d]", \
                        (unsigned int*)client, dsi_iface_tbl[i].v4_ref_count); \
        } \
        else \
        { \
          dsi_iface_tbl[i].v4_ref_count--; \
          DSI_LOG_DEBUG("iface [%d] v4 ref count decr [%d]", i, dsi_iface_tbl[i].v4_ref_count); \
        } \
      }\
      else if(DSI_IP_VERSION_6 == fam)\
      {\
        if(dsi_iface_tbl[i].v6_ref_count <= 0) \
        { \
          DSI_LOG_ERROR("client [%p] attempted to decr v6 ref count " \
                        "that is not positive [%d]", \
                        (unsigned int*)client, dsi_iface_tbl[i].v6_ref_count); \
        } \
        else \
        { \
          dsi_iface_tbl[i].v6_ref_count--; \
          DSI_LOG_DEBUG("iface [%d] v6 ref count decr [%d]", i, dsi_iface_tbl[i].v6_ref_count); \
        } \
      } \
      else if(DSI_IP_VERSION_4_6 == fam)\
      {\
         if( dsi_iface_tbl[i].v4_ref_count <= 0 || \
            dsi_iface_tbl[i].v6_ref_count <= 0 ) \
         {\
           DSI_LOG_ERROR("client [%p] attempted to decr v4/v6 ref count " \
                         "that is not positive v4[%d] v6[%d]", \
                         (unsigned int*)client, dsi_iface_tbl[i].v4_ref_count, \
                         dsi_iface_tbl[i].v6_ref_count); \
         }\
         else\
         {\
           dsi_iface_tbl[i].v4_ref_count--; \
           dsi_iface_tbl[i].v6_ref_count--; \
           DSI_LOG_DEBUG("iface [%d] ref count decr v4[%d] v6[%d]", i, dsi_iface_tbl[i].v4_ref_count, \
                          dsi_iface_tbl[i].v6_ref_count); \
         }\
      }\
    } \
  } while (0)

#define DSI_SET_V4_COUNT(i,rc) \
  ( dsi_iface_tbl[i].v4_ref_count = rc )

#define DSI_SET_V6_COUNT(i,rc) \
  ( dsi_iface_tbl[i].v6_ref_count = rc )

#define DSI_GET_V4_COUNT(i) ( dsi_iface_tbl[i].v4_ref_count )

#define DSI_GET_V6_COUNT(i) ( dsi_iface_tbl[i].v6_ref_count )

#define DSI_IS_V4_CALL_DOWN(ipf, i) \
  ( (ipf == DSI_IP_FAMILY_V4) && (0 == DSI_GET_V4_COUNT(i)) )

#define DSI_IS_V6_CALL_DOWN(ipf, i) \
  ( (ipf == DSI_IP_FAMILY_V6) && (0 == DSI_GET_V6_COUNT(i)) )

#define DSI_IS_V4_V6_CALL_DOWN(ipf, i) \
  ( (ipf == DSI_NUM_IP_FAMILIES) && (0 == DSI_GET_V4_COUNT(i)) && \
    (0 == DSI_GET_V6_COUNT(i)) )

#define DSI_GET_MUTEX(i) \
  ( &(dsi_iface_tbl[i].mutex) )

#define DSI_GET_MODEM(i) \
  ( dsi_iface_tbl[i].modem )

#define DSI_SET_MODEM(i,m) \
  ( dsi_iface_tbl[i].modem = m )

#define DSI_IS_ADDR_VALID(i,ipf,address) \
  ( TRUE == dsi_iface_tbl[i].addr_info_s[ipf].address##_s.valid_addr )

#define DSI_SET_INVALID_ADDR(i,ipf,address) \
  ( dsi_iface_tbl[i].addr_info_s[ipf].address##_s.valid_addr = FALSE )

#define DSI_SET_VALID_ADDR(i,ipf,address) \
  ( dsi_iface_tbl[i].addr_info_s[ipf].address##_s.valid_addr = TRUE )

#define DSI_GET_ADDR(i,ipf,address) \
  ( dsi_iface_tbl[i].addr_info_s[ipf].address##_s.addr )

#define DSI_GET_ADDRINFO(i,ipf) \
  ( dsi_iface_tbl[i].addr_info_s[ipf] )

#define DSI_GET_MTU(i) \
  ( dsi_iface_tbl[i].mtu )

#define DSI_SET_MTU(i,x) \
  ( dsi_iface_tbl[i].mtu = x )

/* Signalling Data structures and functions */
#define NANO_SEC 1000000000

typedef struct dsi_signal_data_s
{
  unsigned long   cond_predicate;
  pthread_mutex_t cond_mutex;
  pthread_cond_t  cond_var;
}dsi_signal_data_t;

/* DS condition variables operations */
#define DSI_INIT_SIGNAL_DATA(signal_ptr) \
  do \
  { \
    pthread_mutex_init (&(signal_ptr)->cond_mutex,NULL); \
    pthread_cond_init (&(signal_ptr)->cond_var,NULL); \
  } while (0)

/* Macro to destroy signal data */
#define DSI_DESTROY_SIGNAL_DATA(signal_ptr) \
  do \
  { \
    pthread_cond_destroy (&(signal_ptr)->cond_var); \
    pthread_mutex_destroy (&(signal_ptr)->cond_mutex); \
  } while (0)

int
dsi_wait_for_sig_with_timeout
(
  dsi_signal_data_t  *signal_ptr,
  int                timeout_secs
);

#define DSI_INIT_SIGNAL_FOR_WAIT(signal_ptr) \
  do \
  { \
    pthread_mutex_lock (&(signal_ptr)->cond_mutex); \
    (signal_ptr)->cond_predicate = FALSE; \
  } while (0)

#define DSI_WAIT_FOR_SIGNAL_WITH_TIMEOUT(signal_ptr, timeout_milli_secs) \
  dsi_wait_for_sig_with_timeout (signal_ptr,timeout_milli_secs)


#define DSI_SEND_SIGNAL(signal_ptr) \
  do \
  { \
    pthread_mutex_lock (&(signal_ptr)->cond_mutex); \
    (signal_ptr)->cond_predicate = TRUE; \
    pthread_cond_signal (&(signal_ptr)->cond_var); \
    pthread_mutex_unlock (&(signal_ptr)->cond_mutex); \
  } while (0)


typedef struct dsi_qmi_to_dsi_tech_s
{
  int qmi_tech;
  int dsi_tech;
} dsi_qmi_to_dsi_tech_t;

extern dsi_qmi_to_dsi_tech_t
dsi_qmi_tech_mapping_tbl[DSI_RADIO_TECH_MAX];

/* ================= DSI CALLS ============================*/
#define DSI_MAX_NUM_OF_TMGI_LIST 64 /* max number of TMGI_List
                                       same value as NUMBER_MAX_V01 defined
                                       qmi_embms_v01.h */
#define DSI_SIZE_OF_TMGI          6 /* TMGI is 6 byte array
                                       same value as NUMBER_MAX_V01 defined
                                       qmi_embms_v01.h */
#define DSI_SIZE_OF_EARFCNLIST    32/* max number of earfcnlist size
                                       same value as EARFCNLIST_MAX_V01 defined
                                       in qmi_embms_v01.h */

#define DSI_EMBMS_SAI_PER_FREQ_MAX_SIZE  QMI_WDS_EMBMS_SAI_LIST_MAX_V01

typedef enum
{
  DSI_STATE_EVT_NET_IS_CONN_UNKNOWN,
  DSI_STATE_EVT_NET_IS_CONN_PENDING,
  DSI_STATE_EVT_NET_IS_CONN_SENT
} dsi_state_evt_net_is_conn_t;

typedef enum
{
  DSI_STATE_CALL_IDLE,
  DSI_STATE_CALL_CONNECTING,
  DSI_STATE_CALL_CONNECTED,
  DSI_STATE_CALL_DISCONNECTING,
  DSI_STATE_CALL_MAX
} dsi_state_call_state_t;

/* status for activate/deactivate indication has only 1 TMGI */
#define DSI_EMBMS_TMGI_STATUS_IND_TMGI_LIST_LEN 1

extern pthread_mutex_t dsi_netmgr_txn_mutex;
static unsigned int dsi_netmgr_txn_cnt = 0;

typedef qdi_wds_call_end_reason_type     dsi_wds_call_end_reason_type;
typedef qdi_qmi_wds_indication_data_type dsi_qmi_wds_indication_data_type;

typedef union
{
  wds_embms_tmgi_activate_req_msg_v01             act_req;
  wds_embms_tmgi_deactivate_req_msg_v01           deact_req;
  wds_embms_tmgi_act_deact_req_msg_v01            act_deact_req;
  wds_embms_tmgi_list_query_req_msg_v01           list_query_req;
  wds_embms_tmgi_list_query_resp_msg_v01          list_info;
  dsi_qmi_wds_embms_content_desc_update_info_type content_desc_info;
  wds_embms_svc_interest_info_req_msg_v01         svc_interest_info;
}dsi_embms_tmgi_priv_params_info;

typedef struct dsi_embms_tmgi_info_s
{
  dsi_embms_tmgi_priv_params_info            embms_tmgi_params;
  wds_embms_tmgi_activate_status_enum_v01    activate_status;
  wds_embms_tmgi_deactivate_status_enum_v01  deactivate_status;
}dsi_embms_tmgi_priv_info_type;

typedef struct
{
  /* Private data for DSI module */
  int dsi_iface_id; /* index into dsi_iface_tbl */

  wds_start_network_interface_req_msg_v01 start_nw_params;

  /* call end reason structure */
  dsi_ce_reason_t ce_reason[DSI_NUM_IP_FAMILIES];

  /* wds async rsp cb may change state of this
     structure. protect it with mutex when needed */
  pthread_mutex_t mutex;
  int rl_qmi_inst;  /* Modem route lookup result */

  /*Route look up params set at the time of route look
    up for a particular store handle*/
  int rl_port_incall;  /*To check if port in use*/
  dsi_link_status_type rl_link_state_v4; /*check v4 wds port status*/
  dsi_link_status_type rl_link_state_v6; /*check v6 wds port status*/

  /* Modem call state */
  dsi_state_call_state_t call_state;

  /* IP version of the call */
  int ip_version;

  /*Technology of the call*/
  wds_technology_name_enum_v01 call_tech;

  dsi_embms_tmgi_priv_info_type   embms_tmgi_list_info;

  /* Flag indicating whether partial retry is being attempted */
  boolean  partial_retry;

  boolean  companion_ip_call;

  int wds_txn_v4_hndl;/* pending QMI v4 txn */
  int wds_txn_v6_hndl;/* pending QMI v6 txn  */

  /* QDI call handle */
  qdi_call_handle_t  qdi_call_hndl;

  /* Call connect ref count associated with the handle */
  int ref_count;

  /* The type of app */
  int app_type;

  /* data for sync netmgr user commands */
  dsi_signal_data_t signal_data;
  netmgr_user_cmd_data_t user_cmd_data;
} dsi_priv_t;

#define DSI_INVALID_QMI_INST (-1)

typedef struct dsi_store_s
{
  dsi_net_ev_cb net_ev_cb; /* caller/owner callback function */
  void * user_data; /* caller/owner cookie */
  dsi_evt_payload_t *ev_payload;  /* Cached event payload */
  dsi_priv_t priv; /* private data */
  int dsi_tech; /* if user provided specific technology
                * other than CDMA/UMTS, use this field
                * to store it */
  void * self; /* used to validate dsi_stor_t ptr */
} dsi_store_t;

/*!
  @brief:
  dsi_store_ptr: Pointer to the dsi store.

  This ptr is given to the higher layers (i.e. dsi_netctrl)
  as a call handle. Each valid dsi_stor_tbl_t entry should
  contain a valid dsi_iface_id in it's priv structure.
*/
typedef struct
{
  unsigned char is_valid;
  void * dsi_store_ptr;
}dsi_store_tbl_t;

extern dsi_store_tbl_t dsi_store_table[DSI_MAX_DATA_CALLS];

/* constant represents that we don't know the call end reason  */
/* This is the default CE reason that we set in our store for all
   calls. Setting this value to -1 as IMS expects -1 for retrying */
#define DSI_CE_REASON_UNKNOWN (-1)

/*===========================================================================
                    VALIDATION MACROS
===========================================================================*/
#define DSI_IS_HNDL_VALID( hndl ) \
  ( (dsi_inited == DSI_TRUE) && ( hndl != NULL) && ( hndl->self == hndl ) )
#define DSI_IS_IDENT_VALID( ident ) \
  ( (DSI_CALL_INFO_MIN < ident) && (ident < DSI_CALL_INFO_MAX) )
/* iface id (index into dsi_iface_tbl starts from 0
   so it can be anything from 0 to DSI_MAX_IFACES */
#define DSI_IS_ID_VALID(id) \
  ( (dsi_inited == DSI_TRUE) && (id < DSI_MAX_IFACES) && (id >= 0) )
#define DSI_IS_IP_FAMILY_VALID(ipf) \
  (((int)(ipf) >= (int)DSI_IP_FAMILY_V4) && ((int)(ipf) < (int)DSI_NUM_IP_FAMILIES))




/*===========================================================================
                      INTERNAL CONFIGURATION VTABLES
===========================================================================*/
/* client can provide their own function
   pointers to adapt to their needs */
typedef int (*dsi_mni_init_fptr)(boolean from_ssr);
typedef void (*dsi_mni_cleanup_fptr)(boolean from_ssr);
typedef int (*dsi_mni_start_fptr)(int i, dsi_store_t * st);
typedef int (*dsi_mni_stop_ftpr)(int i, dsi_store_t * st);
typedef int (*dsi_mni_abort_start_ftpr)(int i, dsi_store_t * st);
typedef int (*dsi_mni_look_up_ftpr)(dsi_store_t * st, int * pi);
typedef int (*dsi_mni_reg_unreg_handoff_ind_fptr)(int i, dsi_store_t * st, boolean reg_unreg);
typedef int (*dsi_mni_register_embms_ind_ftpr)(int i, dsi_store_t * st);
typedef int (*dsi_mni_embms_tmgi_activate_ftpr)(int i, dsi_store_t *st);
typedef int (*dsi_mni_embms_tmgi_deactivate_ftpr)(int i, dsi_store_t *st);
typedef int (*dsi_mni_embms_tmgi_act_deactivate_ftpr)(int i, dsi_store_t *st);
typedef int (*dsi_mni_embms_tmgi_list_query_ftpr)(int i, dsi_store_t *st);
typedef int (*dsi_mni_embms_tmgi_content_desc_update_fptr)(int i, dsi_store_t *st);
typedef int (*dsi_mni_embms_tmgi_svc_interest_list_fptr)(int i, dsi_store_t *st);

typedef struct dsi_mni_vtable_s
{
  dsi_mni_init_fptr                            mni_init_f;
  dsi_mni_cleanup_fptr                         mni_cleanup_f;
  dsi_mni_start_fptr                           mni_start_f;
  dsi_mni_stop_ftpr                            mni_stop_f;
  dsi_mni_abort_start_ftpr                     mni_abort_start_f;
  dsi_mni_look_up_ftpr                         mni_look_up_f;
  dsi_mni_reg_unreg_handoff_ind_fptr           mni_reg_unreg_handoff_ind_f;
  dsi_mni_register_embms_ind_ftpr              mni_register_embms_ind_f;
  dsi_mni_embms_tmgi_activate_ftpr             mni_embms_tmgi_activate_f;
  dsi_mni_embms_tmgi_deactivate_ftpr           mni_embms_tmgi_deactivate_f;
  dsi_mni_embms_tmgi_act_deactivate_ftpr       mni_embms_tmgi_act_deactivate_f;
  dsi_mni_embms_tmgi_list_query_ftpr           mni_embms_tmgi_list_query_f;
  dsi_mni_embms_tmgi_content_desc_update_fptr  mni_embms_tmgi_content_desc_update_f;
  dsi_mni_embms_tmgi_svc_interest_list_fptr mni_embms_tmgi_svc_interest_list_f;
} dsi_mni_vtable_t;

extern dsi_mni_vtable_t dsi_mni_vtbl;

extern boolean dsi_inited;
extern boolean dsi_released;
extern pthread_t dsi_ping_tid;
extern pthread_mutex_t dsi_ping_tid_mutex;
extern boolean is_default_handle_inited[DSI_MAX_MODEMS];


typedef struct dsi_init_cb_info_s
{
  void (*cb_func)( void * );
  void *cb_data;
} dsi_init_cb_info_t;

extern dsi_init_cb_info_t dsi_init_cb_info;

extern boolean netmgr_ready_queried;

/* for testing/demo - set this iface id where we can issue nas query */
#define DSI_NAS_IFACE_ID 0

extern int nas_clnt_id;

extern pthread_mutex_t dsi_global_mutex;

#define DSI_GLOBAL_LOCK \
  DSI_LOCK_MUTEX(&dsi_global_mutex)

#define DSI_GLOBAL_UNLOCK \
  DSI_UNLOCK_MUTEX(&dsi_global_mutex)

extern netmgr_client_hdl_t netmgr_hndl;

/*===========================================================================
                    FUNCTIONS EXPORTED INTERNALLY
Note: to avoid duplicate description for the functions, you'll
only find the description commented in the C file.
===========================================================================*/

/*===========================================================================
  FUNCTION:  dsi_netctrl_join_ping_thread
===========================================================================*/
/*!
    @brief
    Call a join on the init_ping_thread if the thread id is valid. This
    ensures that the thread's resources are released.

    @param
    None

    @return
    None
*/
/*=========================================================================*/
extern void dsi_netctrl_join_ping_thread();

extern const char* dsi_get_str_for_id(dsi_id_str_map_t *str_map,
                                      int str_id,
                                      int map_size);

extern void dsi_cleanup_store_tbl(short int store_index);

extern void dsi_notify_users(int dsi_iface_index, int event,dsi_ip_family_t  ipf);

extern int dsi_fill_addr(const struct sockaddr_storage * src_addr_ptr,
                         struct sockaddr_storage       * dst_addr_ptr);

extern void dsi_fill_addr_info(int dsi_id,
                               dsi_ip_family_t ipf,
                               const netmgr_nl_event_info_t *info_ptr);

extern void dsi_fill_qos_info(int dsi_id, const netmgr_nl_event_info_t * info_ptr);

extern void dsi_release_dsi_iface
(
  int dsi_iface_index,
  dsi_ce_reason_t * ce_reason,
  dsi_ip_family_t ipf
);

extern int dsi_find_any_valid_iface_on_modem(int modem_id);

extern int dsi_find_available_iface_on_modem(int modem_id, int app_type);

extern int dsi_find_dsi_id_by_tech(int tech, boolean unused);

extern int dsi_find_dsi_id_by_qmi(unsigned char qmi_inst,
                                  int dsi_iface_index);

extern int dsi_map_qmi_to_dsi_tech(int qmi_tech);

extern int dsi_map_dsi_to_qmi_tech(int qmi_tech);

extern dsi_ce_reason_type_t
dsi_map_qmi_to_dsi_ce_reason_type
(
  wds_verbose_call_end_reason_type_enum_v01 qmi_ce_reason_type
);

extern dsi_embms_tmgi_deactivate_reason_type_t
dsi_map_qmi_to_dsi_tmgi_deactivated_reason_type(int reason);

extern void dsi_attach_dsi_iface(int dsi_iface_index,
                                            dsi_store_t * st);

extern void dsi_detach_dsi_iface(dsi_store_t * st);

extern int dsi_prepare_route_lookup_params
(
  dsi_store_t * dsi_hndl,
  dsi_qmi_wds_route_look_up_params_type * pparams
);

extern int dsi_init_internal(dsi_mni_vtable_t * mni_vtbl, boolean from_ssr);

extern void dsi_release_internal(void);

extern int dsi_is_qmiport_incall
(
  int iface,
  int *inuse_ptr,
  dsi_link_status_type *link_state_v4,
  dsi_link_status_type *link_state_v6
);

extern void dsi_modem_out_of_service_hdlr(int conn_id);

extern void dsi_modem_in_service_hdlr(int conn_id);
extern void dsi_update_store_table
(
  int iface,
  wds_verbose_call_end_reason_type_enum_v01 type,
  int verbose_code,
  dsi_ip_family_t ipf
);

dsi_data_bearer_tech_t
dsi_translate_qmi_to_dsi_bearer_tech
(
  wds_current_bearer_tech_type_v01 *qmi_bearer_tech
);

dsi_data_bearer_tech_t
dsi_translate_qmi_to_dsi_bearer_tech_ex
(
  wds_bearer_tech_info_type_v01 *qmi_bearer_tech_ex
);

extern void dsi_update_call_tech
(
  int iface,
  wds_technology_name_enum_v01 call_tech,
  int ip_version
);

extern void set_dsi_init_state
(
  boolean dsi_inited_val
);
extern int dsi_set_ril_instance(int instance);

extern int dsi_get_ril_instance(void);

extern int dsi_get_modem_subs_id(void);

extern int dsi_link_release(int link);

#endif
