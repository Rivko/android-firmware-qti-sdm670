/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <cstring>
#include "modules/qmi/NasModemEndPointModule.h"
#include "modules/qmi/NasModemEndPoint.h"

#include "modules/qmi/QmiServiceUpIndMessage.h"
#include "modules/qmi/QmiSendRawSyncMessage.h"
#include "modules/qmi/QmiSendRawAsyncMessage.h"

#include "network_access_service_v01.h"

NasModemEndPointModule::NasModemEndPointModule(string name,
    ModemEndPoint &owner) : ModemEndPointModule(name, owner) {

  mServiceObject = nullptr;
  mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);

  mMessageHandler = {
  };

}

NasModemEndPointModule::~NasModemEndPointModule() {
  mLooper = nullptr;
}

bool NasModemEndPointModule::handleQmiBinding(
    qcril_instance_id_e_type instanceId, int8_t stackId) {

  (void)instanceId;
  if (stackId < 0)
    return false;

  nas_bind_subscription_req_msg_v01 nas_bind_request;
  nas_bind_subscription_resp_msg_v01 nas_bind_resp;
  memset(&nas_bind_request, 0, sizeof(nas_bind_request));
  memset(&nas_bind_resp, 0, sizeof(nas_bind_resp));

  if (stackId == 0) {
    nas_bind_request.subs_type = NAS_PRIMARY_SUBSCRIPTION_V01;
  } else if (stackId == 1) {
    nas_bind_request.subs_type = NAS_SECONDARY_SUBSCRIPTION_V01;
  } else if (stackId == 2) {
    nas_bind_request.subs_type = NAS_TERTIARY_SUBSCRIPTION_V01;
  } else
    return false;

  int ntries = 0;
  do {
    qmi_client_error_type res = qmi_client_send_msg_sync(
         mQmiSvcClient, QMI_NAS_BIND_SUBSCRIPTION_REQ_MSG_V01,
         (void*)&nas_bind_request, sizeof(nas_bind_request),
         (void*)&nas_bind_resp, sizeof(nas_bind_resp),
         ModemEndPoint::DEFAULT_SYNC_TIMEOUT);
    if (QMI_NO_ERR == res && nas_bind_resp.resp.result ==
            QMI_RESULT_SUCCESS) {
      Log::getInstance().d(
        "[NasModemEndPointModule]: QMI binding succeeds. instanceId: "
        + std::to_string((int)instanceId) + " stackId: " + std::to_string(stackId));
      return true;
    }
    usleep(500*1000);
  } while (++ntries < 10);

  return false;
}

qmi_idl_service_object_type NasModemEndPointModule::getServiceObject() {
    return nas_get_service_object_v01();
}

