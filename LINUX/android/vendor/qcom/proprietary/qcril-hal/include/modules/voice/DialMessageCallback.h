/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include <string>

#include "framework/GenericCallback.h"
#include "framework/Message.h"
#include "framework/message_translator.h"

using std::string;

class DialMessageCallback : public GenericCallback<generic_callback_payload> {
 private:
 public:
  inline DialMessageCallback(string clientToken)
      : GenericCallback(clientToken){};
  ~DialMessageCallback();
  Message::Callback *clone() override;
  void onResponse(std::shared_ptr<Message> solicitedMsg, Status status,
                  std::shared_ptr<generic_callback_payload> responseDataPtr) override;
};
