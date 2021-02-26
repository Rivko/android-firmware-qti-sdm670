/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef FRAMEWORK_LEGACY_H_INCLUDED
#define FRAMEWORK_LEGACY_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif
struct message_id_info;
typedef struct message_id_info *message_id_t;

#include <stddef.h>
#include <pthread.h>
#include "IxErrno.h"
#include <comdef.h>
#include <telephony/ril.h>

#ifndef QCRIL_DATA_OFFTARGET
#include "qcril_features_def.h"
#endif /* QCRIL_DATA_OFFTARGET */

#ifdef HOST_EXECUTABLE_BUILD
size_t strlcpy(char *d, const char *s, size_t sz);
size_t strlcat(char *d, const char *s, size_t sz);
#endif

#define QMI_RIL_SYS_PROP_NAME_MULTI_SIM   "persist.radio.multisim.config" // TODO REARCH: remove from fw
#define QMI_RIL_SYS_PROP_LENGTH_MULTI_SIM 4 // TODO REARCH: remove from fw
#define QMI_RIL_MULTI_SIM_STACK_ID "persist.vendor.radio.msim.stackid_" // TODO REARCH: remove from fw
#define RIL_VALID_FILE_HANDLE   (0)
#define QMI_RIL_ZERO                     (0)
#define QMI_RIL_FF                       (0xFF)
#define QMI_RIL_FFFF                     (0xFFFF)
#define QMI_RIL_SINGLE                   (1)
#define QMI_RIL_FOUR_BYTES               (4)
#define QCRIL_NIL                        0

#define FLOWCONTROL_ONREQUEST_NONE                                 (0)
#define FLOWCONTROL_ONREQUEST_DISPATCHED                           (1)
#define FLOWCONTROL_ONREQUEST_FLOWCONTROL_DEFERRED_BUSY            (2)
#define FLOWCONTROL_ONREQUEST_FLOWCONTROL_REJECTED_OVERFLOW        (4)
#define FLOWCONTROL_ONREQUEST_FLOWCONTROL_EXEMPT                   (8)
#define FLOWCONTROL_ONREQUEST_DISPATCHED_CMD_MAIN_THRD             (16)
#define FLOWCONTROL_ONREQUEST_DISPATCHED_CMD_DEDICATED_THRD        (32)

#ifdef QCRIL_FEATURES_DEF_MAX_REQUEST_ID
#define QMI_RIL_FW_ANDROID_REQUEST_HNDL_MAX_EVT_ID    (QCRIL_FEATURES_DEF_MAX_REQUEST_ID)
#else
#define QMI_RIL_FW_ANDROID_REQUEST_HNDL_MAX_EVT_ID    (256)
#endif

#ifdef QCRIL_FEATURES_DEF_MAX_UNSOL_ID
#define QMI_RIL_ANDROID_UNSOL_RESP_MAX_ID             (QCRIL_FEATURES_DEF_MAX_UNSOL_ID)
#else
#define QMI_RIL_ANDROID_UNSOL_RESP_MAX_ID             (1044) // start from RIL_UNSOL_RESPONSE_BASE
#endif

#ifndef QCRIL_NOTUSED
#define QCRIL_NOTUSED( var ) ((void)(var))
#endif
#define QCRIL_SNPRINTF snprintf

#define QCRIL_MUTEX_LOCK( mutex, log_str )                                                        \
  {                                                                                               \
    QCRIL_LOG_ADDITIONAL("BEFORE_LOCK - %s", log_str);                                            \
    pthread_mutex_lock( mutex );                                                                  \
    QCRIL_LOG_ADDITIONAL("AFTER_LOCK - %s", log_str);                                             \
  }

#define QCRIL_MUTEX_UNLOCK( mutex, log_str )                                                      \
  {                                                                                               \
    QCRIL_LOG_ADDITIONAL("BEFORE_UNLOCK - %s", log_str);                                          \
    pthread_mutex_unlock( mutex );                                                                \
    QCRIL_LOG_ADDITIONAL("AFTER_UNLOCK - %s", log_str);                                           \
  }

typedef enum
{
  IPC_MESSAGE_RADIO_POWER = 1,
  IPC_MESSAGE_AM_CALL_STATE = 2
}ipc_message_id_type;

typedef enum
{
  QMI_RIL_FTR_BASEBAND_UNKNOWN = 0,
  QMI_RIL_FTR_BASEBAND_NONE,
  QMI_RIL_FTR_BASEBAND_APQ,
  QMI_RIL_FTR_BASEBAND_CSFB,
  QMI_RIL_FTR_BASEBAND_MSM,
  QMI_RIL_FTR_BASEBAND_SVLTE2,
  QMI_RIL_FTR_BASEBAND_SGLTE,
  QMI_RIL_FTR_BASEBAND_SGLTE2,
  QMI_RIL_FTR_BASEBAND_DSDA,
  QMI_RIL_FTR_BASEBAND_DSDA2,
  QMI_RIL_FTR_BASEBAND_MDM2,
} qcril_qmi_ftr_baseband_info_type;

typedef enum
{
  QMI_RIL_FTR_SGLTE_CSFB_UNKNOWN = 0,
  QMI_RIL_FTR_SGLTE_CSFB_YES,
  QMI_RIL_FTR_SGLTE_CSFB_NO
} qcril_qmi_ftr_sglte_csfb_info_type;

typedef enum
{
  QMI_RIL_FTR_RAT_UNKNOWN = 0,
  QMI_RIL_FTR_RAT_DISBLE,
  QMI_RIL_FTR_RAT_LEGACY,
  QMI_RIL_FTR_RAT_COMBINE
} qcril_qmi_ftr_rat_enable_option;

/* QCRIL Instance ID */
typedef enum
{
  QCRIL_DEFAULT_INSTANCE_ID = 0,
  QCRIL_SECOND_INSTANCE_ID  = 1,
  QCRIL_THIRD_INSTANCE_ID  = 2,
  QCRIL_MAX_INSTANCE_ID
} qcril_instance_id_e_type;

struct qcril_evt_group_t;
typedef struct qcril_evt_group_t qcril_evt_group_t;
typedef qcril_evt_group_t *qcril_evt_group;

struct qcril_evt_group_t { int unused;};

struct qcril_dispatch_tag;
typedef struct qcril_dispatch_tag qcril_event_id_t;


typedef struct
{
  pthread_mutex_t                             lock_mutex;
  pthread_mutexattr_t                         lock_mtx_atr;
} qmi_ril_atomic_lock_info_type;

typedef qcril_event_id_t *qcril_evt_e_type;

struct qcril_request_resp_params_type;
typedef struct qcril_request_resp_params_type qcril_request_resp_params_type;

typedef struct
{
    RIL_Token t;
    // token_id: to avoid use-after-free, since token may be freed by sending response
    int32_t   token_id;
    qcril_instance_id_e_type instance_id;
    qcril_request_resp_params_type *data;
} qcril_free_req_and_dispatch_follower_req_payload_type;

typedef struct
{
    RIL_Token t;
    // token_id: to avoid use-after-free, since token may be freed by sending response
    int32_t token_id;
    qcril_instance_id_e_type instance_id;
} qcril_deferred_free_req_payload_type;

typedef struct
{
  RIL_Token                 token;
  union
  {
    int                     param_int;
    void*                   param_ptr;
  } param_payload_holder;
  uint32_t                  param_info_and_len;
  uint32_t                  moniker;
} qmi_ril_fw_android_request_holder_type;

typedef struct qmi_ril_fw_android_request_kind_execution_overview_type
{
   RIL_Token                                token_under_execution;
   union
   {
     qmi_ril_fw_android_request_holder_type   local_holder;
     qmi_ril_fw_android_request_holder_type*  extra_holders;
   } holders;
   struct qmi_ril_fw_android_request_kind_execution_overview_type *
                                              family_ring;
   uint32_t                                   nof_extra_holders_and_caps_and_dynamics;
   uint32_t                                   chief_moniker;
   qcril_evt_e_type                           original_android_request_id;
   int                                        family_pair_android_request_id;
} qmi_ril_fw_android_request_kind_execution_overview_type;

typedef struct
{
    uint32      timer_id;
    void        *custom_param;
} qcril_timed_callback_handler_params_type;

#define STRINGIZE(name) #name
#define CONCAT(a, b) a b

#define TO_BIT(param) (1U << param)

#define QCRIL_DECLARE_LEGACY_EVENT_GROUP(name) \
extern qcril_evt_group_t name

#define QCRIL_DEFINE_LEGACY_EVENT_GROUP(name) \
qcril_evt_group_t name

#define QCRIL_EXPAND_LEGACY_EVENT_ARGS_(macro, ...) macro __VA_ARGS__
#define QCRIL_EXPAND_LEGACY_EVENT_ARGS(macro, ...) QCRIL_EXPAND_LEGACY_EVENT_ARGS_(macro,(__VA_ARGS__))

#define QCRIL_DECLARE_LEGACY_EVENT(name, cb, group, flags) \
extern qcril_evt_e_type QCRIL_EVT_##name

#define QCRIL_DECLARE_LEGACY_INDICATION(name, group, flags) \
extern qcril_evt_e_type QCRIL_EVT_##name

#ifdef __cplusplus
#define EVENT_NAME_PREFIX "com.qualcomm.qti.qcril.legacy.event."
#define FULL_EVENT_NAME(name) CONCAT(EVENT_NAME_PREFIX,name)
#define QCRIL_DEFINE_LEGACY_EVENT(name, cb, group, flags) \
    qcril_event_id_t qcril_evt_id_##name {group, cb, REG_MSG(FULL_EVENT_NAME(#name)), flags, FULL_EVENT_NAME(#name)}; \
    qcril_evt_e_type QCRIL_EVT_##name = &qcril_evt_id_##name

/* for unsol messages */
#define QCRIL_DEFINE_LEGACY_INDICATION(name, group, flags) \
    qcril_event_id_t qcril_evt_id_##name {group, NULL, NULL, flags, #name}; \
    qcril_evt_e_type QCRIL_EVT_##name = &qcril_evt_id_##name


#else
#define QCRIL_DEFINE_LEGACY_EVENT(name, cb, group, flags) `The_QCRIL_DEFINE_LEGACY_EVENT_macro_can_only_be_used_from_cpp_code`
#endif

#define QCRIL_INIT_LEGACY_EVENT(name, cb, group, flags) \
    QCRIL_EVT_##name = qcril_event_register_event(STRINGIZE(QCRIL_EVT_##name),(flags),cb)

/* Modem stack ID */
typedef enum
{
  QCRIL_INVALID_MODEM_STACK_ID = -1,
  QCRIL_DEFAULT_MODEM_STACK_ID = 0,
  QCRIL_MODEM_PRIMARY_STACK_ID = QCRIL_DEFAULT_MODEM_STACK_ID,
  QCRIL_MODEM_SECONDARY_STACK_ID  = 1,
  QCRIL_MODEM_TERTIARY_STACK_ID  = 2,
  QCRIL_MODEM_MAX_STACK_ID
} qcril_modem_stack_id_e_type;

/* DSDS QCRIL Instance Pair */
#define QCRIL_DSDS_INSTANCE_PAIR( instance_id ) ( ( instance_id + 1 ) % 2 )

#define QCRIL_MAX_MODEM_ID ( (qmi_ril_is_feature_supported(QMI_RIL_FEATURE_SVLTE2) || qmi_ril_is_feature_supported(QMI_RIL_FEATURE_FUSION_CSFB) ) ? 2 : 1)

/* Radio technology */
typedef enum
{
  QCRIL_RADIO_TECH_NONE   = 0, /*!< Indicates that modem is not on any system yet */
  QCRIL_RADIO_TECH_3GPP   = 1, /*!< Indicates that modem is on 1XEVDO system */
  QCRIL_RADIO_TECH_3GPP2  = 2, /*!< Indicates that modem is on GWL system */
  QCRIL_RADIO_TECH_GLOBAL = 3  /*!< Indicates that modem is on global mode but no service */
} qcril_radio_tech_e_type;

#ifndef QMI_RIL_UTF
#define qcril_malloc(size) qcril_malloc_adv(size, __func__, __LINE__)
#else
#define qcril_malloc(size) utf_qcril_malloc_adv(size, __func__, __LINE__)
void *utf_qcril_malloc_adv( size_t size, const char* func_name, int line_num );
#endif
void *qcril_malloc_adv( size_t size, const char* func_name, int line_num );

#ifndef QMI_RIL_UTF
#define qcril_free(mem_ptr) qcril_free_adv(mem_ptr, __func__, __LINE__)
#else
#define qcril_free(mem_ptr) utf_qcril_free_adv(mem_ptr, __func__, __LINE__)
void utf_qcril_free_adv( void *mem_ptr, const char* func_name, int line_num );
#endif
void qcril_free_adv( void *mem_ptr, const char* func_name, int line_num );

#define QCRIL_UNUSED(name) name __attribute__((unused))

#define QCRIL_MAX_MODEM_NUM 2

#define QCRIL_MAX_LOG_MSG_SIZE                512
#define QMI_RIL_THREAD_NAME_MAX_SIZE          64
#define QMI_RIL_THREAD_INFO_MAX_SIZE          64

#define QCRIL_ASSERT( xx_exp ) QCRIL_LOG_ASSERT( xx_exp )

/* QCRIL Event Data Type */
typedef enum
{
  QCRIL_DATA_NOT_ON_STACK = 0,
  QCRIL_DATA_ON_STACK     = 1
} qcril_data_src_e_type;

typedef struct
{
    size_t len;
    uint8_t *data;
}qcril_binary_data_type;

/* SIM or RUIM states */
typedef enum
{
  QCRIL_SIM_STATE_ABSENT                  = 0,
  QCRIL_SIM_STATE_NOT_READY               = 1,
  QCRIL_SIM_STATE_READY                   = 2,
  QCRIL_SIM_STATE_PIN                     = 3,
  QCRIL_SIM_STATE_PUK                     = 4,
  QCRIL_SIM_STATE_NETWORK_PERSONALIZATION = 5,
  QCRIL_SIM_STATE_CARD_ERROR              = 6,
  QCRIL_SIM_STATE_ILLEGAL                 = 7
} qcril_sim_state_e_type;

/* Modem states */
typedef enum
{
  QCRIL_MODEM_STATE_UNAVAILABLE, /*!< Modem is resetting or booting */
  QCRIL_MODEM_STATE_OFF,         /*!< Modem is in low power mode */
  QCRIL_MODEM_STATE_ON           /*!< Modem is in online mode */
} qcril_modem_state_e_type;

/* RIL info */
typedef struct
{
  boolean pri_gw_sim_state_changed;                                    /*!< Indicates if primary GW SIM state changed */
  boolean pri_cdma_sim_state_changed;                                  /*!< Indicates if primary CDMA SIM state changed */
  boolean sec_gw_sim_state_changed;                                    /*!< Indicates if secondary GW SIM state changed */
  boolean sec_cdma_sim_state_changed;                                  /*!< Indicates if secondary CDMA SIM state changed */
  boolean ter_gw_sim_state_changed;                                    /*!< Indicates if tertiary GW SIM state changed */
  boolean ter_cdma_sim_state_changed;                                  /*!< Indicates if tertiary CDMA SIM state changed */
  qcril_modem_state_e_type next_modem_state;                           /*!< Next Modem state if modem_state_changed is set */
  qcril_sim_state_e_type next_pri_gw_sim_state;                        /*!< Next primary GW SIM state if gw_sim_state_changed is set */
  qcril_sim_state_e_type next_pri_cdma_sim_state;                      /*!< Next primary CDMA SIM state if cdma_sim_state_changed is set */
  qcril_sim_state_e_type next_sec_gw_sim_state;                        /*!< Next secondary GW SIM state if gw_sim_state_changed is set */
  qcril_sim_state_e_type next_sec_cdma_sim_state;                      /*!< Next secondary CDMA SIM state if cdma_sim_state_changed is set */
  qcril_sim_state_e_type next_ter_gw_sim_state;                        /*!< Next tertiary GW SIM state if ter_gw_sim_state_changed is set */
  qcril_sim_state_e_type next_ter_cdma_sim_state;                      /*!< Next tertiary CDMA SIM state if ter_cdma_sim_state_changed is set */

} qcril_request_return_type;

enum
{
    QMI_RIL_FEATURE_FUSION_CSFB,
    QMI_RIL_FEATURE_DSDA,
    QMI_RIL_FEATURE_DSDS,
    QMI_RIL_FEATURE_TSTS,
    QMI_RIL_FEATURE_APQ,
    QMI_RIL_FEATURE_MSM,
    QMI_RIL_FEATURE_SVLTE2,
    QMI_RIL_FEATURE_SGLTE,
    QMI_RIL_FEATURE_SGLTE2,
    QMI_RIL_FEATURE_SGLTE_CSFB,
    QMI_RIL_FEATURE_KDDI_HOLD_ANSWER,
    QMI_RIL_FEATURE_IMS,
    QMI_RIL_FEATURE_VOIP_VT,
    QMI_RIL_FEATURE_IMS_RETRY_3GPP,
    QMI_RIL_FEATURE_IMS_RETRY_3GPP2,
    QMI_RIL_FEATURE_PLAIN_ANDROID,
    QMI_RIL_FEATURE_OEM_SOCKET,
    QMI_RIL_FEATURE_OEM_IND_TO_BOTH,
    QMI_RIL_FEATURE_DSDA2,
    QMI_RIL_FEATURE_POSIX_CLOCKS,
    QMI_RIL_FEATURE_LEGACY_RAT,
    QMI_RIL_FEATURE_COMBINE_RAT,
    QMI_RIL_FEATURE_SAP_SILENT_PIN_VERIFY,
    QMI_RIL_FEATURE_SRLTE,
    QMI_RIL_FEATURE_ATEL_STKCC,
    QMI_RIL_FEATURE_SHM,
    QMI_RIL_FEATURE_SUPPRESS_REQ,
    QMI_RIL_FEATURE_EMBMS,
    QMI_RIL_FEATURE_BLOCK_ALLOW_DATA,
    QMI_RIL_FEATURE_RIL_DATA_REG_STATE_CONDITIONAL_REPORT,
    QMI_RIL_FEATURE_POWER_ON_OPTIMIZATION,
};

/* QCRIL Modem ID */
typedef enum
{
  QCRIL_DEFAULT_MODEM_ID = 0,   /* MSM */
  QCRIL_SECOND_MODEM_ID  = 1,   /* MDM */
  QCRIL_INVALID_MODEM_ID = 0xffffffff
} qcril_modem_id_e_type;


/*! @brief Modem IDs List
*/
typedef struct
{
  uint8 num_of_modems;
  qcril_modem_id_e_type modem_id[ QCRIL_MAX_MODEM_NUM ];
} qcril_modem_ids_list_type;

typedef void qcril_response_callback_t(qcril_request_resp_params_type *param_tr);
/* Payload of RIL Request Response */
struct qcril_request_resp_params_type
{
  qcril_instance_id_e_type instance_id;
  RIL_Token t;
  int request_id_android;
  qcril_evt_e_type request_id;
  RIL_Errno ril_err_no;
  void *resp_pkt;
  size_t resp_len;
  const char *logstr;
  int rild_sock_oem_req;
  uint32 flow_decision_flag;
};

/* Payload of RIL Unsolicited Response */
typedef struct
{
  int response_id;
  void *resp_pkt;
  size_t resp_len;
  const char *logstr;
  uint8 instance_id;
} qcril_unsol_resp_params_type;

typedef enum
{
    QMI_RIL_GEN_OPERATIONAL_STATUS_UNKNOWN = 0,
    QMI_RIL_GEN_OPERATIONAL_STATUS_INIT_PENDING,
    QMI_RIL_GEN_OPERATIONAL_STATUS_INIT_ONGOING,
    QMI_RIL_GEN_OPERATIONAL_STATUS_UNRESTRICTED,
    QMI_RIL_GEN_OPERATIONAL_STATUS_SUSPENDING,
    QMI_RIL_GEN_OPERATIONAL_STATUS_SUSPENDED,
    QMI_RIL_GEN_OPERATIONAL_STATUS_RESUME_PENDING,
    QMI_RIL_GEN_OPERATIONAL_STATUS_RESUMING,
    QMI_RIL_GEN_OPERATIONAL_STATUS_RESUME_RETRY,
    QMI_RIL_GEN_OPERATIONAL_STATUS_UNBIND,
    QMI_RIL_GEN_OPERATIONAL_STATUS_HALTED
} qmi_ril_gen_operational_status_type;

#define ALL_BITS_32   0xffffffff
#define TO_MASK(bitn) (1 << (int)(bitn))
#define IS_BIT_SET(mask, bit) (mask & TO_MASK(bit))
#define SET_BIT(mask, bit) (mask |= TO_MASK(bit))
#define CLEAR_BIT(mask, bit) (mask &= ~TO_MASK(bit))

typedef struct qcril_heap_list_info qcril_heap_list_info;
struct qcril_heap_list_info
{
  uint32               mem_id;
  char                 func_name[32];
  void*                addr;
  uint32               line_number;
  size_t               size;
  qcril_heap_list_info *next;
  qcril_heap_list_info *prev;
};
/* Payload of RIL request or AMSS event */
typedef struct
{
  qcril_instance_id_e_type instance_id;
  qcril_modem_id_e_type modem_id;
  int event_id_android;
  qcril_evt_e_type event_id;
  void *data;
  size_t datalen;
  RIL_Token t;
  qcril_response_callback_t *response_callback;
} qcril_request_params_type;


typedef void qcril_req_handler_type
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr /*!< Output parameter */
);

typedef struct
{
    uint32_t                event_id;
    qcril_req_handler_type *event_handler;
} qmi_ril_suppress_event_type;

struct qcril_dispatch_tag
{
  qcril_evt_group group;
  qcril_req_handler_type *handler;
  /* Use the "TO_MASK" macro with values from qmi_ril_gen_operational_status_type
   * e.g. .allowed_radio_states_mask = TO_MASK(QMI_RIL_GEN_OPERATIONAL_STATUS_INIT_PENDING) | TO_MASK(QMI_RIL_GEN_OPERATIONAL_STATUS_UNRESTRICTED);
   *
   * It is also possible to set individual bits like this:
   * SET_BIT(struct.allowed_radio_states_mask, QMI_RIL_GEN_OPERATIONAL_STATUS_INIT_PENDING);
   */

  message_id_t msg_id;
  uint32_t allowed_radio_states_mask;
  const char *name;
};

typedef enum
{
  QMI_RIL_ANDROID_PARAM_CPY_APPRON_INVALID,
  QMI_RIL_ANDROID_PARAM_CPY_APPRON_4_BYTES_SNAPSHOT,
  QMI_RIL_ANDROID_PARAM_CPY_APPRON_DYNAMIC_COPY,
  QMI_RIL_ANDROID_PARAM_CPY_APPRON_EMPTY_NO_ACTION
} qmi_ril_fw_android_param_copy_approach_type;
qmi_ril_fw_android_param_copy_approach_type qmi_ril_fw_create_android_live_params_copy(
                                               int android_request_id,
                                               void * android_request_data,
                                               int android_request_data_len,
                                               void* four_byte_storage,
                                               int* four_byte_storage_occupied,
                                               void** sub_created_custom_storage,
                                               int* custom_storage_len,
                                               IxErrnoType* error);

typedef boolean (*qcril_event_data_free_cb_t)(qcril_evt_e_type event_id, void *data, size_t datalen);

typedef struct
{
  qcril_instance_id_e_type  instance_id;
  qcril_modem_id_e_type     modem_id;
  int                       event_id_android;
  qcril_evt_e_type          event_id;
  RIL_Token                 token;
  union
  {
    uint32                  four_bytes;
    void*                   dynamic_copy;
  } copied_params;
  size_t                    original_data_len;

  qmi_ril_fw_android_param_copy_approach_type
                            param_copy_arrron;
  qcril_response_callback_t *response_callback;
  qcril_event_data_free_cb_t data_free;
} qmi_ril_dedicated_request_exec_params_type;

#define QCRIL_EMBMS_INTERFACE_NAME_LENGTH_MAX (256)

typedef struct
{
    int     cause;
    uint8   call_id;
    char    interafce_name[ QCRIL_EMBMS_INTERFACE_NAME_LENGTH_MAX ];
    unsigned int if_index;
    int32  dbg_trace_id;
    int32  resp_code;
} qcril_embms_enable_response_payload_type;

typedef struct
{
    int     cause;
    uint8   call_id;
    char    interafce_name[ QCRIL_EMBMS_INTERFACE_NAME_LENGTH_MAX ];
    int32  dbg_trace_id;
    int32  resp_code;
} qcril_embms_disable_indication_payload_type;

#define QMI_RIL_FW_ANDROID_REQUEST_INFO_NO_PAYLOAD      ( 1 << 31 )
void qmi_ril_set_thread_name(pthread_t thread_id, const char *thread_name);
qcril_instance_id_e_type qmi_ril_get_process_instance_id(void);
int qcril_setup_timed_callback( qcril_instance_id_e_type instance_id, qcril_modem_id_e_type modem_id, RIL_TimedCallback callback,
                                   const struct timeval *relativeTime, uint32 *timer_id );

void qmi_ril_set_process_instance_id
(
 qcril_instance_id_e_type instance_id
);

uint32_t qmi_ril_get_sim_slot(void);

void qmi_ril_set_sim_slot(uint32_t slot);

extern int qmi_ril_is_multi_sim_feature_supported();
extern int qmi_ril_is_feature_supported(int feature);
const char *qcril_log_lookup_event_name( qcril_evt_e_type event_id );
RIL_RadioState qcril_get_current_radio_state();
const char *qcril_log_ril_radio_state_to_str(RIL_RadioState radio_state);
boolean qcril_is_internal_token(RIL_Token token);

void qcril_qmi_print_hex(void *msg, int msg_len);
int qmi_ril_get_thread_name(pthread_t thread_id, char *thread_name);
void qmi_ril_init_request_return(qcril_instance_id_e_type instance_id, qcril_request_return_type *ret);
void qcril_state_transition
(
  qcril_instance_id_e_type instance_id,
  qcril_modem_id_e_type modem_id,
  qcril_evt_e_type event_id,
  const qcril_request_return_type *ret_ptr
);

void * qmi_ril_fw_dedicated_request_exec_thread(void * param);
void qmi_ril_fw_destroy_android_live_params_copy(qmi_ril_fw_android_param_copy_approach_type used_approach,
                                               qcril_evt_e_type android_request_id,
                                               void* four_byte_storage,
                                               void* sub_created_custom_storage);

qmi_ril_gen_operational_status_type qmi_ril_get_operational_status(void);
void qmi_ril_set_operational_status( qmi_ril_gen_operational_status_type new_status );

void qcril_default_request_resp_params( qcril_instance_id_e_type instance_id, RIL_Token t, qcril_evt_e_type request_id, RIL_Errno err_no,
                                       qcril_request_resp_params_type *param_ptr );

void qcril_send_request_response( qcril_request_resp_params_type *param_ptr );

char* qmi_ril_util_str_clone( char * org_str );

int qmi_ril_is_multi_sim_oem_hook_request (qcril_evt_e_type req_res_id);

void qmi_ril_fw_send_request_response_epilog( qcril_instance_id_e_type instance_id,
                                              RIL_Token token,
                                              qcril_evt_e_type android_request_id,
                                              RIL_Errno resp_cause,
                                              void* resp_data,
                                              uint32 resp_data_len,
                                              int is_abnormal_drop,
                                              qcril_request_resp_params_type *param_ptr_ref );

int qcril_multiple_rild_ipc_send_func(ipc_message_id_type message_id, void * payload, int payload_length, int dest_rild_instance_id);

void qcril_print_heap_memory_list(void *arg);
void qcril_add_heap_memory_to_list(qcril_heap_list_info *info);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_get_bootup_power_optimization_state
===========================================================================*/
uint8_t qcril_qmi_nas_get_bootup_power_optimization_state();

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_set_bootup_power_optimization_state
===========================================================================*/
void qcril_qmi_nas_set_bootup_power_optimization_state(uint8_t status);
void qmi_ril_reset_multi_sim_ftr_info(); // TODO REARCH: Make static?
void qcril_qmi_mgr_modem_state_updated(qcril_instance_id_e_type instance_id, qcril_modem_state_e_type new_modem_state);
void qcril_qmi_nas_update_modem_stack_id(uint8_t stack_id);
qcril_modem_stack_id_e_type qcril_qmi_get_modem_stack_id();
boolean qcril_event_register_event(qcril_evt_e_type event_id );
boolean android_ril_try_respond
(
 qcril_request_resp_params_type *resp
);

void qmi_ril_fw_android_request_flow_control_info_lock( void );
int qmi_ril_fw_android_request_flow_control_request_holders_have_same_param( qmi_ril_fw_android_request_holder_type* origin, qmi_ril_fw_android_request_holder_type* peer );
void qmi_ril_fw_android_request_flow_control_trigger_remains(qcril_timed_callback_handler_params_type * handler_params);
void qmi_ril_fw_android_request_flow_control_info_unlock( void );
int qmi_ril_fw_android_request_flow_control_find_request_for_execution(
        qmi_ril_fw_android_request_kind_execution_overview_type* origin,
        qmi_ril_fw_android_request_kind_execution_overview_type** exec_overview,
        qmi_ril_fw_android_request_holder_type ** exec_req_holder );

void qmi_ril_fw_android_request_flow_control_release_holder_info_bookref(
        qmi_ril_fw_android_request_holder_type* request_holder_org,
        qcril_evt_e_type android_request_id );

void qmi_ril_fw_android_request_flow_control_overview_request_review_holders( qmi_ril_fw_android_request_kind_execution_overview_type* origin );
char *qcril_qmi_get_esoc_link_name ( void);
void qmi_ril_reset_baseband_rat_option(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
