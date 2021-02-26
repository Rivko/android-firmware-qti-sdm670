/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"

class NasEnableIndicationsIndMessage : public UnSolicitedMessage,
                            public add_message_id<NasEnableIndicationsIndMessage>
{
  private:
    int mScreenState;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.nas.enable_indications";
    NasEnableIndicationsIndMessage() = delete;
    ~NasEnableIndicationsIndMessage() = default;

    inline NasEnableIndicationsIndMessage(int state):
                       UnSolicitedMessage(get_class_message_id())
    {
      mScreenState = state;
      mName = MESSAGE_NAME;
    }

    inline std::shared_ptr<UnSolicitedMessage> clone()
    {
      return std::static_pointer_cast<UnSolicitedMessage>(std::make_shared<NasEnableIndicationsIndMessage>(mScreenState));
    }

    inline string dump()
    {
      return mName + "{}" + "state: " + std::to_string(mScreenState);
    }

    inline  int get_screen_status(void)
    {
      return mScreenState;
    }
};
