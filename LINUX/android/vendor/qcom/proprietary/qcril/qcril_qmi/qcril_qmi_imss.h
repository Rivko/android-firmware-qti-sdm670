/******************************************************************************
  @file    qcril_qmi_imss.h
  @brief   qcril qmi - IMS Setting

  DESCRIPTION
    Handles RIL requests, Callbacks, indications for QMI IMS Setting.

  ---------------------------------------------------------------------------

  Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef QCRIL_QMI_IMSS_H
#define QCRIL_QMI_IMSS_H

#include "qmi_client.h"
#include "qcrili.h"
#include "qcril_reqlist.h"
#include "qcril_qmi_radio_config_meta.h"
#include "qcril_qmi_imss_internal.h"
#include "ip_multimedia_subsystem_settings_v01.h"

#define QMI_RIL_DISABLE_MODEM_CONFIG "persist.vendor.radio.disable_modem_cfg"
#define QMI_RIL_DISABLE_MODEM_CONFIG_LENGTH (4)

#define QMI_RIL_IMSS_VOICE_DOMAIN_PREF_ON_IMS_TOGGLE "persist.vendor.radio.vdp_on_ims_cap"

//===========================================================================
//                     IMSS Utility functions
//===========================================================================
boolean is_modem_ims_config_disable(void);

void qcril_qmi_imss_init(void);

void qcril_qmi_imss_cleanup(void);

void qcril_qmi_imss_info_lock(void);

void qcril_qmi_imss_info_unlock(void);

void qcril_qmi_imss_get_ims_reg_config(void);

void qcril_qmi_imss_request_set_ims_registration
(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
);

void qcril_qmi_imss_request_set_ims_srv_status(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
);

void qcril_qmi_imss_request_query_vt_call_quality
(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
);

void qcril_qmi_imss_request_set_vt_call_quality
(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
);

void qcril_qmi_imss_request_get_wifi_calling_status
(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
);

void qcril_qmi_imss_request_set_ims_config
(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
);

void qcril_qmi_imss_request_set_wifi_calling_status
(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
);

void qcril_qmi_imss_request_get_ims_config
(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
);

void qcril_qmi_imss_request_get_volte_preference
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

void qcril_qmi_imss_request_set_volte_preference
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

void qcril_qmi_imss_request_get_handover_config
(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
);

void qcril_qmi_imss_request_set_handover_config
(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
);

void qcril_qmi_imss_get_ims_config_log_and_send_response
(
   RIL_Token t,
   const qcril_qmi_radio_config_params_type *const config_params,
   qcril_qmi_radio_config_error_type error,
   qcril_qmi_radio_config_settings_resp_type settings_resp
);

void qcril_qmi_imss_set_ims_config_log_and_send_response
(
   RIL_Token t,
   const qcril_qmi_radio_config_params_type *const config_params,
   qcril_qmi_radio_config_error_type error,
   qcril_qmi_radio_config_settings_resp_type settings_resp
);

void qcril_qmi_imss_command_cb
(
   qmi_client_type              user_handle,
   unsigned int                 msg_id,
   void                        *resp_c_struct,
   unsigned int                 resp_c_struct_len,
   void                        *resp_cb_data,
   qmi_client_error_type        transp_err
);

void qcril_qmi_imss_command_cb_helper
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

void qcril_qmi_imss_get_client_provisioning_config();
uint8_t qcril_qmi_imss_client_provisioning_wlan_status();
void qcril_qmi_imss_clear_client_provisioning_config_info();

void qcril_qmi_imss_unsol_ind_cb
(
  qmi_client_type    user_handle,
  unsigned int       msg_id,
  void              *ind_buf,
  unsigned int       ind_buf_len,
  void              *ind_cb_data
);

void qcril_qmi_imss_unsol_ind_cb_helper
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
);

void qcril_qmi_imss_client_provisioning_config_ind_hdlr(void *ind_data_ptr);
uint8_t qcril_qmi_imss_compare_wfc_pref_unsol();
void qcril_qmi_imss_fetch_wifi_call_info(uint8_t *wifi_call_valid, qcril_ims_setting_wfc_status_type *wifi_call);
void qcril_qmi_imss_fetch_wifi_call_pref(uint8_t *wifi_call_preference_valid, qcril_ims_setting_wfc_preference_type *wifi_call_preference);

void qcril_qmi_imss_store_wifi_call_preference_to_req_info
(
 uint16                          req_id,
 ims_settings_wfc_preference_v01 wifi_call_preference
);
boolean qcril_qmi_imss_retrieve_wifi_call_preference_from_req_info
(
 qcril_reqlist_public_type       *req_info,
 qcril_ims_setting_wfc_preference_type *wfc_preference
);
RIL_Errno qcril_qmi_imss_set_wifi_pref_to_qcril_data
(
 const qcril_request_params_type *params_ptr
);

RIL_Errno qcril_qmi_imss_set_ims_test_mode_enabled(uint8_t ims_test_mode_enabled, uint16 req_id);
void qcril_qmi_imss_set_voice_domain_preference_follow_up(uint16 req_id);

void qcril_qmi_imss_dispatch_config_response_helper
(
   const qcril_request_params_type *const params_ptr,
   int config_item,
   unsigned long msg_id
);

int qcril_qmi_imss_extract_config_item_from_reqlist_data(void *req_data, size_t req_datalen);

ims_settings_wfc_status_enum_v01 qcril_qmi_imss_convert_ril_to_imss_wfc_status
(
  qcril_ims_setting_wfc_status_type wfc_status
);

ims_settings_wfc_preference_v01 qcril_qmi_imss_convert_ril_to_imss_wfc_preference
(
  qcril_ims_setting_wfc_preference_type wfc_preference
);

qcril_ims_setting_wfc_status_type qcril_qmi_imss_convert_imss_to_ril_wfc_status
(
  ims_settings_wfc_status_enum_v01 wfc_status
);

qcril_ims_setting_wfc_preference_type qcril_qmi_imss_convert_imss_to_ril_wfc_preference
(
  ims_settings_wfc_preference_v01 wfc_preference
);
#endif
