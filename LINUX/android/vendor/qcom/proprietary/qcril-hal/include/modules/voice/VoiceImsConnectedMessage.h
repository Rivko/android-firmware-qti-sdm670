/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/message_id.h"
#include "framework/add_message_id.h"

class VoiceImsConnectedMessage : public UnSolicitedMessage,
                                 public add_message_id<VoiceImsConnectedMessage>
{
 public:
  static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.voice.ims_connected";
  ~VoiceImsConnectedMessage();

  VoiceImsConnectedMessage() :
        UnSolicitedMessage(get_class_message_id()) {
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    return (std::make_shared<VoiceImsConnectedMessage>());
  }

  string dump();
};
