/*!
  @file
  qcril_qmi_radio_config_imss.h

  @brief
  Contains imss related radio config req and response handlers

*/

/*===========================================================================

  Copyright (c) 2015,2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef QCRIL_QMI_RADIO_CONFIG_IMSS_H
#define QCRIL_QMI_RADIO_CONFIG_IMSS_H

#include "imsIF.pb.h"
#include "qcrili.h"
#include "qcril_qmi_radio_config_meta.h"

/* Video Quality values as per imsIF.proto */
typedef enum
{
  QCRIL_QMI_RADIO_CONFIG_IMSS_VIDEO_QUALITY_NONE = -1,
  QCRIL_QMI_RADIO_CONFIG_IMSS_VIDEO_QUALITY_LOW = 0,
  QCRIL_QMI_RADIO_CONFIG_IMSS_VIDEO_QUALITY_MID = 1,
  QCRIL_QMI_RADIO_CONFIG_IMSS_VIDEO_QUALITY_HIGH = 2
}qcril_qmi_radio_config_imss_video_quality;

//Helper enum and functions to convert the
//wifi calling roaming settings.
typedef enum
{
  QCRIL_QMI_RADIO_CONFIG_IMSS_VOICE_OVER_WIFI_ROAMING_NONE = -1,
  QCRIL_QMI_RADIO_CONFIG_IMSS_VOICE_OVER_WIFI_ROAMING_DISABLED = 0,
  QCRIL_QMI_RADIO_CONFIG_IMSS_VOICE_OVER_WIFI_ROAMING_ENABLED = 1
}qcril_qmi_radio_config_imss_voice_over_wifi_roaming_type;

//Helper enum and functions to convert the
//wifi calling preference mode settings.
typedef enum
{
  QCRIL_QMI_RADIO_CONFIG_IMSS_VOICE_OVER_WIFI_MODE_NONE = -1,
  QCRIL_QMI_RADIO_CONFIG_IMSS_VOICE_OVER_WIFI_MODE_WIFI_ONLY = 0,
  QCRIL_QMI_RADIO_CONFIG_IMSS_VOICE_OVER_WIFI_MODE_CELLULAR_PREFERRED = 1,
  QCRIL_QMI_RADIO_CONFIG_IMSS_VOICE_OVER_WIFI_MODE_WIFI_PREFERRED = 2
}qcril_qmi_radio_config_imss_voice_over_wifi_mode_type;

ims_settings_wfc_roaming_enum_v01 qcril_qmi_radio_config_imss_map_radio_config_wifi_roaming_to_ims_wifi_roaming
(
  qcril_qmi_radio_config_imss_voice_over_wifi_roaming_type radio_config_wifi_roaming
);
/*===========================================================================
                          GET REQUEST HANDLERS
===========================================================================*/

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_voip_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_sip_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_reg_mgr_extended_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_client_provisioning_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_user_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_sms_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_presence_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_qipcall_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_reg_mgr_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_handover_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_qipcall_vice_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

/*===========================================================================
                          SET REQUEST HANDLERS
===========================================================================*/

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_voip_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_sip_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_reg_mgr_extended_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_client_provisioning_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_user_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_sms_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_presence_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_qipcall_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_reg_mgr_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_handover_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_qipcall_vice_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

/*===========================================================================
                           GET RESPONSE HANDLERS
===========================================================================*/

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_voip_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_sip_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_reg_mgr_extended_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_client_provisioning_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_user_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_sms_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_presence_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_qipcall_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_reg_mgr_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_handover_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_qipcall_vice_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

/*===========================================================================
                           SET RESPONSE HANDLERS
===========================================================================*/

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_voip_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_sip_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_reg_mgr_extended_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_client_provisioning_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_user_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_sms_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_presence_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_qipcall_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_reg_mgr_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_handover_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_qipcall_vice_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

char* qcril_qmi_radio_config_get_string_amr_or_amr_wb_mode(
  uint16_t byte_value, uint32_t max_index
);

uint64_t qcril_qmi_radio_config_get_amr_or_amr_wb_mode_mask(const char *val, long mode_max);


#endif /* QCRIL_QMI_RADIO_CONFIG_IMSS_H */
