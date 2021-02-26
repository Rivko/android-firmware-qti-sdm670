/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <cstring>
#include "modules/qmi/ImsaModemEndPointModule.h"
#include "modules/qmi/ImsaModemEndPoint.h"

#include "modules/qmi/QmiServiceUpIndMessage.h"
#include "modules/qmi/QmiSendRawSyncMessage.h"
#include "modules/qmi/QmiSendRawAsyncMessage.h"

#include "ip_multimedia_subsystem_application_v01.h"

ImsaModemEndPointModule::ImsaModemEndPointModule(string name,
    ModemEndPoint &owner) : ModemEndPointModule(name, owner) {

  mServiceObject = nullptr;
  mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);

  mMessageHandler = {
  };

}

ImsaModemEndPointModule::~ImsaModemEndPointModule() {
  mLooper = nullptr;
}

qmi_idl_service_object_type ImsaModemEndPointModule::getServiceObject() {
  return imsa_get_service_object_v01();
}

bool ImsaModemEndPointModule::handleQmiBinding(
        qcril_instance_id_e_type instanceId, int8_t stackId)
{
  (void)instanceId;
  if (stackId < 0)
    return false;

  imsa_set_bind_subscription_req_msg_v01  imsa_bind_request;
  imsa_set_bind_subscription_resp_msg_v01 imsa_bind_resp;
  memset(&imsa_bind_request, 0, sizeof(imsa_bind_request));
  memset(&imsa_bind_resp, 0, sizeof(imsa_bind_resp));

  if (stackId == 0) {
    imsa_bind_request.subscription_type = IMSA_BIND_SUBSCRIPTION_TYPE_PRIMARY_V01;
  } else if (stackId == 1) {
    imsa_bind_request.subscription_type = IMSA_BIND_SUBSCRIPTION_TYPE_SECONDARY_V01;
  } else if (stackId == 2) {
    imsa_bind_request.subscription_type = IMSA_BIND_SUBSCRIPTION_TYPE_TERTIARY_V01;
  } else
    return false;

  imsa_bind_request.subscription_type_valid = TRUE;

  int ntries = 0;
  do {
    qmi_client_error_type res = qmi_client_send_msg_sync(
         mQmiSvcClient, QMI_IMSA_SET_BIND_SUBSCRIPTION_REQ_V01,
         (void*)&imsa_bind_request, sizeof(imsa_bind_request),
         (void*)&imsa_bind_resp, sizeof(imsa_bind_resp),
         ModemEndPoint::DEFAULT_SYNC_TIMEOUT);
    if (QMI_NO_ERR == res && imsa_bind_resp.resp.result ==
            QMI_RESULT_SUCCESS_V01) {
      Log::getInstance().d(
        "[ImssModemEndPointModule]: QMI binding succeeds. instanceId: "
        + std::to_string((int)instanceId) + " stackId: " + std::to_string(stackId));
      return true;
    }
    usleep(500*1000);
  } while (++ntries < 10);

  return false;
}

void ImsaModemEndPointModule::init() {
    ModemEndPointModule::init();
    mStackId[0] = 0;
}

