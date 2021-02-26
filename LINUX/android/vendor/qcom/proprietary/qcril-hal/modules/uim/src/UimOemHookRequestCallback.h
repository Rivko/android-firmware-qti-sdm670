/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/GenericCallback.h"
#include "framework/Message.h"
#include "UimOemHookRequestMsg.h"

class UimOemHookRequestCallback : public GenericCallback<UimOemHookRspData> {
 public:
  inline UimOemHookRequestCallback(string str) : GenericCallback(str) {}
  inline ~UimOemHookRequestCallback() {}
  Message::Callback *clone();
  void onResponse(std::shared_ptr<Message>               solicitedMsg,
                  Message::Callback::Status              status,
                  std::shared_ptr<UimOemHookRspData>     responseDataPtr);
  string mClientToken;
};
