/*!
  @file
  qcril_qmi_radio_config_meta.h

  @brief
   Contains structures and functions needed for config settings
*/

/*===========================================================================

  Copyright (c) 2015,2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef QCRIL_QMI_RADIO_CONFIG_META_H
#define QCRIL_QMI_RADIO_CONFIG_META_H

#include "qcrili.h"
#include "ip_multimedia_subsystem_settings_v01.h"

/*===========================================================================
                               Definitions
===========================================================================*/

#define QCRIL_QMI_RADIO_CONFIG_SETTINGS_STRING_LEN_MAX 255

//Max number of values available for amr_mode
#define QCRIL_QMI_RADIO_CONFIG_VOIP_AMR_MODE_MAX_INDEX 8
#define QCRIL_QMI_RADIO_CONFIG_VOIP_AMR_WB_MODE_MAX_INDEX 9

/* Contains the list of Radio Config items supported */
typedef enum
{
    QCRIL_QMI_RADIO_CONFIG_INVALID = -1,
    QCRIL_QMI_RADIO_CONFIG_VOIP_AMR_MODE = 0,
    QCRIL_QMI_RADIO_CONFIG_VOIP_AMR_WB_MODE = 1,
    QCRIL_QMI_RADIO_CONFIG_VOIP_SESSION_EXPIRY_TIMER = 2,
    QCRIL_QMI_RADIO_CONFIG_VOIP_MIN_SESSION_EXPIRY = 3,
    QCRIL_QMI_RADIO_CONFIG_VOIP_SILENT_REDIAL_ENABLED = 4,
    QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_OPERATOR_MODE_A = 5,
    QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_T1 = 6,
    QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_T2 = 7,
    QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TF = 8,
    QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_T4 = 9,
    QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TA_VALUE = 10,
    QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TB_VALUE = 11,
    QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TD_VALUE = 12,
    QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TE_VALUE = 13,
    QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TG_VALUE = 14,
    QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TH_VALUE = 15,
    QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TI_VALUE = 16,
    QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TJ = 17,
    QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TK_VALUE = 18,
    QCRIL_QMI_RADIO_CONFIG_SIP_KEEPALIVE_ENABLED = 19,
    QCRIL_QMI_RADIO_CONFIG_REG_MGR_EXTENDED_T_DELAY = 20,
    QCRIL_QMI_RADIO_CONFIG_REG_MGR_EXTENDED_REG_RETRY_BASE_TIME = 21,
    QCRIL_QMI_RADIO_CONFIG_REG_MGR_EXTENDED_REG_RETRY_MAX_TIME = 22,
    QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_VOLTE = 23,
    QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_VT = 24,
    QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_PRESENCE = 25,
    QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_WIFI_CALL = 26,
    QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_WIFI_CALL_ROAMING = 27,
    QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_WIFI_CALL_PREFERENCE = 28,
    QCRIL_QMI_RADIO_CONFIG_USER_IMS_DOMAIN_NAME = 29,
    QCRIL_QMI_RADIO_CONFIG_SMS_FORMAT = 30,
    QCRIL_QMI_RADIO_CONFIG_SMS_OVER_IP = 31,
    QCRIL_QMI_RADIO_CONFIG_SMS_PSI = 32,
    QCRIL_QMI_RADIO_CONFIG_PRESENCE_PUBLISH_EXPIRY_TIMER = 33,
    QCRIL_QMI_RADIO_CONFIG_PRESENCE_PUBLISH_EXTENDED_EXPIRY_TIMER = 34,
    QCRIL_QMI_RADIO_CONFIG_PRESENCE_CAPABILITIES_CACHE_EXPIRATION = 35,
    QCRIL_QMI_RADIO_CONFIG_PRESENCE_AVAILABILITY_CACHE_EXPIRATION = 36,
    QCRIL_QMI_RADIO_CONFIG_PRESENCE_CAPABILITY_POLL_INTERVAL = 37,
    QCRIL_QMI_RADIO_CONFIG_PRESENCE_MINIMUM_PUBLISH_INTERVAL = 38,
    QCRIL_QMI_RADIO_CONFIG_PRESENCE_MAXIMUM_SUBSCRIPTION_LIST_ENTRIES = 39,
    QCRIL_QMI_RADIO_CONFIG_PRESENCE_CAPABILITY_POLL_LIST_SUBSCRIPTION_EXPIRY_TIMER = 40,
    QCRIL_QMI_RADIO_CONFIG_PRESENCE_GZIP_ENABLED = 41,
    QCRIL_QMI_RADIO_CONFIG_PRESENCE_VOLTE_USER_OPTED_IN_STATUS = 42,
    QCRIL_QMI_RADIO_CONFIG_PRESENCE_CAPABILITY_DISCOVERY_ENABLE = 43,
    QCRIL_QMI_RADIO_CONFIG_QIPCALL_MOBILE_DATA_ENABLED = 44,
    QCRIL_QMI_RADIO_CONFIG_QIPCALL_VOLTE_ENABLED = 45,
    QCRIL_QMI_RADIO_CONFIG_QIPCALL_VT_CALLING_ENABLED = 46,
    QCRIL_QMI_RADIO_CONFIG_QIPCALL_SPEECH_START_PORT = 47,
    QCRIL_QMI_RADIO_CONFIG_QIPCALL_SPEECH_END_PORT = 48,
    QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_WB_OCTET_ALIGNED_DYNAMIC_PT = 49,
    QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_WB_BANDWIDTH_EFFICIENT_DYNAMIC_PT = 50,
    QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_OCTET_ALIGNED_DYNAMIC_PT = 51,
    QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_BANDWIDTH_EFFICIENT_DYNAMIC_PT = 52,
    QCRIL_QMI_RADIO_CONFIG_QIPCALL_DTMF_WB_DYNAMIC_POINT = 53,
    QCRIL_QMI_RADIO_CONFIG_QIPCALL_DTMF_NB_DYNAMIC_PT = 54,
    QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_DEFAULT_MODE = 55,
    QCRIL_QMI_RADIO_CONFIG_QIPCALL_EMERG_CALL_TIMER = 56,
    QCRIL_QMI_RADIO_CONFIG_REG_MGR_CONFIG_REG_MGR_PRIMARY_CSCF = 57,
    QCRIL_QMI_RADIO_CONFIG_QIPCALL_VIDEO_QUALITY = 58,
    QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_LTE_RSRP_LOW = 59,
    QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_LTE_RSRP_MID = 60,
    QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_LTE_RSRP_HIGH = 61,
    QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_WIFI_RSSI_THRESHOLD_LOW = 62,
    QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_WIFI_RSSI_THRESHOLD_HIGH = 63,
    QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_WIFI_SINR_THRESHOLD_LOW = 64,
    QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_WIFI_SINR_THRESHOLD_HIGH = 65,
    QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_LTE_RSRP_LOW = 66,
    QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_LTE_RSRP_MID = 67,
    QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_LTE_RSRP_HIGH = 68,
    QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_WIFI_RSSI_THRESHOLD_LOW = 69,
    QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_WIFI_RSSI_THRESHOLD_HIGH = 70,
    QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_WIFI_SINR_THRESHOLD_LOW = 71,
    QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_WIFI_SINR_THRESHOLD_HIGH = 72,
    QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_ECIO_1X_THRESHOLD_LOW = 73,
    QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_ECIO_1X_THRESHOLD_HIGH = 74,
    QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_LTE_THRESHOLD1 = 75,
    QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_LTE_THRESHOLD2 = 76,
    QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_LTE_THRESHOLD3 = 77,
    QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_1X_THRESHOLD = 78,
    QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_WIFI_THRESHOLDA = 79,
    QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_WIFI_THRESHOLDB = 80,
    QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_WLAN_TO_WWAN_HYS_TIMER = 81,
    QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_WLAN_TO_1X_HYS_TIMER = 82,
    QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_WWAN_TO_WLAN_HYS_TIMER = 83,
    QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_VOWIFI = 84,
    QCRIL_QMI_RADIO_CONFIG_QIPCALL_VICE_ENABLED = 85,
    QCRIL_QMI_RADIO_CONFIG_SET_LTED_CONFIG   = 86,
    QCRIL_QMI_RADIO_CONFIG_GET_LTED_CONFIG   = 87,
    QCRIL_QMI_RADIO_CONFIG_SET_LTED_CATEGORY = 88,
    QCRIL_QMI_RADIO_CONFIG_GET_LTED_CATEGORY = 89,
    QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_RTT = 90,
    QCRIL_QMI_RADIO_CONFIG_RCS_SMS_APP = 91,
    QCRIL_QMI_RADIO_CONFIG_RCS_VVM_APP = 92,
}qcril_qmi_radio_config_item_type;

/* Enum specifying which type the config item is */
typedef enum
{
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INVALID = -1,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_BOOLEAN = 0,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT = 1,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_STRING = 2
}qcril_qmi_radio_config_item_value_type;

/* Enum for settings resp(failure cause) causes
 * TODO: need to decide if we can use this for
 * main error enum for radio config.
 * Keeping it seperate for simplicity*/
typedef enum
{
    QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR = 0,
    QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_IMS_NOT_READY = 1,
    QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_FILE_NOT_AVAILABLE = 2,
    QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_READ_FAILED = 3,
    QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_WRITE_FAILED = 4,
    QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_OTHER_INTERNAL_ERR = 5
}qcril_qmi_radio_config_settings_resp_type;

typedef enum
{
    QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS = 0,
    QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE = 1,
    QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_PARAMETER = 2,
    QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS = 3,
    QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_ITEM_VALUE_TYPE = 4,
    QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT = 5,

    //Possible QMI errors from qmi imss document
    QCRIL_QMI_RADIO_CONFIG_ERROR_NO_MEMORY = 6,
    QCRIL_QMI_RADIO_CONFIG_ERROR_ERROR_CAUSE_CODE = 7,
    QCRIL_QMI_RADIO_CONFIG_ERROR_REQUEST_NOT_SUPPORTED = 8, //REQUESTED_NUM_UNSUPPORTED
    QCRIL_QMI_RADIO_CONFIG_ERROR_MALFORMED_MSG = 9,
    QCRIL_QMI_RADIO_CONFIG_ERROR_INFO_UNAVAILABLE = 10
}qcril_qmi_radio_config_error_type;

/*===========================================================================
                               Structures
===========================================================================*/

/* Common type to be used for all the radio config function
 * handlers (set/get - req/resp)
 */
typedef struct
{
  qcril_qmi_radio_config_item_type config_item;
  void *config_item_value;
  size_t config_item_value_len;
  qcril_qmi_radio_config_item_value_type config_item_value_type;
  void *extra_data;
  size_t extra_data_len;
}qcril_qmi_radio_config_params_type;

typedef struct
{
  qcril_qmi_radio_config_item_type config_item;
  void *data;
  size_t data_len;
}qcril_qmi_radio_config_req_data_type;

/* Common type to be used as a return structure in all
 * the radio config function handlers (set/get - req/resp)
 */
typedef struct
{
  IxErrnoType qmi_client_error;
}qcril_qmi_radio_config_return_type;


/* Common function handler definition for all
 * the config handlers (set/get - req/resp)
 */
typedef qcril_qmi_radio_config_error_type qcril_qmi_radio_config_handler_type
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

/* Static table structure that stores the specific information needed
 * to process each config item
 */
typedef struct
{
  qcril_qmi_radio_config_item_type config_item;
  qcril_qmi_radio_config_handler_type *get_config_req_handler;
  qcril_qmi_radio_config_handler_type *get_config_resp_handler;
  qcril_qmi_radio_config_handler_type *set_config_req_handler;
  qcril_qmi_radio_config_handler_type *set_config_resp_handler;
  qcril_qmi_radio_config_item_value_type config_item_type;
  signed long qmi_get_resp_msg_id;
  signed long qmi_set_resp_msg_id;
  const char* log_str;
} qcril_qmi_radio_config_meta_type;

/*===========================================================================
                               Functions
===========================================================================*/

/* Returns function handler corresponding
 * to the GET REQUEST of the config item
 */
qcril_qmi_radio_config_handler_type* qcril_qmi_radio_config_find_get_config_req_handler
(
  qcril_qmi_radio_config_item_type config_item
);

/* Returns function handler corresponding
 * to the GET RESPONSE of the config item
 */
qcril_qmi_radio_config_handler_type* qcril_qmi_radio_config_validate_and_find_get_config_resp_handler
(
  qcril_qmi_radio_config_item_type config_item,
  unsigned long qmi_msg_id
);

/* Returns function handler corresponding
 * to the SET REQUEST of the config item
 */
qcril_qmi_radio_config_handler_type* qcril_qmi_radio_config_find_set_config_req_handler
(
   qcril_qmi_radio_config_item_type config_item
);

/* Returns function handler corresponding
 * to the SET RESPONSE of the config item
 */
qcril_qmi_radio_config_handler_type* qcril_qmi_radio_config_validate_and_find_set_config_resp_handler
(
   qcril_qmi_radio_config_item_type config_item,
   unsigned long qmi_msg_id
);

/* Returns the type of the value of the Config item */
qcril_qmi_radio_config_item_value_type qcril_qmi_radio_config_get_item_value_type
(
   qcril_qmi_radio_config_item_type config_item
);

/* Returns Log string corresponding
 * to the Config item
 */
const char* qcril_qmi_radio_config_get_item_log_str
(
   qcril_qmi_radio_config_item_type config_item
);

/* Maps Qmi settings response to
 * Radio config settings response type
 */
qcril_qmi_radio_config_settings_resp_type qcril_qmi_radio_config_map_qmi_settings_resp_to_radio_config_settings_resp
(
   ims_settings_rsp_enum_v01 qmi_ims_setting_failcause
);

/* Maps Qmi error response to
 * Radio config error type
 */
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_map_qmi_error_to_radio_config_error
(
   qmi_error_type_v01 qmi_error
);

/* Maps IxErrnoType error to
 * Radio config error type
 */
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_map_internal_error_to_radio_config_error
(
   IxErrnoType error
);

void qcril_qmi_radio_config_update_meta_table
(
  int modem_version
);
#endif /* QCRIL_QMI_RADIO_CONFIG_META_H */
