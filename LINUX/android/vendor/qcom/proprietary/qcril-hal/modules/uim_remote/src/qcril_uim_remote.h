#ifndef QCRIL_UIM_REMOTE_H
#define QCRIL_UIM_REMOTE_H
/*===========================================================================

  Copyright (c) 2014, 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*===========================================================================

                           TYPES

===========================================================================*/
typedef enum
{
  QCRIL_UIM_REMOTE_STATUS_CONN_UNAVAILABLE = 0,
  QCRIL_UIM_REMOTE_STATUS_CONN_AVAILABLE   = 1,
  QCRIL_UIM_REMOTE_STATUS_CARD_INSERTED    = 2,
  QCRIL_UIM_REMOTE_STATUS_CARD_REMOVED     = 3,
  QCRIL_UIM_REMOTE_STATUS_CARD_ERROR       = 4,
  QCRIL_UIM_REMOTE_STATUS_CARD_RESET       = 5,
  QCRIL_UIM_REMOTE_STATUS_CARD_WAKEUP      = 6
} qcril_uim_remote_event_type;

typedef enum
{
  QCRIL_UIM_REMOTE_ERR_SUCCESS           = 0,
  QCRIL_UIM_REMOTE_ERR_GENERIC_FAILURE   = 1,
  QCRIL_UIM_REMOTE_ERR_NOT_SUPPORTED     = 2,
  QCRIL_UIM_REMOTE_ERR_INVALID_PARAMETER = 3
} qcril_uim_remote_response_type;

typedef enum
{
  QCRIL_UIM_REMOTE_CARD_ERROR_UNKNOWN            = 0,
  QCRIL_UIM_REMOTE_CARD_ERROR_NO_LINK_EST        = 1,
  QCRIL_UIM_REMOTE_CARD_ERROR_CMD_TIMEOUT        = 2,
  QCRIL_UIM_REMOTE_CARD_ERROR_POWER_DOWN         = 3,
  QCRIL_UIM_REMOTE_CARD_ERROR_POWER_DOWN_TELECOM = 4
} qcril_uim_remote_error_cause_type;

typedef enum
{
  QCRIL_UIM_REMOTE_TRANSPORT_OTHER     = 0,
  QCRIL_UIM_REMOTE_TRANSPORT_BLUETOOTH = 1,
  QCRIL_UIM_REMOTE_TRANSPORT_IP        = 2
} qcril_uim_remote_transport_type;

typedef enum
{
  QCRIL_UIM_REMOTE_USAGE_REDUCED = 0,
  QCRIL_UIM_REMOTE_USAGE_NORMAL  = 1
} qcril_uim_remote_usage_type;

typedef enum
{
  QCRIL_UIM_REMOTE_CLIENT_EVENT_SUCCESS = 0,
  QCRIL_UIM_REMOTE_CLIENT_EVENT_FAILURE = 1
} qcril_uim_remote_event_resp_type;

typedef enum
{
  QCRIL_UIM_REMOTE_CLIENT_APDU_STATUS_SUCCESS = 0,
  QCRIL_UIM_REMOTE_CLIENT_APDU_STATUS_FAILURE = 1
} qcril_uim_remote_apdu_status;

typedef enum
{
  QCRIL_UIM_REMOTE_CLIENT_APDU_SUCCESS = 0,
  QCRIL_UIM_REMOTE_CLIENT_APDU_FAILURE = 1
} qcril_uim_remote_apdu_resp_type;

typedef enum
{
  QCRIL_UIM_REMOTE_VOLTAGE_CLASS_C_LOW  = 0,
  QCRIL_UIM_REMOTE_VOLTAGE_CLASS_C      = 1,
  QCRIL_UIM_REMOTE_VOLTAGE_CLASS_C_HIGH = 2,
  QCRIL_UIM_REMOTE_VOLTAGE_CLASS_B_LOW  = 3,
  QCRIL_UIM_REMOTE_VOLTAGE_CLASS_B      = 4,
  QCRIL_UIM_REMOTE_VOLTAGE_CLASS_B_HIGH = 5
} qcril_uim_remote_voltage_class_type;

typedef enum
{
  QCRIL_UIM_REMOTE_PDOWN_TELECOM_INTERFACE = 0,
  QCRIL_UIM_REMOTE_PDOWN_CARD              = 1
} qcril_uim_remote_power_down_mode;

typedef struct
{
  qcril_uim_remote_event_type         event_type;
  uint32_t                            atr_len;
  const uint8_t                     * atr;               //Present for event = UIM_REMOTE_STATUS_CARD_INSERTED
  bool                                has_wakeup_support;
  bool                                wakeup_support;    //Present if remote UIM supports wake-up feature
  bool                                has_error_code;
  qcril_uim_remote_error_cause_type   error_code;        //Present for evnt = UIM_REMOTE_STATUS_CARD_ERROR
  bool                                has_transport;
  qcril_uim_remote_transport_type     transport;
  bool                                has_usage;
  qcril_uim_remote_usage_type         usage;
  bool                                has_apdu_timeout;
  uint32_t                            apdu_timeout;
  bool                                has_disable_all_polling;
  uint32_t                            disable_all_polling;
  bool                                has_poll_timer;
  uint32_t                            poll_timer;
} qcril_uim_remote_event_req_type;

#endif /* QCRIL_UIM_REMOTE_H */
