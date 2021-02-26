/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include "framework/add_message_id.h"
#include "framework/Message.h"
#include "framework/SolicitedMessage.h"
#include <modules/ims/ImsSetVoiceDomainPreferenceFollowupMessage.h>

class ImsSetVoiceDomainPreferenceFollowupMessage : public SolicitedMessage<void>,
  public add_message_id<ImsSetVoiceDomainPreferenceFollowupMessage> {
  private:
    uint16 mReqId;
    RIL_Errno mResult;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.ims.set_voice_domain_pref_follow_up";

    ImsSetVoiceDomainPreferenceFollowupMessage() = delete;

    ~ImsSetVoiceDomainPreferenceFollowupMessage() {}

    inline ImsSetVoiceDomainPreferenceFollowupMessage(uint16 reqId) :
      SolicitedMessage<void>(get_class_message_id()) {
      mName = MESSAGE_NAME;
      mReqId = reqId;
    }

    inline uint16 getReqId() {
      return mReqId;
    }

    inline void setResult(RIL_Errno result) {
      mResult = result;
    }

    inline RIL_Errno getResult() {
      return mResult;
    }

    string dump() {
      return mName + "{}";
    }
};
