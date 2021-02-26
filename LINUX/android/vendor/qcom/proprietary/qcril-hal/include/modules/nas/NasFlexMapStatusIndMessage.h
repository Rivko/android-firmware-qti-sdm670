/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"

/*! @brief flex map states
*/

enum class NasFlexmapState {
    NAS_FLEXMAP_STATE_NONE,
    NAS_FLEXMAP_STATE_START,
    NAS_FLEXMAP_STATE_APPLY,
    NAS_FLEXMAP_STATE_FINISH
};

class NasFlexMapStatusIndMessage : public UnSolicitedMessage,
                            public add_message_id<NasFlexMapStatusIndMessage>
{
  private:
    NasFlexmapState mFlexmapState;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.nas.flexmap_status";
    NasFlexMapStatusIndMessage() = delete;
    ~NasFlexMapStatusIndMessage() = default;

    inline NasFlexMapStatusIndMessage(NasFlexmapState  state):
                       UnSolicitedMessage(get_class_message_id())
    {
      mFlexmapState = state;
      mName = MESSAGE_NAME;
    }

    inline std::shared_ptr<UnSolicitedMessage> clone()
    {
      return std::static_pointer_cast<UnSolicitedMessage>(std::make_shared<NasFlexMapStatusIndMessage>(mFlexmapState));
    }

    inline string dump()
    {
      return mName;
    }

    inline  NasFlexmapState get_flexmap_status(void)
    {
      return mFlexmapState;
    }
};
