/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "modules/sample/SampleMessageCallback.h"
#include "framework/Log.h"

void SampleMessageCallback::onResponse(
    std::shared_ptr<Message> solicitedMsg, Status status,
    std::shared_ptr<string> responseDataPtr) {
  if (status == Message::Callback::Status::SUCCESS) {
    Log::getInstance().d("[SampleMessageCallback]: Callback[msg = " +
                         solicitedMsg->dump() + "] executed. client data = " +
                         mClientToken + "response = " + *responseDataPtr);
  } else {
    Log::getInstance().d("[SampleMessageCallback]: Callback[msg = " +
                         solicitedMsg->dump() + "] executed. client data = " +
                         mClientToken + " FAILURE !!");
  }
}

Message::Callback *SampleMessageCallback::clone() {
  SampleMessageCallback *clone = new SampleMessageCallback(mClientToken);
  return clone;
}
