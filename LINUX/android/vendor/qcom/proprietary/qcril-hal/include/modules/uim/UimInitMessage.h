/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include <framework/legacy.h>
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>

/**
  * UimInitMessage: Sent by the Uim module when UIM module is ready
  * handling requests
  *
  */

class UimInitMessage: public UnSolicitedMessage,
                      public add_message_id<UimInitMessage>
{
  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.uim.init";

    string dump()
    {
      return mName + "{}";
    }
    UimInitMessage():
        UnSolicitedMessage(get_class_message_id())
    {
      mName = MESSAGE_NAME;
    }

    std::shared_ptr<UnSolicitedMessage> clone()
    {
      return (std::make_shared<UimInitMessage>());
    }

    inline ~UimInitMessage() {}
};

