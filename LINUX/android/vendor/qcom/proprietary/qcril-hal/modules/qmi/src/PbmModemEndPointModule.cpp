/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <cstring>
#include "modules/qmi/PbmModemEndPointModule.h"
#include "modules/qmi/PbmModemEndPoint.h"

#include "modules/qmi/QmiServiceUpIndMessage.h"
#include "phonebook_manager_service_v01.h"

PbmModemEndPointModule::PbmModemEndPointModule(string name,
    ModemEndPoint &owner) : ModemEndPointModule(name, owner) {

  mServiceObject = nullptr;
  mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);

  mMessageHandler = {
  };

}

PbmModemEndPointModule::~PbmModemEndPointModule() {
  mLooper = nullptr;
}

bool PbmModemEndPointModule::handleQmiBinding(
    qcril_instance_id_e_type instanceId, int8_t stackId) {

  (void)instanceId;
  if (stackId < 0)
    return false;

  pbm_bind_subscription_req_msg_v01 pbm_bind_request;
  pbm_bind_subscription_resp_msg_v01 pbm_bind_resp;
  memset(&pbm_bind_request, 0, sizeof(pbm_bind_request));
  memset(&pbm_bind_resp, 0, sizeof(pbm_bind_resp));

  if (stackId == 0) {
    pbm_bind_request.subs_type = PBM_SUBS_TYPE_PRIMARY_V01;
  } else if (stackId == 1) {
    pbm_bind_request.subs_type = PBM_SUBS_TYPE_SECONDARY_V01;
  } else if (stackId == 2) {
    pbm_bind_request.subs_type = PBM_SUBS_TYPE_TERTIARY_V01;
  } else
    return false;

  int ntries = 0;
  do {
    qmi_client_error_type res = qmi_client_send_msg_sync(
         mQmiSvcClient, QMI_PBM_BIND_SUBSCRIPTION_REQ_V01,
         (void*)&pbm_bind_request, sizeof(pbm_bind_request),
         (void*)&pbm_bind_resp, sizeof(pbm_bind_resp),
         ModemEndPoint::DEFAULT_SYNC_TIMEOUT);
    if (QMI_NO_ERR == res && pbm_bind_resp.resp.result ==
            QMI_RESULT_SUCCESS) {
      Log::getInstance().d(
        "[PbmModemEndPointModule]: QMI binding succeeds. instanceId: "
        + std::to_string((int)instanceId) + " stackId: " + std::to_string(stackId));
      return true;
    }
    usleep(500*1000);
  } while (++ntries < 10);

  return false;
}

qmi_idl_service_object_type PbmModemEndPointModule::getServiceObject() {
    return pbm_get_service_object_v01();
}

