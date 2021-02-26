/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <cstring>
#include "modules/qmi/VoiceModemEndPointModule.h"
#include "modules/qmi/VoiceModemEndPoint.h"
#include "modules/qmi/GetOperatingModeSyncMessage.h"
#include "modules/qmi/QmiServiceUpIndMessage.h"
#include "modules/qmi/QmiSendRawSyncMessage.h"
#include "modules/qmi/QmiSendRawAsyncMessage.h"
#include "framework/ModuleLooper.h"
#include "qmi_client.h"

#include "device_management_service_v01.h"

VoiceModemEndPointModule::VoiceModemEndPointModule(string name,
    ModemEndPoint &owner) : ModemEndPointModule(name, owner) {

  mServiceObject = nullptr;
  mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);

  using std::placeholders::_1;
  mMessageHandler = {
  };
}

VoiceModemEndPointModule::~VoiceModemEndPointModule() { mLooper = nullptr; }

qmi_idl_service_object_type VoiceModemEndPointModule::getServiceObject() {
  return voice_get_service_object_v02();
}

bool VoiceModemEndPointModule::handleQmiBinding(
    qcril_instance_id_e_type instanceId, int8_t stackId) {

  (void)instanceId;
  if (stackId < 0)
    return false;

  voice_bind_subscription_req_msg_v02 voice_bind_request;
  voice_bind_subscription_resp_msg_v02 voice_bind_resp;
  memset(&voice_bind_request, 0, sizeof(voice_bind_request));
  memset(&voice_bind_resp, 0, sizeof(voice_bind_resp));

  if (stackId == 0) {
    voice_bind_request.subs_type = VOICE_SUBS_TYPE_PRIMARY_V02;
  } else if (stackId == 1) {
    voice_bind_request.subs_type = VOICE_SUBS_TYPE_SECONDARY_V02;
  } else if (stackId == 2) {
    voice_bind_request.subs_type = VOICE_SUBS_TYPE_TERTIARY_V02;
  } else
    return false;

  int ntries = 0;
  do {
    qmi_client_error_type res = qmi_client_send_msg_sync(
         mQmiSvcClient, QMI_VOICE_BIND_SUBSCRIPTION_REQ_V02,
         (void*)&voice_bind_request, sizeof(voice_bind_request),
         (void*)&voice_bind_resp, sizeof(voice_bind_resp),
         ModemEndPoint::DEFAULT_SYNC_TIMEOUT);
    if (QMI_NO_ERR == res && voice_bind_resp.resp.result ==
            QMI_RESULT_SUCCESS_V01) {
      Log::getInstance().d(
        "[VoiceModemEndPointModule]: QMI binding succeeds. instanceId: "
        + std::to_string((int)instanceId) + " stackId: " + std::to_string(stackId));
      return true;
    }
    usleep(500*1000);
  } while (++ntries < 10);

  return false;
}
