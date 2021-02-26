/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <vector>
#include "qcril_qmi_imss_misc.h"
#include "qcril_qmi_imss.h"
#include "qcril_qmi_imss_v02.h"
#include "qcril_qmi_ims_radio_agent.h"
#include <framework/Log.h>

#define TAG "QCRIL_IMSS"

std::vector< std::vector<qcril_qmi_imss_table_entry_type> > qcril_qmi_imss_init_imss_table(void) {
  std::vector< std::vector<qcril_qmi_imss_table_entry_type> > imss_table(
      IMSS_MODEM_VERSION_MAX,
      std::vector<qcril_qmi_imss_table_entry_type>(IMSS_HANDLERS_MAX));

  imss_table[0].push_back({ QCRIL_EVT_IMS_SOCKET_REQ_IMS_REG_STATE_CHANGE,    qcril_qmi_imss_request_set_ims_registration });
  imss_table[0].push_back({ QCRIL_EVT_IMS_SOCKET_REQ_SET_SERVICE_STATUS,      qcril_qmi_imss_request_set_ims_srv_status });
  imss_table[0].push_back({ QCRIL_EVT_IMS_SOCKET_REQ_QUERY_VT_CALL_QUALITY,   qcril_qmi_imss_request_query_vt_call_quality });
  imss_table[0].push_back({ QCRIL_EVT_IMS_SOCKET_REQ_SET_VT_CALL_QUALITY,     qcril_qmi_imss_request_set_vt_call_quality });
  imss_table[0].push_back({ QCRIL_EVT_IMS_SOCKET_REQ_GET_WIFI_CALLING_STATUS, qcril_qmi_imss_request_get_wifi_calling_status });
  imss_table[0].push_back({ QCRIL_EVT_IMS_SOCKET_REQ_SET_WIFI_CALLING_STATUS, qcril_qmi_imss_request_set_wifi_calling_status });
  imss_table[0].push_back({ QCRIL_EVT_IMS_SOCKET_REQ_SET_IMS_CONFIG,          qcril_qmi_imss_request_set_ims_config });
  imss_table[0].push_back({ QCRIL_EVT_IMS_SOCKET_REQ_GET_IMS_CONFIG,          qcril_qmi_imss_request_get_ims_config });

  imss_table[1].push_back({ QCRIL_EVT_IMS_SOCKET_REQ_IMS_REG_STATE_CHANGE,    qcril_qmi_imss_request_set_ims_registration_v02 });
  imss_table[1].push_back({ QCRIL_EVT_IMS_SOCKET_REQ_SET_SERVICE_STATUS,      qcril_qmi_imss_request_set_ims_srv_status_v02 });
  imss_table[1].push_back({ QCRIL_EVT_IMS_SOCKET_REQ_QUERY_VT_CALL_QUALITY,   qcril_qmi_imss_request_query_vt_call_quality_v02 });
  imss_table[1].push_back({ QCRIL_EVT_IMS_SOCKET_REQ_SET_VT_CALL_QUALITY,     qcril_qmi_imss_request_set_vt_call_quality_v02 });
  imss_table[1].push_back({ QCRIL_EVT_IMS_SOCKET_REQ_GET_WIFI_CALLING_STATUS, qcril_qmi_imss_request_get_wifi_calling_status_v02 });
  imss_table[1].push_back({ QCRIL_EVT_IMS_SOCKET_REQ_SET_WIFI_CALLING_STATUS, qcril_qmi_imss_request_set_wifi_calling_status_v02 });
  imss_table[1].push_back({ QCRIL_EVT_IMS_SOCKET_REQ_SET_IMS_CONFIG,          qcril_qmi_imss_request_set_ims_config_v02 });
  imss_table[1].push_back({ QCRIL_EVT_IMS_SOCKET_REQ_GET_IMS_CONFIG,          qcril_qmi_imss_request_get_ims_config_v02 });

  return imss_table;
}

void qcril_qmi_imss_init_handlers(void) {
  // query modem and identify imss supports new version or not.
  // set "modem_version" based on the modem version

  uint8_t version = qcril_qmi_imss_get_modem_version();

  std::vector<qcril_qmi_imss_table_entry_type> imss_table = qcril_qmi_imss_init_imss_table()[version];

  QCRIL_LOG_DEBUG("Update the handlers for modem_version : %d", version);

  for (size_t i = 0; i < imss_table.size(); i++) {
    QCRIL_LOG_DEBUG("event_id = %d", imss_table[i].event_id);
    qcril_update_event_table_entry(imss_table[i].event_id, imss_table[i].handler);
  }

  qcril_qmi_radio_config_update_meta_table(version);
  QCRIL_LOG_DEBUG("IMSS table update complete.");
}
