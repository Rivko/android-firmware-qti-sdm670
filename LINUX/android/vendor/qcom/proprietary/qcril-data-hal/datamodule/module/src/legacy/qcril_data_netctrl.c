/*!
  @file
  qcril_data_netctrl.c


  @brief
  Handles RIL requests for DATA services
*/

/*===========================================================================

  Copyright (c) 2008-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //linux/pkgs/proprietary/qc-ril/main/source/qcril_data.c#17 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/12/11   sg      Added timeout functionality for setup data call request
05/05/10   js      replaced dss with dsi_netctrl
03/01/10   fc      Re-architecture to support split modem
06/26/09   fc      Fixed the issue of bogus RIL Request reported in call flow
                   log packet.
05/29/09   fc      Renamed functions.
05/21/09   sm      Passes auth pref to dss from ril
05/14/09   pg      Changed NULL APN handling in data call setup.
                   Mainlined FEATURE_MULTIMODE_ANDROID.
04/05/09   fc      Cleanup log macros.
03/10/09   pg      Fixed multi-mode data call.
12/29/08   fc      Fixed wrong size issue being reported for the response
                   payload of RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE.
12/23/08   asn     code reorg and IP addr fix
12/23/08   asn     Added handling on MO call-end, IP addr issue, APN trunc issue
12/15/08   asn     Fixed call teardown and improved stability
12/08/08   pg      Added multi-mode data call hook up.
11/18/08   fc      Changes to avoid APN string being truncated.
11/14/08   sm      Added temp CDMA data support.
08/29/08   asn     Added data support
08/08/08   asn     Initial version

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <stdio.h>
#include <sys/ioctl.h>
#ifdef QCRIL_DATA_OFFTARGET
  #include <netinet/in.h>
  #include <errno.h>
  #include<linux/sockios.h>
#endif
#include <net/if.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <limits.h>
#ifdef QCRIL_DATA_OFFTARGET
   #include <signal.h>
#else
   #include <asm-generic/siginfo.h>
#endif
#include <linux/socket.h>

#include <telephony/ril.h>
#include "IxErrno.h"
#include "qcril_reqlist.h"
#include "dsi_netctrl.h"
#include "qmi_platform.h"
#include "qcril_data_qmi_wds.h"
#include "qdp.h"

#ifdef QCRIL_DATA_OFFTARGET
  #include "qcril_data_stubs.h"
#else
 #include "qcril_event_facade.h" /* QCRIL_ARB_* defines*/
#endif

#include "qcril_data.h"
#include "qcril_data_defs.h"
#include "qcril_data_utils.h"
#include "DataCommon.h"

#define HACK_MODEM_QUERY_SEARCH_UNAVAIL
#include <string.h>

#include <cutils/properties.h>
#ifdef HAVE_LIBC_SYSTEM_PROPERTIES
   // Don't have system properties on Ubunto x86, use property server
   #include <sys/system_properties.h>
#endif
#include "ds_string.h"
#include "data_system_determination_v01.h"
#include "qmi_client_instance_defs.h"
#include "ds_util.h"
#include "data_port_mapper_v01.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define QCRIL_DATA_INVALID_INSTANCE_ID (QCRIL_MAX_INSTANCE_ID)
#define QCRIL_DATA_INVALID_MODEM_ID    (0xFFFFFFFF)

/*---------------------------------------------------------------------------
   Macros for Android setup data call timer override
---------------------------------------------------------------------------*/
/* System property for setup data call timeout, supported range 0-99999
   if it is set to 0, a default timeout value QCRIL_DATA_PROPERTY_TIMEOUT_DEFAULT
   will be used.
*/
#define QCRIL_DATA_PROPERTY_TIMEOUT             "persist.vendor.radio.datacall_timeout"
#define QCRIL_DATA_PROPERTY_TIMEOUT_SIZE        (6)

#define QCRIL_DATA_PROPERTY_TIMEOUT_INVALID  (0)

#define QCRIL_DATA_CALL_OBJ_MATCH_FOUND      (SUCCESS)
#define QCRIL_DATA_CALL_OBJ_MATCH_NOT_FOUND  (FAILURE)

#define QCRIL_DATA_QMI_DSD_SYNC_MSG_TIMEOUT  (10000)
#define QCRIL_QMI_DSD_TIMEOUT                (30000)

#define QCRIL_DATA_QMI_DPM_SYNC_MSG_TIMEOUT  (10000)
#define QCRIL_QMI_DPM_TIMEOUT                (30000)

typedef enum
{
  QCRIL_DATA_IFACE_IOCTL_GO_DORMANT,
  QCRIL_DATA_IFACE_IOCTL_DORMANCY_INDICATIONS_OFF,
  QCRIL_DATA_IFACE_IOCTL_DORMANCY_INDICATIONS_ON
} qcril_data_iface_ioctl_type;

#ifndef PLATFORM_LTK
#ifdef ASSERT
#undef ASSERT
#endif /* ASSERT */
#define ASSERT( xx_exp ) ((void)0)
#endif /* PLATFORM_LTK */

#define QMI_WDS_CDMA_PROFILE_APP_TYPE_TLV_ID 0x1E
#define QMI_WDS_CDMA_PROFILE_APP_TYPE_TLV_SIZE 4

/* Booleans */
#define TRUE  1
#define FALSE 0

/* Map RIL commands to local names */
#define DS_RIL_REQ_ACT_DATA_CALL        RIL_REQUEST_SETUP_DATA_CALL
#define DS_RIL_REQ_DEACT_DATA_CALL      RIL_REQUEST_DEACTIVATE_DATA_CALL
#define DS_RIL_REQ_GET_CALL_LIST        RIL_REQUEST_DATA_CALL_LIST
#define DS_RIL_REQ_GET_LAST_FAIL_CAUSE  RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE
#define DS_RIL_IND_CALL_LIST_CHANGED    RIL_UNSOL_DATA_CALL_LIST_CHANGED

#define QCRIL_DATA_ADDR_DELIMITER      " "
#define QCRIL_DATA_ADDR_DELIMITER_LEN  1

#define QCRIL_DATA_SUGGESTEDRETRYTIME  (-1)      /* no value suggested, see ril.h */
#define QCRIL_DATA_NORETRY_TIME        (INT_MAX) /* no value suggested, see ril.h */

#if (QCRIL_RIL_VERSION >= 6)

typedef struct
{
  unsigned long       ipv4;
  union
  {
    unsigned long long u64;
    unsigned long      u32[2];
    char                u8[16];
  } ipv6;
} qcril_data_addr_t;


#define DS_CALL_ADDR_FORMAT_IPV4(data,str,len)                          \
        (void)snprintf( str, len,                                       \
                        "%d.%d.%d.%d%c",                                \
                        ((char*)data)[ 0],((char*)data)[ 1],            \
                        ((char*)data)[ 2],((char*)data)[ 3], 0 );

#define DS_CALL_ADDR_FORMAT_IPV6(data,str,len)                          \
        (void)snprintf( str, len,                                       \
                        "%.02x%.02x:%.02x%.02x:%.02x%.02x:%.02x%.02x:"  \
                        "%.02x%.02x:%.02x%.02x:%.02x%.02x:%.02x%.02x%c",\
                        ((char*)data)[ 0],((char*)data)[ 1],            \
                        ((char*)data)[ 2],((char*)data)[ 3],            \
                        ((char*)data)[ 4],((char*)data)[ 5],            \
                        ((char*)data)[ 6],((char*)data)[ 7],            \
                        ((char*)data)[ 8],((char*)data)[ 9],            \
                        ((char*)data)[10],((char*)data)[11],            \
                        ((char*)data)[12],((char*)data)[13],            \
                        ((char*)data)[14],((char*)data)[15], 0 );

#define DS_CALL_ADDR_FORMAT_IPV4V6(addr,str,len)                        \
        if( AF_INET == SASTORAGE_FAMILY(*addr) )                        \
          DS_CALL_ADDR_FORMAT_IPV4( SASTORAGE_DATA(*addr), str, len )   \
        else if( AF_INET6 == SASTORAGE_FAMILY(*addr) )                  \
          DS_CALL_ADDR_FORMAT_IPV6( SASTORAGE_DATA(*addr), str, len )

/*Format IP address with Subnet Mask or prefix length in CIDR notation.
 * if_addr/prefix length: 10.123.240.10/24 */
#define DS_CALL_ADDR_FORMAT_IPV4_WITH_SUBNET_MASK(data,mask,str,len)    \
        (void)snprintf( str, len,                                       \
                        "%d.%d.%d.%d/%d%c",                             \
                        ((char*)data)[ 0],((char*)data)[ 1],            \
                        ((char*)data)[ 2],((char*)data)[ 3],            \
                        mask, 0 );

#define DS_CALL_ADDR_FORMAT_IPV6_WITH_PREFIX_LENGTH(data,mask,str,len) \
        (void)snprintf( str, len,                                            \
                        "%.02x%.02x:%.02x%.02x:%.02x%.02x:%.02x%.02x:"       \
                        "%.02x%.02x:%.02x%.02x:%.02x%.02x:%.02x%.02x/%d%c",  \
                        ((char*)data)[ 0],((char*)data)[ 1],                 \
                        ((char*)data)[ 2],((char*)data)[ 3],                 \
                        ((char*)data)[ 4],((char*)data)[ 5],                 \
                        ((char*)data)[ 6],((char*)data)[ 7],                 \
                        ((char*)data)[ 8],((char*)data)[ 9],                 \
                        ((char*)data)[10],((char*)data)[11],                 \
                        ((char*)data)[12],((char*)data)[13],                 \
                        ((char*)data)[14],((char*)data)[15], mask, 0 );

#define DS_CALL_ADDR_FORMAT_IPV4V6_WITH_PREFIX_LEN(addr,mask,str,len)         \
        if( AF_INET == SASTORAGE_FAMILY(*addr) )                              \
          DS_CALL_ADDR_FORMAT_IPV4_WITH_SUBNET_MASK( SASTORAGE_DATA(*addr),   \
                                                     mask, str, len )         \
        else if( AF_INET6 == SASTORAGE_FAMILY(*addr) )                        \
          DS_CALL_ADDR_FORMAT_IPV6_WITH_PREFIX_LENGTH( SASTORAGE_DATA(*addr), \
                                                       mask, str, len )

#if 0 /* For use when dual-IP supported */
#define DS_CALL_ADDR_FORMAT(data,str,len)                                         \
        if( (data)->ipv4 ) DS_CALL_ADDR_FORMAT_IPV4( &(data)->ipv4, str, len );   \
        if( (data)->ipv4 && (data)->ipv6.u64 ) strncat( str, " ", sizeof(str) );  \
        if( (data)->ipv6.u64 ) DS_CALL_ADDR_FORMAT_IPV6( &(data)->ipv6.u64,       \
                                                         (str+strlen(str)),       \
                                                         (len-strlen(str)) );
#endif /* 0 */

#define QCRIL_DATA_IS_RIL_RADIO_TECH_CDMA_1X_EVDO(t)  ((t == QCRIL_DATA_RIL_RADIO_TECH_IS95A)  || \
                                                       (t == QCRIL_DATA_RIL_RADIO_TECH_IS95B)  || \
                                                       (t == QCRIL_DATA_RIL_RADIO_TECH_1xRTT)  || \
                                                       (t == QCRIL_DATA_RIL_RADIO_TECH_EVDO_0) || \
                                                       (t == QCRIL_DATA_RIL_RADIO_TECH_EVDO_A) || \
                                                       (t == QCRIL_DATA_RIL_RADIO_TECH_EVDO_B))

#define QCRIL_DATA_IS_RIL_RADIO_TECH_EHRPD(t) (t == QCRIL_DATA_RIL_RADIO_TECH_EHRPD)

#define QCRIL_DATA_IS_RIL_RADIO_TECH_CDMA(t)  (QCRIL_DATA_IS_RIL_RADIO_TECH_CDMA_1X_EVDO(t) ||        \
                                                       (t == QCRIL_DATA_RIL_RADIO_TECH_EHRPD))

#define QCRIL_DATA_IS_RIL_RADIO_TECH_3GPP_EHRPD(t)    ((t == QCRIL_DATA_RIL_RADIO_TECH_GPRS)    || \
                                                       (t == QCRIL_DATA_RIL_RADIO_TECH_HSDPA)   || \
                                                       (t == QCRIL_DATA_RIL_RADIO_TECH_HSUPA)   || \
                                                       (t == QCRIL_DATA_RIL_RADIO_TECH_HSPA)    || \
                                                       (t == QCRIL_DATA_RIL_RADIO_TECH_HSPAP)   || \
                                                       (t == QCRIL_DATA_RIL_RADIO_TECH_EDGE)    || \
                                                       (t == QCRIL_DATA_RIL_RADIO_TECH_TDSCDMA) || \
                                                       (t == QCRIL_DATA_RIL_RADIO_TECH_EHRPD))

#endif /* (QCRIL_RIL_VERSION >= 6) */


/*! @brief Typedef variables internal to module qcril_data.c
*/
typedef struct
{
  unsigned char rflag;  /* determines RIL Token validity */
  RIL_Token     rt;     /* valid RIL token if flag is set */
  int           request;
  void         *data;
  void         *self;
} qcril_data_net_cb_info_t;

/*! @brief Typedef variables internal to module qcril_data.c
*/

#define IPCONV( ip_addr , t ) ( ( ip_addr >> (24 - 8 * t)) & 0xFF)

#define QCRIL_MAX_DEV_NAME_SIZE             DS_CALL_INFO_DEV_NAME_MAX_LEN
#define QCRIL_DATA_MAX_DEVS                 16
#define QCRIL_DATA_MAX_DEVS_SIZE            2  /* Digits required */
#define QCRIL_DATA_INVALID_DEV              (-1)



/* temporary define here until available in msg lib */
#define QMI_WDS_CE_REASON_SYS_NOT_PREF 14

#define QCRIL_QMI_PORT_NAME_SIZE  (16)
typedef struct qcril_data_modem_qmi_port_map_s
{
  int modem_id;
  char qmi_port[QCRIL_QMI_PORT_NAME_SIZE+1];
} qcril_data_modem_port_map;





/*===========================================================================

                         GLOBAL VARIABLES

===========================================================================*/

/* these are events that qcril_data posts to
* itself for being processed from within
* the qcril_event thread. We must start
* the enumeration from 1000 in order to
* avoid the collision with dsi events */
typedef enum qcril_data_internal_events_e
{
  QCRIL_DATA_EV_MIN = 1000,
  QCRIL_DATA_EV_DATA_CALL_STATUS_CHANGED =
  QCRIL_DATA_EV_MIN+1,
  QCRIL_DATA_EV_MAX
} qcril_data_internal_events;

qcril_data_call_info_tbl_type info_tbl[ MAX_CONCURRENT_UMTS_DATA_CALLS ];

/* Table containing the state for handling dsi callbacks */
qcril_data_dsi_cb_tbl_type dsi_cb_tbl[ MAX_CONCURRENT_UMTS_DATA_CALLS ];

/*
  This is used to store last call end reason for responding
  to RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE. Meant for Single PDP only.
  Functionality fails for multiple PDP using QCRIL.
*/
int last_call_end_reason = PDP_FAIL_OEM_DCFAILCAUSE_4;
qmi_client_type global_qcril_qmi_wds_hndl = QCRIL_DATA_HNDL_INVALID;
int global_instance_id = QCRIL_DEFAULT_INSTANCE_ID;
int global_modem_id = QCRIL_DEFAULT_MODEM_ID;
int global_subs_id = QCRIL_DEFAULT_MODEM_STACK_ID;
boolean ignore_ril_tech = TRUE;
qmi_client_type  global_qmi_dsd_hndl = NULL;
qmi_client_type  global_qmi_dpm_hndl = NULL;
unsigned int qcril_data_mtu;
static boolean epc_profile_supported = TRUE;
boolean global_qcril_clat_supported;

/* Info about current data system */
typedef struct
{
  wds_curr_pref_data_sys_enum_v01 pref_data_tech;
  wds_data_system_status_type_v01 data_sys_status;
} qcril_data_legacy_dsd_info_t;

typedef struct
{
  qcril_data_legacy_dsd_info_t  legacy;
  dsd_system_status_ind_msg_v01 qmi_dsd;
  boolean roaming;
} qcril_data_dsd_info_t;

static qcril_data_dsd_info_t  global_dsd_info;
static qmi_client_os_params         dsd_os_params;
static qmi_client_os_params         dpm_os_params;
static qmi_cci_os_signal_type qcril_data_qmi_wds_notifier_os_params;
static qmi_client_type        qcril_data_qmi_wds_notifier;
static qmi_cci_os_signal_type qcril_data_qmi_dsd_notifier_os_params;
static qmi_client_type        qcril_data_qmi_dsd_notifier;

#define QCRIL_NETWORK_INFO_LEN 10

#define QCRIL_DATA_PROP_SIZE     PROPERTY_VALUE_MAX
#define QCRIL_DATA_PROP_FORCE_LIMITED_SY_IND "persist.vendor.radio.force_ltd_sys_ind"

static qcril_data_modem_port_map qcril_data_modem_port_map_tbl[] =
{
  { QCRIL_DEFAULT_MODEM_ID, QMI_PORT_RMNET_0 },
  { QCRIL_SECOND_MODEM_ID, QMI_PORT_RMNET_SDIO_0 }
};

char * default_qmi_port = QMI_PORT_RMNET_0;

typedef enum RIL_tethered_mode_states_e
{
  QCRIL_TETHERED_MODE_OFF = 0,
  QCRIL_TETHERED_MODE_ON = 1
} RIL_tethered_mode_state;

static RIL_tethered_mode_state global_tethered_state = QCRIL_TETHERED_MODE_OFF;

/* this mutex protects info tbl */
pthread_mutex_t info_tbl_mutex;

wds_dormancy_status_enum_v01  global_dorm_status = WDS_DORMANCY_STATUS_ENUM_MAX_ENUM_VAL_V01;

static qcril_data_dormancy_ind_switch_type global_dorm_ind = DORMANCY_INDICATIONS_OFF;
static qcril_data_limited_sys_ind_switch_type global_data_sys_ind_switch = LIMITED_SYS_INDICATIONS_OFF;

/* add 1 for ending NULL character */
typedef char qcril_data_go_dormant_params_type[QCRIL_MAX_DEV_NAME_SIZE+1];

extern int dsi_set_ril_instance(int instance);

extern int dsi_set_modem_subs_id(int subs_id);

extern int ds_atoi(const char * str);
extern void ds_get_epid (char *net_dev, ds_ep_type_t *ep_type, int *epid);

#ifdef RIL_REQUEST_SET_INITIAL_ATTACH_APN

#define QCRIL_PROPERTY_DEFAULT_PROFILE       "persist.vendor.radio.attach.profile"
#define QCRIL_LTE_DEFAULT_PROFILE_VALUE      "0"
#define QCRIL_PROPERTY_DEFAULT_PROFILE_SIZE  (5)
#define QCRIL_LTE_ATTACH_SUCCESS             (0)
#define QCRIL_LTE_ATTACH_FAILURE             (1)

unsigned int global_lte_attach_profile = QDP_INVALID_PROFILE;

#endif /* RIL_REQUEST_SET_INITIAL_ATTACH_APN */

int global_data_force_limited_sys_indication = 0;

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

static int qcril_data_iface_go_dormant
(
  qmi_client_type qmi_wds_hndl,
  dsi_ip_family_t ip_family
);

static int qcril_data_iface_set_mtu
(
  dsi_hndl_t dsi_hndl,
  int mtu
);

static void qcril_data_post_dsi_netctrl_event
(
  qcril_instance_id_e_type instance_id,
  qcril_modem_id_e_type    modem_id,
  RIL_Token                pend_tok,
  qcril_data_call_info_tbl_type *info_tbl_ptr,
  dsi_net_evt_t                  net_evt,
  dsi_evt_payload_t             *payload_ptr
);

static int qcril_data_all_calls_dormant
(
  void
);

static int qcril_data_reg_sys_ind
(
  qcril_data_limited_sys_ind_switch_type       sys_ind_switch
);


static void qcril_data_abort_incompatible_pending_calls
(
  wds_sys_network_info_type_v01 *nw_info
);

static RIL_SubscriptionType qcril_data_dsd_to_ril_sub_id
(
  const dsd_bind_subscription_enum_v01 dsd_sub_id
);

static dsd_dds_switch_type qcril_data_dsd_to_ril_switch_type
(
  const dsd_dds_switch_type_enum_v01 dds_switch_type
);

static void qcril_data_qmi_dsd_init
(
  void
);

static void qcril_data_qmi_dsd_release
(
  void
);

static int qcril_data_qmi_dpm_init
(
  void
);

static boolean qcril_data_is_roaming();
static dsi_ip_family_t qcril_data_get_dsi_ip_family(char *ip_family, size_t size);
static void qcril_data_pick_temporary_call_end_reason(
  dsi_ce_reason_t *first,
  dsi_ce_reason_t *second,
  dsi_ce_reason_t *result);
/*===========================================================================

                                FUNCTIONS

===========================================================================*/
/*-------------------------------------------------------------------------

                               CALLBACK FUNCTIONS

-------------------------------------------------------------------------*/
/*===========================================================================

  FUNCTION:  qcril_data_qmi_wds_notify_cb

===========================================================================*/
/*!
  @brief
    Handles modem SSR indication
  @return
    None.
*/
/*=========================================================================*/
void qcril_data_qmi_wds_notify_cb
(
  qmi_client_type               user_handle,
  qmi_idl_service_object_type   service_obj,
  qmi_client_notify_event_type  service_event,
  void                          *notify_cb_data
)
{
  (void)user_handle;
  (void)service_obj;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_LOG_DEBUG("qcril_data_qmi_wds_notify_cb %"PRIdPTR" %d, modem %d",
                   (intptr_t)notify_cb_data, service_event, global_modem_id);

  if(service_event == QMI_CLIENT_SERVICE_COUNT_INC) {
                  qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
                                     global_modem_id,
                                     QCRIL_DATA_ON_STACK,
                                     QCRIL_EVT_DATA_WDS_RESUME_CALLBACK,
                                     NULL,
                                     QMI_RIL_ZERO,
                                     (RIL_Token) QCRIL_TOKEN_ID_INTERNAL );
  }

  QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================

  FUNCTION:  qcril_data_qmi_wds_err_cb

===========================================================================*/
/*!
  @brief
    Handles error on modem SSR
  @return
    None.
*/
/*=========================================================================*/
void qcril_data_qmi_wds_err_cb
(
  qmi_client_type        user_handle,
  qmi_client_error_type  error,
  void                   *err_cb_data
)
{
  (void) user_handle;
  (void) error;

  QCRIL_LOG_DEBUG("%s", "qcril_data_qmi_wds_err_cb: ENTRY");
  QCRIL_LOG_INFO("WDS service down modem %"PRIdPTR" modem %d",
                                   (intptr_t)err_cb_data, global_modem_id);

  qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
                     global_modem_id,
                     QCRIL_DATA_ON_STACK,
                     QCRIL_EVT_DATA_WDS_SUSPEND_CALLBACK,
                     NULL,
                     QMI_RIL_ZERO,
                     (RIL_Token) QCRIL_TOKEN_ID_INTERNAL );

  QCRIL_LOG_DEBUG("%s", "qcril_data_qmi_wds_err_cb: EXIT");
}

/*===========================================================================

  FUNCTION:  qcril_data_qmi_dsd_notify_cb

===========================================================================*/
/*!
  @brief
    Handles modem SSR indication
  @return
   None.
*/
/*=========================================================================*/
void qcril_data_qmi_dsd_notify_cb
(
  qmi_client_type               user_handle,
  qmi_idl_service_object_type   service_obj,
  qmi_client_notify_event_type  service_event,
  void                          *notify_cb_data
)
{
  (void)user_handle;
  (void)service_obj;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_LOG_DEBUG("qcril_data_qmi_dsd_notify_cb %"PRIdPTR" %d, modem %d",
                   (intptr_t)notify_cb_data, service_event, global_modem_id);

  if(service_event == QMI_CLIENT_SERVICE_COUNT_INC) {
    qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
                       global_modem_id,
                       QCRIL_DATA_ON_STACK,
                       QCRIL_EVT_DATA_DSD_RESUME_CALLBACK,
                       NULL,
                       QMI_RIL_ZERO,
                       (RIL_Token) QCRIL_TOKEN_ID_INTERNAL );
  }

  QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================

  FUNCTION:  qcril_data_qmi_dsd_err_cb

===========================================================================*/
/*!
  @brief
    Handles error on modem SSR
  @return
    None.
*/
/*=========================================================================*/
void qcril_data_qmi_dsd_err_cb
(
  qmi_client_type        user_handle,
  qmi_client_error_type  error,
  void                   *err_cb_data
) {
  (void) user_handle;
  (void) error;

  QCRIL_LOG_DEBUG("%s", "qcril_data_qmi_dsd_err_cb: ENTRY");
  QCRIL_LOG_INFO("DSD service down %"PRIdPTR" modem %d",
                                 (intptr_t)err_cb_data, global_modem_id);

  qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
                     global_modem_id,
                     QCRIL_DATA_ON_STACK,
                     QCRIL_EVT_DATA_DSD_SUSPEND_CALLBACK,
                     NULL,
                     QMI_RIL_ZERO,
                     (RIL_Token) QCRIL_TOKEN_ID_INTERNAL );

  QCRIL_LOG_DEBUG("%s", "qcril_data_qmi_dsd_err_cb: EXIT");

}

/*===========================================================================
  FUNCTION:  qcril_data_qmi_dsd_suspend_hdlr

===========================================================================*/
void qcril_data_qmi_dsd_suspend_hdlr(
  const qcril_request_params_type  *const params_ptr,
  qcril_request_return_type        *const ret_ptr    /*output*/
)
{
  QCRIL_NOTUSED(params_ptr);
  QCRIL_NOTUSED(ret_ptr);

  qmi_idl_service_object_type  dsd_svc_obj;
  qmi_client_error_type        client_err = QMI_NO_ERR;

  QCRIL_LOG_DEBUG( "%s", "qcril_data_qmi_dsd_suspend_hdlr: ENTRY" );

  qcril_data_qmi_dsd_release();

  dsd_svc_obj = dsd_get_service_object_v01();
  if (NULL == dsd_svc_obj) {
    QCRIL_DS_ASSERT( 0, "dsd_get_service_object failed");
    return;
  }

  /* Register notifier for QMI DSD service up */
  client_err = qmi_client_notifier_init(dsd_svc_obj,
                                      &qcril_data_qmi_dsd_notifier_os_params,
                                      &qcril_data_qmi_dsd_notifier);
  if(client_err != QMI_NO_ERR) {
    QCRIL_DS_ASSERT( 0, "Error in initializing dsd notifier" );
  }
  else
  {
    client_err = qmi_client_register_notify_cb(qcril_data_qmi_dsd_notifier,
                                               qcril_data_qmi_dsd_notify_cb,
                                               NULL);

    if(client_err != QMI_NO_ERR) {
      QCRIL_DS_ASSERT( 0, "Error in registering dsd notify cb" );
    }
  }

  QCRIL_LOG_DEBUG( "%s", "qcril_data_qmi_dsd_suspend_hdlr: EXIT" );
}

/*===========================================================================
  FUNCTION:  qcril_data_qmi_dsd_resume_hdlr

===========================================================================*/
void qcril_data_qmi_dsd_resume_hdlr(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr  /*output*/
)
{
  QCRIL_NOTUSED(params_ptr);
  QCRIL_NOTUSED(ret_ptr);

  QCRIL_LOG_DEBUG( "%s", "qcril_data_qmi_dsd_resume_hdlr: ENTRY" );

  /* Release DSD notifier */
  if (qcril_data_qmi_dsd_notifier) {
    qmi_client_release(qcril_data_qmi_dsd_notifier);
    qcril_data_qmi_dsd_notifier = NULL;
  }
  qcril_data_qmi_dsd_init();
  global_dsd_info.roaming = qcril_data_is_roaming();
  QCRIL_LOG_DEBUG("%s(): roaming status = %d", __func__, global_dsd_info.roaming);

  QCRIL_LOG_DEBUG( "%s", "qcril_data_qmi_dsd_resume_hdlr: EXIT" );
}

/*===========================================================================
  FUNCTION:  qcril_data_qmi_wds_suspend_hdlr

===========================================================================*/
void qcril_data_qmi_wds_suspend_hdlr(
  const qcril_request_params_type  *const params_ptr,
  qcril_request_return_type        *const ret_ptr    /*output*/
)
{
  QCRIL_NOTUSED(params_ptr);
  QCRIL_NOTUSED(ret_ptr);

  qmi_idl_service_object_type wds_svc_obj;
  qmi_client_error_type client_err = QMI_NO_ERR;

  QCRIL_LOG_DEBUG( "%s", "qcril_data_qmi_wds_suspend_hdlr: ENTRY" );

  qcril_data_qmi_wds_release();

  wds_svc_obj = wds_get_service_object_v01();
  if (NULL == wds_svc_obj) {
    QCRIL_DS_ASSERT( 0, "wds_get_service_object failed");
    return;
  }

  /* Register notifier for QMI WDS service up */
  client_err = qmi_client_notifier_init( wds_svc_obj,
                                         &qcril_data_qmi_wds_notifier_os_params,
                                         &qcril_data_qmi_wds_notifier);
  if(client_err != QMI_NO_ERR) {
    QCRIL_DS_ASSERT( 0, "Error in initializing wds notifier" );
  }
  else
  {
    client_err = qmi_client_register_notify_cb(qcril_data_qmi_wds_notifier,
                                               qcril_data_qmi_wds_notify_cb,
                                               NULL);
    if(client_err != QMI_NO_ERR) {
      QCRIL_DS_ASSERT( 0, "Error in registering wds notify cb" );
    }
  }

  QCRIL_LOG_DEBUG( "%s", "qcril_data_qmi_wds_suspend_hdlr: EXIT" );
}

/*===========================================================================
  FUNCTION:  qcril_data_qmi_wds_resume_hdlr

===========================================================================*/
void qcril_data_qmi_wds_resume_hdlr(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr  /*output*/
)
{
  QCRIL_NOTUSED(params_ptr);
  QCRIL_NOTUSED(ret_ptr);

  QCRIL_LOG_DEBUG( "%s", "qcril_data_qmi_wds_resume_hdlr: ENTRY" );

  /* Release WDS notifier */
  if(qcril_data_qmi_wds_notifier) {
    qmi_client_release(qcril_data_qmi_wds_notifier);
    qcril_data_qmi_wds_notifier = NULL;
  }
  qcril_data_qmi_wds_init(TRUE);

  QCRIL_LOG_DEBUG( "%s", "qcril_data_qmi_wds_resume_hdlr: EXIT" );
}

/*===========================================================================

  FUNCTION:  qcril_data_post_qmi_events

===========================================================================*/
/*!
    @brief
    Called in order to post events (received directly from qmi) to
    qcril event thread.
    Note: do not assume the context in which handler will be called
    Note: do not assume that info_tbl_ptr would be non NULL as some
    generic QMI events (such as bearer tech ind) are not related to
    a particular info_tbl entry

    @pre if info_tbl_ptr is not null, caller must have locked info_tbl_mutex
    prior to calling this function.

    @return
    None.
*/
/*=========================================================================*/
void qcril_data_post_qmi_events(
  qcril_data_call_info_tbl_type * info_tbl_ptr,
  dsi_net_evt_t net_evt
)
{
  int ret = DSI_ERROR;
  qcril_data_event_data_t * evt;
  RIL_Token tok = 0;
  qcril_instance_id_e_type instance_id = global_instance_id;
  qcril_modem_id_e_type modem_id = global_modem_id;

  QCRIL_LOG_DEBUG("%s","qcril_data_post_qmi_events: entry");

  do
  {

    /* validate info_tbl_ptr if it's not NULL */
    if (NULL != info_tbl_ptr)
    {
      if ((!VALIDATE_LOCAL_DATA_OBJ(info_tbl_ptr)))
      {
        QCRIL_LOG_ERROR( "invalid info_tbl_ptr [%p] ",
                         (unsigned int *)info_tbl_ptr);
        break;
      }

      /* we have a valid info_tbl_ptr, use it to
       * populate/override following local params */
      tok = info_tbl_ptr->pend_tok;
      instance_id = info_tbl_ptr->instance_id;
      modem_id = info_tbl_ptr->modem_id;
    }

    /* Allocate from heap here and clean-up on call end */
    evt = ( qcril_data_event_data_t *)malloc( sizeof( qcril_data_event_data_t ) );
    if ( evt == NULL )
    {
      QCRIL_LOG_ERROR( "%s","unable to alloc mem for event obj" );
      break;
    }
    QCRIL_DS_LOG_DBG_MEM( "event obj alloc", evt );
    memset( evt, 0, sizeof( qcril_data_event_data_t ) );

    /* Populate data event obj */
    evt->evt      = net_evt;
    evt->data     = info_tbl_ptr;
    evt->data_len = sizeof( qcril_data_call_info_tbl_type );
    evt->self     = evt;

    /*
      Call QCRIL API to process this event
      The data event hdlr will be called by RIL thread
      In case of unsol event RIL Token will be 0
    */
    QCRIL_LOG_VERBOSE( "queue QCRIL DATA event for RIL Token [%d] "     \
                       "instance_id [%d], and modem_id [%d]",
                       tok, instance_id, modem_id);

    if(E_SUCCESS != qcril_event_queue( instance_id,
                                       modem_id,
                                       QCRIL_DATA_NOT_ON_STACK,
                                       QCRIL_EVT_DATA_EVENT_CALLBACK,
                                       ( void * )evt,
                                       sizeof( qcril_data_event_data_t ),
                                       tok ))
    {
      QCRIL_LOG_ERROR("%s", "qcril_event_queue failed\n");

      /* free allocated memory for immediate failure from qcril_event_queue */
      QCRIL_DATA_RELEASE_STORAGE(evt);
      break;
    }

    ret = DSI_SUCCESS;
  } while (0);

  if (DSI_SUCCESS == ret)
  {
    QCRIL_LOG_DEBUG("%s","qcril_data_post_qmi_events: exit with success");
  }
  else
  {
    QCRIL_LOG_ERROR("%s","qcril_data_post_qmi_events: exit with error");
  }

}/* qcril_data_post_qmi_events() */

/*===========================================================================

  FUNCTION:  qcril_data_process_pref_tech_change_ind

===========================================================================*/
/*!
    @brief
    processes pref_tech_change_ind

    @return
    SUCCESS
    FAILURE
*/
/*=========================================================================*/
int qcril_data_process_pref_tech_change_ind
(
  wds_curr_pref_data_sys_enum_v01 pref_data_sys
)
{
  int ret = FAILURE;
  int reti = SUCCESS;
  qcril_data_pref_data_tech_e_type qcril_arb_tech;

  do
  {
    QCRIL_LOG_DEBUG("%s", "pref data sys ind is received");
    switch(pref_data_sys)
    {
    case WDS_CURR_PREF_SYS_UNKNOWN_V01:
      QCRIL_LOG_DEBUG("%s","QCRIL_DATA_PREF_DATA_TECH_UNKNOWN is reported");
      qcril_arb_tech = QCRIL_DATA_PREF_DATA_TECH_UNKNOWN;
      break;
    case WDS_CURR_PREF_SYS_CDMA_1X_V01:
      QCRIL_LOG_DEBUG("%s","QCRIL_DATA_PREF_DATA_TECH_CDMA is reported");
      qcril_arb_tech = QCRIL_DATA_PREF_DATA_TECH_CDMA;
      break;
    case WDS_CURR_PREF_SYS_EVDO_V01:
      QCRIL_LOG_DEBUG("%s","QCRIL_DATA_PREF_DATA_TECH_EVDO is reported");
      qcril_arb_tech = QCRIL_DATA_PREF_DATA_TECH_EVDO;
      break;
    case WDS_CURR_PREF_SYS_GPRS_V01:
      QCRIL_LOG_DEBUG("%s","QCRIL_DATA_PREF_DATA_TECH_GSM is reported");
      qcril_arb_tech = QCRIL_DATA_PREF_DATA_TECH_GSM;
      break;
    case WDS_CURR_PREF_SYS_WCDMA_V01:
      QCRIL_LOG_DEBUG("%s","QCRIL_DATA_PREF_DATA_TECH_UMTS is reported");
      qcril_arb_tech = QCRIL_DATA_PREF_DATA_TECH_UMTS;
      break;
    case WDS_CURR_PREF_SYS_LTE_V01:
      QCRIL_LOG_DEBUG("%s","QCRIL_DATA_PREF_DATA_TECH_LTE is reported");
      qcril_arb_tech = QCRIL_DATA_PREF_DATA_TECH_LTE;
      break;
    case WDS_CURR_PREF_SYS_TDSCDMA_V01:
      QCRIL_LOG_DEBUG("%s","QCRIL_DATA_PREF_DATA_TECH_TDSCDMA is reported");
      qcril_arb_tech = QCRIL_DATA_PREF_DATA_TECH_TDSCDMA;
      break;
    default:
      QCRIL_LOG_ERROR("wds reported unrecognized preferred " \
                      "technology [%d]",
                      pref_data_sys);
      reti = FAILURE;
      break;
    }
    if (FAILURE == reti)
    {
      break;
    }
    qcril_data_set_pref_data_tech(global_instance_id,
                                 qcril_arb_tech);
    ret = SUCCESS;
  } while (0);

  if (SUCCESS == ret)
  {
    /* If there's a change in the preferred system */
    if (global_dsd_info.legacy.pref_data_tech != pref_data_sys)
    {
      /* Check if we need to start partial retry for any of the calls */
      qcril_data_util_reinitiate_partial_retry(TRUE, FALSE);
    }

    global_dsd_info.legacy.pref_data_tech = pref_data_sys;

    QCRIL_LOG_DEBUG("%s","qcril_data_process_pref_tech_change_ind succeeded");
  }
  else
  {
    QCRIL_LOG_ERROR("%s","qcril_data_process_pref_tech_change_ind failed");
  }

  return ret;
}

/*===========================================================================

  FUNCTION:  qcril_data_async_cb

===========================================================================*/
/*!
    @brief
    Call back function to handle Events from Modem

    @return
*/
/*=========================================================================*/

 void
qcril_data_async_cb
(
  QCRIL_UNUSED(qmi_client_type   clnt_handle),
  unsigned int                   msg_id,
  void                          *resp_c_struct,
  QCRIL_UNUSED(unsigned int                   resp_c_struct_len),
  void                          *resp_cb_data,
  QCRIL_UNUSED(qmi_client_error_type          transp_err)
)
{
  QCRIL_LOG_DEBUG("Debug : CB function invoked with Msg ID [%d]", msg_id);

  if(!resp_c_struct || !resp_cb_data)
  {
    QCRIL_DATA_RELEASE_STORAGE(resp_cb_data);
    QCRIL_LOG_ERROR("Bad input parameters");
    goto bail;
  }

  qcril_data_cb_struct *cmd_data;
  int sys_err_code = QMI_NO_ERR;
  int qmi_err_code = QMI_SERVICE_ERR_NONE;

  cmd_data = (qcril_data_cb_struct *)resp_cb_data;
  cmd_data->err_val = RIL_E_SUCCESS;

  if(msg_id == QMI_DSD_NOTIFY_DATA_SETTING_REQ_V01)
  {

      dsd_notify_data_settings_resp_msg_v01 *resp = (dsd_notify_data_settings_resp_msg_v01 *)resp_c_struct;
      if(resp->resp.result != QMI_RESULT_SUCCESS_V01)
      {
        sys_err_code = QMI_SERVICE_ERR;
        qmi_err_code = resp->resp.error;
        cmd_data->err_val = RIL_E_INTERNAL_ERR;
        QCRIL_LOG_ERROR("Notify data settings failed with error code [%d]", qmi_err_code);
      }

      //At present IMS does not act on the result of setting RAT pref, so it's not enqueued anywhere
      if(cmd_data->calling_func == SET_RAT_PREF)
      {
        if(resp->resp.result != QMI_RESULT_SUCCESS_V01)
        {
          QCRIL_LOG_ERROR("Unable able to set rat_pref [%d]", cmd_data->rat_pref);
        }
        else
        {
          QCRIL_LOG_DEBUG("Successfully set rat_pref [%d]", cmd_data->rat_pref);
        }
        QCRIL_DATA_RELEASE_STORAGE(cmd_data);
         goto bail;
      }

  }

  else if(msg_id == QMI_DSD_GET_DATA_SETTING_REQ_V01)
  {
    dsd_get_data_settings_resp_msg_v01 *resp = (dsd_get_data_settings_resp_msg_v01 *)resp_c_struct;
    if(resp->resp.result != QMI_RESULT_SUCCESS_V01)
    {
      sys_err_code = QMI_SERVICE_ERR;
      qmi_err_code = resp->resp.error;
      cmd_data->err_val = RIL_E_INTERNAL_ERR;
      QCRIL_LOG_ERROR("Unable to fetch previous RAT pref with error code [%d]", qmi_err_code);
    }

    if(resp->rat_preference_valid)
    {
      cmd_data->prev_rat_pref = resp->rat_preference;
    }

    if(cmd_data->prev_rat_pref != cmd_data->rat_pref)
    {
       QCRIL_LOG_DEBUG("Rat pref to set [%d] Previous rat pref [%d]",cmd_data->rat_pref,
                                                                     cmd_data->prev_rat_pref);

       if(E_SUCCESS != qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
                                          global_modem_id,
                                          QCRIL_DATA_NOT_ON_STACK,
                                          QCRIL_EVT_DATA_SET_RAT_PREF_CALLBACK,
                                          ( void * )cmd_data,
                                          sizeof( qcril_data_cb_struct ),
                                          (RIL_Token) QCRIL_TOKEN_ID_INTERNAL ) )
      {
        QCRIL_LOG_ERROR("%s", "qcril_event_queue failed\n");
      }
      goto bail;
    }
    else
    {
      QCRIL_LOG_DEBUG("Rat pref to set [%d] is same as previous one [%d]",cmd_data->rat_pref,
                                                                          cmd_data->prev_rat_pref);
      QCRIL_DATA_RELEASE_STORAGE(cmd_data);
      goto bail;
    }
  }

  else if(msg_id == QMI_DSD_SET_APN_INFO_REQ_V01)
  {
    dsd_set_apn_info_resp_msg_v01 *resp = (dsd_set_apn_info_resp_msg_v01 *)resp_c_struct;
    if(resp->resp.result != QMI_RESULT_SUCCESS_V01)
    {
      sys_err_code = QMI_SERVICE_ERR;
      qmi_err_code = resp->resp.error;
      cmd_data->err_val = RIL_E_INTERNAL_ERR;
      QCRIL_LOG_ERROR("QMI_DSD_SET_APN_INFO_REQ_V01 failed with error code [%d]", qmi_err_code);
    } else
    {
      QCRIL_LOG_DEBUG("QMI_DSD_SET_APN_INFO_REQ_V01 success");
    }
  }

  else if(msg_id == QMI_DSD_BIND_SUBSCRIPTION_REQ_V01)
  {
    dsd_bind_subscription_resp_msg_v01 *resp = (dsd_bind_subscription_resp_msg_v01 *)resp_c_struct;
     if(resp->resp.result != QMI_RESULT_SUCCESS_V01)
    {
      sys_err_code = QMI_SERVICE_ERR;
      qmi_err_code = resp->resp.error;
      cmd_data->err_val = RIL_E_INTERNAL_ERR;
      QCRIL_LOG_ERROR("failed to bind subscription, err=%d",
                        resp->resp.error);
    }
    QCRIL_DATA_RELEASE_STORAGE(cmd_data);
    goto bail;
  }

  else
  {
     QCRIL_LOG_ERROR("Unexpected msg id received [%d]", msg_id);
     goto bail;
  }

  if(E_SUCCESS != qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
                                    global_modem_id,
                                    QCRIL_DATA_NOT_ON_STACK,
                                    QCRIL_EVT_DATA_ASYNC_CALLBACK,
                                    ( void * )cmd_data,
                                    sizeof( qcril_data_cb_struct ),
                                    (RIL_Token) QCRIL_TOKEN_ID_INTERNAL ) )
  {
    QCRIL_LOG_ERROR("%s", "qcril_event_queue failed\n");
  }

bail:
  /* Free memory allocated for resp buffer in async send */
  QCRIL_DATA_RELEASE_STORAGE(resp_c_struct);
  QCRIL_LOG_DEBUG("%s", "qcril_data_async_cb EXIT\n");

}


/*===========================================================================

  FUNCTION:  qcril_data_async_hdlr

===========================================================================*/
/*!
    @brief
    Function called by QCRIL event queue thread
    @return
*/
/*=========================================================================*/
void qcril_data_async_hdlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr  /*output*/
)
{
  QCRIL_LOG_DEBUG("%s","Debug : Async handler invoked");
  qcril_data_cb_struct *struct_ptr = NULL;

  int msg_id;

  do
  {
    if (!params_ptr || !ret_ptr)
    {
      QCRIL_LOG_ERROR("%s", "invalid parameters");
      break;
    }

    if (params_ptr->instance_id >= QCRIL_MAX_INSTANCE_ID)
    {
      QCRIL_LOG_ERROR("%s", "invalid instance id");
      break;
    }

    qcril_request_resp_params_type resp;
    struct_ptr = (qcril_data_cb_struct *)params_ptr->data;
    msg_id = struct_ptr->msg_id;
     QCRIL_LOG_DEBUG("Debug : Msg ID received [%d] ", msg_id);
    if ( !VALIDATE_LOCAL_DATA_OBJ( struct_ptr) )
    {
      QCRIL_LOG_ERROR( "%s", "bad event obj, cannot free mem, ret with err" );
      break;
    }

     QCRIL_LOG_DEBUG("Debug : Err_val [%d] Received token [%d] event_id [%d] msg_id [%d]",
                      struct_ptr->err_val,struct_ptr->t,struct_ptr->event_id,struct_ptr->msg_id);
     QCRIL_LOG_DEBUG("Debug : struct_ptr [%u] self [%u] calling_func [%d]",struct_ptr, struct_ptr->self, struct_ptr->calling_func);
     QCRIL_LOG_DEBUG("Debug : apn_type_mask [%d] apn_name [%s] enabled ", struct_ptr->apn_type_mask, struct_ptr->apn_name);

    qcril_default_request_resp_params(QCRIL_DEFAULT_INSTANCE_ID,
                                      struct_ptr->t,
                                      struct_ptr->event_id,
                                      struct_ptr->err_val,
                                      &resp);
    qcril_send_request_response(&resp);

  } while (0);

  QCRIL_DATA_RELEASE_STORAGE(struct_ptr);
  QCRIL_LOG_DEBUG( "%s", "qcril_data_dsd_event_hdlr: EXIT" );

}


/*===========================================================================

  FUNCTION:  qcril_data_set_rat_pref_hdlr

===========================================================================*/
/*!
    @brief
    Function called by QCRIL event queue thread especially for
    Setting RAT preference
    @return
*/
/*=========================================================================*/
void qcril_data_set_rat_pref_hdlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr  /*output*/
)
{
  QCRIL_LOG_DEBUG("%s","Debug : Async handler for set_rat_pref invoked");
  qcril_data_cb_struct *struct_ptr = NULL;

  int msg_id;

  do
  {
    if (!params_ptr || !ret_ptr)
    {
      QCRIL_LOG_ERROR("%s", "invalid parameters");
      break;
    }

    if (params_ptr->instance_id >= QCRIL_MAX_INSTANCE_ID)
    {
      QCRIL_LOG_ERROR("%s", "invalid instance id");
      break;
    }

    struct_ptr = (qcril_data_cb_struct *)params_ptr->data;
    msg_id = struct_ptr->msg_id;
     QCRIL_LOG_DEBUG("Debug : Msg ID received [%d] ", msg_id);

    if ( !VALIDATE_LOCAL_DATA_OBJ( struct_ptr) )
    {
      QCRIL_LOG_ERROR( "%s", "bad event obj, cannot free mem, ret with err" );
      break;
    }

     QCRIL_LOG_DEBUG("Debug : Err_val [%d] Received token [%d] event_id [%d] msg_id [%d]",
                      struct_ptr->err_val,struct_ptr->t,struct_ptr->event_id,struct_ptr->msg_id);
     QCRIL_LOG_DEBUG("Debug : struct_ptr [%u] self [%u] calling_func [%d]",struct_ptr, struct_ptr->self, struct_ptr->calling_func);
     QCRIL_LOG_DEBUG("Debug : rat_pref [%d] prev_rat_pref [%d] enabled ", struct_ptr->rat_pref, struct_ptr->prev_rat_pref);

    qmi_client_error_type        rc;
    dsd_notify_data_settings_req_msg_v01 data_setting_set_rat_pref_param;
    dsd_notify_data_settings_resp_msg_v01 *data_setting_set_rat_pref_param_resp = NULL;
    qcril_data_cb_struct *cb_data = NULL;
    qmi_txn_handle txn_handle;
    int rat_pref_type = struct_ptr->rat_pref;

    if(global_qmi_dsd_hndl == NULL)
    {
      QCRIL_LOG_ERROR("%s","DSD Client unavailable");
      break;
    }

    QCRIL_DATA_ALLOC_STORAGE(cb_data, sizeof(qcril_data_cb_struct), bail);
    QCRIL_DATA_ALLOC_STORAGE(data_setting_set_rat_pref_param_resp,
             sizeof(dsd_notify_data_settings_resp_msg_v01), bail);

    memset(cb_data,0,sizeof(qcril_data_cb_struct));
    memset(&data_setting_set_rat_pref_param, 0,
                              sizeof(data_setting_set_rat_pref_param));
    memset(data_setting_set_rat_pref_param_resp, 0,
                            sizeof(dsd_notify_data_settings_resp_msg_v01));

    data_setting_set_rat_pref_param.rat_preference_valid = TRUE;

    if(rat_pref_type >= DSD_RAT_PREFERENCE_INACTIVE_V01
         && rat_pref_type <= DSD_RAT_PREFERENCE_WIFI_PREFERRED_V01)
    {
      data_setting_set_rat_pref_param.rat_preference = rat_pref_type;
      QCRIL_LOG_DEBUG("RAT preference to set is [%d]",
                      rat_pref_type);
    }
    else
    {
      QCRIL_LOG_ERROR("RAT preference is INVALID");
      goto bail;
    }

    cb_data->calling_func = SET_RAT_PREF;
    cb_data->rat_pref = rat_pref_type;
    cb_data->msg_id = QMI_DSD_NOTIFY_DATA_SETTING_REQ_V01;
    cb_data->self = (void *)cb_data;
    QCRIL_LOG_DEBUG("Debug : self [%u] msg_id [%d] calling_func [%d]", cb_data->self, cb_data->msg_id, cb_data->calling_func);
    QCRIL_LOG_DEBUG("Debug : rat_pref [%d] ", cb_data->rat_pref);

    rc = qmi_client_send_msg_async(global_qmi_dsd_hndl,
                                    QMI_DSD_NOTIFY_DATA_SETTING_REQ_V01,
                                    &data_setting_set_rat_pref_param,
                                    sizeof(data_setting_set_rat_pref_param),
                                    data_setting_set_rat_pref_param_resp,
                                    sizeof(dsd_notify_data_settings_resp_msg_v01),
                                    qcril_data_async_cb,
                                    (void *)cb_data,
                                    &txn_handle);

    if (QMI_NO_ERR != rc) {
        QCRIL_LOG_ERROR("failed to send QMI message, err=%d",
                        rc);
       goto bail;
    }

    break;

    bail:
    QCRIL_DATA_RELEASE_STORAGE(cb_data);
    QCRIL_DATA_RELEASE_STORAGE(data_setting_set_rat_pref_param_resp);

  } while (0);

  QCRIL_DATA_RELEASE_STORAGE(struct_ptr);
  QCRIL_LOG_DEBUG( "%s", "qcril_data_dsd_event_hdlr: EXIT" );


}


/*===========================================================================

  FUNCTION:  qcril_data_process_data_sys_status_ind

===========================================================================*/
/*!
    @brief
    processes data system status indication
    All the mapping work is done in RIL, only pass it over to upper layer

    @return
    SUCCESS
    FAILURE
*/
/*=========================================================================*/
int qcril_data_process_data_sys_status_ind
(
  wds_data_system_status_type_v01   *sys_status_type
)
{
  unsigned int i, nw_info_len = 0;
  wds_sys_network_info_type_v01 *nw_info = NULL;

  if (!sys_status_type || 0 == sys_status_type->network_info_len)
  {
    QCRIL_LOG_ERROR("Invalid parameter");
    return FAILURE;
  }

  QCRIL_LOG_DEBUG("%s", "qcril_data_process_data_sys_status_ind ENTRY");
  QCRIL_LOG_DEBUG("recevd curr_data_sys_status pref_network=0x%x, len=%d",
                  sys_status_type->preferred_network,
                  sys_status_type->network_info_len);

  for(i = 0; i < sys_status_type->network_info_len; i++)
  {
    QCRIL_LOG_DEBUG("recvd curr_data_sys_status nw=0x%x, rat_mask=0x%x, so_mask=0x%x",
                    sys_status_type->network_info[i].network,
                    sys_status_type->network_info[i].rat_mask,
                    sys_status_type->network_info[i].so_mask);
  }

  /* In case of 1x to ehrpd or vice versa rat change abort the pending calls with older tech */
  if (TRUE == qcril_data_util_is_new_rat_1x_ehrpd(sys_status_type, &nw_info))
  {
    QCRIL_LOG_DEBUG( "data sys status changed to 1x/eHRPD, aborting incompatible calls" );
    qcril_data_abort_incompatible_pending_calls(nw_info);
  }


  /* If there's a change in the data system status, check if partial retry needs to
     be restarted for any active calls */
  if (TRUE == qcril_data_util_data_sys_status_changed(&global_dsd_info.legacy.data_sys_status,
                                                      sys_status_type))
  {
    boolean check_ce_reason;

    QCRIL_LOG_DEBUG( "data sys status has changed, initiating partial retry" );

    /* In case of a change in the preferred network, skip the CE failure code check */
    check_ce_reason = (global_dsd_info.legacy.data_sys_status.preferred_network != sys_status_type->preferred_network) ?
                      FALSE:
                      TRUE;
    qcril_data_util_reinitiate_partial_retry(TRUE, check_ce_reason);
  }
  else
  {
    QCRIL_LOG_DEBUG( "data sys status has not changed, skipping partial retry" );
  }

  nw_info_len = MIN(global_dsd_info.legacy.data_sys_status.network_info_len,
                    sys_status_type->network_info_len);

  memcpy(&global_dsd_info.legacy.data_sys_status.network_info[0],
         sys_status_type->network_info,
         sizeof(wds_sys_network_info_type_v01) * nw_info_len);

  global_dsd_info.legacy.data_sys_status.network_info_len = nw_info_len;
  global_dsd_info.legacy.data_sys_status.preferred_network = sys_status_type->preferred_network;


  QCRIL_LOG_DEBUG( "Updated legacy len - %d,MIN len -%d",
                   global_dsd_info.legacy.data_sys_status.network_info_len, nw_info_len);

  qcril_data_set_data_sys_status(global_instance_id,
                                &global_dsd_info.legacy.data_sys_status);

  QCRIL_LOG_DEBUG("%s", "qcril_data_process_data_sys_status_ind EXIT");

  return SUCCESS;
}

/*===========================================================================

  FUNCTION:  qcril_data_process_tethered_state_change_ind

===========================================================================*/
/*!
    @brief
    processes tethered_state_change_ind

    @return
    SUCCESS
    FAILURE
*/
/*=========================================================================*/
int qcril_data_process_tethered_state_change_ind
(
  wds_data_call_status_change_enum_v01  data_call_status,
  wds_event_data_call_type_enum_v01     data_call_type
)
{
  int ret = FAILURE;
  int reti = SUCCESS;

  QCRIL_LOG_DEBUG("%s", "tethered data call status is received");

  do
  {
    if( data_call_type != WDS_DATA_CALL_TETHERED_V01 )
    {
      QCRIL_LOG_DEBUG( "call type[%d] not tethered data, ignoring",
                       data_call_type );
      ret = SUCCESS;
      break;
    }
    else
    {
      switch( data_call_status )
      {
        case WDS_DATA_CALL_ACTIVATED_V01:
          QCRIL_LOG_DEBUG("%s", "tethered data call is activated "
                          "on the modem side");
          global_tethered_state = QCRIL_TETHERED_MODE_ON;
          break;
        case WDS_DATA_CALL_TERMINATED_V01:
          global_tethered_state = QCRIL_TETHERED_MODE_OFF;
          QCRIL_LOG_DEBUG("%s", "tethered data call is terminated "
                          "on the modem side");

          /* Since a tethered call got terminated, check if we need to
             start our partial retries */
          qcril_data_util_reinitiate_partial_retry(FALSE, TRUE);
          break;
        default:
          QCRIL_LOG_ERROR("wds reported unrecognized data "
                          "call status on modem side [%d]",
                          data_call_status);
          reti = FAILURE;
          break;
      }
      if (FAILURE == reti)
      {
        break;
      }
      /* info_tbl_mutex need not be locked in this case because
         info_tbl_ptr is NULL */
      qcril_data_post_qmi_events(NULL,
                                 (dsi_net_evt_t) QCRIL_DATA_EV_DATA_CALL_STATUS_CHANGED);
      ret = SUCCESS;
    }
  } while (0);

  if (SUCCESS == ret)
  {
    QCRIL_LOG_DEBUG("%s","qcril_data_process_tethered_state_change_ind succeeded");
  }
  else
  {
    QCRIL_LOG_ERROR("%s","qcril_data_process_tethered_state_change_ind failed");
  }

  return ret;
}

/*===========================================================================

  FUNCTION:  qcril_data_process_ext_ip_config_ind

===========================================================================*/
/*!
    @brief
    Processes extended IP config change indication

    @pre caller must have locked info_tbl_mutex prior to calling this function.

    @return
    None.
*/
/*=========================================================================*/
void qcril_data_process_ext_ip_config_ind
(
  qcril_instance_id_e_type        instance_id,
  qmi_client_type                 qmi_wds_hndl,
  wds_req_settings_mask_v01       ext_ip_ind,
  qcril_data_call_info_tbl_type   *info_tbl_ptr
)
{
  wds_get_runtime_settings_resp_msg_v01  call_info;
  wds_req_settings_mask_v01    req_mask = QMI_WDS_MASK_REQ_SETTINGS_OP_RES_PCO_V01;

  QCRIL_LOG_FUNC_ENTRY();

  if (QCRIL_DATA_HNDL_INVALID == qmi_wds_hndl || NULL == info_tbl_ptr)
  {
    QCRIL_LOG_ERROR( "invalid input parameter" );
    goto bail;
  }

  if (ext_ip_ind & QMI_WDS_MASK_REQ_SETTINGS_OP_RES_PCO_V01)
  {
    memset(&call_info, 0, sizeof(call_info));

    /* Query Modem for requested parameters */
    if (E_SUCCESS != qcril_data_qmi_wds_get_runtime_settings(qmi_wds_hndl,
                                                             req_mask,
                                                             &call_info))
    {
      QCRIL_LOG_ERROR("%s", "failed on qcril_data_qmi_wds_get_runtime_settings");
      goto bail;
    }

    /* Send the operator reserved PCO information to the Telephony layer */
    if (call_info.operator_reserved_pco_valid)
    {
      /* Legacy Custom Event */
      QCRIL_LOG_DEBUG( "Operator reserved PCO info update" );
      QCRIL_LOG_DEBUG( "MCC: 0x%x", (int)call_info.operator_reserved_pco.mcc );
      QCRIL_LOG_DEBUG( "MNC: 0x%x", (int)call_info.operator_reserved_pco.mnc );
      QCRIL_LOG_DEBUG( "MNC includes PCS digit: %s",
                       (call_info.operator_reserved_pco.mnc_includes_pcs_digit)? "TRUE": "FALSE" );
      QCRIL_LOG_DEBUG( "App info len: %d", (int)call_info.operator_reserved_pco.app_specific_info_len );
      QCRIL_LOG_DEBUG( "Container ID: 0x%x", (int)call_info.operator_reserved_pco.container_id );

      qcril_hook_unsol_response(instance_id,
                                QCRIL_EVT_HOOK_UNSOL_OPERATOR_RESERVED_PCO,
                                (char*)&call_info.operator_reserved_pco,
                                sizeof(call_info.operator_reserved_pco));

#if (QCRIL_RIL_VERSION >= 14)
      /* RIL Event */
      RIL_PCO_Data pkt;
      pkt.cid = info_tbl_ptr->cid;
      pkt.bearer_proto = (qmi_wds_hndl == info_tbl_ptr->qmi_wds_hndl_v4) ? "IP" : "IPV6";
      pkt.pco_id = call_info.operator_reserved_pco.container_id;
      pkt.contents_length = call_info.operator_reserved_pco.app_specific_info_len;
      pkt.contents = (char*) call_info.operator_reserved_pco.app_specific_info;

      qcril_unsol_resp_params_type resp;
      qcril_default_unsol_resp_params(instance_id, RIL_UNSOL_PCO_DATA, &resp);
      resp.resp_pkt = &pkt;
      resp.resp_len = sizeof(RIL_PCO_Data);
      resp.logstr = NULL;

      qcril_send_unsol_response(&resp);
#endif
    }
  }

#if (QCRIL_RIL_VERSION >= 10)
  if (ext_ip_ind & QMI_WDS_MASK_REQ_SETTINGS_PCSCF_SERVER_ADDR_LIST_V01)
  {
    qcril_data_util_fill_pcscf_addr(info_tbl_ptr);
    qcril_data_unsol_call_list_changed(info_tbl_ptr->instance_id);
  }
#endif

bail:
  QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================
 *
 *     FUNCTION:  qcril_data_get_force_limited_sys_ind()
 *
===========================================================================*/
/*!
 *     @brief
 *         Check ADB property to override force limited sys indication
 *
 *     @return
 *         Integer value read from the property.
 *
 *
*/
/*=========================================================================*/
int qcril_data_get_force_limited_sys_ind()
{
    char args[QCRIL_DATA_PROP_SIZE];
    int   ret;

    memset(args, 0, sizeof(args));
    property_get(QCRIL_DATA_PROP_FORCE_LIMITED_SY_IND, args, "0");

    ret = ds_atoi( args );

    QCRIL_LOG_DEBUG(" persist.data.suggested_retry_time = %d",ret);

    return ret;
}

/*===========================================================================

  FUNCTION:  qcril_data_process_wds_ind

===========================================================================*/
/*!
    @brief
    Processes qmi wds indications
    Currently, this function is responsible for processing
    physlink events - these are specific to an info_tbl entry (or a call)
    bearer tech ind - these are generic events
    modem assisted keep alive indications

    @pre caller must have locked info_tbl_mutex prior to calling this function.

    @return
    None.
*/
/*=========================================================================*/
void qcril_data_process_wds_ind
(
  qmi_client_type               wds_hndl,
  void                         *user_data,
  int                           ind_id,
  qcril_data_wds_indication_data_type *ind_data
)
{
  int ret = FAILURE;
  int reti = SUCCESS;
  qcril_data_call_info_tbl_type * info_tbl_ptr = NULL;
  int all_calls_dormant;

  QCRIL_LOG_DEBUG("%s","qcril_data_process_wds_ind: entry");

  do
  {
    /* sanity checking */
    if (wds_hndl == QCRIL_DATA_HNDL_INVALID)
    {
      QCRIL_LOG_ERROR("invalid wds hndl received [%d]", wds_hndl);
      break;
    }

    if (NULL == ind_data)
    {
      QCRIL_LOG_ERROR("%s","NULL ind_data recieved");
      break;
    }

    QCRIL_LOG_DEBUG("qcril_data_process_wds_ind: Handling ind [%x]",
                    ind_id);

    /* if a non NULL user_data is received, validate it */
    if (NULL != user_data)
    {
      info_tbl_ptr = (qcril_data_call_info_tbl_type *)user_data;
      if(!VALIDATE_LOCAL_DATA_OBJ(info_tbl_ptr))
      {
        /* The call may have been cleaned up through dsi netctrl
           NO_NET event.  Do not report invalid user_data error
           if the self pointer is null.*/
        if (info_tbl_ptr->self != NULL)
        {
          QCRIL_LOG_ERROR("%s","invalid info_tbl_ptr (user_data)" \
                          "received by qcril_data_process_wds_ind");
        }
        break;
      }

      if (info_tbl_ptr->qmi_wds_hndl_v4 != wds_hndl &&
              info_tbl_ptr->qmi_wds_hndl_v6 != wds_hndl)
      {
        QCRIL_LOG_ERROR("wds hndl mismatch [0x%x] passed by qmi versus" \
                        "[0x%x, 0x%x] stored in info_tbl entry",
                        wds_hndl,
                        info_tbl_ptr->qmi_wds_hndl_v4,
                        info_tbl_ptr->qmi_wds_hndl_v6);
        break;
      }
    }
    /* if NULL user_data is received, now try to
     * match wds_hndl with our global wds hndl */
    else if (wds_hndl != global_qcril_qmi_wds_hndl)
    {
      QCRIL_LOG_ERROR("wds indication arrived with a handle [0x%x] "    \
                      "that we do not recognize", wds_hndl);
      break;
    }

    /* handle individual wds indications */
    reti = SUCCESS;

    switch (ind_id)
    {
    case QMI_WDS_EVENT_REPORT_IND_V01:
      if (ind_data->event_report.dormancy_status_valid)
      {
        switch(ind_data->event_report.dormancy_status)
        {
        case WDS_DORMANCY_STATUS_DORMANT_V01:
          QCRIL_LOG_DEBUG("%s","PHYSLINK went dormant");
          /* we mandate that physlink event be associated
           * with a valid call (i.e. info_tbl_ptr), otherwise
           * it's ignored  */
          if (NULL == info_tbl_ptr)
          {
            QCRIL_LOG_DEBUG("%s", "generic physlink indication is ignored" \
                            " by qcril_data_netctrl");
            break;
          }

          info_tbl_ptr->dorm_status = WDS_DORMANCY_STATUS_DORMANT_V01;
          all_calls_dormant = qcril_data_all_calls_dormant();
          if (all_calls_dormant &&
              (global_data_sys_ind_switch == LIMITED_SYS_INDICATIONS_ON) &&
              (global_data_force_limited_sys_indication == 0))
          {
            qcril_data_reg_sys_ind(LIMITED_SYS_INDICATIONS_ON);
          }

          qcril_data_post_qmi_events(info_tbl_ptr,
                                     DSI_EVT_PHYSLINK_DOWN_STATE);
          break;
        case WDS_DORMANCY_STATUS_ACTIVE_V01:
          QCRIL_LOG_DEBUG("%s","PHYSLINK went active");
          /* we mandate that physlink event be associated
           * with a valid call (i.e. info_tbl_ptr), otherwise
           * it's ignored  */
          if (NULL == info_tbl_ptr)
          {
            QCRIL_LOG_DEBUG("%s", "generic physlink indication is ignored" \
                            " by qcril_data_netctrl");
            break;
          }

          info_tbl_ptr->dorm_status = WDS_DORMANCY_STATUS_ACTIVE_V01;
          if ((global_data_sys_ind_switch == LIMITED_SYS_INDICATIONS_ON) &&
              (global_data_force_limited_sys_indication == 0))
          {
            qcril_data_reg_sys_ind(LIMITED_SYS_INDICATIONS_OFF);
          }

          qcril_data_post_qmi_events(info_tbl_ptr,
                                     DSI_EVT_PHYSLINK_UP_STATE);
          break;
        default:
          QCRIL_LOG_ERROR("invalid dorm status [%d] received",
                          ind_data->event_report.dormancy_status);
          reti = FAILURE;
          break;
        }
      }

      if( (ind_data->event_report.data_call_status_valid) &&
          (ind_data->event_report.data_call_type_valid) )
      {
        if (SUCCESS != qcril_data_process_tethered_state_change_ind
                         (ind_data->event_report.data_call_status,
                          ind_data->event_report.data_call_type.data_call_type))
        {
          QCRIL_LOG_ERROR("%s","qcril_data_process_tethered_state_change_ind "\
                          "failed");
          reti = FAILURE;
          break;
        }
      }
      break;

    case QMI_WDS_EXTENDED_IP_CONFIG_IND_V01:
      /* Process extended IP config indication */
      if(ind_data->ext_ip_ind.changed_ip_config_valid)
      {
        qcril_data_process_ext_ip_config_ind((info_tbl_ptr) ? info_tbl_ptr->instance_id :
                                                            (unsigned int) global_instance_id,
                                           wds_hndl,
                                           ind_data->ext_ip_ind.changed_ip_config,
                                           info_tbl_ptr);
      }
      else
      {
        QCRIL_LOG_ERROR("%s","Dropping extended_ip_config ind");
      }
      break;

#if (QCRIL_RIL_VERSION >= 15)
    case QMI_WDS_MODEM_ASSISTED_KA_STATUS_IND_V01:
      QCRIL_LOG_DEBUG("QMI_WDS_MODEM_ASSISTED_KA_STATUS_IND_V01 status = %d", ind_data->ka_status.status);

      wds_modem_assisted_ka_status_ind_msg_v01 payload;
      payload.status = ind_data->ka_status.status;
      payload.keep_alive_handle = ind_data->ka_status.keep_alive_handle;

      if (qcril_event_queue(global_instance_id,
                        global_modem_id,
                        QCRIL_DATA_ON_STACK,
                        QCRIL_EVT_DATA_KEEPALIVE_EVENT_CALLBACK,
                        (void*)&payload,
                        sizeof(wds_modem_assisted_ka_status_ind_msg_v01),
                        (RIL_Token) QCRIL_TOKEN_ID_INTERNAL) != E_SUCCESS) {
        QCRIL_LOG_ERROR("!!! Failed to queue QCRIL_EVT_DATA_KEEPALIVE_EVENT_CALLBACK");
      }
      QCRIL_LOG_DEBUG(">>> QCRIL_EVT_DATA_KEEPALIVE_EVENT_CALLBACK queued");
      break;
#endif /* (QCRIL_RIL_VERSION >= 15) */

    default:
      QCRIL_LOG_DEBUG("Ignoring wds ind cb event %d", ind_id);
      break;
    }
    if (FAILURE == reti)
    {
      break;
    }

    ret = SUCCESS;
  } while (0);

  if (SUCCESS == ret)
  {
    QCRIL_LOG_DEBUG("%s","qcril_data_process_wds_ind: exit with success");
  }
  else
  {
    QCRIL_LOG_ERROR("%s","qcril_data_process_wds_ind: exit with error");
  }
}/*qcril_data_process_wds_ind*/


/*===========================================================================

  FUNCTION:  qcril_data_get_qmi_dsd_subscription_id

===========================================================================*/
/*!
    @brief
    Returns the subscription ID corresponding to the current RIL instance

    @return
    None.
*/
/*=========================================================================*/
static dsd_bind_subscription_enum_v01
qcril_data_get_qmi_dsd_subscription_id(void)
{
  dsd_bind_subscription_enum_v01  sub_num;

  switch (global_subs_id)
  {
    case WDS_SECONDARY_SUBS_V01:
      sub_num = DSD_SECONDARY_SUBS_V01;
      break;

    default:
      sub_num = DSD_PRIMARY_SUBS_V01;
      break;
  }

  return sub_num;
}

/*===========================================================================

  FUNCTION:  qcril_data_process_qmi_dsd_ind

===========================================================================*/
/*!
    @brief
    Processes the QMI DSD indications

    @return
    None.
*/
/*=========================================================================*/
static void qcril_data_process_qmi_dsd_ind
(
  dsd_system_status_ind_msg_v01  *ind_data
)
{
  dsd_system_status_info_type_v01  *info_old = NULL;
  dsd_system_status_info_type_v01  *info_new = NULL;
  unsigned int i;

  QCRIL_LOG_DEBUG( "%s", "qcril_data_process_qmi_dsd_ind: ENTRY" );

  if (!ind_data)
  {
    QCRIL_LOG_ERROR( "invalid input" );
    return;
  }

  /* If the available systems in invalid, report NULL bearer to RIL */
  if (FALSE == ind_data->avail_sys_valid)
  {
    QCRIL_LOG_DEBUG( "invalid avail sys, reporting NULL bearer" );

    ind_data->avail_sys_valid = TRUE;
    ind_data->avail_sys_len = 1;
    ind_data->avail_sys[0].technology = DSD_SYS_NETWORK_3GPP_V01;
    ind_data->avail_sys[0].rat_value = DSD_SYS_RAT_EX_NULL_BEARER_V01;
    ind_data->avail_sys[0].so_mask = 0ull;

    memcpy(&global_dsd_info.qmi_dsd, ind_data, sizeof(dsd_system_status_ind_msg_v01));
  }
  else
  {
    QCRIL_LOG_DEBUG("rcvd avail_sys_len=0x%x", ind_data->avail_sys_len);

    for (i = 0 ; i < ind_data->avail_sys_len; ++i)
    {
      QCRIL_LOG_DEBUG("sys[%d] network=0x%x, rat=0x%08x, so_mask=0x%016llx",
                      i,
                      ind_data->avail_sys[i].technology,
                      ind_data->avail_sys[i].rat_value,
                      ind_data->avail_sys[i].so_mask);
    }

    if(TRUE == ind_data->apn_avail_sys_info_valid)
    {
      QCRIL_LOG_DEBUG("rcvd apn_avil_sys_info_len=0x%x", ind_data->apn_avail_sys_info_len);

      for (i = 0 ; i < ind_data->apn_avail_sys_info_len; ++i)
      {
        QCRIL_LOG_DEBUG("pdn[%d] name=%s",
                        i,
                        ind_data->apn_avail_sys_info[i].apn_name);
      }
    }
    info_old = &global_dsd_info.qmi_dsd.avail_sys[0];
    info_new = &ind_data->avail_sys[0];

    /* If there's a change in the data system status, check if partial retry needs to
       be restarted for any active calls */
    if (DSD_SYS_RAT_EX_NULL_BEARER_V01 != info_new->rat_value &&
        (info_old->technology != info_new->technology ||
         info_old->rat_value  != info_new->rat_value  ||
         info_old->so_mask    != info_new->so_mask))
    {
      boolean check_ce_reason;

      QCRIL_LOG_DEBUG( "data sys status has changed, initiating partial retry" );

      /* In case of a change in the network, skip the CE failure code check */
      check_ce_reason = ((info_old->technology != info_new->technology) ? FALSE : TRUE);

      qcril_data_util_reinitiate_partial_retry(TRUE, check_ce_reason);
    }
    else
    {
      QCRIL_LOG_DEBUG( "data sys status has not changed, skipping partial retry" );
    }

    if (DSD_SYS_RAT_EX_NULL_BEARER_V01 != info_new->rat_value)
    {
      memcpy(&global_dsd_info.qmi_dsd, ind_data, sizeof(dsd_system_status_ind_msg_v01));
    }
    else
    {
      QCRIL_LOG_DEBUG( "NULL bearer reported, skipping cache update" );
    }
  }

  /* Update RIL with the DSD info */
  qcril_data_set_dsd_sys_status(ind_data);

  QCRIL_LOG_DEBUG("%s", "qcril_data_process_qmi_dsd_ind EXIT");
}

/*===========================================================================

  FUNCTION:  qcril_data_qmi_dsd_ind_cb

===========================================================================*/
/*!
    @brief
    Receives QMI-DSD indications

    @return
    None.
*/
/*=========================================================================*/
static void qcril_data_qmi_dsd_ind_cb
(
  qmi_client_type                user_handle,
  unsigned long                  msg_id,
  unsigned char                  *ind_buf,
  int                            ind_buf_len,
  void                           *ind_cb_data
)
{
  dsd_system_status_ind_msg_v01  *dsd_ind_sys_status_msg = NULL;
  dsd_current_dds_ind_msg_v01    *dsd_ind_curr_dds_msg = NULL;
  dsd_switch_dds_ind_msg_v01     *dsd_ind_switch_dds_msg = NULL;

  int                           rc = QMI_INTERNAL_ERR;

  (void) ind_cb_data;

  if (!user_handle || !ind_buf)
  {
    QCRIL_LOG_ERROR("qcril_data_qmi_dsd_ind_cb: bad param(s)");
    goto bail;
  }

  QCRIL_LOG_DEBUG("qcril_data_qmi_dsd_ind_cb: recvd ind=%lu", msg_id);

  switch (msg_id)
  {
    case QMI_DSD_SYSTEM_STATUS_IND_V01:
      dsd_ind_sys_status_msg = calloc(1, sizeof(dsd_system_status_ind_msg_v01));
      if (!dsd_ind_sys_status_msg)
      {
        QCRIL_LOG_ERROR("qcril_data_qmi_dsd_ind_cb: failed to alloc memory");
        goto bail;
      }
      /* Decode the QMI indication message to its corresponding C structure */
      rc = qmi_client_message_decode(user_handle,
                                     QMI_IDL_INDICATION,
                                     msg_id,
                                     ind_buf,
                                     ind_buf_len,
                                     dsd_ind_sys_status_msg,
                                     sizeof(*dsd_ind_sys_status_msg));

      if (QMI_NO_ERR != rc)
      {
        QCRIL_LOG_ERROR("qcril_data_qmi_dsd_ind_cb: indication decode failed err=%d", rc);
        goto bail;
      }

      qcril_event_queue(global_instance_id,
                        global_modem_id,
                        QCRIL_DATA_NOT_ON_STACK,
                        QCRIL_EVT_DATA_DSD_EVENT_CALLBACK,
                        dsd_ind_sys_status_msg,
                        sizeof(*dsd_ind_sys_status_msg),
                        (RIL_Token) QCRIL_TOKEN_ID_INTERNAL);
      break;

    case QMI_DSD_CURRENT_DDS_IND_V01:
      dsd_ind_curr_dds_msg = calloc(1, sizeof(dsd_current_dds_ind_msg_v01));
      if (!dsd_ind_curr_dds_msg)
      {
        QCRIL_LOG_ERROR("qcril_data_qmi_dsd_ind_cb: failed to alloc memory");
        goto bail;
      }
      rc = qmi_client_message_decode(user_handle,
                                     QMI_IDL_INDICATION,
                                     msg_id,
                                     ind_buf,
                                     ind_buf_len,
                                     dsd_ind_curr_dds_msg,
                                     sizeof(*dsd_ind_curr_dds_msg));

      if (QMI_NO_ERR != rc)
      {
        QCRIL_LOG_ERROR("qcril_data_qmi_dsd_ind_cb: indication decode failed err=%d", rc);
        goto bail;
      }

      DDSSubIdInfo ddsInfo;
      ddsInfo.dds_sub_id = qcril_data_dsd_to_ril_sub_id(dsd_ind_curr_dds_msg->dds);
      if( dsd_ind_curr_dds_msg->dds_switch_type_valid )
      {
        ddsInfo.switch_type = qcril_data_dsd_to_ril_switch_type(dsd_ind_curr_dds_msg->dds_switch_type);
      } else {
        ddsInfo.switch_type = DSD_DDS_DURATION_PERMANANT_V01;
      }
      qcril_data_new_dds_info(ddsInfo);
      break;

    case QMI_DSD_SWITCH_DDS_IND_V01:
      dsd_ind_switch_dds_msg = calloc(1, sizeof(dsd_switch_dds_ind_msg_v01));
      if (!dsd_ind_switch_dds_msg)
      {
        QCRIL_LOG_ERROR("qcril_data_qmi_dsd_ind_cb: failed to alloc memory");
        goto bail;
      }
      rc = qmi_client_message_decode(user_handle,
                                     QMI_IDL_INDICATION,
                                     msg_id,
                                     ind_buf,
                                     ind_buf_len,
                                     dsd_ind_switch_dds_msg,
                                     sizeof(*dsd_ind_switch_dds_msg));

      if (QMI_NO_ERR != rc)
      {
        QCRIL_LOG_ERROR("qcril_data_qmi_dsd_ind_cb: indication decode failed err=%d", rc);
        goto bail;
      }

      dsd_dds_switch_result_enum_v01 switch_result = dsd_ind_switch_dds_msg->dds_switch_result;
      if (DSD_DDS_SWITCH_ALLOWED_V01 != switch_result)
      {
        QCRIL_LOG_ERROR("qcril_data_qmi_dsd_ind_cb: switch failed with %d",
          switch_result);
        DDSStatusInfo status;
        status.result = RIL_E_INTERNAL_ERR;
        status.error_cause = switch_result;
        qcril_data_dds_status_followup(&status);
      }
      break;

    case QMI_DSD_ROAMING_STATUS_CHANGE_IND_V01:
    {
      dsd_roaming_status_change_ind_msg_v01 roaming_msg;
      memset(&roaming_msg, 0, sizeof(dsd_roaming_status_change_ind_msg_v01));
      rc = qmi_client_message_decode(user_handle,
                                     QMI_IDL_INDICATION,
                                     msg_id,
                                     ind_buf,
                                     ind_buf_len,
                                     &roaming_msg,
                                     sizeof(roaming_msg));

      QCRIL_LOG_DEBUG("%s(): roaming status valid = %d, roaming status = %d, tech=%d", __func__,
        roaming_msg.roaming_status_valid, roaming_msg.roaming_status, roaming_msg.technology);

      if (roaming_msg.roaming_status_valid) {
        global_dsd_info.roaming = roaming_msg.roaming_status;
      }

    }
    break;

    default:
      break;
  }

bail:
  // We should only attempt to free if there was an error queung since we are
  // sending this via heap
  if (QMI_NO_ERR !=rc) QCRIL_DATA_RELEASE_STORAGE(dsd_ind_sys_status_msg);

  // These should be freed as we are sending them "on stack"
  QCRIL_DATA_RELEASE_STORAGE(dsd_ind_curr_dds_msg);
  QCRIL_DATA_RELEASE_STORAGE(dsd_ind_switch_dds_msg);

  return;
}


/*===========================================================================

  FUNCTION:  qcril_data_net_cb

===========================================================================*/
/*!
    @brief
    Called on call control events from Data Services (DSI).

    @pre Before calling, dsi_cb_tbl_mutex must not be locked by the
         calling thread

    @return
    None.
*/
/*=========================================================================*/
void qcril_data_net_cb(
  dsi_hndl_t     dsi_hndl,
  void          *user_data,
  dsi_net_evt_t      net_evt,
  dsi_evt_payload_t *payload
)
{
  qcril_data_dsi_cb_tbl_type *dsi_cb_tbl_ptr = NULL;

  QCRIL_LOG_DEBUG("%s", "qcril_data_net_cb: ENTRY");

  QCRIL_DATA_MUTEX_LOCK(&dsi_cb_tbl_mutex);

  /* Input Validation, user data was pointer to dsi_cb_tbl */
  dsi_cb_tbl_ptr = ( qcril_data_dsi_cb_tbl_type *) user_data;
  if (NULL == dsi_cb_tbl_ptr)
  {
    QCRIL_LOG_ERROR("%s","qcril_data_net_cb:invalid arg, user_data is NULL");
    goto err_bad_input;
  }

  if ( ( NULL == dsi_cb_tbl_ptr->info_tbl_ptr ) ||
       ( dsi_hndl != dsi_cb_tbl_ptr->dsi_hndl ) ||
       ( QCRIL_DATA_INVALID_INSTANCE_ID == dsi_cb_tbl_ptr->instance_id ) ||
       ( QCRIL_DATA_INVALID_MODEM_ID == dsi_cb_tbl_ptr->modem_id) )
  {
    QCRIL_LOG_ERROR( "invalid arg, user_data [%p], info_tbl_ptr [%p], "
                     "dsi_hndl [%p], dsi_cb_tbl_ptr->dsi_hndl [%p], "
                     "instance_id [%#x], modem_id [%#x], payload [%p]",
                     (unsigned int *)user_data,
                     (unsigned int *)dsi_cb_tbl_ptr->info_tbl_ptr,
                     (unsigned int *)dsi_hndl,
                     (unsigned int *)dsi_cb_tbl_ptr->dsi_hndl,
                     (unsigned int)dsi_cb_tbl_ptr->instance_id,
                     (unsigned int)dsi_cb_tbl_ptr->modem_id,
                     payload );
    goto err_bad_input;
  }

  qcril_data_post_dsi_netctrl_event(dsi_cb_tbl_ptr->instance_id,
                                    dsi_cb_tbl_ptr->modem_id,
                                    dsi_cb_tbl_ptr->pend_tok,
                                    dsi_cb_tbl_ptr->info_tbl_ptr,
                                    net_evt,
                                    payload );

  QCRIL_DATA_MUTEX_UNLOCK(&dsi_cb_tbl_mutex);

  QCRIL_LOG_DEBUG("%s","qcril_data_net_cb: EXIT with suc");
  return;

err_bad_input:
  QCRIL_DATA_MUTEX_UNLOCK(&dsi_cb_tbl_mutex);
  QCRIL_LOG_ERROR("%s","qcril_data_net_cb: EXIT with err");
  return;
}/* qcril_data_net_cb() */


/*===========================================================================
  FUNCTION:  qcril_data_get_numeric_ril_technology
===========================================================================*/
/*!
  @brief
  Maps the given RIL technology string to a numeric value

  @param
  ril_tech - RIL technology string

  @return
  Numeric value corresponding to the ril_tech string
*/
/*=========================================================================*/
qcril_data_ril_radio_tech_t
qcril_data_get_numeric_ril_technology
(
  const char *ril_tech
)
{
  qcril_data_ril_radio_tech_t  numeric_ril_tech = QCRIL_DATA_RIL_RADIO_TECH_UNKNOWN;

#if (QCRIL_RIL_VERSION >= 6)
  long int ret;
  char *end = NULL;

  if (!ril_tech)
  {
    QCRIL_LOG_ERROR( "%s", "bad parameter" );
  }
  else
  {
    ret = strtol(ril_tech, &end, 10);

    /* If we had a successful conversion, update the return value */
    if (end && *end == '\0')
    {
      numeric_ril_tech = (qcril_data_ril_radio_tech_t) ret;
    }
  }
#else
  QCRIL_LOG_DEBUG("%s", "This function is not supported");
#endif

  QCRIL_LOG_DEBUG("ril_tech=%s, numeric_ril_tech=%d",
                  (ril_tech) ? ril_tech : "",
                  numeric_ril_tech);

  return numeric_ril_tech;
}


/*===========================================================================
  FUNCTION:  qcril_data_validate_call_technology

===========================================================================*/
/*!
  @brief
  This function validates that the Android Telephony requested technology and
  the technology on which the call was brought up by the modem match.

  @pre Before calling, info_tbl_mutex must not be locked by the calling thread

  @return
  SUCCESS - if the validation was successful
  FAILURE - otherwise
*/
/*=========================================================================*/
static int
qcril_data_validate_call_technology
(
  qcril_data_call_info_tbl_type *info_tbl_ptr
)
{
  int ret = SUCCESS;
  const char *str = NULL;

#if (QCRIL_RIL_VERSION >= 6)
  dsi_data_bearer_tech_t  bearer_tech;
  qcril_data_ril_radio_tech_t  numeric_ril_tech;

  numeric_ril_tech = qcril_data_get_numeric_ril_technology(info_tbl_ptr->call_params->ril_tech);

  str = qcril_data_util_get_ril_tech_string(numeric_ril_tech);
  QCRIL_LOG_DEBUG("RIL requested technology=%s, numeric=%d",
                   str, numeric_ril_tech);

    memset(&bearer_tech, 0, sizeof(bearer_tech));

    bearer_tech = dsi_get_current_data_bearer_tech(info_tbl_ptr->dsi_hndl);

    QCRIL_LOG_DEBUG("Modem reported data bearer technology=%s, numeric=%d",
                    qcril_data_util_get_dsi_bearer_tech_string(bearer_tech),
                    bearer_tech);

  /* If RIL provided technology is 1x or EVDO, determine the modem technology
     for the brought up call */
  if (QCRIL_DATA_IS_RIL_RADIO_TECH_CDMA_1X_EVDO(numeric_ril_tech))
  {
    /* If the call came up on anything other than 1x/EVDO on the modem side
       return failure (FMC is an exception) */
    if (DSI_DATA_BEARER_TECH_FMC == bearer_tech)
    {
      QCRIL_LOG_DEBUG("%s", "FMC bearer tech detected, returning SUCCESS");
    }
    else if (!QCRIL_DATA_IS_DSI_DATA_BEARER_TECH_CDMA_1X_EVDO(bearer_tech))
    {
      QCRIL_LOG_DEBUG("%s", "Tech mismatch detected between RIL and modem reported techs");
      ret = FAILURE;
    }
    else
    {
      QCRIL_LOG_DEBUG("%s", "Tech matched between RIL and modem reported techs");
    }
  }
  else if(QCRIL_DATA_IS_RIL_RADIO_TECH_3GPP_EHRPD(numeric_ril_tech))
  {
    if(!QCRIL_DATA_IS_DSI_DATA_BEARER_TECH_3GPP_EHRPD(bearer_tech))
    {
      QCRIL_LOG_DEBUG("%s", "Tech mismatch detected between RIL and modem reported techs");
      ret = FAILURE;
    }
    else
    {
      QCRIL_LOG_DEBUG("%s", "Tech matched between RIL and modem reported techs");
    }
  }
  else
  {
    QCRIL_LOG_DEBUG("%s", "RIL requested tech is not CDMA or UMTS, returning SUCCESS");
  }
#else
  QCRIL_LOG_DEBUG("%s", "This function is not supported, returning SUCCESS");
#endif

  return ret;
}

/*===========================================================================
  FUNCTION:  qcril_data_reg_sys_ind

===========================================================================*/
/*!
  @brief
  Helper routine to register for system indications. We can either register
  for full set of indications or limited set of indications using this.

  @return QCRIL_DS_SUCCESS - if registration was successful.
  @return QCRIL_DS_ERROR - if registration failed for some reason.
*/
/*=========================================================================*/
static int qcril_data_reg_sys_ind
(
  qcril_data_limited_sys_ind_switch_type       sys_ind_switch
)
{
  int rc;

  dsd_system_status_change_req_msg_v01  sys_reg_req_msg;
  dsd_system_status_change_resp_msg_v01 sys_reg_resp_msg;
  dsd_get_system_status_resp_msg_v01  sys_resp_msg;

  dsd_system_status_ind_msg_v01 *ind_data = NULL;

  QCRIL_LOG_INFO( "qcril_data_reg_sys_ind: ENTRY, switch %d", sys_ind_switch);

  memset(&sys_reg_req_msg, 0, sizeof(sys_reg_req_msg));
  memset(&sys_reg_resp_msg, 0, sizeof(sys_reg_resp_msg));

  sys_reg_req_msg.limit_so_mask_change_ind_valid = TRUE;

  /* Register to not report SO mask indications when
     Limited sys indications are ON   */
  if (LIMITED_SYS_INDICATIONS_ON == sys_ind_switch)
  {
     sys_reg_req_msg.limit_so_mask_change_ind = TRUE;
  }

  rc = qmi_client_send_msg_sync(global_qmi_dsd_hndl,
                                QMI_DSD_SYSTEM_STATUS_CHANGE_REQ_V01,
                                &sys_reg_req_msg,
                                sizeof(sys_reg_req_msg),
                                &sys_reg_resp_msg,
                                sizeof(sys_reg_resp_msg),
                                QCRIL_DATA_QMI_DSD_SYNC_MSG_TIMEOUT);

  if (QMI_NO_ERR != rc)
  {
    QCRIL_LOG_ERROR("failed to send qmi_dsd_reg_system_status_ind, err=%d",rc);
    goto err_label;
  }
  else if (QMI_NO_ERR != sys_reg_resp_msg.resp.result)
  {
    QCRIL_LOG_ERROR("failed to send qmi_dsd_reg_system_status_ind, err=%d",
                      sys_reg_resp_msg.resp.error);
    goto err_label;
  }

  /* Query the current sys_status to propagate the change
     when Limited sys indications are off */

  if (LIMITED_SYS_INDICATIONS_OFF == sys_ind_switch)
  {
    memset(&sys_resp_msg, 0, sizeof(sys_resp_msg));

    rc = qmi_client_send_msg_sync(global_qmi_dsd_hndl,
                                  QMI_DSD_GET_SYSTEM_STATUS_REQ_V01,
                                  NULL,
                                  0,
                                  &sys_resp_msg,
                                  sizeof(sys_resp_msg),
                                  QCRIL_DATA_QMI_DSD_SYNC_MSG_TIMEOUT);

    if (QMI_NO_ERR != rc)
    {
      QCRIL_LOG_ERROR("failed to send qmi_dsd_get_system_status, err=%d", rc);
    }
    else if (QMI_NO_ERR != sys_resp_msg.resp.result)
    {
      QCRIL_LOG_ERROR("failed to send qmi_dsd_get_system_status, err=%d",
                      sys_resp_msg.resp.error);
    }

    /* Process the system status response */
    ind_data = (dsd_system_status_ind_msg_v01 *)((char *)&sys_resp_msg +
                               offsetof(dsd_get_system_status_resp_msg_v01,
                                                         avail_sys_valid));

    qcril_data_process_qmi_dsd_ind(ind_data);

  }


  QCRIL_LOG_INFO( "%s","qcril_data_reg_sys_ind: EXIT with SUCCESS");
  return QCRIL_DS_SUCCESS;

err_label:
  QCRIL_LOG_ERROR( "%s","qcril_data_reg_sys_ind: EXIT with ERROR");
  return QCRIL_DS_ERROR;
}


/*===========================================================================
  FUNCTION:  qcril_data_reg_sys_ind

===========================================================================*/
/*!
  @brief
  Helper routine to check if all the established calls are dormant.
  Checks from the qcril_data local cache and does not end up
  sending query to modem.

  @return 1 - if all established calls are dormant.
  @return 0 - if any of the established calls is not dormant.
*/
/*=========================================================================*/
static int qcril_data_all_calls_dormant
(
  void
)
{
  int i;
  int dev_instance;
  qmi_client_type qmi_wds_hndl;
  int calls_dormant = 1;

  /* Get the current dormancy status for all calls. */
  for (i = 0; i < MAX_CONCURRENT_UMTS_DATA_CALLS; i++)
  {
    GET_DEV_INSTANCE_FROM_NAME(i, dev_instance);
    if (dev_instance < 0 ||
        dev_instance >= QCRIL_DATA_MAX_DEVS ||
        !VALIDATE_LOCAL_DATA_OBJ(&info_tbl[i]) ||
        info_tbl[i].cid == CALL_ID_INVALID)
    {
      continue;
    }

    qmi_wds_hndl = info_tbl[i].qmi_wds_hndl;
    if(QCRIL_DATA_HNDL_INVALID == qmi_wds_hndl)
    {
      continue;
    }

    QCRIL_LOG_INFO ("Dormancy status for inst: %d, wds_hndl %p, status: %d",
                    i, qmi_wds_hndl, info_tbl[i].dorm_status);

    if (info_tbl[i].dorm_status == WDS_DORMANCY_STATUS_ACTIVE_V01)
    {
      /* If any call is active, return FALSE */
      calls_dormant = 0;
    }

  } /* for */

  return calls_dormant;

} /* qcril_data_all_calls_dormant() */


/*===========================================================================
  FUNCTION:  qcril_data_event_hdlr

===========================================================================*/
/*!
    @brief
    Registered with QCRIL to be called by QCRIL on event
    QCRIL_EVT_DATA_EVENT_CALLBACK

    @pre Before calling, info_tbl_mutex must not be locked by the calling thread

    @return
    None.
*/
/*=========================================================================*/
void qcril_data_event_hdlr(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr  /*output*/
)
{
  qcril_instance_id_e_type       instance_id;
  qcril_reqlist_public_type      reqlist_info;
  qcril_data_call_info_tbl_type *info_tbl_ptr;
  qcril_data_call_response_type  response;
  void                          *response_ptr = (void*)&response;
  qcril_data_event_data_t       *evt_info_ptr;
  qcril_evt_e_type               pend_req  = DS_RIL_REQ_INVALID;
  errno_enum_type                qcril_ret = E_FAILURE;
  char                           buf[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+1];
  boolean                        dorm_status_changed = FALSE;
  int                            call_state;
  int                            all_calls_dormant;
  IxErrnoType                    stop_data_err = E_FAILURE;
  RIL_Errno                      call_end_cause = RIL_E_INTERNAL_ERR;
  int                            call_end_reason = PDP_FAIL_OEM_DCFAILCAUSE_4;

#ifdef FEATURE_DATA_EMBMS
  qcril_embms_enable_response_payload_type     enable_response;
  qcril_embms_disable_indication_payload_type  disable_indication;
  qcril_modem_id_e_type                        modem_id;
  int                                          if_index;

  RIL_Errno reti = RIL_E_INTERNAL_ERR;
#endif

  QCRIL_LOG_DEBUG( "%s", "qcril_data_event_hdlr: ENTRY" );

  /* Input Validation */
  QCRIL_DS_ASSERT( params_ptr != NULL, "validate input params_ptr" );
  QCRIL_DS_ASSERT( ret_ptr    != NULL, "validate input ret_ptr" );
  if ( ( params_ptr == NULL ) || ( ret_ptr == NULL ) )
  {
    goto err_label_exit;
  }

  instance_id = params_ptr->instance_id;
  QCRIL_ASSERT( instance_id < QCRIL_MAX_INSTANCE_ID );
  if (instance_id >= QCRIL_MAX_INSTANCE_ID)
  {
    goto err_label_exit;
  }
#ifdef FEATURE_DATA_EMBMS
  modem_id     = params_ptr->modem_id;
#endif

  memset( &reqlist_info, 0, sizeof( qcril_reqlist_public_type ) );
  memset( &response,     0, sizeof( qcril_data_call_response_type ) );

  QCRIL_DATA_MUTEX_LOCK(&info_tbl_mutex);

  evt_info_ptr = ( qcril_data_event_data_t * )params_ptr->data;
  QCRIL_DS_LOG_DBG_MEM( "event obj", evt_info_ptr );
  if ( !VALIDATE_LOCAL_DATA_OBJ( evt_info_ptr ) )
  {
    QCRIL_LOG_ERROR( "%s", "bad event obj, cannot free mem, ret with err" );
    call_end_cause = RIL_E_INVALID_ARGUMENTS;
    goto err_label_no_free;
  }

  /* data call status is event not associated with any info_tbl entry */
  if ((int) QCRIL_DATA_EV_DATA_CALL_STATUS_CHANGED ==
      (int) evt_info_ptr->evt)
  {
    QCRIL_LOG_INFO("%s","QCRIL_DATA_EV_DATA_CALL_STATUS_CHANGED arrived "
                   "in the qcril_event thread");
    qcril_data_unsol_tethered_mode_state_changed( global_instance_id,
                                                  &global_tethered_state,
                                                  sizeof(RIL_tethered_mode_state));
    goto ret;
  }

  /* Pointer to info Tbl is derived from event posted to QCRIL */
  info_tbl_ptr =  ( qcril_data_call_info_tbl_type *)evt_info_ptr->data;
  QCRIL_DS_LOG_DBG_MEM( "info_tbl", info_tbl_ptr );
  if ( !VALIDATE_LOCAL_DATA_OBJ( info_tbl_ptr ) )
  {
    QCRIL_LOG_ERROR( "%s", "invalid info_tbl ref" );
    QCRIL_DS_ASSERT( evt_info_ptr != NULL, "validate event obj" );
    QCRIL_LOG_INFO( "%s", "try event obj dealloc" );
    QCRIL_DATA_RELEASE_STORAGE( evt_info_ptr );
    QCRIL_DATA_MUTEX_UNLOCK(&info_tbl_mutex);
    return;
  }

#ifdef FEATURE_DATA_EMBMS
  if (evt_info_ptr->evt == DSI_NET_TMGI_ACTIVATED ||
        evt_info_ptr->evt == DSI_NET_TMGI_DEACTIVATED ||
      evt_info_ptr->evt == DSI_NET_TMGI_ACTIVATED_DEACTIVATED ||
        evt_info_ptr->evt == DSI_NET_TMGI_LIST_CHANGED ||
      evt_info_ptr->evt == DSI_NET_CONTENT_DESC_CONTROL ||
        evt_info_ptr->evt == DSI_NET_TMGI_SERVICE_INTERESTED ||
      evt_info_ptr->evt == DSI_NET_SAI_LIST_CHANGED)
  {
    qcril_data_handle_embms_events(instance_id, info_tbl_ptr, evt_info_ptr);
    goto ret;
  }
#endif /*FEATURE_DATA_EMBMS*/

  /* Check whether REQ is pending */
#ifdef FEATURE_DATA_EMBMS
  if( qcril_data_util_is_req_pending( info_tbl_ptr, &pend_req ) &&
      (QCRIL_EVT_QMI_RIL_EMBMS_ENABLE_DATA_REQ != pend_req) &&
      (QCRIL_EVT_HOOK_EMBMS_UNSOL_ACTIVE_TMGI != pend_req))
#else
  if( qcril_data_util_is_req_pending( info_tbl_ptr, &pend_req ))
#endif
  {
    QCRIL_LOG_INFO( "RIL REQ pend [%s], cid [%d], index [%d]",
                    qcril_log_lookup_event_name( pend_req ),
                    info_tbl_ptr->cid,
                    info_tbl_ptr->index );
    QCRIL_LOG_INFO( "DEBUG:  params->t[0x%x] info_tbl_ptr->pend_tok[0x%x]",
                     params_ptr->t, info_tbl_ptr->pend_tok );
    QCRIL_DS_ASSERT( params_ptr->t != NULL, "validate pend RIL Token" );
    QCRIL_DS_ASSERT( params_ptr->t == info_tbl_ptr->pend_tok,
                     "validate pend RIL Token consistency" );

    if ( ( qcril_ret = qcril_reqlist_query( instance_id, params_ptr->t,
                                            &reqlist_info ) ) != E_SUCCESS )
    {
      QCRIL_LOG_ERROR("unable to find reqlist entry. token with params->t[0x%x]",
                      params_ptr->t);
      call_end_cause = RIL_E_INVALID_CALL_ID;
      goto err_label;
    }

    QCRIL_LOG_DEBUG( "Req list entry found for RIL Token [%d]",
                     qcril_log_get_token_id( params_ptr->t ) );

    /* Validate xtracted local info tbl pointer for a pending RIL Token from reqlist Node */
    if ( qcril_ret == E_SUCCESS )
    {
      QCRIL_DS_ASSERT( ( info_tbl_ptr ==
                     ( qcril_data_call_info_tbl_type * )reqlist_info.sub.ds.info ),
                 "validate info_tbl ref" );
    }
    QCRIL_DS_LOG_DBG_MEM( "info_tbl", info_tbl_ptr );

  }
  else /* RIL REQ not pending */
  {
    QCRIL_LOG_INFO( "%s", "RIL REQ NOT pend" );
    QCRIL_DS_ASSERT( params_ptr->t == NULL, "validate null RIL Token" );

    if (NULL != params_ptr->t)
    {
      QCRIL_LOG_INFO( "params_ptr->t=0x%x", params_ptr->t);
    }
  }

  /* Dispatch the specific call event */
  switch( evt_info_ptr->evt )
  {
  case DSI_EVT_NET_IS_CONN:
    QCRIL_LOG_INFO( ">>>DSI_EVT_NET_IS_CONN: START>>> cid [%d], index [%d]",
                    info_tbl_ptr->cid, info_tbl_ptr->index );
    /*
      Check pending RIL SETUP REQ
      NET_IS_CONN event is not expected if a SETUP REQ is not pending
    */
    if ( pend_req == QCRIL_EVT_RIL_REQUEST_SETUP_DATA_CALL )
    {
      memset(buf, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+1);
      /* Now that call is up now, find out what port the call
       * came up on. Also init and store the related wds client
       * handle locally in the info_tbl entry */
      QCRIL_LOG_DEBUG("Get device name and fill the value at [%p]",
                      buf);

      /* We got the data call setup response, so stop the response timer */
      qcril_data_util_stop_timer(&info_tbl_ptr->timer_id);

      /* Validate the technology on which the call was brought up. This is
         necessary to support 1x/EVDO -> eHRPD -> LTE IRAT scenario.
         For example, when a 1x/EVDO call is requested by Android Telephony and
         the technology on the modem side changes to eHRPD in the interim,
         the call would still be brought up (on eHRPD) because a default profile
         was used for the 1x/EVDO call. This causes problems during IRAT to LTE
         because the profile parameters weren't completely specified initially.
         So, the call gets torn down. To avoid this situation, tear down the call
         when a 1x/EVDO call is specified by Android Telephony and the modem
         brings up the call on a technology other than 1x/EVDO. */
      if (SUCCESS != qcril_data_validate_call_technology(info_tbl_ptr))
      {
        /* Stop the data call */
        QCRIL_LOG_DEBUG ("%s", "unexpected technology... stopping data call");
        stop_data_err = qcril_data_stop_data_call(info_tbl_ptr);
        if( E_SUCCESS != stop_data_err )
        {
          if(E_INVALID_ARG == stop_data_err)
          {
            qcril_data_response_generic_failure( info_tbl_ptr->instance_id,
                                                 info_tbl_ptr->pend_tok,
                                                 QCRIL_EVT_RIL_REQUEST_SETUP_DATA_CALL,
                                                 RIL_E_INVALID_ARGUMENTS);
          }
          else
          {
            qcril_data_response_generic_failure( info_tbl_ptr->instance_id,
                                                 info_tbl_ptr->pend_tok,
                                                 QCRIL_EVT_RIL_REQUEST_SETUP_DATA_CALL,
                                                 RIL_E_OEM_ERROR_3);
          }

          /* Cleanup call state */
          qcril_data_cleanup_call_state( info_tbl_ptr );
        }

        /* Response to client will be generated in event handler */
        goto ret;
      }

      if (DSI_SUCCESS != dsi_get_qmi_port_name(info_tbl_ptr->dsi_hndl,
                                               buf,
                                               DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+1))
      {
        QCRIL_LOG_ERROR("couldn't get device name for info_tbl [%d]",
                        info_tbl_ptr->index);
        QCRIL_DS_ASSERT( 0, "programming error: dsi_get_qmi_port_name" );
        qcril_data_cleanup_call_state( info_tbl_ptr );
        call_end_cause = RIL_E_OEM_ERROR_3;
        goto err_label;
      }
      else
      {
        buf[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN] = '\0';
        QCRIL_LOG_DEBUG("device [%s] is associated with info tbl indx [%d]",
                        buf, info_tbl_ptr->index);
      }

      /* Create WDS clients based on the IP version of the call */
      if (E_SUCCESS != qcril_data_qmi_wds_init_qmi_client_ex(buf,
                  info_tbl_ptr,
                  global_subs_id,
                  info_tbl_ptr->dsi_ip_version,
                  &info_tbl_ptr->qmi_wds_hndl_v4,
                  &info_tbl_ptr->qmi_wds_hndl_v6))
      {
        QCRIL_LOG_ERROR("Couldn't init wds srvc client for info_tbl [%d]",
                info_tbl_ptr->index);
        qcril_data_cleanup_call_state( info_tbl_ptr );
        call_end_cause = RIL_E_OEM_ERROR_1;
        goto err_label;
      }

      /* Set the legacy qmi_wds_hndl to the first available IP family */
      int ipv = info_tbl_ptr->dsi_ip_version;
      if (DSI_IP_VERSION_4_6 == ipv)
      {
        ipv = qcril_data_get_dsi_ip_version(info_tbl_ptr->dsi_hndl);
      }

      if (DSI_IP_VERSION_4 == ipv)
      {
        info_tbl_ptr->qmi_wds_hndl = info_tbl_ptr->qmi_wds_hndl_v4;
      }
      else
      {
        info_tbl_ptr->qmi_wds_hndl = info_tbl_ptr->qmi_wds_hndl_v6;
      }

      if (qcril_data_mtu > 0)
      {
        QCRIL_LOG_DEBUG("Changing MTU value of new call to [%d] as specified by RIL",
                        qcril_data_mtu);
        qcril_data_iface_set_mtu(info_tbl->dsi_hndl, qcril_data_mtu);
      }
      /* Get and fill all call attributes */
      if( TRUE != qcril_data_util_fill_call_params( info_tbl_ptr ) )
      {
        QCRIL_LOG_ERROR("Error filling call params for info_tbl [%d]",
                        info_tbl_ptr->index);
        qcril_data_get_call_end_reason( info_tbl_ptr, &call_end_reason );
        info_tbl_ptr->call_info.inactiveReason = call_end_reason;
        last_call_end_reason = call_end_reason;

        QCRIL_LOG_INFO( "%s", "Sending SETUP_DATA_CALL failure reason" );
        /* Failure response, indicated in RIL_REQUEST_SETUP_DATA_CALL.status */
        qcril_data_response_setup_data_call_failure( info_tbl_ptr,
                                                     instance_id,
                                                     params_ptr->t,
                                                     last_call_end_reason );
        qcril_data_cleanup_call_state( info_tbl_ptr );

        /* Notify internal QCRIL clients of call state */
        qcril_data_client_notify( QCRIL_DATA_EVT_CALL_RELEASED);
        goto ret;
      }
      /* Update state of local tbl */
      info_tbl_ptr->pend_tok = NULL;
      info_tbl_ptr->pend_req = DS_RIL_REQ_INVALID;  /* is there a RIL_REQUEST_INVALID */

      QCRIL_DATA_MUTEX_LOCK(&dsi_cb_tbl_mutex);
      dsi_cb_tbl[info_tbl_ptr->index].pend_tok = NULL;
      QCRIL_DATA_MUTEX_UNLOCK(&dsi_cb_tbl_mutex);

      /* Update call_tbl to point to latest call info */
      qcril_data_util_update_call_state( info_tbl_ptr, CALL_ACTIVE_PHYSLINK_UP, PDP_FAIL_NONE );

      /* Create RIL REQ SETUP response */
      qcril_data_util_create_setup_rsp( info_tbl_ptr, &response );

      /* Post RIL Response */
      QCRIL_LOG_DEBUG( ">>>RIL RSP READY>>> cid [%d], ifname [%s], ip_addr [%s]",
                       response.setup_rsp.cid, response.setup_rsp.ifname, response.setup_rsp.addresses );

      /* Notify internal QCRIL clients of call connect */
      qcril_data_client_notify(QCRIL_DATA_EVT_CALL_CONNECTED);

      qcril_data_response_success(instance_id,
                                  params_ptr->t,
                                  QCRIL_EVT_RIL_REQUEST_SETUP_DATA_CALL,
                                  (void *) ( &(response.setup_rsp)),
                                  sizeof( response.setup_rsp));
      QCRIL_LOG_DEBUG("%s", "<<<RIL RSP SENT<<<");

      QCRIL_LOG_INFO("%s", "<<<DSI_EVT_NET_IS_CONN: DONE<<<");

      qcril_data_util_register_and_query_ext_ip_config_ind(info_tbl_ptr);

      /* we only request dormancy indication report on/off when
       there is previous request to do so. Previous request is
       assciated with screen state from Telephony.
       We don't want to request it to be ON if screen is OFF;
       We need to request it to be OFF per previous request */
      if(DORMANCY_INDICATIONS_ON == global_dorm_ind)
      {
        /* register for physlink up/down events */
        /*Register for Physlink UP DOWN Indication Events */
        QCRIL_LOG_DEBUG("%s", "Registering for Physlink Events");
        if( FAILURE == qcril_data_iface_ioctl(info_tbl_ptr,
                       QCRIL_DATA_IFACE_IOCTL_DORMANCY_INDICATIONS_ON,
                       &dorm_status_changed,
                       &call_state))
        {
          QCRIL_LOG_ERROR("%s", "Error registering for Physlink Events");
        }
      }
      else if(DORMANCY_INDICATIONS_OFF == global_dorm_ind)
      {
        QCRIL_LOG_DEBUG("%s", "De-Registering for Physlink Events");
        if(FAILURE == qcril_data_iface_ioctl(info_tbl_ptr,
                      QCRIL_DATA_IFACE_IOCTL_DORMANCY_INDICATIONS_OFF,
                      &dorm_status_changed,
                      &call_state))
        {
          QCRIL_LOG_ERROR("%s", "Error De-registering for Physlink Events");
        }
      }
      if(dorm_status_changed)
      {
        QCRIL_LOG_INFO("<<<[%d] processing started<<<", call_state);
        qcril_data_util_update_call_state(info_tbl_ptr, call_state, PDP_FAIL_NONE);
      }

      /* If we received fewer IP addresses for a Dual-IP call, initiate partial retry */
      if (DSI_IP_VERSION_4_6 == info_tbl_ptr->dsi_ip_version &&
          info_tbl_ptr->last_addr_count < DSI_NUM_IP_FAMILIES)
      {
        qcril_data_util_schedule_partial_retry_attempt(info_tbl_ptr, TRUE, TRUE);
      }
    }
#ifdef FEATURE_DATA_EMBMS
    else if ( pend_req == QCRIL_EVT_QMI_RIL_EMBMS_ENABLE_DATA_REQ )
    {
      do
      {
        /* Update state of local tbl */
        info_tbl_ptr->pend_tok   = NULL;
        info_tbl_ptr->pend_req   = DS_RIL_REQ_INVALID;

        memset ( &enable_response, 0, sizeof( enable_response ) );
        enable_response.call_id = info_tbl_ptr->cid;

        memset(buf, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+1);
        QCRIL_LOG_DEBUG("Get device name and fill the value at [%p]", buf);

        if (DSI_SUCCESS != dsi_get_device_name(info_tbl_ptr->dsi_hndl,
                                               buf,
                                               DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+1))
        {
          QCRIL_LOG_ERROR("couldn't get device name for info_tbl [%d]",
                          info_tbl_ptr->index);
          QCRIL_DS_ASSERT( 0, "programming error: dsi_get_device_name" );
          qcril_data_cleanup_call_state( info_tbl_ptr );
          enable_response.cause = RIL_E_OEM_ERROR_4;
          enable_response.resp_code = QCRIL_DATA_EMBMS_ERROR_UNKNOWN;

          break;
        }
        else
        {
          buf[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN] = '\0';
          QCRIL_LOG_DEBUG("device [%s] is associated with info tbl indx [%d]",
                          buf, info_tbl_ptr->index);
          if(SUCCESS != qcril_data_get_ifindex(buf, &if_index))
          {
            QCRIL_LOG_ERROR("couldn't get if_index for info_tbl [%d]",
                            info_tbl_ptr->index);
            QCRIL_DS_ASSERT(0, "programming error: qcril_data_get_ifindex");
            qcril_data_cleanup_call_state( info_tbl_ptr);
            enable_response.cause = RIL_E_OEM_ERROR_4;
            enable_response.resp_code = QCRIL_DATA_EMBMS_ERROR_UNKNOWN;

            break;
          }
          else
          {
            enable_response.cause = RIL_E_SUCCESS;
            enable_response.resp_code = QCRIL_DATA_EMBMS_ERROR_NONE;
            strlcpy(&enable_response.interafce_name[0],
                    &buf[0],
                    QCRIL_EMBMS_INTERFACE_NAME_LENGTH_MAX);
            enable_response.if_index = if_index;

            /* Update dev_name in info_tbl_ptr */
            memset( info_tbl_ptr->call_info.dev_name, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+1 );
            memcpy( info_tbl_ptr->call_info.dev_name, buf, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN );
          }
        }
        reti = RIL_E_SUCCESS;
      }while(0);

      qcril_data_embms_enable_data_con(&enable_response);

      if(RIL_E_SUCCESS != reti)
      {
        call_end_cause = RIL_E_OEM_ERROR_2;
        goto err_label;
      }
      QCRIL_LOG_DEBUG( "%s", "<<<RIL RSP SENT<<<" );

      QCRIL_LOG_INFO( "%s", "<<<DSI_EVT_NET_IS_CONN: DONE<<<" );
    }/* else if ( pend_req == QCRIL_EVT_QMI_RIL_EMBMS_ENABLE_DATA_REQ ) */
#endif
    else
    {
      QCRIL_LOG_DEBUG( "RIL token [%d], pend req [%d]",
                       qcril_log_get_token_id( info_tbl_ptr->pend_tok ), info_tbl_ptr->pend_req );
      goto err_label_exit;
    }
    break; /* DSI_EVT_NET_IS_CONN */

  case DSI_EVT_NET_PARTIAL_CONN:
#if (QCRIL_RIL_VERSION >= 6)

    QCRIL_LOG_INFO( ">>>DSI_EVT_NET_PARTIAL_CONN: START>>> cid [%d], index [%d]",
                    info_tbl_ptr->cid, info_tbl_ptr->index );

    /* We got the partial retry response, so stop the response timer */
    qcril_data_util_stop_timer(&info_tbl_ptr->timer_id);

    /* A DSI_EVT_NET_PARTIAL_NET event is received for a Dual-IP partial retry
       failure. */
    info_tbl_ptr->is_partial_retry = FALSE;

    QCRIL_LOG_DEBUG( "%s", "Dual-IP Partial Retry: Failure" );

    qcril_data_util_schedule_partial_retry_attempt(info_tbl_ptr, TRUE, TRUE);

    QCRIL_LOG_INFO( "%s", "<<<DSI_EVT_NET_PARTIAL_CONN: processing complete<<<" );
    goto ret; /* DSI_EVT_NET_PARTIAL_CONN */

#endif /* (QCRIL_RIL_VERSION >= 6) */

  case DSI_EVT_NET_NO_NET:
    QCRIL_LOG_INFO( ">>>DSI_EVT_NET_NO_NET: START>>> cid [%d], index [%d]",
                    info_tbl_ptr->cid, info_tbl_ptr->index );

#ifdef FEATURE_DATA_EMBMS
    if (pend_req == QCRIL_EVT_QMI_RIL_EMBMS_ENABLE_DATA_REQ)
    {
      /* Update state of local tbl */
      info_tbl_ptr->pend_tok   = NULL;
      info_tbl_ptr->pend_req   = DS_RIL_REQ_INVALID;

      memset ( &enable_response, 0, sizeof( enable_response ) );
      enable_response.call_id  = info_tbl_ptr->cid;
      enable_response.cause    = RIL_E_INTERNAL_ERR;
      enable_response.resp_code = QCRIL_DATA_EMBMS_ERROR_UNKNOWN;

      qcril_data_embms_enable_data_con(&enable_response);
      goto ret;
    }
    else
#endif
    if (pend_req == QCRIL_EVT_RIL_REQUEST_SETUP_DATA_CALL ||
        pend_req == QCRIL_EVT_RIL_REQUEST_DEACTIVATE_DATA_CALL)
    {
      /* Do nothing */
    }
    else
    {
      QCRIL_DS_ASSERT( info_tbl_ptr->pend_req == DS_RIL_REQ_INVALID,
                       "validate pend_req" );
      QCRIL_DS_ASSERT( info_tbl_ptr->pend_tok == NULL, "validate pend_tok" );
    }

    qcril_data_get_call_end_reason( info_tbl_ptr, &call_end_reason );
    info_tbl_ptr->call_info.inactiveReason = call_end_reason;
    last_call_end_reason = call_end_reason;

    /* Post RIL RSP  */
    if ( pend_req == QCRIL_EVT_RIL_REQUEST_SETUP_DATA_CALL )
    {
      /* Failure response, indicated in RIL_REQUEST_SETUP_DATA_CALL.status */
      qcril_data_response_setup_data_call_failure( info_tbl_ptr,
                                                   instance_id,
                                                   params_ptr->t,
                                                   last_call_end_reason );
      qcril_data_cleanup_call_state( info_tbl_ptr );
    }
    else if ( pend_req == QCRIL_EVT_RIL_REQUEST_DEACTIVATE_DATA_CALL )
    {
      /* Successful response */
      qcril_data_response_success( instance_id,
                                   params_ptr->t,
                                   pend_req,
                                   NULL,
                                   0 );
      /* NO_NET event from LL, send ind to RIL & cleanup call state */
      qcril_data_util_update_call_state( info_tbl_ptr, CALL_INACTIVE, PDP_FAIL_NONE );
      qcril_data_unsol_call_list_changed( instance_id );
      qcril_data_cleanup_call_state( info_tbl_ptr );
    }
    else
    {
#ifdef FEATURE_DATA_EMBMS
      dsi_call_tech_type call_tech;

      if ((dsi_get_call_tech(info_tbl_ptr->dsi_hndl,
                             &call_tech) == DSI_SUCCESS) &&
            (call_tech == DSI_EXT_TECH_EMBMS))
      {
        /*NO_NET indication received for EMBMS call, relay event to NAS*/
        memset(&disable_indication, 0, sizeof(disable_indication));

        /*Populate indication response and send event to NAS*/
        disable_indication.call_id = info_tbl_ptr->cid;
        disable_indication.cause = last_call_end_reason;
        disable_indication.resp_code = QCRIL_DATA_EMBMS_ERROR_NONE;

        qcril_data_embms_disable_data_ind(&disable_indication);
      }
      else
      {
        /* NO_NET event from LL, send ind to RIL & cleanup call state */
        qcril_data_util_update_call_state( info_tbl_ptr, CALL_INACTIVE, last_call_end_reason );
        qcril_data_unsol_call_list_changed( instance_id );
      }
#else
      /* NO_NET event from LL, send ind to RIL & cleanup call state */
      qcril_data_util_update_call_state( info_tbl_ptr, CALL_INACTIVE, last_call_end_reason );
      qcril_data_unsol_call_list_changed( instance_id );
#endif
      qcril_data_cleanup_call_state( info_tbl_ptr );
    }

    /* Notify internal QCRIL clients of call connect */
    qcril_data_client_notify( QCRIL_DATA_EVT_CALL_RELEASED);

    QCRIL_LOG_INFO( "%s", "<<<DSI_EVT_NET_NO_NET: processing complete<<<" );
    goto ret; /*DSI_EVT_NET_NO_NET*/

  case DSI_EVT_PHYSLINK_DOWN_STATE:
    QCRIL_LOG_INFO( "%s", "<<<DSI_EVT_PHYSLINK_DOWN_STATE: processing Started<<<" );

    /* Update call_tbl to point to latest call info */
    qcril_data_util_update_call_state( info_tbl_ptr, CALL_ACTIVE_PHYSLINK_DOWN, PDP_FAIL_NONE );

    /* Notify internal QCRIL clients of call state change */
    qcril_data_client_notify(QCRIL_DATA_EVT_CALL_PHYSLINK_DOWN);

    all_calls_dormant = qcril_data_all_calls_dormant();
    if (DORMANCY_INDICATIONS_OFF == global_dorm_ind && all_calls_dormant)
    {
      QCRIL_LOG_DEBUG("%s", "De-Registering for Physlink Events");
      if(FAILURE == qcril_data_iface_ioctl(info_tbl_ptr,
                    QCRIL_DATA_IFACE_IOCTL_DORMANCY_INDICATIONS_OFF,
                    &dorm_status_changed,
                    &call_state))
      {
        QCRIL_LOG_ERROR("%s", "Error De-registering for Physlink Events");
      }
    }

    goto unsol_rsp;

  case DSI_EVT_PHYSLINK_UP_STATE:
    QCRIL_LOG_INFO( "%s",  "<<<DSI_EVT_PHYSLINK_UP_STATE: processing Started<<<" );

    /* Update call_tbl to point to latest call info */
    qcril_data_util_update_call_state( info_tbl_ptr, CALL_ACTIVE_PHYSLINK_UP, PDP_FAIL_NONE );

    /* Notify internal QCRIL clients of call state change */
    qcril_data_client_notify(QCRIL_DATA_EVT_CALL_PHYSLINK_UP);

    goto unsol_rsp;

  case DSI_EVT_NET_NEWADDR:
  case DSI_EVT_NET_RECONFIGURED:
    if (DSI_EVT_NET_NEWADDR == evt_info_ptr->evt)
    {
      QCRIL_LOG_INFO( ">>>DSI_EVT_NET_NEWADDR: START>>> cid [%d], index [%d]",
                      info_tbl_ptr->cid, info_tbl_ptr->index );
    }
    else if (DSI_EVT_NET_RECONFIGURED == evt_info_ptr->evt)
    {
      QCRIL_LOG_INFO( ">>>DSI_EVT_NET_RECONFIGURED: START>>> cid [%d], index [%d]",
                      info_tbl_ptr->cid, info_tbl_ptr->index );
    }

#if (QCRIL_RIL_VERSION >= 6)
    /* When we receive a DSI_EVT_NET_NEWADDR event during a Dual-IP partial
       retry, send a setup data call success to the upper layer */
    if (TRUE == info_tbl_ptr->is_partial_retry)
    {
      info_tbl_ptr->is_partial_retry = FALSE;

      if (0 == info_tbl_ptr->last_addr_count)
      {
        QCRIL_LOG_DEBUG( "Dual-IP Partial Retry: last_addr_count=%d, "
                         "propagating event", info_tbl_ptr->last_addr_count );

        info_tbl_ptr->partial_retry_count = 0;

        /* Schedule a partial retry attempt if possible */
        qcril_data_util_schedule_partial_retry_attempt(info_tbl_ptr, TRUE, TRUE);
      }
      else if (dsi_get_ip_addr_count(info_tbl_ptr->dsi_hndl) < DSI_NUM_IP_FAMILIES)
      {
        QCRIL_LOG_ERROR( "%s", "Dual-IP Partial Retry: Both address families not up, "
                         "ignoring spurious event" );
        goto ret;
      }

      QCRIL_LOG_DEBUG( "%s", "Dual-IP Partial Retry: Success" );

      /* We got the data call setup response, so stop the response timer */
      qcril_data_util_stop_timer(&info_tbl_ptr->timer_id);

      info_tbl_ptr->partial_retry_count = 0;

      /* Stop the partial retry timer, just in case it is still running */
      qcril_data_util_disarm_timer(&info_tbl_ptr->retry_timer_id);
    }
    /* We got a new address assigned for a Dual-IP call, stop the retry timer */
    else if (DSI_IP_VERSION_4_6 == info_tbl_ptr->dsi_ip_version &&
             info_tbl_ptr->last_addr_count < DSI_NUM_IP_FAMILIES)
    {
      /* For RECONIFGURED event, check for two addresses for stopping the timers */
      if((DSI_EVT_NET_RECONFIGURED != evt_info_ptr->evt) ||
         (dsi_get_ip_addr_count(info_tbl_ptr->dsi_hndl) == DSI_NUM_IP_FAMILIES))
      {
      /* Stop the setup timer, just in case it is still running */
        qcril_data_util_stop_timer(&info_tbl_ptr->timer_id);

      /* Stop the partial retry timer, just in case it is still running */
        qcril_data_util_disarm_timer(&info_tbl_ptr->retry_timer_id);
      }
      info_tbl_ptr->partial_retry_count = 0;
    }
#endif /* (QCRIL_RIL_VERSION >= 6) */
    /* Intentional fallthrough */

  case DSI_EVT_NET_DELADDR:
    if (DSI_EVT_NET_DELADDR == evt_info_ptr->evt)
    {
      QCRIL_LOG_INFO( ">>>DSI_EVT_NET_DELADDR: START>>> cid [%d], index [%d]",
                      info_tbl_ptr->cid, info_tbl_ptr->index );
    }
    /* Get and fill all call attributes */
    if( TRUE != qcril_data_util_fill_call_params( info_tbl_ptr ) )
    {
        QCRIL_LOG_ERROR("Error filling call params for info_tbl [%d]",
                        info_tbl_ptr->index);
    }
    /* Update call_tbl to point to latest call info */
    qcril_data_util_update_call_state( info_tbl_ptr,
                                       CALL_ACTIVE_PHYSLINK_UP,
                                       PDP_FAIL_NONE );

    if (DSI_EVT_NET_RECONFIGURED == evt_info_ptr->evt)
    {
      QCRIL_LOG_INFO( "%s", "<<<DSI_EVT_NET_RECONFIGURED: DONE<<<" );
    }
    else if (DSI_EVT_NET_NEWADDR == evt_info_ptr->evt)
    {
      QCRIL_LOG_INFO( "%s", "<<<DSI_EVT_NET_NEWADDR: DONE<<<" );
    }
    else if (DSI_EVT_NET_DELADDR == evt_info_ptr->evt)
    {
      /* Schedule a partial retry attempt if possible */
      qcril_data_util_schedule_partial_retry_attempt(info_tbl_ptr, TRUE, TRUE);

      QCRIL_LOG_INFO( "%s", "<<<DSI_EVT_NET_DELADDR: DONE<<<" );
    }

    goto unsol_rsp;

  case DSI_EVT_NET_NEWMTU:
    QCRIL_LOG_INFO( ">>>DSI_EVT_NET_NETMTU: START>>> cid [%d], index [%d]",
                    info_tbl_ptr->cid, info_tbl_ptr->index );

#if (QCRIL_RIL_VERSION >= 11)

    qcril_data_util_update_mtu(info_tbl_ptr);

#endif

    QCRIL_LOG_INFO( "%s", "<<<DSI_EVT_NET_NEWMTU: DONE<<<" );
    goto unsol_rsp;

  case DSI_EVT_QOS_STATUS_IND:

    QCRIL_LOG_INFO( ">>>DSI_EVT_QOS_STATUS_IND: START>>> cid [%d], index [%d]",
                    info_tbl_ptr->cid, info_tbl_ptr->index );

#if (RIL_QCOM_VERSION >= 2)
    /* Process QOS status indication to generate response */
    if( SUCCESS != qcril_data_qos_status_ind_handler( evt_info_ptr,
                                                      (void**)&response_ptr,
                                                      &response_len ) )
    {
      QCRIL_LOG_ERROR( "%s", "error on qcril_data_qos_status_ind_handler" );
      call_end_cause = RIL_E_OEM_ERROR_3;
      goto err_label;
    }

    QCRIL_LOG_DEBUG( "%s", ">>>RIL UNSOL RSP READY>>>" );
    qcril_data_unsol_qos_state_changed( instance_id,
                                        response_ptr,
                                        response_len );
    QCRIL_LOG_DEBUG( "%s", "<<<RIL UNSOL RSP SENT<<<" );
#else
    QCRIL_NOTUSED(response_ptr);
#endif

    /* TODO: extend this when list of flows supported */
    QCRIL_DATA_RELEASE_STORAGE( response.qos_resp.dummy.string1 );
    QCRIL_DATA_RELEASE_STORAGE( response.qos_resp.dummy.string2 );
    QCRIL_DATA_RELEASE_STORAGE( response.qos_resp.dummy.string3 );
    QCRIL_DATA_RELEASE_STORAGE( response.qos_resp.dummy.string4 );

    QCRIL_LOG_INFO( "%s", "<<<DSI_EVT_QOS_STATUS_IND: DONE<<<" );
    goto ret;

  case DSI_EVT_WDS_CONNECTED:
    QCRIL_LOG_INFO( "%s", "<<<DSI_EVT_WDS_CONNECTED: DONE<<<" );
    goto ret;

  default:
    QCRIL_LOG_ERROR( "invalid dsi_net_ev [%d]", (dsi_net_evt_t)evt_info_ptr->evt);
    QCRIL_DS_ASSERT( 0, "validate dsi_net_ev" );
    goto err_label_exit;
  }/* switch() */

unsol_rsp:
  qcril_data_unsol_call_list_changed( instance_id);
ret:
  QCRIL_DS_ASSERT( evt_info_ptr != NULL, "validate event obj" );
  QCRIL_LOG_INFO( "%s", "try event obj dealloc" );
  QCRIL_DATA_RELEASE_STORAGE( evt_info_ptr );
  QCRIL_LOG_DEBUG( "%s", "qcril_data_event_hdlr: EXIT with suc" );
  QCRIL_DATA_MUTEX_UNLOCK(&info_tbl_mutex);
  return;

err_label:
  QCRIL_DS_ASSERT( evt_info_ptr != NULL, "validate event obj" );
  QCRIL_LOG_INFO( "%s", "try event obj dealloc" );
  QCRIL_DATA_RELEASE_STORAGE( evt_info_ptr );

err_label_no_free:
  if ( qcril_reqlist_query( instance_id, params_ptr->t, &reqlist_info ) == E_SUCCESS )
  {
    QCRIL_LOG_DEBUG( "%s", "respond to QCRIL as generic failure" );
    qcril_data_response_generic_failure( instance_id, reqlist_info.t, reqlist_info.request,
                                         call_end_cause);
  }

err_label_exit:
  QCRIL_DATA_MUTEX_UNLOCK(&info_tbl_mutex);
  QCRIL_LOG_ERROR("%s", "qcril_data_event_hdlr: EXIT with err");
  return;

}/* qcril_data_event_hdlr() */

/*===========================================================================
  FUNCTION:  QCRIL_DATA_GET_CALL_END_REASON

===========================================================================*/
/*!
    @brief
    Provides call end reason based on IP family type and roaming status.
    For dual-IP call failures, chooses the temporary call end reason (if
    available)
    @pre Before calling, info_tbl_mutex must not be locked by the calling
    thread.

    @return
    QCRIL_DS_SUCCESS on success, QCRIL_DS_ERROR on error
*/
/*=========================================================================*/
int qcril_data_get_call_end_reason
(
  qcril_data_call_info_tbl_type * info_tbl_ptr,
  int * call_end_reason
)
{
  QCRIL_LOG_DEBUG("qcril_data_get_call_end_reason :ENTRY");
  dsi_ip_family_t ipf = DSI_IP_FAMILY_V4;
  dsi_ce_reason_t v4ce = {.reason_type=DSI_CE_TYPE_INVALID,
                          .reason_code=WDS_VCER_TYPE_UNSPECIFIED_V01};
  dsi_ce_reason_t v6ce = {.reason_type=DSI_CE_TYPE_INVALID,
                          .reason_code=WDS_VCER_TYPE_UNSPECIFIED_V01};
  dsi_ce_reason_t tmp_end_reason;
  boolean dual_ip_family = FALSE;

  if (!VALIDATE_LOCAL_DATA_OBJ(info_tbl_ptr) || !call_end_reason)
  {
    QCRIL_LOG_ERROR( "info_tbl_ptr [%p], call_end_reason [%p]",
                     (unsigned int *)info_tbl_ptr, call_end_reason);
    return QCRIL_DS_ERROR;
  }

  *call_end_reason = PDP_FAIL_OEM_DCFAILCAUSE_4;
  if( info_tbl_ptr->call_params != NULL )
  {
    if( global_dsd_info.roaming )
    {
      QCRIL_LOG_DEBUG("Picking roam IP type");
      ipf = qcril_data_get_dsi_ip_family(
                       info_tbl_ptr->call_params->ril_ip_roaming,
                       QCRIL_IP_FAMILY_STR_MAX);
      if (strncmp( info_tbl_ptr->call_params->ril_ip_roaming,
                   QCRIL_DATA_IP_FAMILY_V4V6,
                   QCRIL_IP_FAMILY_STR_MAX) == 0)
      {
        dual_ip_family = TRUE;
      }
    }
    else
    {
      QCRIL_LOG_DEBUG("Picking home IP type");
      ipf = qcril_data_get_dsi_ip_family(
                       info_tbl_ptr->call_params->ril_ipfamily,
                       QCRIL_IP_FAMILY_STR_MAX);
      if (strncmp( info_tbl_ptr->call_params->ril_ipfamily,
                   QCRIL_DATA_IP_FAMILY_V4V6,
                   QCRIL_IP_FAMILY_STR_MAX) == 0)
      {
        dual_ip_family = TRUE;
      }
    }
  }
  else
  {
    /*For embms calls , call params aren't needed & hence aren't provided*/
    QCRIL_LOG_DEBUG("For calls where call params aren't needed");
    if ( DSI_IP_VERSION_6 == info_tbl_ptr->dsi_ip_version )
    {
      ipf = DSI_IP_FAMILY_V6;
    }
    else if (DSI_IP_VERSION_4 == info_tbl_ptr->dsi_ip_version )
    {
      ipf = DSI_IP_FAMILY_V4;
    }
    else if ( DSI_IP_VERSION_4_6 == info_tbl_ptr->dsi_ip_version )
    {
      ipf = DSI_IP_FAMILY_V4;
      dual_ip_family = TRUE;
    }
  }
  /*
   * If requested call was V4V6 and both have failed then we should pick the
   * cause code that is temporary
  */
  if ( dual_ip_family )
  {
    if (dsi_get_call_end_reason(info_tbl_ptr->dsi_hndl,
                                &v4ce,
                                DSI_IP_FAMILY_V4) != DSI_SUCCESS) {

      QCRIL_LOG_ERROR("Unable to get dsi call end reason for V4");
    }

    if (dsi_get_call_end_reason(info_tbl_ptr->dsi_hndl,
                                &v6ce,
                                DSI_IP_FAMILY_V6) != DSI_SUCCESS) {

      QCRIL_LOG_ERROR("Unable to get dsi call end reason for V6");
    }

    qcril_data_pick_temporary_call_end_reason(&v4ce, &v6ce, &tmp_end_reason);
    if (SUCCESS != qcril_data_get_ril_ce_code(&tmp_end_reason,
                                              call_end_reason))
    {
      QCRIL_LOG_DEBUG("%s","**programming err**");
      *call_end_reason = PDP_FAIL_OEM_DCFAILCAUSE_1;
    }
    QCRIL_LOG_INFO( "set call end reason [%d]", *call_end_reason );
  }
  /* Get call fail reason or use default reason */
  else if ( dsi_get_call_end_reason(info_tbl_ptr->dsi_hndl,
                                    &tmp_end_reason,
                                    ipf ) == DSI_SUCCESS )
  {
    QCRIL_LOG_DEBUG("dsi_get_call_end_reason for IP family [%d] returned [%d]",
                    ipf, tmp_end_reason.reason_code);
    if (SUCCESS != qcril_data_get_ril_ce_code(&tmp_end_reason,
                                              call_end_reason))
    {
      QCRIL_LOG_DEBUG("%s","**programming err**");
      *call_end_reason = PDP_FAIL_OEM_DCFAILCAUSE_1;
    }
    QCRIL_LOG_INFO( "set call end reason [%d]", *call_end_reason );
  }
  else
  {
    *call_end_reason = PDP_FAIL_OEM_DCFAILCAUSE_1;
    QCRIL_LOG_ERROR( "get call end reason FAILED, got [%u], use def",
                     *call_end_reason );
  }
  return QCRIL_DS_SUCCESS;
} /* qcril_data_get_call_end_reason() */

/*===========================================================================
  FUNCTION:  qcril_data_wds_event_hdlr

===========================================================================*/
/*!
    @brief
    Registered with QCRIL to be called by QCRIL on event
    QCRIL_EVT_DATA_WDS_EVENT_CALLBACK

    @pre Before calling, info_tbl_mutex must not be locked by the calling thread

    @return
    None.
*/
/*=========================================================================*/
void qcril_data_wds_event_hdlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr  /*output*/
)
{
  qcril_data_wds_event_data_t* event_data;
  QCRIL_LOG_DEBUG( "%s", "qcril_data_wds_event_hdlr: ENTRY" );

  do
  {
    if (!params_ptr || !ret_ptr)
    {
      QCRIL_LOG_ERROR("%s", "invalid parameters");
      break;
    }

    if (params_ptr->instance_id >= QCRIL_MAX_INSTANCE_ID)
    {
      QCRIL_LOG_ERROR("%s", "invalid instance id");
      break;
    }

    event_data = params_ptr->data;

    if (!event_data)
    {
      QCRIL_LOG_ERROR("%s", "invalid event data pointer");
      break;
    }

    QCRIL_DATA_MUTEX_LOCK(&info_tbl_mutex);

    qcril_data_process_wds_ind(event_data->wds_hndl,
                               event_data->user_data,
                               event_data->ind_id,
                               &event_data->ind_data);

    QCRIL_DATA_RELEASE_STORAGE(event_data);

    QCRIL_DATA_MUTEX_UNLOCK(&info_tbl_mutex);
  } while (0);

  QCRIL_LOG_DEBUG( "%s", "qcril_data_wds_event_hdlr: EXIT" );
}/*qcril_data_wds_event_hdlr*/


/*===========================================================================
  FUNCTION:  qcril_data_dsd_event_hdlr

===========================================================================*/
/*!
    @brief
    Registered with QCRIL to be called by QCRIL on event
    QCRIL_EVT_DATA_DSD_EVENT_CALLBACK

    @pre Before calling, info_tbl_mutex must not be locked by the calling thread

    @return
    None.
*/
/*=========================================================================*/
void qcril_data_dsd_event_hdlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr  /*output*/
)
{
  dsd_system_status_ind_msg_v01  *ind_data = NULL;
  QCRIL_LOG_DEBUG( "%s", "qcril_data_dsd_event_hdlr: ENTRY" );

  do
  {
    if (!params_ptr || !ret_ptr)
    {
      QCRIL_LOG_ERROR("%s", "invalid parameters");
      break;
    }

    if (params_ptr->instance_id >= QCRIL_MAX_INSTANCE_ID)
    {
      QCRIL_LOG_ERROR("%s", "invalid instance id");
      break;
    }

    ind_data = params_ptr->data;

    if (!ind_data)
    {
      QCRIL_LOG_ERROR("%s", "invalid event data pointer");
      break;
    }

    qcril_data_process_qmi_dsd_ind(ind_data);

    QCRIL_DATA_RELEASE_STORAGE(ind_data);
  } while (0);

  QCRIL_LOG_DEBUG( "%s", "qcril_data_dsd_event_hdlr: EXIT" );
}


/*-------------------------------------------------------------------------

                           UTILITY ROUTINES

-------------------------------------------------------------------------*/
/*===========================================================================

  FUNCTION:  qcril_data_go_dormant

===========================================================================*/
/*!
    @brief
    puts the physlink corresponding to the qmi_wds_hndl dormant

    @return
    SUCCESS
    FAILURE
*/
/*=========================================================================*/
static int qcril_data_iface_go_dormant
(
  qmi_client_type qmi_wds_hndl,
  dsi_ip_family_t ip_family
)
{
  int ret = FAILURE;

  (void) ip_family;

  do
  {

    if (QCRIL_DATA_HNDL_INVALID == qmi_wds_hndl)
    {
      QCRIL_LOG_ERROR("%s","invalid qmi_wds_hndl received");
      break;
    }

    if(E_SUCCESS != qcril_data_qmi_wds_go_dormant_req(qmi_wds_hndl))
    {
      QCRIL_LOG_ERROR("qcril_data_qmi_wds_go_dormant_req failed");
      break;
    }

    ret = SUCCESS;
  } while (0);

  return ret;
}

/*===========================================================================

  FUNCTION:  qcril_data_iface_set_mtu

===========================================================================*/
static int qcril_data_iface_set_mtu
(
  dsi_hndl_t dsi_hndl,
  int mtu
)
{
  int ret = FAILURE, ioctl_err;
  struct ifreq ifr;
  int socket_fd;
  char iface_name[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+1];

  /* Create a socket to set use for setting the MTU value*/
  socket_fd = socket(PF_INET,SOCK_DGRAM,0);
  if (socket_fd < 0)
  {
    QCRIL_LOG_ERROR("Creating socket failed. Could not set MTU to [%d]", mtu);
    goto socket_err_label;
  }

  /* Get the name of the interface */
  if (DSI_ERROR == dsi_get_device_name(dsi_hndl,
                                       iface_name,
                                       DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+1))
  {
    QCRIL_LOG_ERROR("couldn't get device name. Could not set MTU to [%d]", mtu);
    goto err_label;
  }
  iface_name[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN] = '\0';
  memset(&ifr, 0, sizeof(ifr));
  strlcpy(ifr.ifr_ifrn.ifrn_name, iface_name, sizeof(ifr.ifr_ifrn.ifrn_name) );

  /* Set the correct MTU value*/
  ifr.ifr_ifru.ifru_mtu = mtu;
  ioctl_err = ioctl(socket_fd, SIOCSIFMTU, &ifr);
  if ( ioctl_err < 0 )
  {
    QCRIL_LOG_ERROR("Error setting MTU to [%d] on interface [%s] error %d", mtu, iface_name, ioctl_err);
    goto err_label;
  }

  QCRIL_LOG_DEBUG("MTU set to [%d] on interface [%s]", mtu, iface_name);
  ret = SUCCESS;

err_label:
  close(socket_fd);
socket_err_label:
  return ret;
}


/*===========================================================================

  FUNCTION:  qcril_data_iface_ioctl

===========================================================================*/
/*!
    @brief
    executes the given ioctl on the given interface

    if ioctl is QCRIL_DATA_IOCTL_DORMANCY_INDICATIONS_ON,
    and if current dormancy status has changed from the last
    global status we were aware of, dorm_status_changed would be
    set to TRUE, and call_state would be set to appropriate
    PHYSLINK status.
    CALL_ACTIVE_PHYSLINK_DOWN
    CALL_ACTIVE_PHYSLINK_UP

    @pre caller must have locked info_tbl_mutex prior to calling this function.

    @return
    SUCCESS
    FAILURE
*/
/*=========================================================================*/
int qcril_data_iface_ioctl
(
  qcril_data_call_info_tbl_type * info_tbl_ptr,
  int ioctl,
  boolean * dorm_status_changed,
  int * call_state
)
{
  boolean dorm_ind = FALSE;
  int ret = FAILURE;
  int reti = SUCCESS;
  wds_set_event_report_req_msg_v01 event_report_params;
  qmi_client_type qmi_wds_hndl;
  int state;

  do
  {

    if(!VALIDATE_LOCAL_DATA_OBJ(info_tbl_ptr))
    {
      QCRIL_LOG_ERROR("%s","invlaid info_tbl_ptr received");
      break;
    }

    qmi_wds_hndl = info_tbl_ptr->qmi_wds_hndl;

    if(QCRIL_DATA_HNDL_INVALID == qmi_wds_hndl)
    {
      QCRIL_LOG_ERROR("Invalid qmi_wds_hndl found in this info_tbl_ptr %x",
                      info_tbl_ptr);
      break;
    }

    if(NULL == dorm_status_changed || NULL == call_state)
    {
      QCRIL_LOG_ERROR("%s","bad parameters received");
      break;
    }

    *dorm_status_changed = FALSE;

    switch(ioctl)
    {
    case QCRIL_DATA_IFACE_IOCTL_DORMANCY_INDICATIONS_OFF:
    case QCRIL_DATA_IFACE_IOCTL_DORMANCY_INDICATIONS_ON:
      if (QCRIL_DATA_IFACE_IOCTL_DORMANCY_INDICATIONS_OFF == ioctl)
      {
        dorm_ind = FALSE;
      }
      else
      {
        dorm_ind = TRUE;
      }

      memset(&event_report_params, 0, sizeof(event_report_params));
      event_report_params.report_dormancy_status_valid = TRUE;
      event_report_params.report_dormancy_status = dorm_ind;

      QCRIL_LOG_DEBUG("setting dormancy event report on wds client [%p]",
                      qmi_wds_hndl);
      if (E_SUCCESS != qcril_data_qmi_wds_set_event_report(qmi_wds_hndl,
                                                           &event_report_params))
      {
        QCRIL_LOG_ERROR("%s", "qcril_data_qmi_wds_set_event_report: Set event report failed");
        reti = FAILURE;
        break;
      }

      if (dorm_ind == TRUE)
      {
        QCRIL_LOG_DEBUG("getting dormancy status for wds client [%p]",
                        qmi_wds_hndl);
        if(E_SUCCESS != qcril_data_qmi_wds_get_dormant_status(qmi_wds_hndl,
                                                              &info_tbl_ptr->dorm_status))
        {
          QCRIL_LOG_ERROR("%s", "qcril_data_qmi_wds_get_dormant_status failed");
          reti = FAILURE;
          break;
        }

        if (info_tbl_ptr->dorm_status != global_dorm_status)
        {
          QCRIL_LOG_VERBOSE("dormancy status has changed since "     \
                          "last registration");
          if (info_tbl_ptr->dorm_status == WDS_DORMANCY_STATUS_ACTIVE_V01)
          {
            /* dsi_event = DSI_EVT_PHYSLINK_UP_STATE; */
            state = CALL_ACTIVE_PHYSLINK_UP;
            /*
            QCRIL_LOG_INFO("%s", "<<<DSI_EVT_PHYSLINK_UP " \
            "processing started<<<");*/
          }
          else
          {
            /* dsi_event = DSI_EVT_PHYSLINK_DOWN_STATE; */
            state = CALL_ACTIVE_PHYSLINK_DOWN;
            /* QCRIL_LOG_INFO("%s", "<<<DSI_EVT_PHYSLINK_DOWN "  \
               "processing started<<<"); */
          }

          *dorm_status_changed = TRUE;
          *call_state = state;
        }

        global_dorm_status = info_tbl_ptr->dorm_status;
      }

      break;
    case QCRIL_DATA_IFACE_IOCTL_GO_DORMANT:
      reti = qcril_data_iface_go_dormant(qmi_wds_hndl, info_tbl_ptr->dsi_ip_version);
      break;
    default:
      QCRIL_LOG_ERROR("unsupported ioctl %d", ioctl);
      reti = FAILURE;
      break;
    }
    if(reti == FAILURE)
    {
      break;
    }

    ret = SUCCESS;
  } while (0);

  return ret;
}


/*===========================================================================

  FUNCTION:  qcril_data_post_dsi_netctrl_event

===========================================================================*/
/*!
    @brief
    This routine calls qcril_event_queue to let QCRIL call the appropriate
    event/cmd handler for event/cmd.
    Note: do not assume the context in which handler will be called

    @return
    None.
*/
/*=========================================================================*/
static void qcril_data_post_dsi_netctrl_event
(
  qcril_instance_id_e_type instance_id,
  qcril_modem_id_e_type    modem_id,
  RIL_Token                pend_tok,
  qcril_data_call_info_tbl_type *info_tbl_ptr,
  dsi_net_evt_t            net_evt,
  dsi_evt_payload_t       *payload_ptr
)
{
  qcril_data_event_data_t       *evt;

  QCRIL_DS_LOG_DBG_MEM( "info_tbl", info_tbl_ptr );
  QCRIL_LOG_DEBUG( "dsi net evt [%d], info_tbl index [%d], pend RIL Token [%d]",
                   net_evt, info_tbl_ptr->index,
                   qcril_log_get_token_id( info_tbl_ptr->pend_tok ) );

  /* Allocate from heap here and clean-up on call end */
  evt = ( qcril_data_event_data_t *)malloc( sizeof( qcril_data_event_data_t ) );
  if ( evt == NULL )
  {
    QCRIL_LOG_ERROR( "%s","unable to alloc mem for event obj" );
    return;
  }
  QCRIL_DS_LOG_DBG_MEM( "event obj alloc", evt );
  memset( evt, 0, sizeof( qcril_data_event_data_t ) );

  /* Populate data event obj */
  evt->evt      = net_evt;
  evt->data     = info_tbl_ptr;
  evt->data_len = sizeof( qcril_data_call_info_tbl_type );
  evt->self     = evt;

  if( NULL != payload_ptr )
  {
    evt->payload = *payload_ptr;
  }

  /*
    Call QCRIL API to process this event
    The data event hdlr will be called by RIL thread
    In case of unsol event RIL Token will be 0
  */
  QCRIL_LOG_VERBOSE( "queue QCRIL DATA event for RIL Token [%d] "       \
                     "instance_id [%d], and modem_id [%d]",
                     qcril_log_get_token_id( info_tbl_ptr->pend_tok ),
                     info_tbl_ptr->instance_id, info_tbl_ptr->modem_id);

  if(E_SUCCESS == qcril_event_queue( instance_id,
                                     modem_id,
                                     QCRIL_DATA_NOT_ON_STACK,
                                     QCRIL_EVT_DATA_EVENT_CALLBACK,
                                     ( void * )evt,
                                     sizeof( qcril_data_event_data_t ),
                                     pend_tok ))
  {
    return;
  }
  else
  {
    QCRIL_LOG_ERROR("%s", "qcril_event_queue failed\n");
  }

//err_ret:
  QCRIL_DATA_RELEASE_STORAGE(evt);
}

/*===========================================================================

                        FUNCTIONS REGISTERED WITH QCRIL

===========================================================================*/
/*===========================================================================

  FUNCTION:  qcril_data_command_hdlr

===========================================================================*/
/*!
    @brief
    Command handler for QCRIL Data

    @return
    None.
*/
/*=========================================================================*/
void qcril_data_command_hdlr(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
)
{
  QCRIL_LOG_DEBUG( "%s", "qcril_data_command_hdlr: ENTRY" );

  /*-----------------------------------------------------------------------*/
  QCRIL_DS_ASSERT( ( params_ptr != NULL ), "validate input params_ptr" );
  QCRIL_DS_ASSERT( ( ret_ptr    != NULL ), "validate input ret_ptr" );
  /*-----------------------------------------------------------------------*/

  if ( ( params_ptr == NULL ) || ( ret_ptr == NULL ) )
  {
    QCRIL_LOG_DEBUG( "%s", "qcril_data_command_hdlr: Bad args: EXIT" );
    return;
  }

  qcril_data_cmd_data_t *cmd_data;
  int rc = QMI_NO_ERR;
  dsd_bind_subscription_req_msg_v01  bind_req_msg;
  dsd_bind_subscription_resp_msg_v01  *bind_resp_msg = NULL;
  qcril_data_cb_struct *cb_data = NULL;
  qmi_txn_handle txn_handle;


  cmd_data = ( qcril_data_cmd_data_t * )params_ptr->data;

  QCRIL_DS_LOG_DBG_MEM( "event obj", cmd_data );
  if ( !VALIDATE_LOCAL_DATA_OBJ( cmd_data ) )
  {
    QCRIL_LOG_ERROR( "%s", "bad event obj, cannot free mem, ret with err" );
    QCRIL_LOG_DEBUG( "%s", "qcril_data_command_hdlr: Bad cmd_data: EXIT" );
    return;
  }

  if(cmd_data->cmd_id != QCRIL_DATA_STACK_SWITCH_CMD)
  {
    QCRIL_LOG_ERROR( "Bad cmd id [%d] in qcril_data_command_hdlr()",
        cmd_data->cmd_id);
    QCRIL_LOG_DEBUG( "%s", "qcril_data_command_hdlr: EXIT" );
    QCRIL_DATA_RELEASE_STORAGE(cmd_data);
    return;
  }
  cb_data = (qcril_data_cb_struct *)
            malloc(sizeof(qcril_data_cb_struct));

  bind_resp_msg = (dsd_bind_subscription_resp_msg_v01*)
                           malloc(sizeof(dsd_bind_subscription_resp_msg_v01));

  if(!bind_resp_msg || !cb_data)
  {
    if(bind_resp_msg)
    {
      free(bind_resp_msg);
    }
    if(cb_data)
    {
      free(cb_data);
    }
    QCRIL_LOG_ERROR("Unable to allocate memory");
    QCRIL_LOG_DEBUG( "%s", "qcril_data_command_hdlr: EXIT" );
    return;
  }

  memset(cb_data,0,sizeof(qcril_data_cb_struct));

  switch(cmd_data->new_stack_id)
  {
    case QCRIL_MODEM_PRIMARY_STACK_ID:
     global_subs_id = WDS_PRIMARY_SUBS_V01;
     break;

    case QCRIL_MODEM_SECONDARY_STACK_ID:
     global_subs_id = WDS_SECONDARY_SUBS_V01;
     break;

    case QCRIL_MODEM_TERTIARY_STACK_ID:
     global_subs_id = WDS_TERTIARY_SUBS_V01;
     break;

    default:
     global_subs_id = WDS_DEFAULT_SUBS_V01;
  }

  QCRIL_LOG_DEBUG( "qcril_data_command_hdlr: stack_id: %d, subs_id: %d",
                      cmd_data->new_stack_id,global_subs_id);

  dsi_set_modem_subs_id(global_subs_id);

  qdp_set_subscription(global_subs_id);

  if (E_SUCCESS != qcril_data_qmi_wds_bind_subscription(global_qcril_qmi_wds_hndl,
                                                        global_subs_id))
  {
    QCRIL_LOG_ERROR("%s", "qcril_data_qmi_wds_bind_subscription: subs_id binding failed");
  }

  if(global_qmi_dsd_hndl != NULL)
  {
    /* Bind the subscription */
    memset(&bind_req_msg, 0, sizeof(bind_req_msg));
    memset(bind_resp_msg, 0, sizeof(dsd_bind_subscription_resp_msg_v01));

    bind_req_msg.bind_subs = qcril_data_get_qmi_dsd_subscription_id();

    QCRIL_LOG_DEBUG("binding subscription to subs=%d",
                    bind_req_msg.bind_subs);

    cb_data->t = params_ptr->t;
    cb_data->event_id = params_ptr->event_id;
    cb_data->msg_id = QMI_DSD_BIND_SUBSCRIPTION_REQ_V01;
    cb_data->self = (void *)cb_data;
    cb_data->bind_sub= bind_req_msg.bind_subs;
    QCRIL_LOG_DEBUG("Debug : Token [%d] event_id [%d] self [%u]", cb_data->t, params_ptr->event_id, cb_data->self);
    QCRIL_LOG_DEBUG("Debug : bind subscription [%d]", cb_data->bind_sub);

    rc = qmi_client_send_msg_async(global_qmi_dsd_hndl,
                                QMI_DSD_BIND_SUBSCRIPTION_REQ_V01,
                                &bind_req_msg,
                                sizeof(bind_req_msg),
                                bind_resp_msg,
                                sizeof(dsd_bind_subscription_resp_msg_v01),
                                qcril_data_async_cb,
                                (void *)cb_data,
                                &txn_handle);
    if (QMI_NO_ERR != rc)
    {
      QCRIL_LOG_ERROR("failed to bind subscription, err=%d", rc);
    }
  }

  if(QMI_NO_ERR != rc || !global_qmi_dsd_hndl)
  {
    QCRIL_DATA_RELEASE_STORAGE(bind_resp_msg);
    QCRIL_DATA_RELEASE_STORAGE(cb_data);
  }
  QCRIL_DATA_RELEASE_STORAGE(cmd_data);
  QCRIL_LOG_DEBUG( "%s", "qcril_data_command_hdlr: EXIT" );

 }/* qcril_data_command_callback() */


/*===========================================================================

  FUNCTION:  qcril_data_toggle_dormancy_indications

===========================================================================*/
/*!
    @brief

    Handles request to turn ON/OFF dormancy indications. Typically called to
    turn off indications when in power save mode  and turn back on when out
    of power save mode.

    @pre Before calling, info_tbl_mutex must not be locked by the calling thread

    @return QCRIL_DS_SUCCESS on success and QCRIL_DS_ERROR on failure.
*/
/*=========================================================================*/
int qcril_data_toggle_dormancy_indications
(
  qcril_data_dormancy_ind_switch_type dorm_ind_switch
)
{
  int i, rmnet_physlink_toggled[QCRIL_DATA_MAX_DEVS];
  int dev_instance;
  int ret_val = QCRIL_DS_ERROR;
  boolean dorm_status_changed = FALSE;
  int call_state;
  dsi_net_evt_t dsi_event = DSI_EVT_MAX;
  int all_calls_dormant;


  qcril_data_iface_ioctl_type ioctl;

  memset(rmnet_physlink_toggled, FALSE, sizeof(rmnet_physlink_toggled));

  QCRIL_DATA_MUTEX_LOCK(&info_tbl_mutex);

  if (dorm_ind_switch == DORMANCY_INDICATIONS_OFF)
  {
    ioctl = QCRIL_DATA_IFACE_IOCTL_DORMANCY_INDICATIONS_OFF;
  }
  else if (dorm_ind_switch == DORMANCY_INDICATIONS_ON)
  {
    ioctl = QCRIL_DATA_IFACE_IOCTL_DORMANCY_INDICATIONS_ON;
  }
  else
  {
    QCRIL_LOG_ERROR( "%s","Bad input received.");
    goto err_label;
  }

  /* update global_dorm_ind
     so if data call is up later, we know what is the prvioius request
     from telephony, the request normally is associted with screen state */

  global_dorm_ind = dorm_ind_switch;

  if (dorm_ind_switch == DORMANCY_INDICATIONS_OFF)
  {
    all_calls_dormant = qcril_data_all_calls_dormant();
    if (!all_calls_dormant)
    {
      QCRIL_DATA_MUTEX_UNLOCK(&info_tbl_mutex);
      QCRIL_LOG_ERROR( "%s","qcril_data_toggle_dormancy_indications: EXIT with as calls are not dormant");
      return ret_val;
    }
  }

  QCRIL_LOG_DEBUG("Switch ON/OFF dormancy indications on all active " \
                  "interfaces ioctl:%d Update global_dorm_ind: %d",
                  ioctl, global_dorm_ind);

  for( i = 0; i < MAX_CONCURRENT_UMTS_DATA_CALLS; i++ )
  {
    GET_DEV_INSTANCE_FROM_NAME(i, dev_instance);
    if (QCRIL_DATA_INVALID_DEV == dev_instance)
    {
      continue;
    }
    if (dev_instance < 0 || dev_instance >= QCRIL_DATA_MAX_DEVS)
    {
      QCRIL_LOG_ERROR("invalid dev_instance [%d] derived for index [%d]",
                      dev_instance, i);
      continue;
    }

    if(VALIDATE_LOCAL_DATA_OBJ(&info_tbl[i]) &&
       (info_tbl[i].cid != CALL_ID_INVALID) &&
       rmnet_physlink_toggled[dev_instance] == FALSE)
    {
      QCRIL_LOG_DEBUG( "selected index = %d",i);
      if ((ret_val = qcril_data_iface_ioctl(&info_tbl[i],
                                            ioctl,
                                            &dorm_status_changed,
                                            &call_state))
          == FAILURE)
      {
        QCRIL_LOG_ERROR( "%s","Request to toggle Dormancy indication failed.");
        ret_val = QCRIL_DS_ERROR;
        goto err_label;
      }
      else if (dorm_status_changed)
      {
        /* derive dsi event from the call_state */
        switch(call_state)
        {
        case CALL_ACTIVE_PHYSLINK_UP:
          dsi_event = DSI_EVT_PHYSLINK_UP_STATE;
          break;
        case CALL_ACTIVE_PHYSLINK_DOWN:
          dsi_event = DSI_EVT_PHYSLINK_DOWN_STATE;
          break;
        default:
          QCRIL_LOG_ERROR("invalid call_state [%d]", call_state);
          dsi_event = DSI_EVT_MAX;
          break;
        }
        if (dsi_event != DSI_EVT_MAX)
        {
          /* post an async event to the event thread */
          qcril_data_post_dsi_netctrl_event(info_tbl[i].instance_id,
                                            info_tbl[i].modem_id,
                                            info_tbl[i].pend_tok,
                                            &info_tbl[i],
                                            dsi_event,
                                            NULL);
        }
      }

      rmnet_physlink_toggled[dev_instance] = TRUE;
    }
  }/* for() */

  QCRIL_DATA_MUTEX_UNLOCK(&info_tbl_mutex);
  QCRIL_LOG_INFO( "%s","qcril_data_toggle_dormancy_indications: EXIT with SUCCESS");
  return ret_val = QCRIL_DS_SUCCESS;

err_label:
  QCRIL_DATA_MUTEX_UNLOCK(&info_tbl_mutex);
  QCRIL_LOG_ERROR( "%s","qcril_data_toggle_dormancy_indications: EXIT with ERROR");
  return ret_val;
}

/*===========================================================================

  FUNCTION:  qcril_data_toggle_limited_sys_indications

===========================================================================*/
/*!
    @brief

    Handles request to turn ON/OFF limited data system status change
    indications. Typically called to TURN-ON limited indications when
    in screen-off state. In screen-on state, this is TURNED-OFF so
    full data system status indications can be received.

    @return QCRIL_DS_SUCCESS on success
    @return QCRIL_DS_ERROR on failure.
*/
/*=========================================================================*/
int
qcril_data_toggle_limited_sys_indications
(
  qcril_data_limited_sys_ind_switch_type       sys_ind_switch
)
{
  int all_calls_dormant;
  int res = QCRIL_DS_SUCCESS;

  all_calls_dormant = qcril_data_all_calls_dormant();

  QCRIL_LOG_INFO ("ENTRY: Limited data system indications switch: %d, "
                  "All calls dormancy status: %d",
                   sys_ind_switch, all_calls_dormant);

  global_data_sys_ind_switch = sys_ind_switch;

  if (LIMITED_SYS_INDICATIONS_OFF == sys_ind_switch)
  {
    /* Register for full indications immedately */
    res = qcril_data_reg_sys_ind(LIMITED_SYS_INDICATIONS_OFF);
  }
  else if (all_calls_dormant || global_data_force_limited_sys_indication)
  {
    /* Register for limited indications immediately */
    res = qcril_data_reg_sys_ind(LIMITED_SYS_INDICATIONS_ON);
  }
  else
  {
    /* Defer registration for limited indications */
    QCRIL_LOG_DEBUG ("Calls active, defering limited "
                     "data sys indication registration");

  }

  QCRIL_LOG_INFO ("%s", "EXIT");
  return res;
}


/*===========================================================================

  FUNCTION:  qcril_data_setup_data_call_timeout_hdlr

===========================================================================*/
/*!
    @brief
    Timeout handler for RIL_REQUEST_SETUP_DATA_CALL

    @return
    None.
*/
/*=========================================================================*/
static void qcril_data_setup_data_call_timeout_hdlr
(
  union sigval sval
)
{
  qcril_data_call_info_tbl_type *info_tbl_ptr = sval.sival_ptr;

  QCRIL_LOG_VERBOSE( "%s", "qcril_data_setup_data_call_timeout_hdlr: ENTRY" );

  /* Lock the mutex before cleanup of info_tbl */
  QCRIL_DATA_MUTEX_LOCK(&info_tbl_mutex);

  if (!VALIDATE_LOCAL_DATA_OBJ(info_tbl_ptr))
  {
    QCRIL_LOG_ERROR( "invalid info_tbl_ptr [%p] ",
                     (unsigned int *)info_tbl_ptr);
    goto err_label;
  }

  /* If the timer hasn't been canceled right after timing out */
  if (QCRIL_DATA_INVALID_TIMERID != info_tbl_ptr->timer_id)
  {
    QCRIL_LOG_DEBUG ("timeout handler for timer [%#x], cid [%d], tok [%ld]",
          info_tbl_ptr->timer_id, info_tbl_ptr->cid,
          qcril_log_get_token_id( info_tbl_ptr->pend_tok ));

    /* If this is a Dual-IP partial retry */
    if (TRUE == info_tbl_ptr->is_partial_retry &&
        DSI_IP_VERSION_4_6 == info_tbl_ptr->dsi_ip_version)
    {
      QCRIL_LOG_DEBUG ("%s", "Partial retry had been attempted...");

      /* Clear the flag and send a response to the upper layer */
      info_tbl_ptr->is_partial_retry = FALSE;

      /* Schedule a new partial retry attempt */
      qcril_data_util_schedule_partial_retry_attempt(info_tbl_ptr, TRUE, TRUE);
    }
    else
    {
      /* Stop the data call */
      QCRIL_LOG_DEBUG ("%s", "stopping data call...");
      if( E_SUCCESS != qcril_data_stop_data_call(info_tbl_ptr) )
      {
        qcril_data_response_generic_failure( info_tbl_ptr->instance_id,
                                             info_tbl_ptr->pend_tok,
                                             QCRIL_EVT_RIL_REQUEST_SETUP_DATA_CALL,
                                             RIL_E_OEM_ERROR_3);

        /* Cleanup call state */
        qcril_data_cleanup_call_state( info_tbl_ptr );
      }

      /* Response to client will be generated in event handler */
    }
  }

err_label:
  /* Unock the mutex of info_tbl */
  QCRIL_DATA_MUTEX_UNLOCK(&info_tbl_mutex);

  QCRIL_LOG_VERBOSE( "%s", "qcril_data_setup_data_call_timeout_hdlr: EXIT" );
}


/*===========================================================================

  FUNCTION:  qcril_data_start_response_timer

===========================================================================*/
/*!
    @brief
    Create and start a response timer for setup data call request

    @pre Before calling, info_tbl_mutex must be locked by the calling thread

    @return
    None.
*/
/*=========================================================================*/
void qcril_data_start_response_timer
(
  qcril_data_call_info_tbl_type *info_tbl_ptr
)
{
  struct sigevent sigev;
  struct itimerspec itimers;

  static char  args[PROPERTY_VALUE_MAX];
  char  def[QCRIL_DATA_PROPERTY_TIMEOUT_SIZE+1];
  int   ret;

  QCRIL_LOG_VERBOSE( "%s", "qcril_data_start_response_timer: ENTRY" );

  if (!VALIDATE_LOCAL_DATA_OBJ(info_tbl_ptr))
  {
    QCRIL_LOG_ERROR( "invalid info_tbl_ptr:[%p] ",
                     (unsigned int *)info_tbl_ptr);
    return;
  }

  sigev.sigev_notify            = SIGEV_THREAD;
  sigev.sigev_notify_attributes = NULL;
  sigev.sigev_value.sival_ptr   = info_tbl_ptr;
  sigev.sigev_notify_function   = qcril_data_setup_data_call_timeout_hdlr;

  /* The timer_id should be invalid, if not log an error and delete it */
  if (QCRIL_DATA_INVALID_TIMERID != info_tbl_ptr->timer_id)
  {
    QCRIL_LOG_ERROR( "deleting stale timer_id:[%#"PRIxPTR"] ",
                     (uintptr_t)info_tbl_ptr->timer_id);

    qcril_data_util_stop_timer(&info_tbl_ptr->timer_id);
  }

  /* Create the timer */
  if (-1 == timer_create(CLOCK_REALTIME, &sigev, &info_tbl_ptr->timer_id))
  {
    QCRIL_LOG_ERROR( "failed to create timer for info_tbl_ptr:[%p] ",
                     (unsigned int *)info_tbl_ptr);
    return;
  }

  QCRIL_LOG_VERBOSE( "timer creation success: [%#x]", info_tbl_ptr->timer_id );

  memset( args, 0x0, sizeof(args) );
  memset( def, 0x0, sizeof(def) );

  /* assign default value before query property value */
  itimers.it_value.tv_sec = QCRIL_DATA_PROPERTY_TIMEOUT_DEFAULT;
  itimers.it_value.tv_nsec    = 0;
  itimers.it_interval.tv_sec  = 0;
  itimers.it_interval.tv_nsec = 0;

  /* Query Android proprerty for timeout override */
  std_strlprintf( def, sizeof(def)-1, "%d", QCRIL_DATA_PROPERTY_TIMEOUT_DEFAULT );

  ret = property_get( QCRIL_DATA_PROPERTY_TIMEOUT, args, def );

  if( (QCRIL_DATA_PROPERTY_TIMEOUT_SIZE) < ret )
  {
    /* unexpected property size, use default value */
    QCRIL_LOG_ERROR( "System property %s has unexpected size(%d)"
                     "using default value %d\n",
                     QCRIL_DATA_PROPERTY_TIMEOUT,
                     ret,
                     itimers.it_value.tv_sec );
  }
  else
  {
    ret = ds_atoi( args );
    if( QCRIL_DATA_PROPERTY_TIMEOUT_INVALID < ret )
    {
      /* Update timeout value using property */
      itimers.it_value.tv_sec = ret;
      QCRIL_LOG_VERBOSE("setup data call Timer overide specified, using value %d\n",
                         ret );
    }
    else
    {
      /* invalid property value, use default value */
      QCRIL_LOG_VERBOSE("Invalid data call Timer, using default value %d\n",
                         itimers.it_value.tv_sec);
    }
  }

  /* Start the timer */
  if (-1 == timer_settime(info_tbl_ptr->timer_id, 0, &itimers, NULL))
  {
    QCRIL_LOG_ERROR( "failed to start timer for timer_id [%#"PRIxPTR"], deleting... ",
                     (uintptr_t)info_tbl_ptr->timer_id);

    qcril_data_util_stop_timer(&info_tbl_ptr->timer_id);
  }
}

/*===========================================================================

  FUNCTION:  qcril_data_stop_data_call

===========================================================================*/
/*!
    @brief
    Performs the steps required to stop a data call.

    @pre Before calling, info_tbl_mutex must be locked by the calling thread

    @return
    None.
*/
/*=========================================================================*/
IxErrnoType qcril_data_stop_data_call
(
  qcril_data_call_info_tbl_type *info_tbl_ptr
)
{
  IxErrnoType ret = E_FAILURE;

  QCRIL_LOG_DEBUG( "%s", "call termination starting..." );

  if (!VALIDATE_LOCAL_DATA_OBJ(info_tbl_ptr) ||
      QCRIL_DSI_HNDL_INVALID == info_tbl_ptr->dsi_hndl)
  {
    QCRIL_LOG_ERROR( "%s", "invalid info_tbl_ptr" );
    return E_INVALID_ARG;
  }

  /* Stop the data call */
  if ( ( dsi_stop_data_call( info_tbl_ptr->dsi_hndl ) ) != DSI_SUCCESS )
  {
    QCRIL_LOG_ERROR( "unable to teardown data call, index [%d]", info_tbl_ptr->index );
  }
  else
  {
    /* Removed decrement qdp ref count to avoid in_use errors from modem
       It is done after NO_NET event is received */
    ret = E_SUCCESS;
  }

  return ret;
}


/*===========================================================================

  FUNCTION:  qcril_data_qmi_wds_init

===========================================================================*/
/*!
    @brief
    Initialize the QMI WDS and DSD services on a well known port for receiving
    a set of events

    @return
    none
*/
/*=========================================================================*/
void qcril_data_qmi_wds_init(boolean from_ssr)
{
  int rc;
  qmi_client_error_type        wds_client_err;
  wds_set_event_report_req_msg_v01 event_report_params;
  int ret = DSI_ERROR;
  qcril_modem_stack_id_e_type stack_id;

  QCRIL_LOG_DEBUG("%s","qcril_data_qmi_wds_init: entry");

  global_data_force_limited_sys_indication = qcril_data_get_force_limited_sys_ind();
  /* Fetch subs_id again during SSR initialization*/
  if(from_ssr)
  {
    /* If global_instance_id is a valid one, we expect wds_release to be
       called. Thus we ignore any additional wds_init() calls.
     */
    if(global_instance_id != QCRIL_MAX_INSTANCE_ID)
    {
      QCRIL_LOG_ERROR(" Ignoring duplicate WDS INIT in SSR, global ID = %d", global_instance_id);
      goto exit;
    }
    global_instance_id = qmi_ril_get_process_instance_id();

    dsi_set_ril_instance(global_instance_id);

    stack_id = qmi_ril_get_stack_id(global_instance_id);

    switch(stack_id)
    {
      case QCRIL_MODEM_PRIMARY_STACK_ID:
        global_subs_id = WDS_PRIMARY_SUBS_V01;
        break;

      case QCRIL_MODEM_SECONDARY_STACK_ID:
        global_subs_id = WDS_SECONDARY_SUBS_V01;
        break;

      case QCRIL_MODEM_TERTIARY_STACK_ID:
        global_subs_id = WDS_TERTIARY_SUBS_V01;
        break;

      default:
        global_subs_id = WDS_DEFAULT_SUBS_V01;
    }

    QCRIL_LOG_DEBUG( "qcril_data_qmi_wds_init: stack_id: %d, subs_id: %d",
                                              stack_id,global_subs_id);

    dsi_set_modem_subs_id(global_subs_id);
  }

  do
  {

    if(from_ssr)
    {
      if (DSI_SUCCESS != dsi_init(DSI_MODE_SSR))
      {
        QCRIL_LOG_ERROR("dsi_init in SSR failed");
      }
    }
    else
    {
      if (DSI_SUCCESS != dsi_init(DSI_MODE_GENERAL))
      {
        QCRIL_LOG_ERROR("dsi_init failed");
      }
    }

    global_qcril_qmi_wds_hndl = QCRIL_DATA_HNDL_INVALID;
    memset( &event_report_params, 0x0, sizeof(event_report_params) );

    if(QMI_NO_ERR != (rc = qcril_data_qmi_wds_init_qmi_client(default_qmi_port,
                                                              NULL,
                                                              global_subs_id,
                                                              &global_qcril_qmi_wds_hndl)))
    {
      QCRIL_LOG_ERROR("qcril_data_qmi_wds_init_qmi_client failed with rc [%d]", rc);
      break;
    }

    //Register error callback for QMI WDS client
    wds_client_err = qmi_client_register_error_cb(global_qcril_qmi_wds_hndl,
                                                  qcril_data_qmi_wds_err_cb,
                                                  NULL);
    if(wds_client_err != QMI_NO_ERR) {
      QCRIL_LOG_DEBUG("%s %d", "Error in registering error cb", (int)wds_client_err);
      return;
    }

    QCRIL_LOG_DEBUG("global qmi_wds_hndl [%p] initialized.", global_qcril_qmi_wds_hndl);

    /* Register for data call status indications */
    event_report_params.report_data_call_status_change_valid = TRUE;
    event_report_params.report_data_call_status_change = TRUE;

    if(E_SUCCESS != qcril_data_qmi_wds_set_event_report(global_qcril_qmi_wds_hndl,
                                                        &event_report_params))
    {
      QCRIL_LOG_ERROR("%s", "qcril_data_qmi_wds_set_event_report failed");
      break;
    }

#if (QCRIL_RIL_VERSION >= 15)
    /* Register for QMI_WDS_MODEM_ASSISTED_KA_STATUS_IND_V01 */
    qcril_data_qmi_wds_register_keep_alive_ind(TRUE);
#endif /*QCRIL_RIL_VERSION >= 15*/

    ret = DSI_SUCCESS;
  } while (0);

exit:

  if (DSI_SUCCESS == ret)
  {
    QCRIL_LOG_DEBUG("%s","qcril_data_qmi_wds_init: exit with success");
  }
  else
  {
    QCRIL_LOG_ERROR("%s","qcril_data_qmi_wds_init: exit with error");
  }
}

/*===========================================================================

  FUNCTION:  qcril_data_qmi_dsd_release

===========================================================================*/
/*!
    @brief
    releases QMI DSD resources.
    only used by QMI RIL and only during SSR

    @return
    none
*/
/*=========================================================================*/
static void qcril_data_qmi_dsd_release(void)
{
  qmi_client_error_type  rc;

  QCRIL_LOG_DEBUG("%s", "qcril_data_qmi_dsd_release: ENTRY");

  /* Release DSD handle */
  if (global_qmi_dsd_hndl != QCRIL_DATA_HNDL_INVALID) {
    rc = qmi_client_release(global_qmi_dsd_hndl);
    if (QMI_NO_ERR != rc) {
      QCRIL_LOG_ERROR("%s(): failed to release rc=%d!\n",__func__, rc);
    }
    global_qmi_dsd_hndl = QCRIL_DATA_HNDL_INVALID;
    QCRIL_LOG_DEBUG(".. DSD handle released");
  }

  QCRIL_LOG_DEBUG("%s", "qcril_data_qmi_dsd_release: EXIT");
}

/*===========================================================================

  FUNCTION:  qcril_data_qmi_wds_release

===========================================================================*/
/*!
    @brief
    releases QMI WDS resources.
    only used by QMI RIL and only during SSR

    @return
    none
*/
/*=========================================================================*/
void qcril_data_qmi_wds_release(void)
{
  qmi_client_error_type  rc;

  QCRIL_LOG_DEBUG("%s", "qcril_data_qmi_wds_release: ENTRY");

  /* Release the DSI library */
  if (DSI_SUCCESS != dsi_release(DSI_MODE_GENERAL))
  {
    QCRIL_LOG_ERROR("dsi_release failed");
  }

  qcril_data_set_pref_data_tech( global_instance_id, QCRIL_DATA_PREF_DATA_TECH_INVALID );

  /* Release WDS handle */
  if (global_qcril_qmi_wds_hndl != QCRIL_DATA_HNDL_INVALID)
  {
    rc = qmi_client_release(global_qcril_qmi_wds_hndl);
    qcril_data_qmi_wds_release_qmi_client(global_qcril_qmi_wds_hndl);
    if (QMI_NO_ERR != rc) {
      QCRIL_LOG_ERROR("%s(): failed to release rc=%d!\n",__func__, rc);
    }
    global_qcril_qmi_wds_hndl = QCRIL_DATA_HNDL_INVALID;
    QCRIL_LOG_DEBUG( ".. WDS handle released" );
  }

#ifdef RIL_REQUEST_SET_INITIAL_ATTACH_APN
  /* Try and release attach profile in case of SSR */
  if (QDP_INVALID_PROFILE != global_lte_attach_profile)
  {
    if ( QDP_SUCCESS != qdp_profile_release(global_lte_attach_profile) )
    {
      QCRIL_LOG_ERROR("%s", "LTE attach profile release failed because either the "
                      "profile is a modem profile or there was an internal "
                      "error in qdp");
    }

    global_lte_attach_profile = QDP_INVALID_PROFILE;
  }
#endif

  QCRIL_LOG_DEBUG("%s", "releasing QDP resources");
  qdp_deinit();

  /* Resetting Global Instance */
  global_instance_id = QCRIL_MAX_INSTANCE_ID;

  /* Resetting Global Instance */
  global_instance_id = QCRIL_MAX_INSTANCE_ID;

#if (QCRIL_RIL_VERSION >= 15)
  /* Notify RIL and clear modem assisted keepalive cache when service is lost */
  qcrili_data_ma_ka_cleanup(-1);
#endif

  QCRIL_LOG_DEBUG("%s", "qcril_data_qmi_wds_release: EXIT");
}

/*===========================================================================

  FUNCTION:  qcril_data_find_default_port

===========================================================================*/
/*!
    @brief
    Finds and sets default QMI port to be used.

    @return
    none
*/
/*=========================================================================*/
void qcril_data_set_default_port()
{
  ds_target_t target;
  const char *target_str;

  target = ds_get_target();
  target_str = ds_get_target_str(target);

  QCRIL_LOG_DEBUG("qcril_data_set_default_port(): target: [%d]: [%s]", target, target_str);

  do
  {
    if (DS_TARGET_MSM == target ||
        DS_TARGET_MSM8994 == target ||
        DS_TARGET_MSM8996 == target)
    {
      /* use smd/bam port */
      default_qmi_port = QMI_PORT_RMNET_0;
      QCRIL_LOG_DEBUG("default_qmi_port set to %s", default_qmi_port);
      global_modem_id = QCRIL_DEFAULT_MODEM_ID;
      ignore_ril_tech = TRUE;
    }
    else if (DS_TARGET_SVLTE1 == target ||
             DS_TARGET_SVLTE2 == target ||
             DS_TARGET_CSFB == target)
    {
      if (DS_TARGET_SVLTE2 == target)
      {
        /* ignore ril technology if svlte2 is the target */
        /* in nutshell, we are required to put this hack for certain
         * targets where 3gpp to 3gpp2 handover is possible, and hence
         * preferred ril technology shall be ignored for the profile
         * look up purposes (i.e. both 3gpp and 3gpp2 profiles shall
         * be looked up together regardless of the preferred technology */
        ignore_ril_tech = TRUE;
      }
      /* use sdio port */
      default_qmi_port = QMI_PORT_RMNET_SDIO_0;
      QCRIL_LOG_DEBUG("default_qmi_port set to %s", default_qmi_port);
      global_modem_id = QCRIL_SECOND_MODEM_ID;
    }
    else if (DS_TARGET_MDM == target)
    {
      ignore_ril_tech = TRUE; /* see above */

      /* use usb port */
      default_qmi_port = QMI_PORT_RMNET_USB_0;
      strlcpy( qcril_data_modem_port_map_tbl[QCRIL_SECOND_MODEM_ID].qmi_port,
               QMI_PORT_RMNET_USB_0,
               sizeof(qcril_data_modem_port_map_tbl[QCRIL_SECOND_MODEM_ID].qmi_port));
      QCRIL_LOG_DEBUG("default_qmi_port set to %s", default_qmi_port);
      global_modem_id = QCRIL_DEFAULT_MODEM_ID;
    }
    else if (DS_TARGET_FUSION4_5_PCIE == target ||
             DS_TARGET_FUSION_8084_9X45 == target )
    {
      ignore_ril_tech = TRUE; /* see above */

      /* use MHI port */
      default_qmi_port = QMI_PORT_RMNET_MHI_0;
      strlcpy( qcril_data_modem_port_map_tbl[QCRIL_SECOND_MODEM_ID].qmi_port,
               QMI_PORT_RMNET_MHI_0,
               sizeof(qcril_data_modem_port_map_tbl[QCRIL_SECOND_MODEM_ID].qmi_port));
      QCRIL_LOG_DEBUG("default_qmi_port set to %s", default_qmi_port);
      global_modem_id = QCRIL_DEFAULT_MODEM_ID;
    }
    else if (DS_TARGET_DSDA == target || DS_TARGET_SGLTE2 == target)
    {
      ignore_ril_tech = TRUE; /* see above */

      if (qmi_ril_get_process_instance_id() == QCRIL_DEFAULT_INSTANCE_ID)
      {
         /* use usb port */
         default_qmi_port = QMI_PORT_RMNET_USB_0;
         strlcpy( qcril_data_modem_port_map_tbl[QCRIL_SECOND_MODEM_ID].qmi_port,
                  QMI_PORT_RMNET_USB_0,
                  sizeof(qcril_data_modem_port_map_tbl[QCRIL_SECOND_MODEM_ID].qmi_port));
      }
      else
      {
         /* use smux port */
         default_qmi_port = QMI_PORT_RMNET_SMUX_0;
         strlcpy( qcril_data_modem_port_map_tbl[QCRIL_SECOND_MODEM_ID].qmi_port,
               QMI_PORT_RMNET_SMUX_0,
               sizeof(qcril_data_modem_port_map_tbl[QCRIL_SECOND_MODEM_ID].qmi_port));
      }
      QCRIL_LOG_DEBUG("default_qmi_port set to %s", default_qmi_port);
      global_modem_id = QCRIL_DEFAULT_MODEM_ID;
    }
    else if (DS_TARGET_DSDA2 == target)
    {
      ignore_ril_tech = TRUE; /* see above */

      if (qmi_ril_get_process_instance_id() == QCRIL_DEFAULT_INSTANCE_ID)
      {
         /* use HSIC port */
         default_qmi_port = QMI_PORT_RMNET_USB_0;
         strlcpy( qcril_data_modem_port_map_tbl[QCRIL_SECOND_MODEM_ID].qmi_port,
               QMI_PORT_RMNET_USB_0,
               sizeof(qcril_data_modem_port_map_tbl[QCRIL_SECOND_MODEM_ID].qmi_port));
      }
      else
      {
         /* use USB port corresponding to second modem */
         default_qmi_port = QMI_PORT_RMNET2_USB_0;
         strlcpy( qcril_data_modem_port_map_tbl[QCRIL_SECOND_MODEM_ID].qmi_port,
                  QMI_PORT_RMNET2_USB_0,
                  sizeof(qcril_data_modem_port_map_tbl[QCRIL_SECOND_MODEM_ID].qmi_port));
      }
      QCRIL_LOG_DEBUG("default_qmi_port set to %s", default_qmi_port);
      global_modem_id = QCRIL_DEFAULT_MODEM_ID;
    }
    else
    {
      /* do not set default_qmi_port for any thing else right now
       * as we don't know */
      QCRIL_LOG_DEBUG("default_qmi_port left as-is to %s", default_qmi_port);
    }

  } while (0);

  QCRIL_LOG_DEBUG("%s", "qcril_data_set_default_port: EXIT");
}


/*===========================================================================

  FUNCTION:  qcril_data_init

===========================================================================*/
/*!
    @brief
    Initialize the DATA subsystem of the RIL.

    (1) Call init routine of Data Services Internal Module

    @return
    None.
*/
/*=========================================================================*/
void qcril_data_init()
{
/*-----------------------------------------------------------------------*/
  int i = 0;
  qcril_modem_stack_id_e_type stack_id;
  pthread_mutexattr_t info_tbl_mutex_attr;

  // Always initialize the handle so
  // we can avoid re-intializing DSD client
  // again on SSR
  global_qmi_dsd_hndl = 0;

  QCRIL_LOG_DEBUG( "%s", "qcril_data_init: ENTRY" );

#ifdef FEATURE_WAIT_FOR_MODEM
  sleep(5);
#endif


  /* init dsi */
  dsi_set_ril_instance(global_instance_id);

  stack_id = qmi_ril_get_stack_id(global_instance_id);

  switch(stack_id)
  {
    case QCRIL_MODEM_PRIMARY_STACK_ID:
     global_subs_id = WDS_PRIMARY_SUBS_V01;
     break;

    case QCRIL_MODEM_SECONDARY_STACK_ID:
     global_subs_id = WDS_SECONDARY_SUBS_V01;
     break;

    case QCRIL_MODEM_TERTIARY_STACK_ID:
     global_subs_id = WDS_TERTIARY_SUBS_V01;
     break;

    default:
     global_subs_id = WDS_DEFAULT_SUBS_V01;
  }

  QCRIL_LOG_DEBUG( "qcril_data_init: stack_id: %d, subs_id: %d",
                      stack_id,global_subs_id);

  dsi_set_modem_subs_id(global_subs_id);

#ifdef FEATURE_QCRIL_FUSION
  /* use MDM for Fusion build */
  default_qmi_port = QMI_PORT_RMNET_SDIO_0;
#else
  /* for targets that are not 7630_fusion, use the ro.baseband
   * property to determine the modem_id, and qmi_port */
  qcril_data_set_default_port();
#endif

  /* initialize data_sys_status, it is used by each indication of
     preferred system change and freed in qcril_data_qmi_wds_release */
  memset( &global_dsd_info.legacy.data_sys_status,
          0x00,
          sizeof(global_dsd_info.legacy.data_sys_status));
  global_dsd_info.legacy.data_sys_status.network_info_len = QCRIL_NETWORK_INFO_LEN;

  qcril_data_qmi_wds_init(FALSE);
  qcril_data_qmi_dsd_init();
  (void)qcril_data_qmi_dpm_init();

  global_dsd_info.roaming = qcril_data_is_roaming();
  QCRIL_LOG_DEBUG("%s(): roaming status = %d", __func__, global_dsd_info.roaming);

#ifndef QCRIL_DATA_OFFTARGET
  qcril_data_util_update_partial_retry_enabled_flag();
#endif

  qcril_data_util_update_max_partial_retry_timeout();

  memset( info_tbl, 0, sizeof info_tbl );
  memset( dsi_cb_tbl, 0, sizeof dsi_cb_tbl );

  for( i = 0; i < MAX_CONCURRENT_UMTS_DATA_CALLS; i++ )
  {
    info_tbl[ i ].index = i;
    info_tbl[ i ].timer_id = QCRIL_DATA_INVALID_TIMERID;
    info_tbl[ i ].cid = CALL_ID_INVALID;
    info_tbl[ i ].dorm_status = WDS_DORMANCY_STATUS_DORMANT_V01;

    dsi_cb_tbl[i].instance_id = QCRIL_DATA_INVALID_INSTANCE_ID;
    dsi_cb_tbl[i].modem_id = QCRIL_DATA_INVALID_MODEM_ID;
    dsi_cb_tbl[i].dsi_hndl = QCRIL_DSI_HNDL_INVALID;

    QCRIL_LOG_VERBOSE("info_tbl[%d].index = %d", i, i);
  }/* for() */


  pthread_mutex_init(&dsi_cb_tbl_mutex, NULL);

  /* Get the default set of attributes */
  pthread_mutexattr_init(&info_tbl_mutex_attr);

  /* Set this mutex to be recursive i.e. the same thread can lock the
     mutex multiple times without being blocked.
     This is required because we can get the dsi callbacks for some
     dsi calls (e.g. dsi_stop_data_call) in the context of the calling
     qcril thread (which might hold the info_tbl_mutex before calling)
     and we could potentially reacquire the info_tbl_mutex
     in qcril_data_net_cb() and deadlock */
  pthread_mutexattr_settype(&info_tbl_mutex_attr, PTHREAD_MUTEX_RECURSIVE);

  pthread_mutex_init(&info_tbl_mutex, &info_tbl_mutex_attr);

  /* release resource */
  pthread_mutexattr_destroy(&info_tbl_mutex_attr);

#if (RIL_QCOM_VERSION >= 2)
  /* Initialize QOS support */
  qcril_data_qos_init();
#endif /* RIL_QCOM_VERSION >= 2 */

  QCRIL_LOG_DEBUG( "%s", "qcril_data_init: EXIT" );
} /* qcril_data_init() */

/*===========================================================================

  FUNCTION:  qcril_data_abort_incompatible_pending_calls

===========================================================================*/
/*!
    @brief
    Abort pending calls on older tech on a RAT change

    @return
    NONE
*/
/*=========================================================================*/
static void qcril_data_abort_incompatible_pending_calls
(
  wds_sys_network_info_type_v01 *nw_info
)
{
  int i;
  const char *str = NULL;

  if (!nw_info)
  {
    QCRIL_LOG_ERROR("Invalid parameter");
    return;
  }

  QCRIL_DATA_MUTEX_LOCK(&info_tbl_mutex);

  for (i = 0; i < MAX_CONCURRENT_UMTS_DATA_CALLS; ++i)
  {
    qcril_data_ril_radio_tech_t    numeric_ril_tech;
    qcril_data_call_info_tbl_type  *info_tbl_ptr = &info_tbl[i];

    if (!VALIDATE_LOCAL_DATA_OBJ(info_tbl_ptr)                ||
        QCRIL_EVT_RIL_REQUEST_SETUP_DATA_CALL != info_tbl_ptr->pend_req ||
        !info_tbl_ptr->call_params)
    {
      continue;
    }

    numeric_ril_tech = qcril_data_get_numeric_ril_technology(info_tbl_ptr->call_params->ril_tech);

    str = qcril_data_util_get_ril_tech_string(numeric_ril_tech);

    /* If RIL provided technology is 1x or EVDO and the new Modem reported
       technology is not 1x or EVDO, abort the call */
    if (QCRIL_DATA_IS_RIL_RADIO_TECH_CDMA_1X_EVDO(numeric_ril_tech) &&
        !QCRIL_DATA_IS_DATA_SYS_STATUS_RAT_MASK_3GPP2_1X_DO(nw_info->network,
                                                            nw_info->rat_mask,
                                                            nw_info->so_mask))
    {
      QCRIL_LOG_DEBUG("RIL requested technology=%s, Modem reported technology=!1x/DO",
                      str);
      qcril_data_stop_data_call(info_tbl_ptr);
    }
    /* If RIL provided technology is eHRPD and the new Modem reported
       technology is not eHRPD, abort the call */
    else if (QCRIL_DATA_IS_RIL_RADIO_TECH_EHRPD(numeric_ril_tech) &&
             !QCRIL_DATA_IS_DATA_SYS_STATUS_RAT_MASK_3GPP2_EHRPD(nw_info->network,
                                                                 nw_info->rat_mask,
                                                                 nw_info->so_mask))
    {
      QCRIL_LOG_DEBUG("RIL requested technology=%s, Modem reported technology=!eHRPD - aborting call",
                      str);
      qcril_data_stop_data_call(info_tbl_ptr);
    }
    else
    {
      QCRIL_LOG_DEBUG("RIL requested technology=%s, skipping abort", str);
    }
  }

  QCRIL_DATA_MUTEX_UNLOCK(&info_tbl_mutex);
} /*qcril_data_abort_incompatible_pending_calls() */

/*===========================================================================

  FUNCTION:  qcril_data_update_mtu

===========================================================================*/
/*!
    @brief
    Changes MTU value on all active calls
    @return
    NONE
*/

void qcril_data_update_mtu
(
  unsigned int mtu
)
{
  int i;
  int dev_instance;

  qcril_data_mtu = mtu;

  if (qcril_data_mtu > 0)
  {
    /* Looking for all active calls*/
    for (i = 0; i < MAX_CONCURRENT_UMTS_DATA_CALLS; i++)
    {
      GET_DEV_INSTANCE_FROM_NAME(i, dev_instance);
      if (dev_instance < 0 ||
          dev_instance >= QCRIL_DATA_MAX_DEVS ||
          !VALIDATE_LOCAL_DATA_OBJ(&info_tbl[i]) ||
          info_tbl[i].cid == CALL_ID_INVALID)
      {
        continue;
      }

      /* Check if dsi_hdnl is valid and set MTU */
      if (info_tbl[i].dsi_hndl != NULL)
      {
        QCRIL_LOG_DEBUG("Changing default MTU value to [%d] for Call [%d]",
                        mtu, info_tbl[i].dsi_hndl);
        qcril_data_iface_set_mtu(info_tbl[i].dsi_hndl, mtu);

        /* Post a MTU update event */
        qcril_data_post_dsi_netctrl_event(info_tbl[i].instance_id,
                                          info_tbl[i].modem_id,
                                          info_tbl[i].pend_tok,
                                          &info_tbl[i],
                                          DSI_EVT_NET_NEWMTU,
                                          NULL);
      }
    } /* for */
  }
}
void qcril_data_process_stack_switch
(
  qcril_modem_stack_id_e_type old_stack_id,
  qcril_modem_stack_id_e_type new_stack_id,
  qcril_instance_id_e_type instance_id
)
{
 qcril_data_cmd_data_t *cmd_data;

 QCRIL_LOG_VERBOSE( "%s", "qcril_data_process_stack_switch: ENTRY" );

 if(old_stack_id != new_stack_id)
 {
   QCRIL_LOG_DEBUG("Stack ID changed from %d to %d",
     old_stack_id,new_stack_id);
 }
 else
 {
   goto bail;
 }

 QCRIL_DATA_ALLOC_STORAGE(cmd_data, sizeof(qcril_data_cmd_data_t), bail);

 cmd_data->old_stack_id = old_stack_id;
 cmd_data->new_stack_id = new_stack_id;
 cmd_data->self = cmd_data;
 cmd_data->cmd_id = QCRIL_DATA_STACK_SWITCH_CMD;

 if(E_SUCCESS != qcril_event_queue( instance_id,
                                      global_modem_id,
                                      QCRIL_DATA_NOT_ON_STACK,
                                      QCRIL_EVT_DATA_COMMAND_CALLBACK,
                                      ( void * )cmd_data,
                                      sizeof( qcril_data_cmd_data_t ),
                                      (RIL_Token) QCRIL_TOKEN_ID_INTERNAL ) )
 {
   QCRIL_LOG_ERROR("%s", "qcril_event_queue failed\n");
   goto bail;
 }

bail:

 QCRIL_LOG_VERBOSE( "%s", "qcril_data_process_stack_switch: EXIT" );
}

/*===========================================================================

  FUNCTION:   qcril_data_set_rat_preference

===========================================================================*/
/*!
    @brief
    This function sets the RAT preference

    @return
    RIL_E_SUCCESS         :- If the modem accepts async request
    RIL_E_GENERIC_FAILURE :- If modem rejects the async request
*/
/*=========================================================================*/

RIL_Errno qcril_data_set_rat_preference
(
  int rat_pref_type
)
{
  qmi_client_error_type rc;
  dsd_get_data_settings_resp_msg_v01 *data_setting_get_rat_pref_param_resp = NULL;
  qcril_data_cb_struct *cb_data = NULL;
  qmi_txn_handle txn_handle;
  RIL_Errno call_fail_cause = RIL_E_SUCCESS;

  if(global_qmi_dsd_hndl == NULL)
  {
    QCRIL_LOG_ERROR("%s","DSD Client unavailable");
    return RIL_E_OEM_ERROR_1;
  }

  if(rat_pref_type >= DSD_RAT_PREFERENCE_INACTIVE_V01
       && rat_pref_type <= DSD_RAT_PREFERENCE_WIFI_PREFERRED_V01)
  {
    QCRIL_LOG_DEBUG("RAT pref to set is [%d]",rat_pref_type);
  }
  else
  {
    QCRIL_LOG_ERROR("Invalid RAT pref [%d]",rat_pref_type);
    return RIL_E_INVALID_ARGUMENTS;
  }

  cb_data = (qcril_data_cb_struct *)
           malloc(sizeof(qcril_data_cb_struct));

  data_setting_get_rat_pref_param_resp = (dsd_get_data_settings_resp_msg_v01*)
                           malloc(sizeof(dsd_get_data_settings_resp_msg_v01));

  if(!data_setting_get_rat_pref_param_resp || !cb_data)
  {
    QCRIL_LOG_ERROR("Unable to allocate memory");
    call_fail_cause = RIL_E_NO_MEMORY;
    goto bail;
  }

  memset(cb_data,0,sizeof(qcril_data_cb_struct));

  memset(data_setting_get_rat_pref_param_resp, 0,
                            sizeof(dsd_get_data_settings_resp_msg_v01));

  cb_data->calling_func = GET_RAT_PREF;
  cb_data->rat_pref = rat_pref_type;

  //Intitialised to INVALID pref
  cb_data->prev_rat_pref = -1;

  cb_data->msg_id = QMI_DSD_GET_DATA_SETTING_REQ_V01;
  cb_data->self = (void *)cb_data;
  QCRIL_LOG_DEBUG("Debug : self [%u] msg_id [%d] calling_func [%d]", cb_data->self, cb_data->msg_id, cb_data->calling_func);
  QCRIL_LOG_DEBUG("Debug : rat_pref [%d] ", cb_data->rat_pref);

  rc = qmi_client_send_msg_async(global_qmi_dsd_hndl,
                                 QMI_DSD_GET_DATA_SETTING_REQ_V01,
                                 NULL,
                                 0,
                                 data_setting_get_rat_pref_param_resp,
                                 sizeof(dsd_get_data_settings_resp_msg_v01),
                                 qcril_data_async_cb,
                                 (void *)cb_data,
                                 &txn_handle);
  if (QMI_NO_ERR != rc) {
      QCRIL_LOG_ERROR("failed to send QMI message, err=%d",
                      rc);
      call_fail_cause = RIL_E_OEM_ERROR_2;
      goto bail;
  }

  return RIL_E_SUCCESS;

  bail:
  QCRIL_DATA_RELEASE_STORAGE(cb_data);
  QCRIL_DATA_RELEASE_STORAGE(data_setting_get_rat_pref_param_resp);

  return call_fail_cause;

}

/*===========================================================================

  FUNCTION:   qcril_data_set_quality_measurement_info

===========================================================================*/
/*!
    @brief
    This function configures the Modem with RAT specific quailiy measurement
    parameters

    @return
    None

*/
/*=========================================================================*/
qmi_response_type_v01 qcril_data_set_quality_measurement
(
  dsd_set_quality_measurement_info_req_msg_v01 *info
)
{

  dsd_set_quality_measurement_info_resp_msg_v01  resp;
  qmi_response_type_v01                          qcril_resp;
  int rc;

  memset(&qcril_resp, 0, sizeof(qcril_resp));
  qcril_resp.result = QMI_RESULT_FAILURE_V01;
  qcril_resp.error = QMI_ERR_INTERNAL_V01;

  if (!info)
  {
    QCRIL_LOG_ERROR("%s","invalid input");
    goto bail;
  }

  if(global_qmi_dsd_hndl == NULL)
  {
    QCRIL_LOG_ERROR("%s","DSD Client unavailable");
    goto bail;
  }

  memset(&resp, 0, sizeof(resp));

  rc = qmi_client_send_msg_sync(global_qmi_dsd_hndl,
                                QMI_DSD_SET_QUALITY_MEASUREMENT_INFO_REQ_V01,
                                info,
                                sizeof(dsd_set_quality_measurement_info_req_msg_v01),
                                &resp,
                                sizeof(resp),
                                QCRIL_DATA_QMI_DSD_SYNC_MSG_TIMEOUT);

  if (QMI_NO_ERR != rc)
  {
    QCRIL_LOG_ERROR("failed to send QMI msg rc=%d",rc);
    goto bail;
  }
  else
  {
    qcril_resp = resp.resp;
  }

bail:
  QCRIL_LOG_DEBUG("qcril_data_set_quality_measurement(): result=%d err=%d",
                  qcril_resp.result,
                  qcril_resp.error);
  return qcril_resp;
}

/*===========================================================================

  FUNCTION:  qcril_data_get_dds_sub_info

===========================================================================*/
/*!
    @brief
    Query the current DDS sub ID from modem

    @pre None

    @return
      if modem query is successful then returns sub id
      else returns -1
*/
/*=========================================================================*/
DDSSubIdInfo qcril_data_get_dds_sub_info(void)
{
  dsd_get_current_dds_req_msg_v01 req;
  dsd_get_current_dds_resp_msg_v01 resp;
  DDSSubIdInfo ddsInfo;
  ddsInfo.dds_sub_id = QCRIL_INVALID_MODEM_STACK_ID;
  ddsInfo.switch_type = DSD_DDS_DURATION_ENUM_MIN_ENUM_VAL_V01;

  int rc = -1;

  if(global_qmi_dsd_hndl == NULL)
  {
    QCRIL_LOG_ERROR("%s","DSD Client unavailable");
    goto bail;
  }

  memset(&req, 0, sizeof(req));
  memset(&resp, 0, sizeof(resp));

  rc = qmi_client_send_msg_sync(global_qmi_dsd_hndl,
                                QMI_DSD_GET_CURRENT_DDS_REQ_V01,
                                &req,
                                sizeof(req),
                                &resp,
                                sizeof(resp),
                                QCRIL_DATA_QMI_DSD_SYNC_MSG_TIMEOUT);

  if (QMI_NO_ERR != rc)
  {
    QCRIL_LOG_ERROR("failed to send QMI msg rc=%d",rc);
    goto bail;
  }
  else
  {
    if (resp.dds_valid) {
      ddsInfo.dds_sub_id = (int) qcril_data_dsd_to_ril_sub_id(resp.dds);
      if(resp.dds_switch_type_valid) {
        ddsInfo.switch_type = qcril_data_dsd_to_ril_switch_type(resp.dds_switch_type);
      } else {
        ddsInfo.switch_type = DSD_DDS_DURATION_PERMANANT_V01; //default value
      }
    } else {
      QCRIL_LOG_ERROR("dds_valid is not set!");
    }
  }

bail:
  return ddsInfo;
}

/*===========================================================================

  FUNCTION:  qcril_data_dsd_to_ril_sub_id

===========================================================================*/
/*!
    @brief
    Converts QMI DSD sub_id to RIL sub_id

    @pre None

    @return
      RIL_SubscriptionType
*/
/*=========================================================================*/
static RIL_SubscriptionType qcril_data_dsd_to_ril_sub_id
(
  const dsd_bind_subscription_enum_v01 dsd_sub_id
)
{
  switch(dsd_sub_id) {
    case DSD_PRIMARY_SUBS_V01:
      return RIL_SUBSCRIPTION_1;
    case DSD_SECONDARY_SUBS_V01:
      return RIL_SUBSCRIPTION_2;
    case DSD_TERTIARY_SUBS_V01:
      return RIL_SUBSCRIPTION_3;
    default:
      return RIL_SUBSCRIPTION_1;
  }
}

static dsd_dds_switch_type qcril_data_dsd_to_ril_switch_type
(
const dsd_dds_switch_type_enum_v01 dds_switch_type
)
{
  switch(dds_switch_type) {
    case DSD_DDS_SWITCH_PERMANENT_V01:
      return DSD_DDS_DURATION_PERMANANT_V01;
    case DSD_DDS_SWITCH_TEMPORARY_V01:
      return DSD_DDS_DURATION_TEMPORARY_V01;
    default:
      return DSD_DDS_DURATION_PERMANANT_V01;
  }
}
/*===========================================================================

  FUNCTION:  qcril_data_ril_to_dsd_sub_id

===========================================================================*/
/*!
    @brief
    Converts RIL sub_id to QMI DSD sub_id

    @pre None

    @return
      dsd_bind_subscription_enum_v01
*/
/*=========================================================================*/
static dsd_bind_subscription_enum_v01 qcril_data_ril_to_dsd_sub_id
(
  const RIL_SubscriptionType ril_sub_id
)
{
  switch(ril_sub_id) {
    case RIL_SUBSCRIPTION_1:
      return DSD_PRIMARY_SUBS_V01;
    case RIL_SUBSCRIPTION_2:
      return DSD_SECONDARY_SUBS_V01;
    case RIL_SUBSCRIPTION_3:
      return DSD_TERTIARY_SUBS_V01;
    default:
      return DSD_PRIMARY_SUBS_V01;
  }
}

static dsd_dds_switch_type_enum_v01 qcril_data_ril_to_dsd_switch_type
(
const dsd_dds_switch_type dds_switch_type
)
{
  switch(dds_switch_type) {
    case DSD_DDS_DURATION_PERMANANT_V01:
      return DSD_DDS_SWITCH_PERMANENT_V01;
    case DSD_DDS_DURATION_TEMPORARY_V01:
      return DSD_DDS_SWITCH_TEMPORARY_V01;
    default:
      return DSD_DDS_SWITCH_PERMANENT_V01;
  }
}
/*===========================================================================

  FUNCTION:  qcril_data_request_dds_switch

===========================================================================*/
/*!
    @brief
    Request DDS switch

    @pre None

    @return RIL_Errno

*/
/*=========================================================================*/
RIL_Errno qcril_data_request_dds_switch(DDSSubIdInfo dds_info)
{
  int ret = RIL_E_INTERNAL_ERR;
  dsd_switch_dds_req_msg_v01 req;
  dsd_switch_dds_resp_msg_v01 resp;

  if (dds_info.dds_sub_id < 0) {
    QCRIL_LOG_ERROR("invalid sub id %d received", dds_info.dds_sub_id);
    ret = RIL_E_INVALID_ARGUMENTS;
    goto bail;
  }

  if (global_qmi_dsd_hndl == NULL) {
    QCRIL_LOG_ERROR("DSD Client unavailable");
    goto bail;
  }

  memset(&req, 0, sizeof(req));
  memset(&resp, 0, sizeof(resp));
  req.subscription = qcril_data_ril_to_dsd_sub_id(dds_info.dds_sub_id);
  req.dds_switch_type = qcril_data_ril_to_dsd_switch_type(dds_info.switch_type);
  req.dds_switch_type_valid = 1;
  int rc = qmi_client_send_msg_sync(global_qmi_dsd_hndl,
                            QMI_DSD_SWITCH_DDS_REQ_V01,
                            &req,
                            sizeof(req),
                            &resp,
                            sizeof(resp),
                            QCRIL_DATA_QMI_DSD_SYNC_MSG_TIMEOUT);
  if (QMI_NO_ERR != rc) {
    QCRIL_LOG_ERROR("failed to send QMI msg rc=%d",rc);
    goto bail;
  }

  if (resp.resp.result == QMI_RESULT_FAILURE_V01) {
    QCRIL_LOG_ERROR("Request failed due to reason=%d", resp.resp.error);
    goto bail;
  }

  ret = RIL_E_SUCCESS;

bail:
  return ret;
}

/*===========================================================================

FUNCTION:  qcril_data_qmi_dsd_client_init

===========================================================================*/
/*!
    @brief
  Initialize the QMI DSD service client
    @return
    none
*/
/*=========================================================================*/
void qcril_data_qmi_dsd_client_init(void)
{
  qmi_idl_service_object_type  dsd_svc_obj;
  qmi_client_error_type        rc;

  dsd_bind_subscription_req_msg_v01    bind_req_msg;
  dsd_bind_subscription_resp_msg_v01   bind_resp_msg;
  dsd_indication_register_req_msg_v01  ind_reg_req;
  dsd_indication_register_resp_msg_v01 ind_reg_resp;

  if (NULL != global_qmi_dsd_hndl) {
    qmi_client_release(global_qmi_dsd_hndl);
    global_qmi_dsd_hndl = NULL;
  }

  // Init DSD client
  dsd_svc_obj = dsd_get_service_object_v01();
  if (NULL == dsd_svc_obj) {
    QCRIL_LOG_ERROR("dsd_get_service_object failed");
    return;
  }

  memset(&dsd_os_params, 0, sizeof(dsd_os_params));
  rc = qmi_client_init_instance(dsd_svc_obj,
                  QMI_CLIENT_INSTANCE_ANY,
                  (qmi_client_ind_cb) qcril_data_qmi_dsd_ind_cb,
                  NULL,
                  &dsd_os_params,
                  QCRIL_QMI_DSD_TIMEOUT,
                  &global_qmi_dsd_hndl);

  if (QMI_NO_ERR != rc) {
    QCRIL_LOG_ERROR("Failed to allocate DSD client, rc=%d", rc);
    return;
  }

  // Bind subscription
  memset(&bind_req_msg, 0, sizeof(bind_req_msg));
  memset(&bind_resp_msg, 0, sizeof(bind_resp_msg));

  bind_req_msg.bind_subs = qcril_data_get_qmi_dsd_subscription_id();

  QCRIL_LOG_DEBUG("binding subscription to subs=%d",
                  bind_req_msg.bind_subs);

  rc = qmi_client_send_msg_sync(global_qmi_dsd_hndl,
                                QMI_DSD_BIND_SUBSCRIPTION_REQ_V01,
                                &bind_req_msg,
                                sizeof(bind_req_msg),
                                &bind_resp_msg,
                                sizeof(bind_resp_msg),
                                QCRIL_DATA_QMI_DSD_SYNC_MSG_TIMEOUT);

  if (QMI_NO_ERR != rc ||
      QMI_RESULT_SUCCESS_V01 != bind_resp_msg.resp.result) {
    QCRIL_LOG_ERROR("failed to bind subscription, rc=%d, err=%d",
                    rc, bind_resp_msg.resp.error);

  }

  // Register indications
  memset(&ind_reg_req, 0, sizeof(ind_reg_req));
  memset(&ind_reg_resp, 0, sizeof(ind_reg_resp));

  ind_reg_req.report_current_dds_valid = 1;
  ind_reg_req.report_current_dds = 1;
  ind_reg_req.report_roaming_status_change_valid = 1;
  ind_reg_req.report_roaming_status_change = 1;

  rc = qmi_client_send_msg_sync(global_qmi_dsd_hndl,
                                QMI_DSD_INDICATION_REGISTER_REQ_V01,
                                &ind_reg_req,
                                sizeof(ind_reg_req),
                                &ind_reg_resp,
                                sizeof(ind_reg_resp),
                                QCRIL_DATA_QMI_DSD_SYNC_MSG_TIMEOUT);

  if (QMI_NO_ERR != rc ||
      QMI_RESULT_SUCCESS_V01 != ind_reg_resp.resp.result) {
    QCRIL_LOG_ERROR("failed to register DDS switch ind, rc=%d, err=%d",
                    rc, ind_reg_resp.resp.error);
  }

    rc = qmi_client_register_error_cb(global_qmi_dsd_hndl,
                                    qcril_data_qmi_dsd_err_cb,
                                      (void *)default_qmi_port);
  if (QMI_NO_ERR != rc) {
      QCRIL_LOG_ERROR("Failed to qmi_client_register_error_cb, error = %d", rc);
  }

  QCRIL_LOG_DEBUG("Done DSD client init");
}

/*===========================================================================

  FUNCTION:  qcril_data_qmi_dsd_init

===========================================================================*/
/*!
    @brief
    Initialize the QMI DSD services

    @return
    none
*/
/*=========================================================================*/
static void qcril_data_qmi_dsd_init(void)
{
  int rc;
  int ret = DSI_ERROR;
  char args[PROPERTY_VALUE_MAX];

  do {
    /*Initialize QCCI dsd client to send recieve mesages */
    if (global_qmi_dsd_hndl == NULL) {
      qcril_data_qmi_dsd_client_init();
    } else {
      QCRIL_LOG_DEBUG(" DSD already initialized, skipping...");
    }

    if (NULL != global_qmi_dsd_hndl)
    {
      dsd_get_system_status_resp_msg_v01  sys_resp_msg;
      dsd_system_status_ind_msg_v01 *ind_data = NULL;

      dsd_system_status_change_req_msg_v01 sys_reg_req_msg;
      dsd_system_status_change_resp_msg_v01 sys_reg_resp_msg;

      QCRIL_LOG_DEBUG("%s", "Using QMI-DSD indication for data system status updates");

      /* Query the current system status from QMI-DSD service */
      memset(&sys_resp_msg, 0, sizeof(sys_resp_msg));

      rc = qmi_client_send_msg_sync( global_qmi_dsd_hndl,
                                     QMI_DSD_GET_SYSTEM_STATUS_REQ_V01,
                                     NULL,
                                     0,
                                     &sys_resp_msg,
                                     sizeof(sys_resp_msg),
                                     QCRIL_DATA_QMI_DSD_SYNC_MSG_TIMEOUT);

      if (QMI_NO_ERR != rc) {
        QCRIL_LOG_ERROR("failed to send qmi_dsd_get_system_status, err=%d",rc);
        break;
      }
      else if (QMI_NO_ERR != sys_resp_msg.resp.result) {
        QCRIL_LOG_ERROR("failed to send qmi_dsd_get_system_status, err=%d",
                                     sys_resp_msg.resp.error);
        break;
      }

      /* Process the system status response */
      ind_data = (dsd_system_status_ind_msg_v01 *)((char *)&sys_resp_msg +
                                                  offsetof(dsd_get_system_status_resp_msg_v01,
                                                  avail_sys_valid));
      qcril_data_process_qmi_dsd_ind(ind_data);

      /* Register for the QMI-DSD indications */
      memset(&sys_reg_req_msg, 0, sizeof(sys_reg_req_msg));
      memset(&sys_reg_resp_msg, 0, sizeof(sys_reg_resp_msg));

      rc = qmi_client_send_msg_sync( global_qmi_dsd_hndl,
                                     QMI_DSD_SYSTEM_STATUS_CHANGE_REQ_V01,
                                     &sys_reg_req_msg,
                                     sizeof(sys_reg_req_msg),
                                     &sys_reg_resp_msg,
                                     sizeof(sys_reg_resp_msg),
                                     QCRIL_DATA_QMI_DSD_SYNC_MSG_TIMEOUT);

      if (QMI_NO_ERR != rc) {
        QCRIL_LOG_ERROR("failed to send qmi_dsd_reg_system_status_ind, err=%d",rc);
        break;
      }
      else if (QMI_NO_ERR != sys_reg_resp_msg.resp.result) {
        QCRIL_LOG_ERROR("failed to send qmi_dsd_reg_system_status_ind, err=%d",
                                     sys_reg_resp_msg.resp.error);
        break;
      }
    }

    /* init qdp */
    qdp_init(default_qmi_port);

    memset(args, 0, sizeof(args));
    property_get("persist.data.dont_use_epc", args, "true");
    if (0 == strcmp(args, "true"))
    {
      QCRIL_LOG_DEBUG("%s", "persist.data.dont_use_epc = true");
      epc_profile_supported = FALSE;
    }

    qdp_set_subscription(global_subs_id);

#ifdef FEATURE_DATA_LQE
    QCRIL_LOG_DEBUG("%s", "initializing LQE");
    qcril_data_lqe_init(global_instance_id, global_subs_id);
#endif /* FEATURE_DATA_LQE */
    ret = DSI_SUCCESS;
    //qcril_data_reg_for_svc_notif();
  }while (0);

  if (DSI_SUCCESS == ret)
  {
    QCRIL_LOG_DEBUG("%s","qcril_data_qmi_dsd_init: exit with success");
  }
  else
  {
    QCRIL_LOG_ERROR("%s","qcril_data_qmi_dsd_init: exit with error");
  }
}

/*===========================================================================

  FUNCTION:  qcril_data_qmi_dpm_init

===========================================================================*/
/**
    @brief
    Initializes the QMI DPM module.

    @details
    Performs the QCCI client library initialization to be able to send
    and receive DPM messages.

    @params None.
    @return SUCCESS: in case of success.
    @return FAILURE: in case of error.
    @dependencies None.
*/
static int qcril_data_qmi_dpm_init(void)
{
  qmi_idl_service_object_type  dpm_svc_obj;
  qmi_client_error_type        qmi_err = QMI_NO_ERR;
  dpm_get_supported_capabilities_req_msg_v01 cap_req;
  dpm_get_supported_capabilities_resp_msg_v01 cap_resp;

  global_qcril_clat_supported = FALSE;

  if (NULL != global_qmi_dpm_hndl) {
    qmi_client_release(global_qmi_dpm_hndl);
    global_qmi_dpm_hndl = NULL;
  }

  /* Initialize QCCI client to send the capability request */
  dpm_svc_obj = dpm_get_service_object_v01();
  if (dpm_svc_obj == NULL)
  {
    QCRIL_LOG_ERROR("%s(): DPM service not available", __func__);
    return FAILURE;
  }

  memset(&dpm_os_params, 0, sizeof(dpm_os_params));
  qmi_err = qmi_client_init_instance(dpm_svc_obj,
                                QMI_CLIENT_INSTANCE_ANY,
                                NULL,
                                NULL,
                                &dpm_os_params,
                                QCRIL_QMI_DPM_TIMEOUT,
                                &global_qmi_dpm_hndl);

  if (qmi_err != QMI_NO_ERR)
  {
    QCRIL_LOG_ERROR("%s(): qmi_client_init_instance failed, err %d\n",
                    __func__, qmi_err);
    return FAILURE;
  }

  QCRIL_LOG_DEBUG("qcril_data_qmi_dpm_init(): qmi_client_init_instance " \
      "SUCCESS, client: 0x%x\n", (unsigned int)global_qmi_dpm_hndl);

  /* Query capabilities */
  memset( &cap_req,
          0x0,
          sizeof(dpm_get_supported_capabilities_req_msg_v01));
  memset( &cap_resp,
          0x0,
          sizeof(dpm_get_supported_capabilities_resp_msg_v01));

  qmi_err = qmi_client_send_msg_sync
            ( global_qmi_dpm_hndl,
              QMI_DPM_GET_SUPPORTED_CAPABILITIES_REQ_V01,
              (void*) &cap_req,
              sizeof(dpm_get_supported_capabilities_req_msg_v01),
              (void*) &cap_resp,
              sizeof(dpm_get_supported_capabilities_resp_msg_v01),
              QCRIL_DATA_QMI_DPM_SYNC_MSG_TIMEOUT );
  if (QMI_NO_ERR != qmi_err)
  {
    QCRIL_LOG_ERROR("%s: error getting supported capabilities qmi_err[%d]",
                   __func__, qmi_err);
    (void) qmi_client_release(global_qmi_dpm_hndl);
    return FAILURE;
  }

  if (cap_resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QCRIL_LOG_ERROR("%s(): Query capability failed with result [%d], " \
                    "err [0x%x]",__func__,
                    cap_resp.resp.result,
                    cap_resp.resp.error);
    (void) qmi_client_release(global_qmi_dpm_hndl);
    return FAILURE;
  }

  if (TRUE == cap_resp.is_clat_supported_valid &&
      TRUE == cap_resp.is_clat_supported)
  {
    global_qcril_clat_supported = TRUE;
    QCRIL_LOG_DEBUG("%s(): Modem xlat supported", __func__);
  }
  else
  {
    global_qcril_clat_supported = FALSE;
    QCRIL_LOG_DEBUG("%s(): Modem xlat not supported", __func__);
  }

  return SUCCESS;
}

/*===========================================================================

  FUNCTION:  qcril_data_is_roaming

===========================================================================*/
/*!
    @brief
    Get roaming status from DSD service

    @return
    TRUE if roaming
*/
/*=========================================================================*/
boolean qcril_data_is_roaming()
{
  if (global_qmi_dsd_hndl == NULL) {
    QCRIL_LOG_ERROR("dsd handle unavailable");
    return FALSE;
  }

  dsd_get_current_roaming_status_info_req_msg_v01 req;
  dsd_get_current_roaming_status_info_resp_msg_v01 resp;
  memset(&resp, 0, sizeof(dsd_get_current_roaming_status_info_resp_msg_v01));

  int rc = qmi_client_send_msg_sync(global_qmi_dsd_hndl,
                                QMI_DSD_GET_CURRENT_ROAMING_STATUS_INFO_REQ_V01,
                                &req,
                                sizeof(req),
                                &resp,
                                sizeof(resp),
                                QCRIL_DATA_QMI_DSD_SYNC_MSG_TIMEOUT);

  if (QMI_NO_ERR != rc)
  {
    QCRIL_LOG_ERROR("failed to send QMI msg rc=%d, err=%d", rc, resp.resp.error);
    return FALSE;
  }

  if (resp.roaming_status_valid) {
    if (resp.roaming_status) {
      return TRUE;
    } else {
      return FALSE;
    }
  }
  return FALSE;
}

/*===========================================================================

  FUNCTION:  qcril_data_get_dsi_ip_family

===========================================================================*/
/*!
    @brief
    Get DSI family parameter based on IP family type from RIL

    @return
    DSI_IP_FAMILY_V4 or DSI_IP_FAMILY_V6
*/
/*=========================================================================*/
dsi_ip_family_t qcril_data_get_dsi_ip_family(char *ip_family, size_t size)
{
  if (ip_family == NULL) {
    QCRIL_LOG_ERROR("No IP Family provided, default to DSI_IP_FAMILY_V4");
    return DSI_IP_FAMILY_V4;
  }
  if (strncmp(ip_family, QCRIL_DATA_IP_FAMILY_V4, size) == 0) {
    QCRIL_LOG_DEBUG("%s --> DSI_IP_FAMILY_V4", ip_family);
    return DSI_IP_FAMILY_V4;
  } else if (strncmp(ip_family, QCRIL_DATA_IP_FAMILY_V6, size) == 0) {
    QCRIL_LOG_DEBUG("%s --> DSI_IP_FAMILY_V6", ip_family);
    return DSI_IP_FAMILY_V6;
  }

  QCRIL_LOG_DEBUG("%s --> DSI_IP_FAMILY_V4", ip_family);

  /* DSI type only supports V4 or V6 */
  return DSI_IP_FAMILY_V4;
}

/*===========================================================================

  FUNCTION:  qcril_data_pick_temporary_call_end_reason

===========================================================================*/
/*!
    @brief
    Pick temporary cause code given two cause codes.
    If both are temporary then the first will be picked.
    if both are permanent, then the second will be picked.

    @return
    none
*/
/*=========================================================================*/
static void qcril_data_pick_temporary_call_end_reason
(
  dsi_ce_reason_t *first,
  dsi_ce_reason_t *second,
  dsi_ce_reason_t *result
)
{
  QCRIL_LOG_DEBUG( "%s", "qcril_data_pick_temporary_call_end_reason: ENTRY" );
  if ((result == NULL) || (first == NULL) || (second == NULL)) {
    QCRIL_LOG_ERROR("Unexpected NULL param received");
    return;
  }

  int ril_ce_code = PDP_FAIL_OEM_DCFAILCAUSE_1;
  if ((qcril_data_get_ril_ce_code(first,&ril_ce_code) == SUCCESS) &&
    !qcril_data_util_is_ce_failure_permanent(ril_ce_code)) {

    result->reason_type = first->reason_type;
    result->reason_code = first->reason_code;
    QCRIL_LOG_DEBUG("Picking first reason type = %d, code = %d",
      result->reason_type, result->reason_code);
  } else {
    result->reason_type = second->reason_type;
    result->reason_code = second->reason_code;
    QCRIL_LOG_DEBUG("Picking second reason type = %d, code = %d",
      result->reason_type, result->reason_code);
  }
}
