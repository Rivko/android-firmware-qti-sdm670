/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "modules/qmi/DmsModemEndPoint.h"

//#include "qmi_idl_lib.h"
#include "common_v01.h"
#include "device_management_service_v01.h"
#include "qmi_client.h"
#include "voice_service_v02.h"
//#include "qmi.h"
#include "modules/qmi/GetOperatingModeSyncMessage.h"
#include "modules/qmi/QmiServiceUpIndMessage.h"
#include "modules/qmi/QmiSetupRequest.h"

using std::to_string;

void DmsModemEndPoint::requestSetup(string clientToken,
                                    GenericCallback<string>* callback) {
  auto shared_setupMsg = std::make_shared<QmiSetupRequest>
        (clientToken, 0, nullptr, callback);
  mModule->dispatch(shared_setupMsg);
  // setupMsg->dispatch();
}

Message::Callback::Status DmsModemEndPoint::getOperatingModeSync(
    std::shared_ptr<int>& shared_operatingMode) {
  // Allow any QMI IDL API invocation only if we are in OPERATIONAL state.
  if (getState() == ModemEndPoint::State::OPERATIONAL) {
    // Note that callback is not required for sync calls.
    auto getOperatingModeMsg =
        std::make_shared<GetOperatingModeSyncMessage>(nullptr);
    // getOperatingModeMsg->dispatch();
    Message::Callback::Status apiStatus;
    apiStatus = getOperatingModeMsg->dispatchSync(shared_operatingMode);
    return apiStatus;
  } else {
    return Message::Callback::Status::FAILURE;
  }
}
