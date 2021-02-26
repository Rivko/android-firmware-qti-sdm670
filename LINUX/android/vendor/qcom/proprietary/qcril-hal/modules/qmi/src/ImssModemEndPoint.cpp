/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "modules/qmi/ImssModemEndPoint.h"
#include "modules/qmi/QmiServiceUpIndMessage.h"
#include "modules/qmi/QmiSetupRequest.h"

using std::to_string;
constexpr const char *ImssModemEndPoint::NAME;

void ImssModemEndPoint::requestSetup(string clientToken, GenericCallback<string>* callback) {
  Log::getInstance().d("[" + mName + "]: requestSetup");
  auto setupMsg = std::make_shared<QmiSetupRequest>(clientToken, 0, nullptr, callback);
  mModule->dispatch(setupMsg);
}
