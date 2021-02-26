/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <cstring>
#include "modules/qmi/ImssModemEndPointModule.h"
#include "modules/qmi/ImssModemEndPoint.h"

#include "modules/qmi/QmiServiceUpIndMessage.h"
#include "modules/qmi/QmiSendRawSyncMessage.h"
#include "modules/qmi/QmiSendRawAsyncMessage.h"

#include "ip_multimedia_subsystem_settings_v01.h"

ImssModemEndPointModule::ImssModemEndPointModule(string name,
    ModemEndPoint &owner) : ModemEndPointModule(name, owner) {

  mServiceObject = nullptr;
  mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);

  mMessageHandler = {
  };

}
void ImssModemEndPointModule::init() {
    ModemEndPointModule::init();
    mStackId[0] = 0;
}

ImssModemEndPointModule::~ImssModemEndPointModule() {
  mLooper = nullptr;
}

qmi_idl_service_object_type ImssModemEndPointModule::getServiceObject() {
  return imss_get_service_object_v01();
}
bool ImssModemEndPointModule::handleQmiBinding(
        qcril_instance_id_e_type instanceId, int8_t stackId)
{
  (void)instanceId;
  if (stackId < 0)
    return false;

  ims_set_bind_subscription_req_msg_v01  imss_bind_request;
  ims_set_bind_subscription_resp_msg_v01 imss_bind_resp;
  memset(&imss_bind_request, 0, sizeof(imss_bind_request));
  memset(&imss_bind_resp, 0, sizeof(imss_bind_resp));

  if (stackId == 0) {
    imss_bind_request.subscription_type = IMS_SUBSCRIPTION_TYPE_PRIMARY_V01;
  } else if (stackId == 1) {
    imss_bind_request.subscription_type = IMS_SUBSCRIPTION_TYPE_SECONDARY_V01;
  } else if (stackId == 2) {
    imss_bind_request.subscription_type = IMS_SUBSCRIPTION_TYPE_TERTIARY_V01;
  } else
    return false;

  int ntries = 0;
  do {
    qmi_client_error_type res = qmi_client_send_msg_sync(
         mQmiSvcClient, QMI_IMS_SET_BIND_SUBSCRIPTION_REQ_V01,
         (void*)&imss_bind_request, sizeof(imss_bind_request),
         (void*)&imss_bind_resp, sizeof(imss_bind_resp),
         ModemEndPoint::DEFAULT_SYNC_TIMEOUT);
    if (QMI_NO_ERR == res && imss_bind_resp.resp.result ==
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
