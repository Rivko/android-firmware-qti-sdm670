/******************************************************************************
  @file    qcril_qmi_radio_config_imss_v02.h

  DESCRIPTION
    Handles data related radio config req and response handlers

  ---------------------------------------------------------------------------

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ---------------------------------------------------------------------------
******************************************************************************/
#ifndef QCRIL_QMI_RADIO_CONFIG_IMSS_V02_H
#define QCRIL_QMI_RADIO_CONFIG_IMSS_V02_H

#include "imsIF.pb.h"
#include "qcrili.h"
#include "qcril_qmi_radio_config_meta.h"

void qcril_qmi_imss_store_wifi_call_mode_preference_to_req_info
(
 uint16                          req_id,
 ims_settings_call_mode_option_enum_v01 call_mode_preference
);

/*===========================================================================
                          GET REQUEST HANDLERS
===========================================================================*/

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_voice_dynamic_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_codec_dynamic_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_sip_new_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_reg_mgr_new_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_ims_service_enable_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_sms_new_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_presence_new_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_rtp_dynamic_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_emer_dynamic_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_video_dynamic_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_rcs_new_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_ims_new_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

/*===========================================================================
                          SET REQUEST HANDLERS
===========================================================================*/

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_voice_dynamic_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_codec_dynamic_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_sip_new_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_reg_mgr_new_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_ims_service_enable_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_sms_new_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_presence_new_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_rtp_dynamic_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_emer_dynamic_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_video_dynamic_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_rcs_new_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_ims_new_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

/*===========================================================================
                           GET RESPONSE HANDLERS
===========================================================================*/

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_voice_dynamic_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_codec_dynamic_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_sip_new_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_reg_mgr_new_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_ims_service_enable_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_sms_new_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_presence_new_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_rtp_dynamic_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_emer_dynamic_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_video_dynamic_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_rcs_new_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_ims_new_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

/*===========================================================================
                           SET RESPONSE HANDLERS
===========================================================================*/

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_voice_dynamic_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_codec_dynamic_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_sip_new_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_reg_mgr_new_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_ims_service_enable_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_sms_new_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_presence_new_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_rtp_dynamic_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_emer_dynamic_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_video_dynamic_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_rcs_new_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_ims_new_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);
#endif /* QCRIL_QMI_RADIO_CONFIG_IMSS_V02_H */
