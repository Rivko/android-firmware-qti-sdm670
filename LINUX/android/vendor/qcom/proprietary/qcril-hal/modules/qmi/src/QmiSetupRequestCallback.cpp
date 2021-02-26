/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "modules/qmi/QmiSetupRequestCallback.h"
#include "framework/Log.h"

void QmiSetupRequestCallback::onResponse(
    std::shared_ptr<Message> solicitedMsg, Message::Callback::Status status,
    std::shared_ptr<string> responseDataPtr) {
  (void)solicitedMsg;
  (void)status;
  (void)responseDataPtr;
  // std::shared_ptr<string> shared_respPtr(std::static_pointer_cast<string>
  // (responseDataPtr));

  Log::getInstance().d(
      "[QmiSetupRequestCallback]: Callback executed. client data = " +
      mClientToken);
}

Message::Callback *QmiSetupRequestCallback::clone() {
  QmiSetupRequestCallback *clone = new QmiSetupRequestCallback(mClientToken);
  return clone;
}
