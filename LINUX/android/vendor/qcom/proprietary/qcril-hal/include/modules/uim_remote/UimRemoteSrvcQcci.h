/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#define QMI_UIM_REMOTE_MAX_ATR_LEN 32
#define QMI_UIM_REMOTE_MAX_COMMAND_APDU_LEN 261
#define QMI_UIM_REMOTE_MAX_RESPONSE_APDU_SEGMENT_LEN 1024

typedef enum
{
  QMI_UIM_REMOTE_RESULT_SUCCESS = 0x0,
  QMI_UIM_REMOTE_RESULT_FAILURE = 0x1
} qmi_uim_remote_result;

typedef enum
{
  QMI_UIM_REMOTE_CONNECTION_UNAVAILABLE = 0x0,
  QMI_UIM_REMOTE_CONNECTION_AVAILABLE = 0x1,
  QMI_UIM_REMOTE_CARD_INSERTED = 0x2,
  QMI_UIM_REMOTE_CARD_REMOVED = 0x3,
  QMI_UIM_REMOTE_CARD_ERROR = 0x4,
  QMI_UIM_REMOTE_CARD_RESET = 0x5,
  QMI_UIM_REMOTE_CARD_WAKEUP = 0x6
} qmi_uim_remote_event_type;

typedef enum
{
  QMI_UIM_REMOTE_SLOT_NOT_APPLICABLE = 0x0,
  QMI_UIM_REMOTE_SLOT_1 = 0x1,
  QMI_UIM_REMOTE_SLOT_2 = 0x2,
  QMI_UIM_REMOTE_SLOT_3 = 0x3
} qmi_uim_remote_slot_type;

typedef enum
{
  QMI_UIM_REMOTE_CARD_ERROR_UNKNOWN_ERROR = 0x0,
  QMI_UIM_REMOTE_CARD_ERROR_NO_LINK_ESTABLISHED = 0x1,
  QMI_UIM_REMOTE_CARD_ERROR_COMMAND_TIMEOUT = 0x2,
  QMI_UIM_REMOTE_CARD_ERROR_DUE_TO_POWER_DOWN = 0x3,
  QMI_UIM_REMOTE_CARD_ERROR_DUE_TO_POWER_DOWN_TELECOM = 0x4
} qmi_uim_remote_card_error_type;

typedef enum
{
  QMI_UIM_REMOTE_TRANSPORT_OTHER = 0x0,
  UIM_REMOTE_TRANSPORT_BLUETOOTH = 0x1,
  UIM_REMOTE_TRANSPORT_IP = 0x2
} qmi_uim_remote_transport;

typedef enum
{
  QMI_UIM_REMOTE_USAGE_REDUCED_V01 = 0x0,
  UIM_REMOTE_USAGE_NORMAL = 0x1,
} qmi_uim_remote_usage;

typedef enum
{
  QMI_UIM_REMOTE_VOLTAGE_CLASS_C_LOW = 0x0,
  QMI_UIM_REMOTE_VOLTAGE_CLASS_C = 0x1,
  QMI_UIM_REMOTE_VOLTAGE_CLASS_C_HIGH = 0x2,
  QMI_UIM_REMOTE_VOLTAGE_CLASS_B_LOW = 0x3,
  QMI_UIM_REMOTE_VOLTAGE_CLASS_B = 0x4,
  QMI_UIM_REMOTE_VOLTAGE_CLASS_B_HIGH = 0x5
} qmi_uim_remote_voltage_class;

typedef enum
{
  QMI_UIM_REMOTE_POWER_DOWN_TELECOM_INTERFACE = 0x0,
  QMI_UIM_REMOTE_POWER_DOWN_CARD = 0x1
} qmi_uim_remote_power_down_mode;

typedef enum
{
  QMI_UIM_REMOTE_INVALID_REQUEST = 0x0,
  QMI_UIM_REMOTE_REQ_EVENT_REQUEST = 0x1,
  QMI_UIM_REMOTE_REQ_APDU_REQUEST = 0x2
} qmi_uim_remote_req_type;

typedef enum
{
  QMI_UIM_REMOTE_APDU_IND = 0x0,
  QMI_UIM_REMOTE_CONNECT_IND = 0x1,
  QMI_UIM_REMOTE_DISCONNECT_IND = 0x2,
  QMI_UIM_REMOTE_POWER_DOWN_IND = 0x3,
  QMI_UIM_REMOTE_POWER_UP_IND = 0x4,
  QMI_UIM_REMOTE_CARD_RESET_IND = 0x5,
  QMI_UIM_REMOTE_SRVC_UP_IND_MSG = 0x6,
  QMI_UIM_REMOTE_SRVC_DOWN_IND_MSG = 0x7
} qmi_uim_remote_indication_type;

typedef struct
{
  qmi_uim_remote_event_type event;
  qmi_uim_remote_slot_type slot;
  uint32_t atr_len;
  uint8_t atr[QMI_UIM_REMOTE_MAX_ATR_LEN];
  bool wakeup_support_valid;
  uint8_t wakeup_support;
  bool error_cause_valid;
  qmi_uim_remote_card_error_type error_cause;
  bool transport_valid;
  qmi_uim_remote_transport transport;
  bool usage_valid;
  qmi_uim_remote_usage usage;
  bool apdu_timeout_valid;
  uint32_t apdu_timeout;
  bool disable_all_polling_valid;
  uint8_t disable_all_polling;
  bool poll_interval_valid;
  uint32_t poll_interval;
} qmi_uim_remote_event_req_msg;

typedef struct
{
  uint32_t total_response_apdu_size;
  uint32_t response_apdu_segment_offset;
} qmi_uim_remote_response_apdu_info_type;

typedef struct
{
  qmi_uim_remote_result apdu_status;
  qmi_uim_remote_slot_type slot;
  uint32_t apdu_id;
  bool response_apdu_info_valid;
  qmi_uim_remote_response_apdu_info_type response_apdu_info;
  uint32_t response_apdu_segment_len;
  uint8_t response_apdu_segment[QMI_UIM_REMOTE_MAX_RESPONSE_APDU_SEGMENT_LEN];
} qmi_uim_remote_apdu_req_msg;

typedef struct
{
  int32_t                   token;
  qmi_uim_remote_req_type   req_id;
  union {
    qmi_uim_remote_event_req_msg event_req;
    qmi_uim_remote_apdu_req_msg  apdu_req;
  } req_data;
} qmi_uim_remote_request_msg;

typedef struct
{
  qmi_uim_remote_result result;
  int                   trans_err;
  uint32_t              err_code;
} qmi_uim_remote_resp_msg;

typedef struct
{
  uint32_t apdu_id;
  uint32_t command_apdu_len;
  uint8_t command_apdu[QMI_UIM_REMOTE_MAX_COMMAND_APDU_LEN];
} qmi_uim_remote_apdu_ind_msg;

typedef struct
{
  bool mode_valid;
  qmi_uim_remote_power_down_mode mode;
} qmi_uim_remote_card_power_down_ind_msg;

typedef struct
{
  bool response_timeout_valid;
  uint32_t response_timeout;
  bool voltage_class_valid;
  qmi_uim_remote_voltage_class voltage_class;
} qmi_uim_remote_card_power_up_ind_msg;

typedef struct
{
  qmi_uim_remote_indication_type  ind_id;
  union {
    qmi_uim_remote_apdu_ind_msg            apdu_ind;
    qmi_uim_remote_card_power_down_ind_msg pdown_ind;
    qmi_uim_remote_card_power_up_ind_msg   pup_ind;
  }                               ind_data;
} qmi_uim_remote_ind_msg;
