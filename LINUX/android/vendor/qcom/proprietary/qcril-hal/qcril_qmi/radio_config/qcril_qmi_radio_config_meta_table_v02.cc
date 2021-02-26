/******************************************************************************
#  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    qcril_qmi_radio_config_meta_table_v02.c

  DESCRIPTION
    Handles imss related radio config req and response handlers

******************************************************************************/

#include <vector>

#include "qcril_qmi_radio_config_meta.h"
#include "qcril_qmi_radio_config_imss.h"
#include "qcril_qmi_radio_config_imss_v02.h"
#include "qcril_qmi_radio_config_data.h"

/***************************************************************************
*      Table consisiting of information needed to process the              *
*      Radio Config  Item                                                  *
*                        START                                             *
****************************************************************************/
static std::vector<qcril_qmi_radio_config_meta_type> qcril_qmi_radio_config_meta_data_v02;

void init_meta_data_table_v02() {
  qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_VOIP_AMR_MODE,
   qcril_qmi_radio_config_imss_get_codec_dynamic_config_req_handler,
   qcril_qmi_radio_config_imss_get_codec_dynamic_config_resp_handler,
   qcril_qmi_radio_config_imss_set_codec_dynamic_config_req_handler,
   qcril_qmi_radio_config_imss_set_codec_dynamic_config_resp_handler,
   QCRIL_QMI_RADIO_CONFIG_ITEM_IS_STRING,
   QMI_IMS_SETTINGS_GET_CODEC_DYNAMIC_CONFIG_RSP_V01,
   QMI_IMS_SETTINGS_SET_CODEC_DYNAMIC_CONFIG_RSP_V01,
   "QCRIL_QMI_RADIO_CONFIG_VOIP_AMR_MODE"});

  qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_VOIP_AMR_WB_MODE,
   qcril_qmi_radio_config_imss_get_codec_dynamic_config_req_handler,
   qcril_qmi_radio_config_imss_get_codec_dynamic_config_resp_handler,
   qcril_qmi_radio_config_imss_set_codec_dynamic_config_req_handler,
   qcril_qmi_radio_config_imss_set_codec_dynamic_config_resp_handler,
   QCRIL_QMI_RADIO_CONFIG_ITEM_IS_STRING,
   QMI_IMS_SETTINGS_GET_CODEC_DYNAMIC_CONFIG_RSP_V01,
   QMI_IMS_SETTINGS_SET_CODEC_DYNAMIC_CONFIG_RSP_V01,
   "QCRIL_QMI_RADIO_CONFIG_VOIP_AMR_WB_MODE"});

  qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_VOIP_SESSION_EXPIRY_TIMER,
   qcril_qmi_radio_config_imss_get_voice_dynamic_config_req_handler,
   qcril_qmi_radio_config_imss_get_voice_dynamic_config_resp_handler,
   qcril_qmi_radio_config_imss_set_voice_dynamic_config_req_handler,
   qcril_qmi_radio_config_imss_set_voice_dynamic_config_resp_handler,
   QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
   QMI_IMS_SETTINGS_GET_VOICE_DYNAMIC_CONFIG_RSP_V01,
   QMI_IMS_SETTINGS_SET_VOICE_DYNAMIC_CONFIG_RSP_V01,
   "QCRIL_QMI_RADIO_CONFIG_VOIP_SESSION_EXPIRY_TIMER"});

  qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_VOIP_MIN_SESSION_EXPIRY,
   qcril_qmi_radio_config_imss_get_voice_dynamic_config_req_handler,
   qcril_qmi_radio_config_imss_get_voice_dynamic_config_resp_handler,
   qcril_qmi_radio_config_imss_set_voice_dynamic_config_req_handler,
   qcril_qmi_radio_config_imss_set_voice_dynamic_config_resp_handler,
   QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
   QMI_IMS_SETTINGS_GET_VOICE_DYNAMIC_CONFIG_RSP_V01,
   QMI_IMS_SETTINGS_SET_VOICE_DYNAMIC_CONFIG_RSP_V01,
   "QCRIL_QMI_RADIO_CONFIG_VOIP_MIN_SESSION_EXPIRY"});

  qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_VOIP_SILENT_REDIAL_ENABLED,
   qcril_qmi_radio_config_imss_get_voice_dynamic_config_req_handler,
   qcril_qmi_radio_config_imss_get_voice_dynamic_config_resp_handler,
   qcril_qmi_radio_config_imss_set_voice_dynamic_config_req_handler,
   qcril_qmi_radio_config_imss_set_voice_dynamic_config_resp_handler,
   QCRIL_QMI_RADIO_CONFIG_ITEM_IS_BOOLEAN,
   QMI_IMS_SETTINGS_GET_VOICE_DYNAMIC_CONFIG_RSP_V01,
   QMI_IMS_SETTINGS_SET_VOICE_DYNAMIC_CONFIG_RSP_V01,
   "QCRIL_QMI_RADIO_CONFIG_VOIP_SILENT_REDIAL_ENABLED"});

  qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_OPERATOR_MODE_A,
   qcril_qmi_radio_config_imss_get_sip_config_req_handler,
   qcril_qmi_radio_config_imss_get_sip_config_resp_handler,
   qcril_qmi_radio_config_imss_set_sip_config_req_handler,
   qcril_qmi_radio_config_imss_set_sip_config_resp_handler,
   QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
   QMI_IMS_SETTINGS_GET_SIP_CONFIG_RSP_V01,
   QMI_IMS_SETTINGS_SET_SIP_CONFIG_RSP_V01,
   "QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_OPERATOR_MODE_A"});

  qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_T1,
   qcril_qmi_radio_config_imss_get_sip_new_config_req_handler,
   qcril_qmi_radio_config_imss_get_sip_new_config_resp_handler,
   qcril_qmi_radio_config_imss_set_sip_new_config_req_handler,
   qcril_qmi_radio_config_imss_set_sip_new_config_resp_handler,
   QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
   QMI_IMS_SETTINGS_GET_SIP_NEW_CONFIG_RSP_V01,
   QMI_IMS_SETTINGS_SET_SIP_NEW_CONFIG_RSP_V01,
   "QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_T1"});

  qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_T2,
   qcril_qmi_radio_config_imss_get_sip_new_config_req_handler,
   qcril_qmi_radio_config_imss_get_sip_new_config_resp_handler,
   qcril_qmi_radio_config_imss_set_sip_new_config_req_handler,
   qcril_qmi_radio_config_imss_set_sip_new_config_resp_handler,
   QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
   QMI_IMS_SETTINGS_GET_SIP_NEW_CONFIG_RSP_V01,
   QMI_IMS_SETTINGS_SET_SIP_NEW_CONFIG_RSP_V01,
   "QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_T2"});

  qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TF,
   qcril_qmi_radio_config_imss_get_sip_new_config_req_handler,
   qcril_qmi_radio_config_imss_get_sip_new_config_resp_handler,
   qcril_qmi_radio_config_imss_set_sip_new_config_req_handler,
   qcril_qmi_radio_config_imss_set_sip_new_config_resp_handler,
   QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
   QMI_IMS_SETTINGS_GET_SIP_NEW_CONFIG_RSP_V01,
   QMI_IMS_SETTINGS_SET_SIP_NEW_CONFIG_RSP_V01,
   "QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TF"});

  qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_T4,
   qcril_qmi_radio_config_imss_get_sip_new_config_req_handler,
   qcril_qmi_radio_config_imss_get_sip_new_config_resp_handler,
   qcril_qmi_radio_config_imss_set_sip_new_config_req_handler,
   qcril_qmi_radio_config_imss_set_sip_new_config_resp_handler,
   QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
   QMI_IMS_SETTINGS_GET_SIP_NEW_CONFIG_RSP_V01,
   QMI_IMS_SETTINGS_SET_SIP_NEW_CONFIG_RSP_V01,
   "QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_T4"});

  qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TA_VALUE,
   qcril_qmi_radio_config_imss_get_sip_new_config_req_handler,
   qcril_qmi_radio_config_imss_get_sip_new_config_resp_handler,
   qcril_qmi_radio_config_imss_set_sip_new_config_req_handler,
   qcril_qmi_radio_config_imss_set_sip_new_config_resp_handler,
   QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
   QMI_IMS_SETTINGS_GET_SIP_NEW_CONFIG_RSP_V01,
   QMI_IMS_SETTINGS_SET_SIP_NEW_CONFIG_RSP_V01,
   "QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TA_VALUE"});

  qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TB_VALUE,
   qcril_qmi_radio_config_imss_get_sip_new_config_req_handler,
   qcril_qmi_radio_config_imss_get_sip_new_config_resp_handler,
   qcril_qmi_radio_config_imss_set_sip_new_config_req_handler,
   qcril_qmi_radio_config_imss_set_sip_new_config_resp_handler,
   QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
   QMI_IMS_SETTINGS_GET_SIP_NEW_CONFIG_RSP_V01,
   QMI_IMS_SETTINGS_SET_SIP_NEW_CONFIG_RSP_V01,
   "QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TB_VALUE"});

  qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TD_VALUE,
   qcril_qmi_radio_config_imss_get_sip_new_config_req_handler,
   qcril_qmi_radio_config_imss_get_sip_new_config_resp_handler,
   qcril_qmi_radio_config_imss_set_sip_new_config_req_handler,
   qcril_qmi_radio_config_imss_set_sip_new_config_resp_handler,
   QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
   QMI_IMS_SETTINGS_GET_SIP_NEW_CONFIG_RSP_V01,
   QMI_IMS_SETTINGS_SET_SIP_NEW_CONFIG_RSP_V01,
   "QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TD_VALUE"});

  qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TE_VALUE,
   qcril_qmi_radio_config_imss_get_sip_new_config_req_handler,
   qcril_qmi_radio_config_imss_get_sip_new_config_resp_handler,
   qcril_qmi_radio_config_imss_set_sip_new_config_req_handler,
   qcril_qmi_radio_config_imss_set_sip_new_config_resp_handler,
   QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
   QMI_IMS_SETTINGS_GET_SIP_NEW_CONFIG_RSP_V01,
   QMI_IMS_SETTINGS_SET_SIP_NEW_CONFIG_RSP_V01,
   "QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TE_VALUE"});

  qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TG_VALUE,
   qcril_qmi_radio_config_imss_get_sip_new_config_req_handler,
   qcril_qmi_radio_config_imss_get_sip_new_config_resp_handler,
   qcril_qmi_radio_config_imss_set_sip_new_config_req_handler,
   qcril_qmi_radio_config_imss_set_sip_new_config_resp_handler,
   QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
   QMI_IMS_SETTINGS_GET_SIP_NEW_CONFIG_RSP_V01,
   QMI_IMS_SETTINGS_SET_SIP_NEW_CONFIG_RSP_V01,
   "QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TG_VALUE"});

  qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TH_VALUE,
   qcril_qmi_radio_config_imss_get_sip_new_config_req_handler,
   qcril_qmi_radio_config_imss_get_sip_new_config_resp_handler,
   qcril_qmi_radio_config_imss_set_sip_new_config_req_handler,
   qcril_qmi_radio_config_imss_set_sip_new_config_resp_handler,
   QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
   QMI_IMS_SETTINGS_GET_SIP_NEW_CONFIG_RSP_V01,
   QMI_IMS_SETTINGS_SET_SIP_NEW_CONFIG_RSP_V01,
   "QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TH_VALUE"});

  qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TI_VALUE,
   qcril_qmi_radio_config_imss_get_sip_new_config_req_handler,
   qcril_qmi_radio_config_imss_get_sip_new_config_resp_handler,
   qcril_qmi_radio_config_imss_set_sip_new_config_req_handler,
   qcril_qmi_radio_config_imss_set_sip_new_config_resp_handler,
   QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
   QMI_IMS_SETTINGS_GET_SIP_NEW_CONFIG_RSP_V01,
   QMI_IMS_SETTINGS_SET_SIP_NEW_CONFIG_RSP_V01,
   "QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TI_VALUE"});

  qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TJ,
   qcril_qmi_radio_config_imss_get_sip_new_config_req_handler,
   qcril_qmi_radio_config_imss_get_sip_new_config_resp_handler,
   qcril_qmi_radio_config_imss_set_sip_new_config_req_handler,
   qcril_qmi_radio_config_imss_set_sip_new_config_resp_handler,
   QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
   QMI_IMS_SETTINGS_GET_SIP_NEW_CONFIG_RSP_V01,
   QMI_IMS_SETTINGS_SET_SIP_NEW_CONFIG_RSP_V01,
   "QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TJ"});

  qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TK_VALUE,
   qcril_qmi_radio_config_imss_get_sip_new_config_req_handler,
   qcril_qmi_radio_config_imss_get_sip_new_config_resp_handler,
   qcril_qmi_radio_config_imss_set_sip_new_config_req_handler,
   qcril_qmi_radio_config_imss_set_sip_new_config_resp_handler,
   QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
   QMI_IMS_SETTINGS_GET_SIP_NEW_CONFIG_RSP_V01,
   QMI_IMS_SETTINGS_SET_SIP_NEW_CONFIG_RSP_V01,
   "QCRIL_QMI_RADIO_CONFIG_SIP_TIMER_TK_VALUE"});

  qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_SIP_KEEPALIVE_ENABLED,
   qcril_qmi_radio_config_imss_get_sip_new_config_req_handler,
   qcril_qmi_radio_config_imss_get_sip_new_config_resp_handler,
   qcril_qmi_radio_config_imss_set_sip_new_config_req_handler,
   qcril_qmi_radio_config_imss_set_sip_new_config_resp_handler,
   QCRIL_QMI_RADIO_CONFIG_ITEM_IS_BOOLEAN,
   QMI_IMS_SETTINGS_GET_SIP_NEW_CONFIG_RSP_V01,
   QMI_IMS_SETTINGS_SET_SIP_NEW_CONFIG_RSP_V01,
   "QCRIL_QMI_RADIO_CONFIG_SIP_KEEPALIVE_ENABLED"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_REG_MGR_EXTENDED_T_DELAY,
    qcril_qmi_radio_config_imss_get_reg_mgr_new_config_req_handler,
    qcril_qmi_radio_config_imss_get_reg_mgr_new_config_resp_handler,
    qcril_qmi_radio_config_imss_set_reg_mgr_new_config_req_handler,
    qcril_qmi_radio_config_imss_set_reg_mgr_new_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_REG_MGR_NEW_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_REG_MGR_NEW_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_REG_MGR_EXTENDED_T_DELAY"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_REG_MGR_EXTENDED_REG_RETRY_BASE_TIME,
    qcril_qmi_radio_config_imss_get_reg_mgr_new_config_req_handler,
    qcril_qmi_radio_config_imss_get_reg_mgr_new_config_resp_handler,
    qcril_qmi_radio_config_imss_set_reg_mgr_new_config_req_handler,
    qcril_qmi_radio_config_imss_set_reg_mgr_new_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_REG_MGR_NEW_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_REG_MGR_NEW_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_REG_MGR_EXTENDED_REG_RETRY_BASE_TIME"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_REG_MGR_EXTENDED_REG_RETRY_MAX_TIME,
    qcril_qmi_radio_config_imss_get_reg_mgr_new_config_req_handler,
    qcril_qmi_radio_config_imss_get_reg_mgr_new_config_resp_handler,
    qcril_qmi_radio_config_imss_set_reg_mgr_new_config_req_handler,
    qcril_qmi_radio_config_imss_set_reg_mgr_new_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_REG_MGR_NEW_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_REG_MGR_NEW_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_REG_MGR_EXTENDED_REG_RETRY_MAX_TIME"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_VOLTE,
    qcril_qmi_radio_config_imss_get_ims_service_enable_config_req_handler,
    qcril_qmi_radio_config_imss_get_ims_service_enable_config_resp_handler,
    qcril_qmi_radio_config_imss_set_ims_service_enable_config_req_handler,
    qcril_qmi_radio_config_imss_set_ims_service_enable_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_BOOLEAN,
    QMI_IMS_SETTINGS_GET_IMS_SERVICE_ENABLE_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_IMS_SERVICE_ENABLE_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_VOLTE"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_VT,
    qcril_qmi_radio_config_imss_get_ims_service_enable_config_req_handler,
    qcril_qmi_radio_config_imss_get_ims_service_enable_config_resp_handler,
    qcril_qmi_radio_config_imss_set_ims_service_enable_config_req_handler,
    qcril_qmi_radio_config_imss_set_ims_service_enable_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_BOOLEAN,
    QMI_IMS_SETTINGS_GET_IMS_SERVICE_ENABLE_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_IMS_SERVICE_ENABLE_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_VT"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_PRESENCE,
    qcril_qmi_radio_config_imss_get_ims_service_enable_config_req_handler,
    qcril_qmi_radio_config_imss_get_ims_service_enable_config_resp_handler,
    qcril_qmi_radio_config_imss_set_ims_service_enable_config_req_handler,
    qcril_qmi_radio_config_imss_set_ims_service_enable_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_BOOLEAN,
    QMI_IMS_SETTINGS_GET_IMS_SERVICE_ENABLE_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_IMS_SERVICE_ENABLE_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_PRESENCE"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_WIFI_CALL,
    qcril_qmi_radio_config_imss_get_ims_service_enable_config_req_handler,
    qcril_qmi_radio_config_imss_get_ims_service_enable_config_resp_handler,
    qcril_qmi_radio_config_imss_set_ims_service_enable_config_req_handler,
    qcril_qmi_radio_config_imss_set_ims_service_enable_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_IMS_SERVICE_ENABLE_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_IMS_SERVICE_ENABLE_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_WIFI_CALL"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_WIFI_CALL_ROAMING,
    qcril_qmi_radio_config_imss_get_ims_service_enable_config_req_handler,
    qcril_qmi_radio_config_imss_get_ims_service_enable_config_resp_handler,
    qcril_qmi_radio_config_imss_set_ims_service_enable_config_req_handler,
    qcril_qmi_radio_config_imss_set_ims_service_enable_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_IMS_SERVICE_ENABLE_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_IMS_SERVICE_ENABLE_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_WIFI_CALL_ROAMING"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_WIFI_CALL_PREFERENCE,
    qcril_qmi_radio_config_imss_get_ims_service_enable_config_req_handler,
    qcril_qmi_radio_config_imss_get_ims_service_enable_config_resp_handler,
    qcril_qmi_radio_config_imss_set_ims_service_enable_config_req_handler,
    qcril_qmi_radio_config_imss_set_ims_service_enable_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_IMS_SERVICE_ENABLE_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_IMS_SERVICE_ENABLE_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_WIFI_CALL_PREFERENCE"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_USER_IMS_DOMAIN_NAME,
    qcril_qmi_radio_config_imss_get_ims_new_config_req_handler,
    qcril_qmi_radio_config_imss_get_ims_new_config_resp_handler,
    qcril_qmi_radio_config_imss_set_ims_new_config_req_handler,
    qcril_qmi_radio_config_imss_set_ims_new_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_STRING,
    QMI_IMS_SETTINGS_GET_IMS_NEW_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_IMS_NEW_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_USER_IMS_DOMAIN_NAME"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_SMS_FORMAT,
    qcril_qmi_radio_config_imss_get_sms_new_config_req_handler,
    qcril_qmi_radio_config_imss_get_sms_new_config_resp_handler,
    qcril_qmi_radio_config_imss_set_sms_new_config_req_handler,
    qcril_qmi_radio_config_imss_set_sms_new_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_SMS_NEW_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_SMS_NEW_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_SMS_FORMAT"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_SMS_OVER_IP,
    qcril_qmi_radio_config_imss_get_ims_service_enable_config_req_handler,
    qcril_qmi_radio_config_imss_get_ims_service_enable_config_resp_handler,
    qcril_qmi_radio_config_imss_set_ims_service_enable_config_req_handler,
    qcril_qmi_radio_config_imss_set_ims_service_enable_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_BOOLEAN,
    QMI_IMS_SETTINGS_GET_IMS_SERVICE_ENABLE_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_IMS_SERVICE_ENABLE_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_SMS_OVER_IP"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_SMS_PSI,
    qcril_qmi_radio_config_imss_get_sms_new_config_req_handler,
    qcril_qmi_radio_config_imss_get_sms_new_config_resp_handler,
    qcril_qmi_radio_config_imss_set_sms_new_config_req_handler,
    qcril_qmi_radio_config_imss_set_sms_new_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_STRING,
    QMI_IMS_SETTINGS_GET_SMS_NEW_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_SMS_NEW_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_SMS_PSI"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_PRESENCE_PUBLISH_EXPIRY_TIMER,
    qcril_qmi_radio_config_imss_get_presence_new_config_req_handler,
    qcril_qmi_radio_config_imss_get_presence_new_config_resp_handler,
    qcril_qmi_radio_config_imss_set_presence_new_config_req_handler,
    qcril_qmi_radio_config_imss_set_presence_new_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_PRESENCE_NEW_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_PRESENCE_NEW_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_PRESENCE_PUBLISH_EXPIRY_TIMER"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_PRESENCE_PUBLISH_EXTENDED_EXPIRY_TIMER,
    qcril_qmi_radio_config_imss_get_presence_new_config_req_handler,
    qcril_qmi_radio_config_imss_get_presence_new_config_resp_handler,
    qcril_qmi_radio_config_imss_set_presence_new_config_req_handler,
    qcril_qmi_radio_config_imss_set_presence_new_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_PRESENCE_NEW_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_PRESENCE_NEW_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_PRESENCE_PUBLISH_EXTENDED_EXPIRY_TIMER"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_PRESENCE_CAPABILITIES_CACHE_EXPIRATION,
    qcril_qmi_radio_config_imss_get_presence_new_config_req_handler,
    qcril_qmi_radio_config_imss_get_presence_new_config_resp_handler,
    qcril_qmi_radio_config_imss_set_presence_new_config_req_handler,
    qcril_qmi_radio_config_imss_set_presence_new_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_PRESENCE_NEW_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_PRESENCE_NEW_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_PRESENCE_CAPABILITIES_CACHE_EXPIRATION"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_PRESENCE_AVAILABILITY_CACHE_EXPIRATION,
    qcril_qmi_radio_config_imss_get_presence_new_config_req_handler,
    qcril_qmi_radio_config_imss_get_presence_new_config_resp_handler,
    qcril_qmi_radio_config_imss_set_presence_new_config_req_handler,
    qcril_qmi_radio_config_imss_set_presence_new_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_PRESENCE_NEW_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_PRESENCE_NEW_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_PRESENCE_AVAILABILITY_CACHE_EXPIRATION"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_PRESENCE_CAPABILITY_POLL_INTERVAL,
    qcril_qmi_radio_config_imss_get_presence_new_config_req_handler,
    qcril_qmi_radio_config_imss_get_presence_new_config_resp_handler,
    qcril_qmi_radio_config_imss_set_presence_new_config_req_handler,
    qcril_qmi_radio_config_imss_set_presence_new_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_PRESENCE_NEW_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_PRESENCE_NEW_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_PRESENCE_CAPABILITY_POLL_INTERVAL"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_PRESENCE_MINIMUM_PUBLISH_INTERVAL,
    qcril_qmi_radio_config_imss_get_presence_new_config_req_handler,
    qcril_qmi_radio_config_imss_get_presence_new_config_resp_handler,
    qcril_qmi_radio_config_imss_set_presence_new_config_req_handler,
    qcril_qmi_radio_config_imss_set_presence_new_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_PRESENCE_NEW_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_PRESENCE_NEW_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_PRESENCE_MINIMUM_PUBLISH_INTERVAL"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_PRESENCE_MAXIMUM_SUBSCRIPTION_LIST_ENTRIES,
    qcril_qmi_radio_config_imss_get_presence_new_config_req_handler,
    qcril_qmi_radio_config_imss_get_presence_new_config_resp_handler,
    qcril_qmi_radio_config_imss_set_presence_new_config_req_handler,
    qcril_qmi_radio_config_imss_set_presence_new_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_PRESENCE_NEW_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_PRESENCE_NEW_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_PRESENCE_MAXIMUM_SUBSCRIPTION_LIST_ENTRIES"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_PRESENCE_CAPABILITY_POLL_LIST_SUBSCRIPTION_EXPIRY_TIMER,
    qcril_qmi_radio_config_imss_get_presence_new_config_req_handler,
    qcril_qmi_radio_config_imss_get_presence_new_config_resp_handler,
    qcril_qmi_radio_config_imss_set_presence_new_config_req_handler,
    qcril_qmi_radio_config_imss_set_presence_new_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_PRESENCE_NEW_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_PRESENCE_NEW_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_PRESENCE_CAPABILITY_POLL_LIST_SUBSCRIPTION_EXPIRY_TIMER"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_PRESENCE_GZIP_ENABLED,
    qcril_qmi_radio_config_imss_get_presence_new_config_req_handler,
    qcril_qmi_radio_config_imss_get_presence_new_config_resp_handler,
    qcril_qmi_radio_config_imss_set_presence_new_config_req_handler,
    qcril_qmi_radio_config_imss_set_presence_new_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_BOOLEAN,
    QMI_IMS_SETTINGS_GET_PRESENCE_NEW_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_PRESENCE_NEW_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_PRESENCE_GZIP_ENABLED"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_PRESENCE_VOLTE_USER_OPTED_IN_STATUS,
    qcril_qmi_radio_config_imss_get_ims_service_enable_config_req_handler,
    qcril_qmi_radio_config_imss_get_ims_service_enable_config_resp_handler,
    qcril_qmi_radio_config_imss_set_ims_service_enable_config_req_handler,
    qcril_qmi_radio_config_imss_set_ims_service_enable_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_BOOLEAN,
    QMI_IMS_SETTINGS_GET_IMS_SERVICE_ENABLE_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_IMS_SERVICE_ENABLE_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_PRESENCE_VOLTE_USER_OPTED_IN_STATUS"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_PRESENCE_CAPABILITY_DISCOVERY_ENABLE,
    qcril_qmi_radio_config_imss_get_presence_new_config_req_handler,
    qcril_qmi_radio_config_imss_get_presence_new_config_resp_handler,
    qcril_qmi_radio_config_imss_set_presence_new_config_req_handler,
    qcril_qmi_radio_config_imss_set_presence_new_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_BOOLEAN,
    QMI_IMS_SETTINGS_GET_PRESENCE_NEW_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_PRESENCE_NEW_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_PRESENCE_CAPABILITY_DISCOVERY_ENABLE"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_QIPCALL_MOBILE_DATA_ENABLED,
    qcril_qmi_radio_config_imss_get_ims_service_enable_config_req_handler,
    qcril_qmi_radio_config_imss_get_ims_service_enable_config_resp_handler,
    qcril_qmi_radio_config_imss_set_ims_service_enable_config_req_handler,
    qcril_qmi_radio_config_imss_set_ims_service_enable_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_BOOLEAN,
    QMI_IMS_SETTINGS_GET_IMS_SERVICE_ENABLE_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_IMS_SERVICE_ENABLE_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_IMS_SERVICE_ENABLE_MOBILE_DATA_ENABLED"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_QIPCALL_VOLTE_ENABLED,
    qcril_qmi_radio_config_imss_get_ims_service_enable_config_req_handler,
    qcril_qmi_radio_config_imss_get_ims_service_enable_config_resp_handler,
    qcril_qmi_radio_config_imss_set_ims_service_enable_config_req_handler,
    qcril_qmi_radio_config_imss_set_ims_service_enable_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_BOOLEAN,
    QMI_IMS_SETTINGS_GET_IMS_SERVICE_ENABLE_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_IMS_SERVICE_ENABLE_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_QIPCALL_VOLTE_ENABLED"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_QIPCALL_VT_CALLING_ENABLED,
    qcril_qmi_radio_config_imss_get_ims_service_enable_config_req_handler,
    qcril_qmi_radio_config_imss_get_ims_service_enable_config_resp_handler,
    qcril_qmi_radio_config_imss_set_ims_service_enable_config_req_handler,
    qcril_qmi_radio_config_imss_set_ims_service_enable_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_BOOLEAN,
    QMI_IMS_SETTINGS_GET_IMS_SERVICE_ENABLE_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_IMS_SERVICE_ENABLE_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_QIPCALL_VT_CALLING_ENABLED"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_QIPCALL_SPEECH_START_PORT,
    qcril_qmi_radio_config_imss_get_rtp_dynamic_config_req_handler,
    qcril_qmi_radio_config_imss_get_rtp_dynamic_config_resp_handler,
    qcril_qmi_radio_config_imss_set_rtp_dynamic_config_req_handler,
    qcril_qmi_radio_config_imss_set_rtp_dynamic_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_RTP_DYNAMIC_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_RTP_DYNAMIC_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_QIPCALL_SPEECH_START_PORT"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_QIPCALL_SPEECH_END_PORT,
    qcril_qmi_radio_config_imss_get_rtp_dynamic_config_req_handler,
    qcril_qmi_radio_config_imss_get_rtp_dynamic_config_resp_handler,
    qcril_qmi_radio_config_imss_set_rtp_dynamic_config_req_handler,
    qcril_qmi_radio_config_imss_set_rtp_dynamic_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_RTP_DYNAMIC_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_RTP_DYNAMIC_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_QIPCALL_SPEECH_END_PORT"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_WB_OCTET_ALIGNED_DYNAMIC_PT,
    qcril_qmi_radio_config_imss_get_codec_dynamic_config_req_handler,
    qcril_qmi_radio_config_imss_get_codec_dynamic_config_resp_handler,
    qcril_qmi_radio_config_imss_set_codec_dynamic_config_req_handler,
    qcril_qmi_radio_config_imss_set_codec_dynamic_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_CODEC_DYNAMIC_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_CODEC_DYNAMIC_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_WB_OCTET_ALIGNED_DYNAMIC_PT"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_WB_BANDWIDTH_EFFICIENT_DYNAMIC_PT ,
    qcril_qmi_radio_config_imss_get_codec_dynamic_config_req_handler,
    qcril_qmi_radio_config_imss_get_codec_dynamic_config_resp_handler,
    qcril_qmi_radio_config_imss_set_codec_dynamic_config_req_handler,
    qcril_qmi_radio_config_imss_set_codec_dynamic_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_CODEC_DYNAMIC_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_CODEC_DYNAMIC_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_WB_BANDWIDTH_EFFICIENT_DYNAMIC_PT"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_OCTET_ALIGNED_DYNAMIC_PT,
    qcril_qmi_radio_config_imss_get_codec_dynamic_config_req_handler,
    qcril_qmi_radio_config_imss_get_codec_dynamic_config_resp_handler,
    qcril_qmi_radio_config_imss_set_codec_dynamic_config_req_handler,
    qcril_qmi_radio_config_imss_set_codec_dynamic_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_CODEC_DYNAMIC_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_CODEC_DYNAMIC_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_OCTET_ALIGNED_DYNAMIC_PT"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_BANDWIDTH_EFFICIENT_DYNAMIC_PT,
    qcril_qmi_radio_config_imss_get_codec_dynamic_config_req_handler,
    qcril_qmi_radio_config_imss_get_codec_dynamic_config_resp_handler,
    qcril_qmi_radio_config_imss_set_codec_dynamic_config_req_handler,
    qcril_qmi_radio_config_imss_set_codec_dynamic_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_CODEC_DYNAMIC_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_CODEC_DYNAMIC_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_BANDWIDTH_EFFICIENT_DYNAMIC_PT"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_QIPCALL_DTMF_WB_DYNAMIC_POINT,
    qcril_qmi_radio_config_imss_get_codec_dynamic_config_req_handler,
    qcril_qmi_radio_config_imss_get_codec_dynamic_config_resp_handler,
    qcril_qmi_radio_config_imss_set_codec_dynamic_config_req_handler,
    qcril_qmi_radio_config_imss_set_codec_dynamic_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_CODEC_DYNAMIC_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_CODEC_DYNAMIC_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_QIPCALL_DTMF_WB_DYNAMIC_POINT"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_QIPCALL_DTMF_NB_DYNAMIC_PT,
    qcril_qmi_radio_config_imss_get_codec_dynamic_config_req_handler,
    qcril_qmi_radio_config_imss_get_codec_dynamic_config_resp_handler,
    qcril_qmi_radio_config_imss_set_codec_dynamic_config_req_handler,
    qcril_qmi_radio_config_imss_set_codec_dynamic_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_CODEC_DYNAMIC_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_CODEC_DYNAMIC_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_QIPCALL_DTMF_NB_DYNAMIC_PT"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_DEFAULT_MODE,
    qcril_qmi_radio_config_imss_get_codec_dynamic_config_req_handler,
    qcril_qmi_radio_config_imss_get_codec_dynamic_config_resp_handler,
    qcril_qmi_radio_config_imss_set_codec_dynamic_config_req_handler,
    qcril_qmi_radio_config_imss_set_codec_dynamic_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_BOOLEAN,
    QMI_IMS_SETTINGS_GET_CODEC_DYNAMIC_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_CODEC_DYNAMIC_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_QIPCALL_AMR_DEFAULT_MODE"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_QIPCALL_EMERG_CALL_TIMER,
    qcril_qmi_radio_config_imss_get_emer_dynamic_config_req_handler,
    qcril_qmi_radio_config_imss_get_emer_dynamic_config_resp_handler,
    qcril_qmi_radio_config_imss_set_emer_dynamic_config_req_handler,
    qcril_qmi_radio_config_imss_set_emer_dynamic_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_EMER_DYNAMIC_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_EMER_DYNAMIC_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_QIPCALL_EMERG_CALL_TIMER"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_REG_MGR_CONFIG_REG_MGR_PRIMARY_CSCF,
    qcril_qmi_radio_config_imss_get_reg_mgr_new_config_req_handler,
    qcril_qmi_radio_config_imss_get_reg_mgr_new_config_resp_handler,
    qcril_qmi_radio_config_imss_set_reg_mgr_new_config_req_handler,
    qcril_qmi_radio_config_imss_set_reg_mgr_new_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_STRING,
    QMI_IMS_SETTINGS_GET_REG_MGR_NEW_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_REG_MGR_NEW_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_REG_MGR_CONFIG_REG_MGR_PRIMARY_CSCF"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_QIPCALL_VIDEO_QUALITY,
    qcril_qmi_radio_config_imss_get_video_dynamic_config_req_handler,
    qcril_qmi_radio_config_imss_get_video_dynamic_config_resp_handler,
    qcril_qmi_radio_config_imss_set_video_dynamic_config_req_handler,
    qcril_qmi_radio_config_imss_set_video_dynamic_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_VIDEO_DYNAMIC_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_VIDEO_DYNAMIC_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_QIPCALL_VIDEO_QUALITY"});

#if 0
// TODO: REFACTOR enable this once Linux data is ready.

    qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_LTE_RSRP_LOW,
    NULL, //currently get not present or supported
    NULL,
    qcril_qmi_radio_config_data_set_quality_measurement_req_handler,
    qcril_qmi_radio_config_data_set_quality_measurement_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    -1,
    -1,
    "QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_LTE_RSRP_LOW"});

    qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_LTE_RSRP_MID,
    NULL, //currently get not present or supported
    NULL,
    qcril_qmi_radio_config_data_set_quality_measurement_req_handler,
    qcril_qmi_radio_config_data_set_quality_measurement_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    -1,
    -1,
    "QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_LTE_RSRP_MID"});

    qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_LTE_RSRP_HIGH,
    NULL, //currently get not present or supported
    NULL,
    qcril_qmi_radio_config_data_set_quality_measurement_req_handler,
    qcril_qmi_radio_config_data_set_quality_measurement_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    -1,
    -1,
    "QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_LTE_RSRP_HIGH"});

    qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_WIFI_RSSI_THRESHOLD_LOW,
    NULL, //currently get not present or supported
    NULL,
    qcril_qmi_radio_config_data_set_quality_measurement_req_handler,
    qcril_qmi_radio_config_data_set_quality_measurement_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    -1,
    -1,
    "QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_WIFI_RSSI_THRESHOLD_LOW"});

    qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_WIFI_RSSI_THRESHOLD_HIGH,
    NULL, //currently get not present or supported
    NULL,
    qcril_qmi_radio_config_data_set_quality_measurement_req_handler,
    qcril_qmi_radio_config_data_set_quality_measurement_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    -1,
    -1,
    "QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_WIFI_RSSI_THRESHOLD_HIGH"});

    qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_WIFI_SINR_THRESHOLD_LOW,
    NULL, //currently get not present or supported
    NULL,
    qcril_qmi_radio_config_data_set_quality_measurement_req_handler,
    qcril_qmi_radio_config_data_set_quality_measurement_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    -1,
    -1,
    "QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_WIFI_SINR_THRESHOLD_LOW"});

    qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_WIFI_SINR_THRESHOLD_HIGH,
    NULL, //currently get not present or supported
    NULL,
    qcril_qmi_radio_config_data_set_quality_measurement_req_handler,
    qcril_qmi_radio_config_data_set_quality_measurement_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    -1,
    -1,
    "QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_WIFI_SINR_THRESHOLD_HIGH"});

    qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_LTE_RSRP_LOW,
    NULL, //currently get not present or supported
    NULL,
    qcril_qmi_radio_config_data_set_quality_measurement_req_handler,
    qcril_qmi_radio_config_data_set_quality_measurement_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    -1,
    -1,
    "QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_LTE_RSRP_LOW"});

    qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_LTE_RSRP_MID,
    NULL, //currently get not present or supported
    NULL,
    qcril_qmi_radio_config_data_set_quality_measurement_req_handler,
    qcril_qmi_radio_config_data_set_quality_measurement_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    -1,
    -1,
    "QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_LTE_RSRP_MID"});

    qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_LTE_RSRP_HIGH,
    NULL, //currently get not present or supported
    NULL,
    qcril_qmi_radio_config_data_set_quality_measurement_req_handler,
    qcril_qmi_radio_config_data_set_quality_measurement_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    -1,
    -1,
    "QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_LTE_RSRP_HIGH"});

    qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_WIFI_RSSI_THRESHOLD_LOW,
    NULL, //currently get not present or supported
    NULL,
    qcril_qmi_radio_config_data_set_quality_measurement_req_handler,
    qcril_qmi_radio_config_data_set_quality_measurement_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    -1,
    -1,
    "QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_WIFI_RSSI_THRESHOLD_LOW"});

    qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_WIFI_RSSI_THRESHOLD_HIGH,
    NULL, //currently get not present or supported
    NULL,
    qcril_qmi_radio_config_data_set_quality_measurement_req_handler,
    qcril_qmi_radio_config_data_set_quality_measurement_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    -1,
    -1,
    "QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_WIFI_RSSI_THRESHOLD_HIGH"});

    qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_WIFI_SINR_THRESHOLD_LOW,
    NULL, //currently get not present or supported
    NULL,
    qcril_qmi_radio_config_data_set_quality_measurement_req_handler,
    qcril_qmi_radio_config_data_set_quality_measurement_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    -1,
    -1,
    "QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_WIFI_SINR_THRESHOLD_LOW"});

    qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_WIFI_SINR_THRESHOLD_HIGH,
    NULL, //currently get not present or supported
    NULL,
    qcril_qmi_radio_config_data_set_quality_measurement_req_handler,
    qcril_qmi_radio_config_data_set_quality_measurement_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    -1,
    -1,
    "QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_WIFI_SINR_THRESHOLD_HIGH"});

    qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_ECIO_1X_THRESHOLD_LOW,
    NULL, //currently get not present or supported
    NULL,
    qcril_qmi_radio_config_data_set_quality_measurement_req_handler,
    qcril_qmi_radio_config_data_set_quality_measurement_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    -1,
    -1,
    "QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_ECIO_1X_THRESHOLD_LOW"});

    qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_ECIO_1X_THRESHOLD_HIGH,
    NULL, //currently get not present or supported
    NULL,
    qcril_qmi_radio_config_data_set_quality_measurement_req_handler,
    qcril_qmi_radio_config_data_set_quality_measurement_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    -1,
    -1,
    "QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_ECIO_1X_THRESHOLD_HIGH"});
#endif

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_LTE_THRESHOLD1,
    qcril_qmi_radio_config_imss_get_handover_config_req_handler,
    qcril_qmi_radio_config_imss_get_handover_config_resp_handler,
    qcril_qmi_radio_config_imss_set_handover_config_req_handler,
    qcril_qmi_radio_config_imss_set_handover_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_HANDOVER_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_HANDOVER_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_LTE_THRESHOLD1"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_LTE_THRESHOLD2,
    qcril_qmi_radio_config_imss_get_handover_config_req_handler,
    qcril_qmi_radio_config_imss_get_handover_config_resp_handler,
    qcril_qmi_radio_config_imss_set_handover_config_req_handler,
    qcril_qmi_radio_config_imss_set_handover_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_HANDOVER_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_HANDOVER_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_LTE_THRESHOLD2"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_LTE_THRESHOLD3,
    qcril_qmi_radio_config_imss_get_handover_config_req_handler,
    qcril_qmi_radio_config_imss_get_handover_config_resp_handler,
    qcril_qmi_radio_config_imss_set_handover_config_req_handler,
    qcril_qmi_radio_config_imss_set_handover_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_HANDOVER_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_HANDOVER_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_LTE_THRESHOLD3"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_1X_THRESHOLD,
    qcril_qmi_radio_config_imss_get_handover_config_req_handler,
    qcril_qmi_radio_config_imss_get_handover_config_resp_handler,
    qcril_qmi_radio_config_imss_set_handover_config_req_handler,
    qcril_qmi_radio_config_imss_set_handover_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_HANDOVER_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_HANDOVER_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_1X_THRESHOLD"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_WIFI_THRESHOLDA,
    qcril_qmi_radio_config_imss_get_handover_config_req_handler,
    qcril_qmi_radio_config_imss_get_handover_config_resp_handler,
    qcril_qmi_radio_config_imss_set_handover_config_req_handler,
    qcril_qmi_radio_config_imss_set_handover_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_HANDOVER_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_HANDOVER_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_WIFI_THRESHOLDA"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_WIFI_THRESHOLDB,
    qcril_qmi_radio_config_imss_get_handover_config_req_handler,
    qcril_qmi_radio_config_imss_get_handover_config_resp_handler,
    qcril_qmi_radio_config_imss_set_handover_config_req_handler,
    qcril_qmi_radio_config_imss_set_handover_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_HANDOVER_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_HANDOVER_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_WIFI_THRESHOLDB"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_WLAN_TO_WWAN_HYS_TIMER,
    qcril_qmi_radio_config_imss_get_handover_config_req_handler,
    qcril_qmi_radio_config_imss_get_handover_config_resp_handler,
    qcril_qmi_radio_config_imss_set_handover_config_req_handler,
    qcril_qmi_radio_config_imss_set_handover_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_HANDOVER_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_HANDOVER_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_WLAN_TO_WWAN_HYS_TIMER"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_WLAN_TO_1X_HYS_TIMER,
    qcril_qmi_radio_config_imss_get_handover_config_req_handler,
    qcril_qmi_radio_config_imss_get_handover_config_resp_handler,
    qcril_qmi_radio_config_imss_set_handover_config_req_handler,
    qcril_qmi_radio_config_imss_set_handover_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_HANDOVER_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_HANDOVER_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_WLAN_TO_1X_HYS_TIMER"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_WWAN_TO_WLAN_HYS_TIMER,
    qcril_qmi_radio_config_imss_get_handover_config_req_handler,
    qcril_qmi_radio_config_imss_get_handover_config_resp_handler,
    qcril_qmi_radio_config_imss_set_handover_config_req_handler,
    qcril_qmi_radio_config_imss_set_handover_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_HANDOVER_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_HANDOVER_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_HANDOVER_CONFIG_WWAN_TO_WLAN_HYS_TIMER"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_VOWIFI,
    qcril_qmi_radio_config_imss_get_ims_service_enable_config_req_handler,
    qcril_qmi_radio_config_imss_get_ims_service_enable_config_resp_handler,
    qcril_qmi_radio_config_imss_set_ims_service_enable_config_req_handler,
    qcril_qmi_radio_config_imss_set_ims_service_enable_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_BOOLEAN,
    QMI_IMS_SETTINGS_GET_IMS_SERVICE_ENABLE_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_IMS_SERVICE_ENABLE_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_VOWIFI"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_QIPCALL_VICE_ENABLED,
    qcril_qmi_radio_config_imss_get_voice_dynamic_config_req_handler,
    qcril_qmi_radio_config_imss_get_voice_dynamic_config_resp_handler,
    qcril_qmi_radio_config_imss_set_voice_dynamic_config_req_handler,
    qcril_qmi_radio_config_imss_set_voice_dynamic_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_BOOLEAN,
    QMI_IMS_SETTINGS_GET_VOICE_DYNAMIC_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_VOICE_DYNAMIC_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_QIPCALL_VICE_ENABLED"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_RTT,
    qcril_qmi_radio_config_imss_get_ims_service_enable_config_req_handler,
    qcril_qmi_radio_config_imss_get_ims_service_enable_config_resp_handler,
    qcril_qmi_radio_config_imss_set_ims_service_enable_config_req_handler,
    qcril_qmi_radio_config_imss_set_ims_service_enable_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_BOOLEAN,
    QMI_IMS_SETTINGS_GET_IMS_SERVICE_ENABLE_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_IMS_SERVICE_ENABLE_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_CLIENT_PROVISIONING_ENABLE_RTT"});

   qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_RCS_SMS_APP,
    qcril_qmi_radio_config_imss_get_rcs_new_config_req_handler,
    qcril_qmi_radio_config_imss_get_rcs_new_config_resp_handler,
    qcril_qmi_radio_config_imss_set_rcs_new_config_req_handler,
    qcril_qmi_radio_config_imss_set_rcs_new_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_RCS_SM_NEW_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_RCS_SM_NEW_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_RCS_SMS_APP"});

    qcril_qmi_radio_config_meta_data_v02.push_back({QCRIL_QMI_RADIO_CONFIG_RCS_VVM_APP,
    qcril_qmi_radio_config_imss_get_rcs_new_config_req_handler,
    qcril_qmi_radio_config_imss_get_rcs_new_config_resp_handler,
    qcril_qmi_radio_config_imss_set_rcs_new_config_req_handler,
    qcril_qmi_radio_config_imss_set_rcs_new_config_resp_handler,
    QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT,
    QMI_IMS_SETTINGS_GET_RCS_SM_NEW_CONFIG_RSP_V01,
    QMI_IMS_SETTINGS_SET_RCS_SM_NEW_CONFIG_RSP_V01,
    "QCRIL_QMI_RADIO_CONFIG_RCS_VVM_APP"});
};
/***************************************************************************
*      Table consisiting of information needed to process the              *
*      Radio Config  Item                                                  *
*                        END                                               *
****************************************************************************/
//===================================
//  get_length_meta_table_v02
//===================================
size_t get_length_meta_table_v02()
{
  return qcril_qmi_radio_config_meta_data_v02.size();
}

const std::vector<qcril_qmi_radio_config_meta_type> *get_meta_table_v02()
{
  if (qcril_qmi_radio_config_meta_data_v02.size() == 0) {
    init_meta_data_table_v02();
  }
  return &qcril_qmi_radio_config_meta_data_v02;
}
