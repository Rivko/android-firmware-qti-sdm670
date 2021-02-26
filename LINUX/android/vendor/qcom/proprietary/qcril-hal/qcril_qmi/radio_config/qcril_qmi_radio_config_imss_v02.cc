/******************************************************************************
#  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    qcril_qmi_radio_config_imss_v02.c
  @brief   qcril qmi - radio config ims handlers

  DESCRIPTION
    Handles imss related radio config req and response handlers

******************************************************************************/

#include "qcril_log.h"
#include "qcril_qmi_client.h"
#include "qcril_qmi_radio_config_imss.h"
#include "qcril_qmi_radio_config_imss_v02.h"
#include "qcril_qmi_radio_config_meta.h"
#include "modules/ims/qcril_qmi_imss.h"
#include "modules/ims/qcril_qmi_imss_v02.h"
#include "modules/ims/qcril_qmi_imss_qmi.h"
#include "qcril_qmi_radio_config_misc.h"
#include "ip_multimedia_subsystem_settings_v01.h"
#include "qcril_reqlist.h"
#include "qcril_qmi_radio_config_meta_table_v02.h"
#include "modules/ims/qcril_qmi_imss_internal.h"

#if defined(FEATURE_TARGET_GLIBC_x86) || defined(QMI_RIL_UTF)
   extern "C" size_t strlcat(char *, const char *, size_t);
   extern "C" size_t strlcpy(char *, const char *, size_t);
#endif

ims_settings_call_mode_option_enum_v01 qcril_qmi_radio_config_map_qcril_wifi_mode_to_ims_wifi_mode
(
  qcril_qmi_radio_config_imss_voice_over_wifi_mode_type radio_config_wifi_mode
)
{
  ims_settings_call_mode_option_enum_v01 ims_wifi_mode;
  switch(radio_config_wifi_mode)
  {
    case QCRIL_QMI_RADIO_CONFIG_IMSS_VOICE_OVER_WIFI_MODE_WIFI_ONLY:
      ims_wifi_mode = IMS_SETTINGS_CALL_MODE_PREFERRED_WLAN_ONLY_V01;
      break;
    case QCRIL_QMI_RADIO_CONFIG_IMSS_VOICE_OVER_WIFI_MODE_CELLULAR_PREFERRED:
      ims_wifi_mode = IMS_SETTINGS_CALL_MODE_PREFERRED_CELLULAR_V01;
      break;
    case QCRIL_QMI_RADIO_CONFIG_IMSS_VOICE_OVER_WIFI_MODE_WIFI_PREFERRED:
      ims_wifi_mode = IMS_SETTINGS_CALL_MODE_PREFERRED_WIFI_V01;
      break;
    default:
      ims_wifi_mode = IMS_SETTINGS_CALL_MODE_PREFERRED_NONE_V01;
      break;
  }
  QCRIL_LOG_INFO("Mapped radio config wifi calling preference mode: %d, to ims wifi mode: %d",
                 radio_config_wifi_mode, ims_wifi_mode);
  return ims_wifi_mode;
}

qcril_qmi_radio_config_imss_voice_over_wifi_mode_type qcril_qmi_radio_config_map_ims_wifi_mode_to_qcril_wifi_mode
(
  ims_settings_call_mode_option_enum_v01 ims_wifi_mode
)
{
  qcril_qmi_radio_config_imss_voice_over_wifi_mode_type radio_config_wifi_mode;
  switch(ims_wifi_mode)
  {
    case IMS_SETTINGS_CALL_MODE_PREFERRED_WLAN_ONLY_V01:
      radio_config_wifi_mode = QCRIL_QMI_RADIO_CONFIG_IMSS_VOICE_OVER_WIFI_MODE_WIFI_ONLY;
      break;
    case IMS_SETTINGS_CALL_MODE_PREFERRED_CELLULAR_V01:
      radio_config_wifi_mode = QCRIL_QMI_RADIO_CONFIG_IMSS_VOICE_OVER_WIFI_MODE_CELLULAR_PREFERRED;
      break;
    case IMS_SETTINGS_CALL_MODE_PREFERRED_WIFI_V01:
      radio_config_wifi_mode = QCRIL_QMI_RADIO_CONFIG_IMSS_VOICE_OVER_WIFI_MODE_WIFI_PREFERRED;
      break;
    default:
      radio_config_wifi_mode = QCRIL_QMI_RADIO_CONFIG_IMSS_VOICE_OVER_WIFI_MODE_NONE;
      break;
  }
  QCRIL_LOG_INFO("Mapped ims wifi calling preference mode: %d, to radio config wifi mode: %d",
                 ims_wifi_mode, radio_config_wifi_mode);
  return radio_config_wifi_mode;
}

//===========================================================================
// GET REQUEST HANDLERS
//===========================================================================

//===========================================================================
// qcril_qmi_radio_config_imss_get_codec_dynamic_config_req_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_codec_dynamic_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
  qmi_client_error_type qmi_client_error = QMI_NO_ERR;

  do
  {
    /* Validate the config pointer */
    if((config_ptr == NULL) ||
       (config_ptr->extra_data_len == 0) ||
       (config_ptr->extra_data == NULL))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }

    qmi_client_error = qmi_client_imss_send_async(
        QMI_IMS_SETTINGS_GET_CODEC_DYNAMIC_CONFIG_REQ_V01,
        NULL,
        0,
        sizeof(ims_settings_get_codec_dynamic_config_rsp_msg_v01),
        qcril_qmi_imss_command_cb_v02,
        config_ptr->extra_data);

    QCRIL_LOG_INFO(".. qmi send async res %d", (int)qmi_client_error);
    radio_config_error = qcril_qmi_radio_config_map_internal_error_to_radio_config_error(qmi_client_error);
  }while(FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(radio_config_error);
  return radio_config_error;
} // qcril_qmi_radio_config_imss_get_codec_dynamic_config_req_handler

//===========================================================================
// qcril_qmi_radio_config_imss_get_voice_dynamic_config_req_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_voice_dynamic_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
  qmi_client_error_type qmi_client_error = QMI_NO_ERR;

  do
  {
    /* Validate the config pointer */
    if((config_ptr == NULL) ||
       (config_ptr->extra_data_len == 0) ||
       (config_ptr->extra_data == NULL))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }

    qmi_client_error = qmi_client_imss_send_async(
        QMI_IMS_SETTINGS_GET_VOICE_DYNAMIC_CONFIG_REQ_V01,
        NULL,
        0,
        sizeof(ims_settings_get_voice_dynamic_config_rsp_msg_v01),
        qcril_qmi_imss_command_cb_v02,
        config_ptr->extra_data);

    QCRIL_LOG_INFO(".. qmi send async res %d", (int)qmi_client_error);
    radio_config_error = qcril_qmi_radio_config_map_internal_error_to_radio_config_error(qmi_client_error);
  }while(FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(radio_config_error);
  return radio_config_error;
} // qcril_qmi_radio_config_imss_get_voice_dynamic_config_req_handler

//===========================================================================
// qcril_qmi_radio_config_imss_get_sip_new_config_req_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_sip_new_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
  qmi_client_error_type qmi_client_error = QMI_NO_ERR;

  do
  {
    /* Validate the config pointer */
    if((config_ptr == NULL) ||
       (config_ptr->extra_data_len == 0) ||
       (config_ptr->extra_data == NULL))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }

    qmi_client_error = qmi_client_imss_send_async(
        QMI_IMS_SETTINGS_GET_SIP_NEW_CONFIG_REQ_V01,
        NULL,
        0,
        sizeof(ims_settings_get_sip_new_config_rsp_msg_v01),
        qcril_qmi_imss_command_cb_v02,
        config_ptr->extra_data);

    QCRIL_LOG_INFO(".. qmi send async res %d", (int)qmi_client_error);
    radio_config_error = qcril_qmi_radio_config_map_internal_error_to_radio_config_error(qmi_client_error);
  }while(FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(radio_config_error);
  return radio_config_error;
} // qcril_qmi_radio_config_imss_get_sip_new_config_req_handler

//===========================================================================
// qcril_qmi_radio_config_imss_get_reg_mgr_new_config_req_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_reg_mgr_new_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
  qmi_client_error_type qmi_client_error = QMI_NO_ERR;

  do
  {
    /* Validate the config pointer */
    if((config_ptr == NULL) ||
       (config_ptr->extra_data_len == 0) ||
       (config_ptr->extra_data == NULL))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }

    qmi_client_error = qmi_client_imss_send_async(
        QMI_IMS_SETTINGS_GET_REG_MGR_NEW_CONFIG_REQ_V01,
        NULL,
        0,
        sizeof(ims_settings_get_reg_mgr_new_config_rsp_msg_v01),
        qcril_qmi_imss_command_cb_v02,
        config_ptr->extra_data);

    QCRIL_LOG_INFO(".. qmi send async res %d", (int)qmi_client_error);
    radio_config_error = qcril_qmi_radio_config_map_internal_error_to_radio_config_error(qmi_client_error);
  }while(FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(radio_config_error);
  return radio_config_error;
} // qcril_qmi_radio_config_imss_get_reg_mgr_new_config_req_handler

//===========================================================================
// qcril_qmi_radio_config_imss_get_ims_service_enable_config_req_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_ims_service_enable_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
  qmi_client_error_type qmi_client_error = QMI_NO_ERR;

  do
  {
    /* Validate the config pointer */
    if((config_ptr == NULL) ||
       (config_ptr->extra_data_len == 0) ||
       (config_ptr->extra_data == NULL))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }

    qmi_client_error = qmi_client_imss_send_async(
        QMI_IMS_SETTINGS_GET_IMS_SERVICE_ENABLE_CONFIG_REQ_V01,
        NULL,
        0,
        sizeof(ims_settings_get_ims_service_enable_config_rsp_msg_v01),
        qcril_qmi_imss_command_cb_v02,
        config_ptr->extra_data);

    QCRIL_LOG_INFO(".. qmi send async res %d", (int)qmi_client_error);
    radio_config_error = qcril_qmi_radio_config_map_internal_error_to_radio_config_error(qmi_client_error);
  }while(FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(radio_config_error);
  return radio_config_error;
} // qcril_qmi_radio_config_imss_get_ims_service_enable_config_req_handler

//===========================================================================
// qcril_qmi_radio_config_imss_get_sms_new_config_req_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_sms_new_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
  qmi_client_error_type qmi_client_error = QMI_NO_ERR;

  do
  {
    /* Validate the config pointer */
    if((config_ptr == NULL) ||
       (config_ptr->extra_data_len == 0) ||
       (config_ptr->extra_data == NULL))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }

    qmi_client_error = qmi_client_imss_send_async(
        QMI_IMS_SETTINGS_GET_SMS_NEW_CONFIG_REQ_V01,
        NULL,
        0,
        sizeof(ims_settings_get_sms_new_config_rsp_msg_v01),
        qcril_qmi_imss_command_cb_v02,
        config_ptr->extra_data);

    QCRIL_LOG_INFO(".. qmi send async res %d", (int)qmi_client_error);
    radio_config_error = qcril_qmi_radio_config_map_internal_error_to_radio_config_error(qmi_client_error);
  }while(FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(radio_config_error);
  return radio_config_error;
} // qcril_qmi_radio_config_imss_get_sms_new_config_req_handler

//===========================================================================
// qcril_qmi_radio_config_imss_get_presence_new_config_req_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_presence_new_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
  qmi_client_error_type qmi_client_error = QMI_NO_ERR;

  do
  {
    /* Validate the config pointer */
    if((config_ptr == NULL) ||
       (config_ptr->extra_data_len == 0) ||
       (config_ptr->extra_data == NULL))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }

    qmi_client_error = qmi_client_imss_send_async(
        QMI_IMS_SETTINGS_GET_PRESENCE_NEW_CONFIG_REQ_V01,
        NULL,
        0,
        sizeof(ims_settings_get_presence_new_config_rsp_msg_v01),
        qcril_qmi_imss_command_cb_v02,
        config_ptr->extra_data);

    QCRIL_LOG_INFO(".. qmi send async res %d", (int)qmi_client_error);
    radio_config_error = qcril_qmi_radio_config_map_internal_error_to_radio_config_error(qmi_client_error);
  }while(FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(radio_config_error);
  return radio_config_error;
} // qcril_qmi_radio_config_imss_get_presence_new_config_req_handler

//===========================================================================
// qcril_qmi_radio_config_imss_get_rtp_dynamic_config_req_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_rtp_dynamic_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
  qmi_client_error_type qmi_client_error = QMI_NO_ERR;

  do
  {
    /* Validate the config pointer */
    if((config_ptr == NULL) ||
       (config_ptr->extra_data_len == 0) ||
       (config_ptr->extra_data == NULL))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }

    qmi_client_error = qmi_client_imss_send_async(
        QMI_IMS_SETTINGS_GET_RTP_DYNAMIC_CONFIG_REQ_V01,
        NULL,
        0,
        sizeof(ims_settings_get_rtp_dynamic_config_rsp_msg_v01),
        qcril_qmi_imss_command_cb_v02,
        config_ptr->extra_data);

    QCRIL_LOG_INFO(".. qmi send async res %d", (int)qmi_client_error);
    radio_config_error = qcril_qmi_radio_config_map_internal_error_to_radio_config_error(qmi_client_error);
  }while(FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(radio_config_error);
  return radio_config_error;
} // qcril_qmi_radio_config_imss_get_rtp_dynamic_config_req_handler

//===========================================================================
// qcril_qmi_radio_config_imss_get_emer_dynamic_config_req_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_emer_dynamic_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
  qmi_client_error_type qmi_client_error = QMI_NO_ERR;

  do
  {
    /* Validate the config pointer */
    if((config_ptr == NULL) ||
       (config_ptr->extra_data_len == 0) ||
       (config_ptr->extra_data == NULL))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }

    qmi_client_error = qmi_client_imss_send_async(
        QMI_IMS_SETTINGS_GET_EMER_DYNAMIC_CONFIG_REQ_V01,
        NULL,
        0,
        sizeof(ims_settings_get_emer_dynamic_config_rsp_msg_v01),
        qcril_qmi_imss_command_cb_v02,
        config_ptr->extra_data);

    QCRIL_LOG_INFO(".. qmi send async res %d", (int)qmi_client_error);
    radio_config_error = qcril_qmi_radio_config_map_internal_error_to_radio_config_error(qmi_client_error);
  }while(FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(radio_config_error);
  return radio_config_error;
} // qcril_qmi_radio_config_imss_get_emer_dynamic_config_req_handler

//===========================================================================
// qcril_qmi_radio_config_imss_get_video_dynamic_config_req_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_video_dynamic_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  qcril_qmi_radio_config_error_type radio_config_error =
    QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
  qmi_client_error_type qmi_client_error = QMI_NO_ERR;

  QCRIL_LOG_FUNC_ENTRY();
  do
  {
    /* Validate the config pointer */
    if((config_ptr == NULL) ||
        (config_ptr->extra_data_len == 0) ||
        (config_ptr->extra_data == NULL))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }

    qmi_client_error = qmi_client_imss_send_async(
        QMI_IMS_SETTINGS_GET_VIDEO_DYNAMIC_CONFIG_REQ_V01,
        NULL,
        0,
        sizeof(ims_settings_get_video_dynamic_config_rsp_msg_v01),
        qcril_qmi_imss_command_cb_v02,
        config_ptr->extra_data);

    QCRIL_LOG_INFO(".. qmi send async res %d", (int)qmi_client_error);
    radio_config_error = qcril_qmi_radio_config_map_internal_error_to_radio_config_error(
        qmi_client_error);
  }while(FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(radio_config_error);
  return radio_config_error;
} // qcril_qmi_radio_config_imss_get_video_dynamic_config_req_handler

//===========================================================================
// qcril_qmi_radio_config_imss_get_rcs_new_config_req_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_rcs_new_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_qmi_radio_config_error_type radio_config_error =
          QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
  qmi_client_error_type qmi_client_error = E_FAILURE;

  do
  {
    /* Validate the config pointer */
    if((config_ptr == NULL) ||
       (config_ptr->extra_data_len == 0) ||
       (config_ptr->extra_data == NULL))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }

    qmi_client_error = qmi_client_imss_send_async(QMI_IMS_SETTINGS_GET_RCS_SM_NEW_CONFIG_REQ_V01,
            NULL,
            0,
            sizeof(ims_settings_get_rcs_new_config_rsp_msg_v01),
            qcril_qmi_imss_command_cb_v02,
            config_ptr->extra_data);

    QCRIL_LOG_INFO(".. qmi send async res %d", (int)qmi_client_error);
    radio_config_error = qcril_qmi_radio_config_map_internal_error_to_radio_config_error(
           qmi_client_error);
  }while(FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(radio_config_error);
  return radio_config_error;
} // qcril_qmi_radio_config_imss_get_rcs_new_config_req_handler

//===========================================================================
// qcril_qmi_radio_config_imss_get_ims_new_config_req_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_ims_new_config_req_handler
(
 const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_qmi_radio_config_error_type radio_config_error =
            QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
  qmi_client_error_type qmi_client_error = E_FAILURE;

  do
  {
    /* Validate the config pointer */
    if((config_ptr == NULL) ||
        (config_ptr->extra_data_len == 0) ||
        (config_ptr->extra_data == NULL))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }

    qmi_client_error = qmi_client_imss_send_async(QMI_IMS_SETTINGS_GET_IMS_NEW_CONFIG_REQ_V01,
          NULL,
          0,
          sizeof(ims_settings_get_ims_new_config_rsp_msg_v01),
          qcril_qmi_imss_command_cb_v02,
          config_ptr->extra_data);

    QCRIL_LOG_INFO(".. qmi send async res %d", (int)qmi_client_error);
    radio_config_error = qcril_qmi_radio_config_map_internal_error_to_radio_config_error(
                                 qmi_client_error);
  } while (FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(radio_config_error);
  return radio_config_error;
} // qcril_qmi_radio_config_imss_get_ims_new_config_req_handler

//===========================================================================
// SET REQUEST HANDLERS
//===========================================================================

//===========================================================================
// qcril_qmi_radio_config_imss_set_codec_dynamic_config_req_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_codec_dynamic_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  ims_settings_set_codec_dynamic_config_req_msg_v01 qmi_req;
  qcril_qmi_radio_config_item_value_type item_value_type;
  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
  qmi_client_error_type qmi_client_error = QMI_NO_ERR;

  //placeholder for validating integer values
  uint32_t int_value = 0;

  do
  {
    /* Validate the config pointer */
    if( (config_ptr == NULL) ||
        (config_ptr->extra_data_len == 0) ||
        (config_ptr->extra_data == NULL) ||
        (config_ptr->config_item_value_len == 0) ||
        (config_ptr->config_item_value == NULL) )
    {
      QCRIL_LOG_ERROR("Invalid config params");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }

    memset(&qmi_req, 0, sizeof(qmi_req));

    /* Validate if the item_value_type sent in the config params matches with the
       one in the table. The table should have the right value type for the item */
    item_value_type = qcril_qmi_radio_config_get_item_value_type(config_ptr->config_item);
    if(config_ptr->config_item_value_type != item_value_type)
    {
      QCRIL_LOG_ERROR("Invalid item type..doesnt match with the table");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_ITEM_VALUE_TYPE;
      break;
    }

    QCRIL_LOG_INFO("Config item to set: %s, type: %d",
                   qcril_qmi_radio_config_get_item_log_str(config_ptr->config_item),
                   item_value_type);

    //start with success
    radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;

    /* Depending on the Config item,
       validate the type processed and put it in the request*/
    switch(config_ptr->config_item)
    {
      case QCRIL_QMI_RADIO_CONFIG_VOIP_AMR_MODE:
        qmi_req.amr_mode_set_mask_valid = TRUE;
        qmi_req.amr_mode_set_mask = qcril_qmi_radio_config_get_amr_or_amr_wb_mode_mask(
            (const char *)config_ptr->config_item_value,
            QCRIL_QMI_RADIO_CONFIG_VOIP_AMR_MODE_MAX_INDEX-1);
        QCRIL_LOG_INFO("Set config amr_mode_set_mask to: 0x%x", qmi_req.amr_mode_set_mask);
        break;

      case QCRIL_QMI_RADIO_CONFIG_VOIP_AMR_WB_MODE:
        qmi_req.amr_wb_mode_set_mask_valid = TRUE;
        qmi_req.amr_wb_mode_set_mask = qcril_qmi_radio_config_get_amr_or_amr_wb_mode_mask(
            (const char *)config_ptr->config_item_value,
            QCRIL_QMI_RADIO_CONFIG_VOIP_AMR_WB_MODE_MAX_INDEX-1);
        QCRIL_LOG_INFO("Set config amr_wb_mode_set_mask to: %d", qmi_req.amr_wb_mode_set_mask);
        break;

      case QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_WB_OCTET_ALIGNED_DYNAMIC_PT:
        int_value = *((uint32_t *)config_ptr->config_item_value);
        if(int_value <= UINT16_MAX)
        {
          qmi_req.amr_wb_octet_aligned_dynamic_pt_valid = TRUE;
          qmi_req.amr_wb_octet_aligned_dynamic_pt = int_value;
          QCRIL_LOG_INFO("Set config QIPCALL amr_wb_octet_aligned_dynamic_pt to: %d",
                          qmi_req.amr_wb_octet_aligned_dynamic_pt);
        }
        else
        {
          QCRIL_LOG_INFO("Value is greater than the limit!");
          radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_PARAMETER;
        }
        break;

      case QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_WB_BANDWIDTH_EFFICIENT_DYNAMIC_PT:
        int_value = *((uint32_t *)config_ptr->config_item_value);
        if(int_value <= UINT16_MAX)
        {
          qmi_req.amr_wb_bandwidth_efficient_dynamic_pt_valid = TRUE;
          qmi_req.amr_wb_bandwidth_efficient_dynamic_pt = int_value;
          QCRIL_LOG_INFO("Set config QIPCALL amr_wb_bandwidth_efficient_dynamic_pt to: %d",
                          qmi_req.amr_wb_bandwidth_efficient_dynamic_pt);
        }
        else
        {
          QCRIL_LOG_INFO("Value is greater than the limit!");
          radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_PARAMETER;
        }
        break;

      case QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_OCTET_ALIGNED_DYNAMIC_PT:
        int_value = *((uint32_t *)config_ptr->config_item_value);
        if(int_value <= UINT16_MAX)
        {
          qmi_req.amr_octet_aligned_dynamic_pt_valid = TRUE;
          qmi_req.amr_octet_aligned_dynamic_pt = int_value;
          QCRIL_LOG_INFO("Set config QIPCALL amr_octet_aligned_dynamic_pt to: %d",
                          qmi_req.amr_octet_aligned_dynamic_pt);
        }
        else
        {
          QCRIL_LOG_INFO("Value is greater than the limit!");
          radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_PARAMETER;
        }
        break;

      case QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_BANDWIDTH_EFFICIENT_DYNAMIC_PT:
        int_value = *((uint32_t *)config_ptr->config_item_value);
        if(int_value <= UINT16_MAX)
        {
          qmi_req.amr_bandwidth_efficient_dynamic_pt_valid = TRUE;
          qmi_req.amr_bandwidth_efficient_dynamic_pt = int_value;
          QCRIL_LOG_INFO("Set config QIPCALL amr_bandwidth_efficient_dynamic_pt to: %d",
                          qmi_req.amr_bandwidth_efficient_dynamic_pt);
        }
        else
        {
          QCRIL_LOG_INFO("Value is greater than the limit!");
          radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_PARAMETER;
        }
        break;

      case QCRIL_QMI_RADIO_CONFIG_QIPCALL_DTMF_WB_DYNAMIC_POINT:
        int_value = *((uint32_t *)config_ptr->config_item_value);
        if(int_value <= UINT16_MAX)
        {
          qmi_req.dtmf_wb_dynamic_pt_valid = TRUE;
          qmi_req.dtmf_wb_dynamic_pt = int_value;
          QCRIL_LOG_INFO("Set config QIPCALL dtmf_wb_dynamic_pt to: %d",
                          qmi_req.dtmf_wb_dynamic_pt);
        }
        else
        {
          QCRIL_LOG_INFO("Value is greater than the limit!");
          radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_PARAMETER;
        }
        break;

      case QCRIL_QMI_RADIO_CONFIG_QIPCALL_DTMF_NB_DYNAMIC_PT:
        int_value = *((uint32_t *)config_ptr->config_item_value);
        if(int_value <= UINT16_MAX)
        {
          qmi_req.dtmf_nb_dynamic_pt_valid = TRUE;
          qmi_req.dtmf_nb_dynamic_pt = int_value;
          QCRIL_LOG_INFO("Set config QIPCALL dtmf_nb_dynamic_pt_valid to: %d",
                          qmi_req.dtmf_nb_dynamic_pt_valid);
        }
        else
        {
          QCRIL_LOG_INFO("Value is greater than the limit!");
          radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_PARAMETER;
        }
        break;

      case QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_DEFAULT_MODE:
        int_value = *((uint32_t *)config_ptr->config_item_value);
        if(int_value <= UINT16_MAX)
        {
          qmi_req.amr_wb_default_mode_valid = TRUE;
          qmi_req.amr_wb_default_mode = (ims_settings_amr_wb_codec_mode_option_enum_v01)int_value;
          QCRIL_LOG_INFO("Set config amr_wb_mode_set to: %d", qmi_req.amr_wb_default_mode);
        }
        else
        {
          QCRIL_LOG_INFO("Value is greater than the limit!");
          radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_PARAMETER;
        }
        break;

      default:
        radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
        QCRIL_LOG_ERROR("Invalid item..");
        break;
    }
    /* If the qmi req structure generation is SUCCESS then send
       QMI async request */
    if(radio_config_error == QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS)
    {
      qmi_client_error = qmi_client_imss_send_async(
          QMI_IMS_SETTINGS_SET_CODEC_DYNAMIC_CONFIG_REQ_V01,
          &qmi_req,
          sizeof(ims_settings_set_codec_dynamic_config_req_msg_v01),
          sizeof(ims_settings_set_codec_dynamic_config_rsp_msg_v01),
          qcril_qmi_imss_command_cb_v02,
          config_ptr->extra_data);

      QCRIL_LOG_INFO(".. qmi send async res %d", (int)qmi_client_error);
      radio_config_error = qcril_qmi_radio_config_map_internal_error_to_radio_config_error(qmi_client_error);
    }

  }while(FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(radio_config_error);
  return radio_config_error;
}// qcril_qmi_radio_config_imss_set_codec_dynamic_config_req_handler

//===========================================================================
// qcril_qmi_radio_config_imss_set_voice_dynamic_config_req_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_voice_dynamic_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  ims_settings_set_voice_dynamic_config_req_msg_v01 qmi_req;
  qcril_qmi_radio_config_item_value_type item_value_type;
  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
  qmi_client_error_type qmi_client_error = QMI_NO_ERR;

  //placeholder for validating integer values
  uint32_t int_value = 0;

  do
  {
    /* Validate the config pointer */
    if( (config_ptr == NULL) ||
        (config_ptr->extra_data_len == 0) ||
        (config_ptr->extra_data == NULL) ||
        (config_ptr->config_item_value_len == 0) ||
        (config_ptr->config_item_value == NULL) )
    {
      QCRIL_LOG_ERROR("Invalid config params");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }

    memset(&qmi_req, 0, sizeof(qmi_req));

    /* Validate if the item_value_type sent in the config params matches with the
       one in the table. The table should have the right value type for the item */
    item_value_type = qcril_qmi_radio_config_get_item_value_type(config_ptr->config_item);
    if(config_ptr->config_item_value_type != item_value_type)
    {
      QCRIL_LOG_ERROR("Invalid item type..doesnt match with the table");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_ITEM_VALUE_TYPE;
      break;
    }

    QCRIL_LOG_INFO("Config item to set: %s, type: %d",
                   qcril_qmi_radio_config_get_item_log_str(config_ptr->config_item),
                   item_value_type);

    //start with success
    radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;

    /* Depending on the Config item,
       validate the type processed and put it in the request*/
    switch(config_ptr->config_item)
    {
      case QCRIL_QMI_RADIO_CONFIG_VOIP_SESSION_EXPIRY_TIMER:
        int_value = *((uint32_t *)config_ptr->config_item_value);
        if(int_value <= UINT16_MAX)
        {
          qmi_req.session_expires_valid = TRUE;
          qmi_req.session_expires = int_value;
          QCRIL_LOG_INFO("Set config VOIP session_expiry_timer to: %d", qmi_req.session_expires);
        }
        else
        {
          QCRIL_LOG_INFO("Value is greater than the limit!");
          radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_PARAMETER;
        }
        break;

      case QCRIL_QMI_RADIO_CONFIG_VOIP_MIN_SESSION_EXPIRY:
        int_value = *((uint32_t *)config_ptr->config_item_value);
        if(int_value <= UINT16_MAX)
        {
          qmi_req.min_session_expiry_valid = TRUE;
          qmi_req.min_session_expiry = int_value;
          QCRIL_LOG_INFO("Set config VOIP min_session_expiry to: %d", qmi_req.min_session_expiry);
        }
        else
        {
          QCRIL_LOG_INFO("Value is greater than the limit!");
          radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_PARAMETER;
        }
        break;

      case QCRIL_QMI_RADIO_CONFIG_VOIP_SILENT_REDIAL_ENABLED:
        qmi_req.silent_redial_enable_valid = TRUE;
        qmi_req.silent_redial_enable = *((bool *)config_ptr->config_item_value);
        QCRIL_LOG_INFO("Set config VOIP voip_silent_redial_enabled to: %d", qmi_req.silent_redial_enable);
        break;

      case QCRIL_QMI_RADIO_CONFIG_QIPCALL_VICE_ENABLED:
        qmi_req.vice_enabled_valid = true;
        qmi_req.vice_enabled = *((bool *)config_ptr->config_item_value);
        QCRIL_LOG_INFO("Set config QIPCALL qipcall_vice_enabled to: %d",
                      qmi_req.vice_enabled);
        break;

      default:
        radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
        QCRIL_LOG_ERROR("Invalid item..");
        break;
    }
    /* If the qmi req structure generation is SUCCESS then send
       QMI async request */
    if(radio_config_error == QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS)
    {
      qmi_client_error = qmi_client_imss_send_async(
          QMI_IMS_SETTINGS_SET_VOICE_DYNAMIC_CONFIG_REQ_V01,
          &qmi_req,
          sizeof(ims_settings_set_voice_dynamic_config_req_msg_v01),
          sizeof(ims_settings_set_voice_dynamic_config_rsp_msg_v01),
          qcril_qmi_imss_command_cb_v02,
          config_ptr->extra_data);

      QCRIL_LOG_INFO(".. qmi send async res %d", (int)qmi_client_error);
      radio_config_error = qcril_qmi_radio_config_map_internal_error_to_radio_config_error(qmi_client_error);
    }

  }while(FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(radio_config_error);
  return radio_config_error;
}// qcril_qmi_radio_config_imss_set_voice_dynamic_config_req_handler

//===========================================================================
// qcril_qmi_radio_config_imss_set_sip_config_req_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_sip_new_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  ims_settings_set_sip_new_config_req_msg_v01 qmi_req;
  qcril_qmi_radio_config_item_value_type item_value_type;
  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
  qmi_client_error_type qmi_client_error = QMI_NO_ERR;

  //placeholder for validating integer values
  uint32_t int_value = 0;

  do
  {
    /* Validate the config pointer */
    if( (config_ptr == NULL) ||
        (config_ptr->extra_data_len == 0) ||
        (config_ptr->extra_data == NULL) ||
        (config_ptr->config_item_value_len == 0) ||
        (config_ptr->config_item_value == NULL) )
    {
      QCRIL_LOG_ERROR("Invalid config params");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }

    memset(&qmi_req, 0, sizeof(qmi_req));

    /* Validate if the item_value_type sent in the config params matches with the
       one in the table. The table should have the right value type for the item */
    item_value_type = qcril_qmi_radio_config_get_item_value_type(config_ptr->config_item);
    if(config_ptr->config_item_value_type != item_value_type)
    {
      QCRIL_LOG_ERROR("Invalid item type..doesnt match with the table");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_ITEM_VALUE_TYPE;
      break;
    }

    QCRIL_LOG_INFO("Config item to set: %s, type: %d",
                   qcril_qmi_radio_config_get_item_log_str(config_ptr->config_item),
                   item_value_type);

    //start with success
    radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;

    /* Depending on the Config item,
       validate the type processed and put it in the request*/
    switch(config_ptr->config_item)
    {
      case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_T1:
        qmi_req.timer_t1_valid = TRUE;
        qmi_req.timer_t1 = *((uint32_t *)config_ptr->config_item_value);
        QCRIL_LOG_INFO("Set config SIP timer_t1 to: %d", qmi_req.timer_t1);
        break;

      case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_T2:
        qmi_req.timer_t2_valid = TRUE;
        qmi_req.timer_t2 = *((uint32_t *)config_ptr->config_item_value);
        QCRIL_LOG_INFO("Set config SIP timer_t2 to: %d", qmi_req.timer_t2);
        break;

      case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TF:
        qmi_req.timer_tf_valid = TRUE;
        qmi_req.timer_tf = *((uint32_t *)config_ptr->config_item_value);
        QCRIL_LOG_INFO("Set config SIP timer_tf to: %d", qmi_req.timer_tf);
        break;

      case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_T4:
        qmi_req.timer_t4_valid = TRUE;
        qmi_req.timer_t4 = *((uint32_t *)config_ptr->config_item_value);
        QCRIL_LOG_INFO("Set config SIP timer_t4 to: %d", qmi_req.timer_t4);
        break;

      case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TA_VALUE:
        qmi_req.timer_ta_valid = TRUE;
        qmi_req.timer_ta = *((uint32_t *)config_ptr->config_item_value);
        QCRIL_LOG_INFO("Set config SIP timer_ta to: %d", qmi_req.timer_ta);
        break;

      case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TB_VALUE:
        qmi_req.timer_tb_valid = TRUE;
        qmi_req.timer_tb = *((uint32_t *)config_ptr->config_item_value);
        QCRIL_LOG_INFO("Set config SIP timer_tb to: %d", qmi_req.timer_tb);
        break;

      case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TD_VALUE:
        qmi_req.timer_td_valid = TRUE;
        qmi_req.timer_td = *((uint32_t *)config_ptr->config_item_value);
        QCRIL_LOG_INFO("Set config SIP timer_td to: %d", qmi_req.timer_td);
        break;

      case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TE_VALUE:
        qmi_req.timer_te_valid = TRUE;
        qmi_req.timer_te = *((uint32_t *)config_ptr->config_item_value);
        QCRIL_LOG_INFO("Set config SIP timer_te to: %d", qmi_req.timer_te);
        break;

      case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TG_VALUE:
        qmi_req.timer_tg_valid = TRUE;
        qmi_req.timer_tg = *((uint32_t *)config_ptr->config_item_value);
        QCRIL_LOG_INFO("Set config SIP timer_tg to: %d", qmi_req.timer_tg);
        break;

      case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TH_VALUE:
        qmi_req.timer_th_valid = TRUE;
        qmi_req.timer_th = *((uint32_t *)config_ptr->config_item_value);
        QCRIL_LOG_INFO("Set config SIP timer_th to: %d", qmi_req.timer_th);
        break;

      case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TI_VALUE:
        qmi_req.timer_ti_valid = TRUE;
        qmi_req.timer_ti = *((uint32_t *)config_ptr->config_item_value);
        QCRIL_LOG_INFO("Set config SIP timer_ti to: %d", qmi_req.timer_ti);
        break;

      case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TJ:
        int_value = *((uint32_t *)config_ptr->config_item_value);
        if(int_value <= UINT16_MAX)
        {
          qmi_req.timer_tj_valid = TRUE;
          qmi_req.timer_tj = int_value;
          QCRIL_LOG_INFO("Set config SIP timer_tj to: %d", qmi_req.timer_tj);
        }
        else
        {
          QCRIL_LOG_INFO("Value is greater than the limit!");
          radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_PARAMETER;
        }
        break;

      case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TK_VALUE:
        qmi_req.timer_tk_valid = TRUE;
        qmi_req.timer_tk = *((uint32_t *)config_ptr->config_item_value);
        QCRIL_LOG_INFO("Set config SIP timer_tk to: %d", qmi_req.timer_tk);
        break;

      case QCRIL_QMI_RADIO_CONFIG_SIP_KEEPALIVE_ENABLED:
        qmi_req.keepalive_enabled_valid = TRUE;
        qmi_req.keepalive_enabled = *((bool *)config_ptr->config_item_value);
        QCRIL_LOG_INFO("Set config SIP keepalive_enabled to: %d", qmi_req.keepalive_enabled);
        break;

      default:
        radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
        QCRIL_LOG_ERROR("Invalid item..")
        break;
    }
    /* If the qmi req structure generation is SUCCESS then send
       QMI async request */
    if(radio_config_error == QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS)
    {
      qmi_client_error = qmi_client_imss_send_async(
          QMI_IMS_SETTINGS_SET_SIP_NEW_CONFIG_REQ_V01,
          &qmi_req,
          sizeof(ims_settings_set_sip_new_config_req_msg_v01),
          sizeof(ims_settings_set_sip_new_config_rsp_msg_v01),
          qcril_qmi_imss_command_cb_v02,
          config_ptr->extra_data);

      QCRIL_LOG_INFO(".. qmi send async res %d", (int)qmi_client_error);
      radio_config_error = qcril_qmi_radio_config_map_internal_error_to_radio_config_error(qmi_client_error);
    }

  }while(FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(radio_config_error);
  return radio_config_error;
}// qcril_qmi_radio_config_imss_set_sip_new_config_req_handler

//===========================================================================
// qcril_qmi_radio_config_imss_set_reg_mgr_new_config_req_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_reg_mgr_new_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  ims_settings_set_reg_mgr_new_config_req_msg_v01 qmi_req;
  qcril_qmi_radio_config_item_value_type item_value_type;
  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
  qmi_client_error_type qmi_client_error = QMI_NO_ERR;

  //placeholder for validating integer values
  uint32_t int_value = 0;

  do
  {
    /* Validate the config pointer */
    if( (config_ptr == NULL) ||
        (config_ptr->extra_data_len == 0) ||
        (config_ptr->extra_data == NULL) ||
        (config_ptr->config_item_value_len == 0) ||
        (config_ptr->config_item_value == NULL) )
    {
      QCRIL_LOG_ERROR("Invalid config params");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }

    memset(&qmi_req, 0, sizeof(qmi_req));

    /* Validate if the item_value_type sent in the config params matches with the
       one in the table. The table should have the right value type for the item */
    item_value_type = qcril_qmi_radio_config_get_item_value_type(config_ptr->config_item);
    if(config_ptr->config_item_value_type != item_value_type)
    {
      QCRIL_LOG_ERROR("Invalid item type..doesnt match with the table");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_ITEM_VALUE_TYPE;
      break;
    }

    QCRIL_LOG_INFO("Config item to set: %s, type: %d",
                   qcril_qmi_radio_config_get_item_log_str(config_ptr->config_item),
                   item_value_type);

    //start with success
    radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;

    /* Depending on the Config item,
       validate the type processed and put it in the request*/
    switch(config_ptr->config_item)
    {
      case QCRIL_QMI_RADIO_CONFIG_REG_MGR_EXTENDED_T_DELAY:
        int_value = *((uint32_t *)config_ptr->config_item_value);
        if(int_value <= UINT16_MAX)
        {
          qmi_req.reg_irat_hystersis_timer_valid = TRUE;
          qmi_req.reg_irat_hystersis_timer = int_value;
          QCRIL_LOG_INFO("Set config REG MGR EXTENDED t_delay to: %d",
                                                   qmi_req.reg_irat_hystersis_timer);
        }
        else
        {
          QCRIL_LOG_INFO("Value is greater than the limit!");
          radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_PARAMETER;
        }
        break;

      case QCRIL_QMI_RADIO_CONFIG_REG_MGR_EXTENDED_REG_RETRY_BASE_TIME:
        int_value = *((uint32_t *)config_ptr->config_item_value);
        if(int_value <= UINT16_MAX)
        {
          qmi_req.reg_retry_base_time_valid = TRUE;
          qmi_req.reg_retry_base_time = int_value;
          QCRIL_LOG_INFO("Set config REG MGR EXTENDED reg_retry_base_time_valid to: %d",
                                                   qmi_req.reg_retry_base_time);
        }
        else
        {
          QCRIL_LOG_INFO("Value is greater than the limit!");
          radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_PARAMETER;
        }
        break;

      case QCRIL_QMI_RADIO_CONFIG_REG_MGR_EXTENDED_REG_RETRY_MAX_TIME:
        int_value = *((uint32_t *)config_ptr->config_item_value);
        if(int_value <= UINT16_MAX)
        {
          qmi_req.reg_retry_max_time_valid = TRUE;
          qmi_req.reg_retry_max_time = int_value;
          QCRIL_LOG_INFO("Set config REG MGR EXTENDED reg_retry_max_time to: %d",
                                                   qmi_req.reg_retry_max_time);
        }
        else
        {
          QCRIL_LOG_INFO("Value is greater than the limit!");
          radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_PARAMETER;
        }
        break;

      case QCRIL_QMI_RADIO_CONFIG_REG_MGR_CONFIG_REG_MGR_PRIMARY_CSCF:
        qmi_req.regmgr_primary_pcscf_valid = TRUE;
        strlcpy(qmi_req.regmgr_primary_pcscf,
                (const char *)config_ptr->config_item_value,
                IMS_SETTINGS_STRING_LEN_MAX_V01);
        QCRIL_LOG_INFO("Set config REG_MGR regmgr_primary_pcscf to: %s",
                        qmi_req.regmgr_primary_pcscf);
        break;

      default:
        radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
        QCRIL_LOG_ERROR("Invalid item..")
        break;
    }
    /* If the qmi req structure generation is SUCCESS then send
       QMI async request */
    if(radio_config_error == QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS)
    {
      qmi_client_error = qmi_client_imss_send_async(
          QMI_IMS_SETTINGS_SET_REG_MGR_NEW_CONFIG_REQ_V01,
          &qmi_req,
          sizeof(ims_settings_set_reg_mgr_new_config_req_msg_v01),
          sizeof(ims_settings_set_reg_mgr_new_config_rsp_msg_v01),
          qcril_qmi_imss_command_cb_v02,
          config_ptr->extra_data);

      QCRIL_LOG_INFO(".. qmi send async res %d", (int)qmi_client_error);
      radio_config_error = qcril_qmi_radio_config_map_internal_error_to_radio_config_error(qmi_client_error);
    }

  }while(FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(radio_config_error);
  return radio_config_error;
}// qcril_qmi_radio_config_imss_set_reg_mgr_extended_config_req_handler

//===========================================================================
// qcril_qmi_radio_config_imss_set_ims_service_enable_config_req_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_ims_service_enable_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  ims_settings_set_ims_service_enable_config_req_msg_v01 qmi_req;
  qcril_qmi_radio_config_item_value_type item_value_type;
  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
  qmi_client_error_type qmi_client_error = QMI_NO_ERR;
  ims_settings_omadm_services_type_mask_v01 service_mask =
    IMS_SETTINGS_OMADM_SERVICES_BIT_MASK_NONE_V01;

  //placeholder to validate enum values
  uint32_t enum_int_val;

  do
  {
    /* Validate the config pointer */
    if( (config_ptr == NULL) ||
        (config_ptr->extra_data_len == 0) ||
        (config_ptr->extra_data == NULL) ||
        (config_ptr->config_item_value_len == 0) ||
        (config_ptr->config_item_value == NULL) )
    {
      QCRIL_LOG_ERROR("Invalid config params");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }

    memset(&qmi_req, 0, sizeof(qmi_req));

    /* Validate if the item_value_type sent in the config params matches with the
       one in the table. The table should have the right value type for the item */
    item_value_type = qcril_qmi_radio_config_get_item_value_type(config_ptr->config_item);
    if(config_ptr->config_item_value_type != item_value_type)
    {
      QCRIL_LOG_ERROR("Invalid item type..doesnt match with the table");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_ITEM_VALUE_TYPE;
      break;
    }

    QCRIL_LOG_INFO("Config item to set: %s, type: %d",
                   qcril_qmi_radio_config_get_item_log_str(config_ptr->config_item),
                   item_value_type);

    //start with success
    radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;

    /* Depending on the Config item,
       validate the type processed and put it in the request*/

    service_mask = qcril_qmi_imss_get_service_mask_by_network_enabled();

    //TODO: Check if IMS_SETTINGS_OMADM_SERVICES_ENABLE_BIT_MASK_V01 bit needs to be
    //disabled if all services are disabled
    switch(config_ptr->config_item)
    {
      case QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_VOLTE:
        qmi_req.service_mask_by_network_enabled_valid = TRUE;
        service_mask = (*((bool *)config_ptr->config_item_value))?
            (service_mask | (IMS_SETTINGS_OMADM_SERVICES_VOLTE_ENABLE_BIT_MASK_V01 |
             IMS_SETTINGS_OMADM_SERVICES_ENABLE_BIT_MASK_V01)) :
            (service_mask & ~(IMS_SETTINGS_OMADM_SERVICES_VOLTE_ENABLE_BIT_MASK_V01));
        qmi_req.service_mask_by_network_enabled = service_mask;
        QCRIL_LOG_INFO("Set config CLIENT PROVISIONING enable_volte to: %d",
                        qmi_req.service_mask_by_network_enabled);
        break;

      case QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_VT:
        qmi_req.service_mask_by_network_enabled_valid = TRUE;
        service_mask = (*((bool *)config_ptr->config_item_value))?
            (service_mask | (IMS_SETTINGS_OMADM_SERVICES_VT_ENABLE_BIT_MASK_V01 |
             IMS_SETTINGS_OMADM_SERVICES_ENABLE_BIT_MASK_V01)) :
            (service_mask & ~(IMS_SETTINGS_OMADM_SERVICES_VT_ENABLE_BIT_MASK_V01));
        qmi_req.service_mask_by_network_enabled = service_mask;
        QCRIL_LOG_INFO("Set config CLIENT PROVISIONING enable_vt to: %d",
                        qmi_req.service_mask_by_network_enabled);
        break;

      case QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_VOWIFI:
        qmi_req.service_mask_by_network_enabled_valid = TRUE;
        service_mask = (*((bool *)config_ptr->config_item_value))?
            (service_mask | (IMS_SETTINGS_OMADM_SERVICES_VOWIFI_ENABLE_BIT_MASK_V01 |
             IMS_SETTINGS_OMADM_SERVICES_ENABLE_BIT_MASK_V01)) :
            (service_mask & ~(IMS_SETTINGS_OMADM_SERVICES_VOWIFI_ENABLE_BIT_MASK_V01));
        qmi_req.service_mask_by_network_enabled = service_mask;
        QCRIL_LOG_INFO("Set config CLIENT PROVISIONING enable_vowifi to: %d",
                        qmi_req.service_mask_by_network_enabled);
        break;

      case QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_PRESENCE:
        qmi_req.service_mask_by_network_enabled_valid = TRUE;
        service_mask = (*((bool *)config_ptr->config_item_value))?
            (service_mask | (IMS_SETTINGS_OMADM_SERVICES_PRESENCE_ENABLE_BIT_MASK_V01 |
             IMS_SETTINGS_OMADM_SERVICES_ENABLE_BIT_MASK_V01)) :
            (service_mask & ~(IMS_SETTINGS_OMADM_SERVICES_PRESENCE_ENABLE_BIT_MASK_V01));
        qmi_req.service_mask_by_network_enabled = service_mask;
        QCRIL_LOG_INFO("Set config CLIENT PROVISIONING enable_presence to: %d",
            qmi_req.service_mask_by_network_enabled);
        break;

      case QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_WIFI_CALL_ROAMING:
        qmi_req.wifi_calling_in_roaming_enabled_valid = TRUE;
        qmi_req.wifi_calling_in_roaming_enabled = *((bool *)config_ptr->config_item_value);
        QCRIL_LOG_INFO("Set config CLIENT PROVISIONING enable_vowifi to: %d",
                        qmi_req.wifi_calling_in_roaming_enabled);
        break;

      case QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_RTT:
        qmi_req.rtt_enabled_valid = TRUE;
        qmi_req.rtt_enabled = *((bool *)config_ptr->config_item_value);
        QCRIL_LOG_INFO("Set config RTT Configuration to : %d",
                        qmi_req.rtt_enabled);
        break;

      case QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_WIFI_CALL_PREFERENCE:
        enum_int_val = *((uint32_t *)config_ptr->config_item_value);
        enum_int_val =
            qcril_qmi_radio_config_map_qcril_wifi_mode_to_ims_wifi_mode(
                (qcril_qmi_radio_config_imss_voice_over_wifi_mode_type)enum_int_val);

        //The Conversion function returns NONE if the value
        //is other than wifi_only/wifi_pref/cellular_pref
        //for which error needs to be sent
        if(enum_int_val != IMS_SETTINGS_CALL_MODE_PREFERRED_NONE_V01)
        {
          qmi_req.call_mode_preference_valid = TRUE;
          qmi_req.call_mode_preference = (ims_settings_call_mode_option_enum_v01)enum_int_val;
          QCRIL_LOG_INFO("Set config CLIENT PROVISIONING wifi_call_preference to: %d",
                          qmi_req.call_mode_preference);

          // Store the wifi preference in the reqlist_entry
          // IMSS REFACTOR: TODO Integrate with Radio Config
          qcril_qmi_imss_store_wifi_call_mode_preference_to_req_info(
                  QCRIL_EXTRACT_USER_ID_FROM_USER_DATA((uint32)(uintptr_t)config_ptr->extra_data),
                  qmi_req.call_mode_preference); // wifi_call_preference
        }
        else
        {
          QCRIL_LOG_INFO("Invalid Wifi preference setting..");
          radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
        }
        break;

      case QCRIL_QMI_RADIO_CONFIG_SMS_OVER_IP:
        qmi_req.sms_enabled_valid = TRUE;
        qmi_req.sms_enabled = *((bool *)config_ptr->config_item_value);
        QCRIL_LOG_INFO("Set config SMS sms_over_ip_network_indication to: %d",
                        qmi_req.sms_enabled);
        break;

      case QCRIL_QMI_RADIO_CONFIG_QIPCALL_MOBILE_DATA_ENABLED:
        qmi_req.mobile_data_enabled_valid = TRUE;
        qmi_req.mobile_data_enabled = *((bool *)config_ptr->config_item_value);
        QCRIL_LOG_INFO("Set config QIPCALL mobile_data_enabled to: %d",
                        qmi_req.mobile_data_enabled);
        break;

      case QCRIL_QMI_RADIO_CONFIG_PRESENCE_VOLTE_USER_OPTED_IN_STATUS:
        qmi_req.presence_enabled_valid = TRUE;
        qmi_req.presence_enabled = *((bool *)config_ptr->config_item_value);
        QCRIL_LOG_INFO("Set config PRESENCE presence_enabled to: %d",
                        qmi_req.presence_enabled);
        break;

      default:
        radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
        QCRIL_LOG_ERROR("Invalid item..");
        break;
    }
    /* If the qmi req structure generation is SUCCESS then send
       QMI async request */
    if(radio_config_error == QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS)
    {
      qmi_client_error = qmi_client_imss_send_async(
        QMI_IMS_SETTINGS_SET_IMS_SERVICE_ENABLE_CONFIG_REQ_V01,
        &qmi_req,
        sizeof(ims_settings_set_ims_service_enable_config_req_msg_v01),
        sizeof(ims_settings_set_ims_service_enable_config_rsp_msg_v01),
        qcril_qmi_imss_command_cb_v02,
        config_ptr->extra_data);

      QCRIL_LOG_INFO(".. qmi send async res %d", (int)qmi_client_error);
      radio_config_error =
        qcril_qmi_radio_config_map_internal_error_to_radio_config_error(qmi_client_error);
    }

  }while(FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(radio_config_error);
  return radio_config_error;
}// qcril_qmi_radio_config_imss_set_ims_service_enable_config_req_handler

//===========================================================================
// qcril_qmi_radio_config_imss_set_sms_config_req_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_sms_new_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  ims_settings_set_sms_new_config_req_msg_v01 qmi_req;
  qcril_qmi_radio_config_item_value_type item_value_type;
  qcril_qmi_radio_config_error_type radio_config_error =
    QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
  qmi_client_error_type qmi_client_error = QMI_NO_ERR;

  //placeholder to validate enum values
  uint32_t enum_int_val;

  do
  {
    /* Validate the config pointer */
    if( (config_ptr == NULL) ||
        (config_ptr->extra_data_len == 0) ||
        (config_ptr->extra_data == NULL) ||
        (config_ptr->config_item_value_len == 0) ||
        (config_ptr->config_item_value == NULL) )
    {
      QCRIL_LOG_ERROR("Invalid config params");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }

    memset(&qmi_req, 0, sizeof(qmi_req));

    /* Validate if the item_value_type sent in the config params matches with the
       one in the table. The table should have the right value type for the item */
    item_value_type = qcril_qmi_radio_config_get_item_value_type(config_ptr->config_item);
    if(config_ptr->config_item_value_type != item_value_type)
    {
      QCRIL_LOG_ERROR("Invalid item type..doesnt match with the table");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_ITEM_VALUE_TYPE;
      break;
    }

    QCRIL_LOG_INFO("Config item to set: %s, type: %d",
                   qcril_qmi_radio_config_get_item_log_str(config_ptr->config_item),
                   item_value_type);

    //start with success
    radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;

    /* Depending on the Config item,
       validate the type processed and put it in the request*/
    switch(config_ptr->config_item)
    {
      case QCRIL_QMI_RADIO_CONFIG_SMS_FORMAT:
        //TODO: misc functions for conversion. and internal enums
        enum_int_val = *((uint32_t *)config_ptr->config_item_value);
        if((enum_int_val == IMS_SETTINGS_SMS_FORMAT_3GPP2_V01) ||
           (enum_int_val == IMS_SETTINGS_SMS_FORMAT_3GPP_V01))
        {
          qmi_req.sms_format_valid = TRUE;
          qmi_req.sms_format = (ims_settings_sms_format_enum_v01)enum_int_val;
          QCRIL_LOG_INFO("Set config SMS sms_format to: %d", qmi_req.sms_format);
        }
        else
        {
          QCRIL_LOG_ERROR("Invalid Sms Format setting..");
          radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
        }
        break;

      case QCRIL_QMI_RADIO_CONFIG_SMS_PSI:
        qmi_req.sms_psi_valid = TRUE;
        strlcpy(qmi_req.sms_psi,
                (const char *)config_ptr->config_item_value,
                IMS_SETTINGS_IMS_SMS_PSI_LEN_V01);
        QCRIL_LOG_INFO("Set config SMS sms_psi to: %s", qmi_req.sms_psi);
        break;

      default:
        radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
        QCRIL_LOG_ERROR("Invalid item..")
        break;
    }
    /* If the qmi req structure generation is SUCCESS then send
       QMI async request */
    if(radio_config_error == QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS)
    {
      qmi_client_error = qmi_client_imss_send_async(
          QMI_IMS_SETTINGS_SET_SMS_NEW_CONFIG_REQ_V01,
          &qmi_req,
          sizeof(ims_settings_set_sms_new_config_req_msg_v01),
          sizeof(ims_settings_set_sms_new_config_rsp_msg_v01),
          qcril_qmi_imss_command_cb_v02,
          config_ptr->extra_data);

      QCRIL_LOG_INFO(".. qmi send async res %d", (int)qmi_client_error);
      radio_config_error = qcril_qmi_radio_config_map_internal_error_to_radio_config_error(qmi_client_error);
    }

  }while(FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(radio_config_error);
  return radio_config_error;
}// qcril_qmi_radio_config_imss_set_sms_new_config_req_handler

//===========================================================================
// qcril_qmi_radio_config_imss_set_presence_new_config_req_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_presence_new_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  ims_settings_set_presence_new_config_req_msg_v01 qmi_req;
  qcril_qmi_radio_config_item_value_type item_value_type;
  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
  qmi_client_error_type qmi_client_error = QMI_NO_ERR;

  do
  {
    /* Validate the config pointer */
    if( (config_ptr == NULL) ||
        (config_ptr->extra_data_len == 0) ||
        (config_ptr->extra_data == NULL) ||
        (config_ptr->config_item_value_len == 0) ||
        (config_ptr->config_item_value == NULL) )
    {
      QCRIL_LOG_ERROR("Invalid config params");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }

    memset(&qmi_req, 0, sizeof(qmi_req));

    /* Validate if the item_value_type sent in the config params matches with the
       one in the table. The table should have the right value type for the item */
    item_value_type = qcril_qmi_radio_config_get_item_value_type(config_ptr->config_item);
    if(config_ptr->config_item_value_type != item_value_type)
    {
      QCRIL_LOG_ERROR("Invalid item type..doesnt match with the table");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_ITEM_VALUE_TYPE;
      break;
    }

    QCRIL_LOG_INFO("Config item to set: %s, type: %d",
                   qcril_qmi_radio_config_get_item_log_str(config_ptr->config_item),
                   item_value_type);

    //start with success
    radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;

    /* Depending on the Config item,
       validate the type processed and put it in the request*/
    switch(config_ptr->config_item)
    {
      case QCRIL_QMI_RADIO_CONFIG_PRESENCE_PUBLISH_EXPIRY_TIMER:
        qmi_req.publish_timer_valid = TRUE;
        qmi_req.publish_timer = *((uint32_t *)config_ptr->config_item_value);
        QCRIL_LOG_INFO("Set config PRESENCE publish_expiry_timer to: %d",
                        qmi_req.publish_timer);
        break;

      case QCRIL_QMI_RADIO_CONFIG_PRESENCE_PUBLISH_EXTENDED_EXPIRY_TIMER:
        qmi_req.publish_extended_timer_valid = TRUE;
        qmi_req.publish_extended_timer = *((uint32_t *)config_ptr->config_item_value);
        QCRIL_LOG_INFO("Set config PRESENCE publish_extended_expiry_timer to: %d",
                        qmi_req.publish_extended_timer);
        break;

      case QCRIL_QMI_RADIO_CONFIG_PRESENCE_CAPABILITIES_CACHE_EXPIRATION:
        qmi_req.capabilites_cache_expiry_valid = TRUE;
        qmi_req.capabilites_cache_expiry = *((uint32_t *)config_ptr->config_item_value);
        QCRIL_LOG_INFO("Set config PRESENCE capabilites_cache_expiration to: %d",
                        qmi_req.capabilites_cache_expiry);
        break;

      case QCRIL_QMI_RADIO_CONFIG_PRESENCE_AVAILABILITY_CACHE_EXPIRATION:
        qmi_req.availability_cache_expiry_valid = TRUE;
        qmi_req.availability_cache_expiry = *((uint32_t *)config_ptr->config_item_value);
        QCRIL_LOG_INFO("Set config PRESENCE availability_cache_expiration to: %d",
                        qmi_req.availability_cache_expiry);
        break;

      case QCRIL_QMI_RADIO_CONFIG_PRESENCE_CAPABILITY_POLL_INTERVAL:
        qmi_req.capability_poll_interval_valid = TRUE;
        qmi_req.capability_poll_interval = *((uint32_t *)config_ptr->config_item_value);
        QCRIL_LOG_INFO("Set config PRESENCE capability_poll_interval to: %d",
                        qmi_req.capability_poll_interval);
        break;

      case QCRIL_QMI_RADIO_CONFIG_PRESENCE_MINIMUM_PUBLISH_INTERVAL:
        qmi_req.publish_src_throttle_timer_valid = TRUE;
        qmi_req.publish_src_throttle_timer = *((uint32_t *)config_ptr->config_item_value);
        QCRIL_LOG_INFO("Set config PRESENCE minimum_publish_interval to: %d",
                        qmi_req.publish_src_throttle_timer);
        break;

      case QCRIL_QMI_RADIO_CONFIG_PRESENCE_MAXIMUM_SUBSCRIPTION_LIST_ENTRIES:
        qmi_req.max_enties_in_list_subscribe_valid = TRUE;
        qmi_req.max_enties_in_list_subscribe = *((uint32_t *)config_ptr->config_item_value);
        QCRIL_LOG_INFO("Set config PRESENCE max_subcription_list_entries to: %d",
                        qmi_req.max_enties_in_list_subscribe);
        break;

      case QCRIL_QMI_RADIO_CONFIG_PRESENCE_CAPABILITY_POLL_LIST_SUBSCRIPTION_EXPIRY_TIMER:
        qmi_req.list_subscription_expiry_valid = TRUE;
        qmi_req.list_subscription_expiry = *((uint32_t *)config_ptr->config_item_value);
        QCRIL_LOG_INFO("Set config PRESENCE capability_poll_list_subscription_expiry_timer to: %d",
                        qmi_req.list_subscription_expiry);
        break;

      case QCRIL_QMI_RADIO_CONFIG_PRESENCE_GZIP_ENABLED:
        qmi_req.gzip_enabled_valid = TRUE;
        qmi_req.gzip_enabled = *((bool *)config_ptr->config_item_value);
        QCRIL_LOG_INFO("Set config PRESENCE gzip_enabled to: %d",
                        qmi_req.gzip_enabled);
        break;

      case QCRIL_QMI_RADIO_CONFIG_PRESENCE_CAPABILITY_DISCOVERY_ENABLE:
        qmi_req.capability_discovery_enabled_valid = TRUE;
        qmi_req.capability_discovery_enabled = *((bool *)config_ptr->config_item_value);
        QCRIL_LOG_INFO("Set config PRESENCE capability_discovery_enable to: %d",
                        qmi_req.capability_discovery_enabled);
        break;

      default:
        radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
        QCRIL_LOG_ERROR("Invalid item..")
          break;
    }
    /* If the qmi req structure generation is SUCCESS then send
       QMI async request */
    if(radio_config_error == QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS)
    {
      qmi_client_error = qmi_client_imss_send_async(
          QMI_IMS_SETTINGS_SET_PRESENCE_NEW_CONFIG_REQ_V01,
          &qmi_req,
          sizeof(ims_settings_set_presence_new_config_req_msg_v01),
          sizeof(ims_settings_set_presence_new_config_rsp_msg_v01),
          qcril_qmi_imss_command_cb_v02,
          config_ptr->extra_data);

      QCRIL_LOG_INFO(".. qmi send async res %d", (int)qmi_client_error);
      radio_config_error = qcril_qmi_radio_config_map_internal_error_to_radio_config_error(qmi_client_error);
    }

  }while(FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(radio_config_error);
  return radio_config_error;
}// qcril_qmi_radio_config_imss_set_presence_new_config_req_handler

//===========================================================================
// qcril_qmi_radio_config_imss_set_rtp_dynamic_config_req_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_rtp_dynamic_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  ims_settings_set_rtp_dynamic_config_req_msg_v01 qmi_req;
  qcril_qmi_radio_config_item_value_type item_value_type;
  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
  qmi_client_error_type qmi_client_error = QMI_NO_ERR;
  //placeholder for validating integer values
  uint32_t int_value = 0;

  do
  {
    /* Validate the config pointer */
    if( (config_ptr == NULL) ||
        (config_ptr->extra_data_len == 0) ||
        (config_ptr->extra_data == NULL) ||
        (config_ptr->config_item_value_len == 0) ||
        (config_ptr->config_item_value == NULL) )
    {
      QCRIL_LOG_ERROR("Invalid config params");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }

    memset(&qmi_req, 0, sizeof(qmi_req));

    /* Validate if the item_value_type sent in the config params matches with the
       one in the table. The table should have the right value type for the item */
    item_value_type = qcril_qmi_radio_config_get_item_value_type(config_ptr->config_item);
    if(config_ptr->config_item_value_type != item_value_type)
    {
      QCRIL_LOG_ERROR("Invalid item type..doesnt match with the table");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_ITEM_VALUE_TYPE;
      break;
    }

    QCRIL_LOG_INFO("Config item to set: %s, type: %d",
                   qcril_qmi_radio_config_get_item_log_str(config_ptr->config_item),
                   item_value_type);

    //start with success
    radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;

    /* Depending on the Config item,
       validate the type processed and put it in the request*/
    switch(config_ptr->config_item)
    {
      case QCRIL_QMI_RADIO_CONFIG_QIPCALL_SPEECH_START_PORT:
        int_value = *((uint32_t *)config_ptr->config_item_value);
        if(int_value <= UINT16_MAX)
        {
          qmi_req.speech_start_port_valid = TRUE;
          qmi_req.speech_start_port = int_value;
          QCRIL_LOG_INFO("Set config QIPCALL speech_start_port to: %d",
                          qmi_req.speech_start_port);
        }
        else
        {
          QCRIL_LOG_INFO("Value is greater than the limit!");
          radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_PARAMETER;
        }
        break;

      case QCRIL_QMI_RADIO_CONFIG_QIPCALL_SPEECH_END_PORT:
        int_value = *((uint32_t *)config_ptr->config_item_value);
        if(int_value <= UINT16_MAX)
        {
          qmi_req.speech_stop_port_valid = TRUE;
          qmi_req.speech_stop_port = int_value;
          QCRIL_LOG_INFO("Set config QIPCALL speech_stop_port to: %d",
                          qmi_req.speech_stop_port);
        }
        else
        {
          QCRIL_LOG_INFO("Value is greater than the limit!");
          radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_PARAMETER;
        }
        break;

      default:
        radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
        QCRIL_LOG_ERROR("Invalid item..")
          break;
    }
    /* If the qmi req structure generation is SUCCESS then send
       QMI async request */
    if(radio_config_error == QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS)
    {
      qmi_client_error = qmi_client_imss_send_async(
          QMI_IMS_SETTINGS_SET_RTP_DYNAMIC_CONFIG_REQ_V01,
          &qmi_req,
          sizeof(ims_settings_set_rtp_dynamic_config_req_msg_v01),
          sizeof(ims_settings_set_rtp_dynamic_config_rsp_msg_v01),
          qcril_qmi_imss_command_cb_v02,
          config_ptr->extra_data);

      QCRIL_LOG_INFO(".. qmi send async res %d", (int)qmi_client_error);
      radio_config_error = qcril_qmi_radio_config_map_internal_error_to_radio_config_error(qmi_client_error);
    }

  }while(FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(radio_config_error);
  return radio_config_error;
}// qcril_qmi_radio_config_imss_set_rtp_dynamic_config_req_handler

//===========================================================================
// qcril_qmi_radio_config_imss_set_emer_dynamic_config_req_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_emer_dynamic_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  ims_settings_set_emer_dynamic_config_req_msg_v01 qmi_req;
  qcril_qmi_radio_config_item_value_type item_value_type;
  qcril_qmi_radio_config_error_type radio_config_error =
        QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
  qmi_client_error_type qmi_client_error = QMI_NO_ERR;

  do
  {
    /* Validate the config pointer */
    if( (config_ptr == NULL) ||
        (config_ptr->extra_data_len == 0) ||
        (config_ptr->extra_data == NULL) ||
        (config_ptr->config_item_value_len == 0) ||
        (config_ptr->config_item_value == NULL) )
    {
      QCRIL_LOG_ERROR("Invalid config params");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }

    memset(&qmi_req, 0, sizeof(qmi_req));

    /* Validate if the item_value_type sent in the config params matches with the
       one in the table. The table should have the right value type for the item */
    item_value_type = qcril_qmi_radio_config_get_item_value_type(config_ptr->config_item);
    if(config_ptr->config_item_value_type != item_value_type)
    {
      QCRIL_LOG_ERROR("Invalid item type..doesnt match with the table");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_ITEM_VALUE_TYPE;
      break;
    }

    QCRIL_LOG_INFO("Config item to set: %s, type: %d",
                   qcril_qmi_radio_config_get_item_log_str(config_ptr->config_item),
                   item_value_type);

    //start with success
    radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;

    if(config_ptr->config_item == QCRIL_QMI_RADIO_CONFIG_QIPCALL_EMERG_CALL_TIMER)
    {
      qmi_req.e911_call_setup_timer_valid = true;
      qmi_req.e911_call_setup_timer = *((uint32_t*)config_ptr->config_item_value);
      QCRIL_LOG_INFO("Set config QIPCALL e911_call_setup_timer to: %d",
                      qmi_req.e911_call_setup_timer);
    }
    else
    {
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
    }

    /* If the qmi req structure generation is SUCCESS then send
       QMI async request */
    if(radio_config_error == QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS)
    {
      qmi_client_error = qmi_client_imss_send_async(
          QMI_IMS_SETTINGS_SET_EMER_DYNAMIC_CONFIG_REQ_V01,
          &qmi_req,
          sizeof(ims_settings_set_emer_dynamic_config_req_msg_v01),
          sizeof(ims_settings_set_emer_dynamic_config_rsp_msg_v01),
          qcril_qmi_imss_command_cb_v02,
          config_ptr->extra_data);

      QCRIL_LOG_INFO(".. qmi send async res %d", (int)qmi_client_error);
      radio_config_error =
         qcril_qmi_radio_config_map_internal_error_to_radio_config_error(qmi_client_error);
    }

  }while(FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(radio_config_error);
  return radio_config_error;
}

//===========================================================================
// qcril_qmi_radio_config_imss_set_video_dynamic_config_req_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_video_dynamic_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  ims_settings_set_video_dynamic_config_req_msg_v01 qmi_req;
  qcril_qmi_radio_config_item_value_type item_value_type;
  qcril_qmi_radio_config_error_type radio_config_error =
    QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
  qmi_client_error_type qmi_client_error = QMI_NO_ERR;

  QCRIL_LOG_FUNC_ENTRY();
  do
  {
    /* Validate the config pointer */
    if( (config_ptr == NULL) ||
        (config_ptr->extra_data_len == 0) ||
        (config_ptr->extra_data == NULL) ||
        (config_ptr->config_item_value_len == 0) ||
        (config_ptr->config_item_value == NULL) )
    {
      QCRIL_LOG_ERROR("Invalid config params");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }

    memset(&qmi_req, 0, sizeof(qmi_req));

    /* Validate if the item_value_type sent in the config params matches with the
       one in the table. The table should have the right value type for the item */
    item_value_type = qcril_qmi_radio_config_get_item_value_type(config_ptr->config_item);
    if(config_ptr->config_item_value_type != item_value_type)
    {
      QCRIL_LOG_ERROR("Invalid item type..doesnt match with the table");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_ITEM_VALUE_TYPE;
      break;
    }

    QCRIL_LOG_INFO("Config item to set: %s, type: %d",
        qcril_qmi_radio_config_get_item_log_str(config_ptr->config_item),
        item_value_type);

    //start with success
    radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;

    /* Depending on the Config item,
       validate the type processed and put it in the request*/
    switch(config_ptr->config_item)
    {
      case QCRIL_QMI_RADIO_CONFIG_QIPCALL_VIDEO_QUALITY:
        if (qcril_qmi_radio_config_imss_map_radio_config_video_quality_to_imss_vt_quality(
              *((qcril_qmi_radio_config_imss_video_quality *)config_ptr->config_item_value), &qmi_req.vt_quality_selector))
        {
          qmi_req.vt_quality_selector_valid = TRUE;
          QCRIL_LOG_INFO("Set config vt_quality_selector to: %d",
              qmi_req.vt_quality_selector);
        }
        else
        {
          QCRIL_LOG_INFO("Invalid parameter received for video quality");
          radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_PARAMETER;
        }
        break;

      default:
        radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
        QCRIL_LOG_ERROR("Invalid item..")
          break;
    }
    /* If the qmi req structure generation is SUCCESS then send
       QMI async request */
    if(radio_config_error == QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS)
    {
      qmi_client_error = qmi_client_imss_send_async(
          QMI_IMS_SETTINGS_SET_VIDEO_DYNAMIC_CONFIG_REQ_V01,
          &qmi_req,
          sizeof(ims_settings_set_video_dynamic_config_req_msg_v01),
          sizeof(ims_settings_set_video_dynamic_config_rsp_msg_v01),
          qcril_qmi_imss_command_cb_v02,
          config_ptr->extra_data);

      QCRIL_LOG_INFO(".. qmi send async res %d", (int)qmi_client_error);
      radio_config_error = qcril_qmi_radio_config_map_internal_error_to_radio_config_error(
          qmi_client_error);
    }
  }while(FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(radio_config_error);
  return radio_config_error;
}// qcril_qmi_radio_config_imss_set_video_dynamic_config_req_handler

//===========================================================================
// qcril_qmi_radio_config_imss_set_rcs_new_req_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_rcs_new_config_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  ims_settings_set_rcs_new_config_req_msg_v01 qmi_req;
  qcril_qmi_radio_config_item_value_type item_value_type;
  qcril_qmi_radio_config_error_type radio_config_error =
          QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
  qmi_client_error_type qmi_client_error = E_FAILURE;
  uint32_t int_val;

  do
  {
    /* Validate the config pointer */
    if( (config_ptr == NULL) ||
        (config_ptr->extra_data_len == 0) ||
        (config_ptr->extra_data == NULL) ||
        (config_ptr->config_item_value_len == 0) ||
        (config_ptr->config_item_value == NULL) )
    {
      QCRIL_LOG_ERROR("Invalid config params");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }

    memset(&qmi_req, 0, sizeof(qmi_req));

    /* Validate if the item_value_type sent in the config params matches with the
       one in the table. The table should have the right value type for the item */
    item_value_type = qcril_qmi_radio_config_get_item_value_type(config_ptr->config_item);
    if(config_ptr->config_item_value_type != item_value_type)
    {
      QCRIL_LOG_ERROR("Invalid item type..doesnt match with the table");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_ITEM_VALUE_TYPE;
      break;
    }

    QCRIL_LOG_INFO("Config item to set: %s, type: %d",
                   qcril_qmi_radio_config_get_item_log_str(config_ptr->config_item),
                   item_value_type);

    //start with success
    radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
    int_val = *((uint32_t *)config_ptr->config_item_value);

    /* Depending on the Config item,
       validate the type processed and put it in the request*/
    switch(config_ptr->config_item)
    {
      case QCRIL_QMI_RADIO_CONFIG_RCS_SMS_APP:
        qmi_req.default_sms_app_valid = TRUE;
        qmi_req.default_sms_app = int_val;
        QCRIL_LOG_INFO("Set default sms app to: %d", int_val);
        break;

      case QCRIL_QMI_RADIO_CONFIG_RCS_VVM_APP:
        qmi_req.default_vvm_app_valid = TRUE;
        qmi_req.default_vvm_app = int_val;
        QCRIL_LOG_INFO("Set default vvm app to: %d", int_val);
        break;

      default:
        radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
        QCRIL_LOG_ERROR("Invalid item..");
        break;
    }
    /* If the qmi req structure generation is SUCCESS then send
       QMI async request */
    if(radio_config_error == QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS)
    {
      qmi_client_error = qmi_client_imss_send_async(QMI_IMS_SETTINGS_SET_RCS_SM_NEW_CONFIG_REQ_V01,
              &qmi_req,
              sizeof(ims_settings_set_rcs_new_config_req_msg_v01),
              sizeof(ims_settings_set_rcs_new_config_rsp_msg_v01),
              qcril_qmi_imss_command_cb_v02,
              config_ptr->extra_data);

      QCRIL_LOG_INFO(".. qmi send async res %d", (int)qmi_client_error);
      radio_config_error =
        qcril_qmi_radio_config_map_internal_error_to_radio_config_error(qmi_client_error);
    }

  }while(FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(radio_config_error);
  return radio_config_error;
}// qcril_qmi_radio_config_imss_set_rcs_new_config_req_handler

//===========================================================================
// qcril_qmi_radio_config_imss_set_ims_new_config_req_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_ims_new_config_req_handler
(
 const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  ims_settings_set_ims_new_config_req_msg_v01 qmi_req;
  qcril_qmi_radio_config_item_value_type item_value_type;
  qcril_qmi_radio_config_error_type radio_config_error =
            QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
  qmi_client_error_type qmi_client_error = E_FAILURE;

  do
  {
    /* Validate the config pointer */
    if( (config_ptr == NULL) ||
        (config_ptr->extra_data_len == 0) ||
        (config_ptr->extra_data == NULL) ||
        (config_ptr->config_item_value_len == 0) ||
        (config_ptr->config_item_value == NULL) )
    {
      QCRIL_LOG_ERROR("Invalid config params");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }

    memset(&qmi_req, 0, sizeof(qmi_req));

    /* Validate if the item_value_type sent in the config params matches with the
       one in the table. The table should have the right value type for the item */
    item_value_type = qcril_qmi_radio_config_get_item_value_type(config_ptr->config_item);
    if(config_ptr->config_item_value_type != item_value_type)
    {
      QCRIL_LOG_ERROR("Invalid item type..doesnt match with the table");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_ITEM_VALUE_TYPE;
      break;
    }

    QCRIL_LOG_INFO("Config item to set: %s, type: %d",
        qcril_qmi_radio_config_get_item_log_str(config_ptr->config_item), item_value_type);

    //start with success
    radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;

    /* Depending on the Config item,
       validate the type processed and put it in the request*/
    switch(config_ptr->config_item)
    {
      case QCRIL_QMI_RADIO_CONFIG_USER_IMS_DOMAIN_NAME:
        strlcpy(qmi_req.ims_user_agent, (const char *)config_ptr->config_item_value,
            IMS_SETTINGS_IMS_USER_AGENT_LEN_V01);
        QCRIL_LOG_INFO("Set config USER ims_domain to: %s", qmi_req.ims_user_agent);
        break;

      default:
        radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
        QCRIL_LOG_ERROR("Invalid item..")
          break;
    }
    /* If the qmi req structure generation is SUCCESS then send
       QMI async request */
    if(radio_config_error == QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS)
    {
      qmi_client_error = qmi_client_imss_send_async(QMI_IMS_SETTINGS_SET_IMS_NEW_CONFIG_REQ_V01,
          &qmi_req,
          sizeof(qmi_req),
          sizeof(ims_settings_set_ims_new_config_rsp_msg_v01),
          qcril_qmi_imss_command_cb_v02,
          config_ptr->extra_data);

      QCRIL_LOG_INFO(".. qmi send async res %d", (int)qmi_client_error);
      radio_config_error = qcril_qmi_radio_config_map_internal_error_to_radio_config_error(
                                   qmi_client_error);
    }
  }while(FALSE);

  QCRIL_LOG_FUNC_RETURN_WITH_RET(radio_config_error);

  return radio_config_error;
}// qcril_qmi_radio_config_imss_set_ims_new_config_req_handler

/*===========================================================================
                           GET RESPONSE HANDLERS
===========================================================================*/

//===========================================================================
// qcril_qmi_radio_config_imss_get_codec_dynamic_config_resp_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_codec_dynamic_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  qcril_request_params_type *params_ptr = NULL;

  ims_settings_get_codec_dynamic_config_rsp_msg_v01 *qmi_resp = NULL;
  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;

  qcril_qmi_radio_config_params_type config_params;
  memset(&config_params, 0, sizeof(config_params));

  /* To check if error cause code is sent in FAILURE case */
  bool error_cause_code_present = FALSE;

  //place holder for copying the values.
  uint32_t int_value = 0;
  char string_value[QCRIL_QMI_RADIO_CONFIG_SETTINGS_STRING_LEN_MAX];

  QCRIL_LOG_FUNC_ENTRY();

  do
  {
    /* Validate config ptr */
    if((config_ptr == NULL) ||
       (config_ptr->extra_data == NULL) ||
       (config_ptr->extra_data_len <=0))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }

    /* Create a basic config_params structure
       which will be used to send the response*/
    config_params.config_item = config_ptr->config_item;
    config_params.config_item_value_type = qcril_qmi_radio_config_get_item_value_type(config_params.config_item);

    /* Read request_params_type */
    params_ptr = (qcril_request_params_type *)config_ptr->extra_data;
    if(params_ptr->data == NULL)
    {
      QCRIL_LOG_ERROR("Params data is null..");
      break;
    }

    /* Get the qmi response structure from req_params_type */
    qmi_resp = (ims_settings_get_codec_dynamic_config_rsp_msg_v01 *) params_ptr->data;
    /* If the qmi response is success, then process the data */
    if(qmi_resp->resp.result == QMI_RESULT_SUCCESS_V01)
    {
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
      //Check for the item requested by the request and initialize it to the value to be sent back
      switch(config_ptr->config_item)
      {
        case QCRIL_QMI_RADIO_CONFIG_VOIP_AMR_MODE:
          if(qmi_resp->amr_mode_set_mask_valid)
          {
            strlcpy(string_value, qcril_qmi_radio_config_get_string_amr_or_amr_wb_mode(
                  qmi_resp->amr_mode_set_mask,QCRIL_QMI_RADIO_CONFIG_VOIP_AMR_MODE_MAX_INDEX),
                QCRIL_QMI_RADIO_CONFIG_SETTINGS_STRING_LEN_MAX);
            config_params.config_item_value = (void*)&string_value;
            config_params.config_item_value_len = sizeof(config_params.config_item_value);
            QCRIL_LOG_INFO("get VOIP amr_mode_set_mask value: 0x%x, converted to: %s",
                            qmi_resp->amr_mode_set_mask, string_value);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_VOIP_AMR_WB_MODE:
          if(qmi_resp->amr_wb_mode_set_mask_valid)
          {
            strlcpy(string_value, qcril_qmi_radio_config_get_string_amr_or_amr_wb_mode(
                  qmi_resp->amr_wb_mode_set_mask,QCRIL_QMI_RADIO_CONFIG_VOIP_AMR_MODE_MAX_INDEX),
                QCRIL_QMI_RADIO_CONFIG_SETTINGS_STRING_LEN_MAX);
            config_params.config_item_value = (void*)&string_value;
            config_params.config_item_value_len = sizeof(config_params.config_item_value);
            QCRIL_LOG_INFO("get VOIP amr_wb_mode value: %d, converted to: %s",
                            qmi_resp->amr_wb_mode_set_mask, string_value);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_WB_OCTET_ALIGNED_DYNAMIC_PT:
          if(qmi_resp->amr_wb_octet_aligned_dynamic_pt_valid)
          {
            int_value = qmi_resp->amr_wb_octet_aligned_dynamic_pt;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get QIPCALL amr_wb_octet_aligned_dynamic_pt value: %d",
                            qmi_resp->amr_wb_octet_aligned_dynamic_pt);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_WB_BANDWIDTH_EFFICIENT_DYNAMIC_PT:
          if(qmi_resp->amr_wb_bandwidth_efficient_dynamic_pt_valid)
          {
            int_value = qmi_resp->amr_wb_bandwidth_efficient_dynamic_pt;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get QIPCALL amr_wb_bandwidth_efficient_dynamic_pt: %d",
                            qmi_resp->amr_wb_bandwidth_efficient_dynamic_pt);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_OCTET_ALIGNED_DYNAMIC_PT:
          if(qmi_resp->amr_octet_aligned_dynamic_pt_valid)
          {
            int_value = qmi_resp->amr_octet_aligned_dynamic_pt;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get QIPCALL amr_octet_aligned_dynamic_pt value: %d",
                            qmi_resp->amr_octet_aligned_dynamic_pt);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_BANDWIDTH_EFFICIENT_DYNAMIC_PT:
          if(qmi_resp->amr_bandwidth_efficient_dynamic_pt_valid)
          {
            int_value = qmi_resp->amr_bandwidth_efficient_dynamic_pt;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get QIPCALL amr_bandwidth_efficient_dynamic_pt value: %d",
                            qmi_resp->amr_bandwidth_efficient_dynamic_pt);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_QIPCALL_DTMF_WB_DYNAMIC_POINT:
          if(qmi_resp->dtmf_wb_dynamic_pt_valid)
          {
            int_value = qmi_resp->dtmf_wb_dynamic_pt;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get QIPCALL dtmf_wb_dynamic_pt value: %d",
                            qmi_resp->dtmf_wb_dynamic_pt);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_QIPCALL_DTMF_NB_DYNAMIC_PT:
          if(qmi_resp->dtmf_nb_dynamic_pt_valid)
          {
            int_value = qmi_resp->dtmf_nb_dynamic_pt;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get QIPCALL dtmf_nb_dynamic_pt value: %d",
                            qmi_resp->dtmf_nb_dynamic_pt);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_DEFAULT_MODE:
          if(qmi_resp->amr_wb_default_mode_valid)
          {
            snprintf(string_value, QCRIL_QMI_RADIO_CONFIG_SETTINGS_STRING_LEN_MAX, "%d",
              qmi_resp->amr_wb_default_mode);
            config_params.config_item_value = (void*)&string_value;
            config_params.config_item_value_len = sizeof(config_params.config_item_value);
            QCRIL_LOG_INFO("get VOIP amr_default_wb_mode value: %d, converted to: %s",
                            qmi_resp->amr_wb_default_mode, string_value);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        default:
          radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
          QCRIL_LOG_INFO("Item not valid in qmi structure..");
          break;
      }
      if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS)
      {
        break;
      }
    }
    /* otherwise, If the response is specifically ERR_CAUSE_CODE
     * specifically look if settings response is sent by the modem
     * if it is then send the settings resp else empty response
     * is sent */
    else if(qmi_resp->resp.error == QMI_ERR_CAUSE_CODE_V01)
    {
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(qmi_resp->resp.error);
      if(qmi_resp->settings_resp_valid)
      {
        //flag to determine if the FailureCause is sent
        error_cause_code_present = TRUE;
      }
      else
      {
        QCRIL_LOG_INFO("qmi error response");
        break;
      }
    }
    /* If not any of the above, radio_config_error is generic error and
    * Empty payload response is sent back */
    else
    {
      QCRIL_LOG_INFO("qmi error response");
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(qmi_resp->resp.error);
      break;
    }
    /* Send response with success or error cause and config item in params */
    qcril_qmi_imss_get_ims_config_log_and_send_response( params_ptr->t,
                                                         &config_params,
                                                         radio_config_error,
                                                         ((error_cause_code_present) ?
                                                         qcril_qmi_radio_config_map_qmi_settings_resp_to_radio_config_settings_resp(qmi_resp->settings_resp) :
                                                         QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR) );
  }while(FALSE);

  /* If ril_err is GENERIC_FAILURE and also if error cause code response
   * is not sent, send an empty payload response back */
  if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS && !error_cause_code_present && params_ptr)
  {
    QCRIL_LOG_INFO("Sending Error response..");
    qcril_qmi_imss_get_ims_config_log_and_send_response( params_ptr->t,
                                                         NULL,
                                                         radio_config_error,
                                                         QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR);
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(0);
  return QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
}// qcril_qmi_radio_config_imss_get_codec_dynamic_config_resp_handler

//===========================================================================
// qcril_qmi_radio_config_imss_get_voice_dynamic_config_resp_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_voice_dynamic_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_request_params_type *params_ptr = NULL;

  ims_settings_get_voice_dynamic_config_rsp_msg_v01 *qmi_resp = NULL;
  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;

  qcril_qmi_radio_config_params_type config_params;
  memset(&config_params, 0, sizeof(config_params));

  /* To check if error cause code is sent in FAILURE case */
  bool error_cause_code_present = FALSE;

  //place holder for copying the values.
  bool bool_value = 0;
  uint32_t int_value = 0;

  do
  {
    /* Validate config ptr */
    if((config_ptr == NULL) ||
       (config_ptr->extra_data == NULL) ||
       (config_ptr->extra_data_len <=0))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }

    /* Create a basic config_params structure
       which will be used to send the response*/
    config_params.config_item = config_ptr->config_item;
    config_params.config_item_value_type = qcril_qmi_radio_config_get_item_value_type(config_params.config_item);

    /* Read request_params_type */
    params_ptr = (qcril_request_params_type *)config_ptr->extra_data;
    if(params_ptr->data == NULL)
    {
      QCRIL_LOG_ERROR("Params data is null..");
      break;
    }

    /* Get the qmi response structure from req_params_type */
    qmi_resp = (ims_settings_get_voice_dynamic_config_rsp_msg_v01 *) params_ptr->data;
    /* If the qmi response is success, then process the data */
    if(qmi_resp->resp.result == QMI_RESULT_SUCCESS_V01)
    {
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
      //Check for the item requested by the request and initialize it to the value to be sent back
      switch(config_ptr->config_item)
      {
        case QCRIL_QMI_RADIO_CONFIG_VOIP_SESSION_EXPIRY_TIMER:
          if(qmi_resp->session_expires_valid)
          {
            int_value = qmi_resp->session_expires;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get VOIP session_expiry_timer value: %d",
                            qmi_resp->session_expires);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_VOIP_MIN_SESSION_EXPIRY:
          if(qmi_resp->min_session_expiry_valid)
          {
            int_value = qmi_resp->min_session_expiry;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get VOIP min_session_expiry value: %d",
                            qmi_resp->min_session_expiry);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_VOIP_SILENT_REDIAL_ENABLED:
          if(qmi_resp->silent_redial_enable_valid)
          {
            bool_value = (qmi_resp->silent_redial_enable) ? TRUE : FALSE;
            config_params.config_item_value_len = sizeof(bool_value);
            config_params.config_item_value = (void*) &bool_value;
            QCRIL_LOG_INFO("get VOIP voip_silent_redial_enabled value: %d",
                            qmi_resp->silent_redial_enable);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_QIPCALL_VICE_ENABLED:
          if(qmi_resp->vice_enabled_valid)
          {
            bool_value = (qmi_resp->vice_enabled) ? TRUE : FALSE;
            config_params.config_item_value_len = sizeof(bool_value);
            config_params.config_item_value = (void*) &bool_value;
            QCRIL_LOG_INFO("get VOIP vice_enabled value: %d",
                            qmi_resp->vice_enabled);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        default:
          radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
          QCRIL_LOG_INFO("Item not valid in qmi structure..");
          break;
      }
      if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS)
      {
        break;
      }
    }
    /* otherwise, If the response is specifically ERR_CAUSE_CODE
     * specifically look if settings response is sent by the modem
     * if it is then send the settings resp else empty response
     * is sent */
    else if(qmi_resp->resp.error == QMI_ERR_CAUSE_CODE_V01)
    {
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(qmi_resp->resp.error);
      if(qmi_resp->settings_resp_valid)
      {
        //flag to determine if the FailureCause is sent
        error_cause_code_present = TRUE;
      }
      else
      {
        QCRIL_LOG_INFO("qmi error response");
        break;
      }
    }
    /* If not any of the above, radio_config_error is generic error and
    * Empty payload response is sent back */
    else
    {
      QCRIL_LOG_INFO("qmi error response");
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(qmi_resp->resp.error);
      break;
    }
    /* Send response with success or error cause and config item in params */
    qcril_qmi_imss_get_ims_config_log_and_send_response( params_ptr->t,
                                                         &config_params,
                                                         radio_config_error,
                                                         ((error_cause_code_present) ?
                                                         qcril_qmi_radio_config_map_qmi_settings_resp_to_radio_config_settings_resp(qmi_resp->settings_resp) :
                                                         QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR) );
  }while(FALSE);

  /* If ril_err is GENERIC_FAILURE and also if error cause code response
   * is not sent, send an empty payload response back */
  if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS && !error_cause_code_present && params_ptr)
  {
    QCRIL_LOG_INFO("Sending Error response..");
    qcril_qmi_imss_get_ims_config_log_and_send_response( params_ptr->t,
                                                         NULL,
                                                         radio_config_error,
                                                         QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR);
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(0);
  return QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
}// qcril_qmi_radio_config_imss_get_voice_dynamic_config_resp_handler

//===========================================================================
// qcril_qmi_radio_config_imss_get_sip_new_config_resp_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_sip_new_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_request_params_type *params_ptr = NULL;

  ims_settings_get_sip_new_config_rsp_msg_v01 *qmi_resp = NULL;
  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;

  qcril_qmi_radio_config_params_type config_params;
  memset(&config_params, 0, sizeof(config_params));

  /* To check if error cause code is sent in FAILURE case */
  bool error_cause_code_present = FALSE;

  //place holder for copying the values.
  uint32_t int_value = 0;
  bool bool_value = 0;

  do
  {
    /* Validate config ptr */
    if((config_ptr == NULL) ||
       (config_ptr->extra_data == NULL) ||
       (config_ptr->extra_data_len <=0))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }
    /* Create a basic config_params structure
       which will be used to send the response*/
    config_params.config_item = config_ptr->config_item;
    config_params.config_item_value_type = qcril_qmi_radio_config_get_item_value_type(config_params.config_item);

    /* Read request_params_type */
    params_ptr = (qcril_request_params_type *) config_ptr->extra_data;
    if(params_ptr->data == NULL)
    {
      QCRIL_LOG_ERROR("Params data is null..");
      break;
    }

    /* Get the qmi response structure from req_params_type */
    qmi_resp = (ims_settings_get_sip_new_config_rsp_msg_v01 *) params_ptr->data;

    /* If the qmi response is success, then process the data */
    if(qmi_resp->resp.result == QMI_RESULT_SUCCESS_V01)
    {
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
      //Check for the item requested by the request and initialize it to the value to be sent back
      switch(config_ptr->config_item)
      {
        case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_T1:
          if(qmi_resp->timer_t1_valid)
          {
            int_value = qmi_resp->timer_t1;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get SIP timer_t1 value: %d",
                            qmi_resp->timer_t1);
          }
          else
          {
              QCRIL_LOG_INFO("Did not get the value requested in qmi message");
              radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_T2:
          if(qmi_resp->timer_t2_valid)
          {
            int_value = qmi_resp->timer_t2;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get SIP timer_t2 value: %d",
                            qmi_resp->timer_t2);
          }
          else
          {
              QCRIL_LOG_INFO("Did not get the value requested in qmi message");
              radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TF:
          if(qmi_resp->timer_tf_valid)
          {
            int_value = qmi_resp->timer_tf;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get SIP timer_tf value: %d",
                            qmi_resp->timer_tf);
          }
          else
          {
              QCRIL_LOG_INFO("Did not get the value requested in qmi message");
              radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;


        case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_T4:
          if(qmi_resp->timer_t4_valid)
          {
            int_value = qmi_resp->timer_t4;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get SIP timer_t4 value: %d",
                            qmi_resp->timer_t4);
          }
          else
          {
              QCRIL_LOG_INFO("Did not get the value requested in qmi message");
              radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TA_VALUE:
          if(qmi_resp->timer_ta_valid)
          {
            int_value = qmi_resp->timer_ta;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get SIP timer_ta value: %d",
                            qmi_resp->timer_ta);
          }
          else
          {
              QCRIL_LOG_INFO("Did not get the value requested in qmi message");
              radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TB_VALUE:
          if(qmi_resp->timer_tb_valid)
          {
            int_value = qmi_resp->timer_tb;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get SIP timer_tb value: %d",
                            qmi_resp->timer_tb);
          }
          else
          {
              QCRIL_LOG_INFO("Did not get the value requested in qmi message");
              radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TD_VALUE:
          if(qmi_resp->timer_td_valid)
          {
            int_value = qmi_resp->timer_td;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get SIP timer_td value: %d",
                            qmi_resp->timer_td);
          }
          else
          {
              QCRIL_LOG_INFO("Did not get the value requested in qmi message");
              radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TE_VALUE:
          if(qmi_resp->timer_te_valid)
          {
            int_value = qmi_resp->timer_te;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get SIP timer_te value: %d",
                            qmi_resp->timer_te);
          }
          else
          {
              QCRIL_LOG_INFO("Did not get the value requested in qmi message");
              radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TG_VALUE:
          if(qmi_resp->timer_tg_valid)
          {
            int_value = qmi_resp->timer_tg;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get SIP timer_tg value: %d",
                            qmi_resp->timer_tg);
          }
          else
          {
              QCRIL_LOG_INFO("Did not get the value requested in qmi message");
              radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TH_VALUE:
          if(qmi_resp->timer_th_valid)
          {
            int_value = qmi_resp->timer_th;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get SIP timer_th value: %d",
                            qmi_resp->timer_th);
          }
          else
          {
              QCRIL_LOG_INFO("Did not get the value requested in qmi message");
              radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TI_VALUE:
          if(qmi_resp->timer_ti_valid)
          {
            int_value = qmi_resp->timer_ti;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get SIP timer_ti value: %d",
                            qmi_resp->timer_ti);
          }
          else
          {
              QCRIL_LOG_INFO("Did not get the value requested in qmi message");
              radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TJ:
          if(qmi_resp->timer_tj_valid)
          {
            int_value = qmi_resp->timer_tj;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get SIP timer_tj value: %d",
                            qmi_resp->timer_tj);
          }
          else
          {
              QCRIL_LOG_INFO("Did not get the value requested in qmi message");
              radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TK_VALUE:
          if(qmi_resp->timer_tk_valid)
          {
            int_value = qmi_resp->timer_tk;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get SIP timer_tk value: %d",
                            qmi_resp->timer_tk);
          }
          else
          {
              QCRIL_LOG_INFO("Did not get the value requested in qmi message");
              radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_SIP_KEEPALIVE_ENABLED:
          if(qmi_resp->keepalive_enabled_valid)
          {
            bool_value = (qmi_resp->keepalive_enabled) ? TRUE : FALSE;
            config_params.config_item_value_len = sizeof(bool_value);
            config_params.config_item_value = (void*) &bool_value;
            QCRIL_LOG_INFO("get SIP keepalive_enabled value: %d",
                            qmi_resp->keepalive_enabled);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        default:
          radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
          QCRIL_LOG_INFO("Item not valid in qmi structure..");
          break;
      }
      if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS)
      {
        break;
      }
    }
    /* otherwise, If the response is specifically ERR_CAUSE_CODE
     * specifically look if settings response is sent by the modem
     * if it is then send the settings resp else empty response
     * is sent */
    else if(qmi_resp->resp.error == QMI_ERR_CAUSE_CODE_V01)
    {
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(qmi_resp->resp.error);
      if(qmi_resp->settings_resp_valid)
      {
        //flag to determine if the FailureCause is sent
        error_cause_code_present = TRUE;
      }
      else
      {
        QCRIL_LOG_INFO("qmi error response");
        break;
      }
    }
    /* If not any of the above, ril_err is generic error and
    * Empty payload response is sent back */
    else
    {
      QCRIL_LOG_INFO("qmi error response");
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(qmi_resp->resp.error);
      break;
    }
    /* Send response with success or error cause and config item in params */
    qcril_qmi_imss_get_ims_config_log_and_send_response( params_ptr->t,
                                                         &config_params,
                                                         radio_config_error,
                                                         ((error_cause_code_present) ?
                                                         qcril_qmi_radio_config_map_qmi_settings_resp_to_radio_config_settings_resp(qmi_resp->settings_resp) :
                                                         QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR) );
  }while(FALSE);

  /* If ril_err is GENERIC_FAILURE and also if error cause code response
   * is not sent, send an empty payload response back */
  if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS && !error_cause_code_present && params_ptr)
  {
    QCRIL_LOG_INFO("Sending Error response..");
    qcril_qmi_imss_get_ims_config_log_and_send_response( params_ptr->t,
                                                         NULL,
                                                         radio_config_error,
                                                         QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR);
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(0);
  return QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
}// qcril_qmi_radio_config_imss_get_sip_config_resp_handler

//===========================================================================
// qcril_qmi_radio_config_imss_get_reg_mgr_new_config_resp_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_reg_mgr_new_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_request_params_type *params_ptr = NULL;

  ims_settings_get_reg_mgr_new_config_rsp_msg_v01 *qmi_resp = NULL;
  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;

  qcril_qmi_radio_config_params_type config_params;
  memset(&config_params, 0, sizeof(config_params));

  /* To check if error cause code is sent in FAILURE case */
  bool error_cause_code_present = FALSE;

  //place holder for copying the values.
  uint32_t int_value = 0;
  char string_value[QCRIL_QMI_RADIO_CONFIG_SETTINGS_STRING_LEN_MAX];
  memset(&string_value, 0, sizeof(string_value));

  do
  {
    /* Validate config ptr */
    if((config_ptr == NULL) ||
       (config_ptr->extra_data == NULL) ||
       (config_ptr->extra_data_len <=0))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }
    /* Create a basic config_params structure
       which will be used to send the response*/
    config_params.config_item = config_ptr->config_item;
    config_params.config_item_value_type = qcril_qmi_radio_config_get_item_value_type(config_params.config_item);

    /* Read request_params_type */
    params_ptr = (qcril_request_params_type *)config_ptr->extra_data;
    if(params_ptr->data == NULL)
    {
      QCRIL_LOG_ERROR("Params data is null..");
      break;
    }

    /* Get the qmi response structure from req_params_type */
    qmi_resp = (ims_settings_get_reg_mgr_new_config_rsp_msg_v01 *) params_ptr->data;

    /* If the qmi response is success, then process the data */
    if(qmi_resp->resp.result == QMI_RESULT_SUCCESS_V01)
    {
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
      //Check for the item requested by the request and initialize it to the value to be sent back
      switch(config_ptr->config_item)
      {
        case QCRIL_QMI_RADIO_CONFIG_REG_MGR_EXTENDED_T_DELAY:
          if(qmi_resp->reg_irat_hystersis_timer_valid)
          {
            int_value = qmi_resp->reg_irat_hystersis_timer;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get REG MGR EXT t_delay value: %d",
                            qmi_resp->reg_irat_hystersis_timer);
          }
          else
          {
              QCRIL_LOG_INFO("Did not get the value requested in qmi message");
              radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_REG_MGR_EXTENDED_REG_RETRY_BASE_TIME:
          if(qmi_resp->reg_retry_base_time_valid)
          {
            int_value = qmi_resp->reg_retry_base_time;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get REG MGR EXT reg_retry_base_time value: %d",
                            qmi_resp->reg_retry_base_time);
          }
          else
          {
              QCRIL_LOG_INFO("Did not get the value requested in qmi message");
              radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_REG_MGR_EXTENDED_REG_RETRY_MAX_TIME:
          if(qmi_resp->reg_retry_max_time_valid)
          {
            int_value = qmi_resp->reg_retry_max_time;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get REG MGR EXT reg_retry_max_time value: %d",
                            qmi_resp->reg_retry_max_time);
          }
          else
          {
              QCRIL_LOG_INFO("Did not get the value requested in qmi message");
              radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_REG_MGR_CONFIG_REG_MGR_PRIMARY_CSCF:
          if(qmi_resp->regmgr_primary_pcscf_valid)
          {
            config_params.config_item_value_len = strlen(qmi_resp->regmgr_primary_pcscf);
            strlcpy(string_value,
                    (const char *)qmi_resp->regmgr_primary_pcscf,
                    IMS_SETTINGS_STRING_LEN_MAX_V01);
            config_params.config_item_value = (void*)&string_value;
            QCRIL_LOG_INFO("get REG_MGR regmgr_primary_pcscf value: %s", string_value);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        default:
          radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
          QCRIL_LOG_INFO("Item not valid in qmi structure..");
          break;
      }
      if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS)
      {
        break;
      }
    }
    /* otherwise, If the response is specifically ERR_CAUSE_CODE
     * specifically look if settings response is sent by the modem
     * if it is then send the settings resp else empty response
     * is sent */
    else if(qmi_resp->resp.error == QMI_ERR_CAUSE_CODE_V01)
    {
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(qmi_resp->resp.error);
      if(qmi_resp->settings_resp_valid)
      {
        //flag to determine if the settings resp is sent
        error_cause_code_present = TRUE;
      }
      else
      {
        QCRIL_LOG_INFO("qmi error response");
        break;
      }
    }
    /* If not any of the above, ril_err is generic error and
    * Empty payload response is sent back */
    else
    {
      QCRIL_LOG_INFO("qmi error response");
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(qmi_resp->resp.error);
      break;
    }
    /* Send response with success or error cause and config item in params */
    qcril_qmi_imss_get_ims_config_log_and_send_response( params_ptr->t,
                                                         &config_params,
                                                         radio_config_error,
                                                         ((error_cause_code_present) ?
                                                         qcril_qmi_radio_config_map_qmi_settings_resp_to_radio_config_settings_resp(qmi_resp->settings_resp) :
                                                         QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR) );
  }while(FALSE);

  /* If ril_err is GENERIC_FAILURE and also if error cause code response
   * is not sent, send an empty payload response back */
  if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS && !error_cause_code_present && params_ptr)
  {
    QCRIL_LOG_INFO("Sending Error response..");
    qcril_qmi_imss_get_ims_config_log_and_send_response( params_ptr->t,
                                                         NULL,
                                                         radio_config_error,
                                                         QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR);
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(0);
  return QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
}// qcril_qmi_radio_config_imss_get_reg_mgr_new_config_resp_handler

//===========================================================================
// qcril_qmi_radio_config_imss_get_ims_service_enable_config_resp_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_ims_service_enable_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_request_params_type *params_ptr = NULL;

  ims_settings_get_ims_service_enable_config_rsp_msg_v01 *qmi_resp = NULL;
  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;

  qcril_qmi_radio_config_params_type config_params;
  memset(&config_params, 0, sizeof(config_params));

  /* To check if error cause code is sent in FAILURE case */
  bool error_cause_code_present = FALSE;

  //place holder for copying values.
  bool bool_value = 0;
  int int_value = 0;

  do
  {
    /* Validate config ptr */
    if((config_ptr == NULL) ||
       (config_ptr->extra_data == NULL) ||
       (config_ptr->extra_data_len <=0))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }
    /* Create a basic config_params structure
       which will be used to send the response*/
    config_params.config_item = config_ptr->config_item;
    config_params.config_item_value_type = qcril_qmi_radio_config_get_item_value_type(config_params.config_item);

    /* Read request_params_type */
    params_ptr = (qcril_request_params_type *)config_ptr->extra_data;
    if(params_ptr->data == NULL)
    {
      QCRIL_LOG_ERROR("Params data is null..");
      break;
    }

    /* Get the qmi response structure from req_params_type */
    qmi_resp = (ims_settings_get_ims_service_enable_config_rsp_msg_v01 *) params_ptr->data;

    /* If the qmi response is success, then process the data */
    if(qmi_resp->resp.result == QMI_RESULT_SUCCESS_V01)
    {
      qcril_qmi_imss_store_get_ims_service_enable_resp(qmi_resp);
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
      //Check for the item requested by the request and initialize it to the value to be sent back
      switch(config_ptr->config_item)
      {
        case QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_VOLTE:
          if(qmi_resp->service_mask_by_network_enabled_valid)
          {
            bool_value = (qmi_resp->service_mask_by_network_enabled &
                IMS_SETTINGS_OMADM_SERVICES_VOLTE_ENABLE_BIT_MASK_V01) ? TRUE : FALSE;
            config_params.config_item_value_len = sizeof(bool_value);
            config_params.config_item_value = (void*) &bool_value;
            QCRIL_LOG_INFO("get CLIENT PROVISIONING enable_volte value: %d",
                            bool_value);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_VOWIFI:
          if(qmi_resp->service_mask_by_network_enabled_valid)
          {
            bool_value = (qmi_resp->service_mask_by_network_enabled &
                IMS_SETTINGS_OMADM_SERVICES_VOWIFI_ENABLE_BIT_MASK_V01) ? TRUE : FALSE;
            config_params.config_item_value_len = sizeof(bool_value);
            config_params.config_item_value = (void*) &bool_value;
            QCRIL_LOG_INFO("get CLIENT PROVISIONING enable_vowifi value: %d",
                            bool_value);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_VT:
          if(qmi_resp->service_mask_by_network_enabled_valid)
          {
            bool_value = (qmi_resp->service_mask_by_network_enabled &
                IMS_SETTINGS_OMADM_SERVICES_VT_ENABLE_BIT_MASK_V01) ? TRUE : FALSE;
            config_params.config_item_value_len = sizeof(bool_value);
            config_params.config_item_value = (void*) &bool_value;
            QCRIL_LOG_INFO("get CLIENT PROVISIONING enable_vt value: %d",
                            bool_value);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_PRESENCE:
          if(qmi_resp->service_mask_by_network_enabled_valid)
          {
            bool_value = (qmi_resp->service_mask_by_network_enabled &
                IMS_SETTINGS_OMADM_SERVICES_PRESENCE_ENABLE_BIT_MASK_V01) ? TRUE : FALSE;
            config_params.config_item_value_len = sizeof(bool_value);
            config_params.config_item_value = (void*) &bool_value;
            QCRIL_LOG_INFO("get CLIENT PROVISIONING enable_presence value: %d",
                            bool_value);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_WIFI_CALL_ROAMING:
          if( (qmi_resp->wifi_calling_in_roaming_enabled_valid) )
          {
            int_value = qmi_resp->wifi_calling_in_roaming_enabled;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get CLIENT PROVISIONING wifi_calling_in_roaming value: %d",
                            int_value);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_WIFI_CALL_PREFERENCE:
          //TODO: misc functions for conversion. and internal enums
          if( (qmi_resp->call_mode_preference_valid) )
          {
            int_value =
                qcril_qmi_radio_config_map_ims_wifi_mode_to_qcril_wifi_mode(
                    qmi_resp->call_mode_preference);

            //None is sent by the conversion function if the value is one
            //of the values which are not supported by framework.
            //Supported ones: wifi_only, wifi_pref, cellular_pref
            if(int_value != QCRIL_QMI_RADIO_CONFIG_IMSS_VOICE_OVER_WIFI_MODE_NONE)
            {
              config_params.config_item_value_len = sizeof(int_value);
              config_params.config_item_value = (void*) &int_value;
              QCRIL_LOG_INFO("get CLIENT PROVISIONING wifi_call_preference value: %d",
                              int_value);
            }
            else
            {
              QCRIL_LOG_INFO("Value %d not supported, hence send error",
                      qmi_resp->call_mode_preference);
              radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
            }
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_QIPCALL_MOBILE_DATA_ENABLED:
          if(qmi_resp->mobile_data_enabled_valid)
          {
            bool_value = (qmi_resp->mobile_data_enabled) ? TRUE : FALSE;
            config_params.config_item_value_len = sizeof(bool_value);
            config_params.config_item_value = (void*) &bool_value;
            QCRIL_LOG_INFO("get QIPCALL mobile_data_enabled value: %d",
                            qmi_resp->mobile_data_enabled);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_SMS_OVER_IP:
          if(qmi_resp->sms_enabled_valid)
          {
            bool_value = (qmi_resp->sms_enabled) ? TRUE : FALSE;
            config_params.config_item_value_len = sizeof(bool_value);
            config_params.config_item_value = (void*) &bool_value;
            QCRIL_LOG_INFO("get SMS sms_enabled value: %d",
                            qmi_resp->sms_enabled);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_PRESENCE_VOLTE_USER_OPTED_IN_STATUS:
          if(qmi_resp->presence_enabled_valid)
          {
            bool_value = (qmi_resp->presence_enabled) ? TRUE : FALSE;
            config_params.config_item_value_len = sizeof(bool_value);
            config_params.config_item_value = (void*) &bool_value;
            QCRIL_LOG_INFO("get presence_enabled (VOLTE_USER_OPTED_IN_STATUS) value: %d",
                            qmi_resp->presence_enabled);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_RTT:
          if(qmi_resp->rtt_enabled_valid)
          {
            bool_value = (qmi_resp->rtt_enabled) ? TRUE : FALSE;
            config_params.config_item_value_len = sizeof(bool_value);
            config_params.config_item_value = (void*) &bool_value;
            QCRIL_LOG_INFO("get CLIENT PROVISIONING enable_rtt value: %d",
                            qmi_resp->rtt_enabled);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        default:
          radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
          QCRIL_LOG_INFO("Item not valid in qmi structure..");
          break;
      }
      if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS)
      {
        break;
      }
    }
    /* otherwise, If the response is specifically ERR_CAUSE_CODE
     * specifically look if settings response is sent by the modem
     * if it is then send the settings resp else empty response
     * is sent */
    else if(qmi_resp->resp.error == QMI_ERR_CAUSE_CODE_V01)
    {
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(qmi_resp->resp.error);
      if(qmi_resp->settings_resp_valid)
      {
        //flag to determine if the FailureCause is sent
        error_cause_code_present = TRUE;
      }
      else
      {
        QCRIL_LOG_INFO("qmi error response");
        break;
      }
    }
    /* If not any of the above, ril_err is generic error and
    * Empty payload response is sent back */
    else
    {
      QCRIL_LOG_INFO("qmi error response");
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(qmi_resp->resp.error);
      break;
    }
    /* Send response with success or error cause and config item in params */
    qcril_qmi_imss_get_ims_config_log_and_send_response( params_ptr->t,
                                                         &config_params,
                                                         radio_config_error,
                                                         ((error_cause_code_present) ?
                                                         qcril_qmi_radio_config_map_qmi_settings_resp_to_radio_config_settings_resp(qmi_resp->settings_resp) :
                                                         QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR) );
  }while(FALSE);

  /* If ril_err is GENERIC_FAILURE and also if error cause code response
   * is not sent, send an empty payload response back */
  if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS && !error_cause_code_present && params_ptr)
  {
    QCRIL_LOG_INFO("Sending Error response..");
    qcril_qmi_imss_get_ims_config_log_and_send_response( params_ptr->t,
                                                         NULL,
                                                         radio_config_error,
                                                         QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR);
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(0);
  return QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
}// qcril_qmi_radio_config_imss_get_ims_service_enable_config_resp_handler

//===========================================================================
// qcril_qmi_radio_config_imss_get_sms_new_config_resp_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_sms_new_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_request_params_type *params_ptr = NULL;

  ims_settings_get_sms_new_config_rsp_msg_v01 *qmi_resp = NULL;
  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;

  qcril_qmi_radio_config_params_type config_params;
  memset(&config_params, 0, sizeof(config_params));

  /* To check if error cause code is sent in FAILURE case */
  bool error_cause_code_present = FALSE;

  //place holder for copying values.
  uint32_t int_value = 0;

  //placeholder to copy string value, to avoid malloc
  char string_value[QCRIL_QMI_RADIO_CONFIG_SETTINGS_STRING_LEN_MAX];
  memset(&string_value, 0, sizeof(string_value));

  do
  {
    /* Validate config ptr */
    if((config_ptr == NULL) ||
       (config_ptr->extra_data == NULL) ||
       (config_ptr->extra_data_len <=0))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }
    /* Create a basic config_params structure
       which will be used to send the response*/
    config_params.config_item = config_ptr->config_item;
    config_params.config_item_value_type = qcril_qmi_radio_config_get_item_value_type(config_params.config_item);

    /* Read request_params_type */
    params_ptr = (qcril_request_params_type *)config_ptr->extra_data;
    if(params_ptr->data == NULL)
    {
      QCRIL_LOG_ERROR("Params data is null..");
      break;
    }

    /* Get the qmi response structure from req_params_type */
    qmi_resp = (ims_settings_get_sms_new_config_rsp_msg_v01 *) params_ptr->data;

    /* If the qmi response is success, then process the data */
    if(qmi_resp->resp.result == QMI_RESULT_SUCCESS_V01)
    {
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
      //Check for the item requested by the request and initialize it to the value to be sent back
      switch(config_ptr->config_item)
      {
        case QCRIL_QMI_RADIO_CONFIG_SMS_FORMAT:
          //TODO: misc functions for conversion. and internal enums
          if( (qmi_resp->sms_format_valid) &&
              (qmi_resp->sms_format == IMS_SETTINGS_SMS_FORMAT_3GPP2_V01 ||
               qmi_resp->sms_format == IMS_SETTINGS_SMS_FORMAT_3GPP_V01) )
          {
            int_value = qmi_resp->sms_format;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get SMS sms_format value: %d",
                            qmi_resp->sms_format);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_SMS_PSI:
          if(qmi_resp->sms_psi_valid)
          {
            config_params.config_item_value_len = strlen(qmi_resp->sms_psi);
            strlcpy(string_value,
                    (const char *)qmi_resp->sms_psi,
                    IMS_SETTINGS_IMS_SMS_PSI_LEN_V01);
            config_params.config_item_value = (void*)&string_value;
            QCRIL_LOG_INFO("get SMS sms_psi value: %s", string_value);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        default:
          radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
          QCRIL_LOG_INFO("Item not valid in qmi structure..");
          break;
      }
      if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS)
      {
        break;
      }
    }
    /* otherwise, If the response is specifically ERR_CAUSE_CODE
     * specifically look if settings response is sent by the modem
     * if it is then send the settings resp else empty response
     * is sent */
    else if(qmi_resp->resp.error == QMI_ERR_CAUSE_CODE_V01)
    {
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(qmi_resp->resp.error);
      if(qmi_resp->settings_resp_valid)
      {
        //flag to determine if the FailureCause is sent
        error_cause_code_present = TRUE;
      }
      else
      {
        QCRIL_LOG_INFO("qmi error response");
        break;
      }
    }
    /* If not any of the above, ril_err is generic error and
    * Empty payload response is sent back */
    else
    {
      QCRIL_LOG_INFO("qmi error response");
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(qmi_resp->resp.error);
      break;
    }
    /* Send response with success or error cause and config item in params */
    qcril_qmi_imss_get_ims_config_log_and_send_response( params_ptr->t,
                                                         &config_params,
                                                         radio_config_error,
                                                         ((error_cause_code_present) ?
                                                         qcril_qmi_radio_config_map_qmi_settings_resp_to_radio_config_settings_resp(qmi_resp->settings_resp) :
                                                         QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR) );
  }while(FALSE);

  /* If ril_err is GENERIC_FAILURE and also if error cause code response
   * is not sent, send an empty payload response back */
  if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS && !error_cause_code_present && params_ptr)
  {
    QCRIL_LOG_INFO("Sending Error response..");
    qcril_qmi_imss_get_ims_config_log_and_send_response( params_ptr->t,
                                                         NULL,
                                                         radio_config_error,
                                                         QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR);
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(0);
  return QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
}// qcril_qmi_radio_config_imss_get_sms_new_config_resp_handler

//===========================================================================
// qcril_qmi_radio_config_imss_get_presence_new_config_resp_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_presence_new_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_request_params_type *params_ptr = NULL;

  ims_settings_get_presence_new_config_rsp_msg_v01 *qmi_resp = NULL;
  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;

  qcril_qmi_radio_config_params_type config_params;
  memset(&config_params, 0, sizeof(config_params));

  /* To check if error cause code is sent in FAILURE case */
  bool error_cause_code_present = FALSE;

  //place holder for copying values.
  bool bool_value = 0;
  uint32_t int_value = 0;

  do
  {
    /* Validate config ptr */
    if((config_ptr == NULL) ||
       (config_ptr->extra_data == NULL) ||
       (config_ptr->extra_data_len <=0))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }
    /* Create a basic config_params structure
       which will be used to send the response*/
    config_params.config_item = config_ptr->config_item;
    config_params.config_item_value_type = qcril_qmi_radio_config_get_item_value_type(config_params.config_item);

    /* Read request_params_type */
    params_ptr = (qcril_request_params_type *)config_ptr->extra_data;
    if(params_ptr->data == NULL)
    {
      QCRIL_LOG_ERROR("Params data is null..");
      break;
    }

    /* Get the qmi response structure from req_params_type */
    qmi_resp = (ims_settings_get_presence_new_config_rsp_msg_v01 *) params_ptr->data;
    /* If the qmi response is success, then process the data */
    if(qmi_resp->resp.result == QMI_RESULT_SUCCESS_V01)
    {
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
      //Check for the item requested by the request and initialize it to the value to be sent back
      switch(config_ptr->config_item)
      {
        case QCRIL_QMI_RADIO_CONFIG_PRESENCE_PUBLISH_EXPIRY_TIMER:
          if(qmi_resp->publish_timer_valid)
          {
            int_value = qmi_resp->publish_timer;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get PRESENCE publish_timer value: %d",
                            qmi_resp->publish_timer);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_PRESENCE_PUBLISH_EXTENDED_EXPIRY_TIMER:
          if(qmi_resp->publish_extended_timer_valid)
          {
            int_value = qmi_resp->publish_extended_timer;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get PRESENCE publish_extended_timer value: %d",
                            qmi_resp->publish_extended_timer);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_PRESENCE_CAPABILITIES_CACHE_EXPIRATION:
          if(qmi_resp->capabilites_cache_expiry_valid)
          {
            int_value = qmi_resp->capabilites_cache_expiry;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get PRESENCE capabilites_cache_expiry value: %d",
                            qmi_resp->capabilites_cache_expiry);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_PRESENCE_AVAILABILITY_CACHE_EXPIRATION:
          if(qmi_resp->availability_cache_expiry_valid)
          {
            int_value = qmi_resp->availability_cache_expiry;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get PRESENCE availability_cache_expiry value: %d",
                            qmi_resp->availability_cache_expiry);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_PRESENCE_CAPABILITY_POLL_INTERVAL:
          if(qmi_resp->capability_poll_interval_valid)
          {
            int_value = qmi_resp->capability_poll_interval;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get PRESENCE capability_poll_interval value: %d",
                            qmi_resp->capability_poll_interval);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_PRESENCE_MINIMUM_PUBLISH_INTERVAL:
          if(qmi_resp->publish_src_throttle_timer_valid)
          {
            int_value = qmi_resp->publish_src_throttle_timer;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get PRESENCE publish_src_throttle_timer value: %d",
                            qmi_resp->publish_src_throttle_timer);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_PRESENCE_MAXIMUM_SUBSCRIPTION_LIST_ENTRIES:
          if(qmi_resp->max_enties_in_list_subscribe_valid)
          {
            int_value = qmi_resp->max_enties_in_list_subscribe;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get PRESENCE max_enties_in_list_subscribe value: %d",
                            qmi_resp->max_enties_in_list_subscribe);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_PRESENCE_CAPABILITY_POLL_LIST_SUBSCRIPTION_EXPIRY_TIMER:
          if(qmi_resp->list_subscription_expiry_valid)
          {
            int_value = qmi_resp->list_subscription_expiry;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get PRESENCE list_subscription_expiry value: %d",
                            qmi_resp->list_subscription_expiry);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_PRESENCE_GZIP_ENABLED:
          if(qmi_resp->gzip_enabled_valid)
          {
            bool_value = (qmi_resp->gzip_enabled) ? TRUE : FALSE;
            config_params.config_item_value_len = sizeof(bool_value);
            config_params.config_item_value = (void*) &bool_value;
            QCRIL_LOG_INFO("get PRESENCE gzip_enabled value: %d",
                            qmi_resp->gzip_enabled);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_PRESENCE_CAPABILITY_DISCOVERY_ENABLE:
          if(qmi_resp->capability_discovery_enabled_valid)
          {
            bool_value = (qmi_resp->capability_discovery_enabled) ? TRUE : FALSE;
            config_params.config_item_value_len = sizeof(bool_value);
            config_params.config_item_value = (void*) &bool_value;
            QCRIL_LOG_INFO("get PRESENCE capability_discovery_enabled value: %d",
                            qmi_resp->capability_discovery_enabled);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        default:
          radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
          QCRIL_LOG_INFO("Item not valid in qmi structure..");
          break;
      }
      if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS)
      {
        break;
      }
    }
    /* otherwise, If the response is specifically ERR_CAUSE_CODE
     * specifically look if settings response is sent by the modem
     * if it is then send the settings resp else empty response
     * is sent */
    else if(qmi_resp->resp.error == QMI_ERR_CAUSE_CODE_V01)
    {
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(qmi_resp->resp.error);
      if(qmi_resp->settings_resp_valid)
      {
        //flag to determine if the FailureCause is sent
        error_cause_code_present = TRUE;
      }
      else
      {
        QCRIL_LOG_INFO("qmi error response");
        break;
      }
    }
    /* If not any of the above, ril_err is generic error and
    * Empty payload response is sent back */
    else
    {
      QCRIL_LOG_INFO("qmi error response");
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(qmi_resp->resp.error);
      break;
    }
    /* Send response with success or error cause and config item in params */
    qcril_qmi_imss_get_ims_config_log_and_send_response( params_ptr->t,
                                                         &config_params,
                                                         radio_config_error,
                                                         ((error_cause_code_present) ?
                                                         qcril_qmi_radio_config_map_qmi_settings_resp_to_radio_config_settings_resp(qmi_resp->settings_resp) :
                                                         QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR) );
  }while(FALSE);

  /* If ril_err is GENERIC_FAILURE and also if error cause code response
   * is not sent, send an empty payload response back */
  if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS && !error_cause_code_present && params_ptr)
  {
    QCRIL_LOG_INFO("Sending Error response..");
    qcril_qmi_imss_get_ims_config_log_and_send_response( params_ptr->t,
                                                         NULL,
                                                         radio_config_error,
                                                         QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR);
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(0);
  return QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
}// qcril_qmi_radio_config_imss_get_presence_new_config_resp_handler

//===========================================================================
// qcril_qmi_radio_config_imss_get_rtp_dynamic_config_resp_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_rtp_dynamic_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_request_params_type *params_ptr = NULL;

  ims_settings_get_rtp_dynamic_config_rsp_msg_v01 *qmi_resp = NULL;
  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;

  qcril_qmi_radio_config_params_type config_params;
  memset(&config_params, 0, sizeof(config_params));

  /* To check if error cause code is sent in FAILURE case */
  bool error_cause_code_present = FALSE;

  //place holder for integer value
  uint32_t int_value = 0;

  do
  {
    /* Validate config ptr */
    if((config_ptr == NULL) ||
       (config_ptr->extra_data == NULL) ||
       (config_ptr->extra_data_len <=0))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }
    /* Create a basic config_params structure
       which will be used to send the response*/
    config_params.config_item = config_ptr->config_item;
    config_params.config_item_value_type = qcril_qmi_radio_config_get_item_value_type(config_params.config_item);

    /* Read request_params_type */
    params_ptr = (qcril_request_params_type *)config_ptr->extra_data;
    if(params_ptr->data == NULL)
    {
      QCRIL_LOG_ERROR("Params data is null..");
      break;
    }

    /* Get the qmi response structure from req_params_type */
    qmi_resp = (ims_settings_get_rtp_dynamic_config_rsp_msg_v01 *) params_ptr->data;

    /* If the qmi response is success, then process the data */
    if(qmi_resp->resp.result == QMI_RESULT_SUCCESS_V01)
    {
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
      //Check for the item requested by the request and initialize it to the value to be sent back
      switch(config_ptr->config_item)
      {
        case QCRIL_QMI_RADIO_CONFIG_QIPCALL_SPEECH_START_PORT:
          if(qmi_resp->speech_start_port_valid)
          {
            int_value = qmi_resp->speech_start_port;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get QIPCALL speech_start_port value: %d",
                            qmi_resp->speech_start_port);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_QIPCALL_SPEECH_END_PORT:
          if(qmi_resp->speech_stop_port_valid)
          {
            int_value = qmi_resp->speech_stop_port;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get QIPCALL speech_stop_port value: %d",
                            qmi_resp->speech_stop_port);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        default:
          radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
          QCRIL_LOG_INFO("Item not valid in qmi structure..");
          break;
      }
      if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS)
      {
        break;
      }
    }
    /* otherwise, If the response is specifically ERR_CAUSE_CODE
     * specifically look if settings response is sent by the modem
     * if it is then send the settings resp else empty response
     * is sent */
    else if(qmi_resp->resp.error == QMI_ERR_CAUSE_CODE_V01)
    {
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(qmi_resp->resp.error);
      if(qmi_resp->settings_resp_valid)
      {
        //flag to determine if the FailureCause is sent
        error_cause_code_present = TRUE;
      }
      else
      {
        QCRIL_LOG_INFO("qmi error response");
        break;
      }
    }
    /* If not any of the above, ril_err is generic error and
    * Empty payload response is sent back */
    else
    {
      QCRIL_LOG_INFO("qmi error response");
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(qmi_resp->resp.error);
      break;
    }
    /* Send response with success or error cause and config item in params */
    qcril_qmi_imss_get_ims_config_log_and_send_response( params_ptr->t,
                                                         &config_params,
                                                         radio_config_error,
                                                         ((error_cause_code_present) ?
                                                         qcril_qmi_radio_config_map_qmi_settings_resp_to_radio_config_settings_resp(qmi_resp->settings_resp) :
                                                         QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR) );
  }while(FALSE);

  /* If ril_err is GENERIC_FAILURE and also if error cause code response
   * is not sent, send an empty payload response back */
  if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS && !error_cause_code_present && params_ptr)
  {
    QCRIL_LOG_INFO("Sending Error response..");
    qcril_qmi_imss_get_ims_config_log_and_send_response( params_ptr->t,
                                                         NULL,
                                                         radio_config_error,
                                                         QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR);
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(0);
  return QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
}// qcril_qmi_radio_config_imss_get_rtp_dynamic_config_resp_handler

//===========================================================================
// qcril_qmi_radio_config_imss_get_emer_dynamic_config_resp_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_emer_dynamic_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_request_params_type *params_ptr = NULL;

  ims_settings_get_emer_dynamic_config_rsp_msg_v01 *qmi_resp = NULL;
  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;

  qcril_qmi_radio_config_params_type config_params;
  memset(&config_params, 0, sizeof(config_params));

  /* To check if error cause code is sent in FAILURE case */
  bool error_cause_code_present = FALSE;

  //place holder for integer value
  uint32_t int_value = 0;

  do
  {
    /* Validate config ptr */
    if((config_ptr == NULL) ||
       (config_ptr->extra_data == NULL) ||
       (config_ptr->extra_data_len <=0))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }
    /* Create a basic config_params structure
       which will be used to send the response*/
    config_params.config_item = config_ptr->config_item;
    config_params.config_item_value_type = qcril_qmi_radio_config_get_item_value_type(config_params.config_item);

    /* Read request_params_type */
    params_ptr = (qcril_request_params_type *)config_ptr->extra_data;
    if(params_ptr->data == NULL)
    {
      QCRIL_LOG_ERROR("Params data is null..");
      break;
    }

    /* Get the qmi response structure from req_params_type */
    qmi_resp = (ims_settings_get_emer_dynamic_config_rsp_msg_v01 *) params_ptr->data;

    /* If the qmi response is success, then process the data */
    if(qmi_resp->resp.result == QMI_RESULT_SUCCESS_V01)
    {
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
      //Check for the item requested by the request and initialize it to the value to be sent back
      switch(config_ptr->config_item)
      {
        case QCRIL_QMI_RADIO_CONFIG_QIPCALL_EMERG_CALL_TIMER:
          if(qmi_resp->e911_call_setup_timer_valid)
          {
            int_value = qmi_resp->e911_call_setup_timer;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get QIPCALL e911_call_setup_timer value: %d",
                            qmi_resp->e911_call_setup_timer);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        default:
          radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
          QCRIL_LOG_INFO("Item not valid in qmi structure..");
          break;
      }
      if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS)
      {
        break;
      }
    }
    /* otherwise, If the response is specifically ERR_CAUSE_CODE
     * specifically look if settings response is sent by the modem
     * if it is then send the settings resp else empty response
     * is sent */
    else if(qmi_resp->resp.error == QMI_ERR_CAUSE_CODE_V01)
    {
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(qmi_resp->resp.error);
      if(qmi_resp->settings_resp_valid)
      {
        //flag to determine if the FailureCause is sent
        error_cause_code_present = TRUE;
      }
      else
      {
        QCRIL_LOG_INFO("qmi error response");
        break;
      }
    }
    /* If not any of the above, ril_err is generic error and
    * Empty payload response is sent back */
    else
    {
      QCRIL_LOG_INFO("qmi error response");
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(qmi_resp->resp.error);
      break;
    }
    /* Send response with success or error cause and config item in params */
    qcril_qmi_imss_get_ims_config_log_and_send_response( params_ptr->t,
                                                         &config_params,
                                                         radio_config_error,
                                                         ((error_cause_code_present) ?
                                                         qcril_qmi_radio_config_map_qmi_settings_resp_to_radio_config_settings_resp(qmi_resp->settings_resp) :
                                                         QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR) );
  }while(FALSE);

  /* If ril_err is GENERIC_FAILURE and also if error cause code response
   * is not sent, send an empty payload response back */
  if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS && !error_cause_code_present && params_ptr)
  {
    QCRIL_LOG_INFO("Sending Error response..");
    qcril_qmi_imss_get_ims_config_log_and_send_response( params_ptr->t,
                                                         NULL,
                                                         radio_config_error,
                                                         QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR);
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(0);
  return QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
}// qcril_qmi_radio_config_imss_get_emer_dynamic_config_resp_handler

//===========================================================================
// qcril_qmi_radio_config_imss_get_video_dynamic_config_resp_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_video_dynamic_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_request_params_type *params_ptr = NULL;

  ims_settings_get_video_dynamic_config_rsp_msg_v01 *qmi_resp = NULL;
  qcril_qmi_radio_config_error_type radio_config_error =
    QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;

  qcril_qmi_radio_config_params_type config_params;
  memset(&config_params, 0, sizeof(config_params));

  /* To check if error cause code is sent in FAILURE case */
  bool error_cause_code_present = FALSE;

  //place holder for int value
  uint32_t int_value = 0;

  do
  {
    /* Validate config ptr */
    if((config_ptr == NULL) ||
        (config_ptr->extra_data == NULL) ||
        (config_ptr->extra_data_len <=0))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }
    /* Create a basic config_params structure
       which will be used to send the response*/
    config_params.config_item = config_ptr->config_item;
    config_params.config_item_value_type = qcril_qmi_radio_config_get_item_value_type(
        config_params.config_item);

    /* Read request_params_type */
    params_ptr = (qcril_request_params_type *)config_ptr->extra_data;
    if(params_ptr->data == NULL)
    {
      QCRIL_LOG_ERROR("Params data is null..");
      break;
    }

    /* Get the qmi response structure from req_params_type */
    qmi_resp = (ims_settings_get_video_dynamic_config_rsp_msg_v01 *)params_ptr->data;

    /* If the qmi response is success, then process the data */
    if(qmi_resp->resp.result == QMI_RESULT_SUCCESS_V01)
    {
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
      //Check for the item requested by the request and initialize it to the value to be sent back
      switch(config_ptr->config_item)
      {
        case QCRIL_QMI_RADIO_CONFIG_QIPCALL_VIDEO_QUALITY:
          if(qmi_resp->vt_quality_selector_valid)
          {
            if (qcril_qmi_radio_config_imss_map_imss_vt_quality_to_radio_config_video_quality(
                  qmi_resp->vt_quality_selector,
                  (qcril_qmi_radio_config_imss_video_quality *)&int_value))
            {
              config_params.config_item_value_len = sizeof(int_value);
              config_params.config_item_value = (void*) &int_value;
              QCRIL_LOG_INFO("get: vt_quality_selector value: %d",
                  qmi_resp->vt_quality_selector);
            }
            else
            {
              QCRIL_LOG_INFO("Invalid parameter recevied in vt_quality_selector = %d",
                  qmi_resp->vt_quality_selector);
              radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_PARAMETER;
            }
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        default:
          radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
          QCRIL_LOG_INFO("Item not valid in qmi structure..");
          break;
      }
      if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS)
      {
        break;
      }
    }
    /* otherwise, If the response is specifically ERR_CAUSE_CODE
     * specifically look if settings response is sent by the modem
     * if it is then send the settings resp else empty response
     * is sent */
    else if(qmi_resp->resp.error == QMI_ERR_CAUSE_CODE_V01)
    {
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(
          qmi_resp->resp.error);
      if(qmi_resp->settings_resp_valid)
      {
        //flag to determine if the FailureCause is sent
        error_cause_code_present = TRUE;
      }
      else
      {
        QCRIL_LOG_INFO("qmi error response");
        break;
      }
    }
    /* If not any of the above, ril_err is generic error and
     * Empty payload response is sent back */
    else
    {
      QCRIL_LOG_INFO("qmi error response");
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(
          qmi_resp->resp.error);
      break;
    }
    /* Send response with success or error cause and config item in params */
    qcril_qmi_imss_get_ims_config_log_and_send_response( params_ptr->t,
        &config_params,
        radio_config_error,
        ((error_cause_code_present) ?
         qcril_qmi_radio_config_map_qmi_settings_resp_to_radio_config_settings_resp(
           qmi_resp->settings_resp) :
         QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR) );
  }while(FALSE);

  /* If ril_err is GENERIC_FAILURE and also if error cause code response
   * is not sent, send an empty payload response back */
  if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS &&
      !error_cause_code_present && params_ptr)
  {
    QCRIL_LOG_INFO("Sending Error response..");
    qcril_qmi_imss_get_ims_config_log_and_send_response( params_ptr->t,
        NULL,
        radio_config_error,
        QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR);
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(0);
  return QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
}// qcril_qmi_radio_config_imss_get_video_dynamic_config_resp_handler

//===========================================================================
// qcril_qmi_radio_config_imss_get_rcs_new_config_resp_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_rcs_new_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_request_params_type *params_ptr = NULL;

  ims_settings_get_rcs_new_config_rsp_msg_v01 *qmi_resp = NULL;
  qcril_qmi_radio_config_error_type radio_config_error =
          QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;

  qcril_qmi_radio_config_params_type config_params;
  memset(&config_params, 0, sizeof(config_params));

  /* To check if error cause code is sent in FAILURE case */
  bool error_cause_code_present = FALSE;

  //place holder for copying values.
  int int_value = 0;

  do
  {
    /* Validate config ptr */
    if((config_ptr == NULL) ||
       (config_ptr->extra_data == NULL) ||
       (config_ptr->extra_data_len <=0))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }
    /* Create a basic config_params structure
       which will be used to send the response*/
    config_params.config_item = config_ptr->config_item;
    config_params.config_item_value_type = qcril_qmi_radio_config_get_item_value_type(
            config_params.config_item);

    /* Read request_params_type */
    params_ptr = (qcril_request_params_type *)config_ptr->extra_data;
    if(params_ptr->data == NULL)
    {
      QCRIL_LOG_ERROR("Params data is null..");
      break;
    }

    /* Get the qmi response structure from req_params_type */
    qmi_resp = (ims_settings_get_rcs_new_config_rsp_msg_v01 *)params_ptr->data;

    /* If the qmi response is success, then process the data */
    if(qmi_resp->resp.result == QMI_RESULT_SUCCESS_V01)
    {
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
      //Check for the item requested by the request and initialize it to the value to be sent back
      switch(config_ptr->config_item)
      {
        case QCRIL_QMI_RADIO_CONFIG_RCS_SMS_APP:
          if( (qmi_resp->default_sms_app_valid) )
          {
            int_value = qmi_resp->default_sms_app;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get default sms app value: %d",
                            int_value);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        case QCRIL_QMI_RADIO_CONFIG_RCS_VVM_APP:
          if( (qmi_resp->default_vvm_app_valid) )
          {
            int_value = qmi_resp->default_vvm_app;
            config_params.config_item_value_len = sizeof(int_value);
            config_params.config_item_value = (void*) &int_value;
            QCRIL_LOG_INFO("get default vvm app value: %d",
                            int_value);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        default:
          radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
          QCRIL_LOG_INFO("Item not valid in qmi structure..");
          break;
      }
      if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS)
      {
        break;
      }
    }
    /* otherwise, If the response is specifically ERR_CAUSE_CODE
     * specifically look if settings response is sent by the modem
     * if it is then send the settings resp else empty response
     * is sent */
    else if(qmi_resp->resp.error == QMI_ERR_CAUSE_CODE_V01)
    {
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(
              qmi_resp->resp.error);
      if(qmi_resp->settings_resp_valid)
      {
        //flag to determine if the FailureCause is sent
        error_cause_code_present = TRUE;
      }
      else
      {
        QCRIL_LOG_INFO("qmi error response");
        break;
      }
    }
    /* If not any of the above, ril_err is generic error and
    * Empty payload response is sent back */
    else
    {
      QCRIL_LOG_INFO("qmi error response");
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(
              qmi_resp->resp.error);
      break;
    }
    /* Send response with success or error cause and config item in params */
    qcril_qmi_imss_get_ims_config_log_and_send_response( params_ptr->t,
            &config_params,
            radio_config_error,
            ((error_cause_code_present) ?
            qcril_qmi_radio_config_map_qmi_settings_resp_to_radio_config_settings_resp(
                    qmi_resp->settings_resp) : QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR) );
  }while(FALSE);

  /* If ril_err is GENERIC_FAILURE and also if error cause code response
   * is not sent, send an empty payload response back */
  if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS && !error_cause_code_present &&
          params_ptr)
  {
    QCRIL_LOG_INFO("Sending Error response..");
    qcril_qmi_imss_get_ims_config_log_and_send_response( params_ptr->t,
            NULL,
            radio_config_error,
            QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR);
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(0);
  return radio_config_error;
}// qcril_qmi_radio_config_imss_get_rcs_new_config_resp_handler

//===========================================================================
// qcril_qmi_radio_config_imss_get_ims_new_config_resp_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_get_ims_new_config_resp_handler
(
 const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_request_params_type *params_ptr = NULL;

  ims_settings_get_ims_new_config_rsp_msg_v01 *qmi_resp = NULL;
  qcril_qmi_radio_config_error_type radio_config_error =
             QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;

  qcril_qmi_radio_config_params_type config_params;
  memset(&config_params, 0, sizeof(config_params));

  /* To check if error cause code is sent in FAILURE case */
  bool error_cause_code_present = FALSE;

  //placeholder to copy string value, to avoid malloc
  char string_value[IMS_SETTINGS_IMS_USER_AGENT_LEN_V01];
  memset(&string_value, 0, sizeof(string_value));

  do
  {
    /* Validate config ptr */
    if((config_ptr == NULL) ||
        (config_ptr->extra_data == NULL) ||
        (config_ptr->extra_data_len <=0))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }
    /* Create a basic config_params structure
       which will be used to send the response*/
    config_params.config_item = config_ptr->config_item;
    config_params.config_item_value_type =
      qcril_qmi_radio_config_get_item_value_type(config_params.config_item);

    /* Read request_params_type */
    params_ptr = (qcril_request_params_type *)config_ptr->extra_data;
    if(params_ptr->data == NULL)
    {
      QCRIL_LOG_ERROR("Params data is null..");
      break;
    }

    /* Get the qmi response structure from req_params_type */
    qmi_resp = (ims_settings_get_ims_new_config_rsp_msg_v01 *)params_ptr->data;

    /* If the qmi response is success, then process the data */
    if(qmi_resp->resp.result == QMI_RESULT_SUCCESS_V01)
    {
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
      //Check for the item requested by the request and initialize it to the value to be sent back
      switch(config_ptr->config_item)
      {
        case QCRIL_QMI_RADIO_CONFIG_USER_IMS_DOMAIN_NAME:
          if(qmi_resp->ims_user_agent_valid)
          {
            config_params.config_item_value_len = strlen(qmi_resp->ims_user_agent);
            strlcpy(string_value, (const char *)qmi_resp->ims_user_agent,
                IMS_SETTINGS_IMS_USER_AGENT_LEN_V01);
            config_params.config_item_value = (void*)&string_value;
            QCRIL_LOG_INFO("get USER ims_domain value: %s", string_value);
          }
          else
          {
            QCRIL_LOG_INFO("Did not get the value requested in qmi message");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_ITEM_NOT_PRESENT;
          }
          break;

        default:
          radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
          QCRIL_LOG_INFO("Item not valid in qmi structure..");
          break;
      }
      if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS)
      {
        break;
      }
    }
    /* otherwise, If the response is specifically ERR_CAUSE_CODE
     * specifically look if settings response is sent by the modem
     * if it is then send the settings resp else empty response
     * is sent */
    else if(qmi_resp->resp.error == QMI_ERR_CAUSE_CODE_V01)
    {
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(
                                   qmi_resp->resp.error);
      if(qmi_resp->settings_resp_valid)
      {
        //flag to determine if the FailureCause is sent
        error_cause_code_present = TRUE;
      }
      else
      {
        QCRIL_LOG_INFO("qmi error response");
        break;
      }
    }
    /* If not any of the above, ril_err is generic error and
     * Empty payload response is sent back */
    else
    {
      QCRIL_LOG_INFO("qmi error response");
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(
                                   qmi_resp->resp.error);
      break;
    }
    /* Send response with success or error cause and config item in params */
    qcril_qmi_imss_get_ims_config_log_and_send_response(params_ptr->t,
        &config_params,
        radio_config_error,
        ((error_cause_code_present) ?
         qcril_qmi_radio_config_map_qmi_settings_resp_to_radio_config_settings_resp(
           qmi_resp->settings_resp) :
         QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR));
  }while(FALSE);

  /* If ril_err is GENERIC_FAILURE and also if error cause code response
   * is not sent, send an empty payload response back */
  if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS &&
      !error_cause_code_present && params_ptr)
  {
    QCRIL_LOG_INFO("Sending Error response..");
    qcril_qmi_imss_get_ims_config_log_and_send_response( params_ptr->t,
        NULL,
        radio_config_error,
        QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR);
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(0);

  return QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
}// qcril_qmi_radio_config_imss_get_ims_new_config_resp_handler

/*===========================================================================
SET RESPONSE HANDLERS
===========================================================================*/

//===========================================================================
// qcril_qmi_radio_config_imss_set_codec_dynamic_config_resp_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_codec_dynamic_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_request_params_type *params_ptr = NULL;

  ims_settings_set_codec_dynamic_config_rsp_msg_v01 *qmi_resp = NULL;
  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;

  qcril_qmi_radio_config_params_type config_params;
  memset(&config_params, 0, sizeof(config_params));

  /* To check if error cause code is sent in FAILURE case */
  bool error_cause_code_present = FALSE;

  do
  {
    /* Validate config ptr */
    if((config_ptr == NULL) ||
       (config_ptr->extra_data == NULL) ||
       (config_ptr->extra_data_len <=0))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }
    /* Create a basic config_params structure
       which will be used to send the response*/
    config_params.config_item = config_ptr->config_item;

    /* Read request_params_type */
    params_ptr = (qcril_request_params_type *)config_ptr->extra_data;

    if(params_ptr->data == NULL)
    {
      QCRIL_LOG_ERROR("Params data is null..");
      break;
    }
    /* Get the qmi response structure from req_params_type */
    qmi_resp = (ims_settings_set_codec_dynamic_config_rsp_msg_v01 *) params_ptr->data;

    /* If the qmi response is success, then send success */
    if(qmi_resp->resp.result == QMI_RESULT_SUCCESS_V01)
    {
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
      QCRIL_LOG_INFO("response success");
    }
    /* otherwise, If the response is specifically ERR_CAUSE_CODE
     * specifically look if settings response is sent by the modem
     * if it is then send the settings resp else empty response
     * is sent */
    else if(qmi_resp->resp.error == QMI_ERR_CAUSE_CODE_V01)
    {
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(qmi_resp->resp.error);
      if(qmi_resp->settings_resp_valid)
      {
        //flag to determine if the FailureCause is sent
        error_cause_code_present = TRUE;
      }
      else
      {
        QCRIL_LOG_INFO("qmi error response");
        break;
      }
    }
    /* If not any of the above,
    * Empty payload response is sent back */
    else
    {
      QCRIL_LOG_INFO("qmi error response");
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(qmi_resp->resp.error);
      break;
    }

    /* Send response with success or error cause and config item in params */
    qcril_qmi_imss_set_ims_config_log_and_send_response( params_ptr->t,
                                                         &config_params,
                                                         radio_config_error,
                                                         ((error_cause_code_present) ?
                                                         qcril_qmi_radio_config_map_qmi_settings_resp_to_radio_config_settings_resp(qmi_resp->settings_resp) :
                                                         QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR) );
  }while(FALSE);

  /* If ril_err is GENERIC_FAILURE and also if error cause code response
   * is not sent, send an empty payload response back */
  if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS && !error_cause_code_present && params_ptr)
  {
    QCRIL_LOG_INFO("Sending Error response..");
    qcril_qmi_imss_set_ims_config_log_and_send_response( params_ptr->t,
                                                         NULL,
                                                         radio_config_error,
                                                         QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR);
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(0);
  return QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
}// qcril_qmi_radio_config_imss_set_codec_dynamic_config_resp_handler

//===========================================================================
//qcril_qmi_radio_config_imss_set_voice_dynamic_config_resp_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_voice_dynamic_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_request_params_type *params_ptr = NULL;

  ims_settings_set_voice_dynamic_config_rsp_msg_v01 *qmi_resp = NULL;
  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;

  qcril_qmi_radio_config_params_type config_params;
  memset(&config_params, 0, sizeof(config_params));

  /* To check if error cause code is sent in FAILURE case */
  bool error_cause_code_present = FALSE;

  do
  {
    /* Validate config ptr */
    if((config_ptr == NULL) ||
       (config_ptr->extra_data == NULL) ||
       (config_ptr->extra_data_len <=0))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }
    /* Create a basic config_params structure
       which will be used to send the response*/
    config_params.config_item = config_ptr->config_item;

    /* Read request_params_type */
    params_ptr = (qcril_request_params_type *)config_ptr->extra_data;

    if(params_ptr->data == NULL)
    {
      QCRIL_LOG_ERROR("Params data is null..");
      break;
    }
    /* Get the qmi response structure from req_params_type */
    qmi_resp = (ims_settings_set_voice_dynamic_config_rsp_msg_v01 *) params_ptr->data;

    /* If the qmi response is success, then send success */
    if(qmi_resp->resp.result == QMI_RESULT_SUCCESS_V01)
    {
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
      QCRIL_LOG_INFO("response success");
    }
    /* otherwise, If the response is specifically ERR_CAUSE_CODE
     * specifically look if settings response is sent by the modem
     * if it is then send the settings resp else empty response
     * is sent */
    else if(qmi_resp->resp.error == QMI_ERR_CAUSE_CODE_V01)
    {
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(qmi_resp->resp.error);
      if(qmi_resp->settings_resp_valid)
      {
        //flag to determine if the FailureCause is sent
        error_cause_code_present = TRUE;
      }
      else
      {
        QCRIL_LOG_INFO("qmi error response");
        break;
      }
    }
    /* If not any of the above,
    * Empty payload response is sent back */
    else
    {
      QCRIL_LOG_INFO("qmi error response");
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(qmi_resp->resp.error);
      break;
    }

    /* Send response with success or error cause and config item in params */
    qcril_qmi_imss_set_ims_config_log_and_send_response( params_ptr->t,
                                                         &config_params,
                                                         radio_config_error,
                                                         ((error_cause_code_present) ?
                                                         qcril_qmi_radio_config_map_qmi_settings_resp_to_radio_config_settings_resp(qmi_resp->settings_resp) :
                                                         QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR) );
  }while(FALSE);

  /* If ril_err is GENERIC_FAILURE and also if error cause code response
   * is not sent, send an empty payload response back */
  if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS && !error_cause_code_present && params_ptr)
  {
    QCRIL_LOG_INFO("Sending Error response..");
    qcril_qmi_imss_set_ims_config_log_and_send_response( params_ptr->t,
                                                         NULL,
                                                         radio_config_error,
                                                         QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR);
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(0);
  return QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
}// qcril_qmi_radio_config_imss_set_voice_dynamic_config_resp_handler

//===========================================================================
// qcril_qmi_radio_config_imss_set_sip_new_config_resp_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_sip_new_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_request_params_type *params_ptr = NULL;

  ims_settings_set_sip_new_config_rsp_msg_v01 *qmi_resp = NULL;
  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;

  qcril_qmi_radio_config_params_type config_params;
  memset(&config_params, 0, sizeof(config_params));

  /* To check if error cause code is sent in FAILURE case */
  bool error_cause_code_present = FALSE;

  do
  {
    /* Validate config ptr */
    if((config_ptr == NULL) ||
       (config_ptr->extra_data == NULL) ||
       (config_ptr->extra_data_len <=0))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }
    /* Create a basic config_params structure
       which will be used to send the response*/
    config_params.config_item = config_ptr->config_item;

    /* Read request_params_type */
    params_ptr = (qcril_request_params_type *)config_ptr->extra_data;

    if(params_ptr->data == NULL)
    {
      QCRIL_LOG_ERROR("Params data is null..");
      break;
    }
    /* Get the qmi response structure from req_params_type */
    qmi_resp = (ims_settings_set_sip_new_config_rsp_msg_v01 *) params_ptr->data;

    /* If the qmi response is success, then send success */
    if(qmi_resp->resp.result == QMI_RESULT_SUCCESS_V01)
    {
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
      QCRIL_LOG_INFO("response success");
    }
    /* otherwise, If the response is specifically ERR_CAUSE_CODE
     * specifically look if settings response is sent by the modem
     * if it is then send the settings resp else empty response
     * is sent */
    else if(qmi_resp->resp.error == QMI_ERR_CAUSE_CODE_V01)
    {
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(qmi_resp->resp.error);
      if(qmi_resp->settings_resp_valid)
      {
        //flag to determine if the FailureCause is sent
        error_cause_code_present = TRUE;
      }
      else
      {
        QCRIL_LOG_INFO("qmi error response");
        break;
      }
    }
    /* If not any of the above,
    * Empty payload response is sent back */
    else
    {
      QCRIL_LOG_INFO("qmi error response");
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(qmi_resp->resp.error);
      break;
    }

    /* Send response with success or error cause and config item in params */
    qcril_qmi_imss_set_ims_config_log_and_send_response( params_ptr->t,
                                                         &config_params,
                                                         radio_config_error,
                                                         ((error_cause_code_present) ?
                                                         qcril_qmi_radio_config_map_qmi_settings_resp_to_radio_config_settings_resp(qmi_resp->settings_resp) :
                                                         QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR) );
  }while(FALSE);

  /* If ril_err is GENERIC_FAILURE and also if error cause code response
   * is not sent, send an empty payload response back */
  if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS && !error_cause_code_present && params_ptr)
  {
    QCRIL_LOG_INFO("Sending Error response..");
    qcril_qmi_imss_set_ims_config_log_and_send_response( params_ptr->t,
                                                         NULL,
                                                         radio_config_error,
                                                         QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR);
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(0);
  return QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
}// qcril_qmi_radio_config_imss_set_sip_new_config_resp_handler

//===========================================================================
// qcril_qmi_radio_config_imss_set_reg_mgr_new_config_resp_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_reg_mgr_new_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_request_params_type *params_ptr = NULL;

  ims_settings_set_reg_mgr_new_config_rsp_msg_v01 *qmi_resp = NULL;
  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;

  qcril_qmi_radio_config_params_type config_params;
  memset(&config_params, 0, sizeof(config_params));

  /* To check if error cause code is sent in FAILURE case */
  bool error_cause_code_present = FALSE;

  do
  {
    /* Validate config ptr */
    if((config_ptr == NULL) ||
       (config_ptr->extra_data == NULL) ||
       (config_ptr->extra_data_len <=0))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }
    /* Create a basic config_params structure
       which will be used to send the response*/
    config_params.config_item = config_ptr->config_item;

    /* Read request_params_type */
    params_ptr = (qcril_request_params_type *)config_ptr->extra_data;

    if(params_ptr->data == NULL)
    {
      QCRIL_LOG_ERROR("Params data is null..");
      break;
    }
    /* Get the qmi response structure from req_params_type */
    qmi_resp = (ims_settings_set_reg_mgr_new_config_rsp_msg_v01 *) params_ptr->data;

    /* If the qmi response is success, then send success */
    if(qmi_resp->resp.result == QMI_RESULT_SUCCESS_V01)
    {
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
      QCRIL_LOG_INFO("response success");
    }
    /* otherwise, If the response is specifically ERR_CAUSE_CODE
     * specifically look if settings response is sent by the modem
     * if it is then send the settings resp else empty response
     * is sent */
    else if(qmi_resp->resp.error == QMI_ERR_CAUSE_CODE_V01)
    {
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(qmi_resp->resp.error);
      if(qmi_resp->settings_resp_valid)
      {
        //flag to determine if the FailureCause is sent
        error_cause_code_present = TRUE;
      }
      else
      {
        QCRIL_LOG_INFO("qmi error response");
        break;
      }
    }
    /* If not any of the above,
    * Empty payload response is sent back */
    else
    {
      QCRIL_LOG_INFO("qmi error response");
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(qmi_resp->resp.error);
      break;
    }

    /* Send response with success or error cause and config item in params */
    qcril_qmi_imss_set_ims_config_log_and_send_response( params_ptr->t,
                                                         &config_params,
                                                         radio_config_error,
                                                         ((error_cause_code_present) ?
                                                         qcril_qmi_radio_config_map_qmi_settings_resp_to_radio_config_settings_resp(qmi_resp->settings_resp) :
                                                         QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR) );
  }while(FALSE);

  /* If ril_err is GENERIC_FAILURE and also if error cause code response
   * is not sent, send an empty payload response back */
  if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS && !error_cause_code_present && params_ptr)
  {
    QCRIL_LOG_INFO("Sending Error response..");
    qcril_qmi_imss_set_ims_config_log_and_send_response( params_ptr->t,
                                                         NULL,
                                                         radio_config_error,
                                                         QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR);
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(0);
  return QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
}// qcril_qmi_radio_config_imss_set_reg_mgr_new_config_resp_handler

//===========================================================================
// qcril_qmi_radio_config_imss_set_client_provisioning_config_resp_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_ims_service_enable_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_request_params_type *params_ptr = NULL;

  ims_settings_set_ims_service_enable_config_rsp_msg_v01 *qmi_resp = NULL;
  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;

  qcril_qmi_radio_config_params_type config_params;
  memset(&config_params, 0, sizeof(config_params));

  /* To check if error cause code is sent in FAILURE case */
  bool error_cause_code_present = FALSE;

  do
  {
    /* Validate config ptr */
    if((config_ptr == NULL) ||
       (config_ptr->extra_data == NULL) ||
       (config_ptr->extra_data_len <=0))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }
    /* Create a basic config_params structure
       which will be used to send the response*/
    config_params.config_item = config_ptr->config_item;

    /* Read request_params_type */
    params_ptr = (qcril_request_params_type *)config_ptr->extra_data;

    if(params_ptr->data == NULL)
    {
      QCRIL_LOG_ERROR("Params data is null..");
      break;
    }
    /* Get the qmi response structure from req_params_type */
    qmi_resp = (ims_settings_set_ims_service_enable_config_rsp_msg_v01 *) params_ptr->data;

    /* If the qmi response is success, then send success */
    if(qmi_resp->resp.result == QMI_RESULT_SUCCESS_V01)
    {
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
      QCRIL_LOG_INFO("response success");
    }
    /* otherwise, If the response is specifically ERR_CAUSE_CODE
     * specifically look if settings response is sent by the modem
     * if it is then send the settings resp else empty response
     * is sent */
    else if(qmi_resp->resp.error == QMI_ERR_CAUSE_CODE_V01)
    {
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(qmi_resp->resp.error);
      if(qmi_resp->settings_resp_valid)
      {
        //flag to determine if the FailureCause is sent
        error_cause_code_present = TRUE;
      }
      else
      {
        QCRIL_LOG_INFO("qmi error response");
        break;
      }
    }
    /* If not any of the above,
    * Empty payload response is sent back */
    else
    {
      QCRIL_LOG_INFO("qmi error response");
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(qmi_resp->resp.error);
      break;
    }

    if (config_params.config_item ==
            QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_WIFI_CALL_PREFERENCE)
    {
      qcril_qmi_imss_set_wifi_pref_to_qcril_data(params_ptr);
    }


    /* Send response with success or error cause and config item in params */
    qcril_qmi_imss_set_ims_config_log_and_send_response( params_ptr->t,
                                                         &config_params,
                                                         radio_config_error,
                                                         ((error_cause_code_present) ?
                                                         qcril_qmi_radio_config_map_qmi_settings_resp_to_radio_config_settings_resp(qmi_resp->settings_resp) :
                                                         QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR) );
  }while(FALSE);

  /* If ril_err is GENERIC_FAILURE and also if error cause code response
   * is not sent, send an empty payload response back */
  if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS && !error_cause_code_present && params_ptr)
  {
    QCRIL_LOG_INFO("Sending Error response..");
    qcril_qmi_imss_set_ims_config_log_and_send_response( params_ptr->t,
                                                         NULL,
                                                         radio_config_error,
                                                         QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR);
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(0);
  return QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
}// qcril_qmi_radio_config_imss_set_ims_service_enable_config_resp_handler

//===========================================================================
// qcril_qmi_radio_config_imss_set_sms_new_config_resp_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_sms_new_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_request_params_type *params_ptr = NULL;

  ims_settings_set_sms_new_config_rsp_msg_v01 *qmi_resp = NULL;
  qcril_qmi_radio_config_error_type radio_config_error =
    QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;

  qcril_qmi_radio_config_params_type config_params;
  memset(&config_params, 0, sizeof(config_params));

  /* To check if error cause code is sent in FAILURE case */
  bool error_cause_code_present = FALSE;

  do
  {
    /* Validate config ptr */
    if((config_ptr == NULL) ||
       (config_ptr->extra_data == NULL) ||
       (config_ptr->extra_data_len <=0))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }
    /* Create a basic config_params structure
       which will be used to send the response*/
    config_params.config_item = config_ptr->config_item;

    /* Read request_params_type */
    params_ptr = (qcril_request_params_type *)config_ptr->extra_data;

    if(params_ptr->data == NULL)
    {
      QCRIL_LOG_ERROR("Params data is null..");
      break;
    }
    /* Get the qmi response structure from req_params_type */
    qmi_resp = (ims_settings_set_sms_new_config_rsp_msg_v01 *) params_ptr->data;

    /* If the qmi response is success, then send success */
    if(qmi_resp->resp.result == QMI_RESULT_SUCCESS_V01)
    {
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
      QCRIL_LOG_INFO("response success");
    }
    /* otherwise, If the response is specifically ERR_CAUSE_CODE
     * specifically look if settings response is sent by the modem
     * if it is then send the settings resp else empty response
     * is sent */
    else if(qmi_resp->resp.error == QMI_ERR_CAUSE_CODE_V01)
    {
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(
        qmi_resp->resp.error);
      if(qmi_resp->settings_resp_valid)
      {
        //flag to determine if the FailureCause is sent
        error_cause_code_present = TRUE;
      }
      else
      {
        QCRIL_LOG_INFO("qmi error response");
        break;
      }
    }
    /* If not any of the above,
    * Empty payload response is sent back */
    else
    {
      QCRIL_LOG_INFO("qmi error response");
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(
        qmi_resp->resp.error);
      break;
    }

    /* Send response with success or error cause and config item in params */
    qcril_qmi_imss_set_ims_config_log_and_send_response( params_ptr->t,
      &config_params,
      radio_config_error,
      ((error_cause_code_present) ?
      qcril_qmi_radio_config_map_qmi_settings_resp_to_radio_config_settings_resp(
        qmi_resp->settings_resp) :
      QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR) );
  }while(FALSE);

  /* If ril_err is GENERIC_FAILURE and also if error cause code response
   * is not sent, send an empty payload response back */
  if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS &&
    !error_cause_code_present && params_ptr)
  {
    QCRIL_LOG_INFO("Sending Error response..");
    qcril_qmi_imss_set_ims_config_log_and_send_response( params_ptr->t,
      NULL,
      radio_config_error,
      QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR);
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(0);
  return QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
}// qcril_qmi_radio_config_imss_set_sms_new_config_resp_handler

//===========================================================================
// qcril_qmi_radio_config_imss_set_presence_config_resp_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_presence_new_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_request_params_type *params_ptr = NULL;

  ims_settings_set_presence_new_config_rsp_msg_v01 *qmi_resp = NULL;
  qcril_qmi_radio_config_error_type radio_config_error =
    QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;

  qcril_qmi_radio_config_params_type config_params;
  memset(&config_params, 0, sizeof(config_params));

  /* To check if error cause code is sent in FAILURE case */
  bool error_cause_code_present = FALSE;

  do
  {
    /* Validate config ptr */
    if((config_ptr == NULL) ||
       (config_ptr->extra_data == NULL) ||
       (config_ptr->extra_data_len <=0))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }
    /* Create a basic config_params structure
       which will be used to send the response*/
    config_params.config_item = config_ptr->config_item;

    /* Read request_params_type */
    params_ptr = (qcril_request_params_type *)config_ptr->extra_data;

    if(params_ptr->data == NULL)
    {
      QCRIL_LOG_ERROR("Params data is null..");
      break;
    }
    /* Get the qmi response structure from req_params_type */
    qmi_resp = (ims_settings_set_presence_new_config_rsp_msg_v01 *) params_ptr->data;

    /* If the qmi response is success, then send success */
    if(qmi_resp->resp.result == QMI_RESULT_SUCCESS_V01)
    {
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
      QCRIL_LOG_INFO("response success");
    }
    /* otherwise, If the response is specifically ERR_CAUSE_CODE
     * specifically look if settings response is sent by the modem
     * if it is then send the settings resp else empty response
     * is sent */
    else if(qmi_resp->resp.error == QMI_ERR_CAUSE_CODE_V01)
    {
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(
        qmi_resp->resp.error);
      if(qmi_resp->settings_resp_valid)
      {
        //flag to determine if the FailureCause is present
        error_cause_code_present = TRUE;
      }
      else
      {
        QCRIL_LOG_INFO("qmi error response");
        break;
      }
    }
    /* If not any of the above,
    * Empty payload response is sent back */
    else
    {
      QCRIL_LOG_INFO("qmi error response");
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(
        qmi_resp->resp.error);
      break;
    }

    /* Send response with success or error cause and config item in params */
    qcril_qmi_imss_set_ims_config_log_and_send_response( params_ptr->t,
      &config_params,
      radio_config_error,
      ((error_cause_code_present) ?
      qcril_qmi_radio_config_map_qmi_settings_resp_to_radio_config_settings_resp(
        qmi_resp->settings_resp) :
      QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR) );
  }while(FALSE);

  /* If ril_err is GENERIC_FAILURE and also if error cause code response
   * is not sent, send an empty payload response back */
  if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS &&
    !error_cause_code_present && params_ptr)
  {
    QCRIL_LOG_INFO("Sending Error response..");
    qcril_qmi_imss_set_ims_config_log_and_send_response( params_ptr->t,
      NULL,
      radio_config_error,
      QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR);
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(0);
  return QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
}// qcril_qmi_radio_config_imss_set_presence_new_config_resp_handler

//===========================================================================
// qcril_qmi_radio_config_imss_set_rtp_dynamic_config_resp_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_rtp_dynamic_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_request_params_type *params_ptr = NULL;

  ims_settings_set_rtp_dynamic_config_rsp_msg_v01 *qmi_resp = NULL;
  qcril_qmi_radio_config_error_type radio_config_error =
    QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;

  qcril_qmi_radio_config_params_type config_params;
  memset(&config_params, 0, sizeof(config_params));

  /* To check if error cause code is sent in FAILURE case */
  bool error_cause_code_present = FALSE;

  do
  {
    /* Validate config ptr */
    if((config_ptr == NULL) ||
       (config_ptr->extra_data == NULL) ||
       (config_ptr->extra_data_len <=0))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }
    /* Create a basic config_params structure
       which will be used to send the response*/
    config_params.config_item = config_ptr->config_item;

    /* Read request_params_type */
    params_ptr = (qcril_request_params_type *)config_ptr->extra_data;

    if(params_ptr->data == NULL)
    {
      QCRIL_LOG_ERROR("Params data is null..");
      break;
    }
    /* Get the qmi response structure from req_params_type */
    qmi_resp = (ims_settings_set_rtp_dynamic_config_rsp_msg_v01 *) params_ptr->data;

    /* If the qmi response is success, then send success */
    if(qmi_resp->resp.result == QMI_RESULT_SUCCESS_V01)
    {
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
      QCRIL_LOG_INFO("response success");
    }
    /* otherwise, If the response is specifically ERR_CAUSE_CODE
     * specifically look if settings response is sent by the modem
     * if it is then send the settings resp else empty response
     * is sent */
    else if(qmi_resp->resp.error == QMI_ERR_CAUSE_CODE_V01)
    {
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(
        qmi_resp->resp.error);
      if(qmi_resp->settings_resp_valid)
      {
        //flag to determine if the FailureCause is sent
        error_cause_code_present = TRUE;
      }
      else
      {
        QCRIL_LOG_INFO("qmi error response");
        break;
      }
    }
    /* If not any of the above,
    * Empty payload response is sent back */
    else
    {
      QCRIL_LOG_INFO("qmi error response");
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(
        qmi_resp->resp.error);
      break;
    }

    /* Send response with success or error cause and config item in params */
    qcril_qmi_imss_set_ims_config_log_and_send_response( params_ptr->t,
      &config_params,
      radio_config_error,
      ((error_cause_code_present) ?
      qcril_qmi_radio_config_map_qmi_settings_resp_to_radio_config_settings_resp(
        qmi_resp->settings_resp) :
      QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR) );
  }while(FALSE);

  /* If ril_err is GENERIC_FAILURE and also if error cause code response
   * is not sent, send an empty payload response back */
  if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS &&
    !error_cause_code_present && params_ptr)
  {
    QCRIL_LOG_INFO("Sending Error response..");
    qcril_qmi_imss_set_ims_config_log_and_send_response( params_ptr->t,
      NULL,
      radio_config_error,
      QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR);
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(0);
  return QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
}// qcril_qmi_radio_config_imss_set_rtp_dynamic_config_resp_handler

//===========================================================================
// qcril_qmi_radio_config_imss_set_emer_dynamic_config_resp_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_emer_dynamic_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_request_params_type *params_ptr = NULL;

  ims_settings_set_emer_dynamic_config_rsp_msg_v01 *qmi_resp = NULL;
  qcril_qmi_radio_config_error_type radio_config_error =
    QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;

  qcril_qmi_radio_config_params_type config_params;
  memset(&config_params, 0, sizeof(config_params));

  /* To check if error cause code is sent in FAILURE case */
  bool error_cause_code_present = FALSE;

  do
  {
    /* Validate config ptr */
    if((config_ptr == NULL) ||
       (config_ptr->extra_data == NULL) ||
       (config_ptr->extra_data_len <=0))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }
    /* Create a basic config_params structure
       which will be used to send the response*/
    config_params.config_item = config_ptr->config_item;

    /* Read request_params_type */
    params_ptr = (qcril_request_params_type *)config_ptr->extra_data;

    if(params_ptr->data == NULL)
    {
      QCRIL_LOG_ERROR("Params data is null..");
      break;
    }
    /* Get the qmi response structure from req_params_type */
    qmi_resp = (ims_settings_set_emer_dynamic_config_rsp_msg_v01 *) params_ptr->data;

    /* If the qmi response is success, then send success */
    if(qmi_resp->resp.result == QMI_RESULT_SUCCESS_V01)
    {
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
      QCRIL_LOG_INFO("response success");
    }
    /* otherwise, If the response is specifically ERR_CAUSE_CODE
     * specifically look if settings response is sent by the modem
     * if it is then send the settings resp else empty response
     * is sent */
    else if(qmi_resp->resp.error == QMI_ERR_CAUSE_CODE_V01)
    {
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(
        qmi_resp->resp.error);
      if(qmi_resp->settings_resp_valid)
      {
        //flag to determine if the FailureCause is sent
        error_cause_code_present = TRUE;
      }
      else
      {
        QCRIL_LOG_INFO("qmi error response");
        break;
      }
    }
    /* If not any of the above,
    * Empty payload response is sent back */
    else
    {
      QCRIL_LOG_INFO("qmi error response");
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(
        qmi_resp->resp.error);
      break;
    }

    /* Send response with success or error cause and config item in params */
    qcril_qmi_imss_set_ims_config_log_and_send_response( params_ptr->t,
      &config_params,
      radio_config_error,
      ((error_cause_code_present) ?
      qcril_qmi_radio_config_map_qmi_settings_resp_to_radio_config_settings_resp(
        qmi_resp->settings_resp) :
      QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR) );
  }while(FALSE);

  /* If ril_err is GENERIC_FAILURE and also if error cause code response
   * is not sent, send an empty payload response back */
  if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS &&
    !error_cause_code_present && params_ptr)
  {
    QCRIL_LOG_INFO("Sending Error response..");
    qcril_qmi_imss_set_ims_config_log_and_send_response( params_ptr->t,
      NULL,
      radio_config_error,
      QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR);
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(0);
  return QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
}// qcril_qmi_radio_config_imss_set_emer_dynamic_config_resp_handler

//===========================================================================
// qcril_qmi_radio_config_imss_set_video_dynamic_config_resp_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_video_dynamic_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_request_params_type *params_ptr = NULL;

  ims_settings_set_video_dynamic_config_rsp_msg_v01 *qmi_resp = NULL;
  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;

  qcril_qmi_radio_config_params_type config_params;
  memset(&config_params, 0, sizeof(config_params));

  /* To check if error cause code is sent in FAILURE case */
  bool error_cause_code_present = FALSE;

  do
  {
    /* Validate config ptr */
    if((config_ptr == NULL) ||
        (config_ptr->extra_data == NULL) ||
        (config_ptr->extra_data_len <=0))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }
    /* Create a basic config_params structure
       which will be used to send the response*/
    config_params.config_item = config_ptr->config_item;

    /* Read request_params_type */
    params_ptr = (qcril_request_params_type *)config_ptr->extra_data;

    if(params_ptr->data == NULL)
    {
      QCRIL_LOG_ERROR("Params data is null..");
      break;
    }
    /* Get the qmi response structure from req_params_type */
    qmi_resp = (ims_settings_set_video_dynamic_config_rsp_msg_v01 *)params_ptr->data;

    /* If the qmi response is success, then send success */
    if(qmi_resp->resp.result == QMI_RESULT_SUCCESS_V01)
    {
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
      QCRIL_LOG_INFO("response success");
    }
    /* otherwise, If the response is specifically ERR_CAUSE_CODE
     * specifically look if settings response is sent by the modem
     * if it is then send the settings resp else empty response
     * is sent */
    else if(qmi_resp->resp.error == QMI_ERR_CAUSE_CODE_V01)
    {
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(
          qmi_resp->resp.error);
      if(qmi_resp->settings_resp_valid)
      {
        //flag to determine if the FailureCause is sent
        error_cause_code_present = TRUE;
      }
      else
      {
        QCRIL_LOG_INFO("qmi error response");
        break;
      }
    }
    /* If not any of the above,
     * Empty payload response is sent back */
    else
    {
      QCRIL_LOG_INFO("qmi error response");
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(
          qmi_resp->resp.error);
      break;
    }

    /* Send response with success or error cause and config item in params */
    qcril_qmi_imss_set_ims_config_log_and_send_response( params_ptr->t,
        &config_params,
        radio_config_error,
        ((error_cause_code_present) ?
         qcril_qmi_radio_config_map_qmi_settings_resp_to_radio_config_settings_resp(
           qmi_resp->settings_resp) :
         QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR) );
  }while(FALSE);

  /* If ril_err is GENERIC_FAILURE and also if error cause code response
   * is not sent, send an empty payload response back */
  if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS &&
      !error_cause_code_present && params_ptr)
  {
    QCRIL_LOG_INFO("Sending Error response..");
    qcril_qmi_imss_set_ims_config_log_and_send_response( params_ptr->t,
        NULL,
        radio_config_error,
        QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR);
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(0);
  return QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
}// qcril_qmi_radio_config_imss_set_video_dynamic_config_resp_handler

//===========================================================================
// qcril_qmi_radio_config_imss_set_rcs_new_config_resp_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_rcs_new_config_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_request_params_type *params_ptr = NULL;

  ims_settings_set_rcs_new_config_rsp_msg_v01 *qmi_resp = NULL;
  qcril_qmi_radio_config_error_type radio_config_error =
          QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;

  qcril_qmi_radio_config_params_type config_params;
  memset(&config_params, 0, sizeof(config_params));

  /* To check if error cause code is sent in FAILURE case */
  bool error_cause_code_present = FALSE;

  do
  {
    /* Validate config ptr */
    if((config_ptr == NULL) ||
       (config_ptr->extra_data == NULL) ||
       (config_ptr->extra_data_len <=0))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }
    /* Create a basic config_params structure
       which will be used to send the response*/
    config_params.config_item = config_ptr->config_item;

    /* Read request_params_type */
    params_ptr = (qcril_request_params_type *)config_ptr->extra_data;

    if(params_ptr->data == NULL)
    {
      QCRIL_LOG_ERROR("Params data is null..");
      break;
    }
    /* Get the qmi response structure from req_params_type */
    qmi_resp = (ims_settings_set_rcs_new_config_rsp_msg_v01 *)params_ptr->data;

    /* If the qmi response is success, then send success */
    if(qmi_resp->resp.result == QMI_RESULT_SUCCESS_V01)
    {
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
      QCRIL_LOG_INFO("response success");
    }
    /* otherwise, If the response is specifically ERR_CAUSE_CODE
     * specifically look if settings response is sent by the modem
     * if it is then send the settings resp else empty response
     * is sent */
    else if(qmi_resp->resp.error == QMI_ERR_CAUSE_CODE_V01)
    {
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(
              qmi_resp->resp.error);
      if(qmi_resp->settings_resp_valid)
      {
        //flag to determine if the FailureCause is sent
        error_cause_code_present = TRUE;
      }
      else
      {
        QCRIL_LOG_INFO("qmi error response");
        break;
      }
    }
    /* If not any of the above,
    * Empty payload response is sent back */
    else
    {
      QCRIL_LOG_INFO("qmi error response");
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(
              qmi_resp->resp.error);
      break;
    }

    /* Send response with success or error cause and config item in params */
    qcril_qmi_imss_set_ims_config_log_and_send_response( params_ptr->t,
            &config_params,
            radio_config_error,
            ((error_cause_code_present) ?
            qcril_qmi_radio_config_map_qmi_settings_resp_to_radio_config_settings_resp(
                    qmi_resp->settings_resp) :
            QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR) );
  }while(FALSE);

  /* If ril_err is GENERIC_FAILURE and also if error cause code response
   * is not sent, send an empty payload response back */
  if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS && !error_cause_code_present &&
          params_ptr)
  {
    QCRIL_LOG_INFO("Sending Error response..");
    qcril_qmi_imss_set_ims_config_log_and_send_response( params_ptr->t,
            NULL,
            radio_config_error,
            QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR);
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(0);
  return radio_config_error;
}// qcril_qmi_radio_config_imss_set_rcs_new_config_resp_handler

//===========================================================================
// qcril_qmi_radio_config_imss_set_ims_new_config_resp_handler
//===========================================================================
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_imss_set_ims_new_config_resp_handler
(
 const qcril_qmi_radio_config_params_type *const config_ptr
)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_request_params_type *params_ptr = NULL;

  ims_settings_set_ims_new_config_rsp_msg_v01 *qmi_resp = NULL;
  qcril_qmi_radio_config_error_type radio_config_error =
            QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;

  qcril_qmi_radio_config_params_type config_params;
  memset(&config_params, 0, sizeof(config_params));

  /* To check if error cause code is sent in FAILURE case */
  bool error_cause_code_present = FALSE;

  do
  {
    /* Validate config ptr */
    if((config_ptr == NULL) ||
        (config_ptr->extra_data == NULL) ||
        (config_ptr->extra_data_len <=0))
    {
      QCRIL_LOG_ERROR("Invalid config params..");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_CONFIG_PARAMS;
      break;
    }
    /* Create a basic config_params structure
       which will be used to send the response*/
    config_params.config_item = config_ptr->config_item;

    /* Read request_params_type */
    params_ptr = (qcril_request_params_type *)config_ptr->extra_data;

    if(params_ptr->data == NULL)
    {
      QCRIL_LOG_ERROR("Params data is null..");
      break;
    }
    /* Get the qmi response structure from req_params_type */
    qmi_resp = (ims_settings_set_ims_new_config_rsp_msg_v01 *)params_ptr->data;

    /* If the qmi response is success, then send success */
    if(qmi_resp->resp.result == QMI_RESULT_SUCCESS_V01)
    {
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
      QCRIL_LOG_INFO("response success");
    }
    /* otherwise, If the response is specifically ERR_CAUSE_CODE
     * specifically look if settings response is sent by the modem
     * if it is then send the settings resp else empty response
     * is sent */
    else if(qmi_resp->resp.error == QMI_ERR_CAUSE_CODE_V01)
    {
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(
                                   qmi_resp->resp.error);
      if(qmi_resp->settings_resp_valid)
      {
        //flag to determine if the FailureCause is sent
        error_cause_code_present = TRUE;
      }
      else
      {
        QCRIL_LOG_INFO("qmi error response");
        break;
      }
    }
    /* If not any of the above,
     * Empty payload response is sent back */
    else
    {
      QCRIL_LOG_INFO("qmi error response");
      radio_config_error = qcril_qmi_radio_config_map_qmi_error_to_radio_config_error(
                                   qmi_resp->resp.error);
      break;
    }

    /* Send response with success or error cause and config item in params */
    qcril_qmi_imss_set_ims_config_log_and_send_response(params_ptr->t, &config_params,
        radio_config_error,
        ((error_cause_code_present) ?
          qcril_qmi_radio_config_map_qmi_settings_resp_to_radio_config_settings_resp(
            qmi_resp->settings_resp) :
          QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR));
  }while(FALSE);

  /* If ril_err is GENERIC_FAILURE and also if error cause code response
   * is not sent, send an empty payload response back */
  if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS &&
      !error_cause_code_present && params_ptr)
  {
    QCRIL_LOG_INFO("Sending Error response..");
    qcril_qmi_imss_set_ims_config_log_and_send_response(params_ptr->t, NULL, radio_config_error,
        QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR);
  }
  QCRIL_LOG_FUNC_RETURN_WITH_RET(0);

  return QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
}// qcril_qmi_radio_config_imss_set_ims_new_config_resp_handler

//===========================================================================
// qcril_qmi_imss_store_wifi_call_mode_preference_to_req_info
//===========================================================================
void qcril_qmi_imss_store_wifi_call_mode_preference_to_req_info
(
 uint16                          req_id,
 ims_settings_call_mode_option_enum_v01 call_mode_preference
)
{
    qcril_qmi_radio_config_req_data_type *config_data = NULL;
    qcril_reqlist_public_type             req_info;
    qcril_instance_id_e_type              instance_id = QCRIL_DEFAULT_INSTANCE_ID;
    qcril_ims_setting_wfc_preference_type *wfc_pref    = NULL;

    QCRIL_LOG_INFO("store wifi_call_preference = %d to req_id %d\n",
            call_mode_preference, req_id);

    if ((qcril_reqlist_query_by_req_id(req_id, &instance_id, &req_info ) == E_SUCCESS) &&
            req_info.req_data)
    {
        wfc_pref = (qcril_ims_setting_wfc_preference_type *)qcril_malloc(
                sizeof(qcril_ims_setting_wfc_preference_type));
        if (wfc_pref)
        {
            *wfc_pref = qcril_qmi_imss_convert_imss_to_ril_wfc_preference_v02(call_mode_preference);
            config_data = (qcril_qmi_radio_config_req_data_type *)req_info.req_data;
            config_data->data_len = sizeof(qcril_ims_setting_wfc_preference_type);
            config_data->data = (void *)wfc_pref;
        }
    }
} // qcril_qmi_imss_store_wifi_call_mode_preference_to_req_info
