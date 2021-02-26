/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/GenericCallback.h"
#include "framework/Message.h"

class QmiCatSetupRequestCallback : public GenericCallback<string> {
 public:
  inline QmiCatSetupRequestCallback(string str) : GenericCallback(str) {}
  inline ~QmiCatSetupRequestCallback() {}
  Message::Callback *clone();
  void onResponse(std::shared_ptr<Message> solicitedMsg,
                  Message::Callback::Status status,
                  std::shared_ptr<string> responseDataPtr);
  string mClientToken;
};

