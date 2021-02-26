/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <cstring>
#include "modules/qmi/DmsModemEndPointModule.h"
#include "modules/qmi/GetOperatingModeSyncMessage.h"
#include "modules/qmi/QmiServiceUpIndMessage.h"
#include "modules/qmi/QmiSetupRequest.h"
#include "modules/qmi/QmiSendRawSyncMessage.h"
#include "modules/qmi/QmiSendRawAsyncMessage.h"
#include "device_management_service_v01.h"

DmsModemEndPointModule::DmsModemEndPointModule(string name,
    ModemEndPoint &owner) : ModemEndPointModule(name, owner) {

  mServiceObject = nullptr;
  mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);

  using std::placeholders::_1;
  mMessageHandler = {
      HANDLER(GetOperatingModeSyncMessage, DmsModemEndPointModule::handleGetOperatingMode),
  };
}

DmsModemEndPointModule::~DmsModemEndPointModule() {
    mLooper = nullptr;
}

bool DmsModemEndPointModule::handleQmiBinding(
    qcril_instance_id_e_type instanceId, int8_t stackId) {

  (void)instanceId;
  if (stackId < 0)
    return false;

  dms_bind_subscription_req_msg_v01 dms_bind_request;
  dms_bind_subscription_resp_msg_v01 dms_bind_resp;
  memset(&dms_bind_request, 0, sizeof(dms_bind_request));
  memset(&dms_bind_resp, 0, sizeof(dms_bind_resp));

  if (stackId == 0) {
    dms_bind_request.bind_subs = DMS_PRIMARY_SUBS_V01;
  } else if (stackId == 1) {
    dms_bind_request.bind_subs = DMS_SECONDARY_SUBS_V01;
  } else if (stackId == 2) {
    dms_bind_request.bind_subs = DMS_TERTIARY_SUBS_V01;
  } else
    return false;

  int ntries = 0;
  do {
    qmi_client_error_type res = qmi_client_send_msg_sync(
         mQmiSvcClient, QMI_DMS_BIND_SUBSCRIPTION_REQ_V01,
         (void*)&dms_bind_request, sizeof(dms_bind_request),
         (void*)&dms_bind_resp, sizeof(dms_bind_resp),
         ModemEndPoint::DEFAULT_SYNC_TIMEOUT);
    if (QMI_NO_ERR == res && dms_bind_resp.resp.result ==
            QMI_RESULT_SUCCESS) {
      Log::getInstance().d(
        "[DmsModemEndPointModule]: QMI binding succeeds. instanceId: "
        + std::to_string((int)instanceId) + " stackId: " + std::to_string(stackId));
      return true;
    }
    usleep(500*1000);
  } while (++ntries < 10);

  return false;
}


qmi_idl_service_object_type DmsModemEndPointModule::getServiceObject() {
  return dms_get_service_object_v01();
}

void DmsModemEndPointModule::handleGetOperatingMode(
    std::shared_ptr<GetOperatingModeSyncMessage> getOperatingModeMessage) {
  if (mQmiSvcClient != nullptr) {
    qmi_client_error_type rc;
    dms_get_operating_mode_req_msg_v01 get_mode_request_msg;
    dms_get_operating_mode_resp_msg_v01 get_mode_rsp_msg;
    memset(&get_mode_request_msg, 0x00, sizeof(get_mode_request_msg));
    memset(&get_mode_rsp_msg, 0x00, sizeof(get_mode_rsp_msg));
    rc = qmi_client_send_msg_sync(
        mQmiSvcClient, QMI_DMS_GET_OPERATING_MODE_REQ_V01,
        (void *)&get_mode_request_msg, sizeof(get_mode_request_msg),
        (void *)&get_mode_rsp_msg, sizeof(get_mode_rsp_msg), 500);
    if (QMI_NO_ERR != rc) {
      Log::getInstance().d(
          "[DmsModemEndPointModule]: failed get_operating_mode request");
      getOperatingModeMessage->sendResponse(
          getOperatingModeMessage, Message::Callback::Status::FAILURE, nullptr);

    } else {
      if (QMI_RESULT_SUCCESS_V01 != get_mode_rsp_msg.resp.result) {
        Log::getInstance().d(
            "[DmsModemEndPointModule]: get_operating_mode response not good");
        getOperatingModeMessage->sendResponse(
            getOperatingModeMessage, Message::Callback::Status::FAILURE, nullptr);

      } else {
        Log::getInstance().d(
            "[DmsModemEndPointModule]: get_operating_mode good !!");
        Log::getInstance().d(
            "[DmsModemEndPointModule]: get_mode_rsp_msg.operating_mode = " +
            std::to_string(get_mode_rsp_msg.operating_mode));
        getOperatingModeMessage->sendResponse(
            getOperatingModeMessage, Message::Callback::Status::SUCCESS,
            std::shared_ptr<int>(new int{get_mode_rsp_msg.operating_mode}));
      }
    }
  }
}

