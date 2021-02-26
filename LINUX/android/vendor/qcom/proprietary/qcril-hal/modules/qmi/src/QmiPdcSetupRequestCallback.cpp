/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "modules/qmi/QmiPdcSetupRequestCallback.h"
#include "framework/Log.h"

void QmiPdcSetupRequestCallback::onResponse(
    std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status,
    std::shared_ptr<string> responseDataPtr) {
  (void)solicitedMsg;
  (void)status;
  (void)responseDataPtr;
  Log::getInstance().d(
      "[QmiPdcSetupRequestCallback]: Callback executed. client data = " +
      mClientToken);
}

Message::Callback *QmiPdcSetupRequestCallback::clone() {
  QmiPdcSetupRequestCallback *clone = new QmiPdcSetupRequestCallback(mClientToken);
  return clone;
}
