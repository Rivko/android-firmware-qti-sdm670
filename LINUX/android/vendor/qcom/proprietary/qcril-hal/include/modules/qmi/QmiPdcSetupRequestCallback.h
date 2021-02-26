/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/GenericCallback.h"
#include "framework/Message.h"

class QmiPdcSetupRequestCallback : public GenericCallback<string> {
 public:
  inline QmiPdcSetupRequestCallback(string str) : GenericCallback(str) {}
  inline ~QmiPdcSetupRequestCallback() {}
  Message::Callback *clone();
  void onResponse(std::shared_ptr<Message> solicitedMsg,
                  Message::Callback::Status status,
                  std::shared_ptr<string> responseDataPtr);
  string mClientToken;
};
