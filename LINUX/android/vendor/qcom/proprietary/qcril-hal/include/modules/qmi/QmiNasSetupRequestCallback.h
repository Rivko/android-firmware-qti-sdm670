/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/GenericCallback.h"
#include "framework/Message.h"
#include "framework/Log.h"


class QmiNasSetupRequestCallback : public GenericCallback<string> {
 public:
  inline QmiNasSetupRequestCallback(string str) : GenericCallback(str) {}
  inline ~QmiNasSetupRequestCallback() {}
  Message::Callback *clone();
  void onResponse(std::shared_ptr<Message> solicitedMsg,
                  Message::Callback::Status status,
                  std::shared_ptr<string> responseDataPtr);
  string mClientToken;
};
