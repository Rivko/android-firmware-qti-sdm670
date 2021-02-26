/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/GenericCallback.h"
#include "framework/Message.h"

class QmiUimHttpSetupRequestCallback : public GenericCallback<string> {
 public:
  inline QmiUimHttpSetupRequestCallback(string str) : GenericCallback(str) {}
  inline ~QmiUimHttpSetupRequestCallback() {}
  Message::Callback *clone();
  void onResponse(std::shared_ptr<Message> solicitedMsg,
                  Message::Callback::Status status,
                  std::shared_ptr<string> responseDataPtr);
  string mClientToken;
};
