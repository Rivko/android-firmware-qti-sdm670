/******************************************************************************
#  Copyright (c) 2010-2013, 2015, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    qcril_qmi_nas.h
  @brief   qcril qmi - NAS

  DESCRIPTION
    Handles RIL requests, Callbacks, indications for QMI NAS.

******************************************************************************/


#ifndef QCRIL_QMI_NAS_H
#define QCRIL_QMI_NAS_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "qcrili.h"
#include "utils_standard.h"
#include "qmi_client.h"
#include "voice_service_v02.h"
#include "device_management_service_v01.h"
#include "network_access_service_v01.h"
#include "wireless_messaging_service_v01.h"
#include "modules/nas/qcril_arb.h"
#include "modules/data/DataCommon.h"
//#include "qcril_data_client.h"

/*===========================================================================

                        DEFINITIONS AND TYPES

===========================================================================*/

#define NAS_CELL_ID_NOT_PRESENT USHRT_MAX
#define QCRIL_MCC_FROM_BCD_PLMN(plmn) \
  (((plmn)[0] & 0x0f) * 100 + ((plmn)[0] >> 4) * 10 + ((plmn)[1] & 0x0f))

#define QCRIL_MNC_FROM_BCD_PLMN(plmn) \
  (((plmn)[1] >> 4) == 0x0f) ? (((plmn)[2] & 0x0f) * 10 + ((plmn)[2] >> 4)) : \
  (((plmn)[1] >> 4) + ((plmn)[2] >> 4) * 10 + ((plmn)[2] & 0x0f) * 100)


#define NAS_NIL     QMI_RIL_ZERO
#define NAS_ZERO    QMI_RIL_ZERO

#define QCRIL_INVALID_APP_INDEX (-1)

#define NAS_NMR_MAX_NUM_V01  6
#define QCRIL_MCC_MNC_MAX_SIZE     (4)
#define NAS_MCC_MNC_MAX_SIZE       (QCRIL_MCC_MNC_MAX_SIZE)

#define QMI_CODING_SCHEME_CELL_BROADCAST_DATA 0x00
#define QMI_CODING_SCHEME_UCS2                0x01
#define NAS_3GPP2_MCC_MAX_SIZE     (5)

#define QCRIL_CSG_ID_INVALID                            (0xFFFFFFFF)

#define QCRIL_POWER_OPT_BOOT_UP_TRIGGER_LPM       0
#define QCRIL_POWER_OPT_BOOT_UP_TRIGGER_ONLINE    1
#define QCRIL_POWER_OPT_BOOT_UP_TRIGGER_COMPLETE  2

#define NAS_RADIO_IF_GSM  0x04
#define NAS_RADIO_IF_UMTS 0x05
#define NAS_RADIO_IF_LTE  0x08

#define NAS_SIGNAL_STRENGTH_UNK     (-1)
#define NAS_SIGNAL_STRENGTH_UNK_GW  (99)

typedef enum
{
    NONE = -1,
    USER,
    VOLTE
}atel_dds_reason;

typedef enum
{
    QCRIL_QMI_NAS_DATA_EVT_UNKNOWN = -1,
    QCRIL_QMI_NAS_DATA_EVT_CALL_CONNECTED,
    QCRIL_QMI_NAS_DATA_EVT_CALL_PHYSLINK_UP,
    QCRIL_QMI_NAS_DATA_EVT_CALL_PHYSLINK_DOWN,
    QCRIL_QMI_NAS_DATA_EVT_CALL_RELEASED,
    QCRIL_QMI_NAS_DATA_EVT_MAX
}qcril_qmi_nas_data_call_event_type;

typedef enum
{
    QMI_RIL_RTE_NONE = 0,
    QMI_RIL_RTE_FIRST,
    QMI_RIL_RTE_1x = QMI_RIL_RTE_FIRST,
    QMI_RIL_RTE_GSM,
    QMI_RIL_RTE_WCDMA,
    QMI_RIL_RTE_TDSCDMA,
    QMI_RIL_RTE_SUB_LTE,
    QMI_RIL_RTE_SUB_DO,
    QMI_RIL_RTE_LAST = QMI_RIL_RTE_SUB_DO,
    QMI_RIL_RTE_CAP
} qmi_ril_nw_reg_rte_type;

typedef enum
{
    QMI_RIL_RTE_KIND_NONE = 0,
    QMI_RIL_RTE_KIND_VOICE,
    QMI_RIL_RTE_KIND_DATA,
    QMI_RIL_RTE_KIND_IMS,
    QMI_RIL_RTE_KIND_CAP
} qmi_ril_nw_reg_rte_kind_type;

typedef enum
{
    QMI_RIL_RAT_CONFIDENCE_UNKNOWN = 0,
    QMI_RIL_RAT_CONFIDENCE_FULL_SVC,
    QMI_RIL_RAT_CONFIDENCE_LTD_SVC,
    QMI_RIL_RAT_CONFIDENCE_ANTICIPATED,
    QMI_RIL_RAT_CONFIDENCE_NO_SVC
} qmi_ril_nw_reg_rat_confidence_tag_type;

typedef struct
{
  qmi_ril_nw_reg_rte_type                   voice_rte;
  qmi_ril_nw_reg_rte_type                   data_rte;
  qmi_ril_nw_reg_rte_type                   ims_rte;
  qmi_ril_nw_reg_rat_confidence_tag_type    voice_rte_confidence_tag;
  qmi_ril_nw_reg_rat_confidence_tag_type    data_rte_confidence_tag;
  qmi_ril_nw_reg_rat_confidence_tag_type    ims_rte_confidence_tag;
} qmi_ril_nw_reg_rte_snapshot_type;

typedef enum
{
    QMI_RIL_EME_CBM_INVALID,
    QMI_RIL_EME_CBM_NOT_ACTIVE,
    QMI_RIL_EME_CBM_ACTIVE
} qmi_ril_emergency_callback_mode_state_type;

typedef enum
{

    QMI_RIL_NW_REG_FULL_SERVICE             = 1,
    QMI_RIL_NW_REG_LIMITED_SERVICE          = 2,
    QMI_RIL_NW_REG_VOICE_CALLS_AVAILABLE    = 4,
    QMI_RIL_NW_REG_VOIP_CALLS_AVAILABLE     = 8
} qmi_ril_nw_reg_status_overview_item_type;

typedef enum
{
    QMI_RIL_DMS_RADIO_PWR_CANCEL_NONE = 0,
    QMI_RIL_DMS_RADIO_PWR_CANCEL_SSR,
} qmi_ril_dms_radio_pwr_cancel_e_type;

typedef enum
{
    SUBSCRIPTION_SOURCE_RUIM_SIM = 0,
    SUBSCRIPTION_SOURCE_NV,
    SUBSCRIPTION_SOURCE_RUIM_IF_AVAIL
}oem_hook_cdma_subscription_source_e_type;

typedef enum
{
    FTM_SUBSCRIPTION_INVALID = -1,
    FTM_SUBSCRIPTION_CDMA,
    FTM_SUBSCRIPTION_GSM,
    FTM_SUBSCRIPTION_WCDMA
}oem_hook_ftm_subscription_source_e_type;

typedef enum
{
    QMI_RIL_NAS_INIT_APN_ATTCH_NONE   = 0,
    QMI_RIL_NAS_INIT_APN_ATTCH_DETACH,
    QMI_RIL_NAS_INIT_APN_ATTCH_ATTACH,
    QMI_RIL_NAS_INIT_ALLOW_DATA_ATTCH_ATTACH,
    QMI_RIL_NAS_INIT_ALLOW_DATA_ATTCH_ON_DEMAND_ATTACH,
    QMI_RIL_NAS_INIT_ALLOW_DATA_ATTCH_WAIT_FOR_RAT_EXP,
    QMI_RIL_NAS_INIT_ALLOW_DATA_ATTCH_DETACH
} qmi_ril_nas_init_apn_attch_state_e_type;

typedef enum
{
    QCRIL_QMI_ACQ_ORDER_NONE = 0,
    QCRIL_QMI_ACQ_ORDER_LTE_TDS_GSM,
    QCRIL_QMI_ACQ_ORDER_TDS_GSM_LTE,
    QCRIL_QMI_ACQ_ORDER_LTE_UMTS_GSM
} qcril_qmi_acq_order_e_type;

typedef enum
{
    QCRIL_QMI_BAND_PREF_NONE = 0,
    QCRIL_QMI_BAND_PREF_LTE_FULL,
    QCRIL_QMI_BAND_PREF_TDD_LTE,
    QCRIL_QMI_BAND_PREF_FDD_LTE
} qcril_qmi_band_pref_e_type;

typedef enum
{
    QCRIL_QMI_RAT_BAND_NONE = 1,
    QCRIL_QMI_LTE_BAND,
} qcril_qmi_rat_band_e_type;


typedef enum
{
    QCRIL_CELL_LOCATION_INCREASE_PERIOD,
    QCRIL_CELL_LOCATION_REDUCE_PERIOD,
    QCRIL_CELL_LOCATION_RESET_PERIOD,
} qmi_ril_cell_location_period_action;

typedef enum
{
  QCRIL_FLEXI_MAP_STATUS_NONE     = 0,
  QCRIL_FLEXI_MAP_STATUS_START_DEACTIVATE = 1,
  QCRIL_FLEXI_MAP_STATUS_START_UNBIND = 2,
  QCRIL_FLEXI_MAP_STATUS_APPLY_BIND   = 3,
  QCRIL_FLEXI_MAP_STATUS_APPLY_ACTIVATE  = 4,
  QCRIL_FLEXI_MAP_STATUS_FINISH   = 5,
  QCRIL_FLEXI_MAP_STATUS_START_DEACTIVATE_FAIL = 6,
  QCRIL_FLEXI_MAP_STATUS_START_UNBIND_FAIL = 7,
  QCRIL_FLEXI_MAP_STATUS_APPLY_BIND_FAIL   = 8,
  QCRIL_FLEXI_MAP_STATUS_APPLY_ACTIVATE_FAIL  = 9,
  QCRIL_FLEXI_MAP_STATUS_FINISH_FAIL   = 10
} qcril_flexi_map_status;

/* */
typedef enum
{
  QCRIL_APP_STATUS_MIN                       = 0,
  QCRIL_APP_STATUS_NOT_PROVISIONED           = QCRIL_APP_STATUS_MIN,
  QCRIL_APP_STATUS_PROVISIONING              = 1,
  QCRIL_APP_STATUS_PROVISIONED               = 2,
  QCRIL_APP_STATUS_DEPROVISIONING            = 3,
  QCRIL_APP_STATUS_DEPROVISIONED             = 4,
  QCRIL_APP_STATUS_PENDING_PROVISIONING      = 5,
  QCRIL_APP_STATUS_PENDING_DEPROVISIONING    = 6,
  QCRIL_APP_STATUS_MAX                       = QCRIL_APP_STATUS_PENDING_DEPROVISIONING,
} qcril_card_app_provision_status;

typedef enum
{
    QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_NONE = 0,
    QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_MCC,
    QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_VOICE_MCC,
    QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_HARD_MCC,
    QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_HARD,
    QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_NW,
    QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_ESCV_IIN,
    QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_ESCV_NW,
    QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_MCC_MNC,
    QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_VOICE_MCC_MNC,
    QMI_RIL_CUSTOM_EMERGENCY_NUMBERS_SOURCE_MAX
} qmi_ril_custom_emergency_numbers_source_type;

typedef enum
{
    QMI_RIL_NAS_RECEIVE_DATA_DORMANCY_IND_OFF,
    QMI_RIL_NAS_RECEIVE_DATA_DORMANCY_IND_ON,
    QMI_RIL_NAS_RECEIVE_DATA_DORMANCY_IND_ON_SCREEN_STATE
} qmi_ril_nas_data_dormancy_ind_type;

typedef enum {
    QMI_RIL_NAS_MODEM_SHUTDOWN_STATE_BEGIN = 0,
    QMI_RIL_NAS_MODEM_SHUTDOWN_STATE_WAITING_FOR_DATA_SHUTDOWN,
    QMI_RIL_NAS_MODEM_SHUTDOWN_STATE_WAITING_FOR_IMS_DEREGISTRATION,
    QMI_RIL_NAS_MODEM_SHUTDOWN_STATE_IMS_DEREGISTRATION_COMPLETED,
    QMI_RIL_NAS_MODEM_SHUTDOWN_STATE_WAITING_FOR_CARD_SHUTDOWN,
    QMI_RIL_NAS_MODEM_SHUTDOWN_STATE_CARD_SHUTDOWN_COMPLETED,
    QMI_RIL_NAS_MODEM_SHUTDOWN_STATE_END,
} qmi_ril_nas_modem_shutdown_states_e_type;

typedef enum {
    QMI_RIL_NAS_MODEM_SHUTDOWN_EVENT_BEGIN_SEQUENCE = 0,
    QMI_RIL_NAS_MODEM_SHUTDOWN_EVENT_REQUEST_DATA_SHUTDOWN,
    QMI_RIL_NAS_MODEM_SHUTDOWN_EVENT_DATA_SHUTDOWN_COMPLETE,
    QMI_RIL_NAS_MODEM_SHUTDOWN_EVENT_DATA_SHUTDOWN_TIMEOUT,
    QMI_RIL_NAS_MODEM_SHUTDOWN_EVENT_IMS_DEREGISTERED,
    QMI_RIL_NAS_MODEM_SHUTDOWN_EVENT_IMS_DEREGISTRATION_TIMEOUT,
    QMI_RIL_NAS_MODEM_SHUTDOWN_EVENT_REQUEST_CARD_SHUTDOWN,
    QMI_RIL_NAS_MODEM_SHUTDOWN_EVENT_CARD_SHUTDOWN_COMPLETE,
    QMI_RIL_NAS_MODEM_SHUTDOWN_EVENT_POWER_DOWN_MODEM,
} qmi_ril_nas_modem_shutdown_events_e_type;

typedef struct
{
  nas_subs_type_enum_v01 dds_sub;
  nas_dds_duration_enum_v01 dds_duration;
}dds_sub_pref_cause;

typedef struct{
  int result; //success/failure
  int error_cause; //valid value if result is failure
} dds_status_info;

typedef struct
{
  int    enum_id;
  const char * enum_name;
}qcril_qmi_enum_log_type;

struct ftm_subs_status
{
  oem_hook_ftm_subscription_source_e_type subs_type;
  int is_enable;
};

struct cdma_ftm_data
{
    uint32_t srv_status;
    uint32_t srv_domain;
    uint32_t system_mode;
    uint32_t roam_status;
    uint32_t mcc;
    uint32_t mnc;
    int8_t rssi;
    int16_t ecio;
    float cdma_1x_rx0_agc;
    float cdma_1x_rx1_agc;
    float cdma_evdo_rx0_agc;
    float cdma_evdo_rx1_agc;
};

struct gsm_neigh_cell_info
{
    uint32_t cell_id;
    uint16_t bcch;
    uint8_t bsic;
    uint16_t rx_level;
};

struct gsm_ftm_data
{
    uint32_t srv_status;
    uint32_t srv_domain;
    uint32_t system_mode;
    uint32_t roam_status;
    uint32_t mcc;
    uint32_t mnc;
    uint16_t lac;
    int8_t rssi;
    uint16_t bcch;
    uint8_t bsic;
    uint16_t rx_level;
    uint16_t rx_qual_full;
    uint16_t rx_qual_sub;
    uint16_t ta;
    uint32_t no_of_neigh_cell_info_len;
    struct gsm_neigh_cell_info gsm_neigh_cell[ NAS_NMR_MAX_NUM_V01 ];
};

struct wcdma_neigh_cell_info
{
    uint32_t cell_id;
    uint16_t uarfcn;
    uint16_t psc;
    int16_t rscp;
    int16_t ecio;
};

struct wcdma_ftm_data
{
    uint32_t srv_status;
    uint32_t srv_domain;
    uint32_t system_mode;
    uint32_t roam_status;
    uint32_t mcc;
    uint32_t mnc;
    uint16_t lac;
    uint16_t bler;
    int16_t ecio;
    int16_t rscp;
    float rx_agc;
    float tx_agc;
    uint16_t uarfcn;
    uint16_t psc;
    uint32_t no_of_neigh_cell_info_len;
    struct wcdma_neigh_cell_info wcdma_neigh_cell[ NAS_NMR_MAX_NUM_V01 ];
};

#if 0
typedef struct
{
    qcril_provision_info_type provision_info;
    qcril_evt_e_type          repost_event_id;
} qcril_nas_dsds_activate_repost_params;
#endif //0

typedef struct
{
    int  is_eme_ip_call;
    int  is_eme_call_connected;
} qcril_nas_emer_call_end_hndlr_params;

#ifdef __cplusplus
#include "modules/nas/NasSetVoiceDomainPreferenceRequest.h"
#include <memory>

/*=========================================================================
FUNCTION:  qcril_qmi_nas_set_voice_domain_preference
===========================================================================*/
RIL_Errno qcril_qmi_nas_set_voice_domain_preference
(
 std::shared_ptr<NasSetVoiceDomainPreferenceRequest> msg
 );
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*=========================================================================
  FUNCTION:  qcril_qmi_dms_unsolicited_indication_cb_helper

===========================================================================*/
void qcril_qmi_dms_unsolicited_indication_cb_helper
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_dms_unsolicited_indication_cb

===========================================================================*/
void qcril_qmi_dms_unsolicited_indication_cb
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  void                          *ind_buf,
  unsigned int                   ind_buf_len,
  void                          *ind_cb_data
);

#ifdef QMI_RIL_UTF
/*=========================================================================
  FUNCTION:  qcril_qmi_dsd_ind_cb

===========================================================================*/
void qcril_qmi_dsd_ind_cb
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  void                          *ind_buf,
  unsigned int                   ind_buf_len,
  void                          *ind_cb_data
);

/*=========================================================================
  FUNCTION:  qcril_qmi_dsd_unsolicited_indication_cb_helper

===========================================================================*/
void qcril_qmi_dsd_unsolicited_indication_cb_helper
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_reboot_cleanup

===========================================================================*/
void qcril_qmi_nas_reboot_cleanup(void);
#endif

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_unsolicited_indication_cb_helper

===========================================================================*/
void qcril_qmi_nas_unsolicited_indication_cb_helper
(
  unsigned int   msg_id,
  unsigned char *decoded_payload,
  uint32_t       decoded_payload_len
);


/*=========================================================================
  FUNCTION:  qcril_qmi_dms_async_cb_helper

===========================================================================*/
void qcril_qmi_dms_async_cb_helper
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_async_cb_helper

===========================================================================*/
void qcril_qmi_nas_async_cb_helper
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_unsolicited_indication_cb

===========================================================================*/
void qcril_qmi_nas_unsolicited_indication_cb
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  void                          *ind_buf,
  unsigned int                   ind_buf_len,
  void                          *ind_cb_data
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_init

===========================================================================*/
void qcril_qmi_nas_init(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_pre_init

===========================================================================*/
void qcril_qmi_nas_pre_init(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_dms_init

===========================================================================*/
void qcril_qmi_dms_init(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_multi_sim_init

===========================================================================*/
void qcril_qmi_nas_multi_sim_init(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_get_device_capability

===========================================================================*/
RIL_Errno qcril_qmi_nas_get_device_capability(int need_msim_capability,
                                              int need_feature_mode,
                                              int need_sub_capability,
                                              int need_ims_capability);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_get_subscription_info

===========================================================================*/
void qcril_qmi_nas_get_subscription_info(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_update_modem_stack_id

===========================================================================*/
void qcril_qmi_nas_update_modem_stack_id
(
uint8_t stack_id
);

/*=========================================================================
  FUNCTION:  qcril_qmi_util_decode_operator_name

===========================================================================*/
void qcril_qmi_util_decode_operator_name
(
  char *dest,
  uint16 max_dest_length,
  int coding_scheme,
  const uint8 *src,
  uint16 src_length
);

/*=========================================================================
  FUNCTION:  qcril_qmi_util_decode_operator_name_in_little_endian

===========================================================================*/
void qcril_qmi_util_decode_operator_name_in_little_endian
(
  char *dest,
  size_t max_dest_length,
  int coding_scheme,
  const uint8 *src,
  uint16 src_length
);

/*=========================================================================
  FUNCTION:  qmi_ril_nw_reg_get_status_overview

===========================================================================*/
uint32_t qmi_ril_nw_reg_get_status_overview( void );


/*=========================================================================
  FUNCTION:  qmi_ril_in_apm_leave_window

===========================================================================*/
boolean qmi_ril_in_apm_leave_window(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_is_in_service_in

===========================================================================*/
int qcril_qmi_nas_is_in_service_in( uint8_t radio_if1, uint8_t radio_if2 );

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_request_signal_strength

===========================================================================*/
void qcril_qmi_nas_request_signal_strength
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_request_data_registration_state

===========================================================================*/
void qcril_qmi_nas_request_data_registration_state
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

// wrapper function for qcril_qmi_nas_request_data_registration_state
void qcril_qmi_nas_request_data_registration_state_wrapper
(
  void* cb_data
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_request_registration_state

===========================================================================*/
void qcril_qmi_nas_request_registration_state
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

// wrapper function for qcril_qmi_nas_request_registration_state
void qcril_qmi_nas_request_registration_state_wrapper
(
  void* cb_data
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_request_operator

===========================================================================*/
void qcril_qmi_nas_request_operator
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

// wrapper function for qcril_qmi_nas_request_operator
void qcril_qmi_nas_request_operator_wrapper
(
  void* cb_data
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_query_network_selection_mode

===========================================================================*/
void qcril_qmi_nas_query_network_selection_mode
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

// wrapper function for qcril_qmi_nas_query_network_selection_mode
void qcril_qmi_nas_query_network_selection_mode_wrapper
(
  void* cb_data
);
/*=========================================================================
  FUNCTION:  qcril_qmi_nas_request_power

===========================================================================*/
void qcril_qmi_nas_request_power
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_request_screen_state

===========================================================================*/
void qcril_qmi_nas_request_screen_state
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_request_set_unsol_response_filter

===========================================================================*/
void qcril_qmi_nas_request_set_unsol_response_filter
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_request_send_device_state

===========================================================================*/
void qcril_qmi_nas_request_send_device_state
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_dms_get_current_power_state

===========================================================================*/
RIL_RadioState qcril_qmi_nas_dms_get_current_power_state();

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_dms_is_in_online_mode

===========================================================================*/
boolean qcril_qmi_nas_dms_is_in_online_mode(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_dms_is_in_ftm_mode

===========================================================================*/
boolean qcril_qmi_nas_dms_is_in_ftm_mode(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_is_radio_power_check_enabled

===========================================================================*/
boolean qcril_qmi_nas_is_radio_power_check_enabled(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_exit_emergency_callback_mode

===========================================================================*/
void qcril_qmi_nas_exit_emergency_callback_mode
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_set_network_selection_automatic

===========================================================================*/
void qcril_qmi_nas_set_network_selection_automatic
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_set_network_selection_manual

===========================================================================*/
void qcril_qmi_nas_set_network_selection_manual
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_query_available_networks

===========================================================================*/
void qcril_qmi_nas_query_available_networks
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_cancel_query_available_networks

===========================================================================*/
void qcril_qmi_nas_cancel_query_available_networks
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type *const ret_ptr // Output parameter
);

//===========================================================================
// RIL_REQUEST_START_NETWORK_SCAN
//===========================================================================
void qcril_qmi_nas_start_advanced_scan
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_set_band_mode

===========================================================================*/
void qcril_qmi_nas_set_band_mode
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_query_available_band_mode

===========================================================================*/
void qcril_qmi_nas_query_available_band_mode
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_exit_emergency_callback_mode

===========================================================================*/
void qcril_qmi_nas_exit_emergency_callback_mode
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_request_radio_tech

===========================================================================*/
void qcril_qmi_nas_request_radio_tech
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_request_get_dc_rt_info

===========================================================================*/
void qcril_qmi_nas_request_get_dc_rt_info
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_get_neighboring_cell_ids

===========================================================================*/
void qcril_qmi_nas_get_neighboring_cell_ids
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_set_location_updates

===========================================================================*/
void qcril_qmi_nas_set_location_updates
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_request_cdma_subscription

===========================================================================*/
void qcril_qmi_nas_request_cdma_subscription
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_set_subscription_source

===========================================================================*/
void qcril_qmi_nas_set_subscription_source
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_set_subscription_source_with_spc

===========================================================================*/
void qcril_qmi_nas_set_subscription_source_with_spc
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_get_subscription_source

===========================================================================*/
void qcril_qmi_nas_get_subscription_source
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_cdma_validate_and_write_key

===========================================================================*/
void qcril_qmi_nas_cdma_validate_and_write_key
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_dms_request_device_identity

===========================================================================*/
void qcril_qmi_nas_dms_request_device_identity
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_dms_request_baseband_version

===========================================================================*/
void qcril_qmi_nas_dms_request_baseband_version
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_dms_commmon_pre_init

===========================================================================*/
void qcril_qmi_nas_dms_commmon_pre_init(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_dms_commost_post_cleanup

===========================================================================*/
void qcril_qmi_nas_dms_commost_post_cleanup(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_ril_domestic_service_is_screen_off

===========================================================================*/
int qcril_qmi_ril_domestic_service_is_screen_off(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_dms_event_update_fdn_status

===========================================================================*/
void qcril_qmi_nas_dms_event_update_fdn_status
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr /*!< Output parameter */
);

/*=========================================================================
  FUNCTION:  qcril_qmi_dms_fusion_csfb_unsolicited_indication_cb

===========================================================================*/
void qcril_qmi_dms_fusion_csfb_unsolicited_indication_cb
(
  qmi_client_type                user_handle,
  unsigned long                  message_id,
  unsigned char                  * ind_buf,
  int                            ind_buf_len,
  void                           *ind_cb_data
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_cleanup

===========================================================================*/
void qcril_qmi_nas_cleanup();


/*=========================================================================
  FUNCTION:  qcril_qmi_nas_minority_command_cb

===========================================================================*/
void qcril_qmi_nas_minority_command_cb
(
  qmi_client_type              user_handle,
  unsigned int                 msg_id,
  void                        *resp_c_struct,
  unsigned int                 resp_c_struct_len,
  void                        *resp_cb_data,
  qmi_client_error_type        transp_err
);

/*=========================================================================
  FUNCTION:  qcril_qmi_dms_command_cb

===========================================================================*/
void qcril_qmi_dms_command_cb
(
  qmi_client_type              user_handle,
  unsigned int                 msg_id,
  void                        *resp_c_struct,
  unsigned int                 resp_c_struct_len,
  void                        *resp_cb_data,
  qmi_client_error_type        transp_err
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_dsds_request_set_subscription_mode

===========================================================================*/
void qcril_qmi_nas_dsds_request_set_subscription_mode
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_prov_process_deferred_request

===========================================================================*/
int qcril_qmi_prov_process_deferred_request(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_prov_status_hndlr

===========================================================================*/
void qcril_qmi_nas_prov_status_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_set_uicc_subscription

===========================================================================*/
void qcril_qmi_nas_set_uicc_subscription
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_dsds_request_set_tune_away

===========================================================================*/
void qcril_qmi_nas_dsds_request_set_tune_away
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_dsds_request_get_tune_away

===========================================================================*/
void qcril_qmi_nas_dsds_request_get_tune_away
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_dsds_request_set_paging_priority

===========================================================================*/
void qcril_qmi_nas_dsds_request_set_paging_priority
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_dsds_request_get_paging_priority

===========================================================================*/
void qcril_qmi_nas_dsds_request_get_paging_priority
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_dsds_request_set_data_subscription

===========================================================================*/
void qcril_qmi_nas_dsds_request_set_data_subscription
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_dsds_request_set_default_voice_sub

===========================================================================*/
void qcril_qmi_nas_dsds_request_set_default_voice_sub
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_event_subs_followup

===========================================================================*/
void qcril_qmi_nas_event_subs_followup
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_event_subs_deactivate_followup

===========================================================================*/
void qcril_qmi_nas_event_subs_deactivate_followup
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

//===========================================================================
//QCRIL_EVT_QMI_PROV_FM_START_STATUS
//===========================================================================
void qcril_qmi_nas_prov_fm_start_status_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

//===========================================================================
//QCRIL_EVT_QMI_PROV_FM_APPLY_STATUS
//===========================================================================
void qcril_qmi_nas_prov_fm_apply_status_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_event_card_status_update

===========================================================================*/
void qcril_qmi_nas_event_card_status_update
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_event_app_status_update

===========================================================================*/
void qcril_qmi_nas_event_app_status_update
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_get_modem_capability

===========================================================================*/
void qcril_qmi_nas_get_modem_capability
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_update_sub_binding

===========================================================================*/
void qcril_qmi_nas_update_sub_binding
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_request_allow_data

===========================================================================*/
void qcril_qmi_nas_request_allow_data
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_trigger_propagate_known_signal_strength_ind

===========================================================================*/
void qcril_qmi_trigger_propagate_known_signal_strength_ind
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_dms_request_imei

===========================================================================*/
void qcril_qmi_nas_dms_request_imei
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_dms_request_imeisv

===========================================================================*/
void qcril_qmi_nas_dms_request_imeisv
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_bootup_kick

===========================================================================*/
void qcril_qmi_nas_bootup_kick
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_nw_select_handle_total_cleanup

===========================================================================*/
void qcril_qmi_nas_nw_select_handle_total_cleanup(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_send_unsol_radio_state_changed

===========================================================================*/
void qcril_qmi_nas_send_unsol_radio_state_changed
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_connected_emergency_call_end_hdlr

===========================================================================*/
void qcril_qmi_nas_connected_emergency_call_end_hdlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_emergency_mode_on_hdlr

===========================================================================*/
void qcril_qmi_nas_emergency_mode_on_hdlr(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_emergency_mode_off_hdlr

===========================================================================*/
void qcril_qmi_nas_emergency_mode_off_hdlr(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_util_enable_networking_indications

===========================================================================*/
RIL_Errno qcril_qmi_util_enable_networking_indications( int enable_nwreg,
                                                        int enable_signal_strength,
                                                        int force_update );

/*=========================================================================
  FUNCTION:  qmi_ril_initiate_bootup_kick

===========================================================================*/
void qmi_ril_initiate_bootup_kick(void);

/*=========================================================================
  FUNCTION:  qmi_ril_is_qmi_tdscdma_support_available

===========================================================================*/
int qmi_ril_is_qmi_tdscdma_support_available(void);

/*=========================================================================
  FUNCTION:  qmi_ril_emergency_callback_mode_state_type

===========================================================================*/
qmi_ril_emergency_callback_mode_state_type qmi_ril_nwr_get_eme_cbm(void);

/*=========================================================================
  FUNCTION:  qmi_ril_nwr_set_eme_cbm

===========================================================================*/
void qmi_ril_nwr_set_eme_cbm(qmi_ril_emergency_callback_mode_state_type new_mode);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_initiate_radio_state_changed_ind

===========================================================================*/
void qcril_qmi_nas_initiate_radio_state_changed_ind(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_dms_handle_fetch_prl_request

===========================================================================*/
void qcril_qmi_nas_dms_handle_fetch_prl_request(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_set_registered_on_ims

===========================================================================*/
void qcril_qmi_nas_set_registered_on_ims( int registered );

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_embms_requst_enable

===========================================================================*/
void qcril_qmi_nas_embms_requst_enable
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_embms_enable_data_con

===========================================================================*/
void qcril_qmi_nas_embms_enable_data_con
(
  const qcril_embms_enable_response_payload_type* response_payload
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_embms_requst_disable

===========================================================================*/
void qcril_qmi_nas_embms_requst_disable
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_embms_disable_data_ind

===========================================================================*/
void qcril_qmi_nas_embms_disable_data_ind
(
  const qcril_embms_disable_indication_payload_type* indication_payload
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_embms_request_get_coverage_state

===========================================================================*/
void qcril_qmi_nas_embms_request_get_coverage_state
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_embms_get_rssi

===========================================================================*/
void qcril_qmi_nas_embms_get_rssi
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_embms_get_sib16_coverage

===========================================================================*/
void qcril_qmi_nas_embms_get_sib16_coverage
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_embms_set_sntp_time

===========================================================================*/
void qcril_qmi_nas_embms_set_sntp_time
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_embms_get_utc_time

===========================================================================*/
void qcril_qmi_nas_embms_get_utc_time
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_embms_send_radio_state_helper

===========================================================================*/
void qcril_qmi_nas_embms_send_radio_state_helper
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_embms_get_active_log_packet_ids

===========================================================================*/
void qcril_qmi_nas_embms_get_active_log_packet_ids
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_embms_deliver_log_packet

===========================================================================*/
void qcril_qmi_nas_embms_deliver_log_packet
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_embms_get_e911_state

===========================================================================*/
void qcril_qmi_nas_embms_get_e911_state
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_embms_get_sib_plmn

===========================================================================*/
void qcril_qmi_nas_embms_get_sib_plmn
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_embms_get_embms_status

===========================================================================*/
void qcril_qmi_nas_embms_get_embms_status
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_post_voice_rte_change_ind_handler

===========================================================================*/
void qcril_qmi_nas_post_voice_rte_change_ind_handler
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_set_rfm_scenario_req

===========================================================================*/
void qcril_qmi_nas_set_rfm_scenario_req
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_get_rfm_scenario_req

===========================================================================*/
void qcril_qmi_nas_get_rfm_scenario_req
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_get_provisioned_table_revision_req

===========================================================================*/
void qcril_qmi_nas_get_provisioned_table_revision_req
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_embms_set_enable_cmd_cb_helper

===========================================================================*/
void qcril_qmi_nas_embms_set_enable_cmd_cb_helper
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

#if 0
/*=========================================================================
  FUNCTION:  qcril_qmi_nas_dms_update_card_status

===========================================================================*/
void qcril_qmi_nas_dms_update_card_status
(
  qcril_instance_id_e_type instance_id,
  uint8 slot,
  boolean pwr_oprt_in_progress,
  qcril_card_status_e_type card_status
);
#endif //0

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_set_reported_voice_radio_tech

===========================================================================*/
void qcril_qmi_nas_set_reported_voice_radio_tech(RIL_RadioTechnology voice_radio_tech);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_get_reported_voice_radio_tech

===========================================================================*/
RIL_RadioTechnology qcril_qmi_nas_get_reported_voice_radio_tech(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_is_using_rte_helper

===========================================================================*/
int qcril_qmi_nas_is_using_rte_helper(qmi_ril_nw_reg_rte_type rte,
                                      qmi_ril_nw_reg_rte_kind_type rte_kind);

/*=========================================================================
  FUNCTION:  qcril_qmi_dms_is_prl_info_available

===========================================================================*/
int qcril_qmi_dms_is_prl_info_available(uint16_t * prl_version);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_initiate_voice_rte_change_propagation

===========================================================================*/
void qcril_qmi_nas_initiate_voice_rte_change_propagation( void );

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_reevaluate_vrte_helper

===========================================================================*/
void qcril_qmi_nas_reevaluate_vrte_helper (qcril_timed_callback_handler_params_type *param);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_control_signal_nas_on_current_calls_change

===========================================================================*/
void qcril_qmi_nas_control_signal_nas_on_current_calls_change(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_dms_commmon_post_init

===========================================================================*/
void qcril_qmi_nas_dms_commmon_post_init(void);

/*=========================================================================
  FUNCTION:  qmi_ril_nw_reg_snapshot_init

===========================================================================*/
void qmi_ril_nw_reg_snapshot_init( qmi_ril_nw_reg_rte_snapshot_type* snapshot );

/*=========================================================================
  FUNCTION:  qmi_ril_nw_reg_snapshot_cur_rte

===========================================================================*/
void qmi_ril_nw_reg_snapshot_cur_rte( qmi_ril_nw_reg_rte_snapshot_type* snapshot );

/*=========================================================================
  FUNCTION:  qmi_ril_nw_reg_snapshot_is_same_as

===========================================================================*/
int qmi_ril_nw_reg_snapshot_is_same_as( qmi_ril_nw_reg_rte_snapshot_type* snapshot1, qmi_ril_nw_reg_rte_snapshot_type* snapshot2 );

/*=========================================================================
  FUNCTION:  qmi_ril_nw_reg_voice_is_voice_call_mode_reasonable_against_dev_cfg

===========================================================================*/
int qmi_ril_nw_reg_voice_is_voice_call_mode_reasonable_against_dev_cfg( call_mode_enum_v02 call_mode );

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_evaluate_data_rte_on_pref_data_tech_change

===========================================================================*/
void qcril_qmi_nas_evaluate_data_rte_on_pref_data_tech_change(void);

/*=========================================================================
  FUNCTION:  qmi_ril_nw_reg_data_pref_changed_action

===========================================================================*/
void qmi_ril_nw_reg_data_pref_changed_action(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_check_power_save_and_screen_off_status

===========================================================================*/
int  qcril_qmi_nas_check_power_save_and_screen_off_status(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_util_retrieve_technology_name

===========================================================================*/
const char* qcril_qmi_util_retrieve_technology_name(int technology);

/*=========================================================================
  FUNCTION:  qcril_qmi_util_retrieve_pref_data_tech_name

===========================================================================*/
const char* qcril_qmi_util_retrieve_pref_data_tech_name(int pref_data_tech);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_retrieve_rf_band_info

===========================================================================*/
void qcril_qmi_nas_retrieve_rf_band_info(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_get_mode_pref_from_nv_10

===========================================================================*/
boolean qcril_qmi_nas_get_mode_pref_from_nv_10(void);

/*=========================================================================
  FUNCTION:  qmi_ril_nwreg_mode_pref_enforcement_deferred_ncl

===========================================================================*/
int qmi_ril_nwreg_mode_pref_enforcement_deferred_ncl( int android_mode_pref );

/*=========================================================================
  FUNCTION:  qmi_ril_nwreg_mode_pref_is_pending_deferred_enforcement_ncl

===========================================================================*/
int qmi_ril_nwreg_mode_pref_is_pending_deferred_enforcement_ncl ( int * android_mode_pref );

/*=========================================================================
  FUNCTION:  qmi_ril_nwreg_mode_pref_enforce_deferred_op_handler

===========================================================================*/
void qmi_ril_nwreg_mode_pref_enforce_deferred_op_handler
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qmi_ril_nw_reg_get_current_rte_of_kind

===========================================================================*/
qmi_ril_nw_reg_rte_type qmi_ril_nw_reg_get_current_rte_of_kind( qmi_ril_nw_reg_rte_kind_type rte_kind );

/*=========================================================================
  FUNCTION:  qmi_ril_nw_reg_initiate_post_cfg_ban_for_data_reg_extrapolation_ncl

===========================================================================*/
void qmi_ril_nw_reg_initiate_post_cfg_ban_for_data_reg_extrapolation_ncl(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_fetch_system_selection_preference

===========================================================================*/
RIL_Errno qcril_qmi_fetch_system_selection_preference(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_get_mode_pref

===========================================================================*/
uint8_t qcril_qmi_nas_get_mode_pref(uint16_t *mode_pref);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_request_shutdown

===========================================================================*/
void qcril_qmi_nas_request_shutdown
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type       *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_dms_get_activity_info

===========================================================================*/
void qcril_qmi_nas_dms_get_activity_info
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_csg_handle_oem_hook_perform_network_scan

===========================================================================*/
void qcril_qmi_nas_csg_handle_oem_hook_perform_network_scan
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_csg_handle_oem_hook_set_sys_selection

===========================================================================*/
void qcril_qmi_nas_csg_handle_oem_hook_set_sys_selection
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_nw_select_dedicated_unsolicited_indicaton_event_thrd_handler

===========================================================================*/
void qcril_qmi_nas_nw_select_dedicated_unsolicited_indicaton_event_thrd_handler
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qmi_ril_nw_reg_data_sys_update_pre_update_action

===========================================================================*/
void qmi_ril_nw_reg_data_sys_update_pre_update_action( void );

/*=========================================================================
  FUNCTION:  qmi_ril_nwreg_common_ind_subscribe_consider_action_handler

===========================================================================*/
void qmi_ril_nwreg_common_ind_subscribe_consider_action_handler(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_cancel_radio_power_process

===========================================================================*/
void qcril_qmi_nas_cancel_radio_power_process(qmi_ril_dms_radio_pwr_cancel_e_type cancel_reason);

/*=========================================================================
  FUNCTION:  qmi_ril_nwreg_post_oprt_online_action_handler

===========================================================================*/
void qmi_ril_nwreg_post_oprt_online_action_handler
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_retrieve_reported_data_technology

===========================================================================*/
int qcril_qmi_nas_retrieve_reported_data_technology(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_retrieve_sv_capability

===========================================================================*/
dms_simul_voice_and_data_capability_mask_v01 qcril_qmi_nas_retrieve_sv_capability(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_assess_emergency_number_list_handler

===========================================================================*/
void qcril_qmi_nas_assess_emergency_number_list_handler
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_setting_srv_type_based_on_elaboration_and_rat

===========================================================================*/
voice_dial_call_service_type_enum_v02 qcril_qmi_nas_setting_srv_type_based_on_elaboration_and_rat
(
  uint64_t ela
);

/*=========================================================================
  FUNCTION:  is_operator_name_empty_or_white_space

===========================================================================*/
boolean is_operator_name_empty_or_white_space ( char * str, int max_len);

/*=========================================================================
  FUNCTION:  qmi_ril_nwreg_is_designated_number_enforcable_ncl

===========================================================================*/
int qmi_ril_nwreg_is_designated_number_enforcable_ncl(char * emergency_number);

/*=========================================================================
  FUNCTION:  qmi_ril_nwreg_convert_emergency_number_to_ims_address

===========================================================================*/
int qmi_ril_nwreg_convert_emergency_number_to_ims_address(char* emergency_number);

/*=========================================================================
  FUNCTION:  qmi_ril_nwreg_is_designated_number_emergency_for_display_purposes_only

===========================================================================*/
int qmi_ril_nwreg_is_designated_number_emergency_for_display_purposes_only(char * emergency_number);

/*=========================================================================
  FUNCTION:  qmi_ril_nwreg_designated_number_ensure_fresh_check_ncl

===========================================================================*/
void qmi_ril_nwreg_designated_number_ensure_fresh_check_ncl(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_voice_move_device_to_online_for_emer_call_conditionally

===========================================================================*/
RIL_Errno qcril_qmi_nas_voice_move_device_to_online_for_emer_call_conditionally(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_voice_move_device_to_lpm_after_emer_call_conditionally

===========================================================================*/
RIL_Errno qcril_qmi_nas_voice_move_device_to_lpm_after_emer_call_conditionally(void *);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_start_wait_for_pbm_ind_timer

===========================================================================*/
void qcril_qmi_nas_start_wait_for_pbm_ind_timer(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_centralized_pbm_ecc_support

===========================================================================*/
void qcril_qmi_nas_centralized_pbm_ecc_support(int);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_cancel_wait_for_pbm_ind_timer

===========================================================================*/
void qcril_qmi_nas_cancel_wait_for_pbm_ind_timer(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_embms_send_radio_state

===========================================================================*/
void qcril_qmi_nas_embms_send_radio_state(radio_state_enum_v01 radio_state);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_cdma_avoid_system

===========================================================================*/
void qcril_qmi_nas_cdma_avoid_system
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_cdma_clear_avoid_list

===========================================================================*/
void qcril_qmi_nas_cdma_clear_avoid_list
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_get_cdma_avoid_system_list

===========================================================================*/
void qcril_qmi_nas_get_cdma_avoid_system_list
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_enable_engineer_mode

===========================================================================*/
void qcril_qmi_nas_enable_engineer_mode
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_set_builtin_plmn_list

===========================================================================*/
void qcril_qmi_nas_set_builtin_plmn_list
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_perform_incremental_network_scan

===========================================================================*/
void qcril_qmi_nas_perform_incremental_network_scan
(
const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_get_cell_info_list_ncl

===========================================================================*/
void qcril_qmi_nas_get_cell_info_list_ncl

(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_set_cell_info_list_rate

===========================================================================*/
void qcril_qmi_nas_set_cell_info_list_rate
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_poll_cell_info_list

===========================================================================*/
void qcril_qmi_nas_poll_cell_info_list
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_cell_info_list_changed

===========================================================================*/
void qcril_qmi_nas_cell_info_list_changed
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_get_escv_type

===========================================================================*/
int qcril_qmi_nas_get_escv_type
(
    char *emergency_number
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_get_lte_disable_cause

===========================================================================*/
uint8_t qcril_qmi_nas_get_lte_disable_cause
(
    nas_lte_disable_cause_enum_type_v01 *lte_disable_cause
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_cancel_srv_domain_camped_timer_helper

===========================================================================*/
void qcril_qmi_nas_cancel_srv_domain_camped_timer_helper(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_modem_power_set_boot_in_apm

===========================================================================*/
void qcril_qmi_nas_modem_power_set_boot_in_apm(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_modem_power_ril_resumed

===========================================================================*/
void qcril_qmi_nas_modem_power_ril_resumed(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_modem_power_is_mdm_shdn_in_apm

===========================================================================*/
boolean qcril_qmi_nas_modem_power_is_mdm_shdn_in_apm(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_modem_power_load_apm_mdm_not_pwdn

===========================================================================*/
void qcril_qmi_nas_modem_power_load_apm_mdm_not_pwdn(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_drop_sig_info_cache

===========================================================================*/
void qcril_qmi_drop_sig_info_cache(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_sys_sel_pref_validity_tmr_expry_handler

===========================================================================*/
void qcril_qmi_nas_sys_sel_pref_validity_tmr_expry_handler(void * param);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_sys_sel_pref_setup_timed_callback

===========================================================================*/
int qcril_qmi_nas_sys_sel_pref_setup_timed_callback (void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_initialize_is_indication_received

===========================================================================*/
void qcril_qmi_nas_initialize_is_indication_received(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_cancel_sys_sel_pref_tmr

===========================================================================*/
void qcril_qmi_nas_cancel_sys_sel_pref_tmr(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_mode_pref_request_response_helper

===========================================================================*/
RIL_Errno qcril_qmi_nas_mode_pref_request_response_helper(const qcril_request_params_type *const params_ptr, uint8 *is_change);

/*=========================================================================
  FUNCTION:  qcril_qmi_is_srlte_supported

===========================================================================*/
int qcril_qmi_is_srlte_supported(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_fetch_lte_sms_status

===========================================================================*/
void qcril_qmi_nas_fetch_lte_sms_status ( uint8_t *lte_sms_status_valid, nas_sms_status_enum_type_v01 *lte_sms_status );

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_set_is_data_enabled

===========================================================================*/
void qcril_qmi_nas_set_is_data_enabled
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type       *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_set_is_data_roaming_enabled

===========================================================================*/
void qcril_qmi_nas_set_is_data_roaming_enabled
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type       *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_set_apn_info

===========================================================================*/
void qcril_qmi_nas_set_apn_info
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type       *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_dsds_request_set_lte_tune_away

===========================================================================*/
void qcril_qmi_nas_dsds_request_set_lte_tune_away
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_dsds_enable_lte_tune_away_cb

===========================================================================*/
void qcril_qmi_dsds_enable_lte_tune_away_cb ( RIL_Errno resp_res );

/*=========================================================================
  FUNCTION:  qcril_qmi_dsds_disable_lte_tune_away_cb

===========================================================================*/
void qcril_qmi_dsds_disable_lte_tune_away_cb ( RIL_Errno resp_res );

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_get_sim_mcc_mnc

===========================================================================*/
int qcril_qmi_nas_get_sim_mcc_mnc(char mcc[QCRIL_MCC_MNC_MAX_SIZE],
                                  char mnc[QCRIL_MCC_MNC_MAX_SIZE]);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_update_sim_mcc_mnc

===========================================================================*/
void qcril_qmi_nas_update_sim_mcc_mnc(boolean valid,
                                      char    mcc[QCRIL_MCC_MNC_MAX_SIZE],
                                      char    mnc[QCRIL_MCC_MNC_MAX_SIZE]);

/*=========================================================================
  FUNCTION:  qcril_uim_process_mcc_mnc_info

===========================================================================*/
void qcril_uim_process_mcc_mnc_info ( const qcril_request_params_type *const params_ptr,
                                      qcril_request_return_type       *const ret_ptr);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_did_rat_expansion_occur

===========================================================================*/
boolean qcril_qmi_nas_did_rat_expansion_occur
(
    void
);

/*=========================================================================
  FUNCTION:  qcril_qmi_get_modem_stack_id

===========================================================================*/
qcril_modem_stack_id_e_type qcril_qmi_get_modem_stack_id(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_wave_voice_data_status
===========================================================================*/
void qcril_qmi_nas_wave_voice_data_status();

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_fetch_data_reg_rat
===========================================================================*/
uint8_t qcril_qmi_nas_fetch_data_reg_rat(uint8_t *reported_tech_valid, int *reported_tech);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_reset_data_snapshot_cache_and_timer
===========================================================================*/
void qcril_qmi_nas_reset_data_snapshot_cache_and_timer();

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_get_radio_capability

===========================================================================*/
void qcril_qmi_nas_get_radio_capability
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_set_radio_capability

===========================================================================*/
void qcril_qmi_nas_set_radio_capability
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_embms_send_embms_status

===========================================================================*/
void qcril_qmi_nas_embms_send_embms_status (uint8_t is_avaliable);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_reset_modem

===========================================================================*/
RIL_Errno qcril_qmi_nas_reset_modem
(
    void
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_check_for_hardware_update

===========================================================================*/
void qcril_qmi_nas_check_for_hardware_update(boolean is_on_request);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_check_ps_attach_status

===========================================================================*/
void qcril_qmi_nas_check_ps_attach_status();

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_set_init_attch_state

===========================================================================*/
void qcril_qmi_nas_set_init_attch_state
(
    qmi_ril_nas_init_apn_attch_state_e_type init_attch_state
);

/*=========================================================================
  FUNCTION:  qmi_ril_is_rat_tlv_support_available

===========================================================================*/
int qmi_ril_is_rat_tlv_support_available(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_get_band_pref_map

===========================================================================*/
uint8_t qcril_qmi_nas_get_band_pref_map
(
    qcril_qmi_rat_band_e_type band_type,
    qcril_qmi_band_pref_e_type *band_pref_map
);

/*=========================================================================
  FUNCTION:  qmi_ril_nas_cache_deferred_band_pref

===========================================================================*/
uint8_t qmi_ril_nas_cache_deferred_band_pref
(
    qcril_qmi_rat_band_e_type band_type,
    qcril_qmi_band_pref_e_type band_pref_map
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_get_band_pref

===========================================================================*/
uint8_t qcril_qmi_nas_get_band_pref
(
    qcril_qmi_rat_band_e_type band_type,
    uint64_t *band_pref
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_get_max_data_handler

===========================================================================*/
void qcril_qmi_nas_get_max_data_handler
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_send_unsol_csg_id_changed

===========================================================================*/
void qcril_qmi_nas_send_unsol_max_data_changed();

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_get_csg_id_handler

===========================================================================*/
void qcril_qmi_nas_get_csg_id_handler
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_dms_get_max_apss_subscritions

===========================================================================*/
int qcril_qmi_dms_get_max_apss_subscritions(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_set_lte_band_pref

===========================================================================*/
void qcril_qmi_nas_set_lte_band_pref
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_populate_sig_config_delta_info

===========================================================================*/
void qcril_qmi_nas_populate_sig_config_delta_info(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_configure_sig_info2

===========================================================================*/
RIL_Errno qcril_qmi_nas_configure_sig_info2(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_is_ims_available

===========================================================================*/
uint8_t qcril_qmi_nas_is_ims_available(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_send_known_signal_strength

===========================================================================*/
void qcril_qmi_nas_send_known_signal_strength(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_fetch_sys_info

===========================================================================*/
RIL_Errno qcril_qmi_nas_fetch_sys_info(void);

#if 0
/*=========================================================================
  FUNCTION:  qcril_qmi_nas_conv_nas_mode_pref_to_qcril

===========================================================================*/
qcril_subs_mode_pref qcril_qmi_nas_conv_nas_mode_pref_to_qcril
(
    uint8_t info_valid,
    uint16_t qmi_nas_mode_pref,
    RIL_SelectUiccSub* master_request_ref
);
#endif //0

/*===========================================================================

  FUNCTION:  qcril_qmi_nas_update_prov_iccid

===========================================================================*/
/*!
    @brief
    Send updated iccid to prov module.

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_nas_update_prov_iccid
(
  int slot
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_dsds_is_active

===========================================================================*/
int qcril_qmi_nas_dsds_is_active(void);

/*=========================================================================
  FUNCTION:  qmi_ril_enable_ril_operational_status

===========================================================================*/
void qmi_ril_enable_ril_operational_status(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_get_flexi_map_status

===========================================================================*/
qcril_flexi_map_status qcril_qmi_nas_get_flexi_map_status(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_prov_set_gw_provision_state

===========================================================================*/
void qcril_qmi_prov_set_gw_provision_state(qcril_card_app_provision_status prov_state);

/*=========================================================================
  FUNCTION:  qcril_qmi_prov_set_cdma_provision_state

===========================================================================*/
void qcril_qmi_prov_set_cdma_provision_state(qcril_card_app_provision_status prov_state);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_send_unsol_csg_id_changed

===========================================================================*/
void qcril_qmi_nas_send_unsol_csg_id_changed(uint32_t csg_id);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_get_embms_status

===========================================================================*/
uint8_t qcril_qmi_nas_get_embms_status(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_update_embms_status

===========================================================================*/
void qcril_qmi_nas_update_embms_status(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_report_change_in_csg_id

===========================================================================*/
void qcril_qmi_nas_report_change_in_csg_id();

//=========================================================================
// qcril_qmi_nas_is_registered_plmn
//===========================================================================
int qcril_qmi_nas_is_registered_plmn(char *mcc, char *mnc);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_is_voice_over_lte_registered

===========================================================================*/
uint8_t qcril_qmi_nas_is_voice_over_lte_registered(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_check_long_short_name_as_mcc_mnc
===========================================================================*/
uint8_t qcril_qmi_nas_check_long_short_name_as_mcc_mnc(const char *mcc, const char *mnc, char *qmi_nas_eons_name, char *ril_eons_name);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_is_lte_only_registered

===========================================================================*/
uint8_t qcril_qmi_nas_is_lte_only_registered(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_get_rat_rac_handler
===========================================================================*/
void qcril_qmi_nas_get_rat_rac_handler
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_send_unsol_rat_rac_changed
===========================================================================*/
void qcril_qmi_nas_send_unsol_rat_rac_changed(uint8_t rat, uint8_t rac);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_clear_reported_rat_rac_cache
===========================================================================*/
void qcril_qmi_nas_clear_reported_rat_rac_cache();

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_report_change_in_rat_rac
===========================================================================*/
void qcril_qmi_nas_report_change_in_rat_rac();

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_is_apm_enabled
===========================================================================*/
int qcril_qmi_nas_is_apm_enabled();

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_set_operating_mode_state
===========================================================================*/
void qcril_qmi_nas_set_operating_mode_state(dms_operating_mode_enum_v01 requested_operating_mode);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_decide_mode_pref_to_set_on_modem
===========================================================================*/
void qcril_qmi_nas_decide_mode_pref_to_set_on_modem(int *android_mode_pref);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_update_mode_pref_prop
===========================================================================*/
void qcril_qmi_nas_update_mode_pref_prop(int android_mode_pref);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_get_bootup_power_optimization_state
===========================================================================*/
uint8_t qcril_qmi_nas_get_bootup_power_optimization_state();

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_set_bootup_power_optimization_state
===========================================================================*/
void qcril_qmi_nas_set_bootup_power_optimization_state(uint8_t status);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_report_vrt_for_power_optimization
===========================================================================*/
void qcril_qmi_nas_report_vrt_for_power_optimization();

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_oem_hook_set_atel_ui_status_to_modem
===========================================================================*/
void qcril_qmi_nas_oem_hook_set_atel_ui_status_to_modem
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_set_atel_ui_status_to_modem
===========================================================================*/
RIL_Errno qcril_qmi_nas_set_atel_ui_status_to_modem(uint8_t ui_status);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_get_atel_ui_status_from_modem
===========================================================================*/
RIL_Errno qcril_qmi_nas_get_atel_ui_status_from_modem(uint8_t *ui_status_valid, uint8_t *ui_status);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_get_atel_ui_status_from_cache
===========================================================================*/
uint8_t qcril_qmi_nas_get_atel_ui_status_from_cache();

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_set_atel_ui_status_to_cache
===========================================================================*/
void qcril_qmi_nas_set_atel_ui_status_to_cache(uint8_t ui_status);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_clean_atel_ui_status_from_cache
===========================================================================*/
void qcril_qmi_nas_clean_atel_ui_status_from_cache();

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_notify_ussd_ind_hdlr
===========================================================================*/
void qcril_qmi_nas_notify_ussd_ind_hdlr(void* decoded_payload, uint32_t decoded_payload_len);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_notify_mt_sms_ind_hdlr
===========================================================================*/
void qcril_qmi_nas_notify_mt_sms_ind_hdlr(wms_event_report_ind_msg_v01* decoded_payload, qcril_instance_id_e_type instance_id);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_mt_sms_power_opt_transaction_expired
===========================================================================*/
void qcril_qmi_nas_mt_sms_power_opt_transaction_expired(void * param);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_set_atel_ui_ready_req_came_to_cache
===========================================================================*/
void qcril_qmi_nas_set_atel_ui_ready_req_came_to_cache(uint8_t status);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_clean_atel_ui_ready_req_came_from_cache
===========================================================================*/
void qcril_qmi_nas_clean_atel_ui_ready_req_came_from_cache();

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_get_atel_ui_ready_req_came_from_cache
===========================================================================*/
uint8_t qcril_qmi_nas_get_atel_ui_ready_req_came_from_cache();

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_notify_ril_nitz_time_to_atel
===========================================================================*/
void qcril_qmi_nas_notify_ril_nitz_time_to_atel();

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_is_feature_power_opt_enable
===========================================================================*/
uint8_t qcril_qmi_nas_is_feature_power_opt_enable();

void qcril_qmi_nas_disable_data_dormancy_indication( void );

void qcril_qmi_nas_vops_info_get_request
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type *const ret_ptr /*!< Output parameter */
);

void qcril_qmi_nas_ssac_info_get_request
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type *const ret_ptr /*!< Output parameter */
);

void qcril_qmi_nas_ssac_info_resp_handler( const qcril_request_params_type * );

void qcril_qmi_nas_vops_info_resp_handler( const qcril_request_params_type * );

void update_sys_info_cache_from_resp(nas_get_sys_info_resp_msg_v01 * msg);

QCRIL_QMI_EXTERN(qmi_dms_get_msisdn_resp_hldr);

void qcril_qmi_nas_add_emergency_numbers
(
    uint32_t* cur_status,
    int* res,
    int* need_update_cur_status,
    char* emergency_nums
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_start_timer_if_early_radio_power_req_came
===========================================================================*/
void qcril_qmi_nas_start_timer_if_early_radio_power_req_came(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_initiate_radio_power_process
===========================================================================*/
void qcril_qmi_nas_initiate_radio_power_process(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_invalidate_data_snapshot_if_applicable
===========================================================================*/
void qcril_qmi_nas_invalidate_data_snapshot_if_applicable(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_is_appropriate_srv_status
===========================================================================*/
uint8_t qcril_qmi_nas_is_appropriate_srv_status(nas_service_status_enum_type_v01 srv_status);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_evaluate_data_rat_to_decide_ccs
===========================================================================*/
int qcril_qmi_nas_evaluate_data_rat_to_decide_ccs();

int qcril_qmi_nas_signal_strength_con_conv_cache2ril(RIL_SignalStrength* ril_msg);
int qcril_qmi_nas_is_rte_registered(qmi_ril_nw_reg_rte_type rte);
int qcril_qmi_nas_util_calculate_neighbor_cell_rssi_no_cache(int cell_value, int radioif);
/*=========================================================================
  FUNCTION:  qcril_qmi_nas_wave_data_reg_in_case_of_csfb
===========================================================================*/
void qcril_qmi_nas_wave_data_reg_in_case_of_csfb(uint8_t is_need_to_report_unsol);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_invalidate_data_snapshot_in_case_of_csfb_in_alerting_state
===========================================================================*/
void qcril_qmi_nas_invalidate_data_snapshot_in_case_of_csfb_in_alerting_state();

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_is_mcc_of_gcf_card
===========================================================================*/
uint8_t qcril_qmi_nas_is_mcc_of_gcf_card(const char *mcc_from_imsi);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_is_gcf_card_present_in_slot
===========================================================================*/
void qcril_qmi_nas_is_gcf_card_present_in_slot(uint8_t slot, RIL_CardStatus_v6 *ril_card_status);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_print_atel_ui_status
===========================================================================*/
void qcril_qmi_nas_print_atel_ui_status();

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_event_card_status_change_ind
===========================================================================*/
void qcril_qmi_nas_event_card_status_change_ind
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_get_voice_domain_preference
===========================================================================*/
RIL_Errno qcril_qmi_nas_get_voice_domain_preference
(
  nas_voice_domain_pref_enum_type_v01 *voice_domain_pref
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_query_network_time
===========================================================================*/
uint8 qcril_qmi_nas_query_network_time(char *ril_nitz_time_msg);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_query_sib16_network_time
===========================================================================*/
uint8 qcril_qmi_nas_query_sib16_network_time(char *ril_nitz_time_msg);

// function to get the maximum subscription that modem allowed
uint8_t qcril_qmi_nas_get_max_subscriptions( void );
void qcril_common_update_current_imsi( char * imsi_str, int is_gwl );
void qcril_qmi_start_mbn_update();

/*=========================================================================
FUNCTION:  qcril_qmi_nas_handle_data_call_status_notification
===========================================================================*/
void qcril_qmi_nas_handle_data_call_status_notification(qcril_qmi_nas_data_call_event_type evt);

/*=========================================================================
FUNCTION:  qcril_qmi_nas_decide_need_to_reg_pbm_ind
===========================================================================*/
uint8_t qcril_qmi_nas_decide_need_to_reg_pbm_ind();

void qcril_qmi_nas_set_initial_attach_apn_part1(const qcril_request_params_type *const params_ptr);
void qcril_qmi_nas_set_initial_attach_apn_part2(RIL_Errno data_res, const qcril_request_params_type *const params_ptr);

// check if SIM is provisioned for current instance
boolean qcril_qmi_nas_is_sim_provisioned();

/*=========================================================================
  FUNCTION:  QCRIL_EVT_DATA_NEW_DDS_INFO
===========================================================================*/
void qcril_qmi_nas_data_event_new_dds_info
(
  DDSSubIdInfo dds_sub_info
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_is_dds_change
===========================================================================*/
uint8_t qcril_qmi_nas_is_dds_change();

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_dds_to_be_updated
===========================================================================*/
void qcril_qmi_nas_dds_to_be_updated(DDSSubIdInfo *dds_sub);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_data_event_dds_status_follow_up
===========================================================================*/
void qcril_qmi_nas_data_event_dds_status_follow_up
(
  dds_status_info dds_status
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_set_dds_timeout_handler
===========================================================================*/
void qcril_qmi_nas_set_dds_timeout_handler
(
    void *param
);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_set_dds_through_data_layer
===========================================================================*/
RIL_Errno qcril_qmi_nas_set_dds_start_timer();

RIL_Errno qcril_qmi_nas_update_dds_sub_id(DDSSubIdInfo *dds_sub);

/*===========================================================================
  FUNCTION:  qcril_qmi_nas_get_roam_pref
===========================================================================*/
uint8_t qcril_qmi_nas_get_roam_pref(uint16_t *roam_pref);

/*=====================================================================================
  FUNCTION:  qcril_qmi_nas_fetch_sub_blocked_status
======================================================================================*/
RIL_Errno qcril_qmi_nas_fetch_sub_blocked_status();

/*=====================================================================================
  FUNCTION:  qcril_qmi_nas_sub_blocked_status_ind_handler
======================================================================================*/
void qcril_qmi_nas_sub_blocked_status_ind_handler(nas_sub_blocked_status_ind_msg_v01 * qmi_msg,
                                                  int *network_state_updated);

/*=====================================================================================
  FUNCTION:  qcril_qmi_nas_send_response_for_pending_network_selection_request
======================================================================================*/
void qcril_qmi_nas_send_response_for_pending_network_selection_request();
#ifdef QMI_RIL_UTF
void qcril_qmi_hal_nas_module_cleanup();
#endif

/*=====================================================================================
  FUNCTION:  qcril_qmi_nas_nw_select_is_registered_plmn_same_as_requested_plmn_with_rat
======================================================================================*/
uint8_t qcril_qmi_nas_nw_select_is_registered_plmn_same_as_requested_plmn_with_rat();

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_is_1x_sms_blocked
===========================================================================*/
int qcril_qmi_nas_is_1x_sms_blocked(void);

/*=========================================================================
  FUNCTION:  qcril_qmi_nas_bar_fake_gcell
===========================================================================*/
void qcril_qmi_nas_bar_fake_gcell
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type *const ret_ptr
);

/*=====================================================================================
  FUNCTION:  qcril_qmi_nas_is_using_radio_based_on_data_rat_sys_info
======================================================================================*/
int qcril_qmi_nas_is_using_radio_based_on_data_rat_sys_info(uint8_t radio_if);

/*======================================================
  FUNCTION:  qcril_qmi_nas_send_response_for_dds_request
========================================================*/
void qcril_qmi_nas_send_response_for_dds_request(void);

/*=====================================================================================
  FUNCTION:  qcril_qmi_nas_get_l_plus_l_feature_support_status
======================================================================================*/
void qcril_qmi_nas_get_l_plus_l_feature_support_status
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

/*=====================================================================================
  FUNCTION:  qcril_qmi_nas_is_device_registered_for_cs_domain
======================================================================================*/
uint8_t qcril_qmi_nas_is_device_registered_for_cs_domain(void);

#ifdef __cplusplus
} // end of extern "C"
#endif

int qcril_qmi_nas_has_normal_voice_call_capability();

void qcril_qmi_nas_modem_shutdown_state_machine(
    qmi_ril_nas_modem_shutdown_events_e_type evt);

void qcril_qmi_nas_cancel_ims_deregistration_timer();

#endif /* QCRIL_QMI_NAS_H */

