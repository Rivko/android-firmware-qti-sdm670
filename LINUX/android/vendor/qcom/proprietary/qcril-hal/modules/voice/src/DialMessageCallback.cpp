/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "modules/voice/DialMessageCallback.h"
#include "framework/Log.h"
#include "framework/Message.h"

DialMessageCallback::~DialMessageCallback() {
  mClientToken = "";
  // Log::getInstance().d("DialMessageCallback desctructor");
}

void DialMessageCallback::onResponse(std::shared_ptr<Message> solicitedMsg,
                                     Status status,
                                     std::shared_ptr<generic_callback_payload> responseDataPtr) {
  // std::shared_ptr<string> shared_respPtr(std::static_pointer_cast<string>
  // (responseDataPtr));
  (void) responseDataPtr;
  if (status == Message::Callback::Status::SUCCESS) {
    Log::getInstance().d("[DialMessageCallback]: Callback[msg = " +
                         solicitedMsg->dump() + "] executed. client data = " +
                         mClientToken + "response = nullptr");
  } else {
    Log::getInstance().d("[DialMessageCallback]: Callback[msg = " +
                         solicitedMsg->dump() + "] executed. client data = " +
                         mClientToken + " FAILURE !!");
  }
}

Message::Callback *DialMessageCallback::clone() {
  DialMessageCallback *clone = new DialMessageCallback(mClientToken);
  return clone;
}
