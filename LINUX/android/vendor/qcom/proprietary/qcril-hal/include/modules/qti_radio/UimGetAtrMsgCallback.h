/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/GenericCallback.h"
#include "modules/uim/UimGetAtrRequestMsg.h"

class UimGetAtrMsgCallback : public GenericCallback<UimAtrRspParam> {

 public:
     inline UimGetAtrMsgCallback(string clientToken)
         : GenericCallback(clientToken){}

     inline ~UimGetAtrMsgCallback(){};

     Message::Callback *clone() override;

     void onResponse(std::shared_ptr<Message> solicitedMsg, Status status,
                  std::shared_ptr<UimAtrRspParam> responseDataPtr) override;
     std::shared_ptr<void> mResponseData;
};
