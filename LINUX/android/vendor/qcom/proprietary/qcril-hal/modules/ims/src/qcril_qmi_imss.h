/******************************************************************************
#  Copyright (c) 2013-2015, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    qcril_qmi_imss.h
  @brief   qcril qmi - IMS Setting

  DESCRIPTION
    Handles RIL requests, Callbacks, indications for QMI IMS Setting.

******************************************************************************/

#ifndef QCRIL_QMI_IMSS_H
#define QCRIL_QMI_IMSS_H

#include <modules/ims/imsIF.pb.h>
#include "qmi_client.h"
#include "qmi.h"
#include "qcril_reqlist.h"
#include "qcril_qmi_radio_config_meta.h"
#include "qcril_qmi_imss_internal.h"
#include "ip_multimedia_subsystem_settings_v01.h"
#include "network_access_service_v01.h"

#ifdef __cplusplus
#include <memory>
#include "modules/ims/ImsServiceStatusInd.h"
#include "modules/ims/ImsWfcSettingsStatusInd.h"
#endif

#define QMI_RIL_DISABLE_MODEM_CONFIG "persist.vendor.radio.disable_modem_cfg"
#define QMI_RIL_DISABLE_MODEM_CONFIG_LENGTH (4)

#define QMI_RIL_IMSS_VOICE_DOMAIN_PREF_ON_IMS_TOGGLE "persist.vendor.radio.vdp_on_ims_cap"

typedef struct
{
  qcril_evt_e_type event_id;
  qcril_req_handler_type *handler;
} qcril_qmi_imss_table_entry_type;

//===========================================================================
//                     IMSS Utility functions
//===========================================================================
void qcril_qmi_imss_pre_init(void);

void qcril_qmi_imss_init(void);

void qcril_qmi_imss_cleanup(void);

void qcril_qmi_imss_info_lock(void);

void qcril_qmi_imss_info_unlock(void);

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


#ifdef __cplusplus
void qcril_qmi_imss_command_cb
(
   unsigned int                 msg_id,
   std::shared_ptr<void>        resp_c_struct,
   unsigned int                 resp_c_struct_len,
   void                        *resp_cb_data,
   qmi_client_error_type        transp_err
);
void qcril_qmi_imss_command_cb_v02
(
   unsigned int                 msg_id,
   std::shared_ptr<void>        resp_c_struct,
   unsigned int                 resp_c_struct_len,
   void                        *resp_cb_data,
   qmi_client_error_type        transp_err
);
#endif

void qcril_qmi_imss_get_client_provisioning_config();

void qcril_qmi_imss_unsol_ind_cb_helper
(
 unsigned int   msg_id,
 unsigned char *decoded_payload,
 uint32_t       decoded_payload_len
);

void qcril_qmi_imss_client_provisioning_config_ind_hdlr(void *ind_data_ptr);

boolean qcril_qmi_imss_retrieve_wifi_call_preference_from_req_info
(
 qcril_reqlist_public_type       *req_info,
 qcril_ims_setting_wfc_preference_type *wfc_preference
);

RIL_Errno qcril_qmi_imss_send_set_qipcall_config_req
(
 ims_settings_set_qipcall_config_req_msg_v01 *qmi_req,
 uint32 user_data
);
RIL_Errno qcril_qmi_imss_set_ims_test_mode_enabled(uint8_t ims_test_mode_enabled, uint16 req_id);

void qcril_qmi_imss_set_voice_domain_preference_follow_up_hdlr(uint16 req_id, RIL_Errno result);

RIL_Errno qcril_qmi_imss_nas_set_voice_domain_preference
(
 nas_voice_domain_pref_enum_type_v01 voice_domain_pref,
 uint16 req_id
);

void qcril_qmi_imss_handle_ims_set_service_status_event_resp
(
 const qcril_request_params_type *const params_ptr,
 RIL_Errno ril_err
);

void qcril_qmi_imss_dispatch_config_response_helper
(
   const qcril_request_params_type *const params_ptr,
   int config_item,
   unsigned long msg_id
);

int qcril_qmi_imss_extract_config_item_from_reqlist_data(void *req_data, size_t req_datalen);
uint8_t qcril_qmi_imss_get_modem_version(void);

void qcril_qmi_imss_store_wifi_call_preference_to_req_info
(
 uint16                          req_id,
 ims_settings_wfc_preference_v01 wifi_call_preference
);

void qcril_qmi_imss_set_ims_config_log_and_send_response
(
   RIL_Token t,
   const qcril_qmi_radio_config_params_type *const config_params,
   qcril_qmi_radio_config_error_type error,
   qcril_qmi_radio_config_settings_resp_type settings_resp
);

void qcril_qmi_imss_get_ims_config_log_and_send_response
(
   RIL_Token t,
   const qcril_qmi_radio_config_params_type *const config_params,
   qcril_qmi_radio_config_error_type error,
   qcril_qmi_radio_config_settings_resp_type settings_resp
);

RIL_Errno qcril_qmi_imss_set_wifi_pref_to_qcril_data
(
 const qcril_request_params_type *params_ptr
);

void qcril_qmi_imss_broadcast_wfc_settings
(
 struct ims_cached_info_type ims_info
);

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
