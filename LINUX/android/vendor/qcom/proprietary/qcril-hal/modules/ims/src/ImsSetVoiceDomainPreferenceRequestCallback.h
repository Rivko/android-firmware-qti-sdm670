/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "framework/SolicitedMessage.h"
#include <framework/GenericCallback.h>

class ImsSetVoiceDomainPreferenceRequestCallback : public GenericCallback<RIL_Errno> {
  public:
    inline ImsSetVoiceDomainPreferenceRequestCallback(string clientToken, uint16 reqId) :
      GenericCallback(clientToken), mReqId (reqId) {
    };

    Message::Callback *clone() override;

    void onResponse(std::shared_ptr<Message> solicitedMsg, Status status,
        std::shared_ptr<RIL_Errno> responseDataPtr) override;

    Status getStatus();

  private:
    Status mStatus;
    std::shared_ptr<RIL_Errno> mResponseData;
    uint16 mReqId;
};
