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
 * ImsInitMessage : Sent by the ImsModule.
 * Indicates that ImsModule is ready to receive requests.
 */
class ImsInitMessage : public UnSolicitedMessage, public add_message_id<ImsInitMessage> {
  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.ims.init";

  public:
    string dump() {
      return ImsInitMessage::MESSAGE_NAME;
    }

    ImsInitMessage() :
      UnSolicitedMessage(get_class_message_id()) {
        mName = MESSAGE_NAME;
      }

    std::shared_ptr<UnSolicitedMessage> clone() {
      return (std::make_shared<ImsInitMessage>());
    }

    inline ~ImsInitMessage() {}
};
