/*!
  @file
  qcril_data_utils.c

  @brief
  Handles utility functions of RIL requests for DATA services.

*/

/*===========================================================================

    Copyright (c) 2011-2016, 2018 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/17/11   sy     Initial version

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <netinet/in.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include <cutils/properties.h>
#include <ctype.h>
#include <arpa/inet.h>

#include "qcril_data.h"
#include "qcril_data_defs.h"
#include "qcril_data_utils.h"
#include "qcril_log.h"
#include <linux/if.h>

/* Retry timer values (in msec) */
#define RIL_RETRY_INVALID (-1)

#define QCRIL_DATA_UTIL_DEFAULT_PARTIAL_RETRY_TIMEOUT  (5)
#define QCRIL_DATA_UTIL_DEFAULT_MAX_PARTIAL_RETRY_TIMEOUT    (100)

#define QCRIL_DATA_PROPERTY_MAX_PARTIAL_RETRY_TIMEOUT  "persist.vendor.radio.max_retry_timeout"

/* Property to disable/enable partial retry */
#define QCRIL_DATA_PROPERTY_DISABLE_PARTIAL_RETRY          "persist.vendor.radio.disable_retry"
#define QCRIL_DATA_PROPERTY_DISABLE_PARTIAL_RETRY_DEFAULT  "false"
#define QCRIL_DATA_PROPERTY_DISABLE_PARTIAL_RETRY_SIZE     (6)

#define QCRIL_DATA_UTIL_GET_PARTIAL_RETRY_TIMEOUT(num_retry)    \
  (QCRIL_DATA_UTIL_DEFAULT_PARTIAL_RETRY_TIMEOUT << (num_retry))

#define QCRIL_DATA_EMBMS_TMGI_SIZE            (6)
#define PDP_FAIL_ACTIVATION_NOT_ALLOWED       (7)

/*Format IP address with Subnet Mask or prefix length in CIDR notation.
 * if_addr/prefix length: 10.123.240.10/24 */
#define DS_CALL_ADDR_FORMAT_IPV4_WITH_SUBNET_MASK(data,mask,str,len)    \
        (void)snprintf( str, len,                                       \
                        "%d.%d.%d.%d/%d%c",                             \
                        ((char*)data)[ 0],((char*)data)[ 1],            \
                        ((char*)data)[ 2],((char*)data)[ 3],            \
                        mask, 0 );

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


#define DS_CALL_ADDR_FORMAT_IPV4V6_WITH_PREFIX_LEN(addr,mask,str,len)         \
        if( AF_INET == SASTORAGE_FAMILY(*addr) )                              \
          DS_CALL_ADDR_FORMAT_IPV4_WITH_SUBNET_MASK( SASTORAGE_DATA(*addr),   \
                                                     mask, str, len )         \
        else if( AF_INET6 == SASTORAGE_FAMILY(*addr) )                        \
          DS_CALL_ADDR_FORMAT_IPV6_WITH_PREFIX_LENGTH( SASTORAGE_DATA(*addr), \
                                                       mask, str, len );

#define DS_CALL_ADDR_FORMAT_IPV4V6(addr,str,len)                        \
        if( AF_INET == SASTORAGE_FAMILY(*addr) )                        \
          DS_CALL_ADDR_FORMAT_IPV4( SASTORAGE_DATA(*addr), str, len )   \
        else if( AF_INET6 == SASTORAGE_FAMILY(*addr) )                  \
          DS_CALL_ADDR_FORMAT_IPV6( SASTORAGE_DATA(*addr), str, len );

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


#define QCRIL_DATA_ADDR_DELIMITER      " "

#define QCRIL_DATA_ADDR_DELIMITER_LEN  1



typedef struct
{
  dsi_data_bearer_tech_t  dsi_bearer_tech;
  const char *tech_string;
}dsi_bearer_tech_strings_s;

/* Table for RIL error code translation from verbose QMI error code. */
/* Note: Only exception cases appear here. Certain DSI types are
 * passthough as RIL API values match QMI values. */
qcril_data_ce_map qcril_data_ce_map_tbl[] =
{
/*{RIL code,                           QMI code,                           DSI type}*/
  /*3GPP defined error codes*/
  {PDP_FAIL_OPERATOR_BARRED,                          WDS_VCER_3GPP_OPERATOR_DETERMINED_BARRING_V01,               DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_NAS_SIGNALLING,                           WDS_VCER_3GPP_NAS_SIGNALLING_ERROR_V01,                      DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_LLC_SNDCP,                                WDS_VCER_3GPP_LLC_SNDCP_FAILURE_V01,                         DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_INSUFFICIENT_RESOURCES,                   WDS_VCER_3GPP_INSUFFICIENT_RESOURCES_V01,                    DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_MISSING_UKNOWN_APN,                       WDS_VCER_3GPP_UNKNOWN_APN_V01,                               DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_UNKNOWN_PDP_ADDRESS_TYPE,                 WDS_VCER_3GPP_UNKNOWN_PDP_V01,                               DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_USER_AUTHENTICATION,                      WDS_VCER_3GPP_AUTH_FAILED_V01,                               DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_ACTIVATION_REJECT_GGSN,                   WDS_VCER_3GPP_GGSN_REJECT_V01,                               DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_ACTIVATION_REJECT_UNSPECIFIED,            WDS_VCER_3GPP_ACTIVATION_REJECT_V01,                         DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_SERVICE_OPTION_NOT_SUPPORTED,             WDS_VCER_3GPP_OPTION_NOT_SUPPORTED_V01,                      DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_SERVICE_OPTION_NOT_SUBSCRIBED,            WDS_VCER_3GPP_OPTION_UNSUBSCRIBED_V01,                       DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_SERVICE_OPTION_OUT_OF_ORDER,              WDS_VCER_3GPP_OPTION_TEMP_OOO_V01,                           DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_NSAPI_IN_USE,                             WDS_VCER_3GPP_NSAPI_ALREADY_USED_V01,                        DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_REGULAR_DEACTIVATION,                     WDS_VCER_3GPP_REGULAR_DEACTIVATION_V01,                      DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_QOS_NOT_ACCEPTED,                         WDS_VCER_3GPP_QOS_NOT_ACCEPTED_V01,                          DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_NETWORK_FAILURE,                          WDS_VCER_3GPP_NETWORK_FAILURE_V01,                           DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_UMTS_REACTIVATION_REQ,                    WDS_VCER_3GPP_UMTS_REACTIVATION_REQ_V01,                     DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_FEATURE_NOT_SUPP,                         WDS_VCER_3GPP_FEATURE_NOT_SUPP_V01,                          DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_TFT_SEMANTIC_ERROR,                       WDS_VCER_3GPP_TFT_SEMANTIC_ERROR_V01,                        DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_TFT_SYTAX_ERROR,                          WDS_VCER_3GPP_TFT_SYTAX_ERROR_V01,                           DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_UNKNOWN_PDP_CONTEXT,                      WDS_VCER_3GPP_UNKNOWN_PDP_CONTEXT_V01,                       DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_FILTER_SEMANTIC_ERROR,                    WDS_VCER_3GPP_FILTER_SEMANTIC_ERROR_V01,                     DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_FILTER_SYTAX_ERROR,                       WDS_VCER_3GPP_FILTER_SYTAX_ERROR_V01,                        DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_PDP_WITHOUT_ACTIVE_TFT,                   WDS_VCER_3GPP_PDP_WITHOUT_ACTIVE_TFT_V01,                    DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_ONLY_IPV4_ALLOWED,                        WDS_VCER_3GPP_IP_V4_ONLY_ALLOWED_V01,                        DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_ONLY_IPV6_ALLOWED,                        WDS_VCER_3GPP_IP_V6_ONLY_ALLOWED_V01,                        DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_ONLY_SINGLE_BEARER_ALLOWED,               WDS_VCER_3GPP_SINGLE_ADDR_BEARER_ONLY_V01,                   DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_ESM_INFO_NOT_RECEIVED,                    WDS_VCER_3GPP_ESM_INFO_NOT_RECEIVED_V01,                     DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_PDN_CONN_DOES_NOT_EXIST,                  WDS_VCER_3GPP_PDN_CONN_DOES_NOT_EXIST_V01,                   DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_MULTI_CONN_TO_SAME_PDN_NOT_ALLOWED,       WDS_VCER_3GPP_MULTI_CONN_TO_SAME_PDN_NOT_ALLOWED_V01,        DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_MAX_ACTIVE_PDP_CONTEXT_REACHED,           WDS_VCER_3GPP_MAX_ACTIVE_PDP_CONTEXT_REACHED_V01,            DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_UNSUPPORTED_APN_IN_CURRENT_PLMN,          WDS_VCER_3GPP_UNSUPPORTED_APN_IN_CURRENT_PLMN_V01,           DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_INVALID_TRANSACTION_ID,                   WDS_VCER_3GPP_INVALID_TRANSACTION_ID_V01,                    DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_MESSAGE_INCORRECT_SEMANTIC,               WDS_VCER_3GPP_MESSAGE_INCORRECT_SEMANTIC_V01,                DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_INVALID_MANDATORY_INFO,                   WDS_VCER_3GPP_INVALID_MANDATORY_INFO_V01,                    DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_MESSAGE_TYPE_UNSUPPORTED,                 WDS_VCER_3GPP_MESSAGE_TYPE_UNSUPPORTED_V01,                  DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_MSG_TYPE_NONCOMPATIBLE_STATE,             WDS_VCER_3GPP_MSG_TYPE_NONCOMPATIBLE_STATE_V01,              DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_UNKNOWN_INFO_ELEMENT,                     WDS_VCER_3GPP_UNKNOWN_INFO_ELEMENT_V01,                      DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_CONDITIONAL_IE_ERROR,                     WDS_VCER_3GPP_CONDITIONAL_IE_ERROR_V01,                      DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE,      WDS_VCER_3GPP_MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE_V01,       DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_PROTOCOL_ERRORS,                          WDS_VCER_3GPP_PROTOCOL_ERROR_V01,                            DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_APN_TYPE_CONFLICT,                        WDS_VCER_3GPP_APN_TYPE_CONFLICT_V01,                         DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_INVALID_PCSCF_ADDR,                       WDS_VCER_3GPP_INVALID_PCSCF_ADDR_V01,                        DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_INTERNAL_CALL_PREEMPT_BY_HIGH_PRIO_APN,   WDS_VCER_3GPP_INTERNAL_CALL_PREEMPT_BY_HIGH_PRIO_APN_V01,    DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_EMM_ACCESS_BARRED,                        WDS_VCER_3GPP_EMM_ACCESS_BARRED_V01,                         DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_EMERGENCY_IFACE_ONLY,                     WDS_VCER_3GPP_EMERGENCY_IFACE_ONLY_V01,                      DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_IFACE_MISMATCH,                           WDS_VCER_3GPP_IFACE_MISMATCH_V01,                            DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_COMPANION_IFACE_IN_USE,                   WDS_VCER_3GPP_COMPANION_IFACE_IN_USE_V01,                    DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_IP_ADDRESS_MISMATCH,                      WDS_VCER_3GPP_IP_ADDRESS_MISMATCH_V01,                       DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_IFACE_AND_POL_FAMILY_MISMATCH,            WDS_VCER_3GPP_IFACE_AND_POL_FAMILY_MISMATCH_V01,             DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_EMM_ACCESS_BARRED_INFINITE_RETRY,         WDS_VCER_3GPP_EMM_ACCESS_BARRED_INFINITE_RETRY_V01,          DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  {PDP_FAIL_AUTH_FAILURE_ON_EMERGENCY_CALL,           WDS_VCER_3GPP_AUTH_FAILURE_ON_EMERGENCY_CALL_V01,            DSI_CE_TYPE_3GPP_SPEC_DEFINED},
  /*CM call end codes*/
  {PDP_FAIL_SIGNAL_LOST,                  WDS_VCER_CM_NO_SRV_V01,          DSI_CE_TYPE_CALL_MANAGER_DEFINED},
  {PDP_FAIL_SIGNAL_LOST,                  WDS_VCER_CM_FADE_V01,            DSI_CE_TYPE_CALL_MANAGER_DEFINED},
  {PDP_FAIL_PREF_RADIO_TECH_CHANGED,      WDS_VCER_CM_UE_RAT_CHANGE_V01,   DSI_CE_TYPE_CALL_MANAGER_DEFINED},
  {PDP_FAIL_PROTOCOL_ERRORS,              WDS_VCER_IPV6_ERR_HRPD_IPV6_DISABLED_V01, DSI_CE_TYPE_IPV6},
  {PDP_FAIL_USER_AUTHENTICATION,          WDS_VCER_PPP_CHAP_FAILURE_V01,            DSI_CE_TYPE_PPP},
  {PDP_FAIL_USER_AUTHENTICATION,          WDS_VCER_PPP_PAP_FAILURE_V01,             DSI_CE_TYPE_PPP},
  {PDP_FAIL_ONLY_IPV4_ALLOWED,            WDS_VCER_INTERNAL_ERR_PDN_IPV6_CALL_DISALLOWED_V01, DSI_CE_TYPE_INTERNAL},
  {PDP_FAIL_ONLY_IPV6_ALLOWED,            WDS_VCER_INTERNAL_ERR_PDN_IPV4_CALL_DISALLOWED_V01, DSI_CE_TYPE_INTERNAL},
  {PDP_FAIL_MISSING_UKNOWN_APN,           WDS_VCER_INTERNAL_APN_DISABLED_V01,                 DSI_CE_TYPE_INTERNAL},
  {PDP_FAIL_ONLY_IPV4_ALLOWED,            WDS_VCER_IPV6_DISABLED_V01,                         DSI_CE_TYPE_IPV6},
  {PDP_FAIL_INSUFFICIENT_RESOURCES,       WDS_VCER_INTERNAL_MAX_V4_CONNECTIONS_V01,           DSI_CE_TYPE_INTERNAL},
  {PDP_FAIL_INSUFFICIENT_RESOURCES,       WDS_VCER_INTERNAL_MAX_V6_CONNECTIONS_V01,           DSI_CE_TYPE_INTERNAL},
  {PDP_FAIL_UNKNOWN_PDP_ADDRESS_TYPE,     WDS_VCER_INTERNAL_IP_VERSION_MISMATCH_V01,          DSI_CE_TYPE_INTERNAL},
  {PDP_FAIL_SERVICE_OPTION_NOT_SUPPORTED, WDS_VCER_INTERNAL_DUN_CALL_DISALLOWED_V01,          DSI_CE_TYPE_INTERNAL},
#if (QCRIL_RIL_VERSION >= 6)
  {PDP_FAIL_TETHERED_CALL_ACTIVE,      WDS_VCER_INTERNAL_APP_PREEMPTED_V01,   DSI_CE_TYPE_INTERNAL}
#else
  {PDP_FAIL_TETHERED_CALL_ON,          WDS_VCER_INTERNAL_APP_PREEMPTED_V01,   DSI_CE_TYPE_INTERNAL}
#endif /* (QCRIL_RIL_VERSION >= 6) */
};
#ifdef FEATURE_DATA_EMBMS

qcril_data_embms_status_map qcril_data_embms_status_map_tbl[] =
{
  {QCRIL_DATA_EMBMS_ERROR_UNKNOWN              /*1*/, WDS_TMGI_ACTIVATE_FAILURE_UNKNOWN_V01},
  {QCRIL_DATA_EMBMS_ERROR_UNKNOWN              /*1*/, DSI_EMBMS_TMGI_DEACTIVATED_TYPE_ACT_FAIL_RADIO_CONFIG},
  {QCRIL_DATA_EMBMS_ERROR_UNKNOWN              /*1*/, DSI_EMBMS_TMGI_DEACTIVATED_TYPE_ACT_FAIL_CHANNEL_UNAVAIL},
  {QCRIL_DATA_EMBMS_ERROR_UNKNOWN              /*1*/, DSI_EMBMS_TMGI_DEACTIVATED_TYPE_ACT_FAIL_EMBMS_NOT_ENABLED},
  {QCRIL_DATA_EMBMS_ERROR_UNKNOWN              /*1*/, DSI_EMBMS_TMGI_DEACTIVATED_TYPE_ACT_FAIL_OUT_OF_COVERAGE},
  {QCRIL_DATA_EMBMS_ERROR_ALREADY_DONE         /*2*/, WDS_TMGI_ACTIVATE_SUCCESS_DUPLICATE_V01},
  {QCRIL_DATA_EMBMS_ERROR_NOT_ALLOWED          /*3*/, WDS_TMGI_ACTIVATE_FAILURE_NOT_ALLOWED_V01},
  {QCRIL_DATA_EMBMS_ERROR_MISSING_CONTROL_INFO /*4*/, WDS_TMGI_ACTIVATE_FAILURE_MISSING_CONTROL_INFO_V01},
  {QCRIL_DATA_EMBMS_ERROR_MISSING_TMGI         /*5*/, WDS_TMGI_ACTIVATE_FAILURE_MISSING_TMGI_V01},
  {QCRIL_DATA_EMBMS_ERROR_MCAST_OOC            /*6*/, WDS_TMGI_ACTIVATE_FAILURE_MCAST_OOS_V01},
  {QCRIL_DATA_EMBMS_ERROR_UCAST_OOS            /*7*/, WDS_TMGI_ACTIVATE_FAILURE_UCAST_OOS_V01},
  {QCRIL_DATA_EMBMS_ERROR_FREQUENCY_CONFLICT   /*8*/, WDS_TMGI_ACTIVATE_FAILURE_CAMPED_ON_OTHER_FREQ_V01},
  {QCRIL_DATA_EMBMS_ERROR_ALREADY_DONE         /*2*/, WDS_EMBMS_ECC_TMGI_ACTIVATION_IN_PROGRESS_V01},
  {QCRIL_DATA_EMBMS_ERROR_UNKNOWN              /*1*/, WDS_EMBMS_EEC_TMGI_DEACTIVATION_IN_PROGRESS_V01},
  {QCRIL_DATA_EMBMS_ERROR_UNKNOWN,                    WDS_EMBMS_EEC_TMGI_NOT_SUPPORTED_V01},
  {QCRIL_DATA_EMBMS_ERROR_ALREADY_DONE,               WDS_EMBMS_EEC_TMGI_INVALID_V01},
  {QCRIL_DATA_EMBMS_ERROR_UNKNOWN,                    WDS_EMBMS_EEC_TMGI_DEACTIVATION_IN_PROGRESS_V01},
  {QCRIL_DATA_EMBMS_ERROR_UNKNOWN,                    WDS_TMGI_ACTIVATE_FAILURE_SAI_MISMATCH_V01},
  {QCRIL_DATA_EMBMS_ERROR_MAX_TMGI_ALREADY_ACTIVE,    WDS_TMGI_ACTIVATION_FAILURE_MAX_TMGI_ALREADY_ACTIVE_V01},
  {QCRIL_DATA_EMBMS_SUCCESS_RADIO_TUNE_IN_PROGRESS_UCAST_IDLE, WDS_TMGI_ACTIVATE_SUCCESS_IDLE_RADIO_TUNE_V01},
  {QCRIL_DATA_EMBMS_SUCCESS_RADIO_TUNE_IN_PROGRESS_UCAST_CONNECTED, WDS_TMGI_ACTIVATE_SUCCESS_CONN_RADIO_TUNE_V01}
};
#endif


/* this mutex protects dsi callback tbl */
pthread_mutex_t dsi_cb_tbl_mutex;

extern void qcril_data_partial_retry_hdlr
(
  union sigval sval
);

static boolean qcril_data_is_partial_retry_disabled = FALSE;

static unsigned long qcril_data_max_partial_retry_timeout = QCRIL_DATA_UTIL_DEFAULT_MAX_PARTIAL_RETRY_TIMEOUT;

/*===========================================================================
                            LOCAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================

  FUNCTION:  qcril_data_util_is_ce_failure_permanent

===========================================================================*/
/*!
@brief
  Determine if the call end reason corresponds to a permanent failure (which
  implies that no retries are intended.)

@return
  TRUE  - if failure is permanent
  FALSE - Otherwise
*/
/*=========================================================================*/
boolean qcril_data_util_is_ce_failure_permanent
(
  int  ril_ce_reason
)
{
  boolean ret = FALSE;

  switch (ril_ce_reason)
  {
     case PDP_FAIL_SERVICE_OPTION_NOT_SUPPORTED:
     case PDP_FAIL_SERVICE_OPTION_NOT_SUBSCRIBED:
     case PDP_FAIL_NSAPI_IN_USE:
     case PDP_FAIL_ONLY_IPV4_ALLOWED:
     case PDP_FAIL_ONLY_IPV6_ALLOWED:
     case PDP_FAIL_PROTOCOL_ERRORS:
     case PDP_FAIL_MISSING_UKNOWN_APN:
     case PDP_FAIL_UNKNOWN_PDP_ADDRESS_TYPE:
       ret = TRUE;
       break;

     default:
       break;
  }

  return ret;
}

/*===========================================================================
                            GLOBAL FUNCTION DEFINITIONS
===========================================================================*/

/*=========================================================================
  FUNCTION:  qcril_data_cleanup_call_info_tbl

===========================================================================*/
/*!
    @brief
    frees up info_tbl entry. Updates the call information in
    call_tbl before info_tbl entry is cleaned up.

    @pre caller must have locked info_tbl_mutex prior to calling this function.

    @return
    None
*/
/*=========================================================================*/
void qcril_data_cleanup_call_state
(
  qcril_data_call_info_tbl_type *info_tbl_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  if ( !VALIDATE_LOCAL_DATA_OBJ( info_tbl_ptr ) )
  {
    QCRIL_LOG_ERROR( "invalid info_tbl_ptr" );
    goto err_label;
  }

  /* Invalidate the corresponding dsi callback table entry */
  qcril_data_clear_dsi_cb_tbl_entry(&dsi_cb_tbl[info_tbl_ptr->index]);

  /* Cleanup the corresponding response timer */
  qcril_data_util_stop_timer(&info_tbl_ptr->timer_id);

  /* Cleanup the corresponding partial retry timer */
  qcril_data_util_stop_timer(&info_tbl_ptr->retry_timer_id);

  /* update the corresponding entry in the call tbl as well */
  qcril_data_util_update_call_state( info_tbl_ptr, CALL_INACTIVE, PDP_FAIL_NONE );

  QCRIL_LOG_DEBUG( "clean up local info tbl, index [%d], cid [%d]",
                   info_tbl_ptr->index, info_tbl_ptr->cid );

  if (QCRIL_DSI_HNDL_INVALID != info_tbl_ptr->dsi_hndl)
  {
    QCRIL_LOG_DEBUG("releasing dsi_hndl [0x%x]", info_tbl_ptr->dsi_hndl);
    dsi_rel_data_srvc_hndl(info_tbl_ptr->dsi_hndl);
  }

#if (QCRIL_RIL_VERSION >= 15)
  /* Clean up all keepalive sessions */
  qcrili_data_ma_ka_cleanup(info_tbl_ptr->cid);
  QCRIL_LOG_DEBUG("Cleaned up keepalive sessions for call id %d", info_tbl_ptr->cid);
#endif
  if (QCRIL_DATA_HNDL_INVALID != info_tbl_ptr->qmi_wds_hndl_v4)
  {
    QCRIL_LOG_DEBUG("releasing qmi_wds_hndl [0x%x]", info_tbl_ptr->qmi_wds_hndl_v4);
    qcril_data_qmi_wds_release_qmi_client(info_tbl_ptr->qmi_wds_hndl_v4);
  }
  if (QCRIL_DATA_HNDL_INVALID != info_tbl_ptr->qmi_wds_hndl_v6)
  {
    QCRIL_LOG_DEBUG("releasing qmi_wds_hndl [0x%x]", info_tbl_ptr->qmi_wds_hndl_v6);
    qcril_data_qmi_wds_release_qmi_client(info_tbl_ptr->qmi_wds_hndl_v6);
  }
  info_tbl_ptr->qmi_wds_hndl    = QCRIL_DATA_HNDL_INVALID;
  info_tbl_ptr->qmi_wds_hndl_v4 = QCRIL_DATA_HNDL_INVALID;
  info_tbl_ptr->qmi_wds_hndl_v6 = QCRIL_DATA_HNDL_INVALID;

  /* call is down. Let go of the profiles associated with it. */
  if (info_tbl_ptr->qdp_umts_profile_id != QCRIL_INVALID_PROFILE_ID)
  {
    QCRIL_LOG_DEBUG("releasing QDP profile id [%d]",
                    info_tbl_ptr->qdp_umts_profile_id);
    qdp_profile_release(info_tbl_ptr->qdp_umts_profile_id);
  }
  if (info_tbl_ptr->qdp_cdma_profile_id != QCRIL_INVALID_PROFILE_ID)
  {
    QCRIL_LOG_DEBUG("releasing QDP profile id [%d]",
                    info_tbl_ptr->qdp_cdma_profile_id);
    qdp_profile_release(info_tbl_ptr->qdp_cdma_profile_id);
  }

  info_tbl_ptr->cid      = CALL_ID_INVALID;
  info_tbl_ptr->dsi_hndl = QCRIL_DSI_HNDL_INVALID;
  info_tbl_ptr->qmi_wds_hndl = QCRIL_DATA_HNDL_INVALID;
  info_tbl_ptr->pend_tok = NULL;
  info_tbl_ptr->pend_req = DS_RIL_REQ_INVALID;
  info_tbl_ptr->self     = NULL;
  info_tbl_ptr->info_flg = FALSE;
  info_tbl_ptr->is_partial_retry = FALSE;
  info_tbl_ptr->partial_retry_count = 0;
  info_tbl_ptr->qdp_umts_profile_id = QCRIL_INVALID_PROFILE_ID;
  info_tbl_ptr->qdp_cdma_profile_id = QCRIL_INVALID_PROFILE_ID;

  memset( &info_tbl_ptr->call_info, 0, sizeof info_tbl_ptr->call_info );

  QCRIL_DATA_RELEASE_STORAGE(info_tbl_ptr->call_params);

  QCRIL_LOG_DEBUG( "EXIT with succ" );
  return;

err_label:
  QCRIL_LOG_DEBUG( "exit with FAILURE" );
  return;
}/* qcril_data_cleanup_call_state() */

/*===========================================================================

  FUNCTION:  qcril_data_update_dsi_cb_tbl_entry

===========================================================================*/
/*!
    @brief
    Updates the requested dsi_cb_tbl[] entry with the given values

    @return
    None.

    @note
    This funcion should not call any DSI or QCRIL API functions
*/
/*=========================================================================*/
void qcril_data_update_dsi_cb_tbl_entry
(
  qcril_data_dsi_cb_tbl_type *dsi_cb_tbl_ptr,
  qcril_instance_id_e_type   instance_id,
  qcril_modem_id_e_type      modem_id,
  RIL_Token                  pend_tok,
  dsi_hndl_t                 dsi_hndl,
  qcril_data_call_info_tbl_type *info_tbl_ptr
)
{
  if (NULL == dsi_cb_tbl_ptr)
  {
    QCRIL_LOG_ERROR( "invalid dsi_cb_tbl_ptr" );
    return;
  }

  QCRIL_DATA_MUTEX_LOCK(&dsi_cb_tbl_mutex);

  QCRIL_LOG_DEBUG( "updating entry [%#x]", dsi_cb_tbl_ptr );

  /* Update the corresponding dsi callback table entry */
  dsi_cb_tbl_ptr->instance_id  = instance_id;
  dsi_cb_tbl_ptr->modem_id     = modem_id;
  dsi_cb_tbl_ptr->pend_tok     = pend_tok;
  QCRIL_LOG_INFO( "DEBUG:  %s step %d - dsi_cb_tbl_ptr->pend_tok[0x%x]",
                  __func__, 0, dsi_cb_tbl_ptr->pend_tok );
  dsi_cb_tbl_ptr->dsi_hndl     = dsi_hndl;
  dsi_cb_tbl_ptr->info_tbl_ptr = info_tbl_ptr;

  QCRIL_DATA_MUTEX_UNLOCK(&dsi_cb_tbl_mutex);
}

/*===========================================================================

  FUNCTION:  qcril_data_clear_dsi_cb_tbl_entry

===========================================================================*/
/*!
    @brief
    Clears the requested entry in dsi_cb_tbl[]

    @return
    None.

    @note
    This funcion should not call any DSI or QCRIL API functions
*/
/*=========================================================================*/
void qcril_data_clear_dsi_cb_tbl_entry
(
  qcril_data_dsi_cb_tbl_type *dsi_cb_tbl_ptr
)
{
  if (NULL == dsi_cb_tbl_ptr)
  {
    QCRIL_LOG_ERROR( "invalid dsi_cb_tbl_ptr" );
    return;
  }

  QCRIL_DATA_MUTEX_LOCK(&dsi_cb_tbl_mutex);

  QCRIL_LOG_DEBUG( "cleaning up entry [%#x]", dsi_cb_tbl_ptr );

  /* Invalidate the corresponding dsi callback table entry */
  dsi_cb_tbl_ptr->instance_id  = QCRIL_DATA_INVALID_INSTANCE_ID;
  dsi_cb_tbl_ptr->modem_id     = QCRIL_DATA_INVALID_MODEM_ID;
  dsi_cb_tbl_ptr->pend_tok     = NULL;
  dsi_cb_tbl_ptr->dsi_hndl     = QCRIL_DSI_HNDL_INVALID;
  dsi_cb_tbl_ptr->info_tbl_ptr = NULL;

  QCRIL_DATA_MUTEX_UNLOCK(&dsi_cb_tbl_mutex);
}


/*=========================================================================
  FUNCTION:  qcril_data_util_update_call_state

===========================================================================*/
/*!
    @brief
    Updates call state.

    @return
    NONE
*/
/*=========================================================================*/
void qcril_data_util_update_call_state
(
  qcril_data_call_info_tbl_type *info_tbl_ptr,
  int call_state,
  int status
)
{
  /* validate input parameter */
  if(!info_tbl_ptr )
  {
    QCRIL_LOG_ERROR("bad param(s) "
                    "info_tbl_ptr=0x%x", info_tbl_ptr);
    return;
  }

  QCRIL_DS_ASSERT( ( info_tbl_ptr->index < MAX_CONCURRENT_UMTS_DATA_CALLS ),
                   "validate info_tbl index value range" );

  /* lock call_tbl while updating */
  QCRIL_DATA_MUTEX_LOCK(&info_tbl_mutex);
  info_tbl_ptr->call_info.active = call_state;

#if (QCRIL_RIL_VERSION >= 6)
  info_tbl_ptr->status = status;
#endif /* #if (QCRIL_RIL_VERSION >= 6) */

  /* unlock call_tbl now */
  QCRIL_DATA_MUTEX_UNLOCK(&info_tbl_mutex);
}/* qcril_data_util_update_call_state */

#if (QCRIL_RIL_VERSION >= 6)
/*===========================================================================

  FUNCTION:  qcril_data_util_get_dual_ip_call_status

===========================================================================*/
/*!
    @brief
    Routine to obtain the call status of a Dual-IP call. This function must
    only be called for a Dual-IP call otherwise a PDP_FAIL_OEM_DCFAILCAUSE_4
    is returned.

    @pre Before calling, info_tbl_mutex must be locked by the calling thread

    @return
    Call status (PDP_FAIL_xxx).
*/
/*=========================================================================*/
boolean qcril_data_util_get_dual_ip_call_status
(
  qcril_data_call_info_tbl_type  *info_tbl_ptr,
  int                            *err_code
)
{
  dsi_ce_reason_t ce_reason_v4 = {DSI_CE_TYPE_INVALID, WDS_VCER_MIP_HA_REASON_UNKNOWN_V01};
  dsi_ce_reason_t ce_reason_v6 = {DSI_CE_TYPE_INVALID, WDS_VCER_MIP_HA_REASON_UNKNOWN_V01};
  int             status_v4    = PDP_FAIL_OEM_DCFAILCAUSE_4;
  int             status_v6    = PDP_FAIL_OEM_DCFAILCAUSE_4;
  boolean         attempt_retry = FALSE;

  if (!info_tbl_ptr || !err_code)
  {
    QCRIL_LOG_ERROR("Invalid input param");
    goto bail;
  }

  *err_code = PDP_FAIL_OEM_DCFAILCAUSE_1;

  /* For a Dual-IP call, verify the status of both V4, V6 calls */
  if (DSI_IP_VERSION_4_6 != info_tbl_ptr->dsi_ip_version)
  {
    QCRIL_LOG_ERROR("not a dual-ip call");
    goto bail;
  }

  if (DSI_SUCCESS != dsi_get_call_end_reason(info_tbl_ptr->dsi_hndl,
                                             &ce_reason_v4,
                                             DSI_IP_FAMILY_V4))
  {
    goto bail;
  }

  if (DSI_SUCCESS != dsi_get_call_end_reason(info_tbl_ptr->dsi_hndl,
                                             &ce_reason_v6,
                                             DSI_IP_FAMILY_V6))
  {
    goto bail;
  }

  /* If either of the CE reasons hasn't been populated, don't retry.
     Initiate retry on the next DSI_EVT_NET_PARTIAL_CONN event */
  if (DSI_CE_TYPE_UNINIT == ce_reason_v4.reason_type ||
      DSI_CE_TYPE_UNINIT == ce_reason_v6.reason_type)
  {
    QCRIL_LOG_VERBOSE("Uninitialized CE type v4=%d, v6=%d, skipping retry",
                      ce_reason_v4.reason_type,
                      ce_reason_v6.reason_type);
    goto bail;
  }

  /* Get a more specific error code for V4 */
  if (DSI_CE_TYPE_INVALID != ce_reason_v4.reason_type)
  {
    QCRIL_LOG_DEBUG("dsi_get_call_end_reason for V4 returned [%d]",
                    ce_reason_v4.reason_code);
    if (SUCCESS != qcril_data_get_ril_ce_code(&ce_reason_v4, &status_v4))
    {
      QCRIL_LOG_ERROR("qcril_data_get_ril_ce_code failed for V4");
    }
  }

  /* Get a more specific error code for V6 */
  if (DSI_CE_TYPE_INVALID != ce_reason_v6.reason_type)
  {
    if (SUCCESS != qcril_data_get_ril_ce_code(&ce_reason_v6, &status_v6))
    {
      QCRIL_LOG_ERROR("qcril_data_get_ril_ce_code failed for V6");
    }
  }

  QCRIL_LOG_DEBUG( "RIL CE status_v4=%d, status_v6=%d",
                   status_v4,
                   status_v6 );

  /* Return a more specific status if available */
  if (PDP_FAIL_OEM_DCFAILCAUSE_4  != status_v4)
  {
    *err_code = status_v4;
  }
  else
  {
    *err_code = status_v6;
  }

  attempt_retry = TRUE;

bail:
  if (NULL != err_code)
  {
    QCRIL_LOG_INFO( "set call end reason [%d]", *err_code );
  }
  return attempt_retry;
}/* qcril_data_util_get_dual_ip_call_status() */

#endif

/*=========================================================================
  FUNCTION:  qcril_data_get_ril_ce_code

===========================================================================*/
/*!
    @brief
    gets ril call end reason code given qmi call end reason code

    @return
    FAILURE - if bad params received
    SUCCESS - other wise (must return valid RIL cause code)
*/
/*=========================================================================*/
int qcril_data_get_ril_ce_code
(
  dsi_ce_reason_t * dsi_ce_reason,
  int * ril_ce_code
)
{
  unsigned int i=0;
  unsigned int max=sizeof(qcril_data_ce_map_tbl)/sizeof(qcril_data_ce_map);
  int ret = FAILURE;

  do
  {
    /* the only real error condition for this function */
    if (NULL == dsi_ce_reason ||
        NULL == ril_ce_code )
    {
      QCRIL_LOG_ERROR("**programming err** NULL param rcvd");
      break;
    }

    /* we must return some cause code. If we can't find any,
     * we return UNSPECIFIED, but this function must return success
     * from this point onwards */
    ret = SUCCESS;

    /* check to see if it matches with cause codes as defined in
     * qcril_data_ce_map_tbl */
    for(i=0; i<max; i++)
    {
      if (qcril_data_ce_map_tbl[i].qmi_data_ce_code == dsi_ce_reason->reason_code &&
          qcril_data_ce_map_tbl[i].reason_category == dsi_ce_reason->reason_type
        )
      {
        *ril_ce_code = qcril_data_ce_map_tbl[i].ril_data_ce_code;
        QCRIL_LOG_DEBUG("found matching cause code [%d]",
                        *ril_ce_code);
        break;
      } else if ( ( dsi_ce_reason->reason_type == DSI_CE_TYPE_CALL_MANAGER_DEFINED ) && ( ( dsi_ce_reason->reason_code == WDS_VCER_CM_DETACH_EPS_SERVICES_NOT_ALLOWED_V01 ) || ( dsi_ce_reason->reason_code == WDS_VCER_CM_NEW_GPRS_SERV_NOT_ALLOWED_V01 ) ) ) {
        *ril_ce_code = PDP_FAIL_ACTIVATION_NOT_ALLOWED;
        QCRIL_LOG_DEBUG("found matching cause code [%d]",
                        *ril_ce_code);
        break;
      }
    }
    /* break out if we found matching cause code */
    if (i < max)
    {
      break;
    }

    /* if no match found in qcril_data_ce_map_tbl, handle passthrough
     * and unsupported cases */
    switch( dsi_ce_reason->reason_type )
    {
      case DSI_CE_TYPE_3GPP_SPEC_DEFINED:
        *ril_ce_code = PDP_FAIL_OEM_DCFAILCAUSE_6;
        QCRIL_LOG_DEBUG("returning 3gpp spec defined cause code [%d]",
                        *ril_ce_code);
        break;

      case DSI_CE_TYPE_INTERNAL:
        QCRIL_LOG_DEBUG("returning Internal cause code [%d]",
                        dsi_ce_reason->reason_code);
        *ril_ce_code = PDP_FAIL_OEM_DCFAILCAUSE_4;
        break;

      case DSI_CE_TYPE_MOBILE_IP:
        QCRIL_LOG_DEBUG("returning Mobile IP defined cause code [%d]",
                        dsi_ce_reason->reason_code);
        *ril_ce_code = PDP_FAIL_OEM_DCFAILCAUSE_3 ;
        break;

      case DSI_CE_TYPE_CALL_MANAGER_DEFINED:
        QCRIL_LOG_DEBUG("returning Call Manager defined cause code [%d]",
                        dsi_ce_reason->reason_code);
        *ril_ce_code = PDP_FAIL_OEM_DCFAILCAUSE_5;
        break;

      case DSI_CE_TYPE_PPP:
        QCRIL_LOG_DEBUG("%s","returning PPP defined cause code [%d]",
                        dsi_ce_reason->reason_code);
        *ril_ce_code = PDP_FAIL_OEM_DCFAILCAUSE_7;
        break;

      case DSI_CE_TYPE_EHRPD:
        QCRIL_LOG_DEBUG("%s","returning EHRPD defined cause code [%d]",
                        dsi_ce_reason->reason_code);
        *ril_ce_code = PDP_FAIL_OEM_DCFAILCAUSE_8;
        break;

      default:
        /* if we reach here, return UNSPECIFIED cause code */
        *ril_ce_code = PDP_FAIL_OEM_DCFAILCAUSE_4;
        QCRIL_LOG_DEBUG("unspecified/invalid call end reason rcvd");
        break;
    }
  } while (0);

  return ret;
}
#ifdef FEATURE_DATA_EMBMS

/*=========================================================================
  FUNCTION:  qcril_data_utils_embms_copy_tmgi_list

===========================================================================*/
int qcril_data_utils_embms_copy_tmgi_list
(
  wds_embms_tmgi_type_v01 **tmgi_list_ptr_to,
  wds_embms_tmgi_type_v01  *tmgi_list_ptr_from,
  unsigned char             len
)
{
  int count = 0;
  int ret = FAILURE;

  QCRIL_LOG_DEBUG("%s", ">>>qcril_data_utils_embms_copy_tmgi_list: ENTRY");

  do
  {
    /* validate input */
    if(NULL == tmgi_list_ptr_from ||
       NULL == tmgi_list_ptr_to)
    {
      QCRIL_LOG_ERROR("%s", "rcvd invalid input data");
      break;
    }

    /* allocate memory */
    *tmgi_list_ptr_to = (wds_embms_tmgi_type_v01 *)
                       malloc(len * sizeof(wds_embms_tmgi_type_v01));
    if(NULL != (*tmgi_list_ptr_to))
    {
      /* zero out memory */
      memset(*tmgi_list_ptr_to,
             0,
             len * sizeof(wds_embms_tmgi_type_v01));
      for (count = 0; count < len; count++)
      {
        memcpy((*tmgi_list_ptr_to)[count].tmgi,
               tmgi_list_ptr_from[count].tmgi,
               QCRIL_DATA_EMBMS_TMGI_SIZE);
        (*tmgi_list_ptr_to)[count].session_id =
                         tmgi_list_ptr_from[count].session_id;

        (*tmgi_list_ptr_to)[count].session_id_valid =
                         tmgi_list_ptr_from[count].session_id_valid;

      }
    }
    else
    {
      QCRIL_LOG_ERROR("%s", "can not allocate memory!");
      break;
    }
    ret = SUCCESS;
  }while(0);

  if (SUCCESS == ret)
  {
    QCRIL_LOG_VERBOSE("%s", ">>>qcril_data_utils_embms_copy_tmgi_list: EXIT with suc");
  }
  else
  {
    QCRIL_LOG_VERBOSE("%s", ">>>qcril_data_utils_embms_copy_tmgi_list: EXIT with err");
  }
  return ret;
}
/*=========================================================================
  FUNCTION:  qcril_data_utils_embms_get_ril_status_code

===========================================================================*/
/*!
    @brief
    gets ril activate or deactivate status code given qmi status code

    @return
    FAILURE - if bad params received
    SUCCESS - other wise (must return valid RIL cause code)
*/
/*=========================================================================*/
int qcril_data_utils_embms_get_ril_status_code
(
  int   qmi_status,
  int * ril_status
)
{
  unsigned int i=0;
  unsigned int max=sizeof(qcril_data_embms_status_map_tbl)/sizeof(qcril_data_embms_status_map);
  int ret = FAILURE;

  do
  {
    /* the only real error condition for this function */
    if (NULL == ril_status )
    {
      QCRIL_LOG_ERROR("**programming err** NULL param rcvd");
      break;
    }

    /* we must return some cause code. If we can't find any,
     * we return UNSPECIFIED, but this function must return success
     * from this point onwards */
    ret = SUCCESS;

    /* check to see if it matches with cause codes as defined in
     * qcril_data_embms_status_map_tbl */
    for(i=0; i<max; i++)
    {
      if (qcril_data_embms_status_map_tbl[i].qmi_embms_status_code == qmi_status )
      {
        *ril_status = qcril_data_embms_status_map_tbl[i].ril_embms_status_code;
        QCRIL_LOG_DEBUG("found matching cause code [%d]",
                        *ril_status);
        break;
      }
    }
    /* break out if we found matching cause code */
    if (i < max)
    {
      break;
    }
    /* for others not mapped n qcril_data_embms_status_map_tbl, assign
       generic ERROR_UNKNOWN */
    *ril_status = QCRIL_DATA_EMBMS_ERROR_UNKNOWN;
  } while (0);

  return ret;
}

#endif /* FEATURE_DATA_EMBMS */
/*===========================================================================

  FUNCTION:  qcril_data_util_stop_timer

===========================================================================*/
/*!
    @brief
    Routine to stop and delete the given timer

    @pre Before calling, info_tbl_mutex must be locked by the calling thread

    @return
    None.
*/
/*=========================================================================*/
void qcril_data_util_stop_timer
(
  timer_t  *timer_id
)
{
  QCRIL_LOG_FUNC_ENTRY();

  if (!timer_id)
  {
    QCRIL_LOG_ERROR( "invalid input" );
    return;
  }
  else if (QCRIL_DATA_INVALID_TIMERID == *timer_id)
  {
    QCRIL_LOG_DEBUG( "invalid timer_id" );
    return;
  }

  QCRIL_LOG_VERBOSE( "deleting timer [%#x]", *timer_id );

  /* Delete the timer */
  timer_delete(*timer_id);
  *timer_id = QCRIL_DATA_INVALID_TIMERID;

  QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================

  FUNCTION:  qcril_data_util_disarm_timer

===========================================================================*/
/*!
    @brief
    Routine to disarm (stop) the given timer

    @pre Before calling, info_tbl_mutex must be locked by the calling thread

    @return
    None.
*/
/*=========================================================================*/
void qcril_data_util_disarm_timer
(
  timer_t  *timer_id
)
{
  struct itimerspec itimers;

  QCRIL_LOG_FUNC_ENTRY();

  if (!timer_id)
  {
    QCRIL_LOG_ERROR( "invalid input" );
    return;
  }
  else if (QCRIL_DATA_INVALID_TIMERID == *timer_id)
  {
    QCRIL_LOG_DEBUG( "invalid timer_id" );
    return;
  }

  QCRIL_LOG_VERBOSE( "disarming timer [%#x]", *timer_id );

  /* Disarm the timer */
  memset(&itimers, 0, sizeof(itimers));

  /* Start the timer */
  if (-1 == timer_settime(*timer_id, 0, &itimers, NULL))
  {
    QCRIL_LOG_ERROR( "failed to disarm timer for timer_id [%#x]... ",
                     *timer_id);
  }

  QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================

  FUNCTION:  qcril_data_get_ifindex

===========================================================================*/
/*!
    @brief
    Retrive ifindex from device name.

    @return
    SUCCESS: ifindex is retrieved successfully
    FAILURE: ifindx is not retrieved
*/
/*=========================================================================*/
int qcril_data_get_ifindex(const char * dev, int * ifindex)
{
  struct ifreq ifr;
  int fd;

  QCRIL_LOG_FUNC_ENTRY();

  if(NULL == ifindex || NULL == dev)
  {
    QCRIL_LOG_ERROR("invalid input");
    return FAILURE;
  }

  *ifindex = -1;

  /* Open a temporary socket of datagram type to use for issuing the ioctl */
  if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    QCRIL_LOG_ERROR("Open datagram socket failed");
    return FAILURE;
  }

  /* Set device name in the ioctl req struct */
  memset(&ifr, 0x0, sizeof(ifr));
  (void)strlcpy(ifr.ifr_name, dev, sizeof(ifr.ifr_name));

  /* Issue ioctl on the device */
  if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0) {
    QCRIL_LOG_ERROR("Issue ioctl on the device failed, errno: %d", errno);
    close(fd);
    return FAILURE;
  }
  *ifindex = ifr.ifr_ifindex;

  /* Close temporary socket */
  close(fd);

  QCRIL_LOG_FUNC_RETURN();

  return SUCCESS;
}


/*===========================================================================

  FUNCTION:  qcril_data_get_ril_tech_string

===========================================================================*/
/*!
@brief
  Return the string corresponding to the given ril technology value

@return
  String representation of the given ril technology
*/
/*=========================================================================*/
const char *
qcril_data_util_get_ril_tech_string(qcril_data_ril_radio_tech_t  ril_tech)
{
  static const char *ril_tech_strings[] =
  {
    "RADIO_TECH_UNKNOWN",
    "RADIO_TECH_GPRS",
    "RADIO_TECH_EDGE",
    "RADIO_TECH_UMTS",
    "RADIO_TECH_IS95A",
    "RADIO_TECH_IS95B",
    "RADIO_TECH_1xRTT",
    "RADIO_TECH_EVDO_0",
    "RADIO_TECH_EVDO_A",
    "RADIO_TECH_HSDPA",
    "RADIO_TECH_HSUPA",
    "RADIO_TECH_HSPA",
    "RADIO_TECH_EVDO_B",
    "RADIO_TECH_EHRPD",
    "RADIO_TECH_LTE",
    "RADIO_TECH_LTE_CA",
    "RADIO_TECH_HSPAP",
    "RADIO_TECH_GSM"
    ,"RADIO_TECH_TD_SCDMA"
  };

  const char *ril_string = ril_tech_strings[0];

  if (ril_tech < QCRIL_DATA_RIL_RADIO_TECH_UNKNOWN || ril_tech >= QCRIL_DATA_RIL_RADIO_TECH_LAST)
  {
    QCRIL_LOG_ERROR("qcril_data_util_get_ril_tech_string: invalid input: %d", ril_tech);
    goto bail;
  }

  /* QCRIL Data RIL technology definitions are RIL_RadioTechnolog+2 */
  ril_string = ril_tech_strings[ril_tech-2];

bail:
  return ril_string;
}

/*===========================================================================

  FUNCTION:  qcril_data_get_dsi_bearer_tech_string

===========================================================================*/
/*!
@brief
  Return the string corresponding to the given dsi bearer technology value

@return
  String representation of the given dsi bearer technology
*/
/*=========================================================================*/
const char *
qcril_data_util_get_dsi_bearer_tech_string(dsi_data_bearer_tech_t  dsi_bearer_tech)
{
  static dsi_bearer_tech_strings_s dsi_bearer_tech_strings[] =
  {
    {DSI_DATA_BEARER_TECH_UNKNOWN,       "BEARER_TECH_UNKNOWN"},
    /* CDMA related data bearer technologies */
    {DSI_DATA_BEARER_TECH_CDMA_1X,       "BEARER_TECH_CDMA_1X"},
    {DSI_DATA_BEARER_TECH_EVDO_REV0,     "BEARER_TECH_EVDO_REV0"},
    {DSI_DATA_BEARER_TECH_EVDO_REVA,     "BEARER_TECH_EVDO_REVA"},
    {DSI_DATA_BEARER_TECH_EVDO_REVB,     "BEARER_TECH_EVDO_REVB"},
    {DSI_DATA_BEARER_TECH_EHRPD,         "BEARER_TECH_EHRPD"},
    {DSI_DATA_BEARER_TECH_FMC,           "BEARER_TECH_FMC"},
    {DSI_DATA_BEARER_TECH_HRPD,          "BEARER_TECH_HRPD"},
    {DSI_DATA_BEARER_TECH_3GPP2_WLAN,    "BEARER_TECH_3GPP2_WLAN"},
    /* UMTS related data bearer technologies */
    {DSI_DATA_BEARER_TECH_WCDMA,         "BEARER_TECH_WCDMA"},
    {DSI_DATA_BEARER_TECH_GPRS,          "BEARER_TECH_GPRS"},
    {DSI_DATA_BEARER_TECH_HSDPA,         "BEARER_TECH_HSDPA"},
    {DSI_DATA_BEARER_TECH_HSUPA,         "BEARER_TECH_HSUPA"},
    {DSI_DATA_BEARER_TECH_EDGE,          "BEARER_TECH_EDGE"},
    {DSI_DATA_BEARER_TECH_LTE,           "BEARER_TECH_LTE"},
    {DSI_DATA_BEARER_TECH_HSDPA_PLUS,    "BEARER_TECH_HSDPA_PLUS"},
    {DSI_DATA_BEARER_TECH_DC_HSDPA_PLUS, "BEARER_TECH_DC_HSDPA_PLUS"},
    {DSI_DATA_BEARER_TECH_64_QAM,        "BEARER_TECH_64_QAM"},
    {DSI_DATA_BEARER_TECH_TDSCDMA,       "BEARER_TECH_TDSCDMA"},
    {DSI_DATA_BEARER_TECH_GSM,           "BEARER_TECH_GSM"},
    {DSI_DATA_BEARER_TECH_3GPP_WLAN,     "BEARER_TECH_3GPP_WLAN"},
    {DSI_DATA_BEARER_TECH_MAX,           "BEARER_TECH_UNKNOWN"}
  };

  const char *dsi_bearer_string = dsi_bearer_tech_strings[0].tech_string;
  int i = 0;

  if (dsi_bearer_tech >= DSI_DATA_BEARER_TECH_MAX)
  {
    QCRIL_LOG_ERROR("invalid input");
    goto bail;
  }

  while (dsi_bearer_tech_strings[i].dsi_bearer_tech < DSI_DATA_BEARER_TECH_MAX)
  {
    if (dsi_bearer_tech == dsi_bearer_tech_strings[i].dsi_bearer_tech)
    {
      dsi_bearer_string = dsi_bearer_tech_strings[i].tech_string;
    }
    i++;
  }

bail:
  return dsi_bearer_string;
}

/*===========================================================================

  FUNCTION:  qcril_data_get_active_call_list

===========================================================================*/
/*!
    @brief
    Retrive a list of active calls.

    @return
    None
*/
/*=========================================================================*/
void qcril_data_get_active_call_list
(
  void                 **response,
  size_t                *response_len
)
{
  /* validate input parameter */
  if(!response || !response_len)
  {
    QCRIL_LOG_ERROR("bad param(s) "
                    "response=0x%x, response_len=0x%x",
                    response, response_len);
    return;
  }
  size_t active_call_tbl_size = 0;
  size_t call_tbl_entry_size = 0;
  int i = 0;
  int index = 0;

  int ret = SUCCESS;

#if (QCRIL_RIL_VERSION >= 6)

#if (QCRIL_RIL_VERSION >= 11)

  RIL_Data_Call_Response_v11 *active_call_table = NULL;

#elif (QCRIL_RIL_VERSION >= 10)

  RIL_Data_Call_Response_v9 *active_call_table = NULL;

#else

  RIL_Data_Call_Response_v6 *active_call_table = NULL;

#endif /* QCRIL_RIL_VERSION >= 11 */


#else
  RIL_Data_Call_Response    *active_call_table = NULL;
#endif /* QCRIL_RIL_VERSION >= 6 */

  /*-----------------------------------------------------------------------*/

  QCRIL_LOG_FUNC_ENTRY();

#if (QCRIL_RIL_VERSION >= 6)

#if (QCRIL_RIL_VERSION >= 11)

  call_tbl_entry_size = sizeof(RIL_Data_Call_Response_v11);

#elif (QCRIL_RIL_VERSION >= 10)

  call_tbl_entry_size = sizeof(RIL_Data_Call_Response_v9);

#else

  call_tbl_entry_size = sizeof(RIL_Data_Call_Response_v6);

#endif /* (QCRIL_RIL_VERSION >= 11)  */


#else
  call_tbl_entry_size = sizeof(RIL_Data_Call_Response);
#endif /* (QCRIL_RIL_VERSION >= 6) */

  QCRIL_DATA_MUTEX_LOCK(&info_tbl_mutex);

  /* Loop over call table record number of active call */
  for (i = 0; i < MAX_CONCURRENT_UMTS_DATA_CALLS; i++)
  {
    if(CALL_ID_INVALID != info_tbl[i].cid &&
       NULL == info_tbl[i].pend_tok)
    {
      active_call_tbl_size ++;
    }
  }

  do
  {
    /* no need to check <0 case since active_call_tbl_size is size_t*/
    if (0 == active_call_tbl_size)
    {
      ret = FAILURE;
      /* no active data call, no need to process further */
      QCRIL_LOG_VERBOSE("No active call");
      break;
    }

    /* allocate memory for active data call */
#if (QCRIL_RIL_VERSION >= 6)

#if (QCRIL_RIL_VERSION >= 11)

    active_call_table = (RIL_Data_Call_Response_v11 *)qcril_malloc(
                         active_call_tbl_size * call_tbl_entry_size);

#elif (QCRIL_RIL_VERSION >= 10)

    active_call_table = (RIL_Data_Call_Response_v9 *)qcril_malloc(
                         active_call_tbl_size * call_tbl_entry_size);


#else

    active_call_table = (RIL_Data_Call_Response_v6 *)qcril_malloc(
                         active_call_tbl_size * call_tbl_entry_size);


#endif /* (QCRIL_RIL_VERSION >= 11)  */


#else
    active_call_table = (RIL_Data_Call_Response *)qcril_malloc(
                         active_call_tbl_size * call_tbl_entry_size);
#endif /* (QCRIL_RIL_VERSION >= 6) */

    if( NULL == active_call_table)
    {
      ret = FAILURE;
      QCRIL_LOG_ERROR(" Can not allocate memory");
      break;
    }

    /* loop over call table, copy active call entry */
    for(i = 0; i < MAX_CONCURRENT_UMTS_DATA_CALLS; i++)
    {
      if(CALL_ID_INVALID != info_tbl[i].cid &&
         NULL == info_tbl[i].pend_tok)
      {
        active_call_table[index].active = info_tbl[i].call_info.active;
        active_call_table[index].type = info_tbl[i].call_info.type;
#if (QCRIL_RIL_VERSION >= 6)
        active_call_table[index].addresses = info_tbl[i].call_info.address.fmtstr;
        active_call_table[index].cid = info_tbl[i].cid;
        active_call_table[index].dnses = info_tbl[i].call_info.dns.fmtstr;
        active_call_table[index].gateways = info_tbl[i].call_info.gateway.fmtstr;
        active_call_table[index].ifname = info_tbl[i].call_info.dev_name;
        active_call_table[index].status = info_tbl[i].status;
        active_call_table[index].suggestedRetryTime = info_tbl[i].suggestedRetryTime;

#if (QCRIL_RIL_VERSION >= 10)

        active_call_table[index].pcscf = info_tbl[i].call_info.pcscf.fmtstr;

#endif

#if (QCRIL_RIL_VERSION >= 11)

        active_call_table[index].mtu = info_tbl[i].call_info.mtu;
#endif

#else
        active_call_table[index].apn = info_tbl[i].call_info.apn;
        active_call_table[index].addresses = info_tbl[i].call_info.address;
        active_call_table[index].radioTech = info_tbl[i].call_info.radioTech;
        active_call_table[index].inactiveReason = info_tbl[i].call_info.inactiveReason;
#endif /* (QCRIL_RIL_VERSION >= 6)*/

        index++;
      }
    }
  } while(0);

  QCRIL_DATA_MUTEX_UNLOCK( &info_tbl_mutex );

  *response = (void *)active_call_table;
  *response_len = active_call_tbl_size * call_tbl_entry_size;

  QCRIL_LOG_DEBUG("active call table size:[%d] "
                  "start from: [0x%x]\n", active_call_tbl_size, active_call_table);
  if(FAILURE == ret)
  {
    QCRIL_LOG_DEBUG( "EXIT with failure" );
  }
  else
  {
    QCRIL_LOG_DEBUG( "EXIT with succ" );
  }
  return;
}/* qcril_data_get_active_call_list */


/*===========================================================================

  FUNCTION:  qcril_data_util_is_partial_retry_allowed

===========================================================================*/
/*!
@brief
  Determines if a partial retry for a Dual-IP call is allowed or not based
  on the current bearer and the corresponding profile configuration on the
  modem

@return
  TRUE  - if partial retry is allowed
  FALSE - Otherwise
*/
/*=========================================================================*/
int
qcril_data_util_is_partial_retry_allowed
(
  qcril_data_call_info_tbl_type  *info_tbl_ptr
)
{
  qdp_profile_pdn_type    pdn_type = QDP_PROFILE_PDN_TYPE_INVALID;
  int                     ret = TRUE;
  dsi_data_bearer_tech_t  bearer_tech = DSI_DATA_BEARER_TECH_UNKNOWN;


  if (TRUE == qcril_data_is_partial_retry_disabled)
  {
    return FALSE;
  }
  else if (!info_tbl_ptr)
  {
    QCRIL_LOG_ERROR("Invalid input");
    return FALSE;
  }
  else if (DSI_IP_VERSION_4_6 != info_tbl_ptr->dsi_ip_version)
  {
    QCRIL_LOG_ERROR("Not a Dual-IP call cid [%d]",
                    info_tbl_ptr->cid);
    return FALSE;
  }
  else if (DSI_NUM_IP_FAMILIES == info_tbl_ptr->last_addr_count)
  {
    QCRIL_LOG_ERROR("Both address families are already UP cid [%d]",
                    info_tbl_ptr->cid);
    return FALSE;
  }
  else if (TRUE == info_tbl_ptr->is_partial_retry)
  {
    QCRIL_LOG_ERROR("Partial retry already in progress cid [%d]",
                    info_tbl_ptr->cid);
    return FALSE;
  }

  bearer_tech = dsi_get_current_data_bearer_tech(info_tbl_ptr->dsi_hndl);

  if (QCRIL_DATA_IS_DSI_DATA_BEARER_TECH_3GPP(bearer_tech))
  {
    pdn_type = info_tbl_ptr->qdp_3gpp_profile_pdn_type;
  }
  else if (QCRIL_DATA_IS_DSI_DATA_BEARER_TECH_3GPP2(bearer_tech))
  {
    pdn_type = info_tbl_ptr->qdp_3gpp2_profile_pdn_type;
  }

  if (QDP_PROFILE_PDN_TYPE_IPV4 == pdn_type ||
      QDP_PROFILE_PDN_TYPE_IPV6 == pdn_type)
  {
    ret = FALSE;
  }

  QCRIL_LOG_DEBUG("bearer_tech: [%s], Profile PDN type: 3gpp [%d] 3gpp2 [%d], ret: [%s]",
                  qcril_data_util_get_dsi_bearer_tech_string(bearer_tech),
                  info_tbl_ptr->qdp_3gpp_profile_pdn_type,
                  info_tbl_ptr->qdp_3gpp2_profile_pdn_type,
                  ret?"TRUE":"FALSE");

  return ret;
}

/*===========================================================================

  FUNCTION:  qcril_data_util_schedule_partial_retry_attempt

===========================================================================*/
/*!
    @brief
    Create and start a partial retry timer for Dual-IP calls

    @return
    None.

    @note
    info_tbl_mutex must be locked by the caller
*/
/*=========================================================================*/
void qcril_data_util_schedule_partial_retry_attempt
(
  qcril_data_call_info_tbl_type  *info_tbl_ptr,
  boolean                        check_teth_state,
  boolean                        check_ce_reason
)
{
  struct itimerspec itimers;
  unsigned long timeout = 0;
  int  ril_ce_reason;


  QCRIL_LOG_FUNC_ENTRY();

  if (!VALIDATE_LOCAL_DATA_OBJ(info_tbl_ptr))
  {
    QCRIL_LOG_ERROR( "invalid info_tbl_ptr:[%p] ",
                     (unsigned int *)info_tbl_ptr);
    goto bail;
  }
  /* Ensure that the modem profile configuration allows a retry */
  else if (FALSE == qcril_data_util_is_partial_retry_allowed(info_tbl_ptr))
  {
    goto bail;
  }

  /* Check if a retry shouldn't be attempted due to special network errors */
  if (FALSE == qcril_data_util_get_dual_ip_call_status(info_tbl_ptr, &ril_ce_reason))
  {
    QCRIL_LOG_VERBOSE( "Skipping partial retry due to invalid CE reason");
    goto bail;
  }

  if (TRUE == check_ce_reason &&
      TRUE == qcril_data_util_is_ce_failure_permanent(ril_ce_reason))
  {
    QCRIL_LOG_VERBOSE( "Partial retry not allowed due to permanent fail reason=%d",
                       ril_ce_reason );
    goto bail;
  }
  /* Check if a retry shouldn't be attempted due to tethered state */
  else if (TRUE == check_teth_state &&
           PDP_FAIL_TETHERED_CALL_ACTIVE == ril_ce_reason)
  {
    QCRIL_LOG_VERBOSE( "Partial retry not allowed due to active tethered call reason=%d",
                       ril_ce_reason );
    goto bail;
  }
  else if (QCRIL_DATA_INVALID_TIMERID == info_tbl_ptr->retry_timer_id)
  {
    QCRIL_LOG_ERROR( "invalid retry_timer_id:[%#"PRIxPTR"], skipping retry",
                     (uintptr_t)info_tbl_ptr->retry_timer_id);
    goto bail;
  }

  /* If the setup timer_id is active, delete it */
  qcril_data_util_stop_timer(&info_tbl_ptr->timer_id);

  timeout = QCRIL_DATA_UTIL_GET_PARTIAL_RETRY_TIMEOUT(info_tbl_ptr->partial_retry_count);

  if (timeout > qcril_data_max_partial_retry_timeout)
  {
    timeout = qcril_data_max_partial_retry_timeout;
  }

  ++info_tbl_ptr->partial_retry_count;

  /* Set the timeout */
  itimers.it_value.tv_sec     = timeout;
  itimers.it_value.tv_nsec    = 0;
  itimers.it_interval.tv_sec  = 0;
  itimers.it_interval.tv_nsec = 0;

  /* Start the timer */
  if (-1 == timer_settime(info_tbl_ptr->retry_timer_id, 0, &itimers, NULL))
  {
    QCRIL_LOG_ERROR( "failed to start timer for timer_id [%#"PRIxPTR"], deleting... ",
                     (uintptr_t)info_tbl_ptr->retry_timer_id);

    qcril_data_util_stop_timer(&info_tbl_ptr->retry_timer_id);
  }
  else
  {
    QCRIL_LOG_VERBOSE( "call [%d] scheduling next partial retry [%lu] attempt after [%lu] seconds",
                       info_tbl_ptr->index,
                       info_tbl_ptr->partial_retry_count,
                       timeout );
  }

bail:
  QCRIL_LOG_FUNC_RETURN();
}


/*===========================================================================

  FUNCTION:  qcril_data_util_reinitiate_partial_retry

===========================================================================*/
/*!
    @brief
    Go through the info_tbl and check if we need to start partial retry
    attempts for any of the active calls

    @pre Before calling, info_tbl_mutex must be locked by the calling thread

    @return
    None.
*/
/*=========================================================================*/
void qcril_data_util_reinitiate_partial_retry
(
  boolean  check_teth_state,
  boolean  check_ce_reason
)
{
  int i;

  QCRIL_DATA_MUTEX_LOCK(&info_tbl_mutex);

  for (i = 0; i < MAX_CONCURRENT_UMTS_DATA_CALLS; ++i)
  {
    if (!VALIDATE_LOCAL_DATA_OBJ(&info_tbl[i]) ||
        CALL_ID_INVALID == info_tbl[i].cid     ||
        CALL_INACTIVE == info_tbl[i].call_info.active)
    {
      continue;
    }

    /* If there's an active Dual-IP call but with fewer addresses*/
    if (DSI_IP_VERSION_4_6 == info_tbl[i].dsi_ip_version &&
        info_tbl[i].last_addr_count < DSI_NUM_IP_FAMILIES)
    {
      QCRIL_LOG_DEBUG( "scheduling retry for info_tbl index:[%d] ",
                       i);

      info_tbl[i].partial_retry_count = 0;
      qcril_data_util_schedule_partial_retry_attempt(&info_tbl[i],
                                                     check_teth_state,
                                                     check_ce_reason);
    }
  }

  QCRIL_DATA_MUTEX_UNLOCK(&info_tbl_mutex);
}


/*===========================================================================

  FUNCTION:  qcril_data_util_data_sys_status_changed

===========================================================================*/
/*!
    @brief
    Compares the current and new data system status information

    @return
    TRUE  - if the new_sys_status is different from cur_sys_status
    FALSE - otherwise
*/
/*=========================================================================*/
boolean qcril_data_util_data_sys_status_changed
(
  wds_data_system_status_type_v01 *cur_sys_status,
  wds_data_system_status_type_v01 *new_sys_status
)
{
  boolean ret = FALSE;
  int i;
  int network_len;
  wds_sys_network_enum_v01  pref_nw;
  wds_sys_network_info_type_v01 *cur_nw_info = NULL;
  wds_sys_network_info_type_v01 *new_nw_info = NULL;

  /* Validate the input */
  if (!cur_sys_status                                                      ||
      !new_sys_status                                                      ||
      cur_sys_status->network_info_len != new_sys_status->network_info_len ||
      0 == new_sys_status->network_info_len)
  {
    QCRIL_LOG_ERROR("Invalid input");
    goto bail;
  }

  /* Verify that the new system status corresponds to a valid technology */
  network_len = new_sys_status->network_info_len;
  pref_nw = new_sys_status->preferred_network;

  /* Find the new network_info entry corresponding to the preferred network */
  for (i = 0; i < network_len; ++i)
  {
    if (pref_nw == new_sys_status->network_info[i].network)
    {
      new_nw_info = &new_sys_status->network_info[i];
      break;
    }
  }

  if (!new_nw_info)
  {
    goto bail;
  }
  else if (WDS_SYS_NETWORK_3GPP_V01 == pref_nw)
  {
    /* Check if the new 3GPP technology is valid */
    if (QMI_WDS_RAT_DONT_CARE_V01 == new_nw_info->rat_mask ||
        QMI_WDS_RAT_NULL_BEARER_V01 == new_nw_info->rat_mask)
    {
      goto bail;
    }
  }
  else if (WDS_SYS_NETWORK_3GPP2_V01 == pref_nw)
  {
    /* Check if the new 3GPP2 technology is valid */
    if (QMI_WDS_RAT_DONT_CARE_V01 == new_nw_info->rat_mask ||
        QMI_WDS_RAT_NULL_BEARER_V01 == new_nw_info->rat_mask)
    {
      goto bail;
    }
  }

  /* If the preferred network is different */
  if (cur_sys_status->preferred_network != pref_nw)
  {
    ret = TRUE;
  }
  else
  {
    /* Find the current network_info entry corresponding to the preferred network */
    for (i = 0; i < network_len; ++i)
    {
      if (pref_nw == cur_sys_status->network_info[i].network)
      {
        cur_nw_info = &cur_sys_status->network_info[i];
        break;
      }
    }

    if (!cur_nw_info)
    {
      QCRIL_LOG_ERROR("Unable to find network_info entry for pref_network=%d",
                      pref_nw);
      goto bail;
    }

    /* Check if the technology has changed */
    if (WDS_SYS_NETWORK_3GPP_V01 == pref_nw)
    {
      if (cur_nw_info->rat_mask != new_nw_info->rat_mask)
      {
        ret = TRUE;
      }
    }
    else if (WDS_SYS_NETWORK_3GPP2_V01 == pref_nw)
    {
      if ((cur_nw_info->rat_mask != new_nw_info->rat_mask) ||
          (cur_nw_info->so_mask != new_nw_info->so_mask))
      {
        ret = TRUE;
      }
    }
  }


bail:
  return ret;
}

/*===========================================================================

  FUNCTION:  qcril_data_util_update_partial_retry_enabled_flag

===========================================================================*/
/*!
    @brief
    Updates the flag indicating whether partial retries are enabled based on
    the Android property

    @return
    None
*/
/*=========================================================================*/
void qcril_data_util_update_partial_retry_enabled_flag (void)
{
  static char  args[PROPERTY_VALUE_MAX];
  char  def[QCRIL_DATA_PROPERTY_DISABLE_PARTIAL_RETRY_SIZE+1];
  int ret;

  /* Default property value */
  memset(def, 0, sizeof(def));
  strlcpy(def, QCRIL_DATA_PROPERTY_DISABLE_PARTIAL_RETRY_DEFAULT, sizeof(def));

  /* Prepare destination buffer */
  memset(args, 0, sizeof(args));

  /* Read the partial retry property */
  ret = property_get(QCRIL_DATA_PROPERTY_DISABLE_PARTIAL_RETRY, args, def);

  if (ret > PROPERTY_VALUE_MAX)
  {
    QCRIL_LOG_ERROR("property_get for partial_retry returned %d size", ret);
    return;
  }

  QCRIL_LOG_DEBUG("%s property has %s value set on it",
                  QCRIL_DATA_PROPERTY_DISABLE_PARTIAL_RETRY,
                  args);

  /* Check if partial retries should be disabled */
  if (!strcasecmp(args, "true"))
  {
    QCRIL_LOG_DEBUG("disabling partial retries");
    qcril_data_is_partial_retry_disabled = TRUE;
  }
}

/*===========================================================================

  FUNCTION:  qcril_data_util_update_max_partial_retry_timeout

===========================================================================*/
/*!
    @brief
    Updates the max partial retry timeout for QCRIL data calls

    @return
    None
*/
/*=========================================================================*/
void qcril_data_util_update_max_partial_retry_timeout (void)
{
  static char  args[PROPERTY_VALUE_MAX];
  char *end_ptr;
  int ret;
  int len;
  unsigned long ret_val;

  /* Prepare destination buffer */
  memset(args, 0, sizeof(args));

  /* Read the partial retry property */
  ret = property_get(QCRIL_DATA_PROPERTY_MAX_PARTIAL_RETRY_TIMEOUT, args, "");

  if (ret > PROPERTY_VALUE_MAX)
  {
    QCRIL_LOG_ERROR("property_get for max partial_retry timeout returned %d size", ret);
    return;
  }

  len = strlen( args );
  if ( len > 0 )
  {
    ret_val = strtoul( args, &end_ptr, 0 );
    if ( ( errno == ERANGE ) && ( ( ret_val == ULONG_MAX ) || ( ret_val == 0 ) ) )
    {
      QCRIL_LOG_ERROR( "Fail to convert partial retry timeout %s\n", args );
    }
    else
    {
      QCRIL_LOG_DEBUG("%s property has %s value set on it",
                  QCRIL_DATA_PROPERTY_MAX_PARTIAL_RETRY_TIMEOUT,
                  args);
      qcril_data_max_partial_retry_timeout = ret_val;
    }
  }
}

/*===========================================================================

  FUNCTION:  qcril_data_util_is_new_rat_1x_ehrpd

===========================================================================*/
/*!
    @brief
    Check if there is a new RAT change to 1x or eHRPD

    @return
    TRUE - If RAT changed to 1x or eHRPD
    FALSE - otherwise
*/
/*=========================================================================*/
boolean qcril_data_util_is_new_rat_1x_ehrpd
(
  wds_data_system_status_type_v01 *new_sys_status,
  wds_sys_network_info_type_v01  **nw_info
)
{
  boolean ret = FALSE;
  int i;
  int network_len;
  wds_sys_network_enum_v01  pref_nw;
  wds_sys_network_info_type_v01 *new_nw_info = NULL;

  /* Validate the input */
  if (!new_sys_status                       ||
      0 == new_sys_status->network_info_len ||
      !nw_info)
  {
    QCRIL_LOG_ERROR("Invalid input");
    goto bail;
  }

    /* Verify that the new system status corresponds to a valid technology */
  network_len = new_sys_status->network_info_len;
  pref_nw = new_sys_status->preferred_network;

  /* We are only interested in 3GPP2 RAT change */
  if (WDS_SYS_NETWORK_3GPP2_V01 != pref_nw)
  {
    goto bail;
  }

  /* Find the new network_info entry corresponding to the preferred network */
  for (i = 0; i < network_len; ++i)
  {
    if (pref_nw == new_sys_status->network_info[i].network)
    {
      new_nw_info = &new_sys_status->network_info[i];
      break;
    }
  }

  if (!new_nw_info)
  {
    QCRIL_LOG_ERROR("Unable to find network_info entry for pref_network=%d",
                    pref_nw);
    goto bail;
  }

  /* Check if the new 3GPP2 technology is valid */
  if (QMI_WDS_RAT_DONT_CARE_V01 == new_nw_info->rat_mask ||
      QMI_WDS_RAT_NULL_BEARER_V01 == new_nw_info->rat_mask)
  {
    goto bail;
  }

  /* Check if the reported 3GPP2 technology is not FMC */
  if (QMI_WDS_MASK_RAT_FMC_V01 != new_nw_info->rat_mask)
  {
    *nw_info = new_nw_info;
    ret = TRUE;
  }

bail:
  return ret;
}

/*===========================================================================*/
/*!
    @brief
    Register for extended IP config change indication

    @return
    None
*/
/*=========================================================================*/
void qcril_data_util_register_ext_ip_config_ind
(
  qcril_data_call_info_tbl_type  *info_tbl_ptr,
  qmi_client_type qmi_wds_hndl
)
{
  wds_indication_register_req_msg_v01 ext_ip_req;

  QCRIL_LOG_FUNC_ENTRY();

  if (NULL == info_tbl_ptr ||
      QCRIL_DATA_HNDL_INVALID == qmi_wds_hndl)
  {
    QCRIL_LOG_ERROR( "invalid input parameter" );
    goto bail;
  }

  memset(&ext_ip_req, 0, sizeof(ext_ip_req));

  ext_ip_req.report_extended_ip_config_change_valid = TRUE;
  ext_ip_req.report_extended_ip_config_change = TRUE;

  if (E_SUCCESS != qcril_data_qmi_wds_indication_register(qmi_wds_hndl,
                                                          &ext_ip_req))
  {
    QCRIL_LOG_ERROR( "failed to register for ext IP config ind cid=%d",
                     info_tbl_ptr->cid);
  }
  else
  {
    QCRIL_LOG_DEBUG( "successfully registered for ext IP config ind cid=%d",
                     info_tbl_ptr->cid );
  }

bail:
  QCRIL_LOG_FUNC_RETURN();
}

/*===========================================================================*/
/*!
    @brief
    Register for extended IP config change indications and query current info

    @return
    None
*/
/*=========================================================================*/
void qcril_data_util_register_and_query_ext_ip_config_ind
(
  qcril_data_call_info_tbl_type  *info_tbl_ptr
)
{
  if (NULL == info_tbl_ptr)
  {
      QCRIL_LOG_ERROR( "invalid input parameter" );
      return;
  }

  if (QCRIL_DATA_HNDL_INVALID != info_tbl_ptr->qmi_wds_hndl_v4)
  {
    qcril_data_util_register_ext_ip_config_ind(info_tbl_ptr, info_tbl_ptr->qmi_wds_hndl_v4);

    /* Query PCO in case modem already has it before we register */
    QCRIL_LOG_INFO("Query operator reserved PCO v4");
    qcril_data_process_ext_ip_config_ind(info_tbl_ptr->instance_id,
                                         info_tbl_ptr->qmi_wds_hndl_v4,
                                         QMI_WDS_MASK_REQ_SETTINGS_OP_RES_PCO_V01,
                                         info_tbl_ptr);
  }
  if (QCRIL_DATA_HNDL_INVALID != info_tbl_ptr->qmi_wds_hndl_v6)
  {
    qcril_data_util_register_ext_ip_config_ind(info_tbl_ptr, info_tbl_ptr->qmi_wds_hndl_v6);

    /* Query PCO in case modem already has it before we register */
    QCRIL_LOG_INFO("Query operator reserved PCO v6");
    qcril_data_process_ext_ip_config_ind(info_tbl_ptr->instance_id,
                                         info_tbl_ptr->qmi_wds_hndl_v6,
                                         QMI_WDS_MASK_REQ_SETTINGS_OP_RES_PCO_V01,
                                         info_tbl_ptr);

  }
}

/*=========================================================================
  FUNCTION:  qcril_data_util_buffer_dump

===========================================================================*/
/*!
    @brief

    @return
    NONE
*/
/*=========================================================================*/
#if 0
static void qcril_data_util_buffer_dump(
  const dsi_call_param_value_t    *const info
)
{
  int i = 0;
  const dsi_call_param_value_t * tmp = (dsi_call_param_value_t *) info;

  if ( tmp == NULL )
  {
    QCRIL_LOG_ERROR( "%s", "cannot dump null ptr" );
    goto err_label;
  }

  QCRIL_LOG_DEBUG( "%s", ">>>start buffer dump>>>" );

  for( i = 0; i < tmp->num_val; i++ )
  {
    if ( isalpha( ( (char *)tmp->buf_val )[ i ]) )
    {
      QCRIL_LOG_DEBUG( "buffer[%d] = %c", i, ((char *)tmp->buf_val)[ i ] );
    }
    else
    {
      QCRIL_LOG_DEBUG( "buffer[%d] = %p", i, ((unsigned int *)tmp->buf_val)[ i ] );
    }
  }

  QCRIL_LOG_DEBUG( "%s", "<<<end buffer dump<<<" );

err_label:
  return;
}/* qcril_data_buffer_dump() */
#endif



/*=========================================================================
  FUNCTION:  qcril_data_util_is_pending

===========================================================================*/
/*!
    @brief

    @pre caller must have locked info_tbl_mutex prior to calling this function.

    @return
    NONE
*/
/*=========================================================================*/
int qcril_data_util_is_req_pending
(
  const qcril_data_call_info_tbl_type *info_tbl_ptr,
  qcril_evt_e_type *pend_req
)
{
  int ret = FALSE;

  QCRIL_DS_ASSERT_H( info_tbl_ptr != NULL, "validate input ptr");
  QCRIL_DS_ASSERT_H( pend_req     != NULL, "validate ret ptr" );

  if (info_tbl_ptr == NULL || pend_req == NULL)
  {
    return ret;
  }

  if ( ( info_tbl_ptr->pend_tok == NULL ) &&
       ( info_tbl_ptr->pend_req == DS_RIL_REQ_INVALID ) )
  {
    *pend_req = DS_RIL_REQ_INVALID;
    goto err_label;
  }
  else if ( ( info_tbl_ptr->pend_tok != NULL ) &&
            ( info_tbl_ptr->pend_req != DS_RIL_REQ_INVALID ) )
  {
    *pend_req = info_tbl_ptr->pend_req;
    ret = TRUE;
  }
  else
  {
    QCRIL_DS_ASSERT_H( 0, "bad state, pend_tok and pend_req out of sync" );
    goto err_label;
  }

err_label:
  return ret;
}/* qcril_data_util_is_req_pending() */



/*=========================================================================
  FUNCTION:  qcril_data_util_fill_bearer_tech

===========================================================================*/
/*!
    @brief
    converts qcril_data_qmi_wds_data_bearer_tech_type into a valid
    RIL_RadioTechnology and stores it in the info_tbl_ptr

    @pre caller must have locked info_tbl_mutex prior to calling this function.

    @return
    NONE
*/
/*=========================================================================*/
static void qcril_data_util_fill_bearer_tech
(
  qcril_data_call_info_tbl_type    *info_tbl_ptr,
  wds_current_bearer_tech_type_v01 *db_tech_type
)
{
  if (NULL == info_tbl_ptr ||
      NULL == db_tech_type)
  {
    QCRIL_LOG_ERROR("%s","invalid parameters received");
    return;
  }

  switch(db_tech_type->current_nw)
  {
  case WDS_CURRENT_NETWORK_3GPP2_V01:

    if (db_tech_type->rat_mask & QMI_WDS_MASK_RAT_CDMA_1X_V01)
    {
      QCRIL_LOG_DEBUG("call [%d] bearer is [%s]",
                      info_tbl_ptr->index, "CDMA_1X");
      info_tbl_ptr->call_info.radioTech = RADIO_TECH_1xRTT;
      break;
    }

    if (db_tech_type->rat_mask & QMI_WDS_MASK_RAT_EVDO_REVO_V01)
    {
      QCRIL_LOG_DEBUG("call [%d] bearer is [%s]",
                      info_tbl_ptr->index, "CDMA_EVDO_REV0");
      info_tbl_ptr->call_info.radioTech = RADIO_TECH_EVDO_0;
      break;
    }

    if (db_tech_type->rat_mask & QMI_WDS_MASK_RAT_EVDO_REVA_V01)
    {
      QCRIL_LOG_DEBUG("call [%d] bearer is [%s]",
                      info_tbl_ptr->index, "CDMA_EVDO_REVA");
      info_tbl_ptr->call_info.radioTech = RADIO_TECH_EVDO_A;
      break;
    }

    /* 0x08 and 0x20 are EHRPD enumerations in QMI */
    if (db_tech_type->so_mask & QMI_WDS_MASK_SO_EVDO_REVA_EMPA_EHRPD_V01 ||
        db_tech_type->so_mask & QMI_WDS_MASK_SO_EVDO_REVB_EMPA_EHRPD_V01 ||
        db_tech_type->so_mask & QMI_WDS_MASK_SO_EVDO_REVB_MMPA_EHRPD_V01)
    {
      QCRIL_LOG_DEBUG("call [%d] bearer is [%s]",
                      info_tbl_ptr->index, "EHRPD");
      info_tbl_ptr->call_info.radioTech = RADIO_TECH_EHRPD;
      break;
    }

    /* we reach here, that means we have not found
     any of the radio technologies we care about */
    QCRIL_LOG_ERROR("QMI returned unknown CDMA rat mask [%d]",
                    db_tech_type->rat_mask);
    break;

  case WDS_CURRENT_NETWORK_3GPP_V01:

    if (db_tech_type->rat_mask & QMI_WDS_MASK_RAT_WCDMA_V01)
    {
      QCRIL_LOG_DEBUG("call [%d] bearer is [%s]",
                      info_tbl_ptr->index, "WCDMA");
      info_tbl_ptr->call_info.radioTech = RADIO_TECH_UMTS;
      break;
    }

    if (db_tech_type->rat_mask & QMI_WDS_MASK_RAT_GPRS_V01)
    {
      QCRIL_LOG_DEBUG("call [%d] bearer is [%s]",
                      info_tbl_ptr->index, "GPRS");
      info_tbl_ptr->call_info.radioTech = RADIO_TECH_GPRS;
      break;
    }

    if (db_tech_type->rat_mask & QMI_WDS_MASK_RAT_HSDPA_V01)
    {
      QCRIL_LOG_DEBUG("call [%d] bearer is [%s]",
                      info_tbl_ptr->index, "HSDPA");
      info_tbl_ptr->call_info.radioTech = RADIO_TECH_HSDPA;
      break;
    }

    if (db_tech_type->rat_mask & QMI_WDS_MASK_RAT_HSUPA_V01)
    {
      QCRIL_LOG_DEBUG("call [%d] bearer is [%s]",
                      info_tbl_ptr->index, "HSUPA");
      info_tbl_ptr->call_info.radioTech = RADIO_TECH_HSUPA;
      break;
    }

    if (db_tech_type->rat_mask & QMI_WDS_MASK_RAT_EDGE_V01)
    {
      info_tbl_ptr->call_info.radioTech = RADIO_TECH_EDGE;
      break;
    }

    if (db_tech_type->rat_mask & QMI_WDS_MASK_RAT_LTE_V01)
    {
      info_tbl_ptr->call_info.radioTech = RADIO_TECH_LTE;
      break;
    }

    if (db_tech_type->rat_mask & QMI_WDS_MASK_RAT_TDSCDMA_V01)
    {
      info_tbl_ptr->call_info.radioTech = RADIO_TECH_TD_SCDMA;
      break;
    }

    /* we reach here, that means we have not found
     any of the radio technologies we care about */
    QCRIL_LOG_ERROR("QMI returned unknown UMTS rat mask [%d]",
                    db_tech_type->rat_mask);
    break;
  default:
    QCRIL_LOG_ERROR("%s","QMI returned unknown bearer technology");
    break;
  }
}



/*=========================================================================
  FUNCTION:  qcril_data_util_fill_bearer_tech_ex

===========================================================================*/
/*!
    @brief
    converts qmi_wds_data_bearer_tech_type into a valid
    RIL_RadioTechnology and stores it in the info_tbl_ptr

    @pre caller must have locked info_tbl_mutex prior to calling this function.

    @return
    NONE
*/
/*=========================================================================*/
static void qcril_data_util_fill_bearer_tech_ex
(
  qcril_data_call_info_tbl_type * info_tbl_ptr,
  wds_bearer_tech_info_type_v01 * db_tech_type_ex
)
{
  if (NULL == info_tbl_ptr ||
      NULL == db_tech_type_ex)
  {
    QCRIL_LOG_ERROR("%s","invalid parameters received");
    return;
  }

  if (WDS_BEARER_TECH_NETWORK_3GPP2_V01 == db_tech_type_ex->technology)
  {
    if (WDS_BEARER_TECH_RAT_EX_3GPP2_1X_V01 == db_tech_type_ex->rat_value)
    {
      info_tbl_ptr->call_info.radioTech = RADIO_TECH_1xRTT;
    }
    else if (WDS_BEARER_TECH_RAT_EX_3GPP2_HRPD_V01 == db_tech_type_ex->rat_value)
    {
      if (QMI_WDS_3GPP2_SO_MASK_HDR_REV0_DPA_V01 & db_tech_type_ex->so_mask)
      {
        info_tbl_ptr->call_info.radioTech = RADIO_TECH_EVDO_0;
      }
      else if ((QMI_WDS_3GPP2_SO_MASK_HDR_REVA_DPA_V01 & db_tech_type_ex->so_mask) ||
               (QMI_WDS_3GPP2_SO_MASK_HDR_REVA_MPA_V01 & db_tech_type_ex->so_mask) ||
               (QMI_WDS_3GPP2_SO_MASK_HDR_REVA_EMPA_V01 & db_tech_type_ex->so_mask))
      {
        info_tbl_ptr->call_info.radioTech = RADIO_TECH_EVDO_A;
      }
      else if ((QMI_WDS_3GPP2_SO_MASK_HDR_REVB_DPA_V01 & db_tech_type_ex->so_mask) ||
               (QMI_WDS_3GPP2_SO_MASK_HDR_REVB_MPA_V01 & db_tech_type_ex->so_mask) ||
               (QMI_WDS_3GPP2_SO_MASK_HDR_REVB_EMPA_V01 & db_tech_type_ex->so_mask)||
               (QMI_WDS_3GPP2_SO_MASK_HDR_REVB_MMPA_V01 & db_tech_type_ex->so_mask))

      {
        info_tbl_ptr->call_info.radioTech = RADIO_TECH_EVDO_B;
      }
    }
    else if (WDS_BEARER_TECH_RAT_EX_3GPP2_EHRPD_V01 == db_tech_type_ex->rat_value)
    {
      info_tbl_ptr->call_info.radioTech = RADIO_TECH_EHRPD;
    }
    else if (WDS_BEARER_TECH_RAT_EX_3GPP2_WLAN_V01 == db_tech_type_ex->rat_value)
    {
      info_tbl_ptr->call_info.radioTech = RADIO_TECH_IWLAN;
    }
    else
    {
      /* we reach here, that means we have not found
         any of the radio technologies we care about */
      QCRIL_LOG_ERROR("QMI returned unknown 3GPP2 rat mask [%d]",
                    db_tech_type_ex->rat_value);
    }
  }
  else if (WDS_BEARER_TECH_NETWORK_3GPP_V01 == db_tech_type_ex->technology)
  {
    if (WDS_BEARER_TECH_RAT_EX_3GPP_WCDMA_V01 == db_tech_type_ex->rat_value)
    {
      if((QMI_WDS_3GPP_SO_MASK_HSDPAPLUS_V01 & db_tech_type_ex->so_mask) ||
         (QMI_WDS_3GPP_SO_MASK_DC_HSDPAPLUS_V01 & db_tech_type_ex->so_mask) ||
         (QMI_WDS_3GPP_SO_MASK_64_QAM_V01 & db_tech_type_ex->so_mask))
      {
        info_tbl_ptr->call_info.radioTech = RADIO_TECH_HSPAP;
      }
      else if ((QMI_WDS_3GPP_SO_MASK_HSPA_V01 & db_tech_type_ex->so_mask) ||
               ((QMI_WDS_3GPP_SO_MASK_HSUPA_V01 & db_tech_type_ex->so_mask) &&
                (QMI_WDS_3GPP_SO_MASK_HSDPA_V01 & db_tech_type_ex->so_mask)))
      {
        info_tbl_ptr->call_info.radioTech = RADIO_TECH_HSPA;
      }
      else if (QMI_WDS_3GPP_SO_MASK_HSUPA_V01 & db_tech_type_ex->so_mask)
      {
        info_tbl_ptr->call_info.radioTech = RADIO_TECH_HSUPA;
      }
      else if (QMI_WDS_3GPP_SO_MASK_HSDPA_V01 & db_tech_type_ex->so_mask)
      {
        info_tbl_ptr->call_info.radioTech = RADIO_TECH_HSDPA;
      }
      else
      {
        info_tbl_ptr->call_info.radioTech = RADIO_TECH_UMTS;
      }
    }
    else if (WDS_BEARER_TECH_RAT_EX_3GPP_GERAN_V01 == db_tech_type_ex->rat_value)
    {
      if (QMI_WDS_3GPP_SO_MASK_EDGE_V01 & db_tech_type_ex->so_mask)
      {
        info_tbl_ptr->call_info.radioTech = RADIO_TECH_EDGE;
      }
      else if (QMI_WDS_3GPP_SO_MASK_GPRS_V01 & db_tech_type_ex->so_mask)
      {
        info_tbl_ptr->call_info.radioTech = RADIO_TECH_GPRS;
      }
      else if (QMI_WDS_3GPP_SO_MASK_GSM_V01 & db_tech_type_ex->so_mask)
      {
        info_tbl_ptr->call_info.radioTech = RADIO_TECH_GSM;
      }
    }
    else if (WDS_BEARER_TECH_RAT_EX_3GPP_LTE_V01 == db_tech_type_ex->rat_value)
    {
      info_tbl_ptr->call_info.radioTech = RADIO_TECH_LTE;
      if ((QMI_WDS_3GPP_SO_MASK_LTE_CA_DL_V01 == db_tech_type_ex->so_mask) ||
          (QMI_WDS_3GPP_SO_MASK_LTE_CA_UL_V01 == db_tech_type_ex->so_mask))
      {
        info_tbl_ptr->call_info.radioTech = RADIO_TECH_LTE_CA;
      }
    }
    else if (WDS_BEARER_TECH_RAT_EX_3GPP_TDSCDMA_V01 == db_tech_type_ex->rat_value)
    {
      info_tbl_ptr->call_info.radioTech = RADIO_TECH_TD_SCDMA;
    }
    else if (WDS_BEARER_TECH_RAT_EX_3GPP_WLAN_V01 == db_tech_type_ex->rat_value &&
             QMI_WDS_3GPP_SO_MASK_S2B_V01 == db_tech_type_ex->so_mask)
    {
      info_tbl_ptr->call_info.radioTech = RADIO_TECH_IWLAN;
    }
    else
    {
      /* we reach here, that means we have not found
       any of the radio technologies we care about */
      QCRIL_LOG_ERROR("QMI returned unknown 3GPP rat mask [%d]",
                      db_tech_type_ex->rat_value);
    }
  }
  else
  {
    QCRIL_LOG_ERROR("QMI returned unknown bearer technology=[%d]",
                    db_tech_type_ex->technology);
  }
}



/*=========================================================================
  FUNCTION:  qcril_data_util_fill_call_type

===========================================================================*/
/*!
    @brief
    Stores the call type in the info_tbl_ptr

    @pre caller must have locked info_tbl_mutex prior to calling this function.

    @return
    NONE
*/
/*=========================================================================*/
static void qcril_data_util_fill_call_type
(
  qcril_data_call_info_tbl_type  *info_tbl_ptr
)
{
  if (NULL == info_tbl_ptr)
  {
    QCRIL_LOG_ERROR("%s","invalid parameters received");
    return;
  }

  switch (info_tbl_ptr->dsi_ip_version)
  {
  case DSI_IP_VERSION_4:
      strlcpy(info_tbl_ptr->call_info.type,
              QCRIL_DATA_IP_FAMILY_V4,
              sizeof(info_tbl_ptr->call_info.type));
      break;

    case DSI_IP_VERSION_6:
      strlcpy(info_tbl_ptr->call_info.type,
              QCRIL_DATA_IP_FAMILY_V6,
              sizeof(info_tbl_ptr->call_info.type));
      break;

    case DSI_IP_VERSION_4_6:
      strlcpy(info_tbl_ptr->call_info.type,
              QCRIL_DATA_IP_FAMILY_V4V6,
              sizeof(info_tbl_ptr->call_info.type));
      break;

    default:
      QCRIL_LOG_ERROR("Invalid IP version=%d", info_tbl_ptr->dsi_ip_version);
      break;
  }
}


/*===========================================================================*/
/*!
    @brief
    Update the MTU in the call_info table

    @return
    None
*/
/*=========================================================================*/
void qcril_data_util_update_mtu
(
  qcril_data_call_info_tbl_type  *info_tbl_ptr
)
{
#if (QCRIL_RIL_VERSION >= 11)
  unsigned int mtu;

  if (NULL == info_tbl_ptr)
  {
    QCRIL_DS_ASSERT(0, "programming error: NULL info_tbl_ptr passed");
    return;
  }

  /* Update iface MTU */
  if (qcril_data_mtu > 0)
  {
    info_tbl_ptr->call_info.mtu = qcril_data_mtu;
  }
  else if (DSI_SUCCESS != dsi_get_link_mtu(info_tbl_ptr->dsi_hndl, &mtu))
  {
    QCRIL_LOG_ERROR( "%s", "failed to get link MTU" );
  }
  else
  {
    info_tbl_ptr->call_info.mtu = mtu;
  }

  QCRIL_LOG_INFO( "MTU set to %d", info_tbl_ptr->call_info.mtu );
#else
  (void) info_tbl_ptr;
#endif

  return;
}


#if (QCRIL_RIL_VERSION >= 6)

/*=========================================================================
  FUNCTION:  qcril_data_util_format_sockaddr

===========================================================================*/
/*!
    @brief
    This function formats the given sockaddr to a string and prepends a space
    delimiter (if necessary)

    @pre caller must have locked info_tbl_mutex prior to calling this function.

    @param
    addr               [in]    - sockaddr to convert to string
    prefix_len         [in]    - prefix length
    prefix_len_valid   [in]    - if given prefix_len is valid or not
    addr_str           [out]   - storage for the formatted address string
    avail_len          [inout] - size of available storage in addr_str before and
                                 after storing the formatted address string
    preprend_delimiter [in]    - if a delimiter should be prepended to the address
                                 string

    @return
    None
*/
/*=========================================================================*/
static void qcril_data_util_format_sockaddr
(
  struct sockaddr_storage  *addr,
  unsigned int             prefix_len,
  boolean                  prefix_len_valid,
  char                     *addr_str,
  int                      addr_str_size,
  boolean                  prepend_delimiter
)
{
  char fmtstr[DS_CALL_INFO_ADDR_IPV6_MAX_LEN + 1] = "";

  if (!addr || !addr_str || addr_str_size <= 0)
  {
    QCRIL_LOG_ERROR("%s", "qcril_data_util_format_sockaddr: bad param(s)");
    return;
  }

  QCRIL_LOG_DEBUG("prefix_len=%u, prefix_len_valid=%d, prepend_delimiter=%d",
                  prefix_len, prefix_len_valid, prepend_delimiter);

  if (prefix_len_valid)
  {
    DS_CALL_ADDR_FORMAT_IPV4V6_WITH_PREFIX_LEN( addr,
                                                prefix_len,
                                                fmtstr,
                                                sizeof(fmtstr) );
  }
  else
  {
    DS_CALL_ADDR_FORMAT_IPV4V6( addr,
                                fmtstr,
                                sizeof(fmtstr) );
  }

  QCRIL_LOG_DEBUG("formatted address string=%s", fmtstr);

  strlcat(addr_str,
          prepend_delimiter ? QCRIL_DATA_ADDR_DELIMITER : "",
          addr_str_size);

  strlcat(addr_str,
          fmtstr,
          addr_str_size);
}


/*=========================================================================
  FUNCTION:  qcril_data_util_dsi_addr_info_to_str

===========================================================================*/
/*!
    @brief
    Converts the IP, gateway, DNS addresses in the given array of addr_info
    structures into a space delimited string and stores the corresponding
    value in the given call_info table entry.

    @param
    addr_info [in]     - array of dsi_addr_info_t structures to be converted
    num_addr_info [in] - number of elements of the addr_info array
    call_info [out]    - storage for the converted IP, Gateway and DNS address
                         strings

    @pre caller must have locked info_tbl_mutex prior to calling this function.

    @return
    SUCCESS - If conversion to string was successful
    FAILURE - Otherwise
*/
/*=========================================================================*/
static int qcril_data_util_dsi_addr_info_to_str
(
  dsi_addr_info_t *addr_info,
  int num_addr_info,
  qcril_data_call_info_t *call_info
)
{
  int i;
  int ret = FAILURE;
  int address_avail_len;
  int gateway_avail_len;
  int dns_avail_len;

  if (!addr_info || num_addr_info <= 0 || !call_info)
  {
    QCRIL_LOG_ERROR("qcril_data_util_dsi_addr_to_str: bad param(s) "
                    "addr_info=0x%x, num_addr_info=%d, call_info=0x%x",
                    addr_info, num_addr_info, call_info);
    return FAILURE;
  }

  address_avail_len = sizeof(call_info->address.fmtstr);
  gateway_avail_len = sizeof(call_info->gateway.fmtstr);
  dns_avail_len     = sizeof(call_info->dns.fmtstr);

  memset(call_info->address.fmtstr, 0x0, address_avail_len);
  memset(call_info->gateway.fmtstr, 0x0, gateway_avail_len);
  memset(call_info->dns.fmtstr, 0x0, dns_avail_len);

  for (i = 0; i < num_addr_info; ++i)
  {
    /* Copy iface address if valid */
    if ( addr_info[i].iface_addr_s.valid_addr )
    {
      qcril_data_util_format_sockaddr(&addr_info[i].iface_addr_s.addr,
                                      addr_info[i].iface_mask,
                                      TRUE,
                                      call_info->address.fmtstr,
                                      address_avail_len,
                                      (i > 0) ? TRUE : FALSE);
    }
    else
    {
      QCRIL_LOG_ERROR("qcril_data_util_dsi_addr_to_str: iface address invalid [%d]",
                      i);
      goto bail;
    }

    /* Copy gateway address if valid */
    if ( addr_info[i].gtwy_addr_s.valid_addr )
    {
      qcril_data_util_format_sockaddr(&addr_info[i].gtwy_addr_s.addr,
                                      addr_info[i].gtwy_mask,
                                      FALSE,
                                      call_info->gateway.fmtstr,
                                      gateway_avail_len,
                                      (i > 0) ? TRUE : FALSE);
    }
    else
    {
      QCRIL_LOG_ERROR("qcril_data_util_dsi_addr_to_str: gateway address invalid [%d]",
                      i);
    }

    /* Copy DNS primary address if valid */
    if ( addr_info[i].dnsp_addr_s.valid_addr )
    {
      qcril_data_util_format_sockaddr(&addr_info[i].dnsp_addr_s.addr,
                                      0,
                                      FALSE,
                                      call_info->dns.fmtstr,
                                      dns_avail_len,
                                      (i > 0) ? TRUE : FALSE);

      /* Copy DNS secondary address if valid */
      if ( addr_info[i].dnss_addr_s.valid_addr )

      {
        /* Always prepend a delimiter between primary and secondary DNS addresses */
        qcril_data_util_format_sockaddr(&addr_info[i].dnss_addr_s.addr,
                                        0,
                                        FALSE,
                                        call_info->dns.fmtstr,
                                        dns_avail_len,
                                        TRUE);
      }
    }
  }

  ret = SUCCESS;

bail:
  return ret;
}
#endif /* (QCRIL_RIL_VERSION >= 6) */

/*=========================================================================
  FUNCTION:  qcril_data_util_fill_call_params

===========================================================================*/
/*!
    @brief
    Uses QMI WDS service to get current call params and fill into
    the call table

    @pre caller must have locked info_tbl_mutex prior to calling this function.

    @return
    TRUE: Call params retrieval success
    FALSE: Call params retrieval failed
*/
/*=========================================================================*/
boolean qcril_data_util_fill_call_params
(
  qcril_data_call_info_tbl_type *info_tbl_ptr
)
{
  wds_get_data_bearer_technology_ex_resp_msg_v01 bearer_tech_ex_rsp;
  wds_get_current_data_bearer_technology_resp_msg_v01 bearer_tech_rsp;
  int rc;
  int ret = FAILURE;
  int reti = SUCCESS;
  char buf[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+1];
  dsi_addr_info_t *addr_info = NULL;
  unsigned int count;

  QCRIL_LOG_DEBUG("%s","qcril_data_util_fill_call_params: ENTRY");

  memset(&bearer_tech_ex_rsp, 0, sizeof(bearer_tech_ex_rsp));
  memset(&bearer_tech_rsp, 0, sizeof(bearer_tech_rsp));

  do
  {
    if (NULL == info_tbl_ptr)
    {
      QCRIL_DS_ASSERT(0, "programming error: NULL info_tbl_ptr passed");
      break;
    }

    /***** Fill CID *******/
    snprintf( info_tbl_ptr->call_info.cid, 4, "%d", info_tbl_ptr->cid);

    /****** Fill the call type ******/
    qcril_data_util_fill_call_type(info_tbl_ptr);

    /* Fill device name */
    QCRIL_LOG_DEBUG("Get device name and fill the value at [%p]",
                    buf);
    rc = dsi_get_device_name(info_tbl_ptr->dsi_hndl,
                             buf,
                             DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+1);
    if (DSI_SUCCESS != rc)
    {
      QCRIL_LOG_DEBUG("couldn't get device name for info_tbl [%d]",
                      info_tbl_ptr->index);
      QCRIL_DS_ASSERT( 0, "programming error: dsi_get_device_name" );
      break;
    }
    /* Copy according to presentation layer, limit size to IFNAMSIZ  */
    memcpy( info_tbl_ptr->call_info.dev_name, buf, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN );
    info_tbl_ptr->call_info.dev_name[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN] = '\0';
    QCRIL_LOG_DEBUG( "dev_name [%s]", info_tbl_ptr->call_info.dev_name);
    /* this info_tbl_ptr now contains a valid device name */
    info_tbl_ptr->info_flg = TRUE;

    /******* Fill ip address ******/
#if (QCRIL_RIL_VERSION >= 6)
    count = dsi_get_ip_addr_count(info_tbl_ptr->dsi_hndl);
#else
    count = 1;
#endif

    info_tbl_ptr->last_addr_count = count;

    if (0 == count)
    {
      QCRIL_LOG_DEBUG("couldn't get valid count of addr info for info_tbl [%d]",
                      info_tbl_ptr->index);
      break;
    }

    addr_info = calloc(count, sizeof(dsi_addr_info_t));

    if (NULL == addr_info)
    {
      QCRIL_LOG_ERROR("couldn't allocate addr info for info_tbl [%d] count [%u]",
                      info_tbl_ptr->index, count);
      break;
    }

    rc = dsi_get_ip_addr(info_tbl_ptr->dsi_hndl, addr_info, count);
    if (DSI_SUCCESS != rc)
    {
      QCRIL_LOG_ERROR("couldn't get addr info for info_tbl [%d] err [%d]",
                      info_tbl_ptr->index, rc);
      break;
    }

#if (QCRIL_RIL_VERSION >= 6)
    /* Convert addr_info[] to string */
    rc = qcril_data_util_dsi_addr_info_to_str(addr_info, count, &info_tbl_ptr->call_info);
    if (SUCCESS != rc)
    {
      QCRIL_LOG_ERROR("couldn't convert addr info to str info_tbl [%d] err [%d]",
                      info_tbl_ptr->index, rc);
      break;
    }

    QCRIL_LOG_DEBUG("IP Addresses     : %s", info_tbl_ptr->call_info.address.fmtstr);
    QCRIL_LOG_DEBUG("Gateway Addresses: %s", info_tbl_ptr->call_info.gateway.fmtstr);
    QCRIL_LOG_DEBUG("DNS Addresses    : %s", info_tbl_ptr->call_info.dns.fmtstr);
    QCRIL_LOG_DEBUG("Call Type        : %s", info_tbl_ptr->call_info.type);

#if (QCRIL_RIL_VERSION >= 10)
    qcril_data_util_fill_pcscf_addr(info_tbl_ptr);
#endif

#if (QCRIL_RIL_VERSION >= 11)
    qcril_data_util_update_mtu(info_tbl_ptr);

#endif

    QCRIL_NOTUSED(reti);
#else
    /* prepare the network ordered ip addr for presentation */
    reti = SUCCESS;
    switch( SASTORAGE_FAMILY(addr_info->iface_addr_s.addr) )
    {
    case AF_INET:
      QCRIL_LOG_DEBUG("IPv4 address found. data buf is [%d] long",
                      sizeof( SASTORAGE_DATA(addr_info->iface_addr_s.addr) ));
      snprintf(info_tbl_ptr->call_info.address,
               sizeof(info_tbl_ptr->call_info.address),
               "%d.%d.%d.%d/%d%c",
               SASTORAGE_DATA(addr_info.iface_addr_s.addr)[0],
               SASTORAGE_DATA(addr_info.iface_addr_s.addr)[1],
               SASTORAGE_DATA(addr_info.iface_addr_s.addr)[2],
               SASTORAGE_DATA(addr_info.iface_addr_s.addr)[3],
               addr_info.iface_mask, '\0');
      QCRIL_LOG_DEBUG("IPv4 address [%s] populated in info_tbl[%d]",
                      info_tbl_ptr->call_info.address,
                      info_tbl_ptr->index);
      break;
    case AF_INET6:
      QCRIL_LOG_DEBUG("IPv6 address found. data buf is [%d] long",
                      sizeof(SASTORAGE_DATA(addr_info->iface_addr_s.addr)));
      snprintf(info_tbl_ptr->call_info.address,
               sizeof(info_tbl_ptr->call_info.address),
               "%.2x%.2x:%.2x%.2x:%.2x%.2x:%.2x%.2x:"
               "%.2x%.2x:%.2x%.2x:%.2x%.2x:%.2x%.2x/%d%c",
               SASTORAGE_DATA(addr_info.iface_addr_s.addr)[0],
               SASTORAGE_DATA(addr_info.iface_addr_s.addr)[1],
               SASTORAGE_DATA(addr_info.iface_addr_s.addr)[2],
               SASTORAGE_DATA(addr_info.iface_addr_s.addr)[3],
               SASTORAGE_DATA(addr_info.iface_addr_s.addr)[4],
               SASTORAGE_DATA(addr_info.iface_addr_s.addr)[5],
               SASTORAGE_DATA(addr_info.iface_addr_s.addr)[6],
               SASTORAGE_DATA(addr_info.iface_addr_s.addr)[7],
               SASTORAGE_DATA(addr_info.iface_addr_s.addr)[8],
               SASTORAGE_DATA(addr_info.iface_addr_s.addr)[9],
               SASTORAGE_DATA(addr_info.iface_addr_s.addr)[10],
               SASTORAGE_DATA(addr_info.iface_addr_s.addr)[11],
               SASTORAGE_DATA(addr_info.iface_addr_s.addr)[12],
               SASTORAGE_DATA(addr_info.iface_addr_s.addr)[13],
               SASTORAGE_DATA(addr_info.iface_addr_s.addr)[14],
               SASTORAGE_DATA(addr_info.iface_addr_s.addr)[15],
               addr_info.iface_mask, '\0');
      QCRIL_LOG_DEBUG("IPv6 address [%s] populated in info_tbl[%d]",
                      info_tbl_ptr->call_info.address,
                      info_tbl_ptr->index);
      break;
    default:
      QCRIL_LOG_ERROR("invalid/unsupported ss_family [%d]",
                      SASTORAGE_FAMILY(addr_info->iface_addr_s.addr));
      sprintf( info_tbl_ptr->call_info.address, "%s", "unknown\0" );
      reti = FAILURE;
      break;
    }
    if (SUCCESS != reti)
    {
      break;
    }
#endif /* (QCRIL_RIL_VERSION >= 6) */

    /****** fill in the current bearer tech ******/

    if(E_SUCCESS != qcril_data_qmi_wds_get_current_bearer_tech_ex(info_tbl_ptr->qmi_wds_hndl,
                                                                  &bearer_tech_ex_rsp))
    {
      QCRIL_LOG_ERROR("%s", "qcril_data_qmi_wds_get_current_bearer_tech_ex failed");

      QCRIL_LOG_DEBUG("%s", "Fallback on qcril_data_qmi_wds_get_current_bearer_tech");

      ret = SUCCESS;
      if(E_SUCCESS != qcril_data_qmi_wds_get_current_bearer_tech(info_tbl_ptr->qmi_wds_hndl,
                                                                 &bearer_tech_rsp))
      {
        QCRIL_LOG_ERROR("%s", "qcril_data_qmi_wds_get_current_bearer_tech failed");
        break;
      }

      qcril_data_util_fill_bearer_tech(info_tbl_ptr,
                                       &bearer_tech_rsp.current_bearer_tech);
    }
    else
    {
      qcril_data_util_fill_bearer_tech_ex(info_tbl_ptr,
                                          &bearer_tech_ex_rsp.bearer_tech);
      ret = SUCCESS;
    }

    ret = SUCCESS;
  } while(0);

  QCRIL_DATA_RELEASE_STORAGE(addr_info);

  if (SUCCESS == ret)
  {
    QCRIL_LOG_DEBUG("%s","qcril_data_util_fill_call_params: EXIT success");
    return TRUE;
  }
  else
  {
    QCRIL_LOG_DEBUG("%s","qcril_data_util_fill_call_params: EXIT error");
    return FALSE;
  }
}/* qcril_data_util_fill_call_params() */



#if (QCRIL_RIL_VERSION >= 6)
/*=========================================================================
  FUNCTION:  qcril_data_util_create_setup_rsp

===========================================================================*/
/*!
    @brief
    Create RIL REQ SETUP response.

    @pre caller must have locked info_tbl_mutex prior to calling this function.

    @return
    NONE
*/
/*=========================================================================*/
void qcril_data_util_create_setup_rsp
(
  qcril_data_call_info_tbl_type *info_tbl_ptr,
  qcril_data_call_response_type *rsp_ptr
)
{
  /* validate input parameter */
  if(!info_tbl_ptr || !rsp_ptr)
  {
    QCRIL_LOG_ERROR("qcril_data_util_create_setup_rsp: bad param(s) "
                    "info_tbl_ptr=0x%x, rsp_ptr=0x%x",
                    info_tbl_ptr, rsp_ptr);
    return;
  }

  /* Transfer the call state */
  rsp_ptr->setup_rsp.status = info_tbl_ptr->status;
  rsp_ptr->setup_rsp.suggestedRetryTime = info_tbl_ptr->suggestedRetryTime;
  rsp_ptr->setup_rsp.cid = info_tbl_ptr->cid;
  rsp_ptr->setup_rsp.active = info_tbl_ptr->call_info.active;
  rsp_ptr->setup_rsp.type = info_tbl_ptr->call_info.type;
  rsp_ptr->setup_rsp.ifname = info_tbl_ptr->call_info.dev_name;
  rsp_ptr->setup_rsp.addresses = info_tbl_ptr->call_info.address.fmtstr;
  rsp_ptr->setup_rsp.dnses = info_tbl_ptr->call_info.dns.fmtstr;
  rsp_ptr->setup_rsp.gateways = info_tbl_ptr->call_info.gateway.fmtstr;

#if (QCRIL_RIL_VERSION >= 10)

  rsp_ptr->setup_rsp.pcscf = info_tbl_ptr->call_info.pcscf.fmtstr;

#endif

#if (QCRIL_RIL_VERSION >= 11)

  rsp_ptr->setup_rsp.mtu = info_tbl_ptr->call_info.mtu;

#endif

  rsp_ptr->size               = sizeof(rsp_ptr->setup_rsp);

}/* qcril_data_util_create_setup_rsp() */

#else
/*=========================================================================
  FUNCTION:  qcril_data_util_create_setup_rsp

===========================================================================*/
/*!
    @brief
    Create RIL REQ SETUP response.

    @pre caller must have locked info_tbl_mutex prior to calling this function.

    @return
    NONE
*/
/*=========================================================================*/
void qcril_data_util_create_setup_rsp
(
  qcril_data_call_info_tbl_type *info_tbl_ptr,
  qcril_data_call_response_type *rsp_ptr
)
{
  /* validate input parameter */
  if(!info_tbl_ptr || !rsp_ptr)
  {
    QCRIL_LOG_ERROR("qcril_data_util_create_setup_rsp: bad param(s) "
                    "info_tbl_ptr=0x%x, rsp_ptr=0x%x",
                    info_tbl_ptr, rsp_ptr);
    return;
  }

  rsp_ptr->setup_rsp.cid      = info_tbl_ptr->call_info.cid;
  rsp_ptr->setup_rsp.dev_name = info_tbl_ptr->call_info.dev_name;
  rsp_ptr->setup_rsp.ip_addr  = info_tbl_ptr->call_info.address;
  rsp_ptr->size               = sizeof rsp_ptr->setup_rsp ;
}/* qcril_data_util_create_setup_rsp() */
#endif /* (QCRIL_RIL_VERSION >= 6) */


/*===========================================================================

  FUNCTION:   qcril_data_get_dsi_ip_version

===========================================================================*/
/*!
    @brief
    This function retrieves DSI data call IP version for the 1st address

    @return DSI_IP_VERSION_4 or DSI_IP_VERSION_6
*/
/*=========================================================================*/
int qcril_data_get_dsi_ip_version
(
  dsi_hndl_t dsi_hndl
)
{
  dsi_addr_info_t  addr_info;
  int              ip_version = DSI_IP_VERSION_4; // XXX

  memset(&addr_info, 0, sizeof(addr_info));

  // Retrieve the 1st available IP address
  if ((DSI_SUCCESS != dsi_get_ip_addr(dsi_hndl, &addr_info, 1)) ||
      (!addr_info.iface_addr_s.valid_addr))
  {
    QCRIL_LOG_ERROR("Can't get IP address from dsi hndl [0x%x]", dsi_hndl);
    return ip_version;
  }

  switch (SASTORAGE_FAMILY(addr_info.iface_addr_s.addr))
  {
    case AF_INET:
        ip_version = DSI_IP_VERSION_4;
        break;

    case AF_INET6:
        ip_version = DSI_IP_VERSION_6;
        break;

    default:
        QCRIL_LOG_ERROR("Unknown address family");
        break;
   }

   QCRIL_LOG_INFO("dsi hndl [0x%x] IP addr version (1st) is %d",
                           dsi_hndl, ip_version);

   return ip_version;
}

#if (QCRIL_RIL_VERSION >= 10)
/*=========================================================================
  FUNCTION:  qcril_data_util_fill_pcscf_addr

===========================================================================*/
/*!
    @brief
    Uses DSI to get current PCSCF server addresses and fill into
    the call table

    @pre caller must have locked info_tbl_mutex prior to calling this function.

    @return
    NONE
*/
/*=========================================================================*/
void qcril_data_util_fill_pcscf_addr
(
  qcril_data_call_info_tbl_type *info_tbl_ptr
)
{
  dsi_pcscf_addr_info_t *pcscf_addr_list = NULL;
  struct sockaddr_in *sin = NULL;
  struct sockaddr_in6 *sin6 = NULL;
  char addr_buf[50];
  const char *addr_ptr = NULL;
  int rc;
  int err_code = DSI_ERROR;
  unsigned int i;
  unsigned int k = 0;
  char *fmtstr_ptr;
  size_t fmtstr_size;

  if (NULL == info_tbl_ptr)
  {
    QCRIL_LOG_ERROR("Null info tbl ptr");
    return;
  }

  fmtstr_ptr = info_tbl_ptr->call_info.pcscf.fmtstr;
  fmtstr_size = sizeof(info_tbl_ptr->call_info.pcscf.fmtstr);

  memset(fmtstr_ptr, 0, fmtstr_size);

  pcscf_addr_list = (dsi_pcscf_addr_info_t *)
               calloc(1, sizeof(dsi_pcscf_addr_info_t));

  if (NULL == pcscf_addr_list)
  {
    QCRIL_LOG_ERROR("No memory!");
    return;
  }

  rc = dsi_iface_ioctl(info_tbl_ptr->dsi_hndl,
                       DSI_IFACE_IOCTL_GET_PCSCF_SERV_ADDRESS,
                       pcscf_addr_list,
                       &err_code);

  if (DSI_SUCCESS != rc || DSI_SUCCESS != err_code)
  {
    QCRIL_LOG_ERROR("Failed to get PCSCF address - %d, %d", rc, err_code);
    QCRIL_DATA_RELEASE_STORAGE(pcscf_addr_list);
    return;
  }

  for (i = 0; i < pcscf_addr_list->addr_count; i++)
  {
    if (pcscf_addr_list->pcscf_address[i].valid_addr != TRUE ||
          (pcscf_addr_list->pcscf_address[i].addr.ss_family != AF_INET &&
           pcscf_addr_list->pcscf_address[i].addr.ss_family != AF_INET6))
    {
      QCRIL_LOG_ERROR("Invalid PCSCF address");
      continue;
    }
    if (pcscf_addr_list->pcscf_address[i].addr.ss_family == AF_INET)
    {
      sin = (struct sockaddr_in *)&pcscf_addr_list->pcscf_address[i].addr;
      addr_ptr = inet_ntop(AF_INET, &sin->sin_addr, addr_buf, sizeof(addr_buf));
    }
    else
    {
      sin6 = (struct sockaddr_in6 *)&pcscf_addr_list->pcscf_address[i].addr;
      addr_ptr = inet_ntop(AF_INET6, &sin6->sin6_addr, addr_buf, sizeof(addr_buf));
    }

    if (NULL != addr_ptr)
    {
      if ((fmtstr_size - strlen(fmtstr_ptr)) <= (strlen(addr_ptr) + 1))
      {
        break;
      }
      if (k++ > 0)
      {
        strlcat(fmtstr_ptr, QCRIL_DATA_ADDR_DELIMITER, fmtstr_size);
      }
      strlcat(fmtstr_ptr, addr_ptr, fmtstr_size);
    }
  }

  QCRIL_DATA_RELEASE_STORAGE(pcscf_addr_list);

  QCRIL_LOG_DEBUG(" PCSCF Addresses  : %s", info_tbl_ptr->call_info.pcscf.fmtstr);
}
#endif /* QCRIL_RIL_VERSION >= 10 */

#if (QCRIL_RIL_VERSION >= 15)
void qcril_data_ma_ka_send_unsol(uint32_t handle, RIL_KeepaliveStatusCode status_code) {
  QCRIL_LOG_DEBUG("Sending keepalive unsol for handle = %d, status = %d", handle,
    status_code);
  qcril_unsol_resp_params_type unsol_resp;
  RIL_KeepaliveStatus pkt;
  pkt.sessionHandle = handle;
  pkt.code = status_code;
  qcril_default_unsol_resp_params( QCRIL_DEFAULT_INSTANCE_ID, RIL_UNSOL_KEEPALIVE_STATUS, &unsol_resp );
  unsol_resp.resp_pkt = &pkt;
  unsol_resp.resp_len = sizeof(RIL_KeepaliveStatus);
  qcril_send_unsol_response( &unsol_resp );
}

/*===========================================================================

  FUNCTION:  qcrili_data_ma_ka_cleanup

===========================================================================*/
/*!
    @brief
    Clean up keepalive sessions if cid >= 0.
    To clean up all sessions regardless of call if, pass in cid of -1.

    @return
    none
*/
/*=========================================================================*/
void qcrili_data_ma_ka_cleanup(const int cid)
{
  if (cid >= 0) {
    //clean up all requests for given call id
    qcril_data_ma_ka_delete_elems_by_cid(cid);
  } else {
    // clean up all requests
    while (qcril_data_ma_ka_list_head != NULL) {
      qcril_data_ma_ka_list_elem_t *next = qcril_data_ma_ka_list_head->next;
      QCRIL_LOG_DEBUG("%s():Cleanup for handle %d", __func__, qcril_data_ma_ka_list_head->handle);
      qcril_data_stop_keepalive_for_handle(qcril_data_ma_ka_list_head->handle, true);

      free(qcril_data_ma_ka_list_head);
      qcril_data_ma_ka_list_head = next;
    }
  }
}

void qcril_data_ma_ka_send_succ_resp(
  uint32_t handle,
  RIL_Token t,
  qcril_evt_e_type request_type,
  qcril_data_ma_ka_status_t status
  ) {

  RIL_KeepaliveStatus resp;
  resp.sessionHandle = handle;
  if (status == QCRIL_DATA_KA_STATUS_REQUESTED) {
    QCRIL_LOG_DEBUG("Keepalive pending for handle %d", handle);
    resp.code = KEEPALIVE_PENDING;
  } else if (status == QCRIL_DATA_KA_STATUS_RUNNING) {
    QCRIL_LOG_DEBUG("Keepalive active for handle %d", handle);
    resp.code = KEEPALIVE_ACTIVE;
  } else {
    QCRIL_LOG_DEBUG("Keepalive inactive for handle %d", handle);
    resp.code = KEEPALIVE_INACTIVE;
  }
  qcril_data_response_success(QCRIL_DEFAULT_INSTANCE_ID,
    t,
    request_type,
    &resp,
    sizeof(resp));

}

#endif /* (QCRIL_RIL_VERSION >= 15) */
