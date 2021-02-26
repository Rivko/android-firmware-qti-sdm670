/******************************************************************************
  @file    qcril_qmi_imss.h
  @brief   qcril qmi - IMS Setting

  DESCRIPTION
    Handles RIL requests, Callbacks, indications for QMI IMS Setting.

  ---------------------------------------------------------------------------

  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef QCRIL_QMI_IMSS_V02_H
#define QCRIL_QMI_IMSS_V02_H

#include "qmi.h"
#include "qmi_client.h"
#include "qcrili.h"
#include "qcril_reqlist.h"
#include "qcril_qmi_radio_config_meta.h"
#include "ip_multimedia_subsystem_settings_v01.h"


void qcril_qmi_imss_request_set_ims_registration_v02
(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
);
void qcril_qmi_imss_request_set_ims_srv_status_v02
(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
);
void qcril_qmi_imss_request_query_vt_call_quality_v02
(
 const qcril_request_params_type *const params_ptr,
 qcril_request_return_type *const ret_ptr /*!< Output parameter */
);
void qcril_qmi_imss_request_set_vt_call_quality_v02
(
 const qcril_request_params_type *const params_ptr,
 qcril_request_return_type *const ret_ptr /*!< Output parameter */
);
void qcril_qmi_imss_request_get_wifi_calling_status_v02
(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
);
void qcril_qmi_imss_request_set_wifi_calling_status_v02
(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
);
void qcril_qmi_imss_request_set_ims_config_v02
(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
);
void qcril_qmi_imss_request_get_ims_config_v02
(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
);
void qcril_qmi_imss_command_cb_helper_v02
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);
void qcril_qmi_imss_request_send_rtt_msg
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr /*!< Output parameter */
);
void qcril_qmi_imss_unsol_ind_cb_helper_v02
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

RIL_Errno qcril_qmi_imss_set_ims_service_enabled(uint8_t ims_service_enabled, uint16 req_id);

void qcril_qmi_imss_request_send_rtt_msg_resp_hdlr
(
  const qcril_request_params_type *const params_ptr
);

void qcril_qmi_imss_set_ims_service_enable_config_resp_hdlr
(
 const qcril_request_params_type *const params_ptr
);

void qcril_qmi_imss_get_ims_service_enable_config();

uint8_t qcril_qmi_imss_convert_ril_to_imss_wfc_status_v02
(
  qcril_ims_setting_wfc_status_type wfc_status
);

ims_settings_call_mode_option_enum_v01 qcril_qmi_imss_convert_ril_to_imss_wfc_preference_v02
(
  qcril_ims_setting_wfc_preference_type wfc_preference
);

qcril_ims_setting_wfc_status_type qcril_qmi_imss_convert_imss_to_ril_wfc_status_v02
(
  uint8_t wfc_status
);

qcril_ims_setting_wfc_preference_type qcril_qmi_imss_convert_imss_to_ril_wfc_preference_v02
(
  ims_settings_call_mode_option_enum_v01 wfc_preference
);

void qcril_qmi_imss_store_get_ims_service_enable_resp
(
  ims_settings_get_ims_service_enable_config_rsp_msg_v01* qmi_resp
);

ims_settings_omadm_services_type_mask_v01 qcril_qmi_imss_get_service_mask_by_network_enabled();

void qcril_qmi_imss_rtt_rcvd_data_ind_hdlr(void *ind_data_ptr);

#endif
