/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/message_id.h"
#include "framework/add_message_id.h"

class VoiceRteChangePropagationMessage : public UnSolicitedMessage,
                                 public add_message_id<VoiceRteChangePropagationMessage>
{
 public:
  static constexpr const char *MESSAGE_NAME = "VOICE_RTE_CHANGE_PROPAGATION";
  ~VoiceRteChangePropagationMessage();

  VoiceRteChangePropagationMessage() :
        UnSolicitedMessage(get_class_message_id()) {
  }

  std::shared_ptr<UnSolicitedMessage> clone() {
    return (std::make_shared<VoiceRteChangePropagationMessage>());
  }

  string dump();
};
