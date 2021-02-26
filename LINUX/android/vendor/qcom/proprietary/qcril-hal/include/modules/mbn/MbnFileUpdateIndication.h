/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include "framework/Message.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/add_message_id.h"


class MbnFileUpdateIndication : public UnSolicitedMessage,
                            public add_message_id<MbnFileUpdateIndication>
{
  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.mbn.file_update_event";
    ~MbnFileUpdateIndication() = default;

    inline MbnFileUpdateIndication(): UnSolicitedMessage(get_class_message_id())
    {
      mName = MESSAGE_NAME;
    }

    inline std::shared_ptr<UnSolicitedMessage> clone()
    {
      return std::static_pointer_cast<UnSolicitedMessage>(std::make_shared<MbnFileUpdateIndication>());
    }

    inline string dump()
    {
      return mName;
    }
};
