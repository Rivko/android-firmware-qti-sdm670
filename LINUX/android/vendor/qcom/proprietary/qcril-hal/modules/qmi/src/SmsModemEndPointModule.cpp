/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <cstring>
#include "modules/qmi/SmsModemEndPointModule.h"
#include "modules/qmi/SmsModemEndPoint.h"

#include "modules/qmi/QmiServiceUpIndMessage.h"
#include "modules/qmi/QmiSendRawSyncMessage.h"
#include "modules/qmi/QmiSendRawAsyncMessage.h"

#include "wireless_messaging_service_v01.h"

SmsModemEndPointModule::SmsModemEndPointModule(string name,
    ModemEndPoint &owner) : ModemEndPointModule(name, owner) {
  mServiceObject = nullptr;
  mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);

  mMessageHandler = {};
}

SmsModemEndPointModule::~SmsModemEndPointModule() {
  mLooper = nullptr;
}

qmi_idl_service_object_type SmsModemEndPointModule::getServiceObject() {
  return wms_get_service_object_v01();
}

bool SmsModemEndPointModule::handleQmiBinding(
    qcril_instance_id_e_type instanceId, int8_t stackId) {

  (void)instanceId;
  if (stackId < 0)
    return false;

  wms_bind_subscription_req_msg_v01 wms_bind_request;
  wms_bind_subscription_resp_msg_v01 wms_bind_resp;
  memset(&wms_bind_request, 0, sizeof(wms_bind_request));
  memset(&wms_bind_resp, 0, sizeof(wms_bind_resp));

  if (stackId == 0) {
    wms_bind_request.subs_type = WMS_PRIMARY_SUBSCRIPTION_V01;;
  } else if (stackId == 1) {
    wms_bind_request.subs_type = WMS_SECONDARY_SUBSCRIPTION_V01;
  } else if (stackId == 2) {
    wms_bind_request.subs_type = WMS_TERTIARY_SUBSCRIPTION_V01;
  } else
    return false;

  int ntries = 0;
  do {
    qmi_client_error_type res = qmi_client_send_msg_sync(
         mQmiSvcClient, QMI_WMS_BIND_SUBSCRIPTION_REQ_V01,
         (void*)&wms_bind_request, sizeof(wms_bind_request),
         (void*)&wms_bind_resp, sizeof(wms_bind_resp),
         ModemEndPoint::DEFAULT_SYNC_TIMEOUT);
    if (QMI_NO_ERR == res && wms_bind_resp.resp.result ==
            QMI_RESULT_SUCCESS_V01) {
      Log::getInstance().d(
        "[SmsModemEndPointModule]: QMI binding succeeds. instanceId: "
        + std::to_string((int)instanceId) + " stackId: " + std::to_string(stackId));
      return true;
    }
    usleep(500*1000);
  } while (++ntries < 10);

  return false;
}
